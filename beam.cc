#include "config.h"

#ifdef DOS
#include <dos.h>
#include <conio.h>
#endif

#include <stdlib.h>
#include <string.h>
//#include <time.h>
#include "externs.h"

#include "bang.h"
#include "beam.h"
//#include "chardump.h"
//#include "command.h"
//#include "debug.h"
//#include "decks.h"
//#include "describe.h"
#include "direct.h"
//#include "dungeon.h"
//#include "effects.h"
#include "fight.h"
//#include "files.h"
//#include "food.h"
//#include "invent.h"
#include "it_use2.h"
//#include "it_use3.h"
//#include "item_use.h"
#include "itemname.h"
#include "items.h"
#include "levels.h"
//#include "maps.h"
#include "message.h"
#include "misc.h"
//#include "monplace.h"
#include "monstuff.h"
#include "mstruct.h"
#include "mstuff2.h"
//#include "newgame.h"
#include "ouch.h"
#include "output.h"
#include "player.h"
#include "priest.h"
#include "shopping.h"
#include "skills.h"
//#include "skills2.h"
//#include "spell.h"
//#include "spells.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "stuff.h"
#include "view.h"

int check_mons_resists(struct bolt beam [1], int o, int hurted);
int check_mons_magres(int mn, int pow);
int mons_ench_f2(int o, char is_near, int func_pass [10], struct bolt beam [1]);


// Up to line 1111


/*
Handles any projectiles which leave visible non-smoke trails and can affect
multiple opponents. Eg firebolt, but not magic dart or thrown weapons.
Should be merged with missile(), as there is a lot of redundant code here.
*/
void beam(struct bolt beam [1])
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

        if (beam[0].move_x > 0) beam_sign_x = 1;
        if (beam[0].move_x < 0)
        {
                beam_sign_x = -1;
                beam[0].move_x *= -1;
        }

        if (beam[0].move_y > 0) beam_sign_y = 1;
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
                        if (beam_sign_x == 1) bmay = (float) (beam[0].move_y) / (beam[0].move_x + 1);
                        if (beam_sign_x == -1) bmay = (float) (beam[0].move_y) / (beam[0].move_x + 1);
                        bmax = 1;
                } else
            {
                if (beam_sign_y == 1) bmay = (float) (beam[0].move_x) / (beam[0].move_y + 1);
                if (beam_sign_y == -1) bmay = (float) (beam[0].move_x) / (beam[0].move_y + 1);
                bmax = 2;
                }
                bmoy = 1;



        } //else beam_inc = 0;



        for (n = 1; n < beam[0].range; n++)
        {

                if (beam[0].beam_name [0] == '0' && beam[0].flavour != 4)
                {
                        return;
                }

                beam[0].move_x = beam_sign_x; beam[0].move_y = beam_sign_y;


                if (bmax == 1)
                {
                        beam[0].move_x = beam_sign_x;
                        bmoy += bmay;
                        if (bmoy >= 1)
                        {
                                beam[0].move_y = beam_sign_y;
                                bmoy -= 1;
                        } else beam[0].move_y = 0;
                }



                if (bmax == 2)
                {
                        beam[0].move_y = beam_sign_y;
                        bmoy += bmay;
                        if (bmoy >= 1)
                        {
                                beam[0].move_x = beam_sign_x;
                                bmoy -= 1;
                        } else beam[0].move_x = 0;
                }

        if (strcmp(beam[0].beam_name, "blast of poison") == 0)
        {
                if (beam[0].thing_thrown != 2) place_cloud(4, beam[0].bx, beam[0].by, random2(4) + 2);
                else place_cloud(104, beam[0].bx, beam[0].by, random2(4) + 2);
        }

        if (strcmp(beam[0].beam_name, "blast of poison") == 0 && beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y)
        {
                if (beam[0].thing_thrown != 2) big_cloud(4, beam[0].bx, beam[0].by, 0);
                else big_cloud(104, beam[0].bx, beam[0].by, 0);
                beam[0].aim_down = 0;
                return;
        }

                if (beam[0].colour == 6 && beam[0].beam_name [0] == '0')
                {
                        beam[0].bx += beam[0].move_x;
                        beam[0].by += beam[0].move_y;

//#ifndef DEBUG
                        if (grd [beam[0].bx] [beam[0].by] == 2 || grd [beam[0].bx] [beam[0].by] == 4 || beam[0].bx <= 5 || beam[0].by <= 5 || beam[0].bx >= GXM - 5 || beam[0].by >= GYM - 5)
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
                        if (grd [beam[0].bx] [beam[0].by] == 1)
                        {
                                grd [beam[0].bx] [beam[0].by] = 67;
                        if (crumble == 0) // && see_grid(beam[0].bx, beam[0].by) == 1)
                        {
                                mpr("You hear a grinding noise.");
                                crumble = 1;
                        }
                        }
                } else
                if (grd [beam[0].bx + beam[0].move_x] [beam[0].by + beam[0].move_y] >= MINMOVE)
                        {
                                beam[0].bx += beam[0].move_x; beam[0].by += beam[0].move_y;
                        } else
                        {

/*              if (beam[0].flavour == 10)
                {
                        explosion1(beam);
                        beam[0].aim_down = 0;
                        return;
                }*/
                                if (beam[0].colour == 15 && beam[0].hit >= 15) /* a powerful disruption works the same */
                {
                 if ((grd [beam[0].bx + beam[0].move_x] [beam[0].by + beam[0].move_y] == 1 || grd [beam[0].bx + beam[0].move_x] [beam[0].by + beam[0].move_y] == 8) && !(beam[0].bx <= 6 || beam[0].by <= 6 || beam[0].bx >= GXM - 6 || beam[0].by >= GYM - 6))
                 {
                                   grd [beam[0].bx + beam[0].move_x] [beam[0].by + beam[0].move_y] = 67;
                           mpr("You hear a grinding noise.");
                 }
                 if (grd [beam[0].bx + beam[0].move_x] [beam[0].by + beam[0].move_y] == 7 || (grd [beam[0].bx + beam[0].move_x] [beam[0].by + beam[0].move_y] >= 21 && grd [beam[0].bx + beam[0].move_x] [beam[0].by + beam[0].move_y] <= 39))
                 {
                                   grd [beam[0].bx + beam[0].move_x] [beam[0].by + beam[0].move_y] = 67;
                   if (see_grid(beam[0].bx + beam[0].move_x, beam[0].by + beam[0].move_y) == 0)
                            mpr("You hear a hideous screaming!");
                     else mpr("The statue screams as its substance crumbles away!");
                 }
                 return;
                }

                                if (beam[0].beam_name [0] == 48 || beam[0].flavour != 5)
                // ie non-bouncing enchantments, non-electrical beams etc
                                {
                                        beam[0].aim_down = 0;
                                        return;
                                }

                                n++; // reduces beam's range by 1
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
                                if (beam[0].move_x == 0 ^ beam[0].move_y == 0) //   ^ XOR
                                {
                                        beam_sign_x *= -1;
                                        beam_sign_y *= -1;
                                }
                                if (beam[0].move_x != 0 && beam[0].move_y != 0)
                                {
                                        if (grd [beam[0].bx] [beam[0].by + beam[0].move_y] >= MINMOVE && grd [beam[0].bx + beam[0].move_x] [beam[0].by] >= MINMOVE)
                                        {
                                                beam_sign_x *= -1; beam_sign_y *= -1;
                                        }
                                        if (grd [beam[0].bx + beam[0].move_x] [beam[0].by] < MINMOVE && grd [beam[0].bx] [beam[0].by + beam[0].move_y] >= MINMOVE)
                                        {
                                                beam_sign_x *= -1;
                                        }
                                        if (grd [beam[0].bx] [beam[0].by + beam[0].move_y] < MINMOVE && grd [beam[0].bx + beam[0].move_x] [beam[0].by] >= MINMOVE)
                                        {
                                                beam_sign_y *= -1;
                                        }
                                        if ((grd [beam[0].bx] [beam[0].by + beam[0].move_y] < MINMOVE && grd [beam[0].bx + beam[0].move_x] [beam[0].by] < MINMOVE))// || (grd [beam[0].bx] [beam[0].by + beam[0].move_y] > 10 && grd [beam[0].bx + beam[0].move_x] [beam[0].by]))
                                        {
                                                beam_sign_x *= -1; beam_sign_y *= -1;
                                        }

                                }
                        }

/*      if (beam[0].bx > you[0].x_pos - 9 && beam[0].bx < you[0].x_pos + 9 && beam[0].by > you[0].y_pos - 9 && beam[0].by < you[0].y_pos + 9 && (show [beam[0].bx - you[0].x_pos + 9] [beam[0].by - you[0].y_pos + 9] != 0 || (beam[0].bx == you[0].x_pos && beam[0].by == you[0].y_pos)) && beam[0].type != 0)*/
                if (((beam[0].bx > you[0].x_pos - 9 && beam[0].bx < you[0].x_pos + 9 && beam[0].by > you[0].y_pos - 9 && beam[0].by < you[0].y_pos + 9) || (beam[0].bx == you[0].x_pos && beam[0].by == you[0].y_pos)) && beam[0].type != 0)
                {
                        if (beam[0].range != n)
                        {
                                textcolor(beam[0].colour);
                                gotoxy(beam[0].bx - you[0].x_pos + 18, beam[0].by - you[0].y_pos + 9);
                                putch(beam[0].type);
                                delay(15);
                        }
                }

                if (bounce > 0) bounce --;

                if (env[0].cgrid [beam[0].bx] [beam[0].by] != CNG) // hit a cloud
                {

                        if (beam[0].beam_name [0] == 48 && beam[0].colour == 8)
                        {
                                env[0].cloud_type [env[0].cgrid [beam[0].bx] [beam[0].by]] = random2(8) + 1;
                        }

                        if (beam[0].colour == 200 || beam[0].beam_name [0] == 48) goto out_of_cloud_bit;

                        clouty = env[0].cgrid [beam[0].bx] [beam[0].by];

/* 1 = fire, 3 = cold, 2 = stinking */

                        if ((env[0].cloud_type [clouty] == 3 && beam[0].flavour == 2) || (env[0].cloud_type [clouty] == 1 && beam[0].flavour == 3))
                        {
                                if (see_grid(beam[0].bx, beam[0].by) == 1)
                                {
                                        mpr("You hear a sizzling sound!");
                                }

                                env[0].cloud_type [clouty] = 0;
                                env[0].cgrid [env[0].cloud_x [clouty]] [env[0].cloud_y [clouty]] = CNG;
                                env[0].cloud_decay [clouty] = 0;
                                env[0].cloud_no--;

                                n += 5;

                        }

                }

                out_of_cloud_bit: if (beam[0].bx == you[0].x_pos && beam[0].by == you[0].y_pos && beam[0].colour != 6) // have to do something about enchantments here.
                {                                                                              // ^^^ digging
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

                        if (beam[0].beam_name [0] != 48) // ie enchantments always hit
                        {
                                if ((you[0].equip [6] == -1 || you[0].inv_type [you[0].equip [6]] < 2 || (you[0].inv_type [you[0].equip [6]] >= 22 && you[0].inv_type [you[0].equip [6]] <= 25) || you[0].inv_dam [you[0].equip [6]] / 30 == 4) && random() % 2 == 0 && beam[0].move_x != 0 || beam[0].move_y != 0)
                                exercise(14, 1);
                                if (beam[0].hit < random2(player_evasion()) + random2(you[0].dex) / 3 - 2 && (beam[0].move_x != 0 || beam[0].move_y != 0))
                                {
                                strcpy(info, "The ");
                                strcat(info, beam[0].beam_name);
                                    strcat(info, " misses you.");
                                    mpr(info);
                                    continue;
                                }
                        }




                        if (beam[0].beam_name [0] == 48)
                        {
                                beam[0].aim_down = 0;
                                if (beam[0].colour != 1 && beam[0].colour != 5 && beam[0].colour != 2 && (beam[0].colour != 7 || beam[0].move_x != 0 || beam[0].move_y != 0))
// if ((beam[0].thing_thrown == 1 || beam[0].thing_thrown == 3) && you_resist_magic(beam[0].ench_power) == 1)
                                if (you_resist_magic(beam[0].ench_power) == 1)
                                {
                                        strcpy(info, "You resist.");
                                        mpr(info);
                                        return;
                                }
                                switch(beam[0].colour)
                                {
                                        case 0: potion_effect(9, beam[0].ench_power); return; // slow
                                        case 1: potion_effect(2, beam[0].ench_power); return; // haste
                                        case 2: potion_effect(1, beam[0].ench_power); return; // heal (heal wounds potion eff)
                                        case 3: potion_effect(10, beam[0].ench_power); return; // paralysis
                                        case 4: potion_effect(11, beam[0].ench_power); return; // confusion
                                        case 5: potion_effect(12, beam[0].ench_power); return; // invisibility
                        // 6 is used by digging
                                        case 7: you_teleport(); return;
                        case 8: strcpy(info, "This is polymorph other only!");
                            mpr(info);
                                return;
                                        case 9: potion_effect(11, beam[0].ench_power); return; // enslavement - confusion?
                                        case 13: // pain
                                                if (you[0].is_undead != 0)
                                                {
                                                strcpy(info, "You are unaffected.");
                                                mpr(info);
                                                return;
                                                }
                                                strcpy(info, "Pain shoots through your body!");
                                                mpr(info);
                                                strcpy(beam[0].beam_name, "spell");
                                                ouch(random2(beam[0].hit), 0, 3);
                                                return;
                                        case 15:
                                                mpr("You are blasted!");
                                                strcpy(beam[0].beam_name, "spell");
                                                ouch(random2(beam[0].hit), 0, 3);
                                                return;



                                } // end of switch (beam[0].colour)

                        } // end of if beam[0].beam_name [0] == 48



                        strcpy(info, "The ");
                        strcat(info, beam[0].beam_name);
                        strcat(info, " hits you");

                        hurted = 0;

                        if (beam[0].damage > 100)
                        {
                                hurted += random2(beam[0].damage - 100);
                                hurted += random2(beam[0].damage - 100);
                                hurted += random2(beam[0].damage - 100);
                        } else hurted += random2(beam[0].damage);

                        hurted -= random2(player_AC() + 1);
                        hurted -= random2(player_shield_class()); // don't bother with + 1 here

                        strcat(info, "!");
                        mpr(info);

                        if (you[0].equip [5] != -1)
                        if (beam[0].move_x != 0 || beam[0].move_y != 0) exercise(17, (random() % 3) / 2);

                        if (you[0].equip [6] != -1)
                        if (random() % 1000 <= mass(2, you[0].inv_type [you[0].equip [6]]) && random() % 4 == 0)
                                exercise(13, 1);

                        if (hurted <= 0) hurted = 0;


                        check_your_resists(hurted, beam[0].flavour);

/*      check_your_resists(); */
/*      strcat(info, "!");
        if (strcmp(info, "How terrible") != 0) mpr(info);*/

                        if (beam[0].flavour == 2 || stricmp(beam[0].beam_name, "hellfire") == 0) scrolls_burn(3, 6); // also above
                        if (beam[0].flavour == 3) scrolls_burn(3, 8);

                        ouch(hurted, 0, 3);

                        you[0].hp_ch = 1;

                        n++; // reduces beam's range
                if (beam[0].flavour != 5) n += random2(5) + 3; /* If it isn't lightning, reduce range by a lot */

                        if (beam[0].flavour == 10)
                        {
                                explosion1(beam);
                                return;
                }

                        if (beam[0].move_x == 0 && beam[0].move_y == 0) break;

                        goto check_aimed;
                }

                out_of_hit_you :

                        if (beam[0].target_x == beam[0].bx && beam[0].target_y == beam[0].by && beam[0].aim_down == 1 && ((grd [beam[0].bx] [beam[0].by] == 61 && beam[0].flavour == 3) || (grd [beam[0].bx] [beam[0].by] == 62 && beam[0].flavour == 2)))
                                place_cloud(8, beam[0].bx, beam[0].by, 2 + random2(5)); // steam

                        if (mgrd [beam[0].bx] [beam[0].by] != MNG && (beam[0].colour != 6 || beam[0].beam_name [0] != '0'))
                        {
                                o = mgrd [beam[0].bx] [beam[0].by];

                                if (beam[0].beam_name [0] == '0') goto enchanting;
                                if (beam[0].beam_name [0] != 48)
                                {
// tracer = 0 = run out of range
// tracer = 1 = hits you in range
// tracer_mons = 0 = hits no monsters
//       "     = 1 = hits monster before you (possibly also after)
//       "     = 2 = hits monster after but not before
//       "     = 3 = hits tamed monster(s) but no others
//       "     = 4 = hits tamed monster(s) + possibly other things

                                        if (menv [o].m_class >= MLAVA0 && menv [o].m_sec == 1 && (beam[0].bx != beam[0].target_x || beam[0].by != beam[0].target_y || beam[0].aim_down != 1)) goto check_aimed;

                                        if (beam[0].beam_name [0] != 48) // ie enchantments always hit
                                        {
                                                if (beam[0].hit < random2(menv [o].m_ev))
                                                {
                                                strcpy(info, "The ");
                                                    strcat(info, beam[0].beam_name);
                                                strcat(info, " misses ");
                                                        strcat(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 1)); //gmon_name [menv_class [o]]);
                                                    strcat(info, ".");
                                                mpr(info);
                                                goto check_aimed;
                                                }
                                        }



                                        strcpy(info, "The ");
                                        strcat(info, beam[0].beam_name);
                                        strcat(info, " hits ");
                                        strcat(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 1)); //gmon_name [mons_class [o]]);
                                        strcat(info, ".");
                                        mpr(info);

                                        hurted = 0;

                                        if (beam[0].damage > 100)
                                        {
                                                hurted += random2(beam[0].damage - 100);
                                                hurted += random2(beam[0].damage - 100);
                                                hurted += random2(beam[0].damage - 100);
                                        } else hurted += random2(beam[0].damage);

                                        hurted -= random2(menv[o].m_AC + 1);
                                        if (hurted <= 0) hurted = 0;
                                        hurted = check_mons_resists(beam, o, hurted);

                                        menv [o].m_hp -= hurted;


//if ((beam[0].thing_thrown == 1 || beam[0].thing_thrown == 3) && mons_holiness(menv [o].m_class) < 0 && hurted > 0) naughty(15, 4);

                                        if (menv [o].m_beh == 0) menv [o].m_beh = 1;
                                        if ((beam[0].thing_thrown == 1 || beam[0].thing_thrown == 3) && menv [o].m_beh == 7 && hurted > 0)
                                        {
                                                menv [o].m_beh = 1;
//   naughty(8, 5);
                                        }
                                        if (menv [o].m_hp <= 0)
                                        {
                                                switch(beam[0].thing_thrown)
                                                {
                                                        case 1:                    /* your beam */
                                                        case 3: monster_die(o, 3, 0);
                                                                break;                     /*  "    " */
                                                        case 2: monster_die(o, 4, beam[0].beam_source);
                                                                break;                     /* dragon breath &c */
                                                }
                                        } else
                                        {
                                          if ((beam[0].thing_thrown == 1 || beam[0].thing_thrown == 3) && mons_near(o) == 1) print_wounds(o);
                      if (menv [o].m_class >= 389 && menv [o].m_class <= 393) mimic_alert(o);
                    }

                                        if (beam[0].flavour == 10 && strcmp(beam[0].beam_name, "hellfire") != 0)
                                        {
                                                explosion1(beam);
                        beam[0].aim_down = 0;
                                                return;
                                        }
                        if (beam[0].flavour != 5) n += random2(5) + 3; /* If it isn't lightning, reduce range by a lot */

                                }



                                enchanting : if (menv [o].m_x == beam[0].bx && menv [o].m_y == beam[0].by && menv [o].m_class != -1 && beam[0].beam_name [0] == 48)
                                {

                                if (beam[0].colour == 200)
                                {
                                        if (beam[0].trac_targ != MHITYOU)
                                        {


                                                if (menv [o].m_beh == 7 && beam[0].tracer_mons == 0)
                                                {
                                                        beam[0].tracer_mons = 3;
                                                beam[0].trac_hit_tamed = 1;
                                                goto check_aimed;
                                                }
                                        if (menv [o].m_beh == 7 && menv [beam[0].beam_source].m_beh == 7)
                                                {
                                                beam[0].tracer_mons = 4;
                                                        goto check_aimed;
                                                }



                                        if (o == menv [beam[0].beam_source].m_hit)
                                        {
                                                beam[0].trac_hit_mons = 1;
                                                goto check_aimed;
                                        }

                                                if (beam[0].tracer_mons == 0)
                                                {
                                                        beam[0].tracer_mons = 3;
                                                goto check_aimed;
                                                }
                                                if (beam[0].trac_hit_mons == 1 && beam[0].tracer_mons != 1) beam[0].tracer_mons = 2;// else
                                                if (beam[0].trac_hit_mons == 0) beam[0].tracer_mons = 1;
                                        } else // Aimed at you:
                                {

                                           if (beam[0].tracer == 1 && beam[0].tracer_mons != 1) beam[0].tracer_mons = 2;// else
                                           if (beam[0].tracer == 0) beam[0].tracer_mons = 1;
                                }
                                        goto check_aimed;

                                }


                                if (you[0].invis != 0 && mons_see_invis(menv [0].m_class) != 1 && menv [o].m_beh == 0) menv [o].m_beh = 1;

                                if (beam[0].colour ==  7) //: // teleportation
                                {
                        if (check_mons_magres(o, beam[0].ench_power) == 0 && (beam[0].move_x != 0 || beam[0].move_y)) goto it_resists;
                                        if (mons_near(o) == 1 && menv [o].m_ench [2] != 6)
                                        {
                                                strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                                strcat(info, " looks slightly unstable.");
                                                mpr(info);
                                        }
                                        monster_teleport(o, 0);
                                        beam[0].aim_down = 0;
                                        return;
                                }

                        if (beam[0].colour == 8)
                        {
                            if (check_mons_magres(o, beam[0].ench_power) == 0) goto it_resists;
                            monster_polymorph(o, 250, 100);
                            beam[0].aim_down = 0;
                            return;
                                }

                            if (beam[0].colour == 10)
                        {
                            if (check_mons_magres(o, beam[0].ench_power) == 0) goto it_resists;
                            monster_die(o, 6, beam[0].beam_source);
                            beam[0].aim_down = 0;
                            return;
                            }

                        if (beam[0].colour == 11)
                        {
                        if (mons_holiness(menv [o].m_class) != 0 || menv [o].m_class == 131) goto unaffected;
                        if (check_mons_magres(o, beam[0].ench_power) == 0) goto it_resists;
                        monster_polymorph(o, 131, 100);
                        beam[0].aim_down = 0;
                            return;
                        }

                                if (beam[0].colour == 14)
                                {
                                        if (mons_holiness(menv [o].m_class) != 1)
                                        {
                                        unaffected : strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                        strcat(info, " is unaffected.");
                                        mpr(info);
                                            beam[0].aim_down = 0;
                                        return;
                                        }
                                        strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                        strcat(info, " convulses!");
                                        mpr(info);
                                        menv[o].m_hp -= random2(beam[0].hit);
                                        menv[o].m_hp -= random2(beam[0].hit);
                                        menv[o].m_hp -= random2(beam[0].hit);
                                        strcpy(beam[0].beam_name, "spell");
                                        char killer = 0;
                                        switch(beam[0].thing_thrown)
                                        {
                                                case 1: killer = 3; break; // your beam
                                                case 3: killer = 3; break; //  "    "
                                                case 2: killer = 4; break; // dragon breath &c
                                        }
                                        if (menv [o].m_hp <= 0) monster_die(o, killer, beam[0].beam_source);
                                        else
                    {
                      print_wounds(o);
                      if (menv [o].m_class >= 389 && menv [o].m_class <= 393) mimic_alert(o);
                    }
                                        beam[0].aim_down = 0;
                                        return;
                                }

                        if (check_mons_magres(o, beam[0].ench_power) == 0 && beam[0].colour != 1 && beam[0].colour != 2 && beam[0].colour != 5)
                        {
                                it_resists : strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                strcat(info, " resists.");
                                mpr(info);
                                beam[0].aim_down = 0;
                                return;
                            }


                                if (beam[0].colour == 13) /* pain/agony */
                                {
                                        if (mons_holiness(menv [o].m_class) > 0) goto it_resists;
                                        strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                        strcat(info, " convulses in agony!");
                                        mpr(info);
                                        if (strstr(beam[0].beam_name, "agony") != NULL)
                                        {
                                                menv[o].m_hp = menv[o].m_hp / 2;
                                                if (menv [o].m_hp <= 1) menv [o].m_hp = 1;
                                                strcpy(beam[0].beam_name, "agony");
                                        } else
                                        {
                                                menv[o].m_hp -= random2(beam[0].hit);
                                                strcpy(beam[0].beam_name, "pain");
                                        }
                                        char killer = 0;
                                        switch(beam[0].thing_thrown)
                                        {
                                                case 1: killer = 3; break; // your beam
                                                case 3: killer = 3; break; //  "    "
                                                case 2: killer = 4; break; // dragon breath &c
                                        }
                                        if (menv [o].m_hp <= 0) monster_die(o, killer, beam[0].beam_source);
                                        else
                    {
                      print_wounds(o);
                      if (menv [o].m_class >= 389 && menv [o].m_class <= 393) mimic_alert(o);
                    }
                                        beam[0].aim_down = 0;
                                        return;
                                }

                                if (beam[0].colour == 15) /* disrupt/disintegrate */
                                {
                                        strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                        strcat(info, " is blasted.");
                                        mpr(info);
                                        menv[o].m_hp -= random2(beam[0].hit + 1);
                                        strcpy(beam[0].beam_name, "spell");
                                        char killer = 0;
                                        switch(beam[0].thing_thrown)
                                        {
                                                case 1: killer = 3; break; // your beam
                                                case 3: killer = 3; break; //  "    "
                                                case 2: killer = 4; break; // dragon breath &c
                                        }
                                        if (menv [o].m_hp <= 0) monster_die(o, killer, beam[0].beam_source);
                                        else
                    {
                      print_wounds(o);
                      if (menv [o].m_class >= 389 && menv [o].m_class <= 393) mimic_alert(o);
                    }
                                        beam[0].aim_down = 0;
                                        return;
                                }

                                int func_pass [10];

                                if (beam[0].colour == 12 && mons_holiness(menv [o].m_class) == 1)
                                {
                                        strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                        strcat(info, " is enslaved.");
                                        mpr(info);
                                        menv [o].m_beh = 7;
                                        return;
                                }
                                if (beam[0].colour == 12)
                                {
                                        goto unaffected;
                                }

                                mons_ench_f2(o, mons_near(o), func_pass, beam);

                                beam[0].wand_id = func_pass [1];
                                beam[0].aim_down = 0;
                                return;

                        } // end of if ...

                }

                check_aimed : if (beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y && beam[0].aim_down == 1)
                {
                        goto landed;
                }

                if (beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y && (beam[0].flavour == 10 || beam[0].flavour == 18))
                {
                        explosion1(beam);
                beam[0].aim_down = 0;
                        return;
                }



        } // end of for n

        landed :
    // had if beam == tracer
        if (beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y && beam[0].aim_down == 1)
        {
                beam[0].aim_down = 0;
                return;
        }

        if (beam[0].flavour == 10)
        {
                explosion1(beam);
                beam[0].aim_down = 0;
                return;
        }


        if (beam[0].beam_name [0] == 48 && beam[0].colour != 6 && beam[0].colour != 200)
        {
                strcpy(info, "Nothing appears to happen.");
            mpr(info);
        }

        beam[0].aim_down = 0;
/* ^ must have! */

}  // end of void beam(void);



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

throw_2 is the item, either in your invent or in env[0] for a monster.
*/
void missile(struct bolt beam [1], int throw_2)
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
brek = 0;*/

/*beam[0].tracer = 0;*/

        int hurted = 0;

        if (beam[0].move_x > 1 || beam[0].move_y > 1 || beam[0].move_x < -1 || beam[0].move_y < -1)
        {

                if (beam[0].move_x > 0) beam_sign_x = 1;
                if (beam[0].move_x < 0)
                {
                        beam_sign_x = -1;
                        beam[0].move_x *= -1;
                }

                if (beam[0].move_y > 0) beam_sign_y = 1;
                if (beam[0].move_y < 0)
                {
                        beam_sign_y = -1;
                        beam[0].move_y *= -1;
                }

                if (beam[0].move_x > beam[0].move_y)
                {
                        if (beam_sign_x == 1) bmay = (float) (beam[0].move_y) / (beam[0].move_x + 1);
                        if (beam_sign_x == -1) bmay = (float) (beam[0].move_y) / (beam[0].move_x + 1);
                        bmax = 1;
                } else
            {
                if (beam_sign_y == 1) bmay = (float) (beam[0].move_x) / (beam[0].move_y + 1);
                if (beam_sign_y == -1) bmay = (float) (beam[0].move_x) / (beam[0].move_y + 1);
                bmax = 2;
                }
                bmoy = 1;



        }

        int n;

        for (n = 1; n < beam[0].range; n++)
        {

                if (beam[0].beam_name [0] == '0' && beam[0].flavour != 4)
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
                        } else beam[0].move_y = 0;
                }



                if (bmax == 2)
                {
                        beam[0].move_y = beam_sign_y;
                        bmoy += bmay;
                        if (bmoy >= 1)
                        {
                                beam[0].move_x = beam_sign_x;
                                bmoy -= 1;
                        } else beam[0].move_x = 0;
                }

                if (grd [beam[0].bx + beam[0].move_x] [beam[0].by + beam[0].move_y] >= MINMOVE)
                {
                        beam[0].bx += beam[0].move_x; beam[0].by += beam[0].move_y;

                        if (beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y)
                        {
                        if (beam[0].flavour == 10)
                            {
                            if (beam[0].thing_thrown == 3) beam[0].thing_thrown = 1;
                                        if (beam[0].thing_thrown == 4) beam[0].thing_thrown = 2;
                                explosion1(beam);
                                beam[0].aim_down = 0;
                                return;
                            }

                                if (strcmp(beam[0].beam_name, "orb of electricity") == 0)
                                {
                                        explosion1(beam);
                        beam[0].aim_down = 0;
                                        return;
                                }



                            if (beam[0].flavour == 8) // cloud
                            {
                            beam[0].aim_down = 0;
                            return;
                            }

                        }

                        if (strcmp(beam[0].beam_name, "orb of energy") == 0)
                        {
                                place_cloud(7, beam[0].bx, beam[0].by, random2(5) + 1);
                        }

                        if (strcmp(beam[0].beam_name, "ball of steam") == 0)
                        {
                                place_cloud(8, beam[0].bx, beam[0].by, random2(5) + 1);
                        }

                        if (strcmp(beam[0].beam_name, "sticky flame") == 0)
                        {
                                place_cloud(10, beam[0].bx, beam[0].by, random2(4) + 1);
                        }

                        if (strcmp(beam[0].beam_name, "poison gas") == 0)
                        {
                                if (beam[0].thing_thrown != 2) place_cloud(4, beam[0].bx, beam[0].by, random2(4) + 2);
                        else place_cloud(104, beam[0].bx, beam[0].by, random2(4) + 2);
                        }

                if (strcmp(beam[0].beam_name, "foul vapour") == 0 && beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y)
                {
                        if (beam[0].thing_thrown != 2) big_cloud(2, beam[0].bx, beam[0].by, 0);
                         else big_cloud(102, beam[0].bx, beam[0].by, 0);
                        return;
                }


/*if (beam[0].bx > you[0].x_pos - 9 && beam[0].bx < you[0].x_pos + 9 && beam[0].by > you[0].y_pos - 9 && beam[0].by < you[0].y_pos + 9 && show [beam[0].bx - you[0].x_pos + 9] [beam[0].by - you[0].y_pos + 9] != 0)*/
                        if (beam[0].colour != 200) /* tracer doesn't update the screen */
                        if (beam[0].bx > you[0].x_pos - 9 && beam[0].bx < you[0].x_pos + 9 && beam[0].by > you[0].y_pos - 9 && beam[0].by < you[0].y_pos + 9)
                        {
#ifdef DOS_TERM
                                viewwindow(1);
#endif
                                textcolor(beam[0].colour);
                                gotoxy(beam[0].bx - you[0].x_pos + 18, beam[0].by - you[0].y_pos + 9);
                                putch(beam[0].type);
                                delay(15);
                        }

                        if (beam[0].bx == you[0].x_pos && beam[0].by == you[0].y_pos)
                        {
 /* I find it more logical that there can be virtually unavoidable missiles than missiles that can almost always be avoided. */

                                if (beam[0].colour == 200) /* tracer */
                                {
                                        if (beam[0].trac_hit_mons == 1 || you[0].invis == 0)
                                        {
                                                beam[0].tracer = 1;
                                                return;
                                        }
                                }
                                else /* Start of : if it's not a tracer */
                                {

                                        if (you[0].equip [5] != -1)
                                        if (beam[0].move_x != 0 || beam[0].move_y != 0) exercise(17, random() % 2);

                                        if (player_shield_class() > 0 && random2(beam[0].hit * 5) <= random2(player_shield_class()))
                                        {
                                                strcpy(info, "You block the ");
                                                strcat(info, beam[0].beam_name);
                                                strcat(info, ".");
                                                mpr(info);
                                                if (beam[0].flavour == 10)
                                                {
                                                        explosion1(beam);
                                                        beam[0].aim_down = 0;
                                                        return;
                                                }
                                                if (strcmp(beam[0].beam_name, "orb of electricity") == 0)
                                                {
                                                        explosion1(beam);
                                    beam[0].aim_down = 0;
                                                        return;
                                                }
                                                break;
                                        } // end of block

                                        if ((you[0].equip [6] == -1 || you[0].inv_type [you[0].equip [6]] < 2 || (you[0].inv_type [you[0].equip [6]] >= 22 && you[0].inv_type [you[0].equip [6]] <= 25) || you[0].inv_dam [you[0].equip [6]] / 30 == 4) && random() % 2 == 0 && beam[0].move_x != 0 || beam[0].move_y != 0)
                                                exercise(14, 1);


                                        if (beam[0].hit >= random2(player_evasion()) + random2(you[0].dex) / 3 - 2 && you[0].duration [2] == 0)// || random2(6) == 0)
                                        {

                                                strcpy(info, "The ");
                                                strcat(info, beam[0].beam_name);
                                                strcat(info, " hits you");
/*      if (beam[0].thing_thrown == 3 || beam[0].thing_thrown == 4)
        {
        if (beam[0].bx > you[0].x_pos - 7 && beam[0].bx < you[0].x_pos + 7 && beam[0].by > you[0].y_pos - 7 && beam[0].by < you[0].y_pos + 7)
        {
                viewwindow(1);
                textcolor(beam[0].colour);
                gotoxy(beam[0].bx - you[0].x_pos + 18, beam[0].by - you[0].y_pos + 9);
                putch(35);
        }
        }*/

                                                hurted = 0;
                                                if (beam[0].damage > 100)
                                                {
                                                        hurted += random2(beam[0].damage - 100);
                                                        hurted += random2(beam[0].damage - 100);
                                                        hurted += random2(beam[0].damage - 100);
                                                } else hurted += random2(beam[0].damage);

                                                strcat(info, "!");
                                                mpr(info);

                                                check_your_resists(hurted, beam[0].flavour);

                                                if (strstr(beam[0].beam_name, "poison") != NULL && beam[0].flavour != 6 && player_res_poison() == 0 && random2(hurted) - random2(player_AC()) > 0)
                                                {
                                                        mpr("You are poisoned.");
                                                        you[0].poison += 1 + random2(3);
                                                }

                                                if (beam[0].flavour == 20 || beam[0].flavour == 20 || (beam[0].flavour == 2 && strcmp(beam[0].beam_name, "ball of steam") != 0) || stricmp(beam[0].beam_name, "hellfire") == 0) scrolls_burn(2, 6);
                                                if (beam[0].flavour == 3) scrolls_burn(2, 8);

                                                hurted -= random2(player_AC() + 1);
                                                if (hurted <= 0) hurted = 0;


                                                if (you[0].equip [6] != -1)
                                                if (random() % 1000 <= mass(2, you[0].inv_type [you[0].equip [6]]) && random() % 4 == 0)
                                                        exercise(13, 1);

                                                ouch(hurted, 0, 3);

                                                you[0].hp_ch = 1;

                                    if (strcmp(beam[0].beam_name, "sticky flame") == 0 && (you[0].species != 25 || you[0].xl < 6))
                                        {
                                                   if (you[0].equip [6] == -1 || you[0].inv_type [you[0].equip [6]] != 25)
                                                you[0].duration [0] += 1 + random2(3) + random2(3) + random2(3);
                                                }

                                                break;
                                        } else
                                        {
                                                strcpy(info, "The ");
                                                strcat(info, beam[0].beam_name);
                                                strcat(info, " misses you!");
                                        mpr(info);
                                                if (beam[0].move_x == 0 && beam[0].move_y == 0) break;
                                        }


                                } /* end of if not tracer */



                                if (beam[0].flavour == 10)
                                {
                                        explosion1(beam);
                                        beam[0].aim_down = 0;
                                        return;
                                }
                                if (strcmp(beam[0].beam_name, "orb of electricity") == 0)
                                {
                                        explosion1(beam);
                        beam[0].aim_down = 0;
                                        return;
                                }


                        }

                        char count_x = 0;

                        if (mgrd [beam[0].bx] [beam[0].by] != MNG)
                        {
                                count_x = 0;

  /* this won't display a "you miss it" if you throw an beam[0].aim_down thing at a lava worm, only if you hit. */
                                int o = mgrd [beam[0].bx] [beam[0].by];

                                if (beam[0].colour == 200)  /* tracer */
                                {
                                        if (menv [o].m_class < MLAVA0 || menv [o].m_sec == 0)
                                        {
                                                if (beam[0].trac_hit_mons == 1 || menv [o].m_ench [2] != 6)
                                                {
                                                        if (o == beam[0].trac_targ) beam[0].tracer_mons = 1;
                                                        if (menv [0].m_beh == 7) beam[0].tracer = 2; else beam[0].tracer = 3;
                                                        return;
                                                }
                                        }
                                } else /* Start of : It's not a tracer */
                                {

                                        if (beam[0].hit >= random2(menv[o].m_ev) && ((menv [o].m_class < MLAVA0 || menv [o].m_sec == 0) || (beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y && beam[0].aim_down == 1)))
                                        {
                                                hurted = 0;

                                                if (beam[0].damage > 100)
                                                {
                                                        hurted += random2(beam[0].damage - 100);
                                                        hurted += random2(beam[0].damage - 100);
                                                        hurted += random2(beam[0].damage - 100);
                                                } else hurted += random2(beam[0].damage);

                                                if (mons_near(o) == 1)
                                                {
                                                        strcpy(info, "The ");
                                                        strcat(info, beam[0].beam_name);
                                                        strcat(info, " hits ");
                                                        strcat(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 1)); //gmon_name [mons_class [o]]);
                                                        strcat(info, ".");
                                        mpr(info);
                                                }

                                                hurted = check_mons_resists(beam, o, hurted);

                                                menv[o].m_hp -= hurted;

                                                if ((beam[0].thing_thrown == 1 || beam[0].thing_thrown == 3) && menv [o].m_beh == 7 && hurted > 0)
                                                {
//   naughty(8, 5);
                                                        menv [o].m_beh = 1;
                                                }
//  if ((beam[0].thing_thrown == 1 || beam[0].thing_thrown == 3) && mons_holiness(menv [o].m_class) < 0 && hurted > 0) naughty(15, 4);
                                                if ((beam[0].thing_thrown == 1 || beam[0].thing_thrown == 3) && mons_near(o) == 1) print_wounds(o);
                        if (menv [o].m_class >= 389 && menv [o].m_class <= 393) mimic_alert(o);

                                                if (menv [o].m_beh == 0) menv [o].m_beh = 1;


/*              if (beam[0].thing_thrown == 3 || beam[0].thing_thrown == 4)
                {
                if (beam[0].bx > you[0].x_pos - 7 && beam[0].bx < you[0].x_pos + 7 && beam[0].by > you[0].y_pos - 7 && beam[0].by < you[0].y_pos + 7)
                {
                        viewwindow(1);
                        textcolor(beam[0].colour);
                        gotoxy(beam[0].bx - you[0].x_pos + 18, beam[0].by - you[0].y_pos + 9);
                        putch(35);
                }
                }*/
                                                if (menv [o].m_hp <= 0)
                                                {
                                                        switch(beam[0].thing_thrown)
                                                        {
                                                                case 1: /* You threw */
                                                                case 3: monster_die(o, 3, beam[0].beam_source); break; /* You cast a missile spell which doesn't leave an item. */
                                                                case 2: /* mons threw */
                                                                case 4: monster_die(o, 4, beam[0].beam_source); break; /* mons cast a missile spell which doesn't leave an item. */

                                                        }
                                                }
                                else
                                                {
                                                        if (strcmp(beam[0].beam_name, "sticky flame") == 0)
                                                        sticky_flame_monster(o, 0, hurted);

                 /* looks for missiles which aren't poison but are poison*ed* */
                                        if (strstr(beam[0].beam_name, "poison") != NULL && beam[0].flavour != 6 && random2(hurted) - random2(menv [o].m_AC) > 0)
                                    switch(beam[0].thing_thrown)
                                                    {
                                                        case 1:
                                                        case 3:
                                                                poison_monster(o, 0);
                                                                    break;

                                                            case 2:
                                                            case 4:
                                                                    poison_monster(o, 1);
                                                                    break;
                                                    }
                                }
                                                count_x = 1;

                                                if (beam[0].flavour == 10)
                                                {
                                                        explosion1(beam);
                                                        beam[0].aim_down = 0;
                                                        return;
                                                }

                                                if (strcmp(beam[0].beam_name, "orb of electricity") == 0)
                                                {
                                                        explosion1(beam);
                                beam[0].aim_down = 0;
                                                        return;
                                                }


                                                break;
                                        } else
                                                if (beam[0].thing_thrown != 2 && beam[0].thing_thrown != 4 && mons_near(o) == 1 && (menv [o].m_class < MLAVA0 || menv [o].m_sec == 0)) // No message if monster missile misses
                                                {
                                                        strcpy(info, "The ");
                                                        strcat(info, beam[0].beam_name);
                                                        strcat(info, " misses ");
                                                        strcat(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 1));
                                                        strcat(info, ".");
                                        mpr(info);

                                                        if (you[0].invis != 0 && mons_see_invis(menv [o].m_class) != 0 && menv [o].m_beh == 0) menv [o].m_beh = 1;

                                                }

                                } /* end of if not a tracer*/

                        } // end of if (mgrid ... != MNG)


                        if (count_x == 1)
                        {
                                count_x = 0;
                                break;
                        }

                } // end of if.

                if (beam[0].bx == beam[0].target_x && beam[0].by == beam[0].target_y && beam[0].aim_down == 1)
                {
                        goto landed;
                }


        } // end of for n loop

        landed :
        if (beam[0].flavour == 10)
        {
                explosion1(beam);
                beam[0].aim_down = 0;
                return;
        }

        if (beam [0].colour == 200) return;

        if (strcmp(beam[0].beam_name, "orb of electricity") == 0)
        {
                explosion1(beam);
        beam[0].aim_down = 0;
                return;
        }


        if (beam[0].thing_thrown == 1) // ie if you threw it.
        {
                if (grd [beam[0].bx] [beam[0].by] != 61 && grd [beam[0].bx] [beam[0].by] != 62)
                {
                        if (you[0].inv_class [throw_2] != 1 || random2(2) == 0)
                                item_place(throw_2, beam[0].bx, beam[0].by, 1);
                }

                if (throw_2 == you[0].equip [0])
                {
                        you[0].equip [0] = -1;
                        strcpy(info, "You are empty handed.");
                mpr(info);
                }
        }


        if (beam[0].thing_thrown == 2 && (grd [beam[0].bx] [beam[0].by] != 61 && grd [beam[0].bx] [beam[0].by] != 62) && random2(2) != 0) // monster threw it.
        {

                if (mitm.iquant [throw_2] == 0) // 0 instead of 1 because the mitm.iquant is reduced by 1 when it is thrown.
                {
                } else
            {

                        if (igrd [beam[0].bx] [beam[0].by] != 501)
                        {
                                if ((mitm.iclass [throw_2] == 1 || mitm.iclass [throw_2] == 4 || mitm.iclass [throw_2] == 6 || mitm.iclass [throw_2] == 8 || mitm.iclass [throw_2] == 9) && mitm.iclass [throw_2] == mitm.iclass [igrd [beam[0].bx] [beam[0].by]] && mitm.itype [throw_2] == mitm.itype [igrd [beam[0].bx] [beam[0].by]] && mitm.iplus [throw_2] == mitm.iplus [igrd [beam[0].bx] [beam[0].by]] && mitm.iplus2 [throw_2] == mitm.iplus2 [igrd [beam[0].bx] [beam[0].by]] && mitm.idam [throw_2] == mitm.idam [igrd [beam[0].bx] [beam[0].by]])
                                {
                                        mitm.iquant [igrd [beam[0].bx] [beam[0].by]] ++;
                                        beam[0].aim_down = 0;
                                        return;
                                }
                        } // end of if igrd != 501

                        int o = 0;

                        for (o = 0; o < ITEMS; o++)
                        {
                                if (mitm.iquant [o] == 0)
                                {
                                        mitm.iid [o] = mitm.iid [throw_2];
                                        mitm.iclass [o] = mitm.iclass [throw_2];
                                        mitm.itype [o] = mitm.itype [throw_2];
                                        mitm.iplus [o] = mitm.iplus [throw_2];
                                        mitm.iplus2 [o] = mitm.iplus2 [throw_2];
                                        mitm.idam [o] = mitm.idam [throw_2];
                                        mitm.icol [o] = mitm.icol [throw_2];
                                        mitm.iquant [o] = 1;
                                        mitm.ilink [o] = 501;
        /*                              it_no ++;*/
                                        break;
                                }
                        } // end of o loop

                        if (igrd [beam[0].bx] [beam[0].by] == 501)
                        {
                                igrd [beam[0].bx] [beam[0].by] = o;
                        } else
                        {
                                int hug = igrd [beam[0].bx] [beam[0].by];
                                igrd [beam[0].bx] [beam[0].by] = o;
                                mitm.ilink [o] = hug;
                        }

                } // end of else

        } // if (thing_throw == 2) ...


        beam[0].aim_down = 0;
/* ^ must have this before every return in this function! */

} // end of missile()


/*
Returns the damage taken by a monster from a "flavoured" (fire, ice etc)
attack. Damage from clouds and branded weapons is handled elsewhere.
*/
int check_mons_resists(struct bolt beam [1], int o, int hurted)
{

        switch(beam[0].flavour)
        {
                case 2: /* fire */
                        if (mons_res_fire(menv [o].m_class) > 0)
                        {
                            if (mons_near(o) == 1 && menv [o].m_ench [2] != 6)
                            {
                                        strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                        strcat(info, " appears unharmed.");
                                        mpr(info);
                            }
                            hurted = 0;
                        } else
                if (menv [o].m_inv [2] != 501 && mitm.idam [menv [o].m_inv [2]] % 30 == 2)
                {
                        if (mons_near(o) == 1 && menv [o].m_ench [2] != 6)
                    {
                        strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                strcat(info, " resists.");
                        mpr(info);
                    }
                    hurted /= 3;
                }

                        if (mons_res_fire(menv [o].m_class) == -1 && (menv [o].m_inv [2] == 501 || mitm.idam [menv [o].m_inv [2]] % 30 != 2))
                        {
                        strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                            strcat(info, " is burned terribly!");
                            mpr(info);
                            hurted *= 15;
                        hurted /= 10;
                        }
                        break;


                case 3: /* cold */
                        if (mons_res_cold(menv [o].m_class) > 0)
                        {
                            if (mons_near(o) == 1 && menv [o].m_ench [2] != 6)
                            {
                                        strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                        strcat(info, " appears unharmed.");
                                        mpr(info);
                            }
                                hurted = 0;
                        } else
                if (menv [o].m_inv [2] != 501 && mitm.idam [menv [o].m_inv [2]] % 30 == 3)
                {
                            if (mons_near(o) == 1 && menv [o].m_ench [2] != 6)
                    {
                            strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                        strcat(info, " resists.");
                            mpr(info);
                    }
                    hurted /= 3;
                }

                        if (mons_res_cold(menv [o].m_class) == -1 && (menv [o].m_inv [2] == 501 || mitm.idam [menv [o].m_inv [2]] % 30 != 3))
                        {
                        strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                        strcat(info, " is frozen!");
                        mpr(info);
                        hurted *= 15;
                        hurted /= 10;
                        }
                        break;

                case 5:
                        if (mons_res_elec(menv [o].m_class) > 0)
                        {
                        if (mons_near(o) == 1 && menv [o].m_ench [2] != 6)
                {
                        strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                strcat(info, " appears unharmed.");
                    mpr(info);
                }
                        hurted = 0;
                        }
                        break;


                case 6: /* poison */
                        if (random2(3) != 0)
                        {
                        switch(beam[0].thing_thrown)
                        {
                                    case 1:
                                    case 3:
                                        poison_monster(o, 0);
                                        break;

                                case 2:
                                    case 4:
                                        poison_monster(o, 1);
                                        break;
                        }
                        }

                        if (mons_res_poison(menv [o].m_class) > 0)
                        {
                            if (mons_near(o) == 1 && menv [o].m_ench [2] != 6)
                            {
                                        strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                        strcat(info, " appears unharmed.");
                                        mpr(info);
                            }
                                hurted = 0;
                        }
                        break;

                case 7: /* negativity */
                        if (mons_holiness(menv [o].m_class) > 0)
                        {
                        if (mons_near(o) == 1 && menv [o].m_ench [2] != 6)
                            {
                                        strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                        strcat(info, " appears unharmed.");
                                        mpr(info);
                            }
                            hurted = 0;
                        } else
                        {
                        if (mons_near(o) == 1)
                        {
                                strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                strcat(info, " is drained.");
                                mpr(info);
                        }
                        if (random() % 5 == 0) menv [o].m_HD --;
                        menv [o].m_hp_max -= 2 + random2(3);
                        menv[o].m_hp -= 2 + random2(3);
                        if (menv [o].m_hp >= menv [o].m_hp_max) menv [o].m_hp = menv [o].m_hp_max;
                        if (menv [o].m_HD <= 0) menv [o].m_hp = 0;
                        } /* end else */

                        break;


                case 18: /* flame of cleansing */
                        if (mons_holiness(menv [o].m_class) <= 0)
                        {
                                if (mons_near(o) == 1 && menv [o].m_ench [2] != 6)
                        {
                                        strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                        strcat(info, " appears unharmed.");
                                        mpr(info);
                            }
                            hurted = 0;
                        }
                        break;


        } /* end of switch */


        if (stricmp(beam[0].beam_name, "hellfire") == 0 || beam[0].flavour == 20)
        {
                if (mons_res_fire(menv [o].m_class) == 2)
                {
                if (mons_near(o) == 1 && menv [o].m_ench [2] != 6)
                    {
                                strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                strcat(info, " appears unharmed.");
                                mpr(info);
                        }
                        hurted = 0;
                }
                if (mons_res_fire(menv [o].m_class) == 1 || (menv [o].m_inv [2] != 501 && mitm.idam [menv [o].m_inv [2]] % 30 == 2))
                {
                    if (mons_near(o) == 1 && menv [o].m_ench [2] != 6)
                    {
                                strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                                strcat(info, " partially resists.");
                                mpr(info);
                    }
                        hurted /= 2;
                }
                if (mons_res_fire(menv [o].m_class) == -1 && (menv [o].m_inv [2] == 501 || mitm.idam [menv [o].m_inv [2]] % 30 != 2))
                {
                strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                strcat(info, " is burned terribly!");
                mpr(info);
                    hurted *= 12;/* hellfire */
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

        int mrs = mon_resist_mag(menv [mn].m_class, menv [mn].m_HD);

        if (mrs == 5000) return 0;

//if (random2(pow) + random2(pow) + random2(pow) >= mrs) return 1;
//if (random2(pow) + random2(pow) + random2(pow) + random2(pow) + random2(pow) >= mrs) return 1;
//if (random2(pow) + random2(pow) + random2(pow

        if (pow > 40) pow = ((pow - 40) / 2) + 40;
        if (pow > 70) pow = ((pow - 70) / 2) + 70;
        if (pow > 90) pow = ((pow - 90) / 2) + 90;

        if (pow > 120) pow = 120;


        int mrchance = 100 + mrs;
        mrchance -= pow;
        int mrch2 = random2(100) + random2(101);

#ifdef DEBUG
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

        if (mrch2 < mrchance) return 0; /* ie saved successfully */

        return 1;

} /* end of check_mons_magres */

/*
Enchants all monsters in player's sight.
*/
void mass_enchantment(int wh_enchant, int pow)
{

        int i = 0;
        int p;
        char brek = 0;

        viewwindow(0);
        for (i = 0; i < MNST; i ++)
        {
                if (menv [i].m_class == -1) continue;
        if (mons_near(i) == 0) continue;

                if (wh_enchant == 30 && mons_holiness(menv [i].m_class) != 1) continue;
  /* assuming that the only mass charm is control undead */

                if (wh_enchant != 30 || mons_holiness(menv [i].m_class) != 1)
/*   if (check_mons_magres(i, pow) == 0 && beam[0].colour != 1 && beam[0].colour != 2 && beam[0].colour != 5) */
                if (check_mons_magres(i, pow) == 0) /* && beam[0].colour != 1 && beam[0].colour != 2 && beam[0].colour != 5) */
                {
                strcpy(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 0));
                strcat(info, " resists.");
                mpr(info);
/*      beam[0].aim_down = 0;*/
                        continue;
                }

                if (menv [i].m_x > you[0].x_pos - 9 && menv [i].m_x < you[0].x_pos + 9 && menv [i].m_y > you[0].y_pos - 9 && menv [i].m_y < you[0].y_pos + 9 && see_grid(menv [i].m_x, menv [i].m_y) != 0) /*show [menv [i].m_x - you[0].x_pos + 9] [menv [i].m_y - you[0].y_pos + 9] != 0)*/
                {
                        if (menv [i].m_ench_1 == 1)
                                for (p = 0; p < 3; p ++)
                                {
                                        if (menv [i].m_ench [p] == wh_enchant) brek = 1;
                                } // end of for p

                        for (p = 0; p < 3; p ++)
                        {
                                if (brek == 1)
                                {
                                        brek = 0;
                                        break;
                                }
                                if (menv [i].m_ench [p] == 0)
                                {
                                        menv [i].m_ench [p] = wh_enchant;
                                        menv [i].m_ench_1 = 1;
                                if (menv [i].m_ench [2] != 6 || player_see_invis() != 0)
                                    {
                                        strcpy(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 0));
                                            switch(wh_enchant)
                                        {
                                                case 4: strcat(info, " looks frightened.");
                                                        break;
                                                case 5: strcat(info, " looks rather confused.");
                                                        break;
                                                        case 30: strcat(info, " is charmed.");
                                                            break;
                                        }
                                        mpr(info);
                                            break;
                                        } else mpr("Nothing appears to happen.");
                                }
                        } // end of for p
                } // end of if visible
        } // end of for i
} // end of void cause_fear()


/*
Monster has probably failed save, now it gets enchanted somehow.
*/
int mons_ench_f2(int o, char is_near, int func_pass [10], struct bolt beam [1])
{

        char brek = 0;
        char p;

    switch(beam[0].colour) /* put in magic resistance */
    {
        case 0: /* slow monster */
                for (p = 0; p < 3; p ++)
            {
                if (menv [o].m_ench [p] == 1)
                {
                        strcpy(info, "Nothing appears to happen.");
                    mpr(info);
                    return 1;
                }
                if (menv [o].m_ench [p] == 2)
                {
                        menv [o].m_ench [p] = 0;
                    if (menv [o].m_speed >= 100)
                    {
                        menv [o].m_speed = ((menv [o].m_speed - 100) / 2) + 100;
                    } else menv [o].m_speed /= 2;
                    if (menv [o].m_ench [0] == 0 && menv [o].m_ench [1] == 0 && menv [o].m_ench [2] == 0)
                    menv [o].m_ench_1 = 0;
                    brek = 1;
                    break;
                }
                } /* end of for p */

            if (brek == 0)
                for (p = 0; p < 3; p ++)
                {
                        if (menv [o].m_ench [p] == 0) /* || p == 2) replaces 3rd enchantment if all full. */
                    {
                        menv [o].m_ench [p] = 1;
                        if (menv [o].m_speed >= 100)
                        {
                                menv [o].m_speed = ((menv [o].m_speed - 100) / 2) + 100;
                        } else menv [o].m_speed /= 2;
                        menv [o].m_ench_1 = 1;
                        break;
                    }
                        }


              /* put in an exception for fungi, plants and other things you won't notice slow down. */
                strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
            strcat(info, " seems to slow down.");
                mpr(info);
            func_pass [1] = 1;
            return 1;

                case 1: // haste
                for (p = 0; p < 3; p ++)
            {
                if (menv [o].m_ench [p] == 2) goto nothinghap;
                if (menv [o].m_ench [p] == 1)
                {
                        menv [o].m_ench [p] = 0;
                    if (menv [o].m_speed >= 100)
                    {
                        menv [o].m_speed = ((menv [o].m_speed - 100) * 2) + 100;
                    } else menv [o].m_speed *= 2;
                        if (menv [o].m_ench [0] == 0 && menv [o].m_ench [1] == 0 && menv [o].m_ench [2] == 0)
                        menv [o].m_ench_1 = 0;
                    brek = 1;
                    break;
                }
                } /* end of for p */

            if (brek == 0)
                for (p = 0; p < 3; p ++)
                {
                        if (menv [o].m_ench [p] == 0)
                    {
                        menv [o].m_ench [p] = 2;
                        if (menv [o].m_speed >= 100)
                        {
                                menv [o].m_speed = ((menv [o].m_speed - 100) * 2) + 100;
                        } else menv [o].m_speed *= 2;
                        menv [o].m_ench_1 = 1;
                        break;
                    }
                }
            if (p == 2 && menv [o].m_ench [p] != 0) goto nothinghap;

            /* put in an exception for fungi, plants and other things you won't notice speed up. */
            strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
            strcat(info, " seems to speed up.");
                mpr(info);
            func_pass [1] = 1;
            return 1;


        case 2: /* healing */
            if (menv [o].m_hp == menv [o].m_hp_max)
            {
                            goto nothinghap;
            }
            menv [o].m_hp += random2(15) + 5;
            if (menv [o].m_hp >= menv [o].m_hp_max)
            {
                menv [o].m_hp = menv [o].m_hp_max;
                strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                strcat(info, "'s wounds heal themselves!");
                mpr(info);
                func_pass [1] = 1;
                return 1;
                }
            strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
            strcat(info, " is healed somewhat."); /* is a little less battered */
            mpr(info);
            func_pass [1] = 1;
            return 1;

        case 3: /* paralysis */
                        menv [o].m_speed_inc = 0;
                if (is_near == 1)
            {
                strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                strcat(info, " suddenly stops moving!");
                mpr(info);
                func_pass [1] = 1;
            }
                        if (grd [menv [o].m_x] [menv [o].m_y] == 11 || grd [menv [o].m_x] [menv [o].m_y] == 12)
            {
                        if (mons_flies(menv [o].m_class) == 1)
                {
                        if (is_near == 1) /* don't worry about invisibility - you should be able to see if something has fallen into the lava */
                    {
                            strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));//monam (menv [o].m_class, menv [o].m_ench [2], 0, you[0].see_invis)); //gmon_name [menv [o].m_class]);
                                                if (grd [menv [o].m_x] [menv [o].m_y] == 12)
                        {
                                strcat(info, " falls into the water!");
                                                } else strcat(info, " falls into the lava!");
                        mpr(info);
                        }
                                        switch(beam[0].thing_thrown)
                                        {
                                                case 1:                    /* your beam */
                                                case 3: monster_die(o, 1, beam[0].beam_source);
                                                        break;                     /*  "    " */
                                                case 2: monster_die(o, 4, beam[0].beam_source);
                                                        break;                     /* dragon breath &c */
                                        }
                }
            }
                        return 1;

        case 4: /* confusion */
            for (p = 0; p < 3; p ++)
            {
                    if (menv [o].m_ench [p] == 4) return 1;
            } /* end of for p */
            for (p = 0; p < 3; p ++)
            {
                if (menv [o].m_ench [p] == 0 || p == 2) /* replaces 3rd enchantment if all full. */
                {
                        menv [o].m_ench [p] = 5;
                    menv [o].m_ench_1 = 1;
                    break;
                }
            }
                /* put in an exception for fungi, plants and other things you won't notice you[0].slow down. */
            if (is_near == 1)
            {
                    strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                strcat(info, " appears confused.");
                mpr(info);
                func_pass [1] = 1;
            }
            return 1;


        case 5: /* invisibility */
                if (menv [o].m_ench [2] == 6 || is_near != 1)
                goto nothinghap;

            if (menv [o].m_ench [2] != 0)
            {
                       strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
               strcat(info, " flickers for a moment.");
               mpr(info);
               func_pass [1] = 1;
               return 1;
                }

            if (is_near == 1)
            {
                    strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));//monam (menv [o].m_class, menv [o].m_ench [2], 0, you[0].see_invis)); //gmon_name [menv [o].m_class]);
                strcat(info, " flickers and vanishes!");
                mpr(info);
                func_pass [1] = 1;
            }
            menv [o].m_ench [2] = 6;
            return 1;

                /* 6 is used by digging
                   7            teleport
                   8            polymorph */
                case 9: /* charm */
                for (p = 0; p < 3; p ++)
            {
                if (menv [o].m_ench [p] == 7) return 1;
            } /* end of for p */
            for (p = 0; p < 4; p ++)
            {
                                if (p == 3) return 1;
                if (menv [o].m_ench [p] == 0) /* replaces 3rd enchantment if all full. */
                {
                        menv [o].m_ench [p] = 30;
                    menv [o].m_ench_1 = 1;
                                    menv [o].m_beh = 7;
                        break;
                }
            }
            /* put in an exception for fungi, plants and other things you won't notice slow down. */
            if (is_near == 1)
            {
                strcpy(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 0));
                strcat(info, " is charmed.");
                mpr(info);
                func_pass [1] = 1;
            }
            return 1;

    } /* end of switch (beam_colour) */


    goto somethinghap;


    nothinghap:
    strcpy(info, "Nothing appears to happen.");
    mpr(info);


    somethinghap: return 1;


}






/*
Puts the poison value into a monster's enchantment variable.
*/
void poison_monster(int mn, char source)
{

        int p;
        int brek = 0;

        if (menv [mn].m_class == -1) return;

        if (mons_res_poison(menv [mn].m_class) > 0) return;
        if (menv [mn].m_inv [2] != 501 && mitm.idam [menv [mn].m_inv [2]] % 30 == 4) return;

        for (p = 0; p < 3; p ++)
    {
            if (menv [mn].m_ench [p] % 50 == 10)
        {
            return;
        }
        if (menv [mn].m_ench [p] % 50 >= 7 && menv [mn].m_ench [p] % 50 < 10)
        {
                menv [mn].m_ench [p] ++;
            brek = 1;
                if (mons_near(mn) == 1 && (menv [mn].m_ench [2] != 6 || player_see_invis() != 0))
                        {
                        strcpy(info, monam (menv [mn].m_sec, menv [mn].m_class, menv [mn].m_ench [2], 0));
                            strcat(info, " looks rather sicker.");
                            mpr(info);
                    }
                    break;
            }
    } /* end of for p */

    if (brek == 0)
        for (p = 0; p < 3; p ++)
        {

            if (menv [mn].m_ench [p] == 0)
            {
                    menv [mn].m_ench [p] = 7;
                menv [mn].m_ench_1 = 1;
                if (mons_near(mn) == 1 && (menv [mn].m_ench [2] != 6 || player_see_invis() != 0))
                {
                        strcpy(info, monam (menv [mn].m_sec, menv [mn].m_class, menv [mn].m_ench [2], 0));
                    strcat(info, " looks rather sick.");
                    mpr(info);
                }
                break;
            }
        }

        if (source == 1 && menv [mn].m_ench [p] <= 10 && menv [mn].m_ench [p] >= 7)
        {
        menv [mn].m_ench [p] += 50;
        }

}


/*
Like poison_monster; makes the monster burn if hit by napalm.
*/
void sticky_flame_monster(int mn, char source, int power) /* modelled on the poison_monster function */
{

        int long_last = 0;
        int brek = 0;
        int p;

        if (menv [mn].m_class == -1) return;

        if (mons_res_fire(menv [mn].m_class) > 0) return;
        if (menv [mn].m_inv [2] != 501 && mitm.idam [menv [mn].m_inv [2]] % 30 == 2) return;

        long_last = random2(power) / 2 + 1;
        if (long_last >= 4) long_last = 4;

        for (p = 0; p < 3; p ++)
    {
            if (menv [mn].m_ench [p] == 34 || menv [mn].m_ench [p] == 64) // already covered in sticky flame - ouch!
        {
                return;
        }
        if ((menv [mn].m_ench [p] >= 31 && menv [mn].m_ench [p] < 34) || (menv [mn].m_ench [p] >= 61 && menv [mn].m_ench [p] < 64))
        {
            menv [mn].m_ench [p] += long_last;
            if (menv [mn].m_ench [p] > 34 && menv [mn].m_ench [p] < 60) menv [mn].m_ench [p] = 34;
            if (menv [mn].m_ench [p] > 64) menv [mn].m_ench [p] = 64;
            brek = 1;
                    if (mons_near(mn) == 1 && (menv [mn].m_ench [2] != 6 || player_see_invis() != 0))
                    {
                        strcpy(info, monam (menv [mn].m_sec, menv [mn].m_class, menv [mn].m_ench [2], 0));
                        strcat(info, " is covered in liquid fire!");
                        mpr(info);
                }
                    break;
                }
    } /* end of for p */

    if (brek == 0)
        for (p = 0; p < 3; p ++)
        {
                if (menv [mn].m_ench [p] == 0)
            {
                menv [mn].m_ench [p] = 30 + long_last;
                menv [mn].m_ench_1 = 1;
                    if (mons_near(mn) == 1 && (menv [mn].m_ench [2] != 6 || player_see_invis() != 0))
                {
                        strcpy(info, monam (menv [mn].m_sec, menv [mn].m_class, menv [mn].m_ench [2], 0));
                    strcat(info, " is covered in liquid fire!");
                    mpr(info);
                }
                break;
                }
        }

        if (source == 1 && menv [mn].m_ench [p] <= 60 && menv [mn].m_ench [p] >= 31)
        {
        menv [mn].m_ench [p] += 30;
        }

}

/*
Places a cloud with the given stats. May delete old clouds to make way if
there are too many (30) on level, but won't place a cloud over another
cloud.
*/
void place_cloud(unsigned char cl_type, unsigned char ctarget_x, unsigned char ctarget_y, unsigned char cl_range) // if env[0].cloud_type > 100, it is a monster's cloud
{
        unsigned char ci = 0;

        char c_deleted = 100;

        if (env[0].cloud_no >= 30) c_deleted = random2(30);

        if (env[0].cgrid [ctarget_x] [ctarget_y] != CNG) return; // hmmm...

    for (ci = 0; ci < CLOUDS; ci++)
    {
                if (ci == c_deleted)
                {
                        env[0].cloud_type [ci] = 0;
                        env[0].cgrid [env[0].cloud_x [ci]] [env[0].cloud_y [ci]] = CNG;
                        env[0].cloud_decay [ci] = 0;
                        env[0].cloud_no--;
                }

        if (env[0].cloud_type [ci] == 0) // ie is empty
        {
                env[0].cloud_type [ci] = cl_type;
            env[0].cloud_x [ci] = ctarget_x;
            env[0].cloud_y [ci] = ctarget_y;
            env[0].cloud_decay [ci] = cl_range * 10;
            env[0].cloud_no++;
            env[0].cgrid [env[0].cloud_x [ci]] [env[0].cloud_y [ci]] = ci;
            break;
        }

        }

//      move_x = 0; move_y = 0;

} // end of void place_cloud();



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
void tracer_f(int i, struct bolt beem [1])
{
        strcpy(beem[0].beam_name, "0tracer");
        beem[0].trac_hit_mons = mons_see_invis(menv [i].m_class);
        beem[0].tracer = 0;
        beem[0].tracer_mons = 0;
        beem[0].trac_targ = menv [i].m_hit;
        beem[0].colour = 200;
        beem[0].beam_name [0] = 48;
        beem[0].type = 0;
        beem[0].flavour = 4;
        beem[0].damage = 0;
        beem[0].range = 10;
        beem[0].move_x = beem[0].trac_targ_x - menv [i].m_x;
        beem[0].move_y = beem[0].trac_targ_y - menv [i].m_y;
        beem[0].source_x = menv [i].m_x;
        beem[0].source_y = menv [i].m_y;
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

for (p = 0; p < 3; p ++)
{
  if (menv [mimic].m_ench [p] >= 40 && menv [mimic].m_ench [p] <= 43)
    return;
}

monster_teleport(mimic, 0);

}

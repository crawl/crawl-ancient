#include "config.h"

#ifdef DOS
#include <conio.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "externs.h"
#include "enum.h"
#include "fight.h"

#include "bang.h"
#include "beam.h"
#include "direct.h"
#include "effects.h"
#include "fight.h"
#include "itemname.h"
#include "message.h"
#include "misc.h"
#include "monstuff.h"
#include "mstruct.h"
#include "ouch.h"
#include "output.h"
#include "player.h"
#include "skills.h"
#include "skills2.h"
#include "spell.h"
#include "spells.h"
#include "spells0.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "stuff.h"
#include "view.h"

/*
Takes a bolt struct and refines it for use in the explosion function. Called
from missile() and beam() in beam.cc. Explosions which do not follow from
beams (eg scrolls of immolation) bypass this function.
*/
void explosion1(struct bolt beam [1])
{
        char exsize = 0;


        if (stricmp(beam[0].beam_name, "hellfire") == 0)
        {
        if (see_grid(beam[0].bx, beam[0].by) == 1 || (you[0].x_pos == beam[0].bx && you[0].y_pos == beam[0].by)) strcpy(info, "The hellfire explodes!");
            else strcpy(info, "You hear a strangely unpleasant explosion.");
                mpr(info);
                beam[0].type = 43;
                beam[0].damage = 115;
                beam[0].flavour = 13; // hellfire
        }

        if (stricmp(beam[0].beam_name, "golden flame") == 0)
        {
            if (see_grid(beam[0].bx, beam[0].by) == 1 || (you[0].x_pos == beam[0].bx && you[0].y_pos == beam[0].by)) strcpy(info, "The flame explodes!");
            else strcpy(info, "You hear a strange explosion.");
                mpr(info);
                beam[0].type = 43;
                beam[0].flavour = 18;
        }


        if (stricmp(beam[0].beam_name, "fireball") == 0)
        {
            if (see_grid(beam[0].bx, beam[0].by) == 1 || (you[0].x_pos == beam[0].bx && you[0].y_pos == beam[0].by)) strcpy(info, "The fireball explodes!");
        else strcpy(info, "You hear an explosion.");
                mpr(info);
                beam[0].type = 43;
                beam[0].damage += 100; // it should already be set for this.
                beam[0].flavour = 2;
        }

        if (stricmp(beam[0].beam_name, "orb of electricity") == 0)
        {
        if (see_grid(beam[0].bx, beam[0].by) == 1 || (you[0].x_pos == beam[0].bx && you[0].y_pos == beam[0].by)) strcpy(info, "The orb of electricity explodes!");
        else strcpy(info, "You hear a clap of thunder!");
                mpr(info);
                beam[0].type = 43;
                beam[0].damage += 100; // it should already be set for this.
                beam[0].flavour = 5;
        beam[0].colour = LIGHTCYAN;
        exsize = 1;
        }

        if (stricmp(beam[0].beam_name, "blast") == 0)
        {
        if (see_grid(beam[0].bx, beam[0].by) == 1 || (you[0].x_pos == beam[0].bx && you[0].y_pos == beam[0].by)) strcpy(info, "The orb of energy explodes.");
        else strcpy(info, "You hear an explosion.");
                mpr(info);
        exsize = 0;
        }

        if (stricmp(beam[0].beam_name, "metal orb") == 0)
        {
        if (see_grid(beam[0].bx, beam[0].by) == 1 || (you[0].x_pos == beam[0].bx && you[0].y_pos == beam[0].by)) strcpy(info, "The orb explodes into a blast of deadly shrapnel!");
        else strcpy(info, "You hear an explosion!");
                mpr(info);
        strcpy(beam[0].beam_name, "blast of shrapnel");
                beam[0].type = '#';
                beam[0].damage += 100; // it should already be set for this.
                beam[0].flavour = 19; /* sets it from pure damage to shrapnel, which is absorbed extra by armour */
        exsize = 0;
        }

        if (stricmp(beam[0].beam_name, "great blast of cold") == 0)
        {
        if (see_grid(beam[0].bx, beam[0].by) == 1 || (you[0].x_pos == beam[0].bx && you[0].y_pos == beam[0].by)) strcpy(info, "The blast explodes into a great storm of ice!");
         else strcpy(info, "You hear a raging storm!");
                mpr(info);
        strcpy(beam[0].beam_name, "ice storm");
                beam[0].type = '#';
                beam[0].damage += 100; // it should already be set for this.
        beam[0].colour = WHITE;
        exsize = 1;
        }



/*
if (stricmp(beam[0].beam_name, "concussion rocket") == 0)
{
        if (see_grid(beam[0].bx, beam[0].by) == 1) strcpy(info, "The rocket explodes!");
           else strcpy(info, "You hear a powerful explosion.");
        mpr(info);
        strcpy(beam[0].beam_name, "explosion");
        beam[0].type = 43;
        beam[0].damage = 112;
        beam[0].flavour = 0;
        beam[0].colour = LIGHTGRAY;
}

if (stricmp(beam[0].beam_name, "fragmentation rocket") == 0)
{
        if (see_grid(beam[0].bx, beam[0].by) == 1) strcpy(info, "The rocket explodes!");
           else strcpy(info, "You hear a fragmentary explosion."); // this is not proper usage, BTW
        mpr(info);
        strcpy(beam[0].beam_name, "blast of shrapnel");
        beam[0].type = 43;
        beam[0].damage = 110;
        beam[0].flavour = 0; // hmmm
        beam[0].colour = CYAN;
        exsize = 1;
}

if (stricmp(beam[0].beam_name, "incendiary rocket") == 0)
{
        if (see_grid(beam[0].bx, beam[0].by) == 1) strcpy(info, "The rocket explodes!");
           else strcpy(info, "You hear an explosion.");
        mpr(info);
        strcpy(beam[0].beam_name, "fiery explosion");
        beam[0].type = 43;
        beam[0].damage = 114;
        beam[0].flavour = 2;
        beam[0].colour = RED;
}
*/
/*
if (stricmp(beam[0].beam_name, "blast") == 0)
{
        if (see_grid(beam[0].bx, beam[0].by) == 1) strcpy(info, "The blast explodes!");
           else strcpy(info, "You hear an explosion.");
        mpr(info);
//      strcpy(info, "The blast explodes!");
//      mpr(info);
        beam[0].type = 43;
        beam[0].flavour = 0;
}
*/
        explosion(exsize, beam);
}







/*
Makes a bang. ex_size determines whether the explosion is 3x3 or 5x5 without
corners.
*/
void explosion(char ex_size, struct bolt beam [1])
{

        char clouty = 0;
        char empty [5] [5];
        int n = 0;
        int q = 0;

        for (n = 0; n < 5; n ++)
        {
                for (q = 0; q < 5; q ++)
                {
                        empty [n] [q] = 0;
                }
        }

        int hurted = 0;

        beam[0].source_x = beam[0].bx;
        beam[0].source_y = beam[0].by;


        for (n = 0; n < 5; n++) // x
        {
                for (q = 0; q < 5; q++) // y
                {
                        empty [n] [q] = 1;
                        if (grd [beam[0].source_x - 2 + n] [beam[0].source_y - 2 + q] < 10) empty [n] [q] = 0;
                }
        }

        if (ex_size == 0)
        {
                for (n = 0; n < 5; n ++)
                {
                        empty [n] [0] = 0;
                        empty [n] [4] = 0;
                        empty [0] [n] = 0;
                        empty [4] [n] = 0;
                }
        }

        if (empty [1] [2] == 0) empty [0] [2] = 0;
        if (empty [3] [2] == 0) empty [3] [2] = 0;

        if (empty [2] [3] == 0) empty [4] [3] = 0;
        if (empty [2] [1] == 0) empty [4] [1] = 0;

        if (empty [1] [1] == 0) empty [0] [0] = 0;
        if (empty [3] [3] == 0) empty [4] [4] = 0;

        if (empty [1] [3] == 0) empty [0] [4] = 0;
        if (empty [3] [1] == 0) empty [4] [0] = 0;

        if (empty [1] [3] == 0) empty [0] [3] = 0;
        if (empty [1] [3] == 0) empty [1] [4] = 0;

        if (empty [1] [1] == 0) empty [1] [0] = 0;
        if (empty [1] [1] == 0) empty [0] [1] = 0;

        if (empty [3] [1] == 0) empty [3] [0] = 0;
        if (empty [3] [1] == 0) empty [4] [1] = 0;

        if (empty [3] [3] == 0) empty [4] [3] = 0;
        if (empty [3] [3] == 0) empty [3] [4] = 0;

        empty [0] [0] = 0;
        empty [4] [0] = 0;
        empty [0] [4] = 0;
        empty [4] [4] = 0;

//      brek = 1;
//      viewwindow(); // this resets the window so that eg dragons don't appear to do weird things.
//      brek = 0;


        for (n = -2; n < 3; n++) // x
        {

                for (q = -2; q < 3; q++) // y
                {

                        beam[0].bx = beam[0].source_x + n;
                        beam[0].by = beam[0].source_y + q;

                        if (empty [n + 2] [q + 2] == 0) continue;

                        if (grd [beam[0].bx] [beam[0].by] < 10) continue;

//      if (beam[0].bx > you[0].x_pos - 9 && beam[0].bx < you[0].x_pos + 9 && beam[0].by > you[0].y_pos - 9 && beam[0].by < you[0].y_pos + 9 && (show [beam[0].bx - you[0].x_pos + 9] [beam[0].by - you[0].y_pos + 9] != 0 || (beam[0].bx == you[0].x_pos && beam[0].by == you[0].y_pos)))// && beam[0].type != 0)
                if (see_grid(beam[0].bx, beam[0].by) == 1 || (beam[0].bx == you[0].x_pos && beam[0].by == you[0].y_pos))
                        {
                        //viewwindow;
                        //window(2,2,34,17);
                                textcolor(beam[0].colour);
                //show [beam[0].bx - you[0].x_pos + 7] [beam[0].by - you[0].y_pos + 7] = 35;
                                gotoxy(beam[0].bx - you[0].x_pos + 18, beam[0].by - you[0].y_pos + 9);
                                putch(35);
                        }

                }
        }

//delay(500);
// test relay_message();

//scrloc = 0;
        more();

        for (n = -2; n < 3; n++) // x
        {

                for (q = -2; q < 3; q++) // y
                {

                        beam[0].bx = beam[0].source_x + n;
                        beam[0].by = beam[0].source_y + q;

                        if (empty [n + 2] [q + 2] == 0) continue;



//      }

                        if (grd [beam[0].bx] [beam[0].by] < 10) continue;

                        if (igrd [beam[0].bx] [beam[0].by] != 501)
                        {
                                if (mitm.iclass [igrd [beam[0].bx] [beam[0].by]] == 6)
                                {
                                        if (beam[0].flavour == 2 || stricmp(beam[0].beam_name, "hellfire") == 0)
                                        {
                                                destroy_item(igrd [beam[0].bx] [beam[0].by]);
                                                if (see_grid(beam[0].bx, beam[0].by) == 1)
                                                {
                                                strcpy(info, "You see a puff of smoke.");
                                                    mpr(info);
                                                }
                                        }
                                }
                        }


                        if (beam[0].bx == you[0].x_pos && beam[0].by == you[0].y_pos) // && beam[0].colour != 6) // have to do something about enchantments here.
                        {                                                       // ^^^ digging

                                if (beam[0].colour == 200)
                                {
                                        beam[0].tracer = 1;
                                        return;
                                }



                                strcpy(info, "The ");
                                strcat(info, beam[0].beam_name);
                                strcat(info, " engulfs you!");
                                mpr(info);

                                hurted = 0;

                                if (beam[0].damage > 100)
                                {
                                        hurted += random2(beam[0].damage - 100);
                                        hurted += random2(beam[0].damage - 100);
                                        hurted += random2(beam[0].damage - 100);
                                } else hurted += random2(beam[0].damage);

                                hurted = check_your_resists(hurted, beam[0].flavour);

                                hurted -= random2(player_AC() + 1);
                                if (beam[0].flavour == 19) // shrapnel
                                {
                                        hurted -= random2(player_AC() + 1);
                                        hurted -= random2(player_AC() + 1);
                                }
                                if (you[0].equip [EQ_BODY_ARMOUR] != -1)
                                if (random() % 1000 <= mass(OBJ_ARMOUR, you[0].inv_type [you[0].equip [EQ_BODY_ARMOUR]]) && random() % 4 == 0)
                                exercise(SK_ARMOUR, 1);

                                if (hurted <= 0) hurted = 0;
                                strcat(info, "!");

                                if (beam[0].flavour == 2 || stricmp(beam[0].beam_name, "hellfire") == 0) scrolls_burn(5, 6);
                                if (beam[0].flavour == 3) scrolls_burn(5, 8);
                                if (beam[0].flavour == 11) scrolls_burn(2, 4); // spores!

                if (beam[0].thing_thrown == 1 || beam[0].thing_thrown == 3)
                                 ouch(hurted, 0, 22);
                  else
                                   ouch(hurted, beam[0].beam_source, 3);

                                you[0].hp_ch = 1;

        //n++; // reduces beam's range
                        }


                        if (env[0].cgrid [beam[0].bx] [beam[0].by] != CNG) // hit a cloud
                        {

                                clouty = env[0].cgrid [beam[0].bx] [beam[0].by];

//1 = fire, 3 = cold, 2 = stinking

                                if ((env[0].cloud_type [clouty] == 3 && beam[0].flavour == 2) || (env[0].cloud_type [clouty] == 1 && beam[0].flavour == 3))
                                {
                                        if (see_grid(beam[0].bx, beam[0].by) == 1)
                                        {
                                                strcpy(info, "You hear a sizzling sound!");
                                                mpr(info);
                                        }

                                        env[0].cloud_type [clouty] = 0;
                                        env[0].cgrid [env[0].cloud_x [clouty]] [env[0].cloud_y [clouty]] = CNG;
                                        env[0].cloud_decay [clouty] = 0;
                                        env[0].cloud_no--;

                                }
                        } else
                                if (stricmp(beam[0].beam_name, "ice storm") == 0)
                                {
                 place_cloud(3, beam[0].bx, beam[0].by, 2 + random2(3) + random2(3));
                                }



                        int o = 0;

                        if (mgrd [beam[0].bx] [beam[0].by] != MNG)
                        {
                                if (beam[0].beam_name [0] != 48)
                                {
                                        o = mgrd [beam[0].bx] [beam[0].by];
                                        if (menv [o].m_class >= MLAVA0 && menv [o].m_sec == 1) continue;

                                        hurted = 0;

                                        if (beam[0].damage > 100)
                                        {
                                                hurted += random2(beam[0].damage - 100);
                                                hurted += random2(beam[0].damage - 100);
                                                hurted += random2(beam[0].damage - 100);
                                        } else hurted += random2(beam[0].damage);

                                        hurted -= random2(menv [o].m_AC + 1);
                                        if (beam[0].flavour == 19) // shrapnel
                                        {
                                                hurted -= random2(menv [o].m_AC + 1);
                                                hurted -= random2(menv [o].m_AC + 1);
                                        }
                                        if (hurted <= 0) hurted = 0;

                                        strcpy(info, "The ");
                                        strcat(info, beam[0].beam_name);
                                        strcat(info, " engulfs ");
                                        strcat(info, monam (menv [o].m_sec, menv [o].m_class, menv [o].m_ench [2], 1)); //gmon_name [mons_class [o]]);
                                        strcat(info, ".");
                                        mpr(info);

                                        hurted = check_mons_resists(beam, o, hurted);

                                        menv [o].m_hp -= hurted;

                                        if (menv [o].m_beh == 7)
                                        {
                                if ((beam[0].thing_thrown == 3 || beam[0].thing_thrown == 1) && hurted > 0) menv [o].m_beh = 1;
                                        } // else menv [o].m_beh = 1;

                                        if (menv [o].m_hp <= 0)
                                        {
                                                switch(beam[0].thing_thrown)
                                                {
                                                        case 1: monster_die(o, 3, beam[0].beam_source); break; // your explosion
                                                        case 3: monster_die(o, 3, beam[0].beam_source); break; //  "    "
                                                        case 2: monster_die(o, 4, beam[0].beam_source); break; // someone else's
                                                        case 4: monster_die(o, 4, beam[0].beam_source); break; // someone else's
                                                }
                                        } else if ((beam[0].thing_thrown == 1 || beam[0].thing_thrown == 3) && mons_near(o) == 1)
                           {
                             print_wounds(o);
                             if (menv [o].m_class >= 389 && menv [o].m_class <= 393) mimic_alert(o);
                           }
                                }
                        }

                } // end of for q
        } // end of for n


} // end of void explosion

/*
*  File:       bang.cc
*  Summary:    Functions related to explosions.
*  Written by: Linley Henzell
*
*  Change History (most recent first):
*
*  <2>    11/05/99        cdl             new ouch() call for spores
*  <1>     -/--/--        LRH             Created
*/

#include "AppHdr.h"
#include "bang.h"

#ifdef DOS
#include <conio.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "externs.h"

#include "fight.h"
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
void explosion1(struct bolt beam[1])
{
  char exsize = 0;

  if (stricmp(beam[0].beam_name, "hellfire") == 0)
    {
      if ( see_grid(beam[0].bx, beam[0].by)
            || ( you.x_pos == beam[0].bx && you.y_pos == beam[0].by ) )
        strcpy(info, "The hellfire explodes!");
      else
        strcpy(info, "You hear a strangely unpleasant explosion.");
      mpr(info);
      beam[0].type = 43;
      beam[0].damage = 115;
      beam[0].flavour = BEAM_HELLFIRE;        // hellfire - differ fr. DMNBM_HELLFIRE? [dlb]
    }

  if (stricmp(beam[0].beam_name, "golden flame") == 0)
    {
      if ( see_grid(beam[0].bx, beam[0].by)
            || ( you.x_pos == beam[0].bx && you.y_pos == beam[0].by ) )
        strcpy(info, "The flame explodes!");
      else
        strcpy(info, "You hear a strange explosion.");
      mpr(info);
      beam[0].type = 43;
      beam[0].flavour = BEAM_HOLY;            // same as golden flame? [dlb]
    }

  if (stricmp(beam[0].beam_name, "fireball") == 0)
    {
      if ( see_grid(beam[0].bx, beam[0].by)
            || ( you.x_pos == beam[0].bx && you.y_pos == beam[0].by ) )
        strcpy(info, "The fireball explodes!");
      else
        strcpy(info, "You hear an explosion.");
      mpr(info);
      beam[0].type = 43;
      beam[0].damage += 100;  // it should already be set for this.
      beam[0].flavour = BEAM_FIRE;
    }

  if (stricmp(beam[0].beam_name, "orb of electricity") == 0)
    {
      if ( see_grid(beam[0].bx, beam[0].by)
            || ( you.x_pos == beam[0].bx && you.y_pos == beam[0].by ) )
        strcpy(info, "The orb of electricity explodes!");
      else
        strcpy(info, "You hear a clap of thunder!");
      mpr(info);
      beam[0].type = 43;
      beam[0].damage += 100;  // it should already be set for this.
      beam[0].flavour = BEAM_ELECTRICITY;
      beam[0].colour = LIGHTCYAN;
      exsize = 1;
    }

  if (stricmp(beam[0].beam_name, "blast") == 0)
    {
      if ( see_grid(beam[0].bx, beam[0].by)
            || ( you.x_pos == beam[0].bx && you.y_pos == beam[0].by ) )
        strcpy(info, "The orb of energy explodes.");
      else
        strcpy(info, "You hear an explosion.");
      mpr(info);
      exsize = 0;
    }

  if (stricmp(beam[0].beam_name, "metal orb") == 0)
    {
      if ( see_grid(beam[0].bx, beam[0].by)
            || ( you.x_pos == beam[0].bx && you.y_pos == beam[0].by ) )
        strcpy(info, "The orb explodes into a blast of deadly shrapnel!");
      else
        strcpy(info, "You hear an explosion!");
      mpr(info);
      strcpy(beam[0].beam_name, "blast of shrapnel");
      beam[0].type = SYM_ZAP;
      beam[0].damage += 100;  // it should already be set for this.
      beam[0].flavour = BEAM_FRAG;   // sets it from pure damage to shrapnel (which is absorbed extra by armour)
      exsize = 0;
    }

  if (stricmp(beam[0].beam_name, "great blast of cold") == 0)
    {
      if ( see_grid(beam[0].bx, beam[0].by)
            || ( you.x_pos == beam[0].bx && you.y_pos == beam[0].by ) )
        strcpy(info, "The blast explodes into a great storm of ice!");
      else
        strcpy(info, "You hear a raging storm!");
      mpr(info);
      strcpy(beam[0].beam_name, "ice storm");
      beam[0].type = SYM_ZAP;
      beam[0].damage += 100;  // it should already be set for this.
      beam[0].colour = WHITE;
      exsize = 1;
    }


/*
if (stricmp(beam[0].beam_name, "concussion rocket") == 0)
{
if (see_grid(beam[0].bx, beam[0].by)) strcpy(info, "The rocket explodes!");
else strcpy(info, "You hear a powerful explosion.");
mpr(info);
strcpy(beam[0].beam_name, "explosion");
beam[0].type = 43;
beam[0].damage = 112;
beam[0].flavour = 0;                        // doesn't seem right [dlb]
beam[0].colour = LIGHTGRAY;
}

if (stricmp(beam[0].beam_name, "fragmentation rocket") == 0)
{
if (see_grid(beam[0].bx, beam[0].by)) strcpy(info, "The rocket explodes!");
else strcpy(info, "You hear a fragmentary explosion."); // this is not proper usage, BTW
mpr(info);
strcpy(beam[0].beam_name, "blast of shrapnel");
beam[0].type = 43;
beam[0].damage = 110;
beam[0].flavour = 0;                        // doesn't seem right [dlb]
beam[0].colour = CYAN;
exsize = 1;
}

if (stricmp(beam[0].beam_name, "incendiary rocket") == 0)
{
if (see_grid(beam[0].bx, beam[0].by)) strcpy(info, "The rocket explodes!");
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
if (see_grid(beam[0].bx, beam[0].by)) strcpy(info, "The blast explodes!");
else strcpy(info, "You hear an explosion.");
mpr(info);
//   strcpy(info, "The blast explodes!");
//   mpr(info);
beam[0].type = 43;
beam[0].flavour = 0;                        // doesn't seem right [dlb]
}
*/


    explosion(exsize, beam);

}




/*
Makes a bang. ex_size determines whether the explosion is 3x3 or 5x5 without
corners.
*/
void explosion(char ex_size, struct bolt beam[1])
{

    char clouty = 0;
    char empty[5][5];
    int n = 0;
    int q = 0;
    int hurted = 0;


    for (n = 0; n < 5; n++)
      for (q = 0; q < 5; q++)
        empty[n][q] = 0;

    beam[0].source_x = beam[0].bx;
    beam[0].source_y = beam[0].by;

    for (n = 0; n < 5; n++)     // x
    {
        for (q = 0; q < 5; q++) // y
        {
            empty[n][q] = 1;
            if (grd[beam[0].source_x - 2 + n][beam[0].source_y - 2 + q] < 10)
                empty[n][q] = 0;
        }
    }

    if (ex_size == 0)
      {
        for (n = 0; n < 5; n++)
        {
            empty[n][0] = 0;
            empty[n][4] = 0;
            empty[0][n] = 0;
            empty[4][n] = 0;
        }
      }

    if (empty[1][2] == 0)
      empty[0][2] = 0;
    if (empty[3][2] == 0)
      empty[3][2] = 0;

    if (empty[2][3] == 0)
      empty[4][3] = 0;
    if (empty[2][1] == 0)
      empty[4][1] = 0;

    if (empty[1][1] == 0)
      empty[0][0] = 0;
    if (empty[3][3] == 0)
      empty[4][4] = 0;

    if (empty[1][3] == 0)
      empty[0][4] = 0;
    if (empty[3][1] == 0)
      empty[4][0] = 0;

    if (empty[1][3] == 0)
      empty[0][3] = 0;
    if (empty[1][3] == 0)
      empty[1][4] = 0;

    if (empty[1][1] == 0)
      empty[1][0] = 0;
    if (empty[1][1] == 0)
      empty[0][1] = 0;

    if (empty[3][1] == 0)
      empty[3][0] = 0;
    if (empty[3][1] == 0)
      empty[4][1] = 0;

    if (empty[3][3] == 0)
      empty[4][3] = 0;
    if (empty[3][3] == 0)
      empty[3][4] = 0;

    empty[0][0] = 0;
    empty[4][0] = 0;
    empty[0][4] = 0;
    empty[4][4] = 0;

    //brek = 1;
    //viewwindow(); // this resets the window so that eg dragons don't appear to do weird things.
    //brek = 0;


    for (n = -2; n < 3; n++)    // x
    {
        for (q = -2; q < 3; q++)        // y
        {

            beam[0].bx = beam[0].source_x + n;
            beam[0].by = beam[0].source_y + q;

            if (empty[n + 2][q + 2] == 0)
              continue;

            if (grd[beam[0].bx][beam[0].by] < 10)
              continue;

            //if (beam[0].bx > you.x_pos - 9 && beam[0].bx < you.x_pos + 9 && beam[0].by > you.y_pos - 9 && beam[0].by < you.y_pos + 9 && (show [beam[0].bx - you.x_pos + 9] [beam[0].by - you.y_pos + 9] != 0 || (beam[0].bx == you.x_pos && beam[0].by == you.y_pos)))// && beam[0].type != 0)
            if ( see_grid(beam[0].bx, beam[0].by)
                  || ( beam[0].bx == you.x_pos && beam[0].by == you.y_pos ) )
            {
                //viewwindow;
                //window(2,2,34,17);
                textcolor(beam[0].colour);
                //show [beam[0].bx - you.x_pos + 7] [beam[0].by - you.y_pos + 7] = 35;
                gotoxy(beam[0].bx - you.x_pos + 18, beam[0].by - you.y_pos + 9);
                putch(35);
            }
        }
    }

    //delay(500);
    // test relay_message();

    //scrloc = 0;
    more();

    int objs_vulnerable = -1;
    switch (beam[0].flavour)
    {
      case BEAM_FIRE:
        objs_vulnerable = OBJ_SCROLLS;
        break;

      case BEAM_COLD:
        objs_vulnerable = OBJ_POTIONS;
        break;

      case BEAM_SPORE:
        objs_vulnerable = OBJ_FOOD;
        break;
    }

    if (stricmp(beam[0].beam_name, "hellfire") == 0)
      objs_vulnerable = OBJ_SCROLLS;

    for (n = -2; n < 3; n++)    // x
    {
        for (q = -2; q < 3; q++)        // y
        {

            beam[0].bx = beam[0].source_x + n;
            beam[0].by = beam[0].source_y + q;

            if (empty[n + 2][q + 2] == 0)
                continue;

            if (grd[beam[0].bx][beam[0].by] < 10)
                continue;

            if (igrd[beam[0].bx][beam[0].by] != ING)
            {
                if (objs_vulnerable != -1 &&
                            mitm.base_type[igrd[beam[0].bx][beam[0].by]]
                                                        == objs_vulnerable)
                {
                    destroy_item(igrd[beam[0].bx][beam[0].by]);
                    if ( see_grid(beam[0].bx, beam[0].by) )
                    {
                        switch (beam[0].flavour)
                        {
                        case BEAM_FIRE:
                            mpr("You see a puff of smoke.");
                            break;
                        case BEAM_COLD:
                            mpr("You hear glass shatter.");
                            break;
                        }
                    }
                }
            }

            if (beam[0].bx == you.x_pos && beam[0].by == you.y_pos)
            // && beam[0].colour != 6) // have to do something about enchantments here.
            {                   // ^^^ digging

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
                }
                else
                    hurted += random2(beam[0].damage);

                hurted = check_your_resists(hurted, beam[0].flavour);
                hurted -= random2(player_AC() + 1);

                if (beam[0].flavour == BEAM_FRAG)
                {
                    hurted -= random2(player_AC() + 1);
                    hurted -= random2(player_AC() + 1);
                }

                if (you.equip[EQ_BODY_ARMOUR] != -1)
                    if (random2(1000) <= mass(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]]) && one_chance_in(4) )
                        exercise(SK_ARMOUR, 1);

                if (hurted <= 0)
                    hurted = 0;

                strcat(info, "!");

                if ( beam[0].flavour == BEAM_FIRE
                      || stricmp(beam[0].beam_name, "hellfire") == 0 )
                  scrolls_burn(5, OBJ_SCROLLS);

                if ( beam[0].flavour == BEAM_COLD )
                  scrolls_burn(5, OBJ_POTIONS);

                if ( beam[0].flavour == BEAM_SPORE )
                  scrolls_burn(2, OBJ_FOOD);

                if (beam[0].thing_thrown == 1 || beam[0].thing_thrown == 3)
                    ouch(hurted, 0, KILLED_BY_TARGETTING);
                else if (beam[0].flavour == BEAM_SPORE)  // cdl
                     ouch(hurted, 0, KILLED_BY_SPORE);
                else
                    ouch(hurted, beam[0].beam_source, KILLED_BY_BEAM);

                you.redraw_hit_points = 1;
                    //n++; // reduces beam's range
            }


            if (env.cgrid[beam[0].bx][beam[0].by] != CNG)       // hit a cloud
            {

                clouty = env.cgrid[beam[0].bx][beam[0].by];

                if ((env.cloud_type[clouty] == CLOUD_COLD && beam[0].flavour == BEAM_FIRE) || (env.cloud_type[clouty] == CLOUD_FIRE && beam[0].flavour == BEAM_COLD))
                {
                    if ( see_grid(beam[0].bx, beam[0].by) )
                    {
                        strcpy(info, "You hear a sizzling sound!");
                        mpr(info);
                    }

                    env.cloud_type[clouty] = CLOUD_NONE;
                    env.cgrid[env.cloud_x[clouty]][env.cloud_y[clouty]] = CNG;
                    env.cloud_decay[clouty] = 0;
                    env.cloud_no--;

                }
            }
            else if (stricmp(beam[0].beam_name, "ice storm") == 0)
            {
                place_cloud(CLOUD_COLD, beam[0].bx, beam[0].by, 2 + random2(3) + random2(3));
            }

            int o = 0;

            if (mgrd[beam[0].bx][beam[0].by] != MNG)
            {
                if (beam[0].beam_name[0] != 48)
                {
                    o = mgrd[beam[0].bx][beam[0].by];
                    if (menv[o].type >= MONS_LAVA_WORM && menv[o].number == 1)
                        continue;

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
                    if (beam[0].flavour == BEAM_FRAG)

                    {
                        hurted -= random2(menv[o].armor_class + 1);
                        hurted -= random2(menv[o].armor_class + 1);
                    }
                    if (hurted <= 0)
                        hurted = 0;

                    strcpy(info, "The ");
                    strcat(info, beam[0].beam_name);
                    strcat(info, " engulfs ");
                    strcat(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 1));       //gmon_name [mons_class [o]]);

                    strcat(info, ".");
                    mpr(info);

                    hurted = check_mons_resists(beam, o, hurted);

                    menv[o].hit_points -= hurted;

                    if (menv[o].behavior == BEH_ENSLAVED)
                    {
                        if ((beam[0].thing_thrown == 3 || beam[0].thing_thrown == 1) && hurted > 0)
                            menv[o].behavior = BEH_CHASING_I;
                    }           // else menv [o].behavior = 1;

                    if (menv[o].hit_points <= 0)
                    {
                        switch (beam[0].thing_thrown)
                        {
                        case 1:
                            monster_die(o, KILL_YOU_MISSILE, beam[0].beam_source);
                            break;      // your explosion

                        case 3:
                            monster_die(o, KILL_YOU_MISSILE, beam[0].beam_source);
                            break;      //  "    "

                        case 2:
                            monster_die(o, KILL_MON_MISSILE, beam[0].beam_source);
                            break;      // someone else's

                        case 4:
                            monster_die(o, KILL_MON_MISSILE, beam[0].beam_source);
                            break;      // someone else's

                        }
                    }
                    else if ((beam[0].thing_thrown == 1 || beam[0].thing_thrown == 3) && mons_near(o))
                    {
                        print_wounds(o);
                        if ( menv[o].type >= MONS_GOLD_MIMIC && menv[o].type <= MONS_POTION_MIMIC )
                          mimic_alert(o);
                    }
                }
            }
        }                       // end of for q
    }                           // end of for n
}                               // end of void explosion

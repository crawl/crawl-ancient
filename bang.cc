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

#include <stdlib.h>
#include <string.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "beam.h"
#include "fight.h"
#include "itemname.h"
#include "misc.h"
#include "monstuff.h"
#include "mon-util.h"
#include "ouch.h"
#include "player.h"
#include "skills.h"
#include "stuff.h"
#include "view.h"




/*
   Takes a bolt struct and refines it for use in the explosion function. Called
   from missile() and beam() in beam.cc. Explosions which do not follow from
   beams (eg scrolls of immolation) bypass this function.
 */
void explosion1( struct bolt *pbolt )
{

    bool is5x5 = false;

    if ( stricmp(pbolt->beam_name, "hellfire") == 0 )
    {
        if ( see_grid(pbolt->bx, pbolt->by)
              || ( you.x_pos == pbolt->bx && you.y_pos == pbolt->by ) )
          mpr("The hellfire explodes!");
        else if ( !silenced(pbolt->bx, pbolt->by) )
          mpr("You hear a strangely unpleasant explosion.");

        pbolt->type = SYM_BURST;
        pbolt->damage = 115;
        pbolt->flavour = BEAM_HELLFIRE;        // hellfire - differ fr. DMNBM_HELLFIRE? [dlb]
    }

    if ( stricmp(pbolt->beam_name, "golden flame") == 0 )
    {
        if ( see_grid(pbolt->bx, pbolt->by)
              || ( you.x_pos == pbolt->bx && you.y_pos == pbolt->by ) )
          mpr("The flame explodes!");
        else if ( !silenced(pbolt->bx, pbolt->by) )
          mpr("You hear a strange explosion.");

        pbolt->type = SYM_BURST;
        pbolt->flavour = BEAM_HOLY;    // same as golden flame? [dlb]
    }

    if ( stricmp(pbolt->beam_name, "fireball") == 0 )
    {
        if ( see_grid(pbolt->bx, pbolt->by)
              || ( you.x_pos == pbolt->bx && you.y_pos == pbolt->by ) )
          mpr("The fireball explodes!");
        else if ( !silenced(pbolt->bx, pbolt->by) )
          mpr("You hear an explosion.");

        pbolt->type = SYM_BURST;
        pbolt->damage += 100;  // it should already be set for this.
        pbolt->flavour = BEAM_FIRE;
    }

    if ( stricmp(pbolt->beam_name, "orb of electricity") == 0 )
    {
        if ( see_grid(pbolt->bx, pbolt->by)
              || ( you.x_pos == pbolt->bx && you.y_pos == pbolt->by ) )
          mpr("The orb of electricity explodes!");
        else if ( !silenced(pbolt->bx, pbolt->by) )
          mpr("You hear a clap of thunder!");

        pbolt->type = SYM_BURST;
        pbolt->damage += 100;  // it should already be set for this.
        pbolt->flavour = BEAM_ELECTRICITY;
        pbolt->colour = LIGHTCYAN;
        is5x5 = true;
    }

    if ( stricmp(pbolt->beam_name, "blast") == 0 )
    {
        if ( see_grid(pbolt->bx, pbolt->by)
              || ( you.x_pos == pbolt->bx && you.y_pos == pbolt->by ) )
          mpr("The orb of energy explodes.");
        else if ( !silenced(pbolt->bx, pbolt->by) )
          mpr("You hear an explosion.");

        is5x5 = false;
    }

    if ( stricmp(pbolt->beam_name, "metal orb") == 0 )
    {
        if ( see_grid(pbolt->bx, pbolt->by)
              || ( you.x_pos == pbolt->bx && you.y_pos == pbolt->by ) )
          mpr("The orb explodes into a blast of deadly shrapnel!");
        else if ( !silenced(pbolt->bx, pbolt->by) )
          mpr("You hear an explosion!");

        strcpy(pbolt->beam_name, "blast of shrapnel");
        pbolt->type = SYM_ZAP;
        pbolt->damage += 100;          // it should already be set for this.
        pbolt->flavour = BEAM_FRAG;    // sets it from pure damage to shrapnel (which is absorbed extra by armour)
        is5x5 = false;
    }

    if ( stricmp(pbolt->beam_name, "great blast of cold") == 0 )
    {
        if ( see_grid(pbolt->bx, pbolt->by)
              || ( you.x_pos == pbolt->bx && you.y_pos == pbolt->by ) )
          mpr("The blast explodes into a great storm of ice!");
        else if ( !silenced(pbolt->bx, pbolt->by) )
          mpr("You hear a raging storm!");

        strcpy(pbolt->beam_name, "ice storm");
        pbolt->type = SYM_ZAP;
        pbolt->damage += 100;  // it should already be set for this.
        pbolt->colour = WHITE;
        is5x5 = true;
    }


/* ************************************************************************

   if (stricmp(pbolt->beam_name, "concussion rocket") == 0)
   {
   if (see_grid(pbolt->bx, pbolt->by)) mpr("The rocket explodes!");
   else if ( !silenced(pbolt->bx, pbolt->by) ) mpr("You hear a powerful explosion.");
   strcpy(pbolt->beam_name, "explosion");
   pbolt->type = SYM_BURST;
   pbolt->damage = 112;
   pbolt->flavour = 0;                        // doesn't seem right [dlb]
   pbolt->colour = LIGHTGRAY;
   }

   if (stricmp(pbolt->beam_name, "fragmentation rocket") == 0)
   {
   if (see_grid(pbolt->bx, pbolt->by)) mpr("The rocket explodes!");
   else if ( !silenced(pbolt->bx, pbolt->by) ) mpr("You hear a fragmentary explosion."); // this is not proper usage, BTW
   strcpy(pbolt->beam_name, "blast of shrapnel");
   pbolt->type = SYM_BURST;
   pbolt->damage = 110;
   pbolt->flavour = 0;                        // doesn't seem right [dlb]
   pbolt->colour = CYAN;
   is5x5 = true;
   }

   if (stricmp(pbolt->beam_name, "incendiary rocket") == 0)
   {
   if (see_grid(pbolt->bx, pbolt->by)) mpr("The rocket explodes!");
   else if ( !silenced(pbolt->bx, pbolt->by) ) mpr("You hear an explosion.");
   strcpy(pbolt->beam_name, "fiery explosion");
   pbolt->type = SYM_BURST;
   pbolt->damage = 114;
   pbolt->flavour = 2;
   pbolt->colour = RED;
   }

   if (stricmp(pbolt->beam_name, "blast") == 0)
   {
   if (see_grid(pbolt->bx, pbolt->by)) mpr("The blast explodes!");
   else if ( !silenced(pbolt->bx, pbolt->by) ) mpr("You hear an explosion.");
   pbolt->type = SYM_BURST;
   pbolt->flavour = 0;                        // doesn't seem right [dlb]
   }

************************************************************************ */

    explosion(is5x5, pbolt);

}          // end explosion1()




/*
 * Makes a bang!!!
 * is5x5 determines whether the explosion is 3x3 or 5x5 without corners.
 */
void explosion( bool is5x5, struct bolt *pbolt )
{

    char clouty = 0;
    unsigned char empty[5][5];
    char n = 0, q = 0;             // loop variables {dlb}
    int hurted = 0;

    pbolt->source_x = pbolt->bx;
    pbolt->source_y = pbolt->by;

    for (n = 0; n < 5; n++)     // x
      for (q = 0; q < 5; q++)   // y
      {
          empty[n][q] = 1;

          if ( grd[pbolt->source_x - 2 + n][pbolt->source_y - 2 + q] < DNGN_LAST_SOLID_TILE )
            empty[n][q] = 0;
      }

    if ( !is5x5 )
      for (n = 0; n < 5; n++)
      {
          empty[n][0] = 0;
          empty[n][4] = 0;
          empty[0][n] = 0;
          empty[4][n] = 0;
      }


    if ( empty[1][2] == 0 )
      empty[0][2] = 0;

    if ( empty[2][3] == 0 || empty[3][3] == 0 )
      empty[4][3] = 0;

    if ( empty[2][1] == 0 )
      empty[4][1] = 0;

    if ( empty[1][3] == 0 )
    {
      empty[0][3] = 0;
      empty[1][4] = 0;
    }

    if ( empty[1][1] == 0 )
    {
      empty[1][0] = 0;
      empty[0][1] = 0;
    }

    if ( empty[3][1] == 0 )
    {
      empty[3][0] = 0;
      empty[4][1] = 0;
    }

    if ( empty[3][3] == 0 )
      empty[3][4] = 0;

    empty[0][0] = 0;
    empty[4][0] = 0;
    empty[0][4] = 0;
    empty[4][4] = 0;


    //brek = 1;
    //viewwindow(); // this resets the window so that eg dragons don't appear to do weird things.
    //brek = 0;


    for (n = -2; n < 3; n++)      // x
      for (q = -2; q < 3; q++)    // y
      {
          pbolt->bx = pbolt->source_x + n;
          pbolt->by = pbolt->source_y + q;

          if ( empty[n + 2][q + 2] == 0 || grd[pbolt->bx][pbolt->by] < DNGN_LAST_SOLID_TILE )
            continue;

          if ( see_grid(pbolt->bx, pbolt->by)
                || ( pbolt->bx == you.x_pos && pbolt->by == you.y_pos ) )
          {
              //viewwindow;
              //window(2,2,34,17);
              textcolor(pbolt->colour);
              //show [pbolt->bx - you.x_pos + 7] [pbolt->by - you.y_pos + 7] = 35;
              gotoxy(pbolt->bx - you.x_pos + 18, pbolt->by - you.y_pos + 9);
              putch('#');
          }
      }

    //delay(500);
    //scrloc = 0;

    more();

    char objs_vulnerable = -1;

    switch ( pbolt->flavour )
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

    if ( stricmp(pbolt->beam_name, "hellfire") == 0 )
      objs_vulnerable = OBJ_SCROLLS;

    for (n = -2; n < 3; n++)      // x
      for (q = -2; q < 3; q++)    // y
      {
          pbolt->bx = pbolt->source_x + n;
          pbolt->by = pbolt->source_y + q;

          if ( empty[n + 2][q + 2] == 0 || grd[pbolt->bx][pbolt->by] < DNGN_LAST_SOLID_TILE )
            continue;

          if ( igrd[pbolt->bx][pbolt->by] != ING )
          {
              if ( objs_vulnerable != -1
                  && mitm.base_type[igrd[pbolt->bx][pbolt->by]] == objs_vulnerable )
              {

                  destroy_item(igrd[pbolt->bx][pbolt->by]);

                  if ( see_grid(pbolt->bx, pbolt->by) )
                  {
                      switch ( pbolt->flavour )
                      {
                        case BEAM_FIRE:
                          mpr("You see a puff of smoke.");
                          break;
                        case BEAM_COLD:
                          if ( !silenced(pbolt->bx, pbolt->by) )    // see_grid() check seems contrary to this {dlb}
                            mpr("You hear glass shatter.");
                          break;
                      }
                  }
              }
          }

      // engulfs player: {dlb}
          if ( pbolt->bx == you.x_pos && pbolt->by == you.y_pos )
              // && pbolt->colour != 6) // have to do something about enchantments here.
          {                   // ^^^ digging

              if ( pbolt->colour == 200 )
              {
                  pbolt->tracer = 1;
                  return;
              }

              strcpy(info, "The ");
              strcat(info, pbolt->beam_name);
              strcat(info, " engulfs you!");
              mpr(info);

              hurted = 0;

              if ( pbolt->damage > 100 )
                hurted += random2(pbolt->damage - 100) + random2(pbolt->damage - 100) + random2(pbolt->damage - 100);
              else
                hurted += random2(pbolt->damage);

              hurted = check_your_resists(hurted, pbolt->flavour);

              hurted -= random2(1 + player_AC());

              if ( pbolt->flavour == BEAM_FRAG )
                hurted -= random2(1 + player_AC()) + random2(1 + player_AC());

              if ( you.equip[EQ_BODY_ARMOUR] != -1 )
                if ( !player_light_armour() && one_chance_in(4)
                    && random2(1000) <= mass(OBJ_ARMOUR, you.inv_type[you.equip[EQ_BODY_ARMOUR]]) )
                  exercise(SK_ARMOUR, 1);

              if ( hurted < 1 )
                hurted = 0;

              if ( pbolt->flavour == BEAM_FIRE
                    || stricmp(pbolt->beam_name, "hellfire") == 0 )
                scrolls_burn(5, OBJ_SCROLLS);
              else if ( pbolt->flavour == BEAM_COLD )
                scrolls_burn(5, OBJ_POTIONS);
              else if ( pbolt->flavour == BEAM_SPORE )
                scrolls_burn(2, OBJ_FOOD);

              if ( pbolt->thing_thrown == KILL_YOU
                    || pbolt->thing_thrown == KILL_YOU_MISSILE )
                ouch(hurted, 0, KILLED_BY_TARGETTING);
              else if ( pbolt->flavour == BEAM_SPORE )         // cdl
                ouch(hurted, 0, KILLED_BY_SPORE);
              else
                ouch(hurted, pbolt->beam_source, KILLED_BY_BEAM);

              //n++; // reduces beam's range
          }


      // cloud hits cloud (???): {dlb}
          if ( env.cgrid[pbolt->bx][pbolt->by] != CNG )
          {
              clouty = env.cgrid[pbolt->bx][pbolt->by];

              if ( ( ( env.cloud_type[clouty] == CLOUD_COLD
                        || env.cloud_type[clouty] == CLOUD_COLD_MON )
                     && pbolt->flavour == BEAM_FIRE )
                  || ( ( env.cloud_type[clouty] == CLOUD_FIRE
                        || env.cloud_type[clouty] == CLOUD_FIRE_MON )
                     && pbolt->flavour == BEAM_COLD ) )
              {
                  if ( see_grid(pbolt->bx, pbolt->by) && !silenced(pbolt->bx, pbolt->by) )    // what is the range of hearing? {dlb}
                    mpr("You hear a sizzling sound!");

                  env.cloud_type[clouty] = CLOUD_NONE;
                  env.cgrid[env.cloud_x[clouty]][env.cloud_y[clouty]] = CNG;
                  env.cloud_decay[clouty] = 0;
                  env.cloud_no--;
              }
          }
          else if (stricmp(pbolt->beam_name, "ice storm") == 0)
          {
              place_cloud(CLOUD_COLD, pbolt->bx, pbolt->by, 2 + random2avg(5,2));
          }

      // hey - don't lose sight of these declarations (!!!) {dlb}
          int o = 0;
          struct monsters *monster = 0;    // NULL {dlb}
      // hey - don't lose sight of these declarations (!!!) {dlb}

          if ( mgrd[pbolt->bx][pbolt->by] != MNG )
          {
              if ( pbolt->beam_name[0] != '0' )
              {
                  o = mgrd[pbolt->bx][pbolt->by];
                  monster = &menv[o];

                  if ( monster_habitat(monster->type) != DNGN_FLOOR && monster->number == 1 )
                    continue;

                  hurted = 0;

                  if ( pbolt->damage > 100 )
                    hurted += random2(pbolt->damage - 100) + random2(pbolt->damage - 100) + random2(pbolt->damage - 100);
                  else
                    hurted += random2(pbolt->damage);

                  hurted -= random2(1 + monster->armor_class);

                  if ( pbolt->flavour == BEAM_FRAG )
                    hurted -= random2(1 + monster->armor_class) + random2(1 + monster->armor_class);

                  if ( hurted < 1 )
                    hurted = 0;

                  strcpy(info, "The ");
                  strcat(info, pbolt->beam_name);
                  strcat(info, " engulfs ");
                  strcat(info, monam(monster->number, monster->type, monster->enchantment[2], 1));
                  strcat(info, ".");
                  mpr(info);

                  hurted = check_mons_resists(monster, pbolt, hurted);

                  hurt_monster(monster, hurted);

                  if ( monster->behavior == BEH_ENSLAVED )
                    if ( ( pbolt->thing_thrown == KILL_YOU_MISSILE
                            || pbolt->thing_thrown == KILL_YOU ) && hurted > 0 )
                      monster->behavior = BEH_CHASING_I;

                  if ( monster->hit_points < 1 )
                  {
                      switch ( pbolt->thing_thrown )
                      {
                        case KILL_YOU:
                        case KILL_YOU_MISSILE:
                          monster_die(monster, KILL_YOU_MISSILE, pbolt->beam_source);
                          break;      // your explosion

                        case KILL_MON:
                        case KILL_MON_MISSILE:
                          monster_die(monster, KILL_MON_MISSILE, pbolt->beam_source);
                          break;      // someone else's
                      }
                  }
                  else if ( ( pbolt->thing_thrown == KILL_YOU || pbolt->thing_thrown == KILL_YOU_MISSILE ) && mons_near(monster) )
                  {
                      print_wounds(monster);

                      if ( mons_category(monster->type) == MC_MIMIC )
                        mimic_alert(monster);
                  }
              }
          }
      }                       // end "for q,n"

}          // end explosion()

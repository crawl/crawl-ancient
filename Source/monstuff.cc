/*
 *  File:       monstuff.cc
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <5>     31 July 2000   GDL     More Manticore fixes.
 *      <4>     29 July 2000   JDJ     Fixed a bunch of places in mons_pickup where MSLOT_WEAPON
 *                                     was being erroneously used.
 *      <3>     25 July 2000   GDL     Fixed Manticores
 *      <2>     11/23/99       LRH     Upgraded monster AI
 *      <1>     -/--/--        LRH     Created
 */

#include "AppHdr.h"
#include "monstuff.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "beam.h"
#include "debug.h"
#include "fight.h"
#include "itemname.h"
#include "misc.h"
#include "monplace.h"
#include "mon-util.h"
#include "mstuff2.h"
#include "player.h"
#include "spells2.h"
#include "stuff.h"
#include "view.h"


static bool handle_special_ability(struct monsters *monster, bolt & beem);
static bool mons_pickup(struct monsters *monster);
static bool mons_speaks(struct monsters *monster);
static void flag_ench(struct monsters *monster, int p);
static void handle_behavior(struct monsters *monster);
static void mons_in_cloud(struct monsters *monster);
static void monster_move(struct monsters *monster);
static void plant_spit(struct monsters *monster, struct bolt *pbolt);


char mmov_x, mmov_y;




bool curse_an_item(char which, char power)
{
/* use which later, if I want to curse weapon/gloves whatever
   which, for now: 0 = non-mummy, 1 = mummy (potions as well)
   don't change mitm.special of !odecay */

/* I think this is wrong??? */

    FixedVector<char, ENDOFPACK> possib;
    char cu = power;

    cu = 0;
    char cu1 = 0;
    char cu2 = 0;
    char cu3 = 0;


    for (cu = 0; cu < ENDOFPACK; cu++)
      possib[cu] = 100;

    for (cu = 0; cu < ENDOFPACK; cu++)
    {
        if ( you.inv_quantity[cu] == 0 )
          continue;

        cu1 = you.inv_class[cu];

/*  if (cu1 == OBJ_WEAPONS || cu1 == OBJ_ARMOUR)
   {
   if (you.inv_dam [cu] % 30 >= 25) continue; //no randarts
   } */

        if (cu1 == OBJ_JEWELLERY)
        {
            if (you.inv_dam[cu] == 200)
              continue;       // no randarts
        }

        if (cu1 == OBJ_WEAPONS || cu1 == OBJ_ARMOUR || cu1 == OBJ_JEWELLERY)
        {
            if (you.inv_plus[cu] >= 130)
                continue;
            possib[cu2] = cu;
            cu2++;
        }

        if (cu1 == OBJ_POTIONS && which == 1)
        {
            if (you.inv_type[cu] == POT_DECAY)
                continue;
            possib[cu2] = cu;
            cu2++;
        }

    }

    if (cu2 == 0)
        return 0;

    do
    {
        cu3 = random2(cu2);
    }
    while (possib[cu3] == 100);


    if (you.inv_class[possib[cu3]] == OBJ_POTIONS)
    {

        you.inv_type[possib[cu3]] = POT_DECAY;  /* don't change you.inv_dam (just for fun) */
        return 1;
        /* problem: changes large piles of potions */
    }

    you.inv_plus[possib[cu3]] += 100;

    return 1;

}




// who wrote this? for shame! {dlb} --
// I can't be bothered writing an intelligent function,
// so I'll make it ugly:
void monster_blink( struct monsters *monster )
{

    FixedVector<int, 2> passed;    // for random_near_space f(x) {dlb}

    if ( !random_near_space(passed) )
      return;

    mgrd[monster->x][monster->y] = NON_MONSTER;

    monster->x = passed[0];
    monster->y = passed[1];

    mgrd[monster->x][monster->y] = monster_index(monster);

}          // end monster_blink()




bool random_near_space( FixedVector<int, 2>& passed )
{

    passed[0] = 0;
    passed[1] = 0;

    int blx = passed[0];
    int bly = passed[1];
    int tries = 0;

    do
    {
        blx = you.x_pos - 6 + random2(14);
        bly = you.y_pos - 6 + random2(14);

        tries++;

        if ( tries > 149 )
          return false;
    }
    while ( !see_grid(blx, bly)
             || grd[blx][bly] < DNGN_SHALLOW_WATER
             || mgrd[blx][bly] != NON_MONSTER
             || (you.x_pos == blx && you.y_pos == bly) );

    passed[0] = blx;
    passed[1] = bly;

    return true;

}          // end random_near_space()




void swap_places( struct monsters *monster )
{

    mpr("You swap places.");

    mgrd[monster->x][monster->y] = NON_MONSTER;

    monster->x = you.x_pos;
    monster->y = you.y_pos;

    mgrd[monster->x][monster->y] = monster_index(monster);

}          // end swap_places()



void print_wounds( struct monsters *monster )
{

    char str_wound[200];                       // prevents segfault -- cannot use info[] here {dlb}

    if ( monster->type == -1 )
      return;

    if ( monster->hit_points == monster->max_hit_points || monster->hit_points < 1 )
      return;

    if ( monster_descriptor(monster->type, MDSC_NOMSG_WOUNDS) )
      return;

    strcpy(str_wound, " is ");

    if ( monster->hit_points <= monster->max_hit_points / 6 )
    {
        strcat(str_wound, "almost ");
        strcat(str_wound, wounded_damaged(monster->type) ? "destroyed" : "dead");
    }
    else
    {
        strcat(str_wound, (monster->hit_points <= monster->max_hit_points / 6)     ? "horribly " :
                          (monster->hit_points <= monster->max_hit_points / 3)     ? "heavily " :
                          (monster->hit_points <= 3*(monster->max_hit_points / 4)) ? "moderately "
                                                                                   : "lightly " );

        strcat(str_wound, wounded_damaged(monster->type) ? "damaged" : "wounded");
    }

    strcat(str_wound, ".");

    simple_monster_message(monster, str_wound);

}          // end print_wounds()




// (true == 'damaged') [constructs, undead, etc.]
// and (false == 'wounded') [living creatures, etc.] {dlb}
bool wounded_damaged( int wound_class )
{

// this schema needs to be abstracted into real categories {dlb}:
    if ( mons_holiness(wound_class) == MH_UNDEAD )
      return true;
    if ( wound_class >= MONS_CLAY_GOLEM && wound_class <= MONS_TOENAIL_GOLEM )
      return true;
    if ( wound_class == MONS_FUNGUS || wound_class == MONS_ABOMINATION_LARGE || wound_class == MONS_PLANT || wound_class == MONS_FIRE_VORTEX || wound_class == MONS_ABOMINATION_SMALL || wound_class == MONS_DANCING_WEAPON || wound_class == MONS_SPATIAL_VORTEX )
      return true;

    return false;

}          // end wounded_damaged()




//---------------------------------------------------------------
//
// handle_behavior
//
// Uses the monster's behavior flag to adjust the square the
// monster will move towards.
//
//---------------------------------------------------------------
static void handle_behavior( struct monsters *monster )
{

    bool monsterNearby = mons_near(monster);    // single calculation permissible {dlb}

    switch ( monster->behavior )
    {
      case BEH_SLEEP:
        monster->target_x = monster->x;
        monster->target_y = monster->y;
        break;

      case BEH_CHASING_I:
      case BEH_CHASING_II:
        if ( ( monsterNearby
              && ( !you.invis || mons_see_invis(monster->type)
                     || ( grd[you.x_pos][you.y_pos] == DNGN_SHALLOW_WATER && !you.levitation ) ) )
            || ( !monsterNearby
                && monster->target_x == monster->x && monster->target_y == monster->y ) )
        {
            monster->target_x = you.x_pos;
            monster->target_y = you.y_pos;
        }
        break;

  // this should be made more rigorous {dlb}
      case BEH_WANDER:
        if ((monster->target_x == monster->x && monster->target_y == monster->y) || one_chance_in(20))
        {
            monster->target_x = 10 + random2(GXM - 10);
            monster->target_y = 10 + random2(GYM - 10);
        }
        break;

    case BEH_FLEE:
        if ( monsterNearby && ( !you.invis || mons_see_invis(monster->type) ) )
        {
            monster->target_x = you.x_pos;
            monster->target_y = you.y_pos;
        }

        if ( monster->hit_points > (monster->max_hit_points / 2) )
          monster->behavior = BEH_CHASING_I;   // change the '= 1' to whatever monster used to be doing

        break;

    case BEH_FIGHT:             // attacking other monster
        if ( monster->monster_foe == MHITNOT )
        {
            monster->behavior = BEH_WANDER;      // ?
        }
        else
        {
            monster->target_x = menv[monster->monster_foe].x;
            monster->target_y = menv[monster->monster_foe].y;
        }
        break;

    case BEH_ENSLAVED: //To be nice (although not strictly accurate) they should follow you even when they can't see you
        if ( you.pet_target != MHITNOT )
        {
            if ( monster->monster_foe == MHITNOT )
              monster->monster_foe = you.pet_target;
        }

        if ( monster->monster_foe == monster_index(monster) )    // monsters do not attack themselves {dlb}
          monster->monster_foe = MHITNOT;
        else if ( ( monster->monster_foe != MHITNOT && monster->monster_foe != MHITYOU )
                   && menv[monster->monster_foe].behavior == BEH_ENSLAVED )    // enslaved monsters do not attack other enslaved monsters {dlb}
          monster->monster_foe = MHITNOT;

    // this needs to be fixed for MHITYOU or segfaults {dlb}
        if ( monster->monster_foe != MHITNOT
            && ( ( monsterNearby && mons_near(&menv[monster->monster_foe]) )
                    || distance(monster->x, menv[monster->monster_foe].x, monster->y, menv[monster->monster_foe].y) == 1 ) )
        {
            monster->target_x = menv[monster->monster_foe].x;
            monster->target_y = menv[monster->monster_foe].y;
        }
        else if ( monsterNearby && ( monster->target_x == monster->x && monster->target_y == monster->y )
                   || one_chance_in(3) )
        {

        // see BEH_WANDER case for my commentary {dlb}:
            if ( one_chance_in(7) )
            {
                monster->target_x = 10 + random2(GXM - 10);
                monster->target_y = 10 + random2(GYM - 10);

            }
            else
            {
                monster->target_x = you.x_pos;
                monster->target_y = you.y_pos;
            }
        }

        if ( !monsterNearby )
        {
            monster->target_x = you.x_pos;
            monster->target_y = you.y_pos;
        }
        break;
    }

}          // end handle_behavior()




//---------------------------------------------------------------
//
// flag_ench
//
// Handle the result of a spell cast upon a monster. Returns
// true if the monster died.
//
//---------------------------------------------------------------

static void flag_ench( struct monsters *monster, int p )
{

    monster->enchantment[p] = ENCH_NONE;

    if ( monster->enchantment[0] == ENCH_NONE
        && monster->enchantment[1] == ENCH_NONE
        && monster->enchantment[2] == ENCH_NONE )
      monster->enchantment1 = 0;

}          // end flag_ench()




// note that this function *completely* blocks messaging for monsters
// distant or invisible to the player ... look elsewhere for a function
// permitting output of "It" messages for the invisible {dlb}
// utilizes both str_pass[] and info[] so be careful calling it {dlb}:
bool simple_monster_message( struct monsters *monster, const char *event )
{

    if ( mons_near(monster) && ( monster->enchantment[2] != ENCH_INVIS || player_see_invis() ) )
    {
        if ( monster->type != MONS_DANCING_WEAPON )
          strcpy(info, monam(monster->number, monster->type, monster->enchantment[2], 0));
        else
        {
            it_name(monster->inv[MSLOT_WEAPON], 0, str_pass);
            strcpy(info, str_pass);
        }

        strcat(info, event);
        mpr(info);
        return true;
    }

    return false;

}          // end simple_monster_message()




static bool handle_enchantment( struct monsters *monster )
{

    bool died = false;

    if ( monster->enchantment1 )
    {
        for (int p = 0; p < 3 && !died; p++)
        {
            switch ( monster->enchantment[p] )
            {
              case ENCH_SLOW:
                if ( random2(250) <= 10 + monster->hit_dice )
                {
                    if (monster->speed >= 100)
                      monster->speed = 100 + ((monster->speed - 100) << 1);
                    else
                      monster->speed <<= 1;

                    flag_ench(monster, p);
                    simple_monster_message(monster, " seems to speed up.");
                }
                break;

              case ENCH_HASTE:
                if ( one_chance_in(20) )
                {
                    if (monster->speed >= 100)
                      monster->speed = ((monster->speed - 100) / 2) + 100;
                    else
                      monster->speed /= 2;

                    flag_ench(monster, p);
                    simple_monster_message(monster, " seems to slow down.");
                }
                break;

              case ENCH_FEAR:
                monster->behavior = BEH_FLEE;

                if ( random2(150) <= 10 + monster->hit_dice )
                {
                    monster->behavior = BEH_CHASING_I;   // reset to monster's original behaviour
                    simple_monster_message(monster, " seems to regain its courage.");
                    flag_ench(monster, p);
                }
                break;

              case ENCH_CONFUSION:
                monster->behavior = BEH_CONFUSED;
                if ( random2(120) < monster->hit_dice + 5 )
                {
                    if ( monster->type != MONS_BUTTERFLY
                        && monster->type != MONS_FIRE_VORTEX
                        && monster->type != MONS_SPATIAL_VORTEX
                        && monster->type != MONS_VAPOUR )
                      {
                        monster->behavior = BEH_CHASING_I;
                        simple_monster_message(monster, " seems less confused.");
                        flag_ench(monster, p);
                      }
                }
                break;

              case ENCH_INVIS:
                if ( ( monster_habitat(monster->type) != DNGN_FLOOR && monster->number == 0 )
                        || ( monster->type == MONS_AIR_ELEMENTAL && one_chance_in(3) )
                        || one_chance_in(20) )
                {
                    if ( !mons_flag(monster->type, M_INVIS) )    // invisible monsters stay invisible
                    {
                        if ( monster_habitat(monster->type) == DNGN_FLOOR || monster->number != 1)
                        {
                            flag_ench(monster, p);
                            // Can't use simple_message, because we want
                            //  'a thing appears' (not 'the')
                            // Note: this message not printed if the player
                            //  could already see the monster.
                            if ( mons_near(monster)
                                && ( !player_see_invis() || monster_habitat(monster->type) != DNGN_FLOOR ) )
                              {
                                strcpy(info, monam(monster->number, monster->type, monster->enchantment[2], 2));
                                strcat(info, " appears!");
                                mpr(info);
                              }
                        }
                    }
                }
                break;

              case ENCH_POISON_I:
              case ENCH_POISON_II:
              case ENCH_POISON_III:
              case ENCH_POISON_IV:
              case ENCH_YOUR_POISON_I:
              case ENCH_YOUR_POISON_II:
              case ENCH_YOUR_POISON_III:
              case ENCH_YOUR_POISON_IV:
                {
                    int poisonval = monster->enchantment[p] % 50;

                    if ( coinflip() )
                      hurt_monster(monster, 1 + random2(poisonval - 3));

                    if ( mons_res_poison(monster->type) < 0 )
                      hurt_monster(monster, 1 + random2(poisonval - 5) + random2(poisonval - 5));

                    if ( monster->hit_points < 1 )
                    {
                        monster_die(monster, ((monster->enchantment[p] < ENCH_POISON_I) ? KILL_YOU : KILL_MISC), 0);

                        died = true;
                    }

                    if ( one_chance_in(5) )
                    {
                        monster->enchantment[p]--;       //= 0;

                        if ( monster->enchantment[p] % 50 == 6 )
                          flag_ench(monster, p);
                    }
                }
                break;

              case ENCH_YOUR_ROT_I:
                if ( one_chance_in(4) )
                  flag_ench(monster,p);
                else if ( monster->hit_points > 1 && one_chance_in(3) )
                  hurt_monster(monster, 1);
                break;

          //jmf: FIXME: if (undead) make_small_rot_cloud();
              case ENCH_YOUR_ROT_II:
              case ENCH_YOUR_ROT_III:
              case ENCH_YOUR_ROT_IV:
                if ( monster->hit_points > 1 && one_chance_in(3) )
                  hurt_monster(monster, 1);
                if ( one_chance_in(4) )
                  monster->enchantment[p]--;
                break;

              case ENCH_BACKLIGHT_I:
                if ( one_chance_in(3) )
                  flag_ench(monster, p);
                break;

              case ENCH_BACKLIGHT_II:
              case ENCH_BACKLIGHT_III:
              case ENCH_BACKLIGHT_IV:
                if ( one_chance_in(3) )
                  monster->enchantment[p]--;
                break;

          // assumption: mons_res_fire has already been checked
              case ENCH_STICKY_FLAME_I:
              case ENCH_STICKY_FLAME_II:
              case ENCH_STICKY_FLAME_III:
              case ENCH_STICKY_FLAME_IV:
              case ENCH_YOUR_STICKY_FLAME_I:
              case ENCH_YOUR_STICKY_FLAME_II:
              case ENCH_YOUR_STICKY_FLAME_III:
              case ENCH_YOUR_STICKY_FLAME_IV:
                hurt_monster(monster, ((1 + random2avg(7,2)) * 10) / monster->speed);

                if ( mons_res_fire(monster->type) == -1 )
                  hurt_monster(monster, ((1 + random2avg(9,2)) * 10) / monster->speed);

                simple_monster_message(monster, " burns!");

                if ( monster->hit_points < 1 )
                {
                    monster_die(monster, ((monster->enchantment[p] < ENCH_STICKY_FLAME_I) ? KILL_YOU : KILL_MISC), 0);
                    died = true;
                }

                if (coinflip())
                {
                    monster->enchantment[p]--;   //= 0;

                    if ( monster->enchantment[p] == ENCH_STICKY_FLAME_I - 1 || monster->enchantment[p] == ENCH_YOUR_STICKY_FLAME_I - 1 )
                      flag_ench(monster, p);
                }
                break;

          // 19 is taken by summoning:
          // If these are changed, must also change abjuration
              case ENCH_ABJ_I:
              case ENCH_ABJ_II:
              case ENCH_ABJ_III:
              case ENCH_ABJ_IV:
              case ENCH_ABJ_V:
              case ENCH_ABJ_VI:
              case ENCH_FRIEND_ABJ_I:
              case ENCH_FRIEND_ABJ_II:
              case ENCH_FRIEND_ABJ_III:
              case ENCH_FRIEND_ABJ_IV:
              case ENCH_FRIEND_ABJ_V:
              case ENCH_FRIEND_ABJ_VI:
                if ( one_chance_in(10) )
                  monster->enchantment[p]--;

                if ( monster->enchantment[p] == ENCH_ABJ_I - 1 || monster->enchantment[p] == ENCH_FRIEND_ABJ_I - 1 )
                {
                    monster_die(monster, KILL_RESET, 0);
                    died = true;
                }
                break;

              case ENCH_CHARM:
                if ( monster->behavior != BEH_ENSLAVED || random2(500) <= 10 + monster->hit_dice )
                {
                    monster->behavior = BEH_CHASING_I;   // reset to monster's original behaviour
                    flag_ench(monster, p);
                    simple_monster_message(monster, " is no longer your friend.");
                }
                break;

              case ENCH_GLOWING_SHAPESHIFTER:     // this ench never runs out
                if ( monster->type == MONS_GLOWING_SHAPESHIFTER || one_chance_in(4) )
                  monster_polymorph(monster, RANDOM_MONSTER, 0);
                break;

              case ENCH_SHAPESHIFTER:     // this ench never runs out
                if ( monster->type == MONS_SHAPESHIFTER || one_chance_in(15) )
                  monster_polymorph(monster, RANDOM_MONSTER, 0);
                break;

              case ENCH_TP_I:
                monster_teleport(monster, true);
                flag_ench(monster, p);
                break;

              case ENCH_TP_II:
              case ENCH_TP_III:
              case ENCH_TP_IV:
                monster->enchantment[p]--;
                break;
            }
        }
    }

    return died;

}          // end handle_enchantment()




//---------------------------------------------------------------
//
// handle_movement
//
// Move the monster close to its target square.
//
//---------------------------------------------------------------
static void handle_movement( struct monsters *monster )
{

// move the monster:
    if ( monster->behavior == BEH_FLEE )
    {
        mmov_x = ( (monster->target_x > monster->x) ? -1 :
                   (monster->target_x < monster->x) ?  1
                                                    :  0 );

        mmov_y = ( (monster->target_y > monster->y) ? -1 :
                   (monster->target_y < monster->y) ?  1
                                                    :  0 );
    }
    else
    {
        mmov_x = ( (monster->target_x > monster->x) ?  1 :
                   (monster->target_x < monster->x) ? -1
                                                    :  0 );

        mmov_y = ( (monster->target_y > monster->y) ?  1 :
                   (monster->target_y < monster->y) ? -1
                                                    :  0 );
    }

    // see if we attacked another monster
    if ( mgrd[monster->x + mmov_x][monster->y + mmov_y] != NON_MONSTER && mgrd[monster->x + mmov_x][monster->y + mmov_y] != monster_index(monster) )
    {
        int mnt = mgrd[monster->x + mmov_x][monster->y + mmov_y];

        if ( monster->behavior == BEH_ENSLAVED && menv[mnt].behavior != BEH_ENSLAVED )
          monster->monster_foe = mnt;

        if ( monster->behavior != BEH_ENSLAVED && menv[mnt].behavior == BEH_ENSLAVED )
          monster->monster_foe = mnt;
    }

}          // end handle_movement()


//---------------------------------------------------------------
//
// handle_nearby_ability
//
// Gives monsters a chance to use a special ability when they're
// next to the player.
//
//---------------------------------------------------------------
static void handle_nearby_ability( struct monsters *monster )
{

    if ( mons_near(monster) && monster->behavior != BEH_SLEEP )
    {
        if ( mons_flag(monster->type, M_SPEAKS) && one_chance_in(21)
             && !silenced(monster->x, monster->y))
          mons_speaks(monster); // mv: removed silence check

        switch ( monster->type )
        {
            case MONS_SPATIAL_VORTEX:
            case MONS_KILLER_KLOWN:
                monster->number = random_colour(); // I think it is used for colouring {dlb}
                break;

            case MONS_GIANT_EYEBALL:
                if ( coinflip() )
                {
                    simple_monster_message(monster, " stares at you.");

                    if ( you.paralysis < 10 )
                      you.paralysis += 2 + random2(3);
                }
                break;

            case MONS_EYE_OF_DRAINING:
                if ( coinflip() )
                {
                    simple_monster_message(monster, " stares at you.");

                    dec_mp(2 + random2avg(13,3));

                    heal_monster(monster, 5, true);    // heh heh {dlb}
                }
                break;

            case MONS_LAVA_WORM:
            case MONS_LAVA_FISH:
            case MONS_LAVA_SNAKE:
            case MONS_ANOTHER_LAVA_THING:
            case MONS_BIG_FISH:
            case MONS_GIANT_GOLDFISH:
            case MONS_ELECTRICAL_EEL:
            case MONS_JELLYFISH:
            case MONS_WATER_ELEMENTAL:
            case MONS_SWAMP_WORM:
                if ( grd[monster->x][monster->y] == DNGN_SHALLOW_WATER || grd[monster->x][monster->y] == DNGN_BLUE_FOUNTAIN )
                {
                    monster->number = 0;
                    monster->enchantment[2] = ENCH_NONE;
                }
                else if ( monster->number == 0 && grd[monster->x][monster->y] != DNGN_SHALLOW_WATER && grd[monster->x][monster->y] != DNGN_BLUE_FOUNTAIN )
                {                   // visible
                    if (one_chance_in(5))
                    {
                        monster->number = 1;
                        monster->enchantment[2] = ENCH_INVIS;
                        monster->enchantment1 = 1;
                    }

                }
                else if ((monster->x != you.x_pos || monster->y != you.y_pos)
                         && one_chance_in(10))
                {
                    monster->number = 0;
                }
                break;

            case MONS_AIR_ELEMENTAL:
                if ( one_chance_in(5) )
                {
                    monster->enchantment[2] = ENCH_INVIS;
                    monster->enchantment1 = 1;
                }
                break;

            case MONS_PANDEMONIUM_DEMON:
                if ( ghost.values[13] )
                  monster->number = random_colour();
                break;
        }
    }

}          // end handle_nearby_ability()




//---------------------------------------------------------------
//
// handle_special_ability
//
// $$$ not sure what to say here...
//
//---------------------------------------------------------------
static bool handle_special_ability( struct monsters *monster, bolt & beem )
{
    bool used = false;

    FixedArray<unsigned int, 19, 19> show;

    losight(show, grd, you.x_pos, you.y_pos);

    switch ( monster->type )
    {
      case MONS_LAVA_SNAKE:
        if ( monster->behavior == BEH_CONFUSED )
          break;

        if ( you.invis && !mons_see_invis(monster->type) )
          break;

        if ( monster->number == 1 || monster->enchantment[2] == ENCH_INVIS || coinflip() )
          break;

        // viewwindow was here.
        if ( show[monster->x - you.x_pos + 6][monster->y - you.y_pos + 6] )
        {
            beem.move_x = 0;
            beem.move_y = 0;

            beem.move_x = beem.trac_targ_x - monster->x;
            beem.move_y = beem.trac_targ_y - monster->y;

            if ( beem.move_x || beem.move_y )
            {
                viewwindow(1, false);
                beem.target_x = beem.trac_targ_x;
                beem.target_y = beem.trac_targ_y;
                beem.aim_down = 1;
                beem.range = 5 + random2(10);
                beem.source_x = monster->x;
                beem.source_y = monster->y;
                strcpy(beem.beam_name, "glob of lava");
                beem.damage = 108;
                beem.colour = RED;
                beem.type = SYM_ZAP;
                beem.flavour = BEAM_LAVA;
                beem.hit = 20;
                beem.beam_source = monster_index(monster);
                beem.thing_thrown = 4;

                simple_monster_message(monster, " spits lava!");

                missile(&beem, 0);
            }
        }
        break;

    case MONS_ELECTRICAL_EEL:
        if ( monster->behavior == BEH_CONFUSED )
          break;

        if ( you.invis && !mons_see_invis(monster->type) )
          break;

        if ( monster->number == 1 || monster->enchantment[2] == ENCH_INVIS )
          break;

        if ( show[monster->x - you.x_pos + 6][monster->y - you.y_pos + 6] )
        {
            beem.move_x = 0;
            beem.move_y = 0;
            beem.move_x = beem.trac_targ_x - monster->x;
            beem.move_y = beem.trac_targ_y - monster->y;

            if ( beem.move_x || beem.move_y )
            {
                beem.target_x = beem.trac_targ_x;
                beem.target_y = beem.trac_targ_y;
                beem.aim_down = 1;
                viewwindow(1, false);
                beem.range = 5 + random2(10);
                beem.source_x = monster->x;
                beem.source_y = monster->y;
                strcpy(beem.beam_name, "bolt of electricity");
                beem.damage = 105;
                beem.colour = LIGHTCYAN;
                beem.type = SYM_ZAP;
                beem.flavour = BEAM_ELECTRICITY;
                beem.hit = 150;
                beem.beam_source = monster_index(monster);
                beem.thing_thrown = 2;

                simple_monster_message(monster, " shoots out a bolt of electricity!");

                beam(&beem);
            }
        }
        break;

    case MONS_OKLOB_PLANT:
        if ( monster->behavior == BEH_CONFUSED )
          break;

        if ( you.invis && !mons_see_invis(monster->type) )
          break;

        if ( one_chance_in(3) )
          plant_spit(monster, &beem);
        break;

    case MONS_PIT_FIEND:
        if (one_chance_in(3))
          break;
// deliberate fall through
    case MONS_FIEND:
        if ( monster->behavior == BEH_CONFUSED )
          break;

        if (one_chance_in(4))
        {
            int spell_cast;

            switch (random2(4))
            {
            case 0:
                spell_cast = MS_TORMENT;
                mons_cast(monster, &beem, spell_cast);        /* beem should be irrelevant here */
                break;
            case 1:
            case 2:
            case 3:
                tracer_f(monster, &beem);
                if ( monster->behavior == BEH_ENSLAVED
                    && ( beem.tracer == 1 || beem.tracer == 2 ) )
                  break;

                if ( beem.tracer != 0 )
                { // || (beem.trac_targ != MHITYOU && beem.trac_hit_mons != 0)
                    simple_monster_message(monster, " makes a gesture!");
                    spell_cast = MS_HELLFIRE;
                    mons_cast(monster, &beem, spell_cast);
                }
                break;
            }

            mmov_x = 0;
            mmov_y = 0;
        }
        break;

    case MONS_IMP:
    case MONS_PHANTOM:
    case MONS_INSUBSTANTIAL_WISP:
    case MONS_BLINK_FROG:
        if ( one_chance_in(7) )
        {
            simple_monster_message(monster, " blinks.");
            monster_blink(monster);
        }
        break;

    case MONS_MANTICORE:
        if ( you.invis && !mons_see_invis(monster->type) )
            break;

        if ( monster->behavior == BEH_CONFUSED )
            break;

        if (!mons_near(monster))
            break;

        // calculate deltaX and deltaY, quit if blocked.
        beem.move_x = beem.trac_targ_x - monster->x;
        beem.move_y = beem.trac_targ_y - monster->y;
        if (beem.move_x == 0 && beem.move_y == 0)
            break;

        // the fewer spikes the manticore has left,  the less
        // likely it will use them.
        if (random2(16) >= monster->number)
            break;

        // do the throwing right here,  since the beam is so
        // easy to set up and doesn't involve inventory.

        // message
        simple_monster_message(monster, " flicks its tail!");

        // set up the beam
        beem.range = 9;
        strcpy(beem.beam_name, "volley of spikes");
        beem.hit = 14;
        beem.damage = 10;
        beem.beam_source = monster_index(monster);
        beem.type = SYM_MISSILE;
        beem.source_x = monster->x;
        beem.source_y = monster->y;
        beem.colour = LIGHTGREY;
        beem.flavour = BEAM_MISSILE;
        beem.thing_thrown = KILL_MON;

        // call missile direct, with -1 to indicate 'fake' item.
        missile(&beem, -1);

        // decrement # of volleys left
        monster->number -= 1;
        used = true;
        break;

        // dragon breath weapon:
    case MONS_DRAGON:
    case MONS_HELL_HOUND:
    case MONS_ICE_DRAGON:
    case MONS_LINDWORM:
    case MONS_FIREDRAKE:
    case MONS_XTAHUA:
        if ( you.invis && !mons_see_invis(monster->type) )
          break;

        if ( monster->behavior == BEH_CONFUSED )
          break;

        if ( ( monster->type != MONS_HELL_HOUND && random2(13) < 3 ) || one_chance_in(10) )
        {
            if ( monster->behavior == BEH_FIGHT || monster->behavior == BEH_ENSLAVED )
            {
                beem.trac_targ = monster->monster_foe;
                tracer_f(monster, &beem);

                if ( beem.tracer == 0 )
                  break;

                if ( monster->behavior == BEH_ENSLAVED )
                  if ( beem.tracer == 1 || beem.tracer == 2 || beem.tracer == 4 )
                    break;

                if ( monster->behavior == BEH_FIGHT )
                  if ( beem.tracer == 3 && beem.tracer_mons == 0 )
                    break;
            }

            dragon(monster, &beem);
            mmov_x = 0;
            used = true;        // $$$ why don't more of these cases set this?

        }
        break;
    }

    return used;

}          // end handle_special_ability()




//---------------------------------------------------------------
//
// handle_potion
//
// Give the monster a chance to quaff a potion. Returns true if
// the monster imbibed.
//
//---------------------------------------------------------------
static bool handle_potion( struct monsters *monster, bolt & beem )
{

// yes, there is a logic to this ordering {dlb}:
    if ( monster->behavior == BEH_SLEEP )
      return false;
    else if ( monster->inv[MSLOT_POTION] == NON_ITEM )
      return false;
    else if ( !one_chance_in(3) )
      return false;
    else
    {
        bool imbibed = false;

        switch ( mitm.sub_type[monster->inv[MSLOT_POTION]] )
        {
            case POT_HEALING:
            case POT_HEAL_WOUNDS:
              if ( monster->hit_points <= monster->max_hit_points / 2 )
              {
                  simple_monster_message(monster, " drinks a potion.");

                  if ( heal_monster(monster, 5 + random2(7), false) )
                    simple_monster_message(monster, " is healed!");

                  if ( mitm.sub_type[monster->inv[MSLOT_POTION]] == POT_HEAL_WOUNDS )
                    heal_monster(monster, 10 + random2avg(28,3), false);

                  imbibed = true;
              }
              break;

            case POT_SPEED:
              beem.colour = BLUE;    // notice that these are the same odd colours used in mons_ench_f2() {dlb}
        // intentional fall through
            case POT_INVISIBILITY:
              if ( mitm.sub_type[monster->inv[MSLOT_POTION]] == POT_INVISIBILITY )
                beem.colour = MAGENTA;

              if ( !mons_near(monster) )    // why only drink these if not near player? {dlb}
              {
                  simple_monster_message(monster, " drinks a potion.");

                  mons_ench_f2(monster, &beem);

                  imbibed = true;
              }
              break;
        }

        if ( imbibed )
        {
            mitm.quantity[monster->inv[MSLOT_POTION]]--;

            if ( mitm.quantity[monster->inv[MSLOT_POTION]] < 1 )
              destroy_item(monster->inv[MSLOT_POTION]);
        }

        return imbibed;

    }


}          // end handle_potion()




static bool handle_reaching( struct monsters *monster )
{

    bool ret = false;

    if ( monster->inv[MSLOT_WEAPON] != NON_ITEM && mitm.base_type[monster->inv[MSLOT_WEAPON]] == OBJ_WEAPONS
        && mitm.special[monster->inv[MSLOT_WEAPON]] < NWPN_SINGING_SWORD
        && mitm.special[monster->inv[MSLOT_WEAPON]] % 30 == SPWPN_REACHING )
    {
        int dx = abs(monster->x - you.x_pos);
        int dy = abs(monster->y - you.y_pos);

        if ( (dx == 2 && dy <= 2) || (dy == 2 && dx <= 2) )
        {
            ret = true;
            monster_attack(monster_index(monster));
        }
    }

    return ret;

}          // end handle_reaching()




//---------------------------------------------------------------
//
// handle_scroll
//
// Give the monster a chance to read a scroll. Returns true if
// the monster read something.
//
//---------------------------------------------------------------
static bool handle_scroll( struct monsters *monster )
{

// yes, there is a logic to this ordering {dlb}:
    if ( monster->behavior == BEH_CONFUSED || monster->behavior == BEH_SLEEP )
      return false;
    else if ( monster->inv[MSLOT_SCROLL] == NON_ITEM )
      return false;
    else if ( !one_chance_in(3) )
      return false;
    else
    {
        bool read = false;

    // notice how few cases are actually accounted for here {dlb}:
        switch ( mitm.sub_type[monster->inv[MSLOT_SCROLL]] )
        {
            case SCR_TELEPORTATION:
              if ( monster->enchantment[0] != ENCH_TP_I
                  && monster->enchantment[1] != ENCH_TP_I
                  && monster->enchantment[2] != ENCH_TP_I )
              {
                  if ( monster->behavior == BEH_FLEE )
                  {
                      simple_monster_message(monster, " reads a scroll.");
                      monster_teleport(monster, false);
                      read = true;
                  }
              }
              break;

            case SCR_BLINKING:
              if ( monster->behavior == BEH_FLEE )
              {
                  if ( mons_near(monster) )
                  {
                      simple_monster_message(monster, " reads a scroll.");
                      simple_monster_message(monster, " blinks!");
                      monster_blink(monster);
                      read = true;
                  }
              }
              break;

            case SCR_SUMMONING:
              if ( mons_near(monster) )
              {
                  simple_monster_message(monster, " reads a scroll.");
                  create_monster(MONS_ABOMINATION_SMALL, 21, monster->behavior, monster->x, monster->y, monster->monster_foe, 250);
                  read = true;
              }
              break;
        }

        if ( read )
        {
            mitm.quantity[monster->inv[MSLOT_SCROLL]]--;

            if ( mitm.quantity[monster->inv[MSLOT_SCROLL]] < 1 )
              destroy_item(monster->inv[MSLOT_SCROLL]);
        }

        return read;

    }

}          // end handle_scroll()




//---------------------------------------------------------------
//
// handle_wand
//
// Give the monster a chance to zap a wand. Returns true if the
// monster zapped.
//
//---------------------------------------------------------------
static bool handle_wand( struct monsters *monster, bolt & beem )
{

// yes, there is a logic to this ordering {dlb}:
    if ( monster->behavior == BEH_SLEEP )
      return false;
    else if ( monster->behavior == BEH_CHASING_II && !mons_near(monster) )
      return false;
    else if ( ( monster->behavior == BEH_FIGHT || monster->behavior == BEH_ENSLAVED ) && monster->monster_foe == MHITNOT )
      return false;
    else if ( monster->inv[MSLOT_WAND] == NON_ITEM || mitm.pluses[monster->inv[MSLOT_WAND]] <= 0 )
      return false;
    else if ( coinflip() )
      return false;
    else
    {
        bool zap = false;

        switch ( mitm.sub_type[monster->inv[MSLOT_WAND]] )
        {
        // these have been deemed "to tricky" at this time {dlb}:
            case WAND_POLYMORPH_OTHER:
            case WAND_ENSLAVEMENT:
            case WAND_DIGGING:
            case WAND_FIREBALL:
            case WAND_RANDOM_EFFECTS:
              return false;
              break;

        // these are wands that monsters will aim at themselves {dlb}:
            case WAND_HASTING:
              if ( monster->enchantment[0] != ENCH_HASTE
                  && monster->enchantment[1] != ENCH_HASTE
                  && monster->enchantment[2] == ENCH_HASTE )
              {
                  beem.target_x = monster->x;
                  beem.target_y = monster->y;

                  zap = true;
              }
              break;

            case WAND_HEALING:
              if ( monster->hit_points <= monster->max_hit_points / 2 )
              {
                  beem.target_x = monster->x;
                  beem.target_y = monster->y;

                  zap = true;
              }
              break;

            case WAND_INVISIBILITY:
              if ( monster->enchantment[2] != ENCH_INVIS
                  && !player_see_invis() )    // added this second bit, a cheat, I know {dlb}
              {
                  beem.target_x = monster->x;
                  beem.target_y = monster->y;
                  zap = true;
              }
              break;

            case WAND_TELEPORTATION:
              if ( monster->hit_points <= monster->max_hit_points / 2 )
              {
                  if ( monster->enchantment[0] != ENCH_TP_I
                      && monster->enchantment[1] != ENCH_TP_I
                      && monster->enchantment[2] != ENCH_TP_I )
                  {
                      beem.target_x = monster->x;
                      beem.target_y = monster->y;

                      zap = true;
                  }
              }
              break;

        // every other wand will be aimed at something else {dlb}:
            default:
              zap = true;

              tracer_f(monster, &beem);

              if ( beem.tracer == 0 )
                zap = false;

              if ( monster->behavior == BEH_ENSLAVED )
              {
                  if ( beem.tracer == 1 || beem.tracer == 2 )
                    zap = false;

                  if ( beem.tracer_mons == 4 )
                    zap = false;
              }
              else
              {
                  if ( beem.tracer == 3 && beem.tracer_mons == 0 )
                    zap = false;
              }
              break;

        }        // end "switch mitm.sub_type[monster->inv[MSLOT_WAND]]"

    // now the monster will attempt to actually zap the wand {dlb}:
        if ( zap )
        {
            int mzap = 0;

            switch ( mitm.sub_type[monster->inv[MSLOT_WAND]] )
            {
                case WAND_FLAME:
                  mzap = MS_FLAME;
                  break;
                case WAND_FROST:
                  mzap = MS_FROST;
                  break;
                case WAND_SLOWING:
                  mzap = MS_SLOW;
                  break;
                case WAND_HASTING:
                  mzap = MS_HASTE;
                  break;
                case WAND_MAGIC_DARTS:
                  mzap = MS_MMISSILE;
                  break;
                case WAND_HEALING:
                  mzap = MS_HEAL;
                  break;
                case WAND_PARALYSIS:
                  mzap = MS_PARALYSIS;
                  break;
                case WAND_FIRE:
                  mzap = MS_FIRE_BOLT;
                  break;
                case WAND_COLD:
                  mzap = MS_COLD_BOLT;
                  break;
                case WAND_CONFUSION:
                  mzap = MS_CONFUSE;
                  break;
                case WAND_INVISIBILITY:
                  mzap = MS_INVIS;
                  break;
                case WAND_TELEPORTATION:
                  mzap = MS_TELEPORT_OTHER;
                  break;
                case WAND_LIGHTNING:
                  mzap = MS_LIGHTNING_BOLT;
                  break;
                case WAND_DRAINING:
                  mzap = MS_NEGATIVE_BOLT;
                  break;
            }

            viewwindow(1, false);

            beem.move_x = beem.trac_targ_x - monster->x;
            beem.move_y = beem.trac_targ_y - monster->y;

            beem.target_x = beem.trac_targ_x;
            beem.target_y = beem.trac_targ_y;

            beem.source_x = monster->x;
            beem.source_y = monster->y;

            int power = 25;

            struct SBeam theBeam = mons_spells(mzap, power);

            beem.colour = theBeam.colour;
            beem.range = theBeam.range;
            beem.damage = theBeam.damage;
            beem.hit = theBeam.hit;
            beem.type = theBeam.type;
            beem.flavour = theBeam.flavour;
            beem.thing_thrown = theBeam.thrown;

            strcpy(beem.beam_name, theBeam.name.c_str());

            beem.beam_source = monster_index(monster);
            beem.ench_power = beem.damage;
            // !!!    // was set to 20 immediately prior to this,
            // but this is the value passed along {dlb}

            if ( mzap == MS_HASTE || mzap == MS_HEAL || mzap == MS_INVIS
                 || mzap == MS_TELEPORT_OTHER )
            {
                beem.move_x = 0;
                beem.move_y = 0;
            }
            else
            {
                if ( beem.move_x == 0 && beem.move_y == 0 )
                zap = false;
            }

            if ( zap )
            {
                if ( !simple_monster_message(monster, " zaps a wand.") )
                  if ( !silenced(you.x_pos, you.y_pos) )
                    mpr("You hear a zap.");

                mitm.pluses[monster->inv[MSLOT_WAND]]--; // charge expenditure {dlb}

                if (theBeam.isBeam)
                  beam(&beem);
                else
                  missile(&beem, 0);
            }

// remember: the monster may well be dead here (eg if it zapped a /o lightning)

        }

        return zap;

    }

}          // end handle_wand()




//---------------------------------------------------------------
//
// handle_spell
//
// Give the monster a chance to cast a spell. Returns true if
// a spell was cast.
//
//---------------------------------------------------------------
static bool handle_spell( struct monsters *monster, bolt & beem )
{

    int temp_rand = 0; // probability determination {dlb}
    bool monsterNearby = mons_near(monster);
    bool finalAnswer = false; // as in: "Is that your ... ?" {dlb}

    // yes, there is a logic to this ordering {dlb}:
    if ( monster->behavior == BEH_SLEEP
         || !mons_flag(monster->type, M_SPELLCASTER) ) // oopsie - bug waiting to happen - I'll fix this with the new monster struct {dlb}
      return false;
    if ( ( mons_flag(monster->type, M_ACTUAL_SPELLS)
           || mons_flag(monster->type, M_PRIEST ) )
         && ( monster->enchantment[1] == ENCH_GLOWING_SHAPESHIFTER
              || monster->enchantment[1] == ENCH_SHAPESHIFTER ))
      return false; //jmf: shapeshiftes don't get spells, just
                    //     physical powers.
    else if ( you.invis && !mons_see_invis(monster->type) )
      return false;
    else if (monster->behavior == BEH_CONFUSED && monster->type != MONS_VAPOUR)
      return false;
    else if ( ( monster->behavior == BEH_FIGHT
                || monster->behavior == BEH_ENSLAVED )
              && monster->monster_foe == MHITNOT )
      return false;
    else if ( monster->type == MONS_PANDEMONIUM_DEMON && ghost.values[9] == 0 )
      return false;
    else if ( random2(200) > 50 + monster->hit_dice )
      return false;
    else
    {
        int spell_cast = 100;
        int hspell_pass[6] = { 100, 100, 100, 100, 100 };

    // this little bit makes no sense, esp. given how
    // hellions are treated below -- a remnant of past
    // ideas on how to handle hellion casting? {dlb}:
        if ( monster->type == MONS_HELLION )
        {
            temp_rand = random2(3);

            monster->number = ( (temp_rand == 0) ? RED :
                                (temp_rand == 1) ? LIGHTRED
                                                 : YELLOW );
        }

        int msecc = ( (monster->type == MONS_HELLION)           ? 30 :
                      (monster->type == MONS_PANDEMONIUM_DEMON) ? 119
                                                         : monster->number );

        // tracer = 0 = run out of range
        // tracer = 1 = hits you in range
        // tracer_mons = 0 = hits no monsters
        //       "     = 1 = hits monster before you (possibly also after)
        //       "     = 2 = hits monster after but not before
        //       "     = 3 = hits tamed monster(s) but no others

        mons_spell_list(msecc, hspell_pass);

    // forces the casting of dig when player not visible - this is EVIL!
        if ( !monsterNearby )
        {
          if ( hspell_pass[4] == MS_DIG && monster->behavior == BEH_CHASING_I )
            {
              spell_cast = MS_DIG;
              finalAnswer = true;
            }
          else if ( beem.trac_targ == MHITYOU )
            return false;
        }

        /*
           Used by monsters in "planning" which spell to cast. Fires off a
           "tracer" which tells the monster what it'll hit if it breathes/casts

           tracer = 0 = run out of range or hits a wall
           tracer = 1 = hits you in range
           tracer = 2 = hits friendly monster
           tracer = 3 = hits hostile monster

           tracer_mons = 1 = hits monster specified in trac_targ

           tracer_hit_mons now holds value of mons_see_invis of attacking
           monster -- This is used in missile().
           If it's 0, won't register invis monsters or you

           Note: only the missile() function is used for tracers. The tracer
           code in beam() is obsolete and useless.
         */

        if ( !finalAnswer )
        {
            tracer_f(monster, &beem);

            if ( beem.tracer == 0 )
              return false;

            if ( monster->behavior == BEH_ENSLAVED )
            {
            // Although it would be nice if friendly monsters also cast
            // smiting etc like this, too tricky right now.
                if ( beem.tracer == 1 || beem.tracer == 2 )
                  return false;

            }
            else
            {
            // Is a monster in between? Then cast
            // a direct spell (smiting etc) or a self-enchantment:
                if ( beem.tracer == 3 && beem.tracer_mons == 0 )
                {
                    spell_cast = hspell_pass[2];
                    finalAnswer = true;
                }
            }
        }


    // yes, this all needs to be kept apart from the above {dlb}:
        if ( !finalAnswer )
        {
        // should monster not have selected dig by now, it never will:
            if ( hspell_pass[4] == MS_DIG )
              hspell_pass[4] = 100;

        // select which spell to cast {dlb}:
            if ( monster->behavior != BEH_FLEE )
            {
              if ( ( monster->behavior != BEH_ENSLAVED
                     && (beem.tracer == 1 || beem.tracer == 2
                         || beem.tracer_mons == 1) )
                   || (monster->behavior == BEH_ENSLAVED && beem.tracer == 3) )
                {
                // monsters get up to three tries to pick a spell {dlb}:
                  for (int loopy = 0; loopy < 3; loopy++)
                    {
                      spell_cast = hspell_pass[random2(5)];

                      if ( spell_cast != 100 )
                        break;
                    }
                }
              else if ( ( monster->behavior == BEH_ENSLAVED
                          && beem.tracer_mons == 0 ) || coinflip() )
                spell_cast = hspell_pass[2];
              else
                spell_cast = 100;
            }
            else
              {
                spell_cast = ( one_chance_in(5) ? 100 : hspell_pass[5] );
              }

            finalAnswer = true;
        }


    // should the monster *still* not have a spell, well, too bad {dlb}:
        if ( spell_cast == 100 )
          return false;

    // can't see anything to animate:
        if ( spell_cast == MS_ANIMATE_DEAD
             && ( !monsterNearby || !animate_dead(100, monster->behavior,
                                                  monster->monster_foe, 0) ) )
          return false;

        if ( monsterNearby )    // handle monsters within range of player
        {
            switch ( monster->type )
            {
                default:
                  if ( silenced(monster->x, monster->y) )
                    return false;

                  simple_monster_message(monster, (mons_flag(monster->type, M_PRIEST)) ? " utters an invocation." : " casts a spell." );
                  break;

                case MONS_STEAM_DRAGON:
                case MONS_MOTTLED_DRAGON:
                case MONS_STORM_DRAGON:
                case MONS_GOLDEN_DRAGON:
                case MONS_SHADOW_DRAGON:
                case MONS_SWAMP_DRAGON:
                case MONS_SWAMP_DRAKE:
                case MONS_HELL_HOG:
                case MONS_SERPENT_OF_HELL:
                case MONS_QUICKSILVER_DRAGON:
                case MONS_IRON_DRAGON:
                  if ( !simple_monster_message(monster, " breathes.") )
                    if ( !silenced(monster->x, monster->y) && !silenced(you.x_pos, you.y_pos) )
                      mpr("You hear a roar.");
                  break;

                case MONS_GERYON:
                  if ( silenced(monster->x, monster->y) )
                    return false;
                  simple_monster_message(monster, " winds a great silver horn.");
                  break;

                case MONS_VAPOUR:
                  monster->enchantment[2] = ENCH_INVIS;
                  break;

                case MONS_BRAIN_WORM :
                  break;

                case MONS_GREAT_ORB_OF_EYES:
                case MONS_SHINING_EYE:
                case MONS_EYE_OF_DEVASTATION:
                  simple_monster_message(monster, " gazes.");
                  break;

                case MONS_GIANT_ORANGE_BRAIN:
                  simple_monster_message(monster, " pulsates.");
                  break;

                case MONS_NAGA:
                case MONS_NAGA_WARRIOR:
                  simple_monster_message(monster, " spits poison.");
                  break;

                case MONS_EFREET:
                case MONS_HELLION:
                case MONS_ROTTING_DEVIL:
                case MONS_TORMENTOR:
                case MONS_REAPER:
                case MONS_SOUL_EATER:
                case MONS_HAIRY_DEVIL:
                case MONS_ICE_DEVIL:
                case MONS_BLUE_DEVIL:
                case MONS_BEAST:
                case MONS_IRON_DEVIL:
                case MONS_WHITE_IMP:
                case MONS_LEMURE:
                case MONS_UFETUBUS:
                case MONS_MANES:
                case MONS_MIDGE:
                case MONS_NEQOXEC:
                case MONS_ORANGE_DEMON:
                case MONS_HELLWING:
                case MONS_SMOKE_DEMON:
                case MONS_YNOXINUL:
                case MONS_EXECUTIONER:
                case MONS_GREEN_DEATH:
                case MONS_BLUE_DEATH:
                case MONS_BALRUG:
                case MONS_CACODEMON:
                  simple_monster_message(monster, " gestures.");
                  break;
            }
        }
        else    // handle far-away monsters
        {
            if ( monster->type == MONS_GERYON
                 && !silenced(you.x_pos, you.y_pos) )
              mpr("You hear a weird and mournful sound.");
        }


    // FINALLY! determine primary spell effects {dlb}:
        if ( spell_cast == MS_BLINK && monsterNearby ) // why only cast blink if nearby? {dlb}
        {
            simple_monster_message(monster, " blinks!");
            monster_blink(monster);
        }
        else
        {
            beem.damage = 8 * monster->hit_dice; // really??? // hmmm... {dlb}
            beem.beam_source = monster_index(monster);
            mons_cast(monster, &beem, spell_cast);
            mmov_x = 0;
            beem.trac_hit_tamed = 0;
        }

    } // end "if mons_flag(monster->type, M_SPELLCASTER) && monster->behavior != BEH_SLEEP"

    return true;

}          // end handle_spell()




//---------------------------------------------------------------
//
// handle_throw
//
// Give the monster a chance to throw something. Returns true if
// the monster hurled.
//
//---------------------------------------------------------------
static bool handle_throw( struct monsters *monster, bolt & beem )
{

// yes, there is a logic to this ordering {dlb}:
    if ( monster->behavior == BEH_CONFUSED || monster->behavior == BEH_SLEEP )
      return false;
    else if ( ( monster->behavior == BEH_FIGHT || monster->behavior == BEH_ENSLAVED ) && monster->monster_foe == MHITNOT )
      return false;
    else if ( you.invis && !mons_see_invis(monster->type) )
      return false;
    else if ( !mons_itemuse(monster->type) )
      return false;
    else if ( monster->inv[MSLOT_MISSILE] == NON_ITEM || mitm.quantity[monster->inv[MSLOT_MISSILE]] <= 0 )
      return false;
    else if ( beem.trac_targ == MHITYOU && ( beem.tracer_mons == 1 || !mons_near(monster) ) )
      return false;
    else if ( monster->type == MONS_TWO_HEADED_OGRE )    // poor two-headed ogres... {dlb}
      return false;
    else if ( one_chance_in(5) )
      return false;
    else
    {
        tracer_f(monster, &beem);

        if ( beem.tracer == 0 )
          return false;

        if ( monster->behavior == BEH_ENSLAVED )
        {
            if ( ( beem.tracer == 1 || beem.tracer == 2 ) )
              return false;

            if ( beem.tracer == 1 && beem.tracer_mons != 1 )
              return false;

            if ( beem.tracer_mons == 4 )
              return false;
        }
        else
        {
            if ( beem.tracer == 3 && beem.tracer_mons == 0 )
              return false;
        }

        if ( beem.tracer != 0 || ( beem.trac_targ != MHITYOU && beem.trac_hit_mons != 0 ) )
        {               // doesn't need to worry about you.haste
            beem.range = 6;

            item_name(mitm.pluses2[monster->inv[MSLOT_MISSILE]], mitm.base_type[monster->inv[MSLOT_MISSILE]], mitm.sub_type[monster->inv[MSLOT_MISSILE]], mitm.special[monster->inv[MSLOT_MISSILE]], mitm.pluses[monster->inv[MSLOT_MISSILE]], 1, mitm.id[monster->inv[MSLOT_MISSILE]], 6, str_pass);
            strcpy(beem.beam_name, str_pass);

            return mons_throw(monster, &beem, monster->inv[MSLOT_MISSILE]);
        }
    }

    return false;

}          // end handle_throw()


//---------------------------------------------------------------
//
// monster
//
// This is the routine that controls monster AI.
//
//---------------------------------------------------------------
void monster( void )
{
    bool bat = false;
    bool brkk = false;
    int monc = 0;
    int j;

    struct bolt beem[1];

    FixedArray<unsigned int, 19, 19> show;

    losight(show, grd, you.x_pos, you.y_pos);

    for (int i = 0; i < MAX_MONSTERS; i++)
    {
        struct monsters *monster = &menv[i];

        if ( monster->type != -1 )
        {
            monc++;

            if (monster->hit_points > monster->max_hit_points)
              monster->hit_points = monster->max_hit_points;

            if (monster->type == MONS_GIANT_SPORE && monster->hit_points < 1)
            {
                for (j = 0; j < 3; j++)
                  monster->enchantment[j] = ENCH_NONE;

                monster->enchantment1 = 0;
                monster->type = -1;
                mgrd[monster->x][monster->y] = NON_MONSTER;
                spore_goes_pop(monster);      // has to be after the above, so that spore isn't killed twice.

                continue;
            }

            monster->speed_increment += (monster->speed * you.time_taken / 10);

            if (you.slow > 0)
              monster->speed_increment += (monster->speed * you.time_taken / 10);

            while (monster->speed_increment >= 80)
            {                   // The continues & breaks are WRT this.

                if ( monster->type != -1 && monster->hit_points < 1 )
                {
                    monster_die(monster, KILL_MISC, 0);

                    if ( monster->type != MONS_GIANT_SPORE )
                      continue;
                }

                monster->speed_increment -= 10;

                if (env.cgrid[monster->x][monster->y] != EMPTY_CLOUD)
                {
                    if ( monster_habitat(monster->type) != DNGN_FLOOR && monster->number == 1 )
                      break;

                    if ( monster->type == -1 )
                      break;  // problem with vortices

                    mons_in_cloud(monster);

                    if (monster->type == -1)
                    {
                        monster->speed_increment = 1;
                        break;
                    }
                }

                if ( you.invis && !mons_see_invis(monster->type) && monster->target_x == monster->x && monster->target_y == monster->y)
                  monster->behavior = BEH_WANDER;

                if ( monster->type == MONS_GLOWING_SHAPESHIFTER )
                  monster->enchantment[1] = ENCH_GLOWING_SHAPESHIFTER;

                if ( monster->type == MONS_SHAPESHIFTER )
                  monster->enchantment[1] = ENCH_SHAPESHIFTER;         // otherwise there are potential problems with summonings

                handle_behavior(monster);

                if ( handle_enchantment(monster) )
                  goto endbat;

            // regenerate:
                if ( monster_descriptor(monster->type, MDSC_REGENERATES) || one_chance_in(25) )
                  heal_monster(monster, 1, false);

                if ( monster->speed >= 100 )
                  continue;

                if ( monster->type == MONS_ZOMBIE_SMALL || monster->type == MONS_ZOMBIE_LARGE
                      || monster->type == MONS_SKELETON_SMALL || monster->type == MONS_SKELETON_LARGE )
                  monster->max_hit_points = monster->hit_points;

                if ( igrd[monster->x][monster->y] != NON_ITEM && ( mons_itemuse(monster->type) == 3 || monster->type == MONS_JELLY || monster->type == MONS_NECROPHAGE || monster->type == MONS_GHOUL ) )
                {
                    if ( mons_pickup(monster) )
                      continue;
                }

                handle_movement(monster);

                brkk = false;

                if (monster->behavior == BEH_CONFUSED)
                {
                    mmov_x = random2(3) - 1;
                    mmov_y = random2(3) - 1;

                    if (mgrd[monster->x + mmov_x][monster->y + mmov_y] != NON_MONSTER && (mmov_x != 0 || mmov_y != 0))
                    {
                        if ( monsters_fight(i, mgrd[monster->x + mmov_x][monster->y + mmov_y]) )
                        {
                            mmov_x = 0;
                            mmov_y = 0;
                            brkk = true;
                            break;
                        }
                    }
                }

                if ( brkk )
                  continue;

                handle_nearby_ability(monster);

                if (((monster->behavior == BEH_CHASING_I || monster->behavior == BEH_FLEE) && mons_near(monster)) || ((monster->behavior == BEH_FIGHT || monster->behavior == BEH_ENSLAVED) && monster->monster_foe != MHITNOT))
                {
                    if ((monster->behavior == BEH_CHASING_I || monster->behavior == BEH_FLEE) && mons_near(monster) )
                        you.pet_target = i;

                    if ((monster->behavior == BEH_FIGHT || monster->behavior == BEH_ENSLAVED) && (monster->monster_foe == MHITNOT || monster->monster_foe == MHITYOU))
                        goto end_switch;

                    if (monster->behavior == BEH_FIGHT || monster->behavior == BEH_ENSLAVED)
                    {
                        beem[0].trac_targ = monster->monster_foe;

                        if (monster->monster_foe == MHITNOT)
                        {
                            beem[0].trac_targ_x = monster->x;
                            beem[0].trac_targ_y = monster->y;

                        }
                        else
                        {
                            beem[0].trac_targ_x = menv[monster->monster_foe].x;
                            beem[0].trac_targ_y = menv[monster->monster_foe].y;
                        }

                    }
                    else
                    {
                        beem[0].trac_targ = MHITYOU;
                        beem[0].trac_targ_x = you.x_pos;
                        beem[0].trac_targ_y = you.y_pos;
                    }

                    if ( beem[0].trac_targ == MHITYOU && !mons_near(monster) && monster->type != MONS_CACODEMON )
                      goto end_switch;

                    if ( handle_special_ability(monster, beem[0]) )
                      continue;

                    if ( handle_potion(monster, beem[0]) )
                      continue;

                    if ( handle_scroll(monster) )
                      continue;

                    if ( handle_wand(monster, beem[0]) )
                      continue;
                    else if ( monster->behavior == BEH_ENSLAVED && beem[0].tracer_mons == 4 )
                      goto end_throw;         // $$$ is this right?

                    if ( handle_spell(monster, beem[0]) )
                      continue;

                    if ( handle_reaching(monster) )
                      continue;

end_switch:
                    if ( handle_throw(monster, beem[0]) )
                      continue;
                }

end_throw:
                if ((monster->behavior == BEH_ENSLAVED || monster->behavior == BEH_FIGHT) && (mmov_x != 0 || mmov_y != 0) && mgrd[monster->x + mmov_x][monster->y + mmov_y] != NON_MONSTER)
                {
                    if ( monsters_fight(i, mgrd[monster->x + mmov_x][monster->y + mmov_y]) )
                    {
                        if (monster->type == MONS_GIANT_BAT || monster->type == MONS_UNSEEN_HORROR || monster->type == MONS_GIANT_BLOWFLY)
                          monster->speed_increment -= monster->speed;

                        mmov_x = 0;
                        mmov_y = 0;
                        brkk = true;
                        break;
                    }
                }

                if ( brkk )
                  continue;

                if (monster->x + mmov_x == you.x_pos && monster->y + mmov_y == you.y_pos)
                {
                    mmov_x = monster->inv[MSLOT_WEAPON];

                    if ( monster->type == MONS_GIANT_BAT || monster->type == MONS_UNSEEN_HORROR || monster->type == MONS_GIANT_BLOWFLY )
                    {
                        if ( !bat )
                          monster_attack(i);

                        if ( monster->behavior != BEH_ENSLAVED )
                        {
                            monster->behavior = BEH_WANDER;
                            bat = true;
                        }
                    }
                    else
                      monster_attack(i);

                    if ( monster->type == MONS_GIANT_SPORE && monster->hit_points < 1 )
                    {
                        for (j = 0; j < 3; j++)
                          monster->enchantment[j] = ENCH_NONE;

                        monster->enchantment1 = 0;
                        //no_mons--;
                        monster->type = -1;
                        mgrd[monster->x][monster->y] = NON_MONSTER;
                        spore_goes_pop(monster);      /* has to be after the above, so that spore isn't killed twice. */
                        continue;
                    }

                    mmov_x = 0;
                    mmov_y = 0;
                    continue;   //break;

                }

                if ( monster->type == -1 || monster->type == MONS_OKLOB_PLANT || monster->type == MONS_CURSE_SKULL || (monster->type >= MONS_CURSE_TOE && monster->type <= MONS_POTION_MIMIC ) )
                  continue;

                monster_move(monster);

                if ( monster->behavior == BEH_CHASING_I )
                {
                    if ( mons_near(monster) && ( !you.invis || mons_see_invis(monster->type) ) )
                    {
                        monster->target_x = you.x_pos;
                        monster->target_y = you.y_pos;
                    }
                }

                // Tame monsters can't become afraid, because fear overwrites tameness
                if (monster->hit_points <= (monster->max_hit_points / 4 - 1) && mons_intel(monster->type) > I_ANIMAL && monster->behavior != BEH_ENSLAVED)
                  monster->behavior = BEH_FLEE;
            }                   // end while

endbat:
            bat = false;

            if ( monster->behavior == BEH_FIGHT && !one_chance_in(3) )
              monster->behavior = BEH_CHASING_I;       // I think?

        }                       // end of if (mons_class != -1)

    }                           // end of for loop

}          // end monster()




//---------------------------------------------------------------
//
// mons_pickup
//
// Returns 0 if monster doesn't spend any time pickup up
//
//---------------------------------------------------------------
static bool mons_pickup( struct monsters *monster )
{

    bool monsterNearby = mons_near(monster);    // single calculation permissible {dlb}

    if ( monster->type == MONS_JELLY
          || monster->type == MONS_BROWN_OOZE
          || monster->type == MONS_ACID_BLOB
          || monster->type == MONS_ROYAL_JELLY )
    {
        int hps_gained;
        unsigned int quant_eated = 1 + random2(10);    // value range: [1,10] {dlb}

        if ( quant_eated > mitm.quantity[igrd[monster->x][monster->y]] )
          quant_eated = mitm.quantity[igrd[monster->x][monster->y]];

        if ( mitm.base_type[igrd[monster->x][monster->y]] == OBJ_WEAPONS
            && mitm.special[igrd[monster->x][monster->y]] >= NWPN_SINGING_SWORD )
          return false;           /* unique items */

        if ( mitm.base_type[igrd[monster->x][monster->y]] == OBJ_MISSILES
            && ( mitm.sub_type[igrd[monster->x][monster->y]] == MI_STONE
                  || mitm.sub_type[igrd[monster->x][monster->y]] == MI_LARGE_ROCK ) )
        {
            /* shouldn't eat stone things.
               - but what about stone wands & rings? */
            return false;
        }

        if ( mitm.base_type[igrd[monster->x][monster->y]] != OBJ_GOLD )
        {
            hps_gained = quant_eated * mass(mitm.base_type[igrd[monster->x][monster->y]], mitm.sub_type[igrd[monster->x][monster->y]]);
            hps_gained /= 20;
        }
        else
          hps_gained = 1;    // seems a bit of a trifle not to scale this {dlb}

        if ( hps_gained < 1 )
          hps_gained = 1;
        else if ( hps_gained > 50 )
          hps_gained = 50;

        mitm.quantity[igrd[monster->x][monster->y]] -= quant_eated;

        if ( mitm.quantity[igrd[monster->x][monster->y]] < 1 )
          igrd[monster->x][monster->y] = mitm.link[igrd[monster->x][monster->y]];

        monster->hit_points += hps_gained;

    // note here, that this makes jellies "grow" {dlb}:
        if ( monster->hit_points > monster->max_hit_points )
          monster->max_hit_points = monster->hit_points;

        if ( !silenced(you.x_pos, you.y_pos) && !silenced(monster->x, monster->y) )
        {
            strcpy(info, "You hear a");
            if ( !monsterNearby )
              strcat(info, " distant");
            strcat(info, " slurping noise.");
            mpr(info);
        }

        if ( monster->hit_points > 50 )
          if ( !jelly_divide(monster) )    // and here is where the jelly might divide {dlb}
            monster->hit_points = 50;

        return false;

    }        // end "if jellies"


    switch ( mitm.base_type[igrd[monster->x][monster->y]] )
    {
      case OBJ_WEAPONS:
        if ( monster->inv[MSLOT_WEAPON] != NON_ITEM )
          return false;
        if ( mitm.special[igrd[monster->x][monster->y]] >= NWPN_SINGING_SWORD )
          return false;
        if ( mitm.special[igrd[monster->x][monster->y]] % 30 >= SPWPN_RANDART_I )
          return false;
// wimpy monsters (Kob, gob) shouldn't pick up halberds etc
// of course, this also block knives {dlb}:
        if ( ( mons_charclass(monster->type) == MONS_KOBOLD
                || mons_charclass(monster->type) == MONS_GOBLIN )
            && property(mitm.base_type[igrd[monster->x][monster->y]], mitm.sub_type[igrd[monster->x][monster->y]], PWPN_HIT) <= 0 )
          return false;
// Nobody picks up giant clubs:
        if ( mitm.sub_type[igrd[monster->x][monster->y]] == WPN_GIANT_CLUB
              || mitm.sub_type[igrd[monster->x][monster->y]] == WPN_GIANT_SPIKED_CLUB )
          return false;

        monster->inv[MSLOT_WEAPON] = igrd[monster->x][monster->y];
        igrd[monster->x][monster->y] = mitm.link[igrd[monster->x][monster->y]];
        mitm.link[monster->inv[MSLOT_WEAPON]] = NON_ITEM;

        if ( mitm.special[monster->inv[MSLOT_WEAPON]] % 30 == SPWPN_PROTECTION )
          monster->armor_class += 3;

        if ( monsterNearby )
        {
            strcpy(info, monam(monster->number, monster->type, monster->enchantment[2], 0));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_WEAPON], 3, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

      case OBJ_MISSILES:
/* Removed check for item_plus2 - probably irrelevant */
        if ( monster->inv[MSLOT_MISSILE] != NON_ITEM
            && mitm.sub_type[monster->inv[MSLOT_MISSILE]] == mitm.sub_type[igrd[monster->x][monster->y]]
            && mitm.pluses[monster->inv[MSLOT_MISSILE]] == mitm.pluses[igrd[monster->x][monster->y]]
            && mitm.special[monster->inv[MSLOT_MISSILE]] == mitm.special[igrd[monster->x][monster->y]] )
        {
            if ( monsterNearby )
            {
                strcpy(info, monam(monster->number, monster->type, monster->enchantment[2], 0));
                strcat(info, " picks up ");
                it_name(igrd[monster->x][monster->y], 3, str_pass);
                strcat(info, str_pass);
                strcat(info, ".");
                mpr(info);
            }

            mitm.quantity[monster->inv[MSLOT_MISSILE]] += mitm.quantity[igrd[monster->x][monster->y]];
            mitm.quantity[igrd[monster->x][monster->y]] = 0;
            igrd[monster->x][monster->y] = mitm.link[igrd[monster->x][monster->y]];
            mitm.link[monster->inv[MSLOT_MISSILE]] = NON_ITEM;
            break;
        }
    // nobody bothers to pick up rocks if they don't already have some:
        if ( mitm.sub_type[igrd[monster->x][monster->y]] == MI_LARGE_ROCK )
          return false;
    // nobody stoops to pick up just one more of something they already have {dlb}
        if ( monster->inv[MSLOT_MISSILE] != NON_ITEM || mitm.quantity[igrd[monster->x][monster->y]] == 1 )
          return false;
    // monsters with powerful melee attacks don't bother
        if ( mondamage(monster->type, 0) > 5 )
          return false;

        monster->inv[MSLOT_MISSILE] = igrd[monster->x][monster->y];
        igrd[monster->x][monster->y] = mitm.link[igrd[monster->x][monster->y]];
        mitm.link[monster->inv[MSLOT_MISSILE]] = NON_ITEM;

        if ( monsterNearby )
        {
            strcpy(info, monam(monster->number, monster->type, monster->enchantment[2], 0));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_MISSILE], 3, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

      case OBJ_WANDS:
        if ( monster->inv[MSLOT_WAND] != NON_ITEM )
          return false;

        monster->inv[MSLOT_WAND] = igrd[monster->x][monster->y];
        igrd[monster->x][monster->y] = mitm.link[igrd[monster->x][monster->y]];
        mitm.link[monster->inv[MSLOT_WAND]] = NON_ITEM;

        if ( monsterNearby )
        {
            strcpy(info, monam(monster->number, monster->type, monster->enchantment[2], 0));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_WAND], 3, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

      case OBJ_SCROLLS:
        if ( monster->inv[MSLOT_SCROLL] != NON_ITEM )
          return false;

        monster->inv[MSLOT_SCROLL] = igrd[monster->x][monster->y];
        igrd[monster->x][monster->y] = mitm.link[igrd[monster->x][monster->y]];
        mitm.link[monster->inv[MSLOT_SCROLL]] = NON_ITEM;

        if ( monsterNearby )
        {
            strcpy(info, monam(monster->number, monster->type, monster->enchantment[2], 0));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_SCROLL], 3, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

      case OBJ_POTIONS:
        if ( monster->inv[MSLOT_POTION] != NON_ITEM )
          return false;

        monster->inv[MSLOT_POTION] = igrd[monster->x][monster->y];
        igrd[monster->x][monster->y] = mitm.link[igrd[monster->x][monster->y]];
        mitm.link[monster->inv[MSLOT_POTION]] = NON_ITEM;

        if ( monsterNearby )
        {
            strcpy(info, monam(monster->number, monster->type, monster->enchantment[2], 0));
            strcat(info, " picks up ");
            it_name(monster->inv[MSLOT_POTION], 3, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }
        break;

      case OBJ_CORPSES:
        if ( monster->type != MONS_NECROPHAGE && monster->type != MONS_GHOUL )
          return false;

        monster->hit_points += 1 + random2(mons_weight(mitm.pluses[igrd[monster->x][monster->y]])) / 100;

    // limited growth factor here -- should 77 really be the cap? {dlb}:
        if ( monster->hit_points > 77 )
          monster->hit_points = 77;
        if ( monster->hit_points > monster->max_hit_points )
          monster->max_hit_points = monster->hit_points;

        if ( monsterNearby )
        {
            strcpy(info, monam(monster->number, monster->type, monster->enchantment[2], 0));
            strcat(info, " eats ");
            it_name(igrd[monster->x][monster->y], 1, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");
            mpr(info);
        }

        destroy_item(igrd[monster->x][monster->y]);
        break;

      default:
        return false;

    }

    //if (monster->speed_increment > 25)
    //  monster->speed_increment -= monster->speed;

    return true;

}          // end mons_pickup()




static void monster_move( struct monsters *monster )
{

    bool which_first = coinflip();
    FixedArray<bool, 3, 3> good_move;
    int count_x, count_y, vacated_x, vacated_y;
    int okmove = DNGN_SHALLOW_WATER;

// effectively slows down monster movement across water.
// Fire elementals can't cross at all.
    if ( monster->type == MONS_FIRE_ELEMENTAL || one_chance_in(3) )
      okmove = DNGN_WATER_STUCK;

    if ( mons_flies(monster->type) > 0 || monster_habitat(monster->type) != DNGN_FLOOR )
      okmove = MINMOVE;

    for (count_x = 0; count_x < 3; count_x++)
      for (count_y = 0; count_y < 3; count_y++)
      {
          good_move[count_x][count_y] = true;

          if ( monster->type == MONS_BORING_BEETLE )
          {
              if (grd[monster->x + count_x - 1][monster->y + count_y - 1] == DNGN_ROCK_WALL && monster->x + count_x - 1 > 7 && monster->x + count_x - 1 < (GXM - 8) && monster->y + count_y - 1 > 7 && monster->y + count_y - 1 < (GYM - 8))
                goto tunnel;
          }

          if ( grd[monster->x + count_x - 1][monster->y + count_y - 1] < okmove )
          {
              good_move[count_x][count_y] = false;
              continue;
          }

tunnel:
          if (mgrd[monster->x + count_x - 1][monster->y + count_y - 1] != NON_MONSTER)
          {
              if (!(menv[mgrd[monster->x + count_x - 1][monster->y + count_y - 1]].behavior == BEH_ENSLAVED || monster->behavior == BEH_ENSLAVED && menv[mgrd[monster->x + count_x - 1][monster->y + count_y - 1]].behavior != monster->behavior))
              {
                  good_move[count_x][count_y] = false;
                  continue;
              }
          }

          if (env.cgrid[monster->x + count_x - 1][monster->y + count_y - 1] != EMPTY_CLOUD)
          {
              if ( env.cgrid[monster->x][monster->y] != EMPTY_CLOUD && env.cloud_type[env.cgrid[monster->x + count_x - 1][monster->y + count_y - 1]] == env.cloud_type[env.cgrid[monster->x][monster->y]] )
                continue;

              if ( mons_intel(monster->type) < I_ANIMAL )
                continue;

              switch ( env.cloud_type[env.cgrid[monster->x + count_x - 1][monster->y + count_y - 1]] )
              {
                case CLOUD_FIRE:
                case CLOUD_FIRE_MON:
                  if (mons_res_fire(monster->type) > 0 || (monster->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE))
                    continue;
                  if ( monster->hit_points >= 15 + random2avg(46,5) )
                    continue;
                  break;
                case CLOUD_STINK:
                case CLOUD_STINK_MON:
                  if ( mons_res_poison(monster->type) > 0 )
                    continue;
                  if ( 1 + random2(5) < monster->hit_dice )
                    continue;
                  if ( monster->hit_points >= random2avg(19,2) )
                    continue;
                  break;
                case CLOUD_COLD:
                case CLOUD_COLD_MON:
                  if ( mons_res_cold(monster->type) > 0 || (monster->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_COLD_RESISTANCE) )
                    continue;
                  if ( monster->hit_points >= 15 + random2avg(46,5) )
                    continue;
                  break;
                case CLOUD_POISON:
                case CLOUD_POISON_MON:
                  if ( mons_res_poison(monster->type) > 0 )
                    continue;
                  if ( monster->hit_points >= random2avg(37,4) )
                    continue;
                  break;
                case CLOUD_GREY_SMOKE:          // why is this classed as a harmful cloud? {dlb}
                case CLOUD_GREY_SMOKE_MON:
                  if (mons_res_fire(monster->type) > 0 || (monster->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE))
                    continue;
                  if ( monster->hit_points >= random2avg(19,2) )
                    continue;
                  break;
                default:
                  continue;   // harmless clouds
              }

              good_move[count_x][count_y] = false;
          }
      }

// this is a cheat, as all monsters know of secret doors
// even dumb ones like jellies <explicitly included below> {dlb}
    if (grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_CLOSED_DOOR || grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_SECRET_DOOR)
    {
        if (monster->type == MONS_ZOMBIE_SMALL || monster->type == MONS_ZOMBIE_LARGE || monster->type == MONS_SKELETON_SMALL || monster->type == MONS_SKELETON_LARGE || monster->type == MONS_SPECTRAL_THING)
        {
            if ( mons_itemuse(monster->type) > 0)    // this is most certainly wrong {dlb}
            {
                grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_OPEN_DOOR;
                return;
            }
        }
        else if ( mons_itemuse(monster->type) > 0 )
        {
            grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_OPEN_DOOR;
            return;
        }

    // this should also be tweaked to include open doors, too
        if ( monster->type == MONS_JELLY
              || monster->type == MONS_BROWN_OOZE
              || monster->type == MONS_ACID_BLOB
              || monster->type == MONS_ROYAL_JELLY )
        {
            grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_FLOOR;

            if ( !silenced(you.x_pos, you.y_pos) && !silenced(monster->x, monster->y) )
            {
                strcpy(info, "You hear a");
                if ( !mons_near(monster) )
                  strcat(info, " distant");
                strcat(info, " slurping noise.");
                mpr(info);
            }

            monster->hit_points += 5;

        // note here, that this makes jellies "grow" {dlb}:
            if ( monster->hit_points > monster->max_hit_points )
              monster->max_hit_points = monster->hit_points;

            if ( monster->hit_points > 50 )
              if ( !jelly_divide(monster) )    // and here is where the jelly might divide {dlb}
                monster->hit_points = 50;
        }
    }


/*  equivalent of your move() for monsters: */
    if ( mmov_x != 0 && mmov_y != 0 )
    {
    // Now, we want to make the monster move in a straight line unless an
    //  oblique line is faster (often, either is optimum above a certain
    //  distance from the target). But should be a little random
        if ( mmov_x != 0 && mmov_y != 0    // seems a bit redundant of the above {dlb}
            && abs(monster->y - monster->target_y) != abs(monster->x - monster->target_x)
            && !one_chance_in(4) )
        {
            if ( abs(monster->target_x - monster->x) > abs(monster->target_y - monster->y) )
            {
                if ( good_move[mmov_x + 1][1] )
                  mmov_y = 0;
            }
            else if ( good_move[1][mmov_y + 1] )
              mmov_x = 0;
        }

        if ( !(good_move[mmov_x + 1][mmov_y + 1]) )
        {
// now why on earth would this all have to be repeated? {dlb}
// what about jellies down here? {dlb}
            /* some monsters opening doors */
            if ( mons_itemuse(monster->type) > 0 && (grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_CLOSED_DOOR || grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_SECRET_DOOR))
            {
                if (monster->type == MONS_ZOMBIE_SMALL || monster->type == MONS_ZOMBIE_LARGE || monster->type == MONS_SKELETON_SMALL || monster->type == MONS_SKELETON_LARGE || monster->type == MONS_SPECTRAL_THING)
                {
                    if ( mons_itemuse(monster->type) > 0 )    // see above for comments {dlb}
                    {
                        grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_OPEN_DOOR;
                        return;
                    }
                }
                else
                {
                    grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_OPEN_DOOR;
                    return;
                }
            }

            if ( which_first )
            {
                if ( good_move[mmov_x + 1][1] )
                  mmov_y = 0;
                else if ( good_move[1][mmov_y + 1] )
                  mmov_x = 0;
            }
            else
            {
                if ( good_move[1][mmov_y + 1] )
                  mmov_x = 0;
                else if ( good_move[mmov_x + 1][1] )
                  mmov_y = 0;
            }

            if ( mmov_x != 0 && mmov_y != 0 )
            {
                mmov_x = random2(3) - 1;
                mmov_y = random2(3) - 1;
            }

        }
    }
    else
        /* end of: if (mmov_x !=0 etc */
    {
        if (mmov_x != 0 ^ mmov_y != 0)  /* ^ = XOR, || = OR(inc) */
        {
            if ( mmov_x != 0 && !(good_move[mmov_x + 1][1]) )
            {
                if ( coinflip() )
                {
                    if ( good_move[1][0] )
                    {
                        mmov_y = -1;
                        mmov_x = 0;
                    }
                    else
                    {
                        if ( good_move[1][2] )
                        {
                            mmov_y = 1;
                            mmov_x = 0;
                        }
                    }
                }
                else
                {
                    if ( good_move[1][2] )
                    {
                        mmov_y = 1;
                        mmov_x = 0;
                    }
                    else
                    {
                        if ( good_move[1][0] )
                        {
                            mmov_y = -1;
                            mmov_x = 0;
                        }
                    }
                }
            }
            else if ( mmov_y != 0 && !(good_move[1][mmov_y + 1]) )
            {
                if ( coinflip() )
                {
                    if ( good_move[0][1] )
                    {
                        mmov_x = -1;
                        mmov_y = 0;
                    }
                    else if ( good_move[2][1] )
                    {
                        mmov_x = 1;
                        mmov_y = 0;
                    }
                }
                else
                {
                    if ( good_move[2][1] )
                    {
                        mmov_x = 1;
                        mmov_y = 0;
                    }
                    else if ( good_move[0][1] )
                    {
                        mmov_x = -1;
                        mmov_y = 0;
                    }
                }
            }

            if (mmov_x != 0 && mmov_y != 0)
            {
                if ( good_move[mmov_x + 1][mmov_y + 1] )
                {
                    mmov_y = random2(3) - 1;
                    mmov_x = random2(3) - 1;
                }
            }

        }                       // end of if (mmov_x != 0 ^ etc for the 2nd time

    }                           // end of else above


/* maybe: (makes worms stupid, though) */
    if ( monster->type == MONS_BORING_BEETLE && mmov_x != 0 && mmov_y != 0)
    {
        if ( coinflip() )
          mmov_x = 0;
        else
          mmov_y = 0;

        if (grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_STONE_WALL && monster->x + mmov_x > 6 && monster->x + mmov_x < (GXM - 7) && monster->y + mmov_y > 6 && monster->y + mmov_y < (GYM - 7))
        {
            grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_FLOOR;

            if ( !silenced(you.x_pos, you.y_pos) )
              mpr("You hear a grinding noise.");
        }
    }

    unsigned char grik = grd[monster->x + mmov_x][monster->y + mmov_y];

    mgrd[monster->x][monster->y] = NON_MONSTER;

    if (grik >= okmove && mgrd[monster->x + mmov_x][monster->y + mmov_y] == NON_MONSTER)
    {
        if (monster->x + mmov_x == you.x_pos && monster->y + mmov_y == you.y_pos)
        {
            monster_attack(monster_index(monster));
            mmov_x = 0;
            mmov_y = 0;
        }

        vacated_x = monster->x;
        vacated_y = monster->y;

        if ( monster_habitat(monster->type) == DNGN_LAVA && grik != DNGN_LAVA)
        {
            mmov_x = 0;
            mmov_y = 0;
        }

        if ( monster_habitat(monster->type) == DNGN_DEEP_WATER
            && grik != DNGN_DEEP_WATER
            && grik != DNGN_SHALLOW_WATER
            && grik != DNGN_BLUE_FOUNTAIN)
        {
            mmov_x = 0;
            mmov_y = 0;
        }

        if ( monster->type == MONS_WANDERING_MUSHROOM
            && see_grid(monster->x + mmov_x, monster->y + mmov_y) )
        {
            mmov_x = 0;
            mmov_y = 0;
        }

        if ( monster->type == MONS_EFREET || monster->type == MONS_FIRE_ELEMENTAL )
          place_cloud(CLOUD_FIRE_MON, monster->x, monster->y, 2 + random2(4));

        /* this appears to be the real one, ie where the movement occurs: */
        monster->x += mmov_x;
        monster->y += mmov_y;
    }
    else
    {

    }

/* need to put in something so that monster picks up multiple items (eg ammunition) identical to those it's carrying. */
    mgrd[monster->x][monster->y] = monster_index(monster);

// monsters stepping on traps:
    if ( grd[monster->x][monster->y] >= DNGN_TRAP_MECHANICAL && grd[monster->x][monster->y] <= DNGN_UNDISCOVERED_TRAP
        && ( mmov_x != 0 || mmov_y != 0 ) )
      mons_trap(monster);

}          // end monster_move()




// I suppose some day this will have to be rewritten
// so plants can spit at other monsters, too {dlb}
static void plant_spit( struct monsters *monster, struct bolt *pbolt )
{

    if ( mons_near(monster) )
    {
        simple_monster_message(monster, " spits at you.");

        pbolt->move_x = pbolt->trac_targ_x - monster->x;
        pbolt->move_y = pbolt->trac_targ_y - monster->y;

        strcpy(pbolt->beam_name, "plant spit");

        pbolt->type = SYM_ZAP;
        pbolt->source_x = monster->x;
        pbolt->source_y = monster->y;
        pbolt->range = 10;
        pbolt->colour = YELLOW;
        pbolt->flavour = BEAM_ACID;
        pbolt->beam_source = monster_index(monster);
        pbolt->damage = 107;
        pbolt->hit = 20 + (3 * monster->hit_dice);
        pbolt->thing_thrown = KILL_MON_MISSILE;

        missile(pbolt, 0);
    }

    return;

}          // end plant_spit()




static void mons_in_cloud( struct monsters *monster )
{

    int wc = env.cgrid[monster->x][monster->y];
    int hurted = 0;
    struct bolt beam[1];

    if ( mons_category(monster->type) == MC_MIMIC )
    {
        mimic_alert(monster);
        return;
    }

    switch ( env.cloud_type[wc] )
    {
      case CLOUD_DEBUGGING:
        cprintf("Fatal error: monster steps on nonexistant cloud!");
        exit(0);
        return;


      case CLOUD_FIRE:
      case CLOUD_FIRE_MON:
        if ( monster->type == MONS_FIRE_VORTEX
              || monster->type == MONS_EFREET
              || monster->type == MONS_FIRE_ELEMENTAL )
          return;

        simple_monster_message(monster, " is engulfed in flame!");

        if ( mons_res_fire(monster->type) > 0 )
          return;

        hurted += ( ( random2avg(16,3) + 6 ) * 10 ) / monster->speed;

        if ( mons_res_fire(monster->type) < 0
            && !( monster->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE ) )
          hurted += ( random2(15) * 10 ) / monster->speed;

        // remember that the above is in addition to the other you.damage.

        hurted -= random2(1 + monster->armor_class);

        if ( monster->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE )
          hurted /= 3;

        break;     // to damage routine at end {dlb}


      case CLOUD_STINK:
      case CLOUD_STINK_MON:
        simple_monster_message(monster, " is engulfed in noxious gasses!");

        if ( mons_res_poison(monster->type) > 0 )
          return;

        beam[0].colour = RED;

        if ( 1 + random2(27) >= monster->hit_dice )
          mons_ench_f2(monster, &beam[0]);

        hurted += ( random2(3) * 10 ) / monster->speed;

        break;     // to damage routine at end {dlb}


      case CLOUD_COLD:
      case CLOUD_COLD_MON:
        simple_monster_message(monster, " is engulfed in freezing vapours!");

        if ( mons_res_cold(monster->type) > 0 )
          return;

        hurted += ( (6 + random2avg(16,3)) * 10 ) / monster->speed;

        if ( mons_res_cold(monster->type) < 0
            && !( monster->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_COLD_RESISTANCE ) )
          hurted += (random2(15) * 10) / monster->speed;

        // remember that the above is in addition to the other damage.

        hurted -= random2(1 + monster->armor_class);

        if ( monster->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_COLD_RESISTANCE )
          hurted /= 3;
        break;     // to damage routine at end {dlb}


// what of armour of poison resistance here? {dlb}
      case CLOUD_POISON:
      case CLOUD_POISON_MON:
        simple_monster_message(monster, " is engulfed in a cloud of poison!");

        if ( mons_res_poison(monster->type) > 0 )
          return;

        poison_monster(monster, (env.cloud_type[wc] == CLOUD_POISON));

        hurted += ( random2(8) * 10 ) / monster->speed;

        if ( mons_res_poison(monster->type) < 0 )
          hurted += ( random2(4) * 10 ) / monster->speed;

        break;     // to damage routine at end {dlb}


      case CLOUD_STEAM:
      case CLOUD_STEAM_MON:     // couldn't be bothered coding for armour of res fire
        if ( monster->type == MONS_STEAM_DRAGON )    // what of whether it is wearing steam dragon armour? {dlb}
          return;

        simple_monster_message(monster, " is engulfed in steam!");

        if ( mons_res_fire(monster->type) > 0
              || ( monster->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE ) )
          return;

        hurted += (random2(6) * 10) / monster->speed;

        if ( mons_res_fire(monster->type ) < 0
            && !( monster->inv[MSLOT_ARMOUR] != NON_ITEM && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 == SPARM_FIRE_RESISTANCE ) )
          hurted += (random2(6) * 10) / monster->speed;

        hurted -= random2(1 + monster->armor_class);

        break;     // to damage routine at end {dlb}


      case CLOUD_MIASMA:
      case CLOUD_MIASMA_MON:
        simple_monster_message(monster, " is engulfed in a dark miasma!");

        if ( mons_holiness(monster->type) != MH_NATURAL )
          return;

        poison_monster(monster, (env.cloud_type[wc] == CLOUD_MIASMA));

        if ( monster->max_hit_points > 4 && coinflip() )
          monster->max_hit_points--;

        beam[0].colour = BLACK;

        if ( one_chance_in(3) )
          mons_ench_f2(monster, &beam[0]);

        hurted += (10 * random2avg(12,3)) / monster->speed;        // 3

        break;     // to damage routine at end {dlb}

      default:          // 'harmless' clouds -- colored smoke, etc {dlb}.
        return;
    }


    if ( hurted < 0 )
      hurted = 0;
    else if ( hurted > 0 )
      {
         hurt_monster(monster, hurted);

         if (monster->hit_points < 1)
           {

              switch ( env.cloud_type[wc] )
              {
                case CLOUD_FIRE_MON:
                case CLOUD_STINK_MON:
                case CLOUD_COLD_MON:
                case CLOUD_POISON_MON:
                case CLOUD_STEAM_MON:
                case CLOUD_MIASMA_MON:
                  monster_die(monster, KILL_MISC, 0);
                default:
                  monster_die(monster, KILL_YOU, 0);
              }

              switch ( env.cloud_type[wc] )
              {
                case CLOUD_FIRE:
                case CLOUD_FIRE_MON:
                case CLOUD_COLD:
                case CLOUD_COLD_MON:
                case CLOUD_STEAM:
                case CLOUD_STEAM_MON:
                  monster->speed_increment = 1;
              }

            }
      }

}          // end mons_in_cloud()


static bool mons_speaks( struct monsters *monster ) //returns true if something is said
{
    int temp_rand; //probability determination

    strcpy(info, monam(monster->number, monster->type, monster->enchantment[2], 0));

    if ( monster->enchantment[2] == ENCH_INVIS ) return false;
         // invisible monster tries to remain unnoticed

    if ( silenced(monster->x, monster->y) ) //mv: if it's also invisible, program never gets here
        {
        if ( !one_chance_in(3) ) return false; // while silenced, don't bother so often
        switch ( monster->behavior )
         {
         BEH_FLEE:
         BEH_FLEE_FRIEND:
                 temp_rand = random2(10);
                 strcat(info, (temp_rand < 3) ? " glances furtively about." :
                     (temp_rand == 3) ? " opens its mouth, as if shouting." :
                     (temp_rand == 4) ? " looks around." :
                     (temp_rand == 5) ? " appears indicisive." :
                     (temp_rand == 6) ? " ponders situation." :
                     " seems to says something." );
                 break;
         BEH_ENSLAVED:
                 temp_rand = random2(10);
                 strcat(info, (temp_rand < 4) ? " gives you a thumbs up." :
                     (temp_rand == 4) ? " looks at you." :
                     (temp_rand == 5) ? " waves at you." :
                     (temp_rand == 6) ? " smiles happily." :
                     " says something but you don't hear anything." );
                 break;
         BEH_CONFUSED:
         BEH_CONFUSED_FRIEND:
         MONS_INSANE:
                 temp_rand = random2(10);
                 strcat(info, (temp_rand < 4) ? " wildly gestures." :
                     (temp_rand == 4) ? " looks confused." :
                     (temp_rand == 5) ? " grins evily." :
                     (temp_rand == 6) ? " smiles happily." :
                     (temp_rand == 7) ? " cries." :
                     " says something but you don't hear anything." );
                 break;
         default:
           temp_rand = random2(10);
           strcat(info, (temp_rand < 3) ? " gestures." :
                  (temp_rand == 3) ? " gestures obscenely." :
                  (temp_rand == 4) ? " grins." :
                  (temp_rand == 5) ? " looks angry." :
                  (temp_rand == 6) ? " seems to be listening." :
                  " says something but you don't hear anything." );
           break;
         } //end switch silenced monster's behaviour
        mpr (info);
        return true;
        } // end silenced monster

    switch ( monster->behavior )
      {
      case BEH_FLEE:
        {
          switch (random2(14)) // speaks for unfriendly fleeing monsters
            {
            case 0:
              sprintf(info, " %s \"Help!\"",
                      coinflip() ? "yells" : "wails"); break;
            case 1:
              sprintf(info, " %s \"Help!\"",
                      coinflip() ? "crys" : "screams"); break;
            case 2:
              sprintf(info, " %s \"Why can't we all just get along?\"",
                      coinflip() ? "begs" : "pleads");
              break;
            case 3:
              sprintf(info, " %s trips in trying to escape.",
                      coinflip() ? "nearly" : "almost");
                 break;
               case 4:
         sprintf(info, " %s, \"Of all the rotten luck!\"",
                coinflip() ? "mutters" : "mumbles");
                break;
               case 5:
         sprintf(info, " %s, \"Oh dear! Oh dear!\"",
                coinflip() ? "moans" : "wails");
               case 6:
                 sprintf(info, " %s, \"Damn and blast!\"",
                         coinflip() ? "mutters" : "mumbles");
                 break;
               case 7:
                    strcat(info, " prays for help."); break;
               case 8:
                    strcat(info, " shouts \"No! I'll never do that again!\"");
                    break;
               case 9:
                    sprintf(info, " %s \"Mercy!\"",
                            coinflip() ? "begs for" : "cries");
                    break;
               case 10:
                 sprintf(info, " %s \"%s!\"",
                         coinflip() ? "blubbers" : "cries",
                         coinflip() ? "Mommeee" : "Daddeee");
                 break;
               case 11:
                 sprintf(info, " %s \"Please don't kill me!\"",
                         coinflip() ? "begs" : "pleads");
                 break;
               case 12:
                 sprintf(info, " %s \"Please don't hurt me!\"",
                         coinflip() ? "begs" : "pleads");
                 break;
               case 13:
                 sprintf(info, " %s \"Please, I have a lot of children...\"",
                         coinflip() ? "begs" : "pleads");
                 break;
                    }
       } break; // end BEH_FLEE
     case BEH_FLEE_FRIEND: // speaks for friendly fleeing monsters
       {
        switch (random2(11))
               {
               case 0:
                    sprintf(info, " %s \"WAIT FOR ME!\"",
                            coinflip() ? "shouts" : "yells");
                    strcat(info, you.your_name);
                    strcat(info,", could you help me?\""); break;
               case 1:
                    strcat(info, " screams \"Help!\""); break;
               case 2:
                    strcat(info, " shouts \"Cover me!\""); break;
               case 3:
                    strcat(info, " screams \"");
                    strcat(info, you.your_name);
                    strcat(info,"! Help me!\""); break;
               case 4:
               case 5:
               case 6:
                    strcat(info, " tries to hide somewhere."); break;
               case 7:
                    strcat(info, " prays for help."); break;
               case 8:
                    strcat(info, " looks at you beseechingly."); break;
               case 9:
                    strcat(info, " shouts \"Protect me!\""); break;
               case 10:
                    strcat(info, " cries \"Don't forget your friends!\"");
                    break;
                    }
       } break;//end BEH_FLEE_FRIEND

     case BEH_ENSLAVED: // speaks for charmed monsters
       {
       if ( ( monster->type == MONS_IMP ) && ( random2(10) ) ) return false;
          // friendly imps are too common so they speak very very rarely
       switch (random2(16))
              {
               case 0:
                    strcat(info, " yells \"Run! I'll cover you!\""); break;
               case 1:
                    strcat(info, " shouts \"Die, monster!\""); break;
               case 2:
                    strcat(info, " says \"It's nice to have friends.\"");
                    break;
               case 3:
               case 4:
               case 5:
               case 6:
               case 7:
               case 8:
                    strcat(info, " looks at you."); break;
               case 9:
                    strcat(info, " smiles at you."); break;
               case 10:
                    strcat(info, " says \"");
                    strcat(info, you.your_name);
                    strcat(info,", you are my only friend.\""); break;
               case 11:
                    strcat(info, " says \"");
                    strcat(info, you.your_name);
                    strcat(info,", I like you.\""); break;

               case 12:
                    strcat(info, " waves at you."); break;
               case 13:
                    strcat(info, " says \"Be careful!\""); break;
               case 14:
                    strcat(info, " says \"Don't worry. I'm here with you.\"");
                    break;
               case 15:
                    strcat(info, " smiles happily."); break;
               case 16:
                    strcat(info, " shouts \"No mercy! Kill them all!"); break;
               case 17:
                    strcat(info, " winks at you."); break;
               case 18:
                    strcat(info, " says \"Me and you. It sounds cool.\"");
                    break;
               case 19:
                    strcat(info, " says \"I'll never leave you.\""); break;
               case 20:
                    strcat(info, " says \"I would die for you.\""); break;
               case 21:
                    strcat(info, " shouts \"Beware of monsters!\""); break;
               case 22:
               case 23:
               case 24:
               case 25:
               case 26:
               case 27:
                    strcat(info, " looks friendly."); break;
               }
       } break;//end BEH_ENSLAVED

     case BEH_CONFUSED:
       {
       switch (random2(15)) // speaks for unfriendly confused monsters
               {
               case 0:
                    strcat(info, " yells \"Get them off of me!\""); break;
               case 1:
                    strcat(info, " screams \"I kill you anyway!\""); break;
               case 2:
                    strcat(info, " shouts \"What's happening?\""); break;
               case 3:
               case 4:
               case 5:
                    strcat(info, " wildly gestures."); break;
               case 6:
                    strcat(info, " cries."); break;
               case 7:
                    strcat(info, " shouts \"NO!\""); break;
               case 8:
                    strcat(info, " shouts \"YES!\""); break;
               case 9:
                    strcat(info, " laughs crazily."); break;
               case 10:
                    strcat(info, " ponders situation."); break;
               case 11:
                    strcat(info, " grins madly."); break;
               case 12:
                    strcat(info, " looks very confused."); break;
               case 13:
                    strcat(info, " mumbles something."); break;
               case 14:
                    strcat(info, " says \"I'm little bit confused.\""); break;
               }
       } break; // end BEH_CONFUSED

     case BEH_CONFUSED_FRIEND:
       {
       switch (random2(18)) // speaks for friendly confused monsters
               {
               case 0:
                    strcat(info, " prays for help."); break;
               case 1:
                    strcat(info, " screams \" Help!\""); break;
               case 2:
                    strcat(info, " shouts \"I'm losing control!\""); break;
               case 3:
                    strcat(info, " shouts \"What's happening?\""); break;
               case 4:
               case 5:
                    strcat(info, " wildly gestures."); break;
               case 6:
                    strcat(info, " cries."); break;
               case 7:
                    strcat(info, " shouts \"Yeah!\""); break;
               case 8:
                    strcat(info, " sings."); break;
               case 9:
                    strcat(info, " laughs crazily."); break;
               case 10:
                    strcat(info, " ponders situation."); break;
               case 11:
                    strcat(info, " grins madly."); break;
               case 12:
                    strcat(info, " looks very confused."); break;
               case 13:
                    strcat(info, " mumbles something."); break;
               case 14:
                    strcat(info, " giggles crazily."); break;
               case 15:
                    strcat(info, " screams \"");
                    strcat(info, you.your_name);
                    strcat(info,"! Help!\""); break;
               case 16:
                    strcat(info, " screams \"");
                    strcat(info, you.your_name);
                    strcat(info,"! What's going on?\""); break;
               case 17:
                    strcat(info, " says \"");
                    strcat(info, you.your_name);
                    strcat(info,", I'm little bit confused.\""); break;
                    }
       } break; // end BEH_CONFUSED_FRIEND

    default: // normal monster shouts
     {
        switch ( monster->type )
        {

           case MONS_TERENCE: // fighter who likes to kill
             switch (random2(9))
             {
               case 0:
                 strcat(info, " screams \"I'm going to kill you ! \""); break;
               case 1:
                 strcat(info, " shouts \"Now you die.\""); break;
               case 2:
                 strcat(info, " says \"Rest in peace.\""); break;
               case 3:
                 strcat(info, " screams \"Die !\""); break;
               case 4:
                 strcat(info, " says \"How do you enjoy it ?\""); break;
               case 5:
                 strcat(info, " shouts \"Get ready for death !\""); break;
               case 6:
                 strcat(info, " says \"You are history.\""); break;
               case 7:
                 strcat(info, " says \"Do you want it fast or slow ?.\""); break;
               case 8:
                 strcat(info, " says \"Did you write testament ? You should...\""); break;
             } break; // end Terence

             case MONS_EDMUND: // mercenaries guarding dungeon
             case MONS_LOUISE:
             case MONS_FRANCES:
             case MONS_DUANE:
             case MONS_ADOLF:
             switch (random2(15))
             {
               case 0:
                 strcat(info, " screams \"I'm going to kill you! Now!\""); break;
               case 1:
                 strcat(info, " shouts \"Return immediately or I'll kill you!\""); break;
               case 2:
                 strcat(info, " says \"Now you've reached the end of your way.\""); break;
               case 3:
                 strcat(info, " screams \"One false step and I'll kill you!\""); break;
               case 4:
                 strcat(info, " says \"Drop everything you've found here and go home.\""); break;
               case 5:
                 strcat(info, " shouts \"You will never get the Orb.\""); break;
               case 6:
                 strcat(info, " looks very unfriendly."); break;
               case 7:
                 strcat(info, " looks very cold."); break;
               case 8:
                 strcat(info, " shouts \"It's the end of the party!\""); break;
               case 9:
                 strcat(info, " says \"Return every stolen item!\""); break;
               case 10:
                 strcat(info, " says \"No trespassers are allowed here.\""); break;
               case 11:
                 strcat(info, " grins evilly."); break;
               case 12:
                 strcat(info, " screams \"You must be punished!\""); break;
               case 13:
                 strcat(info, " says \"It's nothing personal...\""); break;
               case 14:
                 strcat(info, " says \"Dead adventured is good adventurer.\""); break;
             } break; // end Edmund & Co

             case MONS_JOSEPH:
             switch (random2(16))
             {
               case 0:
                 strcat(info, " smiles happily."); break;
               case 1:
                 strcat(info, " says \"I'm happy to see you. And I'll be happy to kill you.\""); break;
               case 2:
                 strcat(info, " says \"I've wait for this moment for such a long time.\""); break;
               case 3:
                 strcat(info, " says \"It's nothing personal but I kill you.\""); break;
               case 5:
                 strcat(info, " says \"You will never get the Orb, sorry.\""); break;
               case 9:
                 strcat(info, " shouts \"I love fight! I love killing!\""); break;
               case 10:
                 strcat(info, " says \"I'm here to kill trespassers. I like my job.\""); break;
               case 11:
                 strcat(info, " tries to grin evilly."); break;
               case 12:
                 strcat(info, " says \"You must be punished! Or... I want to punish you!\""); break;
               case 13:
                 strcat(info, " sighs \"Being guard is usually so boring...\""); break;
               case 14:
                 strcat(info, " shouts \"At last some action!\""); break;
               case 15:
                 strcat(info, " shouts \"Wow!\""); break;
             } break; // end Joseph

             case MONS_ORC_HIGH_PRIEST: // priest, servants of dark ancient God
             case MONS_DEEP_ELF_HIGH_PRIEST:
             switch (random2(10))
             {
               case 0:
                 strcat(info, " prays."); break;
               case 1:
                 strcat(info, " says \"My Lord BOG, aid me.\""); break;
               case 2:
                 strcat(info, " mumbles some strange prayers."); break;
               case 3:
                 strcat(info, " shouts \"You are heretic. And you must be destroyed.\""); break;
               case 4:
                 strcat(info, " shouts \"Confess!\""); break;
               case 5:
                 strcat(info, " says \"All sinners must die.\""); break;
               case 6:
                 strcat(info, " looks excited."); break;
               case 7:
                 strcat(info, " says \"You will be sacrificed.\""); break;
               case 8:
                 strcat(info, " starts to sing some prayer."); break;
               case 9:
                 strcat(info, " shouts \"You must be punished.\""); break;
             } break; // end priests

             case MONS_ORC_SORCEROR: // hateful wizards, using strange powers
             case MONS_WIZARD:
             switch (random2(18))
             {
               case 0:
               case 1:
               case 2:
                 strcat(info, " wildly gestures."); break;
               case 3:
               case 4:
               case 5:
                 strcat(info, " mumbles some strange words."); break;
               case 6:
                 strcat(info, " shouts \"You can't face my power !\""); break;
               case 7:
                 strcat(info, " shouts \"You are history.\""); break;
               case 8:
                 strcat(info, " becomes transparent for a moment."); break;
               case 9:
                 strcat(info, " throws some strange powder towards you."); break;
               case 10:
                 strcat(info, " glows brightly for a moment."); break;
               case 11:
                 strcat(info, " says \"argatax netranoch dertex\""); break;
               case 12:
                 strcat(info, " says \"dogrw nutew berg\""); break;
               case 13:
                 strcat(info, " shouts \"Entram moth deg ulag !\""); break;
               case 14:
                 strcat(info, " becomes larger for a moment."); break;
               case 15:
                 strcat(info, "'s fingertips starts to glow."); break;
               case 16:
                 strcat(info, "'s eyes starts to glow."); break;
               case 17:
                 strcat(info, " tries to paralyze you with his gaze."); break;
             } break; // end wizards

             case MONS_JESSICA: // sorceress disturbed by player
             switch (random2(10))
             {
               case 0:
                 strcat(info, " grins evilly."); break;
               case 1:
                 strcat(info, " says \"I'm really upset.\""); break;
               case 2:
                 strcat(info, " shouts \"I don't like beings like you.\""); break;
               case 3:
                 strcat(info, " shouts \"Stop bothering me, or I'll kill you !\""); break;
               case 4:
                 strcat(info, " very coldly says \"I hate your company.\""); break;
               case 5:
                 strcat(info, " mumbles something strange."); break;
               case 6:
                 strcat(info, " looks very angry."); break;
               case 7:
                 strcat(info, " shouts \"You disturbed me so I kill you.\""); break;
               case 8:
                 strcat(info, " screams \"You are ghastly nuisance !\""); break;
               case 9:
                 strcat(info, " gestures wildly."); break;
             } break; // end Jessica

             case MONS_SIGMUND: // mad old wizard
             switch (random2(17))
             {
               case 0:
               case 1:
               case 2:
               strcat(info, " laughs crazily."); break;
               case 3:
                 strcat(info, " says \"Don't worry, I'll kill you fast.\""); break;
               case 4:
                 strcat(info, " grinds his teeth."); break;
               case 5:
                 strcat(info, " asks \"Do you like me ?\""); break;
               case 6:
                 strcat(info, " screams \"Die, monster !\""); break;
               case 7:
                 strcat(info, " says \"You will forget everthing soon.\""); break;
               case 8:
                 strcat(info, " screams \"You will never ... NEVER !\""); break;
               case 9:
                 strcat(info, "'s eyes starts to glow with a red light. "); break;
               case 10:
                 strcat(info, " says \"Look in to my eyes.\""); break;
               case 11:
                 strcat(info, " says \"I'm your fate.\""); break;
               case 12:
                 strcat(info, " is suddenly surrounded by pale blue light."); break;
               case 13:
                 strcat(info, " tries to bite you."); break;
               case 14:
                 strcat(info, " is suddenly surrounded by pale green light."); break;
               case 15:
                 strcat(info, " screams \"Blood ! I want some blood !\""); break;
               case 16:
                 strcat(info, " screams \"Only death can liberate you!\""); break;
             } break; // end Sigmund

             case MONS_IMP: // small demon
             if  ( coinflip () ) return false; // imps are too common to bother so often
             switch (random2(11))
             {
               case 0:
                    strcat(info, " laughs crazily."); break;
               case 1:
                    strcat(info, " grins evilly."); break;
               case 2:
               strcat(info, " breathes a bit of smoke at you."); break;
               case 3:
                 strcat(info, " lashes with his tail."); break;
               case 4:
                 strcat(info, " grinds his teeth."); break;
               case 5:
                 strcat(info, " sputters."); break;
               case 6:
                 strcat(info, " breathes some steam toward you."); break;
               case 7:
                 strcat(info, " spits at you."); break;
               case 8:
                 strcat(info, " disappears for a moment."); break;
               case 9:
                 strcat(info, " summons swarm of flyies."); break;
               case 10:
                 strcat(info, " picks up some beetle and eats it."); break;
             } break; // end imp

             case MONS_TORMENTOR: // cruel devil
             switch (random2(18))
             {
               case 0:
                    strcat(info, " laughs crazily."); break;
               case 1:
                    strcat(info, " grins evilly."); break;
               case 2:
               strcat(info, " says \"Your nightmares come true.\""); break;
               case 3:
                 strcat(info, " says \"I will show you what pain is.\""); break;
               case 4:
                 strcat(info, " shouts \"I'll tear you all apart.\""); break;
               case 5:
                 strcat(info, " says \"You will wish to die when I get you.\""); break;
               case 6:
                 strcat(info, " says \"I will drown you in you blood.\""); break;
               case 7:
                 strcat(info, " says \"You will die. Some very very ugly way.\""); break;
               case 8:
                 strcat(info, " says \"I will eat your liver.\""); break;
               case 9:
                 strcat(info, " grins madly."); break;
               case 10:
                 strcat(info, " shouts \"Prepare for my thousand needles !\""); break;
               case 11:
                 strcat(info, " says \"I know thousand and one way to kill you.\""); break;
               case 12:
                 strcat(info, " says \"I'll show you my torture chamber !\""); break;
               case 13:
                 strcat(info, " says \"I'll crush all your bones. One by one.\""); break;
               case 14:
                 strcat(info, " says \"Prepare for my thousand needles !\""); break;
               case 15:
                 strcat(info, " says \"I know your fate. It's pain.\""); break;
               case 16:
                 strcat(info, " says \"Get ready ! Throes await you.\""); break;
               case 17:
                 strcat(info, " grins malevolently."); break;
             } break; // end tormentor

             case MONS_PLAYER_GHOST: // ghost of unsuccesful player
             switch (random2(24))
             {
               case 0:
                    strcat(info, " laughs crazily."); break;
               case 1:
                    strcat(info, " grins evilly."); break;
               case 2:
               strcat(info, " shouts \"You will never get the ORB !\""); break;
               case 3: // mv: ghosts are usually wailing, aren't ?
               case 4:
               case 5:
               case 6:
               case 7:
               case 8:
               case 9:
               case 10:
               case 11:
                 strcat(info, " wails."); break;
               case 12:
                 strcat(info, " stares at you . You feel cold."); break;
               case 13:
                 strcat(info, " screams \"You will join me soon !\""); break;
               case 14:
                 strcat(info, " wails \"To die, to sleep, no more.\""); break; //Hamlet
               case 15:
                 strcat(info, " screams \"You must not succeed where I failed.\""); break;
               case 16:
                 strcat(info, " screams \"I'll kill anyone who wants the ORB.\""); break;
               case 17:
                 strcat(info, " whispers \"Meet emptiness of death !\""); break;
               case 18:
                 strcat(info, " whispers \"Death is liberation.\""); break;
               case 19:
                 strcat(info, " whispers \"Everlasting silence awaits you.\""); break;
               case 20:
                 strcat(info, " screams \"Don't try to defend. You have no chance !\""); break;
               case 21:
                 strcat(info, " whispers \"Death doesn't hurt. What you feel is life.\""); break;
               case 22:
                 strcat(info, " whispers \"The ORB doesn't exist.\""); break;
               case 23:
                 strcat(info, " wails \"Death is your only future.\""); break;
             } break; // end players ghost

             case MONS_PSYCHE: // insane girl
             switch (random2(20))
             {
               case 0:
                 strcat(info, " smiles happily."); break;
               case 1:
                 strcat(info, " giggles crazily."); break;
               case 2:
                 strcat(info, " cries."); break;
               case 3:
                 strcat(info, " stares at you for a moment."); break;
               case 4:
                 strcat(info, " sings."); break;
               case 5:
                 strcat(info, " says \"Please, could you be dying faster ?\""); break;
               case 6:
                 strcat(info, " says \"I'm bad girl. But I can't to anything about it.\""); break;
               case 7:
                 strcat(info, " screams \"YOU ARE VIOLATING AREA SECURITY!\""); break;
               case 8:
                 strcat(info, " cries \"I hate blood and violence.\""); break;
               case 9:
                 strcat(info, " screams \"Peace ! Flowers ! Freedom ! Dead adventurers !\""); break;
               case 10:
                 strcat(info, " says \"I'm so lonely. Only corpses are my friends.\""); break;
               case 11:
                 strcat(info, " cries \"You've killed my pet.\""); break;
               case 12:
                 strcat(info, " cries \"You want to stole my collection of the orbs.\""); break;
               case 13:
                 strcat(info, " sings some strange song."); break;
               case 14:
                 strcat(info, " bursts in tears."); break;
               case 15:
                 strcat(info, " sucks her thumb."); break;
               case 16:
                 strcat(info, " whispers \"Hold me, thrill me, kiss me, kill me.\""); break; //(c) U2 ?
               case 17:
                 strcat(info, " says \"I'll kill you and take you home.\""); break;
               case 18:
                 strcat(info, " shouts \"Well, maybe I'm nutty, but who cares ?\""); break;
               case 19:
                 strcat(info, " shouts \"I hope that you are sorry for that.\""); break;
             } break; // end Psyche

             case MONS_DONALD: // adventurers hating competition
             case MONS_WAYNE:
             switch (random2(11))
             {
               case 0:
                 strcat(info, " screams \"Return home !\""); break;
               case 1:
                 strcat(info, " screams \"The Orb is mine !\""); break;
               case 2:
                 strcat(info, " screams \"Give me all your treasure !\""); break;
               case 3:
                 strcat(info, " screams \"You will never get the Orb !\""); break;
               case 4:
                 strcat(info, " screams \"I was here first !\""); break;
               case 5:
                 strcat(info, " frowns.\""); break;
               case 6:
                 strcat(info, " looks very upset."); break;
               case 7:
                 strcat(info, " screams \"Get away or die !\""); break;
               case 8:
                 strcat(info, " screams \"Die !\""); break;
               case 9:
                 strcat(info, " screams \"First you have to pass me !\""); break;
               case 10:
                 strcat(info, " screams \"I hate you !\""); break;
             } break; // end Donald

             case MONS_MICHAEL: // spellcaster who wanted to be alone
             switch (random2(11))
             {
               case 0:
                 strcat(info, " looks very angry."); break;
               case 1:
                 strcat(info, " frowns."); break;
               case 2:
                 strcat(info, " screams \"I want to be alone !\""); break;
               case 3:
                 strcat(info, " says \"You are really nuisance.\""); break;
               case 4:
                 strcat(info, " screams \"I wanted to be alone. And you...\""); break;
               case 5:
                 strcat(info, " screams \"Get away ! Or better die !\""); break;
               case 6:
                 strcat(info, " mumbles some strange words."); break;
               case 7:
                 strcat(info, " points at you."); break;
               case 8:
                 strcat(info, " shakes with wrath."); break;
               case 9:
                 strcat(info, " drinks a potion."); break;
               case 10:
                 strcat(info, " gestures wildly."); break;
             } break; // end Michael

             case MONS_ERICA: // wild tempered adventuress
             switch (random2(12))
             {
               case 0:
                 strcat(info, " screams \"Die !\""); break;
               case 1:
                 strcat(info, " screams \"Do you want it fast or slow ?\""); break;
               case 2:
                 strcat(info, " looks angry."); break;
               case 3:
                 strcat(info, " drinks a potion."); break;
               case 4:
                 strcat(info, " says \"I'm much more better than you.\""); break;
               case 5:
                 strcat(info, " says \"Fast and perfect. Such is my way of killing.\""); break;
               case 6:
                 strcat(info, " screams \"Hurry ! Death awaits !\""); break;
               case 7:
                 strcat(info, " laughs wildly."); break;
               case 8:
                 strcat(info, " screams \"I'll never tell where it is.\""); break;
               case 9:
                 strcat(info, " screams \"You'll never get it.\""); break;
               case 10:
                 strcat(info, " screams \"Coming here was suicide.\""); break;
               case 11:
                 strcat(info, " says \"I love fight. I also love ... Erm, killing is better.\""); break;
             } break; // end Erica

             case MONS_JOSEPHINE: // ugly old witch looking for somone to kill
             switch (random2(13))
             {
               case 0:
               case 1:
               case 2:
                 strcat(info, " grins evilly."); break;
               case 3:
               case 4:
                 strcat(info, " screams \"I will kill you !\""); break;
               case 5:
                 strcat(info, " grinds her teeth."); break;
               case 6:
                 strcat(info, " grins malevolently."); break;
               case 7:
                 strcat(info, " laughs insanely."); break;
               case 8:
                 strcat(info, " screams \"Die !\""); break;
               case 9:
                 strcat(info, " screams \"I have something special for you !\""); break;
               case 10:
                 strcat(info, " screams \"I'll use your head as decoration in my hut.\""); break;
               case 11:
                 strcat(info, " says \"I'll make rug of your skin.\""); break;
               case 12:
                 strcat(info, " says \"What about some decapitation ?\""); break;
             } break; // end Josephine

             case MONS_HAROLD: // middle aged man, hired to kill you. He is in a hurry.
             switch (random2(10))
             {
               case 0:
                 strcat(info, " looks nervous."); break;
               case 1:
                 strcat(info, " screams \"Hurry up!\""); break;
               case 2:
                 strcat(info, " screams \"Could you die faster ?\""); break;
               case 3:
                 strcat(info, " says \"Stand still. I'd like to kill you.\""); break;
               case 4:
                 strcat(info, " screams \"Die !\""); break;
               case 5:
                 strcat(info, " says \"I hope you die soon !\""); break;
               case 6:
                 strcat(info, " says \"Only few hits and and its over.\"."); break;
               case 7:
                 strcat(info, " says \"You know, I'm in a hurry.\""); break;
               case 8:
                 strcat(info, " screams \"I'll finish you soon !\""); break;
               case 9:
                 strcat(info, " screams \"Don't delay it.\""); break;
               } break; // end Harold

             case MONS_NORBERT: // skilled warrior looking for some fame. More deads = more fame
             switch (random2(13))
             {
               case 0:
                 strcat(info, " smiles happily."); break;
               case 1:
                 strcat(info, " screams \"Die, monster !\""); break;
               case 2:
                 strcat(info, " screams \"I'm hero !\""); break;
               case 3:
                 strcat(info, " shouts \"YES! Another frag !\""); break;
               case 4:
                 strcat(info, " says \"I hope I'll get many XP for you.\""); break;
               case 5:
                 strcat(info, " screams \"Pray, because you'll die soon !\""); break;
               case 6:
                 strcat(info, " asks \" Did you write testament ? You should.\"."); break;
               case 7:
                 strcat(info, " says \"I love killing ugly monsters like you.\""); break;
               case 8:
                 strcat(info, " screams \"Blood and destruction !\""); break;
               case 9:
                 strcat(info, " says \"It's honour to die by my hand.\""); break;
               case 10:
                 strcat(info, " shouts \"Your time has come !\""); break;
               case 11:
                 strcat(info, " says \"I'm sorry but you don't have a chance.\""); break;
               case 12:
                 strcat(info, " says \"Another dead monster... It must be my lucky day !\""); break;
             } break; // end Norbert

             case MONS_JOZEF: // bounty hunter
             switch (random2(14))
             {
               case 0:
                 strcat(info, " looks satisfied."); break;
               case 1:
                 strcat(info, " screams \"Die !\""); break;
               case 2:
                 strcat(info, " screams \"At last I found you !\""); break;
               case 3:
                 strcat(info, " shouts \"I'll get 500 for your head !\""); break;
               case 4:
                 strcat(info, " says \"You don't look worth for that money.\""); break;
               case 5:
                 strcat(info, " says \"It's nothing personal. I'm paid for it !\""); break;
               case 6:
                 strcat(info, " asks \" Did you write testament ? You should.\"."); break;
               case 7:
                 strcat(info, " says \"You are ");
                 strcat(info, you.your_name);
                 strcat(info,", aren't you ?.\""); break;
               case 8:
                 strcat(info, " says \"I suppose that you are ");
                 strcat(info, you.your_name);
                 strcat(info,". Sorry, if I'm wrong.\""); break;
               case 9:
                 strcat(info, " says \"One dead ");
                 strcat(info, you.your_name);
                 strcat(info, ", 500 gold pieces. It's in my contract.\""); break;
               case 10:
                 strcat(info, " shouts \"Your time has come !\""); break;
               case 11:
                 strcat(info, " says \"My job is sometimes very exciting. Sometimes...\""); break;
               case 12:
                 strcat(info, " says \"I think, I deserve my money.\""); break;\
               case 13:
                 strcat(info, " screams \"Die ! I've got more contracts today.\""); break;
             } break; // end Jozef

             case MONS_AGNES: // she is trying to get money and treasure
             switch (random2(10))
             {
               case 0:
                 strcat(info, " screams \"Give me all your money !\""); break;
               case 1:
                 strcat(info, " screams \"All treasure is mine !\""); break;
               case 2:
                 strcat(info, " screams \"You'll never get my money !\""); break;
               case 3:
                 strcat(info, " grins evilly."); break;
               case 4:
                 strcat(info, " screams \"Give me everything and get away !\""); break;
               case 5:
                 strcat(info, " says \"I need new robe. I'll buy it for your money.\""); break;
               case 6:
                 strcat(info, " screams \"I want your rings ! And amulets ! And ... EVERYTHING !\""); break;
               case 7:
                 strcat(info, " screams \"I hate dirty adventurers like you.\""); break;
               case 8:
                 strcat(info, " says \"How can you wear so ugly dress ?\""); break;
               case 9:
                 strcat(info, " screams \"Die, beast !\""); break;
             } break; // end Agnes

             case MONS_MAUD: // warrior princess looking for sword "Entarex"
             switch (random2(10))
             {
               case 0:
                 strcat(info, " screams \" Submit or die !\""); break;
               case 1:
                 strcat(info, " screams \"Give me \" Entarex\" !\""); break;
               case 2:
                 strcat(info, " screams \"If you give me \"Entarex\" I'll not kill you!\""); break;
               case 3:
                 strcat(info, " frowns."); break;
               case 4:
                 strcat(info, " looks upset."); break;
               case 5:
                 strcat(info, " screams \"You can't face my power !\""); break;
               case 6:
                 strcat(info, " screams \"Give me that sword ! Immediately !\""); break;
               case 7:
                 strcat(info, " screams \"Your life or \"Entarex\" ! You must choose.\""); break;
               case 8:
                 strcat(info, " screams \"I want it !\""); break;
               case 9:
                 strcat(info, " screams \"Die, you thief !\""); break;
             } break; // end Maud

             case MONS_FRANCIS: // wizard looking for bodyparts as spell components
             switch (random2(15))
             {
               case 0:
                 strcat(info, " says \" You've nice eyes. I could use them.\""); break;
               case 1:
                 strcat(info, " says \"Excuse me, but I need your head.\""); break;
               case 2:
                 strcat(info, " says \"I need only few of your organs !\""); break;
               case 3:
                 strcat(info, " ponders situation."); break;
               case 4:
                 strcat(info, " looks for scalpel."); break;
               case 5:
                 strcat(info, "'s hands started to glow with soft light."); break;
               case 6:
                 strcat(info, " says \"It will not hurt.\""); break;
               case 7:
                 strcat(info, " throws something at you."); break;
               case 8:
                 strcat(info, " says \"I want you in my laboratory !\""); break;
               case 9:
                 strcat(info, " says \"What about little dissection ?\""); break;
               case 10:
                 strcat(info, " says \"I have something special for you.\""); break;
               case 11:
                 strcat(info, " screams \"Don't move ! I want to cut your ear !\""); break;
               case 12:
                 strcat(info, " says \"What about your heart ? Do you need it ?\""); break;
               case 13:
                 strcat(info, " says \"Did you know that corpses are important raw material ?\""); break;
               case 14:
                 strcat(info, " says \"Don't worry, I'll cut only few pieces.\""); break;
             } break; // end Francis

             case MONS_RUPERT: // crazy adventurer
             switch (random2(11))
             {
               case 0:
                 strcat(info, " says \" You are monster, aren't you ?.\""); break;
               case 1:
                 strcat(info, " screams \"Die, monster !\""); break;
               case 2:
                 strcat(info, " screams \"Give me Holy Grail !\""); break;
               case 3:
                 strcat(info, " screams \"Where is Holy Trail ?\""); break;
               case 4:
                 strcat(info, " looks confused."); break;
               case 5:
                 strcat(info, " looks excited."); break;
               case 6:
                 strcat(info, " shouts \"I'm great and powerful hero !\""); break;
               case 7:
                 strcat(info, " screams \"Get ready ! I'll kill you ! Or something like it...\""); break;
               case 8:
                 strcat(info, " says \"My Mom always said, kill them all.\""); break;
               case 9:
                 strcat(info, " screams \" You killed all those lovely monsters, you murderer !\""); break;
               case 10:
                 strcat(info, " screams \" Hurray !\""); break;
              } break; // end Rupert

             case MONS_NORRIS: // enlighten but crazy man
             switch (random2(21))
             {
               case 0:
                 strcat(info, " sings \"Hare Rama, Hare Krishna !\""); break;
               case 1:
                 strcat(info, " smiles at you."); break;
               case 2:
                 strcat(info, " says \"After death you'll find inner peace.\""); break;
               case 3:
                 strcat(info, " says \"Life is just a suffering. I'll help you.\""); break;
               case 4:
                 strcat(info, " is surrounded with aura of peace."); break;
               case 5:
                 strcat(info, " looks very balanced."); break;
               case 6:
                 strcat(info, " says \"Don't resist. I do it for you.\""); break;
               case 7:
                 strcat(info, " screams \"Enter NIRVANA ! Now !\""); break;
               case 8:
                 strcat(info, " says \"Death is just a liberation !\""); break;
               case 9:
                 strcat(info, " says \"Feel free to die. It's great thing.\""); break;
               case 10:
                 strcat(info, " says \"OHM MANI PADME HUM !\""); break;
               case 11:
                 strcat(info, " mumbles some mantras."); break;
               case 12:
                 strcat(info, " says \"Breath deep.\""); break;
               case 13:
                 strcat(info, " screams \"Love ! Eternal love !\""); break;
               case 14:
                 strcat(info, " screams \"Peace ! I bring you eternal peace !\""); break;
               case 15:
                 strcat(info, " sighs \"Enlightenment is such a responsibility.\""); break;
               case 16:
                 strcat(info, " looks relaxed."); break;
               case 17:
                 strcat(info, " screams \"Free your soul ! Die !\""); break;
               case 18:
                 strcat(info, " screams \"Blow your mind !\""); break;
               case 19:
                 strcat(info, " says \"The Orb is only your imagination. Forget it.\""); break;
               case 20:
                 strcat(info, " says \"It's all maya.\""); break;
               } break; // end Norris

             case MONS_MARGERY: // powerful sorceress, guarding the ORB
             switch (random2(22))
             {
               case 0:
                 strcat(info, " says \"You are dead.\""); break;
               case 1:
                 strcat(info, " looks very self-confident."); break;
               case 2:
                 strcat(info, " screams \"You must be punished !\""); break;
               case 3:
                 strcat(info, " screams \"You can't face my power !\""); break;
               case 4:
                 strcat(info, " is surrounded with aura of power."); break;
               case 5:
                 strcat(info, "'s eyes starts to glow with a red light."); break;
               case 6:
                 strcat(info, "'s eyes starts to glow with a green light."); break;
               case 7:
                 strcat(info, "'s eyes starts to glow with a blue light."); break;
               case 8:
                 strcat(info, " screams \"All trespassers must die.\""); break;
               case 9:
                 strcat(info, " says \"Die !\""); break;
               case 10:
                 strcat(info, " screams \"You have to pass me !\""); break;
               case 11:
                 strcat(info, " becomes transparent for a moment."); break;
               case 12:
                 strcat(info, " gestures."); break;
               case 13:
                 strcat(info, "'s hands start to glow."); break;
               case 14:
                 strcat(info, " screams \"Ergichanteg reztahaw !\"");
                 mpr (info);
                 mpr ("You feel really bad.");
                 return true; break;
               case 15:
                 strcat(info, " screams \"You are doomed !\""); break;
               case 16:
                 strcat(info, " screams \"Nothing can help you.\""); break;
               case 17:
                 strcat(info, " screams \"Death is my second name !\""); break;
               case 18:
                 strcat(info, " gestures.");
                 mpr (info);
                 mpr ("You feel doomed.");
                 return true; break;
               case 19:
                 strcat(info, " gestures.");
                 mpr (info);
                 mpr ("You feel weakened.");
                 return true; break;
               case 20:
                 strcat(info, " throws some weird powder towards you.");
                 mpr (info);
                 mpr ("You feel cursed.");
                 return true; break;
               case 21:
                 strcat(info, " screams \"The ORB is only tale. But I kill you anyway !"); break;
               } break; // end Margery

             case MONS_IJYB: // twisted goblin
             switch (random2(11))
             {
               case 0:
                 strcat(info, " screams \"Die !\""); break;
               case 1:
                 strcat(info, " screams \"Me kill you !\""); break;
               case 2:
                 strcat(info, " screams \"Me stronger than you !\""); break;
               case 3:
               case 4:
                 strcat(info, " grins evilly."); break;
               case 5:
                 strcat(info, " screams \"It's all mine !\""); break;
               case 6:
                 strcat(info, " screams \"Get away !\""); break;
               case 7:
                 strcat(info, " screams \"This level is mine ! Whole !\""); break;
               case 8:
                 strcat(info, " screams \"I'll cut your head away !\""); break;
               case 9:
                 strcat(info, " screams \"I'll be skip on your corpse !\""); break;
               case 10:
                 strcat(info, " screams \"Me very upset !\""); break;
             } break; // end IJYB

             case MONS_BLORK_THE_ORC: // unfriendly orc
             switch (random2(20))
             {
               case 0:
                 strcat(info, " screams \"I don't like you !\""); break;
               case 1:
                 strcat(info, " screams \"I'm going to kill you !\""); break;
               case 2:
                 strcat(info, " screams \"I'm much more stronger than you !\""); break;
               case 3:
               case 4:
                 strcat(info, " grins evilly."); break;
               case 5:
                 strcat(info, " frowns."); break;
               case 6:
               case 7:
               case 8:
               case 9:
                 strcat(info, " looks angry."); break;
               case 10:
                 strcat(info, " screams \"I'll eat your brain ! And then I'll vomit it !\""); break;
               case 11:
                 strcat(info, " screams \"You are the ugliest creature I've ever seen !\""); break;
               case 12:
                 strcat(info, " screams \"I'll cut your head off !\""); break;
               case 13:
                 strcat(info, " screams \"I'll break your legs !\""); break;
               case 14:
                 strcat(info, " screams \"I'll break your arms !\""); break;
               case 15:
                 strcat(info, " screams \"I'll crush all your ribs ! One by one !\""); break;
               case 16:
                 strcat(info, " screams \"I'll make dress of your skin !\""); break;
               case 17:
                 strcat(info, " screams \"I'll decorate my home with your organs !\""); break;
               case 18:
                 strcat(info, " screams \"Die !\""); break;
               case 19:
                 strcat(info, " screams \"I'll cover dungeon with your blood !\""); break;
             } break; // end Blork

             case MONS_EROLCHA: // ugly ogre
             switch (random2(11))
             {
               case 0:
                 strcat(info, " tries to grin evilly."); break;
               case 1:
                 strcat(info, " screams \"Eat !\""); break;
               case 2:
                 strcat(info, " screams \"Stand ! Erolcha want hit you !\""); break;
               case 3:
                 strcat(info, " screams \"Blood !\""); break;
               case 4:
                 strcat(info, " screams \"Erolcha kill you!\""); break;
               case 5:
                 strcat(info, " screams \"Erolcha want your head crushed !\""); break;
               case 6:
                 strcat(info, " roars."); break;
               case 7:
                 strcat(info, " growls."); break;
               case 8:
                 strcat(info, " screams \"You lunch !\""); break;
               case 9:
                 strcat(info, " screams \"Erolcha be happy to kill you !\""); break;
               case 10:
                 strcat(info, " screams \"Erolcha be angry !\""); break;
             } break; // end Erolcha

             case MONS_URUG: // orc hired to kill you
             switch (random2(11))
             {
               case 0:
                 strcat(info, " grins evilly."); break;
               case 1:
                 strcat(info, " screams \"Die !\""); break;
               case 2:
                 strcat(info, " screams \"I'm going to kill you ! Now !\""); break;
               case 3:
                 strcat(info, " screams \"Blood and destruciton !\""); break;
               case 4:
                 strcat(info, " screams \"Are you innocent ? I'll kill you anyway.\""); break;
               case 5:
                 strcat(info, " screams \"I'll get 30 silver pieces for your head !\""); break;
               case 6:
                 strcat(info, " roars."); break;
               case 7:
                 strcat(info, " howls with blood-lust."); break;
               case 8:
                 strcat(info, " screams \"You are already dead.\""); break;
               case 9:
                 strcat(info, " says \"Maybe you aren't ");
                 strcat(info, you.your_name);
                 strcat(info,". It doesn't matter.\""); break;
               case 10:
                 strcat(info, " screams \"I love blood !\""); break;
             } break; // end Urug

             case MONS_SNORG: // troll
             switch (random2(14))
             {
               case 0:
                 strcat(info, " grins."); break;
               case 1:
               case 2:
               case 3:
                 strcat(info, " smells terribly."); break;
               case 4:
               case 5:
               case 6:
                 strcat(info, " looks very hungry."); break;
               case 7:
                 strcat(info, " screams \"Snack !\""); break;
               case 8:
                 strcat(info, " roars."); break;
               case 9:
                 strcat(info, " says \"Food !\""); break;
               case 10:
                 strcat(info, " screams \"Snorg hungry !\""); break;
               case 11:
                 strcat(info, " screams \"Snorg very very hungry !\"");
               case 12:
                 strcat(info, " says \"Snorg will eat you.\""); break;
               case 13:
                 strcat(info, " says \"Snorg happy to meet food !\""); break;
             } break; // end Snorg

             case MONS_XTAHUA: // ancient dragon
             switch (random2(13))
             {
               case 0:
                 strcat(info, " roars \"DIE !\""); break;
               case 1:
                 strcat(info, " roars \"YOU BORE ME SO !\""); break;
               case 2:
                 strcat(info, " roars \"YOU WILL DIE SOON !\""); break;
               case 3:
                 strcat(info, " roars \"I HATE BEING BOTHERED !\""); break;
               case 4:
                 strcat(info, " roars \"YOU WILL END UP LIKE A SNACK !\""); break;
               case 5:
                 strcat(info, " roars \"I HATE ADVENTURERS !\""); break;
               case 6:
                 strcat(info, " roars \"FACE MY WRATH !\""); break;
               case 7:
                 strcat(info, " looks upset."); break;
               case 8:
                 strcat(info, " roars \"COMING HERE WAS YOUR LAST MISTAKE !\""); break;
               case 9:
                 strcat(info, " roars \"I'VE KILLED HUNDREDS OF ADVENTURERS LIKE YOU !\""); break;
               case 10:
               case 11:
               case 12:
                 strcat(info, " roars horribly.");
                 mpr (info);
                 mpr ("You are feared.");
                 return true; break ;
             } break; // end Xtahua

             case MONS_BORIS: // ancient lich
             switch (random2(18))
             {
               case 0:
                 strcat(info, " says \"I didn't invite you.\""); break;
               case 1:
                 strcat(info, " says \"You can't imagine my power.\""); break;
               case 2:
                 strcat(info, " says \"Orb ? You want the Orb ? You'll never get it.\""); break;
               case 3:
                 strcat(info, " says \"The world, the flesh, and the devil.\""); break;
               case 4:
                 strcat(info, " gestures."); break;
               case 5:
                 strcat(info, " stares at you.");
                 mpr (info);
                 mpr ("You feel drained.");
                 return true; break;
               case 6:
                 strcat(info, " stares at you.");
                 mpr (info);
                 mpr ("You feel weakened.");
                 return true; break;
               case 7:
                 strcat(info, " stares at you.");
                 mpr (info);
                 mpr ("You feel troubles.");
                 return true; break;
               case 8:
                 strcat(info, " say \"Magic. You know nothing about it.\""); break;
               case 9:
                 strcat(info, " says \"My power is unlimited.\""); break;
               case 10:
                 strcat(info, " says \"You can not kill me. I'm immortal. \""); break;
               case 11:
                 strcat(info, " casts spell.");
                 mpr (info);
                 mpr ("You equipment sudddenly weights more.");
                 return true; break;
               case 12:
                 strcat(info, " says \"I know the secret of eternal life. You don't.\""); break;
               case 13:
                 strcat(info, " smiles crazily."); break;
               case 14:
                 strcat(info, " casts spell.");
                 mpr (info);
                 mpr ("You resist.");
                 return true; break;
               case 15:
                 strcat(info, " casts spell.");
                 mpr (info);
                 mpr ("Suddenly you are surrounded with pale green light.");
                 return true; break;
               case 16:
                 strcat(info, " casts spell.");
                 mpr (info);
                 mpr ("You have terrible head-ache.");
                 return true; break;
               case 17:
                 strcat(info, " says \"I know future. Your future is death.\""); break;
               } break; // end BORIS


           case MONS_DEATH_COB:
              return false; break; // mv: I don't know what could _cob_ say
                                   // any suggestions ?

           case MONS_KILLER_KLOWN: // Killer Klown - guess !
             switch (random2(10))
             {
               case 0:
                 strcat(info, " giggles crazily."); break;
               case 1:
                 strcat(info, " laughs merrily."); break;
               case 2:
                 strcat(info, " beckons to you."); break;
               case 3:
                 strcat(info, " does a flip."); break;
               case 4:
                 strcat(info, " does a somersault."); break;
               case 5:
                 strcat(info, " smiles at you."); break;
               case 6:
                 strcat(info, " grins with merry abandon."); break;
               case 7:
                 strcat(info, " howls with blood-lust!"); break;
               case 8:
                 strcat(info, " pokes out its tongue."); break;
               case 9:
                 strcat(info, " says \"Come and play with me!\""); break;
             } break; // end Killer Klown
        default :
          strcat(info, " says \"I don't know what to say. It's a bug.\"");
          break;
        } // end monster->type - monster type switch
     } // end default
   } // end switch monster->behavior

    mpr(info);
    return true;

} // end mons_speaks = end of routine

unsigned char monster_habitat( int which_class )
{

    switch ( which_class )
    {
      case MONS_BIG_FISH:
      case MONS_GIANT_GOLDFISH:
      case MONS_ELECTRICAL_EEL:
      case MONS_JELLYFISH:
      case MONS_WATER_ELEMENTAL:
      case MONS_SWAMP_WORM:
        return DNGN_DEEP_WATER; // no shallow water (only) monsters? {dlb}
                 // must remain DEEP_WATER for now, else breaks code {dlb}

      case MONS_LAVA_WORM:
      case MONS_LAVA_FISH:
      case MONS_LAVA_SNAKE:
      case MONS_ANOTHER_LAVA_THING:
        return DNGN_LAVA;

      default:
        return DNGN_FLOOR;         // closest match to terra firma {dlb}
    }

}          // end monster_habitat()




bool monster_descriptor( int which_class, unsigned char which_descriptor )
{

    switch ( which_descriptor )
    {
        case MDSC_LEAVES_HIDE:
          switch ( which_class )
          {
             case MONS_DRAGON :
             case MONS_TROLL :
             case MONS_ICE_DRAGON :
             case MONS_STEAM_DRAGON :
             case MONS_MOTTLED_DRAGON :
             case MONS_STORM_DRAGON :
             case MONS_GOLDEN_DRAGON :
             case MONS_SWAMP_DRAGON :
               return true;
             default:
               return false;
          }
          break;
        case MDSC_REGENERATES:
         switch ( which_class )
         {
             case MONS_CACODEMON:
             case MONS_DEEP_TROLL:
             case MONS_HELLWING:
             case MONS_IMP:
             case MONS_IRON_TROLL:
             case MONS_LEMURE:
             case MONS_ROCK_TROLL:
             case MONS_SLIME_CREATURE:
             case MONS_SNORG:
             case MONS_TROLL:
               return true;
             default:
               return false;
         }
         break;
       case MDSC_NOMSG_WOUNDS:
         switch ( which_class )
         {
             case MONS_RAKSHASA:
             case MONS_RAKSHASA_FAKE:
             case MONS_SKELETON_LARGE:
             case MONS_SKELETON_SMALL:
             case MONS_ZOMBIE_LARGE:
             case MONS_ZOMBIE_SMALL:
               return true;
             default:
               return false;
         }
         break;
        default:
          return false;
    }

}          // end monster_descriptor()




bool message_current_target( void )
{
    if ( !( you.prev_targ == MHITNOT || you.prev_targ == MHITYOU ) )
    {
        struct monsters *montarget = &menv[you.prev_targ];

        if ( mons_near(montarget) && ( montarget->enchantment[2] != ENCH_INVIS || player_see_invis() ) )
        {
            strcpy(info, "You are currently targetting ");
            strcat(info, monam(montarget->number, montarget->type, montarget->enchantment[2], 1));
            strcat(info, " (p to target).");
            mpr(info);

            return true;    // early return {dlb}
        }
        else
          mpr("You have no current target.");
    }

    return false;
}          // end message_current_target()




// aaah, the simple joys of pointer arithmetic! {dlb}:
int monster_index( struct monsters *monster )
{
    return (monster - menv.buffer());

}          // end monster_index()




bool hurt_monster( struct monsters *victim, int damage_dealt )
{
    bool just_a_scratch = true;


    if ( damage_dealt > 0 )
    {
        just_a_scratch = false;
        victim->hit_points -= damage_dealt;
    }

    return ( !just_a_scratch );

}          // end hurt_monster()




bool heal_monster( struct monsters *patient, int health_boost, bool permit_growth )
{
    if ( health_boost < 1 )
      return false;
    else if ( !permit_growth && patient->hit_points == patient->max_hit_points )
      return false;
    else
    {
        if ( (patient->hit_points += health_boost) > patient->max_hit_points )
        {
            if ( permit_growth )
              patient->max_hit_points++;

            patient->hit_points = patient->max_hit_points;
        }
    }

    return true;

}          // end heal_monster()

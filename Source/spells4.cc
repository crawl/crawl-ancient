/*
 *  File:       spells4.cc
 *  Summary:    new spells, focusing on transmigration, divination and
 *              other neglected areas of Crawl magic ;^)
 *  Written by: Copyleft Josh Fishman 1999-2000, All Rights Preserved
 *
 *  Change History (most recent first):
 *
 *   <2> 29jul2000  jdj  Made a zillion functions static.
 *   <1> 06jan2000  jmf  Created
 */

#include "AppHdr.h"
#include "spells4.h"

#include <string>
#include <stdio.h>

#include "externs.h"

#include "abyss.h"
#include "bang.h"
#include "beam.h"
#include "debug.h"
#include "describe.h"
#include "direct.h"
#include "dungeon.h"
#include "effects.h"
#include "fight.h"
#include "it_use2.h"
#include "itemname.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mon-util.h"
#include "mstuff2.h"
#include "ouch.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "spells.h"
#include "spells1.h"
#include "stuff.h"
#include "view.h"


extern bool wield_change;    // defined in output.cc


// this does not belong in enum.h until well-implemented(!!!) {dlb}:
//jmf: why not? there's tons of stuff in there that isn't.
//     including your CE_foos, for a pointed example.
enum DEBRIS // jmf: add for shatter, dig, and Giants to throw
{
    DEBRIS_METAL,                      //    0
    DEBRIS_ROCK,
    DEBRIS_STONE,
    DEBRIS_WOOD,
    DEBRIS_CRYSTAL,
    NUM_DEBRIS
}; // jmf: ...and I'll actually implement the items Real Soon Now...


static bool mons_can_host_shuggoth(int type);
static int make_a_random_cloud(char x, char y, int pow, int ctype);
static int make_a_rot_cloud(char x, char y, int pow, int ctype);
static int quadrant_blink(char x, char y, int pow, int garbage);
static void apply_area_square(int (*func)(char, char, int, int), char cx, char cy, int power);
static void apply_area_visible(int (*func)(char, char, int, int), int power);
//void cast_animate_golem(int pow); // see actual function for reasoning {dlb}
//void cast_detect_magic(int pow);  //jmf: as above...
//void cast_eringyas_surprising_bouquet(int powc);
void do_monster_rot(int mon);




//jmf: FIXME: put somewhere else (misc.cc?)
// A feeble attempt at Nethack-like completeness for cute messages.
static const char *your_hand( int plural )
{
    static char buffer[80];

    switch (you.attribute[ATTR_TRANSFORMATION])
    {
      default:
        mpr("ERROR: unknown transformation in your_hand() (spells4.cc)");
      case TRAN_NONE:
      case TRAN_STATUE:
        if (you.species == SP_TROLL || you.species == SP_GHOUL)
          {
            strcpy(buffer, "claw");
            break;
          }
      case TRAN_ICE_BEAST:
      case TRAN_LICH:
        strcpy(buffer, "hand");
        break;
      case TRAN_SPIDER:
        strcpy(buffer, "front leg");
        break;
      case TRAN_SERPENT_OF_HELL:
      case TRAN_DRAGON:
        strcpy(buffer, "foreclaw");
        break;
      case TRAN_BLADE_HANDS:
        strcpy(buffer, "scythe-like blade");
        break;
      case TRAN_AIR:
        strcpy(buffer, "misty tendril");
        break;
    }

    if (plural)
      strcat(buffer, "s");

    return buffer;

}


// I need to make some debris for metal, crystal and stone.
// They could go in OBJ_MISSILES, but I think I'd rather move
// MI_LARGE_ROCK into OBJ_DEBRIS and code giants to throw any
// OBJ_DEBRIS they get their meaty mits on.
static void place_debris( char x, char y, int debris_type )
{

#ifdef USE_DEBRIS_CODE

    int large = 0, small = 0;

    switch( debris_type )
    {
    // hate to say this, but the first parameter only allows specific quantity
    // for *food* and nothing else -- and I would hate to see that parameter
    // (force_unique) abused any more than it already has been ... {dlb}:
        case DEBRIS_STONE:
          large = items(random2(3), OBJ_MISSILES, MI_LARGE_ROCK, 1, 1, 250);
          small = items(3 + random2(6) + random2(6) + random2(6),
                        OBJ_MISSILES, MI_STONE, 1, 1, 250);
          break;
        case DEBRIS_METAL:
        case DEBRIS_WOOD:
        case DEBRIS_CRYSTAL:
          break;
    }

    if ( small )
    {
        mitm.link[small] = igrd[x][y];
        igrd[x][y] = small;
    }

    if ( large )
    {
        mitm.link[large] = igrd[x][y];
        igrd[x][y] = large;
    }

#else

  return;

#endif

}          // end place_debris()




// just to avoid typing this over and over
inline void player_hurt_monster( int monster, int damage )
{
    ASSERT(monster != NON_MONSTER);

    if (damage > 0)
    {
      hurt_monster(&menv[monster], damage);

      if ( menv[monster].hit_points < 1 )
        monster_die(&menv[monster], KILL_YOU, 0);
      else
        print_wounds(&menv[monster]);
    }

    return;
}          // end player_hurt_monster()




int enchant_monster( int mon, int ench )
{
    int p;

    ASSERT(mon != NON_MONSTER);

    if (menv[mon].enchantment1 == 1)
      for (p = 0; p < 3; p++)
      {
          if (menv[mon].enchantment[p] == ench)
          return 0;
      }

    for (p = 0; p < 3; p++)
    {
        if (menv[mon].enchantment[p] == 0)
        {
            menv[mon].enchantment[p] = ench;
            menv[mon].enchantment1 = 1;
            return 1;
        }
    }

    return 0;
}          // end enchant_monster()




// FUNCTION APPLICATORS: Idea from Juho Snellman <jsnell@lyseo.edu.ouka.fi>
//                       on the Roguelike News pages, Development section.
//                       <URL:http://www.skoardy.demon.co.uk/rlnews/>
// Here are some function applicators: sort of like brain-dead,
// home-grown iterators for the container "dungeon".

// apply a function-pointer to all visible squares
static void apply_area_visible( int (*func)(char, char, int, int), int power )
{

    int x, y;

    //jmf: FIXME: randomly start from other quadrants, like raise_dead?

    for (x = you.x_pos -8; x <= you.x_pos + 8; x++)
      for (y = you.y_pos -8; y <= you.y_pos + 8; y++)
        if ( see_grid(x,y) )
          func(x,y,power,0);

}          // end apply_area_visible()




// NB: this actually has yet to be used {dlb}:
//jmf: It's for Fireball and Mephitic Cloud, if we incorporate those
//     into this ugly function-pointer system.
static void apply_area_square( int (*func)(char, char, int, int),
                               char cx, char cy, int power )
{
    char x,y;

    for (x = cx-1; x <= cx+1; x++)
      for (y = cy-1; y <= cy+1; y++)
        func(x,y,power,0);
}          // end apply_area_square()




static void apply_area_around_player( int (*func)(char, char, int, int), int power )
{
    char x, y;

    for (x = you.x_pos -1; x <= you.x_pos +1; x++)
      for (y = you.y_pos -1; y <= you.y_pos +1; y++)
        if (x == you.x_pos && y == you.y_pos)
          continue;
        else
         func(x,y,power,0);
}          // end apply_area_around_player()




static void apply_one_neighboring_square( int (*func)(char,char,int,int), int power)
{
  struct dist bmove[1];

 try_again:
  mpr("Which direction? [ESC to cancel]");
  direction(0, bmove);

  if (bmove[0].nothing == -1)
    {
      canned_msg(MSG_SPELL_FIZZLES);
      return;
    }

  if ( (abs(bmove[0].move_x) > 1) || (abs(bmove[0].move_y) > 1) )
    {
      mpr("This spell doesn't reach that far.");
      return;
    }

  if( func(you.x_pos + bmove[0].move_x, you.y_pos + bmove[0].move_y, power, 1))
    return;
  else
    canned_msg(MSG_NOTHING_HAPPENS);
    //goto try_again; //jmf: used to keep giving chances...
}          // end apply_one_neighboring_square()




void apply_area_within_radius( int (*func)(char,char,int,int),
                               char x, char y, int pow, int radius, int ctype )
{
    int ix, iy;
    int sq_radius = radius * radius;

    //jmf: FIXME: randomly start from other quadrants, like raise_dead?
    //            or maybe start from center (x,y) and work out?

    for (ix = 0; ix < GXM; ix++)
      for (iy = 0; iy < GYM; iy++)    // was "iy < 80" but that is WRONG {dlb}
        if ( distance(x,y,ix,iy) <= sq_radius )
          func(ix,iy,pow,ctype);
}          // end apply_area_within_radius()




static inline bool cloud_helper(int (*func)(char,char,int,int),
                                char x, char y, int pow, int ctype)
{
  if ( grd[x][y] > DNGN_LAST_SOLID_TILE && env.cgrid[x][y] == EMPTY_CLOUD )
  {
      func(x,y,pow,ctype);
      return true;
  }
  else
    return false;
}




// apply_area_cloud:
// Try to make a realistic cloud by expanding from a point, filling empty
// floor tiles until we run out of material (passed in as number).
// We really need some sort of a queue structure, since ideally I'd like
// to do a (shallow) breadth-first-search of the dungeon floor.
// This ought to work okay for small clouds.
void apply_area_cloud( int (*func)(char,char,int,int),char x, char y, int pow, int number, int ctype )
{

    int spread, clouds_left = number;
    int good_squares = 0, neighbors[8] = {0,0,0,0,0,0,0,0};
    int dx = 1, dy = 1;
    bool x_first;

    if ( clouds_left && cloud_helper(func, x, y, pow, ctype) )
      clouds_left--;

    if ( !clouds_left )
      return;

    if (coinflip())
      dx *= -1;
    if (coinflip())
      dy *= -1;

    x_first = coinflip();

    if (x_first)
    {
        if ( clouds_left && cloud_helper(func, x+dx, y, pow, ctype) )
        {
            clouds_left--;
            good_squares++;
            neighbors[0]++;
        }

        if ( clouds_left && cloud_helper(func, x-dx, y, pow, ctype) )
        {
            clouds_left--;
            good_squares++;
            neighbors[1]++;
        }

        if ( clouds_left && cloud_helper(func, x, y+dy, pow, ctype) )
        {
            clouds_left--;
            good_squares++;
            neighbors[2]++;
        }

        if ( clouds_left && cloud_helper(func, x, y-dy, pow, ctype) )
        {
            clouds_left--;
            good_squares++;
            neighbors[3]++;
        }
    }
    else
    {
        if (clouds_left && cloud_helper(func, x, y+dy, pow, ctype))
        {
            clouds_left--;
            good_squares++;
            neighbors[2]++;
        }

        if (clouds_left && cloud_helper(func, x, y-dy, pow, ctype))
        {
            clouds_left--;
            good_squares++;
            neighbors[3]++;
        }

        if (clouds_left && cloud_helper(func, x+dx, y, pow, ctype))
        {
            clouds_left--;
            good_squares++;
            neighbors[0]++;
        }

        if (clouds_left && cloud_helper(func, x-dx, y, pow, ctype))
        {
            clouds_left--;
            good_squares++;
            neighbors[1]++;
        }
    }

    // now diagonals; we could randomize dx & dy again here
    if (clouds_left && cloud_helper(func, x+dx, y+dy, pow, ctype))
    {
        clouds_left--;
        good_squares++;
        neighbors[4]++;
    }

    if (clouds_left && cloud_helper(func, x-dx, y+dy, pow, ctype))
    {
        clouds_left--;
        good_squares++;
        neighbors[5]++;
    }

    if (clouds_left && cloud_helper(func, x+dx, y-dy, pow, ctype))
    {
        clouds_left--;
        good_squares++;
        neighbors[6]++;
    }

    if (clouds_left && cloud_helper(func, x-dx, y-dy, pow, ctype))
    {
        clouds_left--;
        good_squares++;
        neighbors[7]++;
    }

    if ( !(clouds_left && good_squares) )
      return;

    for (int i = 0; i < 8 && clouds_left; i++)
    {
        if ( neighbors[i] == 0 )
          continue;

        spread = clouds_left / good_squares;
        clouds_left -= spread;
        good_squares--;

        switch (i)
        {
            case 0:
              apply_area_cloud(func, x+dx, y, pow, spread, ctype);
              break;
            case 1:
              apply_area_cloud(func, x-dx, y, pow, spread, ctype);
              break;
            case 2:
              apply_area_cloud(func, x, y+dy, pow, spread, ctype);
              break;
            case 3:
              apply_area_cloud(func, x, y-dy, pow, spread, ctype);
              break;
            case 4:
              apply_area_cloud(func, x+dx, y+dy, pow, spread, ctype);
              break;
            case 5:
              apply_area_cloud(func, x-dx, y+dy, pow, spread, ctype);
              break;
            case 6:
              apply_area_cloud(func, x+dx, y-dy, pow, spread, ctype);
              break;
            case 7:
              apply_area_cloud(func, x-dx, y-dy, pow, spread, ctype);
              break;
        }
    }

}          // end apply_area_cloud()




// Here begin the actual spells:




static int shatter_monsters(char x, char y, int pow, int garbage)
{
  int damage, monster = mgrd[x][y];
  if (monster == NON_MONSTER)                   return 0;
  damage = 15 + (random2(pow) +random2(pow) +random2(pow) +random2(pow))/4;
  switch( menv[monster].type )
    {
    case MONS_ICE_BEAST:      // 3/2 damage
    case MONS_ICE_DEVIL:
    case MONS_ICE_FIEND:
    case MONS_ICE_DRAGON:
    //case MONS_ROCK_TROLL: //jmf: not actually rock, is it?
    case MONS_DANCING_WEAPON:
    //case MONS_METAL_GARGOYLE:
    case MONS_MOLTEN_GARGOYLE:
    case MONS_QUICKSILVER_DRAGON:
      damage *= 15;
      damage /= 10;
      break;
    case MONS_IRON_DEVIL:     // double damage
    case MONS_SKELETON_SMALL:
    case MONS_SKELETON_LARGE:
    case MONS_CURSE_SKULL:
    case MONS_CLAY_GOLEM:
    case MONS_STONE_GOLEM:
    case MONS_IRON_GOLEM:
    case MONS_CRYSTAL_GOLEM:
    case MONS_EARTH_ELEMENTAL:
    case MONS_GARGOYLE:
    case MONS_SKELETAL_DRAGON:
    case MONS_SKELETAL_WARRIOR:
    case MONS_IRON_DRAGON:
      damage *= 2;
      break;
    case MONS_VAPOUR:
    case MONS_INSUBSTANTIAL_WISP:
    case MONS_AIR_ELEMENTAL:
    case MONS_FIRE_ELEMENTAL:
    case MONS_WATER_ELEMENTAL:
    case MONS_SPECTRAL_WARRIOR:
    case MONS_FREEZING_WRAITH:
    case MONS_WRAITH:
    case MONS_PHANTOM:
    case MONS_PLAYER_GHOST:
    case MONS_SHADOW:
    case MONS_HUNGRY_GHOST:
    case MONS_FLAYED_GHOST:
    case MONS_SMOKE_DEMON: //jmf: I hate these bastards...
      damage = 0;
      break;
    case MONS_PULSATING_LUMP:
    case MONS_JELLY:
    case MONS_SLIME_CREATURE:
    case MONS_BROWN_OOZE:
    case MONS_AZURE_JELLY:
    case MONS_DEATH_OOZE:
    case MONS_ACID_BLOB:
    case MONS_ROYAL_JELLY:
    case MONS_OOZE:
    case MONS_SPECTRAL_THING:
    case MONS_JELLYFISH:
      damage = 1 + (damage / 4);
      break;
    default: // normal damage
      break;
    }
  //FIXME: resist maybe? maybe not...
  player_hurt_monster(monster, damage);
  return damage;

}          // end shatter_monsters()




static int shatter_items(char x, char y, int pow, int garbage)
{

    int broke_stuff = 0, next, obj = igrd[x][y];

    if ( obj == 501 )                   return 0;

    while (obj != 501)
    {
        next = mitm.link[obj];

        switch ( mitm.base_type[obj] )
        {
            case OBJ_POTIONS:
              if ( !one_chance_in(10) )
              {
                  broke_stuff++;
                  destroy_item(obj);
              }
              break;
            default:
              break;
        }

        obj = next;
    }

    if (broke_stuff)
    {
        if ( !silenced(x,y) && !silenced(you.x_pos, you.y_pos) )
          mpr("You hear glass break.");

        return 1;
    }
    else
      return 0;

}          // end shatter_items()




static int shatter_walls(char x, char y, int pow, int garbage)
{

    int chance = pow / 4;
    int stuff = 0;

    switch (grd[x][y])
    {
        case DNGN_SECRET_DOOR:
          if ( see_grid(x,y) )
            mpr("A secret door shatters!");
          grd[x][y] = DNGN_FLOOR;
          stuff = DEBRIS_WOOD;
          break;
        case DNGN_CLOSED_DOOR:
          if ( see_grid(x,y) )
            mpr("A door shatters!");
          grd[x][y] = DNGN_FLOOR;
          stuff = DEBRIS_WOOD;
          break;

        case DNGN_SILVER_STATUE: //FIXME: decrement statue[whatever].stuff
        case DNGN_METAL_WALL:
          stuff = DEBRIS_METAL;
          chance /= 4;
          break;

        case DNGN_GRANITE_STATUE:
        case DNGN_STONE_WALL:
          chance /= 2;
        case DNGN_ORCISH_IDOL:
          stuff = DEBRIS_STONE;
          break;

        case DNGN_ROCK_WALL:
          stuff = DEBRIS_ROCK;
          break;

        case DNGN_GREEN_CRYSTAL_WALL:
        case DNGN_ORANGE_CRYSTAL_STATUE:
          stuff = DEBRIS_CRYSTAL;
          break;

        default:
          break;
    }

    if (stuff && random2(100) < chance)
    {
        if ( !silenced(x,y) && !silenced(you.x_pos, you.y_pos) )
          mpr("Ka-crash!");

        grd[x][y] = DNGN_FLOOR;
        place_debris(x,y,stuff);
        return 1;
    }
    else
      return 0;

}          // end shatter_walls()




void cast_shatter( int pow )
{

    int damage = 0;

    strcpy(info, "The dungeon ");
    strcat(info, (silenced(you.x_pos, you.y_pos)) ? "shakes!" : "rumbles!" );
    mpr(info);

    if ( !silenced(you.x_pos, you.y_pos) )
      noisy(you.x_pos, you.y_pos, 30);


    switch ( you.attribute[ATTR_TRANSFORMATION] )
    {
        case TRAN_NONE:
        case TRAN_SPIDER:
        case TRAN_LICH:
        case TRAN_DRAGON:
        case TRAN_AIR:
        case TRAN_SERPENT_OF_HELL:
          break;
        case TRAN_STATUE: // full damage
          damage = 15 +(random2(pow) +random2(pow) +random2(pow) +random2(pow))/4;
          break;
        case TRAN_ICE_BEAST: // 1/2 damage
          damage = 10 +(random2(pow) +random2(pow) +random2(pow) +random2(pow))/9;
          break;
        case TRAN_BLADE_HANDS: // 2d3 damage
          mpr("Your scythe-like blades vibrate painfully!");
          damage = 2 + random2avg(5,2);
          break;
        default:
          mpr("cast_shatter(): unknown transformation in spells4.cc");
    }

    if ( damage )
      ouch(damage, 0, KILLED_BY_TARGETTING);

    apply_area_within_radius(shatter_items, you.x_pos, you.y_pos, pow, 4, 0);
    apply_area_within_radius(shatter_monsters, you.x_pos, you.y_pos, pow, 4, 0);
    apply_area_within_radius(shatter_walls, you.x_pos, you.y_pos, pow, 4, 0);

}          // end cast_shatter()




// cast_forescry: raises evasion (by 8 currently) via divination
void cast_forescry(int pow)
{

    if ( !you.duration[DUR_FORESCRY] )
      mpr("You begin to receive glimpses of the immediate future...");

    you.duration[DUR_FORESCRY] += 5 + random2avg(1 + (pow / 2),2);

    if ( you.duration[DUR_FORESCRY] > 50 )
      you.duration[DUR_FORESCRY] = 50;

    you.redraw_evasion = 1;

}          // end cast_forescry()




void cast_see_invisible(int pow)
{

    if ( player_see_invis() && !you.duration[DUR_SEE_INVISIBLE] )
      mpr("Nothing seems to happen.");
    else
      mpr("Your vision seems to sharpen.");

    // no message if you already are under the spell

    you.duration[DUR_SEE_INVISIBLE] += 10 + random2(2 + (pow / 2));

  if ( you.duration[DUR_SEE_INVISIBLE] > 75 )
    you.duration[DUR_SEE_INVISIBLE] = 75;

}          // end cast_see_invisible()




// FIXME: This would be kinda cool if implemented right.
//        The idea is that, like detect_secret_doors, the spell gathers all
//        sorts of information about a thing and then tells the caster a few
//        cryptic hints. So for a (+3,+5) Mace of Flaming, one might detect
//        "enchantment and heat", but for a cursed ring of hunger, one might
//        detect "enchantment and ice" (since it gives you a 'deathly cold'
//        feeling when you put it on) or "necromancy" (since it's evil).
//        A weapon of Divine Wrath and a randart that makes you angry might
//        both give similar messages. The key would be to not tell more than
//        hints about whether an item is benign or cursed, but give info
//        on how strong its enchantment is (and therefore how valuable it
//        probably is).
static void cast_detect_magic(int pow)
{
  struct dist bmove[1];
  char x, y;
  int monster = 0, item = 0, next; //int max;
  FixedVector<int, NUM_SPELL_TYPES> found;
  int strong = 0; // int curse = 0;

  for (next = 0; next < NUM_SPELL_TYPES; next++)
    found[next] = 0;

  mpr("Which direction?");
  direction(0, bmove);

  if (bmove[0].nothing == -1)
  {
    bmove[0].move_x = 0;
    bmove[0].move_y = 0;
    canned_msg(MSG_SPELL_FIZZLES);
    return;
  }

  if ((abs(bmove[0].move_x) > 1) || (abs(bmove[0].move_y) > 1))
  {
    mpr("This spell doesn't reach that far.");
    return;
  }

  if (bmove[0].move_x == 0 && bmove[0].move_y == 0)
  {
    mpr("You detect a divination in progress.");
    return;
  }

  x = you.x_pos + bmove[0].move_x;
  y = you.y_pos + bmove[0].move_y;

  monster = mgrd[x][y];
  if (monster == NON_MONSTER)
    goto do_items;
  else
    goto all_done;

 do_items:
  item = igrd[x][y];
  if (item == 501) // is NON_ITEM == 501?
    goto all_done;

  while (item != 501)
  {
    next = mitm.link[item];
    if (is_dumpable_artifact(mitm.base_type[item], mitm.sub_type[item], mitm.pluses[item], mitm.pluses2[item], mitm.special[item], 0, 0))
      {
        strong++;
        //FIXME: do checks for randart properties
      }
    else
      switch (mitm.base_type[item])
      {
      case OBJ_WEAPONS:
        found[SPTYP_ENCHANTMENT] += (mitm.pluses[item] > 50);
        found[SPTYP_ENCHANTMENT] += (mitm.pluses2[item] > 50);

        break;

      case OBJ_MISSILES:
        found[SPTYP_ENCHANTMENT] += (mitm.pluses[item] > 50);
        found[SPTYP_ENCHANTMENT] += (mitm.pluses2[item] > 50);

        break;

      case OBJ_ARMOUR:
        found[SPTYP_ENCHANTMENT] += mitm.pluses[item];
      }
  }

 all_done:
  if (monster)
  {
    mpr("You detect a morphogenic field, such as a monster might have.");
  }
  if (strong)
  {
    mpr("You detect very strong enchantments.");
    return;
  }
  else
  {
    //FIXME:
  }
  return;
}





void cast_detect_secret_doors(int pow)
{

    int tmp, x, y, dy, dx, i =0;
    FixedVector<int, 30> xfound;
    FixedVector<int, 30> yfound;

    for (tmp = 0; tmp < 30; tmp++)
    {
        xfound[tmp] = 0;
        yfound[tmp] = 0;
    }

    // potentially may produce out-of-bounds on grd[][] {dlb}:
    for (x = you.x_pos - 6; x < you.x_pos + 7; x++)
      for (y = you.y_pos - 6; y < you.y_pos + 7; y++)
      {
          if ( !see_grid(x,y) ) // too evil to tell about doors they can't see
            continue;

          if (grd[x][y] == DNGN_SECRET_DOOR)
          {
              xfound[i] = x;
              yfound[i] = y;
              i++;

              if (i >= 30)
                break;
          }
      }

    if ( i ) // found something
    {
        tmp = (i > 1) ? random2(i-1) : 0;

        grd[xfound[tmp]][yfound[tmp]] = DNGN_CLOSED_DOOR;

        dx = you.x_pos - xfound[tmp];
        dy = you.y_pos - yfound[tmp];
        strcpy(info, "You sense a secret door ");

        if (dy < -2)
          strcat(info, "south");

        if (dy > 2)
          strcat(info, "north");

        if (dx < -2)
          strcat(info, "east");

        if (dx > 2)
          strcat(info, "west");

        if (abs(dx) <= 2 && abs(dy) <= 2)
          strcat(info, "nearby. ");
        else
          strcat(info, " of your position. ");

        mpr(info);
    }
    else
    {
      mpr("You don't sense any secret doors nearby.");
    }

    return;

}          // end cast_detect_secret_doors()




void cast_summon_butterflies( int pow )
{
    for (int scount = 1 + (pow/2); scount > 0; scount--)
      if ( scount > random2(10) )
        create_monster(MONS_BUTTERFLY, 22, BEH_ENSLAVED,
                       you.x_pos, you.y_pos,MHITNOT, 250);
}




void cast_summon_large_mammal( int pow )
{
    int mon;
    int temp_rand = random2(pow);

    if (temp_rand < 10)
      mon = MONS_JACKAL;
    else if (temp_rand < 15)
      mon = MONS_HOUND;
    else switch (temp_rand % 5)
      {
      case 0:  mon = MONS_WAR_DOG; break;
      case 1:
      case 2:  mon = MONS_HOUND;   break;
      default: mon = MONS_JACKAL;  break;
      }

    create_monster(mon, 22, BEH_ENSLAVED, you.x_pos, you.y_pos, MHITNOT, 250);
}




void cast_sticks_to_snakes( int pow )
{
    int mon, i, how_many = 0, max, behaviour;
    bool NICE = true; // delete all ammo?

    max = 1 + random2(2+ you.skills[SK_TRANSMIGRATION]/5);

    if (max > 6)
      max = 6;

  // how_many = you.inv_quantity[you.equip[EQ_WEAPON]];

    if (you.equip[EQ_WEAPON] == -1)
    {
        sprintf(info, "Your %s feel slithery!", your_hand(1));
        mpr(info);
        return;
    }

  //jmf: FIXME: is this correct for missiles and melee weapons?
    behaviour = (you.inv_plus[you.equip[EQ_WEAPON]] > 130) ? BEH_CHASING_I : BEH_ENSLAVED;

    if ((you.inv_class[you.equip[EQ_WEAPON]] == OBJ_MISSILES
       && (you.inv_type[you.equip[EQ_WEAPON]] == MI_ARROW)))
    {
        if (you.inv_quantity[you.equip[EQ_WEAPON]] < max)
          max = you.inv_quantity[you.equip[EQ_WEAPON]];

        for (i = 0; i <= max; i++)
        {
            if (pow > 35 && one_chance_in(3))
              mon = MONS_SNAKE; //jmf: perhaps also check for poison ammo?
            else
              mon = MONS_SMALL_SNAKE;

            if ( create_monster(mon, 22, behaviour, you.x_pos, you.y_pos, MHITNOT, 250) != -1 || !NICE )
              how_many++;
        }
    }

    if ( you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS
        && (you.inv_type[you.equip[EQ_WEAPON]] == WPN_CLUB
             || you.inv_type[you.equip[EQ_WEAPON]] == WPN_SPEAR
             || you.inv_type[you.equip[EQ_WEAPON]] == WPN_QUARTERSTAFF
             || you.inv_type[you.equip[EQ_WEAPON]] == WPN_SCYTHE
             || you.inv_type[you.equip[EQ_WEAPON]] == WPN_GIANT_CLUB
             || you.inv_type[you.equip[EQ_WEAPON]] == WPN_BOW
             || you.inv_type[you.equip[EQ_WEAPON]] == WPN_ANCUS
             || you.inv_type[you.equip[EQ_WEAPON]] == WPN_HALBERD
             || you.inv_type[you.equip[EQ_WEAPON]] == WPN_GLAIVE ))
    {
        how_many = 1;
        mon = MONS_SNAKE;

        if (pow > 80 && one_chance_in(3))
          mon = MONS_BLACK_SNAKE;

        if (pow > 40  && one_chance_in(3))
          mon = MONS_YELLOW_SNAKE;

        if (pow > 15  && one_chance_in(3))
          mon = MONS_BROWN_SNAKE;

        create_monster(mon, 22, behaviour, you.x_pos, you.y_pos, MHITNOT, 250);
    }

#ifdef USE_DEBRIS_CODE
    if ( you.inv_class[you.equip[EQ_WEAPON]] == OBJ_DEBRIS
        && (you.inv_type[you.equip[EQ_WEAPON]] == DEBRIS_WOOD))
    {
    // this is how you get multiple big snakes
        how_many = 1;
        mpr("FIXME: impliment OBJ_DEBRIS conversion! (spells4.cc)");
    }
#endif // USE_DEBRIS_CODE

    if (how_many)
    {
        if (you.inv_quantity[you.equip[EQ_WEAPON]] <= how_many)
        {
            unwield_item(you.equip[EQ_WEAPON]);
            you.inv_quantity[you.equip[EQ_WEAPON]] = 0;
            you.num_inv_items--;
            you.equip[EQ_WEAPON] = -1;
            mpr("You are now empty handed.");
        }
        else
          you.inv_quantity[you.equip[EQ_WEAPON]] -= how_many;

        wield_change = true;
        burden_change();
        sprintf(info, "You create %s snake%s!",
                how_many > 1 ? "some" : "a",
                how_many > 1 ? "s"    : "");
    }
    else
    {
        sprintf(info, "Your %s feel slithery!", your_hand(1));
    }

    mpr(info);

    return;

}          // end cast_sticks_to_snakes()




void cast_summon_dragon(int pow)
{
  int num, happy, i;

  num = (pow / 16);

  if (num < 1)
    num = 1;

  if (num > 4)
    num = 4;

  for (i = 0; i < num ; i++)
  {
    happy = random2(pow) > 3;

    if ( create_monster(MONS_DRAGON, 22, happy ? BEH_ENSLAVED : BEH_CHASING_I,
                        you.x_pos, you.y_pos, MHITNOT, 250) != -1)
      {
        strcpy(info, "A dragon appears.");

        if ( !happy )
          strcat(info, " It doesn't look very happy.");
      }
    else
      strcpy(info, "Nothing happens.");

    mpr(info);
  }
}          // end cast_summon_dragon()




static int sleep_monsters( char x, char y, int pow, int garbage )
{
    int mnstr = mgrd[x][y];

    if ( mnstr == NON_MONSTER )                                      return 0;
    if ( mons_holiness(menv[mnstr].type) != MH_NATURAL )     return 0;
    if ( !check_mons_magres(&menv[mnstr], pow) )             return 0;
    if ( menv[mnstr].behavior == BEH_ENSLAVED )              return 0;
    //jmf: now that sleep == hibernation:
    if ((mons_res_cold(menv[mnstr].type) > 0) && coinflip()) return 0;

    if ( mons_flag(menv[mnstr].type, M_COLD_BLOOD ) )
      enchant_monster( mnstr, ENCH_SLOW );
    menv[mnstr].behavior = BEH_SLEEP;

    return 1;
}          // end sleep_monsters()




void cast_mass_sleep( int pow )
{

    apply_area_visible(sleep_monsters, pow);

}          // end cast_mass_sleep()




static int tame_beast_monsters(char x, char y, int pow, int garbage)
{
    int which_mons = mgrd[x][y];
    struct monsters *monster = &menv[which_mons];

    if ( which_mons == NON_MONSTER )                               return 0;
    if ( mons_holiness(monster->type) != MH_NATURAL )      return 0;
    if ( mons_intel_type(monster->type) != I_ANIMAL )      return 0;
    if ( monster->behavior == BEH_ENSLAVED )               return 0;

    if ( monster->type == MONS_HOUND || monster->type == MONS_WAR_DOG )
      pow += pow / 4; // 25% bonus for dogs, add cats if they get implemented

    if ( !check_mons_magres(monster, pow) )                return 0;

// I'd like to make the monsters affected permanently, but that's
// pretty powerful. Maybe a small (pow/10) chance of being permanently
// tamed, large chance of just being enslaved.

    simple_monster_message(monster, " is tamed!");

    if ( random2(100) < random2(pow/10) )
      monster->behavior = BEH_ENSLAVED;    // permanent, right?
    else
      for (unsigned char i = 0; i < 3; i++)
        if ( monster->enchantment[i] == 0 )
        {
            monster->enchantment[i] = ENCH_CHARM;
            monster->enchantment1 = 1;
            monster->behavior = BEH_ENSLAVED;
            break;
        }

    return 1;

}          // end tame_beast_monsters()




void cast_tame_beasts( int pow )
{

    apply_area_visible(tame_beast_monsters, pow);

}          // end cast_tame_beasts()




static int ignite_poison_objects( char x, char y, int pow, int garbage )
{
    int obj = igrd[x][y], next, strength = 0;

    if ( obj == NON_ITEM ) return 0;
    while (obj != 501)
    {
        next = mitm.link[obj];
        if ( mitm.base_type[obj] == OBJ_POTIONS )
        {
            switch ( mitm.sub_type[obj] )
            {
            // intentional fall-through all the way down
                case POT_STRONG_POISON:
                  strength += 20;
                case POT_DEGENERATION:
                  strength += 10;
                case POT_POISON:
                  strength += 10;
                  destroy_item(obj);
                default:
                  break;
            }
        }

    // FIXME: impliment burning poisoned ammo
    // else if ( it's ammo that's poisoned) {
    //   strength += number_of_ammo;
    //   destroy_item(ammo);
    //  }
        obj = next;
    }

    if ( strength > 0 )
      place_cloud(CLOUD_FIRE, x, y, strength + random2(1+ strength /4) + random2(1+ strength /4)+ random2(1+ strength /4));

    return strength;

}          // end ignite_poison_objects()




static int ignite_poison_clouds(char x, char y, int pow, int garbage)
{

    int ctemp;
    bool did_anything = false;

    if (env.cgrid[x][y] != EMPTY_CLOUD)
    {
        if ( env.cloud_type[env.cgrid[x][y]] == CLOUD_STINK
              || env.cloud_type[env.cgrid[x][y]] == CLOUD_STINK_MON )
        {
            did_anything = true;
            env.cloud_type[env.cgrid[x][y]] += (CLOUD_FIRE - CLOUD_STINK);
        // must find cloud in order to lower its duration
        // is there a better way to find a cloud (e.g. cgrid?)
            for (ctemp = 0; ctemp < MAX_CLOUDS; ctemp++)
            {
                if (env.cloud_x[ctemp] == x && env.cloud_y[ctemp] == y)
                  break;
            }

            env.cloud_decay[ctemp] /= 2;

            if (env.cloud_decay[ctemp] < 1)
              env.cloud_decay[ctemp] = 1;
        }
        else if ( env.cloud_type[env.cgrid[x][y]] == CLOUD_POISON
                   || env.cloud_type[env.cgrid[x][y]] == CLOUD_POISON_MON )
        {
            did_anything = true;
            env.cloud_type[env.cgrid[x][y]] += (CLOUD_FIRE - CLOUD_POISON);
        }
    }

    return ( (int) did_anything );

}          // end ignite_poison_clouds()




static int ignite_poison_monsters(char x, char y, int pow, int garbage)
{
  struct bolt beam[1];
  //FIXME: go through monster inventories for potions + poison weapons
  //FIXME: hurt monsters that have been poisoned
  int monster = mgrd[x][y];
  if (monster == NON_MONSTER)                           return 0;

  int damage = 1 + random2(pow/4) + random2(pow/4) + random2(pow/4) + random2(pow/4);

  //FIXME: apply to monsters with poison attack but not poison corpses?
  if (mons_corpse_thingy( menv[monster].type ) == CE_POISONOUS)
    {
      beam[0].flavour = BEAM_FIRE;
      damage = check_mons_resists(&menv[monster], &beam[0], damage);
      player_hurt_monster(monster, damage);
      return 1;
    }
  return 0;
}




void cast_ignite_poison( int pow )
{

    int damage = 0, strength = 0;
    char item;

// temp weapon of venom => temp fire brand
    if ( you.duration[ DUR_WEAPON_BRAND ] )
      if ( (you.inv_dam[you.equip[EQ_WEAPON]] % 30) == SPWPN_VENOM )
      {
          in_name(you.equip[EQ_WEAPON], 4, str_pass);
          strcpy(info, str_pass);
          strcat(info, " bursts into flame!");
          mpr(info);

          you.inv_dam[you.equip[EQ_WEAPON]] += (SPWPN_FLAMING - SPWPN_VENOM);
          wield_change = true;
          you.duration[ DUR_WEAPON_BRAND ] = 1 + you.duration[ DUR_WEAPON_BRAND ] / 2;
      }

    for (item = 0; item < ENDOFPACK; item++)
    {
        if ( !you.inv_quantity[item] )
          continue;

        if ( you.inv_class[item] == OBJ_MISSILES )
          if ( you.inv_dam[item] == 3 )
          { // burn poison ammo
              strength += you.inv_quantity[item];
              you.inv_quantity[item] = 0;
              you.num_inv_items = 0;
              mpr("Some ammo you are carrying burns!");
          }

        if ( you.inv_class[item] == OBJ_POTIONS )
          switch ( you.inv_type[item] )
          {
              case POT_STRONG_POISON:
                strength += 20 * you.inv_quantity[item];
              case POT_DEGENERATION:
                strength += 10 * you.inv_quantity[item];
              case POT_POISON:
                strength += 10 * you.inv_quantity[item];
                sprintf(info, "%s potion%s you are %s explode%s!",
                you.inv_quantity[item] > 1 ? "Some" : "A",
                you.inv_quantity[item] > 1 ? "s" : "",
                you.equip[EQ_WEAPON] == item ? "wielding" : "carrying",
                you.inv_quantity[item] > 1 ? "" : "s");
                mpr(info);
                you.inv_quantity[item] = 0;
                you.num_inv_items--;
                if (item == you.equip[EQ_WEAPON])
                {
                    you.equip[EQ_WEAPON] = -1;
                    mpr("You are now empty handed.");
                }
          }
    }

    if ( strength )
      place_cloud(CLOUD_FIRE, you.x_pos, you.y_pos, random2(strength/4 +1) + random2(strength/4 +1) + random2(strength/4 +1) + random2(strength/4 +1) + 1);

    if ( you.mutation[MUT_SPIT_POISON] || you.mutation[MUT_STINGER] )
      goto poisonous_player;

    switch ( you.attribute[ATTR_TRANSFORMATION] )
    {
        case TRAN_NONE:
          switch ( you.species )
          {
              case SP_GREEN_DRACONIAN:
              case SP_KOBOLD:
              case SP_NAGA:
                goto poisonous_player;
              default:
                goto do_poison;
          }
         goto do_poison;

        case TRAN_SPIDER:
          goto poisonous_player;

        default:
          goto do_poison;
    }

poisonous_player:
    damage = random2(pow/4) + random2(pow/4) + random2(pow/4) + random2(pow/4);

do_poison:
    for (int i = 0; i < you.poison; i++)
      damage += 1 + random2(6);

    if ( damage )
    {
        if ( player_res_fire() > 0 )
        {
            mpr("You feel like your blood is boiling!");
            damage = damage / 3;
        }
        else if ( player_res_fire() < 0 )
        {
            damage *= 3;
            mpr("The poison in your system burns terribly!");
        }
        else
          mpr("The poison in your system burns!");

        ouch(damage, 0, KILLED_BY_TARGETTING);
    }

    apply_area_visible(ignite_poison_clouds, pow);
    apply_area_visible(ignite_poison_objects, pow);
    apply_area_visible(ignite_poison_monsters, pow);

}          // end cast_ignite_poison()




void cast_silence( int pow )
{

    if ( !you.attribute[ATTR_WAS_SILENCED] )
      mpr("A profound silence engulfs you.");

    you.attribute[ATTR_WAS_SILENCED] = 1;

    you.duration[DUR_SILENCE] += 30 + pow;

    if ( you.duration[ DUR_SILENCE ] > 250 )        // seems a bit high to me {dlb}
      you.duration[ DUR_SILENCE ] = 250;

}          // end cast_silence()



/* ******************************************************************
// no hooks for this anywhere {dlb}:

void cast_animate_golem(int pow)
{
  // must have more than 20 max_hitpoints

  // must be wielding a Scroll of Paper (for chem)

  // must be standing on a pile of <foo> (for foo in: wood, metal, rock, stone)

  // Will cost you 5-10% of max_hitpoints, or 20 + some, whichever is more
  mpr("You imbue the inanimate form with a portion of your life force.");

  naughty(NAUGHTY_CREATED_LIFE, 10);
}

****************************************************************** */



static int discharge_monsters(char x, char y, int pow, int garbage)
{

    struct bolt beam[1];
    int damage = 0, mon = mgrd[x][y];
    if (mon == NON_MONSTER)
      return 0;
    else if (mons_res_elec(menv[mon].type) /*|| mons_flies(menv[mon].type)*/)
      return 0;
    else
    {
        damage = random2(pow) + random2(pow) + random2(pow) + random2(pow);
        damage /= 9;
        damage += 4;
        //damage = 3 + random2(12) + random2(9) + (random2(pow) / 12) + (random2(pow) / 12);

        beam[0].flavour = BEAM_ELECTRICITY;

        damage = check_mons_resists(&menv[mon], &beam[0], damage);

        if ( damage )
        {
            strcpy(info, "You zap ");
            strcat(info, monam(menv[mon].number, menv[mon].type, menv[mon].enchantment[2], 1) );
            strcat(info, ".");
            mpr(info);
        }

        player_hurt_monster(mon, damage);
    }

    return damage;

}          // end discharge_monsters()




void cast_discharge( int pow )
{

    apply_area_around_player(discharge_monsters, pow);

}          // end cast_discharge()




// NB: this must be checked against the same effects
// in fight.cc for all forms of attack !!! {dlb}
static int distortion_monsters( char x, char y, int pow, int message )
{

    int specdam = 0;
    int monster_attacked = mgrd[x][y];

    if ( monster_attacked == NON_MONSTER )
      return 0;

    struct monsters *defender = &menv[monster_attacked];

    if ( pow > 100 )
      pow = 100;

    if ( x == you.x_pos && y == you.y_pos )
    {
        if (you.skills[SK_TRANSLOCATIONS] < random2(8))
          miscast_effect(SPTYP_TRANSLOCATION, pow/9 +1, pow, 100);
        else
          miscast_effect(SPTYP_TRANSLOCATION, 1, 1, 100);

        return 1;
    }

    if ( defender->type == MONS_BLINK_FROG )    // any others resist?
    {
        int hp = defender->hit_points;
        int max_hp = defender->max_hit_points;

        mpr("The blink frog basks in the translocular energy.");

        if ( hp < max_hp )
          hp += 1 + random2(1 + pow/4) + random2(1 + pow/7);

        if ( hp > max_hp )
          hp = max_hp;

        defender->hit_points = hp;

        return 1;
    }
    else if ( coinflip() )
    {
        strcpy(info, "Space bends around ");
        strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
        strcat(info, ".");
        mpr(info);
        specdam += 1 + random2(1 + pow/4) + random2(pow/7);
    }
    else if ( coinflip() )
    {
        strcpy(info, "Space warps horribly around ");
        strcat(info, monam(defender->number, defender->type, defender->enchantment[2], 1));
        strcat(info, "!");
        mpr(info);
        specdam += 3 + random2(pow/4) + random2(pow/4) + random2(pow/4) + random2(pow/4);
    }
    else if ( one_chance_in(3) )
    {
        monster_blink(defender);
        return 1;
    }
    else if ( one_chance_in(3) )
    {
        monster_teleport(defender, coinflip());
        return 1;
    }
    else if ( one_chance_in(3) )
    {
        monster_die(defender, KILL_RESET, 0);
        return 1;
    }
    else if ( message )
    {
        mpr("Nothing seems to happen.");
        return 1;
    }

    player_hurt_monster(monster_attacked, specdam);

    return specdam;

}          // end distortion_monsters()




void cast_warp_field( int pow )
{

    apply_area_around_player(distortion_monsters, pow);

}          // end cast_warp_field()




void cast_bend( int pow )
{

    apply_one_neighboring_square(distortion_monsters, pow);

}          // end cast_bend()




static int make_a_rot_cloud( char x, char y, int pow, int ctype )
{

    int next = 0, obj = mgrd[x][y];

    if (obj == NON_MONSTER)
      return 0;

    while (obj != 501) // is 501 == NON_MONSTER?
    {
        next = mitm.link[obj];

        if ( mitm.base_type[obj] == OBJ_CORPSES && mitm.sub_type[obj] == CORPSE_BODY )
        {
            if ( !mons_skeleton(mitm.pluses[obj]) )
              destroy_item(obj);
            else
            {
                mitm.sub_type[obj] = CORPSE_SKELETON;
                mitm.special[obj]  = 200;
                mitm.colour[obj]   = LIGHTGREY;
            }

            place_cloud(ctype, x, y, (3 + random2(pow/4) + random2(pow/4) + random2(pow/4)));

            return 1;
        }

        obj = next;
    }

    return 0;

}          // end make_a_rot_cloud()




int make_a_normal_cloud( char x, char y, int pow, int ctype )
{

    place_cloud(ctype, x, y, (3 + random2(pow/4) + random2(pow/4) + random2(pow/4)));

    return 1;

}          // end make_a_normal_cloud()




static int make_a_random_cloud( char x, char y, int pow, int ctype )
{

    if ( ctype == CLOUD_NONE )
      ctype = CLOUD_BLACK_SMOKE;

    unsigned char cloud_material;

    switch ( random2(9) )
    {
      case 0:
        cloud_material = CLOUD_FIRE;
        break;
      case 1:
        cloud_material = CLOUD_STINK;
        break;
      case 2:
        cloud_material = CLOUD_COLD;
        break;
      case 3:
        cloud_material = CLOUD_POISON;
        break;
      case 4:
        cloud_material = CLOUD_BLUE_SMOKE;
        break;
      case 5:
        cloud_material = CLOUD_STEAM;
        break;
      case 6:
        cloud_material = CLOUD_PURP_SMOKE;
        break;
      default:
        cloud_material = ctype;
        break;
    }

// that last bit is equivalent to "random2(pow/4) + random2(pow/4) + random2(pow/4)" {dlb}
// can you see the pattern? {dlb}
    place_cloud(cloud_material, x, y, 3 + random2avg(3 * (pow / 4) - 2, 3));

    return 1;

}          // end make_a_random_cloud()




static int passwall( char x, char y, int pow, int garbage )
{

    char dx, dy, nx = x, ny = y;
    int howdeep = 0, shallow;
    bool done = false;

    shallow = 1 + (you.skills[SK_EARTH_MAGIC] / 8);

    if ( !( grd[x][y] == DNGN_ROCK_WALL || grd[x][y] == DNGN_STONE_WALL
            || grd[x][y] == DNGN_SECRET_DOOR ) ) // allow statues as entry points?
    // Irony: you can start on a secret door but not a door.
    {
      mpr("That's not a passable wall.");
      return 0;
    }

    dx = x - you.x_pos;
    dy = y - you.y_pos;

    while ( !done )
    {
    // I'm trying to figure proper borders out {dlb}
      if ( nx > (GXM - 1) || ny > (GYM - 1) || nx < 2 || ny < 2 ) // FIXME: dungeon border?
        {
          mpr("You sense an overwhelming volume of rock.");
          return 0;
        }

        switch ( grd[nx][ny] )
        {
            default:
              done = true;
              break;
            case DNGN_STONE_WALL:
            case DNGN_ROCK_WALL:
            case DNGN_ORCISH_IDOL:
            case DNGN_GRANITE_STATUE:
            case DNGN_SECRET_DOOR:
              nx += dx;
              ny += dy;
              howdeep++;
              break;
        }
    }

    if ( howdeep > shallow )
    {
        mpr("This rock feels deep.");

        if ( yesno("Try anyway?") )
        {
            if ( 3 + random2(pow/3) < howdeep )
            {
                ouch(1 + you.hp, 0, KILLED_BY_PETRIFICATION);
                //jmf: not return; if wizard, successful transport is option
            }
            else
              goto transport;
        }
        else
        {
            if ( one_chance_in(30) )
              mpr("Wuss.");
            else
              canned_msg(MSG_OK);
            return 1;
        }
    }

 transport:

    switch ( grd[nx][ny] )
    {
        //case DNGN_ROCK_WALL:
        //case DNGN_STONE_WALL:
        case DNGN_METAL_WALL:
        case DNGN_GREEN_CRYSTAL_WALL:
        case DNGN_WAX_WALL:
        case DNGN_SILVER_STATUE:
        case DNGN_ORANGE_CRYSTAL_STATUE:
          ouch(1 + you.hp, 0, KILLED_BY_PETRIFICATION);
          break;
        case DNGN_SECRET_DOOR: // oughtn't happen
        case DNGN_CLOSED_DOOR: // open the door
          grd[nx][ny] = DNGN_OPEN_DOOR;
          break;
        default:
          break;
   }

   if ( mgrd[nx][ny] != NON_MONSTER )
   {
   //jmf: hmm, what to do. kill the monster? (seems too powerful)
   //     displace the monster? randomly teleport the monster?
   //     This seems fair: try to move the monster, but if not
   //     able to, then kill it.
       monster_blink(&menv[mgrd[nx][ny]]);

       if ( mgrd[nx][ny] != NON_MONSTER )
         monster_die(&menv[mgrd[nx][ny]], KILL_YOU, 0);
   }

    you.x_pos = nx;
    you.y_pos = ny;

//jmf: FIXME: deal with being in a pool of water / lava / whatever?
    return 1;

}          // end passwall()




void cast_passwall( int pow )
{

    apply_one_neighboring_square(passwall, pow);

}          // end cast_passwall()




static int intoxicate_monsters( char x, char y, int pow, int garbage )
{
    int mon = mgrd[x][y];

    if ( mon == NON_MONSTER )                                  return 0;
    if ( mons_intel(menv[mon].type) < I_NORMAL )       return 0;
    if ( mons_holiness(menv[mon].type) != MH_NATURAL ) return 0;
    if ( mons_res_poison(menv[mon].type) > 0 )         return 0;

    enchant_monster(mon, ENCH_CONFUSION);
    return 1;
}          // end intoxicate_monsters()




void cast_intoxicate( int pow )
{

    potion_effect(POT_CONFUSION, 10 + pow/10);

    if ( one_chance_in(20) && lose_stat(STAT_INTELLIGENCE, 1 + random2(3)) )
      mpr("Your head spins!");

    apply_area_visible(intoxicate_monsters, pow);

}          // end cast_intoxicate()




#ifdef USE_ELVISH_GLAMOUR_ABILITY
// intended as a high-level Elven (a)bility
static int glamour_monsters( char x, char y, int pow, int garbage )
{
    int mon = mgrd[x][y];

    if ( mon == NON_MONSTER )                              return 0;
    if ( mons_intel(menv[mon].type) < I_NORMAL )   return 0;
    if ( one_chance_in(3) )                        return 0;

    switch ( mons_charclass(menv[mon].type) )
    {
      case 'o': // orcs resist thru hatred of elves
      case 'e': // elves resist cause they're elves
        pow = (pow / 2) + 1;
        break;
      case 'c':
      case 'C':
      case 'O':
      case 'u':
      case 'K':
      case 'N':
      case '@':
      case 'T':
        break;
      case 'g':
        if ( menv[mon].type == MONS_GARGOYLE
              || menv[mon].type == MONS_METAL_GARGOYLE
              || menv[mon].type == MONS_MOLTEN_GARGOYLE
              || menv[mon].type == MONS_BOGGART ) return 0;
        break;
      default:                                    return 0;
    }

    if ( !check_mons_magres(&menv[mon], pow) )
      return 0;

    switch (random2(6))
    {
      case 0:
        enchant_monster(mon, ENCH_FEAR);
        break;
      case 1:
      case 4:
        enchant_monster(mon, ENCH_CONFUSION);
        break;
      case 2:
      case 5:
        if ( enchant_monster(mon, ENCH_CHARM) )
          menv[mon].behavior = BEH_ENSLAVED;
        break;
      case 3:
        menv[mon].behavior = BEH_SLEEP;
        break;
    }

// why no, there's no message as to which effect happened >:^)

    if ( !one_chance_in(4) )
    {
        strcpy(info, monam(menv[mon].number, menv[mon].type, menv[mon].enchantment[2], 0));

        switch (random2(4))
        {
          case 0:
            strcat(info, " looks dazed.");
            break;
          case 1:
            strcat(info, " blinks several times.");
            break;
          case 2:
            strcat(info, " rubs its eye");
            if (menv[mon].type != MONS_CYCLOPS) strcat(info, "s");
            strcat(info, ".");
            break;
          case 4:
            strcat(info, " tilts its head.");
            break;
        }

        mpr(info);
    }

    return 1;

}          // end glamour_monsters()




void cast_glamour( int pow )
{

    apply_area_visible(glamour_monsters, pow);

}          // end cast_glamour()
#endif




int backlight_monsters( char x, char y, int pow, int garbage )
{

    int mon = mgrd[x][y];
    //int power = pow + (30 - you.experience_level); // FIXME: bonus or no?

    if (mon == NON_MONSTER)
      return 0;

    //if ( !check_mons_magres(&menv[mon], pow+10) )
    //  return 0;

    switch (menv[mon].type)
    {
      //case MONS_INSUBSTANTIAL_WISP: //jmf: I'm not sure if these glow or not
      //case MONS_VAPOUR:
    case MONS_UNSEEN_HORROR: // consider making this visible? probably not.
      return 0;
    case MONS_FIRE_VORTEX:
    case MONS_ANGEL:
    case MONS_FIEND:
    case MONS_SHADOW:
    case MONS_EFREET:
    case MONS_HELLION:
    case MONS_GLOWING_SHAPESHIFTER:
    case MONS_FIRE_ELEMENTAL:
    case MONS_AIR_ELEMENTAL:
    case MONS_SHADOW_FIEND:
    case MONS_SPECTRAL_WARRIOR:
    case MONS_ORANGE_RAT:
    case MONS_BALRUG:
    case MONS_SPATIAL_VORTEX:
    case MONS_PIT_FIEND:
    case MONS_SHINING_EYE:
    case MONS_DAEVA:
    case MONS_SPECTRAL_THING:
    case MONS_EYE_OF_DEVASTATION:
      return 0; // already glowing or invisible
    default:
      break;
    }

    strcpy(info, monam(menv[mon].number, menv[mon].type, menv[mon].enchantment[2], 0));
    strcat(info, " is outlined in light.");

    if (menv[mon].enchantment[2] == ENCH_NONE || menv[mon].enchantment[2] == ENCH_INVIS)
    {
        menv[mon].enchantment[2] = ENCH_BACKLIGHT_IV;
        menv[mon].enchantment1 = 1;
        //menv[mon].evasion -= 5; // FIXME: tune this?

        //if (menv[mon].evasion < 0)
        //  menv[mon].evasion = 0;

        mpr(info);

        return 1;
    }

    return 0;

}          // end backlight_monsters()




void cast_evaporate( int pow )
{

    if ( you.equip[EQ_WEAPON] == -1
           || you.inv_class[you.equip[EQ_WEAPON]] != OBJ_POTIONS )
    {
        sprintf(info, "Wisps of steam play over your %s!", your_hand(1));
        mpr(info);
        return;
    }

    switch ( you.inv_type[you.equip[EQ_WEAPON]] )
    {
        case POT_DEGENERATION:
        case POT_STRONG_POISON:
          apply_area_cloud(make_a_normal_cloud, you.x_pos, you.y_pos, 3 + pow * 3, 5 + random2avg(9,2), CLOUD_POISON);
          break;
        case POT_POISON:
          apply_area_cloud(make_a_normal_cloud, you.x_pos, you.y_pos, 5 + pow, 3 + random2avg(7,2), CLOUD_POISON);
          break;
        case POT_DECAY:
          apply_area_cloud(make_a_normal_cloud, you.x_pos, you.y_pos, 2 + pow, 6 + random2avg(5,2), CLOUD_MIASMA);
          if ( you.species != SP_MUMMY )
          {
              mpr("You smell decay.");
              if ( one_chance_in(4) )
                you.rotting += 1 + random2(5);
          }
          break;
        case POT_CONFUSION:
        case POT_SLOWING:
        case POT_PARALYSIS:
          if ( you.species != SP_MUMMY )    // mummies can't smell
            mpr("Whew! That stinks!");
          apply_area_cloud(make_a_normal_cloud, you.x_pos, you.y_pos, 8 + pow/3, 3 + random2avg(7,2), CLOUD_STINK);
          break;
        case POT_MUTATION:
        case POT_MAGIC:
          apply_area_cloud(make_a_random_cloud, you.x_pos, you.y_pos, 5 + pow, 5 + random2avg(7,2), CLOUD_STEAM);
          break;
        case POT_WATER:
          apply_area_cloud(make_a_normal_cloud, you.x_pos, you.y_pos, 5 + pow/2, 3 + random2avg(7,2), CLOUD_STEAM);
          break;
        default:
          if ( coinflip() )
            apply_area_cloud(make_a_random_cloud, you.x_pos, you.y_pos, 5 + pow, 5 + random2avg(7,2), CLOUD_BLACK_SMOKE);
          else
            apply_area_cloud(make_a_normal_cloud, you.x_pos, you.y_pos, 5 + pow/2, 3 + random2avg(7,2), CLOUD_STEAM);
          break;
    }

    you.inv_quantity[you.equip[EQ_WEAPON]]--;

    if ( you.inv_quantity[you.equip[EQ_WEAPON]] == 0 )
    {
        unwield_item(you.equip[EQ_WEAPON]);
        you.num_inv_items --;
        you.equip[EQ_WEAPON] = -1;
        mpr("You are now empty handed.");
    }

    wield_change = true;
    burden_change();

    return;

}          // end cast_evaporate()




void make_shuggoth( char x, char y, int hp )
{

    int mon = create_monster(MONS_SHUGGOTH, 100 + random2avg(58,3), BEH_INSANE,
                             x, y, MHITNOT, 250);

    if ( mon != -1 )
    {
        menv[mon].hit_points = hp;
        menv[mon].max_hit_points = hp;
    }

    return;

}          // end make_shuggoth()




static int rot_living(char x, char y, int pow, int message)
{

    int mon = mgrd[x][y];
    int ench;

    if ( mon == NON_MONSTER )
      return 0;

    if ( mons_holiness(menv[mon].type) != MH_NATURAL )
      return 0;

    if ( !check_mons_magres(&menv[mon], pow) )
      return 0;

    ench = ((random2(pow) + random2(pow) + random2(pow) + random2(pow)) / 4);

    if (ench >= 50)
      ench = ENCH_YOUR_ROT_IV;
    else if (ench >= 35)
      ench = ENCH_YOUR_ROT_III;
    else if (ench >= 20)
      ench = ENCH_YOUR_ROT_II;
    else
      ench = ENCH_YOUR_ROT_I;

    enchant_monster(mon, ench);

    return 1;

}          // end rot_living()




static int rot_undead(char x, char y, int pow, int garbage)
{

    int mon = mgrd[x][y];
    int ench;

    if ( mon == NON_MONSTER )
      return 0;

    if ( mons_holiness(menv[mon].type) != MH_UNDEAD )
      return 0;

    if ( !check_mons_magres(&menv[mon], pow) )
      return 0;


// this does not make sense -- player mummies are
// immune to rotting (or have been) -- so what is
// the schema in use here to determine rotting??? {dlb}

    //jmf: up for discussion. it is clearly unfair to
    //     rot player mummies.
    //     the `shcema' here is: corporeal non-player undead
    //     rot, discorporeal undead don't rot. if you wanna
    //     insist that monsters get the same treatment as
    //     players, I demand my player mummies get to worship
    //     the evil mummy & orc god.
    switch(menv[mon].type)
    {
        case MONS_NECROPHAGE:
        case MONS_ZOMBIE_SMALL:
        case MONS_LICH:
        case MONS_MUMMY:
        case MONS_VAMPIRE:
        case MONS_ZOMBIE_LARGE:
        case MONS_WIGHT:
        case MONS_GHOUL:
        case MONS_BORIS:
        case MONS_ANCIENT_LICH:
        case MONS_VAMPIRE_KNIGHT:
        case MONS_VAMPIRE_MAGE:
        case MONS_GUARDIAN_MUMMY:
        case MONS_GREATER_MUMMY:
        case MONS_MUMMY_PRIEST:
          break;
        case MONS_ROTTING_HULK:
        default:
          return 0; // immune (no flesh) or already rotting
    }

    ench = ((random2(pow) + random2(pow) + random2(pow) + random2(pow)) / 4);

    if (ench >= 50)
      ench = ENCH_YOUR_ROT_IV;
    else if (ench >= 35)
      ench = ENCH_YOUR_ROT_III;
    else if (ench >= 20)
      ench = ENCH_YOUR_ROT_II;
    else
      ench = ENCH_YOUR_ROT_I;

    enchant_monster(mon, ench);

    return 1;

}          // end rot_undead()




static int rot_corpses( char x, char y, int pow, int garbage )
{

    return make_a_rot_cloud(x,y,pow,CLOUD_MIASMA);

}          // end rot_corpses()




void cast_rotting( int pow )
{

    apply_area_visible(rot_living, pow);

    apply_area_visible(rot_undead, pow);

    apply_area_visible(rot_corpses, pow);

    return;

}          // end cast_rotting()



void do_monster_rot( int mon )
{
    int damage = 1 + random2(3);

    if (mons_holiness(menv[mon].type) == MH_UNDEAD && random2(5))
      apply_area_cloud(make_a_normal_cloud, menv[mon].x, menv[mon].y,
                       10, 1, CLOUD_MIASMA);

    player_hurt_monster(mon, damage);

    return;
}          // end do_monster_rot()



static int snake_charm_monsters( char x, char y, int pow, int message )
{
  int mon = mgrd[x][y];

  if ( mon == NON_MONSTER )                              return 0;
  if ( menv[mon].behavior == BEH_ENSLAVED )      return 0;
  if ( one_chance_in(4) )                        return 0;
  if ( mons_charclass(menv[mon].type) != 'S' )   return 0;
  if ( !check_mons_magres(&menv[mon], pow ) )    return 0;

  menv[mon].behavior = BEH_ENSLAVED;
  sprintf(info, "%s sways back and forth a few times.",
          monam(menv[mon].number, menv[mon].type, menv[mon].enchantment[2],0));
  mpr(info);

  return 1;
}


void cast_snake_charm( int pow )
{
  // powc = (you.experience_level * 2) + (you.skills[SK_INVOCATIONS] * 3);
  apply_one_neighboring_square( snake_charm_monsters, pow );
}



void cast_fragmentation(int pow) // jmf: ripped idea from airstrike
{

    struct dist beam[1];
    struct bolt blast[1];
    int debris = 0;
    int i, hurted;
    bool size = false;
    bool explode = false;
    char * what = 0;

// most of hurted seems to be saved against
    hurted = random2(pow) + random2(pow) + random2(pow) + random2(pow);
    hurted /= 2;
    hurted += 10 + random2(12);

    mpr("Fragment what (e.g. a wall)?");
    direction(100, beam);

    if (beam[0].nothing == -1)
    {
        canned_msg(MSG_SPELL_FIZZLES);
        return;
    }

//FIXME: if (player typed '>' to attack floor) goto do_terrain;

    blast[0].bx = beam[0].target_x;
    blast[0].by = beam[0].target_y;
    blast[0].thing_thrown = 1;
    blast[0].type = '#';
    blast[0].colour = 0;

    i = mgrd[beam[0].target_x][beam[0].target_y];

    if (i != NON_MONSTER)
    {
        //struct monsters *monster = &menv[i];
        strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));

        switch (menv[i].type)
        {
            case MONS_ICE_BEAST:
            case MONS_ICE_DRAGON:
            case MONS_ICE_DEVIL:
            case MONS_ICE_FIEND: // blast of ice fragments
              explode = true;
              strcpy(blast[0].beam_name, "icy blast");
              blast[0].colour = WHITE;
              blast[0].damage = hurted / 4 + 1;
              blast[0].flavour = BEAM_ICE;
              player_hurt_monster(i, hurted);
              break;

            case MONS_FLYING_SKULL:
            case MONS_SKELETON_SMALL:
            case MONS_SKELETON_LARGE: // blast of bone
              explode = true;
              sprintf(info, "The sk%s explodes into sharp fragments of bone!",
                      (menv[i].type == MONS_FLYING_SKULL) ? "ull" : "eleton");
              strcpy(blast[0].beam_name, "blast of bone shards");

              blast[0].colour = LIGHTGREY;
              blast[0].damage = 1 + (hurted / 8);
              blast[0].flavour = BEAM_FRAG;

              if (random2(50) < (pow/5)) // potential insta-kill
                monster_die(&menv[i], KILL_YOU, 0);
              else
                player_hurt_monster(i, hurted);
              goto all_done; // i.e. no "Foo Explodes!"

            case MONS_WOOD_GOLEM:
            case MONS_MOLTEN_GARGOYLE: // major damage; monster only
              explode = false;
              //FIXME: simple_monster_message(monster, " shudders violently!");
              player_hurt_monster(i, hurted + hurted / 2);
              break;

            case MONS_IRON_DEVIL:
            case MONS_IRON_GOLEM:
            case MONS_METAL_GARGOYLE:
            case MONS_QUICKSILVER_DRAGON:
            case MONS_IRON_DRAGON:
              explode = true;
              strcpy(blast[0].beam_name, "blast of metal fragments");
              blast[0].colour = CYAN;
              blast[0].damage = hurted / 2 + 1;
              blast[0].flavour = BEAM_FRAG;
              player_hurt_monster(i, hurted);
              break;

            case MONS_CLAY_GOLEM: // assume baked clay and not wet loam
            case MONS_STONE_GOLEM:
            case MONS_EARTH_ELEMENTAL:
            case MONS_GARGOYLE:
              explode = true;
              size = true;
              strcpy(blast[0].beam_name, "blast of rock fragments");
              blast[0].colour = BROWN;
              blast[0].damage = hurted / 4 + 1;
              blast[0].flavour = BEAM_FRAG;
              player_hurt_monster(i, hurted + hurted/2);
              break;

            case MONS_CRYSTAL_GOLEM:
              explode = true;
              size = true;
              strcpy(blast[0].beam_name, "blast of crystal shards");
              blast[0].colour = WHITE;
              blast[0].damage = hurted / 2 + 1;
              blast[0].flavour = BEAM_FRAG;
              player_hurt_monster(i, (hurted << 1));
              break;

            default:
              player_hurt_monster(i, 1 + (hurted/30));
              goto do_terrain;
        }

        strcat(info, " explodes!");
        mpr(info);
        goto all_done;
    }

 do_terrain:
    // FIXME: do nothing in Abyss & Pandemonium?
    i = grd[beam[0].target_x][beam[0].target_y];

    switch (i)
    {
    case DNGN_ROCK_WALL:
    case DNGN_STONE_WALL:
    case DNGN_SECRET_DOOR:
      what = "wall";
    case DNGN_ORCISH_IDOL:
      if (what == 0)
        what = "stone idol";
    case DNGN_GRANITE_STATUE: // normal rock -- big explosion
      if (what == 0)
        what = "statue";
      explode = true;
      size = true;
      strcpy(blast[0].beam_name, "blast of rock fragments");
      blast[0].colour = BROWN; // FIXME: colour of actual wall?
      blast[0].damage = hurted / 4;
      blast[0].flavour = BEAM_FRAG;

      if ( coinflip() )
        {
          grd[beam[0].target_x][beam[0].target_y] = DNGN_FLOOR;
          debris = DEBRIS_ROCK;
        }
      break;

    case DNGN_METAL_WALL: // metal -- small but nasty explosion
      what = "metal wall";
      if ( one_chance_in(3) )
        {
          grd[beam[0].target_x][beam[0].target_y] = DNGN_FLOOR;
          debris = DEBRIS_METAL;
        }

      blast[0].colour = CYAN;
      // fallthru
    case DNGN_SILVER_STATUE: //jmf: statue not destroyed
      if (what == 0)
        {
          what = "silver statue";
          blast[0].colour = WHITE;
        }
      explode = true;
      strcpy(blast[0].beam_name, "blast of metal fragments");
      blast[0].damage = hurted;
      blast[0].flavour = BEAM_FRAG;
      break;

    case DNGN_GREEN_CRYSTAL_WALL: // crystal -- large & nasty explosion
      what = "crystal wall";
      blast[0].colour = GREEN;

      if ( one_chance_in(3) )
        {
          grd[beam[0].target_x][beam[0].target_y] = DNGN_FLOOR;
          debris = DEBRIS_CRYSTAL;
        }
      // fallthru
    case DNGN_ORANGE_CRYSTAL_STATUE:
      if (what == 0)
        what = "crystal statue";
      if (blast[0].colour == 0)
        blast[0].colour = LIGHTRED; //jmf: == orange, right?

      explode = true;
      size = true;
      strcpy(blast[0].beam_name, "blast of crystal shards");
      blast[0].damage = hurted;
      blast[0].flavour = BEAM_FRAG;
      break;

    case DNGN_TRAP_MECHANICAL:
      if ( coinflip() )
        grd[beam[0].target_x][beam[0].target_y] = DNGN_FLOOR;
    case DNGN_TRAP_MAGICAL:
    case DNGN_TRAP_III:
    case DNGN_UNDISCOVERED_TRAP:
    case DNGN_FLOOR:
    case DNGN_OPEN_DOOR:
    case DNGN_STONE_ARCH: // floor -- small explosion
      what = "floor";
      explode = true;
      strcpy(blast[0].beam_name, "blast of rock fragments");
      blast[0].colour = BROWN; // FIXME: colour of actual dungeon floor?
      blast[0].damage = hurted / 4;
      blast[0].flavour = BEAM_FRAG;
      break;

    default:
      // FIXME: cute message for water?
      break;
    }

 all_done:
    if ( explode )
    {
        if ( what != 0 )
        {
            sprintf(info, "The %s explodes!", what);
            mpr(info);
        }

        explosion(size, &blast[0]);
    }

    if ( debris )
      place_debris(beam[0].target_x, beam[0].target_y, debris);

    return;

}          // end cast_fragmentation()




void cast_sandblast( int pow )
{

    bool big = true;
    struct dist spd[1];
    struct bolt beam[1];
    int hurt = 2 + random2(5) + random2(4) + random2(pow)/20;

    if (you.inv_class[you.equip[EQ_WEAPON]] == 0
         || (you.inv_class[you.equip[EQ_WEAPON]] != OBJ_MISSILES
            && (you.inv_type[you.equip[EQ_WEAPON]] != MI_STONE
                 || you.inv_type[you.equip[EQ_WEAPON]] != MI_LARGE_ROCK)))
      big = false;

    if (spell_direction(spd,&beam[0]) == -1)
      return;

    if (beam[0].move_x == 0 && beam[0].move_y == 0)
    {
        canned_msg(MSG_UNTHINKING_ACT);
        return;
    }

    if (big)
    {
        zapping(ZAP_SANDBLAST, 3 + hurt + hurt/2, &beam[0]);
        you.inv_quantity[you.equip[EQ_WEAPON]] --;
        wield_change = true;

        if ( you.inv_quantity[you.equip[EQ_WEAPON]] < 1 )
        {
            you.equip[EQ_WEAPON] = -1;
            you.num_inv_items--;
            mpr("You are now empty handed.");
        }
    }
    else
    {
        zapping(ZAP_SMALL_SANDBLAST, hurt, &beam[0]);
    }

}          // end cast_sandblast()




static bool mons_can_host_shuggoth( int type ) //jmf: simplified
{
    if ( mons_holiness(type) != MH_NATURAL )
      return false;
    if ( mons_flag((type), M_WARM_BLOOD ) )
      return true;
    return false;
}




void cast_shuggoth_seed( int powc )
{

    struct dist beam[1];
    int i;

    mpr("Sow seed in whom?");

    direction(100, beam);

    if ( beam[0].target_x == you.x_pos && beam[0].target_y == you.y_pos )
    {
        if ( !you.is_undead )
        {
          you.duration[DUR_INFECTED_SHUGGOTH_SEED] = 10;
          mpr("A deathly dread twitches in your chest.");
        }
        else
          mpr("You feel a distant frustration.");
    }

    i = mgrd[beam[0].target_x][beam[0].target_y];

    if (beam[0].nothing == -1 || i == NON_MONSTER)
    {
        mpr("You feel a distant frustration.");
        return;
    }

    if ( mons_can_host_shuggoth(menv[i].type) )
    {
        if (random2(powc) > 100)
          enchant_monster(i, ENCH_YOUR_SHUGGOTH_III);
        else
          enchant_monster(i, ENCH_YOUR_SHUGGOTH_IV);

        simple_monster_message(&menv[i], " twitches.");
    }

    return;

}




void cast_condensation_shield(int pow)
{

    if ( you.equip[EQ_SHIELD] != -1 || you.shock_shield )
      canned_msg(MSG_SPELL_FIZZLES);
    else
    {
        if ( !you.duration[DUR_CONDENSATION_SHIELD] )
        {
            mpr("A crackling disc of dense vapour forms in the air!");
            you.redraw_armor_class = 1;
        }

        you.duration[DUR_CONDENSATION_SHIELD] += 20 + ((random2(pow) + random2(pow)) / 2);

        if ( you.duration[DUR_CONDENSATION_SHIELD] > (10 * you.skills[SK_ICE_MAGIC]) )
          you.duration[DUR_CONDENSATION_SHIELD] = (10 * you.skills[SK_ICE_MAGIC]);
    }

    return;

}          // end cast_condensation_shield()




static int quadrant_blink( char x, char y, int pow, int garbage )
{

    FixedVector<int, 2> passed;
    bool cancel = false;
    bool done = false;
    bool down = (you.y_pos > y);
    bool left = (you.x_pos < x);
    bool right = (you.x_pos > x);
    bool up = (you.y_pos < y);

    if ( x == you.x_pos && y == you.y_pos )
      return 0;

    if (pow > 98)
      pow = 98;

    do
    {
        if ( random_near_space(passed) == 0
              || ( you.x_pos == passed[0] && you.y_pos == passed[1] ) )
        {
            cancel = true;
            pow -= 2;
        }
        else //jmf: FIXME: make a `cone' for non-angle directions
        {
            cancel = false;
            done = true;

            if ( up && passed[1] < you.y_pos )
              done = false;

            if ( down && passed[1] > you.y_pos )
              done = false;
            if ( right && passed[0] < you.x_pos )
              done = false;
            if ( left && passed[0] > you.x_pos )
              done = false;
        }
    }
    while ( !done && !cancel && random2(100) < pow-- );

    if ( cancel )
      return 0;

    you.x_pos = passed[0];
    you.y_pos = passed[1];

    if ( you.level_type == LEVEL_ABYSS )
    {
        abyss_teleport();
        env.cloud_no = 0;
        you.pet_target = MHITNOT;
    }

    return 1;

}




void cast_semi_controlled_blink( int pow )
{
    apply_one_neighboring_square(quadrant_blink, pow);
    return;
}




int torment_monsters( char x, char y, int pow, int garbage )
{
    int mon = mgrd[x][y];
    struct monsters *monster = &menv[mon];

    if ( x == you.x_pos && y == you.y_pos )
    {
        if ( you.is_undead || you.mutation[MUT_TORMENT_RESISTANCE] )
          mpr("You feel a surge of unholy energy.");
        else
        {
            mpr("Your body is wracked with pain!");
            dec_hp((you.hp / 2) - 1, false);
        }

        return 1;
    }

    if ( mon == NON_MONSTER )                                     return 0;
    if ( monster->type == -1 )                            return 0;
    if ( mons_holiness(monster->type) == MH_UNDEAD
         || mons_holiness(monster->type) == MH_DEMONIC )  return 0;

    monster->hit_points = 1 + (monster->hit_points / 2);

    simple_monster_message(monster, " convulses!");

    return 1;
}

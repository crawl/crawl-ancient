/*
 *  File:       monsplace.cc
 *  Summary:    Functions used when placing monsters in the dungeon.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "monplace.h"

#include "externs.h"

#include "dungeon.h"
#include "stuff.h"

char empty_surrounds(int emx, int emy, unsigned char spc_wanted, char allow_centre, char empty[2]);


int mons_place(int typed, int type_place, int px, int py, char behaviour, int hitting, unsigned char plus_seventy, int lev_mons)
{
    int passed[2];

    passed[0] = 0;
    passed[1] = 0;

    char bands = 1;
    int nomons = 0;
    int il;

    for (il = 0; il < MNST; il++)
    {
        if (menv[il].type != -1)
            nomons++;
    }

    if (typed == 2500)          /* random placement */
    {
        if (nomons > 150)
            return -1;
        typed = 250;
    }

    if (nomons > MNST - 2)
        return -1;

    if (you.char_direction == 1 && typed == 250 && you.level_type == LEVEL_DUNGEON)
    {
        typed = 80 + random2(10);
        if ( one_chance_in(5))
            typed = 3;
        if ( one_chance_in(5) )
            typed = 73;
        if ( one_chance_in(7) )
            typed = 23;
        if ( one_chance_in(7) )
            typed = 49;
        if ( one_chance_in(13) )
            typed = 245;
        if ( one_chance_in(3) )
            typed = 220 + random2(15);
    }

    if (typed != 250 && lev_mons != 52)
        bands = 0;

/*if (no_mons == MNST - 1) return 0; */

    int glokp = place_monster(
                                 plus_seventy,
                              typed, type_place, px, py, behaviour, hitting,
                                 bands,
                                 lev_mons, passed);


    if (glokp > 0)
        return passed[0];
    else
        return -1;


}                               /* end of mons_place() */




int create_monster(int cls, int dur, int beha, int cr_x, int cr_y, int hitting, int zsec)
{
  int pets = 0;
  int summd = 0;

  int dem_beha = beha;

  if (beha == BEH_CHASING_II)
    beha = BEH_ENSLAVED;

  unsigned char spcw = DNGN_SHALLOW_WATER;

  if (cls >= MONS_LAVA_WORM)
    spcw = DNGN_LAVA;
  if (cls >= MONS_BIG_FISH)
    spcw = DNGN_DEEP_WATER;

  char empty[2];

  if (empty_surrounds(cr_x, cr_y, spcw, 1, empty) == 0)
    {
puffy:
      if ( see_grid(cr_x, cr_y) )
        {
          strcpy(info, "You see a puff of smoke.");
          mpr(info);
        }
      return -1;
    }
  else
    {
      summd = mons_place(cls, 1, empty[0], empty[1], beha, hitting, zsec, you.your_level);
      if (summd == -1)
        goto puffy;
        goto bkout;
    }                   // end else

bkout:
  pets = 0;

/* This is for the summon greater demons spell, where monsters are hostile
   but charmed: */

  if (dem_beha == BEH_CHASING_II)
    menv[summd].enchantment[0] = ENCH_CHARM;

  if (dur != 0)
    {
      menv[summd].enchantment[1] = dur;     // some monsters, eg butterflies, use [0] for confusion
      menv[summd].enchantment1 = 1;

      if (beha == BEH_ENSLAVED)
        menv[summd].enchantment[1] += ENCH_FRIEND_ABJ_I - ENCH_ABJ_I;
    }
  else
    {
      if (beha == BEH_ENSLAVED)
        menv[summd].enchantment[1] = ENCH_CREATED_FRIENDLY;     // means no xp, no piety for a kill
    }

  if (menv[summd].type == MONS_RAKSHASA_FAKE && !one_chance_in(3) )
    {
      menv[summd].enchantment[2] = ENCH_INVIS;
      menv[summd].enchantment1 = 1;
    }

  return summd;
}




char empty_surrounds(int emx, int emy, unsigned char spc_wanted, char allow_centre, char empty[2])
{

    int count_x, count_y = 0;

    char minx = -1;
    char maxx = 3;
    char miny = -1;
    char maxy = 2;
    char xinc = 1;
    char yinc = 1;

    if ( coinflip() )
    {
        minx = 1;
        maxx = -3;
        xinc = -1;
    }

    if ( coinflip() )
    {
        miny = 1;
        maxy = -2;
        yinc = -1;
    }


    for (count_x = minx; count_x != maxx; count_x += xinc)
    {

        if (count_x == 2 || count_x == -2)
        {
            return 0;
        }

        for (count_y = miny; count_y != maxy; count_y += yinc)
        {

            if (count_x == 0 && count_y == 0 && allow_centre == 0)
                continue;
            if (emx + count_x == you.x_pos && emy + count_y == you.y_pos)
                continue;

            if ( spc_wanted != DNGN_SHALLOW_WATER )
              {
                if ( grd[emx + count_x][emy + count_y] == spc_wanted
                    && mgrd[emx + count_x][emy + count_y] == MNG )
                  goto bkout;
              }
            else if ( grd[emx + count_x][emy + count_y] >= DNGN_SHALLOW_WATER     // huh? something fishy here {dlb}
                     && mgrd[emx + count_x][emy + count_y] == MNG )
              {
                goto bkout;
              }

        }                       /* end of count_y */
    }                           /* end of count_x */

bkout:
    empty[0] = emx + count_x;
    empty[1] = emy + count_y;
    return 1;

}                               /* end empty surrounds */


int summon_any_demon(char demon_class)
{

    if (demon_class == DEMON_LESSER)       // (class 5)
    {
         return table_lookup( 60,
                              MONS_IMP, 50,           // 10 in 60 chance {dlb}
                              MONS_WHITE_IMP, 41,     //  9 in 60 chance {dlb}
                              MONS_LEMURE, 32,        //  9 in 60 chance {dlb}
                              MONS_UFETUBUS, 23,      //  9 in 60 chance {dlb}
                              MONS_MANES, 14,         //  9 in 60 chance {dlb}
                              MONS_MIDGE, 5,          //  9 in 60 chance {dlb}
                              MONS_SHADOW_IMP, 0      //  5 in 60 chance {dlb}
                            );
    }

    if (demon_class == DEMON_COMMON)       // (classes 4 - 2)
    {
        if ( one_chance_in(5) )
            return 80 + random2(10);
        else if ( one_chance_in(20) )
            return 3;
        else if ( one_chance_in(30) )
            return 236 + random2(2);
        else
            return 225 + random2(5);
    }

    if (demon_class == DEMON_GREATER)       // (class 1)
    {
         return table_lookup( 1000,
                              MONS_CACODEMON, 869,    // 131 in 1000 chance {dlb}
                              MONS_BALRUG, 738,       // 131 in 1000 chance {dlb}
                              MONS_BLUE_DEATH, 607,   // 131 in 1000 chance {dlb}
                              MONS_GREEN_DEATH, 476,  // 131 in 1000 chance {dlb}
                              MONS_EXECUTIONER, 345,  // 131 in 1000 chance {dlb}
                              MONS_FIEND, 245,        // 100 in 1000 chance {dlb}
                              MONS_ICE_FIEND, 155,    //  90 in 1000 chance {dlb}
                              MONS_SHADOW_FIEND, 74,  //  81 in 1000 chance {dlb}
                              MONS_PIT_FIEND, 0       //  74 in 1000 chance {dlb}
                            );
    }

    return 0;

}

/*
 *  File:       mon-util.cc
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *   <2>    11/04/99        cdl     added a break to spell selection
 *                                  for kobold Summoners
 *                                  print just "[Ii]t" for invisible undead
 *                                  renamed params to monam()
 *   <1>     -/--/--        LRH     Created
 */

// $pellbinder: (c) D.G.S.E 1998
// some routines snatched from former monsstat.cc

#include "AppHdr.h"
#include "mon-util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "externs.h"

#include "debug.h"
#include "player.h"
#include "stuff.h"


unsigned char mcolour[1000];             // really important extern -- screen redraws suck w/o it {dlb}
char *gmo_n;                    /* used in monam - could possibly be static to that function */


static struct monsterentry mondata[] =
{
#include "mon-data.h"
};


#define MONDATASIZE (sizeof(mondata)/sizeof(struct monsterentry))


// will do as long as spell/sec numbers don't come above 255 or below 0:
/*

   Note: is assumed that most monsters capable of casting the more powerful
   summonings can also cast Abjuration (just for simplicity)

 */
static unsigned char mspell_list[][7] =
{                               // sec  bolt   ench  selfench  misc   misc2  emergency
/*orc wiz              */
    {0,
     MS_MMISSILE,
     MS_SLOW,
     MS_HASTE,
     MS_MMISSILE,
     100,
     MS_BLINK
    },
/*orc wiz              */
    {1,
     MS_FLAME,
     MS_CONFUSE,
     MS_INVIS,
     MS_MMISSILE,
     100,
     100
    },
/*orc wiz              */
    {2,
     MS_FROST,
     100,
     MS_HASTE,
     MS_FLAME,
     MS_MMISSILE,
     100
    },
/*naga                 */
    {10,
     MS_TELEPORT_OTHER,
     MS_TELEPORT_OTHER,
     MS_HEAL,
     MS_VENOM_BOLT,
     MS_SLOW,
     MS_HEAL
    },
/*Lich                 */
    {20,
     MS_COLD_BOLT,
     MS_PARALYSIS,
     MS_SUMMON_DEMON_GREATER,
     MS_ANIMATE_DEAD,
     MS_IRON_BOLT,
     MS_TELEPORT
    },
/*Lich                 */
    {21,
     MS_FIRE_BOLT,
     MS_CONFUSE,
     MS_HASTE,
     MS_NEGATIVE_BOLT,
     MS_SUMMON_DEMON_GREATER,
     MS_BANISHMENT
    },
/*Lich                 */
    {22,
     MS_NEGATIVE_BOLT,
     MS_ANIMATE_DEAD,
     MS_SUMMON_DEMON_GREATER,
     MS_FROST,
     MS_CRYSTAL_SPEAR,
     MS_BLINK
    },
/*Lich                 */
    {23,
     MS_ORB_ENERGY,
     MS_COLD_BOLT,
     MS_INVIS,
     MS_ANIMATE_DEAD,
     MS_IRON_BOLT,
     MS_INVIS
    },
/*burning devil        */
    {30,
     MS_HELLFIRE_BURST,
     MS_HELLFIRE_BURST,
     MS_HELLFIRE_BURST,
     MS_HELLFIRE_BURST,
     MS_HELLFIRE_BURST,
     MS_HELLFIRE_BURST
    },
/*vampire              */
    {40,
     MS_VAMPIRE_SUMMON,
     100,
     100,
     100,
     100,
     MS_VAMPIRE_SUMMON
    },
/*efreet               */
    {50,
     MS_FIRE_BOLT,
     100,
     100,
     MS_FIREBALL,
     100,
     100
    },
/*brain worm           */
    {52,
     MS_BRAIN_FEED,
     100,
     100,
     MS_BRAIN_FEED,
     100,
     100
    },
/*giant orange brain   */
    {53,
     MS_BRAIN_FEED,
     MS_MUTATION,
     MS_CONFUSE,
     MS_LEVEL_SUMMON,
     MS_BLINK,
     MS_TELEPORT
    },
/*R                    */
    {54,
     MS_FAKE_RAKSHASA_SUMMON,
     MS_BLINK,
     MS_INVIS,
     MS_FAKE_RAKSHASA_SUMMON,
     MS_BLINK,
     MS_TELEPORT
    },
/*GooE                 */
    {55,
     MS_PARALYSIS,
     MS_DISINTEGRATE,
     100,
     MS_SLOW,
     MS_CONFUSE,
     MS_TELEPORT_OTHER
    },
/*orc sorceror         */
    {56,
     MS_FIRE_BOLT,
     MS_NEGATIVE_BOLT,
     MS_SUMMON_DEMON,
     52,
     20,
     MS_TELEPORT
    },
/*steam dragon         */
    {57,
     MS_STEAM_BALL,
     MS_STEAM_BALL,
     100,
     MS_STEAM_BALL,
     MS_STEAM_BALL,
     100
    },
/*hell knight          */
    {58,
     100,
     MS_PAIN,
     MS_HASTE,
     100,
     100,
     100
    },
/*hell knight          */
    {59,
     100,
     MS_FIRE_BOLT,
     100,
     100,
     100,
     MS_BLINK
    },
/*necromancer          */
    {60,
     MS_COLD_BOLT,
     MS_NEGATIVE_BOLT,
     100,
     MS_ANIMATE_DEAD,
     MS_ANIMATE_DEAD,
     MS_TELEPORT
    },
/*necromancer          */
    {61,
     MS_FIRE_BOLT,
     MS_PAIN,
     MS_INVIS,
     MS_ANIMATE_DEAD,
     MS_ANIMATE_DEAD,
     MS_BLINK
    },
/*wizard               */
    {62,
     MS_MMISSILE,
     MS_PARALYSIS,
     MS_HASTE,
     MS_LIGHTNING_BOLT,
     MS_CONFUSE,
     MS_TELEPORT
    },
/*wizard               */
    {63,
     MS_VENOM_BOLT,
     MS_ORB_ENERGY,
     MS_INVIS,
     MS_CONFUSE,
     MS_SLOW,
     MS_TELEPORT
    },
/*wizard               */
    {64,
     MS_PARALYSIS,
     MS_CRYSTAL_SPEAR,
     MS_BLINK,
     MS_FIRE_BOLT,
     MS_COLD_BOLT,
     MS_HEAL
    },
/*wizard               */
    {65,
     MS_STONE_ARROW,
     MS_STING,
     MS_BLINK,
     MS_LIGHTNING_BOLT,
     MS_BANISHMENT,
     MS_HEAL
    },
/*wizard               */
    {66,
     MS_PARALYSIS,
     MS_FLAME,
     MS_INVIS,
     MS_TELEPORT_OTHER,
     MS_FIREBALL,
     MS_INVIS
    },
/*orc priest           */
    {67,
     MS_PAIN,
     100,
     MS_SMITE,
     100,
     100,
     MS_HEAL
    },
/*orc high priest      */
    {68,
     MS_PAIN,
     MS_SUMMON_DEMON,
     MS_SMITE,
     MS_SUMMON_DEMON,
     MS_ANIMATE_DEAD,
     MS_HEAL
    },
/*mottled D            */
    {69,
     MS_STICKY_FLAME,
     MS_STICKY_FLAME,
     100,
     100,
     100,
     100
    },
/*Ice Fiend            */
    {70,
     MS_COLD_BOLT,
     MS_COLD_BOLT,
     MS_TORMENT,
     100,
     100,
     MS_SUMMON_DEMON
    },
/*Shadow Fiend         */
    {71,
     MS_COLD_BOLT,
     MS_NEGATIVE_BOLT,
     MS_TORMENT,
     100,
     100,
     MS_SUMMON_DEMON
    },
/*tormentor            */
    {72,
     MS_PAIN,
     100,
     100,
     100,
     100,
     100
    },
/*Storm D + vapour     */
    {73,
     MS_LIGHTNING_BOLT,
     MS_LIGHTNING_BOLT,
     100,
     MS_LIGHTNING_BOLT,
     MS_LIGHTNING_BOLT,
     100
    },
/*WHITE lesser demon   */
    {74,
     MS_FROST,
     100,
     100,
     100,
     100,
     100
    },
/*CYAN demon           */
    {75,
     100,
     MS_IRON_BOLT,
     MS_SUMMON_UFETUBUS,
     100,
     MS_SUMMON_UFETUBUS,
     100
    },
/*MAGENTA demon        */
    {76,
     MS_MUTATION,
     100,
     100,
     MS_BRAIN_FEED,
     MS_SUMMON_DEMON_LESSER,
     100
    },
/*LIGHTGRAY demon (227) */
    {77,
     100,
     100,
     100,
     MS_TELEPORT_OTHER,
     MS_ANIMATE_DEAD,
     MS_TELEPORT
    },
/*LIGHTGRAY demon (228) */
    {78,
     MS_STICKY_FLAME,
     MS_STEAM_BALL,
     MS_SMITE,
     100,
     100,
     100
    },
/*BROWN greater demon       */
    {79,
     MS_SUMMON_DEMON_LESSER,
     MS_SUMMON_DEMON_LESSER,
     MS_MUTATION,
     MS_SUMMON_DEMON_LESSER,
     MS_DIG,
     100
    },
/*GREEN greater demon  */
    {80,
     MS_POISON_BLAST,
     MS_POISON_BLAST,
     100,
     MS_VENOM_BOLT,
     MS_SUMMON_DEMON_LESSER,
     MS_BLINK
    },
/*RED gr demon (233)   */
    {81,
     MS_FIRE_BOLT,
     MS_FIREBALL,
     100,
     MS_STICKY_FLAME,
     MS_SMITE,
     MS_TELEPORT
    },
/*BLUE gr demon        */
    {82,
     MS_LIGHTNING_BOLT,
     MS_COLD_BOLT,
     100,
     MS_SUMMON_DEMON_LESSER,
     MS_LEVEL_SUMMON,
     MS_TELEPORT_OTHER
    },
/*Geryon               */
    {83,
     MS_SUMMON_BEAST,
     100,
     MS_SUMMON_BEAST,
     MS_SUMMON_BEAST,
     100,
     MS_SUMMON_BEAST
    },
/*Dispater             */
    {84,
     MS_SUMMON_DEMON_GREATER,
     MS_IRON_BOLT,
     MS_SUMMON_DEMON,
     MS_LIGHTNING_BOLT,
     MS_HELLFIRE,
     MS_SUMMON_DEMON
    },
/*Asmodeus             */
    {85,
     MS_FIRE_BOLT,
     MS_HELLFIRE,
     MS_SUMMON_DEMON,
     MS_SUMMON_DEMON_GREATER,
     MS_NEGATIVE_BOLT,
     MS_TELEPORT
    },
/*Ereshkigal           */
    {86,
     MS_NEGATIVE_BOLT,
     MS_COLD_BOLT,
     MS_SUMMON_DEMON,
     MS_PAIN,
     MS_PARALYSIS,
     MS_HEAL
    },
/*Antaeus              */
    {87,
     MS_COLD_BOLT,
     MS_LIGHTNING_BOLT,
     100,
     100,
     100,
     100
    },
/*Nemelex Xobeh        */
    {90,
     MS_SUMMON_DEMON,
     MS_SMITE,
     MS_INVIS,
     MS_MUTATION,
     MS_LEVEL_SUMMON,
     MS_TELEPORT
    },
/*Sif Muna             */
    {91,
     MS_LIGHTNING_BOLT,
     MS_COLD_BOLT,
     MS_SUMMON_DEMON,
     MS_BLINK,
     MS_TELEPORT,
     MS_HEAL
    },
/*Okawaru              */
    {92,
     MS_FIRE_BOLT,
     MS_IRON_BOLT,
     100,
     MS_FIREBALL,
     MS_SUMMON_DEMON_LESSER,
     100
    },
/*Kikubaaqudgha        */
    {93,
     MS_POISON_BLAST,
     MS_SLOW,
     100,
     MS_NEGATIVE_BOLT,
     MS_SUMMON_DEMON,
     MS_SUMMON_DEMON
    },
/*Titan                */
    {94,
     MS_LIGHTNING_BOLT,
     100,
     100,
     100,
     100,
     MS_HEAL
    },
/*Golden dragon        */
    {95,
     MS_FIRE_BOLT,
     MS_COLD_BOLT,
     100,
     100,
     MS_POISON_BLAST,
     100
    },
/*deep elf summoner    */
    {96,
     MS_BLINK,
     MS_SUMMON_DEMON_LESSER,
     MS_SUMMON_UFETUBUS,
     MS_VAMPIRE_SUMMON,
     MS_SUMMON_DEMON,
     MS_TELEPORT
    },
/*deep elf conjurer    */
    {97,
     MS_FIRE_BOLT,
     MS_COLD_BOLT,
     100,
     MS_LIGHTNING_BOLT,
     MS_STICKY_FLAME,
     MS_TELEPORT
    },
/*deep elf conjurer 2  */
    {98,
     MS_STICKY_FLAME,
     MS_ORB_ENERGY,
     MS_INVIS,
     MS_STONE_ARROW,
     MS_NEGATIVE_BOLT,
     MS_TELEPORT
    },
/*deep elf priest      */
    {99,
     MS_PAIN,
     100,
     MS_SMITE,
     MS_HEAL,
     MS_ANIMATE_DEAD,
     MS_HEAL
    },
/*deep elf high priest */
    {100,
     MS_SUMMON_DEMON,
     MS_HELLFIRE_BURST,
     MS_SMITE,
     MS_HEAL,
     MS_ANIMATE_DEAD,
     MS_HEAL
    },
/*deep elf demonologist */
    {101,
     MS_SUMMON_DEMON,
     MS_BANISHMENT,
     MS_SUMMON_DEMON_GREATER,
     MS_SUMMON_DEMON,
     MS_SUMMON_DEMON_LESSER,
     MS_TELEPORT
    },
/*deep elf annihilator */
    {102,
     MS_LIGHTNING_BOLT,
     MS_CRYSTAL_SPEAR,
     MS_BLINK,
     MS_IRON_BOLT,
     MS_POISON_BLAST,
     MS_TELEPORT
    },
/*deep elf sorceror    */
    {103,
     MS_NEGATIVE_BOLT,
     MS_PARALYSIS,
     MS_HASTE,
     MS_SUMMON_DEMON,
     MS_ANIMATE_DEAD,
     MS_TELEPORT
    },
/*deep elf death mage  */
    {104,
     MS_NEGATIVE_BOLT,
     MS_NEGATIVE_BOLT,
     MS_HEAL,
     MS_ANIMATE_DEAD,
     MS_ANIMATE_DEAD,
     MS_TELEPORT
    },
/*kobold demonologist  */
    {105,
     MS_SUMMON_DEMON_LESSER,
     MS_SUMMON_DEMON,
     MS_SUMMON_DEMON_LESSER,
     MS_SUMMON_DEMON,
     100,
     100
    },
/*naga                 */
    {106,
     MS_POISON_SPLASH,
     MS_POISON_SPLASH,
     100,
     100,
     100,
     100
    },
/*naga mage            */
    {107,
     MS_VENOM_BOLT,
     MS_ORB_ENERGY,
     MS_HASTE,
     MS_VENOM_BOLT,
     MS_TELEPORT_OTHER,
     MS_TELEPORT
    },
/*curse skull          */
    {108,
     MS_SUMMON_UNDEAD,
     MS_SUMMON_UNDEAD,
     MS_TORMENT,
     100,
     MS_SUMMON_UNDEAD,
     100
    },
/*shining eye          */
    {109,
     MS_MUTATION,
     MS_MUTATION,
     100,
     100,
     100,
     100
    },
/*frost giant          */
    {110,
     MS_COLD_BOLT,
     MS_COLD_BOLT,
     100,
     100,
     100,
     100
    },
/*Angel                */
    {111,
     100,
     100,
     MS_HEAL,
     100,
     MS_HEAL,
     MS_HEAL
    },
/*Daeva                */
    {112,
     MS_SMITE,
     100,
     MS_SMITE,
     100,
     MS_SMITE,
     100
    },
/*Shadow Dragon        */
    {113,
     MS_NEGATIVE_BOLT,
     MS_NEGATIVE_BOLT,
     100,
     100,
     MS_NEGATIVE_BOLT,
     100
    },
/*Sphinx               */
    {114,
     MS_CONFUSE,
     MS_PARALYSIS,
     MS_SMITE,
     MS_SLOW,
     100,
     MS_HEAL
    },
/*Mummy                */
    {115,
     MS_SUMMON_DEMON,
     MS_SMITE,
     MS_TORMENT,
     MS_SMITE,
     MS_PAIN,
     100
    },
/*Shadow imp           */
    {118,
     MS_PAIN,
     100,
     MS_ANIMATE_DEAD,
     MS_ANIMATE_DEAD,
     100,
     100
    },
/*Ghost - in struct    */
    {119,
     100,
     100,
     100,
     100,
     100,
     100
    },
/*Hell-hog             */
    {120,
     MS_STICKY_FLAME,
     MS_STICKY_FLAME,
     100,
     100,
     100,
     100
    },
/*Swamp Dragon         */
    {121,
     MS_POISON_BLAST,
     MS_POISON_BLAST,
     100,
     MS_POISON_BLAST,
     MS_POISON_BLAST,
     100
    },
/*Swamp Drake          */
    {122,
     MS_MARSH_GAS,
     MS_MARSH_GAS,
     100,
     MS_MARSH_GAS,
     MS_MARSH_GAS,
     100
    },
/*Serpent of Hell      */
    {123,
     MS_HELLFIRE,
     MS_HELLFIRE,
     MS_HELLFIRE,
     MS_HELLFIRE,
     MS_HELLFIRE,
     MS_HELLFIRE
    },
/*Boggart              */
    {124,
     MS_CONFUSE,
     MS_SLOW,
     MS_INVIS,
     MS_BLINK,
     MS_LEVEL_SUMMON,
     MS_BLINK
    },
/*Eye of Devastation   */
    {125,
     MS_ENERGY_BOLT,
     MS_ENERGY_BOLT,
     MS_ENERGY_BOLT,
     MS_ENERGY_BOLT,
     MS_ENERGY_BOLT,
     MS_ENERGY_BOLT
    },
/*Quicksilver dragon   */
    {126,
     MS_QUICKSILVER_BOLT,
     MS_QUICKSILVER_BOLT,
     100,
     MS_QUICKSILVER_BOLT,
     MS_QUICKSILVER_BOLT,
     100
    },
/*Iron dragon          */
    {127,
     MS_METAL_SPLINTERS,
     MS_METAL_SPLINTERS,
     100,
     MS_METAL_SPLINTERS,
     MS_METAL_SPLINTERS,
     100
    },
/*Skeletal warrior     */
    {128,
     MS_ANIMATE_DEAD,
     100,
     MS_ANIMATE_DEAD,
     100,
     100,
     100
    },
};


#define SPELLS (sizeof(mspell_list)/(sizeof(unsigned char)*7))


static int mons_exp_mod(int mclass);
static int mons_speed(int mc);
static monsterentry *seekmonster(int *p_monsterid);


// macro that saves some typing, nothing more
#define smc seekmonster(&mc)




/* ******************** BEGIN PUBLIC FUNCTIONS ******************** */


void mon_init( unsigned char mcolour[1000] )
{

    unsigned int x;    // must be unsigned to match size_t {dlb}

    for (x = 0; x < MONDATASIZE; x++)
      mcolour[mondata[x].mc] = mondata[x].colour;

    seekmonster((int *) 0);    // this forces a refresh of internal entry tracking {dlb}

}          // end mon_init()




int mons_flag( int mc, int bf )
{

    return (smc->bitfields & bf) != 0;

}          // end mons_flag()




int mons_holiness( int mc )
{

    return smc->holiness;

}          // end mons_holiness()




int mons_zombie_size( int mc )
{

    return smc->zombie_size;

}          // end mons_zombie_size()




int mons_weight( int mc )
{

    return smc->weight;

}          // end mons_weight()




int mons_corpse_thingy(int mc)
{

    return smc->corpse_thingy;

}          // end mons_corpse_thingy()




int mons_charclass( int mc )
{

    return smc->charclass;

}          // end mons_charclass()




char mons_shouts( int mc )
{

    int u = smc->shouts;

    if (u == -1)
      u = random2(12);

    return u;

}          // end mons_shouts()




char mons_see_invis( int mc )
{

    if ( mc == MONS_PLAYER_GHOST || mc == MONS_PANDEMONIUM_DEMON )
      return ghost.values[3];
    else
      return ( (smc->bitfields & M_SEE_INVIS) != 0 );

}          // end mons_see_invis()




unsigned char mons_char(int mc)
{

    return (unsigned char) smc->showchar;

}          // end mons_char()




char mons_itemuse( int mc )
{

    return (smc->gmon_use);

}          // end mons_itemuse()




unsigned char mons_colour( int mc )
{

    return (smc->colour);

}          // end mons_colour()




int mondamage( int mc, int rt )
{

    ASSERT(rt >= 0);
    ASSERT(rt <= 3);

    if (rt < 0 || rt > 3)    // make it fool-proof
      return 0;
    else if ( rt == 0 && ( mc == MONS_PLAYER_GHOST || mc == MONS_PANDEMONIUM_DEMON ) )
      return ghost.values[7];
    else
      return smc->damage[rt];

}          // end mondamage()




int mon_resist_mag( int mc, char mhd )
{

    int u = smc->resist_magic;

    if (u < 0)
      return mhd * (abs(u) << 1);        // negative values get multiplied with mhd
    else
      return u;

}          // end mon_resist_mag()




int mons_res_elec( int mc )
{

    if ( mc == MONS_PLAYER_GHOST || mc == MONS_PANDEMONIUM_DEMON )
      return (ghost.values[6] > 0);   /* this is a variable,
                                           not a player_xx() function, so can be above 1 */
    int u = 0, f = smc->bitfields;

    // of course it makes no sense setting them both :)
    if (f & M_RES_ELEC)
      u++;                    //if(f&M_ED_ELEC) u--;

    return u;

}          // end mons_res_elec()




int mons_res_poison( int mc )
{

    int u = 0, f = smc->bitfields;

    if (f & M_RES_POISON)
      u++;

    if (f & M_ED_POISON)
      u--;

    return u;

}          // end mons_res_poison()




int mons_res_fire( int mc )
{

    if ( mc == MONS_PLAYER_GHOST || mc == MONS_PANDEMONIUM_DEMON )
      {
        if (ghost.values[4] > 100)
          return 1;
        else if (ghost.values[4] < 100)
          return -1;
        else
          return 0;
      }

    int u = 0, f = smc->bitfields;

    // no Big Prize (tm) here either if you set all three flags. It's a pity uh?
    if (f & M_RES_HELLFIRE)
      u += 2;
    else if (f & M_RES_FIRE)
      u++;

    if (f & M_ED_FIRE)
      u--;

    return u;

}          // end mons_res_fire()




int mons_res_cold( int mc )
{

    if ( mc == MONS_PLAYER_GHOST || mc == MONS_PANDEMONIUM_DEMON )
      {
        if (ghost.values[5] > 100)
          return 1;
        else if (ghost.values[5] < 100)
          return -1;
        else
          return 0;
      }

    int u = 0, f = smc->bitfields;

    if (f & M_RES_COLD)
      u++;

    if (f & M_ED_COLD)
      u--;

    return u;

}          // end mons_res_cold()




int mons_skeleton( int mc )
{

    if ( mons_zombie_size(mc) == 0
          || mons_weight(mc) == 0
          || ((smc->bitfields & M_NO_SKELETON) != 0) )
      return 0;
    else
      return 1;

}          // end mons_skeleton()




char mons_flies( int mc )
{

    if ( mc == MONS_PANDEMONIUM_DEMON )
      return ghost.values[10];

    int f = smc->bitfields;

    if (f & M_LEVITATE)
      return 2;

    if (f & M_FLIES)
      return 1;

    return 0;

}          // end mons_flies()




// this nice routine we keep in exactly the way it was
int hit_points( int hit_dice, int min_hp, int rand_hp )
{

    int hrolled = 0;

    for (int hroll = 0; hroll < hit_dice; hroll++)
    {
        hrolled += random2(1 + rand_hp);
        hrolled += min_hp;
    }

    return hrolled;

}          // end hit_points()




// for a given menv[foo].type, mons_category() will
// return a general category/class to which the monster
// in question belongs - capped at 254 members to allow
// for MC_UNSPCIFIED (255) and NUM_MC [which for now is
// part of the enum] ... this is a shameless hack and
// prevents eloquent handling of subcategories, etc. -
// should Crawl ever make the transition to OO coding,
// this can all be taken care of through classes {dlb}
unsigned char mons_category( int which_mons )
{

    switch ( which_mons )
    {
       case MONS_GOLD_MIMIC:
       case MONS_WEAPON_MIMIC:
       case MONS_ARMOUR_MIMIC:
       case MONS_SCROLL_MIMIC:
       case MONS_POTION_MIMIC:
         return MC_MIMIC;

       default:
         return MC_UNSPECIFIED;

    }

}          // end mons_category()




int exper_value( int mclass, int mHD, int maxhp )
{

    long x_val = 0;

    int speed = mons_speed(mclass);
    int modifier = mons_exp_mod(mclass);


    if ( mclass == MONS_ZOMBIE_SMALL
          || mclass == MONS_ZOMBIE_LARGE
          || mclass == MONS_SKELETON_SMALL
          || mclass == MONS_SKELETON_LARGE )
      {
        x_val = (16 + mHD * 4) * (mHD * mHD) / 10;
      }
    else
      {
        //x_val = ((maxhp / 7) + 1) * (mHD * mHD) + 1;
        x_val = (16 + maxhp) * (mHD * mHD) / 10;
      }

    if (modifier > 0)
      {
        x_val *= mons_exp_mod(mclass);
        x_val /= 10;
      }

    if (speed > 0)
      {
        x_val *= speed;
        x_val /= 10;
      }

    // some trial reductions -- bwross
    if (x_val > 100)
      x_val = 100 + (x_val - 100) * 3 / 4;

    if (x_val > 1000)
      x_val = 1000 + (x_val - 1000) / 2;

    // guarantee the value is within limits
    if (mons_flag(mclass, M_NO_EXP_GAIN))
      x_val = 0;
    else if (x_val <= 0)
      x_val = 1;
    else if (x_val > 15000)
      x_val = 15000;

    return x_val;

}          // end exper_value()




// this needs to be rewritten a la the monsterseek rewrite {dlb}:
void mons_spell_list( unsigned char sec, int splist[6] )
{

    unsigned int x;

    for (x = 0; x < SPELLS; x++)
      if (mspell_list[x][0] == sec)
        break;

    if ( x >= SPELLS )
      return;

// I *KNOW* this can easily be done in a loop
    splist[0] = mspell_list[x][1];       // bolt spell
    splist[1] = mspell_list[x][2];       // enchantment
    splist[2] = mspell_list[x][3];       // self_ench
    splist[3] = mspell_list[x][4];       // misc
    splist[4] = mspell_list[x][5];       // misc2
    splist[5] = mspell_list[x][6];       // emergency

    if ( sec == 119 )             /* ghost */
      for (x = 0; x < 6; x++)
        splist[x] = ghost.values[x + 14];

}          // end mons_spell_list()




// generate a shiny new and unscarred monster
void define_monster( int k, struct monsters mns[MNST] )
{

    int temp_rand = 0;             // probability determination {dlb}
    int m2_class = mns[k].type;
    int m2_HD, m2_hp, m2_hp_max, m2_AC, m2_ev, m2_speed, m2_speed_inc;
    int m2_sec = mns[k].number;
    struct monsterentry *m = seekmonster(&m2_class);

    m2_HD = m->hpdice[0];

// misc
    m2_AC = m->AC;
    m2_ev = m->ev;

// speed
    m2_speed = m->speed; // this is divided by 10 when reloaded ($pellbinder: why?)
    m2_speed_inc = m->speed_inc;

// some monsters are randomized:
// did I get them all?    // I don't think so {dlb}
    switch ( m2_class )
    {

      case MONS_ABOMINATION_SMALL:
        m2_HD = 4 + random2(4);
        m2_AC = 3 + random2(7);
        m2_ev = 7 + random2(6);
        m2_speed = 7 + random2avg(9,2);

        if (m2_sec == 250)
          m2_sec = random_colour();

        break;

      case MONS_ZOMBIE_SMALL:
        m2_HD = ( coinflip() ? 1 : 2 );
        break;

      case MONS_ABOMINATION_LARGE:
        m2_HD = 8 + random2(4);
        m2_AC = 5 + random2avg(9,2);
        m2_ev = 3 + random2(5);
        m2_speed = 6 + random2avg(7,2);

        if (m2_sec == 250)
          m2_sec = random_colour();

        break;

      case MONS_ZOMBIE_LARGE:
        m2_HD = 3 + random2(5);
        break;

      case MONS_BEAST:
        m2_HD = 4 + random2(4);
        m2_AC = 2 + random2(5);
        m2_ev = 7 + random2(5);
        m2_speed = 8 + random2(5);
        break;

     case MONS_HYDRA:
        m2_sec = 4 + random2(5);
        break;

      case MONS_DEEP_ELF_FIGHTER:
      case MONS_DEEP_ELF_KNIGHT:
      case MONS_DEEP_ELF_SOLDIER:
      case MONS_ORC_WIZARD:
        m2_sec = random2(3);
        break;

      case MONS_JESSICA:
      case MONS_DUANE:
        m2_sec = 0;
        break;
      case MONS_SIGMUND:
        m2_sec = 1;
        break;
      case MONS_BLORK_THE_ORC:
      case MONS_PSYCHE:
      case MONS_MICHAEL:
        m2_sec = 2;
        break;
      case MONS_JOZEF:
        m2_sec = 10;
        break;

      case MONS_LICH:
      case MONS_ANCIENT_LICH:
        m2_sec = 20 + random2(4);
        break;

      case MONS_ADOLF:
      case MONS_BORIS:
        m2_sec = 23;
        break;
      case MONS_MARGERY:
        m2_sec = 50;
        break;

      case MONS_HELL_KNIGHT:
        m2_sec = ( coinflip() ? 58 : 59 );
        break;
      case MONS_NECROMANCER:
        m2_sec = ( coinflip() ? 60 : 61 );
        break;

      case MONS_HAROLD:
      case MONS_NORRIS:
        m2_sec = 59;
        break;
      case MONS_JOSEPHINE:
        m2_sec = 60;
        break;

      case MONS_WIZARD:
      case MONS_OGRE_MAGE:
      case MONS_DEEP_ELF_MAGE:
        m2_sec = 62 + random2(5);
        break;

      case MONS_ERICA:
        m2_sec = 63;
        break;
      case MONS_LOUISE:
      case MONS_RUPERT:
        m2_sec = 65;
        break;
      case MONS_WAYNE:
        m2_sec = 67;
        break;
      case MONS_FRANCIS:
      case MONS_FRANCES:
        m2_sec = 68;
        break;
      case MONS_WHITE_IMP:
        m2_sec = 74;
        break;
      case MONS_YNOXINUL:
        m2_sec = 75;
        break;
      case MONS_NEQOXEC:
        m2_sec = 76;
        break;
      case MONS_HELLWING:
        m2_sec = 77;
        break;
      case MONS_SMOKE_DEMON:
        m2_sec = 78;
        break;
      case MONS_CACODEMON:
        m2_sec = 79;
        break;
      case MONS_GREEN_DEATH:
        m2_sec = 80;
        break;
      case MONS_BALRUG:
        m2_sec = 81;
        break;
      case MONS_GERYON:
        m2_sec = 83;
        break;
      case MONS_DISPATER:
        m2_sec = 84;
        break;
      case MONS_ASMODEUS:
        m2_sec = 85;
        break;
      case MONS_ERESHKIGAL:
        m2_sec = 86;
        break;
      case MONS_ANTAEUS:
        m2_sec = 87;
        break;
      case MONS_MNOLEG:
        m2_sec = 90;
        break;
      case MONS_LOM_LOBON:
        m2_sec = 91;
        break;
      case MONS_CEREBOV:
        m2_sec = 92;
        break;
      case MONS_GLOORX_VLOQ:
        m2_sec = 93;
        break;

      case MONS_DEEP_ELF_CONJURER:
        m2_sec = ( coinflip() ? 97 : 98 );
        break;

      case MONS_KOBOLD_DEMONOLOGIST:
        m2_sec = 105;
        break;

      case MONS_BUTTERFLY:
      case MONS_POTION_MIMIC:
      case MONS_SCROLL_MIMIC:
      case MONS_SPATIAL_VORTEX:
        m2_sec = random_colour();
        break;

      case MONS_WEAPON_MIMIC:
      case MONS_ARMOUR_MIMIC:
        temp_rand = random2(100);

        m2_sec = ( (temp_rand > 34) ? BROWN :               // 65% chance {dlb}
                   (temp_rand > 14) ? LIGHTCYAN :           // 20% chance {dlb}
                   (temp_rand >  4) ? CYAN                  // 10% chance {dlb}
                                    : random_colour() );    //  5% chance {dlb}
        break;
    }

// some calculations
    m2_hp = hit_points(m2_HD, m->hpdice[1], m->hpdice[2]);
    m2_hp += m->hpdice[3];
    m2_hp_max = m2_hp;

    if ( m2_sec == 250 )
      m2_sec = m->sec;

    m2_speed_inc *= 10;
    m2_speed_inc += random2(10);

// so let it be written, so let it be done
    mns[k].hit_dice = m2_HD;
    mns[k].hit_points = m2_hp;
    mns[k].max_hit_points = m2_hp_max;
    mns[k].armor_class = m2_AC;
    mns[k].evasion = m2_ev;
    mns[k].speed = m2_speed;
    mns[k].speed_increment = m2_speed_inc;
    mns[k].number = m2_sec;

}          // end define_monster()




/* ------------------------- monam/moname ------------------------- */


char *monam( int mons_num, int mons, char mench, char desc )
{

    char gmo_n2[40];

    free(gmo_n);
    gmo_n = (char *) malloc(sizeof(char) * 40);

    if (gmo_n == NULL)
      return "Malloc Failed Error";

    strcpy(gmo_n, "");

    // if you can't see the critter, let moname() print [Ii]t.
    if ( mench == ENCH_INVIS && !player_see_invis() )
      {
        moname(mons, mench, player_see_invis(), desc, gmo_n);
        return gmo_n;
      }

    if ( mons == MONS_ZOMBIE_SMALL || mons == MONS_ZOMBIE_LARGE )
    {
        moname(mons_num, mench, player_see_invis(), desc, gmo_n);
        strcat(gmo_n, " zombie");
    }
    else if ( mons == MONS_SKELETON_SMALL || mons == MONS_SKELETON_LARGE )
    {
        moname(mons_num, mench, player_see_invis(), desc, gmo_n);
        strcat(gmo_n, " skeleton");
    }
    else if ( mons == MONS_SPECTRAL_THING )
    {
        switch ( desc )
        {
          case 0:
            strcpy(gmo_n, "The ");
            break;
          case 1:
            strcpy(gmo_n, "the ");
            break;
          case 2:
            strcpy(gmo_n, "A ");
            break;
          case 3:
            strcpy(gmo_n, "a ");
            break;
          case 4:         /* do nothing */ ;
            break;
          //default: DEBUGSTR("bad desc flag");
        }

        strcat(gmo_n, "spectral ");
        moname(mons_num, mench, player_see_invis(), 4, gmo_n2);
        strcat(gmo_n, gmo_n2);
    }
    else if ( mons == MONS_PLAYER_GHOST )
    {
        strcpy(gmo_n, ghost.name);
        strcat(gmo_n, "'s ghost");
    }
    else if ( mons == MONS_PANDEMONIUM_DEMON )
    {
        strcpy(gmo_n, ghost.name);
    }
    else
      moname(mons, mench, player_see_invis(), desc, gmo_n);

    return gmo_n;

}          // end monam()




void moname( int mons_num, char mench, char see_inv, char descrip, char glog[40] )
{

    char gmon_name[40] = "";

    strcpy(gmon_name, seekmonster(&mons_num)->name);
    strcpy(glog, "");

    if (mench == ENCH_INVIS && see_inv == 0)
    {
        switch ( descrip )
        {
          case 0:
          case 2:
            strcpy(glog, "It");
            break;
          case 1:
          case 3:
            strcpy(glog, "it");
            break;
        }

        strcpy(gmon_name, glog);
        return;
    }

    if ( mons_num < 250
          || mons_num > 355
          || ( mons_num >= 260 && mons_num < 280 ) )        // note is also a limit for uniques below.

      switch ( descrip )
      {
        case 0:
          strcpy(glog, "The ");
          break;
        case 1:
          strcpy(glog, "the ");
          break;
        case 2:
          strcpy(glog, "A");
          break;
        case 3:
          strcpy(glog, "a");
          break;
        case 4:         /* do nothing */ ;
          break;
        //default: DEBUGSTR("bad descrip flag");
      }

    if ( ( descrip == 2 || descrip == 3 || descrip == 99 )
        && ( mons_num < 250 || mons_num > 355 || ( mons_num >= 260 && mons_num < 280 ) ) )   // 99 from ouch(...)
      switch ( toupper(gmon_name[0]) )
      {
        case 'A':
        case 'E':
        case 'I':
        case 'O':
        case 'U':
          strcat(glog, "n ");
          break;

        default:
          strcat(glog, " ");
          break;
      }

    if ( ( descrip == 2 || descrip == 3 )
        && ( glog[1] != 110 )
        && mench == ENCH_INVIS
        && see_inv == 0 )
      strcat(glog, "n ");

    strcat(glog, gmon_name);

}          // end moname()


/* ------------------------- monam/moname ------------------------- */




/* ********************* END PUBLIC FUNCTIONS ********************* */




// a kludge, I admit, but why should the entire mondata[]
// array be searched each and every time we need info on
// a monster? hoping this will speed things up a bit w/o
// forcing the entries into any particular order -- which
// I like, btw, for the flexibility it affords. {dlb}
static struct monsterentry *seekmonster( int *p_monsterid )
{

    static int mon_entry[NUM_MONSTERS];

// passing null pointer forces refresh -- typically, at gamestart, only {dlb}
    if ( p_monsterid == 0 )
    {
        unsigned int x = 0;    // must be unsigned to match size_t {dlb}

    // first, fill static array with dummy values {dlb};
        for (x = 0; x < NUM_MONSTERS; x++)
          mon_entry[x] = -1;

    // next, fill static array with location of entry in mondata[] {dlb}:
        for (x = 0; x < MONDATASIZE; x++)
          mon_entry[mondata[x].mc] = x;

    // finally, monsters yet with dummy entries point to TTTSNB(tm) {dlb}:
        for (x = 0; x < NUM_MONSTERS; x++)
          if ( mon_entry[x] == -1 )
            mon_entry[x] = mon_entry[MONS_PROGRAM_BUG];

        return (monsterentry *) 0;    // return value should not matter here {dlb}
    }
    else
      return &mondata[mon_entry[(*p_monsterid)]];    // not as safe (or cute) as before, but we'll give it a shot {dlb}

}          // end seekmonster()




static int mons_exp_mod( int mc )
{

    return smc->exp_mod;

}          // end mons_exp_mod()




static int mons_speed(int mc)
{

    return smc->speed;

}          // end mons_speed()




int mons_intel( int mc ) //jmf: "fixed" to work with new I_ types
{

    switch( smc->intel )
    {
      case I_PLANT:
        return I_PLANT;
      case I_INSECT:
      case I_REPTILE:
        return I_INSECT;
      case I_ANIMAL:
      case I_ANIMAL_LIKE:    // I don't see the distinction, they are synonymous {dlb}
        return I_ANIMAL;
      case I_NORMAL:
        return I_NORMAL;
      case I_HIGH:
        return I_HIGH;
      default:
        return I_NORMAL;
    }

}          // ens mons_intel()




int mons_intel_type( int mc ) //jmf: new, used by my spells
{

  return(smc->intel);

}          // end mons_intel_type()








/* ******************************************************************

// In the name of England, I declare this function wasteful! {dlb}

static monsterentry *seekmonster( int mc )
{

    ASSERT(mc >= 0);

    int x = 0;

    while (x < mondatasize)
    {
        if (mondata[x].mc == mc)
          return &mondata[x];

        x++;
    }

    ASSERT(false);

    return seekmonster(MONS_PROGRAM_BUG);    // see the disasters coming if there is no 250?

}          // end seekmonster()
****************************************************************** */


/* ******************************************************************

// only used once, and internal to this file, to boot {dlb}:

// These are easy to implement here. The difficult (dull!) work of converting
// the data structures is finally finished now!
inline char *mons_name( int mc )
{

    return smc->name;

}          // end mons_name()
****************************************************************** */

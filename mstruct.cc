/*
 *  File:       mstruct.cc
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

// $pellbinder: (c) D.G.S.E 1998
// some routines snatched from former monsstat.cc

#include "AppHdr.h"
#include "mstruct.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "defines.h"
//#include "estruct.h"
//#include "colours.h"
#include "mstruct.h"
#include <ctype.h>
#include "externs.h"
#include "enum.h"
#include "player.h"

char mcolour[1000];
char *gmo_n;                    /* used in monam - could possibly be static to that function */


struct monsterentry mondata[] =
{
#include "m_list.h"
};

#define MONDATASIZE (sizeof(mondata)/sizeof(struct monsterentry))
int mondatasize = MONDATASIZE;

unsigned char                   // will do as long as spell/sec numbers don't come above 255 or below 0
  spell_list[][7] =
{                               // sec  bolt   ench  selfench  misc   misc2  emergency
/*orc wiz              */
 {0, 0, 4, 5, 0, 100, 16},
/*orc wiz              */
 {1, 1, 6, 11, 0, 100, 100},
/*orc wiz              */
 {2, 2, 100, 5, 1, 0, 100},
/*naga                 */
 {10, 15, 15, 13, 7, 4, 13},
/*Lich                 */
 {20, 9, 3, 51, 28, 39, 14},
/*Lich                 */
 {21, 8, 6, 5, 19, 51, 52},
/*Lich                 */
 {22, 19, 28, 51, 2, 17, 16},
/*Lich                 */
 {23, 22, 9, 11, 28, 39, 11},
/*burning devil        */
 {30, 20, 20, 20, 20, 20, 20},
/*vampire              */
 {40, 21, 100, 100, 100, 100, 21},
/*efreet               */
 {50, 8, 100, 100, 12, 100, 100},
/*brain worm           */
 {52, 23, 100, 100, 23, 100, 100},
/*giant orange brain   */
 {53, 23, 43, 6, 24, 16, 14},
/*R                    */
 {54, 25, 16, 11, 25, 16, 14},
/*GooE                 */
 {55, 3, 45, 100, 4, 6, 15},
/*orc sorceror         */
 {56, 8, 19, 27, 52, 20, 14},
/*steam dragon         */
 {57, 26, 26, 100, 26, 26, 100},
/*hell knight          */
 {58, 100, 29, 5, 100, 100, 100},
/*hell knight          */
 {59, 100, 8, 100, 100, 100, 16},
/*necromancer          */
 {60, 9, 19, 100, 28, 28, 14},
/*necromancer          */
 {61, 8, 29, 11, 28, 28, 16},
/*wizard               */
 {62, 0, 3, 5, 10, 6, 14},
/*wizard               */
 {63, 7, 22, 11, 6, 4, 14},
/*wizard               */
 {64, 3, 17, 16, 8, 9, 13},
/*wizard               */
 {65, 40, 38, 16, 10, 52, 13},
/*wizard               */
 {66, 3, 1, 11, 15, 12, 11},
/*orc priest           */
 {67, 29, 100, 30, 100, 100, 13},
/*orc high priest      */
 {68, 29, 27, 30, 27, 28, 13},
/*mottled D            */
 {69, 31, 31, 100, 100, 100, 100},
/*Ice Fiend            */
 {70, 9, 9, 48, 100, 100, 27},
/*Shadow Fiend         */
 {71, 9, 19, 48, 100, 100, 27},
/*tormentor            */
 {72, 29, 100, 100, 100, 100, 100},
/*Storm D + vapour     */
 {73, 10, 10, 100, 10, 10, 100},
/*WHITE lesser demon   */
 {74, 2, 100, 100, 100, 100, 100},
/*CYAN demon           */
 {75, 100, 39, 34, 100, 34, 100},
/*MAGENTA demon        */
 {76, 43, 100, 100, 23, 33, 100},
/*LIGHTGRAY demon (227) */
 {77, 100, 100, 100, 15, 28, 14},
/*LIGHTGRAY demon (228) */
 {78, 31, 26, 30, 100, 100, 100},
/*BROWN gr demon       */
 {79, 33, 33, 43, 33, 18, 100},
/*GREEN greater demon  */
 {80, 32, 32, 100, 7, 33, 16},
/*RED gr demon (233)   */
 {81, 8, 12, 100, 31, 30, 14},
/*BLUE gr demon        */
 {82, 10, 9, 100, 33, 24, 15},
/*Geryon               */
 {83, 36, 100, 36, 36, 100, 36},
/*Dispater             */
 {84, 51, 39, 27, 10, 49, 27},
/*Asmodeus             */
 {85, 8, 49, 27, 51, 19, 14},
/*Ereshkigal           */
 {86, 19, 9, 27, 29, 3, 13},
/*Antaeus              */
 {87, 9, 10, 100, 100, 100, 100},
/*Nemelex Xobeh        */
 {90, 27, 30, 11, 43, 24, 14},
/*Sif Muna             */
 {91, 10, 9, 27, 16, 14, 13},
/*Okawaru              */
 {92, 8, 39, 100, 12, 33, 100},
/*Kikubaaqudgha        */
 {93, 32, 4, 100, 19, 27, 27},
/*Titan                */
 {94, 10, 100, 100, 100, 100, 13},
/*Golden dragon        */
 {95, 8, 9, 100, 100, 32, 100},
/*deep elf summoner    */
 {96, 16, 33, 34, 21, 27, 14},
/*deep elf conjurer    */
 {97, 8, 9, 100, 10, 31, 14},
/*deep elf conjurer 2  */
 {98, 31, 22, 11, 40, 19, 14},
/*deep elf priest      */
 {99, 29, 100, 30, 13, 28, 13},
/*deep elf high priest */
 {100, 27, 20, 30, 13, 28, 13},
/*deep elf demonologist */
 {101, 27, 52, 51, 27, 33, 14},
/*deep elf annihilator */
 {102, 10, 17, 16, 39, 32, 14},
/*deep elf sorceror    */
 {103, 19, 3, 5, 27, 28, 14},
/*deep elf death mage  */
 {104, 19, 19, 13, 28, 28, 14},
/*kobold demonologist  */
 {105, 33, 27, 33, 27, 100, 100},
/*naga                 */
 {106, 41, 41, 100, 100, 100, 100},
/*naga mage            */
 {107, 7, 22, 5, 7, 15, 14},
/*curse skull          */
 {108, 42, 42, 48, 100, 42, 100},
/*shining eye          */
 {109, 43, 43, 100, 100, 100, 100},
/*frost giant          */
 {110, 9, 9, 100, 100, 100, 100},
/*Angel                */
 {111, 100, 100, 13, 100, 13, 13},
/*Daeva                */
 {112, 30, 100, 30, 100, 30, 100},
/*Shadow Dragon        */
 {113, 19, 19, 100, 100, 19, 100},
/*Sphinx               */
 {114, 6, 3, 30, 4, 100, 13},
/*Mummy                */
 {115, 27, 30, 48, 30, 29, 100},
/*Dorgi                */
 {116, 44, 44, 100, 44, 44, 44},
/*Sword                */
 {117, 8, 8, 43, 12, 12, 12},
/*Shadow imp           */
 {118, 29, 100, 28, 28, 100, 100},
/*Ghost - in struct    */
 {119, 100, 100, 100, 100, 100, 100},
/*Hell-hog             */
 {120, 31, 31, 100, 100, 100, 100},
/*Swamp Dragon         */
 {121, 32, 32, 100, 32, 32, 100},
/*Swamp Drake          */
 {122, 46, 46, 100, 46, 46, 100},
/*Serpent of Hell      */
 {123, 49, 49, 49, 49, 49, 49},
/*Boggart              */
 {124, 6, 4, 11, 16, 24, 16},
/*Eye of Devastation   */
 {125, 37, 37, 37, 37, 37, 37},
/*Quicksilver dragon   */
 {126, 47, 47, 100, 47, 47, 100},
/*Iron dragon          */
 {127, 50, 50, 100, 50, 50, 100},
/*Skeletal warrior     */
 {128, 28, 100, 28, 100, 100, 100},
};


/*
   case 0: // magic missile
   case 1: // flame
   case 2: // frost
   case 3: // paralysis
   case 4: // slow monster
   case 5: // haste self
   case 6: // confusion
   case 7: // venom bolt
   case 8: // bolt of fire
   case 9: // bolt of cold
   case 10: // lightning
   case 11: // make invisible
   case 12: // fireball
   case 13: // healing
   case 14: // teleportation
   case 15: // teleport away
   case 16: // blink
   case 17: // crystal spear
   case 18: // dig - must be in misc2 position
   case 19: // -ve energy
   case 20: // burst of hellfire
   case 21: // vampire's summoning
   case 22: // mystic blast
   case 23: // brain feeding
   case 24: // summon anything appropriate for level
   case 25: // fake Rs
   case 26: // breathe steam
   case 27: // summon demons
   case 28: // animate dead
   case 29: // pain
   case 30: // smiting
   case 31: // sticky flame
   case 32: // blast of poison
   case 33: // summon lesser demon
   case 34: // summon LIGHTCYAN lesser demon
   case 35: // blast thing
   case 36: // Geryon's summoning
   case 37: // bolt of energy (eye of devastation)
   case 38: // sting
   case 39: // iron bolt
   case 40: // stone arrow
   case 41: // poison spit (naga)
   case 42: // summon undead around player (curse skull)
   case 43: // mutation
   case 44: // fire zulzer
   case 45: // disintegrate
   case 46: // foul vapour
   case 47: // breathe energy
   case 48: // torment
   case 49: // fiend's hellfire
   case 50: // metal splinters
   case 51: // summon greater (class 1) demon
   case 52: // banishment (to the Abyss)

   Note: is assumed that most monsters capable of casting the more powerful
   summonings can also cast Abjuration (just for simplicity)

 */


/*struct monsters
   {
   int type;
   int hit_points;
   int max_hit_points;
   unsigned char hit_dice;
   int armor_class;
   char evasion;
   unsigned char speed;
   unsigned char speed_increment;
   unsigned char x;
   unsigned char y;
   unsigned char target_x;
   unsigned char target_y;
   unsigned int inv [8];
   unsigned char behavior;
   unsigned char number;
   //int number;
   unsigned char monster_foe;
   unsigned char enchantment [3];
   unsigned char enchantment1;
   };
 */


static monsterentry *seekmonster(int mc)
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

    return seekmonster(250);    // see the disasters coming if there is no 250?

}

// macro's that save some typing, nothing more
#define smc seekmonster(mc)
#define mf(bf) (((smc->bitfields)&bf) != 0)



int mons_flag(int mc, int bf)
{
    return (smc->bitfields & bf) != 0;
}

// These are easy to implement here. The difficult (dull!) work of converting
// the data structures is finally finished now!
inline char *mons_name(int mc)
{
    return smc->name;
}
int mons_intel(int mc)
{
    return smc->intel;
}
int mons_holiness(int mc)
{
    return smc->holiness;
}
int mons_zombie_size(int mc)
{
    return smc->zombie_size;
}
int mons_weight(int mc)
{
    return smc->weight;
}
int mons_corpse_thingy(int mc)
{
    return smc->corpse_thingy;
}
int mons_charclass(int mc)
{
    return smc->charclass;
}
char mons_shouts(int mc)
{
    int u = smc->shouts;

    if (u == -1)
        u = random() % 12;
    return u;
}
char mons_see_invis(int mc)
{
    if (mc == 400 || mc == 401)
        return ghost.values[3];
    return mf(M_SEE_INVIS);
}

int mons_exp_mod(int mc)
{
    return smc->exp_mod;
}

int mons_speed(int mc)
{
    return smc->speed;
}

unsigned char mons_char(int mc)
{
    return (unsigned char) smc->showchar;
}

int mondamage(int mc, int rt)
{
    if (rt < 0 || rt > 3)
        return 0;               // make it fool-proof

    if ((mc == 400 || mc == 401) && rt == 0)
        return ghost.values[7];
    return smc->damage[rt];
}

int mon_resist_mag(int mc, char mhd)
{
    int u = smc->resist_magic;

    if (u < 0)
        return mhd * abs(u) * 2;        // negative values get multiplied with mhd

    return u;
}

// it's that easy!
int mons_res_elec(int mc)
{

    if (mc == 400 || mc == 401)
        return (ghost.values[6] > 0);   /* this is a variable,
                                           not a player_xx() function, so can be above 1 */

    int u = 0, f = smc->bitfields;

    // ofcourse it makes no sense setting them both :)
    if (f & M_RES_ELEC)
        u++;                    //if(f&M_ED_ELEC) u--;

    return u;
}

int mons_res_poison(int mc)
{
    int u = 0, f = smc->bitfields;

    if (f & M_RES_POISON)
        u++;
    if (f & M_ED_POISON)
        u--;
    return u;
}

int mons_res_fire(int mc)
{

    if (mc == 400 || mc == 401)
    {
        if (ghost.values[4] > 100)
            return 1;
        if (ghost.values[4] < 100)
            return -1;
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
}

int mons_res_cold(int mc)
{

    if (mc == 400 || mc == 401)
    {
        if (ghost.values[5] > 100)
            return 1;
        if (ghost.values[5] < 100)
            return -1;
        return 0;
    }

    int u = 0, f = smc->bitfields;

    if (f & M_RES_COLD)
        u++;
    if (f & M_ED_COLD)
        u--;
    return u;
}

int mons_skeleton(int mc)
{
//      if (mons_zombie_size(mc) == 0 || mons_weight(mc) == 0) return 0;
    if (mons_zombie_size(mc) == 0 || mons_weight(mc) == 0)
        return 0;
    if (mf(M_NO_SKELETON))
        return 0;
    return 1;
}

char mons_flies(int mc)
{

    if (mc == 401)
    {
        return ghost.values[10];
    }

    int f = smc->bitfields;

    if (f & M_LEVITATE)
        return 2;
    if (f & M_FLIES)
        return 1;
    return 0;
}


void mon_init(char gmon_use[1000], char mcolour[1000])
{
    int x;

    // that saves *MAJOR* space
    for (x = 0; x < mondatasize; x++)
    {
        int mc = mondata[x].mc;

        gmon_use[mc] = mondata[x].gmon_use;
        mcolour[mc] = mondata[x].colour;
    }
}

// this nice routine we keep in exactly the way it was
int hit_points(int hit_dice, int min_hp, int rand_hp)
{
    int hroll = 0;
    int hrolled = 0;

    for (hroll = 0; hroll < hit_dice; hroll++)
    {
        hrolled += random() % (rand_hp + 1);
        hrolled += min_hp;
    }
    return hrolled;
}

// generate a shiny new and unscarred monster
void define_monster(int k, struct monsters mns[MNST])
{
    int m2_class = mns[k].type;
    int m2_HD, m2_hp, m2_hp_max, m2_AC, m2_ev, m2_speed, m2_speed_inc;
    int m2_sec = mns[k].number;
    struct monsterentry *m = seekmonster(m2_class);

    m2_HD = m->hpdice[0];
    // misc
    m2_AC = m->AC;              //gmon_AC [2];

    m2_ev = m->ev;              //gmon_ev [2];

    // speed
    m2_speed = m->speed;        // this is divided by 10 when reloaded ($pellbinder: why?)

    m2_speed_inc = m->speed_inc;

    switch (m2_class)
    {                           // some monsters are randomized

    case 23:                    // x????

        m2_HD = 4 + random() % 4;
        m2_AC = 3 + random() % 7;       //gmon_AC [2];

        m2_ev = 7 + random() % 6;       //gmon_ev [2];   // Look at this one!!!

        m2_speed = 7 + random() % 5 + random() % 5;
        if (m2_sec == 250)
            m2_sec = random() % 15 + 1;
        break;
    case 25:
        m2_HD = random() % 2 + 1;
        break;                  // small zombie

    case 37:
        m2_sec = random() % 4 + 20;
        break;                  // lich

    case 49:                    // Abomination

        m2_HD = 8 + random() % 4;
        m2_AC = 5 + random() % 5 + random() % 5;        //gmon_AC [2];

        m2_ev = 3 + random() % 5;       //gmon_ev [2];

        m2_speed = 6 + random() % 4 + random() % 4;
        if (m2_sec == 250)
            m2_sec = random() % 15 + 1;
        break;
    case 51:
        m2_HD = random() % 5 + 3;
        break;                  // large zombie

    case 53:
        m2_sec = 105;           // kobold demonologist

    case 54:
        m2_sec = random() % 3;
        break;                  // orc wizard

    case 66:
        m2_sec = random() % 15 + 1;
        break;                  // butterfly

    case 88:                    // beast

        m2_HD = 4 + random() % 4;
        m2_AC = 2 + random() % 5;
        m2_ev = 7 + random() % 5;
        m2_speed = 8 + random() % 5;
        break;
    case 106:
        m2_sec = 4 + random() % 5;
        break;                  // hydra

    case 109:
        m2_sec = 58 + random() % 2;
        break;                  // hell knight

    case 110:
        m2_sec = 60 + random() % 2;
        break;                  // necromancer

    case 111:                   // wizard

    case 142:
        m2_sec = 62 + random() % 5;
        break;                  // ogre mage

    case 263:                   // deep elf soldier

    case 264:                   // deep elf fighter

    case 265:
        m2_sec = random() % 3;
        break;                  // deep elf knight

    case 266:
        m2_sec = 62 + random() % 5;
        break;                  // deep elf mage

    case 268:
        m2_sec = 97 + random() % 2;
        break;                  // deep elf conjurer

    case 220:
        m2_sec = 74;
        break;                  // lesser demon

    case 225:
        m2_sec = 76;
        break;                  // demon

    case 227:
        m2_sec = 77;
        break;                  // demon

    case 228:
        m2_sec = 78;
        break;                  // demon

    case 229:
        m2_sec = 75;
        break;                  // demon

    case 231:
        m2_sec = 80;
        break;                  // gr demon

    case 233:
        m2_sec = 81;
        break;                  // gr demon

    case 234:
        m2_sec = 79;
        break;                  // gr demon

    case 244:
        m2_sec = random() % 15 + 1;
        break;                  // gr demon

    case 251:
        m2_sec = 90;
        break;                  // Nemelex Xobeh

    case 252:
        m2_sec = 91;
        break;                  // Sif Muna

    case 253:
        m2_sec = 92;
        break;                  // Okawaru

    case 254:
        m2_sec = 93;
        break;                  // Kiku

    case 281:
        m2_sec = 0;
        break;                  // Jessica

    case 283:
        m2_sec = 1;
        break;                  // Sigmund

    case 284:
        m2_sec = 2;
        break;                  // Blork

    case 286:
        m2_sec = 2;
        break;                  // Psyche

    case 290:
        m2_sec = 2;
        break;                  // Michael

    case 293:
        m2_sec = 63;
        break;                  // Erica

    case 294:
        m2_sec = 60;
        break;                  // Josephine

    case 295:
        m2_sec = 59;
        break;                  // Harold

    case 297:
        m2_sec = 10;
        break;                  // Jozef

    case 300:
        m2_sec = 65;
        break;                  // Louise

    case 301:
        m2_sec = 68;
        break;                  // Francis

    case 302:
        m2_sec = 68;
        break;                  // Frances

    case 303:
        m2_sec = 65;
        break;                  // Rupert

    case 304:
        m2_sec = 67;
        break;                  // Wayne

    case 305:
        m2_sec = 0;
        break;                  // Duane

    case 307:
        m2_sec = 59;
        break;                  // Norris

    case 308:
        m2_sec = 23;
        break;                  // Adolf

    case 309:
        m2_sec = 50;
        break;                  // Margery

    case 310:
        m2_sec = 23;
        break;                  // Boris

    case 340:
        m2_sec = 83;
        break;                  // Geryon

    case 341:
        m2_sec = 84;
        break;                  // Dispater

    case 342:
        m2_sec = 85;
        break;                  // Asmodeus

    case 343:
        m2_sec = 87;
        break;                  // Antaeus

    case 344:
        m2_sec = 86;
        break;                  // Ereshkigal

    case 356:
        m2_sec = random() % 4 + 20;
        break;                  // ancient lich - same as normal one

    case 390:
    case 391:                   // mimics

        if (random() % 4 == 0)
            m2_sec = LIGHTCYAN;
        else
            m2_sec = BROWN;
        if (random() % 10 == 0)
            m2_sec = CYAN;
        if (random() % 20 == 0)
            m2_sec = random() % 15 + 1;
        break;

    case 392:
    case 393:                   // mimics

        m2_sec = 1 + random() % 15;
        break;


        // did I get them all?
    }
    // some calculations
    m2_hp = hit_points(m2_HD, m->hpdice[1], m->hpdice[2]);
    m2_hp += m->hpdice[3];
    m2_hp_max = m2_hp;

    if (m2_sec == 250)
        m2_sec = m->sec;

    m2_speed_inc *= 10;
    m2_speed_inc += random() % 10;

    // so let it be written, so let it be done
    mns[k].hit_dice = m2_HD;
    mns[k].hit_points = m2_hp;
    mns[k].max_hit_points = m2_hp_max;
    mns[k].armor_class = m2_AC;
    mns[k].evasion = m2_ev;
    mns[k].speed = m2_speed;
    mns[k].speed_increment = m2_speed_inc;
    mns[k].number = m2_sec;
}





#define SPELLS (sizeof(spell_list)/(sizeof(unsigned char)*7))
void mons_spell_list(unsigned char sec, int splist[6])
{
    unsigned int x;

    for (x = 0; x < SPELLS; x++)
        if (spell_list[x][0] == sec)
            break;
    if (x >= SPELLS)
        return;
    // I *KNOW* this can easily be done in a loop
    splist[0] = spell_list[x][1];       // bolt spell

    splist[1] = spell_list[x][2];       // enchantment

    splist[2] = spell_list[x][3];       // self_ench

    splist[3] = spell_list[x][4];       // misc

    splist[4] = spell_list[x][5];       // misc2

    splist[5] = spell_list[x][6];       // emergency

    if (sec == 119)             /* ghost */
    {
        for (x = 0; x < 6; x++)
            splist[x] = ghost.values[x + 14];
    }


}

void def_letters(char letters[52][1])
{
/*int x; char temp[2]={0,0};
   for(x=0;x<26;x++) {
   temp[0]=x+'a'; strcpy(letters[x],temp);
   temp[0]=x+'A'; strcpy(letters[x+26],temp);
   } */
    int x;                      //char temp[2]={0,0};

    for (x = 0; x < 26; x++)
    {
//              temp[0]=x+'a';
        letters[x][0] = x + 'a';
//              temp[0]=x+'A';
        letters[x + 26][0] = x + 'A';

    }
}


char *monam(int mons_num, int mons, char desc, char see_invis)
{
    char gmo_n2[40];

    free(gmo_n);
    gmo_n = (char *) malloc(sizeof(char) * 40);

    if (gmo_n == NULL)
        return "Malloc Failed Error";

    strcpy(gmo_n, "");

    if (mons == MONS_SMALL_ZOMBIE || mons == MONS_BIG_ZOMBIE)
    {
        moname(mons_num, desc, player_see_invis(), see_invis, gmo_n);
        strcat(gmo_n, " zombie");

    }
    else if (mons == MONS_SMALL_SKELETON || mons == MONS_LARGE_SKELETON)
    {
        moname(mons_num, desc, player_see_invis(), see_invis, gmo_n);
        strcat(gmo_n, " skeleton");

    }
    else if (mons == MONS_SPECTRAL_THING)
    {
        switch (see_invis)
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
//                      default: DEBUGSTR("bad see_invis flag");
        }

        strcat(gmo_n, "spectral ");
        moname(mons_num, desc, player_see_invis(), 4, gmo_n2);
        strcat(gmo_n, gmo_n2);

    }
    else if (mons == MONS_PLAYER_GHOST)
    {
        strcpy(gmo_n, ghost.name);
        strcat(gmo_n, "'s ghost");

    }
    else if (mons == MONS_PANDEMONIUM_DEMON)
    {
        strcpy(gmo_n, ghost.name);

    }
    else
        moname(mons, desc, player_see_invis(), see_invis, gmo_n);

    return gmo_n;
}


void moname(int mons_num, char mench, char see_inv, char descrip, char glog[40])
{
    char gmon_name[40] = "";

    strcpy(gmon_name, mons_name(mons_num));
    strcpy(glog, "");

    if (mench == 6 && see_inv == 0)
    {
        switch (descrip)
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

    if (mons_num < 250 || mons_num > 355 || (mons_num >= 260 && mons_num < 280))        // note is also a limit for uniques below.

        switch (descrip)
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
//                      default: DEBUGSTR("bad descrip flag");
        }

    if ((descrip == 2 || descrip == 3 || descrip == 99) && (mons_num < 250 || mons_num > 355 || (mons_num >= 260 && mons_num < 280)))   // 99 from ouch(...)

        switch (toupper(gmon_name[0]))
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

    if ((descrip == 2 || descrip == 3) && (glog[1] != 110) && mench == 6 && see_inv == 0)
        strcat(glog, "n ");

    strcat(glog, gmon_name);
}


int exper_value(int mclass, int mHD, int maxhp)
{
    long x_val = 0;

    int speed = mons_speed(mclass);
    int modifier = mons_exp_mod(mclass);


    if (mclass == MONS_SMALL_ZOMBIE || mclass == MONS_BIG_ZOMBIE
            || mclass == MONS_SMALL_SKELETON || mclass == MONS_LARGE_SKELETON)
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


    // guarantee the value is withing limits
    if (mons_flag(mclass, M_NO_EXP_GAIN))
        x_val = 0;
    else if (x_val <= 0)
        x_val = 1;
    else if (x_val > 15000)
        x_val = 15000;

    return x_val;
}


char mons_pan(int mcls)         // is the monster to be found in pandemonium
{

    switch (mcls)
    {
    case 5:                     //strcat(gmon_name, "fungus"); break;

    case 13:                    //strcat(gmon_name, "necrophage"); break;

    case 15:                    //strcat(gmon_name, "phantom"); break;

    case 23:                    //strcat(gmon_name, "abomination"); break;

    case 25:                    //break;

    case 35:                    //strcat(gmon_name, "jelly"); break;

    case 37:                    //strcat(gmon_name, "lich"); break;

    case 38:                    //strcat(gmon_name, "mummy"); break;

    case 43:                    //strcat(gmon_name, "rakshasa"); break;

    case 46:                    //strcat(gmon_name, "unseen horror"); break;

    case 47:                    //strcat(gmon_name, "vampire"); break;

    case 48:                    //strcat(gmon_name, "wraith"); break;

    case 49:                    //strcat(gmon_name, "abomination"); break;

    case 51:                    //break;
        // strcat(gmon_name, "large zombie"); break;

    case 59:                    //strcat(gmon_name, "giant eyeball"); break;

    case 60:                    //strcat(gmon_name, "wight"); break;

    case 63:                    //strcat(gmon_name, "shadow"); break;

    case 64:                    //strcat(gmon_name, "hungry ghost"); break;

    case 65:                    //strcat(gmon_name, "eye of draining"); break;

    case 68:                    //strcat(gmon_name, "efreet"); break;

    case 69:                    //strcat(gmon_name, "brain worm"); break;

    case 70:                    //strcat(gmon_name, "giant orange brain"); break;

    case 72:                    //strcat(gmon_name, "flying skull"); break;

    case 76:                    //strcat(gmon_name, "slime creature"); break;

    case 77:                    //strcat(gmon_name, "freezing wraith"); break;

    case 78:                    //strcat(gmon_name, "rakshasa"); break;

    case 79:                    //strcat(gmon_name, "great orb of eyes"); break;

    case 107:                   //break;

    case 108:                   //// ske

    case 116:                   //strcat(gmon_name, "clay golem"); break;

    case 117:                   //strcat(gmon_name, "wood golem"); break;

    case 118:                   //strcat(gmon_name, "stone golem"); break;

    case 119:                   //strcat(gmon_name, "iron golem"); break;

    case 120:                   //strcat(gmon_name, "crystal golem"); break;

    case 121:                   //strcat(gmon_name, "toenail golem"); break;

    case 122:                   //strcat(gmon_name, "mottled dragon"); break;

    case 123:                   //strcat(gmon_name, "earth elemental"); break;

    case 124:                   //strcat(gmon_name, "fire elemental"); break;

    case 125:                   //strcat(gmon_name, "air elemental"); break;

    case 130:                   //strcat(gmon_name, "spectre"); break;

    case 131:                   //strcat(gmon_name, "pulsating lump"); break;

    case 136:                   //strcat(gmon_name, "stone giant"); break; // stoned giant

    case 137:                   //strcat(gmon_name, "flayed ghost"); break;

    case 140:                   //strcat(gmon_name, "insubstantial wisp"); break;

    case 110:                   //strcat(gmon_name, "necromancer"); break;

    case 111:                   //strcat(gmon_name, "wizard"); break;

    case 220:
    case 221:
    case 222:
    case 223:
    case 224:
    case 225:
    case 226:
    case 227:
    case 228:
    case 229:
    case 230:
    case 231:
    case 232:
    case 233:
    case 234:
        return 52;
    default:
        return 0;
    }
}

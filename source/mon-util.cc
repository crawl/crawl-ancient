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
#include "spells4.h"

//jmf: moved from inside function
static FixedVector < int, NUM_MONSTERS > mon_entry;

// really important extern -- screen redraws suck w/o it {dlb}
FixedVector < unsigned char, 1000 > mcolour;

/* used in monam - could possibly be static to that function */
char *gmo_n;


static struct monsterentry mondata[] = {
#include "mon-data.h"
};

#define MONDATASIZE (sizeof(mondata)/sizeof(struct monsterentry))

static unsigned char mspell_list[][7] = {
#include "mon-spll.h"
};

static int mons_exp_mod(int mclass);
static int mons_speed(int mc);
static monsterentry *seekmonster(int *p_monsterid);

// macro that saves some typing, nothing more
#define smc seekmonster(&mc)

/* ******************** BEGIN PUBLIC FUNCTIONS ******************** */
void mon_init(FixedVector < unsigned char, 1000 > &colour)
{
    unsigned int x;             // must be unsigned to match size_t {dlb}

    for (x = 0; x < MONDATASIZE; x++)
        colour[mondata[x].mc] = mondata[x].colour;

    //unsigned int x = 0;    // must be unsigned to match size_t {dlb}

    // first, fill static array with dummy values {dlb};
    for (x = 0; x < NUM_MONSTERS; x++)
        mon_entry[x] = -1;

    // next, fill static array with location of entry in mondata[] {dlb}:
    for (x = 0; x < MONDATASIZE; x++)
        mon_entry[mondata[x].mc] = x;

    // finally, monsters yet with dummy entries point to TTTSNB(tm) {dlb}:
    for (x = 0; x < NUM_MONSTERS; x++)
    {
        if (mon_entry[x] == -1)
            mon_entry[x] = mon_entry[MONS_PROGRAM_BUG];
    }
    //return (monsterentry *) 0; // return value should not matter here {dlb}
}                               // end mon_init()


int mons_flag(int mc, int bf)
{
    return (smc->bitfields & bf) != 0;
}                               // end mons_flag()


int mons_holiness(int mc)
{
    return smc->holiness;
}                               // end mons_holiness()


int mons_zombie_size(int mc)
{
    return smc->zombie_size;
}                               // end mons_zombie_size()


int mons_weight(int mc)
{
    return smc->weight;
}                               // end mons_weight()


int mons_corpse_thingy(int mc)
{
    return smc->corpse_thingy;
}                               // end mons_corpse_thingy()


int mons_charclass(int mc)
{
    return smc->charclass;
}                               // end mons_charclass()


char mons_shouts(int mc)
{
    int u = smc->shouts;

    if (u == -1)
        u = random2(12);

    return u;
}                               // end mons_shouts()


char mons_see_invis(int mc)
{
    if (mc == MONS_PLAYER_GHOST || mc == MONS_PANDEMONIUM_DEMON)
        return ghost.values[3];
    else
        return ((smc->bitfields & M_SEE_INVIS) != 0);
}                               // end mons_see_invis()


unsigned char mons_char(int mc)
{
    return (unsigned char) smc->showchar;
}                               // end mons_char()


char mons_itemuse(int mc)
{
    return (smc->gmon_use);
}                               // end mons_itemuse()


unsigned char mons_colour(int mc)
{
    return (smc->colour);
}                               // end mons_colour()


int mondamage(int mc, int rt)
{
    ASSERT(rt >= 0);
    ASSERT(rt <= 3);

    if (rt < 0 || rt > 3)       // make it fool-proof
        return 0;
    else if (rt == 0
                && (mc == MONS_PLAYER_GHOST || mc == MONS_PANDEMONIUM_DEMON))
    {
        return ghost.values[7];
    }
    else
        return smc->damage[rt];
}                               // end mondamage()


int mon_resist_mag(int mc, char mhd)
{
    int u = smc->resist_magic;

    // negative values get multiplied with mhd
    if (u < 0)
        return mhd * (abs(u) * 2);
    else
        return u;
}                               // end mon_resist_mag()


int mons_res_elec(int mc)
{
    if (mc == MONS_PLAYER_GHOST || mc == MONS_PANDEMONIUM_DEMON)
        return (ghost.values[6] > 0);

    /* this is a variable, not a player_xx() function, so can be above 1 */
    int u = 0, f = smc->bitfields;

    // of course it makes no sense setting them both :)
    if (f & M_RES_ELEC)
        u++;                    //if(f&M_ED_ELEC) u--;

    return u;
}                               // end mons_res_elec()


int mons_res_poison(int mc)
{
    int u = 0, f = smc->bitfields;

    if (f & M_RES_POISON)
        u++;

    if (f & M_ED_POISON)
        u--;

    return u;
}                               // end mons_res_poison()


int mons_res_fire(int mc)
{
    if (mc == MONS_PLAYER_GHOST || mc == MONS_PANDEMONIUM_DEMON)
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
}                               // end mons_res_fire()


int mons_res_cold(int mc)
{
    if (mc == MONS_PLAYER_GHOST || mc == MONS_PANDEMONIUM_DEMON)
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
}                               // end mons_res_cold()


int mons_skeleton(int mc)
{
    if (mons_zombie_size(mc) == 0
        || mons_weight(mc) == 0 || ((smc->bitfields & M_NO_SKELETON) != 0))
    {
        return 0;
    }

    return 1;
}                               // end mons_skeleton()


char mons_flies(int mc)
{
    if (mc == MONS_PANDEMONIUM_DEMON)
        return ghost.values[10];

    int f = smc->bitfields;

    if (f & M_LEVITATE)
        return 2;

    if (f & M_FLIES)
        return 1;

    return 0;
}                               // end mons_flies()


// this nice routine we keep in exactly the way it was
int hit_points(int hit_dice, int min_hp, int rand_hp)
{
    int hrolled = 0;

    for (int hroll = 0; hroll < hit_dice; hroll++)
    {
        hrolled += random2(1 + rand_hp);
        hrolled += min_hp;
    }

    return hrolled;
}                               // end hit_points()


// for a given menv[foo].type, mons_category() will
// return a general category/class to which the monster
// in question belongs - capped at 254 members to allow
// for MC_UNSPCIFIED (255) and NUM_MC [which for now is
// part of the enum] ... this is a shameless hack and
// prevents eloquent handling of subcategories, etc. -
// should Crawl ever make the transition to OO coding,
// this can all be taken care of through classes {dlb}
unsigned char mons_category(int which_mons)
{
    switch (which_mons)
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
}                               // end mons_category()


int exper_value(int mclass, int mHD, int maxhp)
{
    long x_val = 0;

    int speed = mons_speed(mclass);
    int modifier = mons_exp_mod(mclass);

    if (mclass == MONS_ZOMBIE_SMALL
        || mclass == MONS_ZOMBIE_LARGE
        || mclass == MONS_SIMULACRUM_SMALL
        || mclass == MONS_SIMULACRUM_LARGE
        || mclass == MONS_SKELETON_SMALL
        || mclass == MONS_SKELETON_LARGE)
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
}                               // end exper_value()


// this needs to be rewritten a la the monsterseek rewrite {dlb}:
void mons_spell_list(unsigned char sec, int splist[6])
{
    unsigned int x;

    for (x = 0; x < NUM_MSTYPES; x++)
    {
        if (mspell_list[x][0] == sec)
            break;
    }

    if (x >= NUM_MSTYPES)
        return;

    // I *KNOW* this can easily be done in a loop
    splist[0] = mspell_list[x][1];      // bolt spell
    splist[1] = mspell_list[x][2];      // enchantment
    splist[2] = mspell_list[x][3];      // self_ench
    splist[3] = mspell_list[x][4];      // misc
    splist[4] = mspell_list[x][5];      // misc2
    splist[5] = mspell_list[x][6];      // emergency

    if (sec == 119)             /* ghost */
    {
        for (x = 0; x < 6; x++)
        {
            splist[x] = ghost.values[x + 14];
        }
    }
}                               // end mons_spell_list()


// generate a shiny new and unscarred monster
void define_monster(int k)
{
    int temp_rand = 0;          // probability determination {dlb}
    int m2_class = menv[k].type;
    int m2_HD, m2_hp, m2_hp_max, m2_AC, m2_ev, m2_speed, m2_speed_inc;
    int m2_sec = menv[k].number;
    struct monsterentry *m = seekmonster(&m2_class);

    m2_HD = m->hpdice[0];

    // misc
    m2_AC = m->AC;
    m2_ev = m->ev;

    // speed
    // this is divided by 10 when reloaded ($pellbinder: why?)
    m2_speed = m->speed;
    m2_speed_inc = m->speed_inc;

    // some monsters are randomized:
    // did I get them all?    // I don't think so {dlb}
    switch (m2_class)
    {
    case MONS_ABOMINATION_SMALL:
        m2_HD = 4 + random2(4);
        m2_AC = 3 + random2(7);
        m2_ev = 7 + random2(6);
        m2_speed = 7 + random2avg(9, 2);

        if (m2_sec == 250)
            m2_sec = random_colour();
        break;

    case MONS_ZOMBIE_SMALL:
        m2_HD = (coinflip() ? 1 : 2);
        break;

    case MONS_ZOMBIE_LARGE:
        m2_HD = 3 + random2(5);
        break;

    case MONS_ABOMINATION_LARGE:
        m2_HD = 8 + random2(4);
        m2_AC = 5 + random2avg(9, 2);
        m2_ev = 3 + random2(5);
        m2_speed = 6 + random2avg(7, 2);

        if (m2_sec == 250)
            m2_sec = random_colour();
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

    case MONS_LICH:
    case MONS_ANCIENT_LICH:
        m2_sec = 20 + random2(4);
        break;
    case MONS_HELL_KNIGHT:
        m2_sec = (coinflip() ? 58 : 59);
        break;
    case MONS_NECROMANCER:
        m2_sec = (coinflip() ? 60 : 61);
        break;
    case MONS_WIZARD:
    case MONS_OGRE_MAGE:
    case MONS_DEEP_ELF_MAGE:
        m2_sec = 62 + random2(5);
        break;
    case MONS_DEEP_ELF_CONJURER:
        m2_sec = (coinflip()? 97 : 98);
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

        m2_sec = ((temp_rand > 34) ? BROWN :    // 65% chance {dlb}
                  (temp_rand > 14) ? LIGHTCYAN :// 20% chance {dlb}
                  (temp_rand > 4)  ? CYAN       // 10% chance {dlb}
                                   : random_colour());  //  5% chance {dlb}
        break;
    }

    // some calculations
    m2_hp = hit_points(m2_HD, m->hpdice[1], m->hpdice[2]);
    m2_hp += m->hpdice[3];
    m2_hp_max = m2_hp;

    if (m2_sec == 250)
        m2_sec = m->sec;

    m2_speed_inc *= 10;
    m2_speed_inc += random2(10);

    // so let it be written, so let it be done
    menv[k].hit_dice = m2_HD;
    menv[k].hit_points = m2_hp;
    menv[k].max_hit_points = m2_hp_max;
    menv[k].armor_class = m2_AC;
    menv[k].evasion = m2_ev;
    menv[k].speed = m2_speed;
    menv[k].speed_increment = m2_speed_inc;
    menv[k].number = m2_sec;
}                               // end define_monster()


/* ------------------------- monam/moname ------------------------- */
char *ptr_monam( struct monsters *mon, char desc )
{
    return (monam( mon->number, mon->type, mon->enchantment[2], desc ));
}

char *monam(int mons_num, int mons, char mench, char desc)
{
    char gmo_n2[40];

    free(gmo_n);
    gmo_n = (char *) malloc(sizeof(char) * 40);

    if (gmo_n == NULL)
        return "Malloc Failed Error";

    strcpy(gmo_n, "");

    // if you can't see the critter, let moname() print [Ii]t.
    if (mench == ENCH_INVIS && !player_see_invis())
    {
        moname(mons, mench, player_see_invis(), desc, gmo_n);
        return gmo_n;
    }

    if (mons == MONS_ZOMBIE_SMALL || mons == MONS_ZOMBIE_LARGE)
    {
        moname(mons_num, mench, player_see_invis(), desc, gmo_n);
        strcat(gmo_n, " zombie");
    }
    else if (mons == MONS_SKELETON_SMALL || mons == MONS_SKELETON_LARGE)
    {
        moname(mons_num, mench, player_see_invis(), desc, gmo_n);
        strcat(gmo_n, " skeleton");
    }
    else if (mons == MONS_SIMULACRUM_SMALL || mons == MONS_SIMULACRUM_LARGE)
    {
        moname(mons_num, mench, player_see_invis(), desc, gmo_n);
        strcat(gmo_n, " simulacrum");
    }
    else if (mons == MONS_SPECTRAL_THING)
    {
        switch (desc)
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
        moname(mons, mench, player_see_invis(), desc, gmo_n);

    return gmo_n;
}                               // end monam()


void moname(int mons_num, char mench, char see_inv, char descrip,
            char glog[40])
{
    char gmon_name[40] = "";

    strcpy(gmon_name, seekmonster(&mons_num)->name);
    strcpy(glog, "");

    if (mench == ENCH_INVIS && see_inv == 0)
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

    // note is also a limit for uniques below.
    if (mons_num < 250 || mons_num > 355 || (mons_num >= 260 && mons_num < 280))
    {
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
            //default: DEBUGSTR("bad descrip flag");
        }
    }

    if ((descrip == 2 || descrip == 3 || descrip == 99)
        && (mons_num < 250 || mons_num > 355
            || (mons_num >= 260 && mons_num < 280)))    // 99 from ouch(...)
    {
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
    }

    if ((descrip == 2 || descrip == 3)
        && (glog[1] != 110) && mench == ENCH_INVIS && see_inv == 0)
    {
        strcat(glog, "n ");
    }

    strcat(glog, gmon_name);
}                               // end moname()

/* ********************* END PUBLIC FUNCTIONS ********************* */

// see mon_init for initialization of mon_entry array.
static struct monsterentry *seekmonster(int *p_monsterid)
{
    ASSERT(p_monsterid != 0);
    int me = mon_entry[(*p_monsterid)];
    if (me >= 0)                // PARANOIA
        return &mondata[mon_entry[(*p_monsterid)]];
    else
        return NULL;
}                               // end seekmonster()

static int mons_exp_mod(int mc)
{
    return smc->exp_mod;
}                               // end mons_exp_mod()


static int mons_speed(int mc)
{
    return smc->speed;
}                               // end mons_speed()


int mons_intel(int mc)          //jmf: "fixed" to work with new I_ types
{
    switch (smc->intel)
    {
    case I_PLANT:
        return I_PLANT;
    case I_INSECT:
    case I_REPTILE:
        return I_INSECT;
    case I_ANIMAL:
    case I_ANIMAL_LIKE:
        // I don't see the distinction, they are synonymous {dlb}
        //jmf: look harder; it'd been explained to you twice
        return I_ANIMAL;
    case I_NORMAL:
        return I_NORMAL;
    case I_HIGH:
        return I_HIGH;
    default:
        return I_NORMAL;
    }
}                               // ens mons_intel()


int mons_intel_type(int mc)     //jmf: new, used by my spells
{
    return (smc->intel);
}                               // end mons_intel_type()

int mons_power(int mc)
{
    // for now,  just return monster hit dice.
    return (smc->hpdice[0]);
}

bool mons_aligned(struct monsters *m1, struct monsters *m2)
{
    bool fr1 = (m1->attitude == ATT_FRIENDLY) ||
        monster_has_enchantment(m1, ENCH_CHARM);
    bool fr2 = (m2->attitude == ATT_FRIENDLY) ||
        monster_has_enchantment(m2, ENCH_CHARM);

    return fr1 == fr2;
}

bool mons_friendly(struct monsters *m)
{
    return (m->attitude == ATT_FRIENDLY || monster_has_enchantment(m, ENCH_CHARM));
}

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

/*****************************************************************

  Used to determine whether or not a monster should fire a beam (MUST be
  called _after_ fire_tracer() for meaningful result.

*/

bool mons_should_fire(struct bolt &beam)
{
    // use of foeRatio:
    // the higher this number, the more monsters
    // will _avoid_ collateral damage to their friends.
    // setting this to zero will in fact have all
    // monsters ignore their friends when considering
    // collateral damage.

    // quick check - did we in fact get any foes?
    if (beam.foe_count == 0)
        return false;

    // if we either hit no friends, or monster too dumb to care
    if (beam.fr_count == 0 || !beam.smartMonster)
        return true;

    // only fire if they do acceptably low collateral damage
    // the default for this is 50%;  in other words, don't
    // hit a foe unless you hit 2 or fewer friends.
    if (beam.foe_power >= (beam.foeRatio * beam.fr_power) / 100)
        return true;

    return false;
}


// used to determine whether or not a monster should always
// fire this spell if selected.  If not, we should use a
// tracer.

// note - this function assumes that the monster is "nearby"
// its target!

bool ms_always_fire(int monspell)
{
    bool always = true;

    switch(monspell)
    {
        case MS_BANISHMENT:
        case MS_COLD_BOLT:
        case MS_CONFUSE:
        case MS_CRYSTAL_SPEAR:
        case MS_DISINTEGRATE:
        case MS_ENERGY_BOLT:
        case MS_FIRE_BOLT:
        case MS_FIREBALL:
        case MS_FLAME:
        case MS_FROST:
        case MS_HELLFIRE:
        case MS_IRON_BOLT:
        case MS_LIGHTNING_BOLT:
        case MS_MARSH_GAS:
        case MS_METAL_SPLINTERS:
        case MS_MMISSILE:
        case MS_NEGATIVE_BOLT:
        case MS_ORB_ENERGY:
        case MS_PAIN:
        case MS_PARALYSIS:
        case MS_POISON_BLAST:
        case MS_POISON_SPLASH:
        case MS_QUICKSILVER_BOLT:
        case MS_SLOW:
        case MS_STEAM_BALL:
        case MS_STICKY_FLAME:
        case MS_STING:
        case MS_STONE_ARROW:
        case MS_TELEPORT_OTHER:
        case MS_VENOM_BOLT:
            always = false;
            break;

        // self-niceties and direct effects
        case MS_ANIMATE_DEAD:
        case MS_BLINK:
        case MS_BRAIN_FEED:
        case MS_DIG:
        case MS_FAKE_RAKSHASA_SUMMON:
        case MS_HASTE:
        case MS_HEAL:
        case MS_HELLFIRE_BURST:
        case MS_INVIS:
        case MS_LEVEL_SUMMON:
        case MS_MUTATION:
        case MS_SMITE:
        case MS_SUMMON_BEAST:
        case MS_SUMMON_DEMON_LESSER:
        case MS_SUMMON_DEMON:
        case MS_SUMMON_DEMON_GREATER:
        case MS_SUMMON_UFETUBUS:
        case MS_TELEPORT:
        case MS_TORMENT:
        case MS_VAMPIRE_SUMMON:

        // meaningless, but sure, why not?
        case MS_NO_SPELL:
            break;

        default:
            break;

    }

    return always;
}

// use of variant:
// 0 : She is tap dancing.
// 1 : It seems she is tap dancing. (lower case pronoun)
// 2 : Her sword explodes!          (upper case possessive)
// 3 : It sticks to her sword!      (lower case possessive)
// ... as needed

const char *mons_pronoun(int mon_type, int variant)
{
    int gender = 0;         // its,  1 = male, 2=female

    if (mon_type >= MONS_TERENCE && mon_type <= MONS_BORIS)
    {
        switch(mon_type)
        {
            case MONS_JESSICA:
            case MONS_PSYCHE:
            case MONS_JOSEPHINE:
            case MONS_AGNES:
            case MONS_MAUD:
            case MONS_LOUISE:
            case MONS_FRANCIS:
            case MONS_MARGERY:
            case MONS_EROLCHA:
                gender = 2;
                break;
            default:
                gender = 1;
                break;
        }
    }

    switch(variant)
    {
        case 0:
            return (gender==0)?"It":
                   (gender==1)?"He":"She";
        case 1:
            return (gender==0)?"it":
                   (gender==1)?"he":"she";
        case 2:
            return (gender==0)?"Its":
                   (gender==1)?"His":"Her";
        case 3:
            return (gender==0)?"its":
                   (gender==1)?"his":"her";
    }

    return "";
}

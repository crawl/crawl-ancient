#ifndef M_LIST_H
#define M_LIST_H
//      !!! the big monster list !!!
// note: the order of the list doesn't matter

//#include "colours.h"
#include "defines.h"

/*
   This whole file was very generously condensed from its initial ugly form
   by Wladimir van der Laan ($pellbinder).
 */


/*
   (see "mstruct.h")

   {monster_class, showchar, colour, name, flags,
   weight, exp_mod, charclass, holiness, resist_magic, damage[4],
   hp_dice[4], AC, ev, speed, speed_inc, sec (spell), corpse_thingy, zombie_size, shouts, intel, gmon_use},

   // Explanation:

   monster_class
   - the number which refers to the monster. If this is greater than the greater
   demons here (*not* the demon lords) and it is not unique, you must change
   the random number thing in place_monster in builder2.cc. 160 - 220 are free.

   showchar
   - the monster's symbol

   colour
   - if BLACK, uses value of mons_sec

   name
   - the monster's name; some monsters don't have one, but it is generated
   whenever the moname function is called (eg zombies, skeletons)

   flags
   - various monster properties

   weight
   - weight of monster's corpse. If 0, monster doesn't leave a corpse.

   exp_mod
   - the experience given for killing this monster is calculated something
   like this:
   experience = hp_max * HD * HD * exp_mod / 10
   I think.

   charclass
   - this is the base monster type of a classed monster, eg 14 (orc) for
   an orc knight.

   holiness
   - 4 values:
   H_HOLY - this does almost nothing, except irritate some of the Gods if
   you kill one.
   H_NORMAL - takes normal damage from negative energy, pain, torment etc;
   H_UNDEAD - takes no damage from these things, but extra from weapons
   of holy wraith/maces of disruption. Is affected by repel
   undead/holy word.
   H_DEMONIC - takes no dam from neg energy/pain stuff. Takes lots extra
   from weapons of holy wrath, but no extra from disruption.
   Is affected by holy word but not repel undead.
   Note - this does not mean hellfire resistance (see flags)

   resist_magic
   - If -3, = 3 (5?) * hit dice

   damage [4]
   - up to 4 different attacks

   hp_dice [4]
   - hit dice, min hp per HD, extra random hp per HD, fixed HP (unique mons)

   AC
   - Armour class

   ev
   - evasion

   speed
   - less = slower. 5 = half, 20 = double speed.

   speed_inc
   - this is unnecessary and should be removed. 7 for all monsters.

   corpse_thingy
   - err, bad name. Describes effects of eating corpses.

   zombie_size
   - 0 = no zombie possibly, 1 = small zombie (z), 2 = large zombie (Z)

   shouts
   - various things monsters can do upon seeing you
   #define S_RANDOM -1
   #define S_SILENT 0
   #define S_SHOUT 1 //1=shout
   #define S_BARK 2 //2=bark
   #define S_SHOUT2 3 //3=shout twice - eg 2-headed ogres
   #define S_ROAR 4 //4=roar
   #define S_SCREAM 5 //5=scream
   #define S_BELLOW 6 //6=bellow (?)
   #define S_SCREECH 7 //7=screech
   #define S_BUZZ 8 //8=buzz
   #define S_MOAN 9 //9=moan

   intel
   - How smart it is. So far, differences here have little effects except
   for monster's chance of seeing you if stealthy, and really stupid monsters
   will walk through clouds

   gmon_use
   - 0 = uses nothing, 1 = opens doors, 3 = can use weapons/armour

 */


// monster 250: The Thing That Should Not Be (tm)
// do not remove, or seekmonster will crash on unknown mc request
// it is also a good prototype for new monsters
{
    250, 'B', LIGHTRED, "program bug", M_NO_FLAGS,
        0, 10, 250, H_NORMAL, -3,
    {
        0, 0, 0, 0
    }
    ,
    {
        0, 0, 0, 0
    }
    ,0, 0, 0, 0, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

// real monsters
{
    0, 'a', 8, "giant ant", M_ED_POISON,
        700, 10, 0, H_NORMAL, -3,
    {
        8, 0, 0, 0
    }
    ,
    {
        3, 3, 5, 0
    }
    ,4, 10, 12, 7, 250, C_CLEAN, Z_SMALL, S_SILENT, I_INSECT, 0
}
,

{
    1, 'b', 8, "giant bat", M_FLIES | M_SEE_INVIS,
        150, 10, 1, H_NORMAL, -3,
    {
        1, 0, 0, 0
    }
    ,
    {
        1, 2, 3, 0
    }
    ,1, 14, 30, 7, 250, C_CLEAN, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    2, 'c', BROWN, "centaur", M_NO_FLAGS,
        1500, 10, 2, H_NORMAL, -3,
    {
        10, 0, 0, 0
    }
    ,
    {
        4, 3, 5, 0
    }
    ,3, 7, 15, 7, 250, C_CLEAN, Z_BIG, S_SHOUT, I_NORMAL, 3
}
,

{
    3, '4', 4, "red devil", M_RES_POISON | M_RES_HELLFIRE | M_ED_COLD | M_FLIES,
        0, 10, 3, H_DEMONIC, -7,
    {
        18, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,10, 10, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 1
}
,

{
    4, 'e', 6, "ettin", M_NO_FLAGS,
        0, 10, 4, H_NORMAL, -3,
    {
        18, 12, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,3, 4, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 1
}
,

{
    5, 'f', 7, "fungus", M_NO_EXP_GAIN | M_RES_POISON,
        0, 10, 5, H_NORMAL, 5000,
    {
        0, 0, 0, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,1, 0, 0, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,
/*
   // plain silly (test if changing works)
   {6, 'D', 13, "purple dinosaur", M_NO_FLAGS,
   5000, 10, 6, H_HOLY, -3, {10,10,10,10},
   {10,10,10,80}, 1, 4, 12, 7, 250, C_CLEAN, Z_BIG, S_SCREAM, I_HIGH, 3},
 */

// this is what it should be
{
    6, 'g', 7, "goblin", M_NO_FLAGS,
        400, 10, 6, H_NORMAL, -3,
    {
        4, 0, 0, 0
    }
    ,
    {
        1, 2, 4, 0
    }
    ,1, 13, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    7, 'h', 6, "hound", M_SEE_INVIS,
        300, 10, 7, H_NORMAL, -3,
    {
        6, 0, 0, 0
    }
    ,
    {
        3, 3, 5, 0
    }
    ,2, 13, 15, 7, 250, C_CLEAN, Z_SMALL, S_BARK, I_ANIMAL, 0
}
,

// note: these things regenerate
{
    8, '5', RED, "imp", M_RES_POISON | M_RES_HELLFIRE | M_ED_COLD | M_FLIES | M_SEE_INVIS | M_SPEAKS,
        0, 13, 8, H_DEMONIC, -9,
    {
        4, 0, 0, 0
    }
    ,
    {
        3, 3, 3, 0
    }
    ,3, 14, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SHOUT, I_HIGH, 1
}
,

{
    9, 'j', 14, "jackal", M_NO_FLAGS,
        200, 10, 9, H_NORMAL, -3,
    {
        3, 0, 0, 0
    }
    ,
    {
        1, 3, 5, 0
    }
    ,2, 12, 14, 7, 250, C_CONTAMINATED, Z_SMALL, S_BARK, I_ANIMAL, 0
}
,

{
    10, 'k', 14, "killer bee", M_ED_POISON | M_FLIES,
        150, 11, 10, H_NORMAL, -3,
    {
        10, 0, 0, 0
    }
    ,
    {
        3, 3, 5, 0
    }
    ,2, 18, 20, 7, 250, C_POISON, Z_SMALL, S_BUZZ, I_INSECT, 0
}
,

{
    11, 'w', 7, "killer bee larva", M_ED_POISON | M_NO_SKELETON,
        150, 5, 11, H_NORMAL, -3,
    {
        3, 0, 0, 0
    }
    ,
    {
        1, 3, 5, 0
    }
    ,1, 5, 5, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_INSECT, 0
}
,

{
    12, 'm', 6, "manticore", M_NO_FLAGS,
        1800, 10, 12, H_NORMAL, -3,
    {
        14, 8, 8, 0
    }
    ,
    {
        9, 3, 5, 0
    }
    ,5, 7, 7, 7, 250, C_CONTAMINATED, Z_BIG, S_SILENT, I_NORMAL, 1
}
,

// this thing doesn't have nr. 13 for nothing, has it? ($pellbinder)
{
    13, 'n', DARKGREY, "necrophage", M_RES_POISON | M_RES_COLD,
        500, 10, 13, H_UNDEAD, -5,
    {
        8, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,2, 10, 10, 7, 250, C_HCL, Z_NOZOMBIE, S_SILENT, I_NORMAL, 1
}
,

{
    14, 'o', 12, "orc", M_NO_FLAGS,
        600, 10, 14, H_NORMAL, -3,
    {
        5, 0, 0, 0
    }
    ,
    {
        1, 4, 6, 0
    }
    ,0, 10, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    15, 'p', BLUE, "phantom", M_RES_POISON | M_RES_COLD,
        0, 10, 15, H_UNDEAD, -4,
    {
        10, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,3, 13, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 1
}
,

{
    16, 'q', 7, "quasit", M_RES_POISON | M_RES_FIRE | M_RES_COLD,
        0, 10, 16, H_NORMAL, 50,
    {
        3, 2, 2, 0
    }
    ,
    {
        3, 2, 6, 0
    }
    ,5, 17, 15, 7, 250, C_POISON, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    17, 'r', 6, "rat", M_NO_FLAGS,
        200, 10, 17, H_NORMAL, -3,
    {
        3, 0, 0, 0
    }
    ,
    {
        1, 1, 3, 0
    }
    ,1, 10, 10, 7, 250, C_CLEAN, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    18, 's', DARKGREY, "scorpion", M_ED_POISON,
        500, 10, 18, H_NORMAL, -3,
    {
        10, 0, 0, 0
    }
    ,
    {
        3, 3, 5, 0
    }
    ,5, 10, 10, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_INSECT, 0
}
,

{
    19, 't', 12, "tunneling worm", M_RES_POISON,
        0, 10, 19, H_NORMAL, 5000,
    {
        50, 0, 0, 0
    }
    ,
    {
        10, 5, 5, 0
    }
    ,3, 3, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_ANIMAL, 0
}
,

{
    20, 'u', BROWN, "ugly thing", M_NO_FLAGS,
        600, 10, 20, H_NORMAL, -3,
    {
        12, 0, 0, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,3, 10, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 1
}
,

{
    21, 'v', RED, "fire vortex", M_RES_POISON | M_RES_FIRE | M_ED_COLD | M_LEVITATE,
        0, 5, 21, H_NORMAL, 5000,
    {
        30, 0, 0, 0
    }
    ,
    {
        3, 3, 5, 0
    }
    ,0, 5, 15, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    22, 'w', LIGHTRED, "worm", M_NO_SKELETON,
        350, 4, 22, H_NORMAL, -3,
    {
        12, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,1, 5, 6, 7, 250, C_CLEAN, Z_SMALL, S_SILENT, I_INSECT, 0
}
,

// random
{
    23, 'x', BLACK, "abomination", M_NO_FLAGS,
        0, 10, 23, H_DEMONIC, -5,
    {
        23, 0, 0, 0
    }
    ,
    {
        6, 2, 5, 0
    }
    ,0, 0, 0, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    24, 'y', 14, "yellow wasp", M_ED_POISON | M_FLIES,
        220, 12, 24, H_NORMAL, -3,
    {
        13, 0, 0, 0
    }
    ,
    {
        4, 3, 5, 0
    }
    ,5, 14, 15, 7, 250, C_POISON, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

// small zombie
{
    25, 'z', 6, "", M_RES_POISON | M_RES_COLD,
        0, 6, 25, H_UNDEAD, 5000,
    {
        10, 0, 0, 0
    }
    ,
    {
        1, 5, 5, 0
    }
    ,0, 4, 5, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    26, 'A', WHITE, "Angel", M_RES_POISON | M_FLIES | M_RES_ELEC | M_SPELLCASTER,
        0, 10, 26, H_HOLY, -8,
    {
        20, 0, 0, 0
    }
    ,
    {
        9, 3, 5, 0
    }
    ,10, 10, 10, 7, 111, C_NOCORPSE, Z_NOZOMBIE, S_SHOUT, I_HIGH, 3
}
,

{
    27, 'B', 8, "giant beetle", M_ED_POISON,
        1000, 10, 27, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        5, 7, 6, 0
    }
    ,10, 3, 5, 7, 250, C_POISON, Z_BIG, S_SILENT, I_INSECT, 0
}
,

{
    28, 'C', 6, "cyclops", M_NO_FLAGS,
        2500, 10, 28, H_NORMAL, -3,
    {
        35, 0, 0, 0
    }
    ,
    {
        9, 3, 5, 0
    }
    ,5, 3, 7, 7, 250, C_CLEAN, Z_BIG, S_SHOUT, I_NORMAL, 1
}
,

{
    29, 'D', GREEN, "dragon", M_RES_POISON | M_RES_FIRE | M_ED_COLD | M_FLIES,
        2200, 12, 29, H_NORMAL, -4,
    {
        20, 13, 13, 0
    }
    ,
    {
        12, 5, 5, 0
    }
    ,10, 8, 10, 7, 250, C_CLEAN, Z_BIG, S_SILENT, I_NORMAL, 1
}
,

{
    30, 'O', LIGHTRED, "two-headed ogre", M_NO_FLAGS,
        1500, 11, 30, H_NORMAL, -4,
    {
        17, 13, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,1, 4, 8, 7, 250, C_CONTAMINATED, Z_BIG, S_SHOUT2, I_NORMAL, 1
}
,

{
    31, '1', RED, "Fiend", M_RES_POISON | M_RES_HELLFIRE | M_ED_COLD | M_FLIES | M_SEE_INVIS,
        0, 18, 31, H_DEMONIC, -12,
    {
        25, 15, 15, 0
    }
    ,
    {
        18, 3, 5, 0
    }
    ,15, 6, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_ROAR, I_HIGH, 1
}
,

{
    32, 'G', GREEN, "giant spore", M_RES_POISON | M_LEVITATE,
        0, 10, 32, H_NORMAL, -3,
    {
        1, 0, 0, 0
    }
    ,
    {
        1, 0, 0, 1
    }
    ,0, 10, 15, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    33, 'g', 6, "hobgoblin", M_NO_FLAGS,
        500, 10, 33, H_NORMAL, -3,
    {
        5, 0, 0, 0
    }
    ,
    {
        1, 4, 5, 0
    }
    ,0, 10, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    34, 'I', WHITE, "ice beast", M_RES_POISON | M_ED_FIRE | M_RES_COLD,
        0, 12, 34, H_NORMAL, -3,
    {
        5, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,5, 10, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_ANIMAL, 0
}
,

{
    35, 'J', LIGHTRED, "jelly", M_RES_POISON,
        0, 13, 35, H_NORMAL, -3,
    {
        8, 0, 0, 0
    }
    ,
    {
        3, 5, 5, 0
    }
    ,0, 2, 9, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    36, 'K', 6, "kobold", M_NO_FLAGS,
        400, 10, 36, H_NORMAL, -3,
    {
        4, 0, 0, 0
    }
    ,
    {
        1, 2, 3, 0
    }
    ,2, 12, 10, 7, 250, C_POISON, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    37, 'L', 15, "lich", M_RES_POISON | M_RES_COLD | M_SPELLCASTER | M_SEE_INVIS,
        0, 16, 37, H_UNDEAD, -11,
    {
        15, 0, 0, 0
    }
    ,
    {
        20, 2, 4, 0
    }
    ,10, 10, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SHOUT, I_HIGH, 1
}
,

{
    38, 'M', 15, "mummy", M_RES_POISON | M_ED_FIRE | M_RES_COLD,
        0, 10, 38, H_UNDEAD, -5,
    {
        20, 0, 0, 0
    }
    ,
    {
        3, 5, 3, 0
    }
    ,3, 6, 6, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    39, 'N', LIGHTGREEN, "guardian naga", M_RES_POISON | M_SPELLCASTER | M_SEE_INVIS,
        350, 10, 39, H_NORMAL, -6,
    {
        19, 0, 0, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,6, 14, 15, 7, 10, C_MUTATE, Z_SMALL, S_SHOUT, I_HIGH, 1
}
,

{
    40, 'O', 6, "ogre", M_NO_FLAGS,
        1300, 10, 40, H_NORMAL, -3,
    {
        17, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,1, 6, 10, 7, 250, C_CONTAMINATED, Z_BIG, S_SHOUT, I_NORMAL, 1
}
,

{
    41, 'P', 2, "plant", M_NO_EXP_GAIN,
        0, 10, 41, H_NORMAL, 5000,
    {
        0, 0, 0, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,10, 0, 0, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    42, 'Q', YELLOW, "queen bee", M_ED_POISON | M_FLIES,
        200, 14, 42, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,10, 10, 10, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_INSECT, 0
}
,

{
    43, 'R', 14, "rakshasa", M_RES_POISON | M_SPELLCASTER | M_SEE_INVIS,
        0, 15, 43, H_NORMAL, -10,
    {
        20, 0, 0, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,10, 14, 10, 7, 54, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_HIGH, 3
}
,

{
    44, 'S', GREEN, "snake", M_NO_FLAGS,
        200, 10, 44, H_NORMAL, -3,
    {
        5, 0, 0, 0
    }
    ,
    {
        2, 3, 5, 0
    }
    ,1, 15, 13, 7, 250, C_CONTAMINATED, Z_SMALL, S_SILENT, I_PLANT, 0
}
,

{
    45, 'T', 6, "troll", M_NO_FLAGS,
        1500, 10, 45, H_NORMAL, -3,
    {
        20, 15, 15, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,3, 10, 10, 7, 250, C_CONTAMINATED, Z_BIG, S_SHOUT, I_NORMAL, 1
}
,

{
    46, 'x', MAGENTA, "unseen horror", M_LEVITATE | M_SEE_INVIS | M_INVIS,
        0, 12, 46, H_NORMAL, -3,
    {
        12, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,5, 10, 30, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_ANIMAL, 0
}
,

{
    47, 'V', RED, "vampire", M_RES_POISON | M_RES_COLD | M_SPELLCASTER | M_SEE_INVIS,
        0, 11, 47, H_UNDEAD, -6,
    {
        22, 0, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,10, 10, 10, 7, 40, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 1
}
,

{
    48, 'W', DARKGREY, "wraith", M_RES_POISON | M_RES_COLD | M_LEVITATE | M_SEE_INVIS,
        0, 11, 48, H_UNDEAD, -7,
    {
        13, 0, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,10, 10, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 1
}
,

// Large abom: (the previous one was small)
{
    49, 'X', BLACK, "abomination", M_NO_FLAGS,
        0, 10, 49, H_DEMONIC, -7,
    {
        40, 0, 0, 0
    }
    ,
    {
        11, 2, 5, 0
    }
    ,0, 0, 0, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    50, 'Y', BROWN, "yak", M_NO_FLAGS,
        1200, 10, 50, H_NORMAL, -3,
    {
        18, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,4, 7, 10, 7, 250, C_CLEAN, Z_BIG, S_BELLOW, I_ANIMAL, 0
}
,

// big zombie
{
    51, 'Z', BROWN, "", M_RES_POISON | M_RES_COLD,
        0, 6, 51, H_UNDEAD, 5000,
    {
        23, 0, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,8, 5, 5, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    52, 'o', 14, "orc warrior", M_NO_FLAGS,
        0, 10, 14, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        4, 4, 6, 0
    }
    ,0, 13, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    53, 'K', MAGENTA, "kobold demonologist", M_SPELLCASTER,
        0, 10, 36, H_NORMAL, -5,
    {
        4, 0, 0, 0
    }
    ,
    {
        4, 3, 5, 0
    }
    ,2, 13, 10, 7, 250, C_POISON, Z_NOZOMBIE, S_SHOUT, I_NORMAL, 3
}
,

{
    54, 'o', MAGENTA, "orc wizard", M_SPELLCASTER,
        0, 10, 14, H_NORMAL, -5,
    {
        10, 0, 0, 0
    }
    ,
    {
        3, 3, 4, 0
    }
    ,1, 12, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    55, 'o', LIGHTCYAN, "orc knight", M_NO_FLAGS,
        0, 10, 14, H_NORMAL, -3,
    {
        25, 0, 0, 0
    }
    ,
    {
        9, 4, 7, 0
    }
    ,2, 13, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    56, '#', LIGHTRED, "worm tail", M_NO_EXP_GAIN | M_RES_POISON,
        0, 10, 56, H_NORMAL, 5000,
    {
        0, 0, 0, 0
    }
    ,
    {
        10, 5, 5, 0
    }
    ,3, 3, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    57, 'D', LIGHTRED, "wyvern", M_NO_FLAGS,
        2000, 10, 57, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,5, 10, 15, 7, 250, C_CLEAN, Z_SMALL, S_SILENT, I_PLANT, 0
}
,

{
    58, 'K', RED, "big kobold", M_NO_FLAGS,
        0, 10, 58, H_NORMAL, -3,
    {
        7, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,3, 12, 10, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_ANIMAL, 3
}
,

{
    59, 'G', WHITE, "giant eyeball", M_NO_SKELETON | M_LEVITATE,
        400, 10, 59, H_NORMAL, -3,
    {
        0, 0, 0, 0
    }
    ,
    {
        3, 3, 5, 0
    }
    ,0, 1, 3, 7, 250, C_CLEAN, Z_SMALL, S_SILENT, I_PLANT, 0
}
,

{
    60, 'W', LIGHTGREY, "wight", M_RES_POISON | M_RES_COLD,
        0, 10, 60, H_UNDEAD, -4,
    {
        8, 0, 0, 0
    }
    ,
    {
        3, 3, 5, 0
    }
    ,4, 10, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 3
}
,

{
    61, 'P', GREEN, "oklob plant", M_RES_POISON,
        0, 10, 61, H_NORMAL, -3,
    {
        0, 0, 0, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,10, 0, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    62, 's', BROWN, "wolf spider", M_ED_POISON,
        800, 10, 62, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,3, 10, 15, 7, 250, C_POISON, Z_BIG, S_SILENT, I_INSECT, 0
}
,

{
    63, ' ', 0, "shadow", M_RES_POISON | M_RES_COLD,
        0, 10, 63, H_UNDEAD, -5,
    {
        5, 0, 0, 0
    }
    ,
    {
        3, 3, 5, 0
    }
    ,12, 10, 10, 7, BLACK, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    64, 'p', GREEN, "hungry ghost", M_RES_POISON | M_RES_COLD,
        0, 10, 64, H_UNDEAD, -4,
    {
        5, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,0, 17, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 1
}
,

{
    65, 'G', LIGHTGREY, "eye of draining", M_NO_SKELETON | M_LEVITATE,
        400, 10, 65, H_NORMAL, 5000,
    {
        0, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,3, 1, 5, 7, 250, C_MUTATE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    66, 'b', BLACK, "butterfly", M_FLIES | M_ED_POISON,
        150, 10, 66, H_NORMAL, -3,
    {
        0, 0, 0, 0
    }
    ,
    {
        1, 3, 5, 0
    }
    ,0, 25, 25, 7, 250, C_CLEAN, Z_SMALL, S_SILENT, I_INSECT, 0
}
,

{
    67, 'f', BROWN, "wandering mushroom", M_RES_POISON,
        0, 10, 67, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,5, 0, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    68, 'E', RED, "efreet", M_RES_POISON | M_RES_FIRE | M_ED_COLD | M_SPELLCASTER | M_LEVITATE,
        0, 12, 68, H_NORMAL, -3,
    {
        12, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,10, 5, 10, 7, 50, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_HIGH, 3
}
,

{
    69, 'w', LIGHTMAGENTA, "brain worm", M_ED_POISON | M_SPELLCASTER,
        150, 10, 69, H_NORMAL, -3,
    {
        0, 0, 0, 0
    }
    ,
    {
        5, 3, 3, 0
    }
    ,1, 5, 10, 7, 52, C_POISON, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    70, 'G', LIGHTRED, "giant orange brain", M_NO_SKELETON | M_SPELLCASTER | M_LEVITATE,
        1000, 13, 70, H_NORMAL, -8,
    {
        0, 0, 0, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,2, 4, 10, 7, 53, C_MUTATE, Z_NOZOMBIE, S_SILENT, I_HIGH, 1
}
,

{
    71, 'B', LIGHTGREY, "boulder beetle", M_ED_POISON,
        2500, 10, 71, H_NORMAL, -3,
    {
        35, 0, 0, 0
    }
    ,
    {
        9, 3, 5, 0
    }
    ,20, 2, 3, 7, 250, C_POISON, Z_BIG, S_SILENT, I_INSECT, 0
}
,

{
    72, 'z', WHITE, "flying skull", M_RES_POISON | M_RES_FIRE | M_RES_COLD | M_LEVITATE,
        0, 10, 72, H_UNDEAD, -3,
    {
        7, 0, 0, 0
    }
    ,
    {
        2, 3, 5, 0
    }
    ,10, 17, 15, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SCREAM, I_PLANT, 0
}
,

{
    73, 'h', DARKGREY, "hell hound", M_RES_POISON | M_RES_HELLFIRE | M_ED_COLD | M_SEE_INVIS,
        0, 10, 73, H_DEMONIC, -3,
    {
        13, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,6, 13, 15, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_BARK, I_NORMAL, 0
}
,

{
    74, 'm', LIGHTRED, "minotaur", M_NO_FLAGS,
        1500, 10, 74, H_NORMAL, -3,
    {
        35, 0, 0, 0
    }
    ,
    {
        13, 3, 5, 0
    }
    ,5, 7, 10, 7, 250, C_CONTAMINATED, Z_BIG, S_BELLOW, I_NORMAL, 3
}
,

{
    75, 'D', WHITE, "ice dragon", M_RES_POISON | M_ED_FIRE | M_RES_COLD | M_FLIES,
        2200, 10, 75, H_NORMAL, -3,
    {
        17, 17, 17, 0
    }
    ,
    {
        12, 5, 5, 0
    }
    ,10, 8, 10, 7, 250, C_CLEAN, Z_BIG, S_SILENT, I_NORMAL, 1
}
,

{
    76, 'J', GREEN, "slime creature", M_RES_POISON,
        0, 5, 76, H_NORMAL, -3,
    {
        22, 0, 0, 0
    }
    ,
    {
        11, 3, 5, 0
    }
    ,1, 4, 7, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    77, 'W', LIGHTBLUE, "freezing wraith", M_RES_POISON | M_ED_FIRE | M_RES_COLD | M_LEVITATE | M_SEE_INVIS,
        0, 10, 77, H_UNDEAD, -4,
    {
        19, 0, 0, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,12, 10, 8, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 1
}
,

// fake R - conjured by the R's illusion spell.
{
    78, 'R', YELLOW, "rakshasa", M_RES_POISON,
        0, 10, 78, H_NORMAL, 5000,
    {
        0, 0, 0, 0
    }
    ,
    {
        1, 0, 0, 1
    }
    ,0, 30, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_ROAR, I_PLANT, 0
}
,

{
    79, 'G', GREEN, "great orb of eyes", M_NO_SKELETON | M_RES_POISON | M_SPELLCASTER | M_LEVITATE | M_SEE_INVIS,
        900, 13, 79, H_NORMAL, 5000,
    {
        20, 0, 0, 0
    }
    ,
    {
        12, 3, 5, 0
    }
    ,10, 3, 10, 7, 55, C_MUTATE, Z_NOZOMBIE, S_SILENT, I_HIGH, 1
}
,

{
    80, '3', BLACK, "hellion", M_RES_POISON | M_RES_HELLFIRE | M_ED_COLD | M_SPELLCASTER,
        0, 11, 80, H_DEMONIC, -7,
    {
        10, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,5, 10, 10, 7, RED, C_NOCORPSE, Z_NOZOMBIE, S_SCREAM, I_HIGH, 1
}
,

{
    81, '4', GREEN, "rotting devil", M_RES_POISON | M_RES_COLD,
        0, 10, 81, H_DEMONIC, -7,
    {
        8, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,2, 10, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_HIGH, 1
}
,

{
    82, '3', YELLOW, "tormentor", M_RES_POISON | M_RES_FIRE | M_SPELLCASTER | M_FLIES | M_SPEAKS,
        0, 10, 82, H_DEMONIC, -6,
    {
        8, 8, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,12, 12, 13, 7, 72, C_NOCORPSE, Z_NOZOMBIE, S_ROAR, I_HIGH, 1
}
,

{
    83, '2', LIGHTGREY, "reaper", M_RES_POISON | M_RES_COLD | M_SEE_INVIS,
        0, 10, 83, H_DEMONIC, 5000,
    {
        32, 0, 0, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,15, 10, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_HIGH, 1
}
,

{
    84, '2', DARKGREY, "soul eater", M_RES_POISON | M_RES_COLD | M_LEVITATE | M_SEE_INVIS,
        0, 12, 84, H_DEMONIC, -10,
    {
        25, 0, 0, 0
    }
    ,
    {
        11, 3, 5, 0
    }
    ,18, 10, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_HIGH, 1
}
,

{
    85, '4', LIGHTRED, "hairy devil", M_RES_POISON,
        0, 10, 85, H_DEMONIC, -4,
    {
        9, 9, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,7, 10, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SHOUT, I_HIGH, 1
}
,

{
    86, '2', WHITE, "ice devil", M_RES_POISON | M_ED_FIRE | M_RES_COLD | M_SEE_INVIS,
        0, 11, 86, H_DEMONIC, -6,
    {
        16, 0, 0, 0
    }
    ,
    {
        11, 3, 5, 0
    }
    ,12, 10, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_HIGH, 1
}
,

{
    87, '3', BLUE, "blue devil", M_RES_POISON | M_ED_FIRE | M_RES_COLD | M_FLIES,
        0, 10, 87, H_DEMONIC, -5,
    {
        21, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,14, 10, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_HIGH, 1
}
,

// random
{
    88, '4', BROWN, "beast", M_NO_FLAGS,
        0, 10, 88, H_DEMONIC, -3,
    {
        12, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,0, 0, 0, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_RANDOM, I_NORMAL, 1
}
,

{
    89, '3', CYAN, "iron devil", M_RES_ELEC | M_RES_POISON | M_RES_HELLFIRE | M_RES_COLD,
        0, 10, 89, H_DEMONIC, -6,
    {
        14, 14, 0, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,16, 8, 8, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SCREECH, I_HIGH, 1
}
,

{
    98, '@', RED, "glowing shapeshifter", M_NO_FLAGS,
        600, 10, 99, H_NORMAL, -6,
    {
        15, 0, 0, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,0, 10, 10, 7, 250, C_MUTATE, Z_SMALL, S_SILENT, I_HIGH, 1
}
,

{
    99, '@', LIGHTRED, "shapeshifter", M_NO_FLAGS,
        600, 10, 99, H_NORMAL, -6,
    {
        5, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,0, 10, 10, 7, 250, C_MUTATE, Z_SMALL, S_SILENT, I_HIGH, 1
}
,

{
    100, 's', LIGHTRED, "giant mite", M_ED_POISON,
        350, 10, 100, H_NORMAL, -3,
    {
        5, 0, 0, 0
    }
    ,
    {
        2, 3, 5, 0
    }
    ,1, 7, 10, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_INSECT, 0
}
,

{
    101, 'd', LIGHTGREY, "steam dragon", M_SPELLCASTER | M_FLIES,
        1000, 10, 101, H_NORMAL, -3,
    {
        12, 0, 0, 0
    }
    ,
    {
        4, 5, 5, 0
    }
    ,5, 10, 10, 7, 57, C_CLEAN, Z_SMALL, S_SILENT, I_ANIMAL, 1
}
,

{
    102, 'u', RED, "very ugly thing", M_NO_FLAGS,
        750, 10, 102, H_NORMAL, -3,
    {
        17, 0, 0, 0
    }
    ,
    {
        12, 3, 5, 0
    }
    ,4, 8, 8, 7, 250, C_MUTATE, Z_BIG, S_SHOUT, I_NORMAL, 3
}
,

{
    103, 'o', DARKGREY, "orc sorceror", M_RES_FIRE | M_SPELLCASTER | M_SEE_INVIS | M_SPEAKS,
        600, 12, 14, H_NORMAL, -3,
    {
        7, 0, 0, 0
    }
    ,
    {
        8, 2, 3, 0
    }
    ,5, 12, 10, 7, 56, C_POISON, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    104, 'H', BROWN, "hippogriff", M_FLIES,
        1000, 10, 104, H_NORMAL, -3,
    {
        10, 8, 8, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,2, 7, 10, 7, 250, C_CLEAN, Z_BIG, S_SCREECH, I_ANIMAL, 1
}
,

{
    105, 'H', YELLOW, "griffon", M_FLIES,
        1800, 10, 105, H_NORMAL, -3,
    {
        18, 10, 10, 0
    }
    ,
    {
        12, 3, 5, 0
    }
    ,4, 6, 10, 7, 250, C_CLEAN, Z_BIG, S_SCREECH, I_ANIMAL, 1
}
,

{
    106, 'D', LIGHTGREEN, "hydra", M_RES_POISON,
        1800, 11, 106, H_NORMAL, -3,
    {
        18, 0, 0, 0
    }
    ,
    {
        13, 3, 5, 0
    }
    ,0, 5, 10, 7, 250, C_POISON, Z_NOZOMBIE, S_ROAR, I_ANIMAL, 1
}
,

// small skeleton
{
    107, 'z', LIGHTGREY, "", M_RES_POISON | M_RES_COLD,
        0, 10, 107, H_UNDEAD, 5000,
    {
        0, 0, 0, 0
    }
    ,
    {
        0, 0, 0, 0
    }
    ,0, 0, 0, 0, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

// large skeleton
{
    108, 'Z', LIGHTGREY, "", M_RES_POISON | M_RES_COLD,
        0, 10, 108, H_UNDEAD, 5000,
    {
        0, 0, 0, 0
    }
    ,
    {
        0, 0, 0, 0
    }
    ,0, 0, 0, 0, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    109, '@', RED, "hell knight", M_RES_FIRE | M_SPELLCASTER,
        550, 10, 114, H_NORMAL, -3,
    {
        13, 0, 0, 0
    }
    ,
    {
        10, 3, 6, 0
    }
    ,0, 10, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    110, '@', DARKGREY, "necromancer", M_SPELLCASTER,
        550, 10, 114, H_NORMAL, -4,
    {
        6, 0, 0, 0
    }
    ,
    {
        10, 2, 4, 0
    }
    ,0, 13, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    111, '@', MAGENTA, "wizard", M_SPELLCASTER | M_SPEAKS,
        550, 10, 114, H_NORMAL, -4,
    {
        6, 0, 0, 0
    }
    ,
    {
        10, 2, 4, 0
    }
    ,0, 13, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    112, 'o', LIGHTGREEN, "orc priest", M_SPELLCASTER,
        600, 10, 14, H_NORMAL, -4,
    {
        6, 0, 0, 0
    }
    ,
    {
        3, 3, 4, 0
    }
    ,1, 10, 10, 7, 67, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    113, 'o', GREEN, "orc high priest", M_RES_HELLFIRE | M_SPELLCASTER | M_SEE_INVIS | M_SPEAKS,
        600, 10, 14, H_UNDEAD, -4,
    {
        7, 0, 0, 0
    }
    ,
    {
        11, 3, 4, 0
    }
    ,1, 12, 10, 7, 68, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

// this is a dummy monster, used for corpses
{
    114, 0, LIGHTRED, "human", M_NO_FLAGS,
        550, 10, 114, H_NORMAL, -3,
    {
        6, 0, 0, 0
    }
    ,
    {
        1, 3, 5, 0
    }
    ,0, 10, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    115, 'g', YELLOW, "gnoll", M_NO_FLAGS,
        750, 10, 115, H_NORMAL, -3,
    {
        9, 0, 0, 0
    }
    ,
    {
        2, 4, 5, 0
    }
    ,2, 9, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    116, '8', BROWN, "clay golem", M_RES_POISON | M_RES_FIRE | M_RES_COLD | M_SEE_INVIS,
        0, 10, 116, H_NORMAL, 5000,
    {
        11, 11, 0, 0
    }
    ,
    {
        8, 7, 3, 0
    }
    ,7, 5, 8, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    117, '8', YELLOW, "wood golem", M_RES_POISON | M_ED_FIRE | M_RES_COLD,
        0, 10, 117, H_NORMAL, 5000,
    {
        10, 0, 0, 0
    }
    ,
    {
        6, 6, 3, 0
    }
    ,5, 6, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    118, '8', LIGHTGREY, "stone golem", M_RES_POISON | M_RES_FIRE | M_RES_COLD,
        0, 10, 118, H_NORMAL, 5000,
    {
        28, 0, 0, 0
    }
    ,
    {
        12, 7, 4, 0
    }
    ,12, 4, 7, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    119, '8', CYAN, "iron golem", M_RES_POISON | M_RES_FIRE | M_RES_COLD,
        0, 10, 119, H_NORMAL, 5000,
    {
        35, 0, 0, 0
    }
    ,
    {
        15, 7, 4, 0
    }
    ,15, 3, 7, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    120, '8', WHITE, "crystal golem", M_RES_POISON | M_RES_FIRE | M_RES_COLD,
        0, 10, 120, H_NORMAL, 5000,
    {
        40, 0, 0, 0
    }
    ,
    {
        13, 7, 4, 0
    }
    ,22, 3, 7, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    121, '8', LIGHTGREY, "toenail golem", M_RES_POISON | M_RES_FIRE | M_RES_COLD,
        0, 10, 121, H_NORMAL, 5000,
    {
        13, 0, 0, 0
    }
    ,
    {
        9, 5, 3, 0
    }
    ,8, 5, 8, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    122, 'd', LIGHTMAGENTA, "mottled dragon", M_RES_POISON | M_RES_FIRE | M_SPELLCASTER | M_FLIES,
        1100, 10, 122, H_NORMAL, -3,
    {
        15, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,5, 10, 10, 7, 69, C_POISON, Z_SMALL, S_SILENT, I_ANIMAL, 1
}
,

{
    123, '#', BROWN, "earth elemental", M_RES_POISON | M_RES_FIRE | M_RES_COLD,
        0, 10, 123, H_NORMAL, -3,
    {
        40, 0, 0, 0
    }
    ,
    {
        10, 5, 5, 0
    }
    ,14, 4, 6, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    124, '#', YELLOW, "fire elemental", M_RES_POISON | M_RES_HELLFIRE | M_ED_COLD,
        0, 10, 124, H_NORMAL, -3,
    {
        5, 0, 0, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,4, 12, 13, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    125, 'v', LIGHTGREY, "air elemental", M_RES_ELEC | M_RES_POISON | M_LEVITATE,
        0, 10, 125, H_NORMAL, -3,
    {
        15, 0, 0, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,2, 18, 25, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

// water elementals are later (with the other water monsters)

{
    126, '1', WHITE, "Ice Fiend", M_RES_POISON | M_ED_FIRE | M_RES_COLD | M_SPELLCASTER | M_FLIES | M_SEE_INVIS,
        0, 10, 126, H_DEMONIC, -12,
    {
        25, 25, 0, 0
    }
    ,
    {
        18, 3, 5, 0
    }
    ,15, 6, 10, 7, 70, C_CONTAMINATED, Z_NOZOMBIE, S_ROAR, I_HIGH, 1
}
,

{
    127, '1', DARKGREY, "Shadow Fiend", M_RES_POISON | M_RES_COLD | M_SPELLCASTER | M_LEVITATE | M_SEE_INVIS,
        0, 10, 127, H_DEMONIC, -13,
    {
        25, 15, 15, 0
    }
    ,
    {
        18, 3, 5, 0
    }
    ,15, 6, 10, 7, 71, C_CONTAMINATED, Z_NOZOMBIE, S_ROAR, I_HIGH, 1
}
,

{
    128, 'S', BROWN, "brown snake", M_RES_POISON,
        300, 10, 128, H_NORMAL, -3,
    {
        10, 0, 0, 0
    }
    ,
    {
        4, 3, 5, 0
    }
    ,2, 15, 14, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    129, 'l', BROWN, "giant lizard", M_NO_FLAGS,
        600, 10, 129, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,4, 10, 10, 7, 250, C_CLEAN, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    130, 'W', LIGHTGREEN, "spectral warrior", M_RES_POISON | M_RES_COLD | M_LEVITATE | M_SEE_INVIS,
        0, 13, 130, H_UNDEAD, -6,
    {
        18, 0, 0, 0
    }
    ,
    {
        9, 3, 5, 0
    }
    ,12, 10, 10, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SILENT, I_HIGH, 1
}
,

{
    131, 'J', RED, "pulsating lump", M_RES_POISON,
        0, 3, 131, H_NORMAL, -3,
    {
        13, 0, 0, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,2, 6, 4, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    132, 'D', LIGHTBLUE, "storm dragon", M_RES_ELEC | M_RES_COLD | M_SPELLCASTER | M_FLIES,
        2800, 12, 132, H_NORMAL, -6,
    {
        25, 15, 15, 0
    }
    ,
    {
        14, 5, 5, 0
    }
    ,13, 10, 12, 7, 73, C_CLEAN, Z_BIG, S_ROAR, I_NORMAL, 1
}
,

{
    133, 'c', LIGHTRED, "yaktaur", M_NO_FLAGS,
        2000, 10, 133, H_NORMAL, -3,
    {
        15, 0, 0, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,4, 4, 10, 7, 250, C_CONTAMINATED, Z_BIG, S_SHOUT, I_NORMAL, 3
}
,

{
    134, 'Y', DARKGREY, "death yak", M_NO_FLAGS,
        1500, 10, 134, H_NORMAL, -5,
    {
        30, 0, 0, 0
    }
    ,
    {
        14, 3, 5, 0
    }
    ,9, 5, 10, 7, 250, C_POISON, Z_BIG, S_BELLOW, I_ANIMAL, 0
}
,

{
    135, 'T', LIGHTGREY, "rock troll", M_NO_FLAGS,
        2200, 11, 135, H_NORMAL, -4,
    {
        30, 20, 20, 0
    }
    ,
    {
        11, 3, 5, 0
    }
    ,13, 6, 8, 7, 250, C_CONTAMINATED, Z_BIG, S_SHOUT, I_NORMAL, 1
}
,

{
    136, 'C', LIGHTGREY, "stone giant", M_NO_FLAGS,
        3000, 10, 136, H_NORMAL, -4,
    {
        45, 0, 0, 0
    }
    ,
    {
        16, 3, 5, 0
    }
    ,12, 2, 10, 7, 250, C_CONTAMINATED, Z_BIG, S_SHOUT, I_NORMAL, 1
}
,

{
    137, 'p', RED, "flayed ghost", M_RES_POISON,
        0, 10, 137, H_UNDEAD, -5,
    {
        30, 0, 0, 0
    }
    ,
    {
        11, 3, 5, 0
    }
    ,0, 14, 10, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    138, 'k', RED, "bumblebee", M_ED_POISON | M_FLIES,
        300, 10, 138, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,4, 15, 10, 7, 250, C_POISON, Z_SMALL, S_BUZZ, I_PLANT, 0
}
,

{
    139, 's', RED, "redback", M_ED_POISON,
        1000, 14, 139, H_NORMAL, -3,
    {
        18, 0, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,2, 12, 15, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_INSECT, 0
}
,

{
    140, 'p', LIGHTGREY, "insubstantial wisp", M_RES_ELEC | M_RES_POISON | M_RES_FIRE | M_RES_COLD | M_LEVITATE,
        0, 17, 140, H_NORMAL, 5000,
    {
        12, 0, 0, 0
    }
    ,
    {
        6, 1, 2, 0
    }
    ,20, 20, 10, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_MOAN, I_PLANT, 0
}
,

{
    141, '#', LIGHTGREY, "vapour", M_RES_ELEC | M_RES_POISON | M_SPELLCASTER | M_LEVITATE | M_SEE_INVIS | M_INVIS,
        0, 21, 141, H_NORMAL, 5000,
    {
        0, 0, 0, 0
    }
    ,
    {
        12, 2, 3, 0
    }
    ,0, 12, 10, 7, 73, C_CONTAMINATED, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    142, 'O', MAGENTA, "ogre-mage", M_SPELLCASTER | M_SEE_INVIS,
        0, 16, 40, H_NORMAL, -6,
    {
        12, 0, 0, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,1, 7, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    143, 'w', DARKGREY, "spiny worm", M_ED_POISON,
        1300, 13, 143, H_NORMAL, -3,
    {
        32, 0, 0, 0
    }
    ,
    {
        12, 3, 5, 0
    }
    ,10, 6, 9, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_PLANT, 0
}
,

// these are named more explicitly when they attack, also when you use 'x'
//  to examine them.
{
    144, '(', BLACK, "dancing weapon", M_RES_POISON | M_RES_HELLFIRE | M_RES_COLD | M_LEVITATE,
        0, 20, 144, H_NORMAL, 5000,
    {
        15, 0, 0, 0
    }
    ,
    {
        15, 1, 0, 0
    }
    ,10, 20, 15, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    145, 'C', MAGENTA, "titan", M_RES_ELEC | M_SPELLCASTER,
        3500, 12, 145, H_NORMAL, -7,
    {
        55, 0, 0, 0
    }
    ,
    {
        20, 3, 5, 0
    }
    ,10, 3, 10, 7, 94, C_CLEAN, Z_BIG, S_SHOUT, I_HIGH, 1
}
,

{
    146, 'D', YELLOW, "golden dragon", M_RES_ELEC | M_RES_POISON | M_RES_FIRE | M_RES_COLD | M_SPELLCASTER | M_FLIES,
        3000, 17, 146, H_NORMAL, -6,
    {
        40, 20, 20, 0
    }
    ,
    {
        18, 4, 4, 0
    }
    ,15, 7, 10, 7, 95, C_POISON, Z_BIG, S_ROAR, I_HIGH, 1
}
,

// dummy monster, used for corpses etc
{
    147, 'e', DARKGREY, "elf", M_NO_FLAGS,
        450, 10, 147, H_NORMAL, -3,
    {
        0, 0, 0, 0
    }
    ,
    {
        0, 0, 0, 0
    }
    ,0, 0, 0, 0, 250, C_CONTAMINATED, Z_SMALL, S_SILENT, I_PLANT, 3
}
,


{
    148, 'l', GREEN, "lindworm", M_NO_FLAGS,
        1000, 11, 148, H_NORMAL, -3,
    {
        30, 0, 0, 0
    }
    ,
    {
        9, 3, 5, 0
    }
    ,8, 6, 10, 7, 250, C_CLEAN, Z_BIG, S_ROAR, I_ANIMAL, 0
}
,

{
    149, 'm', LIGHTGREY, "elephant slug", M_ED_POISON | M_NO_SKELETON,
        1500, 10, 149, H_NORMAL, -3,
    {
        40, 0, 0, 0
    }
    ,
    {
        20, 5, 3, 0
    }
    ,2, 1, 4, 10, 250, C_POISON, Z_BIG, S_SILENT, I_INSECT, 0
}
,

{
    150, 'h', CYAN, "war dog", M_SEE_INVIS,
        350, 10, 150, H_NORMAL, -3,
    {
        12, 0, 0, 0
    }
    ,
    {
        4, 3, 5, 0
    }
    ,4, 15, 17, 7, 250, C_CONTAMINATED, Z_SMALL, S_BARK, I_ANIMAL, 0
}
,

{
    151, 'r', LIGHTGREY, "grey rat", M_NO_FLAGS,
        250, 10, 151, H_NORMAL, -3,
    {
        5, 0, 0, 0
    }
    ,
    {
        1, 3, 6, 0
    }
    ,2, 12, 12, 7, 250, C_CONTAMINATED, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    152, 'r', LIGHTGREEN, "green rat", M_NO_FLAGS,
        250, 10, 152, H_NORMAL, -3,
    {
        10, 0, 0, 0
    }
    ,
    {
        2, 3, 5, 0
    }
    ,5, 11, 10, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    153, 'r', LIGHTRED, "orange rat", M_NO_FLAGS,
        250, 10, 153, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        3, 3, 5, 0
    }
    ,7, 10, 12, 7, 250, C_POISON, Z_SMALL, S_ROAR, I_ANIMAL, 0
}
,

{
    154, 'S', DARKGREY, "black snake", M_RES_POISON,
        500, 12, 154, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,4, 15, 18, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    155, 'Y', LIGHTGREY, "sheep", M_NO_FLAGS,
        1200, 10, 155, H_NORMAL, -3,
    {
        13, 0, 0, 0
    }
    ,
    {
        3, 3, 5, 0
    }
    ,2, 7, 10, 7, 250, C_CLEAN, Z_SMALL, S_BELLOW, I_ANIMAL, 0
}
,

{
    156, 'n', RED, "ghoul", M_RES_POISON | M_RES_COLD,
        500, 12, 156, H_UNDEAD, -5,
    {
        9, 0, 0, 0
    }
    ,
    {
        4, 3, 5, 0
    }
    ,4, 10, 10, 7, 250, C_HCL, Z_NOZOMBIE, S_SILENT, I_NORMAL, 1
}
,

{
    157, 'h', LIGHTRED, "hog", M_NO_FLAGS,
        700, 10, 157, H_NORMAL, -3,
    {
        14, 0, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,2, 9, 13, 7, 250, C_CLEAN, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    158, 'y', DARKGREY, "giant mosquito", M_ED_POISON | M_FLIES,
        100, 10, 158, H_NORMAL, -3,
    {
        10, 0, 0, 0
    }
    ,
    {
        1, 3, 5, 0
    }
    ,0, 13, 12, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_WHINE, I_PLANT, 0
}
,

{
    159, 's', GREEN, "giant centipede", M_ED_POISON,
        350, 10, 159, H_NORMAL, -3,
    {
        2, 0, 0, 0
    }
    ,
    {
        2, 3, 3, 0
    }
    ,2, 14, 13, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_INSECT, 0
}
,



{
    160, 'T', CYAN, "iron troll", M_RES_FIRE | M_RES_COLD,
        2400, 10, 160, H_NORMAL, -5,
    {
        35, 25, 25, 0
    }
    ,
    {
        16, 3, 5, 0
    }
    ,20, 4, 7, 7, 250, C_POISON, Z_BIG, S_ROAR, I_NORMAL, 1
}
,

{
    161, 'N', GREEN, "naga", M_RES_POISON | M_SPELLCASTER | M_SEE_INVIS,
        750, 10, 161, H_NORMAL, -6,
    {
        6, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,6, 10, 8, 7, 106, C_POISON, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    162, 'C', RED, "fire giant", M_RES_FIRE | M_SPELLCASTER,
        2400, 11, 162, H_NORMAL, -4,
    {
        30, 0, 0, 0
    }
    ,
    {
        16, 3, 6, 0
    }
    ,8, 4, 10, 7, 50, C_CONTAMINATED, Z_BIG, S_SHOUT, I_NORMAL, 3
}
,

{
    163, 'C', LIGHTBLUE, "frost giant", M_RES_COLD | M_SPELLCASTER,
        2600, 11, 163, H_NORMAL, -4,
    {
        35, 0, 0, 0
    }
    ,
    {
        16, 4, 5, 0
    }
    ,9, 3, 10, 7, 110, C_CONTAMINATED, Z_BIG, S_SHOUT, I_NORMAL, 3
}
,

{
    164, 'd', RED, "firedrake", M_RES_FIRE | M_FLIES,
        900, 10, 164, H_NORMAL, -3,
    {
        8, 0, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,3, 12, 12, 7, 250, C_CONTAMINATED, Z_SMALL, S_SILENT, I_ANIMAL, 1
}
,

{
    165, 'D', DARKGREY, "shadow dragon", M_RES_POISON | M_RES_COLD | M_SPELLCASTER | M_FLIES | M_SEE_INVIS,
        2000, 12, 165, H_NORMAL, -5,
    {
        20, 15, 15, 0
    }
    ,
    {
        17, 5, 5, 0
    }
    ,15, 10, 10, 7, 113, C_CLEAN, Z_BIG, S_ROAR, I_HIGH, 1
}
,

{
    166, 'S', YELLOW, "yellow snake", M_RES_POISON,
        400, 10, 166, H_NORMAL, -3,
    {
        15, 0, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,4, 14, 13, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    167, 'S', LIGHTGREY, "grey snake", M_NO_FLAGS,
        600, 10, 167, H_NORMAL, -3,
    {
        30, 0, 0, 0
    }
    ,
    {
        11, 3, 5, 0
    }
    ,4, 16, 18, 7, 250, C_CLEAN, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    168, 'T', DARKGREY, "deep troll", M_NO_FLAGS,
        1500, 12, 168, H_NORMAL, -3,
    {
        27, 20, 20, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,6, 10, 10, 7, 250, C_CONTAMINATED, Z_BIG, S_SHOUT, I_NORMAL, 1
}
,

{
    169, 'y', LIGHTGREY, "giant blowfly", M_ED_POISON | M_FLIES,
        200, 10, 169, H_NORMAL, -3,
    {
        13, 0, 0, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,2, 15, 19, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_BUZZ, I_PLANT, 0
}
,

{
    170, 'y', RED, "red wasp", M_ED_POISON | M_FLIES,
        400, 14, 170, H_NORMAL, -3,
    {
        23, 0, 0, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,7, 14, 15, 7, 250, C_POISON, Z_NOZOMBIE, S_BUZZ, I_PLANT, 0
}
,

{
    171, 'D', BROWN, "swamp dragon", M_SPELLCASTER | M_FLIES | M_RES_POISON,
        1900, 11, 171, H_NORMAL, -3,
    {
        13, 9, 9, 0
    }
    ,
    {
        9, 5, 5, 0
    }
    ,7, 7, 9, 7, 121, C_CONTAMINATED, Z_BIG, S_ROAR, I_ANIMAL, 1
}
,

{
    172, 'd', BROWN, "swamp drake", M_SPELLCASTER | M_FLIES | M_RES_POISON,
        900, 11, 172, H_NORMAL, -3,
    {
        11, 0, 0, 0
    }
    ,
    {
        4, 5, 5, 0
    }
    ,3, 11, 11, 7, 122, C_CONTAMINATED, Z_SMALL, S_ROAR, I_ANIMAL, 1
}
,

{
    173, 'a', LIGHTGREY, "soldier ant", M_ED_POISON,
        900, 10, 173, H_NORMAL, -3,
    {
        14, 0, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,8, 10, 10, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_INSECT, 0
}
,

{
    174, 'C', LIGHTRED, "hill giant", M_NO_FLAGS,
        1600, 10, 174, H_NORMAL, -3,
    {
        30, 0, 0, 0
    }
    ,
    {
        11, 3, 5, 0
    }
    ,3, 4, 10, 7, 250, C_CONTAMINATED, Z_BIG, S_SHOUT, I_NORMAL, 1
}
,

{
    175, 'Q', DARKGREY, "queen ant", M_ED_POISON,
        1200, 10, 175, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        13, 3, 5, 0
    }
    ,14, 3, 7, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_INSECT, 0
}
,

{
    176, 'w', LIGHTGREY, "ant larva", M_ED_POISON | M_NO_SKELETON,
        350, 5, 176, H_NORMAL, -3,
    {
        5, 0, 0, 0
    }
    ,
    {
        2, 3, 5, 0
    }
    ,2, 6, 6, 7, 250, C_POISON, Z_SMALL, S_SILENT, I_INSECT, 0
}
,


{
    177, 'F', GREEN, "giant frog", M_NO_FLAGS,
        500, 10, 177, H_NORMAL, -3,
    {
        9, 0, 0, 0
    }
    ,
    {
        4, 3, 5, 0
    }
    ,0, 12, 15, 7, 250, C_CLEAN, Z_SMALL, S_CROAK, I_ANIMAL, 0
}
,

{
    178, 'F', BROWN, "giant brown frog", M_NO_FLAGS,
        890, 10, 178, H_NORMAL, -3,
    {
        14, 0, 0, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,2, 11, 13, 7, 250, C_CLEAN, Z_BIG, S_CROAK, I_ANIMAL, 0
}
,

{
    179, 'F', YELLOW, "spiny frog", M_NO_FLAGS,
        1000, 10, 179, H_NORMAL, -3,
    {
        26, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,6, 9, 12, 7, 250, C_CLEAN, Z_SMALL, S_CROAK, I_ANIMAL, 0
}
,

{
    180, 'F', LIGHTGREEN, "blink frog", M_NO_FLAGS,
        800, 12, 180, H_NORMAL, -5,
    {
        20, 0, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,3, 12, 14, 7, 250, C_CLEAN, Z_SMALL, S_CROAK, I_ANIMAL, 0
}
,



{
    220, '5', WHITE, "white imp", M_RES_COLD | M_SPELLCASTER | M_FLIES,
        0, 10, 220, H_DEMONIC, -3,
    {
        4, 0, 0, 0
    }
    ,
    {
        2, 3, 5, 0
    }
    ,4, 10, 10, 7, 74, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_NORMAL, 3
}
,

{
    221, '5', YELLOW, "lemure", M_RES_POISON,
        0, 10, 221, H_DEMONIC, -3,
    {
        12, 0, 0, 0
    }
    ,
    {
        2, 3, 5, 0
    }
    ,1, 12, 12, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_MOAN, I_NORMAL, 1
}
,

{
    222, '5', LIGHTCYAN, "ufetubus", M_ED_FIRE | M_RES_COLD,
        0, 10, 222, H_DEMONIC, -3,
    {
        5, 5, 0, 0
    }
    ,
    {
        1, 4, 6, 0
    }
    ,2, 15, 15, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_NORMAL, 1
}
,

{
    223, '5', LIGHTRED, "manes", M_RES_ELEC | M_RES_FIRE | M_RES_COLD | M_RES_POISON,
        0, 10, 223, H_DEMONIC, -3,
    {
        5, 3, 3, 0
    }
    ,
    {
        3, 3, 5, 0
    }
    ,2, 8, 8, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_NORMAL, 1
}
,

{
    224, '5', LIGHTGREEN, "midge", M_RES_POISON | M_FLIES,
        0, 10, 224, H_DEMONIC, -3,
    {
        8, 0, 0, 0
    }
    ,
    {
        2, 3, 5, 0
    }
    ,4, 10, 10, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_NORMAL, 3
}
,

{
    225, '3', MAGENTA, "neqoxec", M_RES_POISON | M_SPELLCASTER | M_LEVITATE,
        0, 12, 225, H_DEMONIC, -6,
    {
        15, 0, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,4, 12, 10, 7, 76, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_NORMAL, 1
}
,

{
    226, '3', LIGHTRED, "orange demon", M_NO_FLAGS,
        0, 12, 226, H_DEMONIC, -6,
    {
        10, 5, 0, 0
    }
    ,
    {
        8, 4, 5, 0
    }
    ,3, 7, 7, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SCREECH, I_NORMAL, 0
}
,

{
    227, '3', LIGHTGREY, "hellwing", M_RES_POISON | M_SPELLCASTER | M_FLIES,
        0, 12, 227, H_DEMONIC, -6,
    {
        17, 10, 0, 0
    }
    ,
    {
        7, 4, 5, 0
    }
    ,8, 10, 10, 7, 77, C_CONTAMINATED, Z_NOZOMBIE, S_MOAN, I_NORMAL, 1
}
,

{
    228, '4', LIGHTGREY, "smoke demon", M_RES_POISON | M_RES_FIRE | M_SPELLCASTER | M_FLIES,
        0, 12, 228, H_DEMONIC, -6,
    {
        8, 5, 5, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,5, 9, 9, 7, 78, C_CONTAMINATED, Z_NOZOMBIE, S_ROAR, I_NORMAL, 3
}
,

{
    229, '3', CYAN, "ynoxinul", M_RES_ELEC | M_RES_POISON | M_RES_COLD | M_SPELLCASTER | M_FLIES | M_SEE_INVIS,
        0, 12, 229, H_DEMONIC, -6,
    {
        12, 0, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,3, 10, 10, 7, 75, C_CONTAMINATED, Z_NOZOMBIE, S_BELLOW, I_NORMAL, 1
}
,

{
    230, '1', DARKGREY, "Executioner", M_RES_POISON,
        0, 14, 230, H_DEMONIC, -9,
    {
        30, 0, 0, 0
    }
    ,
    {
        12, 3, 5, 0
    }
    ,7, 8, 8, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SCREAM, I_HIGH, 1
}
,

{
    231, '1', GREEN, "Green Death", M_RES_POISON | M_SPELLCASTER,
        0, 14, 231, H_DEMONIC, -9,
    {
        32, 0, 0, 0
    }
    ,
    {
        13, 3, 5, 0
    }
    ,5, 7, 12, 7, 80, C_CONTAMINATED, Z_NOZOMBIE, S_ROAR, I_HIGH, 3
}
,

{
    232, '1', BLUE, "Blue Death", M_RES_POISON | M_ED_FIRE | M_RES_COLD | M_FLIES | M_SEE_INVIS,
        0, 14, 232, H_DEMONIC, -9,
    {
        20, 20, 0, 0
    }
    ,
    {
        12, 3, 5, 0
    }
    ,10, 10, 10, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_HIGH, 3
}
,

{
    233, '1', RED, "Balrug", M_RES_POISON | M_RES_HELLFIRE | M_ED_COLD | M_SPELLCASTER | M_FLIES,
        0, 14, 233, H_DEMONIC, -9,
    {
        25, 0, 0, 0
    }
    ,
    {
        14, 3, 5, 0
    }
    ,5, 12, 10, 7, 81, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_HIGH, 3
}
,

{
    234, '1', YELLOW, "Cacodemon", M_RES_POISON | M_SPELLCASTER | M_LEVITATE | M_SEE_INVIS,
        0, 14, 234, H_DEMONIC, -9,
    {
        22, 0, 0, 0
    }
    ,
    {
        13, 3, 5, 0
    }
    ,11, 10, 10, 7, 79, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_HIGH, 1
}
,


{
    235, '3', DARKGREY, "demonic crawler", M_RES_ELEC | M_RES_POISON | M_RES_COLD | M_RES_FIRE | M_SEE_INVIS,
        0, 12, 235, H_DEMONIC, -6,
    {
        13, 13, 13, 13
    }
    ,
    {
        9, 3, 5, 0
    }
    ,10, 6, 9, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SCREAM, I_NORMAL, 1
}
,

{
    236, '2', YELLOW, "sun demon", M_RES_ELEC | M_RES_POISON | M_ED_COLD | M_RES_HELLFIRE | M_SEE_INVIS | M_LEVITATE,
        0, 14, 236, H_DEMONIC, -6,
    {
        30, 0, 0, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,10, 12, 12, 7, 250, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_NORMAL, 3
}
,

{
    237, '5', DARKGREY, "shadow imp", M_RES_COLD | M_SPELLCASTER | M_FLIES | M_RES_POISON,
        0, 11, 237, H_DEMONIC, -3,
    {
        6, 0, 0, 0
    }
    ,
    {
        2, 3, 5, 0
    }
    ,3, 11, 10, 7, 118, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_NORMAL, 1
}
,

{
    238, '3', DARKGREY, "shadow demon", M_RES_POISON | M_RES_COLD | M_SEE_INVIS | M_INVIS,
        0, 12, 238, H_DEMONIC, -7,
    {
        21, 0, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,7, 12, 11, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_CROAK, I_HIGH, 1
}
,

{
    239, '2', BLUE, "Lorocyproca", M_RES_POISON | M_RES_COLD | M_RES_FIRE | M_RES_ELEC | M_SEE_INVIS | M_INVIS,
        0, 12, 239, H_DEMONIC, -7,
    {
        25, 25, 0, 0
    }
    ,
    {
        12, 3, 5, 0
    }
    ,10, 12, 9, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_MOAN, I_HIGH, 1
}
,

{
    240, 'W', BLUE, "shadow wraith", M_RES_POISON | M_LEVITATE | M_SEE_INVIS | M_INVIS,
        0, 15, 240, H_UNDEAD, -8,
    {
        20, 0, 0, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,7, 7, 10, 10, 250, C_CONTAMINATED, Z_NOZOMBIE, S_MOAN, I_HIGH, 1
}
,

{
    241, 'J', BLUE, "giant amoeba", M_RES_POISON | M_NO_SKELETON,
        1000, 10, 241, H_NORMAL, -3,
    {
        25, 0, 0, 0
    }
    ,
    {
        12, 3, 5, 0
    }
    ,0, 4, 8, 10, 250, C_POISON, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    242, 'm', GREEN, "giant slug", M_ED_POISON | M_NO_SKELETON,
        700, 10, 242, H_NORMAL, -3,
    {
        23, 0, 0, 0
    }
    ,
    {
        10, 5, 3, 0
    }
    ,0, 2, 6, 10, 250, C_POISON, Z_BIG, S_SILENT, I_INSECT, 0
}
,

{
    243, 'm', LIGHTGREEN, "giant snail", M_ED_POISON | M_NO_SKELETON,
        900, 10, 243, H_NORMAL, -3,
    {
        18, 0, 0, 0
    }
    ,
    {
        14, 5, 3, 0
    }
    ,7, 2, 4, 10, 250, C_POISON, Z_BIG, S_SILENT, I_INSECT, 0
}
,

{
    244, 'v', BLACK, "spatial vortex", M_RES_POISON | M_RES_FIRE | M_RES_COLD | M_RES_ELEC | M_LEVITATE,
        0, 5, 244, H_NORMAL, 5000,
    {
        50, 0, 0, 0
    }
    ,
    {
        6, 6, 6, 0
    }
    ,0, 5, 15, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,
// To add a new vortex, must look for references to 244 in CRAWL99, MONSPLS3, BUILDER2

{
    245, '1', BROWN, "Pit Fiend", M_RES_POISON | M_RES_HELLFIRE | M_RES_COLD | M_FLIES | M_SEE_INVIS | M_RES_ELEC,
        0, 18, 245, H_DEMONIC, -12,
    {
        28, 21, 21, 0
    }
    ,
    {
        19, 4, 5, 0
    }
    ,17, 5, 8, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_ROAR, I_HIGH, 1
}
,

{
    246, 'B', BROWN, "boring beetle", M_ED_POISON,
        1300, 10, 246, H_NORMAL, -3,
    {
        26, 0, 0, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,13, 4, 6, 7, 250, C_POISON, Z_BIG, S_SILENT, I_INSECT, 0
}
,

{
    247, 'g', DARKGREY, "gargoyle", M_RES_POISON | M_FLIES,
        0, 12, 247, H_NORMAL, -6,
    {
        10, 6, 6, 0
    }
    ,
    {
        4, 3, 5, 0
    }
    ,18, 6, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 0
}
,

// only appear in Dis castle
{
    248, 'g', CYAN, "metal gargoyle", M_RES_POISON | M_FLIES,
        0, 12, 248, H_NORMAL, -6,
    {
        19, 10, 10, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,20, 4, 7, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_HIGH, 0
}
,

// only appear in Gehenna castle
{
    249, 'g', RED, "molten gargoyle", M_RES_POISON | M_FLIES | M_RES_FIRE,
        0, 12, 249, H_NORMAL, -6,
    {
        12, 8, 8, 0
    }
    ,
    {
        5, 3, 5, 0
    }
    ,14, 7, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 0
}
,


// 250 can't exist


{
    251, '&', LIGHTMAGENTA, "Mnoleg", M_RES_ELEC | M_RES_POISON | M_RES_FIRE | M_SEE_INVIS | M_SPELLCASTER,
        0, 25, 251, H_DEMONIC, 5000,
    {
        23, 23, 0, 0
    }
    ,
    {
        17, 0, 0, 199
    }
    ,10, 13, 13, 7, 90, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_HIGH, 1
}
,

{
    252, '&', LIGHTGREY, "Lom Lobon", M_RES_POISON | M_RES_FIRE | M_RES_COLD | M_LEVITATE | M_SEE_INVIS | M_SPELLCASTER,
        0, 25, 252, H_DEMONIC, 5000,
    {
        40, 0, 0, 0
    }
    ,
    {
        19, 0, 0, 223
    }
    ,10, 7, 8, 7, 91, C_CONTAMINATED, Z_NOZOMBIE, S_SILENT, I_HIGH, 1
}
,

{
    253, '&', RED, "Cerebov", M_RES_ELEC | M_RES_POISON | M_RES_HELLFIRE | M_SPELLCASTER,
        0, 25, 253, H_DEMONIC, -6,
    {
        50, 0, 0, 0
    }
    ,
    {
        21, 0, 0, 253
    }
    ,15, 8, 10, 7, 92, C_CONTAMINATED, Z_NOZOMBIE, S_SILENT, I_NORMAL, 3
}
,

{
    254, '&', DARKGREY, "Gloorx Vloq", M_RES_POISON | M_RES_COLD | M_LEVITATE | M_SEE_INVIS | M_SPELLCASTER,
        0, 25, 254, H_DEMONIC, -14,
    {
        20, 0, 0, 0
    }
    ,
    {
        16, 0, 0, 234
    }
    ,10, 10, 10, 7, 93, C_CONTAMINATED, Z_NOZOMBIE, S_MOAN, I_HIGH, 1
}
,

/*{255, 'U', GREEN, "The Mollusc Lord", M_RES_POISON,
   0, 25, 255, H_DEMONIC, -3, {30,0,0,0},
   {16,0,0,100}, 10, 10, 10, 7, 93, C_POISON, Z_NOZOMBIE, S_SILENT, I_HIGH, 1}, */

{
    260, 'N', LIGHTRED, "naga mage", M_RES_POISON | M_SPELLCASTER | M_SEE_INVIS,
        750, 13, 161, H_NORMAL, -6,
    {
        5, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,6, 10, 8, 7, 107, C_POISON, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    261, 'N', BLUE, "naga warrior", M_RES_POISON | M_SPELLCASTER | M_SEE_INVIS,
        750, 12, 161, H_NORMAL, -6,
    {
        11, 0, 0, 0
    }
    ,
    {
        10, 5, 5, 0
    }
    ,6, 10, 8, 7, 106, C_POISON, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    262, 'o', RED, "orc warlord", M_NO_FLAGS,
        600, 15, 14, H_NORMAL, -3,
    {
        32, 0, 0, 0
    }
    ,
    {
        15, 4, 7, 0
    }
    ,3, 10, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    263, 'e', CYAN, "deep elf soldier", M_SPELLCASTER,
        450, 10, 147, H_NORMAL, -6,
    {
        6, 0, 0, 0
    }
    ,
    {
        3, 3, 3, 0
    }
    ,0, 12, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    264, 'e', LIGHTBLUE, "deep elf fighter", M_SPELLCASTER,
        450, 10, 147, H_NORMAL, -6,
    {
        9, 0, 0, 0
    }
    ,
    {
        6, 3, 3, 0
    }
    ,0, 13, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    265, 'e', BLUE, "deep elf knight", M_SPELLCASTER,
        450, 10, 147, H_NORMAL, -6,
    {
        14, 0, 0, 0
    }
    ,
    {
        11, 3, 3, 0
    }
    ,0, 15, 11, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    266, 'e', LIGHTRED, "deep elf mage", M_SPELLCASTER,
        450, 10, 147, H_NORMAL, -6,
    {
        5, 0, 0, 0
    }
    ,
    {
        4, 3, 3, 0
    }
    ,0, 13, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    267, 'e', YELLOW, "deep elf summoner", M_SPELLCASTER,
        450, 10, 147, H_NORMAL, -6,
    {
        5, 0, 0, 0
    }
    ,
    {
        6, 3, 3, 0
    }
    ,0, 13, 10, 7, 96, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    268, 'e', LIGHTGREEN, "deep elf conjurer", M_SPELLCASTER,
        450, 10, 147, H_NORMAL, -6,
    {
        5, 0, 0, 0
    }
    ,
    {
        6, 3, 3, 0
    }
    ,0, 13, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    269, 'e', LIGHTGREY, "deep elf priest", M_SPELLCASTER,
        450, 10, 147, H_NORMAL, -6,
    {
        9, 0, 0, 0
    }
    ,
    {
        5, 3, 3, 0
    }
    ,0, 13, 10, 7, 99, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    270, 'e', DARKGREY, "deep elf high priest", M_SPELLCASTER | M_SPEAKS,
        450, 10, 147, H_NORMAL, -6,
    {
        14, 0, 0, 0
    }
    ,
    {
        11, 3, 3, 0
    }
    ,3, 13, 10, 7, 100, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    271, 'e', MAGENTA, "deep elf demonologist", M_SPELLCASTER,
        450, 10, 147, H_NORMAL, -6,
    {
        12, 0, 0, 0
    }
    ,
    {
        12, 3, 3, 0
    }
    ,0, 13, 10, 7, 101, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    272, 'e', GREEN, "deep elf annihilator", M_SPELLCASTER,
        450, 10, 147, H_NORMAL, -6,
    {
        12, 0, 0, 0
    }
    ,
    {
        15, 3, 3, 0
    }
    ,0, 13, 10, 7, 102, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    273, 'e', RED, "deep elf sorceror", M_SPELLCASTER,
        450, 10, 147, H_NORMAL, -6,
    {
        12, 0, 0, 0
    }
    ,
    {
        14, 3, 3, 0
    }
    ,0, 13, 10, 7, 103, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    274, 'e', WHITE, "deep elf death mage", M_SPELLCASTER,
        450, 10, 147, H_NORMAL, -6,
    {
        12, 0, 0, 0
    }
    ,
    {
        15, 3, 3, 0
    }
    ,0, 13, 10, 7, 104, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,




{
    275, 'J', LIGHTGREY, "ooze", M_RES_POISON | M_NO_SKELETON,
        0, 11, 275, H_NORMAL, -7,
    {
        25, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,10, 1, 6, 7, 250, C_POISON, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    276, 'J', LIGHTBLUE, "azure jelly", M_RES_POISON | M_RES_COLD | M_ED_FIRE | M_RES_ELEC | M_NO_SKELETON,
        0, 11, 276, H_NORMAL, -4,
    {
        12, 12, 12, 12
    }
    ,
    {
        15, 3, 5, 0
    }
    ,5, 10, 10, 7, 250, C_POISON, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    277, 'J', DARKGREY, "death ooze", M_RES_POISON | M_RES_COLD | M_NO_SKELETON,
        0, 13, 277, H_UNDEAD, -8,
    {
        32, 32, 0, 0
    }
    ,
    {
        11, 3, 3, 0
    }
    ,2, 4, 5, 7, 250, C_POISON, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    278, 'J', LIGHTGREEN, "acid blob", M_RES_POISON | M_NO_SKELETON,
        0, 12, 278, H_NORMAL, -7,
    {
        42, 0, 0, 0
    }
    ,
    {
        18, 3, 5, 0
    }
    ,1, 3, 7, 7, 250, C_POISON, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    279, 'J', YELLOW, "royal jelly", M_RES_POISON | M_NO_SKELETON,
        0, 20, 279, H_NORMAL, -7,
    {
        50, 0, 0, 0
    }
    ,
    {
        21, 0, 0, 111
    }
    ,8, 4, 9, 7, 250, C_POISON, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

/*
   {monster_class, showchar, colour, name, flags,
   weight, exp_mod, charclass, holiness, resist_magic, damage[4],
   hp_dice[4], AC, ev, speed, speed_inc, sec (spell), corpse_thingy, zombie_size, shouts, intel, gmon_use},
 */

{
    280, '@', LIGHTCYAN, "Terence", M_NO_FLAGS | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        3, 0, 0, 0
    }
    ,
    {
        1, 0, 0, 14
    }
    ,0, 10, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    281, '@', LIGHTGREY, "Jessica", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        4, 0, 0, 0
    }
    ,
    {
        1, 0, 0, 10
    }
    ,0, 10, 10, 7, 0, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    282, 'g', BLUE, "Ijyb", M_NO_FLAGS | M_SPEAKS,
        0, 20, 6, H_NORMAL, -3,
    {
        4, 0, 0, 0
    }
    ,
    {
        3, 0, 0, 28
    }
    ,0, 12, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    283, '@', YELLOW, "Sigmund", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        5, 0, 0, 0
    }
    ,
    {
        3, 0, 0, 25
    }
    ,0, 11, 10, 7, 1, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    284, 'o', BROWN, "Blork the orc", M_SPELLCASTER | M_SPEAKS,
        0, 20, 14, H_NORMAL, -3,
    {
        7, 0, 0, 0
    }
    ,
    {
        3, 0, 0, 32
    }
    ,0, 9, 8, 7, 2, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    285, '@', RED, "Edmund", M_NO_FLAGS | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        6, 0, 0, 0
    }
    ,
    {
        4, 0, 0, 27
    }
    ,0, 10, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    286, '@', LIGHTMAGENTA, "Psyche", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        7, 0, 0, 0
    }
    ,
    {
        5, 0, 0, 24
    }
    ,0, 12, 13, 7, 2, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    287, 'O', LIGHTBLUE, "Erolcha", M_NO_FLAGS | M_SPEAKS,
        0, 20, 40, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        6, 0, 0, 45
    }
    ,3, 7, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    288, '@', BLUE, "Donald", M_NO_FLAGS | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        8, 0, 0, 0
    }
    ,
    {
        5, 0, 0, 33
    }
    ,0, 10, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    289, 'o', RED, "Urug", M_NO_FLAGS | M_SPEAKS,
        0, 20, 14, H_NORMAL, -3,
    {
        12, 0, 0, 0
    }
    ,
    {
        6, 0, 0, 38
    }
    ,0, 11, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    290, '@', LIGHTGREY, "Michael", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        9, 0, 0, 0
    }
    ,
    {
        6, 0, 0, 36
    }
    ,0, 10, 10, 7, 2, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    291, '@', CYAN, "Joseph", M_NO_FLAGS | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        9, 0, 0, 0
    }
    ,
    {
        7, 0, 0, 42
    }
    ,0, 10, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    292, 'T', GREEN, "Anita", M_NO_FLAGS | M_SPEAKS,
        0, 20, 45, H_NORMAL, -3,
    {
        20, 15, 15, 0
    }
    ,
    {
        8, 0, 0, 45
    }
    ,0, 10, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 1
}
,

{
    293, '@', MAGENTA, "Erica", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        10, 0, 0, 0
    }
    ,
    {
        9, 0, 0, 43
    }
    ,0, 11, 11, 7, 63, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    294, '@', WHITE, "Josephine", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        11, 0, 0, 0
    }
    ,
    {
        9, 0, 0, 47
    }
    ,0, 10, 10, 7, 60, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    295, '@', LIGHTGREEN, "Harold", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        12, 0, 0, 0
    }
    ,
    {
        9, 0, 0, 51
    }
    ,0, 8, 10, 7, 59, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    296, '@', BROWN, "Norbert", M_NO_FLAGS | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        14, 0, 0, 0
    }
    ,
    {
        10, 0, 0, 53
    }
    ,0, 10, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    297, '@', LIGHTMAGENTA, "Jozef", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        14, 0, 0, 0
    }
    ,
    {
        11, 0, 0, 60
    }
    ,0, 9, 10, 7, 10, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    298, '@', LIGHTBLUE, "Agnes", M_NO_FLAGS | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        11, 0, 0, 0
    }
    ,
    {
        11, 0, 0, 64
    }
    ,0, 10, 15, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    299, '@', RED, "Maud", M_NO_FLAGS | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        14, 0, 0, 0
    }
    ,
    {
        13, 0, 0, 55
    }
    ,0, 10, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    300, '@', BLUE, "Louise", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        12, 0, 0, 0
    }
    ,
    {
        13, 0, 0, 52
    }
    ,0, 10, 10, 7, 65, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    301, '@', YELLOW, "Francis", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        12, 0, 0, 0
    }
    ,
    {
        14, 0, 0, 67
    }
    ,0, 10, 10, 7, 68, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    302, '@', YELLOW, "Frances", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        11, 0, 0, 0
    }
    ,
    {
        14, 0, 0, 70
    }
    ,0, 10, 10, 7, 68, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    303, '@', RED, "Rupert", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        13, 0, 0, 0
    }
    ,
    {
        16, 0, 0, 80
    }
    ,0, 10, 10, 7, 65, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    304, '@', YELLOW, "Wayne", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        14, 0, 0, 0
    }
    ,
    {
        17, 0, 0, 78
    }
    ,1, 10, 7, 7, 67, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    305, '@', YELLOW, "Duane", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        14, 0, 0, 0
    }
    ,
    {
        18, 0, 0, 83
    }
    ,0, 10, 10, 7, 0, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    306, 'D', RED, "Xtahua", M_NO_FLAGS | M_SPEAKS,
        0, 20, 29, H_NORMAL, -3,
    {
        29, 17, 17, 0
    }
    ,
    {
        19, 0, 0, 133
    }
    ,15, 7, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_ROAR, I_NORMAL, 1
}
,

{
    307, '@', LIGHTRED, "Norris", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        16, 0, 0, 0
    }
    ,
    {
        20, 0, 0, 95
    }
    ,1, 9, 9, 7, 59, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    308, '@', DARKGREY, "Adolf", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        17, 0, 0, 0
    }
    ,
    {
        21, 0, 0, 105
    }
    ,0, 10, 10, 7, 23, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    309, '@', RED, "Margery", M_SPELLCASTER | M_SPEAKS,
        0, 20, 114, H_NORMAL, -3,
    {
        18, 0, 0, 0
    }
    ,
    {
        22, 0, 0, 119
    }
    ,0, 10, 10, 7, 50, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    310, 'L', RED, "Boris", M_RES_POISON | M_RES_COLD | M_SPELLCASTER | M_SEE_INVIS | M_SPEAKS,
        0, 23, 37, H_UNDEAD, 300,
    {
        15, 0, 0, 0
    }
    ,
    {
        22, 0, 0, 99
    }
    ,12, 10, 10, 7, 23, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 1
}
,


{
    340, '&', GREEN, "Geryon", M_SPELLCASTER,
        0, 25, 340, H_DEMONIC, -5,
    {
        30, 0, 0, 0
    }
    ,
    {
        15, 0, 0, 240
    }
    ,15, 6, 10, 7, 83, C_CONTAMINATED, Z_NOZOMBIE, S_ROAR, I_NORMAL, 3
}
,

{
    341, '&', MAGENTA, "Dispater", M_RES_ELEC | M_RES_POISON | M_RES_HELLFIRE | M_RES_COLD | M_SPELLCASTER | M_SEE_INVIS,
        0, 25, 341, H_DEMONIC, -10,
    {
        15, 0, 0, 0
    }
    ,
    {
        16, 0, 0, 222
    }
    ,15, 3, 6, 7, 84, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_HIGH, 3
}
,

{
    342, '&', LIGHTMAGENTA, "Asmodeus", M_RES_ELEC | M_RES_POISON | M_RES_HELLFIRE | M_SPELLCASTER | M_FLIES | M_SEE_INVIS,
        0, 25, 342, H_DEMONIC, -12,
    {
        20, 0, 0, 0
    }
    ,
    {
        17, 0, 0, 245
    }
    ,12, 7, 9, 7, 85, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_HIGH, 3
}
,

{
    343, 'C', LIGHTCYAN, "Antaeus", M_RES_ELEC | M_ED_FIRE | M_RES_COLD | M_SPELLCASTER,
        0, 25, 343, H_NORMAL, -9,
    {
        30, 0, 0, 0
    }
    ,
    {
        22, 0, 0, 250
    }
    ,10, 4, 7, 7, 87, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_HIGH, 3
}
,

{
    344, '&', WHITE, "Ereshkigal", M_RES_ELEC | M_RES_POISON | M_RES_COLD | M_SPELLCASTER | M_SEE_INVIS,
        0, 25, 344, H_DEMONIC, -10,
    {
        20, 0, 0, 0
    }
    ,
    {
        18, 0, 0, 238
    }
    ,15, 6, 9, 7, 86, C_CONTAMINATED, Z_NOZOMBIE, S_SHOUT, I_HIGH, 3
}
,

{
    356, 'L', DARKGREY, "ancient lich", M_RES_POISON | M_RES_COLD | M_SPELLCASTER | M_SEE_INVIS | M_RES_FIRE | M_RES_ELEC,
        0, 20, 356, H_UNDEAD, -14,
    {
        20, 0, 0, 0
    }
    ,
    {
        27, 2, 4, 0
    }
    ,20, 10, 12, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SHOUT, I_HIGH, 1
}
,
/* number is set in define_monster */

{
    360, '@', CYAN, "vault guard", M_NO_FLAGS,
        0, 12, 114, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        13, 3, 5, 0
    }
    ,1, 13, 10, 7, 250, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

/* These nasties are never randomly generated, only sometimes specially
   placed in the Crypt. */
{
    361, 'z', DARKGREY, "curse skull", M_RES_ELEC | M_RES_POISON | M_RES_HELLFIRE | M_RES_COLD | M_LEVITATE | M_SPELLCASTER | M_SEE_INVIS,
        0, 50, 361, H_UNDEAD, -6,
    {
        0, 0, 0, 0
    }
    ,
    {
        13, 0, 0, 66
    }
    ,40, 3, 10, 7, 108, C_NOCORPSE, Z_NOZOMBIE, S_MOAN, I_HIGH, 0
}
,

{
    362, 'V', CYAN, "vampire knight", M_RES_POISON | M_RES_COLD | M_SPELLCASTER | M_SEE_INVIS,
        0, 13, 47, H_UNDEAD, -6,
    {
        33, 0, 0, 0
    }
    ,
    {
        11, 3, 7, 0
    }
    ,10, 10, 10, 7, 40, C_NOCORPSE, Z_NOZOMBIE, S_SHOUT, I_HIGH, 3
}
,

{
    363, 'V', MAGENTA, "vampire mage", M_RES_POISON | M_RES_COLD | M_SPELLCASTER | M_SEE_INVIS | M_FLIES,
        0, 15, 47, H_UNDEAD, -6,
    {
        22, 0, 0, 0
    }
    ,
    {
        8, 3, 4, 0
    }
    ,10, 10, 10, 7, 61, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_HIGH, 3
}
,

{
    364, 'G', LIGHTMAGENTA, "shining eye", M_NO_SKELETON | M_LEVITATE | M_SPELLCASTER,
        0, 14, 364, H_NORMAL, 5000,
    {
        0, 0, 0, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,3, 1, 7, 7, 109, C_POISON, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    365, 'X', MAGENTA, "Orb Guardian", M_NO_SKELETON,
        0, 20, 365, H_NORMAL, -5,
    {
        40, 0, 0, 0
    }
    ,
    {
        15, 3, 5, 0
    }
    ,13, 13, 14, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    366, 'A', YELLOW, "Daeva", M_RES_POISON | M_LEVITATE | M_SPELLCASTER,
        0, 12, 366, H_HOLY, -8,
    {
        25, 0, 0, 0
    }
    ,
    {
        12, 3, 5, 0
    }
    ,10, 13, 13, 7, 112, C_NOCORPSE, Z_NOZOMBIE, S_SHOUT, I_HIGH, 3
}
,

/* spectral thing - similar to zombies/skeletons */
{
    367, 'W', GREEN, "", M_RES_POISON | M_RES_COLD | M_LEVITATE,
        0, 11, 367, H_UNDEAD, 5000,
    {
        20, 0, 0, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,8, 5, 7, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    368, 'N', RED, "greater naga", M_RES_POISON | M_SPELLCASTER | M_SEE_INVIS,
        750, 10, 161, H_NORMAL, 5000,
    {
        18, 0, 0, 0
    }
    ,
    {
        15, 3, 5, 0
    }
    ,6, 10, 8, 7, 107, C_POISON, Z_SMALL, S_SHOUT, I_HIGH, 3
}
,

{
    369, 'D', LIGHTGREY, "skeletal dragon", M_RES_POISON | M_RES_FIRE | M_RES_COLD | M_RES_ELEC | M_SEE_INVIS,
        0, 12, 369, H_UNDEAD, -4,
    {
        30, 20, 20, 0
    }
    ,
    {
        20, 8, 8, 0
    }
    ,20, 4, 8, 7, 250, C_CLEAN, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    370, 'X', GREEN, "tentacled monstrosity", M_RES_POISON | M_RES_FIRE | M_RES_COLD | M_RES_ELEC | M_SEE_INVIS,
        0, 10, 370, H_NORMAL, -5,
    {
        22, 17, 13, 19
    }
    ,
    {
        25, 3, 5, 0
    }
    ,5, 5, 9, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 1
}
,

{
    371, 'H', LIGHTGREY, "sphinx", M_FLIES | M_SEE_INVIS | M_SPELLCASTER,
        0, 10, 371, H_NORMAL, -3,
    {
        25, 12, 12, 0
    }
    ,
    {
        16, 3, 5, 0
    }
    ,5, 5, 13, 7, 114, C_CLEAN, Z_NOZOMBIE, S_SHOUT, I_HIGH, 1
}
,

{
    372, 'n', BROWN, "rotting hulk", M_RES_POISON | M_RES_COLD,
        0, 12, 372, H_UNDEAD, -5,
    {
        25, 0, 0, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,5, 7, 8, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 1
}
,

{
    373, 'M', YELLOW, "guardian mummy", M_RES_POISON | M_RES_COLD,
        0, 13, 373, H_UNDEAD, -5,
    {
        30, 0, 0, 0
    }
    ,
    {
        7, 5, 3, 0
    }
    ,6, 9, 9, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 3
}
,

{
    374, 'M', DARKGREY, "greater mummy", M_RES_POISON | M_RES_COLD | M_RES_ELEC | M_SPELLCASTER,
        0, 20, 374, H_UNDEAD, 5000,
    {
        35, 0, 0, 0
    }
    ,
    {
        15, 5, 3, 100
    }
    ,10, 6, 10, 7, 115, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 3
}
,

{
    375, 'M', RED, "mummy priest", M_RES_POISON | M_RES_COLD | M_RES_ELEC | M_SPELLCASTER,
        0, 16, 375, H_UNDEAD, 5000,
    {
        30, 0, 0, 0
    }
    ,
    {
        10, 5, 3, 0
    }
    ,8, 7, 9, 7, 115, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 3
}
,

{
    376, 'c', YELLOW, "centaur warrior", M_NO_FLAGS,
        1500, 12, 2, H_NORMAL, -3,
    {
        16, 0, 0, 0
    }
    ,
    {
        9, 3, 5, 0
    }
    ,4, 8, 15, 7, 250, C_CLEAN, Z_BIG, S_SHOUT, I_NORMAL, 3
}
,

{
    377, 'c', RED, "yaktaur captain", M_NO_FLAGS,
        2000, 10, 133, H_NORMAL, -3,
    {
        23, 0, 0, 0
    }
    ,
    {
        14, 3, 5, 0
    }
    ,5, 5, 10, 7, 250, C_CONTAMINATED, Z_BIG, S_SHOUT, I_NORMAL, 3
}
,

{
    378, '@', BLACK, "Killer Klown", M_SEE_INVIS | M_SPEAKS,
        0, 15, 378, H_NORMAL, 5000,
    {
        30, 0, 0, 0
    }
    ,
    {
        20, 5, 5, 0
    }
    ,10, 15, 15, 7, 1, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_HIGH, 1
}
,

/* these next few monsters are taken from the Chronicles of an Age of
   Darkness, by Hugh Cook */
{
    379, 'R', LIGHTBLUE, "guardian robot", M_SEE_INVIS | M_SPELLCASTER | M_RES_ELEC | M_RES_POISON,
        0, 12, 379, H_NORMAL, 5000,
    {
        0, 0, 0, 0
    }
    ,
    {
        5, 0, 0, 50
    }
    ,20, 5, 5, 7, 73, C_CONTAMINATED, Z_SMALL, S_SILENT, I_PLANT, 0
}
,

{
    380, 'R', DARKGREY, "dorgi", M_SPELLCASTER | M_RES_ELEC | M_RES_FIRE | M_RES_COLD | M_RES_POISON | M_SPEAKS,
        0, 20, 380, H_NORMAL, 5000,
    {
        50, 0, 0, 0
    }
    ,
    {
        20, 0, 0, 150
    }
    ,30, 3, 10, 7, 116, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 1
}
,

{
    381, '*', YELLOW, "Sword", M_SPELLCASTER | M_RES_ELEC | M_RES_FIRE | M_RES_COLD | M_RES_POISON,
        0, 13, 381, H_NORMAL, 5000,
    {
        0, 0, 0, 0
    }
    ,
    {
        30, 0, 0, 150
    }
    ,20, 20, 20, 7, 117, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 1
}
,

{
    382, 'r', LIGHTGREY, "quokka", M_SPEAKS,
        0, 10, 382, H_NORMAL, -3,
    {
        6, 0, 0, 0
    }
    ,
    {
        1, 3, 5, 0
    }
    ,2, 13, 10, 7, 250, C_CLEAN, Z_SMALL, S_SHOUT, I_NORMAL, 1
}
,


{
    385, 'G', YELLOW, "eye of devastation", M_NO_SKELETON | M_LEVITATE | M_SPELLCASTER,
        0, 11, 385, H_NORMAL, 5000,
    {
        0, 0, 0, 0
    }
    ,
    {
        10, 3, 5, 0
    }
    ,12, 1, 7, 7, 125, C_POISON, Z_NOZOMBIE, S_SILENT, I_PLANT, 0
}
,

{
    386, 'y', BROWN, "moth of wrath", M_ED_POISON | M_FLIES,
        0, 10, 386, H_NORMAL, -3,
    {
        25, 0, 0, 0
    }
    ,
    {
        9, 3, 5, 0
    }
    ,0, 10, 12, 7, 250, C_CLEAN, Z_SMALL, S_SHOUT, I_NORMAL, 0
}
,

{
    387, '%', YELLOW, "death cob", M_RES_POISON | M_RES_COLD | M_SPEAKS,
        0, 10, 387, H_UNDEAD, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        10, 4, 5, 0
    }
    ,10, 15, 25, 7, 250, C_CLEAN, Z_SMALL, S_MOAN, I_NORMAL, 1
}
,

{
    388, 'z', DARKGREY, "curse toe", M_RES_ELEC | M_RES_POISON | M_RES_HELLFIRE | M_RES_COLD | M_LEVITATE | M_SPELLCASTER | M_SEE_INVIS,
        0, 60, 388, H_UNDEAD, 5000,
    {
        0, 0, 0, 0
    }
    ,
    {
        14, 0, 0, 77
    }
    ,50, 1, 12, 7, 108, C_NOCORPSE, Z_NOZOMBIE, S_MOAN, I_HIGH, 0
}
,

{
    389, '$', YELLOW, "mimic", M_NO_SKELETON | M_RES_POISON,
        0, 13, 389, H_NORMAL, -3,
    {
        12, 12, 12, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,5, 1, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 0
}
,

{
    390, ')', BLACK, "mimic", M_NO_SKELETON | M_RES_POISON,
        0, 13, 389, H_NORMAL, -3,
    {
        17, 17, 17, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,5, 1, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 0
}
,

{
    391, '[', BLACK, "mimic", M_NO_SKELETON | M_RES_POISON,
        0, 13, 389, H_NORMAL, -3,
    {
        12, 12, 12, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,15, 1, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 0
}
,

{
    392, '?', LIGHTGREY, "mimic", M_NO_SKELETON | M_RES_POISON,
        0, 13, 389, H_NORMAL, -3,
    {
        12, 12, 12, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,5, 1, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 0
}
,

{
    393, '!', BLACK, "mimic", M_NO_SKELETON | M_RES_POISON,
        0, 13, 389, H_NORMAL, -3,
    {
        12, 12, 12, 0
    }
    ,
    {
        8, 3, 5, 0
    }
    ,5, 1, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_NORMAL, 0
}
,

{
    394, 'h', RED, "hell-hog", M_SPELLCASTER,
        0, 10, 394, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        11, 3, 5, 0
    }
    ,2, 9, 14, 7, 120, C_CLEAN, Z_NOZOMBIE, S_SILENT, I_ANIMAL, 0
}
,

{
    395, 'D', RED, "Serpent of Hell", M_SPELLCASTER | M_RES_POISON | M_RES_HELLFIRE | M_FLIES,
        0, 18, 395, H_DEMONIC, -13,
    {
        35, 15, 15, 0
    }
    ,
    {
        20, 4, 4, 0
    }
    ,12, 9, 14, 7, 123, C_CLEAN, Z_NOZOMBIE, S_ROAR, I_HIGH, 1
}
,

{
    396, 'g', DARKGREY, "boggart", M_SPELLCASTER,
        0, 14, 396, H_NORMAL, -7,
    {
        5, 0, 0, 0
    }
    ,
    {
        2, 3, 5, 0
    }
    ,0, 12, 12, 7, 124, C_CONTAMINATED, Z_SMALL, S_SHOUT, I_NORMAL, 3
}
,

{
    397, 'D', LIGHTCYAN, "quicksilver dragon", M_SPELLCASTER | M_FLIES,
        0, 14, 397, H_NORMAL, -7,
    {
        45, 0, 0, 0
    }
    ,
    {
        16, 3, 5, 0
    }
    ,10, 15, 15, 7, 126, C_CONTAMINATED, Z_SMALL, S_ROAR, I_HIGH, 3
}
,

{
    398, 'D', CYAN, "iron dragon", M_SPELLCASTER | M_RES_POISON | M_RES_FIRE | M_RES_COLD,
        0, 14, 398, H_NORMAL, -7,
    {
        25, 25, 25, 0
    }
    ,
    {
        18, 5, 3, 0
    }
    ,20, 6, 8, 7, 127, C_CONTAMINATED, Z_SMALL, S_ROAR, I_HIGH, 3
}
,

{
    399, 'z', CYAN, "skeletal warrior", M_SPELLCASTER | M_RES_POISON | M_RES_COLD,
        0, 10, 399, H_UNDEAD, -7,
    {
        25, 0, 0, 0
    }
    ,
    {
        10, 5, 3, 0
    }
    ,15, 10, 10, 7, 128, C_CONTAMINATED, Z_SMALL, S_SILENT, I_NORMAL, 3
}
,


/* player ghost - only one per level. stats are stored in ghost struct */
{
    400, 'p', DARKGREY, "", M_RES_POISON | M_SPEAKS | M_SPELLCASTER,
        0, 20, 400, H_UNDEAD, -3,
    {
        5, 0, 0, 0
    }
    ,
    {
        4, 2, 3, 0
    }
    ,1, 2, 10, 7, 119, C_CONTAMINATED, Z_NOZOMBIE, S_SILENT, I_HIGH, 1
}
,

/* random demon in pan - only one per level. stats are stored in ghost struct */
{
    401, '&', BLACK, "&", M_SPELLCASTER,
        0, 14, 401, H_DEMONIC, -5,
    {
        5, 0, 0, 0
    }
    ,
    {
        4, 2, 3, 0
    }
    ,1, 2, 10, 7, 119, C_CONTAMINATED, Z_NOZOMBIE, S_RANDOM, I_HIGH, 1
}
,



/*
   {monster_class, showchar, colour, name, flags,
   weight, exp_mod, charclass, holiness, resist_magic, damage[4],
   hp_dice[4], AC, ev, speed, speed_inc, sec (spell), corpse_thingy, zombie_size, shouts, intel, gmon_use},
 */


{
    MLAVA0, 'w', RED, "lava worm", M_RES_FIRE | M_ED_COLD,
        0, 10, MLAVA0, H_NORMAL, -3,
    {
        15, 0, 0, 0
    }
    ,
    {
        6, 3, 5, 0
    }
    ,1, 10, 10, 7, 250, C_NOCORPSE, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    MLAVA1, ';', RED, "lava fish", M_RES_FIRE | M_ED_COLD,
        0, 10, MLAVA1, H_NORMAL, -3,
    {
        10, 0, 0, 0
    }
    ,
    {
        4, 3, 5, 0
    }
    ,4, 15, 10, 7, 250, C_NOCORPSE, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    MLAVA2, 'S', RED, "lava snake", M_RES_FIRE | M_ED_COLD,
        0, 10, MLAVA2, H_NORMAL, -3,
    {
        7, 0, 0, 0
    }
    ,
    {
        3, 3, 5, 0
    }
    ,2, 17, 10, 7, 250, C_NOCORPSE, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    MLAVA3, 0, 0, "another lava thing", M_NO_FLAGS,
        0, 10, MLAVA3, H_NORMAL, -3,
    {
        0, 0, 0, 0
    }
    ,
    {
        0, 0, 0, 0
    }
    ,0, 0, 0, 0, 250, C_NOCORPSE, Z_SMALL, S_SILENT, I_PLANT, 0
}
,

{
    MWATER0, ';', LIGHTGREEN, "big fish", M_NO_FLAGS,
        0, 10, MWATER0, H_NORMAL, -3,
    {
        8, 0, 0, 0
    }
    ,
    {
        4, 3, 5, 0
    }
    ,1, 12, 10, 7, 250, C_NOCORPSE, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    MWATER1, ';', LIGHTRED, "giant goldfish", M_NO_FLAGS,
        0, 10, MWATER1, H_NORMAL, -3,
    {
        15, 0, 0, 0
    }
    ,
    {
        7, 3, 5, 0
    }
    ,5, 7, 10, 7, 250, C_NOCORPSE, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    MWATER2, ';', LIGHTBLUE, "electrical eel", M_RES_ELEC,
        0, 10, MWATER2, H_NORMAL, -3,
    {
        0, 0, 0, 0
    }
    ,
    {
        3, 3, 5, 0
    }
    ,1, 15, 10, 7, 250, C_NOCORPSE, Z_SMALL, S_SILENT, I_ANIMAL, 0
}
,

{
    MWATER3, 'J', CYAN, "jellyfish", M_RES_POISON,
        0, 10, MWATER3, H_NORMAL, -3,
    {
        1, 1, 0, 0
    }
    ,
    {
        4, 3, 5, 0
    }
    ,0, 5, 10, 7, 250, C_NOCORPSE, Z_SMALL, S_SILENT, I_PLANT, 0
}
,

{
    MWATER4, '{', LIGHTBLUE, "water elemental", M_RES_POISON | M_ED_FIRE,
        0, 10, MWATER4, H_NORMAL, -3,
    {
        25, 0, 0, 0
    }
    ,
    {
        8, 5, 3, 0
    }
    ,0, 7, 10, 7, 250, C_NOCORPSE, Z_NOZOMBIE, S_SILENT, I_PLANT, 1
}
,

{
    MWATER5, 'w', BROWN, "swamp worm", M_NO_FLAGS,
        0, 10, MWATER5, H_NORMAL, -3,
    {
        20, 0, 0, 0
    }
    ,
    {
        5, 5, 5, 0
    }
    ,3, 12, 12, 0, 250, C_NOCORPSE, Z_SMALL, S_SILENT, I_PLANT, 0
}

/*
   {MWATER6, 0, 0, "", M_NO_FLAGS,
   0, 10, MWATER6, H_NORMAL, -3, {0,0,0,0},
   {0,0,0,0}, 0, 0, 0, 0, 250, C_NOCORPSE, Z_SMALL, S_SILENT, I_PLANT, 0}
 */


/*

   #define M_RES_ELEC (1<<1)
   #define M_RES_POISON (1<<2)
   #define M_RES_FIRE (1<<3)
   #define M_RES_HELLFIRE (1<<4)
   #define M_RES_COLD (1<<5)
   // vulnerabilities
   #define M_ED_ELEC (1<<6) // can this? - - Not as yet (LRH)
   #define M_ED_POISON (1<<7) // ???  - - This flag is now (2.50) set for insects (LRH)
   #define M_ED_FIRE (1<<8)
   #define M_ED_COLD (1<<9)
   // casts spells?
   #define M_SPELLCASTER (1<<10)
   // hovering?
   #define M_FLIES (1<<11) // will crash to ground if paralysed (wings)
   #define M_LEVITATE (1<<12) // not if this is set
   // infrared/radar/whatever?
   #define M_SEE_INVIS (1<<13)
   // killing this beast only gives 10 experience (makes sense for plants/fungi)
   // must do this manually
   #define M_NO_EXP_GAIN (1<<14)

   // holiness
   #define H_HOLY -1
   #define H_NORMAL 0
   #define H_UNDEAD 1
   #define H_DEMONIC 2
   // corpse_thingy
   #define C_NOCORPSE 0
   #define C_CLEAN 1
   #define C_CONTAMINATED 2
   #define C_POISON 3
   #define C_HCL 4 // your flesh starts to rot away...
   // well, HCl will burn your flesh away. Anyway, couldn't find a better name for now

   //  Maybe C_ROT... Oh well, C_HCL is fine (LRH)

   // zombie size
   #define Z_NOZOMBIE 0 // no zombie (and no skeleton)
   #define Z_SMALL 1
   #define Z_BIG 2
   // Note - this should be set to 0 for classed monsters, eg orc wizard

   // shout
   #define S_RANDOM -1
   #define S_SILENT 0
   #define S_SHOUT 1 //1=shout
   #define S_BARK 2 //2=bark
   #define S_SHOUT2 3 //3=shout twice
   #define S_ROAR 4 //4=roar
   #define S_SCREAM 5 //5=scream
   #define S_BELLOW 6 //6=bellow (?)
   #define S_SCREECH 7 //7=screech
   #define S_BUZZ 8 //8=buzz
   #define S_MOAN 9 //9=moan
 */
#endif

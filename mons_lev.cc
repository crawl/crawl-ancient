/*
 *  File:       mons_lev.cc
 *  Summary:    Functions used to help determine which monsters should appear.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "mons_lev.h"

#include "externs.h"

#include "mstruct.h"

int mons_rarity(int mcls);
int mons_level(int mcls);

int mons_level_gehenna(int mcls);
int mons_level_cocytus(int mcls);
int mons_level_dis(int mcls);
int mons_level_tartarus(int mcls);
int mons_level_abyss(int mcls);

int mons_rare_gehenna(int mcls);
int mons_rare_cocytus(int mcls);
int mons_rare_dis(int mcls);
int mons_rare_tartarus(int mcls);
int mons_rare_abyss(int mcls);

int mons_level_orc_mines(int mcls);
int mons_rare_orc_mines(int mcls);
int mons_level_hive(int mcls);
int mons_rare_hive(int mcls);
int mons_level_lair(int mcls);
int mons_rare_lair(int mcls);
int mons_level_slime_pits(int mcls);
int mons_rare_slime_pits(int mcls);
int mons_level_crypt(int mcls);
int mons_rare_crypt(int mcls);
int mons_level_hall_ob(int mcls);
int mons_rare_hall_ob(int mcls);
int mons_level_snake(int mcls);
int mons_rare_snake(int mcls);
int mons_level_elf(int mcls);
int mons_rare_elf(int mcls);
int mons_level_tomb(int mcls);
int mons_rare_tomb(int mcls);
int mons_level_swamp(int mcls);
int mons_rare_swamp(int mcls);




int mons_level_dis(int mcls)
{
    int mlev = 0;

    switch (mcls)
    {
    case MONS_CLAY_GOLEM:
    case MONS_IMP:
    case MONS_NECROPHAGE:
    case MONS_RED_DEVIL:
    case MONS_SKELETAL_WARRIOR:
    case MONS_ZOMBIE_LARGE:
        mlev = 1;
        break;

    case MONS_PHANTOM:
    case MONS_ZOMBIE_SMALL:
    case MONS_WIGHT:
    case MONS_SHADOW:
    case MONS_HELL_HOUND:
    case MONS_ROTTING_DEVIL:
    case MONS_TORMENTOR:
    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
    case MONS_HELL_KNIGHT:
    case MONS_STONE_GOLEM:
        mlev = 2;
        break;

    case MONS_MUMMY:
    case MONS_EFREET:
    case MONS_FLYING_SKULL:
    case MONS_HELLION:
    case MONS_IRON_GOLEM:
    case MONS_HELL_HOG:
        mlev = 3;
        break;

    case MONS_VAMPIRE:
    case MONS_WRAITH:
    case MONS_HAIRY_DEVIL:
    case MONS_IRON_DEVIL:
    case MONS_FREEZING_WRAITH:
    case MONS_FLAYED_GHOST:
        mlev = 4;
        break;

    case MONS_LICH:
    case MONS_REAPER:
    case MONS_SOUL_EATER:
    case MONS_ICE_DRAGON:
    case MONS_ICE_DEVIL:
    case MONS_BLUE_DEVIL:
    case MONS_SPECTRAL_WARRIOR:
    case MONS_DANCING_WEAPON:
        mlev = 5;
        break;

    case MONS_FIEND:
    case MONS_ANCIENT_LICH:
    case MONS_SKELETAL_DRAGON:
    case MONS_IRON_DRAGON:
        mlev = 6;
        break;

    default:
        return 0;
    }                           // end switch

    return mlev + 26;

}                               // end mons_level_dis()




int mons_rare_dis(int mcls)
{
    switch (mcls)
    {
    case MONS_IMP:
    case MONS_ZOMBIE_SMALL:
    case MONS_ZOMBIE_LARGE:
    case MONS_IRON_DEVIL:
        return 99;

    case MONS_REAPER:
        return 77;

    case MONS_TORMENTOR:
        return 66;

    case MONS_RED_DEVIL:
    case MONS_SKELETAL_WARRIOR:
        return 50;

    case MONS_WRAITH:
        return 48;

    case MONS_SHADOW:
        return 56;

    case MONS_HELL_HOUND:
        return 46;

    case MONS_MUMMY:
    case MONS_WIGHT:
        return 45;

    case MONS_HELLION:
    case MONS_BLUE_DEVIL:
        return 40;

    case MONS_FLYING_SKULL:
        return 35;

    case MONS_FREEZING_WRAITH:
    case MONS_ICE_DEVIL:
        return 30;

    case MONS_NECROPHAGE:
    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
    case MONS_FLAYED_GHOST:
        return 25;

    case MONS_SKELETAL_DRAGON:
    case MONS_HELL_HOG:
        return 20;

    case MONS_VAMPIRE:
        return 19;

    case MONS_PHANTOM:
        return 17;

    case MONS_HAIRY_DEVIL:
        return 15;

    case MONS_LICH:
    case MONS_EFREET:
    case MONS_ROTTING_DEVIL:
    case MONS_SOUL_EATER:
    case MONS_HELL_KNIGHT:
    case MONS_CLAY_GOLEM:
    case MONS_STONE_GOLEM:
    case MONS_IRON_GOLEM:
    case MONS_SPECTRAL_WARRIOR:
    case MONS_DANCING_WEAPON:
        return 10;

    case MONS_IRON_DRAGON:
        return 5;

    case MONS_FIEND:
    case MONS_ANCIENT_LICH:
        return 3;

    default:
        return 0;
    }                           // end switch

}                               // end mons_rare_dis()




int mons_level_gehenna(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case MONS_RED_DEVIL:
    case MONS_ZOMBIE_SMALL:
    case MONS_ZOMBIE_LARGE:
    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
    case MONS_CLAY_GOLEM:
        mlev = 1;
        break;

    case MONS_IMP:
    case MONS_NECROPHAGE:
    case MONS_HELL_HOUND:
    case MONS_STONE_GOLEM:
    case MONS_HELL_HOG:
        mlev = 2;
        break;

    case MONS_PHANTOM:
    case MONS_MUMMY:
    case MONS_WIGHT:
    case MONS_SHADOW:
    case MONS_FLYING_SKULL:
    case MONS_ROTTING_DEVIL:
    case MONS_IRON_GOLEM:
        mlev = 3;
        break;

    case MONS_VAMPIRE:
    case MONS_WRAITH:
    case MONS_HAIRY_DEVIL:
    case MONS_HELL_KNIGHT:
        mlev = 4;
        break;

    case MONS_EFREET:
    case MONS_HELLION:
    case MONS_TORMENTOR:
    case MONS_FLAYED_GHOST:
        mlev = 5;
        break;

    case MONS_FIEND:
    case MONS_LICH:
    case MONS_REAPER:
    case MONS_SOUL_EATER:
    case MONS_SPECTRAL_WARRIOR:
    case MONS_PIT_FIEND:
    case MONS_ANCIENT_LICH:
    case MONS_SKELETAL_DRAGON:
    case MONS_SERPENT_OF_HELL:
        mlev = 6;
        break;

    default:
        return 0;
    }                           // end switch

    return mlev + 26;

}                               // end mons_level_gehenna()




int mons_rare_gehenna(int mcls)
{
    switch (mcls)
    {
    case MONS_ZOMBIE_SMALL:
    case MONS_ZOMBIE_LARGE:
    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
        return 99;

    case MONS_MUMMY:
        return 70;

    case MONS_SHADOW:
        return 61;

    case MONS_WIGHT:
    case MONS_RED_DEVIL:
        return 60;

    case MONS_HELLION:
        return 54;

    case MONS_WRAITH:
        return 53;

    case MONS_NECROPHAGE:
    case MONS_ROTTING_DEVIL:
        return 50;

    case MONS_VAMPIRE:
        return 44;

    case MONS_FLYING_SKULL:
    case MONS_REAPER:
        return 43;

    case MONS_TORMENTOR:
        return 42;

    case MONS_HELL_HOUND:
        return 41;

    case MONS_PHANTOM:
    case MONS_FLAYED_GHOST:
        return 32;

    case MONS_IMP:
    case MONS_IRON_DEVIL:
    case MONS_HELL_HOG:
        return 30;

    case MONS_LICH:
        return 25;

    case MONS_HELL_KNIGHT:
        return 21;

    case MONS_HAIRY_DEVIL:
    case MONS_SPECTRAL_WARRIOR:
        return 20;

    case MONS_CLAY_GOLEM:
    case MONS_SKELETAL_DRAGON:
        return 10;

    case MONS_STONE_GOLEM:
        return 8;

    case MONS_PIT_FIEND:
        return 7;

    case MONS_FIEND:
    case MONS_EFREET:
    case MONS_SOUL_EATER:
    case MONS_IRON_GOLEM:
        return 5;

    case MONS_ANCIENT_LICH:
    case MONS_SERPENT_OF_HELL:
        return 4;

    default:
        return 0;
    }                           // end switch

}                               // end mons_rare_gehenna()




int mons_level_cocytus(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case MONS_NECROPHAGE:
    case MONS_ZOMBIE_SMALL:
    case MONS_ZOMBIE_LARGE:
    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
    case MONS_SKELETAL_WARRIOR:
        mlev = 1;
        break;

    case MONS_PHANTOM:
    case MONS_ICE_BEAST:
    case MONS_SHADOW:
    case MONS_BLUE_DEVIL:
        mlev = 2;
        break;

    case MONS_VAMPIRE:
    case MONS_WIGHT:
    case MONS_FLYING_SKULL:
    case MONS_ROTTING_DEVIL:
        mlev = 3;
        break;

    case MONS_MUMMY:
    case MONS_WRAITH:
    case MONS_HUNGRY_GHOST:
    case MONS_FREEZING_WRAITH:
    case MONS_HAIRY_DEVIL:
    case MONS_SPECTRAL_WARRIOR:
        mlev = 4;
        break;

    case MONS_ICE_DRAGON:
    case MONS_TORMENTOR:
    case MONS_ICE_DEVIL:
        mlev = 5;
        break;

    case MONS_LICH:
    case MONS_REAPER:
    case MONS_SOUL_EATER:
    case MONS_ANCIENT_LICH:
    case MONS_SKELETAL_DRAGON:
        mlev = 6;
        break;

    default:
        return 0;
    }

    return mlev + 26;

}                               // end mons_level_cocytus()




int mons_rare_cocytus(int mcls)
{
    switch (mcls)
    {
    case MONS_FREEZING_WRAITH:
        return 87;

    case MONS_ZOMBIE_SMALL:
    case MONS_ICE_BEAST:
    case MONS_ZOMBIE_LARGE:
    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
        return 85;

    case MONS_ICE_DEVIL:
    case MONS_BLUE_DEVIL:
        return 76;

    case MONS_FLYING_SKULL:
        return 57;

    case MONS_SHADOW:
        return 56;

    case MONS_SKELETAL_WARRIOR:
        return 50;

    case MONS_REAPER:
        return 47;

    case MONS_WRAITH:
    case MONS_WIGHT:
        return 45;

    case MONS_ICE_DRAGON:
        return 38;

    case MONS_ROTTING_DEVIL:
    case MONS_TORMENTOR:
        return 37;

    case MONS_MUMMY:
        return 35;

    case MONS_VAMPIRE:
        return 34;

    case MONS_HUNGRY_GHOST:
    case MONS_HAIRY_DEVIL:
        return 26;

    case MONS_NECROPHAGE:
    case MONS_PHANTOM:
        return 25;

    case MONS_SPECTRAL_WARRIOR:
        return 20;

    case MONS_SOUL_EATER:
        return 19;

    case MONS_LICH:
    case MONS_SKELETAL_DRAGON:
        return 12;

    case MONS_ANCIENT_LICH:
        return 5;

    default:
        return 0;
    }
}                               // end mons_rare_cocytus()




int mons_level_tartarus(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {

    case MONS_RED_DEVIL:
    case MONS_IMP:
    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
    case MONS_SHADOW_IMP:
    case MONS_SKELETAL_WARRIOR:
        mlev = 1;
        break;

    case MONS_NECROPHAGE:
    case MONS_PHANTOM:
    case MONS_ZOMBIE_SMALL:
    case MONS_ZOMBIE_LARGE:
    case MONS_WIGHT:
    case MONS_HELL_KNIGHT:
        mlev = 2;
        break;

    case MONS_WRAITH:
    case MONS_SHADOW:
    case MONS_HELL_HOUND:
    case MONS_FREEZING_WRAITH:
    case MONS_NECROMANCER:
    case MONS_SHADOW_DEMON:
        mlev = 3;
        break;

    case MONS_HUNGRY_GHOST:
    case MONS_MUMMY:
    case MONS_TORMENTOR:
    case MONS_ICE_DEVIL:
    case MONS_BLUE_DEVIL:
    case MONS_SPECTRAL_WARRIOR:
    case MONS_FLAYED_GHOST:
    case MONS_SKELETAL_DRAGON:
        mlev = 4;
        break;

    case MONS_VAMPIRE:
    case MONS_FLYING_SKULL:
    case MONS_HELLION:
    case MONS_ROTTING_DEVIL:
    case MONS_REAPER:
    case MONS_SHADOW_DRAGON:
        mlev = 5;
        break;

    case MONS_LICH:
    case MONS_SOUL_EATER:
    case MONS_HAIRY_DEVIL:
    case MONS_ANCIENT_LICH:
        mlev = 6;
        break;

    default:
        return 0;
    }

    return mlev + 26;

}                               // end mons_level_tartarus()




int mons_rare_tartarus(int mcls)
{
    switch (mcls)
    {

    case MONS_ZOMBIE_SMALL:
    case MONS_ZOMBIE_LARGE:
    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
    case MONS_SHADOW_IMP:
    case MONS_SKELETAL_WARRIOR:
        return 99;

    case MONS_SHADOW:
        return 92;

    case MONS_REAPER:
        return 73;

    case MONS_NECROPHAGE:
        return 72;

    case MONS_WIGHT:
        return 71;

    case MONS_ROTTING_DEVIL:
        return 62;

    case MONS_FREEZING_WRAITH:
        return 60;

    case MONS_FLYING_SKULL:
        return 53;

    case MONS_HELL_HOUND:
    case MONS_PHANTOM:
    case MONS_WRAITH:
        return 52;

    case MONS_SHADOW_DEMON:
        return 50;

    case MONS_SPECTRAL_WARRIOR:
        return 45;

    case MONS_VAMPIRE:
        return 44;

    case MONS_HELLION:
    case MONS_TORMENTOR:
        return 42;

    case MONS_SKELETAL_DRAGON:
        return 40;

    case MONS_SOUL_EATER:
        return 35;

    case MONS_ICE_DEVIL:        // not really appropriate for a fiery hell

        return 34;

    case MONS_MUMMY:
        return 33;

    case MONS_HUNGRY_GHOST:
    case MONS_BLUE_DEVIL:
        return 32;

    case MONS_HAIRY_DEVIL:
    case MONS_FLAYED_GHOST:
        return 30;

    case MONS_LICH:
        return 24;

    case MONS_IMP:
        return 20;

    case MONS_RED_DEVIL:
        return 13;

    case MONS_HELL_KNIGHT:
        return 14;

    case MONS_NECROMANCER:
    case MONS_SHADOW_DRAGON:
        return 12;

    case MONS_ANCIENT_LICH:
        return 6;

    default:
        return 0;
    }
}                               // end mons_rare_tartarus()




int mons_level_abyss(int mcls)
{
//int found = 0;

    switch (mcls)
    {
    case MONS_RED_DEVIL:        //60;

    case MONS_FUNGUS:           //8;

    case MONS_IMP:              //10;

    case MONS_NECROPHAGE:       //10;

    case MONS_PHANTOM:          //8;

    case MONS_UGLY_THING:       //21;

    case MONS_ABOMINATION_SMALL:        //91;

    case MONS_ZOMBIE_SMALL:     //10;

    case MONS_GIANT_SPORE:      //12;

    case MONS_ICE_BEAST:        //9;

    case MONS_JELLY:            //9;

    case MONS_LICH:             //34;

    case MONS_MUMMY:            //10;

    case MONS_GUARDIAN_NAGA:    //13;

    case MONS_RAKSHASA: //17;

    case MONS_UNSEEN_HORROR:    //12;

    case MONS_VAMPIRE:          //19;

    case MONS_WRAITH:           //15;

    case MONS_ABOMINATION_LARGE:        //1500;

    case MONS_ZOMBIE_LARGE:     //20;

    case MONS_GIANT_EYEBALL:    //8;

    case MONS_WIGHT:            //10;

    case MONS_SHADOW:           //11;

    case MONS_HUNGRY_GHOST:     //11;

    case MONS_EYE_OF_DRAINING:  //14;

    case MONS_EFREET:           //18;

    case MONS_BRAIN_WORM:       //16;

    case MONS_GIANT_ORANGE_BRAIN:       //26;

    case MONS_FLYING_SKULL:     //65;

    case MONS_HELL_HOUND:       //61;

    case MONS_FREEZING_WRAITH:  //25;

    case MONS_GREAT_ORB_OF_EYES:        //25;

    case MONS_HELLION:          //60;

    case MONS_ROTTING_DEVIL:    //60;

    case MONS_TORMENTOR:        //62;

    case MONS_REAPER:           //63;

    case MONS_SOUL_EATER:       //65;

    case MONS_HAIRY_DEVIL:      //60;

    case MONS_ICE_DEVIL:        //6500; // not really appropriate for a fiery hell

    case MONS_BLUE_DEVIL:       //6200;

    case MONS_IRON_DEVIL:
    case MONS_VERY_UGLY_THING:  //25;

    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:   //20;

    case MONS_HELL_KNIGHT:      //31;

    case MONS_WIZARD:           //17;

    case MONS_NECROMANCER:      //20;

    case MONS_CLAY_GOLEM:       //20;

    case MONS_WOOD_GOLEM:       //15;

    case MONS_STONE_GOLEM:      //22;

    case MONS_IRON_GOLEM:       //25;

    case MONS_CRYSTAL_GOLEM:    //30;

    case MONS_TOENAIL_GOLEM:    //35;

    case MONS_EARTH_ELEMENTAL:  //22;

    case MONS_FIRE_ELEMENTAL:   //22;

    case MONS_AIR_ELEMENTAL:    //22;

    case MONS_SPECTRAL_WARRIOR: //30;

    case MONS_FLAYED_GHOST:     //29;

    case MONS_INSUBSTANTIAL_WISP:       //32;

    case MONS_VAPOUR:           //29;

    case MONS_SPINY_WORM:
    case MONS_DANCING_WEAPON:
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
    case MONS_DEMONIC_CRAWLER:
    case MONS_SUN_DEMON:
    case MONS_SHADOW_IMP:
    case MONS_SHADOW_DEMON:
    case MONS_LOROCYPROCA:
    case MONS_PIT_FIEND:
    case MONS_NAGA_MAGE:
    case MONS_NAGA_WARRIOR:
    case MONS_ANCIENT_LICH:
    case MONS_SHINING_EYE:
    case MONS_SKELETAL_DRAGON:
    case MONS_TENTACLED_MONSTROSITY:
    case MONS_EYE_OF_DEVASTATION:
    case MONS_SKELETAL_WARRIOR:
        return 1;
        //found = 1; break;
    default:
        return 0;

    }

    return 0;

}                               // end mons_level_abyss()




int mons_rare_abyss(int mcls)
{


    switch (mcls)
    {
    case MONS_ABOMINATION_SMALL:
    case MONS_ABOMINATION_LARGE:
        return 99;

    case MONS_WHITE_IMP:
    case MONS_LEMURE:
    case MONS_UFETUBUS:
    case MONS_MANES:
    case MONS_MIDGE:
        return 80;

    case MONS_NEQOXEC:
    case MONS_ORANGE_DEMON:
    case MONS_HELLWING:
    case MONS_SMOKE_DEMON:
    case MONS_YNOXINUL:
        return 50;

    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
    case MONS_SKELETAL_WARRIOR:
        return 40;

    case MONS_ZOMBIE_SMALL:
    case MONS_ZOMBIE_LARGE:
        return 35;

    case MONS_SKELETAL_DRAGON:
        return 20;

    case MONS_EFREET:
        return 18;

    case MONS_RAKSHASA:
        return 17;

    case MONS_BRAIN_WORM:
        return 16;

    case MONS_GIANT_ORANGE_BRAIN:
    case MONS_FLYING_SKULL:
    case MONS_WRAITH:
    case MONS_FREEZING_WRAITH:
    case MONS_VERY_UGLY_THING:
        return 15;

    case MONS_LICH:
    case MONS_EYE_OF_DRAINING:
        return 14;

    case MONS_UNSEEN_HORROR:
    case MONS_INSUBSTANTIAL_WISP:
        return 12;

    case MONS_SHADOW:
    case MONS_HUNGRY_GHOST:
    case MONS_HELL_HOUND:
        return 11;

    case MONS_MUMMY:
    case MONS_GUARDIAN_NAGA:
    case MONS_IMP:
    case MONS_NECROPHAGE:
    case MONS_WIGHT:
    case MONS_HELLION:
    case MONS_ROTTING_DEVIL:
    case MONS_HAIRY_DEVIL:
    case MONS_ICE_DEVIL:        // not really appropriate for a fiery hell

    case MONS_BLUE_DEVIL:
    case MONS_EXECUTIONER:
    case MONS_GREEN_DEATH:
    case MONS_BLUE_DEATH:
    case MONS_BALRUG:
    case MONS_CACODEMON:
    case MONS_DEMONIC_CRAWLER:
    case MONS_SUN_DEMON:
    case MONS_SHADOW_IMP:
    case MONS_SHADOW_DEMON:
    case MONS_LOROCYPROCA:
        return 10;

    case MONS_ICE_BEAST:
    case MONS_JELLY:
    case MONS_VAMPIRE:
    case MONS_TORMENTOR:
    case MONS_VAPOUR:
        return 9;

    case MONS_PHANTOM:
    case MONS_FUNGUS:
    case MONS_GIANT_EYEBALL:
    case MONS_REAPER:
        return 8;

    case MONS_SOUL_EATER:
        return 7;

    case MONS_IRON_DEVIL:
        return 6;

    case MONS_RED_DEVIL:
    case MONS_GREAT_ORB_OF_EYES:
    case MONS_WIZARD:
    case MONS_CLAY_GOLEM:
    case MONS_WOOD_GOLEM:
    case MONS_STONE_GOLEM:
    case MONS_IRON_GOLEM:
    case MONS_SPECTRAL_WARRIOR:
    case MONS_SPINY_WORM:
    case MONS_PIT_FIEND:
    case MONS_NAGA_MAGE:
    case MONS_NAGA_WARRIOR:
    case MONS_ANCIENT_LICH:
    case MONS_SHINING_EYE:
    case MONS_TENTACLED_MONSTROSITY:
        return 5;

    case MONS_EARTH_ELEMENTAL:
    case MONS_FIRE_ELEMENTAL:
    case MONS_AIR_ELEMENTAL:
    case MONS_FLAYED_GHOST:
        return 4;

    case MONS_UGLY_THING:
    case MONS_HELL_KNIGHT:
    case MONS_NECROMANCER:
    case MONS_CRYSTAL_GOLEM:
    case MONS_EYE_OF_DEVASTATION:
        return 3;

    case MONS_GIANT_SPORE:
    case MONS_DANCING_WEAPON:
        return 2;

    case MONS_TOENAIL_GOLEM:
        return 1;

    default:
        return 0;

    }
    return 0;
}                               // end mons_rare_abyss()




int mons_level(int mcls)
{

    switch (you.where_are_you)
    {
    case BRANCH_DIS:            // about the same as Gehenna

        return mons_level_dis(mcls);

    case BRANCH_GEHENNA:
        return mons_level_gehenna(mcls);

    case BRANCH_COCYTUS:
        return mons_level_cocytus(mcls);

    case BRANCH_TARTARUS:
        return mons_level_tartarus(mcls);

    case BRANCH_ORCISH_MINES:
        return mons_level_orc_mines(mcls);

    case BRANCH_HIVE:
        return mons_level_hive(mcls);

    case BRANCH_LAIR:
        return mons_level_lair(mcls);

    case BRANCH_SLIME_PITS:
        return mons_level_slime_pits(mcls);

// 14 - the vaults - just uses normal monsters

    case BRANCH_CRYPT:
        return mons_level_crypt(mcls);

    case BRANCH_HALL_OF_BLADES:
        return mons_level_hall_ob(mcls);

    case BRANCH_HALL_OF_ZOT:

        switch (mcls)
        {
        case 29:
            return 30;
        case 75:
            return 30;
        case 132:
            return 31;
        case 146:
            return 32;
        case 165:
            return 31;
        case 270:
            return 29;
        case 271:
            return 29;
        case 272:
            return 29;
        case 273:
            return 29;
        case 274:
            return 29;
        case 369:
            return 31;
        case 370:
            return 29;
        case 373:
            return 32;
        case 378:
            return 31;
            /*case 379:
               return 29;
               case 380:
               return 32;
               case 381:
               return 27; */
        case 386:
            return 28;
        case 387:
            return 30;
        }
        break;

    case BRANCH_SNAKE_PIT:
        return mons_level_snake(mcls);

    case BRANCH_ELVEN_HALLS:
        return mons_level_elf(mcls);

    case BRANCH_TOMB:
        return mons_level_tomb(mcls);

    case BRANCH_SWAMP:
        return mons_level_swamp(mcls);

    }

    if (you.level_type == LEVEL_ABYSS)
        return mons_level_abyss(mcls);

    if (you.level_type == LEVEL_PANDEMONIUM)
        return mons_pan(mcls);

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case MONS_GOBLIN:
        return 1;

    case MONS_GIANT_COCKROACH:
    case MONS_SMALL_SNAKE:
    case MONS_OOZE:
        return 2;

    case MONS_GIANT_BAT:
    case MONS_RAT:
    case MONS_KOBOLD:
        return 4;

    case MONS_JACKAL:
    case MONS_KILLER_BEE_LARVA:
    case MONS_HOBGOBLIN:
    case MONS_GIANT_MITE:
    case MONS_GNOLL:
        return 5;

    case MONS_WORM:
    case MONS_SNAKE:
        return 6;

    case MONS_ORC:
        return 7;

    case MONS_OGRE:
    case MONS_ORC_WIZARD:
    case MONS_GIANT_EYEBALL:
    case MONS_PHANTOM:
    case MONS_GIANT_ANT:
    case MONS_FUNGUS:
    case MONS_HOUND:
    case MONS_SCORPION:
        return 8;

    case MONS_IMP:
    case MONS_CENTAUR:
    case MONS_NECROPHAGE:
    case MONS_QUASIT:
    case MONS_ZOMBIE_SMALL:
    case MONS_ICE_BEAST:
    case MONS_JELLY:
    case MONS_BROWN_SNAKE:
        return 9;

    case MONS_DEEP_ELF_SOLDIER:
    case MONS_GIANT_FROG:
    case MONS_GIANT_BEETLE:
    case MONS_GIANT_SPORE:
    case MONS_MUMMY:
    case MONS_ORC_WARRIOR:
    case MONS_WIGHT:
    case MONS_STEAM_DRAGON:
        return 10;

    case MONS_HIPPOGRIFF:
    case MONS_SHADOW:
    case MONS_HUNGRY_GHOST:
    case MONS_YELLOW_WASP:
    case MONS_KILLER_BEE:
    case MONS_GIANT_LIZARD:
        return 11;

    case MONS_ETTIN:
    case MONS_MANTICORE:
    case MONS_PLANT:
    case MONS_UNSEEN_HORROR:
    case MONS_WYVERN:
    case MONS_EYE_OF_DRAINING:
        return 12;

    case MONS_YAK:
    case MONS_BIG_KOBOLD:
    case MONS_OKLOB_PLANT:
    case MONS_WOOD_GOLEM:
    case MONS_TWO_HEADED_OGRE:
    case MONS_TROLL:
    case MONS_GIANT_CENTIPEDE:
    case MONS_GIANT_BROWN_FROG:
        return 13;

    case MONS_SOLDIER_ANT:
    case MONS_HILL_GIANT:
    case MONS_WRAITH:
    case MONS_WOLF_SPIDER:
        return 14;

    case MONS_EFREET:
    case MONS_BRAIN_WORM:
    case MONS_ORC_PRIEST:
    case MONS_MOTTLED_DRAGON:
    case MONS_CYCLOPS:
    case MONS_EYE_OF_DEVASTATION:
    case MONS_GOLD_MIMIC:
    case MONS_WEAPON_MIMIC:
    case MONS_ARMOUR_MIMIC:
    case MONS_SCROLL_MIMIC:
    case MONS_POTION_MIMIC:
    case MONS_SKELETAL_WARRIOR:
        return 15;

    case MONS_GUARDIAN_NAGA:
    case MONS_RAKSHASA:
    case MONS_STONE_GOLEM:
    case MONS_BUTTERFLY:
    case MONS_WANDERING_MUSHROOM:
    case MONS_ZOMBIE_LARGE:
    case MONS_VAMPIRE:
    case MONS_SLIME_CREATURE:
    case MONS_GIANT_BLOWFLY:
    case MONS_BLINK_FROG:
        return 16;

    case MONS_CENTAUR_WARRIOR:
    case MONS_BOGGART:
    case MONS_SHAPESHIFTER:
    case MONS_GRIFFON:
    case MONS_WIZARD:
    case MONS_CLAY_GOLEM:
    case MONS_UGLY_THING:
        return 17;

    case MONS_DRAGON:
    case MONS_KOBOLD_DEMONOLOGIST:
    case MONS_GIANT_AMOEBA:
    case MONS_GARGOYLE:
        return 18;

    case MONS_GIANT_SLUG:
    case MONS_IRON_GOLEM:
    case MONS_TOENAIL_GOLEM:
    case MONS_YAKTAUR:
    case MONS_ROCK_TROLL:
    case MONS_OGRE_MAGE:
        return 19;

    case MONS_QUEEN_BEE:
    case MONS_ORC_KNIGHT:
    case MONS_GIANT_ORANGE_BRAIN:
    case MONS_ICE_DRAGON:
    case MONS_GREAT_ORB_OF_EYES:
    case MONS_VERY_UGLY_THING:
    case MONS_HYDRA:
    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
    case MONS_NECROMANCER:
    case MONS_EARTH_ELEMENTAL:
    case MONS_FIRE_ELEMENTAL:
    case MONS_AIR_ELEMENTAL:
    case MONS_SPINY_WORM:
    case MONS_RED_WASP:
    case MONS_SHADOW_WRAITH:
    case MONS_GIANT_SNAIL:
    case MONS_NAGA_MAGE:
    case MONS_NAGA_WARRIOR:
    case MONS_DEEP_ELF_FIGHTER:
    case MONS_DEEP_ELF_KNIGHT:
    case MONS_DEEP_ELF_MAGE:
    case MONS_DEEP_ELF_SUMMONER:
        return 20;

    case MONS_BOULDER_BEETLE:
    case MONS_ORC_HIGH_PRIEST:
    case MONS_PULSATING_LUMP:
        return 21;

    case MONS_FREEZING_WRAITH:
    case MONS_CRYSTAL_GOLEM:
    case MONS_FLAYED_GHOST:
    case MONS_REDBACK:
    case MONS_VAPOUR:
    case MONS_SPHINX:
    case MONS_BORING_BEETLE:
        return 22;

    case MONS_SHINING_EYE:
    case MONS_ORC_SORCEROR:
        return 23;

    case MONS_TUNNELING_WORM:
    case MONS_BUMBLEBEE:
    case MONS_IRON_TROLL:
    case MONS_YAKTAUR_CAPTAIN:
        return 24;

    case MONS_FIRE_GIANT:
    case MONS_FROST_GIANT:
    case MONS_STONE_GIANT:
    case MONS_INSUBSTANTIAL_WISP:
    case MONS_DANCING_WEAPON:
    case MONS_HELL_KNIGHT:
    case MONS_DEEP_TROLL:
    case MONS_LICH:
        return 25;

    case MONS_SPECTRAL_WARRIOR:
    case MONS_STORM_DRAGON:
    case MONS_DEEP_ELF_CONJURER:
        return 26;

    case MONS_DEEP_ELF_PRIEST:
    case MONS_GLOWING_SHAPESHIFTER:
    case MONS_TENTACLED_MONSTROSITY:
        return 27;

    case MONS_SHADOW_DRAGON:
        return 29;

    case MONS_TITAN:
    case MONS_GOLDEN_DRAGON:
    case MONS_DEEP_ELF_HIGH_PRIEST:
    case MONS_DEEP_ELF_DEMONOLOGIST:
    case MONS_DEEP_ELF_ANNIHILATOR:
    case MONS_DEEP_ELF_SORCEROR:
    case MONS_DEEP_ELF_DEATH_MAGE:
    case MONS_ANCIENT_LICH:
    case MONS_QUICKSILVER_DRAGON:
    case MONS_IRON_DRAGON:
    case MONS_SKELETAL_DRAGON:
        return 30;

        //case MONS_ICE_FIEND:
        //case MONS_SHADOW_FIEND:
        //case MONS_DEATH_YAK: return 32;

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
        return 500;

    default:
        return 99;


    }
    return 50;
}                               // end mons_level()




int mons_rarity(int mcls)
{


    switch (you.where_are_you)
    {
    case BRANCH_DIS:
        return mons_rare_dis(mcls);     // about the same as Gehenna

    case BRANCH_GEHENNA:
        return mons_rare_gehenna(mcls);

    case BRANCH_COCYTUS:
        return mons_rare_cocytus(mcls);

    case BRANCH_TARTARUS:
        return mons_rare_tartarus(mcls);

    case BRANCH_ORCISH_MINES:
        return mons_rare_orc_mines(mcls);

    case BRANCH_HIVE:
        return mons_rare_hive(mcls);

    case BRANCH_LAIR:
        return mons_rare_lair(mcls);

    case BRANCH_SLIME_PITS:
        return mons_rare_slime_pits(mcls);

// 14 - the vaults - uses normal monsters

    case BRANCH_CRYPT:
        return mons_rare_crypt(mcls);

    case BRANCH_HALL_OF_BLADES:
        return mons_rare_hall_ob(mcls);

    case BRANCH_HALL_OF_ZOT:

        switch (mcls)
        {
        case 29:
            return 40;
        case 75:
            return 40;
        case 132:
            return 50;
        case 146:
            return 42;
        case 165:
            return 21;
        case 270:
            return 29;
        case 271:
            return 29;
        case 272:
            return 29;
        case 273:
            return 29;
        case 274:
            return 29;
        case 369:
            return 40;
        case 370:
            return 50;
        case 373:
            return 20;
        case 378:
            return 15;
/*        case 379:
   return 10;
   case 380:
   return 13;
   case 381:
   return 15; */
        case 386:
            return 88;
        case 387:
            return 40;
        }
        break;

    case BRANCH_SNAKE_PIT:
        return mons_rare_snake(mcls);

    case BRANCH_ELVEN_HALLS:
        return mons_rare_elf(mcls);

    case BRANCH_TOMB:
        return mons_rare_tomb(mcls);

    case BRANCH_SWAMP:
        return mons_rare_swamp(mcls);

    }

    if (you.level_type == LEVEL_ABYSS)
        return mons_rare_abyss(mcls);

    switch (mcls)
    {
    case MONS_GIANT_BAT:
    case MONS_RED_DEVIL:
    case MONS_GOBLIN:
    case MONS_IMP:
    case MONS_ORC:
    case MONS_RAT:
    case MONS_UGLY_THING:
    case MONS_ZOMBIE_SMALL:
    case MONS_HOBGOBLIN:
    case MONS_KOBOLD:
    case MONS_ZOMBIE_LARGE:
    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
    case MONS_HILL_GIANT:
    case MONS_GIANT_FROG:
        return 99;

    case MONS_GIANT_ANT:
    case MONS_SNAKE:
        return 80;

    case MONS_CENTAUR_WARRIOR:
        return 80;

    case MONS_FLYING_SKULL:
    case MONS_SLIME_CREATURE:
        return 75;

    case MONS_HELL_HOUND:
        return 71;

    case MONS_HELLION:
    case MONS_CENTAUR:
    case MONS_HOUND:
    case MONS_CYCLOPS:
    case MONS_OGRE:
    case MONS_TROLL:
    case MONS_YAK:
    case MONS_ORC_WARRIOR:
    case MONS_YAKTAUR_CAPTAIN:
    case MONS_GIANT_BROWN_FROG:
        return 70;

    case MONS_ORC_KNIGHT:
    case MONS_JELLY:
    case MONS_ROTTING_DEVIL:
        return 60;

    case MONS_SHAPESHIFTER:
        return 59;

    case MONS_STONE_GIANT:
        return 53;

    case MONS_GIANT_BLOWFLY:
    case MONS_GIANT_COCKROACH:
    case MONS_KILLER_BEE:
    case MONS_SCORPION:
    case MONS_WORM:
    case MONS_GIANT_BEETLE:
    case MONS_ICE_BEAST:
    case MONS_UNSEEN_HORROR:
    case MONS_ORC_WIZARD:
    case MONS_BIG_KOBOLD:
    case MONS_TORMENTOR:
    case MONS_HIPPOGRIFF:
        return 50;

    case MONS_ROCK_TROLL:
        return 48;

    case MONS_MANTICORE:
    case MONS_OGRE_MAGE:
        return 45;

    case MONS_SHADOW:
    case MONS_HUNGRY_GHOST:
        return 41;

    case MONS_YAKTAUR:
    case MONS_GIANT_CENTIPEDE:
    case MONS_WYVERN:
    case MONS_GIANT_EYEBALL:
    case MONS_WIGHT:
    case MONS_WRAITH:
    case MONS_JACKAL:
    case MONS_PHANTOM:
    case MONS_TWO_HEADED_OGRE:
    case MONS_GIANT_SPORE:
    case MONS_REAPER:
    case MONS_HAIRY_DEVIL:
    case MONS_GRIFFON:
    case MONS_MOTTLED_DRAGON:
        return 40;

    case MONS_WOLF_SPIDER:
        return 36;

    case MONS_FREEZING_WRAITH:
    case MONS_GLOWING_SHAPESHIFTER:
    case MONS_SOLDIER_ANT:
    case MONS_GIANT_AMOEBA:
        return 35;

    case MONS_BOULDER_BEETLE:
        return 34;

    case MONS_EYE_OF_DRAINING:
        return 33;

    case MONS_GIANT_SLUG:
        return 32;

    case MONS_NECROPHAGE:
    case MONS_FIRE_VORTEX:
    case MONS_YELLOW_WASP:
    case MONS_DRAGON:
    case MONS_MUMMY:
    case MONS_VAMPIRE:
    case MONS_SOUL_EATER:
    case MONS_GIANT_MITE:
    case MONS_GNOLL:
    case MONS_BROWN_SNAKE:
    case MONS_GIANT_LIZARD:
    case MONS_SPINY_WORM:
    case MONS_SMALL_SNAKE:
    case MONS_GOLD_MIMIC:
    case MONS_WEAPON_MIMIC:
    case MONS_ARMOUR_MIMIC:
    case MONS_SCROLL_MIMIC:
    case MONS_POTION_MIMIC:
    case MONS_SKELETAL_WARRIOR:
        return 30;

    case MONS_FLAYED_GHOST:
        return 29;

    case MONS_BRAIN_WORM:
        return 26;

    case MONS_PLANT:
    case MONS_GREAT_ORB_OF_EYES:
    case MONS_ORC_PRIEST:
    case MONS_BOGGART:
    case MONS_OOZE:
    case MONS_IRON_TROLL:
    case MONS_FIRE_GIANT:
    case MONS_FROST_GIANT:
    case MONS_DEEP_TROLL:
    case MONS_RED_WASP:
    case MONS_DEEP_ELF_FIGHTER:
    case MONS_DEEP_ELF_KNIGHT:
        return 25;

    case MONS_REDBACK:
    case MONS_INSUBSTANTIAL_WISP:
    case MONS_SHADOW_DRAGON:
    case MONS_GIANT_SNAIL:
    case MONS_SPHINX:
    case MONS_FUNGUS:
    case MONS_RAKSHASA:
    case MONS_BUTTERFLY:
    case MONS_ICE_DRAGON:
    case MONS_HYDRA:
    case MONS_WIZARD:
    case MONS_STEAM_DRAGON:
    case MONS_VERY_UGLY_THING:
    case MONS_SPECTRAL_WARRIOR:
    case MONS_STORM_DRAGON:
        return 20;

    case MONS_LICH:
    case MONS_TENTACLED_MONSTROSITY:
    case MONS_BORING_BEETLE:
        return 17;

    case MONS_BLINK_FROG:
    case MONS_EYE_OF_DEVASTATION:
    case MONS_EFREET:
    case MONS_NECROMANCER:
    case MONS_CLAY_GOLEM:
    case MONS_WOOD_GOLEM:
        return 15;

    case MONS_KOBOLD_DEMONOLOGIST:
        return 13;

    case MONS_ORC_HIGH_PRIEST:
    case MONS_BUMBLEBEE:
        return 12;

    case MONS_TITAN:
    case MONS_SHADOW_WRAITH:
    case MONS_DEEP_ELF_SOLDIER:
    case MONS_HELL_KNIGHT:
    case MONS_WANDERING_MUSHROOM:
    case MONS_GIANT_ORANGE_BRAIN:
    case MONS_ORC_SORCEROR:
    case MONS_OKLOB_PLANT:
    case MONS_STONE_GOLEM:
    case MONS_IRON_GOLEM:
    case MONS_SKELETAL_DRAGON:
        return 10;

    case MONS_GOLDEN_DRAGON:
        return 7;

    case MONS_GARGOYLE:
        return 6;

    case MONS_DEEP_ELF_MAGE:
    case MONS_DEEP_ELF_SUMMONER:
    case MONS_DEEP_ELF_HIGH_PRIEST:
    case MONS_QUICKSILVER_DRAGON:
    case MONS_IRON_DRAGON:
    case MONS_NAGA_MAGE:
    case MONS_NAGA_WARRIOR:
    case MONS_CRYSTAL_GOLEM:
    case MONS_VAPOUR:
    case MONS_DANCING_WEAPON:
        return 5;

    case MONS_EARTH_ELEMENTAL:
    case MONS_FIRE_ELEMENTAL:
    case MONS_AIR_ELEMENTAL:
    case MONS_DEEP_ELF_CONJURER:
        return 4;

    case MONS_DEEP_ELF_PRIEST:
    case MONS_DEEP_ELF_DEMONOLOGIST:
    case MONS_DEEP_ELF_ANNIHILATOR:
    case MONS_DEEP_ELF_SORCEROR:
    case MONS_DEEP_ELF_DEATH_MAGE:
    case MONS_ANCIENT_LICH:
    case MONS_GUARDIAN_NAGA:
        return 3;

    case MONS_SHINING_EYE:
    case MONS_TOENAIL_GOLEM:
    case MONS_PULSATING_LUMP:
        return 2;

    case MONS_ETTIN:
    case MONS_KILLER_BEE_LARVA:
    case MONS_QUASIT:
    case MONS_TUNNELING_WORM:
    case MONS_ABOMINATION_SMALL:
    case MONS_ANGEL:
    case MONS_FIEND:
    case MONS_QUEEN_BEE:
    case MONS_ABOMINATION_LARGE:
    case MONS_WORM_TAIL:
    case MONS_MINOTAUR:
    case MONS_RAKSHASA_FAKE:
        return 0;

        //case 114 is human
        //case MONS_DEATH_YAK: return 32;

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
        return 500;

    default:
        return 0;
    }

    return 0;

}                               // end int mons_rarity()




int mons_level_orc_mines(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case MONS_FUNGUS:
    case MONS_GOBLIN:
    case MONS_ORC:
        mlev = 0;
        break;

    case MONS_HOBGOBLIN:
    case MONS_ORC_WARRIOR:
    case MONS_ORC_PRIEST:
        mlev = 1;
        break;

    case MONS_GNOLL:
    case MONS_OGRE:
        mlev = 2;
        break;

    case MONS_CYCLOPS:
    case MONS_TWO_HEADED_OGRE:
    case MONS_TROLL:
    case MONS_ORC_WIZARD:
    case MONS_ORC_KNIGHT:
    case MONS_ORC_SORCEROR:
    case MONS_ORC_HIGH_PRIEST:
    case MONS_ROCK_TROLL:
    case MONS_STONE_GIANT:
    case MONS_OGRE_MAGE:
    case MONS_IRON_TROLL:
    case MONS_ORC_WARLORD:
        mlev = 3;
        break;

    }

    return mlev + you.branch_stairs[0] + 1;

}                               // end mons_level_orc_mines()




int mons_rare_orc_mines(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case MONS_ORC:
        mlev = 300;
        break;

    case MONS_GOBLIN:
    case MONS_ORC_WARRIOR:
        mlev = 30;
        break;

    case MONS_HOBGOBLIN:
    case MONS_OGRE:
        mlev = 20;
        break;

    case MONS_TROLL:
        mlev = 13;
        break;

    case MONS_FUNGUS:
    case MONS_ORC_WIZARD:
    case MONS_ORC_KNIGHT:
    case MONS_ORC_SORCEROR:
    case MONS_ORC_PRIEST:
        mlev = 10;
        break;

    case MONS_CYCLOPS:
    case MONS_TWO_HEADED_OGRE:
    case MONS_ORC_HIGH_PRIEST:
        mlev = 5;
        break;

    case MONS_ROCK_TROLL:
    case MONS_STONE_GIANT:
    case MONS_IRON_TROLL:
        mlev = 3;
        break;

    case MONS_GNOLL:
    case MONS_ORC_WARLORD:
        mlev = 2;
        break;

    case MONS_OGRE_MAGE:
        mlev = 1;
        break;

    }

    return mlev;

}                               // end mons_rare_orc_mines()




int mons_level_hive(int mcls)
{

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case MONS_PLANT:
    case MONS_KILLER_BEE:
        return you.branch_stairs[1] + 1 + 0;

    case MONS_KILLER_BEE_LARVA:
        return you.branch_stairs[1] + 1 + 2;

    default:
        return 99;
    }
    return 50;
}                               // end mons_level_hive()




int mons_rare_hive(int mcls)
{

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case MONS_KILLER_BEE:
        return 300;

    case MONS_PLANT:
        return 100;

    case MONS_KILLER_BEE_LARVA:
        return 50;

    default:
        return 0;
    }

}                               // end mons_rare_hive()




int mons_level_lair(int mcls)
{

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case MONS_GIANT_BAT:
    case MONS_JACKAL:
    case MONS_RAT:
        return you.branch_stairs[2] + 1 + 0;

    case MONS_GIANT_CENTIPEDE:
        return you.branch_stairs[2] + 1 + 1;

    case MONS_HOUND:
    case MONS_GREY_RAT:
    case MONS_GIANT_FROG:
        return you.branch_stairs[2] + 1 + 2;

    case MONS_WORM:
        return you.branch_stairs[2] + 1 + 3;

    case MONS_FUNGUS:
    case MONS_SCORPION:
    case MONS_SNAKE:
    case MONS_GIANT_MITE:
    case MONS_GIANT_LIZARD:
    case MONS_GREEN_RAT:
    case MONS_GIANT_BROWN_FROG:
        return you.branch_stairs[2] + 1 + 4;

    case MONS_YELLOW_WASP:
    case MONS_GIANT_BEETLE:
    case MONS_PLANT:
    case MONS_BUTTERFLY:
    case MONS_HIPPOGRIFF:
    case MONS_BROWN_SNAKE:
    case MONS_WAR_DOG:
    case MONS_SPINY_FROG:
    case MONS_GIANT_SLUG:
        return you.branch_stairs[2] + 1 + 5;

    case MONS_GIANT_SPORE:
    case MONS_YAK:
    case MONS_WOLF_SPIDER:
    case MONS_STEAM_DRAGON:
    case MONS_ORANGE_RAT:
    case MONS_SHEEP:
    case MONS_BLINK_FROG:
    case MONS_GIANT_SNAIL:
        return you.branch_stairs[2] + 1 + 6;

    case MONS_WYVERN:
    case MONS_OKLOB_PLANT:
    case MONS_BRAIN_WORM:
    case MONS_HYDRA:
    case MONS_BUMBLEBEE:
    case MONS_BLACK_SNAKE:
    case MONS_FIREDRAKE:
        return you.branch_stairs[2] + 1 + 7;

    case MONS_WANDERING_MUSHROOM:
    case MONS_GRIFFON:
    case MONS_REDBACK:
    case MONS_LINDWORM:
    case MONS_ELEPHANT_SLUG:
        return you.branch_stairs[2] + 1 + 8;

    case MONS_BOULDER_BEETLE:
    case MONS_DEATH_YAK:
    case MONS_SPINY_WORM:
    case MONS_BORING_BEETLE:
        return you.branch_stairs[2] + 1 + 9;

    default:
        return 99;


    }
    return 50;
}                               // end mons_level_lair()




int mons_rare_lair(int mcls)
{

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case MONS_RAT:
        return 200;

    case MONS_GIANT_BAT:
        return 99;

    case MONS_FUNGUS:
        return 50;

    case MONS_HOUND:
        return 60;

    case MONS_JACKAL:
        return 70;

    case MONS_SCORPION:
        return 10;

    case MONS_WORM:
        return 30;

    case MONS_YELLOW_WASP:
        return 5;

    case MONS_GIANT_BEETLE:
        return 10;

    case MONS_GIANT_SPORE:
        return 2;

    case MONS_PLANT:
    case MONS_SNAKE:
        return 80;

    case MONS_YAK:
        return 50;

    case MONS_WYVERN:
        return 20;

    case MONS_OKLOB_PLANT:
        return 10;

    case MONS_WOLF_SPIDER:
        return 6;

    case MONS_BUTTERFLY:
        return 5;

    case MONS_WANDERING_MUSHROOM:
        return 8;

    case MONS_BRAIN_WORM:
        return 7;

    case MONS_BOULDER_BEETLE:
        return 20;

    case MONS_GIANT_MITE:
        return 30;

    case MONS_STEAM_DRAGON:
        return 10;

    case MONS_HIPPOGRIFF:
        return 50;

    case MONS_GRIFFON:
        return 30;

    case MONS_HYDRA:
        return 20;

    case MONS_BROWN_SNAKE:
    case MONS_GIANT_LIZARD:
        return 90;

    case MONS_DEATH_YAK:
        return 30;

    case MONS_BUMBLEBEE:
        return 7;

    case MONS_REDBACK:
        return 8;

    case MONS_SPINY_WORM:
        return 9;

    case MONS_LINDWORM:
        return 10;

    case MONS_ELEPHANT_SLUG:
        return 30;

    case MONS_WAR_DOG:
        return 35;

    case MONS_GREY_RAT:
        return 99;

    case MONS_GREEN_RAT:
        return 64;

    case MONS_ORANGE_RAT:
        return 10;

    case MONS_BLACK_SNAKE:
        return 47;

    case MONS_SHEEP:
        return 36;

    case MONS_GIANT_CENTIPEDE:
        return 50;

    case MONS_FIREDRAKE:
        return 36;

    case MONS_GIANT_FROG:
    case MONS_GIANT_BROWN_FROG:
        return 99;

    case MONS_SPINY_FROG:
        return 75;

    case MONS_BLINK_FROG:
        return 45;

    case MONS_GIANT_SLUG:
        return 55;

    case MONS_GIANT_SNAIL:
        return 56;

    case MONS_BORING_BEETLE:
        return 29;

    default:
        return 0;


    }
    return 0;
}                               // end mons_rare_lair()




int mons_level_slime_pits(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case MONS_JELLY:
    case MONS_OOZE:
        mlev = 1;
        break;

    case MONS_SLIME_CREATURE:
        mlev = 2;
        break;

    case MONS_GIANT_AMOEBA:
    case MONS_BROWN_OOZE:
    case MONS_AZURE_JELLY:
    case MONS_ACID_BLOB:
        mlev = 3;
        break;

    case MONS_PULSATING_LUMP:
        mlev = 4;
        break;

    case MONS_DEATH_OOZE:
    case MONS_TENTACLED_MONSTROSITY:
        return 5;

    }

    return mlev + you.branch_stairs[3] + 1;

}                               // end mons_level_slime_pits()




int mons_rare_slime_pits(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case MONS_JELLY:
        mlev = 300;
        break;

    case MONS_SLIME_CREATURE:
        mlev = 200;
        break;

    case MONS_BROWN_OOZE:
        mlev = 150;
        break;

    case MONS_GIANT_AMOEBA:
    case MONS_ACID_BLOB:
        mlev = 100;
        break;

    case MONS_OOZE:
        mlev = 50;
        break;

    case MONS_AZURE_JELLY:
        mlev = 30;
        break;

    case MONS_PULSATING_LUMP:
    case MONS_DEATH_OOZE:
        mlev = 20;
        break;

    case MONS_TENTACLED_MONSTROSITY:
        mlev = 2;
        break;

    }

    return mlev;

}                               // end mons_rare_slime_pits()




int mons_level_crypt(int mcls)
{

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */
    int mlev = 0;


    switch (mcls)
    {
    case MONS_ZOMBIE_SMALL:
        mlev = 0;
        break;

    case MONS_PHANTOM:
    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
    case MONS_ZOMBIE_LARGE:
    case MONS_WIGHT:
        mlev = 1;
        break;

    case MONS_SHADOW:
    case MONS_HUNGRY_GHOST:
    case MONS_NECROPHAGE:
    case MONS_SKELETAL_WARRIOR:
        mlev = 2;
        break;

    case MONS_NECROMANCER:
    case MONS_PULSATING_LUMP:
    case MONS_FLAYED_GHOST:
    case MONS_GHOUL:
    case MONS_ROTTING_HULK:
    case MONS_WRAITH:
    case MONS_FLYING_SKULL:
        mlev = 3;
        break;

    case MONS_SPECTRAL_WARRIOR:
    case MONS_SHADOW_WRAITH:
    case MONS_VAMPIRE_KNIGHT:
    case MONS_VAMPIRE_MAGE:
    case MONS_SKELETAL_DRAGON:
    case MONS_ABOMINATION_SMALL:
    case MONS_MUMMY:
    case MONS_VAMPIRE:
    case MONS_ABOMINATION_LARGE:
        mlev = 4;
        break;

    case MONS_REAPER:
    case MONS_ANCIENT_LICH:
    case MONS_LICH:
        mlev = 5;
        break;

    default:
        mlev = 99;


    }
    return mlev + you.branch_stairs[5] + 1;

}                               // end mons_level_crypt()




int mons_rare_crypt(int mcls)
{
    int mlev = 0;

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case MONS_ZOMBIE_SMALL:
    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
        mlev = 410;
        break;

    case MONS_ZOMBIE_LARGE:
        mlev = 300;
        break;

    case MONS_SKELETAL_WARRIOR:
        mlev = 75;
        break;

    case MONS_NECROPHAGE:
        mlev = 50;
        break;

    case MONS_WIGHT:
        mlev = 35;
        break;

    case MONS_WRAITH:
        mlev = 33;
        break;

    case MONS_SHADOW:
        mlev = 30;
        break;

    case MONS_NECROMANCER:
    case MONS_GHOUL:
        mlev = 25;
        break;

    case MONS_MUMMY:
    case MONS_SKELETAL_DRAGON:
        mlev = 24;
        break;

    case MONS_VAMPIRE:
    case MONS_PHANTOM:
        mlev = 21;
        break;

    case MONS_VAMPIRE_KNIGHT:
    case MONS_VAMPIRE_MAGE:
        mlev = 20;
        break;

    case MONS_ROTTING_HULK:
        mlev = 17;
        break;

    case MONS_SPECTRAL_WARRIOR:
        mlev = 14;
        break;

    case MONS_FLYING_SKULL:
    case MONS_FLAYED_GHOST:
        mlev = 13;
        break;

    case MONS_HUNGRY_GHOST:
        mlev = 12;
        break;

    case MONS_SHADOW_WRAITH:
        mlev = 10;
        break;

    case MONS_ANCIENT_LICH:
        mlev = 8;
        break;

    case MONS_ABOMINATION_SMALL:
    case MONS_LICH:
    case MONS_REAPER:
        mlev = 5;
        break;

    case MONS_ABOMINATION_LARGE:
        mlev = 4;
        break;

    case MONS_PULSATING_LUMP:
        mlev = 3;
        break;

    default:
        mlev = 0;


    }
    return mlev;

}                               // end mons_rare_crypt()




int mons_level_snake(int mcls)
{
    int mlev = 0;

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case MONS_SNAKE:
        mlev = 1;
        break;

    case MONS_BROWN_SNAKE:
    case MONS_BLACK_SNAKE:
    case MONS_YELLOW_SNAKE:
    case MONS_GREY_SNAKE:
        mlev = 2;
        break;

    case MONS_NAGA:
        mlev = 3;
        break;

    case MONS_NAGA_WARRIOR:
        mlev = 4;
        break;

    case MONS_NAGA_MAGE:
        mlev = 5;
        break;

    default:
        mlev = 99;


    }

    return mlev + you.branch_stairs[9] + 1;

}                               // end mons_level_snake()




int mons_rare_snake(int mcls)
{
    int mlev = 0;

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case MONS_SNAKE:
    case MONS_BROWN_SNAKE:
        mlev = 99;
        break;

    case MONS_NAGA:
        mlev = 53;
        break;

    case MONS_BLACK_SNAKE:
        mlev = 72;
        break;

    case MONS_NAGA_WARRIOR:
        mlev = 34;
        break;

    case MONS_YELLOW_SNAKE:
    case MONS_GREY_SNAKE:
        mlev = 32;
        break;

    case MONS_NAGA_MAGE:
        mlev = 15;
        break;

    default:
        mlev = 0;


    }

    return mlev;

}                               // end mons_rare_snake()




int mons_level_elf(int mcls)
{
    int mlev = 0;

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case MONS_DEEP_ELF_SOLDIER:
    case MONS_DEEP_ELF_FIGHTER:
    case MONS_ORC:
    case MONS_ORC_WARRIOR:
        mlev = 1;
        break;

    case MONS_ORC_WIZARD:
    case MONS_DEEP_ELF_MAGE:
    case MONS_DEEP_ELF_SUMMONER:
        mlev = 2;
        break;

    case MONS_FUNGUS:
    case MONS_DEEP_ELF_CONJURER:
    case MONS_ORC_KNIGHT:
        mlev = 3;
        break;

    case MONS_ORC_SORCEROR:
    case MONS_DEEP_ELF_PRIEST:
    case MONS_SHAPESHIFTER:
    case MONS_DEEP_ELF_KNIGHT:
        mlev = 4;
        break;

    case MONS_ORC_PRIEST:
    case MONS_ORC_HIGH_PRIEST:
        mlev = 5;
        break;

    case MONS_DEEP_ELF_HIGH_PRIEST:
    case MONS_DEEP_ELF_DEMONOLOGIST:
    case MONS_DEEP_ELF_ANNIHILATOR:
    case MONS_DEEP_ELF_SORCEROR:
    case MONS_DEEP_ELF_DEATH_MAGE:
        mlev = 7;
        break;

    default:
        mlev = 99;
        break;

    }

    return mlev + you.branch_stairs[10] + 1;

}                               // end mons_level_elf()




int mons_rare_elf(int mcls)
{
    int mlev = 0;

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case MONS_FUNGUS:
        mlev = 300;
        break;

    case MONS_DEEP_ELF_SOLDIER:
    case MONS_DEEP_ELF_FIGHTER:
    case MONS_DEEP_ELF_MAGE:
        mlev = 100;
        break;

    case MONS_DEEP_ELF_KNIGHT:
        mlev = 80;
        break;

    case MONS_DEEP_ELF_SUMMONER:
        mlev = 72;
        break;

    case MONS_DEEP_ELF_CONJURER:
        mlev = 63;
        break;

    case MONS_DEEP_ELF_PRIEST:
        mlev = 44;
        break;

    case MONS_SHAPESHIFTER:
        return 25;

    case MONS_ORC:
        mlev = 20;
        break;

    case MONS_DEEP_ELF_DEMONOLOGIST:
    case MONS_DEEP_ELF_SORCEROR:
        mlev = 17;
        break;

    case MONS_DEEP_ELF_ANNIHILATOR:
    case MONS_DEEP_ELF_DEATH_MAGE:
    case MONS_ORC_WIZARD:
        mlev = 13;
        break;

    case MONS_ORC_WARRIOR:
        mlev = 11;
        break;

    case MONS_DEEP_ELF_HIGH_PRIEST:
    case MONS_ORC_SORCEROR:
        mlev = 10;
        break;

    case MONS_ORC_KNIGHT:
    case MONS_ORC_PRIEST:
    case MONS_ORC_HIGH_PRIEST:
        mlev = 5;
        break;

    default:
        mlev = 0;
        break;

    }

    return mlev;

}                               // end mons_rare_elf()




int mons_level_tomb(int mcls)
{

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */
    int mlev = 0;


    switch (mcls)
    {
    case MONS_ZOMBIE_SMALL:
        mlev = 0;
        break;

    case MONS_MUMMY:
    case MONS_ZOMBIE_LARGE:
    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
        mlev = 1;
        break;

    case MONS_GUARDIAN_MUMMY:
    case MONS_FLYING_SKULL:
        mlev = 2;
        break;

    case MONS_LICH:
    case MONS_ANCIENT_LICH:
    case MONS_MUMMY_PRIEST:
        mlev = 3;
        break;

    default:
        mlev = 99;
    }

    return mlev + you.branch_stairs[5] + 1;

}                               // end mons_level_tomb()




int mons_rare_tomb(int mcls)
{

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */
    int mlev = 0;


    switch (mcls)
    {
    case MONS_MUMMY:
        mlev = 300;
        break;

    case MONS_GUARDIAN_MUMMY:
        mlev = 202;
        break;

    case MONS_MUMMY_PRIEST:
        mlev = 40;
        break;

    case MONS_FLYING_SKULL:
        mlev = 33;
        break;

    case MONS_ZOMBIE_LARGE:
    case MONS_SMALL_SKELETON:
    case MONS_LARGE_SKELETON:
        mlev = 21;
        break;

    case MONS_ZOMBIE_SMALL:
        mlev = 20;
        break;

    case MONS_LICH:
        mlev = 4;
        break;

    case MONS_ANCIENT_LICH:
        mlev = 2;
        break;

    default:
        mlev = 0;


    }

    return mlev;

}                               // end mons_rare_tomb()




int mons_level_swamp(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case MONS_GIANT_BAT:
    case MONS_SWAMP_DRAKE:
    case MONS_GIANT_FROG:
    case MONS_GIANT_AMOEBA:
    case MONS_GIANT_SLUG:
    case MONS_RAT:
    case MONS_WORM:
    case MONS_GIANT_BLOWFLY:
        mlev = 1;
        break;

    case MONS_GIANT_BROWN_FROG:
    case MONS_FUNGUS:
    case MONS_NECROPHAGE:
    case MONS_PLANT:
    case MONS_SNAKE:
    case MONS_BUTTERFLY:
    case MONS_GIANT_LIZARD:
    case MONS_GIANT_MOSQUITO:
    case MONS_GIANT_SNAIL:
        mlev = 2;
        break;

    case MONS_SPINY_FROG:
    case MONS_PHANTOM:
    case MONS_JELLY:
    case MONS_UGLY_THING:
    case MONS_HUNGRY_GHOST:
    case MONS_BROWN_SNAKE:
    case MONS_INSUBSTANTIAL_WISP:
    case MONS_RED_WASP:
    case MONS_SWAMP_DRAGON:
        mlev = 3;
        break;

    case MONS_BLINK_FROG:
    case MONS_SLIME_CREATURE:
    case MONS_VERY_UGLY_THING:
    case MONS_HYDRA:
    case MONS_VAPOUR:
    case MONS_TENTACLED_MONSTROSITY:
        mlev = 4;
        break;

    default:
        mlev = 99;

    }

    return mlev + you.branch_stairs[12] + 1;

}                               // end mons_level_swamp()




int mons_rare_swamp(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case MONS_GIANT_MOSQUITO:
        mlev = 250;
        break;

    case MONS_PLANT:
        mlev = 200;
        break;

    case MONS_GIANT_FROG:
        mlev = 150;
        break;

    case MONS_GIANT_BLOWFLY:
        mlev = 100;
        break;

    case MONS_GIANT_BAT:
    case MONS_FUNGUS:
        mlev = 99;
        break;

    case MONS_GIANT_BROWN_FROG:
        mlev = 90;
        break;

    case MONS_SWAMP_DRAKE:
        mlev = 80;
        break;

    case MONS_RAT:
        mlev = 61;
        break;

    case MONS_SLIME_CREATURE:
    case MONS_HYDRA:
        mlev = 54;
        break;

    case MONS_SNAKE:
        mlev = 52;
        break;

    case MONS_INSUBSTANTIAL_WISP:
        mlev = 43;
        break;

    case MONS_BROWN_SNAKE:
        mlev = 33;
        break;

    case MONS_RED_WASP:
    case MONS_SWAMP_DRAGON:
    case MONS_SPINY_FROG:
        mlev = 30;
        break;

    case MONS_JELLY:
        mlev = 23;
        break;

    case MONS_BUTTERFLY:
    case MONS_GIANT_LIZARD:
        mlev = 22;
        break;

    case MONS_WORM:
        mlev = 21;
        break;

    case MONS_VERY_UGLY_THING:
    case MONS_VAPOUR:
        mlev = 14;
        break;

    case MONS_PHANTOM:
    case MONS_UGLY_THING:
    case MONS_HUNGRY_GHOST:
        mlev = 13;
        break;

    case MONS_NECROPHAGE:
        mlev = 12;
        break;

    case MONS_BLINK_FROG:
    case MONS_GIANT_AMOEBA:
    case MONS_GIANT_SLUG:
    case MONS_GIANT_SNAIL:
        mlev = 10;
        break;

    case MONS_TENTACLED_MONSTROSITY:
        mlev = 5;
        break;

    default:
        mlev = 0;


    }

    return mlev;

}                               // end mons_rare_swamp()




int mons_level_hall_ob(int mcls)
{
    switch (mcls)
    {
    case MONS_DANCING_WEAPON:
        return you.branch_stairs[6] + 1;
    }
    return 0;
}




int mons_rare_hall_ob(int mcls)
{
    switch (mcls)
    {
    case MONS_DANCING_WEAPON:
        return 1000;
    }
    return 0;
}




/*
   New branch must be added in:
   new_game stair location
   down/up stairs (to and back) misc.cc
   new_level (2 places) (misc.cc)
   item_check items.cc
   look_around (direct.cc)
   ouch ouch.cc (death message)
   and here...
 */


int branch_depth(int branch)
{
    switch (branch)
    {
    case 0:
        return 4;               // orc mines

    case 1:
        return 4;               // the hive

    case 2:
        return 10;              // the lair

    case 3:
        return 6;               // the slime pits

    case 4:
        return 8;               // the vaults

    case 5:
        return 5;               // the crypt

    case 6:
        return 1;               // the hall of blades

    case 7:
        return 5;               // the hall of Zot

    case 8:
        return 1;               // the ecumenical temple

    case 9:
        return 5;               // the snake pit

    case 10:
        return 7;               // the elven halls

    case 11:
        return 3;               // the Tomb

    case 12:
        return 5;               // the Swamp

    }

    return 0;

}                               // end branch_depth()

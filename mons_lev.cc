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
#include "enum.h"
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
    case 3:
        mlev = 1;
        break;                  // "red devil"

    case 8:
        mlev = 1;
        break;                  // "imp"

    case 13:
        mlev = 1;
        break;                  // "necrophage"

    case 15:
        mlev = 2;
        break;                  // "phantom"

    case 25:
        mlev = 2;
        break;                  // "zombie"

    case 31:
        mlev = 6;
        break;                  // "fiend"

    case 37:
        mlev = 5;
        break;                  // "lich"

    case 38:
        mlev = 3;
        break;                  // "mummy"

    case 47:
        mlev = 4;
        break;                  // "vampire"

    case 48:
        mlev = 4;
        break;                  // "wraith"

    case 51:
        mlev = 1;
        break;                  // "zombie"

    case 60:
        mlev = 2;
        break;                  // "wight"

    case 63:
        mlev = 2;
        break;                  // "shadow"

    case 68:
        mlev = 3;
        break;                  // "efreet"

    case 72:
        mlev = 3;
        break;                  // "flying skull"

    case 73:
        mlev = 2;
        break;                  // "hell hound"

    case 80:
        mlev = 3;
        break;                  // "hellion"

    case 81:
        mlev = 2;
        break;                  // "rotting devil"

    case 82:
        mlev = 2;
        break;                  // "tormentor"

    case 83:
        mlev = 5;
        break;                  // "reaper"

    case 84:
        mlev = 5;
        break;                  // "soul eater"

    case 85:
        mlev = 4;
        break;                  // "hairy devil"

    case 89:
        mlev = 4;
        break;                  // iron devil

    case 75:
        mlev = 5;
        break;                  // "ice dragon"

    case 77:
        mlev = 4;
        break;                  // "freezing wraith"

    case 86:
        mlev = 5;
        break;                  // "ice devil"

    case 87:
        mlev = 5;
        break;                  // "blue devil"

    case 107:
    case 108:
        mlev = 2;
        break;                  // "skeleton"

    case 109:
        mlev = 2;
        break;                  //strcat(gmon_name, "hell knight"); break; break; break;

    case 116:
        mlev = 1;
        break;                  // clay golem

    case 118:
        mlev = 2;
        break;                  // stone golem

    case 119:
        mlev = 3;
        break;                  // iron golem

    case 130:
        mlev = 5;
        break;                  // spec

    case 137:
        mlev = 4;
        break;                  // flayed ghost

    case 144:
        mlev = 5;
        break;                  // dancing weapon

    case 356:
        mlev = 6;
        break;                  // ancient lich

    case 369:
        mlev = 6;
        break;                  // skeletal dragon

    case 394:
        mlev = 3;
        break;                  // hell-hog

    case 398:
        mlev = 6;
        break;                  // iron D

    case 399:
        mlev = 1;
        break;                  // skeletal warrior

    default:
        return 0;
    }                           // end switch

    return mlev + 26;

}


int mons_level_gehenna(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case 3:
        mlev = 1;
        break;                  // "red devil"

    case 8:
        mlev = 2;
        break;                  // "imp"

    case 13:
        mlev = 2;
        break;                  // "necrophage"

    case 15:
        mlev = 3;
        break;                  // "phantom"

    case 25:
        mlev = 1;
        break;                  // "zombie"

    case 31:
        mlev = 6;
        break;                  // "fiend"

    case 37:
        mlev = 6;
        break;                  // "lich"

    case 38:
        mlev = 3;
        break;                  // "mummy"

    case 47:
        mlev = 4;
        break;                  // "vampire"

    case 48:
        mlev = 4;
        break;                  // "wraith"

    case 51:
        mlev = 1;
        break;                  // "zombie"

    case 60:
        mlev = 3;
        break;                  // "wight"

    case 63:
        mlev = 3;
        break;                  // "shadow"

    case 68:
        mlev = 5;
        break;                  // "efreet"

    case 72:
        mlev = 3;
        break;                  // "flying skull"

    case 73:
        mlev = 2;
        break;                  // "hell hound"

    case 80:
        mlev = 5;
        break;                  // "hellion"

    case 81:
        mlev = 3;
        break;                  // "rotting devil"

    case 82:
        mlev = 5;
        break;                  // "tormentor"

    case 83:
        mlev = 6;
        break;                  // "reaper"

    case 84:
        mlev = 6;
        break;                  // "soul eater"

    case 85:
        mlev = 4;
        break;                  // "hairy devil"

    case 107:
    case 108:
        mlev = 1;
        break;                  // "skeleton"

    case 109:
        mlev = 4;
        break;                  //strcat(gmon_name, "hell knight"); break; break; break;

    case 116:
        mlev = 1;
        break;                  // clay golem

    case 118:
        mlev = 2;
        break;                  // stone golem

    case 119:
        mlev = 3;
        break;                  // iron golem

    case 130:
        mlev = 6;
        break;                  // spec

    case 137:
        mlev = 5;
        break;                  // flayed ghost

    case 245:
        mlev = 6;
        break;                  // pit fiend

    case 356:
        mlev = 6;
        break;                  // ancient lich

    case 369:
        mlev = 6;
        break;                  // skeletal dragon

    case 394:
        mlev = 2;
        break;                  // hell-hog

    case 395:
        mlev = 6;
        break;                  // Serpent of Hell

    default:
        return 0;
    }                           // end switch

    return mlev + 26;

}


int mons_rare_dis(int mcls)
{
    switch (mcls)
    {
    case 3:
        return 50;              // "red devil"

    case 8:
        return 99;              // "imp"

    case 13:
        return 25;              // "necrophage"

    case 15:
        return 17;              // "phantom"

    case 25:
        return 99;              // "zombie"

    case 31:
        return 3;               // "fiend"

    case 37:
        return 10;              // "lich"

    case 38:
        return 45;              // "mummy"

    case 47:
        return 19;              // "vampire"

    case 48:
        return 48;              // "wraith"

    case 51:
        return 99;              // "zombie"

    case 60:
        return 45;              // "wight"

    case 63:
        return 56;              // "shadow"

    case 68:
        return 10;              // "efreet"

    case 72:
        return 35;              // "flying skull"

    case 73:
        return 46;              // "hell hound"

    case 80:
        return 40;              // "hellion"

    case 81:
        return 10;              // "rotting devil"

    case 82:
        return 66;              // "tormentor"

    case 83:
        return 77;              // "reaper"

    case 84:
        return 10;              // "soul eater"

    case 85:
        return 15;              // "hairy devil"

    case 89:
        return 99;              // iron devil

    case 77:
        return 30;              // "freezing wraith"

    case 86:
        return 30;              // "ice devil"

    case 87:
        return 40;              // "blue devil"

    case 107:
    case 108:
        return 25;              // "skeleton"

    case 109:
        return 10;              //strcat(gmon_name, "hell knight"); break;

    case 116:
        return 10;              // clay golem

    case 118:
        return 10;              // stone golem

    case 119:
        return 10;              // iron golem

    case 130:
        return 10;              // spec

    case 137:
        return 25;              // flayed ghost

    case 144:
        return 10;              // dancing weapon

    case 356:
        return 3;               // ancient lich

    case 369:
        return 20;              // skeletal dragon

    case 394:
        return 20;              // hell-hog

    case 398:
        return 5;               // iron D

    case 399:
        return 50;              // skeletal warrior

    default:
        return 0;
    }                           // end switch

}


int mons_rare_gehenna(int mcls)
{
    switch (mcls)
    {

    case 3:
        return 60;              // "red devil"

    case 8:
        return 30;              // "imp"

    case 13:
        return 50;              // "necrophage"

    case 15:
        return 32;              // "phantom"

    case 25:
        return 99;              // "zombie"

    case 31:
        return 5;               // "fiend"

    case 37:
        return 25;              // "lich"

    case 38:
        return 70;              // "mummy"

    case 47:
        return 44;              // "vampire"

    case 48:
        return 53;              // "wraith"

    case 51:
        return 99;              // "zombie"

    case 60:
        return 60;              // "wight"

    case 63:
        return 61;              // "shadow"

    case 68:
        return 5;               // "efreet"

    case 72:
        return 43;              // "flying skull"

    case 73:
        return 41;              // "hell hound"

    case 80:
        return 54;              // "hellion"

    case 81:
        return 50;              // "rotting devil"

    case 82:
        return 42;              // "tormentor"

    case 83:
        return 43;              // "reaper"

    case 84:
        return 5;               // "soul eater"

    case 85:
        return 20;              // "hairy devil"

    case 89:
        return 30;              // iron devil - in Dis

    case 107:
    case 108:
        return 99;              // "skeleton"

    case 109:
        return 21;              //strcat(gmon_name, "hell knight"); break;

    case 116:
        return 10;              // clay golem

    case 118:
        return 8;               // stone golem

    case 119:
        return 5;               // iron golem

    case 130:
        return 20;              // spec

    case 137:
        return 32;              // flayed ghost

    case 245:
        return 7;               // pit fiend

    case 356:
        return 4;               // ancient lich

    case 369:
        return 10;              // skeletal dragon

    case 394:
        return 30;              // hell-hog

    case 395:
        return 4;               // Serpent of Hell

    default:
        return 0;
    }                           // end switch

}



int mons_level_cocytus(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case 13:
        mlev = 1;
        break;                  // "necrophage"

    case 15:
        mlev = 2;
        break;                  // "phantom"

    case 25:
        mlev = 1;
        break;                  // "zombie"

    case 34:
        mlev = 2;
        break;                  // "ice beast"

    case 37:
        mlev = 6;
        break;                  // "lich"

    case 38:
        mlev = 4;
        break;                  // "mummy"

    case 47:
        mlev = 3;
        break;                  // "vampire"

    case 48:
        mlev = 4;
        break;                  // "wraith"

    case 51:
        mlev = 1;
        break;                  // "zombie"

    case 60:
        mlev = 3;
        break;                  // "wight"

    case 63:
        mlev = 2;
        break;                  // "shadow"

    case 64:
        mlev = 4;
        break;                  // "hungry ghost"

    case 72:
        mlev = 3;
        break;                  // "flying skull"

    case 75:
        mlev = 5;
        break;                  // "ice dragon"

    case 77:
        mlev = 4;
        break;                  // "freezing wraith"

    case 81:
        mlev = 3;
        break;                  // "rotting devil"

    case 82:
        mlev = 5;
        break;                  // "tormentor"

    case 83:
        mlev = 6;
        break;                  // "reaper"

    case 84:
        mlev = 6;
        break;                  // "soul eater"

    case 85:
        mlev = 4;
        break;                  // "hairy devil"

    case 86:
        mlev = 5;
        break;                  // "ice devil"

    case 87:
        mlev = 2;
        break;                  // "blue devil"

    case 107:
    case 108:
        mlev = 1;
        break;                  // "skeleton"

    case 130:
        mlev = 4;
        break;                  // spec

    case 356:
        mlev = 6;
        break;                  // ancient lich

    case 369:
        mlev = 6;
        break;                  // skeletal dragon

    case 399:
        mlev = 1;
        break;                  // skeletal warrior

    default:
        return 0;
    }

    return mlev + 26;

}

int mons_rare_cocytus(int mcls)
{
    switch (mcls)
    {
    case 13:
        return 25;              // "necrophage"

    case 15:
        return 25;              // "phantom"

    case 25:
        return 85;              // "zombie"

    case 34:
        return 85;              // "ice beast"

    case 37:
        return 12;              // "lich"

    case 38:
        return 35;              // "mummy"

    case 47:
        return 34;              // "vampire"

    case 48:
        return 45;              // "wraith"

    case 51:
        return 85;              // "zombie"

    case 60:
        return 45;              // "wight"

    case 63:
        return 56;              // "shadow"

    case 64:
        return 26;              // "hungry ghost"

    case 72:
        return 57;              // "flying skull"

    case 75:
        return 38;              // "ice dragon"

    case 77:
        return 87;              // "freezing wraith"

    case 81:
        return 37;              // "rotting devil"

    case 82:
        return 37;              // "tormentor"

    case 83:
        return 47;              // "reaper"

    case 84:
        return 19;              // "soul eater"

    case 85:
        return 26;              // "hairy devil"

    case 86:
        return 76;              // "ice devil"

    case 87:
        return 76;              // "blue devil"

    case 107:
    case 108:
        return 85;              // "skeleton"

    case 130:
        return 20;
    case 356:
        return 5;               // ancient lich

    case 369:
        return 12;              // skeletal dragon

    case 399:
        return 50;              // skeletal warrior

    default:
        return 0;
    }
}



int mons_level_tartarus(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {

    case 3:
        mlev = 1;
        break;                  // "red devil"

    case 8:
        mlev = 1;
        break;                  // "imp"

    case 13:
        mlev = 2;
        break;                  // "necrophage"

    case 15:
        mlev = 2;
        break;                  // "phantom"

    case 25:
        mlev = 2;
        break;                  // "zombie"

    case 37:
        mlev = 6;
        break;                  // "lich"

    case 38:
        mlev = 4;
        break;                  // "mummy"

    case 47:
        mlev = 5;
        break;                  // "vampire"

    case 48:
        mlev = 3;
        break;                  // "wraith"

    case 51:
        mlev = 2;
        break;                  // "zombie"

    case 60:
        mlev = 2;
        break;                  // "wight"

    case 63:
        mlev = 3;
        break;                  // "shadow"

    case 64:
        mlev = 4;
        break;                  // "hungry ghost"

    case 72:
        mlev = 5;
        break;                  // "flying skull"

    case 73:
        mlev = 3;
        break;                  // "hell hound"

    case 77:
        mlev = 3;
        break;                  // "freezing wraith"

    case 80:
        mlev = 5;
        break;                  // "hellion"

    case 81:
        mlev = 5;
        break;                  // "rotting devil"

    case 82:
        mlev = 4;
        break;                  // "tormentor"

    case 83:
        mlev = 5;
        break;                  // "reaper"

    case 84:
        mlev = 6;
        break;                  // "soul eater"

    case 85:
        mlev = 6;
        break;                  // "hairy devil"

    case 86:
        mlev = 4;
        break;                  // "ice devil"

    case 87:
        mlev = 4;
        break;                  // "blue devil"

    case 107:
    case 108:
        mlev = 1;
        break;                  // "skeleton"

    case 109:
        mlev = 2;
        break;                  // "hell knight"); break;

    case 110:
        mlev = 3;
        break;                  // "necromancer"); break;

    case 130:
        mlev = 4;
        break;                  // spec

    case 137:
        mlev = 4;
        break;                  // flayed ghost

    case 165:
        mlev = 5;
        break;                  // shadow dragon

    case 237:
        mlev = 1;
        break;                  // shadow imp

    case 238:
        mlev = 3;
        break;                  // shadow demon

    case 356:
        mlev = 6;
        break;                  // ancient lich

    case 369:
        mlev = 4;
        break;                  // skeletal dragon

    case 399:
        mlev = 1;
        break;                  // skeletal warrior

    default:
        return 0;
    }

    return mlev + 26;

}

int mons_rare_tartarus(int mcls)
{
    switch (mcls)
    {

    case 3:
        return 13;              // "red devil"

    case 8:
        return 20;              // "imp"

    case 13:
        return 72;              // "necrophage"

    case 15:
        return 52;              // "phantom"

    case 25:
        return 99;              // "zombie"

    case 37:
        return 24;              // "lich"

    case 38:
        return 33;              // "mummy"

    case 47:
        return 44;              // "vampire"

    case 48:
        return 52;              // "wraith"

    case 51:
        return 99;              // "zombie"

    case 60:
        return 71;              // "wight"

    case 63:
        return 92;              // "shadow"

    case 64:
        return 32;              // "hungry ghost"

    case 72:
        return 53;              // "flying skull"

    case 73:
        return 52;              // "hell hound"

    case 77:
        return 60;              // "freezing wraith"

    case 80:
        return 42;              // "hellion"

    case 81:
        return 62;              // "rotting devil"

    case 82:
        return 42;              // "tormentor"

    case 83:
        return 73;              // "reaper"

    case 84:
        return 35;              // "soul eater"

    case 85:
        return 30;              // "hairy devil"

    case 86:
        return 34;              // "ice devil" not really appropriate for a fiery hell.

    case 87:
        return 32;              // "blue devil"

    case 107:
    case 108:
        return 99;              // "skeleton"

    case 109:
        return 14;              //strcat(gmon_name, "hell knight"); break;

    case 110:
        return 12;              //strcat(gmon_name, "necromancer"); break;

    case 130:
        return 45;
    case 137:
        return 30;              // flayed ghost

    case 165:
        return 12;              // shadow dragon

    case 237:
        return 99;              // shadow imp

    case 238:
        return 50;              // shadow demon

    case 356:
        return 6;               // ancient lich

    case 369:
        return 40;              // skeletal dragon

    case 399:
        return 99;              // skeletal warrior

    default:
        return 0;
    }
}

int mons_level_abyss(int mcls)
{
//int found = 0;

    switch (mcls)
    {
    case 3:                     //60; // "red devil"

    case 5:                     //8; // "fungus"

    case 8:                     //10; // "imp"

    case 13:                    //10; // "necrophage"

    case 15:                    //8; // "phantom"

    case 20:                    //21; // "ugly thing"

    case 23:                    //91; // "abomination"

    case 25:                    //10; // "zombie"

    case 32:                    //12; // "giant spore"

    case 34:                    //9; // "ice beast"

    case 35:                    //9; // "jelly"

    case 37:                    //34; // "lich"

    case 38:                    //10; // "mummy"

    case 39:                    //13; // "naga"

    case 43:                    //17; // "raksasha"

    case 46:                    //12; // "unseen horror"

    case 47:                    //19; // "vampire"

    case 48:                    //15; // "wraith"

    case 49:                    //1500; // "abomination"

    case 51:                    //20; // "zombie"

    case 59:                    //8; // "giant eyeball"

    case 60:                    //10; // "wight"

    case 63:                    //11; // "shadow"

    case 64:                    //11; // "hungry ghost"

    case 65:                    //14; // "eye of draining"

    case 68:                    //18; // "efreet"

    case 69:                    //16; // "brain worm"

    case 70:                    //26; // "giant orange brain"

    case 72:                    //65; // "flying skull"

    case 73:                    //61; // "hell hound"

    case 77:                    //25; // "freezing wraith"

    case 79:                    //25; // GooE

    case 80:                    //60; // "hellion"

    case 81:                    //60; // "rotting devil"

    case 82:                    //62; // "tormentor"

    case 83:                    //63; // "reaper"

    case 84:                    //65; // "soul eater"

    case 85:                    //60; // "hairy devil"

    case 86:                    //6500; // "ice devil" not really appropriate for a fiery hell.

    case 87:                    //6200; // "blue devil"

    case 89:
    case 102:                   //25; //strcat(gmon_name, "very ugly thing"); break;

    case 107:
    case 108:                   //20; // "skeleton"

    case 109:                   //31; //strcat(gmon_name, "hell knight"); break;

    case 110:                   //20; //strcat(gmon_name, "necromancer"); break;

    case 111:                   //17; //strcat(gmon_name, "wizard"); break;

    case 116:                   //20; // clay golem

    case 117:                   //15; // wood golem

    case 118:                   //22; // stone golem

    case 119:                   //25; // iron golem

    case 120:                   //30; // crystal golem

    case 121:                   //35; // toenail golem

    case 123:                   //22; //strcat(gmon_name, "earth elemental"); break;

    case 124:                   //22; //strcat(gmon_name, "fire elemental"); break;

    case 125:                   //22; //strcat(gmon_name, "air elemental"); break;

    case 130:                   //30; //strcat(gmon_name, "spectre"); break;

    case 137:                   //29; // flayed ghost

    case 140:                   //32; // wisp

    case 141:                   //29; // vapour

    case 143:
    case 144:
    case 220:                   // demons:

    case 221:
    case 222:
    case 223:
    case 224:                   //strcat(gmon_name, "lesser demon"); break;

    case 225:
    case 226:
    case 227:
    case 228:
    case 229:                   //strcat(gmon_name, "demon"); break;

    case 230:
    case 231:
    case 232:
    case 233:
    case 234:                   //strcat(gmon_name, "greater demon"); break;

    case 235:
    case 236:
    case 237:
    case 238:
    case 239:
    case 245:                   // pit fiend

    case 260:
    case 261:
    case 356:                   // ancient lich

    case 364:                   // shining eye

    case 369:                   // skeletal dragon

    case 370:                   // tentacled monstr

    case 385:                   // eye of devastation

    case 399:                   // skeletal warrior

        return 1;
//found = 1; break;
    default:
        return 0;


    }

    return 0;

}                               // end mons_level_abyss(mcls)

int mons_rare_abyss(int mcls)
{


    switch (mcls)
    {
    case 3:
        return 5;               // "red devil"

    case 5:
        return 8;               // "fungus"

    case 8:
        return 10;              // "imp"

    case 13:
        return 10;              // "necrophage"

    case 15:
        return 8;               // "phantom"

    case 20:
        return 3;               // "ugly thing"

    case 23:
        return 99;              // "abomination"

    case 25:
        return 35;              // "zombie"

    case 32:
        return 2;               // "giant spore"

    case 34:
        return 9;               // "ice beast"

    case 35:
        return 9;               // "jelly"

    case 37:
        return 14;              // "lich"

    case 38:
        return 10;              // "mummy"

    case 39:
        return 10;              // "naga"

    case 43:
        return 17;              // "raksasha"

    case 46:
        return 12;              // "unseen horror"

    case 47:
        return 9;               // "vampire"

    case 48:
        return 15;              // "wraith"

    case 49:
        return 99;              // "abomination"

    case 51:
        return 35;              // "zombie"

    case 59:
        return 8;               // "giant eyeball"

    case 60:
        return 10;              // "wight"

    case 63:
        return 11;              // "shadow"

    case 64:
        return 11;              // "hungry ghost"

    case 65:
        return 14;              // "eye of draining"

    case 68:
        return 18;              // "efreet"

    case 69:
        return 16;              // "brain worm"

    case 70:
        return 15;              // "giant orange brain"

    case 72:
        return 15;              // "flying skull"

    case 73:
        return 11;              // "hell hound"

    case 77:
        return 15;              // "freezing wraith"

    case 79:
        return 5;               // GooE

    case 80:
        return 10;              // "hellion"

    case 81:
        return 10;              // "rotting devil"

    case 82:
        return 9;               // "tormentor"

    case 83:
        return 8;               // "reaper"

    case 84:
        return 7;               // "soul eater"

    case 85:
        return 10;              // "hairy devil"

    case 86:
        return 10;              // "ice devil" not really appropriate for a fiery hell.

    case 87:
        return 10;              // "blue devil"

    case 89:
        return 6;               // "iron devil"

    case 102:
        return 15;              //strcat(gmon_name, "very ugly thing"); break;

    case 107:
    case 108:
        return 40;              // "skeleton"

    case 109:
        return 3;               //strcat(gmon_name, "hell knight"); break;

    case 110:
        return 3;               //strcat(gmon_name, "necromancer"); break;

    case 111:
        return 5;               //strcat(gmon_name, "wizard"); break;

    case 116:
        return 5;               // clay golem

    case 117:
        return 5;               // wood golem

    case 118:
        return 5;               // stone golem

    case 119:
        return 5;               // iron golem

    case 120:
        return 3;               // crystal golem

    case 121:
        return 1;               // toenail golem

    case 123:
        return 4;               //strcat(gmon_name, "earth elemental"); break;

    case 124:
        return 4;               //strcat(gmon_name, "fire elemental"); break;

    case 125:
        return 4;               //strcat(gmon_name, "air elemental"); break;

    case 130:
        return 5;               //strcat(gmon_name, "spectre"); break;

    case 137:
        return 4;               // flayed ghost

    case 140:
        return 12;              // wisp

    case 141:
        return 9;               // vapour

    case 143:
        return 5;               // spiny worm

    case 144:
        return 2;               // dancing weapon

    case 220:                   // demons:

    case 221:
    case 222:
    case 223:
    case 224:
        return 80;
    case 225:
    case 226:
    case 227:
    case 228:
    case 229:
        return 50;
    case 230:
    case 231:
    case 232:
    case 233:
    case 234:
    case 235:
    case 236:
    case 237:
    case 238:
    case 239:
        return 10;
    case 245:
        return 5;               // pit fiend

    case 260:
    case 261:
    case 356:                   // ancient lich

    case 364:                   // shining eye

    case 370:                   // tentacled monstr

        return 5;
    case 369:
        return 20;              // skeletal dragon

    case 385:
        return 3;               // eye of devastation

    case 399:
        return 40;              // skeletal warrior

    default:
        return 0;


    }
    return 0;
}                               // end mons_level(mcls)


int mons_level(int mcls)
{

    switch (you.where_are_you)
    {
    case 2:
        return mons_level_gehenna(mcls);        // Gehenna

    case 4:
        return mons_level_cocytus(mcls);        // Cocytus

    case 1:
        return mons_level_dis(mcls);    // Dis - about the same as Gehenna

    case 5:
        return mons_level_tartarus(mcls);       // Tartarus

    case 10:
        return mons_level_orc_mines(mcls);      // orcish mines

    case 11:
        return mons_level_hive(mcls);   // the hive

    case 12:
        return mons_level_lair(mcls);   // the lair

    case 13:
        return mons_level_slime_pits(mcls);     // the lair
        /* 14 - the vaults - just uses normal monsters */
    case 15:
        return mons_level_crypt(mcls);  // the lair

    case 16:
        return mons_level_hall_ob(mcls);        // the lair

    case 17:                    // the realm of Zot

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
/*        case 379:
            return 29;
        case 380:
            return 32;
        case 381:
            return 27;*/
        case 386:
            return 28;
        case 387:
            return 30;
        }
        break;
    case 19:
        return mons_level_snake(mcls);  // the lair

    case 20:
        return mons_level_elf(mcls);    // the lair

    case 21:
        return mons_level_tomb(mcls);   // the lair

    case 22:
        return mons_level_swamp(mcls);  // the Swamp

    }

    if (you.level_type == 2)
        return mons_level_abyss(mcls);
    if (you.level_type == 3)
        return mons_pan(mcls);

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case 0:
        return 8;               // "giant ant"

    case 1:
        return 4;               // "giant bat"

    case 2:
        return 9;               // "centaur"

    case 4:
        return 12;              // "ettin"

    case 5:
        return 8;               // "fungus"

    case 6:
        return 1;               // "goblin"

    case 7:
        return 8;               // "hound"

    case 8:
        return 9;               // "imp"

    case 9:
        return 5;               // "jackal"

    case 10:
        return 11;              // "killer bee"

    case 11:
        return 5;               // "killer bee larva"

    case 12:
        return 12;              // "manticore"

    case 13:
        return 9;               // "necrophage"

    case 14:
        return 7;               // "orc"

    case 15:
        return 8;               // "phantom"

    case 16:
        return 9;               // "quasit"

    case 17:
        return 4;               // "rat"

    case 18:
        return 8;               // "scorpion"

    case 19:
        return 24;              // "tunneling worm"

    case 20:
        return 17;              // "ugly thing"

    case 22:
        return 6;               // "worm"

    case 24:
        return 11;              // "yellow wasp"

    case 25:
        return 9;               // "zombie"

    case 27:
        return 10;              // "giant beetle"

    case 28:
        return 15;              // "cyclops"

    case 29:
        return 18;              // "dragon"

    case 30:
        return 13;              // "two-headed ogre"

    case 32:
        return 10;              // "giant spore"

    case 33:
        return 5;               // "hobgoblin"

    case 34:
        return 9;               // "ice beast"

    case 35:
        return 9;               // "jelly"

    case 36:
        return 4;               // "kobold"

    case 37:
        return 25;              // "lich"

    case 38:
        return 10;              // "mummy"

    case 39:
        return 16;              // "naga"

    case 40:
        return 8;               // "ogre"

    case 41:
        return 12;              // "plant"

    case 42:
        return 20;              // "queen bee"

    case 43:
        return 16;              // "raksasha"

    case 44:
        return 6;               // "snake"

    case 45:
        return 13;              // "troll"

    case 46:
        return 12;              // "unseen horror"

    case 47:
        return 16;              // "vampire"

    case 48:
        return 14;              // "wraith"

    case 50:
        return 13;              // "yak"

    case 51:
        return 16;              // "zombie"

    case 52:
        return 10;              // "orc warrior"

    case 53:
        return 18;              // "kobold demonologist"

    case 54:
        return 8;               // "orc wizard"

    case 55:
        return 20;              // "orc knight"

    case 57:
        return 12;              // "wyvern"

    case 58:
        return 13;              // "kobold leader"

    case 59:
        return 8;               // "giant eyeball"

    case 60:
        return 10;              // "wight"

    case 61:
        return 13;              // "oklob plant"

    case 62:
        return 14;              // "wolf spider"

    case 63:
        return 11;              // "shadow"

    case 64:
        return 11;              // "hungry ghost"

    case 65:
        return 12;              // "eye of draining"

    case 66:
        return 16;              // "butterfly"

    case 67:
        return 16;              // "wandering mushroom"

    case 68:
        return 15;              // "efreet"

    case 69:
        return 15;              // "brain worm"

    case 70:
        return 20;              // "giant orange brain"

    case 71:
        return 21;              // "boulder beetle"

    case 75:
        return 20;              // "ice dragon"

    case 76:
        return 16;              // "slime thing"

    case 77:
        return 22;              // "freezing wraith"

    case 79:
        return 20;              // GooE

    case 98:
        return 27;              // glowing shapeshifter

    case 99:
        return 17;              // shapeshifter

    case 100:
        return 5;               //strcat(gmon_name, "giant mite"); break;

    case 101:
        return 10;              //strcat(gmon_name, "steam dragon"); break;

    case 102:
        return 20;              //strcat(gmon_name, "very ugly thing"); break;

    case 103:
        return 23;              //strcat(gmon_name, "orc sorceror"); break;

    case 104:
        return 11;              //strcat(gmon_name, "hippogriff"); break;

    case 105:
        return 17;              //strcat(gmon_name, "griffon"); break;

    case 106:
        return 20;              //strcat(gmon_name, "hydra"); break;

    case 107:
    case 108:
        return 20;              // "skeleton"

    case 109:
        return 25;              //strcat(gmon_name, "hell knight"); break;

    case 110:
        return 20;              //strcat(gmon_name, "necromancer"); break;

    case 111:
        return 17;              //strcat(gmon_name, "wizard"); break;

    case 112:
        return 15;              //strcat(gmon_name, "orc priest"); break;

    case 113:
        return 21;              //strcat(gmon_name, "orc high priest"); break;

    case 115:
        return 5;               // gnoll

    case 116:
        return 17;              // clay golem

    case 117:
        return 13;              // wood golem

    case 118:
        return 16;              // stone golem

    case 119:
        return 19;              // iron golem

    case 120:
        return 22;              // crystal golem

    case 121:
        return 19;              // toenail golem

    case 122:
        return 15;              // mottled dragon

    case 123:
        return 20;              //strcat(gmon_name, "earth elemental"); break;

    case 124:
        return 20;              //strcat(gmon_name, "fire elemental"); break;

    case 125:
        return 20;              //strcat(gmon_name, "air elemental"); break;
        //case 126: strcat(gmon_name, "Ice Fiend"); break;
        //case 127: strcat(gmon_name, "Shadow Fiend"); break;

    case 128:
        return 9;               //strcat(gmon_name, "brown snake"); break;

    case 129:
        return 11;              //strcat(gmon_name, "giant lizard"); break;

    case 130:
        return 26;              //strcat(gmon_name, "spectre"); break;

    case 131:
        return 21;              // pulsating mass

    case 132:
        return 26;              // storm D

    case 133:
        return 19;              //strcat(gmon_name, "yaktaur"); break;
        //case 134: return 32; //strcat(gmon_name, "death yak"); break;

    case 135:
        return 19;              //strcat(gmon_name, "rock troll"); break;

    case 136:
        return 25;              //strcat(gmon_name, "stone giant"); break; // stoned giant

    case 137:
        return 22;              // flayed ghost

    case 138:
        return 24;              // bumblebee

    case 139:
        return 22;              // red back

    case 140:
        return 25;              // wisp

    case 141:
        return 22;              // vapour

    case 142:
        return 19;              // Ogre mage

    case 143:
        return 20;              // spiny worm

    case 144:
        return 25;              // dancing weapon

    case 145:
        return 30;              // Titan

    case 146:
        return 30;              // Golden dragon

    case 159:
        return 13;              // giant centipede

    case 160:
        return 24;              // iron Troll

    case 162:
        return 25;              // fire giant

    case 163:
        return 25;              // frost giant

    case 165:
        return 29;              // shadow dragon

    case 168:
        return 25;              // deep troll

    case 169:
        return 16;              // giant blowfly

    case 170:
        return 20;              // red wasp

    case 173:
        return 14;              // soldier ant

    case 174:
        return 14;              // hill giant

    case 177:
        return 10;              // giant frog

    case 178:
        return 13;              // giant brown frog

    case 180:
        return 16;              // blink frog

    case 240:
        return 20;              // shadow wraith

    case 241:
        return 18;              // giant amoeba

    case 242:
        return 19;              // giant slug

    case 243:
        return 20;              // giant snail

    case 246:
        return 22;              // boring beetle

    case 247:
        return 18;              // gargoyle

    case 260:                   // Naga mage

    case 261:
        return 20;              // Naga warrior

    case 263:
        return 10;              // deep elf soldier

    case 264:
        return 20;              // deep elf fighter

    case 265:
        return 20;              // deep elf knight

    case 266:
        return 20;              // deep elf mage

    case 267:
        return 20;              // deep elf summoner

    case 268:
        return 26;              // deep elf conjurer

    case 269:
        return 27;              // deep elf priest

    case 270:
        return 30;              // deep elf high priest

    case 271:
        return 30;              // deep elf demonologist

    case 272:
        return 30;              // deep elf annihilator

    case 273:
        return 30;              // deep elf sorceror

    case 274:
        return 30;              // deep elf death mage

    case 356:
        return 30;              // ancient lich

    case 364:
        return 23;              // shining eye

    case 369:
        return 30;              // skeletal dragon

    case 370:
        return 27;              // tentacled monstr

    case 371:
        return 22;              // sphinx

    case 376:
        return 17;              // centaur warrior

    case 377:
        return 24;              // yaktaur warrior

    case 385:
        return 15;              // eye of devastation

    case 389:
    case 390:
    case 391:
    case 392:
    case 393:
        return 15;              // mimic

    case 396:
        return 17;              // boggart

    case 397:
        return 30;              // quicksilver D

    case 398:
        return 30;              // iron D

    case 399:
        return 15;              // skeletal warrior

    case MLAVA0:
        return 500;             // "lava worm"

    case MLAVA1:
        return 500;             // "lava fish"

    case MLAVA2:
        return 500;             // "lava snake"

    case MLAVA3:
        return 500;             // "another lava thing"

    case MWATER0:
        return 500;             // "big fish"

    case MWATER1:
        return 500;             // "giant goldfish"

    case MWATER2:
        return 500;             // "electrical eel"

    case MWATER3:
        return 500;             // "jellyfish"

    case MWATER4:
        return 500;             // ""

    case MWATER5:
        return 500;             // ""

    default:
        return 99;


    }
    return 50;
}                               // end mons_level(mcls)



int mons_rarity(int mcls)
{


    switch (you.where_are_you)
    {
    case 2:
        return mons_rare_gehenna(mcls);         // Gehenna

    case 4:
        return mons_rare_cocytus(mcls);         // Cocytus

    case 1:
        return mons_rare_dis(mcls);     // Dis - about the same as Gehenna

    case 5:
        return mons_rare_tartarus(mcls);        // Tartarus

    case 10:
        return mons_rare_orc_mines(mcls);       // orcish mines

    case 11:
        return mons_rare_hive(mcls);    // the hive

    case 12:
        return mons_rare_lair(mcls);    // the lair

    case 13:
        return mons_rare_slime_pits(mcls);      // the lair
        /* 14 - the vaults - uses normal monsters */
    case 15:
        return mons_rare_crypt(mcls);   // the crypt

    case 16:
        return mons_rare_hall_ob(mcls);         // the lair

    case 17:                    // the realm of Zot

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
            return 15;*/
        case 386:
            return 88;
        case 387:
            return 40;
        }
        break;
    case 19:
        return mons_rare_snake(mcls);   // the lair

    case 20:
        return mons_rare_elf(mcls);     // the lair

    case 21:
        return mons_rare_tomb(mcls);    // the tomb

    case 22:
        return mons_rare_swamp(mcls);   // the Swamp

    }

    if (you.level_type == 2)
        return mons_rare_abyss(mcls);

    switch (mcls)
    {
    case 0:
        return 80;              // "giant ant"

    case 1:
        return 99;              // "giant bat"

    case 2:
        return 70;              // "centaur"

    case 3:
        return 99;              // "red devil"

    case 4:
        return 0;               // "ettin"

    case 5:
        return 20;              // "fungus"

    case 6:
        return 99;              // "goblin"

    case 7:
        return 70;              // "hound"

    case 8:
        return 99;              // imp

    case 9:
        return 40;              // "jackal"

    case 10:
        return 50;              // "killer bee"

    case 11:
        return 0;               // "killer bee larva"

    case 12:
        return 45;              // "manticore"

    case 13:
        return 30;              // "necrophage"

    case 14:
        return 99;              // "orc"

    case 15:
        return 40;              // "phantom"

    case 16:
        return 0;               // "quasit"

    case 17:
        return 99;              // "rat"

    case 18:
        return 50;              // "scorpion"

    case 19:
        return 0;               // "tunneling worm" - these are currently too buggy

    case 20:
        return 99;              // "ugly thing"

    case 21:
        return 30;              // "fire vortex"

    case 22:
        return 50;              // "worm"

    case 23:
        return 0;               // "abomination"

    case 24:
        return 30;              // "yellow wasp"

    case 25:
        return 99;
    case 26:
        return 0;               // "angel"

    case 27:
        return 50;              // "giant beetle"

    case 28:
        return 70;              // "cyclops"

    case 29:
        return 30;              // "dragon"

    case 30:
        return 40;              // "two-headed ogre"

    case 31:
        return 0;               // "fiend"

    case 32:
        return 40;              // "giant spore"

    case 33:
        return 99;              // "hobgoblin"

    case 34:
        return 50;              // "ice beast"

    case 35:
        return 60;              // "jelly"

    case 36:
        return 99;              // "kobold"

    case 37:
        return 17;              // "lich"

    case 38:
        return 30;              // "mummy"

    case 39:
        return 3;               // "naga"

    case 40:
        return 70;              // "ogre"

    case 41:
        return 25;              // "plant"

    case 42:
        return 0;               // "queen bee"

    case 43:
        return 20;              // "raksasha"

    case 44:
        return 80;              // "snake"

    case 45:
        return 70;              // "troll"

    case 46:
        return 50;              // "unseen horror"

    case 47:
        return 30;              // "vampire"

    case 48:
        return 40;              // "wraith"

    case 49:
        return 0;               // "abomination"

    case 50:
        return 70;              // "yak"

    case 51:
        return 99;              // Z

    case 52:
        return 70;              // "orc warrior"

    case 53:
        return 13;              // "kobold demonologist"

    case 54:
        return 50;              // "orc wizard"

    case 55:
        return 60;              // "orc knight"

    case 56:
        return 0;               // "worm tail"

    case 57:
        return 40;              // "wyvern"

    case 58:
        return 50;              // "kobold leader"

    case 59:
        return 40;              // "giant eyeball"

    case 60:
        return 40;              // "wight"

    case 61:
        return 10;              // "oklob plant"

    case 62:
        return 36;              // "wolf spider"

    case 63:
        return 41;              // "shadow"

    case 64:
        return 41;              // "hungry ghost"

    case 65:
        return 33;              // "eye of draining"

    case 66:
        return 20;              // "butterfly"

    case 67:
        return 10;              // "wandering mushroom"

    case 68:
        return 15;              // "efreet"

    case 69:
        return 26;              // "brain worm"

    case 70:
        return 10;              // "giant orange brain"

    case 71:
        return 34;              // "boulder beetle"

    case 72:
        return 75;              // "flying skull"

    case 73:
        return 71;              // "hell hound"

    case 74:
        return 0;               // "minotaur"

    case 75:
        return 20;              // "ice dragon"

    case 76:
        return 75;              // "slime thing"

    case 77:
        return 35;              // "freezing wraith"

    case 78:
        return 0;               // fake R

    case 79:
        return 25;              // GooE

    case 80:
        return 70;              // "hellion"

    case 81:
        return 60;              // "rotting devil"

    case 82:
        return 50;              // "tormentor"

    case 83:
        return 40;              // "reaper"

    case 84:
        return 30;              // "soul eater"

    case 85:
        return 40;              // "hairy devil"

    case 98:
        return 35;              // glowing shapeshifter

    case 99:
        return 59;              // shapeshifter

    case 100:
        return 30;              //strcat(gmon_name, "giant mite"); break;

    case 101:
        return 20;              //strcat(gmon_name, "steam dragon"); break;

    case 102:
        return 20;              //strcat(gmon_name, "very ugly thing"); break;

    case 103:
        return 10;              //strcat(gmon_name, "orc sorceror"); break;

    case 104:
        return 50;              //strcat(gmon_name, "hippogriff"); break;

    case 105:
        return 40;              //strcat(gmon_name, "griffon"); break;

    case 106:
        return 20;              //strcat(gmon_name, "hydra"); break;

    case 107:
        return 99;
    case 108:
        return 99;              // skeletons

    case 109:
        return 10;              //strcat(gmon_name, "hell knight"); break;

    case 110:
        return 15;              //strcat(gmon_name, "necromancer"); break;

    case 111:
        return 20;              //strcat(gmon_name, "wizard"); break;

    case 112:
        return 25;              //strcat(gmon_name, "orc priest"); break;

    case 113:
        return 12;              //strcat(gmon_name, "orc high priest"); break;
        // 114 is human

    case 115:
        return 30;              // gnoll

    case 116:
        return 15;              // clay golem

    case 117:
        return 15;              // wood golem

    case 118:
        return 10;              // stone golem

    case 119:
        return 10;              // iron golem

    case 120:
        return 5;               // crystal golem

    case 121:
        return 2;               // toenail golem

    case 122:
        return 40;              // mottled dragon

    case 123:
        return 4;               //strcat(gmon_name, "earth elemental"); break;

    case 124:
        return 4;               //strcat(gmon_name, "fire elemental"); break;

    case 125:
        return 4;               //strcat(gmon_name, "air elemental"); break;

    case 128:
        return 30;              //strcat(gmon_name, "brown snake"); break;

    case 129:
        return 30;              //strcat(gmon_name, "giant lizard"); break;

    case 130:
        return 20;              //strcat(gmon_name, "spectre"); break;

    case 131:
        return 2;               // pulsating mass

    case 132:
        return 20;              // storm D

    case 133:
        return 40;              //strcat(gmon_name, "yaktaur"); break;
        //case 134: return 32; //strcat(gmon_name, "death yak"); break;

    case 135:
        return 48;              //strcat(gmon_name, "rock troll"); break;

    case 136:
        return 53;              //strcat(gmon_name, "stone giant"); break; // stoned giant

    case 137:
        return 29;              // flayed ghost

    case 138:
        return 12;              // bumblebee

    case 139:
        return 20;              // red back

    case 140:
        return 20;              // wisp

    case 141:
        return 5;               // vapour

    case 142:
        return 45;              // Ogre mage

    case 143:
        return 30;              // Spiny worm

    case 144:
        return 5;               // dancing weapon

    case 145:
        return 10;              // Titan

    case 146:
        return 7;               // Golden dragon

    case 159:
        return 40;              // giant centipede

    case 160:
        return 25;              // iron Troll

    case 162:
        return 25;              // fire giant

    case 163:
        return 25;              // frost giant

    case 165:
        return 20;              // shadow dragon

    case 168:
        return 25;              // deep troll

    case 169:
        return 50;              // giant blowfly

    case 170:
        return 25;              // red wasp

    case 173:
        return 35;              // soldier ant

    case 174:
        return 99;              // hill giant

    case 177:
        return 99;              // giant frog

    case 178:
        return 70;              // giant brown frog

    case 180:
        return 15;              // blink frog

    case 240:
        return 10;              // shadow wraith

    case 241:
        return 35;              // giant amoeba

    case 242:
        return 32;              // giant slug

    case 243:
        return 20;              // giant snail

    case 246:
        return 17;              // boring beetle

    case 247:
        return 6;               // gargoyle

    case 260:
    case 261:
        return 5;               // Nagas

    case 263:
        return 10;              // deep elf soldier

    case 264:
        return 25;              // deep elf fighter

    case 265:
        return 25;              // deep elf knight

    case 266:
        return 5;               // deep elf mage

    case 267:
        return 5;               // deep elf summoner

    case 268:
        return 4;               // deep elf conjurer

    case 269:
        return 3;               // deep elf priest

    case 270:
        return 5;               // deep elf high priest

    case 271:
        return 3;               // deep elf demonologist

    case 272:
        return 3;               // deep elf annihilator

    case 273:
        return 3;               // deep elf sorceror

    case 274:
        return 3;               // deep elf death mage

    case 356:
        return 3;               // ancient lich

    case 364:
        return 2;               // shining eye

    case 369:
        return 10;              // skeletal dragon

    case 370:
        return 17;              // tentacled monstr

    case 371:
        return 20;              // sphinx

    case 376:
        return 80;              // centaur warrior

    case 377:
        return 70;              // yaktaur warrior

    case 385:
        return 15;              // eye of devastation

    case 389:
    case 390:
    case 391:
    case 392:
    case 393:
        return 30;              // mimic

    case 396:
        return 25;              // boggart

    case 397:
        return 5;               // quicksilver D

    case 398:
        return 5;               // iron D

    case 399:
        return 30;              // skeletal warrior

    case MLAVA0:
        return 500;             // "lava worm"

    case MLAVA1:
        return 500;             // "lava fish"

    case MLAVA2:
        return 500;             // "lava snake"

    case MLAVA3:
        return 500;             // "another lava thing"

    case MWATER0:
        return 500;             // "big fish"

    case MWATER1:
        return 500;             // "giant goldfish"

    case MWATER2:
        return 500;             // "electrical eel"

    case MWATER3:
        return 500;             // "jellyfish"

    case MWATER4:
        return 500;             // ""

    case MWATER5:
        return 500;             // ""

    default:
        return 0;
    }

    return 0;

}                               // end int mons_rarity




int mons_level_orc_mines(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case 5:
        mlev = 0;
        break;                  // "fungus"

    case 6:
        mlev = 0;
        break;                  // "goblin"

    case 14:
        mlev = 0;
        break;                  // "orc"

    case 28:
        mlev = 3;
        break;                  // "cyclops"

    case 30:
        mlev = 3;
        break;                  // "two-headed ogre"

    case 33:
        mlev = 1;
        break;                  // "hobgoblin"

    case 40:
        mlev = 2;
        break;                  // "ogre"

    case 45:
        mlev = 3;
        break;                  // "troll"

    case 52:
        mlev = 1;
        break;                  // "orc warrior"

    case 54:
        mlev = 3;
        break;                  // "orc wizard"

    case 55:
        mlev = 3;
        break;                  // "orc knight"

    case 103:
        mlev = 3;
        break;                  //strcat(gmon_name, "orc sorceror"); break; break;

    case 112:
        mlev = 1;
        break;                  //strcat(gmon_name, "orc priest"); break; break;

    case 113:
        mlev = 3;
        break;                  //strcat(gmon_name, "orc high priest"); break; break;

    case 115:
        mlev = 2;
        break;                  // gnoll

    case 135:
        mlev = 3;
        break;                  //strcat(gmon_name, "rock troll"); break; break;

    case 136:
        mlev = 3;
        break;                  //strcat(gmon_name, "stone giant"); break; break; break; // stoned giant

    case 142:
        mlev = 3;
        break;                  // Ogre mage

    case 160:
        mlev = 3;
        break;                  // iron Troll

    case 262:
        mlev = 3;
        break;                  // orc warlord

    }

    return mlev + you.branch_stairs[0] + 1;

}


int mons_rare_orc_mines(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case 5:
        mlev = 10;
        break;                  // "fungus"

    case 6:
        mlev = 30;
        break;                  // "goblin"

    case 14:
        mlev = 300;
        break;                  // "orc"

    case 28:
        mlev = 5;
        break;                  // "cyclops"

    case 30:
        mlev = 5;
        break;                  // "two-headed ogre"

    case 33:
        mlev = 20;
        break;                  // "hobgoblin"

    case 40:
        mlev = 20;
        break;                  // "ogre"

    case 45:
        mlev = 13;
        break;                  // "troll"

    case 52:
        mlev = 30;
        break;                  // "orc warrior"

    case 54:
        mlev = 10;
        break;                  // "orc wizard"

    case 55:
        mlev = 10;
        break;                  // "orc knight"

    case 103:
        mlev = 10;
        break;                  //strcat(gmon_name, "orc sorceror"); break; break;

    case 112:
        mlev = 10;
        break;                  //strcat(gmon_name, "orc priest"); break; break;

    case 113:
        mlev = 5;
        break;                  //strcat(gmon_name, "orc high priest"); break; break;

    case 115:
        mlev = 2;
        break;                  // gnoll

    case 135:
        mlev = 3;
        break;                  //strcat(gmon_name, "rock troll"); break; break;

    case 136:
        mlev = 3;
        break;                  //strcat(gmon_name, "stone giant"); break; break; break; // stoned giant

    case 142:
        mlev = 1;
        break;                  // Ogre mage

    case 160:
        mlev = 3;
        break;                  // iron Troll

    case 262:
        mlev = 2;
        break;                  // orc warlord

    }

    return mlev;

}



int mons_level_hive(int mcls)
{

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case 10:
        return you.branch_stairs[1] + 1 + 0;    // "killer bee"

    case 11:
        return you.branch_stairs[1] + 1 + 2;    // "killer bee larva"

    case 41:
        return you.branch_stairs[1] + 1 + 0;    // "plant"

    default:
        return 99;
    }
    return 50;
}                               // end mons_level(mcls)


int mons_rare_hive(int mcls)
{

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case 10:
        return 300;             // "killer bee"

    case 11:
        return 50;              // "killer bee larva"

    case 41:
        return 100;             // "plant"

    default:
        return 0;
    }

}                               // end mons_level(mcls)


int mons_level_lair(int mcls)
{

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case 1:
        return you.branch_stairs[2] + 1 + 0;    // "giant bat"

    case 5:
        return you.branch_stairs[2] + 1 + 4;    // "fungus"

    case 7:
        return you.branch_stairs[2] + 1 + 2;    // "hound"

    case 9:
        return you.branch_stairs[2] + 1 + 0;    // "jackal"

    case 17:
        return you.branch_stairs[2] + 1 + 0;    // "rat"

    case 18:
        return you.branch_stairs[2] + 1 + 4;    // "scorpion"

    case 22:
        return you.branch_stairs[2] + 1 + 3;    // "worm"

    case 24:
        return you.branch_stairs[2] + 1 + 5;    // "yellow wasp"

    case 27:
        return you.branch_stairs[2] + 1 + 5;    // "giant beetle"

    case 32:
        return you.branch_stairs[2] + 1 + 6;    // "giant spore"

    case 41:
        return you.branch_stairs[2] + 1 + 5;    // "plant"

    case 44:
        return you.branch_stairs[2] + 1 + 4;    // "snake"

    case 50:
        return you.branch_stairs[2] + 1 + 6;    // "yak"

    case 57:
        return you.branch_stairs[2] + 1 + 7;    // "wyvern"

    case 61:
        return you.branch_stairs[2] + 1 + 7;    // "oklob plant"

    case 62:
        return you.branch_stairs[2] + 1 + 6;    // "wolf spider"

    case 66:
        return you.branch_stairs[2] + 1 + 5;    // "butterfly"

    case 67:
        return you.branch_stairs[2] + 1 + 8;    // "wandering mushroom"

    case 69:
        return you.branch_stairs[2] + 1 + 7;    // "brain worm"

    case 71:
        return you.branch_stairs[2] + 1 + 9;    // "boulder beetle"

    case 100:
        return you.branch_stairs[2] + 1 + 4;    // giant mite"); break;

    case 101:
        return you.branch_stairs[2] + 1 + 6;    // steam dragon"); break;

    case 104:
        return you.branch_stairs[2] + 1 + 5;    // hippogriff"); break;

    case 105:
        return you.branch_stairs[2] + 1 + 8;    // griffon"); break;

    case 106:
        return you.branch_stairs[2] + 1 + 7;    // hydra"); break;

    case 128:
        return you.branch_stairs[2] + 1 + 5;    // brown snake"); break;

    case 129:
        return you.branch_stairs[2] + 1 + 4;    // giant lizard"); break;

    case 134:
        return you.branch_stairs[2] + 1 + 9;    // death yak"); break;

    case 138:
        return you.branch_stairs[2] + 1 + 7;    // bumblebee

    case 139:
        return you.branch_stairs[2] + 1 + 8;    // red back

    case 143:
        return you.branch_stairs[2] + 1 + 9;    // spiny worm

    case 148:
        return you.branch_stairs[2] + 1 + 8;    // lindworm

    case 149:
        return you.branch_stairs[2] + 1 + 8;    // elephant slug

    case 150:
        return you.branch_stairs[2] + 1 + 5;    // war dog

    case 151:
        return you.branch_stairs[2] + 1 + 2;    // grey rat

    case 152:
        return you.branch_stairs[2] + 1 + 4;    // green rat

    case 153:
        return you.branch_stairs[2] + 1 + 6;    // orange rat

    case 154:
        return you.branch_stairs[2] + 1 + 7;    // black snake

    case 155:
        return you.branch_stairs[2] + 1 + 6;    // sheep

    case 159:
        return you.branch_stairs[2] + 1 + 1;    // giant centipede

    case 164:
        return you.branch_stairs[2] + 1 + 7;    // fire drake

    case 177:
        return you.branch_stairs[2] + 1 + 2;    // giant frog

    case 178:
        return you.branch_stairs[2] + 1 + 4;    // giant brown frog

    case 179:
        return you.branch_stairs[2] + 1 + 5;    // spiny frog

    case 180:
        return you.branch_stairs[2] + 1 + 6;    // blink frog

    case 242:
        return you.branch_stairs[2] + 1 + 5;    // giant slug

    case 243:
        return you.branch_stairs[2] + 1 + 6;    // giant snail

    case 246:
        return you.branch_stairs[2] + 1 + 9;    // boring beetle

    default:
        return 99;


    }
    return 50;
}                               // end mons_level(mcls)


int mons_rare_lair(int mcls)
{

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case 1:
        return 99;              // "giant bat"

    case 5:
        return 50;              // "fungus"

    case 7:
        return 60;              // "hound"

    case 9:
        return 70;              // "jackal"

    case 17:
        return 200;             // "rat"

    case 18:
        return 10;              // "scorpion"

    case 22:
        return 30;              // "worm"

    case 24:
        return 5;               // "yellow wasp"

    case 27:
        return 10;              // "giant beetle"

    case 32:
        return 2;               // "giant spore"

    case 41:
        return 80;              // "plant"

    case 44:
        return 80;              // "snake"

    case 50:
        return 50;              // "yak"

    case 57:
        return 20;              // "wyvern"

    case 61:
        return 10;              // "oklob plant"

    case 62:
        return 6;               // "wolf spider"

    case 66:
        return 5;               // "butterfly"

    case 67:
        return 8;               // "wandering mushroom"

    case 69:
        return 7;               // "brain worm"

    case 71:
        return 20;              // "boulder beetle"

    case 100:
        return 30;              // giant mite"); break;

    case 101:
        return 10;              // steam dragon"); break;

    case 104:
        return 50;              // hippogriff"); break;

    case 105:
        return 30;              // griffon"); break;

    case 106:
        return 20;              // hydra"); break;

    case 128:
        return 90;              // brown snake"); break;

    case 129:
        return 90;              // giant lizard"); break;

    case 134:
        return 30;              // death yak"); break;

    case 138:
        return 7;               // bumblebee

    case 139:
        return 8;               // red back

    case 143:
        return 9;               // spiny worm

    case 148:
        return 10;              // lindworm

    case 149:
        return 30;              // elephant slug

    case 150:
        return 35;              // war dog

    case 151:
        return 99;              // grey rat

    case 152:
        return 64;              // green rat

    case 153:
        return 10;              // orange rat

    case 154:
        return 47;              // black snake

    case 155:
        return 36;              // sheep

    case 159:
        return 50;              // giant centipede

    case 164:
        return 36;              // fire drake

    case 177:
        return 99;              // giant frog

    case 178:
        return 99;              // giant brown frog

    case 179:
        return 75;              // spiny frog

    case 180:
        return 45;              // blink frog

    case 242:
        return 55;              // giant slug

    case 243:
        return 56;              // giant snail

    case 246:
        return 29;              // boring beetle

    default:
        return 0;


    }
    return 0;
}                               // end mons_level(mcls)



int mons_level_slime_pits(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case 35:
        mlev = 1;
        break;                  // "jelly"

    case 76:
        mlev = 2;
        break;                  // "slime thing"

    case 131:
        mlev = 4;
        break;                  // pulsating mass

    case 241:
        mlev = 3;
        break;                  // giant amoeba

    case 275:
        mlev = 3;
        break;                  // ooze

    case 276:
        mlev = 3;
        break;                  // azure jelly

    case 277:
        mlev = 5;
        break;                  // death ooze

    case 278:
        mlev = 3;
        break;                  // acid blob

    case 370:
        return 5;               // tentacled monstr

    }

    return mlev + you.branch_stairs[3] + 1;

}

int mons_rare_slime_pits(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case 35:
        mlev = 300;
        break;                  // "jelly"

    case 76:
        mlev = 200;
        break;                  // "slime thing"

    case 131:
        mlev = 20;
        break;                  // pulsating mass

    case 241:
        mlev = 100;
        break;                  // giant amoeba

    case 275:
        mlev = 150;
        break;                  // ooze

    case 276:
        mlev = 30;
        break;                  // azure jelly

    case 277:
        mlev = 20;
        break;                  // death ooze

    case 278:
        mlev = 100;
        break;                  // acid blob

    case 370:
        mlev = 2;
        break;                  // tentacled monstr

    }

    return mlev;

}


int mons_level_crypt(int mcls)
{

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */
    int mlev = 0;


    switch (mcls)
    {
    case 13:
        mlev = 2;
        break;                  // "necrophage"

    case 15:
        mlev = 1;
        break;                  // "phantom"

    case 23:
        mlev = 4;
        break;                  // "abomination"

    case 25:
        mlev = 0;
        break;                  // "zombie"

    case 37:
        mlev = 5;
        break;                  // "lich"

    case 38:
        mlev = 4;
        break;                  // "mummy"

    case 47:
        mlev = 4;
        break;                  // "vampire"

    case 48:
        mlev = 3;
        break;                  // "wraith"

    case 49:
        mlev = 4;
        break;                  // "abomination"

    case 51:
        mlev = 1;
        break;                  // "zombie"

    case 60:
        mlev = 1;
        break;                  // "wight"

    case 63:
        mlev = 2;
        break;                  // "shadow"

    case 64:
        mlev = 2;
        break;                  // "hungry ghost"

    case 72:
        mlev = 3;
        break;                  // "flying skull"

    case 83:
        mlev = 5;
        break;                  // "reaper"

    case 107:
    case 108:
        mlev = 1;
        break;                  // "skeleton"

    case 110:
        mlev = 3;
        break;                  //strcat(gmon_name, "necromancer");

    case 130:
        mlev = 4;
        break;                  //strcat(gmon_name, "spectre");

    case 131:
        mlev = 3;
        break;                  // pulsating mass

    case 137:
        mlev = 3;
        break;                  // flayed ghost

    case 156:
        mlev = 3;
        break;                  // ghoul

    case 240:
        mlev = 4;
        break;                  // shadow wraith

    case 356:
        mlev = 5;
        break;                  // ancient lich

    case 362:
        mlev = 4;
        break;                  // vampire knight

    case 363:
        mlev = 4;
        break;                  // vampire mage

    case 369:
        mlev = 4;
        break;                  // skeletal dragon

    case 372:
        mlev = 3;
        break;                  // rotting hulk

    case 399:
        mlev = 2;
        break;                  // skeletal warrior

    default:
        mlev = 99;


    }
    return mlev + you.branch_stairs[5] + 1;
}                               // end mons_level(mcls)


int mons_rare_crypt(int mcls)
{
    int mlev = 0;

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case 13:
        mlev = 50;
        break;                  // "necrophage"

    case 15:
        mlev = 21;
        break;                  // "phantom"

    case 23:
        mlev = 5;
        break;                  // "abomination"

    case 25:
        mlev = 410;
        break;                  // "zombie"

    case 37:
        mlev = 5;
        break;                  // "lich"

    case 38:
        mlev = 24;
        break;                  // "mummy"

    case 47:
        mlev = 21;
        break;                  // "vampire"

    case 48:
        mlev = 33;
        break;                  // "wraith"

    case 49:
        mlev = 4;
        break;                  // "abomination"

    case 51:
        mlev = 300;
        break;                  // "zombie"

    case 60:
        mlev = 35;
        break;                  // "wight"

    case 63:
        mlev = 30;
        break;                  // "shadow"

    case 64:
        mlev = 12;
        break;                  // "hungry ghost"

    case 72:
        mlev = 13;
        break;                  // "flying skull"

    case 83:
        mlev = 5;
        break;                  // "reaper"

    case 107:
    case 108:
        mlev = 410;
        break;                  // "skeleton"

    case 110:
        mlev = 25;
        break;                  //strcat(gmon_name, "necromancer");

    case 130:
        mlev = 14;
        break;                  //strcat(gmon_name, "spectre");

    case 131:
        mlev = 3;
        break;                  // pulsating mass

    case 137:
        mlev = 13;
        break;                  // flayed ghost

    case 156:
        mlev = 25;
        break;                  // ghoul

    case 240:
        mlev = 10;
        break;                  // shadow wraith

    case 356:
        mlev = 8;
        break;                  // ancient lich

    case 362:
        mlev = 20;
        break;                  // vampire knight

    case 363:
        mlev = 20;
        break;                  // vampire mage

    case 369:
        mlev = 24;
        break;                  // skeletal dragon

    case 372:
        mlev = 17;
        break;                  // rotting hulk

    case 399:
        mlev = 75;
        break;                  // skeletal warrior

    default:
        mlev = 0;


    }
    return mlev;
}                               // end mons_level(mcls)

int mons_level_hall_ob(int mcls)
{

    switch (mcls)
    {
    case 144:
        return you.branch_stairs[6] + 1;        // dancing weapon

    }

    return 0;

}

int mons_rare_hall_ob(int mcls)
{

    switch (mcls)
    {
    case 144:
        return 1000;            // dancing weapon

    }

    return 0;

}


int mons_level_snake(int mcls)
{
    int mlev = 0;

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case 44:
        mlev = 1;
        break;                  // snake

    case 128:
        mlev = 2;
        break;                  // brown snake

    case 154:
        mlev = 2;
        break;                  // black snake

    case 166:
        mlev = 2;
        break;                  // yellow snake

    case 167:
        mlev = 2;
        break;                  // grey snake

    case 161:
        mlev = 3;
        break;                  // naga

    case 260:
        mlev = 5;
        break;                  // naga mage

    case 261:
        mlev = 4;
        break;                  // naga warrior

    default:
        mlev = 99;


    }

    return mlev + you.branch_stairs[9] + 1;

}

int mons_rare_snake(int mcls)
{
    int mlev = 0;

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case 44:
        mlev = 99;
        break;                  // snake

    case 128:
        mlev = 99;
        break;                  // brown snake

    case 154:
        mlev = 72;
        break;                  // black snake

    case 166:
        mlev = 32;
        break;                  // yellow snake

    case 167:
        mlev = 32;
        break;                  // grey snake

    case 161:
        mlev = 53;
        break;                  // naga

    case 260:
        mlev = 15;
        break;                  // naga mage

    case 261:
        mlev = 34;
        break;                  // naga warrior

    default:
        mlev = 0;


    }

    return mlev;

}

int mons_level_elf(int mcls)
{
    int mlev = 0;

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case 5:
        mlev = 3;
        break;                  // fungus

    case 99:
        mlev = 4;
        break;                  // shapeshifter

    case 263:
        mlev = 1;
        break;                  // deep elf soldier

    case 264:
        mlev = 1;
        break;                  // deep elf fighter

    case 265:
        mlev = 4;
        break;                  // deep elf knight

    case 266:
        mlev = 2;
        break;                  // deep elf mage

    case 267:
        mlev = 2;
        break;                  // deep elf summoner

    case 268:
        mlev = 3;
        break;                  // deep elf conjurer

    case 269:
        mlev = 4;
        break;                  // deep elf priest

    case 270:
        mlev = 7;
        break;                  // deep elf high priest

    case 271:
        mlev = 7;
        break;                  // deep elf demonologist

    case 272:
        mlev = 7;
        break;                  // deep elf annihilator

    case 273:
        mlev = 7;
        break;                  // deep elf sorceror

    case 274:
        mlev = 7;
        break;                  // deep elf death mage


    case 14:
        mlev = 1;
        break;                  // "orc"

    case 52:
        mlev = 1;
        break;                  // "orc warrior"

    case 54:
        mlev = 2;
        break;                  // "orc wizard"

    case 55:
        mlev = 3;
        break;                  // "orc knight"

    case 103:
        mlev = 4;
        break;                  //strcat(gmon_name, "orc sorceror"); break; break;

    case 112:
        mlev = 5;
        break;                  //strcat(gmon_name, "orc priest"); break; break;

    case 113:
        mlev = 5;
        break;                  //strcat(gmon_name, "orc high priest"); break; break;

    default:
        mlev = 99;
        break;

    }

    return mlev + you.branch_stairs[10] + 1;

}

int mons_rare_elf(int mcls)
{
    int mlev = 0;

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */

    switch (mcls)
    {
    case 5:
        mlev = 300;
        break;                  // fungus

    case 99:
        return 25;              // shapeshifter

    case 263:
        mlev = 100;
        break;                  // deep elf soldier

    case 264:
        mlev = 100;
        break;                  // deep elf fighter

    case 265:
        mlev = 80;
        break;                  // deep elf knight

    case 266:
        mlev = 100;
        break;                  // deep elf mage

    case 267:
        mlev = 72;
        break;                  // deep elf summoner

    case 268:
        mlev = 63;
        break;                  // deep elf conjurer

    case 269:
        mlev = 44;
        break;                  // deep elf priest

    case 270:
        mlev = 10;
        break;                  // deep elf high priest

    case 271:
        mlev = 17;
        break;                  // deep elf demonologist

    case 272:
        mlev = 13;
        break;                  // deep elf annihilator

    case 273:
        mlev = 17;
        break;                  // deep elf sorceror

    case 274:
        mlev = 13;
        break;                  // deep elf death mage

    case 14:
        mlev = 20;
        break;                  // "orc"

    case 52:
        mlev = 11;
        break;                  // "orc warrior"

    case 54:
        mlev = 13;
        break;                  // "orc wizard"

    case 55:
        mlev = 5;
        break;                  // "orc knight"

    case 103:
        mlev = 10;
        break;                  //strcat(gmon_name, "orc sorceror"); break; break;

    case 112:
        mlev = 5;
        break;                  //strcat(gmon_name, "orc priest"); break; break;

    case 113:
        mlev = 5;
        break;                  //strcat(gmon_name, "orc high priest"); break; break;

    default:
        mlev = 0;
        break;

    }

    return mlev;

}


int mons_level_tomb(int mcls)
{

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */
    int mlev = 0;


    switch (mcls)
    {
    case 25:
        mlev = 0;
        break;                  // "zombie"

    case 37:
        mlev = 3;
        break;                  // "lich"

    case 38:
        mlev = 1;
        break;                  // "mummy"

    case 51:
        mlev = 1;
        break;                  // "zombie"

    case 72:
        mlev = 2;
        break;                  // "flying skull"

    case 107:
    case 108:
        mlev = 1;
        break;                  // "skeleton"

    case 356:
        mlev = 3;
        break;                  // ancient lich

    case 373:
        mlev = 2;
        break;                  // "guardian mummy"

    case 375:
        mlev = 3;
        break;                  // "mummy priest"

    default:
        mlev = 99;


    }
    return mlev + you.branch_stairs[5] + 1;
}                               // end mons_level(mcls)


int mons_rare_tomb(int mcls)
{

/*
   Note: When adding new branches or levels above 50, must change
   pre-game deletion routine in new_game in newgame.cc.
 */
    int mlev = 0;


    switch (mcls)
    {
    case 25:
        mlev = 20;
        break;                  // "zombie"

    case 37:
        mlev = 4;
        break;                  // "lich"

    case 38:
        mlev = 300;
        break;                  // "mummy"

    case 51:
        mlev = 21;
        break;                  // "zombie"

    case 72:
        mlev = 33;
        break;                  // "flying skull"

    case 107:
    case 108:
        mlev = 21;
        break;                  // "skeleton"

    case 356:
        mlev = 2;
        break;                  // ancient lich

    case 373:
        mlev = 202;
        break;                  // "guardian mummy"

    case 375:
        mlev = 40;
        break;                  // "mummy priest"

    default:
        mlev = 0;


    }
    return mlev;
}                               // end mons_level(mcls)


int mons_level_swamp(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case 1:
        mlev = 1;
        break;                  // "giant bat"

    case 5:
        mlev = 2;
        break;                  // "fungus"

    case 13:
        mlev = 2;
        break;                  // "necrophage"

    case 15:
        mlev = 3;
        break;                  // "phantom"

    case 17:
        mlev = 1;
        break;                  // "rat"

    case 20:
        mlev = 3;
        break;                  // "ugly thing"

    case 22:
        mlev = 1;
        break;                  // "worm"

    case 35:
        mlev = 3;
        break;                  // "jelly"

    case 41:
        mlev = 2;
        break;                  // "plant"

    case 44:
        mlev = 2;
        break;                  // "snake"

    case 64:
        mlev = 3;
        break;                  // "hungry ghost"

    case 66:
        mlev = 2;
        break;                  // "butterfly"

    case 76:
        mlev = 4;
        break;                  // "slime thing"

    case 102:
        mlev = 4;
        break;                  // very ugly thing

    case 106:
        mlev = 4;
        break;                  // hydra

    case 128:
        mlev = 3;
        break;                  // brown snake

    case 129:
        mlev = 2;
        break;                  // giant lizard

    case 140:
        mlev = 3;
        break;                  // wisp

    case 141:
        mlev = 4;
        break;                  // vapour

    case 158:
        mlev = 2;
        break;                  // giant mosquito

    case 169:
        mlev = 1;
        break;                  // giant blowfly

    case 170:
        mlev = 3;
        break;                  // red wasp

    case 171:
        mlev = 3;
        break;                  // swamp dragon

    case 172:
        mlev = 1;
        break;                  // swamp drake

    case 177:
        mlev = 1;
        break;                  // giant frog

    case 178:
        mlev = 2;
        break;                  // giant brown frog

    case 179:
        mlev = 3;
        break;                  // spiny frog

    case 180:
        mlev = 4;
        break;                  // blink frog

    case 241:
        mlev = 1;
        break;                  // giant amoeba

    case 242:
        mlev = 1;
        break;                  // giant slug

    case 243:
        mlev = 2;
        break;                  // giant snail

    case 370:
        mlev = 4;
        break;                  // tentacled monstr

    default:
        mlev = 99;


    }
    return mlev + you.branch_stairs[12] + 1;
}                               // end mons_level_swamp(mcls)

int mons_rare_swamp(int mcls)
{

    int mlev = 0;

    switch (mcls)
    {
    case 1:
        mlev = 99;
        break;                  // "giant bat"

    case 5:
        mlev = 99;
        break;                  // "fungus"

    case 13:
        mlev = 12;
        break;                  // "necrophage"

    case 15:
        mlev = 13;
        break;                  // "phantom"

    case 17:
        mlev = 61;
        break;                  // "rat"

    case 20:
        mlev = 13;
        break;                  // "ugly thing"

    case 22:
        mlev = 21;
        break;                  // "worm"

    case 35:
        mlev = 23;
        break;                  // "jelly"

    case 41:
        mlev = 200;
        break;                  // "plant"

    case 44:
        mlev = 52;
        break;                  // "snake"

    case 64:
        mlev = 13;
        break;                  // "hungry ghost"

    case 66:
        mlev = 22;
        break;                  // "butterfly"

    case 76:
        mlev = 54;
        break;                  // "slime thing"

    case 102:
        mlev = 14;
        break;                  // very ugly thing

    case 106:
        mlev = 54;
        break;                  // hydra

    case 128:
        mlev = 33;
        break;                  // brown snake

    case 129:
        mlev = 22;
        break;                  // giant lizard

    case 140:
        mlev = 43;
        break;                  // wisp

    case 141:
        mlev = 14;
        break;                  // vapour

    case 158:
        mlev = 250;
        break;                  // giant mosquito

    case 169:
        mlev = 100;
        break;                  // giant blowfly

    case 170:
        mlev = 30;
        break;                  // red wasp

    case 171:
        mlev = 30;
        break;                  // swamp dragon

    case 172:
        mlev = 80;
        break;                  // swamp drake

    case 177:
        mlev = 150;
        break;                  // giant frog

    case 178:
        mlev = 90;
        break;                  // giant brown frog

    case 179:
        mlev = 30;
        break;                  // spiny frog

    case 180:
        mlev = 10;
        break;                  // blink frog

    case 241:
        mlev = 10;
        break;                  // giant amoeba

    case 242:
        mlev = 10;
        break;                  // giant slug

    case 243:
        mlev = 10;
        break;                  // giant snail

    case 370:
        mlev = 5;
        break;                  // tentacled monstr

    default:
        mlev = 0;


    }
    return mlev;
}                               // end mons_rare_swamp(mcls)


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
}

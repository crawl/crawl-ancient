#include "colours.h"
#include <string.h>

#define MLAVA0 220
#define MLAVA1 221
#define MLAVA2 222
#define MLAVA3 223
#define MLAVA4 224
#define MLAVA5 225

#define MWATER0 230
#define MWATER1 231
#define MWATER2 232
#define MWATER3 233
#define MWATER4 234
#define MWATER5 235


/*#define
#define
#define
#define*/






int mondamage(int mc, int rt);

void mon_init(char gmon_use [400], char mcolour [400]);

int mon_resist_mag(char mc, char mhd);

int mons_zombie_size(int mclass);


int mondamage(int mc, int rt)
{


switch(mc)
{

case 0: // giant ant
switch(rt)
{
        case 0: return 8;
        default: return 0;
}
break;

case 1: // giant bat
switch(rt)
{
        case 0: return 1;
        default: return 0;
}
break;

case 2: // centaur
switch(rt)
{
        case 0: return 10;
        default: return 0;
}
break;

case 3: // devil
switch(rt)
{
        case 0: return 18;
        default: return 0;
}
break;

case 4: // ettin?
switch(rt)
{
        case 0: return 18;
        case 1: return 12;
        default: return 0;
}
break;

case 5: // fungus
switch(rt)
{
        //case 0: return 8;
        default: return 0;
}
break;

case 6: // goblin
switch(rt)
{
        case 0: return 4;
        default: return 0;
}
break;

case 7: // hound
switch(rt)
{
        case 0: return 6;
        default: return 0;
}
break;

case 8: // imp
switch(rt)
{
        case 0: return 4;
        default: return 0;
}
break;

case 9: // jackal?
switch(rt)
{
        case 0: return 3;
        default: return 0;
}
break;

case 10: // killer bee
switch(rt)
{
        case 0: return 10;
        default: return 0;
}
break;

case 11: // killer bee larva?
switch(rt)
{
        case 0: return 3;
        default: return 0;
}
break;

case 12: // manticore?
switch(rt)
{
        case 0: return 14;
        case 1: return 8;
        case 2: return 8;
        default: return 0;
}
break;

case 13: // necrophage
switch(rt)
{
        case 0: return 8;
        default: return 0;
}

case 14: // orc
switch(rt)
{
        case 0: return 5;
        default: return 0;
}

case 15: // phantom
switch(rt)
{
        case 0: return 10;
        default: return 0;
}

case 16: // quasit
switch(rt)
{
        case 0: return 3;
        case 1: return 2;
        case 2: return 2;
        default: return 0;
}

case 17: // rat
switch(rt)
{
        case 0: return 3;
        default: return 0;
}

case 18: // scorpion
switch(rt)
{
        case 0: return 10;
        default: return 0;
}

case 19: // tunneling worm
switch(rt)
{
        case 0: return 50;
        default: return 0;
}

case 20: // ugly thing
switch(rt)
{
        case 0: return 12;
        default: return 0;
}

case 21: // v?
switch(rt)
{
        case 0: return 30;
        default: return 0;
}

case 22: // worm
switch(rt)
{
        case 0: return 12;
        default: return 0;
}

case 23: // x?
switch(rt)
{
        case 0: return 3;
        default: return 0;
}

case 24: // yellow wasp
switch(rt)
{
        case 0: return 13;
        default: return 0;
}

case 25: // zombie?????
switch(rt)
{
        case 0: return 10;  // This is adjusted in mons_attack
        default: return 0;
}

case 26: // Angel
switch(rt)
{
        case 0: return 20;
        default: return 0;
}

case 27: // giant beetle
switch(rt)
{
        case 0: return 20;
        default: return 0;
}

case 28: // Cyclops
switch(rt)
{
        case 0: return 35;
        default: return 0;
}

case 29: // Dragon
switch(rt)
{
        case 0: return 15;
        case 1: return 10;
        case 2: return 10;
        default: return 0;
}

case 30: // two-headed Ogre
switch(rt)
{
        case 0: return 20;
        case 1: return 15;
        default: return 0;
}

case 31: // Fiend
switch(rt)
{
        case 0: return 40;
        case 1: return 30;
        case 2: return 30;
        default: return 0;
}
/*
case 32: // E?
switch(rt)
{
        case 0: return 3;
        default: return 0;
}
*/
case 32: // Giant spore
switch(rt)
{
        case 0: return 1;
        default: return 0;
}

case 33: // Hobgoblin
switch(rt)
{
        case 0: return 5;
        default: return 0;
}

case 34: // Ice beast
switch(rt)
{
        case 0: return 5;
        default: return 0;
}

case 35: // Jelly
switch(rt)
{
        case 0: return 8; //20;
        default: return 0;
}

case 36: // Kobold
switch(rt)
{
        case 0: return 4;
        default: return 0;
}

case 37: // Lich
switch(rt)
{
        case 0: return 15;
        default: return 0;
}

case 38: // Mummy
switch(rt)
{
        case 0: return 10;
        default: return 0;
}

case 39: // Naga
switch(rt)
{
        case 0: return 15;
        default: return 0;
}


case 40: // Ogre
switch(rt)
{
        case 0: return 15;
        default: return 0;
}

case 41: // Plant?
switch(rt)
{
        case 0: return 0;
        default: return 0;
}

case 42: // Queen bee
switch(rt)
{
        case 0: return 20;
        default: return 0;
}

case 43: // Rakshasa
switch(rt)
{
        case 0: return 15;
        default: return 0;
}

case 44: // Snake
switch(rt)
{
        case 0: return 5;
        default: return 0;
}

case 45: // Troll
switch(rt)
{
        case 0: return 15;
        case 1: return 10;
        case 2: return 10;
        default: return 0;
}

case 46: // U?
switch(rt)
{
        case 0: return 3;
        default: return 0;
}

case 47: // Vampire?
switch(rt)
{
        case 0: return 22;
        default: return 0;
}

case 48: // Wraith
switch(rt)
{
        case 0: return 19;
        default: return 0;
}

case 49: // Abomination
switch(rt)
{
        case 0: return 40;
        default: return 0;
}

case 50: // Yak
switch(rt)
{
        case 0: return 18;
        default: return 0;
}

case 51: // Zombie?
switch(rt)
{
        case 0: return 23; // This is adjusted in mons_attack
        default: return 0;
}

case 52: // orc warrior
switch(rt)
{
        case 0: return 10;
        default: return 0;
}

case 53: // kobold demologist
switch(rt)
{
        case 0: return 4;
        default: return 0;
}

case 54: // orc wizard
switch(rt)
{
        case 0: return 5;
        default: return 0;
}

case 55: // orc knight
switch(rt)
{
        case 0: return 14;
        default: return 0;
}

case 57: // wyvern
switch(rt)
{
   case 0: return 20;
   default: return 0;
}

case 58: // Kobold lord
switch(rt)
{
   case 0: return 7;
   default: return 0;
}

case 59: // giant eyeball
return 0;

case 60: // wight
switch(rt)
{
   case 0: return 10;
   default: return 0;
}

case 61: // oklob plant
switch(rt)
{
//   case 0: return 30;
   default: return 0;
}
//return 0;

case 62: // giant Spider
switch(rt)
{
   case 0: return 20;
   default: return 0;
}

case 63: // Shadow
switch(rt)
{
   case 0: return 5;
   default: return 0;
}

case 64: // hungry ghost
switch(rt)
{
                        case 0: return 5;
                        default: return 0;
}

case 65: // eye of draining
case 66: // butterfly
return 0;

case 67: // wandering muchroom
switch(rt)
{
   case 0: return 20;
   default: return 0;
}

case 68: // Efreet
switch(rt)
{
                        case 0: return 12;
                        default: return 0;
}

case 69: // brain worm
return 0;

case 70: // giant orange brain
switch(rt)
{
//   case 0: return 13;
   default: return 0;
}

case 71: // Boulder Beetle
switch(rt)
{
                        case 0: return 35;
                        default: return 0;
}

case 72: // Flying skull
switch(rt)
{
                        case 0: return 7;
                        default: return 0;
}

case 73: // hell hon\und
switch(rt)
{
                        case 0: return 13;
                        default: return 0;
}

case 74: // minotaur
switch(rt)
{
                        case 0: return 35;
                        default: return 0;
}

case 75: // ice dragon
switch(rt)
{
                        case 0: return 12;
   case 1: return 12;
                        case 2: return 12;
                        default: return 0;
}

case 76: // Slime thing
switch(rt)
{
                        case 0: return 17;
                        default: return 0;
}

case 77: // Blue Wraith
switch(rt)
{
                        case 0: return 19;
                        default: return 0;
}

case 78: // fake R
return 0;

case 79: // great orb of eyes
switch(rt)
{
                        case 0: return 20;
                        default: return 0;
}

case 80: // burning devil
switch(rt)
{
   case 0: return 10;
   default: return 0;
}

case 81: // rotting devil
switch(rt)
{
   case 0: return 8;
   default: return 0;
}

case 82: // yellow devil
switch(rt)
{
   case 0: return 8;
   case 1: return 8;
   default: return 0;
}

case 83: // grey devil
switch(rt)
{
   case 0: return 20;
   default: return 0;
}

case 84: // shadow devil
switch(rt)
{
   case 0: return 15;
   default: return 0;
}

case 85: // hairy devil
switch(rt)
{
   case 0: return 9;
   case 1: return 9;
   default: return 0;
}

case 86: // ice devil
switch(rt)
{
   case 0: return 16;
   default: return 0;
}

case 87: // blue devil
switch(rt)
{
   case 0: return 21;
   default: return 0;
}


case 100: // giant mite
switch(rt)
{
   case 0: return 5;
   default: return 0;
}

case 101: // steam Dragon
switch(rt)
{
        case 0: return 8;
//      case 1: return 4;
//      case 2: return 4;
        default: return 0;
}

case 102: // very ugly thing
switch(rt)
{
        case 0: return 17;
        default: return 0;
}

case 103: // orc sorceror
switch(rt)
{
        case 0: return 7;
        default: return 0;
}

case 104: // hippogriff
switch(rt)
{
        case 0: return 10;
        case 1: return 8;
        case 2: return 8;
        default: return 0;
}

case 105: // griffon
switch(rt)
{
        case 0: return 18;
        case 1: return 10;
        case 2: return 10;
        default: return 0;
}

case 106: // hydra
switch(rt)
{
        case 0: return 10; // !
        default: return 0;
}


case MLAVA0: // lava worm
switch(rt)
{
                        case 0: return 10;
                        default: return 0;
}

case MLAVA1: // lava fish
switch(rt)
{
                        case 0: return 7;
                        default: return 0;
}

case MLAVA2: // lava snake
switch(rt)
{
                        case 0: return 5;
                        default: return 0;
}

case MWATER0: // giant fish
switch(rt)
{
                        case 0: return 5;
                        default: return 0;
}

case MWATER1: // giant goldfish
switch(rt)
{
                        case 0: return 15;
                        default: return 0;
}

case MWATER2: // electric eel
switch(rt)
{
//                      case 0: return 5;
                        default: return 0;
}

case MWATER3: // jellyfish
switch(rt)
{
                        case 0: return 1;
   case 1: return 1;
//                      case 2: return 1;
   default: return 0;
}

default: return 0;

} // end of switch

return 0;

} // end of char mondamage





int mon_resist_mag(int mc, char mhd)
{

switch(mc)
{

        case 31: return 200; // fiend
        //case 54: return 30; // orc wizard
        case 47: return 50; // vampire
        case 48: return 40; // wraith
 case 60: return 30; // wight
 case 43: return 100; // rakshasa
        case 37: return 100; // Lich
        case 19:
        case 56: return 5000; // tunneling worm & tail
 case 21: return 5000; // fire vortex
        case 8: return 50; // imp
        case 3: return 60; // devil
        case 16: return 50; // quasit
 case 70: return 80; // giant orange brain
 case 65: return 5000; // eye of draining

 case 25:
 case 51: return 5000; // zombies? After all, magres is only used for mind enchantments
 case 83: return 150; // grey devil
 case 79: return 5000; // GooE
default: return (mhd * 3);

} // end switch

}




void mon_init(char gmon_use [400], char mcolour [400])
{


//strcpy(gmon_name [0], "giant ant");
mcolour [0] = 8;
//gmon_att [0] [0] = 8;
gmon_use [0] = 0;

//strcpy(gmon_name [1], "giant bat");
mcolour [1] = 8;
//gmon_att [1] [0] = 1;
gmon_use [1] = 0;
gmon_use [1] = 0;

//strcpy(gmon_name [2], "centaur");
mcolour [2] = BROWN;
//gmon_att [2] [0] = 6;
gmon_use [2] = 3;

//strcpy(gmon_name [3], "devil");
mcolour [3] = 4;
//gmon_att [3] [0] = 16;
gmon_use [3] = 1;

//strcpy(gmon_name [4], "ettin");
mcolour [4] = 6;
//gmon_att [4] [0] = 18;
//gmon_att [4] [1] = 18;
gmon_use [4] = 1;

//strcpy(gmon_name [5], "fungus");
mcolour [5] = 7;
//gmon_att [5] [0] = 0;
gmon_use [5] = 0;

//strcpy(gmon_name [6], "goblin");
mcolour [6] = 7;
//gmon_att [6] [0] = 4;
////gmon_att [6] [1] = 6;
////gmon_att [6] [2] = 6;
gmon_use [6] = 3;

//strcpy(gmon_name [7], "hound");
mcolour [7] = 6;
//gmon_att [7] [0] = 10;
gmon_use [7] = 0;

//strcpy(gmon_name [8], "imp");
mcolour [8] = RED;
//gmon_att [8] [0] = 4;
gmon_use [8] = 1;

//strcpy(gmon_name [9], "jackal");
mcolour [9] = 14;
//gmon_att [9] [0] = 5;
gmon_use [9] = 0;

//strcpy(gmon_name [10], "killer bee");
mcolour [10] = 14;
//gmon_att [10] [0] = 8;
gmon_use [10] = 0;

//strcpy(gmon_name [11], "killer bee larva");
mcolour [11] = 7;
//gmon_att [11] [0] = 3;
gmon_use [11] = 0;

//strcpy(gmon_name [12], "manticore");
mcolour [12] = 6;
//gmon_att [12] [0] = 10;
//gmon_att [12] [1] = 8;
//gmon_att [12] [2] = 8;
gmon_use [12] = 1;

//strcpy(gmon_name [13], "necrophage"); // don't know about this one.
mcolour [13] = DARKGRAY;
//gmon_att [13] [0] = 10;
gmon_use [13] = 1;

//strcpy(gmon_name [14], "orc");
mcolour [14] = 12;
//gmon_att [14] [0] = 5;
gmon_use [14] = 1;
gmon_use [14] = 3;

//strcpy(gmon_name [15], "phantom");
mcolour [15] = BLUE;
//gmon_att [15] [0] = 5;
gmon_use [15] = 1;

//strcpy(gmon_name [16], "quasit");
mcolour [16] = 7;
//gmon_att [16] [0] = 3;
//gmon_att [16] [1] = 2;
//gmon_att [16] [2] = 2;
gmon_use [16] = 0;

//strcpy(gmon_name [17], "rat");
mcolour [17] = 6;
//gmon_att [17] [0] = 3;
gmon_use [17] = 0;

//strcpy(gmon_name [18], "scorpion");
mcolour [18] = DARKGRAY;
//gmon_att [18] [0] = 6;
gmon_use [18] = 0;

//strcpy(gmon_name [19], "tunneling worm");
mcolour [19] = 12;
//gmon_att [19] [0] = 25;
gmon_use [19] = 0;

//strcpy(gmon_name [20], "ugly thing
mcolour [20] = BROWN;
//gmon_att [20] [0] = 15;
gmon_use [20] = 1;

//strcpy(gmon_name [21], "vortex");
mcolour [21] = RED;
//gmon_att [21] [0] = 3;
gmon_use [21] = 0;

//strcpy(gmon_name [22], "worm");
mcolour [22] = LIGHTRED;
//gmon_att [22] [0] = 10;
gmon_use [22] = 0;

//strcpy(gmon_name [23], "xax");
mcolour [23] = 13;
//gmon_att [23] [0] = 5;
gmon_use [23] = 0;

//strcpy(gmon_name [24], "yellow wasp");
mcolour [24] = 14;
//gmon_att [24] [0] = 0;
gmon_use [24] = 0;

//strcpy(gmon_name [25], "small zombie");
mcolour [25] = 6;
//gmon_att [25] [0] = 15;
gmon_use [25] = 1;

//strcpy(gmon_name [26], "angel");
mcolour [26] = WHITE;
//gmon_att [26] [0] = 15;
gmon_use [26] = 3;

//strcpy(gmon_name [27], "giant beetle");
mcolour [27] = 8;
//gmon_att [27] [0] = 25;
gmon_use [27] = 0;

//strcpy(gmon_name [28], "cyclops");
mcolour [28] = 6;
//gmon_att [28] [0] = 35;
gmon_use [28] = 1;

//strcpy(gmon_name [29], "dragon");
mcolour [29] = GREEN;
//gmon_att [29] [0] = 15;
//gmon_att [29] [1] = 7;
//gmon_att [29] [2] = 7;
gmon_use [29] = 1;

//strcpy(gmon_name [30], "two-headed Ogre");
mcolour [30] = LIGHTRED;
gmon_use [30] = 1;

//strcpy(gmon_name [31], "pit fiend");
mcolour [31] = RED;
//gmon_att [31] [0] = 40;
//gmon_att [31] [1] = 30;
//gmon_att [31] [2] = 30;
gmon_use [31] = 1;

//strcpy(gmon_name [32], "giant spore");
mcolour [32] = GREEN;
//gmon_att [32] [0] = 0;
////gmon_att [32] [1] = 7;
////gmon_att [32] [2] = 7;
////gmon_att [32] [3] = 4;
//gmon_use [32] = 1;
gmon_use [32] = 0;

//strcpy(gmon_name [33], "hobgoblin");
mcolour [33] = 6;
//gmon_att [33] [0] = 6;
gmon_use [33] = 3;

//strcpy(gmon_name [34], "ice beast");
mcolour [34] = WHITE;
gmon_use [34] = 0;

//strcpy(gmon_name [35], "jelly");
mcolour [35] = LIGHTRED;
//gmon_att [35] [0] = 10;
gmon_use [35] = 0;

//strcpy(gmon_name [36], "kobold");
mcolour [36] = 6;
//gmon_att [36] [0] = 4;
gmon_use [36] = 3;

//strcpy(gmon_name [37], "lich");
mcolour [37] = 15;
//gmon_att [37] [0] = 10;
gmon_use [37] = 1;

//strcpy(gmon_name [38], "mummy");
mcolour [38] = 15;
//gmon_att [38] [0] = 8;
gmon_use [38] = 1;

//strcpy(gmon_name [39], "naga");
mcolour [39] = LIGHTGREEN;
//gmon_att [39] [0] = 6;
gmon_use [39] = 1;

//strcpy(gmon_name [40], "ogre");
mcolour [40] = 6;
//gmon_att [40] [0] = 20;
gmon_use [40] = 1;

//strcpy(gmon_name [41], "plant");
mcolour [41] = 2;
gmon_use [41] = 0;

//strcpy(gmon_name [42], "queen bee");
mcolour [42] = YELLOW;
//gmon_att [42] [0] = 13;
gmon_use [42] = 0;

//strcpy(gmon_name [43], "raksasha");
mcolour [43] = 14;
//gmon_att [43] [0] = 7;
gmon_use [43] = 1;

//strcpy(gmon_name [44], "Snake");
mcolour [44] = GREEN;
//gmon_att [44] [0] = 11;
gmon_use [44] = 0;

//strcpy(gmon_name [45], "troll");
mcolour [45] = 6;
//gmon_att [45] [0] = 15;
//gmon_att [45] [1] = 10;
//gmon_att [45] [2] = 10;
gmon_use [45] = 1;

//strcpy(gmon_name [46], "unseen horror");
mcolour [46] = MAGENTA;
//gmon_att [46] [0] = 3;
gmon_use [46] = 0;

//strcpy(gmon_name [47], "vampire");
mcolour [47] = RED;
//gmon_att [47] [0] = 8;
gmon_use [47] = 1;

//strcpy(gmon_name [48], "wraith");
mcolour [48] = DARKGRAY;
//gmon_att [48] [0] = 7;
gmon_use [48] = 1;

//strcpy(gmon_name [49], "xoryx");
mcolour [49] = 6;
//gmon_att [49] [0] = 20;
//gmon_att [49] [1] = 15;
//gmon_att [49] [2] = 15;
gmon_use [49] = 0;

//strcpy(gmon_name [50], "yak");
mcolour [50] = BROWN;
//gmon_att [50] [0] = 9;
gmon_use [50] = 0;

//strcpy(gmon_name [51], "large zombie");
mcolour [51] = BROWN;
//gmon_att [51] [0] = 20;
gmon_use [51] = 1;

//strcpy(gmon_name [52], "orc warrior");
mcolour [52] = 14;
//gmon_att [52] [0] = 12;
gmon_use [52] = 3;

//strcpy(gmon_name [53], "kobold demonologist");
mcolour [53] = 5;
//gmon_att [53] [0] = 4;
gmon_use [53] = 3;

//strcpy(gmon_name [54], "orc wizard");
mcolour [54] = MAGENTA;
//gmon_att [54] [0] = 5;
gmon_use [54] = 3;

// goblin wizard
mcolour [55] = LIGHTCYAN;
//gmon_att [55] [0] = 4;
gmon_use [55] = 3;

/*//strcpy(gmon_name [53], "kobold demonologist");
mcolour [53] = 5;
//gmon_att [53] [0] = 4;
gmon_use [53] = 1;

// goblin wizard
mcolour [55] = MAGENTA;
//gmon_att [55] [0] = 4;
gmon_use [55] = 1;

/*


//strcpy(gmon_name [54], "Type I demon");
mcolour [54] = 10;
gmon_att [54] [0] = 20;
gmon_att [54] [1] = 15;
gmon_att [54] [2] = 15;
gmon_use [54] = 0;

*/
/*
//strcpy(gmon_name [55], "Type II demon");
mcolour [55] = BROWN;
gmon_att [55] [0] = 35;
gmon_att [55] [1] = 35;
//gmon_att [55] [2] = 15;
gmon_use [55] = 0;
*/

//strcpy(gmon_name [56], "Type III demon");

// tunneling worm tail section
mcolour [56] = LIGHTRED;
//gmon_att [56] [0] = 50;
//gmon_att [56] [1] = 15;
//gmon_att [56] [2] = 15;
gmon_use [56] = 0;

mcolour [57] = LIGHTRED; // wyvern
gmon_use [57] = 0;

mcolour [58] = RED; // K lord
gmon_use [58] = 3;

mcolour [59] = WHITE; // Giant eyeball
gmon_use [59] = 0;

mcolour [60] = LIGHTGRAY; // Wight
gmon_use [60] = 3;

mcolour [61] = GREEN; // oklob plant
gmon_use [61] = 0;

mcolour [62] = BROWN; // wolf spider
gmon_use [62] = 0;

mcolour [63] = 0; // shadow
gmon_use [63] = 1; // ?

mcolour [64] = GREEN; // hungry ghost
gmon_use [64] = 1;

mcolour [65] = LIGHTGRAY; // eye of draining
gmon_use [65] = 0;

mcolour [66] = BLACK; // butterfly
gmon_use [66] = 0;

mcolour [67] = BROWN; // wandering mushroom
gmon_use [67] = 0;

mcolour [68] = RED; // Efreet
gmon_use [68] = 3;

mcolour [69] = LIGHTMAGENTA; // brain worm
gmon_use [69] = 0;

mcolour [70] = LIGHTRED; // giant orange brain
gmon_use [70] = 1;

mcolour [71] = LIGHTGRAY; // Blouder beetle
gmon_use [71] = 0;

mcolour [72] = WHITE; // Flying skull
gmon_use [72] = 0;

mcolour [73] = DARKGRAY; // hell hound
gmon_use [73] = 0;

mcolour [74] = LIGHTRED; //??? // minotaur
gmon_use [74] = 3;

mcolour [75] = WHITE; // ice dragon
gmon_use [75] = 1;

mcolour [76] = GREEN; // SLime thing
gmon_use [76] = 0;

mcolour [77] = LIGHTBLUE; // blue wraith
gmon_use [77] = 1;

mcolour [78] = YELLOW; // fake R
gmon_use [78] = 0;

mcolour [79] = GREEN; // great orb of eyes
gmon_use [79] = 1; // ???

mcolour [80] = RED; // burning devil
gmon_use [80] = 1;

mcolour [81] = BROWN; // rotting devil
gmon_use [81] = 1;

mcolour [82] = YELLOW; // YELLOW devil
gmon_use [82] = 1;

mcolour [83] = LIGHTGRAY; // skeletal devil
gmon_use [83] = 1;

mcolour [84] = DARKGRAY; // shadow devil
gmon_use [84] = 1;

mcolour [85] = LIGHTRED; // hairy devil
gmon_use [85] = 1;

mcolour [86] = WHITE; // ice devil
gmon_use [86] = 1;

mcolour [87] = BLUE; // blue devil
gmon_use [87] = 1;

mcolour [100] = LIGHTRED; // giant mite
gmon_use [100] = 0;

mcolour [101] = LIGHTGRAY; // Steam dragon
gmon_use [101] = 1;

mcolour [102] = RED; // very ugly thing
gmon_use [102] = 3;

mcolour [103] = DARKGRAY; // orc sorceror
gmon_use [103] = 3;

mcolour [104] = BROWN; // hippogriff
gmon_use [104] = 1;

mcolour [105] = YELLOW; // griffon
gmon_use [105] = 1;

mcolour [106] = LIGHTGREEN; // hydra
gmon_use [106] = 1;

mcolour [MLAVA0] = RED; // lava worm
gmon_use [MLAVA0] = 0;

mcolour [MLAVA1] = RED; // lava fish
gmon_use [MLAVA1] = 0;

mcolour [MLAVA2] = RED; // lava snake
gmon_use [MLAVA2] = 0;

mcolour [MWATER0] = LIGHTGREEN; // fish
gmon_use [MWATER0] = 0;

mcolour [MWATER1] = LIGHTRED; // goldfish
gmon_use [MWATER1] = 0;

mcolour [MWATER2] = LIGHTBLUE; // fish
gmon_use [MWATER2] = 0;

mcolour [MWATER3] = CYAN; // jellyfish
gmon_use [MWATER3] = 0;


}





void def_letters(char letters [52] [1])
{
        strcpy(letters [0], "a"); strcpy(letters [1], "b"); strcpy(letters [2], "c"); strcpy(letters [3], "d"); strcpy(letters [4], "e"); strcpy(letters [5], "f"); strcpy(letters [6], "g"); strcpy(letters [7], "h"); strcpy(letters [8], "i"); strcpy(letters [9], "j"); strcpy(letters [10], "k"); strcpy(letters [11], "l"); strcpy(letters [12], "m"); strcpy(letters [13], "n"); strcpy(letters [14], "o"); strcpy(letters [15], "p");
        strcpy(letters [16], "q");
        strcpy(letters [17], "r"); strcpy(letters [18], "s"); strcpy(letters [19], "t"); strcpy(letters [20], "u"); strcpy(letters [21], "v"); strcpy(letters [22], "w"); strcpy(letters [23], "x"); strcpy(letters [24], "y"); strcpy(letters [25], "z"); strcpy(letters [26], "A"); strcpy(letters [27], "B"); strcpy(letters [28], "C"); strcpy(letters [29], "D"); strcpy(letters [30], "E"); strcpy(letters [31], "F");strcpy(letters [32], "G");
        strcpy(letters [33], "H"); strcpy(letters [34], "I"); strcpy(letters [35], "J"); strcpy(letters [36], "K"); strcpy(letters [37], "L"); strcpy(letters [38], "M"); strcpy(letters [39], "N"); strcpy(letters [40], "O"); strcpy(letters [41], "P"); strcpy(letters [42], "Q"); strcpy(letters [43], "R"); strcpy(letters [44], "S"); strcpy(letters [45], "T"); strcpy(letters [46], "U"); strcpy(letters [47], "V"); strcpy(letters [48], "W");
        strcpy(letters [49], "X"); strcpy(letters [50], "Y"); strcpy(letters [51], "Z");
}

void def_properties(int property [4] [30] [3], int mass [15] [30])
{



        //strcpy(all_items [2] [0], "robe");
        property [2] [0] [0] = 1; // AC
        property [2] [0] [1] = 0; // evasion
        mass [2] [0] = 60;
        //icolour [2] [0] = 6;

        //strcpy(all_items [2] [0], "leather armour");
        property [2] [1] [0] = 2; // AC
        property [2] [1] [1] = -1; // evasion
        mass [2] [1] = 150;
        //icolour [2] [1] = 6;

        //strcpy(all_items [2] [1], "ring mail");
        property [2] [2] [0] = 3; // AC
        property [2] [2] [1] = -2; // evasion
        mass [2] [2] = 300;
        //icolour [2] [2] = 11;

        //strcpy(all_items [2] [2], "scale mail");
        property [2] [3] [0] = 4; // AC
        property [2] [3] [1] = -2; // evasion
        mass [2] [3] = 400;
        //icolour [2] [2] = 11;

        //strcpy(all_items [2] [3], "chain mail");
        property [2] [4] [0] = 5; // AC
        property [2] [4] [1] = -3; // evasion
        mass [2] [4] = 450;
        //icolour [2] [3] = 11;

        //strcpy(all_items [2] [4], "splint mail");
        property [2] [5] [0] = 6; // AC
        property [2] [5] [1] = -5; // evasion
        mass [2] [5] = 550;
        //icolour [2] [4] = 11;

        //strcpy(all_items [2] [5], "banded mail");
        property [2] [6] [0] = 6; // AC
        property [2] [6] [1] = -4; // evasion
        mass [2] [6] = 500;
        //icolour [2] [5] = 11;

        //strcpy(all_items [2] [6], "plate mail");
        property [2] [7] [0] = 7; // AC
        property [2] [7] [1] = -5; // evasion
        mass [2] [7] = 650;
        //icolour [2] [6] = 11;

        // other armour:

/*      // shield:
        property [2] [8] [0] = 1; // AC
        property [2] [8] [1] = 2; // evasion
        mass [2] [8] = 100;*/

        // shield:
        property [2] [8] [0] = 0; // AC
        property [2] [8] [1] = 0; // evasion
        mass [2] [8] = 100;

        // cloak
        property [2] [9] [0] = 1; // AC
        property [2] [9] [1] = 0; // evasion
        mass [2] [9] = 20;

        // helmet
        property [2] [10] [0] = 1; // AC
        property [2] [10] [1] = 0; // evasion
        mass [2] [10] = 80;


        // gloves
        property [2] [11] [0] = 1; // AC
        property [2] [11] [1] = 0; // evasion
        mass [2] [11] = 20;

        // boots
        property [2] [12] [0] = 1; // AC
        property [2] [12] [1] = 0; // evasion
        mass [2] [12] = 40;

        // buckler:
        property [2] [13] [0] = 0; // AC
        property [2] [13] [1] = 0; // evasion
        mass [2] [13] = 50;

        // large shield:
        property [2] [14] [0] = 0; // AC
        property [2] [14] [1] = 0; // evasion
        mass [2] [14] = 250;


int i = 0;

for (i = 0; i < 30; i ++)
{
        mass [3] [i] = 100;
        mass [4] [i] = 100;
        mass [5] [i] = 200;
        mass [6] [i] = 50;
        mass [7] [i] = 20;
        mass [8] [i] = 60;
        mass [9] [i] = 5;
   // don't know what these are, yet:
   mass [10] [i] = 100; // books
   mass [11] [i] = 130; // staves
   mass [12] [i] = 300; // the Orb!
   mass [13] [i] = 100;
   mass [14] [i] = 100;




}

// this is food, right?
mass [4] [0] = 120;
mass [4] [1] = 120;
mass [4] [2] = 40;
mass [4] [3] = 40;
mass [4] [4] = 50;
mass [4] [5] = 60;
mass [4] [6] = 60;
mass [4] [7] = 80;
mass [4] [8] = 80;
mass [4] [9] = 50;
mass [4] [10] = 50;
mass [4] [11] = 70;
mass [4] [12] = 15;
mass [4] [13] = 20;
mass [4] [14] = 40;
mass [4] [15] = 7;
mass [4] [16] = 5;
mass [4] [17] = 20;
mass [4] [18] = 50;
mass [4] [19] = 70;
mass [4] [20] = 80;
/*mass [4] [21] = 40;
mass [4] [22] = 50;
mass [4] [23] = 60;
mass [4] [24] = 60;
mass [4] [25] = 100;*/


/*
        case 0: strcat(glog , "meat ration"); break;
        case 1: strcat(glog , "bread ration"); break;
        case 2: strcat(glog , "pear"); break;
        case 3: strcat(glog , "apple"); break; // make this less common.
        case 4: strcat(glog , "choko"); break;
        case 5: strcat(glog , "honeycomb"); break;
        case 6: strcat(glog , "royal jell"); break; // maybe a joke monster of the same name? - mix something with jelly crystals?
        case 7: strcat(glog , "snozzcumber"); break;
 case 8: strcat(glog , "slice of pizza"); break;
 case 9: strcat(glog , "apricot"); break;
 case 10: strcat(glog , "orange"); break;
 case 11: strcat(glog , "banana"); break;
 case 12: strcat(glog , "strawberr"); break;
 case 13: strcat(glog , "rambutan"); break;
 case 14: strcat(glog , "lemon"); break;
 case 15: strcat(glog , "grape"); break;
 case 16: strcat(glog , "sultana"); break;
 case 17: strcat(glog , "lychee"); break;
 case 18: strcat(glog , "beef jerk"); break;
 case 19: strcat(glog , "cheese"); break;
 case 20: strcat(glog , "sausage"); break;
*/






/*
                case 3: burden += (int) 100 * inv_quant [bu]; break;
                case 4: burden += (int) 100 * inv_quant [bu]; break;
                case 5: burden += (int) 200 * inv_quant [bu]; break;
                case 6: burden += (int) 50 * inv_quant [bu]; break;
                case 7: burden += (int) 20 * inv_quant [bu]; break;
                case 8: burden += (int) 60 * inv_quant [bu]; break;
                case 9: burden += (int) 5 * inv_quant [bu]; break;
*/




/*      // weapons: blunt weapons are first to help grouping them together

        // club
        property [0] [0] [0] = 4; // damage
        property [0] [0] [1] = 3; // helps to get past evasion
        property [0] [0] [2] = 1; // helps to get past AC
        mass [0] [0] = 50;

        // mace:
        property [0] [1] [0] = 6; // damage
        property [0] [1] [1] = 3; // helps to get past evasion
        property [0] [1] [2] = 2; // helps to get past AC
        mass [0] [0] = 100;

        // flail
        property [0] [2] [0] = 7; // damage
        property [0] [2] [1] = 1; // helps to get past evasion
        property [0] [2] [2] = 3; // helps to get past AC
        mass [0] [0] = 150;

        // dagger
        property [0] [3] [0] = 3; // damage
        property [0] [3] [1] = 6; // helps to get past evasion
        property [0] [3] [2] = 1; // helps to get past AC
        mass [0] [0] = 20;

        // morningstar
        property [0] [4] [0] = 7; // damage
        property [0] [4] [1] = 3; // helps to get past evasion
        property [0] [4] [2] = 4; // helps to get past AC
        mass [0] [0] = 120;

        // short sword
        property [0] [5] [0] = 6; // damage
        property [0] [5] [1] = 5; // helps to get past evasion
        property [0] [5] [2] = 6; // helps to get past AC
        mass [0] [0] = 100;

        // long sword
        property [0] [6] [0] = 8; // damage
        property [0] [6] [1] = 3; // helps to get past evasion
        property [0] [6] [2] = 8; // helps to get past AC
        mass [0] [0] = 160;

        // great sword
        property [0] [7] [0] = 10; // damage
        property [0] [7] [1] = -2; // helps to get past evasion
        property [0] [7] [2] = 11; // helps to get past AC
        mass [0] [0] = 250;

        // scimitar
        property [0] [8] [0] = 9; // damage
        property [0] [8] [1] = -1; // helps to get past evasion
        property [0] [8] [2] = 10; // helps to get past AC
        mass [0] [0] = 170;

        // hand axe
        property [0] [9] [0] = 7; // damage
        property [0] [9] [1] = 2; // helps to get past evasion
        property [0] [9] [2] = 9; // helps to get past AC
        mass [0] [0] = 110;

        // battleaxe
        property [0] [10] [0] = 12; // damage
        property [0] [10] [1] = -3; // helps to get past evasion
        property [0] [10] [2] = 13; // helps to get past AC
        mass [0] [0] = 200;

        // spear
        property [0] [11] [0] = 4; // damage
        property [0] [11] [1] = 4; // helps to get past evasion
        property [0] [11] [2] = 2; // helps to get past AC
        mass [0] [0] = 50;

        // halberd
        property [0] [12] [0] = 9; // damage
        property [0] [12] [1] = -4; // helps to get past evasion
        property [0] [12] [2] = 10; // helps to get past AC
        mass [0] [0] = 200;

        // sling - the three properties are _not_ irrelevant here - when something hits something else (either may be you) in melee, these are used.
        property [0] [13] [0] = 1; // damage
        property [0] [13] [1] = 0; // helps to get past evasion
        property [0] [13] [2] = 0; // helps to get past AC
        mass [0] [0] = 10;

        // bow
        property [0] [14] [0] = 1; // damage
        property [0] [14] [1] = 0; // helps to get past evasion
        property [0] [14] [2] = 0; // helps to get past AC
        mass [0] [0] = 100;

        // crossbow
        property [0] [15] [0] = 1; // damage
        property [0] [15] [1] = 0; // helps to get past evasion
        property [0] [15] [2] = 0; // helps to get past AC
        mass [0] [0] = 250;

//      I've cut this bit out.
*/



        // weapons: blunt weapons are first to help grouping them together
        //  note: AC property can't be 0 or -ve because of division.
        //        If it's 1, makes no difference


        // NOTE: I have *removed* AC bit for weapons - just doesn't work

        // property [x] [2] is speed

        // club
        property [0] [0] [0] = 4; // damage
        property [0] [0] [1] = 3; // helps to get past evasion
        property [0] [0] [2] = 12; // helps to get past AC
        mass [0] [0] = 50;

        // mace:
        property [0] [1] [0] = 7; // damage
        property [0] [1] [1] = 3; // helps to get past evasion
        property [0] [1] [2] = 13; // helps to get past AC
        mass [0] [1] = 140;

        // flail
        property [0] [2] [0] = 8; // damage
        property [0] [2] [1] = 1; // helps to get past evasion
        property [0] [2] [2] = 15; // helps to get past AC
        mass [0] [2] = 150;

        // dagger
        property [0] [3] [0] = 3; // damage
        property [0] [3] [1] = 6; // helps to get past evasion
        property [0] [3] [2] = 11; // helps to get past AC
        mass [0] [3] = 20;

        // morningstar
        property [0] [4] [0] = 8; // damage
        property [0] [4] [1] = 3; // helps to get past evasion
        property [0] [4] [2] = 14; // helps to get past AC
        mass [0] [4] = 120;

        // short sword
        property [0] [5] [0] = 6; // damage
        property [0] [5] [1] = 5; // helps to get past evasion
        property [0] [5] [2] = 12; // helps to get past AC
        mass [0] [5] = 100;

        // long sword
        property [0] [6] [0] = 10; // damage
        property [0] [6] [1] = 3; // helps to get past evasion
        property [0] [6] [2] = 14; // helps to get past AC
        mass [0] [6] = 160;

        // great sword
        property [0] [7] [0] = 14; // damage
        property [0] [7] [1] = -3; // helps to get past evasion
        property [0] [7] [2] = 17; // helps to get past AC
        mass [0] [7] = 250;

        // scimitar
        property [0] [8] [0] = 11; // damage
        property [0] [8] [1] = -1; // helps to get past evasion
        property [0] [8] [2] = 14; // helps to get past AC
        mass [0] [8] = 170;

        // hand axe
        property [0] [9] [0] = 7; // damage
        property [0] [9] [1] = 2; // helps to get past evasion
        property [0] [9] [2] = 13; // helps to get past AC
        mass [0] [9] = 110;

        // battleaxe
        property [0] [10] [0] = 15; // damage
        property [0] [10] [1] = -4; // helps to get past evasion
        property [0] [10] [2] = 18; // helps to get past AC
        mass [0] [10] = 200;

        // spear
        property [0] [11] [0] = 5; // damage
        property [0] [11] [1] = 4; // helps to get past evasion
        property [0] [11] [2] = 13; // helps to get past AC
        mass [0] [11] = 50;

        // halberd
        property [0] [12] [0] = 13; // damage
        property [0] [12] [1] = -5; // helps to get past evasion
        property [0] [12] [2] = 19; // helps to get past AC
        mass [0] [12] = 200;

        // sling - the three properties are _not_ irrelevant here - when something hits something else (either may be you) in melee, these are used.
        property [0] [13] [0] = 1; // damage
        property [0] [13] [1] = -1; // helps to get past evasion
        property [0] [13] [2] = 11; // helps to get past AC
        mass [0] [13] = 10;

        // bow
        property [0] [14] [0] = 2; // damage
        property [0] [14] [1] = -3; // helps to get past evasion
        property [0] [14] [2] = 11; // helps to get past AC
        mass [0] [14] = 100;

        // crossbow
        property [0] [15] [0] = 2; // damage
        property [0] [15] [1] = -1; // helps to get past evasion
        property [0] [15] [2] = 15; // helps to get past AC
        mass [0] [15] = 250;

 // hand crossbow
        property [0] [16] [0] = 1; // damage
        property [0] [16] [1] = -1; // helps to get past evasion
        property [0] [16] [2] = 11; // helps to get past AC
        mass [0] [16] = 70;

        // glaive
        property [0] [17] [0] = 14; // damage
        property [0] [17] [1] = -5; // helps to get past evasion
        property [0] [17] [2] = 18; // helps to get past AC
        mass [0] [17] = 200;

        // staff - hmmm
        property [0] [18] [0] = 5; // damage
        property [0] [18] [1] = 4; // helps to get past evasion
        property [0] [18] [2] = 12; // helps to get past AC
        mass [0] [18] = 130;

        // scythe
        property [0] [19] [0] = 14; // damage
        property [0] [19] [1] = -8; // helps to get past evasion
        property [0] [19] [2] = 22; // helps to get past AC
        mass [0] [19] = 230;


        // giant club
        property [0] [20] [0] = 20; // damage
        property [0] [20] [1] = -5; // helps to get past evasion
        property [0] [20] [2] = 20; // helps to get past AC
        mass [0] [20] = 750;

        // giant spiked club
        property [0] [21] [0] = 25; // damage
        property [0] [21] [1] = -6; // helps to get past evasion
        property [0] [21] [2] = 20; // helps to get past AC
        mass [0] [21] = 850;

        // these two ^^^ should be the same because of ettins.




        // Missiles:

        // stone
        property [1] [0] [0] = 2;
        property [1] [0] [1] = 5;
        mass [1] [0] = 5;

        // arrow
        property [1] [1] [0] = 2;
        property [1] [1] [1] = 8;
        mass [1] [1] = 10;

        // crossbow bolt
        property [1] [2] [0] = 2;
        property [1] [2] [1] = 10;
        mass [1] [2] = 12;

        // dart
        property [1] [3] [0] = 3;
//      property [1] [3] [1] = whatever - for hand crossbow
        mass [1] [3] = 5;

        // large rock
        property [1] [5] [0] = 30;
        mass [1] [5] = 1000;

}



char damage_type(char wclass, char wtype)
{

if (wclass != 0) return 0; // bludgeon
// 1 = slicing
// 2 = piercing

switch(wtype)
{

        case 0: return 0; //strcat(glog , "club"); break;
        case 1: return 0; //strcat(glog , "mace"); break;
        case 2: return 0; //strcat(glog , "flail"); break;
        case 3: return 1; //strcat(glog , "dagger"); break;
        case 4: return 2; //strcat(glog , "spiked mace"); break;
        case 5: return 1; //strcat(glog , "short sword"); break;
        case 6: return 1; //strcat(glog , "long sword"); break;
        case 7: return 1; //strcat(glog , "great sword"); break;
        case 8: return 1; //strcat(glog , "scimitar"); break;
        case 9: return 1; //strcat(glog , "hand axe"); break;
        case 10: return 1; //strcat(glog , "battleaxe"); break;
        case 11: return 2; //strcat(glog , "spear"); break;
        case 12: return 1; //strcat(glog , "halberd"); break;
        case 13: return 0; //strcat(glog , "sling"); break;
        case 14: return 0; //strcat(glog , "bow"); break;
        case 15: return 0; //strcat(glog , "crossbow"); break;
 case 16: return 0; //strcat(glog , "hand crossbow"); break;
 case 17: return 1; //strcat(glog , "glaive"); break;
        case 18: return 0; //strcat(glog , "quarterstaff"); break;
 case 19: return 1; //strcat(glog , "scythe"); break;
        case 20: return 0; //strcat(glog , "giant club"); break;
        case 21: return 0; //strcat(glog , "giant spiked club"); break;


}


} // end damage_type












/*
void m_all(int mons_alloc [10], int curr_level)
{


switch (curr_level)
{
case 0: mons_alloc [0] = 6;
mons_alloc [1] = 6;
mons_alloc [2] = 6;
mons_alloc [3] = 1;
mons_alloc [4] = 1;
/*mons_alloc [0] = 19;
mons_alloc [1] = 19;
mons_alloc [2] = 19;
mons_alloc [3] = 19;
mons_alloc [4] = 19;* /
mons_alloc [5] = 33;
mons_alloc [6] = 17;
mons_alloc [7] = 36;
mons_alloc [8] = 36;
mons_alloc [9] = 17; break;
/*
mons_alloc [5] = 19;
mons_alloc [6] = 19;
mons_alloc [7] = 19;
mons_alloc [8] = 19;
mons_alloc [9] = 19; break;* /

case 1: mons_alloc [0] = 32;
mons_alloc [1] = 32;
mons_alloc [2] = 21;
mons_alloc [3] = 1;
mons_alloc [4] = 1;
mons_alloc [5] = 33;
mons_alloc [6] = 33;
mons_alloc [7] = 36;
mons_alloc [8] = 36;
mons_alloc [9] = 17; break;

case 2: mons_alloc [0] = 32;
mons_alloc [1] = 21;
mons_alloc [2] = 21;
mons_alloc [3] = 8;
mons_alloc [4] = 14;
mons_alloc [5] = 1;
mons_alloc [6] = 16;
mons_alloc [7] = 16;
mons_alloc [8] = 36;
mons_alloc [9] = 33; break;

case 3: mons_alloc [0] = 32;
mons_alloc [1] = 0;
mons_alloc [2] = 35;
mons_alloc [3] = 8;
mons_alloc [4] = 14;
mons_alloc [5] = 14;
mons_alloc [6] = 41;
mons_alloc [7] = 36;
mons_alloc [8] = 33;
mons_alloc [9] = 16; break;

case 4: mons_alloc [0] = 28;
mons_alloc [1] = 37;
mons_alloc [2] = 38;
mons_alloc [3] = 40;
mons_alloc [4] = 19;
mons_alloc [5] = 31;
mons_alloc [6] = 25;
mons_alloc [7] = 11;
mons_alloc [8] = 13;
mons_alloc [9] = 29; break;
} // end of switch (many_many)


}
*/



int mons_intel(int mclass)
{

switch(mclass)
{
        case 0:
        case 10:
        case 11:
        case 22:
        case 27:
        case 42: // Q
        case 18: // scorpion
 case 62: // wolf spider
 case 66: // buttefly
 case 71: // Boul B
 case 100: // giant mite
        case MLAVA0: // lava worm
 case MLAVA1:
        case MLAVA2:
 case MWATER0: // giant fish
 case MWATER1: // giant fish
 case MWATER2: // giant fish
        return 1; // intelligent, but not much (eg insects)

        case 1:
        case 7:
        case 9:
        case 17:
        case 19:
        case 50: // Y
        case 58: // wyvern
 case 34: // Ice beast
 case 46: // Unseen horror
 case 69: // brain worm????
 case 101: // steam D
 case 104: //strcat(gmon_name, "hippogriff"); break;
 case 105: //strcat(gmon_name, "griffon"); break;
 case 106: //strcat(gmon_name, "hydra"); break;
 return 2; // animal intel

        case 2:
        case 3:
        case 4:
        case 6:
        case 12:
        case 13: // necrophage?
        case 14:
   case 20: // ugly thing
        case 28:
        case 29: // D
        case 30: // 2-h O
        case 33:
        case 36: // K
        case 39: // N
        case 40: // O
        case 45: // T
        case 47: // V
        case 48: // W
        case 52: // o war
        case 53: // K dem
        case 54: // o wiz
        case 55: // o knight
 case 60: // Wight
 case 15: // phantom
 case 64: // hungry ghost
 case 73: // hell hound
 case 74: // minotaur
 case 75: // ice dragon
 case 77: // Blue wraith
 case 102: // very ugly thing
        return 3; // normal intel

        case 8:
        case 26:
        case 31:
        case 37: // Lich
        case 43: // R
 case 68: // Efreet
 case 70: // giant orange brain
 case 79: // Great ooe
 case 80: // devils:
 case 81:
 case 82:
 case 83:
 case 84:
 case 85:
 case 86:
 case 87: // end devils
        //case :
 case 103: // orc sorceror
        return 4; // high intel

        default: return 0; // completely non-intelligent: f, G, J, P, 56(t#)
// oops - where'd I put quasit?

}
// unknown : p, v, x, y, I, M, S, U, X
}



int mons_res_fire(int mclass)
{

// returns 1 if res_fire, 2 if res_hellfire as well
// -1 if takes extra damage from fire
switch(mclass)
{
        case 16: // quasit
 case 21: // fire vortex
        case 29: // dragon
 case 68: // Efreet
 case 72: // flying skull
 case 82: // yellow devil
 case MLAVA0: // lava worm
 case MLAVA1: // lava fish
 case MLAVA2: // lava snake
 case 103: // orc sorceror
        return 1;


        case 31: // fiend
        case 8: // imp
        case 3: // red devil
 case 73: // hell hound
 case 80: // burning devil
        return 2;


 case 38: // mummy
 case 34: // ice beast
// case 60: // wight
 case 75: // ice dragon
 case 77: // blue wraith
 case 86: // devils
 case 87:
 return -1;

        default: return 0;
}

}




int mons_res_poison(int mclass)
{

// returns 1 // if res_fire, 2 if res_hellfire as well

switch(mclass)
{


case 0: //giant ant
case 5: // fungus
case 10: // killer bee
case 13: // necrophage
//case 21: // vortex
case 15: // phantom
case 19: // tunneling worm
case 21: // fire vort
case 22: // giant wasp
//                case           abomination????
case 25: // zombie
case 26: // angel
case 32: // giant spore
case 35: // jelly
case 37: // Lich
case 38: // mummy
case 39: // naga
case 42: // queen bee
case 43: // rakshasa
case 48: // wraith
case 16: // quasit
        case 29: // dragon
        case 31: // fiend
        case 8: // imp
        case 3: // devil
 case 34: // Ice beast
 case 51: // zombie
 case 56: // tunneling worm
 case 60: // wight
 case 61: // oklob plant
 case 63: // shadow
 case 64: // hungry ghost
// case 25: // z
//      case 51: // Z
        case 47: // V
 case 67: // wandering f
 case 68: // Efreet?
 case 71: // Bloud beetle
 case 72: // fly skull
 case 73: // hellhound
 case 75: // ice dragon
 case 76: // slime thing
 case 77: // blue wraith
 case 78: // fake R
 case 79: // great orb of eyes
 case MWATER3: // jellyfish
 case 80:
 case 81:
 case 82:
 case 83:
 case 84:
 case 85:
 case 86:
 case 87:
 case 100: // giant mite
 case 106: //strcat(gmon_name, "hydra"); break;
        return 1;

        default: return 0;
}

}



int mons_holiness(int mclass)
{

switch(mclass)
{

// holy things: -1

case 13: // n
case 15: // p
case 37: // L
case 38: // M
case 47: // V
case 48: // W
case 25: // z
case 51: // Z
case 60: // Wight
case 64: // hungry ghost
case 63: // shadow
case 72: // flying skull
case 77: // blue wraith
return 1; // undead

case 3: // devil???
case 8: // imp
case 31: // fiend
case 73: // hellhound
case 80: // varieties of devils:
case 81:
case 82:
case 83:
case 84:
case 85:
case 86:
case 87:
return 2; // nasty demonic things

default: return 0;

}// end of switch mclass

}


int mons_zombie_size(int mclass)
{


switch(mclass)
{
case 2: // centaur?
// case 19; tunneling worm - no!
case 12: // manticore
case 27: // giant beetle
case 28: // cyclops
case 29: // dragon
case 30: // ettin
//case 31: // fiend - no!
case 40: // ogre
case 45: // troll
case 50: // yak
case 62: // wolf Spider???
case 71: // Bould beetle
case 74: // minotaur
case 75: // ice dragon
case 102: // v u thing
case 104: //strcat(gmon_name, "hippogriff"); break;
case 105: //strcat(gmon_name, "griffon"); break;
return 2;

// creatures which can't become zombies return 0:
case 15: // phantom
case 3: // devil
case 5: // fungus
case 8: // imp
case 13: // necr
case 16: // quasit
case 19: // tunneling worm
case 56: // t#
case 21: // fire vortex
case 23: // abomination??
case 24: // yellow light!!!
case 26: // angel
case 31: // fiend
case 32: // giant spore
//case 34: // I?????????????????
case 35: // jelly
case 37: // lich
case 38: // mummy
case 41: // plant
case 43: // Raks
//case 44: // S?????????????
case 46: // U
case 47: // Vampire
case 48: // Wraith
case 49: // abomination???
case 34: // Ice beast
case 60: // Wight
case 61: // oklob plant
// and, of course:
case 25: // z
case 51: // Z
case 64: // hungry ghost
case 70: // giant orange brain
case 72: //strcat(gmon_name, "flying skull"); break;
case 73: //strcat(gmon_name, "hell hound"); break;
case 76: //strcat(gmon_name, "slime thing"); break;
case 77: // blue wraith
case 78: // fake R
case 79: // GooE
case 80:
case 81:
case 82:
case 83:
case 84:
case 85:
case 86:
case 87:
case 106: // hydra
return 0;

}

return 1; // small

} // end of int mons_zombie size



int mons_res_cold(int mclass)
{

// returns 1 if res_cold, 2 if res_hellfire as well
// -1 if takes extra damage from cold
switch(mclass)
{
/*      case 16: // quasit
        case 29: // dragon
        return 1;*/

 case 29: // Dragon
        case 31: // Fiend
        case 8: // imp
        case 3: // devil
 case 21: // fire vort
 case MLAVA0: // lava worm
 case MLAVA1: // lava fish
 case MLAVA2: // lava snake
 case 68: // Efreet
 case 73: // hellhound
 case 80: // burning devil
        return -1;

 case 16: // quasit???
 case 38: // mummy
 case 13: // n
 case 15: // phantom
 case 25: // z
 case 37: // L
 case 47: // V
 case 48: // Wraith
 case 51: // Zombie
 case 34: // Ice beast
 case 60: // Wight
 case 63: // shadow
 case 64: // hungry ghost
 case 72: // flying skull
 case 75: // ice dragon
 case 77: // blue wraith
 case 84: // shadow devil
 case 81: // rotting devil
 case 83: // reaper
 case 86: // ice
 case 87: // blue
 return 1;


        default: return 0;
}

}

char mons_shouts(int mclass)
{

switch(mclass)
{

 case 2: // centaur
 case 6: // goblin
 case 8: // imp
 case 14: // orc
 case 20: // u
 case 28: // Cyclops
// case 30: // Ettin
// case 31: // Fiend
 case 33: // H
 case 36: // K
 case 37: // Lich
 case 40: // Ogre
 case 45: // Troll
 case 52: // owar
 case 53: // Kdem
 case 54: // owiz
 case 55: // oknight
 case 85:
 case 74: // minotaur
 case 102: // v u thing
 case 103: // orc sorceror
 return 1;

 case 7: // hound
 case 9: // jackal
 case 73: // hell hound
 return 2; // bark

 case 30: // 2-h ogre
 return 3; // 2 shouts

 case 31: // Fiend
 case 78: // Rak
 case 82: // tormentor
 case 106: // hydra
 return 4; // roar

 case 72: // flying skull
 case 80: // hellion
 return 5; // scream

 case 50: // Yak - also minotaur
 return 6; // bellow

case 104: //strcat(gmon_name, "hippogriff"); break;
case 105: //strcat(gmon_name, "griffon"); break;
return 7; // screech

 default: return 0;
}

}


char mons_flies(int mclass)
{

switch(mclass)
{


 case 1: // b
 case 3: // d
 case 8: // i ?
 case 10: // k
// case 21: // v???
 case 24: // y
 case 26: // A
 case 29: // D
 case 31: // F?
 case 42: // Queen bee
 case 66: // butterflue
 case 75: // ice dragon
 case 82: // yellow devil
 case 87: // blue devil
 case 101: // steam dragon
 case 104: // hippogriff
 case 105: // griffon
 return 1; // wings (stops flying if paralysed)

 case 21: // fire vortex
 case 32: // Giant spore
 case 48: // Wraith???
 case 59: // Giant eyeball
 case 65: // G of draining
 case 46: // Unseen horror
 case 68: // Efreet
 case 70: // orange devil
 case 72: // flying skull
 case 77: // blue wraith
 case 79: // Gooe
 case 84: // shadow devil
 return 2; // levitating

 default: return 0;
}


}


char mons_see_invis(int mclass)
{

switch(mclass)
{
case 1: // bat?
case 7: // hound
case 8: // imp
case 31: // fiend ???
case 39: // naga
case 43: // raksh
case 46: // U
case 47: // V
case 48: // Wraith
case 73: // hell hound
case 77: // freezing wraith
case 79: // Gooe
//case 80: //strcat(gmon_name, "hellion"); break;
//case 81: //strcat(gmon_name, "rotting devil"); break;
//case 82: //strcat(gmon_name, "tormentor"); break;
case 83: //strcat(gmon_name, "reaper"); break;
case 84: //strcat(gmon_name, "soul eater"); break;
//case 85: //strcat(gmon_name, "hairy devil"); break;
case 86: //strcat(gmon_name, "ice devil"); break;
//case 87: //strcat(gmon_name, "blue devil"); break;
case 103: // orc sorceror
return 1;
default: return 0;
}
}




char mons_spell_list(int sec, int whichs)
{

int splist [6];

// [4] can be 18 (digging)

switch(sec)
{
case 0: // orc wiz
splist [0] = 0; // bolt spell
splist [1] = 4; // enchantment
splist [2] = 5; // self_ench
splist [3] = 0; // misc
splist [4] = 100; // misc2
splist [5] = 16; // emergency
break;

case 1: // orc wiz
splist [0] = 1; // bolt spell
splist [1] = 6; // enchantment
splist [2] = 11; // self_ench
splist [3] = 0; // misc
splist [4] = 100; // misc2
splist [5] = 100; // emergency
break;

case 2: // orc wiz
splist [0] = 2; // bolt spell
splist [1] = 100; // enchantment
splist [2] = 5; // self_ench
splist [3] = 1; // misc
splist [4] = 0; // misc2
splist [5] = 100; // emergency
break;

case 10: // naga
splist [0] = 15; // bolt spell
splist [1] = 15; // enchantment
splist [2] = 13; // self_ench
splist [3] = 7; // misc
splist [4] = 4; // misc2
splist [5] = 13; // emergency
break;

case 20: // Lich
splist [0] = 9; // bolt spell
splist [1] = 3; // enchantment
splist [2] = 11; // self_ench
splist [3] = 10; // misc
splist [4] = 0; // misc2
splist [5] = 14; // emergency
break;

case 21: // Lich
splist [0] = 8; // bolt spell
splist [1] = 6; // enchantment
splist [2] = 5; // self_ench
splist [3] = 19; // misc
splist [4] = 0; // misc2
splist [5] = 100; // emergency
break;

case 22: // Lich
splist [0] = 19; // bolt spell
splist [1] = 100; // enchantment
splist [2] = 5; // self_ench
splist [3] = 2; // misc
splist [4] = 17; // misc2
splist [5] = 16; // emergency
break;

case 23: // Lich
splist [0] = 22; // bolt spell
splist [1] = 9; // enchantment
splist [2] = 11; // self_ench
splist [3] = 10; // misc
splist [4] = 0; // misc2
splist [5] = 11; // emergency
break;

case 30: // burning devil
splist [0] = 20; // bolt spell
splist [1] = 20; // enchantment
splist [2] = 20; // self_ench
splist [3] = 20; // misc
splist [4] = 20; // misc2
splist [5] = 20; // emergency
break;

case 40: // vampire
splist [0] = 21; // bolt spell
splist [1] = 100; // enchantment
splist [2] = 100; // self_ench
splist [3] = 100; // misc
splist [4] = 100; // misc2
splist [5] = 21; // emergency
break;
// 2 is also used for eg smiting etc

case 50: // efreet
splist [0] = 8; // bolt spell
splist [1] = 100; // enchantment
splist [2] = 100; // self_ench
splist [3] = 12; // misc
splist [4] = 100; // misc2
splist [5] = 100; // emergency
break;

case 52: // brain worm
splist [0] = 23; // bolt spell
splist [1] = 100; // enchantment
splist [2] = 100; // self_ench
splist [3] = 23; // misc
splist [4] = 100; // misc2
splist [5] = 100; // emergency
break;

case 53: // giant orange brain
splist [0] = 23; // bolt spell
splist [1] = 16; // enchantment
splist [2] = 6; // self_ench
splist [3] = 24; // misc
splist [4] = 16; // misc2
splist [5] = 14; // emergency
break;

case 54: // R
splist [0] = 100; // bolt spell
splist [1] = 16; // enchantment
splist [2] = 11; // self_ench
splist [3] = 25; // misc
splist [4] = 16; // misc2
splist [5] = 14; // emergency
break;

case 55: // GooE
splist [0] = 3; // bolt spell
splist [1] = 15; // enchantment
splist [2] = 100; // self_ench
splist [3] = 4; // misc
splist [4] = 6; // misc2
splist [5] = 15; // emergency
break;

case 56: // orc sorceror
splist [0] = 8; // bolt spell
splist [1] = 19; // enchantment
splist [2] = 100; // self_ench
splist [3] = 27; // misc
splist [4] = 20; // misc2
splist [5] = 14; // emergency
break;

case 57: // steam dragon
splist [0] = 26; // bolt spell
splist [1] = 26; // enchantment
splist [2] = 100; // self_ench
splist [3] = 26; // misc
splist [4] = 26; // misc2
splist [5] = 100; // emergency
break;

} // end switch(sec)

return splist [whichs];

}
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
case 17: // splinters
case 18: // dig
case 19: // -ve energy
case 20: // burst of hellfire
case 21: // vampire's summoning
case 22: // mystic blast
case 23: // brain feeding
case 24: // summon anything
case 25: // fake Rs
case 26: // breathe steam
case 27: // summon devils
case 49: // fiend's hellfire
*/


/*


case 0: strcat(gmon_name, "giant ant"); break;
case 1: strcat(gmon_name, "giant bat"); break;
case 2: strcat(gmon_name, "centaur"); break;
case 3: strcat(gmon_name, "devil"); break;
case 4: strcat(gmon_name, "ettin"); break;
case 5: strcat(gmon_name, "fungus"); break;
case 6: strcat(gmon_name, "goblin"); break;
case 7: strcat(gmon_name, "hound"); break;
case 8: strcat(gmon_name, "imp"); break;
case 9: strcat(gmon_name, "jackal"); break;
case 10: strcat(gmon_name, "killer bee"); break;
case 11: strcat(gmon_name, "killer bee larva"); break;
case 12: strcat(gmon_name, "manticore"); break;
case 13: strcat(gmon_name, "necrophage"); break;
case 14: strcat(gmon_name, "orc"); break;
case 15: strcat(gmon_name, "phantom"); break;
case 16: strcat(gmon_name, "quasit"); break;
case 17: strcat(gmon_name, "rat"); break;
case 18: strcat(gmon_name, "scorpion"); break;
case 19: strcat(gmon_name, "tunneling worm"); break;
case 20: strcat(gmon_name, "ugly thing"); break;
case 21: strcat(gmon_name, "vortex"); break;
case 22: strcat(gmon_name, "worm"); break;
case 23: strcat(gmon_name, "abomination"); break;
case 24: strcat(gmon_name, "yellow wasp"); break;
case 25: break;
//strcat(gmon_name, " zombie");
//break;
case 26: strcat(gmon_name, "angel"); break;
case 27: strcat(gmon_name, "giant beetle"); break;
case 28: strcat(gmon_name, "cyclops"); break;
case 29: strcat(gmon_name, "dragon"); break;
case 30: strcat(gmon_name, "two-headed ogre"); break;
case 31: strcat(gmon_name, "fiend"); break;
case 32: strcat(gmon_name, "giant spore"); break;
case 33: strcat(gmon_name, "hobgoblin"); break;
case 34: strcat(gmon_name, "ice beast"); break;// not a real creature
case 35: strcat(gmon_name, "jelly"); break;
case 36: strcat(gmon_name, "kobold"); break;
case 37: strcat(gmon_name, "lich"); break;
case 38: strcat(gmon_name, "mummy"); break;
case 39: strcat(gmon_name, "naga"); break;
case 40: strcat(gmon_name, "ogre"); break;
case 41: strcat(gmon_name, "plant"); break;
case 42: strcat(gmon_name, "queen bee"); break;
case 43: strcat(gmon_name, "raksasha"); break;
case 44: strcat(gmon_name, "snake"); break;
case 45: strcat(gmon_name, "troll"); break;
case 46: strcat(gmon_name, "unseen horror"); break;
case 47: strcat(gmon_name, "vampire"); break;
case 48: strcat(gmon_name, "wraith"); break;
case 49: strcat(gmon_name, "abomination"); break;
case 50: strcat(gmon_name, "yak"); break;
case 51: break;
// strcat(gmon_name, "large zombie"); break;
case 52: strcat(gmon_name, "orc warrior"); break;
case 53: strcat(gmon_name, "kobold demonologist"); break;
case 54: strcat(gmon_name, "orc wizard"); break;
case 55: strcat(gmon_name, "goblin wizard"); break;
case 56: strcat(gmon_name, "worm tail"); break;

//strcat(gmon_name 54, "Type I demon");
//case 56: strcat(gmon_name, "Type II demon"); break;
case 57: strcat(gmon_name, "wyvern"); break;
case 58: strcat(gmon_name, "kobold leader"); break;
case 59: strcat(gmon_name, "giant eyeball"); break;
case 60: strcat(gmon_name, "wight"); break;
case 61: strcat(gmon_name, "oklob plant"); break;
case 62: strcat(gmon_name, "wolf spider"); break;
case 63: strcat(gmon_name, "shadow"); break;
case 64: strcat(gmon_name, "hungry ghost"); break;

case 65: strcat(gmon_name, "eye of draining"); break;
case 66: strcat(gmon_name, "butterfly"); break;
case MLAVA0: strcat(gmon_name, "lava worm"); break;
default: //strcat(gmon_name, "illegal monster"); break;
itoa(mcl, gmon_name, 10); break;

*/

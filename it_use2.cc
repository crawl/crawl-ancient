#include <string.h>

#include "externs.h"

#include "beam.h"
#include "player.h"
#include "spells2.h"
#include "misc.h"
#include "mutation.h"
#include "randart.h"
#include "spells.h"
#include "stuff.h"
#include "itemname.h"
#include "skills2.h"
#include "effects.h"
#include "view.h"

char zappy(int func_pass [10], char str_pass [50], char z_type);
void unuse_randart(unsigned char unw);

extern char wield_change; /* defined in output.cc */



void zapping(char ztype, int power, struct bolt beams [1])
{

int func_pass [10];

beams[0].ench_power = power;

func_pass [2] = power;
func_pass [8] = power;

beams[0].damage = power;

char luggy = zappy(func_pass, str_pass, ztype);

if (ztype == 14)
{
   strcpy(info, "You hear a mighty clap of thunder!");
   mpr(info);
   noisy(25, you[0].x_pos, you[0].y_pos);
}

strcpy(beams[0].beam_name, str_pass);

beams[0].colour = func_pass [0];
beams[0].range = func_pass [1];
beams[0].damage = func_pass [2];
if (func_pass [2] == 0) beams[0].damage = power;
beams[0].hit = func_pass [3];
beams[0].type = func_pass [4];
beams[0].flavour = func_pass [5];
beams[0].thing_thrown = func_pass [6];
beams[0].wand_id = func_pass [7];

beams[0].ench_power = func_pass [8];

func_pass [8] = power;

if (beams[0].beam_name [0] == 48) beams[0].damage = power;

beams[0].thing_thrown = 3;

switch(luggy)
{
        case 1: missile(beams, 0); break;
        case 2: beam(beams); break;
}


}




char zappy(int func_pass [10], char str_pass [50], char z_type)
{

func_pass [7] = 0; // wand_id

func_pass [9] = func_pass [2];

switch(z_type)
{
        case 0: // flame
        func_pass [0] = 4;
        strcpy(str_pass, "puff of flame");
        func_pass [1] = random2(5) + 9;
        func_pass [2] = 104 + func_pass [8] / 16; // should this be the same as magic missile? no
        func_pass [3] = 60;
        func_pass [4] = '#'; // maybe make missile() put a # on wherever it hits?
        func_pass [6] = 4; //3;
        func_pass [5] = 2; // fire
        func_pass [7] = 1; // wand_id
        return 1;
        //break;


        case 1: // frost
//      beam_source_y = y_pos;
        func_pass [0] = 15;
        strcpy(str_pass, "puff of frost");
        func_pass [1] = random2(5) + 9;
        func_pass [2] = 104 + func_pass [8] / 16; // should this be the same as magic missile?
        func_pass [3] = 60;
        func_pass [4] = '#'; // maybe make missile() put a # on wherever it hits?
        func_pass [6] = 4;//3;
        //return 1;
        func_pass [5] = 3; // cold
        func_pass [7] = 1;
        return 1;
        //break;

        case 2: // slow monster
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 9;
        func_pass [4] = 0;
        func_pass [0] = 0;
        func_pass [6] = 2; //1;
        func_pass [2] = 50;
        func_pass [8] *= 15;
        func_pass [8] /= 10;
        func_pass [5] = 4; // magic
        return 2;
        //break;

        case 3: // hasting
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 9;
        func_pass [4] = 0;
        func_pass [0] = 1;
        func_pass [6] = 2; //1;
        func_pass [5] = 4; // magic
        func_pass [8] *= 15;
        func_pass [8] /= 10;
        return 2;
        //break;

        case 4: // magic missile
        func_pass [0] = 13;//you[0].inv_col [throw_2];//icolour [you[0].inv_class [throw_2]] [you[0].inv_type [throw_2]];
        strcpy(str_pass, "magic dart");// you[0].inv_name [throw_2]);
        func_pass [1] = random2(5) + 9;
        func_pass [2] = 102 + (func_pass [8] / 22) + random2(2);
        func_pass [3] = 1500;
        func_pass [4] = 35;
        func_pass [6] = 4;//3;
        func_pass [5] = 1; // a magical missile
        func_pass [7] = 1;
        return 1;
        //break;

        case 5: // healing
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 9;
        func_pass [4] = 0;
        func_pass [0] = 2;
        func_pass [6] = 2; //1;
        func_pass [5] = 4; // magic
        return 2;
        //break;

        case 6: // paralysis
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 9;
        func_pass [4] = 0;
        func_pass [0] = 3;
        func_pass [6] = 2;//1;
        func_pass [5] = 4; //  magic
 func_pass [2] = 0; //func_pass [9];
        func_pass [8] *= 15;
        func_pass [8] /= 10;
        return 2;
        //break;

        case 7:
        strcpy(str_pass, "bolt of fire");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 106 + func_pass [8] / 12;
        func_pass [0] = 4;
        func_pass [4] = 35; // 35
        func_pass [6] = 2;//1;
        func_pass [5] = 2; // fire
        func_pass [7] = 1;
 func_pass [3] = 8 + random2(func_pass [8]) / 80;
        return 2;
        //break;
//}


//if (you[0].inv_type [zap_device_2] == 1)
//{
        case 8:
        strcpy(str_pass, "bolt of cold");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 106 + func_pass [8] / 12;
        func_pass [0] = WHITE; //LIGHTCYAN;
        func_pass [4] = 35;
        func_pass [6] = 2;//1;
        func_pass [5] = 3; // cold
        func_pass [7] = 1;
 func_pass [3] = 8 + random2(func_pass [8]) / 80;
        return 2;
        //break;

        case 9: // confusion
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 9;
        func_pass [4] = 0;
        func_pass [0] = 4;
        func_pass [6] = 2;//1;
        func_pass [5] = 4; // magic
        func_pass [8] *= 15;
        func_pass [8] /= 10;
        return 2;
        //break;

        case 10: // make invisible
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 9;
        func_pass [4] = 0;
        func_pass [0] = 5;
        func_pass [6] = 2;//1;
        func_pass [5] = 4; // magic
        func_pass [8] *= 15;
        func_pass [8] /= 10;
        return 2;
        //break;

        case 11: // digging
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 4 + random2(func_pass [8]);
        func_pass [4] = 0;
        func_pass [0] = 6;
        func_pass [6] = 2;//1;
        func_pass [5] = 4;
        return 2;
        //break;


        case 12: // fireball
        func_pass [0] = 4;
        strcpy(str_pass, "fireball");
        func_pass [1] = random2(5) + 9;
        func_pass [2] = 5 + func_pass [8] / 7;
        func_pass [3] = 40;
        func_pass [4] = 35;
        func_pass [6] = 4;//3;
        func_pass [5] = 10; // fire
        return 1;

        case 13: // teleportation
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 8;
        func_pass [4] = 0;
        func_pass [0] = 7; // 6 is used by digging
        func_pass [6] = 2;//1;
        func_pass [5] = 4; // magic
        func_pass [8] *= 15;
        func_pass [8] /= 10;
        return 2;
        //break;

        case 14: // lightning
        strcpy(str_pass, "bolt of lightning");
        func_pass [1] = random2(10) + 9;
        func_pass [2] = 105 + func_pass [8] / 14;
        func_pass [0] = LIGHTCYAN;
        func_pass [4] = 35;
        func_pass [6] = 2;//1;
        func_pass [5] = 5; // electricity
        func_pass [7] = 1;
 func_pass [3] = 10 + random2(func_pass [8]) / 40; // beam hit - look at it!
        return 2;

   case 15: // polymorph
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 8;
        func_pass [4] = 0;
        func_pass [0] = 8;
        func_pass [6] = 2;//1;
        func_pass [5] = 4; // magic
        func_pass [8] *= 15;
        func_pass [8] /= 10;
        return 2;

        case 16: // venom bolt
        strcpy(str_pass, "bolt of poison");
        func_pass [1] = random2(10) + 9;
        func_pass [2] = 105 + func_pass [8] / 13;
        func_pass [0] = LIGHTGREEN;
        func_pass [4] = 35;
        func_pass [6] = 2;//1;
        func_pass [5] = 6; // poison
        func_pass [7] = 1;
 func_pass [3] = 7 + random2(func_pass [8]) / 80;
        return 2;

 case 17: // negative energy
        strcpy(str_pass, "bolt of negative energy");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 104 + func_pass [8] / 13;
        func_pass [0] = DARKGREY;
        func_pass [4] = 35;
        func_pass [6] = 2;//1;
        func_pass [5] = 7; // -ve energy
        func_pass [7] = 1;
 func_pass [3] = 7 + random2(func_pass [8]) / 80;
 return 2;

 case 18: // was splinters
        strcpy(str_pass, "crystal spear");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 112 + func_pass [8] / 11;
        func_pass [0] = WHITE;
        func_pass [4] = ')';
        func_pass [6] = 2;
        func_pass [5] = 1;
        func_pass [7] = 1;
 func_pass [3] = 6 + random2(func_pass [8]) / 25;
 return 1;

 case 19: // bolt of innacuracy
        strcpy(str_pass, "narrow beam of energy");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 110 + func_pass [8] / 5;// / 10;
        func_pass [0] = YELLOW;
        func_pass [4] = 35;
        func_pass [6] = 2;//1;
        func_pass [5] = 17; // whatever
        func_pass [7] = 1;
 func_pass [3] = 2; //1 + random2(func_pass [8]) / 80;
 return 2;

 case 20: // Mystic Bolt
        func_pass [0] = 13;//you[0].inv_col [throw_2];//icolour [you[0].inv_class [throw_2]] [you[0].inv_type [throw_2]];
        strcpy(str_pass, "orb of energy");// you[0].inv_name [throw_2]);
        func_pass [1] = random2(5) + 8;
        func_pass [2] = 105 + (func_pass [8] / 9);
        func_pass [3] = 10 + (func_pass [8] / 20);
        func_pass [4] = 35;
        func_pass [6] = 4;//3;
        func_pass [5] = 1; // a magical missile
        func_pass [7] = 1;
        return 1;

        case 21: // charm monster
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 8;
        func_pass [4] = 0;
        func_pass [0] = 9;
        func_pass [6] = 2; //1;
        func_pass [2] = 50;
        //beam();
        func_pass [8] *= 15;
        func_pass [8] /= 10;
        func_pass [5] = 4; // magic
        return 2;

        case 22: // pain
        strcpy(str_pass, "0");
        func_pass [1] = random2(8) + 8;
        func_pass [4] = 0;
        func_pass [0] = 13; // pain
        func_pass [6] = 2; //1;
        func_pass [2] = 50;
        func_pass [3] = 8 + (func_pass [8] / 20);  //104 + (func_pass [8] / 150);
        func_pass [5] = 4; // magic
        func_pass [8] *= 35;
        func_pass [8] /= 10;
        return 2;

        case 23: // sticky flame
        func_pass [0] = 4;
        strcpy(str_pass, "sticky flame");
        func_pass [1] = random2(5) + 9;
        func_pass [2] = 102 + func_pass [8] / 16;
        func_pass [3] = 60;
        func_pass [4] = '#';
        func_pass [6] = 4;
        func_pass [5] = 2; // fire
        func_pass [7] = 1; // wand_id
        return 1;
        //break;

        case 24: // dispel undead
        strcpy(str_pass, "0");
        func_pass [1] = random2(8) + 8;
        func_pass [4] = 0;
        func_pass [0] = 14; // pain
        func_pass [6] = 2; //1;
        func_pass [2] = 50;
        func_pass [3] = 9 + (func_pass [8] / 3);
        func_pass [5] = 4; // magic
        func_pass [8] *= 15;
        func_pass [8] /= 10;
        return 2;

        case 25: // flame of cleansing
        strcpy(str_pass, "golden flame");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 110 + func_pass [8] / 5;
        func_pass [0] = YELLOW;
        func_pass [4] = 35;
        func_pass [6] = 2;
        func_pass [5] = 18;
        func_pass [7] = 1;
        func_pass [3] = 55 + random2(func_pass [8]) / 80;
        return 2;

        case 26: // bone shards
        strcpy(str_pass, "spray of bone shards");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 102 + func_pass [8] / 150; // note that f_p [8] has a high value for this spell
        func_pass [0] = LIGHTGREY;
        func_pass [4] = 35;
        func_pass [6] = 2;
        func_pass [5] = 4;
        func_pass [7] = 1;
        func_pass [3] = 55 + random2(func_pass [8]) / 850;
        return 2;

        case 27: // banishment
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 8;
        func_pass [4] = 0;
        func_pass [0] = 10;
        func_pass [6] = 2;
        func_pass [2] = 50;
        func_pass [5] = 4; // magic
        func_pass [8] *= 15;
        func_pass [8] /= 10;
        return 2;

        case 28: // degeneration
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 8;
        func_pass [4] = 0;
        func_pass [0] = 11;
        func_pass [6] = 2;
        func_pass [2] = 50;
        func_pass [5] = 4; // magic
        func_pass [8] *= 15;
        func_pass [8] /= 10;
        return 2;

        case 29: // sting
        func_pass [0] = GREEN;
        strcpy(str_pass, "sting");
        func_pass [1] = random2(5) + 9;
        func_pass [2] = 4 + func_pass [8] / 15;
        func_pass [3] = 60;
        func_pass [4] = '#';
        func_pass [6] = 4;
        func_pass [5] = 6; // fire
        func_pass [7] = 1; // wand_id
        return 1;

        case 30: // hellfire
        strcpy(str_pass, "hellfire");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 20;
        func_pass [0] = RED;
        func_pass [4] = 35;
        func_pass [6] = 2;
        func_pass [5] = 10;
        func_pass [7] = 1;
        func_pass [3] = 20;
        return 2;

        case 31: // Iron Bolt
        func_pass [0] = LIGHTCYAN;
        strcpy(str_pass, "iron bolt");
        func_pass [1] = random2(5) + 5;
        func_pass [2] = 108 + (func_pass [8] / 10);
        func_pass [3] = 7 + (func_pass [8] / 25);
        func_pass [4] = ')';
        func_pass [6] = 2;
        func_pass [5] = 1; // a magical missile - or similarly unresisted thing
        func_pass [7] = 1;
        return 1;

        case 32: // throw pebble
        func_pass [0] = BROWN;
        strcpy(str_pass, "pebble");
        func_pass [1] = random2(5) + 9;
        func_pass [2] = 5;// + (func_pass [8] / 10);
        func_pass [3] = 8 + func_pass [8] / 20;
        func_pass [4] = ')';
        func_pass [6] = 2;
        func_pass [5] = 1; // a magical missile or something
        func_pass [7] = 1;
        return 1;

        case 33: // stone arrow
        func_pass [0] = LIGHTGREY;
        strcpy(str_pass, "stone arrow");
        func_pass [1] = random2(5) + 9;
        func_pass [2] = 105 + (func_pass [8] / 12);
        func_pass [3] = 7 + func_pass [8] / 27;
        func_pass [4] = ')';
        func_pass [6] = 2;
        func_pass [5] = 1; // a magical missile or something
        func_pass [7] = 1;
        return 1;

        case 34: // shock
        func_pass [0] = LIGHTCYAN;
        strcpy(str_pass, "zap");
        func_pass [1] = random2(5) + 9;
        func_pass [2] = 4 + (func_pass [8] / 8);
        func_pass [3] = 1500;
        func_pass [4] = 35;
        func_pass [6] = 2;
        func_pass [5] = 5; // electricity
        func_pass [7] = 1;
        return 2;

        case 35: // orb of elect
        func_pass [0] = LIGHTBLUE;
        strcpy(str_pass, "orb of electricity");
        func_pass [1] = random2(10) + 12;
        func_pass [2] = 5 + func_pass [8] / 5;
        func_pass [3] = 40;
        func_pass [4] = '#';
        func_pass [6] = 2;
        func_pass [5] = 5; // elec
        return 1;

        case 36: // Naga's species ability
        func_pass [0] = GREEN;
        strcpy(str_pass, "splash of poison");
        func_pass [1] = 4 + random2(func_pass [8] / 2 + 1);
        func_pass [2] = 4 + func_pass [8] / 2; // note func_pass is xl
        func_pass [3] = 5 + random2(func_pass [8] / 3 + 1);
        func_pass [4] = '#';
        func_pass [6] = 4;
        func_pass [5] = 6; // poison
        func_pass [7] = 1; // wand_id
        return 1;

        case 37:
        strcpy(str_pass, "debugging ray");
        func_pass [1] = random2(10) + 8;
        func_pass [2] = 150;
        func_pass [0] = random2(15) + 1;
        func_pass [4] = 'X';
        func_pass [6] = 2;
        func_pass [5] = 1;
        func_pass [7] = 1;
        func_pass [3] = 60;
        return 1;

        case 38: // fire breath
        func_pass [0] = RED;
        strcpy(str_pass, "fiery breath");
        func_pass [1] = 4 + random2(func_pass [8] / 2 + 1);
        func_pass [2] = 104 + func_pass [8] / 3; // note func_pass is xl + mut * 4
        func_pass [3] = 8 + random2(func_pass [8] / 3 + 1);
        func_pass [4] = '#';
        func_pass [6] = 4;
        func_pass [5] = 2; // fire
        func_pass [7] = 1; // wand_id
        return 2;

        case 39: // frost breath
        func_pass [0] = WHITE;
        strcpy(str_pass, "freezing breath");
        func_pass [1] = 4 + random2(func_pass [8] / 2 + 1);
        func_pass [2] = 104 + func_pass [8] / 3; // note func_pass is xl + mut * 4
        func_pass [3] = 8 + random2(func_pass [8] / 3 + 1);
        func_pass [4] = '#';
        func_pass [6] = 4;
        func_pass [5] = 3; // frost
        func_pass [7] = 1; // wand_id
        return 2;

        case 40: // spit acid
        func_pass [0] = YELLOW;
        strcpy(str_pass, "acid");
        func_pass [1] = 4 + random2(func_pass [8] / 2 + 1);
        func_pass [2] = 103 + func_pass [8] / 3; // note func_pass is xl
        func_pass [3] = 5 + random2(func_pass [8] / 3 + 1);
        func_pass [4] = '#';
        func_pass [6] = 4;
        func_pass [5] = 8; // acid
        func_pass [7] = 1; // wand_id
        return 1;

        case 41: // poison breath - leaves clouds of gas
        func_pass [0] = GREEN;
        strcpy(str_pass, "poison gas");
        func_pass [1] = 4 + random2(func_pass [8] / 2 + 1);
        func_pass [2] = 102 + func_pass [8] / 6; // note func_pass is xl
        func_pass [3] = 5 + random2(func_pass [8] / 3 + 1);
        func_pass [4] = '#';
        func_pass [6] = 4;
        func_pass [5] = 6; // poison
        func_pass [7] = 1; // wand_id
        return 1;

        case 42: // breathe power
        func_pass [0] = BLUE;
        if (random2(func_pass [8]) >= 8) func_pass [0] = LIGHTBLUE;
        if (random2(func_pass [8]) >= 12) func_pass [0] = MAGENTA;
        if (random2(func_pass [8]) >= 17) func_pass [0] = LIGHTMAGENTA;
        strcpy(str_pass, "bolt of energy");
        func_pass [1] = 7 + random2(func_pass [8] / 2 + 1);
        func_pass [2] = 103 + func_pass [8] / 3; // note func_pass is xl
        func_pass [3] = 11 + random2(func_pass [8] / 3 + 1);
        func_pass [4] = '#';
        func_pass [6] = 4;
        func_pass [5] = 1; // magic missile
        func_pass [7] = 1; // wand_id
        return 1;

        case 43: // enslave undead
        strcpy(str_pass, "0");
        func_pass [1] = random2(5) + 8;
        func_pass [4] = 0;
        func_pass [0] = 12;
        func_pass [6] = 2;
        func_pass [2] = 50;
        func_pass [8] *= 7;
        func_pass [8] /= 10;
        func_pass [5] = 4; // magic
        return 2;

        case 44: // agony
        strcpy(str_pass, "0agony");
        func_pass [1] = random2(8) + 8;
        func_pass [4] = 0;
        func_pass [0] = 13; // pain
        func_pass [6] = 2; // 1
        func_pass [2] = 50;
        func_pass [3] = 6 + (func_pass [8] / 50);  //104 + (func_pass [8] / 150);
        func_pass [5] = 4; // magic
        func_pass [8] *= 30;
        func_pass [8] /= 10;
        return 2;

        case 45: // disruption
        strcpy(str_pass, "0");
        func_pass [1] = random2(8) + 8;
        func_pass [4] = 0;
        func_pass [0] = 15; // disrupt/disint
        func_pass [6] = 2;
        func_pass [2] = 50;
        func_pass [3] = 5 + (func_pass [8] / 50);
        func_pass [5] = 4; // magic
        func_pass [8] *= 30;
        func_pass [8] /= 10;
        return 2;

        case 46: // disintegrate
        strcpy(str_pass, "0");
        func_pass [1] = random2(8) + 8;
        func_pass [4] = 0;
        func_pass [0] = 15; // disrupt/disint
        func_pass [6] = 2;
        func_pass [2] = 50;
        func_pass [3] = 15 + (func_pass [8] / 3);
        func_pass [5] = 4; // magic
        func_pass [8] *= 25;
        func_pass [8] /= 10;
        return 2;

        case 47: // Isk's Cross
        func_pass [0] = 13;
        strcpy(str_pass, "blast");
        func_pass [1] = random2(5) + 12;
        func_pass [2] = 105 + (func_pass [8] / 9);
        func_pass [3] = 500; //10 + (func_pass [8] / 20);
        func_pass [4] = 35;
        func_pass [6] = 4;
        func_pass [5] = 1; // an exploding bolt (also used for fireball)
        func_pass [7] = 1;
        return 1;

        case 48: // breathe steam
        func_pass [0] = LIGHTGREY;
        strcpy(str_pass, "ball of steam");
        func_pass [1] = random2(5) + 7;
        func_pass [2] = 104 + (func_pass [8] / 5);
        func_pass [3] = 10 + random2(func_pass [8] / 5 + 1);
        func_pass [4] = '#';
        func_pass [6] = 4;
        func_pass [5] = 2; // fire
        func_pass [7] = 1; // wand_id
    return 1;

/*
beam_colour = func_pass [0];
beam_range = func_pass [1];
beam_damage = func_pass [2];
if (func_pass [2] == 0) beam_damage = power;
beam_hit = func_pass [3];
beam_type = func_pass [4];
beam_flavour = func_pass [5];
thing_thrown = func_pass [6]; - irrelevant : this is set in zapping(...) in crawl99.cc
wand_id = func_pass [7];
ench_power = 8, I think
*/

} // end of switch

return 0;

}


void potion_effect(char pot_eff, int pow)
{

int new_value = 0;

if (pow >= 150) pow = 150;

switch(pot_eff)
{
        case 0:
        mpr("You feel better.");
        you[0].hp += random2(7) + 5;
        if (you[0].hp >= you[0].hp_max)
        {
          if (you[0].base_hp < 5000) you[0].base_hp ++;
          calc_hp();
          you[0].hp = you[0].hp_max;
        }
        you[0].hp_ch = 1;
                you[0].poison = 0;
                you[0].rotting = 0;
  you[0].disease = 0;
  you[0].conf = 0;
/*      beam[0].wand_id = 1;*/
        break;

        case 1:
        mpr("You feel much better.");
        you[0].hp += random2(10) + random2(10) + random2(10) + 10;
        if (you[0].hp >= you[0].hp_max)
        {
          if (you[0].base_hp < 5000) you[0].base_hp += 2 + random2(3) + random2(3);
          if (you[0].base_hp > 5000) you[0].base_hp = 5000;
          calc_hp();
          you[0].hp = you[0].hp_max;
        }
        if (you[0].hp > you[0].hp_max) you[0].hp = you[0].hp_max;
        you[0].hp_ch = 1;
/*      wand_id = 1; */
        break;

        case 2:
        if (you[0].haste == 0) strcpy(info, "You feel yourself speed up.");
                else strcpy(info, "Your high speed feels more durable.");
        mpr(info);
        if (you[0].slow > 0)
        {
                you[0].slow = 0;
        } else you[0].haste += random2(pow) + 40;
/*      wand_id = 1;*/
 if (you[0].haste > 90) you[0].haste = 90;

 break;
//}

        case 3: // potion of might
        if (you[0].might == 0) strcpy(info, "You feel very mighty all of a sudden.");
        else
        {
                if (you[0].might < 150) you[0].might += random2(pow) + 35;
                strcpy(info, "You're still pretty mighty.");
                mpr(info);
                return;
        }
        mpr(info);
        you[0].might += random2(pow) + 35;
        you[0].strength += 5;
 you[0].max_strength += 5;
        you[0].strength_ch = 1;
 if (you[0].might > 75) you[0].might = 75;
        break;

        case 4: // potion of increase you[0].strength
        mutate(1);
/*      strcpy(info, "You feel strong!");
        mpr(info);
        abil_change = random2 (2) + 1;
 you[0].strength += abil_change;
 if (you[0].strength > you[0].max_strength) you[0].max_strength = you[0].strength; //+= abil_change;
        you[0].strength_ch = 1;*/
        break;

        case 5: // potion of increase dexterity
        mutate(3);
/*      strcpy(info, "You feel dextrous!");
        mpr(info);
        abil_change = random2 (2) + 1;
 you[0].dex += abil_change;
 if (you[0].dex > you[0].max_dex) you[0].max_dex = you[0].dex; // += abil_change;
        you[0].dex_ch = 1;*/
        break;

        case 6: // potion of increase intelligence
        mutate(2);
/*      strcpy(info, "You feel intelligent!");
        mpr(info);
        abil_change = random2 (2) + 1;
 you[0].intel += abil_change;
 if (you[0].intel > you[0].max_intel) you[0].max_intel = you[0].intel; // += abil_change;
        you[0].intel_ch = 1;*/
        break;

        case 7: // levitation
        if (you[0].lev == 0)
        {
                strcpy(info, "You feel very buoyant!");
                mpr(info);
                strcpy(info, "You gently float upwards from the floor.");
                mpr(info);
        } else
              {
              strcpy(info, "You feel more buoyant.");
              mpr(info);
              }
        if (you[0].lev >= 0) you[0].lev += random2 (pow) + 25;
 if (you[0].lev > 100) you[0].lev = 100;
        burden_change();
        break;

        case 8: // poison!!
        if (player_res_poison() > 0)
        {
                strcpy (info, "You feel slightly nauseous.");
                mpr(info);
                break;
        }
        if (you[0].poison > 0)
        {
                strcpy(info, "You feel even sicker.");
                mpr(info);
        } else
              {
              strcpy (info, "That potion tasted very nasty...");
              mpr(info);
              }
        you[0].poison += random2(3) + random2(3) + 1;
/*      wand_id = 1;*/
        break;

        case 9: // slow
        if (wearing_amulet(36) == 1)
        {
         strcpy(info, "You feel momentarily lethargic.");
         mpr(info);
         break;
        }
        if (you[0].slow == 0) strcpy(info, "You feel yourself slow down.");
                else strcpy(info, "Your low speed feels more durable.");
        mpr(info);
        if (you[0].haste > 0)
        {
                you[0].haste = 0;
        } else you[0].slow += random2(pow) + 10;
/*      wand_id = 1;*/
 if (you[0].slow > 90) you[0].slow = 90;
        break;

        case 10: // paralysis
        if (you[0].paralysis == 0) strcpy(info, "You suddenly lose the ability to move!");
                else strcpy(info, "You still can't move!");
        mpr(info);
        new_value = random2(6) + 2;
 if (new_value > you[0].paralysis) you[0].paralysis = new_value;
/*      wand_id = 1;*/
 if (you[0].paralysis > 13) you[0].paralysis = 13;
        break;

        case 11: // confusion
        if (wearing_amulet(37) == 1)
        {
         strcpy(info, "You feel momentarily bewildered.");
         mpr(info);
         break;
        }
        if (you[0].conf == 0) strcpy(info, "You feel confused.");
                else strcpy(info, "You feel very confused.");
        mpr(info);
        new_value = random2(8) + 3;
 if (new_value > you[0].conf) you[0].conf = new_value;
/*      wand_id = 1;*/
 if (you[0].conf > 20) you[0].conf = 20;
        break;

        case 12: // Invisibility (can I do it?)
        if (you[0].invis == 0)  strcpy(info, "You fade into invisibility!");
                else strcpy(info, "You feel hidden.");
        mpr(info);
        if (you[0].invis >= 0) you[0].invis += random2(pow) + 15;
/*      wand_id = 1;*/
 if (you[0].invis > 100) you[0].invis = 100;
        break;

        case 13: // oatmeal - always gluggy white/grey?
        strcpy(info, "That potion was really gluggy!");
        mpr(info);
        you[0].hunger += 6000;
        you[0].hung_ch = 1;
 if (you[0].hunger > 12000) you[0].hunger = 12000;
        break;

        case 14: // degeneration
        strcpy(info, "There was something very wrong with that potion! ");
        mpr(info);
        lose_stat(100, random2(3) + random2(2) + 1);
        break;

        // Don't generate randomly - should be rare and interesting
        case 15: // decay
        if (you[0].is_undead != 0)
        {
         strcpy(info, "You feel terrible.");
         mpr(info);
         break;
        }
        strcpy(info, "You feel your flesh start to rot away!");
        mpr(info);
        you[0].rotting += random2(10) + 10;
        break;

 case 16: // water
 strcpy(info, "This tastes like water.");
 mpr(info);
 you[0].hunger += 20;
 break;

 case 17: // experience
 strcpy(info, "You feel more experienced!");
 mpr(info);
 if (you[0].xl < 27)
  you[0].xp = exp_needed(you[0].xl + 2, you[0].species) + 1;
 level_change();
 break; // I'll let this slip past robe of archmagi

 case 18: // magic
 strcpy(info, "You feel magical!");
 mpr(info);
        you[0].ep += random2(10) + random2(10) + 5;
/* if (you[0].ep > you[0].ep_max)
 {
  you[0].ep_max += (you[0].ep - you[0].ep_max) / 4 + 1;
  you[0].ep = you[0].ep_max;
 }*/
        you[0].ep_ch = 1;
 break;

 case 19: // restore abilities
 strcpy(info, "You feel refreshed."); // ...can't think of anything better right now
 mpr(info);
 restore_str();
 restore_int();
 restore_dex();
 break;

        case 20: // strong poison!!
        if (player_res_poison() > 0)
        {
                strcpy (info, "You feel extremely nauseous.");
                mpr(info);
                break;
        }
        if (player_res_poison() > 0)
        {
                strcpy(info, "You feel even sicker.");
                mpr(info);
        } else
              {
               strcpy(info, "That potion tasted extremely nasty...");
               mpr(info);
              }
        you[0].poison += random2(7) + random2(7) + 3;
/*      wand_id = 1;*/
        break;

 case 21: // berserk
 if (go_berserk() == 0)
 {
  strcpy(info, "You feel angry!");
  mpr(info);
 }
 break;

 case 22:
 mpr("It has a very clean taste.");
 delete_mutation(100);
 delete_mutation(100);
 delete_mutation(100);
 delete_mutation(100);
 delete_mutation(100);
 delete_mutation(100);
 delete_mutation(100);
 break;

 case 23:
 mpr("You feel extremely strange.");
 mutate(100);
 mutate(100);
 mutate(100);
 break;

} // end of switch (pot_eff)



} // end of void potion_effect(pot_eff)




void unwield_item(char unw)
{

 you[0].special_wield = 0;
 wield_change = 1;

if (you[0].inv_class [unw] == 0 && you[0].inv_dam [unw] > 180)
{
 switch(you[0].inv_dam [unw] - 180)
 {

  case 1:
  strcpy(info, "The Singing Sword sighs.");
  mpr(info);
  break;

  case 2:
  strcpy(info, "You feel less violent.");
  mpr(info);
  break;

  case 3:
  you[0].inv_plus [unw] = 50;
  you[0].inv_plus2 [unw] = 50;
  break;

  case 13: /* staff of Olgreb */
/*                you[0].res_poison --;
                you[0].spec_poison --;
                you[0].mag_abil --; */
                break;

  case 14: /* vampire's tooth */
                you[0].hunger_inc -= 9;
                break;

  case 15: /* staff of Wucad Mu */
  you[0].inv_plus [unw] = 50;
  you[0].inv_plus2 [unw] = 50;
  miscast_effect(19, 9, 90, 100);
  break;

 }
 you[0].special_wield = 0;
 return;
}

if (you[0].inv_class [unw] == 0 && you[0].inv_dam [unw] != 0)
{

if (you[0].inv_ident [unw] == 1) you[0].inv_ident [unw] = 2;

item_name(you[0].inv_plus2 [unw], you[0].inv_class [unw], you[0].inv_type [unw], you[0].inv_dam [unw], you[0].inv_plus [unw], you[0].inv_quant [unw], you[0].inv_ident [unw], 4, str_pass);
strcpy(info, str_pass);

char i_dam = you[0].inv_dam [unw] % 30;

if (you[0].inv_dam [unw] % 30 >= 25)
{
 i_dam = randart_wpn_properties(you[0].inv_class [unw], you[0].inv_type [unw], you[0].inv_dam [unw], you[0].inv_plus [unw], you[0].inv_plus2 [unw], 0, 0);
}

switch(i_dam)
{

  case 1:
  strcat(info, " stops flaming.");
  mpr(info);
  break;

  case 2:
  strcat(info, " stops glowing.");
  mpr(info);
  break;

  case 3:
  strcat(info, " stops glowing.");
  mpr(info);
  break;

  case 4:
  strcat(info, " stops crackling.");
  mpr(info);
  break;

  case 6:
  strcat(info, " stops dripping with poison.");
  mpr(info);
  break;

  case 7:
  strcpy(info, "You feel less protected.");
  mpr(info);
/*                player_AC() -= 5; */
         you[0].AC_ch = 1;
                break;

  /* case 8: draining
     case 9: speed, 10 slicing etc */

  case 13:
  you[0].hunger_inc -= 6;
        //      mpr(info);
                break;

  case 16:
  miscast_effect(20, 9, 90, 100);
  break;

/* when more are added here, *must* duplicate unwielding effect in
    vorpalise weapon scroll effect in read_scoll */


} /* end of switch */

if (you[0].inv_dam [unw] % 30 >= 25)
{
 unuse_randart(unw);
}



if (you[0].duration [5] != 0 || you[0].duration [6] != 0 || you[0].duration [7] != 0 || you[0].duration [8] != 0 || you[0].duration [15] != 0)
{
  you[0].duration [5] = 0;
  you[0].duration [6] = 0;
  you[0].duration [7] = 0;
  you[0].duration [8] = 0;
  you[0].duration [15] = 0;
//  you[0].inv_dam [unw] = (you[0].inv_dam [unw] / 30) * 30;
  you[0].inv_dam [unw] -= you[0].inv_dam [unw] % 30;
  mpr("Your branding spell evaporates.");
}


}


if (you[0].inv_class [you[0].equip [0]] == 11)
{
 switch(you[0].inv_type [you[0].equip [0]])
        {
 case 0: /* wizardry */
/* you[0].mag_abil -= 4; */
        break;

 case 1: /* magic */
 you[0].ep_max -= 13;
 you[0].base_ep2 -= 13;
 if (you[0].ep > you[0].ep_max) you[0].ep = you[0].ep_max;
 you[0].ep_ch = 1;
/* you[0].magic_battery --;*/
 calc_ep();
 break;

 case 2: /* fire */
/*        you[0].spec_fire --;
 player_res_fire(you) --;*/
        break;

        case 3: /* cold */
/* you[0].spec_cold --;
 player_res_cold(you) --;*/
        break;

 case 4:
/* you[0].spec_poison --;
 you[0].res_poison --;*/
        break;

 case 5:
/* you[0].energy --; */
        break;

 case 6:
// you[0].spec_death --;
 break;

 case 7:
// you[0].spec_conj --;
 break;

 case 8:
// you[0].spec_ench --;
 break;

 case 9:
// you[0].spec_summ --;
 break;

 case 25:
 you[0].attribute [1] --;
 you[0].attribute [0] --; /* res elec */
 break;

 case 26:
 you[0].attribute [2] --;
 break;

        }
} /* end if inv_cla == 11 */


} /* end of unwield_item */



void unwear_armour(char unw) /* This does *not* call ev_mod! */
{

you[0].AC_ch = 1;
you[0].evasion_ch = 1;

switch(you[0].inv_type [unw])
{
 case 18:
/* player_res_fire(you) --;
 player_res_cold(you) ++;*/
 break;

 case 19:
/* you[0].rate_regen -= 50;*/
// you[0].hunger_inc -= 1;
 break;

 case 21:
/* player_res_fire(you) ++;
 player_res_cold(you) --;*/
 break;

                 case 27: /* storm dragon */
                 you[0].attribute [0] --;
                 break;

                 case 29: /* gold dragon */
/*                 player_res_fire(you) --;
                 player_res_cold(you) --;
                 you[0].res_poison --;*/
                 break;

}

switch(you[0].inv_dam [unw] % 30)
{

  case 1:
  strcpy(info, "You feel rather sluggish.");
  mpr(info);
/*  you[0].fast_run --; */
  break;

  case 2:
//  player_res_fire(you) --;
  strcpy(info, "You feel less resistant to fire.");
  mpr(info);
  break;

  case 3:
//  player_res_cold(you) --;
  if (player_res_cold() == 0)
  {
          strcpy(info, "You feel less resistant to cold.");
          mpr(info);
  }
  break;

  case 4:
//  you[0].res_poison --;
                if (player_res_poison() == 0)
                {
                        strcpy(info, "You feel less healthy.");
                        mpr(info);
                }
  break;

  case 5:
/*  you[0].see_invis ++; */
                if (player_see_invis() == 0)
                {
                          strcpy(info, "You feel less perceptive.");
                          mpr(info);
                }
  break;

 case 6: /* invisibility */
// you[0].invis ++;
 if (you[0].invis != 0) you[0].invis = 1;
// you[0].hunger_inc -= 7;
 break;

 case 7:
 strcpy(info, "You feel weak.");
 mpr(info);
 you[0].strength -= 3;
 you[0].max_strength -= 3;
 you[0].strength_ch = 1;
 break;

 case 8:
 strcpy(info, "You feel clumsy.");
 mpr(info);
 you[0].dex -= 3;
 you[0].max_dex -= 3;
 you[0].dex_ch = 1;
 break;

 case 9:
 strcpy(info, "You feel dopey."); /* inspired messages, aren't they? */
 mpr(info);
 you[0].intel -= 3;
 you[0].max_intel -= 3;
 you[0].intel_ch = 1;
 break;

 case 10:
 strcpy(info, "You feel rather less ponderous.");
 you[0].speed -= 2;
/* player_evasion(you) += 1;*/
 you[0].evasion_ch = 1;
 break;

        case 11: /* levitation */
//        you[0].lev ++;
        if (you[0].lev != 0) you[0].lev = 1;
        break;

 case 12:
 strcpy(info, "You feel less resistant to magic.");
 mpr(info);
/* you[0].res_magic -= 40;*/
 break;

 case 13:
 strcpy(info, "You feel less protected.");
 mpr(info);
// player_AC(you) -= 3;
 break;

 case 14:
 strcpy(info, "You feel less stealthy.");
 mpr(info);
 break;

 case 15:
 strcpy(info, "You feel all hot and cold.");
 mpr(info);
// player_res_cold(you) --;
// player_res_fire(you) --;
 break;

 case 16:
 strcpy(info, "You feel vulnerable.");
 mpr(info);
// player_prot_life(you) --;
 break;

 case 17:
 strcpy(info, "You feel strangely numb.");
 mpr(info);
/* you[0].mag_abil -= 2;
 you[0].spec_conj --;
 you[0].spec_ench --;
 you[0].spec_summ --;
 you[0].spec_death --;*/
 break;


} /* end of switch */

if (you[0].inv_dam [unw] % 30 >= 25)
{
 unuse_randart(unw);
}

} /* end of unwear_armour */


void unuse_randart(unsigned char unw)
{

 if (randart_wpn_properties(you[0].inv_class [unw], you[0].inv_type [unw], you[0].inv_dam [unw], you[0].inv_plus [unw], you[0].inv_plus2 [unw], 0, 1) > 0)
 {
//  mpr("You feel vulnerable.");
  you[0].AC_ch = 1;
 }
 if (randart_wpn_properties(you[0].inv_class [unw], you[0].inv_type [unw], you[0].inv_dam [unw], you[0].inv_plus [unw], you[0].inv_plus2 [unw], 0, 2) > 0)
 {
//  mpr("You feel like a sitting duck.");
  you[0].evasion_ch = 1;
 }
 if (randart_wpn_properties(you[0].inv_class [unw], you[0].inv_type [unw], you[0].inv_dam [unw], you[0].inv_plus [unw], you[0].inv_plus2 [unw], 0, 3) > 0)
 {
//  mpr("You feel weak.");
  you[0].strength_ch = 1;
  you[0].strength -= randart_wpn_properties(you[0].inv_class [unw], you[0].inv_type [unw], you[0].inv_dam [unw], you[0].inv_plus [unw], you[0].inv_plus2 [unw], 0, 3);
  you[0].max_strength -= randart_wpn_properties(you[0].inv_class [unw], you[0].inv_type [unw], you[0].inv_dam [unw], you[0].inv_plus [unw], you[0].inv_plus2 [unw], 0, 3);
 }
 if (randart_wpn_properties(you[0].inv_class [unw], you[0].inv_type [unw], you[0].inv_dam [unw], you[0].inv_plus [unw], you[0].inv_plus2 [unw], 0, 4) > 0)
 {
//  mpr("You feel dopey.");
  you[0].intel_ch = 1;
  you[0].intel -= randart_wpn_properties(you[0].inv_class [unw], you[0].inv_type [unw], you[0].inv_dam [unw], you[0].inv_plus [unw], you[0].inv_plus2 [unw], 0, 4);
  you[0].max_intel -= randart_wpn_properties(you[0].inv_class [unw], you[0].inv_type [unw], you[0].inv_dam [unw], you[0].inv_plus [unw], you[0].inv_plus2 [unw], 0, 4);
 }
 if (randart_wpn_properties(you[0].inv_class [unw], you[0].inv_type [unw], you[0].inv_dam [unw], you[0].inv_plus [unw], you[0].inv_plus2 [unw], 0, 5) > 0)
 {
//  mpr("You feel clumsy.");
  you[0].dex_ch = 1;
  you[0].dex -= randart_wpn_properties(you[0].inv_class [unw], you[0].inv_type [unw], you[0].inv_dam [unw], you[0].inv_plus [unw], you[0].inv_plus2 [unw], 0, 5);
  you[0].max_dex -= randart_wpn_properties(you[0].inv_class [unw], you[0].inv_type [unw], you[0].inv_dam [unw], you[0].inv_plus [unw], you[0].inv_plus2 [unw], 0, 5);
 }
 if (randart_wpn_properties(you[0].inv_class [unw], you[0].inv_type [unw], you[0].inv_dam [unw], you[0].inv_plus [unw], you[0].inv_plus2 [unw], 0, 8) > 0)
 {
//  mpr("You feel vulnerable to electricity.");
  you[0].attribute [0] --;
 }

}

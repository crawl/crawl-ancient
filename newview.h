#include "fn2.h"
#include "crawlfnc.h"
#include "monsstat.h"

#define MINSEE 11

//#include "\borlandc\crawl\fn4.h"

void item(void);
//void monster_grid(void);
void put_show(void);
//void monster_grid2(void);

//void far monster_grid(unsigned char no_mons, unsigned char mons_x [MNST], unsigned char mons_y [MNST], unsigned char mons_beh [MNST], int invis, char see_invis, int mons_class [MNST], char mcolour [MNST], int x_pos, int y_pos, unsigned char show [15] [15], unsigned char show_col [15] [15], char mons_ench [MNST] [3], char running);

void cloud_grid(void);

void monster_grid(void);
//void aware_monster_grid(void); don't bother

void noisy_2(char loudness); // x_plos & y_plos must have values for this function.

void losight(unsigned int sh [15] [15], unsigned char gr [80] [70], int x_p, int y_p);


void viewwindow(void)
{


//   unsigned char buffy [1500]; //[800]; //392];

   int bufcount = 0;

   unsigned char showed = 0;

   /*for (i = 0; i < 800; i ++)
   {
      buffy [i] = 2;
      if (i % 2 == 1) buffy [i] = 1;
//      if (i < 196) buffy [i] = 3;
   }*/


   /*   14
   14

   16
   40

   40
   100*/


   //if (running != 0) return;
   // ^^^ proving that the display is the slow part!

   //window(2,2,16,16);
        _setcursortype(_NOCURSOR);

//      shadowfunc();

        losight(show, grid, x_pos, y_pos);

//      losight(unsigned char sh [14] [14], unsigned char gr [80] [70], int x_p, int y_p)



        for (count_x = 0; count_x < 14; count_x++)
        {
                for (count_y = 0; count_y < 14; count_y++)
                {
                        show_col [count_x] [count_y] = 7; //grid_col [count_x + x_pos - 6] [count_y + y_pos - 6];
//   map [count_x + x_pos - 7] [count_y + y_pos - 7] = show [count_x - 7] [count_y - 7];
                }
                //strcpy(show_col [count_x], row_of_7s);
        }

        item();

        cloud_grid();

                //monster_grid(); // these two need to be here, not in the input function, due to the for count_x for count_y thing above.

                //monster_grid(no_mons, mons_x, mons_y, mons_beh, invis, see_invis, mons_class, mcolour, x_pos, y_pos, show, show_col, mons_ench, running);

                //monster_grid(no_mons, mons_x, mons_y, mons_beh, invis, see_invis, mons_class, mcolour, x_pos, y_pos, show, show_col, mons_ench, running);

                func_pass [0] = running;

//              monster_grid(no_mons, mons_x, mons_y, mons_beh, invis, see_invis, mons_class, mcolour, x_pos, y_pos, show, show_col, mons_ench, func_pass, mons_sec);
                                                                                                                        monster_grid();

//                aware_monster_grid();

                running = func_pass [0];

/*      for (count_x = 0; count_x < 12; count_x++)
        {
                for (count_y = 0; count_y < 12; count_y++)
                {
                if (show [count_x] [count_y] != 0 && pre_show [count_x] [count_y] != 0)
                        show [count_x] [count_y] = pre_show [count_x] [count_y];
                        if (show [count_x] [count_y] == 0)
                        pre_show [count_x] [count_y] = 0;
                        show_col [count_x] [count_y] = 7;
                }
        } */

 bufcount = 132;

        if (brek == 1)
        {


   // this - 6 bit should be right

        for (count_y = (y_pos - 6); (count_y < y_pos + 7); count_y++)
                {
         bufcount += 20;
      for (count_x = (x_pos - 6); (count_x < x_pos + 7); count_x++)
                {


                //                      gotoxy(count_x - x_pos + 7, count_y - y_pos + 7);
                                //put_show();










//            void put_show(int count_x, int count_y, int x_pos, int y_pos, unsigned char show_col [15] [15], unsigned char show [15] [15], int your_colour, int your_sign)
//{

//showed = show [count_x - x_pos + 7] [count_y - y_pos + 7];


      buffy [bufcount + 1] = show_col [count_x - x_pos + 7] [count_y - y_pos + 7]; //MAGENTA;


if (count_x != x_pos | count_y != y_pos)
{
        //textcolor(show_col [count_x - x_pos + 7] [count_y - y_pos + 7]);
        //if (show [count_x - x_pos + 6] [count_y - y_pos + 6] >= 30 && show [count_x - x_pos + 6] [count_y - y_pos + 6] < 97) show [count_x - x_pos + 6] [count_y - y_pos + 6] -= 30;
        switch (show [count_x - x_pos + 7] [count_y - y_pos + 7])
        {

/*         buffy [bufcount] = show [count_x - x_pos + 7] [count_y - y_pos + 7];
         buffy [bufcount + 1] = show_col [count_x - x_pos + 7] [count_y - y_pos + 7]; //MAGENTA;

         bufcount += 2;*/

         //if (bufcount > 5) abort();


                case 0: showed = 0; break;
                case 1: buffy [bufcount + 1] = rock_colour; //BROWN;
                showed = 177; break; // rock wall
                case 2: buffy [bufcount + 1] = LIGHTGRAY;
                showed = 177; break; // stone wall
                //                ^^^^^
                // later have level variables for each of them, as LIGHTGRAY walls may look slightly silly in hell

                case 3: showed = 254; break; //43; break;
  case 4: showed = 177;
  buffy [bufcount + 1] = CYAN; break;
  case 11: showed = 247;
  buffy [bufcount + 1] = RED; // lava!
  break;
  case 12: showed = 247;
  buffy [bufcount + 1] = BLUE; // water
  break;
                case 17: buffy [bufcount + 1] = floor_colour; //LIGHTGRAY;
                showed = 249; break;
                case 18: showed = 240; break; //60; break; // <
                case 19: showed = 240; buffy [bufcount + 1] = RED; break; // staircase to hell
                case 20: showed = 39; break; // open door
  case 21: showed = 240; buffy [bufcount + 1] = BROWN; break; // branch staircase
/*              case 22: showed = 62; break;
                case 23: showed = 60; break;
                case 24: showed = 62; break;*/
                case 25: buffy [bufcount + 1] = 11;
                showed = 94; break; // ^ dart trap
                case 26: buffy [bufcount + 1] = MAGENTA;
                showed = 94; break;
                case 27: buffy [bufcount + 1] = LIGHTGRAY;
                showed = 94; break;
                case 28: showed = 249; buffy [bufcount + 1] = floor_colour;
  break; // undiscovered trap

// if I change anything above here, must also change magic mapping!

                case 38: showed = 41; break; // weapon )
                case 39: showed = 91; break; // armour [
                case 40: showed = 47; break; // food %
                case 41: showed = 37; break; // wands / &c
                case 42: showed = 43; break; // books +
                case 43: showed = 63; break; // scroll ?
                case 44: showed = 61; break; // ring = etc
                case 45: showed = 33; break; // potions !
                case 46: showed = 40; break; // stones
                case 47: showed = '+'; break; // book +
                case 48: showed = 37; break; // corpses part 1
                case 52: showed = 36; break; // $ gold
                case 97: showed = 97; break; // need to find a new starting number for the monsters, to cope with the +30 grid thing with more than 52 monsters.
                case 98: showed = 98; break;
                case 99: showed = 99; break;
                case 100: showed = 100; break;
                case 101: showed = 101; break;
                case 102: showed = 102; break;
                case 103: showed = 103; break; // g
                case 104: showed = 104; break;
                case 105: showed = 105; break;
                case 106: showed = 106; break; // j
                case 107: showed = 107; break;
                case 108: showed = 'w'; break; // l
                case 109: showed = 109; break; // m
                case 110: showed = 110; break; // n
                case 111: showed = 111; break; // o
                case 112: showed = 112; break;
                case 113: showed = 113; break;
                case 114: showed = 114; break;
                case 115: showed = 115; break;
                case 116: showed = 116; break;
                case 117: showed = 117; break;
                case 118: showed = 118; break;
                case 119: showed = 119; break;
                case 120: showed = 120; break;
                case 121: showed = 121; break;
                case 122: showed = 122; break;
                case 123: showed = 65; break; // A
                case 124: showed = 66; break;
                case 125: showed = 67; break;
                case 126: showed = 68; break;
                        case 127: showed = 79; break; // two-headed Ogre (used to be Ettin, but that was boring)
                case 128: showed = 70; break;
                case 129: showed = 71; break; // G
                case 130: showed = 'g'; break; // g
                case 131: showed = 73; break;
                case 132: showed = 74; break;
                case 133: showed = 75; break; // K
                case 134: showed = 76; break;
                case 135: showed = 77; break;
                case 136: showed = 78; break;
                case 137: showed = 79; break;
                case 138: showed = 80; break;
                case 139: showed = 81; break;
                case 140: showed = 82; break;
                case 141: showed = 83; break;
                case 142: showed = 84; break;
                case 143: showed = 85; break;
                case 144: showed = 86; break;
                case 145: showed = 87; break;
                case 146: showed = 88; break;
                case 147: showed = 89; break;
                case 148: showed = 90; break;
                case 149: showed = 111; break; // orc warrior
                case 150: showed = 75; break; // Kobold demonologist
                case 151: showed = 111; break; // orc wizard
                case 152: showed = 111; break; // orc knight
                case 153: showed = 35; break; // #
      case 154: showed = 'D'; break; // wyvern
          case 155: showed = 'K'; break; // K-lord
//          case 157: showed = 'K'; break;
          case 156: showed = 'G'; break; // Giant eyeball
                                                                                case 157: showed = 'W'; break; // Wight
          case 158: showed = 'P'; break; // o Plant
          case 159: showed = 's'; break; // wolf Spider
                                                                                case 160: showed = ' '; break; // shadow
          case 161: showed = 'p'; break; // hungry ghost
          case 162: showed = 'G'; break; // eye of draining
          case 163: showed = 'b'; break; // butterfly
          case 164: showed = 'f'; break; // wandering mushroom
          case 165: showed = 'E'; break; // efreet
          case 166: showed = 'w'; break; // brain worm
          case 167: showed = 'G'; break; // giant orange brain
          case 168: showed = 'B'; break; // boulder beetle
          case 169: showed = 'z'; break; // flying skull
          case 170: showed = 'h'; break; // hell hound
          case 171: showed = 'm'; break; // minotaur
          case 172: showed = 'D'; break; // ice dragon
          case 173: showed = 'J'; break; // slime thing
          case 174: showed = 'W'; break; // blue wraith
          case 175: showed = 'R'; break; // fake R
          case 176: showed = 'G'; break; // Great orb of eyes
          case 177:
                                                                                case 178:
          case 179:
                                                                                case 180:
          case 181:
                                                                                case 182:
          case 183:
                                                                                case 184:
          showed = 'd'; break; // devils
          case 197: showed = 's'; break; // giant mite
                                                                                case 198: showed = 'D'; break; // steam dragon
          case 199: showed = 'u'; break; // very ugly thing
          case 200: showed = 'o'; break; // orc sorceror
                                                                                case 201: showed = 'H'; break; // hippogriff
          case 202: showed = 'H'; break; // griffon
          case 203: showed = 'D'; break; // hydra

          case MLAVA0 + 97: showed = 'w'; break; // lava worm
          case MLAVA1 + 97: showed = ';'; break; // lava fish
          case MLAVA2 + 97: showed = 'S'; break; // lava snake
          case MWATER0 + 97: showed = ';'; break; // big fish
          case MWATER1 + 97: showed = ';'; break; // giant goldfish
          case MWATER2 + 97: showed = ';'; break; // electrical eel
          case MWATER3 + 97: showed = 'J'; break; // electrical eel
          // change above to a #defined thing
default: showed = show [count_x - x_pos + 7] [count_y - y_pos + 7]; break;



                }
//                              putch(show [count_x - x_pos + 6] [count_y - y_pos + 6]);
} else
      {
//      textcolor (your_colour); // your colour!
//      putch(your_sign); // You!
//                     bufcount [
                       //bufcount += 2;
              showed = your_sign;
              buffy [bufcount + 1] = your_colour;
//              bufcount += 2;
      }
//}  // end of put_show(...)




buffy [bufcount] = showed;
// have the colour defined before the switch, because some cases change it.

bufcount += 2;






                                //put_show(count_x, count_y, x_pos, y_pos, show_col, show, your_colour, your_sign);




                                /*
                                //textcolor(7);
                                textcolor(show_col [count_x - x_pos + 6] [count_y - y_pos + 6]);
                                //if (show [count_x - x_pos + 6] [count_y - y_pos + 6] >= 30 && show [count_x - x_pos + 6] [count_y - y_pos + 6] < 97) show [count_x - x_pos + 6] [count_y - y_pos + 6] -= 30;
                                switch (show [count_x - x_pos + 6] [count_y - y_pos + 6])
                                {
                                case 0: putch(32); break;
                                case 1: textcolor(6); putch(177); break; // rock wall
                                case 2: textcolor(7); putch(177); break; // stone wall
                                case 3: putch(43); break;
                                case 17: putch(249); break;

                                case 18: putch(60); break; // <
                                case 19: putch(62); break; // >
                                case 20: putch(39); break; // open door
                                case 21: putch(60); break;
                                case 22: putch(62); break;
                                case 23: putch(60); break;
                                case 24: putch(62); break;
                                case 25: textcolor(11); putch(94); break; // ^ dart trap

                                case 38: putch(41); break; // weapon )
                                case 39: putch(91); break; // armour [
                                case 40: putch(47); break; // food %
                                case 41: putch(37); break; // wands / &c
                                case 42: putch(43); break; // books +
                                case 43: putch(63); break; // scroll ?
                                case 44: putch(61); break; // ring = etc
                                case 45: putch(33); break; // potions !
                                case 46: putch(40); break; // stones
                                case 47: putch(37); break; // tool (
                                case 48: putch(37); break; // corpses part 1
                                case 52: putch(36); break; // $ gold
                                case 97: putch(97); break; // need to find a new starting number for the monsters, to cope with the +30 grid thing with more than 52 monsters.
                                case 98: putch(98); break;
                                case 99: putch(99); break;
                                case 100: putch(100); break;
                                case 101: putch(101); break;
                                case 102: putch(102); break;
                                case 103: putch(103); break;
                                case 104: putch(104); break;
                                case 105: putch(105); break;
                                case 106: putch(106); break;
                                case 107: putch(107); break;
                                case 108: putch(108); break;
                                case 109: putch(109); break;
                                case 110: putch(110); break;
                                case 111: putch(111); break;
                                case 112: putch(112); break;
                                case 113: putch(113); break;
                                case 114: putch(114); break;
                                case 115: putch(115); break;
                                case 116: putch(116); break;
                                case 117: putch(117); break;
                                case 118: putch(118); break;
                                case 119: putch(119); break;
                                case 120: putch(120); break;
                                case 121: putch(121); break;
                                case 122: putch(122); break;
                                case 123: putch(65); break;
                                case 124: putch(66); break;
                                case 125: putch(67); break;
                                case 126: putch(68); break;
                                case 127: putch(69); break;
                                case 128: putch(70); break;
                                case 129: putch(71); break;
                                case 130: putch(72); break;
                                case 131: putch(73); break;
                                case 132: putch(74); break;
                                case 133: putch(75); break;
                                case 134: putch(76); break;
                                case 135: putch(77); break;
                                case 136: putch(78); break;
                                case 137: putch(79); break;
                                case 138: putch(80); break;
                                case 139: putch(81); break;
                                case 140: putch(82); break;
                                case 141: putch(83); break;
                                case 142: putch(84); break;
                                case 143: putch(85); break;
                                case 144: putch(86); break;
                                case 145: putch(87); break;
                                case 146: putch(88); break;
                                case 147: putch(89); break;
                                case 148: putch(90); break;
                                default: putch (show [count_x - x_pos + 6] [count_y - y_pos + 6]); break;
                                }
//                              putch(show [count_x - x_pos + 6] [count_y - y_pos + 6]);
                        }
         */
                        /*if (count_x == x_pos && count_y == y_pos)
                        {
                                //textcolor (your_colour); // your colour!
                                //putch(64); // You!

            bufcount += 2;
                   }*/
                }
  bufcount += 20;
        }

        window(2,1,35,17);
  bufcount = 132;

if (level_type != 1) // labyrinth
        for (count_y = 0; count_y < 13; count_y++)
        {
  bufcount += 20;
                for (count_x = 0; count_x < 13; count_x++)
                {
   if (buffy [bufcount] != 0) map [count_x + x_pos - 7] [count_y + y_pos - 7] = buffy [bufcount];
   bufcount += 2;
                }
  bufcount += 20;
                //strcpy(show_col [count_x], row_of_7s);
        }


bufcount = 0;
  for (count_y = 0; count_y < 17; count_y ++)
                {
   for (count_x = 0; count_x < 33; count_x ++)
                        {
       if (count_x + x_pos - 17 < 3 | count_y + y_pos - 9 < 3 | count_x + x_pos - 14 > 77 | count_y + y_pos - 9 > 67)
       {
            buffy [bufcount] = 0;
            bufcount += 2;
            continue;
       }
       if (count_x >= 10 && count_x <= 22 && count_y >= 2 && count_y <= 14 && buffy [bufcount] != 0)
       {
            //buffy [bufcount] = 0;
            bufcount += 2;
            continue;
       }
         buffy [bufcount] = map [count_x + x_pos - 17] [count_y + y_pos - 9];
         buffy [bufcount + 1] = DARKGRAY;
                                                                        bufcount += 2;
                        }
                }

if (berserker != 0)
{
 for (count_x = 1; count_x < 1400; count_x += 2)
 {
  buffy [count_x] = RED;
 }
}

if (show_green != 0)
{
 for (count_x = 1; count_x < 1400; count_x += 2)
 {
  buffy [count_x] = show_green;
 }
 show_green = 0;
}

/*
  for (count_y = 0; count_y < 13; count_y ++)
                {
   for (count_x = 0; count_x < 33; count_x ++)
                        {

//     cprintf(map [count_x + x_pos - 14] [count_y + y_pos - 7]);
       if (count_x + x_pos - 17 < 10 | count_y + y_pos - 7 < 10 | count_x + x_pos - 14 > 70 | count_y + y_pos - 7 > 60)
            continue;
//       gotoxy(count_x + 1, count_y + 1);
//       textcolor(DARKGRAY);
//       putch(map [count_x + x_pos - 17] [count_y + y_pos - 7]);
                        }
//   cprintf("\n\r");
                }
*/
   //puttext(2,2,14,14, buffy);
//     puttext(12,2,24,14, buffy);
     puttext(2,1,34,17, buffy);
//      window(2,2,35,14);

        } // end of (if brek...)
// _setcursortype(_NORMALCURSOR);







}



//void item(void)
//{
/*
        item variables:
-       Names of all possible objects in a char array called:
                all_items [20?(name)] [11(class)] [30?(type)]

         0: weapons    }
         1: ammunition }
         2: armour     }   need property information
         3: comestibles}
         4: devices    }
         5: books      }
         6: scrolls           }
         7: rings, amulets etc}   properties require special treatment
         8: potions           }   no need for property variable
         9: stones/gems       }
         10: tools & misc.    }
                Possibly all stored in a file somewhere eventually.

-       Some types of objects have certain other values associated with them:
          Property [4(class)] [30?(type)] [3?(property)]

         weapons have hit & damage
         armour has AC & evasion- Time to wear maybe factored into AC or mass.
         comestibles have food value (and decay?)
         devices have (effects?), max charges
         books have type (spell, reference etc) & spell etc
-       ALL items have mass: mass [11] [30?]


-        Real names of items stored in all_items [20?] [11] [30?] (see above)
-      //       Fake names in fake_name [11] [30?]       - NO.
-       colours stored in it_colour [11] [30?]
         char all_items [20] [11] [30];
unsigned int property [4] [30] [3];
   "     int mass [10] [30];
         // char fake_name [20] [11] [30];
   "     int icolour [11] [30];

                ***     Individual items:       ***
-       All items on level in arrays:   Assuming max 200 items on 1 level:
         int item_class [200]
         int item_type [200]
         int item_plus [200] // +, charges, remaining food value
         int item_dam [200] // damage
         int fake_name [200] // fake names of items
                if item_class = -1, item doesn't exist
         int item_x, item_y

*/






/*

void item(void)
{
int s;
//int temp_show = 0;


//itcheck = 0;

for (s = 0; s < ITEMS; s++)
        {

        if (itc > it_no) break;

        if (item_quant [s] != 0)
        {
        itc++;

        if (item_x [s] > x_pos - 7 && item_x [s] < x_pos + 7 && item_y [s] > y_pos - 7 && item_y [s] < y_pos + 7 && show [item_x [s] - x_pos + 7] [item_y [s] - y_pos + 7] != 0)
        {
        switch(item_class [s])
        {
//      case 0: show [item_x [s] - x_pos + 6] [item_y[s] - y_pos + 6] = 38; break;
                // need + 6 because show is 0 - 12, not -6 - +6
/*
        case 0: temp_show = 38; break;
                // need + 6 because show is 0 - 12, not -6 - +6
        case 1: temp_show = 38; break;
        case 2: temp_show = 39; break;
        case 3: temp_show = 40; break;
        case 4: temp_show = 41; break;
        case 5: temp_show = 42; break;
        case 6: temp_show = 43; break;
        case 7: temp_show = 44; break;
        case 8: temp_show = 45; break;
        case 9: temp_show = 46; break;
        case 10: temp_show = 47; break;
        case 11: temp_show = 48; break;
* //



        case 0: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 38; break;
                // need + 6 because show is 0 - 12, not -6 - +6
        case 1: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 38; break;
        case 2: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 39; break;
        case 3: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 40; break;
        case 4: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 41; break;
        case 5: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 42; break;
        case 6: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 43; break;
        case 7: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 44; break;
        case 8: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 45; break;
        case 9: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 46; break;
        case 10: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 47; break;
        case 11: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 48; break;



        case 15:
        if (show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] < 38 | show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] > 52)
                show [item_x [s] - x_pos + 7] [item_y [s] - y_pos + 7] = 52;
//              {
//                      temp_show = 52;
//              }

// this if() thing stops the $ sign being shown in favour of other items.

        break;
        }

                //show [item_x [s] - x_pos + 6] [item_y [s] - y_pos + 6] = temp_show;



                show_col [item_x [s] - x_pos + 7] [item_y [s] - y_pos + 7] = icolour [s];

                //if (show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] < 38 | show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] > 52)
                if (show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] == 52)
                {
                        if (item_class [s] == 15) show_col [item_x [s] - x_pos + 7] [item_y [s] - y_pos + 7] = 14;
                }
                //if (item_x [s] == x_pos && item_y [s] == y_pos) itcheck = 1;
        } // end of if item_x
        }
        //else // Take this part out because later items which have been picked up reset earlier items.
        //{
        //      grid [item_x [i]] [item_y[i]] = 7;
        //      grid_col [item_x [i]] [item_y [i]] = 7;
        //}
        }
        itc = 0;
} // end of item()
*/


void monster_grid(void)
{

int s; // a loop thing
int mnc = 0;

for (s = 0; s < MNST; s++)
{

//if (mnc > no_mons) break; // can't trust no_mons anymore

if (mons_class [s] != -1)
{
        mnc++;

        if (mons_x [s] > x_pos - 7 && mons_x [s] < x_pos + 7 && mons_y [s] > y_pos - 7 && mons_y [s] < y_pos + 7 && show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] != 0)
        // Put the bit commented out on the previous line back to restore shadow checking for monsters
        {
        if ((mons_beh [s] == 0 | mons_beh [s] == 2) && invis == 0)
 {
  mons_beh [s] = 1; // put stealth/invis here.
  mons_targ_1_x [s] = x_pos;
  mons_targ_1_y [s] = y_pos;
  if (mons_shouts(mons_class [s]) > 0)
  {
   x_plos = x_pos;
   y_plos = y_pos;
//   strcpy(info [info_lines], monam (mons_sec [s], mons_class [s], mons_ench [s] [2], 0));
  // two-headed ogre
  switch(mons_shouts(mons_class [s]))
  {
   case 1: strcpy(info [info_lines], "You hear a shout!"); break;
   case 2: strcpy(info [info_lines], "You hear a bark!"); break;
   case 3: strcpy(info [info_lines], "You hear two shouts!"); break;
   case 4: strcpy(info [info_lines], "You hear a roar!"); break;
   case 5: strcpy(info [info_lines], "You hear a hideous shriek!"); break;
   case 6: strcpy(info [info_lines], "You hear a bellow!"); break;
  }
 info_lines++;
 scrloc++;
 scruggle = 1;
        scrloc = message(info, info_lines, scrloc);
        info_lines = 0;

   noisy_2(5);
//  break;
 }
 }
//      if (mons_ench [s] [2] == 6) && show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] > 70)
        if (mons_ench [s] [2] == 6 && (see_invis == 0 | (mons_class [s] >= MLAVA0 && mons_sec [s] == 1)))
        {
                if (show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] > 50)
                        show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] -= 50;
                continue;
        } else if (mons_beh [s] != 7) func_pass [0] = 0;

        show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] = mons_class [s] + 97;
        show_col [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] = mcolour [mons_class [s]];

        if (mons_class [s] == 23 | mons_class [s] == 49 | mons_class [s] == 66 | mons_class [s] == 80) show_col [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] = mons_sec [s];


//      if (mons_class [s] >= 54) show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] = 151;
// Demons: change 54 in above line to class of first demon


        //grid [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] += 30;
        //mons_targ_1_x [s] = x_pos; // monster is after you now!
        }
}
} // end of for i above


/*
void monster_grid(void)
{

for (i = 0; i < 120; i++)
{
if (mons_class [i] != -1)
{
        if (mons_x [i] > x_pos - 6 && mons_x [i] < x_pos + 6 && mons_y [i] > y_pos - 6 && mons_y [i] < y_pos + 6 && show [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] != 0)
        // Put the bit commented out on the previous line back to restore shadow checking for monsters
        {
        show [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] = mons_class [i] + 97;
        show_col [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] = mcolour [mons_class [i]];
        //grid [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] += 30;
        }
}
} // end of for i above




} // end of monster_grid()

*/

} // really the end of monster_grid
















void item(void)
{
//int s;
//int temp_show = 0;


//itcheck = 0;


//for (count_x = 0; count_x <


for (count_y = (y_pos - 6); (count_y < y_pos + 7); count_y++)
{
        for (count_x = (x_pos - 6); (count_x < x_pos + 7); count_x++)
        {
                //gotoxy(count_x - x_pos + 7, count_y - y_pos + 7);

                if (igrid [count_x] [count_y] != 501)
                {
                        if (show [count_x - x_pos + 7] [count_y - y_pos + 7] != 0)
                        {
                                show_col [count_x - x_pos + 7] [count_y - y_pos + 7] = icolour [igrid [count_x] [count_y]];
                                //  ^ before the switch because of gold pieces
                                switch(item_class [igrid [count_x] [count_y]])
                                {
                                //show [count_x - x_pos + 7] [count_y - y_pos + 7]
                                case 0: show [count_x - x_pos + 7] [count_y - y_pos + 7] = 38; break;
                                // need + 6 because show is 0 - 12, not -6 - +6
                                case 1: show [count_x - x_pos + 7] [count_y - y_pos + 7] = 38; break;
                                case 2: show [count_x - x_pos + 7] [count_y - y_pos + 7] = 39; break;
                                case 3: show [count_x - x_pos + 7] [count_y - y_pos + 7] = 40; break;
                                case 4: show [count_x - x_pos + 7] [count_y - y_pos + 7] = 41; break;
                                case 5: show [count_x - x_pos + 7] [count_y - y_pos + 7] = 42; break;
                                case 6: show [count_x - x_pos + 7] [count_y - y_pos + 7] = 43; break;
                                case 7: show [count_x - x_pos + 7] [count_y - y_pos + 7] = 44; break;
                                case 8: show [count_x - x_pos + 7] [count_y - y_pos + 7] = 45; break;
                                case 9: show [count_x - x_pos + 7] [count_y - y_pos + 7] = 46; break;
                                case 10: show [count_x - x_pos + 7] [count_y - y_pos + 7] = 47; break;
                                case 11: show [count_x - x_pos + 7] [count_y - y_pos + 7] = '\\'; break;
                                case 12: show [count_x - x_pos + 7] [count_y - y_pos + 7] = 236; break;
                                case 15: show [count_x - x_pos + 7] [count_y - y_pos + 7] = 52;
                                         show_col [count_x - x_pos + 7] [count_y - y_pos + 7] = YELLOW; break;
                                default: show [count_x - x_pos + 7] [count_y - y_pos + 7] = '8'; break;
                                }
                        }
                }
        } // end of for count_x
} // end of for count_y

/*


for (s = 0; s < ITEMS; s++)
        {

        if (itc > it_no) break;

        if (item_quant [s] != 0)
        {
        itc++;

        if (item_x [s] > x_pos - 7 && item_x [s] < x_pos + 7 && item_y [s] > y_pos - 7 && item_y [s] < y_pos + 7 && show [item_x [s] - x_pos + 7] [item_y [s] - y_pos + 7] != 0)
        {
        switch(item_class [s])
        {
//      case 0: show [item_x [s] - x_pos + 6] [item_y[s] - y_pos + 6] = 38; break;
                // need + 6 because show is 0 - 12, not -6 - +6
/*
        case 0: temp_show = 38; break;
                // need + 6 because show is 0 - 12, not -6 - +6
        case 1: temp_show = 38; break;
        case 2: temp_show = 39; break;
        case 3: temp_show = 40; break;
        case 4: temp_show = 41; break;
        case 5: temp_show = 42; break;
        case 6: temp_show = 43; break;
        case 7: temp_show = 44; break;
        case 8: temp_show = 45; break;
        case 9: temp_show = 46; break;
        case 10: temp_show = 47; break;
        case 11: temp_show = 48; break;
* //



        case 0: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 38; break;
                // need + 6 because show is 0 - 12, not -6 - +6
        case 1: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 38; break;
        case 2: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 39; break;
        case 3: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 40; break;
        case 4: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 41; break;
        case 5: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 42; break;
        case 6: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 43; break;
        case 7: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 44; break;
        case 8: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 45; break;
        case 9: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 46; break;
        case 10: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 47; break;
        case 11: show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] = 48; break;



        case 15:
        if (show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] < 38 | show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] > 52)
                show [item_x [s] - x_pos + 7] [item_y [s] - y_pos + 7] = 52;
//              {
//                      temp_show = 52;
//              }

// this if() thing stops the $ sign being shown in favour of other items.

        break;
        }

                //show [item_x [s] - x_pos + 6] [item_y [s] - y_pos + 6] = temp_show;



                show_col [item_x [s] - x_pos + 7] [item_y [s] - y_pos + 7] = icolour [s];

                //if (show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] < 38 | show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] > 52)
                if (show [item_x [s] - x_pos + 7] [item_y[s] - y_pos + 7] == 52)
                {
                        if (item_class [s] == 15) show_col [item_x [s] - x_pos + 7] [item_y [s] - y_pos + 7] = 14;
                }
                //if (item_x [s] == x_pos && item_y [s] == y_pos) itcheck = 1;
        } // end of if item_x
        }
        //else // Take this part out because later items which have been picked up reset earlier items.
        //{
        //      grid [item_x [i]] [item_y[i]] = 7;
        //      grid_col [item_x [i]] [item_y [i]] = 7;
        //}
        }
        itc = 0;

*/

} // end of item()

























// All items must have show values >= 38, all grid squares must be < 38
// because of monster invisibility.





/*

void put_show(void)
{
if (count_x != x_pos | count_y != y_pos)
{
        textcolor(show_col [count_x - x_pos + 7] [count_y - y_pos + 7]);
        //if (show [count_x - x_pos + 6] [count_y - y_pos + 6] >= 30 && show [count_x - x_pos + 6] [count_y - y_pos + 6] < 97) show [count_x - x_pos + 6] [count_y - y_pos + 6] -= 30;
        switch (show [count_x - x_pos + 7] [count_y - y_pos + 7])
                {
                case 0: putch(32); break;
                case 1: textcolor(6); putch(177); break; // rock wall
                case 2: textcolor(7); putch(177); break; // stone wall
                case 3: putch(43); break;
                case 17: putch(249); break;
                case 18: putch(60); break; // <
                case 19: putch(62); break; // >
                case 20: putch(39); break; // open door
                case 21: putch(60); break;
                case 22: putch(62); break;
                case 23: putch(60); break;
                case 24: putch(62); break;
                case 25: textcolor(11); putch(94); break; // ^ dart trap
                case 26: textcolor(LIGHTGRAY); putch(94); break;
                case 27: textcolor(MAGENTA); putch(94); break;
                case 28: putch(249); break; // undiscovered trap
                case 38: putch(41); break; // weapon )
                case 39: putch(91); break; // armour [
                case 40: putch(47); break; // food %
                case 41: putch(37); break; // wands / &c
                case 42: putch(43); break; // books +
                case 43: putch(63); break; // scroll ?
                case 44: putch(61); break; // ring = etc
                case 45: putch(33); break; // potions !
                case 46: putch(40); break; // stones
                case 47: putch(37); break; // tool (
                case 48: putch(37); break; // corpses part 1
                case 52: putch(36); break; // $ gold
                case 97: putch(97); break; // need to find a new starting number for the monsters, to cope with the +30 grid thing with more than 52 monsters.
                case 98: putch(98); break;
                case 99: putch(99); break;
                case 100: putch(100); break;
                case 101: putch(101); break;
                case 102: putch(102); break;
                case 103: putch(103); break; // g
                case 104: putch(104); break;
                case 105: putch(105); break;
                case 106: putch(106); break; // j
                case 107: putch(107); break;
                case 108: putch(108); break; // l
                case 109: putch(109); break; // m
                case 110: putch(110); break; // n
                case 111: putch(111); break; // o
                case 112: putch(112); break;
                case 113: putch(113); break;
                case 114: putch(114); break;
                case 115: putch(115); break;
                case 116: putch(116); break;
                case 117: putch(117); break;
                case 118: putch(118); break;
                case 119: putch(119); break;
                case 120: putch(120); break;
                case 121: putch(121); break;
                case 122: putch(122); break;
                case 123: putch(65); break; // A
                case 124: putch(66); break;
                case 125: putch(67); break;
                case 126: putch(68); break;
                case 127: putch(69); break;
                case 128: putch(70); break;
                case 129: putch(71); break; // G
                case 130: putch(72); break;
                case 131: putch(73); break;
                case 132: putch(74); break;
                case 133: putch(75); break; // K
                case 134: putch(76); break;
                case 135: putch(77); break;
                case 136: putch(78); break;
                case 137: putch(79); break;
                case 138: putch(80); break;
                case 139: putch(81); break;
                case 140: putch(82); break;
                case 141: putch(83); break;
                case 142: putch(84); break;
                case 143: putch(85); break;
                case 144: putch(86); break;
                case 145: putch(87); break;
                case 146: putch(88); break;
                case 147: putch(89); break;
                case 148: putch(90); break;
                case 149: putch(111); break; // orc warrior
                case 150: putch(75); break; // Kobold demonologist
                case 151: putch(111); break; // orc wizard
                case 152: putch(103); break; // goblin wizard
                default: putch (show [count_x - x_pos + 7] [count_y - y_pos + 7]); break;
                }
//                              putch(show [count_x - x_pos + 6] [count_y - y_pos + 6]);
} else
      {
        textcolor (your_colour); // your colour!
        putch(your_sign); // You!
      }
}  // end of put_show(void)
*/


































































/*
void monster_grid2(void)
{
int s; // a loop thing
int mnc = 0;

for (s = 0; s < MNST; s++)
{

if (mnc > no_mons) break;

if (mons_class [s] != -1)
{
        mnc++;

        if (mons_x [s] > x_pos - 7 && mons_x [s] < x_pos + 7 && mons_y [s] > y_pos - 7 && mons_y [s] < y_pos + 7) // && show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] != 0)
        // Put the bit commented out on the previous line back to restore shadow checking for monsters
        {
        if ((mons_beh [s] == 0 | mons_beh [s] == 2) && invis == 0) mons_beh [s] = 1; // put stealth/invis here.

//      if (mons_ench [s] [2] == 6) && show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] > 70)
        if (mons_ench [s] [2] == 6 && see_invis == 0)
        {
                if (show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] > 50)
                        show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] -= 50;
                continue;
        }

        show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] = mons_class [s] + 97;
        show_col [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] = mcolour [mons_class [s]];


//      if (mons_class [s] >= 54) show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] = 151;
// Demons: change 54 in above line to class of first demon


        //grid [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] += 30;
        //mons_targ_1_x [s] = x_pos; // monster is after you now!
        }
}
} // end of for i above





} // end of monster_grid()
*/












void cloud_grid(void)
{
int s; // a loop thing
int mnc = 0;

for (s = 0; s < CLOUDS; s++)
{

if (mnc > cloud_no) break;

if (cloud_type [s] != 0)
{
        mnc++;

        if (cloud_x [s] > x_pos - 7 && cloud_x [s] < x_pos + 7 && cloud_y [s] > y_pos - 7 && cloud_y [s] < y_pos + 7 && show [cloud_x [s] - x_pos + 7] [cloud_y [s] - y_pos + 7] != 0)
        // Put the bit commented out on the previous line back to restore shadow checking for cloudters
        {
        //if ((cloud_beh [s] == 0 | cloud_beh [s] == 2) && invis == 0) cloud_beh [s] = 1; // put stealth/invis here.

//      if (cloud_ench [s] [2] == 6) && show [cloud_x [s] - x_pos + 7] [cloud_y [s] - y_pos + 7] > 70)
        //if (cloud_ench [s] [2] == 6 && see_invis == 0)
        //{
        //      if (show [cloud_x [s] - x_pos + 7] [cloud_y [s] - y_pos + 7] > 50)
        //              show [cloud_x [s] - x_pos + 7] [cloud_y [s] - y_pos + 7] -= 50;
        //      continue;
        //} else running = 0;

        show [cloud_x [s] - x_pos + 7] [cloud_y [s] - y_pos + 7] = 35; //cloud_class [s] + 97;

   switch(cloud_type [s] % 100)
   {
        case 1: show_col [cloud_x [s] - x_pos + 7] [cloud_y [s] - y_pos + 7] = RED; break; //mcolour [cloud_class [s]];
        case 2: show_col [cloud_x [s] - x_pos + 7] [cloud_y [s] - y_pos + 7] = GREEN; break; //mcolour [cloud_class [s]];
  case 3: show_col [cloud_x [s] - x_pos + 7] [cloud_y [s] - y_pos + 7] = WHITE; break;
  case 4: show_col [cloud_x [s] - x_pos + 7] [cloud_y [s] - y_pos + 7] = LIGHTGREEN; break;
        case 5: show_col [cloud_x [s] - x_pos + 7] [cloud_y [s] - y_pos + 7] = LIGHTGRAY; break;
  case 6: show_col [cloud_x [s] - x_pos + 7] [cloud_y [s] - y_pos + 7] = LIGHTBLUE; break;
  case 7: show_col [cloud_x [s] - x_pos + 7] [cloud_y [s] - y_pos + 7] = MAGENTA; break;
  case 8: show_col [cloud_x [s] - x_pos + 7] [cloud_y [s] - y_pos + 7] = LIGHTGRAY; break;
   }

//      if (cloud_class [s] >= 54) show [cloud_x [s] - x_pos + 7] [cloud_y [s] - y_pos + 7] = 151;
// Decloud: change 54 in above line to class of first demon


        //grid [cloud_x [i] - x_pos + 6] [cloud_y [i] - y_pos + 6] += 30;
        //cloud_targ_1_x [s] = x_pos; // cloudter is after you now!
        }
}
} // end of for i above


/*
void cloudter_grid(void)
{

for (i = 0; i < 120; i++)
{
if (cloud_class [i] != -1)
{
        if (cloud_x [i] > x_pos - 6 && cloud_x [i] < x_pos + 6 && cloud_y [i] > y_pos - 6 && cloud_y [i] < y_pos + 6 && show [cloud_x [i] - x_pos + 6] [cloud_y [i] - y_pos + 6] != 0)
        // Put the bit commented out on the previous line back to restore shadow checking for cloudters
        {
        show [cloud_x [i] - x_pos + 6] [cloud_y [i] - y_pos + 6] = cloud_class [i] + 97;
        show_col [cloud_x [i] - x_pos + 6] [cloud_y [i] - y_pos + 6] = mcolour [cloud_class [i]];
        //grid [cloud_x [i] - x_pos + 6] [cloud_y [i] - y_pos + 6] += 30;
        }
}
} // end of for i above




} // end of cloudter_grid()

*/












} // end of cloudter_grid()




void noisy_2(char loudness) // x_plos & y_plos must have values for this function.
{

for (p = 0; p < MNST; p ++)
{
        if (mons_x [p] >= x_plos - loudness && mons_x [p] <= x_plos + loudness && mons_y [p] >= y_plos - loudness && mons_y [p] <= y_plos + loudness)
        {
        if (mons_beh [p] == 0) // && random2(3) < 2 // and the monster can hear you.
                mons_beh [p] = 1;

      mons_targ_1_x [p] = x_pos;
      mons_targ_1_y [p] = y_pos;

   }

}

} // end of void noisy()




void losight(unsigned int sh [15] [15], unsigned char gr [80] [70], int x_p, int y_p)
{

char shad;
char see_section;
char startPoint_x; // = 8;
char startPoint_y; // = 7;
char behind = 0;
char xs; // the multiplier of the x addition thing
char ys;
char cx = 0;
char cy = 0;

short int see; // see = 1 means 'visible'











// first comes the horizontal east:
see = 1;
for (cx = (x_p + 1); (cx < x_p + 7); cx++)
        {
                if (gr [cx - 1] [y_p] <MINSEE ) see = 0;
                if (see == 1)
                {
                        sh [cx - x_p + 7] [7] = gr [cx] [y_p];
                } else sh [cx - x_p + 7] [7] = 0;
        }

// now the horizontal West:
see = 1;
for (cx = (x_p - 1); (cx > x_p - 7); cx--)
        {
                if (gr [cx + 1] [y_p] <MINSEE) see = 0;
                if (see == 1)
                {
                        sh [cx - x_p + 7] [7] = gr [cx] [y_p];
                } else sh [cx - x_p + 7] [7] = 0;
        }

// now for the North:
see = 1;
for (cy = (y_p - 1); (cy > y_p - 7); cy--)
        {
                if (gr [x_p] [cy + 1] <MINSEE) see = 0;
                if (see == 1)
                {
                        sh [7] [cy - y_p + 7] = gr [x_p] [cy];
                } else sh [7] [cy - y_p + 7] = 0;
        }

// and the South...

see = 1;
for (cy = (y_p + 1); (cy < y_p + 7); cy++)
        {
                if (gr [x_p] [cy - 1] <MINSEE) see = 0;
                if (see == 1)
                {
                        sh [7] [cy - y_p + 7] = gr [x_p] [cy];
                } else sh [7] [cy - y_p + 7] = 0;
        }

// Try the Southeast:

see = 1;
cy = y_p + 1;
for (cx = x_p + 1; cx < x_p + 7; cx++)
{
        if (gr [cx - 1] [cy - 1] <MINSEE) see = 0;
                if (see == 1)
                {
                        sh [cx - x_p + 7] [cy - y_p + 7] = gr [cx] [cy];
                } else sh [cx - x_p + 7] [cy - y_p + 7] = 0;
cy ++;
}

// Now for the Northeast:

see = 1;
cy = y_p - 1;
for (cx = x_p + 1; cx < x_p + 7; cx++)
{
        if (gr [cx - 1] [cy + 1] <MINSEE) see = 0;
                if (see == 1)
                {
                        sh [cx - x_p + 7] [cy - y_p + 7] = gr [cx] [cy];
                } else sh [cx - x_p + 7] [cy - y_p + 7] = 0;
cy --;
}

// The Northwest:

see = 1;
cy = y_p - 1;
for (cx = x_p - 1; cx > x_p - 7; cx--)
{
        if (gr [cx + 1] [cy + 1] <MINSEE) see = 0;
                if (see == 1)
                {
                        sh [cx - x_p + 7] [cy - y_p + 7] = gr [cx] [cy];
                } else sh [cx - x_p + 7] [cy - y_p + 7] = 0;
cy --;
}

// And the Southwest

see = 1;
cy = y_p + 1;
for (cx = x_p - 1; cx > x_p - 7; cx--)
{
        if (gr [cx + 1] [cy - 1] <MINSEE) see = 0;
                if (see == 1)
                {
                        sh [cx - x_p + 7] [cy - y_p + 7] = gr [cx] [cy];
                } else sh [cx - x_p + 7] [cy - y_p + 7] = 0;
cy ++;
}




// Anyway, now for the Fun part!

see = 1;



// int three; // etc
// int four; // whatever


for (shad = 1; shad < 5; shad++)
{

if (shad == 1)
{
        xs = 1;
        ys = 1;
        startPoint_x = 9; startPoint_y = 8;
}

if (shad == 2)
{
        xs = 1;
        ys = -1;
        startPoint_x = 9; startPoint_y = 6;
}

if (shad == 3)
{
        xs = -1;
        ys = 1;
        startPoint_x = 5; startPoint_y = 8;
}

if (shad == 4)
{
        xs = -1;
        ys = -1;
        startPoint_x = 5; startPoint_y = 6;

}

behind = 0;
see = 1;
see_section = 1;

        if (gr [x_p + xs] [y_p + ys] <MINSEE && gr [x_p + xs] [y_p] <MINSEE) see_section = 0;

see = see_section;

        if (see == 1) sh [startPoint_x] [startPoint_y] = gr [x_p + startPoint_x - 7] [y_p + startPoint_y - 7]; else  sh [startPoint_x] [startPoint_y] = 0;

        if (gr [x_p + startPoint_x - 7] [y_p + startPoint_y - 7] <MINSEE) see = 0;

        if (see == 1) sh [startPoint_x + xs] [startPoint_y + ys] =  gr [x_p + startPoint_x + xs - 7] [y_p + startPoint_y + ys - 7]; else sh [startPoint_x + xs] [startPoint_y + ys] = 0;

        if (gr [x_p + startPoint_x + xs - 7] [y_p + startPoint_y + ys - 7] <MINSEE) see = 0;

        if (see == 1) sh [startPoint_x + 2*xs] [startPoint_y + 2*ys] =  gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y + 2*ys - 7]; else sh [startPoint_x + 2*xs] [startPoint_y + 2*ys] = 0;

        if (gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y + 2*ys - 7] <MINSEE) see = 0;

        if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y + 3*ys] =  gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + 3*ys - 7]; else sh [startPoint_x + 3*xs] [startPoint_y + 3*ys] = 0;

        if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y + 2*ys] =  gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + 2*ys - 7]; else sh [startPoint_x + 3*xs] [startPoint_y + 2*ys] = 0;

// Wider:

        // This is done in a different way: see the >= MINSEE instead of < MINSEE

        if (gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + 3*ys - 7] >= MINSEE) //see = 0;
        {
                if (see == 1) sh [startPoint_x + 4*xs] [startPoint_y + 4*ys] =  gr [x_p + startPoint_x + 4*xs - 7] [y_p + startPoint_y + 4*ys - 7]; else sh [startPoint_x + 4*xs] [startPoint_y + 4*ys] = 0;
        } else sh [startPoint_x + 4*xs] [startPoint_y + 4*ys] = 0;

/*
        if (gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + 2*ys - 7] >= MINSEE)
        {
                if (see == 1) sh [startPoint_x + 4*xs] [startPoint_y + 3*ys] =  gr [x_p + startPoint_x + 4*xs - 7] [y_p + startPoint_y + 3*ys - 7]; else sh [startPoint_x + 4*xs] [startPoint_y + 3*ys] = 0;
        }
*/




// That's one line done...

see_section = 1;

        if (gr [x_p + xs] [y_p + ys] <MINSEE && gr [x_p + xs] [y_p] <MINSEE) see_section = 0;

        if (gr [x_p + 2*xs] [y_p + ys] <MINSEE && gr [x_p + 2*xs] [y_p] <MINSEE) see_section = 0;

see = see_section;

        if (see == 1) sh [startPoint_x + xs] [startPoint_y] = gr [x_p + startPoint_x + xs - 7] [y_p + startPoint_y - 7]; else sh [startPoint_x + xs] [startPoint_y] = 0;

        if (gr [x_p + startPoint_x + xs - 7] [y_p + startPoint_y - 7] <MINSEE) see = 0;

                if (see == 1 && gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y - 7] >= MINSEE)
                {
                        sh [startPoint_x + 3*xs] [startPoint_y + ys] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + ys - 7];
                        // Wider:
                        if (gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7] >= MINSEE)
// To get back to normal, replace the ys on last line ^
                        {
                                /*if (see == 1)*/ sh [startPoint_x + 4*xs] [startPoint_y + ys] = gr [x_p + startPoint_x + 4*xs - 7] [y_p + startPoint_y + ys - 7]; //else sh [startPoint_x + 4*xs] [startPoint_y + ys] = 0;
                        } else sh [startPoint_x + 4*xs] [startPoint_y + ys] = 0;
                        // Okay.
                } else
                {
                        sh [startPoint_x + 3*xs] [startPoint_y + ys] = 0;
                        sh [startPoint_x + 4*xs] [startPoint_y + ys] = 0;
                }

        if (see == 1) sh [startPoint_x + 2*xs] [startPoint_y + ys] = gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y + ys - 7]; else sh [startPoint_x + 2*xs] [startPoint_y + ys] = 0;

        if (gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y + ys - 7] < MINSEE) see = 0;

        if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y + 2*ys] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + 2*ys - 7]; else sh [startPoint_x + 3*xs] [startPoint_y + 2*ys] = 0;

// Wider:

        if (gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + 2*ys - 7] >= MINSEE) //see = 0;
        {
                if (see == 1) sh [startPoint_x + 4*xs] [startPoint_y + 3*ys] = gr [x_p + startPoint_x + 4*xs - 7] [y_p + startPoint_y + 3*ys - 7]; else sh [startPoint_x + 4*xs] [startPoint_y + 3*ys] = 0;
        } else sh [startPoint_x + 4*xs] [startPoint_y + 3*ys] = 0;

/*
        if (gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + 2*ys - 7] >= MINSEE | gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + ys - 7] >= MINSEE) //see = 0;
        {
                if (see == 1) sh [startPoint_x + 4*xs] [startPoint_y + 2*ys] = gr [x_p + startPoint_x + 4*xs - 7] [y_p + startPoint_y + 2*ys - 7]; else sh [startPoint_x + 4*xs] [startPoint_y + 2*ys] = 0;
        } else sh [startPoint_x + 4*xs] [startPoint_y + 2*ys] = 0;
*/

        // This should work better:
//        if (gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + 2*ys - 7] >= MINSEE |
        if (gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + ys - 7] >= MINSEE) //see = 0;
        {
                if (see == 1) sh [startPoint_x + 4*xs] [startPoint_y + 2*ys] = gr [x_p + startPoint_x + 4*xs - 7] [y_p + startPoint_y + 2*ys - 7]; else sh [startPoint_x + 4*xs] [startPoint_y + 2*ys] = 0;
        } else sh [startPoint_x + 4*xs] [startPoint_y + 2*ys] = 0;
            // Look at this ^
//                          |

// Okay.

see = see_section;

//      if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y + ys] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + ys - 7]; else sh [startPoint_x + 3*xs] [startPoint_y + ys] = 0;

// And one more for this section:

        if (gr [x_p + 3*xs] [y_p] < MINSEE && gr [x_p + 3*xs] [y_p + ys] < MINSEE) see_section = 0;

see = see_section;




       //       if (see == 1) sh [startPoint_x + 2*xs] [startPoint_y] = gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y - 7]; else sh [startPoint_x + 2*xs] [startPoint_y] = 0;





//      if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7]; else sh [startPoint_x + 3*xs] [startPoint_y] = 0;

        if (gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y - 7] < MINSEE) see = 0;

//      if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7]; else sh [startPoint_x + 3*xs] [startPoint_y] = 0;

// let's just try this:

//              if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7]; else sh [startPoint_x + 3*xs] [startPoint_y] = 0;



        if (gr [x_p + 3*xs] [y_p + ys] < MINSEE && gr [x_p + 3*xs] [y_p + 2*ys] < MINSEE ) see_section = 0;

        // Wider.
//      if (see == 1 && see_section == 1) sh [startPoint_x + 4*xs] [startPoint_y + ys] = gr [x_p + startPoint_x + 4*xs - 7] [y_p + startPoint_y + ys - 7]; else sh [startPoint_x + 4*xs] [startPoint_y + ys] = 0;
        // Okay.


see = see_section;

/*
        // Wider.
                if (see == 1) sh [startPoint_x + 4*xs] [startPoint_y + ys] = gr [x_p + startPoint_x + 4*xs - 7] [y_p + startPoint_y + ys - 7]; else sh [startPoint_x + 4*xs] [startPoint_y + ys] = 0;
        // Okay.
*/
        if (see == 1) sh [startPoint_x + 2*xs] [startPoint_y] = gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y - 7]; else sh [startPoint_x + 2*xs] [startPoint_y] = 0;

// see = see_section

        if (gr [x_p + 4*xs] [y_p + 2*ys] < MINSEE && gr [x_p + 4*xs] [y_p + ys] < MINSEE) see = 0;


        // Wider.
//      if (see == 1 && see_section == 1) sh [startPoint_x + 4*xs] [startPoint_y + ys] = gr [x_p + startPoint_x + 4*xs - 7] [y_p + startPoint_y + ys - 7]; else sh [startPoint_x + 4*xs] [startPoint_y + ys] = 0;
        // Okay.




//              if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7]; else sh [startPoint_x + 3*xs] [startPoint_y] = 0;





//      if (gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y - 7] < MINSEE) see = 0;


//      if (see == 1) sh [startPoint_x + 3] [startPoint_y + 1] = gr [x_p + startPoint_x + 3 - 7] [y_p + startPoint_y + 1 - 7]; else sh [startPoint_x + 3] [startPoint_y + 1] = 0;




if ((gr [x_p + xs] [y_p] < MINSEE && gr [x_p + xs] [y_p + ys] < MINSEE) | (gr [x_p + 2*xs] [y_p] < MINSEE && gr [x_p + 2*xs] [y_p + ys] < MINSEE) | (gr [x_p + 3*xs] [y_p] < MINSEE && gr [x_p + 3*xs] [y_p + ys] < MINSEE) | (gr [x_p + 4*xs] [y_p] < MINSEE && gr [x_p + 4*xs] [y_p + ys] < MINSEE))
{
        sh [startPoint_x + 3*xs] [startPoint_y] = 0;
        sh [startPoint_x + 4*xs] [startPoint_y] = 0;
} else
        {
                sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7];


        if (gr [x_p + 5*xs] [y_p] < MINSEE && gr [x_p + 5*xs] [y_p + ys] < MINSEE)
        {
                sh [startPoint_x + 4*xs] [startPoint_y] = 0;
        } else sh [startPoint_x + 4*xs] [startPoint_y] = gr [x_p + startPoint_x + 4*xs - 7] [y_p + startPoint_y - 7];


        // Wider:
//              if (gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7] >= MINSEE)
//              {
//                      sh [startPoint_x + 4*xs] [startPoint_y] = gr [x_p + startPoint_x + 4*xs - 7] [y_p + startPoint_y - 7];
//              } else sh [startPoint_x + 4*xs] [startPoint_y] = 0;
        // Okay.

        } // end of else







//              }// else sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7];
//      } //else sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7];
//} //else sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7];


if ((gr [x_p + xs] [y_p] < MINSEE && gr [x_p + xs] [y_p + ys] < MINSEE) | (gr [x_p + 2*xs] [y_p] < MINSEE && gr [x_p + 2*xs] [y_p + ys] < MINSEE) | (gr [x_p + 3*xs] [y_p] < MINSEE && gr [x_p + 3*xs] [y_p + ys] < MINSEE))
{
        sh [startPoint_x + 2*xs] [startPoint_y] = 0;
} else sh [startPoint_x + 2*xs] [startPoint_y] = gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y - 7];





/*

if (gr [x_p + xs] [y_p] < MINSEE && gr [x_p + xs] [y_p + ys] < MINSEE)
{
        if (gr [x_p + 2*xs] [y_p] < MINSEE && gr [x_p + 2*xs] [y_p + ys] < MINSEE)
        {
                if (gr [x_p + 3*xs] [y_p] < 17 && gr [x_p + 3*xs] [y_p + ys] < 17)
                {
                        if (gr [x_p + 4*xs] [y_p] < 17 && gr [x_p + 4*xs] [y_p + ys] < 17)
                        {
                        sh [startPoint_x + 3*xs] [startPoint_y] = 0;
                        } else sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7];
                }// else sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7];
        } //else sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7];
} //else sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7];

*/

} // end of the for (i) above.



// The second lot:

for (shad = 1; shad < 5; shad++)
{

if (shad == 1)
{
        xs = 1;
        ys = 1;
//      three = 3;
        startPoint_x = 8; startPoint_y = 9;
}

if (shad == 2)
{
        xs = 1;
        ys = -1;
        startPoint_x = 8; startPoint_y = 5;
}

if (shad == 3)
{
        xs = -1;
        ys = 1;
        startPoint_x = 6; startPoint_y = 9;
}

if (shad == 4)
{
        xs = -1;
        ys = -1;
        startPoint_x = 6; startPoint_y = 5;

}





behind = 0;
see = 1;
see_section = 1;

        if (gr [x_p + xs] [y_p + ys] < MINSEE && gr [x_p] [y_p + ys] < MINSEE) see_section = 0;

see = see_section;

        if (see == 1) sh [startPoint_x] [startPoint_y] = gr [x_p + startPoint_x - 7] [y_p + startPoint_y - 7]; else  sh [startPoint_x] [startPoint_y] = 0;

        if (gr [x_p + startPoint_x - 7] [y_p + startPoint_y - 7] < MINSEE) see = 0;

        if (see == 1) sh [startPoint_x + xs] [startPoint_y + ys] =  gr [x_p + startPoint_x + xs - 7] [y_p + startPoint_y + ys - 7]; else sh [startPoint_x + xs] [startPoint_y + ys] = 0;

        if (gr [x_p + startPoint_x + xs - 7] [y_p + startPoint_y + ys - 7] < MINSEE) see = 0;

        if (see == 1) sh [startPoint_x + 2*xs] [startPoint_y + 2*ys] =  gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y + 2*ys - 7]; else sh [startPoint_x + 2*xs] [startPoint_y + 2*ys] = 0;

        if (gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y + 2*ys - 7] < MINSEE) see = 0;

        if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y + 3*ys] =  gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + 3*ys - 7]; else sh [startPoint_x + 3*xs] [startPoint_y + 3*ys] = 0;

        if (see == 1) sh [startPoint_x + 2*xs] [startPoint_y + 3*ys] =  gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y + 3*ys - 7]; else sh [startPoint_x + 2*xs] [startPoint_y + 3*ys] = 0;

// Wider:

//      if (gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + 3*ys - 7] <MINSEE) see = 0;

//      if (see == 1) sh [startPoint_x + 4*xs] [startPoint_y + 4*ys] =  gr [x_p + startPoint_x + 4*xs - 7] [y_p + startPoint_y + 4*ys - 7]; else sh [startPoint_x + 4*xs] [startPoint_y + 4*ys] = 0;

        if (gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + 3*ys - 7] >= MINSEE) //see = 0;
        {
                if (see == 1) sh [startPoint_x + 4*xs] [startPoint_y + 4*ys] =  gr [x_p + startPoint_x + 4*xs - 7] [y_p + startPoint_y + 4*ys - 7]; else sh [startPoint_x + 4*xs] [startPoint_y + 4*ys] = 0;
        } else sh [startPoint_x + 4*xs] [startPoint_y + 4*ys] = 0;

// Okay.

// That's one line done...

        if (gr [x_p + xs] [y_p + ys] < MINSEE && gr [x_p] [y_p + ys] < MINSEE) see_section = 0;

        if (gr [x_p + xs] [y_p + 2*ys] < MINSEE && gr [x_p] [y_p + 2*ys] < MINSEE) see_section = 0;

see = see_section;

        if (see == 1) sh [startPoint_x] [startPoint_y + ys] = gr [x_p + startPoint_x - 7] [y_p + startPoint_y + ys - 7]; else sh [startPoint_x] [startPoint_y + ys] = 0;

        if (gr [x_p + startPoint_x - 7] [y_p + startPoint_y + ys - 7] < MINSEE) see = 0;

//      if (see == 1 && gr [x_p + startPoint_x - 7] [y_p + startPoint_y + 2*ys - 7] > 7) sh [startPoint_x + xs] [startPoint_y + 3*ys] = gr [x_p + startPoint_x + xs - 7] [y_p + startPoint_y + 3*ys - 7]; else sh [startPoint_x + xs] [startPoint_y + 3*ys] = 0;
//              if (see == 1 && gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y - 7] != MINSEE7) sh [startPoint_x + 3*xs] [startPoint_y + ys] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + ys - 7]; else sh [startPoint_x + 3*xs] [startPoint_y + ys] = 0;


        if (see == 1 && gr [x_p + startPoint_x - 7] [y_p + startPoint_y + 2*ys - 7] >= MINSEE)
                {
                        sh [startPoint_x + xs] [startPoint_y + 3*ys] = gr [x_p + startPoint_x + xs - 7] [y_p + startPoint_y + 3*ys - 7];
                        // Wider:
                        if (gr [x_p + startPoint_x - 7] [y_p + startPoint_y + 3*ys - 7] >= MINSEE)
// To get back to normal, replace the xs on last line ^
                        {
                                /*if (see == 1)*/ sh [startPoint_x + xs] [startPoint_y + 4*ys] = gr [x_p + startPoint_x + xs - 7] [y_p + startPoint_y + 4*ys - 7]; //else sh [startPoint_x + 4*xs] [startPoint_y + ys] = 0;
                        } else sh [startPoint_x + xs] [startPoint_y + 4*ys] = 0;
                        // Okay.
                } else
                {
                        sh [startPoint_x + xs] [startPoint_y + 3*ys] = 0;
                        sh [startPoint_x + xs] [startPoint_y + 4*ys] = 0;
                }


        if (see == 1) sh [startPoint_x + xs] [startPoint_y + 2*ys] = gr [x_p + startPoint_x + xs - 7] [y_p + startPoint_y + 2*ys - 7]; else sh [startPoint_x + xs] [startPoint_y + 2*ys] = 0;

        if (gr [x_p + startPoint_x + xs - 7] [y_p + startPoint_y + 2*ys - 7] < MINSEE) see = 0;

        if (see == 1 && behind == 0) sh [startPoint_x + 2*xs] [startPoint_y + 3*ys] = gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y + 3*ys - 7]; else sh [startPoint_x + 2*xs] [startPoint_y + 3*ys] = 0;


// Wider:

        if (gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y + 3*ys - 7] >= MINSEE) //see = 0;
        {
                if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y + 4*ys] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y + 4*ys - 7]; else sh [startPoint_x + 3*xs] [startPoint_y + 4*ys] = 0;
        } else sh [startPoint_x + 3*xs] [startPoint_y + 4*ys] = 0;


        if (gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y + 3*ys - 7] >= MINSEE | gr [x_p + startPoint_x + xs - 7] [y_p + startPoint_y + 3*ys - 7] >= MINSEE) //see = 0;
        {
                if (see == 1) sh [startPoint_x + 2*xs] [startPoint_y + 4*ys] = gr [x_p + startPoint_x + 2*xs - 7] [y_p + startPoint_y + 4*ys - 7]; else sh [startPoint_x + 2*xs] [startPoint_y + 4*ys] = 0;
        } else sh [startPoint_x + 2*xs] [startPoint_y + 4*ys] = 0;

// Okay.


see = see_section;

// And one more for this section:

        if (gr [x_p ] [y_p + 3*ys] < MINSEE && gr [x_p + xs] [y_p + 3*ys] < MINSEE) see_section = 0;

see = see_section;

        if (see == 1) sh [startPoint_x ] [startPoint_y + 2*ys] = gr [x_p + startPoint_x - 7] [y_p + startPoint_y + 2*ys - 7]; else sh [startPoint_x ] [startPoint_y + 2*ys] = 0;

//      if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7]; else sh [startPoint_x + 3*xs] [startPoint_y] = 0;

        if (gr [x_p + startPoint_x - 7] [y_p + startPoint_y + 2*ys - 7] < MINSEE) see = 0;

//      if (see == 1) sh [startPoint_x + 3*xs] [startPoint_y] = gr [x_p + startPoint_x + 3*xs - 7] [y_p + startPoint_y - 7]; else sh [startPoint_x + 3*xs] [startPoint_y] = 0;

// let's just try this:

        if (gr [x_p + xs] [y_p + 3*ys] < MINSEE && gr [x_p + 2*xs] [y_p + 3*ys] < MINSEE) see_section = 0;

see = see_section;

    //  if (see == 1) sh [startPoint_x ] [startPoint_y + 2*ys ] = gr [x_p + startPoint_x - 7] [y_p + startPoint_y + 2*ys - 7]; else sh [startPoint_x] [startPoint_y + 2*ys] = 0;

see = see_section;

        if (gr [x_p + 2*xs] [y_p + 4*ys] < MINSEE && gr [x_p + xs] [y_p + 4*ys] < MINSEE) see = 0;

       //       if (see == 1) sh [startPoint_x] [startPoint_y + 3*ys] = gr [x_p + startPoint_x - 7] [y_p + startPoint_y + 3*ys - 7]; else sh [startPoint_x] [startPoint_y + 3*ys] = 0;

       //       if (gr [x_p + startPoint_x - 7] [y_p + startPoint_y + 2*ys - 7] < MINSEE) see = 0;


//      if (see == 1) sh [startPoint_x + 3] [startPoint_y + 1] = gr [x_p + startPoint_x + 3 - 7] [y_p + startPoint_y + 1 - 7]; else sh [startPoint_x + 3] [startPoint_y + 1] = 0;


/*
if ((gr [x_p] [y_p + ys] < MINSEE && gr [x_p + xs] [y_p + ys] < MINSEE) | (gr [x_p] [y_p + 2*ys] < MINSEE && gr [x_p + xs] [y_p + 2*ys] < MINSEE) | (gr [x_p] [y_p + 3*ys] < 17 && gr [x_p + xs] [y_p + 3*ys] < 17) | (gr [x_p] [y_p + 4*ys] < 17 && gr [x_p + xs] [y_p + 4*ys] < 17))
{
        sh [startPoint_x] [startPoint_y + 3*ys] = 0;
} else sh [startPoint_x] [startPoint_y + 3*ys] = gr [x_p + startPoint_x - 7] [y_p + startPoint_y + 3*ys - 7];
*/



if ((gr [x_p] [y_p + ys] < MINSEE && gr [x_p + xs] [y_p + ys] < MINSEE) | (gr [x_p] [y_p + 2*ys] < MINSEE && gr [x_p + xs] [y_p + 2*ys] < MINSEE) | (gr [x_p] [y_p + 3*ys] < MINSEE && gr [x_p + xs] [y_p + 3*ys] < MINSEE))
{
        sh [startPoint_x] [startPoint_y + 2*ys] = 0;
} else sh [startPoint_x] [startPoint_y + 2*ys] = gr [x_p + startPoint_x - 7] [y_p + startPoint_y + 2*ys - 7];



if ((gr [x_p] [y_p + ys] < MINSEE && gr [x_p + xs] [y_p + ys] < MINSEE) | (gr [x_p] [y_p + 2*ys] < MINSEE && gr [x_p + xs] [y_p + 2*ys] < MINSEE) | (gr [x_p] [y_p + 3*ys] < MINSEE && gr [x_p + xs] [y_p + 3*ys] < MINSEE) | (gr [x_p] [y_p + 4*ys] < MINSEE && gr [x_p + xs] [y_p + 4*ys] < MINSEE))
{
        sh [startPoint_x] [startPoint_y + 3*ys] = 0;
        sh [startPoint_x] [startPoint_y + 4*ys] = 0;
} else
        {
                sh [startPoint_x] [startPoint_y + 3*ys] = gr [x_p + startPoint_x - 7] [y_p + startPoint_y + 3*ys - 7];


        if (gr [x_p] [y_p + 5*ys] < MINSEE && gr [x_p] [y_p + 5*ys] < MINSEE)
        {
                sh [startPoint_x] [startPoint_y + 4*ys] = 0;
        } else sh [startPoint_x] [startPoint_y + 4*ys] = gr [x_p + startPoint_x - 7] [y_p + startPoint_y + 4*ys - 7];

        } // end of else













} // end of the for (shad) above.














/*
for (cx = 0; cx < 12; cx++)
        {
                for (cy = 0; cy < 12; cy++)
                {
                        pre_sh [cx] [cy] = sh [cx] [cy];
                }
        }
*/

} // end of shadowFunc(void)


/*
void aware_monster_grid(void)
{

int s; // a loop thing
int mnc = 0;

for (s = 0; s < MNST; s++)
{

if (mnc > no_mons) break;

if (mons_class [s] != -1)
{
        mnc++;

        if (mons_x [s] > x_pos - 7 && mons_x [s] < x_pos + 7 && mons_y [s] > y_pos - 7 && mons_y [s] < y_pos + 7 && show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] == 0)
        {

        if (mons_intel(mons_class [s]) == 0)
        {
                if (show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] > 50 && show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] != 0)
                        show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] -= 50;
                continue;
        } else func_pass [0] = 0;

        show [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] = mons_class [s] + 97;
        show_col [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] = mcolour [mons_class [s]];

        if (mons_class [s] == 49) show_col [mons_x [s] - x_pos + 7] [mons_y [s] - y_pos + 7] = mons_sec [s];
        }
}
} // end of for i above

} // really the end of monster_grid
*/

#include "crawlfnc.h"
#include "fn2.h"

#include "builder2.h"




void load(void);
/*
void save_one(void);
void save_two(void);
void save_three(void);
void save_four(void);
void save_six(void);
void save_seven(void);     // Have to put in an arror message if file not found etc.
void save_eight(void);
void save_nine(void);
void save_ten(void);
*/
void screen_show(void);
void save_level(void);

void save_game(void);
void restore_game(void);

void reset_ch(void);

/*
int x_pos = 10;
int y_pos = 10;
int count_x;
int count_y;
unsigned char far grid [80] [70]; // this is what is actually there
                        // Have the two dimensions different (ie 80 & 70) so that the Replace command can distinguish them.
unsigned char show [12] [12]; // this is just what you see
int shad;
int see_section;
int startPoint_x; // = 8;
int startPoint_y; // = 7;
int behind = 0;
int xs; // the multiplier of the x addition thing
int ys;
*/



//int count_x, count_y;
//unsigned char far grid [80] [70];

int temp_int;
char ting_quant [15];


void load(void)
{
int gloj = 0;
int handle;
//char load_file [12];
//unsigned char whole_line [100];
unsigned char rm [54];
//unsigned char cm [1];
//char zm [30];
//int rm;
//int cm;
char cha_fil [80];

char ting_quant [12];

char curr_level = (char) your_level + 48;

char corr_level [4];

//char blobl = &curr_level;

itoa(your_level, corr_level, 10);




window(1,1,80,25);





//char char_fil [15];
/*
switch(your_level)
{
case 0: strcpy(cha_fil, "lev000.dat"); break;
case 1: strcpy(cha_fil, "lev001.dat"); break;
case 2: strcpy(cha_fil, "lev002.dat"); break;
case 3: strcpy(cha_fil, "lev003.dat"); break;
case 4: strcpy(cha_fil, "lev004.dat"); break;
case 5: strcpy(cha_fil, "lev005.dat"); break;
case 6: strcpy(cha_fil, "lev006.dat"); break;
case 7: strcpy(cha_fil, "lev007.dat"); break;
case 8: strcpy(cha_fil, "lev008.dat"); break;
case 9: strcpy(cha_fil, "lev009.dat"); break;
}*/

char extens [3];

//strcpy(cha_fil, "c:\\borlandc\\crawl\\levels\\");

strcpy(cha_fil, "");
strcat(cha_fil, your_name);
strcat(cha_fil, ".lev");
//strcat(cha_fil, corr_level);

//itoa(your_level, extens, 10));





        int sysg = 0;





handle = open(cha_fil, S_IWRITE, S_IREAD);




if (handle == -1)
{

        //sysg = 0;

//      sysg = system("cd ..");

//      if (sysg == -1)
//      {
//              perror("Oh dear... ");
//              exit(0);
//      }

        char eggo [3];
 unsigned char it0 [30];
 unsigned char it1 [30];
 unsigned char it2 [30];
 unsigned char it3 [30];
 unsigned char it4 [30];

/* strcpy(it0, item_description [0]);
 strcpy(it1, item_description [1]);
 strcpy(it2, item_description [2]);
 strcpy(it3, item_description [3]);*/

        //eggo [0] = (char) your_level;

/*
for (i = 0; i < 25; i ++)
{
  itoa(item_description [0] [i], eggo, 10);
  cprintf("\n\rdesc:");
  cprintf(eggo);
}

   getch();
*/

   itoa(your_level, eggo, 10);


        //cout << cha_fil;

//      gloj = spawnl(P_WAIT, "c:\\borlandc\\crawl\\create.exe", cha_fil, cha_fil, cha_fil, cha_fil, NULL);


//      gloj = spawnl(P_WAIT, "createz.exe", eggo, cha_fil, eggo, eggo, item_description [0], item_description [1], item_description [2], item_description [3], item_description [4], NULL);
 // ^^^ old one before builder.pp

//      gloj = spawnl(P_WAIT, "createz.exe", eggo, cha_fil, eggo, eggo, it0, it1, it2, it3, NULL);

cprintf("00");

gloj = builder(
igrid,
grid,
mons_class,
mons_hp,
mons_hp_max,
mons_HD,
mons_AC,
mons_ev,
mons_speed,
mons_speed_inc,
mons_x,
mons_y,
        mons_beh,
        mons_inv,
        mons_targ_1_x,
        mons_targ_1_y,
        mons_sec,
 mons_ench_1,
 mons_ench,
 mons_hit,
it_no,
item_ident,
item_class,
unique_items,
item_type,
item_plus,
item_dam,
item_quant,
item_x,
item_y,
icolour,
item_link,
item_description,
property,
trap_type,
trap_x,
trap_y,
mons_alloc,
gmon_use,
stair_x,
stair_y,
your_level,
x_pos,
y_pos,
level_type);

for (i = 0; i < MNST; i ++)
{
 if (mons_class [i] == 255) mons_class [i] = -1;
   else
       {
         mn_no ++;
         no_mons ++;
         grid [mons_x [i]] [mons_y [i]] += 50;
       }
}

for (i = 0; i < 80; i ++)
{
 for (j = 0; j < 70; j ++)
 {
  map [i] [j] = 0;
  if (char_direction == 1 && grid [i] [j] == 19) grid [i] [j] = 18;
 }
}

 /*builder(igrid, grid, item_description, it_no, item_ident, item_class, item_type,
item_plus, item_dam, item_quant, item_x, item_y, icolour, item_link,
mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev, mons_speed,
mons_speed_inc, mons_x, mons_y, mons_beh, mons_inv, mons_targ_1_x,
mons_targ_1_y, mons_sec, mons_ench_1, mons_ench, igrid, grid, property,
trap_type, trap_x, trap_y, stair_x, stair_y, your_level);*/



//        getch();

        /*
        cprintf("\n\rWelcome back, ");
        cprintf(your_nam);
        cprintf("!");
        close(handle);
        return 0;
        */

        if (gloj == -1)
        {
           perror("Error from spawnl");
           exit(-1);
        }


        levels_exist [your_level] = 1;

reset_ch();
                                                draw_border(BROWN, your_name, clasnam);
//replace_items(item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y);


 return;

}


close(handle);



        sysg = 0;

        //sysg = system("cd c:\borlandc\crawl\levels");

        if (sysg == -1)
        {
                perror("Oh dear... ");
                exit(0);
        }









//rm [0] = getch();


//curr_level -= 48;






//system("cd\borlandc");
/*
switch (curr_level)
{
        case 0: handle = open("lev000.dat", O_RDONLY, S_IWRITE, S_IREAD); break;
//      strcpy(load_file, "lev000.dat"); break;
        case 1: handle = open("lev001.dat", O_RDONLY, S_IWRITE, S_IREAD); break;
  //    strcpy(load_file, "lev001.dat"); break;
        case 2: handle = open("lev002.dat", O_RDONLY, S_IWRITE, S_IREAD); break;
    //  strcpy(load_file, "lev002.dat"); break;
        case 3: handle = open("lev003.dat", O_RDONLY, S_IWRITE, S_IREAD); break;
      //strcpy(load_file, "lev003.dat"); break;
        case 4: handle = open("lev004.dat", O_RDONLY, S_IWRITE, S_IREAD); break;
//      strcpy(load_file, "lev004.dat"); break;
        case 5: handle = open("lev005.dat", O_RDONLY, S_IWRITE, S_IREAD); break;
  //    strcpy(load_file, "lev005.dat"); break;
        case 6: handle = open("lev006.dat", O_RDONLY, S_IWRITE, S_IREAD); break;
    //  strcpy(load_file, "lev006.dat"); break;
        case 7: handle = open("lev007.dat", O_RDONLY, S_IWRITE, S_IREAD); break;
      //strcpy(load_file, "lev007.dat"); break;
        case 8: handle = open("lev008.dat", O_RDONLY, S_IWRITE, S_IREAD); break;
//      strcpy(load_file, "lev008.dat"); break;
        case 9: handle = open("lev009.dat", O_RDONLY, S_IWRITE, S_IREAD); break;

}
//strcpy(load_file, "");
*/

/*
switch(your_level)
{
case 0: strcpy(cha_fil, "lev000.dat"); break;
case 1: strcpy(cha_fil, "lev001.dat"); break;
case 2: strcpy(cha_fil, "lev002.dat"); break;
case 3: strcpy(cha_fil, "lev003.dat"); break;
case 4: strcpy(cha_fil, "lev004.dat"); break;
case 5: strcpy(cha_fil, "lev005.dat"); break;
case 6: strcpy(cha_fil, "lev006.dat"); break;
case 7: strcpy(cha_fil, "lev007.dat"); break;
case 8: strcpy(cha_fil, "lev008.dat"); break;
case 9: strcpy(cha_fil, "lev009.dat"); break;
}
*/



handle = open(cha_fil, O_RDONLY, S_IWRITE, S_IREAD);

//handle = open(cha_fil, O_RDONLY | O_BINARY, S_IWRITE | S_IREAD);


if (handle == -1)
{
        perror("Errghor");
        exit(0);
}




//handle = open(load_file, O_RDONLY, S_IWRITE, S_IREAD); // with O_CREATE, will not change already existing file, ie must delete to refresh.
if (handle > -1)
{

for (count_x = 0; count_x < ITEMS; count_x ++)
{
        item_link [count_x] = 501;
}


for (i = 0; i < 80; i ++)
{
        for (j = 0; j < 70; j ++)
        {
                igrid [i] [j] = 501;
        }
}



for (count_x = 0; count_x < 80; count_x ++)
{
        for (count_y = 0; count_y < 70; count_y ++)
        {
//      read(handle, whole_line, 100);
        //strcpy(grid [count_x], whole_line);
        read(handle, rm, 2);

//              igrid [count_x] [count_y] = 501;

        grid [count_x] [count_y] = rm [0] - 27; //(int) rm [0];
 map [count_x] [count_y] = rm [1]; //(int) rm [1];
        }
}


for (i = 0; i < NTRAPS; i ++)
{
        read(handle, rm, 3);
        trap_type [i] = rm [0] - 40;
        trap_x [i] = rm [1] - 40;
        trap_y [i] = rm [2] - 40;

//      cout << " trap: " << (int) trap_type [i] << "," << (int) grid [trap_x [i]] [trap_y [i]] << " ";

/*      rm [0] = trap_type [i];
        rm [1] = trap_x [i];
        rm [2] = trap_y [i];
        write(handle, rm, 3);*/
}
/*
read(handle, rm, 10);
for (i = 0; i < 10; i += 2)
{
 stair_x [i] = rm [i] - 40;
 stair_y [i] = rm [i + 1] - 40;
}
*/

//keyin = getch();




/*
for (count_x = 0; count_x < 80; count_x ++)
{
        for (count_y = 0; count_y < 100; count_y ++)
        {
                read(handle, rm, 1);
                grid [count_x] [count_y] = rm [0];
        }
}

*/

no_mons = 0;

//}
for (count_x = 0; count_x < MNST; count_x ++)
{
//      read(handle, rm, 1);

        read(handle, rm, 18);
               //       if (rm [0] < 10) read(handle, cm, 1);
               //       rm [0] = rm [0] * 10 + cm [0];
                mons_class [count_x] = rm [0] - 40;
/*              if (mons_class [count_x] == 215) mons_class [count_x] = -1;
                        else
                        {
                                no_mons++;
                                mn_no++;
                        }*/
//              cout << (int) mons_class [count_x] << ", ";

//              mons_class [count_x] = 44; Spectre
//      read(handle, rm, 1);
//      read(handle, rm, 1);
               //       if (rm [0] < 10) read(handle, cm, 1);
               //       rm [0] = rm [0] * 10 + cm [0];
                mons_hp [count_x] = rm [1] - 40;
//      read(handle, rm, 1);
//      read(handle, rm, 1);
               //       if (rm [0] < 10) read(handle, cm, 1);
               //       rm [0] = rm [0] * 10 + cm [0];
                mons_AC [count_x] = rm [2] - 40;

//      read(handle, rm, 1);
//      read(handle, rm, 1);
               //       if (rm [0] < 10) read(handle, cm, 1);
               //       rm [0] = rm [0] * 10 + cm [0];
                mons_hp_max [count_x] = rm [3] - 40;
//      read(handle, rm, 1);
//      read(handle, rm, 1);
               //       if (rm [0] < 10) read(handle, cm, 1);
               //       rm [0] = rm [0] * 10 + cm [0];
                mons_ev [count_x] = rm [4] - 40;
//      read(handle, rm, 1);
//      read(handle, rm, 1);
               //       if (rm [0] < 10) read(handle, cm, 1);
               //       rm [0] = rm [0] * 10 + cm [0];
                mons_HD [count_x] = rm [5] - 40;
//      read(handle, rm, 1);
//      read(handle, rm, 1);

                mons_speed [count_x] = rm [6] - 40;
//              mons_speed [count_x] /= 10;

//      read(handle, rm, 1);

                mons_speed_inc [count_x] = rm [7] - 40;

    //  read(handle, rm, 1);

      //                mons_speed_inc [count_x] = rm [0] - 100;


//              mons_speed_inc [count_x] /= 10;

//      read(handle, rm, 1);

        mons_beh [count_x] = rm [8] - 40;


//      read(handle, rm, 1);
//      if (strlen(rm) == 1) read(handle, cm, 1);
//              if (rm [0] < 10) read(handle, cm, 1);
//              rm [0] = rm [0] * 10 + cm [0];
                mons_x [count_x] = rm [9] - 40;
//      read(handle, rm, 1);
//      read(handle, rm, 1);
//              if (rm [0] < 10) read(handle, cm, 1);
//              rm [0] = rm [0] * 10 + cm [0];
                mons_y [count_x] = rm [10] - 40;
/*
itoa(mons_class [count_x], st_prn, 10);
cprintf(st_prn);
cprintf(", ");
itoa(mons_speed [count_x], st_prn, 10);
cprintf(st_prn);
cprintf(": ");
*/


                mons_targ_1_x [count_x] = rm [11] - 40;
                mons_targ_1_y [count_x] = rm [12] - 40;

                mons_sec [count_x] = rm [13] - 40;

  mons_ench_1 [count_x] = rm [14] - 40;

  for (j = 0; j < 3; j++)
  {
    mons_ench [count_x] [j] = rm [15 + j] - 40;
  }

 read(handle, rm, 5);
        mons_class [count_x] = (rm [4] - 48) + (rm [3] - 48) * 10 + (rm [2] - 48) * 100 + (rm [1] - 48) * 1000;// + (rm [1] - 48) * 10000;
 mons_class [count_x] -= 80;
        if (mons_class [count_x] == -1) no_mons += 0;
                        else
                        {
                                no_mons++;
                                mn_no++;
                        }
 itoa(mons_class [count_x], st_prn, 10);
 cprintf(st_prn);
 cprintf(",");
/*              if (mons_class [count_x] == 215) mons_class [count_x] = -1;
                        else
                        {
                                no_mons++;
                                mn_no++;
                        }*/
/*
 read(handle, rm, 5);
        mons_hp [count_x] = (rm [4] - 48) + (rm [3] - 48) * 10 + (rm [2] - 48) * 100 + (rm [1] - 48) * 1000;// + (rm [1] - 48) * 10000;

 read(handle, rm, 5);
        mons_hp_max [count_x] = (rm [4] - 48) + (rm [3] - 48) * 10 + (rm [2] - 48) * 100 + (rm [1] - 48) * 1000;// + (rm [1] - 48) * 10000;
*/
  int temp_int = 0;
//      cout << " " << count_x << ":";
 strcpy(ting_quant, "");
        for (count_y = 0; count_y < 8; count_y ++)
        {

 read(handle, rm, 5);
        mons_inv [count_x] [count_y] = (rm [4] - 48) + (rm [3] - 48) * 10 + (rm [2] - 48) * 100 + (rm [1] - 48) * 1000;// + (rm [1] - 48) * 10000;

/*      temp_int = 0;

        for (p = 0; p < 5; p ++)  // p < 5
        {
                        ting_quant [p] = rm [p];

 }
 ting_quant [5] = 0;
 ting_quant [6] = 0;
 ting_quant [7] = 0;
 ting_quant [0] = 48;
        temp_int = atoi(ting_quant);

        mons_inv [i] [count_y] = temp_int; // - 10000;

//cprintf(ting_quant);
//cprintf(",");


/*
        read(handle, rm, 2);
//      read(handle, cm, 1);
        rm [0] -= 40;
        rm [1] -= 40;

        if (rm [0] == 215)
        {
                mons_inv [count_x] [count_y] = 501;
                continue;
        }

        if (rm [1] == 0) mons_inv [count_x] [count_y] = rm [0];
        else
            {
         mons_inv [count_x] [count_y] = rm [0] + rm [1];
            }

        /*mons_inv [count_x] [count_y] = rm [0] - 40;
        if (mons_inv [count_x] [count_y] == 215)
                mons_inv [count_x] [count_y] = 501;*/
//      cout << "," << mons_inv [count_x] [count_y] << ",";


        }
// getch();

/*      f7.put(mons_class [i]);
        f7.put(mons_hp [i]);
        f7.put(mons_AC [i]);
        f7.put(mons_hp_max [i]);
        f7.put(mons_ev [i]);
        f7.put(mons_HD [i]);
        f7.put(mons_x [i]);
        f7.put(mons_y [i]);
}

*/



//      mons_class [i] = rm_class;
//      mons_hp [i] = rm_hp;
//        mons_AC [i] = rm_AC

//grid [mons_x [count_x]] [mons_y [count_x]] = 17;
//cout << mons_x [count_x] << " , " << mons_y [count_x] << " " << " : " << mons_class [count_x];
//if (mons_class [count_x] < 0 | mons_class [count_x] > 2) cout << "Error";
//cout << mons_class [count_x] << " ";
//if (mons_x [count_x] > 115)
//{
//cout << mons_x [count_x] << "," << mons_y [count_x] << " : ";
//cout << (int) grid [mons_x [count_x]] [mons_y [count_x]] << "  -  ";
//}
//cout << mons_speed [count_x] << " ";
//cout << mons_class [0] << mons_hp [0]; << mons_class [1] << mons_hp ;

//if (mons_class [count_x] > 51) cout << "Monster+ ";
//if (grid [mons_x [count_x]] [mons_y [count_x]] != 77) cout << "grid! ";

if (mons_class [count_x] != -1) grid [mons_x [count_x]] [mons_y [count_x]] += 50;
//if (mons_class [count_x] == -1) cout << "X";


        //if (mons_x [count_x] < 5 | mons_x [count_x] > 115 | mons_y [count_x] < 5 | mons_y [count_x] > 96)
        //      cout << "!";

        /*
        if (mons_class [count_x] != -1)
        {
                grid [mons_x [count_x]] [mons_y [count_x]] += 60;
        }
                        !!!!!!

        */
}

getch();

//keyin = getch();

for (i = 0; i < ITEMS; i ++)
{

// items:
        //if (item_class [i] == -1) continue;


        //if (eof(handle) == 1) break; // shouldn't need this later. Remember
                // that when I put something else in levxxx.dat files, must
                // put some other kind of check in.
        // it doesn't work anyway.

        read(handle, rm, 24);

        //if (rm == 99) break;
        item_class [i] = rm [0] - 100;
//      if (item_class [i] == 155) // because of the - 100 above.
//      {
//              //item_class [i] = -1;
//              for (j = i; j < 200; j ++)
//              {
//                      item_quant [j] = 0;
//              }
//              break;
//      }
//      read(handle, rm, 1);
        item_type [i] = rm [1] - 100;
//      read(handle, rm, 1);
        item_plus [i] = rm [2] - 40;
//      read(handle, rm, 1);
        item_dam [i] = rm [3] - 40;
//      read(handle, rm, 6);
        item_quant [i] = rm [4] - 100;
        item_quant [i] = (rm [9] - 48) + (rm [8] - 48) * 10 + (rm [7] - 48) * 100 + (rm [6] - 48) * 1000 + (rm [5] - 48) * 10000;

/*
   strcpy(ting_quant, " ");

   for (p = 0; p < 5; p ++)  // p < 5
        {
        //              rm [5 + p] = thing_quant [p];
                        //ting_quant [p] = rm [14 + p];
                        ting_quant [p] = rm [5 + p];


//              item_quant [i] = (rm [19] - 48) + (rm [18] - 48) * 10 + (rm [17] - 48) * 100 + (rm [16] - 48) * 1000 + (rm [15] - 48) * 10000;
//      cprintf(rm [5 + p]);

   }
//      atoi(hunger, temp_int;

   //ting_quant [5 + p] = 0;

   cprintf(ting_quant);

   getch();

        temp_int = atoi(ting_quant);
// change


        item_quant [i] = temp_int; // - 10000;


*/

/*itoa(item_quant [i], st_prn, 10);

cprintf(st_prn);

getch();*/















        // - 48 because ASCII for zero (0) is 48.
//      read(handle, rm, 1);
        icolour [i] = rm [10] - 100;
//      read(handle, rm, 1);
        item_x [i] = rm [11] - 100;
//      read(handle, rm, 1);
        item_y [i] = rm [12] - 100;

        //item_link [i] = 501;

        item_ident [i] = rm [13] - 100;

//      read(handle, rm, 30);
//      strcpy(fake_name [i], rm);


        //item_link [i] = rm [14] - 100;
        //item_link [i] = (rm [19] - 48) + (rm [18] - 48) * 10 + (rm [17] - 48) * 100 + (rm [16] - 48) * 1000 + (rm [15] - 48) * 10000;



 temp_int = 0;


//      item_link [i] = (rm [19] - 48) + (rm [18] - 48) * 10 + (rm [17] - 48) * 100 + (rm [16] - 48) * 1000 + (rm [15] - 48) * 10000;

        strcpy(ting_quant, "");

        for (p = 0; p < 4; p ++)  // p < 5
        {
        //              rm [5 + p] = thing_quant [p];
                        //ting_quant [p] = rm [14 + p];
                        ting_quant [p] = rm [15 + p];


//              item_quant [i] = (rm [19] - 48) + (rm [18] - 48) * 10 + (rm [17] - 48) * 100 + (rm [16] - 48) * 1000 + (rm [15] - 48) * 10000;


   }
//      atoi(hunger, temp_int;


        temp_int = atoi(ting_quant);

// change


        item_link [i] = temp_int; // - 10000;

   strcpy(ting_quant, "");


//   temp_int = 0;

        for (p = 0; p < 4; p ++) // p < 5
        {
        //              rm [5 + p] = thing_quant [p];
                        //ting_quant [p] = rm [14 + p];
                        ting_quant [p] = rm [20 + p];
        }
        //atoi(hunger, temp_int;
        temp_int = atoi(ting_quant);
// change
        igrid [item_x [i]] [item_y [i]] = temp_int; // - 10000;

   strcpy(ting_quant, "");

/*      itoa(temp_int, ting_quant, 10);

   if (strcmp(ting_quant, "501") != 0)
   {
      cprintf(ting_quant);
      cprintf(",");
      //cprintf(rm);
      //cprintf("\n");
      //getch();
   }*/





        //cout << (int) item_link [i] << ", ";
        //getch();

        //cout << item_link [i] << ".";

        //keyin = getch();

        //cout << item_link [i] << ", ";


        if (item_class [i] == 100) item_quant [i] = 0;

        if (item_quant [i] != 0) it_no ++;

        //cout << item_class [i] << ":" << item_quant [i] << " ";
        /*  rm = item_class [i] + 100;
        f1.put(rm);
          rm = item_type [i] + 100;
        f1.put(rm);
          rm = item_plus [i] + 100;
        f1.put(rm);
          rm = item_dam [i] + 100;
        f1.put(rm);
          rm = item_quant [i] + 100;
        f1.put(rm);
          rm = item_x [i] + 100;
        f1.put(rm);
          rm = item_y [i] + 100;
        f1.put(rm);
          strcpy(zm, fake_name [i]);
        f1.put(zm);
        */

}

read(handle, rm, 1);
cloud_no = rm [0] - 40;

for (i = 0; i < CLOUDS; i ++)
{

 read(handle,rm, 9);
 cloud_x [i] = rm [0] - 40;
 cloud_y [i] = rm [1] - 40;
 cloud_type [i] = rm [2] - 40;
// cloud_decay [i] = rm [3] - 40;
        cloud_decay [i] = (rm [8] - 48) + (rm [7] - 48) * 10 + (rm [6] - 48) * 100 + (rm [5] - 48) * 1000 + (rm [4] - 48) * 10000;
 if (cloud_type [i] != 0) grid [cloud_x [i]] [cloud_y [i]] += 100;
}

/*
char cloud_x [CLOUDS];
char cloud_y [CLOUDS];
char cloud_type [CLOUDS];
char cloud_decay [CLOUDS];
int cloud_no;
*/


for (i = 0; i < 20; i ++)
{
        read(handle, rm, 1);
        mons_alloc [i] = rm [0] - 40;

}




//      keyin = getch();






/*
for (i = 0; i < MNST; i ++)
{
        if (mons_inv [i] [0] == 501) continue;

        for (j = 0; j < ITEMS; j ++)
        {
                if (mons_inv [i] [0] == j && (item_x [j] != mons_x [i] | item_y [j] != mons_y [i]))
                {
                //if (mons_x [i] != item_x [j] | mons_y [i] != item_y [j])
                {
                        cout << (int) i << ", " << (int) mons_class [i] << " : " << (int) mons_x [i] << ", " << (int) mons_y [i] << ", " << (int) item_x [j] << ", " << (int) item_y [j] << ".\n";
                        keyin = getch();
                }
                }

        }
}
*/

//keyin = getch();


        // MUST REMOVE THIS!!!!!!!:
        // This must be related to eg staircase use, because this function
        // is called when restoring saved games
        /*for (i = 0; i < MNST; i++)
        {
        if (mons_class [i] != -1 && gmon_use [mons_class [i]] != 0)
                {
                        //monsters_get(mons_class, mons_inv, item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y, it_no, i, mons_x, mons_y, mons_AC, mons_ev, property);
                        //monsters_find(mons_class, mons_inv, item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y, it_no, i, mons_x, mons_y, property, mons_AC, mons_ev);
                        restock_monsters(mons_class, mons_inv, item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y, it_no, i, mons_x, mons_y, item_link);
                }
        }*/







//   getch();




        int sysg = 0;

        //sysg = system("cd c:\borlandc\crawl");

        if (sysg == -1)
        {
                perror("Oh dear... ");
                exit(0);
        }








//screen_show();
  //close(handle);                    /* close the file */
}
else
{
  printf("Error opening file\n");
  exit(0);
}

//


//keyin = getch();

//if (!f1) cerr << "Cannot open level file.";
/*
for (count_x = 0; count_x < 140; count_x ++)
{
        for (count_y = 0; count_y < 140; count_y ++)
        {
                f1.put(grid [count_x] [count_y]);
        }
}
*/


/*
for (i = 0; i < 80; i ++)
{
        for (j = 0; j < 70; j ++)
        {
                if (grid [i] [j] == 28)
                {
                        for (p = 0; p < NTRAPS; p ++)
                        {
                                if (trap_x [p] == i && trap_y [p] == j) cout << " " << (int) p;
                        }
                }
        }
}
getch();
*/


/*

char keyg;

window(1,1,80,25);

for (i = 0; i < ITEMS; i ++)
{



cprintf(fake_name [i]);
cprintf("\n\r");

if (i % 25 == 0)
{
        keyg = getch();
}

} // end of for i

*/




//                                              screen_show();


//getch();


reset_ch();





// put a thing in the level file for the colour of the border.

close(handle);

//cprintf("\n\rDrawing!");

                                                draw_border(BROWN, your_name, clasnam);

//cprintf("Finished Loading!");

//cprintf("\n\rDrawn border!");

//replace_items(item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y);

// note that this is AFTER replace_items : !!!!!







        /*

        for (i = 0; i < ITEMS; i ++)
        {

        // if (igrid [item_x [i]] [item_y [i]] != 501) cout << (int) i << "," << (int) igrid [item_x [i]] [item_y [i]] << ".";

        if (item_quant [i] == 0) continue;

        if (igrid [item_x [i]] [item_y [i]] != 501) continue;

        for (p = 0; p < ITEMS + 1; p ++)
        {
                if (p == ITEMS) goto next_i;

                if (item_quant [p] == 0) continue;

                if (item_link [p] == i) break;

        }






        if (igrid [item_x [i]] [item_y [i]] == 501)
        {
                igrid [item_x [i]] [item_y [i]] = i;
        }

        //getch();

        next_i: char eggyrhdfg;

        } // end of for i

        */


        // Can eliminate item_x/y variables: put this in the middle
        // of the Load_level function and just use a non-array for
        // item_x/y
        //



        /*
        else
        {
                int b;
                int l;


                for (b = 0; b < ITEMS; b++)
                {
                        if (item_x [b] ==

                }




                j = item_link [igrid [item_x [i]] [item_y [i]]];

                while (item_link [j] != 501)
                {
                        b = item_link [j];
                        j = b;
                }

                item_link [j] = i;
        }



        } // end of for i


        */











        /*
        int b = 0;
        int h = 0;
        int m = 0;

        for (i = 0; i < ITEMS; i ++)
        {
                if (item_quant [i] == 0) continue;

                if (igrid [item_x [i]] [item_y [i]] == 501)
                {
                        igrid [item_x [i]] [item_y [i]] = i;
                        //cout << i << ", ";
                        //char groth = getch();
                        continue;
                } else
                {
                //b = item_link [igrid [item_x [i]] [item_y [i]]];
                b = igrid [item_x [i]] [item_y [i]];

                while(b != 501)
                {
                        m = b;
                        h = item_link [b];
                        b = h;
                        cout << h << ", ";
                        keyin = getch();
                } // end of while

                item_link [m] = i;

                //cout << "\n\rLinking" << i << " to " << m;
        //      char gtot = getch();
                }
        } // end of for i

        */






























        // need to sort out mons_inv problems here...






























//cprintf("\n\rCounting!");

it_no = 0;

for (i = 0; i < ITEMS; i ++)
{

if (item_quant [i] > 0)
        it_no ++;
}




//unidentify(fake_name, item_class, item_type, item_dam);



/*
char mni;

mni = random(40) + 50; // Change this!!!

mni -= no_mons;

if (mi > 0)
{
        for (i = 0; i < mi; i++)
        {
                mons_place();
        }
} // end of if mi
*/








//cout << "started linking items\n\r";



  /*



for (i = 1; i < MNST; i ++)
{

        //return;

        if (mons_class [i] == -1) continue;

        //cout << igrid [mons_x [i]] [mons_y [i]] << ", "; //keyin = getch();

        if (igrid [mons_x [i]] [mons_y [i]] == 501) continue;

        for (j = 0; j < 8; j ++)
        {

        if (mons_inv [i] [j] == 501) continue;

        int b = igrid [mons_x [i]] [mons_y [i]];
        int last_item = 501; //igrid [mons_x [i]] [mons_y [i]];

//      j = 0;

//      cout << (int) mons_inv [i] [j]; //igrid [mons_x [i]] [mons_y [i]];
        //cout << "," << (int) item_link [b] << "," << (int) item_type [b];
        //getch();

        while (b != 501)
        {
//      j++;
        //if (item_quant [item_got] == 0)
        //{
                //cout << b << ", "; //keyin = getch();
                //cout << (int) b;

                if (b == mons_inv [i] [j])
                {

                        if (last_item == 501)
                        {
                                igrid [mons_x [i]] [mons_y [i]] = item_link [b];
                                //cout << igrid [x_pos] [y_pos] << "zzz"; keyin = getch();
                        } else
                        {
                                item_link [last_item] = item_link [b];
                                //cout << last_item << "ccc"; keyin = getch();
                        }
                        //it_no --;
                }
                        last_item = b;
                        b = item_link [last_item];

        //cout << "," << (int) b << " ";

        //}


        }










        /*

        while (item_link [yggy] != 501)
        {
                if (yggy == mons_inv [i] [j])
                {
                        item_link
                }

                yggy = item_link [yggy];
        }

        */
















        //if (item_x [mons_inv [i] [j]] != mons_x [i] | item_y [mons_inv [i] [j]] != mons_y [i]);
        //{
        //      mons_inv [i] [j] = 501;
        //}
/*
        for (count_x = 0; count_x < 80; count_x ++)
        {
                for (count_y = 0; count_y < 70; count_y ++)
                {
                        if (igrid [count_x] [count_y] == mons_inv [i] [j])
                        {
                                igrid [count_x] [count_y] = 501;
                                if (item_link [mons_inv [i] [j]] != 501)
                                {
                                        igrid [count_x] [count_y] = mons_inv [i] [j];
                                }
                        cout << "Discarding...\n\r";
                        keyin = getch();
                        }
                }
        }

*/












//      } // end of for j

//} // end of for i






//cout << "finished linking items\n\r";















































i = 0;
j = 0;
/*
for (count_x = 0; count_x < 80; count_x ++)
{
        for (count_y = 0; count_y < 80; count_y ++)
        {
                if (igrid [count_x] [count_y] == 501) continue;

                i = item_link [igrid [count_x] [count_y]];

                while(i != 501)
                {
                        i = item_link [i];
                        j ++;
                }

                //cout << j << "; ";
                j = 0;
        }
}
*/
//keyin = getch();


//screen_show();

//cprintf("\n\rUnlinking! ");
char linky [5];

for (i = 0; i < MNST; i++)
{
 if (mons_class [i] == -1) continue;
// itoa(i, linky, 10);
// cprintf(" ");
// cprintf(linky);
// cprintf(":");
        for (j = 0; j < 8; j ++)
        {
                if (mons_inv [i] [j] == 501) continue;
// itoa(mons_inv [i] [j], linky, 10);
// cprintf(linky);
// cprintf(",");
                if (item_link [mons_inv [i] [j]] != 501)
                {
                        //cout << "Unlinking...\n\r";
//   cprintf("UNL");
                        item_link [mons_inv [i] [j]] = 501;
                }

        }
}

//cprintf("\n\rDeleting!");

 char del_file [25];

        strcpy(del_file, "del ");
        strcat(del_file, cha_fil);
        sysg = system(del_file);

        if (sysg == -1)
        {
                perror("\n\rYou haven't done something naughty to the save file, have you?\n\r");
                exit(-1);
        }



//cprintf("finished loading level\n\r");





//cprintf("\n\rFinished loading!");




} // end of void load_level();




void save_level(void)
{
//switch (many)

//return;

//ofstream f1 ("lev000.dat");
unsigned char rm [140];
int handle;
//char zm [30];
//char ting_quant [12];


//replace_items(item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y);


/*
switch (your_level)
{
        case 0: handle = open("lev000.dat", O_WRONLY, S_IWRITE, S_IREAD); break;
        case 1: handle = open("lev001.dat", O_WRONLY, S_IWRITE, S_IREAD); break;
        case 2: handle = open("lev002.dat", O_WRONLY, S_IWRITE, S_IREAD); break;
        case 3: handle = open("lev003.dat", O_WRONLY, S_IWRITE, S_IREAD); break;
        case 4: handle = open("lev004.dat", O_WRONLY, S_IWRITE, S_IREAD); break;
        case 5: handle = open("lev005.dat", O_WRONLY, S_IWRITE, S_IREAD); break;
        case 6: handle = open("lev006.dat", O_WRONLY, S_IWRITE, S_IREAD); break;
        case 7: handle = open("lev007.dat", O_WRONLY, S_IWRITE, S_IREAD); break;
        case 8: handle = open("lev008.dat", O_WRONLY, S_IWRITE, S_IREAD); break;
        case 9: handle = open("lev009.dat", O_WRONLY, S_IWRITE, S_IREAD); break;
}
*/

char cha_fil [20];
char extens [3];

//strcpy(cha_fil, "c:\\borlandc\\crawl\\levels\\");
strcpy(cha_fil, "");
strcat(cha_fil, your_name);
strcat(cha_fil, ".lev");
//strcat(cha_fil, itoa(your_level, extens, 10));
//corr_level);

//handle = open(cha_fil, O_WRONLY, S_IWRITE, S_IREAD);// break;

        int sysg = 0;

        //sysg = system("cd c:\borlandc\crawl\levels");

        if (sysg == -1)
        {
                perror("Oh dear... ");
                exit(0);
        }



//handle = open(cha_fil, O_WRONLY | O_BINARY, S_IWRITE, S_IREAD);
handle = open(cha_fil, O_CREAT | O_TRUNC | O_BINARY, S_IWRITE | S_IREAD);

if (handle == -1)
{
        perror("dfg");
        abort();
}

int fry;
int frx;

// Setting up mons_x & mons_y:

for (count_x = 0; count_x < 80; count_x ++)
{
        for (count_y = 0; count_y < 70; count_y ++)
        {

        if (igrid [count_x] [count_y] == 501) continue;

        //int fry;
        frx = igrid [count_x] [count_y];

        while(frx != 501)
        {
                //cout << frx; //getch();
                item_x [frx] = count_x;
                item_y [frx] = count_y;

                //cout << "Linking item at " << (int) item_x [frx] << "," << (int) item_y [frx] << "\n\r";
                //getch();


                fry = item_link [frx];
                frx = fry;
        }


        }
}


// ofstream f2 ("lev001.dat");
// ofstream f3 ("lev002.dat");
// ofstream f4 ("lev003.dat");
// ofstream f5 ("lev004.dat");


for (i = 0; i < CLOUDS; i ++)
{
 if (cloud_type [i] != 0) grid [cloud_x [i]] [cloud_y [i]] -= 100;
}

int glorpogop = 0;

for (count_x = 0; count_x < 80; count_x ++)
{
//      for (count_y = 0; count_y < 70; count_y ++)
//      {
//              switch(many)
//              {
//              f1.put(grid [count_x] [count_y]);
//              case 1: f2.put(grid [count_y] [count_x]); break;
//              case 2: f3.put(grid [count_y] [count_x]); break;
//              case 3: f4.put(grid [count_y] [count_x]); break;
//              case 4: f5.put(grid [count_y] [count_x]); break;
///             }
//              if (grid [count_x] [count_y] > 70) grid [count_x] [count_y] -= 70;
//              rm [0] = grid [count_x] [count_y];
//              write(handle, rm, 1);
//      }
                for (count_y = 0; count_y < 70; count_y++)
                {
                        rm [glorpogop] = (grid [count_x] [count_y] % 50) + 27;
//                      if (rm [glorpogop] >= 50) rm [glorpogop] -= 50;
   glorpogop ++;
   rm [glorpogop] = map [count_x] [count_y];
   glorpogop ++;
                }
                //cout << "Y";
                write(handle, rm, 140);

  glorpogop = 0;
}


for (i = 0; i < NTRAPS; i ++)
{
        rm [0] = trap_type [i] + 40;
        rm [1] = trap_x [i] + 40;
        rm [2] = trap_y [i] + 40;
        write(handle, rm, 3);
}
/*
for (i = 0; i < 10; i += 2)
{
        rm [i] = stair_x [i] + 40;
        rm [i + 1] = stair_y [i] + 40;
}
write(handle, rm, 10);
*/
for (i = 0; i < MNST; i++)
{
//      strcpy(mons_class [i]

        if (mons_class [i] == -1) rm [0] = 255; else rm [0] = mons_class [i] + 40;
// rm [0] = 40;
//      rm [0] = mons_class [i];
//      write(handle, rm, 1);
        rm [1] = mons_hp [i] + 40;
//      write(handle, rm, 1);
        rm [2] = mons_AC [i] + 40;
//      write(handle, rm, 1);
        rm [3] = mons_hp_max [i] + 40;
//      write(handle, rm, 1);
        rm [4] = mons_ev [i] + 40;
//      write(handle, rm, 1);
        rm [5] = mons_HD [i] + 40;
//      write(handle, rm, 1);
        rm [6] = (mons_speed [i]) + 40; // * 10
//      write(handle, rm, 1);
        rm [7] = mons_speed_inc [i] + 40;
//      write(handle, rm, 1);
        // put behaviour here.
        rm [8] = mons_beh [i] + 40;
//      write(handle, rm, 1);
        rm [9] = mons_x [i] + 40;
//      write(handle, rm, 1);
        rm [10] = mons_y [i] + 40;
        rm [11] = mons_targ_1_x [i] + 40;
        rm [12] = mons_targ_1_y [i] + 40;
        rm [13] = mons_sec [i] + 40;

 rm [14] = mons_ench_1 [i] + 40;

 for (j = 0; j < 3; j++)
 {
  rm [15 + j] = mons_ench [i] [j] + 40;
 }

 write(handle, rm, 18);

 strcpy(thing_quant, "");
        int temp_int = mons_class [i] + 10080;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [p] = thing_quant [p];
        }
 write(handle, rm, 5);

/*
        temp_int = mons_hp [i] + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [p] = thing_quant [p];
        }
 write(handle, rm, 5);

        temp_int = mons_hp_max [i] + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [p] = thing_quant [p];
        }
 write(handle, rm, 5);
*/

        for (j = 0; j < 8; j ++)
        {
  /*
                if (mons_inv [i] [j] == 501)
                {
                rm [0] = 215 + 40;
                write(handle, rm, 1);
                //f1.put(rm);
                rm [0] = 0 + 40;
                //f1.put(rm);
                write(handle, rm, 1);
                continue;
                }

                if (mons_inv [i] [j] <= 214)
                {
                rm [0] = mons_inv [i] [j] + 40;
                //f1.put(rm);
                write(handle, rm, 1);
                rm [0] = 0 + 40;
                //f1.put(rm);
                write(handle, rm, 1);
                }

                if (mons_inv [i] [j] > 214)
                {
                rm [0] = 214 + 40;//mons_inv [i] [j] - 215;
                write(handle, rm, 1);
                //f1.put(rm);
                rm [0] = (mons_inv [i] [j] - 214) + 40;
                write(handle, rm, 1);
                //f1.put(rm);
                }
                //f1.put(mons_inv [i] [j]);
  */

        temp_int = mons_inv [i] [j] + 10000;
        itoa(temp_int, thing_quant, 10);
        //cout << ting_quant << ", ";
        for (p = 0; p < 5; p ++)
        {
                rm [p] = thing_quant [p];
        }
// cprintf(thing_quant);
// cprintf("\n\r"); getch();
 write(handle, rm, 5);
        }
}

for (i = 0; i < ITEMS; i ++)
{

// items:
     // if (item_quant [i] == 0) continue;

          rm [0] = item_class [i] + 100;
//      write(handle, rm, 1);
        //f1.put(rm);
          rm [1] = item_type [i] + 100;
//      write(handle, rm, 1);
        //f1.put(rm);
          rm [2] = item_plus [i] + 40;
//      write(handle, rm, 1);
        //f1.put(rm);
          rm [3] = item_dam [i] + 40;
//      write(handle, rm, 1);
        //f1.put(rm);
/*
        temp_long = item_quant [i] + 100000;
        ltoa(temp_long, thing_quant, 10);

        //ltoa(item_quant [i], thing_quant, 10);
        for (p = 0; p < 6; p ++)
        {
                rm [4 + p] = thing_quant [p];
                //f1.put(rm);
//              write(handle, rm, 1);
        }*/


        temp_int = item_quant [i] + 100000;
        itoa(temp_int, thing_quant, 10);

        //ltoa(item_quant [i], thing_quant, 10);
        for (p = 0; p < 6; p ++)
        {
                rm [4 + p] = thing_quant [p];
                //f1.put(rm);
//              write(handle, rm, 1);
        }


 /*     for (p = 0; p < 6; p ++)
        {


                rm [0] = item_quant [i] [p];
                write(handle, rm, 1);
                //f1.put(rm);
        }*/

        //  rm = item_quant [i] + 100;
        //f1.put(rm);
          rm [10] = icolour [i] + 100;
//      write(handle, rm, 1);
        //f1.put(rm);
          rm [11] = item_x [i] + 100;
//      write(handle, rm, 1);
        //f1.put(rm);
          rm [12] = item_y [i] + 100;
//      write(handle, rm, 1);
        //f1.put(rm);

          rm [13] = item_ident [i] + 100;

        int temp_int = item_link [i] + 10000;
        itoa(temp_int, thing_quant, 10);
        //cout << ting_quant << ", ";
        for (p = 0; p < 5; p ++)
        {
                rm [14 + p] = thing_quant [p];
        }

        temp_int = igrid [item_x [i]] [item_y [i]] + 10000;
        itoa(temp_int, thing_quant, 10);
        //cout << ting_quant << ", ";
        for (p = 0; p < 5; p ++)
        {
                rm [19 + p] = thing_quant [p];
        }


        write(handle, rm, 24);

//      for (j = 0; j < 30; j ++)
//      {
//              rm [0] = fake_name [i] [j];
//              write(handle, rm, 1);
                //f1.put(rm);
//              write(handle, fake_name [i], 30);
//      }


        //  strcpy(zm, fake_name [i]);
        //f1.put(zm);


}

rm [0] = cloud_no + 40;
write(handle, rm, 1);

for (i = 0; i < CLOUDS; i ++)
{
 rm [0] = cloud_x [i] + 40;
 rm [1] = cloud_y [i] + 40;
 rm [2] = cloud_type [i] + 40;

        temp_int = cloud_decay [i] + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [3 + p] = thing_quant [p];
        }
 write(handle,rm, 9);
}




for (i = 0; i < 20; i++)
{
        rm [0] = mons_alloc [i] + 40;
        write(handle, rm, 1);

}

close(handle);


//cout << "Finished saving!";
//getch();


//cout << "finished level 1!";
//f1.put(255);

//unidentify(fake_name, item_class, item_type, item_dam);
/*

char keyg;

window(1,1,80,25);

for (i = 0; i < ITEMS; i ++)
{


cprintf(fake_name [i]);
cprintf("\n\r");

if (i % 25 == 0)
{
        keyg = getch();
}

} // end of for i

*/

//reset_ch();

//draw_border(6, your_name, clasnam);

}



/*
void screen_show(void)
{

window(1,1,80,25);

clrscr();
nothing = 0;
for (l = 0; l < 2; l++)
{


for (count_x = 1; count_x < 80; count_x ++)
{
        for (count_y = 1 + 25 * l; count_y < 25 + 25 * l; count_y ++)
        {
                gotoxy(count_x, count_y - 25 * l);
                switch(grid [count_x] [count_y])
                {
                case 1: textcolor(6); putch(177); textcolor(7); break;
                case 2: textcolor(7); putch(177); textcolor(7); break;
                case 3: putch(43); break;
  case 11: textcolor(RED); putch(247); textcolor(7); break;
  case 12: textcolor(BLUE); putch(247); textcolor(7); break;
                case 17: textcolor(7); putch(249); textcolor(7); break;
                case 18: putch(61); break;
                case 19: putch(62); break;
                case 21: putch(60); break;
                case 22: putch(62); break;
                case 23: putch(60); break;
                case 24: putch(62); break;
                default : putch(grid [count_x] [count_y]); break;
                }
                if (grid [count_x] [count_y] > 50)
                {
                for (i = 0; i < MNST; i ++)
                {
                        if (mons_x [i] == count_x && mons_y [i] == count_y && mons_class [i] != -1)
                        {
                        gotoxy(count_x, count_y - 25 * l);
                        putch(42);
                        nothing = 1;
                        }
                }


      if (igrid [count_x] [count_y] != 501)
      {
         gotoxy(count_x, count_y - 25 * l);
         putch(254);
      }




                //              if (nothing == 0) cout << "Problem!!!";
                nothing = 0;
                }
        }
}
nothing = 0;
nothing = getch();
clrscr();

/*
for (count_x = 79; count_x < 80; count_x ++)
{
        for (count_y = 1 + 25 * l; count_y < 25 + 25 * l; count_y ++)
        {
                gotoxy(count_x - 78, count_y - 25 * l);
                switch(grid [count_x] [count_y])
                {
                case 1: textcolor(6); putch(177); textcolor(7); break;
                case 2: textcolor(7); putch(177); textcolor(7); break;
                case 3: putch(43); break;
                case 17: textcolor(7); putch(249); textcolor(7); break;
                case 18: putch(61); break;
                case 19: putch(62); break;
                case 21: putch(60); break;
                case 22: putch(62); break;
                case 23: putch(60); break;
                case 24: putch(62); break;
                default : putch(grid [count_x] [count_y]); break;
                }
                if (grid [count_x] [count_y] > 50)
                {
                for (i = 0; i < MNST; i ++)
                {
                        if (mons_x [i] == count_x && mons_y [i] == count_y && mons_class [i] != -1)
                        {
                        gotoxy(count_x - 78, count_y - 25 * l);
                        putch(42);
                        nothing = 1;
                        }
                }
                if (nothing == 0) cout << "Problem!!!";
                nothing = 0;
                }
        }
}

nothing = getch();
clrscr();

* /









} // end of for l

*/








//} // end of screen_show







void save_game(void)
{
int handle;
//char load_file [12];
//unsigned char whole_line [100];
unsigned char rm [69];
int temp_int = 0;
long temp_long;
char char_f [15];
char itf;


/*for (i = 0; i < ITEMS; i ++)
{
        item_ident [i] = 0;
}*/


//strcpy(char_f, "c:\\borlandc\\crawl\\levels\\");
strcpy(char_f, "");
strncat(char_f, your_name, 8);
strcat(char_f, ".sav");


// Use the first one to amend already created files, and the second to create
//  new - the 2nd will be used eventually, as save games will be deleted after
//  loading.
//handle = open(char_f, O_TEXT | O_RDWR | S_IWRITE);//O_WRONLY, S_IWRITE, S_IREAD); //O_WRONLY); //, O_CREAT | O_BINARY);

//handle = open(char_f, O_CREAT | O_TRUNC | O_BINARY, S_IWRITE);
/*
switch(newc)
{
        case 0: strcpy(char_f, "char00.dat"); break;
        case 1: strcpy(char_f, "char01.dat"); break;
        case 2: strcpy(char_f, "char02.dat"); break;
        case 3: strcpy(char_f, "char03.dat"); break;
        case 4: strcpy(char_f, "char04.dat"); break;
        case 5: strcpy(char_f, "char05.dat"); break;
        case 6: strcpy(char_f, "char06.dat"); break;
        case 7: strcpy(char_f, "char07.dat"); break;
        case 8: strcpy(char_f, "char08.dat"); break;
        case 9: strcpy(char_f, "char09.dat"); break;
//case 0: strcpy(char_fil, "char01.dat"); break;
}
*/




        int sysg = 0;

//      sysg = system("cd c:\borlandc\crawl\levels");

        if (sysg == -1)
        {
                perror("Oh dear... ");
                exit(0);
        }





//handle = open(char_f, O_WRONLY | O_BINARY, S_IWRITE, S_IREAD);
handle = open(char_f, O_CREAT | O_TRUNC | O_BINARY, S_IWRITE | S_IREAD);

//handle = open("me.sav", O_CREAT | O_BINARY);

if (handle == -1)
{
        perror("Unable to open file for writing");
        exit(-1);
}

//int haste = 0; // haste
rm [0] = teleport + 40;
rm [1] = see_invis + 40;
rm [2] = res_fire + 40;
rm [3] = res_poison + 40;
rm [4] = res_cold + 40;
rm [5] = fast_run + 40;
rm [6] = shock_shield + 40;
rm [7] = sust_abil + 40;
rm [8] = magic_battery + 40;
rm [9] = deaths_door + 40;
rm [10] = spec_fire + 40;
rm [11] = spec_cold + 40;
rm [12] = spec_poison + 40;
rm [13] = pet_target + 40;
rm [14] = prot_life + 40;
temp_int = res_magic + 10000;
itoa(temp_int, thing_quant, 10);
for (p = 0; p < 5; p ++)
{
                rm [p + 15] = thing_quant [p];
}
rm [20] = spell_levels + 80;
rm [21] = max_level + 40;
rm [22] = where_are_you + 40;
rm [23] = char_direction + 40;
rm [24] = your_level + 40; // I can't believe it took me this long to realise that this wasn't being saved!
rm [25] = shield_class + 40;
rm [26] = special_wield + 40;
rm [27] = berserker + 40;
rm [28] = energy + 40;
rm [29] = level_type + 40;
write(handle, rm, 30);

temp_int = hp + 10000;
itoa(temp_int, thing_quant, 10);
for (p = 0; p < 5; p ++)
{
                rm [p] = thing_quant [p];
}

temp_int = hp_max + 10000;
itoa(temp_int, thing_quant, 10);
for (p = 0; p < 5; p ++)
{
                rm [5 + p] = thing_quant [p];
}

write(handle, rm, 10);


if (haste > 215) haste = 215;
rm [0] = haste + 40;

if (might > 215) might = 215;
rm [1] = might + 40;

if (lev > 215) lev = 215;
rm [2] = lev + 40;

if (poison > 215) poison = 215;
rm [3] = poison + 40;

//int can_see; // Are you blind?
rm [4] = can_see + 40;

//int delay_t = 0; I don't think it's neccesary to save these, do you?
//int delay_doing = 0;

//int hunger = 1000;  //                Look at an itoa thing for this.
temp_int = hunger + 100000;// If you're a large creature, stomach can hold much more than human.
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 6; p ++)
        {
                rm [5 + p] = thing_quant [p];
        } // DON'T save when hunger is -ve


//rm [10] = hunger_inc + 40; // this has been shifted to max values to make way for hunger

rm [11] = item_wielded + 40;

/*rm [12] = body_armour + 40;

rm [13] = head_armour + 40;

rm [14] = cloak_armour + 40;

rm [15] = hand_armour + 40;

rm [16] = foot_armour + 40;

rm [17] = shield_armour + 40;
*/

for (i = 0; i < 6; i++)
{
        rm [12 + i] = armour [i] + 40;
}

rm [18] = ring [0] + 40;
rm [19] = ring [1] + 40;

rm [20] = 40;//hp + 40;

rm [21] = 40;//hp_max + 40;

rm [22] = ep + 40;

rm [23] = ep_max + 40;

rm [24] = strength + 40;

rm [25] = intel + 40;

rm [26] = dex + 40;

rm [27] = AC + 80; // Remember that AC shown is 10 - AC, but AC still can be -ve.

rm [28] = evasion + 40;

rm [29] = damage + 40;

rm [30] = nat_damage + 40;

//float rate_regen = 0.06;
temp_int = (int) (rate_regen * 100) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [31 + p] = thing_quant [p];
        }

rm [31] = rate_regen + 40;


//float incr_regen = 0; // How am I going to do this???
temp_int = (int) (incr_regen * 100) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [36 + p] = thing_quant [p];
        }

rm [32] = incr_regen + 40;

/*      temp_long = xp + 100000;
        ultoa(temp_long, thing_quant, 10);
        for (p = 0; p < 6; p ++)
        {
                rm [41 + p] = thing_quant [p];
        }*/


        temp_int = xp + 100000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 6; p ++)
        {
                rm [41 + p] = thing_quant [p];
        }



rm [47] = xl + 40;

temp_int = gp + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [48 + p] = thing_quant [p];
        }

rm [53] = clas + 40;

temp_int = (int) (f_abil) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [54 + p] = thing_quant [p];
        }

temp_int = (int) (speed * 100) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [59 + p] = thing_quant [p];
        }

temp_int = (int) (speed_inc * 100) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [64 + p] = thing_quant [p];
        }

        write(handle, rm, 69);

// max values
rm [0] = max_strength + 40;
rm [1] = max_intel + 40;
rm [2] = max_dex + 40;
rm [3] = hunger_inc + 40;
write(handle, rm, 4);


temp_int = (int) (mag_abil) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [0 + p] = thing_quant [p];
        }


temp_int = (int) (thr_abil) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [5 + p] = thing_quant [p];
        }

 write(handle, rm, 10);




        rm [0] = (unsigned char) x_pos + 40;
        rm [1] = (unsigned char) y_pos + 40;

//        cprintf(rm);

//      itoa(x_pos, itf, 10);
//      rm [0] = itf;

//      itoa(y_pos, itf, 10);
//      rm [1] = itf;

        write(handle, rm, 2);

        strcpy(rm, clasnam);
        write(handle, rm, 13);

temp_int = burden + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [p] = thing_quant [p];
        }
        write(handle, rm, 5);

//       clasnam [13]; // Now?
/*
        char inv_name [52] [30];
        char inv_class [52];
        char inv_type [52];
        char inv_plus [52];
        char inv_dam [52];
        char inv_col [52];
        int inv_quant [52]; // multiple items, eg ammo or potions
        int burden; // total weight of items carried.
        char inv_no; // number of items carried.


        handle = open("lev000.dat", O_RDONLY, S_IWRITE, S_IREAD); break;


        rm [0] = mons_inv [i] [j] + 40;

                write(handle, rm, 1);
        */

 for (i = 0; i < 25; i ++)
 {
  rm [i] = spells [i] + 30;
 }
 write(handle, rm, 25);

        for (i = 0; i < 52; i++)
        {
//      strcpy(rm, inv_name [i]);
//      write(handle, rm, 30);
        rm [0] = inv_class [i] + 40;
        rm [1] = inv_type [i] + 40;
        rm [2] = inv_plus [i] + 40;
        rm [3] = inv_dam [i] + 40;
        rm [4] = inv_col [i] + 40;
        rm [5] = inv_ident [i] + 40;

        temp_int = inv_quant [i] + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [6 + p] = thing_quant [p];
        }
        write(handle, rm, 11);
        } // end of for i

/*
        for (i = 0; i < 4; i ++)
        {
                for (p = 0; p < 30; p ++)
                {
                rm [p] = get_id(i, p) + 40;
                }
                write(handle, rm, 30);
        }
*/
                for (i = 0; i < 30; i ++)
        {
                rm [i] = levels_exist [i];
                //lev_ex [i] = 0;
        }
        write(handle, rm, 30);



for (i = 0; i < 5; i ++)
{
   for (j = 0; j < 30; j ++)
   {
    rm [j] = item_description [i] [j] + 40;
   }

   write(handle, rm, 30);
}

char identy [4] [30];
save_id(identy);


for (i = 0; i < 4; i ++)
{
   for (j = 0; j < 30; j ++)
   {
//    rm [j] = item_description [i] [j] + 40;
      rm [j] = identy [i] [j] + 40;
   }
   write(handle, rm, 30);
}


   for (j = 0; j < 50; j ++)
   {
      rm [j] = unique_items [j] + 40; // unique items
   }
   write(handle, rm, 50);


        close(handle);

/*

        strncpy(char_f, your_name, 8);
        strcat(char_f, ".dat");

        handle = open(char_f, O_CREAT | O_TRUNC | O_BINARY, S_IWRITE);


        x_pos = 50;

        rm [0] = (unsigned char) x_pos;
        rm [1] = (unsigned char) y_pos;

        write(handle, rm, 2);

        close(handle);

*/





        /*

        char tm [10] [30];


        handle = open("control.dat", S_IWRITE, S_IREAD);

        for (i = 0; i < 10; i++)
        {
                read(handle, tm [i] , 30);
        }

        strcpy(tm [newc], your_name);

        close (handle);


        handle = open("control.dat", O_WRONLY, S_IWRITE, S_IREAD);

        for (i = 0; i < 10; i++)
        {
                write(handle, tm [i] , 30);

        }

        //strcpy(tm [i], "deceased");

        close (handle);

        */



        //int sysg = 0;

//      sysg = system("cd c:\borlandc\crawl");

        if (sysg == -1)
        {
                perror("Oh dear... ");
                exit(0);
        }



 save_level();





window(1, 1, 80, 25);

clrscr();

cprintf("See you soon!");

//abort();
exit(0);

} // end of save_game()



void reset_ch(void)
{

        hp_ch = 1;
        hp_max_ch = 1;
        ep_ch = 1;
        ep_max_ch = 1;
        strength_ch = 1;
        intel_ch = 1;
        dex_ch = 1;
        AC_ch = 1; // remember that the AC shown = 10 - AC
        evasion_ch = 1;
        damage_ch = 1;
        xp_ch = 1;
        xl_ch = 1;
        gp_ch = 1;
        hung_ch = 1;

}














void restore_game(void)
{
int handle;
//char load_file [12];
//unsigned char whole_line [100];
unsigned char rm [69];
int temp_int = 0;
char itf;
long temp_long;
char char_f [15];

//strcpy(char_f, "c:\\borlandc\\crawl\\levels\\");
strcpy(char_f, "");
strncat(char_f, your_name, 8);
strcat(char_f, ".sav");

//handle = open(char_f, O_RDONLY, S_IWRITE, S_IREAD);
/*
switch(newc)
{
        case 0: strcpy(char_f, "char00.dat"); break;
        case 1: strcpy(char_f, "char01.dat"); break;
        case 2: strcpy(char_f, "char02.dat"); break;
        case 3: strcpy(char_f, "char03.dat"); break;
        case 4: strcpy(char_f, "char04.dat"); break;
        case 5: strcpy(char_f, "char05.dat"); break;
        case 6: strcpy(char_f, "char06.dat"); break;
        case 7: strcpy(char_f, "char07.dat"); break;
        case 8: strcpy(char_f, "char08.dat"); break;
        case 9: strcpy(char_f, "char09.dat"); break;
//case 0: strcpy(char_fil, "char01.dat"); break;
}
*/

        int sysg = 0;

//      sysg = system("cd c:\borlandc\crawl\levels");

        if (sysg == -1)
        {
                perror("Oh dear... ");
                exit(0);
        }


//handle = open(char_f, O_CREAT | O_TRUNC | O_BINARY, S_IREAD);
//handle = open(char_f, O_CREAT | O_TRUNC | O_BINARY, S_IWRITE | S_IREAD);

handle = open(char_f, O_RDONLY, S_IWRITE, S_IREAD);


if (handle == -1)
{
        perror("Unable to open file for reading");
        exit(-1);
}

read(handle, rm, 30);
teleport = rm [0] - 40;
see_invis = rm [1] - 40;
res_fire = rm [2] - 40;
res_poison = rm [3] - 40;
res_cold = rm [4] - 40;
fast_run = rm [5] - 40;
shock_shield = rm [6] - 40;
sust_abil = rm [7] - 40;
magic_battery = rm [8] - 40;
deaths_door = rm [9] - 40;
spec_fire = rm [10] - 40;
spec_cold = rm [11] - 40;
spec_poison = rm [12] - 40;
pet_target = rm [13] - 40;
prot_life = rm [14] - 40;
        for (p = 0; p < 5; p ++)
        {
                thing_quant [p] = rm [p + 15];
        }
temp_int = atoi(thing_quant);
res_magic = temp_int - 10000;
spell_levels = rm [20] - 80;
max_level = rm [21] - 40;
where_are_you = rm [22] - 40;
char_direction = rm [23] - 40;
your_level = rm [24] - 40;
shield_class = rm [25] - 40;
special_wield = rm [26] - 40;
berserker = rm [27] - 40;
energy = rm [28] - 40;
level_type = rm [29] - 40;
//magic_battery = rm [10] - 40;
//write(handle, rm, 7);



read(handle, rm, 10);

        for (p = 0; p < 5; p ++)
        {
//              rm [5 + p] = thing_quant [p];
                thing_quant [p] = rm [p];
        }
//atoi(hunger, temp_int;
temp_int = atoi(thing_quant);
// change
hp = temp_int - 10000;

        for (p = 0; p < 5; p ++)
        {
//              rm [5 + p] = thing_quant [p];
                thing_quant [p] = rm [5 + p];
        }
//atoi(hunger, temp_int;
temp_int = atoi(thing_quant);
// change
hp_max = temp_int - 10000;


/*
temp_int = xhp + 10000;
itoa(temp_int, xthing_quant, 10);
for (p = 0; p < 5; p ++)
{
                rm [p] = xthing_quant [p];
}

temp_int = xhp_max + 10000;
itoa(temp_int, xthing_quant, 10);
for (p = 0; p < 5; p ++)
{
                rm [5 + p] = xthing_quant [p];
}
*/


//int haste = 0; // haste
read(handle, rm, 69);


//if (haste > 215) haste = 215;
//rm [0] = haste + 40;

haste = rm [0] - 40;

//if (might > 215) might = 215;
//rm [1] = might + 40;

might = rm [1] - 40;

//if (lev > 215) lev = 215;
//rm [2] = lev + 40;

lev = rm [2] - 40;

//if (poison > 215) poison = 215;
//rm [3] = poison + 40;

poison = rm [3] - 40;

//int can_see; // Are you blind?
//rm [4] = can_see + 40;

can_see = rm [4] - 40;

//int delay_t = 0; I don't think it's neccesary to save these, do you?
//int delay_doing = 0;

//      itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 6; p ++)
        {
//              rm [5 + p] = thing_quant [p];
                thing_quant [p] = rm [5 + p];
        }
//atoi(hunger, temp_int;
temp_int = atoi(thing_quant);
// change
hunger = temp_int - 100000;


//rm [10] = hunger_inc + 40;
//hunger_inc = rm [10] - 40;

//rm [11] = item_wielded + 40;
item_wielded = rm [11] - 40;

/*
//rm [12] = body_armour + 40;
body_armour = rm [12] - 40;

//rm [13] = head_armour + 40;
head_armour = rm [13] - 40;

//rm [14] = cloak_armour + 40;
cloak_armour = rm [14] - 40;

//rm [15] = hand_armour + 40;
hand_armour = rm [15] - 40;

//rm [16] = foot_armour + 40;
foot_armour = rm [16] - 40;

//rm [17] = shield_armour + 40;
shield_armour = rm [17] - 40;
*/

for (i = 0; i < 6; i ++)
{
        armour [i] = rm [12 + i] - 40;
}

//rm [18] = ring [0] + 40;
ring [0] = rm [18] - 40;
//rm [19] = ring [1] + 40;
ring [1] = rm [19] - 40;

//rm [20] = hp + 40;
//hp = 100; //rm [20] - 40;

//rm [21] = hp_max + 40;
//hp_max = 100; //rm [21] - 40;

//rm [22] = ep + 40;
ep = rm [22] - 40;

//rm [23] = ep_max + 40;
ep_max = rm [23] - 40;

//rm [24] = strength + 40;
strength = rm [24] - 40;

//rm [25] = intel + 40;
intel = rm [25] - 40;

//rm [26] = dex + 40;
dex = rm [26] - 40;

//rm [27] = AC + 80; // Remember that AC shown is 10 - AC, but AC still can be -ve.
AC = rm [27] - 80;

//rm [28] = evasion + 40;
evasion = rm [28] - 40;

//rm [29] = damage + 40;
damage = rm [29] - 40;

//rm [30] = nat_damage + 40;
nat_damage = rm [30] - 40;


/*
for (p = 0; p < 5; p ++)
{
        thing_quant [p] = rm [31 + p];
}
temp_int = atoi(thing_quant);
rate_regen = (float) (temp_int / 100) - 10000;
*/
        rate_regen = ((int) ((rm [35] - 48) + (rm [34] - 48) * 10 + (rm [33] - 48) * 100 + (rm [32] - 48) * 1000)); // / 100;
rate_regen = rm [31] - 40;


/*
for (p = 0; p < 5; p ++)
{
        thing_quant [p] = rm [36 + p];
}
temp_int = atoi(thing_quant);
incr_regen = (float) (temp_int / 100) - 10000;
*/


        incr_regen = ((int) ((rm [40] - 48) + (rm [39] - 48) * 10 + (rm [38] - 48) * 100 + (rm [37] - 48) * 1000)); // / 100;
incr_regen = rm [32] - 40;


for (p = 0; p < 6; p ++)
{
        thing_quant [p] = rm [41 + p];
}
temp_long = atol(thing_quant);
xp = temp_long - 100000;

xl = rm [47] - 40;

//rm [45] = xl + 40;
/*
for (p = 0; p < 5; p ++)
{
        thing_quant [p] = rm [48 + p];
}
temp_int = atoi(thing_quant);
gp = temp_int - 10000;
*/
        gp = (rm [52] - 48) + (rm [51] - 48) * 10 + (rm [50] - 48) * 100 + (rm [49] - 48) * 1000;

//rm [50] = clas + 40;
clas = rm [53] - 40;

/*
for (p = 0; p < 5; p ++)
{
        thing_quant [p] = rm [58 + p];
}
temp_int = atoi(thing_quant);
f_abil = (temp_int / 100) - 10000;*/

        f_abil = (int) ((rm [58] - 48) + (rm [57] - 48) * 10 + (rm [56] - 48) * 100 + (rm [55] - 48) * 1000);

/*
for (p = 0; p < 5; p ++)
{
        thing_quant [p] = rm [63 + p];
}
temp_int = atoi(thing_quant);
speed = (temp_int / 100) - 10000;*/

        speed = (int) ((rm [63] - 48) + (rm [62] - 48) * 10 + (rm [61] - 48) * 100 + (rm [60] - 48) * 1000);// / 100;


/*
for (p = 0; p < 5; p ++)
{
        thing_quant [p] = rm [68 + p];
}
temp_int = atoi(thing_quant);
speed_inc = (temp_int / 100) - 10000;*/

        speed_inc = (int) ((rm [68] - 48) + (rm [67] - 48) * 10 + (rm [66] - 48) * 100 + (rm [65] - 48) * 1000);// / 100;

read(handle, rm, 4);
// max values
max_strength = rm [0] - 40;
max_intel = rm [1] - 40;
max_dex = rm [2] - 40;
hunger_inc = rm [3] - 40;

 read(handle, rm, 10);

        mag_abil = (int) ((rm [4] - 48) + (rm [3] - 48) * 10 + (rm [2] - 48) * 100 + (rm [1] - 48) * 1000);
        thr_abil = (int) ((rm [9] - 48) + (rm [8] - 48) * 10 + (rm [7] - 48) * 100 + (rm [6] - 48) * 1000);




        read(handle, rm, 2);

        x_pos = (int) rm [0] - 40;
        y_pos = (int) rm [1] - 40;

//      x_pos = atoi(itf);
//      rm [0] = itf;

//      y_pos = atoi(itf);
//      rm [1] = itf;


        read(handle, rm, 13);

        strcpy(clasnam, rm);

read(handle, rm, 5);
/*for (p = 0; p < 5; p ++)
{
        thing_quant [p] = rm [0 + p];
}
temp_int = atoi(thing_quant);
burden = temp_int - 10000;*/

        burden = (rm [4] - 48) + (rm [3] - 48) * 10 + (rm [2] - 48) * 100 + (rm [1] - 48) * 1000;
        // burden reloads wrongly if it is negative.

 spell_no = 0;
 read(handle, rm, 25);
 for (i = 0; i < 25; i ++)
 {
  spells [i] = rm [i] - 30;
  if (spells [i] != -1) spell_no ++;
 }

inv_no = 0;


for (i = 0; i < 52; i++)
{
//      read(handle, rm, 30);
//      strcpy(inv_name [i], rm);
        read(handle, rm, 11);

        inv_class [i] = rm [0] - 40;
        inv_type [i] = rm [1] - 40;
        inv_plus [i] = rm [2] - 40;
        inv_dam [i] = rm [3] - 40;
        inv_col [i] = rm [4] - 40;

/*      for (p = 0; p < 5; p ++)
        {
                thing_quant [p] = rm [5 + p];
        }
        temp_int = atoi(thing_quant);
        inv_quant [i] = temp_int - 10000;*/

        inv_ident [i] = rm [5] - 40;

        inv_quant [i] = (rm [10] - 48) + (rm [9] - 48) * 10 + (rm [8] - 48) * 100 + (rm [7] - 48) * 1000;// + (rm [4] - 48) * 10000;

        if (inv_quant [i] > 0) inv_no++;

} // end of for i

/*
        for (i = 0; i < 4; i ++)
        {
                read(handle, rm, 30);
                for (p = 0; p < 30; p ++)
                {
                        //rm [p] = get_id [i] [p] + 40;
                        //cout << (int) rm [p];
                        set_id(i + 100, p, rm [p] - 40);
                }
        } // end of for i
*/

        read(handle, rm, 30);
        for (i = 0; i < 30; i ++)
        {
                levels_exist [i] = rm [i];
                //lev_ex [i] = 0;
        }
        //write(handle, rm, 30);


for (i = 0; i < 5; i ++)
{
read(handle, rm, 30);

   for (j = 0; j < 30; j ++)
   {
    item_description [i] [j] = rm [j] - 40;
   }
}

for (i = 0; i < 4; i ++)
{
read(handle, rm, 30);

   for (j = 0; j < 30; j ++)
   {
//    item_description [i] [j] = rm [j] - 40;
//      set_id(i, j, rm [j] - 40);
 switch(i)
 {
        case 0: set_id(3, j, rm [j] - 40); break;
        case 1: set_id(6, j, rm [j] - 40); break;
        case 2: set_id(7, j, rm [j] - 40); break;
        case 3: set_id(8, j, rm [j] - 40); break;
 }
   }
}

   read(handle, rm, 50);

   for (j = 0; j < 50; j ++)
   {
      unique_items [j] = rm [j] - 40; // unique items
   }




        //rm [0] = getch();


//       clasnam [13]; // Now?
/*
        char inv_name [52] [30];
        char inv_class [52];
        char inv_type [52];
        char inv_plus [52];
        char inv_dam [52];
        char inv_col [52];
        int inv_quant [52]; // multiple items, eg ammo or potions
        int burden; // total weight of items carried.
        char inv_no; // number of items carried.


        handle = open("lev000.dat", O_RDONLY, S_IWRITE, S_IREAD); break;


        rm [0] = mons_inv [i] [j] + 40;

                write(handle, rm, 1);
        */

        close(handle);

/*

        strncpy(char_f, your_name, 8);
        strcat(char_f, ".dat");

        handle = open(char_f, O_CREAT | O_TRUNC | O_BINARY, S_IREAD);

        read(handle,rm,2);

        x_pos = (int) rm [0];
        y_pos = (int) rm [1];

        close(handle);

*/















        /*
        char tm [10] [30];

        handle = open("control.dat", S_IWRITE, S_IREAD);

        for (i = 0; i < 10; i++)
        {
                read(handle, tm [i] , 30);
        }

        strcpy(tm [newc], "deceased");

        close (handle);


        handle = open("control.dat", O_WRONLY, S_IWRITE, S_IREAD);

        for (i = 0; i < 10; i++)
        {
                write(handle, tm [i] , 30);

        }

        //strcpy(tm [i], "deceased");

        close (handle);

        */



        //int sysg = 0;

//      sysg = system("cd c:\borlandc\crawl");

        if (sysg == -1)
        {
                perror("Oh dear... ");
                exit(0);
        }





 char del_file [25];

        strcpy(del_file, "del ");
        strcat(del_file, char_f);
        sysg = system(del_file);

        if (sysg == -1)
        {
                perror("\n\rYou haven't done something naughty to the save file, have you?\n\r");
                exit(-1);
        }





//abort();






   speed = 10;



} // end of save_game()

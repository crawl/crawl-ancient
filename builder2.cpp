
#include <iostream.h>
//#include <graphics.h>

#include <stdlib.h>

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <fstream.h>
#include <fcntl.h>
#include <io.h>
#include <SYS\STAT.H>

#include <assert.h>

#define ITEMS 500
#define MNST 200
#define NTRAPS 30
#define CLOUDS 30

#define MHITNOT 201
//#include <dos.h>

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




unsigned char border_type = 1;

//int igrid [80] [70];
//unsigned char grid [80] [70];
//char something;
int bcount_x;
int bcount_y;
int bi;
int bj;
int bk;
//int bl;
int bp;
int room_x1;
int room_y1;
int room_x2;
int room_y2;
int x_max;
int y_max;
int no_rooms;
int max_doors;
int diag_door = 0;

//unsigned char stair_x [10];
//unsigned char stair_y [10];

// these are the variables for individual levels.
int doorlevel;
int corrlength;
int roomsize;
int no_corr;
int intersect_chance = 0;
int bno_mons = 0;

int time_run = 0; // for repeating search for 1-door rooms
//int many = 0;
int many_many = 0; // for multiple levels

int x_start, y_start, x_fin, y_fin;
int x_ps; int y_ps;
int finish = 0;
int length;
int dir_x;
int dir_y;
int old_x = 0;
int old_y = 0;
int dir_x2 = 0;
int dir_y2 = 0;
int rannumber;
int rannum;




int put_x;
int put_y;
int q;

int passout;

int handle;
char save_file [9];
char whole_line [80];
//int counting;
int leng_str;
char wm_1 [5];
char wm_2;

//char gmon_use [200];

int skipped = 0;
int randnum;

int lev_mons = 0;
int band = 0;
int band_no = 0;


int x1, x2, y1, y2;
int cx = 0;
int cy = 0;
int sx = 0;
int sy = 0;
int dx = 0;
int dy = 0;
int rd = 0;

char dung1;
char dung2;

char oblique;
char oblique_max;

char is_a_specroom = 0;





char filenam [80];




//                      REMEMBER to run levels.bat !!!



int mons_rarity(int mcls);
int mons_level(int mcls);



void define_monster(int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_sec [MNST], int k);//, int x_pos, int y_pos);

void make_room(unsigned char grid [80] [70]);

//void screen_show(void);

void make_trail(unsigned char grid [80] [70]);

int items(int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
unsigned char grid [80] [70],
int force_class,
int force_type,
int force_place,
int many_many
);

void give_item(int igrid [80] [70],
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
int mons_inv [MNST] [8],
unsigned char mons_sec [MNST],
unsigned char mons_ench_1 [MNST],
unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
unsigned char grid [80] [70]
);

void special_room(
int igrid [80] [70],
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
unsigned char grid [80] [70]);

void specr_2(unsigned char grid [80] [70]);

//void name_item(void);
//void save(void);
//void save_one(void);

void place_traps(unsigned char grid [80] [70], unsigned char trap_type [NTRAPS],
 unsigned char trap_x [NTRAPS], unsigned char trap_y [NTRAPS]);

int place_specific_trap(unsigned char grid [80] [70], unsigned char trap_type [NTRAPS],
 unsigned char trap_x [NTRAPS], unsigned char trap_y [NTRAPS], unsigned char spec_x,
unsigned char spec_y, unsigned char spec_type);


//void stair_check(void);

void link_items(int mons_inv [MNST] [8], unsigned char item_class [ITEMS], char unique_items [50], unsigned char item_x [ITEMS],
 unsigned char item_x [ITEMS], unsigned char item_y [ITEMS], int item_link [ITEMS],
 int igrid [80] [70], int item_quant [ITEMS]);

void big_room(unsigned char grid [80] [70]);

void diamond_rooms(unsigned char grid [80] [70]);

void octa_room(unsigned char grid [80] [70], unsigned char type_floor);

void item_colour(int p,
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
unsigned char icolour [ITEMS],
unsigned char item_description [5] [30]);

int random3(unsigned int rmax);

void roguey_level(
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
unsigned char grid [80] [70],
unsigned char stair_x [10],
unsigned char stair_y [10]);

void city_level(
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
unsigned char grid [80] [70],
unsigned char stair_x [10],
unsigned char stair_y [10],
int igrid [80] [70]);

void labyrinth_level(
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
unsigned char grid [80] [70],
unsigned char stair_x [10],
unsigned char stair_y [10],
unsigned char mons_hit [MNST],
char gmon_use [400],
int igrid [80] [70],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS]);



void box_room(int bx1, int bx2, int by1, int by2, unsigned char grid [80] [70], int wall_type);

void beehive(
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
unsigned char grid [80] [70]
);

void check_doors(unsigned char grid [80] [70]);


void monalloc(int levy, unsigned char mons_alloc [20]);

void morgue(
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
unsigned char grid [80] [70]);


int xmons_zombie_size(int mclass);

void define_zombie(char not_zombsize, int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_sec [MNST]);



int builder(
int igrid [80] [70],
unsigned char grid [80] [70],
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
 unsigned char mons_hit [MNST],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
char gmon_use [400],
unsigned char stair_x [10],
unsigned char stair_y [10],
unsigned int lev_numb,
int x_pos,
int y_pos,
char level_type
);


int place_monster(int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
int mons_inv [MNST] [8],
unsigned char mons_sec [MNST],
unsigned char mons_ench_1 [MNST],
unsigned char mons_ench [MNST] [3],
unsigned char mons_hit [MNST],
unsigned char mons_beh [MNST],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
char plus_seventy,
int typed, int type_place, int px, int py, char behaviour, int hitting,
char allow_bands,
char gmon_use [400],
unsigned char grid [80] [70],
unsigned char mons_alloc [20],
int passed [2],
int igrid [80] [70],
int x_pos,
int y_pos
);

/*
int place_monster(int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
int mons_inv [MNST] [8],
unsigned char mons_sec [MNST],
unsigned char mons_ench_1 [MNST],
unsigned char mons_ench [MNST] [3],
unsigned char mons_hit [MNST],
unsigned char mons_beh [MNST],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
char plus_seventy,
int typed, int type_place, int px, int py, char behaviour, int hitting,
char allow_bands,
char gmon_use [200],
unsigned char grid [80] [70],
unsigned char mons_alloc [20]
);
*/




void spellbook_template(int sbook_type, int func_pass [10]);





int builder(
int igrid [80] [70],
unsigned char grid [80] [70],
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
 unsigned char mons_hit [MNST],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
char gmon_use [400],
unsigned char stair_x [10],
unsigned char stair_y [10],
unsigned int lev_numb,
int x_pos,
int y_pos,
char level_type
)
{
//randomize();

unsigned char xtra_quant;

srandom(time(NULL));

many_many = lev_numb;




/*if (argc != 3)
{
        cprintf("Wrong number of args!");
        exit(-1);

}*/

/*cout << argv [0];
cout << argv [1];
cout << argv [2];
*
cprintf("args:");
cprintf("\n\r");
cprintf(argv [0]);
cprintf("\n\r");
cprintf(argv [1]);
cprintf("\n\r");
cprintf(argv [2]);
cprintf("\n\r");
cprintf(argv [3]);
cprintf("\n\r");
cprintf(argv [4]);
cprintf("\n\r");
cprintf(argv [5]);
cprintf("\n\r");
cprintf(argv [6]);
cprintf("\n\r");
cprintf(argv [7]);
cprintf("\n\r");
/*cprintf(argv [8]);
cprintf("\n\r");*/

//getch();

char eggo [10];


strcpy(filenam, "Linley.lev");



                        dung1 = random3(100);
                        dung2 = random3(100);

border_type = 1;
if (many_many == 60) border_type = 4;

for (bcount_x = 0; bcount_x < 80; bcount_x ++)
{
        for (bcount_y = 0; bcount_y < 70; bcount_y ++)
        {
                grid [bcount_x] [bcount_y] = 1; // must be 1
                grid [bcount_x] [6] = border_type;
                grid [bcount_x] [63] = border_type;
                grid [6] [bcount_y] = border_type;
                grid [73] [bcount_y] = border_type;
                igrid [bcount_x] [bcount_y] = 501;
        }
}

for (bcount_x = 0; bcount_x < NTRAPS; bcount_x ++)
{
        trap_type [bcount_x] = 100;
        trap_x [bcount_x] = 1;
        trap_y [bcount_x] = 1;
}


for (bcount_x = 0; bcount_x < ITEMS; bcount_x ++)
{
        item_class [bcount_x] = 100;
        item_type [bcount_x] = 0;
        item_plus [bcount_x] = 0;
        item_dam [bcount_x] = 0;
        bcount_y = 100000;
        //ltoa(bcount_y, item_quant [bcount_x], 10);//remember that the 10 is the base (radix)!
//      itoa(bcount_y, item_quant [bcount_x], 10);
 item_quant [bcount_x] = 0; //bcount_y;
        icolour [bcount_x] = 0;
        item_x [bcount_x] = 0;
        item_y [bcount_x] = 0;
        item_ident [bcount_x] = 0;
        item_link [bcount_x] = 501;
//      strcpy (fake_name [bcount_x], "");
}

for (bi = 0; bi < 10; bi ++)
{
 stair_x [bi] = 1;
 stair_y [bi] = 1;
}

for (bi = 0; bi < MNST; bi++)
{
        mons_class [bi] = 255;
        mons_targ_1_x [bi] = random3(80);
        mons_targ_1_y [bi] = random3(70);
        mons_beh [bi] = 0;
 mons_ench_1 [bi] = 0;
 mons_ench [bi] [0] = 0;
 mons_ench [bi] [1] = 0;
 mons_ench [bi] [2] = 0;

 if (random3(10) == 0) mons_beh [bi] = 1;
        //if (random3(3) == 0) mons_beh [bi] = 2 + 100;
 for (bj = 0; bj < 8; bj ++)
        {
                mons_inv [bi] [bj] = 501;
        }
        mons_sec [bi] = 0;

}



/*
for (bcount_y = 20; bcount_y < 40; bcount_y ++)
{
        grid [25] [bcount_y] = 105;
        grid [55] [bcount_y] = 105;
}

for (bcount_y = 25; bcount_y < 56; bcount_y ++)
{
        grid [bcount_y] [20] = 105;
        grid [bcount_y] [40] = 105;
}
*/
/*
for (bcount_x = 30; bcount_x < 50; bcount_x ++)
{
        for (bcount_y = 30; bcount_y < 50; bcount_y ++)
        {
                grid [bcount_x] [bcount_y] = 105;
        }
}
*/
//cprintf("11");
if (level_type == 1)
{
//cprintf("22");
        labyrinth_level(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_x, mons_y,    mons_beh,       mons_inv,
 mons_targ_1_x, mons_targ_1_y,  mons_sec, mons_ench_1, mons_ench,
no_it, item_ident, item_class, unique_items, item_type, item_plus, item_dam, item_quant,
item_x, item_y, icolour, xtra_quant, item_link, item_description, property,
trap_type, trap_x, trap_y, mons_alloc, grid, stair_x, stair_y, mons_hit,
gmon_use, igrid, trap_type, trap_x, trap_y);
return 0;
}
// must correct gmon_use and mcolour to a higher subscript value

is_a_specroom = 0;
int done_city = 0;

if (many_many == 60) goto do_city; // Dis
//goto do_city;

                if (random3(3) == 0 && many_many != 65) skipped = 1;
// was that^^

//      skipped = 1;


            //V was 3
if ((random3(3) == 0 && skipped == 0) | many_many == 65)
{

        // is_a_specroom can be changed to 2 in this function:
        // in which case it shouldn't be done again.

        roguey_level(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_x, mons_y,    mons_beh,       mons_inv,
 mons_targ_1_x, mons_targ_1_y,  mons_sec, mons_ench_1, mons_ench,
no_it, item_ident, item_class, unique_items, item_type, item_plus, item_dam, item_quant,
item_x, item_y, icolour, xtra_quant, item_link, item_description, property,
trap_type, trap_x, trap_y, mons_alloc, grid, stair_x, stair_y);

        if (skipped == 1)
        goto skip_level;
} else
if (random3(8) == 0 && skipped == 0 && many_many > 13)
{
do_city : city_level(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_x, mons_y,    mons_beh,       mons_inv,
 mons_targ_1_x, mons_targ_1_y,  mons_sec, mons_ench_1, mons_ench,
 no_it, item_ident, item_class, unique_items, item_type, item_plus, item_dam, item_quant,
 item_x, item_y, icolour, xtra_quant, item_link, item_description, property,
 trap_type, trap_x, trap_y, mons_alloc, grid, stair_x, stair_y, igrid);
 done_city = 1;
done_city = 1;
// goto ending;
// return 0;
}


// is_a_specroom: 0 = none, 2 = in roguey_level (no need for specr_2), 1 = special_room (vault)
if (random3(5) == 0 && is_a_specroom != 2 && many_many > 5 && done_city == 0 && many_many < 50)
//         (3)
//if (is_a_specroom != 2)
{
        is_a_specroom = 1;
        special_room(igrid, mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_x, mons_y,    mons_beh,       mons_inv,
 mons_targ_1_x, mons_targ_1_y,  mons_sec, mons_ench_1, mons_ench,
no_it, item_ident, item_class, unique_items, item_type, item_plus, item_dam, item_quant,
item_x, item_y, icolour, xtra_quant, item_link, item_description, property,
trap_type, trap_x, trap_y, mons_alloc, grid);

}

//room_x1 = 8;
//room_x2 = 15;
//room_y1 = 7;
//room_y2 = 16;



//for (bi = 0; bi < 10; bi ++)



//for (bj = 0; j < 25; j ++)
//{

doorlevel = random3(11);
corrlength = random3 (14) + 2;
roomsize = random3(5) + 4;
no_corr = random3(200) + 30;
if (random3(100) == 0) no_corr = 500 + random3(500);
//no_corr = random3(360) + 965; // new, smaller, leaner dungeon.
intersect_chance = random3(20);
if (random3(20) == 0) intersect_chance = 400;

//no_corr = 10;

if (done_city == 0)
{

        do
        {
                x_start = random3(30) + 35 ; y_start = random3(20) + 35;
        } while(grid [x_start] [y_start] != 1 && grid [x_start] [y_start] != 17);
        grid [x_start] [y_start] = 18;
 stair_x [0] = x_start;
 stair_y [0] = y_start;
        x_ps = x_start;
        y_ps = y_start;
        make_trail(grid);
        grid [x_ps] [y_ps] = 18; //19;
 stair_x [1] = x_start;
 stair_y [1] = y_start;




//if (random3(10) <= 7)
//{
        do
        {
                x_start = random3(15) + 10 ; y_start = random3(15) + 10;
        } while(grid [x_start] [y_start] != 1 && grid [x_start] [y_start] != 17);
        grid [x_start] [y_start] = 18;
 stair_x [2] = x_start;
 stair_y [2] = y_start;
        finish = 0;
        x_ps = x_start;
        y_ps = y_start;
        make_trail(grid);
        grid [x_ps] [y_ps] = 18;
 stair_x [3] = x_start;
 stair_y [3] = y_start;

//}

//if (random3(10) <= 7) // must be omitted so that each level has one of each kind of stair.
//{
        do
        {
                x_start = random3(20) + 50 ; y_start = random3(15) + 10;
        } while(grid [x_start] [y_start] != 1 && grid [x_start] [y_start] != 17);
        grid [x_start] [y_start] = 18;
 stair_x [4] = x_start;
 stair_y [4] = y_start;
        finish = 0;
        x_ps = x_start;
        y_ps = y_start;
        make_trail(grid);
        grid [x_ps] [y_ps] = 18;
 stair_x [5] = x_start;
 stair_y [5] = y_start;
//}

if (random3(4) == 0)
{
        do
        {
                x_start = random3(20) + 10 ; y_start = random3(20) + 40;
        } while(grid [x_start] [y_start] != 1 && grid [x_start] [y_start] != 17);
        grid [x_start] [y_start] = 18;
// stair_x [4] = x_start;
// stair_y [4] = y_start;
        finish = 0;
        x_ps = x_start;
        y_ps = y_start;
        make_trail(grid);
        grid [x_ps] [y_ps] = 18;
}


if (random3(4) == 0)
{
        do
        {
                x_start = random3(20) + 50 ; y_start = random3(20) + 40;
        } while(grid [x_start] [y_start] != 1 && grid [x_start] [y_start] != 17);
        grid [x_start] [y_start] = 18;
// stair_x [4] = x_start;
// stair_y [4] = y_start;
        finish = 0;
        x_ps = x_start;
        y_ps = y_start;
        make_trail(grid);
        grid [x_ps] [y_ps] = 18;
}

} // end of done_city



//if (random3(10) <= 7)
/*
{
        do
        {
                x_start = random3(20) + 10 ; y_start = random3(20) + 40;
        } while(grid [x_start] [y_start] != 1 && grid [x_start] [y_start] != 17);
        finish = 0;
        x_ps = x_start;
        y_ps = y_start;
        make_trail(grid);
}

//if (random3(10) <= 7)
{
        do
        {
                x_start = random3(20) + 50 ; y_start = random3(20) + 40;
        } while(grid [x_start] [y_start] != 1 && grid [x_start] [y_start] != 17);
        finish = 0;
        x_ps = x_start;
        y_ps = y_start;
        make_trail(grid);
}


        do
        {
                x_start = random3(10) + 40 ; y_start = random3(10) + 35;
        } while(grid [x_start] [y_start] != 1 && grid [x_start] [y_start] != 17);
//      grid [x_start] [y_start] = 18;
        x_ps = x_start;
        y_ps = y_start;
        make_trail(grid);
//      grid [x_ps] [y_ps] = 18;

*/



/*
if (random3(10) <= 7)
{
x_start = random3(20) + 40 ; y_start = random3(20) + 110;
        finish = 0;
        x_ps = x_start;
        y_ps = y_start;
        make_trail();
}

if (random3(10) <= 7)
{
x_start = random3(20) + 110 ; y_start = random3(20) + 80;
        finish = 0;
        x_ps = x_start;
        y_ps = y_start;
        make_trail();
}

*/

/*
if (random3(10) == 0) // big room
{

        max_doors = 100;
        room_x1 = random3 (55) + 8;
        room_y1 = random3 (45) + 8;
        room_x2 = room_x1 + random3(roomsize) + 2;
        room_y2 = room_y1 + random3(roomsize) + 2;

make_room();
        if (time_run > 30)
        {
                time_run = 0;
                k++;
                continue;
        }


}
*/

if (random3(25) == 0 && done_city == 0)
{
        big_room(grid);
}

if ((random3(many_many) >= 10 && random3(3) == 0) | many_many > 60)
{
        diamond_rooms(grid);
}

no_rooms = random3 (15) + random3(15) + 5;


if (random3(15) == 0)
{
        no_rooms = 100;
}

if (random3(50) == 0)
{
        no_rooms = 1;
}

if (done_city == 1) no_rooms = 0;
//                      no_rooms = random3 (150);// + 50;

//no_rooms = 3;

max_doors = 2 + random3 (8);



for (bk = 0; bk < no_rooms; bk ++)
{
room_x1 = random3 (55) + 8;
room_y1 = random3 (45) + 8;
room_x2 = room_x1 + random3(roomsize) + 2;
room_y2 = room_y1 + random3(roomsize) + 2;

make_room(grid);
if (time_run > 30)
{
time_run = 0;
bk++;
continue;
}

}

no_rooms = random3 (3) + 1;
max_doors = 1;

if (done_city == 1) no_rooms = 0;

for (bk = 0; bk < no_rooms; bk ++)
{
room_x1 = random3 (55) + 8;
room_y1 = random3 (45) + 8;
room_x2 = room_x1 + random3(6) + 5;
room_y2 = room_y1 + random3(6) + 5;

make_room(grid);
if (time_run > 30)
{
time_run = 0;
bk++;
continue;
}

}

if (is_a_specroom == 1) specr_2(grid);


skip_level: bi = 0;

/*if (is_a_specroom == 0 && many_many > 2 && random3(50) == 0)
{
        for (bcount_x = ?0?; bcount_x < ?80?;


}*/




//save();
/*

switch (many)
{
        case 0: handle = open("level.000", O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE); break; // with O_CREATE, will not change already existing file, ie must delete to refresh.
//strcpy(save_file, "level.000"); break;
        case 1: handle = open("level.001", O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE); break; // with O_CREATE, will not change already existing file, ie must delete to refresh.
//strcpy(save_file, "level.001"); break;
        case 2: handle = open("level.002", O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE); break;
//strcpy(save_file, "level.002"); break;
        case 3: handle = open("level.003", O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE); break;
//strcpy(save_file, "level.003"); break;
        case 4: handle = open("level.004", O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE); break;
//strcpy(save_file, "level.004"); break;
}
*/
/*
switch (many)
{
        case 0: handle = open("level.000", O_WRONLY | O_CREAT | O_TRUNC, S_IWRITE, S_IREAD); break; //, S_IREAD | S_IWRITE); break; // with O_CREATE, will not change already existing file, ie must delete to refresh.
//strcpy(save_file, "level.000"); break;
        case 1: handle = open("level.001", O_WRONLY | O_CREAT | O_TRUNC, S_IWRITE, S_IREAD); break; // with O_CREATE, will not change already existing file, ie must delete to refresh.
//strcpy(save_file, "level.001"); break;
        case 2: handle = open("level.002", O_WRONLY | O_CREAT | O_TRUNC, S_IWRITE, S_IREAD); break;
//strcpy(save_file, "level.002"); break;
        case 3: handle = open("level.003", O_WRONLY | O_CREAT | O_TRUNC, S_IWRITE); break;
//strcpy(save_file, "level.003"); break;
        case 4: handle = open("level.004", O_WRONLY | O_CREAT | O_TRUNC, S_IWRITE); break;
//strcpy(save_file, "level.004"); break;
}



//handle = open("level.000", O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);

//strcpy(save_file, "level.004");
//if ((handle = open("TEST.$$$", O_CREAT)) != -1)
//handle = open(save_file, O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE); // with O_CREATE, will not change already existing file, ie must delete to refresh.
//                      O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE
if (handle > -1)
{
for (counter = 0; counter < 140; counter ++)
{
        //for (bcount_y = 0; bcount_y < 140; bcount_y ++)
        //{
        strcpy(whole_line, grid [counter]);
        leng_str = strlen(whole_line);
        if (write(handle, whole_line, leng_str) != leng_str)
        {
        printf("Something wrong with write");
        something = getch();
        return;
        }
        //}
}

  close(handle);                    /* close the file */
/*
}
else
{
  printf("Error opening file\n");
}

handle = 0;
printf("Hello!");




//if (!f1) cerr << "Cannot open level file.";
/*
for (bcount_x = 0; bcount_x < 140; bcount_x ++)
{
        for (bcount_y = 0; bcount_y < 140; bcount_y ++)
        {
                f1.put(grid [bcount_x] [bcount_y]);
        }
}
*/






//screen_show();
//something = getch();








/*

for (bcount_x = 0; bcount_x < 140; bcount_x ++)
{
        for (bcount_y = 0; bcount_y < 140; bcount_y ++)
        {
                grid [bcount_x] [bcount_y] = 1;
                grid [bcount_x] [1] = 2;
                grid [bcount_x] [139] = 2;
                grid [1] [bcount_y] = 2;
                grid [139] [bcount_y] = 2;
        }
}


*/




/*



strcpy(gmon_name [0], "hairy blob");
//gmon_HD [0] = 3;
gmon_att [0] [0] = 5;
//gmon_AC [0] = 6;
//gmon_ev [0] = 4;
//int gmon_speed [52]; // speed - not in use yet
//gmon_use [0] = 0;
mcolour [0] = 12;

strcpy(gmon_name [1], "hairless blob");
//gmon_HD [1] = 3;
gmon_att [1] [0] = 5;
//gmon_AC [1] = 6;
//gmon_ev [1] = 4;
//int gmon_speed [52]; // speed - not in use yet
//gmon_use [1] = 0;
mcolour [1] = 13;

strcpy(gmon_name [2], "fuzzy blob");
//gmon_HD [2] = 3;
gmon_att [2] [0] = 5;
//gmon_AC [2] = 6;
//gmon_ev [2] = 4;
//int gmon_speed [52]; // speed - not in use yet
//gmon_use [2] = 0;
mcolour [2] = 5;
*/

// WAS HERE!!!
bno_mons = 0;

bno_mons += random3(25) + random3 (15) + random3 (15) + random3 (many_many);// + random3 (15);

if (bno_mons > 120) bno_mons = 120;

bk = 0;

//bno_mons = 100; //115;

//cout << "Started monsters...   ";



/*
place_monster(mons_class,
mons_hp,
mons_hp_max,
mons_HD,
mons_AC,
mons_ev,
mons_speed,
mons_speed_inc,
mons_inv,
mons_sec,
mons_ench_1,
mons_ench,
mons_hit,
mons_beh,
no_it,
item_ident,
item_class,
item_type,
item_plus,
item_dam,
item_quant,
item_x,
item_y,
icolour,
xtra_quant,
item_link,
item_description,
property,
mons_x,
mons_y,
1, //plus_seventy,
250, // typed,
1, //type_place,
1, //px,
1, //py,
0, //behaviour,
MHITNOT, //hitting,
1,//allow_bands
gmon_use,
grid,
mons_alloc
);
*/
int kloppo = 0;

for (kloppo = 0; kloppo < bno_mons; kloppo ++)
{
int passed [2];

kloppo += place_monster(mons_class,
mons_hp,
mons_hp_max,
mons_HD,
mons_AC,
mons_ev,
mons_speed,
mons_speed_inc,
mons_inv,
mons_sec,
mons_ench_1,
mons_ench,
mons_hit,
mons_beh,
no_it,
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
xtra_quant,
item_link,
item_description,
property,
mons_x,
mons_y,
1, //plus_seventy,
250, // typed,
0, //type_place,
1, //px,
1, //py,
0, //behaviour,
MHITNOT, //hitting,
1,//1,//allow_bands
gmon_use,
grid,
mons_alloc,
passed,
igrid,
2, //x_pos,
2  //y_pos
);

}
// keep kloppo for later (aquatic monsters)

/*
' ',
0, 0, 0, 0, ' ', 0,
' ',
gmon_use,
grid,
mons_alloc
);
/*char plus_seventy,
int typed, int type_place, int px, int py, char behaviour, int hitting,
char allow_bands,
char gmon_use [200],
char grid [80] [70],
unsigned char mons_alloc [20]*/





// do aquatic and lava monsters:

int lava_spaces = 0;
int water_spaces = 0;
int aq_creatures = 0;
int type_of_aq = 0;
int swimming_things [4];

for (bcount_x = 0; bcount_x < 80; bcount_x ++)
{
        for (bcount_y = 0; bcount_y < 70; bcount_y ++)
        {
                if (grid [bcount_x] [bcount_y] == 11) lava_spaces ++;
                if (grid [bcount_x] [bcount_y] == 12) water_spaces ++;
        }
}

if (lava_spaces >= 50)
{
for (bcount_x = 0; bcount_x < 4; bcount_x ++)
{
 swimming_things [bcount_x] = MLAVA0 + random3(3);
}
 aq_creatures = random3(5) + random3(5) + random3(lava_spaces) / 10;
 if (aq_creatures > 25) aq_creatures = 25;
 for (bcount_x = 0; bcount_x < aq_creatures; bcount_x ++)
 {
//  type_of_aq = MLAVA0 + random3(3);
type_of_aq = swimming_things [random3(4)];
int passed [2];
  kloppo += place_monster(mons_class,
mons_hp,
mons_hp_max,
mons_HD,
mons_AC,
mons_ev,
mons_speed,
mons_speed_inc,
mons_inv,
mons_sec,
mons_ench_1,
mons_ench,
mons_hit,
mons_beh,
no_it,
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
xtra_quant,
item_link,
item_description,
property,
mons_x,
mons_y,
1, //plus_seventy,
type_of_aq, // typed,
0, //type_place,
1, //px,
1, //py,
0, //behaviour,
MHITNOT, //hitting,
1,//1,//allow_bands
gmon_use,
grid,
mons_alloc,
passed,
igrid,
2, //x_pos,
2  //y_pos
);
if (kloppo >= 100) break;

 }
}

if (water_spaces >= 50)
{
for (bcount_x = 0; bcount_x < 4; bcount_x ++)
{
 swimming_things [bcount_x] = MWATER0 + random3(4);
}
 aq_creatures = random3(5) + random3(5) + random3(water_spaces) / 7;
 if (aq_creatures > 25) aq_creatures = 25;
 for (bcount_x = 0; bcount_x < aq_creatures; bcount_x ++)
 {
//  type_of_aq = MWATER0 + random3(4);
type_of_aq = swimming_things [random3(4)];
int passed [2];
kloppo += place_monster(mons_class,
mons_hp,
mons_hp_max,
mons_HD,
mons_AC,
mons_ev,
mons_speed,
mons_speed_inc,
mons_inv,
mons_sec,
mons_ench_1,
mons_ench,
mons_hit,
mons_beh,
no_it,
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
xtra_quant,
item_link,
item_description,
property,
mons_x,
mons_y,
1, //plus_seventy,
type_of_aq, // typed,
0, //type_place,
1, //px,
1, //py,
0, //behaviour,
MHITNOT, //hitting,
1,//1,//allow_bands
gmon_use,
grid,
mons_alloc,
passed,
igrid,
2, //x_pos,
2  //y_pos
);
if (kloppo >= 100) break;
 }
}






if (many_many >= 20 && many_many < 50)
{
 do
 {
  bi = random3(70);
  bj = random3(60);
 } while (grid [bi] [bj] != 17);
 grid [bi] [bj] = 19;
}

char strs = 0;
if (many_many == 26)
{
 for (bi = 1; bi < 80; bi ++)
 {
        for (bj = 1; bj < 70; bj ++)
        {
                if (grid [bi] [bj] == 18)
   {
     if (strs == 0)
     {
      strs ++;
      continue; // need to adapt this for trapdoors, descent etc!
     }
     grid [bi] [bj] = 19;
   }
        }
 }
}

int orb = 0;

if (many_many == 65)
{
 for (bi = 1; bi < 80; bi ++)
 {
        for (bj = 1; bj < 70; bj ++)
        {
                if (grid [bi] [bj] == 18 | grid [bi] [bj] == 19) grid [bi] [bj] = 17;
        }
 }

orb = items(no_it, item_ident, item_class, unique_items, item_type, item_plus, item_dam,
 item_quant, item_x, item_y, icolour, xtra_quant, item_link, item_description,
 grid, 12, 250, 0, many_many);
int passed [2];
place_monster(mons_class,
mons_hp,
mons_hp_max,
mons_HD,
mons_AC,
mons_ev,
mons_speed,
mons_speed_inc,
mons_inv,
mons_sec,
mons_ench_1,
mons_ench,
mons_hit,
mons_beh,
no_it,
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
xtra_quant,
item_link,
item_description,
property,
mons_x,
mons_y,
1, //plus_seventy,
31, // typed,
1, //type_place,
item_x [orb], //px,
item_y [orb], //py,
0, //behaviour,
MHITNOT, //hitting,
0,//1,//allow_bands
gmon_use,
grid,
mons_alloc,
passed,
igrid,
2, //x_pos,
2  //y_pos
);


}



for (bi = 1; bi < 80; bi ++)
{
        for (bj = 1; bj < 70; bj ++)
        {
                if (grid [bi] [bj] >= 70) grid [bi] [bj] -= 70;
        }
}




                                                        check_doors(grid);

//                                                      stair_check();

place_traps(grid, trap_type, trap_x, trap_y);


no_it = random3 (15) + random3 (10) + random3(many_many);// + random3(30);
if (random3(500 - many_many) <= 3) no_it += random3(100); // rich level!
int plojy = 0;
if (many_many >= 50) no_it = 0;
 else
  for (plojy = 0; plojy < no_it; plojy++)
  {

   items(no_it, item_ident, item_class, unique_items, item_type, item_plus, item_dam,
    item_quant, item_x, item_y, icolour, xtra_quant, item_link, item_description,
    grid, 250, 250, 0, many_many);
  }



// resets spec_room characters:
// The numbers are -70 because of the bit above where all grid spaces which
// are >70 get -70.

for (bcount_x = 0; bcount_x < 80; bcount_x ++)
{
        for (bcount_y = 0; bcount_y < 70; bcount_y ++)
        {
                if (grid [bcount_x] [bcount_y] == 35) grid [bcount_x] [bcount_y] = 1;
                if (grid [bcount_x] [bcount_y] == 47) grid [bcount_x] [bcount_y] = 17;
                if (grid [bcount_x] [bcount_y] == 36) grid [bcount_x] [bcount_y] = 17;
      if (grid [bcount_x] [bcount_y] == 67) grid [bcount_x] [bcount_y] = 17;
      if (grid [bcount_x] [bcount_y] == 117) grid [bcount_x] [bcount_y] = 17;
        }
}









link_items(mons_inv, item_class, unique_items, item_x, item_x, item_y, item_link, igrid, item_quant);

//                                                      save_one(); // break;


//cprintf("built!");

ending : return 0;

} // end of int main(..., ...)



void monalloc(int levy, unsigned char mons_alloc [20])
{

/*
switch (levy)
{
case 0: mons_alloc [0] = 6;
mons_alloc [1] = 6;
mons_alloc [2] = 6;
mons_alloc [3] = 1;
mons_alloc [4] = 1;
mons_alloc [5] = 33;
mons_alloc [6] = 17;
mons_alloc [7] = 36;
mons_alloc [8] = 36;
mons_alloc [9] = 17;
mons_alloc [10] = 6;
mons_alloc [11] = 6;
mons_alloc [12] = 6;
mons_alloc [13] = 1;
mons_alloc [14] = 1;
mons_alloc [15] = 33;
mons_alloc [16] = 17;
mons_alloc [17] = 36;
mons_alloc [18] = 36;
mons_alloc [19] = 17; break;

case 1: mons_alloc [0] = 6;
mons_alloc [1] = 6;
mons_alloc [2] = 6;
mons_alloc [3] = 1;
mons_alloc [4] = 1;
mons_alloc [5] = 33;
mons_alloc [6] = 33;
mons_alloc [7] = 36;
mons_alloc [8] = 36;
mons_alloc [9] = 17;
mons_alloc [10] = 6;
mons_alloc [11] = 6;
mons_alloc [12] = 6;
mons_alloc [13] = 1;
mons_alloc [14] = 1;
mons_alloc [15] = 33;
mons_alloc [16] = 33;
mons_alloc [17] = 36;
mons_alloc [18] = 36;
mons_alloc [19] = 17;

break;

case 2: mons_alloc [0] = 6;
mons_alloc [1] = 6;
mons_alloc [2] = 1;
mons_alloc [3] = 1;
mons_alloc [4] = 1;
mons_alloc [5] = 33;
mons_alloc [6] = 33;
mons_alloc [7] = 36;
mons_alloc [8] = 36;
mons_alloc [9] = 33;
mons_alloc [10] = 6;
mons_alloc [11] = 6;
mons_alloc [12] = 1;
mons_alloc [13] = 1;
mons_alloc [14] = 1;
mons_alloc [15] = 33;
mons_alloc [16] = 33;
mons_alloc [17] = 36;
mons_alloc [18] = 36;
mons_alloc [19] = 33;
break;

case 3: mons_alloc [0] = 6;
mons_alloc [1] = 7;
mons_alloc [2] = 1;
mons_alloc [3] = 33;
mons_alloc [4] = 36;
mons_alloc [5] = 14;
mons_alloc [6] = 14;
mons_alloc [7] = 36;
mons_alloc [8] = 33;
mons_alloc [9] = 25;
mons_alloc [10] = 6;
mons_alloc [11] = 7;
mons_alloc [12] = 1;
mons_alloc [13] = 33;
mons_alloc [14] = 58; // kobold band
mons_alloc [15] = 14;
mons_alloc [16] = 14;
mons_alloc [17] = 36;
mons_alloc [18] = 33;
mons_alloc [19] = 25;
break;

case 4: mons_alloc [0] = 6;
mons_alloc [1] = 7;
mons_alloc [2] = 33;
mons_alloc [3] = 14;
mons_alloc [4] = 14;
mons_alloc [5] = 14;
mons_alloc [6] = 45;
mons_alloc [7] = 0;
mons_alloc [8] = 36;
mons_alloc [9] = 0;
mons_alloc [10] = 6;
mons_alloc [11] = 7;
mons_alloc [12] = 33;
mons_alloc [13] = 58;
mons_alloc [14] = 58;
mons_alloc [15] = 14;
mons_alloc [16] = 45;
mons_alloc [17] = 58;
mons_alloc [18] = 36;
mons_alloc [19] = 0;
break;

case 5: mons_alloc [0] = 1;
mons_alloc [1] = 14;
mons_alloc [2] = 14;
mons_alloc [3] = 14;
mons_alloc [4] = 58;
mons_alloc [5] = 0;
mons_alloc [6] = 0;
mons_alloc [7] = 58;
mons_alloc [8] = 55;
mons_alloc [9] = 25;
mons_alloc [10] = 34;
mons_alloc [11] = 14;
mons_alloc [12] = 14;
mons_alloc [13] = 14;
mons_alloc [14] = 58;
mons_alloc [15] = 0;
mons_alloc [16] = 0;
mons_alloc [17] = 38;
mons_alloc [18] = 55;
mons_alloc [19] = 25;
break;

case 6: mons_alloc [0] = 58;
mons_alloc [1] = 14;
mons_alloc [2] = 14;
mons_alloc [3] = 0;
mons_alloc [4] = 40;
mons_alloc [5] = 32;
mons_alloc [6] = 58;
mons_alloc [7] = 25;
mons_alloc [8] = 27;
mons_alloc [9] = 40;
mons_alloc [10] = 14;
mons_alloc [11] = 14;
mons_alloc [12] = 58;
mons_alloc [13] = 0;
mons_alloc [14] = 34;
mons_alloc [15] = 32;
mons_alloc [16] = 5;
mons_alloc [17] = 25;
mons_alloc [18] = 27;
mons_alloc [19] = 40;
break;


case 7: mons_alloc [0] = 52;
mons_alloc [1] = 14;
mons_alloc [2] = 0;
mons_alloc [3] = 12;
mons_alloc [4] = 58;
mons_alloc [5] = 58;
mons_alloc [6] = 5;
mons_alloc [7] = 34;
mons_alloc [8] = 40;
mons_alloc [9] = 38;
mons_alloc [10] = 14;
mons_alloc [11] = 14;
mons_alloc [12] = 0;
mons_alloc [13] = 12;
mons_alloc [14] = 34;
mons_alloc [15] = 10;
mons_alloc [16] = 5;
mons_alloc [17] = 40;
mons_alloc [18] = 40;
mons_alloc [19] = 38;
break;

case 8: mons_alloc [0] = 14;
mons_alloc [1] = 14;
mons_alloc [2] = 45;
mons_alloc [3] = 35;
mons_alloc [4] = 34;
mons_alloc [5] = 38;
mons_alloc [6] = 58;
mons_alloc [7] = 40;
mons_alloc [8] = 40;
mons_alloc [9] = 27;
mons_alloc [10] = 14;
mons_alloc [11] = 14;
mons_alloc [12] = 45;
mons_alloc [13] = 35;
mons_alloc [14] = 10;
mons_alloc [15] = 38;
mons_alloc [16] = 52;
mons_alloc [17] = 40;
mons_alloc [18] = 40;
mons_alloc [19] = 27;
break;

case 9: mons_alloc [0] = 52;
mons_alloc [1] = 52;
mons_alloc [2] = 14;
mons_alloc [3] = 35;
mons_alloc [4] = 30;
mons_alloc [5] = 28;
mons_alloc [6] = 40;
mons_alloc [7] = 53;
mons_alloc [8] = 53;
mons_alloc [9] = 27;
mons_alloc [10] = 14;
mons_alloc [11] = 14;
mons_alloc [12] = 58;
mons_alloc [13] = 35;
mons_alloc [14] = 30;
mons_alloc [15] = 28;
mons_alloc [16] = 40;
mons_alloc [17] = 53;
mons_alloc [18] = 53;
mons_alloc [19] = 27;
break;

case 10: mons_alloc [0] = 52;
mons_alloc [1] = 52;
mons_alloc [2] = 32;
mons_alloc [3] = 35;
mons_alloc [4] = 30;
mons_alloc [5] = 28;
mons_alloc [6] = 40;
mons_alloc [7] = 53;
mons_alloc [8] = 53;
mons_alloc [9] = 27;
mons_alloc [10] = 14;
mons_alloc [11] = 14;
mons_alloc [12] = 58;
mons_alloc [13] = 35;
mons_alloc [14] = 30;
mons_alloc [15] = 28;
mons_alloc [16] = 40;
mons_alloc [17] = 53;
mons_alloc [18] = 53;
mons_alloc [19] = 27;
break;

} // end of switch (many_many)

*/

} // end of monalloc(int ...)



int mons_level(int mcls)
{

switch(mcls)
{
case 0: return 8; // "giant ant"
case 1: return 4; // "giant bat"
case 2: return 7; // "centaur"
case 3: return 60; // "red devil"
case 4: return 12; // "ettin"
case 5: return 8; // "fungus"
case 6: return 1; // "goblin"
case 7: return 6; // "hound"
case 8: if (many_many < 60) return 10; // "imp"
            else return 60;
case 9: return 5; // "jackal"
case 10: return 11; // "killer bee"
case 11: return 5; // "killer bee larva"
case 12: return 15; // "manticore"
case 13: return 10; // "necrophage"
case 14: return 7; // "orc"
case 15: return 8; // "phantom"
case 16: return 9; // "quasit"
case 17: return 4; // "rat"
case 18: return 8; // "scorpion"
case 19: return 24; // "tunneling worm"
case 20: return 21; // "ugly thing"
case 21: return 60; // "fire vortex"
case 22: return 6; // "worm"
case 23: return 91; // "abomination"
case 24: return 9; // "yellow wasp"
case 25: if (many_many < 60) return 10; // "zombie"
           else return 60;
case 26: return 15; // "angel"
case 27: return 12; // "giant beetle"
case 28: return 18; // "cyclops"
case 29: return 20; // "dragon"
case 30: return 15; // "two-headed ogre"
case 31: return 500; // "fiend"
case 32: return 12; // "giant spore"
case 33: return 3; // "hobgoblin"
case 34: return 9; // "ice beast"
case 35: return 9; // "jelly"
case 36: return 2; // "kobold"
case 37: return 27; // "lich"
case 38: return 10; // "mummy"
case 39: return 13; // "naga"
case 40: return 9; // "ogre"
case 41: return 14; // "plant"
case 42: return 22; // "queen bee"
case 43: return 17; // "raksasha"
case 44: return 6; // "snake"
case 45: return 13; // "troll"
case 46: return 12; // "unseen horror"
case 47: return 16; // "vampire"
case 48: return 15; // "wraith"
case 49: return 1500; // "abomination"
case 50: return 13; // "yak"
case 51: if (many_many < 60) return 20; // "zombie"
           else return 60;
case 52: return 10; // "orc warrior"
case 53: return 11; // "kobold demonologist"
case 54: return 8; // "orc wizard"
case 55: return 22; // "orc knight"
case 56: return 9999; // "worm tail"
case 57: return 12; // "wyvern"
case 58: return 15; // "kobold leader"
case 59: return 8; // "giant eyeball"
case 60: return 10; // "wight"
case 61: return 13; // "oklob plant"
case 62: return 16; // "wolf spider"
case 63: return 11; // "shadow"
case 64: return 11; // "hungry ghost"
case 65: return 14; // "eye of draining"
case 66: return 20; // "butterfly"
case 67: return 18; // "wandering mushroom"
case 68: return 18; // "efreet"
case 69: return 16; // "brain worm"
case 70: return 26; // "giant orange brain"

case 71: return 24; // "boulder beetle"
case 72: return 65; // "flying skull"
case 73: return 61; // "hell hound"
case 74: return 50; // "minotaur"
case 75: return 25; // "ice dragon"
case 76: return 15; // "slime thing"
case 77: return 25; // "freezing wraith"
case 78: return 5000; // fake R
case 79: return 25; // GooE
case 80: return 60; // "hellion"
case 81: return 60; // "rotting devil"
case 82: return 62; // "tormentor"
case 83: return 63; // "reaper"
case 84: return 65; // "soul eater"
case 85: return 60; // "hairy devil"
case 86: return 6500; // "ice devil" not really appropriate for a fiery hell.
case 87: return 6200; // "blue devil"
case 100: return 5; //strcat(gmon_name, "giant mite"); break;
case 101: return 10; //strcat(gmon_name, "steam dragon"); break;
case 102: return 25; //strcat(gmon_name, "very ugly thing"); break;
case 103: return 24; //strcat(gmon_name, "orc sorceror"); break;
case 104: return 15; //strcat(gmon_name, "hippogriff"); break;
case 105: return 21; //strcat(gmon_name, "griffon"); break;
case 106: return 23; //strcat(gmon_name, "hydra"); break;
case MLAVA0: return 500; // "lava worm"
case MLAVA1: return 500; // "lava fish"
case MLAVA2: return 500; // "lava snake"
case MLAVA3: return 500; // "another lava thing"
case MWATER0: return 500; // "big fish"
case MWATER1: return 500; // "giant goldfish"
case MWATER2: return 500; // "electrical eel"
case MWATER3: return 500; // "jellyfish"
case MWATER4: return 500; // ""
case MWATER5: return 500; // ""

default: return 99;


}
 return 50;
} // end mons_level(mcls)



int mons_rarity(int mcls)
{

switch(mcls)
{
case 0: return 80; // "giant ant"
case 1: return 99; // "giant bat"
case 2: return 70; // "centaur"
case 3: return 99; // "red devil"
case 4: return 0; // "ettin"
case 5: return 20; // "fungus"
case 6: return 99; // "goblin"
case 7: return 70; // "hound"
case 8: return 99; // imp
case 9: return 80; // "jackal"
case 10: return 50; // "killer bee"
case 11: return 0; // "killer bee larva"
case 12: return 45; // "manticore"
case 13: return 30; // "necrophage"
case 14: return 99; // "orc"
case 15: return 40; // "phantom"
case 16: return 0; // "quasit"
case 17: return 99; // "rat"
case 18: return 50; // "scorpion"
case 19: return 20; // "tunneling worm"
case 20: return 99; // "ugly thing"
case 21: return 30; // "fire vortex"
case 22: return 50; // "worm"
case 23: return 0; // "abomination"
case 24: return 30; // "yellow wasp"
case 25: return 99;
case 26: return 0; // "angel"
case 27: return 50; // "giant beetle"
case 28: return 70; // "cyclops"
case 29: return 30; // "dragon"
case 30: return 40; // "two-headed ogre"
case 31: return 0; // "fiend"
case 32: return 40; // "giant spore"
case 33: return 99; // "hobgoblin"
case 34: return 50; // "ice beast"
case 35: return 60; // "jelly"
case 36: return 99; // "kobold"
case 37: return 17; // "lich"
case 38: return 30; // "mummy"
case 39: return 10; // "naga"
case 40: return 70; // "ogre"
case 41: return 25; // "plant"
case 42: return 0; // "queen bee"
case 43: return 20; // "raksasha"
case 44: return 80; // "snake"
case 45: return 70; // "troll"
case 46: return 50; // "unseen horror"
case 47: return 30; // "vampire"
case 48: return 40; // "wraith"
case 49: return 0; // "abomination"
case 50: return 70; // "yak"
case 51: return 99; // Z
case 52: return 70; // "orc warrior"
case 53: return 0; // "kobold demonologist"
case 54: return 50; // "orc wizard"
case 55: return 60; // "orc knight"
case 56: return 0; // "worm tail"
case 57: return 40; // "wyvern"
case 58: return 50; // "kobold leader"
case 59: return 40; // "giant eyeball"
case 60: return 40; // "wight"
case 61: return 10; // "oklob plant"
case 62: return 36; // "wolf spider"
case 63: return 41; // "shadow"
case 64: return 41; // "hungry ghost"
case 65: return 33; // "eye of draining"
case 66: return 20; // "butterfly"
case 67: return 10; // "wandering mushroom"
case 68: return 15; // "efreet"
case 69: return 26; // "brain worm"
case 70: return 10; // "giant orange brain"

case 71: return 34; // "boulder beetle"
case 72: return 75; // "flying skull"
case 73: return 71; // "hell hound"
case 74: return 0; // "minotaur"
case 75: return 20; // "ice dragon"
case 76: return 75; // "slime thing"
case 77: return 45; // "freezing wraith"
case 78: return 0; // fake R
case 79: return 25; // GooE
case 80: return 70; // "hellion"
case 81: return 60; // "rotting devil"
case 82: return 50; // "tormentor"
case 83: return 40; // "reaper"
case 84: return 30; // "soul eater"
case 85: return 40; // "hairy devil"
case 86: return 0; // "ice devil" not really appropriate for a fiery hell.
case 87: return 0; // "blue devil"
case 100: return 30; //strcat(gmon_name, "giant mite"); break;
case 101: return 20; //strcat(gmon_name, "steam dragon"); break;
case 102: return 20; //strcat(gmon_name, "very ugly thing"); break;
case 103: return 10; //strcat(gmon_name, "orc sorceror"); break;
case 104: return 50; //strcat(gmon_name, "hippogriff"); break;
case 105: return 40; //strcat(gmon_name, "griffon"); break;
case 106: return 20; //strcat(gmon_name, "hydra"); break;
case MLAVA0: return 500; // "lava worm"
case MLAVA1: return 500; // "lava fish"
case MLAVA2: return 500; // "lava snake"
case MLAVA3: return 500; // "another lava thing"
case MWATER0: return 500; // "big fish"
case MWATER1: return 500; // "giant goldfish"
case MWATER2: return 500; // "electrical eel"
case MWATER3: return 500; // "jellyfish"
case MWATER4: return 500; // ""
case MWATER5: return 500; // ""

default: return 0;
}

return 50;

} // end int mons_rarity




/*

case 0: return ; // "giant ant"
case 1: return ; // "giant bat"
case 2: return ; // "centaur"
case 3: return ; // "red devil"
case 4: return ; // "ettin"
case 5: return ; // "fungus"
case 6: return ; // "goblin"
case 7: return ; // "hound"
case 8: return ; // "imp"
case 9: return ; // "jackal"
case 10: return ; // "killer bee"
case 11: return ; // "killer bee larva"
case 12: return ; // "manticore"
case 13: return ; // "necrophage"
case 14: return ; // "orc"
case 15: return ; // "phantom"
case 16: return ; // "quasit"
case 17: return ; // "rat"
case 18: return ; // "scorpion"
case 19: return ; // "tunneling worm"
case 20: return ; // "ugly thing"
case 21: return ; // "fire vortex"
case 22: return ; // "worm"
case 23: return ; // "abomination"
case 24: return ; // "yellow wasp"
case 25: break;
case 26: return ; // "angel"
case 27: return ; // "giant beetle"
case 28: return ; // "cyclops"
case 29: return ; // "dragon"
case 30: return ; // "two-headed ogre"
case 31: return ; // "fiend"
case 32: return ; // "giant spore"
case 33: return ; // "hobgoblin"
case 34: return ; // "ice beast"
case 35: return ; // "jelly"
case 36: return ; // "kobold"
case 37: return ; // "lich"
case 38: return ; // "mummy"
case 39: return ; // "naga"
case 40: return ; // "ogre"
case 41: return ; // "plant"
case 42: return ; // "queen bee"
case 43: return ; // "raksasha"
case 44: return ; // "snake"
case 45: return ; // "troll"
case 46: return ; // "unseen horror"
case 47: return ; // "vampire"
case 48: return ; // "wraith"
case 49: return ; // "abomination"
case 50: return ; // "yak"
case 51: break;
case 52: return ; // "orc warrior"
case 53: return ; // "kobold demonologist"
case 54: return ; // "orc wizard"
case 55: return ; // "orc knight"
case 56: return ; // "worm tail"
case 57: return ; // "wyvern"
case 58: return ; // "kobold leader"
case 59: return ; // "giant eyeball"
case 60: return ; // "wight"
case 61: return ; // "oklob plant"
case 62: return ; // "wolf spider"
case 63: return ; // "shadow"
case 64: return ; // "hungry ghost"
case 65: return ; // "eye of draining"
case 66: return ; // "butterfly"
case 67: return ; // "wandering mushroom"
case 68: return ; // "efreet"
case 69: return ; // "brain worm"
case 70: return ; // "giant orange brain"

case 71: return ; // "boulder beetle"
case 72: return ; // "flying skull"
case 73: return ; // "hell hound"
case 74: return ; // "minotaur"
case 75: return ; // "ice dragon"
case 76: return ; // "slime thing"
case 77: return ; // "freezing wraith"
case 78: return ; // ""
case 80: return ; // "hellion"
case 81: return ; // "rotting devil"
case 82: return ; // "tormentor"
case 83: return ; // "reaper"
case 84: return ; // "soul eater"
case 85: return ; // "hairy devil"
case 86: return ; // "ice devil"
case 87: return ; // "blue devil"
case MLAVA0: return ; // "lava worm"
case MLAVA1: return ; // "lava fish"
case MLAVA2: return ; // "lava snake"
case MLAVA3: return ; // "another lava thing"
case MWATER0: return ; // "big fish"
case MWATER1: return ; // "giant goldfish"
case MWATER2: return ; // "electrical eel"
case MWATER3: return ; // "jellyfish"
case MWATER4: return ; // ""
case MWATER5: return ; // ""

*/


int place_monster(int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
int mons_inv [MNST] [8],
unsigned char mons_sec [MNST],
unsigned char mons_ench_1 [MNST],
unsigned char mons_ench [MNST] [3],
unsigned char mons_hit [MNST],
unsigned char mons_beh [MNST],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
char plus_seventy,
int typed, int type_place, int px, int py, char behaviour, int hitting,
char allow_bands,
char gmon_use [400],
unsigned char grid [80] [70],
unsigned char mons_alloc [20],
int passed [2],
int igrid [80] [70],
int x_pos,
int y_pos)
{

unsigned char grid_ok = 17;
int worm = 0;
int worm_init = 1;
int plussed = 0;

int k = 0;

band = 0;
band_no = 0;

start_here : for (k = 0; k < MNST; k ++)
{
 if (k == MNST - 1) return plussed;
        if (mons_class [k] == 255 | mons_class [k] == -1) break;
}

if (type_place == 1 && grid [px] [py] >= 50)
{
 return 0;
}

bk = k;
passed [0] = k;
//cprintf("x");
//while (bk < bno_mons)
//{

//if (mons_class [bk] != 255 && mons_class [bk] != -1)
//{
//      bk++;
//      if (bno_mons < 150) bno_mons++;
//      continue;
//}



// //} // end while

if (typed == 250)
{

monster_cla :


if (random3(4) == 0 && many_many < 50) lev_mons = random3(many_many);
        else lev_mons = many_many;




//monalloc(lev_mons, mons_alloc);

//mons_class [bk] = mons_alloc [random3(20)];

do
{
 mons_class [bk] = random3(107);
 if (mons_rarity(mons_class [bk]) == 0) continue;
} while(!(lev_mons > mons_level(mons_class [bk]) - 5 && lev_mons < mons_level(mons_class [bk]) + 5 && random3(100) <= mons_rarity(mons_class [bk]) - (mons_level(mons_class [bk]) - lev_mons) * (mons_level(mons_class [bk]) - lev_mons)));

//mons_class [bk] = random3(69);
//if (mons_class [bk] == 56 | mons_class [bk] == 19) mons_class [bk] = 58;

//if (random3(3) != 0) mons_class [bk] = 19;

if (type_place == 1 && mons_class [bk] == 19) goto monster_cla;

if (bk > 50 && mons_class [bk] == 19) goto monster_cla;

//if (random3(3) == 0) mons_class [bk] = 72;

}
 else (mons_class [bk] = typed);

//if (random3(4) == 0)
//mons_class [bk] = 19; // choose

if (mons_class [bk] >= MLAVA0)
{
 mons_sec [bk] = 0;
 if (mons_class [bk] >= MWATER0) grid_ok = 12; else grid_ok = 11;
}

worm_tail:
if (worm == 5)
{
        worm = 0;
        goto start_here; //continue;
}
if (worm > 0 && worm < 5)
{
        if (worm > 1)
        {
                bk ++;
        }
        mons_class [bk] = 56;
        worm++;
        //cout << "Worm tail! ";
}


if (mons_class [bk] == 19)
{
        //cout << "A worm! " << (int) k; //(int) mons_x [bk] - 100 << "," << (int) mons_y [bk] - 100;
        worm = 1;
 mons_class [bk] = -1;
 while ((mons_class [bk + 1] != 255 | mons_class [bk + 2] != 255 | mons_class [bk + 3] != 255 | mons_class [bk + 4] != 255) && (mons_class [bk + 1] != -1 | mons_class [bk + 2] != -1 | mons_class [bk + 3] != -1 | mons_class [bk + 4] != -1))
 {
  bk ++;
//  cprintf("x");
  if (bk > MNST - 10) return 0;
 }
 mons_class [bk] = 19;
}



switch(band)
{
case 0: break;
case 1: if (band_no > 0) mons_class [bk] = 36; break;
case 2: if (band_no > 0) mons_class [bk] = 14; break;
case 3: if (band_no > 0)
switch (random3(8))
{
case 0: mons_class [bk] = 14; break;
case 3:
case 4:
case 1: mons_class [bk] = 52; break;
case 2: mons_class [bk] = 54; break;
case 5: mons_class [bk] = 40; break;
case 6: mons_class [bk] = 45; break;
case 7: mons_class [bk] = 103; break; // orc sorceror
} break;
case 4: if (band_no > 0) mons_class [bk] = 10; break; // k bees
case 5: if (band_no > 0) mons_class [bk] = 72; break; // flying skulls
case 6: if (band_no > 0) mons_class [bk] = 76; break; // slime thing
case 7: if (band_no > 0) mons_class [bk] = 50; break; // yak
case 8: if (band_no > 0) mons_class [bk] = 20;
if (band_no > 0 && random3(4) == 0 && many_many > 21) mons_class [bk] = 102;
break; // ugly thing
case 9: if (band_no > 0) mons_class [bk] = 73; break; // hell hound
case 10: if (band_no > 0) mons_class [bk] = 9; break; // jackal
}



if (mons_class [bk] < MLAVA0) mons_sec [bk] = 250;

define_monster(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_sec, bk);

plussed ++;

if (mons_class [bk] == 25 | mons_class [bk] == 51) define_zombie(3, mons_class,
mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev, mons_speed, mons_speed_inc,
mons_sec);

if (mons_class [bk] == 46)
{
 mons_ench_1 [bk] = 1;
 mons_ench [bk] [2] = 6;
}

if (mons_class [bk] == 66 | mons_class [bk] == 21) // butter or vort
{
 mons_ench_1 [bk] = 1;
 mons_ench [bk] [0] = 5;
}



//do
{


switch(band)
{
case 0: // no band
passout = 0;

//while (passout == 0)
if (mons_class [bk] == 19) // assuming can never be summoned
{
  do
  {
   mons_x [bk] = 10 + random3(70);
   mons_y [bk] = 10 + random3(60);
  } while (grid [mons_x [bk]] [mons_y [bk]] < 17 | grid [mons_x [bk]] [mons_y [bk]] >= 50 | grid [mons_x [bk] + 1] [mons_y [bk]] < 17 | grid [mons_x [bk] + 1] [mons_y [bk]] >= 50 | grid [mons_x [bk] + 2] [mons_y [bk]] < 17 | grid [mons_x [bk] + 2] [mons_y [bk]] >= 50 | grid [mons_x [bk] + 3] [mons_y [bk]] < 17 | grid [mons_x [bk] + 3] [mons_y [bk]] >= 50 | grid [mons_x [bk] + 4] [mons_y [bk]] < 17 | grid [mons_x [bk] + 4] [mons_y [bk]] >= 50);
// forget checking for x_pos/y_pos
} else
if (type_place == 1) // summoned
{
   mons_x [bk] = px;
   mons_y [bk] = py;
} else
 if (grid_ok == 17)
        {
  do
  {
   mons_x [bk] = 10 + random3(70);
   mons_y [bk] = 10 + random3(60);
  } while (grid [mons_x [bk]] [mons_y [bk]] < 17 | grid [mons_x [bk]] [mons_y [bk]] >= 50 | (mons_x [bk] == x_pos && mons_y [bk] == y_pos));
 } // end if
else do
                                          {
        mons_x [bk] = 10 + random3(70);
        mons_y [bk] = 10 + random3(60);
       } while (grid [mons_x [bk]] [mons_y [bk]] != grid_ok | (mons_x [bk] == x_pos && mons_y [bk] == y_pos));

if (mons_class [bk] == 56) //worm > 1) // worm test
{
        mons_x [bk] = mons_x [bk - 1] + 1;
        mons_y [bk] = mons_y [bk - 1];
        if (grid [mons_x [bk]] [mons_y [bk]] == 1) grid [mons_x [bk]] [mons_y [bk]] = grid_ok;
        bj++;
        if (plus_seventy == 1) grid [mons_x [bk]] [mons_y [bk]] += 70;
   else grid [mons_x [bk]] [mons_y [bk]] += 50;
//      assert(bj < 50);
        goto worm_tail;
}

bj = 0;



break;


case 2: // orcs following warrior
case 1: // kobolds
default: //  right?
mons_x [bk] = mons_x [bk - 1];
mons_y [bk] = mons_y [bk - 1];
//passout = 0;

//while (passout == 0)
//{

do
{
        mons_x [bk] += random3 (3) - 1;
        mons_y [bk] += random3 (3) - 1;
        if (mons_x [bk] > 79 | mons_x [bk] < 5 | mons_y [bk] > 69 | mons_y [bk] < 5)
        {
                mons_x [bk] = mons_x [bk - 1];
                mons_y [bk] = mons_y [bk - 1];
        }
} while (grid [mons_x [bk]] [mons_y [bk]] < 17 | grid [mons_x [bk]] [mons_y [bk]] >= 50 | (mons_x [bk] == x_pos && mons_y [bk] == y_pos)); //!= grid_ok);
band_no --;
// cannot have bands of water creatures!
break;



}


if (allow_bands == 1)
{
switch(mons_class [bk])
{
case 52: if (bk < MNST - 50 && band == 0 && band_no == 0)// && many_many > 10)
 {
        band = 2; // orcs
        band_no = 2 + random3(3);
 }
break;

case 58: if (bk < MNST - 50 && band == 0 && band_no == 0 && many_many > 3)
 {
//  mons_class [bk] = 36;
        band = 1; // K
        band_no = 2 + random3(6);
 }
break;

case 55: if (bk < MNST - 50 && band == 0 && band_no == 0)
 {
  band = 3; // orcs + knight
  band_no = 3 + random3(4);
 }
break;

case 10: if (bk < MNST - 50 && band == 0 && band_no == 0)
 {
  band = 4; // killer bees
  band_no = 2 + random3(4);
 }
break;

case 72: if (bk < MNST - 50 && band == 0 && band_no == 0)
 {
  band = 5; // flying skulls
  band_no = 2 + random3(4);
 }
break;

case 76: if (bk < MNST - 50 && band == 0 && band_no == 0)
 {
  band = 6; // slime creatures
  band_no = 2 + random3(4);
 }
break;


case 50: if (bk < MNST - 50 && band == 0 && band_no == 0)
 {
  band = 7; // yaks
  band_no = 2 + random3(4);
 }
break;


case 20: if (bk < MNST - 50 && band == 0 && band_no == 0)
 {
  band = 8; // ugly things
  band_no = 2 + random3(4);
 }
break;


case 73: if (bk < MNST - 50 && band == 0 && band_no == 0)
 {
  band = 9; // hell hound
  band_no = 2 + random3(3);
 }
break;

case 9: if (bk < MNST - 50 && band == 0 && band_no == 0)
 {
  band = 10; // jackal
  band_no = 2 + random3(4);
 }
break;

} // end switch
} // end if


if (band > 0 && band_no == 0)
{
   band = 0;
}


} //while (grid [mons_x [bk]] [mons_y [bk]] != grid_ok);
    // surely I don't need this?!

if (grid [mons_x [bk]] [mons_y [bk]] != grid_ok && mons_class [bk] != 74)
{
        if (band_no > 0) band_no ++;
 goto start_here; // what?!
        //continue;
}


//if (mons_class [bk] < MLAVA0)
{
if (plus_seventy == 1) grid [mons_x [bk]] [mons_y [bk]] += 70;
  else grid [mons_x [bk]] [mons_y [bk]] += 50;
}


                                if (gmon_use [mons_class [bk]] > 0) give_item(igrid,
mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev, mons_speed,
mons_speed_inc, mons_inv, mons_sec, mons_ench_1, mons_ench, no_it,
item_ident, item_class, unique_items, item_type, item_plus, item_dam, item_quant,
item_x, item_y, icolour, xtra_quant, item_link, item_description,
property, mons_x, mons_y, grid);

                                if (mons_class [bk] == 30 | mons_class [bk] == 2) give_item(igrid, mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev, mons_speed,
mons_speed_inc, mons_inv, mons_sec, mons_ench_1, mons_ench, no_it,
item_ident, item_class, unique_items, item_type, item_plus, item_dam, item_quant,
item_x, item_y, icolour, xtra_quant, item_link, item_description,
property, mons_x, mons_y, grid);

//plussed ++;
//bk++;

mons_beh [bk] = behaviour;
mons_hit [bk] = hitting;


if (mons_class [bk] == 46)
{
 mons_ench_1 [bk] = 1;
 mons_ench [bk] [2] = 6;
}

if (mons_class [bk] == 66)
{
        mons_ench_1 [bk] = 1;
        mons_ench [bk] [0] = 5;
}

if (band != 0) goto start_here;


if (mons_class [bk] == 0)
{
//      continue;
}



if (worm != 0) goto start_here;


return plussed;

//}
//cprintf("z");

} // end int place_monster















void define_monster(int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_sec [MNST], int k)
{

//int k = bk;


switch(mons_class [k])
{

case 0: // giant ant
mons_HD [k] = 3;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 4;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 12; // this is divided by 10 when reloaded
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 1: // giant bat
mons_HD [k] = 1;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmon_AC [2];
mons_ev [k] = 20;//gmon_ev [2];
mons_speed [k] = 30;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 2: // centaur
mons_HD [k] = 4;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 3;//gmon_AC [2];
mons_ev [k] = 7;//gmon_ev [2];
mons_speed [k] = 15; // this is divided by 10 when reloaded
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 3;
break;

case 3: // devil
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 10;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 4: // ettin
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 3;//gmon_AC [2];
mons_ev [k] = 4;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 5: // fungus
mons_HD [k] = 8;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmon_AC [2];
mons_ev [k] = 0;//gmon_ev [2];
mons_speed [k] = 0;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 6: // goblin
mons_HD [k] = 1;
mons_hp [k] = (random3((mons_HD [k]) * 4) + (mons_HD [k]) * 2);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmon_AC [2];
mons_ev [k] = 13;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;


case 7: // hound
mons_HD [k] = 3;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 2;//gmon_AC [2];
mons_ev [k] = 13;//gmon_ev [2];
mons_speed [k] = 15;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 8: // imp
mons_HD [k] = 1;
mons_hp [k] = (random3((mons_HD [k]) * 10) + (mons_HD [k]) * 10);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 3;//gmon_AC [2];
mons_ev [k] = 20;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 9: // jackal
mons_HD [k] = 1;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 2;//gmon_AC [2];
mons_ev [k] = 12;//gmon_ev [2];
mons_speed [k] = 14;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 10: // killer bee
mons_HD [k] = 3;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 2;//gmon_AC [2];
mons_ev [k] = 18;//gmon_ev [2];
mons_speed [k] = 20;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 11: // killer bee larva
mons_HD [k] = 1;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmon_AC [2];
mons_ev [k] = 5;//gmon_ev [2];
mons_speed [k] = 5;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 12: // manticore
mons_HD [k] = 9;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 5;//gmon_AC [2];
mons_ev [k] = 7;//gmon_ev [2];
mons_speed [k] = 7;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 13: // nymph???
mons_HD [k] = 3;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 2;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7; //      Look at this one!
//gmon_use [mons_class [k]] = 0;
break;

case 14: // orc
mons_HD [k] = 1;
mons_hp [k] = (random3((mons_HD [k]) * 8) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 0;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 15: // phantom
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 3;//gmon_AC [2];
mons_ev [k] = 13;//gmon_ev [2];                 // Look at this one!!
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 16: // quasit
mons_HD [k] = 1;
mons_hp [k] = (random3((mons_HD [k]) * 12) + (mons_HD [k]) * 8);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 5;//gmon_AC [2];
mons_ev [k] = 17;//gmon_ev [2];
mons_speed [k] = 15;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 17: // rat
mons_HD [k] = 1;
mons_hp [k] = (random3((mons_HD [k]) * 3) + (mons_HD [k]) * 1);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 18: // scorpion
mons_HD [k] = 3;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 5;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 19: // tunneling worm
mons_HD [k] = 10;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 5);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 3;//gmon_AC [2];                  // Look at this one!!!
mons_ev [k] = 3;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 20: // ugly thing
mons_HD [k] = 8;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 3;//gmon_AC [2]; //               Look at this one!
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 3;
break;

case 21: // vortex
mons_HD [k] = 3;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 0;//gmon_AC [2];
mons_ev [k] = 5;//gmon_ev [2];  // Look at this one!!!
mons_speed [k] = 15;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 22: // worm
mons_HD [k] = 5;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmon_AC [2];
mons_ev [k] = 5;//gmon_ev [2];
mons_speed [k] = 6;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 23: // x????
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 10;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];   // Look at this one!!!
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
if (mons_sec [k] == 250) mons_sec [k] = random3(15) + 1;
//gmon_use [mons_class [k]] = 0;
break;

case 24: // yellow wasp
mons_HD [k] = 4;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 5;//gmon_AC [2];
mons_ev [k] = 14;//gmon_ev [2]; // Look at this one!!!
mons_speed [k] = 15;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 25: // small zombie
mons_HD [k] = random3(2) + 1;
mons_hp [k] = (random3((mons_HD [k]) * 10) + (mons_HD [k]) * 10);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 0;//gmon_AC [2];
mons_ev [k] = 4;//gmon_ev [2];
mons_speed [k] = 5;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 26: // Angel
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 10;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2]; //      Look at this one!!!
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 27: // giant Beetle
mons_HD [k] = 5;
mons_hp [k] = (random3((mons_HD [k]) * 7) + (mons_HD [k]) * 6);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 10;//gmon_AC [2];
mons_ev [k] = 3;//gmon_ev [2];
mons_speed [k] = 5;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 28: // Cyclops
mons_HD [k] = 9;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 5;//gmon_AC [2];
mons_ev [k] = 3;//gmon_ev [2];
mons_speed [k] = 7;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 29: // Dragon
mons_HD [k] = 10;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 5);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 10;//gmon_AC [2];
mons_ev [k] = 8;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 30: // two-headed Ogre
mons_HD [k] = 5;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmon_AC [2];
mons_ev [k] = 4;//gmon_ev [2];  // Look at this one!!!
mons_speed [k] = 8;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 31: // Fiend
mons_HD [k] = 15;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 15;//gmon_AC [2];
mons_ev [k] = 6;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 32: // Giant spore
mons_HD [k] = 1;
mons_hp [k] = 1; // (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 0;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 15;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 33: // Hobgoblin
mons_HD [k] = 1;
mons_hp [k] = (random3((mons_HD [k]) * 6) + (mons_HD [k]) * 4);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 0;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 34: // Ice beast
mons_HD [k] = 5;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 5;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 35: // Jelly
mons_HD [k] = 3;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 5);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 0;//gmon_AC [2];
mons_ev [k] = 2;//gmon_ev [2];
mons_speed [k] = 7;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 36: // Kobold
mons_HD [k] = 1;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 2;//gmon_AC [2];
mons_ev [k] = 14;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 37: // Lich
mons_HD [k] = 20;
mons_hp [k] = (random3((mons_HD [k]) * 4) + (mons_HD [k]) * 2);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 13;//gmon_AC [2];
mons_ev [k] = 15;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
if (mons_sec [k] == 250) // I don't see why it wouldn't be
{
 mons_sec [k] = random3(4) + 20;
}
break;

case 38: // Mummy
mons_HD [k] = 3;
mons_hp [k] = (random3((mons_HD [k]) * 2) + (mons_HD [k]) * 6);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 3;//gmon_AC [2];
mons_ev [k] = 6;//gmon_ev [2];
mons_speed [k] = 6;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 39: // Naga
mons_HD [k] = 6;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 6;//gmon_AC [2];
mons_ev [k] = 14;//gmon_ev [2];
mons_speed [k] = 15;
mons_speed_inc [k] = 7;
mons_sec [k] = 10;
//gmon_use [mons_class [k]] = 0;
break;

case 40: // Ogre
mons_HD [k] = 4;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmon_AC [2];
mons_ev [k] = 6;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 41: // Plant
mons_HD [k] = 10;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 10;//gmon_AC [2];
mons_ev [k] = 0;//gmon_ev [2];
mons_speed [k] = 0;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 42: // Q something
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 10;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2]; // Look!!!
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 43: // Raksasha
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 6;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
mons_sec [k] = 54;
//gmon_use [mons_class [k]] = 0;
break;

case 44: // Snake
mons_HD [k] = 2;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmon_AC [2];
mons_ev [k] = 15;//gmon_ev [2]; // Look!!!
mons_speed [k] = 13;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 45: // Troll
mons_HD [k] = 5;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 3;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 46: // Unseen horror
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 5;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2]; // Look!!!
mons_speed [k] = 30;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 47: // Vampire
mons_HD [k] = 6;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 10;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
mons_sec [k] = 40;
//gmon_use [mons_class [k]] = 0;
break;

case 48: // Wraith
mons_HD [k] = 6;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 10;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 49: // Abomination
mons_HD [k] = 12;
mons_hp [k] = (random3((mons_HD [k]) * 4) + (mons_HD [k]) * 4);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 15;//gmon_AC [2];
mons_ev [k] = 5;//gmon_ev [2];
mons_speed [k] = 6;
mons_speed_inc [k] = 7;
if (mons_sec [k] == 250) mons_sec [k] = random3(15) + 1;
//gmon_use [mons_class [k]] = 0;
break;

case 50: // Yak
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 4;//gmon_AC [2];
mons_ev [k] = 7;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 51: // Large Zombie
mons_HD [k] = random3(5) + 3;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 8;//gmon_AC [2];
mons_ev [k] = 5;//gmon_ev [2];
mons_speed [k] = 5;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 52: // orc warrior
mons_HD [k] = 4;
mons_hp [k] = (random3((mons_HD [k]) * 8) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 0;//gmon_AC [2];
mons_ev [k] = 13;//gmon_ev [2];
mons_speed [k] = 12;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 53: // kobold demonologist
mons_HD [k] = 3;
mons_hp [k] = (random3((mons_HD [k]) * 6) + (mons_HD [k]) * 4);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 2;//gmon_AC [2];
mons_ev [k] = 14;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 54: // orc wizard
mons_HD [k] = 4;
mons_hp [k] = (random3((mons_HD [k]) * 6) + (mons_HD [k]) * 2);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmons_AC [k] [2];
mons_ev [k] = 12;//gmons_ev [k] [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
if (mons_sec [k] == 250) // I don't see why it wouldn't be
{
        mons_sec [k] = random3(3);
}
break;

case 55: // orc knight
mons_HD [k] = 9;
mons_hp [k] = (random3((mons_HD [k]) * 8) + (mons_HD [k]) * 2);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 2;
mons_ev [k] = 13;
mons_speed [k] = 12;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 3;
break;

case 56: // tunneling worm tail
mons_HD [k] = 10;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 5);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 3;//gmon_AC [2];                  // Look at this one!!!
mons_ev [k] = 3;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 57: // Wyvern (D)
mons_HD [k] = 4;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 5;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 15;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 58: // Kobold lord
mons_HD [k] = 5;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 3;//gmon_AC [2];
mons_ev [k] = 14;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 59: // giant eyeball
mons_HD [k] = 3;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 8);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 0;//gmon_AC [2];
mons_ev [k] = 1;//gmon_ev [2];
mons_speed [k] = 3;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 60: // Wight
mons_HD [k] = 3;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 4;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 3;
break;

case 61: // oklob plant
mons_HD [k] = 10;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 10;//gmon_AC [2];
mons_ev [k] = 0;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 62: // wolf spider
mons_HD [k] = 8;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 3;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 15;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 63: // shadow
mons_HD [k] = 3;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 12;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 64: // hungry ghost (p)
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 0;//gmon_AC [2];
mons_ev [k] = 17;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 65: // eye of draining
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 8);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 3;
mons_ev [k] = 1;
mons_speed [k] = 5;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 66: // butterfly
mons_HD [k] = 1;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 0;
mons_ev [k] = 25;
mons_speed [k] = 25;
mons_speed_inc [k] = 7;
if (mons_sec [k] == 250) mons_sec [k] = random3(15) + 1;
//gmon_use [mons_class [k]] = 0;
break;

case 67: // wandering mushroom
mons_HD [k] = 8;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 5;
mons_ev [k] = 0;
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 68: // Efreet
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 10;
mons_ev [k] = 5;
mons_speed [k] = 10; // was higher
mons_speed_inc [k] = 7;
mons_sec [k] = 50;
//gmon_use [mons_class [k]] = 1;
break;

case 69: // brain worm
mons_HD [k] = 5;
mons_hp [k] = (random3((mons_HD [k]) * 2) + (mons_HD [k]) * 2);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmon_AC [2];
mons_ev [k] = 5;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
mons_sec [k] = 52;
//gmon_use [mons_class [k]] = 0;
break;

case 70: // giant orange brain
mons_HD [k] = 10;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 2;//gmon_AC [2];
mons_ev [k] = 4;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
mons_sec [k] = 53;
//gmon_use [mons_class [k]] = 0;
break;

case 71: // Boulder Beetle
mons_HD [k] = 9;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 20;//gmon_AC [2];
mons_ev [k] = 2;//gmon_ev [2];
mons_speed [k] = 3;
mons_speed_inc [k] = 7;
break;

case 72: // flying skull
mons_HD [k] = 4;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 10;//gmon_AC [2];
mons_ev [k] = 20;//gmon_ev [2];
mons_speed [k] = 15;
mons_speed_inc [k] = 7;
break;

case 73: // hell hound
mons_HD [k] = 5;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 6;//gmon_AC [2];
mons_ev [k] = 13;//gmon_ev [2];
mons_speed [k] = 15;
mons_speed_inc [k] = 7;
break;

case 74: // minotaur
mons_HD [k] = 10;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 5;//gmon_AC [2];
mons_ev [k] = 7;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
break;

case 75: // ice Dragon
mons_HD [k] = 10;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 5);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 10;//gmon_AC [2];
mons_ev [k] = 8;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
break;

case 76: // Slime thing
mons_HD [k] = 11;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmon_AC [2];
mons_ev [k] = 4;//gmon_ev [2];
mons_speed [k] = 7;
mons_speed_inc [k] = 7;
break;

case 77: // Blue Wraith
mons_HD [k] = 8;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 12;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 8;
mons_speed_inc [k] = 7;
break;

case 78: // fake rakshasa
mons_HD [k] = 1;
mons_hp [k] = 1; //(random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 0;//gmon_AC [2];
mons_ev [k] = 30;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
break;

case 79: // GooE
mons_HD [k] = 12;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 10;//gmon_AC [2];
mons_ev [k] = 3;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
mons_sec [k] = 55;
break;

case 80: // burning devil
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 5;
mons_ev [k] = 10;
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
mons_sec [k] = RED;
//gmon_use [mons_class [k]] = 1;
break;

case 81: // rotting devil
mons_HD [k] = 5;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 2;
mons_ev [k] = 10;
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 82: // yellow devil
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 12;
mons_ev [k] = 12;
mons_speed [k] = 13;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 83: // skeletal devil
mons_HD [k] = 8;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 15;
mons_ev [k] = 10;
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 84: // shadowy devil
mons_HD [k] = 11;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 18;
mons_ev [k] = 10;
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 85: // hairy devil
mons_HD [k] = 6;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 7;
mons_ev [k] = 10;
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 86: // ice devil
mons_HD [k] = 11;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 12;
mons_ev [k] = 10;
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 87: // blue devil
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 14;
mons_ev [k] = 10;
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 1;
break;

case 100: // giant mite
mons_HD [k] = 2;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmon_AC [2];
mons_ev [k] = 7;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
break;

case 101: // steam Dragon
mons_HD [k] = 4;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 5);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 5;//gmon_AC [2];
mons_ev [k] = 10;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 0;
mons_sec [k] = 57;
break;

case 102: // very ugly thing
mons_HD [k] = 12;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 4;//gmon_AC [2]; //               Look at this one!
mons_ev [k] = 8;//gmon_ev [2];
mons_speed [k] = 8;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 3;
break;

case 103: // orc sorceror
mons_HD [k] = 8;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 5;//gmons_AC [k] [2];
mons_ev [k] = 12;//gmons_ev [k] [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
if (mons_sec [k] == 250) // I don't see why it wouldn't be
{
        mons_sec [k] = 56;
}
break;

case 104: // hippogriff
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 2;//gmon_AC [2]; //               Look at this one!
mons_ev [k] = 7;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 3;
break;

case 105: // griffon
mons_HD [k] = 12;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 4;//gmon_AC [2]; //               Look at this one!
mons_ev [k] = 6;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
//gmon_use [mons_class [k]] = 3;
break;

case 106: // hydra
mons_HD [k] = 13;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 0;//gmon_AC [2]; //               Look at this one!
mons_ev [k] = 5;//gmon_ev [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
mons_sec [k] = 4 + random3(5);
//gmon_use [mons_class [k]] = 3;
break;

case MLAVA0: // lava worm
mons_HD [k] = 6;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmons_AC [k] [2];
mons_ev [k] = 10;//gmons_ev [k] [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
break;

case MLAVA1: // lava fish
mons_HD [k] = 4;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 4;//gmons_AC [k] [2];
mons_ev [k] = 15;//gmons_ev [k] [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
break;

case MLAVA2: // lava snake
mons_HD [k] = 3;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 2;//gmons_AC [k] [2];
mons_ev [k] = 17;//gmons_ev [k] [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
break;



case MWATER0: // giant fish
mons_HD [k] = 4;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmons_AC [k] [2];
mons_ev [k] = 12;//gmons_ev [k] [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
break;

case MWATER1: // giant goldfish
mons_HD [k] = 7;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 5;//gmons_AC [k] [2];
mons_ev [k] = 7;//gmons_ev [k] [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
break;

case MWATER2: // electronic eel
mons_HD [k] = 3;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 1;//gmons_AC [k] [2];
mons_ev [k] = 15;//gmons_ev [k] [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
break;

case MWATER3: // jellyfish
mons_HD [k] = 4;
mons_hp [k] = (random3((mons_HD [k]) * 5) + (mons_HD [k]) * 3);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 0;//gmons_AC [k] [2];
mons_ev [k] = 5;//gmons_ev [k] [2];
mons_speed [k] = 10;
mons_speed_inc [k] = 7;
break;






/*
default:


//mons_class [k] = random3(33);
//mons_class [k] += 100;
//mons_x [bi] = 11;
//mons_y [bi] = 23;
//grid [x] [y] = 7;
mons_speed [k] = 10 + 100;
mons_speed_inc [k] = 10 + 100;
mons_hp [k] = 30 + 100;
mons_HD [k] = 10 + 100;
*/


} // end of switch


/*
if (mons_hp [k] > 90)
{
mons_hp [k] = 90;
mons_hp_max [k] = 90;
}
*/

//mons_class [k] += 100;
/*
mons_HD [k] += 100;
mons_hp [k] += 100;
mons_hp_max [k] += 100;
mons_AC [k] += 100;
mons_ev [k] += 100;
mons_speed [k] += 100;
mons_speed_inc [k] = 70;
mons_speed_inc [k] += 100 + random3(9);
*/

mons_speed_inc [k] *= 10;
mons_speed_inc [k] += random3(10);


} // end of define_monster()








void make_room(unsigned char grid [80] [70])
{

int find_door = 0;
int diag_door = 0;

for (bcount_x = room_x1; bcount_x < room_x2; bcount_x ++)
{
        if ((grid [bcount_x] [room_y1 - 1] > 16 | grid [bcount_x] [room_y1 - 1] == 3) && grid [bcount_x] [room_y1 - 1] < 100) find_door ++;
        if ((grid [bcount_x] [room_y2] > 16 | grid [bcount_x] [room_y2] == 3) && grid [bcount_x] [room_y2] < 100) find_door ++;
}

for (bcount_y = room_y1; bcount_y < room_y2; bcount_y ++)
{
        if ((grid [room_x1 - 1] [bcount_y] > 16 | grid [room_x1 - 1] [bcount_y] == 3) && grid [room_x1 - 1] [bcount_y] < 100) find_door ++;
        if ((grid [room_x2] [bcount_y] > 16 | grid [room_x2] [bcount_y] == 3) && grid [room_x2] [bcount_y] < 100) find_door ++;
}

if (grid [room_x1 - 1] [room_y1 - 1] > 16 | grid [room_x1 - 1] [room_y1 - 1] == 3) diag_door ++;
if (grid [room_x1 - 1] [room_y2] > 16 | grid [room_x1 - 1] [room_y2] == 3) diag_door ++;
if (grid [room_x2] [room_y1 - 1] > 16 | grid [room_x2] [room_y1 - 1] == 3) diag_door ++;
if (grid [room_x2] [room_y2] > 16 | grid [room_x2] [room_y2] == 3) diag_door ++;


if (diag_door + find_door > 1 && max_doors == 1)
{
bk--;
time_run ++;
return;
}


if (find_door == 0 | find_door > max_doors)
{
bk--;
time_run ++;
return;
}

for (bcount_x = room_x1; bcount_x < room_x2; bcount_x ++)
{
        for (bcount_y = room_y1; bcount_y < room_y2; bcount_y ++)
        {
                if (grid [bcount_x] [bcount_y] == 105) //               grid [bcount_x] [bcount_y] = 17;
                {
                        bk--;
                        time_run ++;
                        return;
                }
        }
}


for (bcount_x = room_x1; bcount_x < room_x2; bcount_x ++)
{
        for (bcount_y = room_y1; bcount_y < room_y2; bcount_y ++)
        {
                if (grid [bcount_x] [bcount_y] < 18) grid [bcount_x] [bcount_y] = 17;
        }
}


for (bcount_y = room_y1; bcount_y < room_y2; bcount_y ++)
{
        if (grid [room_x1 - 1] [bcount_y] == 17 && grid [room_x1 - 1] [bcount_y - 1] <= 16 && grid [room_x1 - 1] [bcount_y + 1] <= 16 && bcount_y > room_y1 && bcount_y < room_y2)
        {
                if (random3(10) < doorlevel) grid [room_x1 - 1] [bcount_y] = 3;
        }

        if (grid [room_x2] [bcount_y] == 17 && grid [room_x2] [bcount_y - 1] <= 16 && grid [room_x2] [bcount_y + 1] <= 16 && bcount_y > room_y1 && bcount_y < room_y2)
        {
                if (random3(10) < doorlevel) grid [room_x2] [bcount_y] = 3;
        }
}


for (bcount_x = room_x1; bcount_x < room_x2; bcount_x ++)
{
        if (grid [bcount_x] [room_y1 - 1] == 17 && grid [bcount_x - 1] [room_y1 - 1] <= 16 && grid [bcount_x + 1] [room_y1 - 1] <= 16 && bcount_x > room_x1 && bcount_x < room_x2)
        {
                if (random3(10) < doorlevel) grid [bcount_x] [room_y1 - 1] = 3;
        }

        if (grid [bcount_x] [room_y2] == 17 && grid [bcount_x - 1] [room_y2] <= 16 && grid [bcount_x + 1] [room_y2] <= 16 && bcount_x > room_x1 && bcount_x < room_x2)
        {
                if (random3(10) < doorlevel) grid [bcount_x] [room_y2] = 3;
        }
}




time_run++;



} //end of void make_room()


/*
void screen_show(void)
{

clrscr();

for (bl = 0; l < 3; l++)
{


for (bcount_x = 1; bcount_x < 80; bcount_x ++)
{
        for (bcount_y = 1 + 25 * l; bcount_y < 25 + 25 * l; bcount_y ++)
        {
                gotoxy(count_x, count_y - 25 * l);
                switch(grid [count_x] [count_y])
                {
                case 1: textcolor(6); putch(177); textcolor(7); break;
                case 2: textcolor(7); putch(177); textcolor(7); break;
                case 3: putch(43); break;
                case 17: textcolor(7); putch(249); textcolor(7); break;
                case 18: putch(60); break;
                case 19: putch(62); break;
//              case 21: putch(60); break;
                //case 22: putch(62); break;
//              case 23: putch(60); break;
//              case 24: putch(62); break;
                case 105: textcolor(8); putch(38); textcolor(7); break;
                default : putch(grid [count_x] [count_y]);
                //cout << (int) grid [count_x] [count_y];
                //getch();
                 break;
                }
                for (i = 0; bi < MNST; bi ++)
                {
                        if (mons_x [bi] == count_x && mons_y [bi] == count_y && mons_class [bi] != 255)
                        {
                        gotoxy(count_x, count_y - 25 * l);
                        putch(42);
                        }
                }

   if (igrid [count_x] [count_y] != 501)
   {
        gotoxy(count_x, count_y - 25 * l);
        putch(254);
         if (grid [count_x] [count_y] == 1)
         {
                 gotoxy(count_x, count_y - 25 * l);
                  textcolor(RED);
                 putch(254);
                  textcolor(7);
         }
   }

        if (count_y > 68) break;
        }
}

something = getch();
clrscr();

/*
for (count_x = 79; count_x < 120; count_x ++)
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
                case 18: putch(60); break;
                case 19: putch(62); break;
                case 21: putch(60); break;
                case 22: putch(62); break;
                case 23: putch(60); break;
                case 24: putch(62); break;
                default : putch(grid [count_x] [count_y]); break;
                }
                for (bi = 0; bi < MNST; bi ++)
                {
                        if (mons_x [i] - 100 == count_x && mons_y [i] - 100 == count_y)
                        {
                        gotoxy(count_x - 78, count_y - 25 * l);
                        putch(42);
                        }
                }
        }
}

something = getch();
clrscr();

* /
} // end of for l









} // end of screen_show

*/









void big_room(unsigned char grid [80] [70])
{

if (random3 (3) == 0)
{
        oblique = random3(20) + 5;
        oblique_max = oblique;

        room_x1 = random3 (30) + 8;
        room_y1 = random3 (22) + 8;
        room_x2 = room_x1 + random3(10) + 20;
        room_y2 = room_y1 + random3(8) + 20;

        if (random3(5) != 0 && many_many <= 7 + random3(8))
 {
  octa_room(grid, 17);
        return;
 }
 if (random3(many_many) <= 10) octa_room(grid, 12); else octa_room(grid, 11);
 return;
}

room_x1 = random3 (30) + 8;
room_y1 = random3 (22) + 8;
room_x2 = room_x1 + random3(10) + 20;
room_y2 = room_y1 + random3(8) + 20;

for (bi = room_x1; bi < room_x2; bi ++)
{
        for (bj = room_y1; bj < room_y2; bj ++)
        {

                if (grid [bi] [bj] == 105) return;

        } // end of for j

} // end of for i

for (bi = room_x1; bi < room_x2; bi ++)
{
        for (bj = room_y1; bj < room_y2; bj ++)
        {

                if (grid [bi] [bj] == 1 | grid [bi] [bj] == 3) grid [bi] [bj] = 17;

        } // end of for j

} // end of for i



}


void diamond_rooms(unsigned char grid [80] [70])
{

char numb_diam = random3(10) + 1;
char type_floor = 17;
int runthru = 0;

if (many_many >= 8 + random3(5) && random3(2) != 0) type_floor = 12; // water
if (many_many >= 18 && random3(2) == 0) type_floor = 11; // lava
if (random3(15) == 0 && many_many > 10) type_floor = random3(2) + 1;
if (random3(20) == 0 && many_many > 12) type_floor = 4;

if (many_many > 60) type_floor = 11;

for (bk = 0; bk < numb_diam; bk++)
{
        room_x1 = random3 (43) + 8;
        room_y1 = random3 (35) + 8;
        room_x2 = room_x1 + random3(15) + 5;
        room_y2 = room_y1 + random3(10) + 5;

        oblique = (room_x2 - room_x1) / 2; //random3(20) + 5;
        oblique_max = oblique;



for (bi = room_x1; bi < room_x2; bi ++)
{
        for (bj = room_y1 + oblique; bj < room_y2 - oblique; bj ++)
        {

        //if (bj - room_y1 <= oblique && i - room_x1 <= oblique) continue;

        //if (bj - room_y1 + i - room_x1 < oblique) continue;

        //if ((room_y2 - j) * -1 + room_x2 - 1 < oblique) continue;

  if (grid [bi] [bj] == 17) goto draw_it;

        } // end of for j
        if (oblique > 0) oblique--;
        if (bi > room_x2 - oblique_max) oblique += 2;
} // end of for i


 bk--;
 runthru ++;
 if (runthru >= 10)
 {
  runthru = 0;
  bk ++;
 }
 continue;

// bk ++;
        draw_it : octa_room(grid, type_floor);
}


} // end of diamond_rooms()





void octa_room(unsigned char grid [80] [70], unsigned char type_floor)
{


for (bi = room_x1; bi < room_x2; bi ++)
{
        for (bj = room_y1 + oblique; bj < room_y2 - oblique; bj ++)
        {

        //if (bj - room_y1 <= oblique && i - room_x1 <= oblique) continue;

        //if (bj - room_y1 + i - room_x1 < oblique) continue;

        //if ((room_y2 - j) * -1 + room_x2 - 1 < oblique) continue;

                if (grid [bi] [bj] == 105) return;

        } // end of for j

        if (oblique > 0) oblique--;

        if (bi > room_x2 - oblique_max) oblique += 2;

} // end of for i

oblique = oblique_max;


for (bi = room_x1; bi < room_x2; bi ++)
{
        for (bj = room_y1 + oblique; bj < room_y2 - oblique; bj ++)
        {

        //if (bj - room_y1 <= oblique && i - room_x1 <= oblique) continue;

        //if (bj - room_y1 + i - room_x1 < oblique) continue;

        //if ((room_y2 - j) * -1 + room_x2 - 1 < oblique) continue;

                if (grid [bi] [bj] == 1) grid [bi] [bj] = type_floor;
                if (type_floor >= 10 && grid [bi] [bj] == 3) grid [bi] [bj] = 17; // ick
                if (type_floor < 10 && grid [bi] [bj] == 3) grid [bi] [bj] = 1; // ick!


        } // end of for j

        if (oblique > 0) oblique--;

        if (bi > room_x2 - oblique_max) oblique += 2;

} // end of for i






}



































void make_trail(unsigned char grid [80] [70])
{

//return;

//for (bi =
/*
x_start = random3 (78) + 2; y_start = random3 (23) + 2;
*/


//do // first do loop - no longer needed
//{



do // (while finish < no_corr)
{


if (x_ps < 15) dir_x2 = 1; else dir_x2 = 0;
if (x_ps > 65) dir_x2 = -1;
if (y_ps < 15) dir_y2 = 1; else dir_y2 = 0;
if (y_ps > 55) dir_y2 = -1;

rannumber = random3(10);

if (random3 (2) == 0) // Put something in to make it go to parts of map it isn't in now
{
        if (dir_x2 != 0 && rannumber < 6) dir_x = dir_x2;
        if (dir_x2 == 0 | rannumber >= 6)
        {
                switch(random3 (2))
                {
                        case 0: dir_x = -1; break;
                        case 1: dir_x = 1; break;
                }
        }
//      dir_x = random3(3) - 1;
        dir_y = 0;
} else
        {
        if (dir_y2 != 0 && rannumber < 6) dir_y = dir_y2;
        if (dir_y2 == 0 | rannumber >= 6)
        {
                switch(random3 (2))
                {
                        case 0: dir_y = -1; break;
                        case 1: dir_y = 1; break;
                }
        }
//      dir_y = random3(3) - 1;
        dir_x = 0;
        }




if (dir_x == 0 && dir_y == 0) continue;
/*
if (dir_x == 0 - old_x && dir_x != 0) continue;
if (dir_y == 0 - old_y && dir_y != 0) continue;
//if (dir_x == 0 - old_x) continue;
//if (dir_y == 0 - old_y) continue;

if (dir_x == old_x && dir_y == old_y) continue;

if (dir_x == 0 && dir_y == 0 && (x_ps < 1 | y_ps < 1)) continue;


if (dir_x == old_x && dir_y == old_y) continue;
if (dir_x == 0 && dir_y == 0) continue;
*/


if (x_ps < 8)
{
        dir_x = 1; dir_y = 0; //random3(3) - 1;
}

if (y_ps < 8)
{
        dir_y = 1;
        dir_x = 0; //random3(3) - 1;
}

if (x_ps > 72)
{
        dir_x = -1; dir_y = 0;//random3(3) - 1;
}

if (y_ps > 62)
{
        dir_y = -1; dir_x = 0;// random3(3) - 1;
}


// This bit is a near copy of the above.

/*
if (x_ps < 8)
{
        dir_x = 1; dir_y = random3(3) - 1;
}

if (y_ps < 8)
{
        dir_y = 1;
        dir_x = random3(3) - 1;
}

if (x_ps > 72)
{
        dir_x = -1; dir_y = random3(3) - 1;
}

if (y_ps > 62)
{
        dir_y = -1; dir_x = random3(3) - 1;
}
*/


if (dir_x == 0) length = random3 (corrlength) + 2; // else length = random3 (corrlength) + 2;

//length = 3;
bi = 0;
for (bi = 0; bi < length; bi ++)
//while (i < length && grid [x_ps + dir_x] [y_ps + dir_y] != 105)
{

        //if (dir_y == 0) x_ps += dir_x; else y_ps +;

/*
        if (dir_y == 0 && (x_ps <= 4 | x_ps >= 76))
        {
        //x_start = x_ps; y_start = y_ps;
        //grid [x_ps] [y_ps] = 2;
        dir_y = 0; //if (random3 (2) == 0) dir_x = -1; else dir_x = 1;
        dir_x = 0 - dir_x;
//      continue;
        }

        if (dir_x == 0 && (y_ps <= 4 | y_ps >= 22))
        {
        //x_start = x_ps; y_start = y_ps;
        //grid [x_ps] [y_ps] = 2;
        dir_x = 0; bif (random3 (2) == 0) dir_y = -1; else dir_y = 1;
//      continue;
        dir_y = 0-dir_y;
        }
*/

//      Below, I've changed the values of the unimportant variable from 0
//      to random3(3) - 1 to avoid getting stuck on the "stuck!!" bit.

        if (x_ps < 9)
        {
        dir_y = 0; //random3(3) - 1;
        dir_x = 1;
        }

        if (x_ps > 71)
        {
        dir_y = 0; //random3(3) - 1;
        dir_x = -1;
        }

        if (y_ps < 9)
        {
        dir_y = 1;
        dir_x = 0; //random3(3) - 1;
        }

        if (y_ps > 61)
        {
        dir_y = -1;
        dir_x = 0; //random3(3) - 1;
        }

        // This bit is a near copy of the above:


        /*if (x_ps < 9)
        {
        dir_y = random3(3) - 1;
        dir_x = 1;
        }

        if (x_ps > 71)
        {
        dir_y = random3(3) - 1;
        dir_x = -1;
        }

        if (y_ps < 9)
        {
        dir_y = 1;
        dir_x = random3(3) - 1;
        }

        if (y_ps > 61)
        {
        dir_y = -1;
        dir_x = random3(3) - 1;
        }*/

        if (grid [x_ps + dir_x] [y_ps + dir_y] == 105) break;


//      if (grid [x_ps + 2 * dir_x] [y_ps + 2 * dir_y] == 17 | random3 (4) == 0) break;

// Shouldn't this be:
        if (grid [x_ps + 2 * dir_x] [y_ps + 2 * dir_y] == 17 && random3 (intersect_chance) != 0) break;




















        x_ps += dir_x;
        y_ps += dir_y;

        if (grid [x_ps] [y_ps] == 1) grid [x_ps] [y_ps] = 17;



//x_start = x_ps; y_start = y_ps;

//grid [x_ps] [y_ps] = 17;

//i++;
} // end of (while) i loop



if (finish == no_corr - 1 && grid [x_ps] [y_ps] != 17)
{
//      cout << "stuck!!";

        /*if (random3 (30) == 0)
        {
                do
                {
                        x_ps = random3 (60) + 10;
                        y_ps = random3 (50) + 10;
                } while (grid [x_ps] [y_ps] != 1);
        }*/

        finish -= 2;

//continue;

}






finish++;

//gotoxy(finish,1); putch(dir_x + 98);
//gotoxy(finish,2); putch(dir_y + 98);

//old_x = dir_x;
//old_y = dir_y;


//} // end of for i loop

} while (finish < no_corr); // end of do while finish loop


finish = 0;
//if (((unsigned int) x_ps - x_start) + ((unsigned int) y_ps - y_start) < 30)
//{
//      finish = 34;
//}


//} while (finish < 67);



} // end of void make_trail(void)







int items(int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
unsigned char grid [80] [70],
int force_class,
int force_type,
int force_place,
int many_many // remember, this is the level of the item, which can be
// different from global many_many eg in treasure vaults.
)
{

int quant;
char brk = 0;
//int p;
//unsigned int gold_q;
unsigned long gold_q;
int secchar;
int pot = 0;
int bkk = 0;
int fpass [10];
// used in books:
  char strungy [9];
  int icky = 0;
  int numbo = 0;
  int multip = 0;
  int xj = 0;
  int bp;

  int no_unique = 0;

//no_it = random3(100);

//                      no_it = 350;

//no_it = 0;


//for (bp = 0; bp < no_it; bp ++)
for (bp = 0; bp < 400; bp ++)
{
//      if (item_class [bp] != 100) continue;
   if (item_class [bp] == 100 | item_quant [bp] == 0) break;
   if (bp == 380) return 501;
}
//cprintf("B");
        item_class [bp] = random3(12);// + 2;
 item_ident [bp] = 0;
 item_dam [bp] = 0;
 item_plus [bp] = 0;
 item_type [bp] = 0;


        if (((item_class [bp] == 3 | item_class [bp] == 7) && random3(5) < 2) | random3(10) == 0) item_class [bp] = 4;

//      item_class [bp] = 11;

//      if (random3(5) == 0) item_class [bp] = 6;
 if ((item_class [bp] == 10 | item_class [bp] == 11) && random3(4) != 0) item_class [bp] = 6;
 if ((item_class [bp] == 3 | item_class [bp] == 7) && random3(3) == 0) item_class [bp] = 1; // missile
 if ((item_class [bp] == 3 | item_class [bp] == 7) && random3(3) == 0) item_class [bp] = 5; // missile

//      item_class [bp] = 10;

//      item_class [bp] = random3 (3) + 3;

//item_class [bp] = 11;

//              if (random3 (4) == 0) item_class [bp] = 0;
//              if (random3 (4) == 0) item_class [bp] = 1;

//              item_class [bp] = 0;

if (force_class != 250) item_class [bp] = force_class;

        switch(item_class [bp])
        {
                case 0: item_type [bp] = random3(19); // weapons
  if (random3(200) < 20 - many_many) item_type [bp] = 18;
  if (random3(200) < 20 - many_many) item_type [bp] = 13;
  if (random3(200) < 20 - many_many) item_type [bp] = 11;
  if (random3(200) < 20 - many_many) item_type [bp] = 9;
  if (random3(200) < 20 - many_many) item_type [bp] = 3;
  if (random3(200) < 20 - many_many) item_type [bp] = 1;
  if (random3(200) < 20 - many_many) item_type [bp] = 3;
  if (random3(200) < 20 - many_many) item_type [bp] = 0;

if (force_class == 250)
  {
   if (random3(4000) <= many_many + 100)
   {
   no_unique = random3(5);
   if (unique_items [no_unique] != 0) goto out_of_uniques;
   item_plus [bp] = 50;
   switch(no_unique)
   {
    case 0: item_type [bp] = 6; item_dam [bp] = 181; item_plus [bp] += 7; break;
    case 1: item_type [bp] = 10; item_dam [bp] = 182; item_plus [bp] += 5; break;
    case 2: item_type [bp] = 19; item_dam [bp] = 183; item_plus [bp] += 11; break;
    case 3: item_type [bp] = 1; item_dam [bp] = 184; item_plus [bp] += random3(8); break;
    case 4: item_type [bp] = 17; item_dam [bp] = 185; item_plus [bp] += 8; break;
   }
   quant = 1;
   unique_items [no_unique] = 1;
   break;
  }
  }

  out_of_uniques : if (force_type != 250) item_type [bp] = force_type;
                quant = 1;
                /*if ((many_many < 4 | random3(4) == 0) && random3(3) != 0) // ie these weapons are more common on any level.
                {
//                      item_type [bp] = random3(5);
                if (random3 (10) > 6) item_plus [bp] = random3 (3);
      if (random3 (4) == 0) item_plus [bp] = random3 (3) + 97;

                   item_plus [bp] += 50;
                        switch(random3(6))
                        {
                                case 0: item_type [bp] = 0; break;
                                case 1: item_type [bp] = 1; break;
                                case 2: item_type [bp] = 3; break;
                                case 3: item_type [bp] = 4; break;
                                case 4: item_type [p] = 5; break;
                                case 5: item_type [p] = 14; break;
                        }
                break;
                }*/
/*              if (many_many >= 4 && many_many < 7)
                {
                        switch(random3(6))
                        {
                                case 0: item_type [bp] = 0; break;
                                case 1: item_type [p] = 1; break;
                                case 2: item_type [p] = 3; break;
                                case 3: item_type [p] = 4; break;
                                case 4: item_type [p] = 5; break;
                                case 5: item_type [p] = 14; break;
                        }
                }*/
                /*if (random3 (10) > 7) item_plus [p] = random3 (10);
                if (item_plus [p] < 4) item_plus [p] += 100;
                item_plus [p] -= 4;
                item_plus [p] += 50;*/

      item_plus [bp] = 50;

//      item_type [bp] = random3 (16);

      if (random3(100) <= 10 + (many_many) && item_type [bp] != 0 && item_type [bp] != 19 && item_type [bp] != 20 && item_type [bp] != 21) // nobody would bother enchanting a club
      {
         item_plus [bp] += random3(3) + 1;
         if (random3(100) <= 20 + many_many) item_plus [bp] += random3(3);
         if (random3(100) <= 10 + many_many && (item_type [bp] < 13 | item_type [bp] > 16))
         {
           item_dam [bp] = random3(9);
//           if (item_type [bp] == 0 | item_type [bp] >= 13) item_type [bp] = 6;
           if (item_dam [bp] == 4) item_plus [bp] = random3(5) + random3(5) + 1;
         }
      } else
            {
               if (random3(8) == 0)
               {
                  item_plus [bp] = 149;
                  if (random3(3) == 0) item_plus [bp] -= random3(3);
               }
            }

//  item_dam [bp] = random3(9);
  if ((item_dam [bp] != 0 | (item_plus [bp] != 50 && random3(3) == 0)) && item_type [bp] != 0) item_dam [bp] += random3(6) * 30;
                //if (many_many >= 4) // && many_many < 7)
//  cprintf("A");
                break;

                case 1: // missiles
  item_plus [bp] = 0;
                item_type [bp] = random3(4);
  if (force_type != 250) item_type [bp] = force_type;
                quant = random3(7) + random3 (6) + 1;// + random3 (10) + 1;
                if (random3 (10) > 7) item_plus [bp] = random3 (5);
                item_plus [bp] += 50;
                break;

                case 2: //item_type [bp] = random3(12); //      armour
                quant = 1;
/*              if (many_many < 4 | random3(4) == 0) // ie these armours are more common on any level.
                {
                        if (random3 (10) > 6) item_plus [p] = random3 (3);
                        if (random3 (5) == 0) item_plus [p] = 99 - random3(3);
                        item_plus [p] += 50;
                        switch(random3(7))
                        {
                                case 0: item_type [p] = 0; break;
                                case 1: item_type [p] = 1; break;
                                case 2: item_type [p] = 2; break;
                                case 3: item_type [p] = 3; break;
                                case 4: item_type [p] = 9; break;
                                case 5: item_type [p] = 12; break;
                                case 6: item_type [p] = 8; break;
                        }
                        break;        3,5,8
                }*/
//              if (many_many >= 4 && many_many < 7)
                        item_type [bp] = random3 (3);
   item_plus [bp] = 0;
   if (random3(35) <= many_many + 10) item_type [bp] = random3(5);
   if (random3(60) <= many_many + 10) item_type [bp] = random3(8);
   if (random3(5) == 0) item_type [bp] = 8 + random3(7);

   if (force_type != 250) item_type [bp] = force_type;
//  if (10 + many_many >= random3(


                {
                        if (10 + many_many >= random3(70))
   {
                         item_plus [bp] = random3 (3);
    if (10 + many_many >= random3(40)) item_plus [bp] += random3(3);
 if (10 + many_many >= random3(200) && force_class == 250) // to avoid tricky things w/ monsters getting armours of resistance etc, which I'll do eventually but not now.
   switch(item_type [bp])
                        {
                                case 8: // shield - must do special things for this!
                                break;
                                case 9: // cloak
                                switch(random3(3))
                                {
                                        case 0: item_dam [bp] = 12; break;
                                        case 1: item_dam [bp] = 4; break;
                                 case 2: item_dam [bp] = 6; break;
                                }
                                break;
                                case 10: // helmet
                                switch(random3(3))
                                {
                                 case 0: item_dam [bp] = 5; break;
                                 case 1: item_dam [bp] = 6; break;
                                 case 2: item_dam [bp] = 9; break;
                                }
                                break;
                                case 11: // gloves
                                switch(random3(2))
                                {
                                 case 0: item_dam [bp] = 7; break;
                                 case 1: item_dam [bp] = 8; break;
                                }
                                break;
                                case 12: // boots
                                switch(random3(2))
                                {
                                 case 0: item_dam [bp] = 1; break;
                                 case 1: item_dam [bp] = 11; break;
/*                               case 2: item_dam [bp] = 10;
                                 item_plus [bp] += 1 + random3(4);
                                 break;*/
                                }
                                case 0: // robe:
                                switch(random3(2))
                                {
                                        case 0: item_dam [bp] = random3(2) + 2; break;
                                 case 1: item_dam [bp] = 12; break;
                                }
                                break;

                                default: // body armours:
                                item_dam [bp] = 2 + random3(2);
                                if (item_type [bp] == 7 && random3(15) == 0)
                                {
                                 item_dam [bp] = 10;
                                 item_plus [bp] += 1 + random3(4);
                                }
                                break;
                        }


   } else
                        if (random3 (5) == 0)
   {
     item_plus [bp] = 99 - random3(2);
     if (random3(5) == 0) item_plus [bp] -= random3(3);
   }
                        //item_plus [bp] += 50;
//                      if (item_type [bp] == 6 && random3(3) == 0) item_type [bp] = 7;
         // ^^^ no idea what this does - ???
                }

  if (item_dam [bp] != 0 | (item_plus [bp] != 0 && random3(3) == 0)) item_dam [bp] += random3(6) * 30;

                item_plus [bp] += 50;

  break;

                case 3: // item_type [bp] = random3(9); //      wands
/*              if (many_many < 4 | random3(3) == 0) // ie these wands are more common on any level.
                {
                        item_type [bp] = random3(7);
                        if (item_type [p] < 2) item_plus [p] = random3 (10) + random3 (10);
                        if (item_type [p] >= 2 && item_type [p] < 7) item_plus [p] = random3 (5) + random3 (5);
                        // break;
                }        */
//              if (many_many >= 4 && many_many < 7)
//              {
                        item_type [bp] = random3 (16);
                        if (item_type [bp] < 2) item_plus [bp] = random3 (10) + random3(10) + random3(10);
                        if (item_type [bp] >= 2) item_plus [bp] = random3 (5) + random3 (5) + random3 (5);
//              }
                quant = 1;
                break;



                case 4:
                item_type [bp] = random3 (5); // food
      if (random3(2) == 0) item_type [bp] = random3(2);
      if (random3(16) == 0) item_type [bp] = random3(21);
  if (force_type != 250) item_type [bp] = force_type;
                quant = 1;
                if (random3(4) == 0) quant = random3(3) + 1;
  if (item_type [bp] == 12 | item_type [bp] == 15 | item_type [bp] == 16)
      quant = random3(15) + 3;
                break;

                case 5: item_class [bp] = 8; // potions // This one not finished
                //item_type [bp] = random3 (4);
      //item_type [bp] = random3 (10);
      pot = 0;

      pot = random3(8); // general type of potion;

      switch(pot)
      {
         case 0:
         case 1:
         case 2:
         // healing potions
         if (random3(3) == 0) item_type [bp] = 1; // wound heal
            else item_type [bp] = 0; // healing
         break;

         case 3:
         case 4:
         // enhancements
         if (random3(2) == 0) item_type [bp] = 2; // speed
            else item_type [bp] = 3; // might

         if (random3(5) == 0) item_type [bp] = 12; // invis
         if (random3(6) == 0) item_type [bp] = 7; // lev - an enhancement?

         if (random3(50) == 0) item_type [bp] = 13; // oatmeal
         break;

         case 5:
         // gain ability
         item_type [bp] = 4 + random3(3);// str/dex/int
         if (random3(2) == 0) item_type [bp] = 0; // just healing
         break;

         case 6:
         case 7:
         // bad things
         switch(random3(5))
         {
            case 4:
            case 0:
            if (many_many > 1)
            {
                                                                                                                        item_type [bp] = 8; // poison
            if (many_many > 10 && random3(4) == 0) item_type [bp] = 20; //str poison
            break;
            } // otherwise it just falls through
            case 1: item_type [bp] = 9; break; // slow

            case 2: item_type [bp] = 10; break; // para

            case 3: item_type [bp] = 11; break; // confus
         }
         if (random3(8) == 0) item_type [bp] = 14; // disease

         if (random3(1000) == 0) item_type [bp] = 15; // decay! nasty...
         break;

      }

      quant = 1;
      if (force_type != 250) item_type [bp] = force_type;
      if (random3(18) == 0) quant += 1;
      if (random3(25) == 0) quant += 1;
      item_plus [bp] = 0;
      break;

                case 6: // item_class [bp] = 15; // scrolls
//              quant = random3 (30) + random3 (many_many * 10);
                item_type [bp] = random3(18);
                if (random3 (5) == 0) item_type [bp] = 0; // identify should be very common
  if (random3 (5) == 0) item_type [bp] = 4; // so should remove curse
  if (many_many < 4 && item_type [bp] == 1) item_type [bp] = 1;
   // having a level 1 character finding a scroll of immolation would be a bit unfair.
  if (force_type != 250) item_type [bp] = force_type;
                quant = 1;
                if (random3 (16) == 0) quant = random3 (3) + 1;
  if (item_type [bp] == 9 && random3(3) != 0) item_type [bp] = 5;
                //item_plus [bp] = random3(200);
//              item_plus [bp] = dung1;
//              item_dam [bp] = dung2;
  item_plus [bp] = 0;
                break;

                case 7: // rings
                item_class [bp] = 7;
                item_type [bp] = random3(21);
  if (force_type != 250) item_type [bp] = force_type;
//              if (item_type [bp] < 4) item_dam [bp] = random3(13);
//               else item_dam [bp] = random3(13) + (random3(13) * 13);
                quant = 1;
  item_plus [bp] = 50;
                if (item_type [bp] == 1 | item_type [bp] == 5 | item_type [bp] == 11 | item_type [bp] == 14 | item_type [bp] == 15)
                {
                        /*do
                        {
                        item_plus [bp] = random3 (3) + random3 (3) + random3 (3);
                        item_plus [bp] -= 4;
                        item_plus [bp] += 50;
                        } while (item_plus [bp] != 50);*/
        item_plus [bp] = 50;
        item_plus [bp] += random3(3) + 1;
        if (random3(3) == 0) item_plus [bp] += random3(4);

        if (random3(5) == 0)
        {
            item_plus [bp] = 150;
            item_plus [bp] -= random3(2) + 1;
            if (random3(3) == 0) item_plus [bp] -= random3(4);
        }

                }
      switch(item_type [bp])
      {
         case 9:
         case 10:
              item_plus [bp] = 150;
         break;
      }
                break;

  case 10: // books
  item_plus [bp] = 127;
  itoa(127, strungy, 2);
//  item_type [bp] = random3(22); // which book is it?
  if (force_type != 250) item_type [bp] = force_type;

  do
  {
   item_type [bp] = random3(22);
   if (item_type [bp] == 14 && random3(100) < 10 + many_many) break;
   if (item_type [bp] == 17 && random3(100) < 20 + many_many) break;
   if (item_type [bp] == 15 && random3(100) < 20 + many_many) break;
   if (item_type [bp] == 21 && random3(100) < 40 + many_many) break;
   if (item_type [bp] != 14 && item_type [bp] != 17 && item_type [bp] != 15 && item_type [bp] != 21) break;
//   item_type [bp] = random3(22);
  } while (item_type [bp] < 100);

/*
        case 0: strcat(glog , "book of minor magic"); break;
         case 1: strcat(glog , "book of minor magic"); break;
        case 2: strcat(glog , "book of minor magic"); break;
        case 3: strcat(glog , "book of minor magic"); break;
        case 4: strcat(glog , "book of minor magic"); break;
        case 5: strcat(glog , "book of minor magic"); break;
        case 6: strcat(glog , "book of minor magic"); break;
  case 7: strcat(glog , "book of summonings"); break;
 case 8: strcat(glog , "book of fire"); break;
 case 9: strcat(glog , "book of ice"); break;
 case 10: strcat(glog , "book of divinations"); break;
 case 11: strcat(glog , "book of spatial translocations"); break;
 case 12: strcat(glog , "book of enchantments"); break;
 case 13: strcat(glog , "book of poisonings"); break;
 case 14: strcat(glog , "book of greater conjurations"); break;
 case 15: strcat(glog , "book of necromancy"); break;
 case 16: strcat(glog , "book of enchantments"); break;
 case 17: strcat(glog , "book of holy invocations"); break;
 case 18: strcat(glog , "book of bodily restorations"); break;
 case 19: strcat(glog , "book of useful magic"); break;
 case 20: strcat(glog , "book of prayers"); break;
 case 21: strcat(glog , "book of clouds"); break;
*/

  if (random3(3) == 0) strungy [1 + random3(6)] = '0';
  if (random3(3) == 0) strungy [1 + random3(6)] = '0';
  if (random3(3) == 0) strungy [1 + random3(6)] = '0';
  if (random3(3) == 0) strungy [1 + random3(6)] = '0';

      spellbook_template(item_type [bp], fpass);

      for (bkk = 1; bkk < 7; bkk++)
                                                {
                                                        if (fpass [bkk] == 120) strungy [bkk] = '0';
                                                }

      icky = strlen(strungy);

      multip = 1;
      numbo = 0;

      for (xj = icky; xj >= 1; xj --)
      {
         if (strungy [xj] != '1' && strungy [xj] != '0') continue;
         if (strungy [xj] == '0')
         {
            multip *= 2;
            continue;

         }

         if (strungy [xj] == '1')
         {
               numbo += multip;
               multip *= 2;
         }
       }
      strcpy(strungy, "");

item_plus [bp] = numbo + 64;

//  item_dam [bp] = 127; // !!!

  item_dam [bp] = random3(5);
                if (random3(10) == 0) item_dam [bp] += random3(8) * 10;
  quant = 1;
  break;



  case 11: // staves
  item_type [bp] = random3(6);
  if (random3(2) == 0) item_type [bp] = 10 + random3(8);
  if (item_type [bp] >= 12 && item_type [bp] <= 15 && random3(3) == 0) item_type [bp] = random3(6);
  item_dam [bp] = random3(9);
if (item_type [bp] >= 10)
{
  item_plus [bp] = 127;
  if (force_type != 250) item_type [bp] = force_type;

  itoa(127, strungy, 2);
  if (force_type != 250) item_type [bp] = force_type;
/*  if (random3(3) == 0) strungy [1 + random3(6)] = '0';
  if (random3(3) == 0) strungy [1 + random3(6)] = '0';
  if (random3(3) == 0) strungy [1 + random3(6)] = '0';
  if (random3(3) == 0) strungy [1 + random3(6)] = '0';*/

      spellbook_template(item_type [bp] + 40, fpass);

      for (bkk = 1; bkk < 7; bkk++)
                                                {
                                                        if (fpass [bkk] == 120) strungy [bkk] = '0';
                                                }

      icky = strlen(strungy);

      multip = 1;
      numbo = 0;

      for (xj = icky; xj >= 1; xj --)
      {
         if (strungy [xj] != '1' && strungy [xj] != '0') continue;
         if (strungy [xj] == '0')
         {
            multip *= 2;
            continue;

         }

         if (strungy [xj] == '1')
         {
               numbo += multip;
               multip *= 2;
         }
       }
      strcpy(strungy, "");

  item_plus [bp] = numbo + 64;
}
  quant = 1;
  break;

  case 12: // Magical Orbs of Power
  quant = 1;
  break;


/*
                case 8: // potions
                item_type [bp] = random3(9);
                quant = 1;
                if (random3(4) == 0) quant = random3(3) + 2;
                break;*/

                default: // money - class = 15
                item_class [bp] = 15;
                quant = (random3(10) + random3(3) + 1) * (many_many + 1);
                break;

        }
//      if (item_class [bp] == 4) item_class [bp] = 6;
//      if (item_class [bp] == 5) item_class [bp] = 8;


        //item_class [bp] = 15;
        //item_type [bp] = random3(7);

        //if (item_class [bp] == 15)
        //{
        //gold_q = random3(30000) + 100000;
        //gold_q = 10;

 item_quant [bp] = quant;

        //      gold_q = quant + 100000;



        //gold_q = 10 + 100000;
//      cout << gold_q << " + ";
                //gold_q = 100000;
        //ltoa(gold_q, item_quant [bp], 10);//remember that the 10 is the base!








//      itoa(gold_q, item_quant [bp], 10);












        // use ltoa because gold_q is a long variable.

//cout << (int) item_class [bp] << ", " << (int) item_type [bp] << " " << (unsigned long) gold_q << " ";
//cprintf(item_quant [bp]);
//cout << "\n";
//if (quant > 40) something = getch();
        //for (bk = 0; bk < 5; bk ++)
        //{

/*      if (gold_q < 10000) item_quant [p] [0] = 48; // ASCII 48 is 0.
        if (gold_q < 1000) item_quant [p] [1] = 48;
        if (gold_q < 100) item_quant [p] [2] = 48;
        if (gold_q < 10) item_quant [p] [3] = 48;
        if (gold_q == 0) item_quant [p] [4] = 48;
*/
        //}
        //itoa(damage, damage_print, 10);

//      cprintf(item_quant [p]);
//      cprintf(" * ");
        //}

        //item_quant [p] = 1;
//      item_plus [p] = random3(5);

        //if (item_class [p] != 15) item_class [p] += 100000;

 char st_prn [5];

        if (force_place == 1)
 {
  item_x [bp] = 1;
                item_y [bp] = 1;
 }
 else do
        {
                item_x [bp] = random3 (80);
                item_y [bp] = random3 (70);
/*  itoa(item_x [bp], st_prn, 10);
  cprintf(st_prn);
  cprintf(",");
  itoa(item_y [bp], st_prn, 10);
  cprintf(st_prn);
  cprintf(",");
  itoa(grid [item_x [bp]] [item_y [bp]], st_prn, 10);
  cprintf(st_prn);
  cprintf(".");


//  cprintf("grid");
  if (getch() == 'q') break;*/
        } while (grid [item_x [bp]] [item_y [bp]] != 17);

// cprintf("colour:");
        item_colour(bp, item_class, unique_items, item_type, item_plus, item_dam, icolour,
        item_description);
// cprintf("finished");







/*
        j = 0;
//      while (igrid [item_x] [item_y] != 501)
        {
                j = item_link

        }
*/



/*
        if (bp < 150)
{

        for (bi = 0; i < MNST; i++)
        {
                if (mons_class [i] == 255 | gmon_use [mons_class [i] - 100] == 0) continue;

                for (bj = 0; j < 2; j ++)
                {
                if (item_class [bp] == j && quant > 0 && mons_inv [i] [bj] == 501)
                {
                        if (random3 (10) > 6)
                        {
                        mons_inv [i] [bj] = p; // + 40;
                        item_x [bp] = mons_x [i] - 100;
                        item_y [bp] = mons_y [i] - 100;
                        brk = 1;
                        //cout << (int) mons_inv [i] [bj] << " ";
                        break;
                        }
                }
                }
        if (brk == 1)
        {
                brk = 0;
                break;
        }
        }

} // end of if p


*/ // end of giving items to monsters - now in func called from monster

        //}// else



        //strcpy(fake_name [bp], all_items [item_class [bp]] [item_type [bp]]);



//      cout << (int) item_class [bp] << " " << (int) item_type [bp] << " ";
//      cprintf(fake_name [p]); cout << "   ";

        //if (strcmp(fake_name [p], all_items [item_class [p]] [item_type [p]]) != 0)
        //{
        //cprintf(fake_name [p]); cprintf(":"); cprintf(all_items [item_class [p]] [item_type [p]]);
        //cprintf("   ");
        //}
        //if (item_class [p] == 1 && item_type [p] == 0) strcpy(fake_name [p], "dart");


//name_item();

/*
if (bp < 0) // impossible - this bit is left out.
{
        switch(item_class [p])
        {

        case 0: // weapons
        icolour [p] = 11;
        if (item_type [p] > 8) icolour [p] = 6;
        switch(item_type [p])
        {
        case 0: strcpy(fake_name [p], "club");
        icolour [p] = 6;
        break;
        case 1: strcpy(fake_name [p], "mace"); break;
        case 2: strcpy(fake_name [p], "flail"); break;
        case 3: strcpy(fake_name [p], "dagger"); break;
        case 4: strcpy(fake_name [p], "morningstar"); break;
        case 5: strcpy(fake_name [p], "short sword"); break;
        case 6: strcpy(fake_name [p], "long sword"); break;
        case 7: strcpy(fake_name [p], "great sword"); break;
        case 8: strcpy(fake_name [p], "scimitar"); break;
        case 9: strcpy(fake_name [p], "hand axe"); break;
        case 10: strcpy(fake_name [p], "battleaxe"); break;
        case 12: strcpy(fake_name [p], "spear"); break;
        case 13: strcpy(fake_name [p], "halberd"); break;
        case 14: strcpy(fake_name [p], "sling"); break;
        case 15: strcpy(fake_name [p], "bow"); break;
        case 16: strcpy(fake_name [p], "crossbow"); break;
        }
        break;

        case 1: // ammunition
        icolour [p] = 11;
        switch(item_type [p])
        {
        case 0: strcpy(fake_name [p], "stone"); break;
        case 1: strcpy(fake_name [p], "arrow"); break;
        case 2: strcpy(fake_name [p], "bolt"); break;
        case 3: strcpy(fake_name [p], "dart"); break;
        case 4: strcpy(fake_name [p], ""); break;
        case 5: strcpy(fake_name [p], ""); break;
        case 6: strcpy(fake_name [p], ""); break;
        case 7: strcpy(fake_name [p], ""); break;
        case 8: strcpy(fake_name [p], ""); break;
        case 9: strcpy(fake_name [p], ""); break;
        case 10: strcpy(fake_name [p], ""); break;
        case 12: strcpy(fake_name [p], ""); break;
        case 13: strcpy(fake_name [p], ""); break;
        case 14: strcpy(fake_name [p], ""); break;
        case 15: strcpy(fake_name [p], ""); break;
        case 16: strcpy(fake_name [p], ""); break;
        } break;


        //break;



        case 2: // armour
        icolour [p] = 11;
        switch(item_type [p])
        {
        case 0: strcpy(fake_name [p], "robe");
        icolour [p] = random3(15) + 1;
        case 1: strcpy(fake_name [p], "leather armour");
        icolour [p] = 6;
        break;
        case 2: strcpy(fake_name [p], "ring mail"); break;
        case 3: strcpy(fake_name [p], "scale mail"); break;
        case 4: strcpy(fake_name [p], "chain mail"); break;
        case 5: strcpy(fake_name [p], "splint mail"); break;
        case 6: strcpy(fake_name [p], "banded mail"); break;
        case 7: strcpy(fake_name [p], "plate mail"); break;
        case 8: strcpy(fake_name [p], "shield"); break;
        case 9: strcpy(fake_name [p], "cloak");
        icolour [p] = random3(15) + 1;
        break;
        case 10: strcpy(fake_name [p], "helmet"); break;
        case 11: strcpy(fake_name [p], "pair of gloves");
        icolour [p] = 6;
        break;
        case 12: strcpy(fake_name [p], "pair of boots");
        icolour [p] = 6;
        break;
        } break;


        case 3: // devices
        //icolour [p] = 12;
        //switch(item_type [p])
        //{
//      case 0: strcpy(fake_name [p], "wand of fire"); break;
//      case 1: strcpy(fake_name [p], "wand of cold"); break;
//      case 2: strcpy(fake_name [p], "wand of slowing"); break;
//      case 3: strcpy(fake_name [p], "wand of magic missile"); break;
//      case 4: strcpy(fake_name [p], "wand of healing"); break;
//      case 5: strcpy(fake_name [p], "wand of paralysis"); break;
//      case 6: strcpy(fake_name [p], ""); break;
//      case 7: strcpy(fake_name [p], ""); break;
        // Common wands: note: fire & cold will be uncommon, and will be replaced by burning and chilling
        // Lightning will always be blackened
        // If the wand is common:
        if (item_type [p] < 7)
        {
        randnum = random3(12);
        switch(randnum)
        {
        case 0: strcpy(fake_name [p], "iron wand");
        icolour [p] = 11; break;
        case 1: strcpy(fake_name [p], "brass wand");
        icolour [p] = 14; break;
        case 2: strcpy(fake_name [p], "bone wand");
        icolour [p] = 7;  break;
        case 3: strcpy(fake_name [p], "wooden wand");
        icolour [p] = 6;  break;
        case 4: strcpy(fake_name [p], "copper wand");
        icolour [p] = 6;  break;
        case 5: strcpy(fake_name [p], "gold wand");
        icolour [p] = 14; break;
        case 6: strcpy(fake_name [p], "silver wand");
        icolour [p] = 7;  break;
        case 7: strcpy(fake_name [p], "bronze wand");
        icolour [p] = 6;  break;
        case 8: strcpy(fake_name [p], "ivory wand");
        icolour [p] = 11; break;
        case 9: strcpy(fake_name [p], "glass wand");
        icolour [p] = 7; break;
        case 10: strcpy(fake_name [p], "lead wand");
        icolour [p] = 7;  break;
        case 11: strcpy(fake_name [p], "pewter wand");
        icolour [p] = 6;  break;
//      case 12: strcpy(fake_name [p], " wand");
//      icolour [p] = 6;  break;
//      case 13: strcpy(fake_name [p], " wand");
//      icolour [p] = 14; break;
//      case 14: strcpy(fake_name [p], " wand");
//      icolour [p] = 7;  break;
//      case 15: strcpy(fake_name [p], " wand");
//      icolour [p] = 7; break;

        } // end of switch(randnum)

        } else //ie if it's uncommon:
        {
        secchar = random3(16);
        switch(secchar) // secondary characteristic of uncommon wand
        {
        case 0: strcpy(fake_name [p], "blackened ");
        icolour [p] = 11; break;
        case 1: strcpy(fake_name [p], "jewelled ");
        icolour [p] = 14; break;
        case 2: strcpy(fake_name [p], "curved ");
        icolour [p] = 7;  break;
        case 3: strcpy(fake_name [p], "long ");
        icolour [p] = 6;  break;
        case 4: strcpy(fake_name [p], "short ");
        icolour [p] = 6;  break;
        case 5: strcpy(fake_name [p], "twisted ");
        icolour [p] = 14; break;
        case 6: strcpy(fake_name [p], "crooked ");
        icolour [p] = 7;  break;
        case 7: strcpy(fake_name [p], "forked ");
        icolour [p] = 7;  break;
        case 8: strcpy(fake_name [p], "shiny ");
        icolour [p] = 11; break;
        case 9: strcpy(fake_name [p], "pitted ");
        icolour [p] = 14; break;
        case 10: strcpy(fake_name [p], "tapering ");
        icolour [p] = 7;  break;
        case 11: strcpy(fake_name [p], "glowing ");
        icolour [p] = 6;  break;
        case 12: strcpy(fake_name [p], "worn ");
        icolour [p] = 6;  break;
        case 13: strcpy(fake_name [p], "encrusted ");
        icolour [p] = 14; break;
        case 14: strcpy(fake_name [p], "runed ");
        icolour [p] = 7;  break;
        case 15: strcpy(fake_name [p], "sharpened ");
        icolour [p] = 7;  break;

        } // end of switch(randnum)


        randnum = random3(10);
        switch(randnum)
        {
        case 0: strcat(fake_name [p], "iron wand");
        icolour [p] = 11; break;
        case 1: strcat(fake_name [p], "brass wand");
        icolour [p] = 14; break;
        case 2: strcat(fake_name [p], "bone wand");
        icolour [p] = 7;  break;
        case 3: strcat(fake_name [p], "wooden wand");
        icolour [p] = 6;  break;
        case 4: strcat(fake_name [p], "copper wand");
        icolour [p] = 6;  break;
        case 5: strcat(fake_name [p], "gold wand");
        icolour [p] = 14; break;
        case 6: strcat(fake_name [p], "silver wand");
        icolour [p] = 7;  break;
        case 7: strcat(fake_name [p], "bronze wand");
        icolour [p] = 6;  break;
        case 8: strcat(fake_name [p], "ivory wand");
        icolour [p] = 7; break;
        case 9: strcat(fake_name [p], "glass wand");
        icolour [p] = 7; break;
        case 10: strcat(fake_name [p], "lead wand");
        icolour [p] = 7;  break;
        case 11: strcat(fake_name [p], "pewter wand");
        icolour [p] = 7;  break;
//      case 12: strcat(fake_name [p], " wand");
//      icolour [p] = 6;  break;
//      case 13: strcat(fake_name [p], " wand");
//      icolour [p] = 14; break;
//      case 14: strcat(fake_name [p], " wand");
//      icolour [p] = 7;  break;
//      case 15: strcat(fake_name [p], " wand");
//      icolour [p] = 7; break;

        } // end of switch(randnum)
        if (secchar == 0) icolour [p] = 8;

        } // end of if uncommon wand
        // rare wands (eg disintegration - these will be very rare): maybe only 1 thing, like: crystal, shining, etc.
        break;


        case 8: // potions
//      icolour [p] = 13;
//      switch(item_type [p])
//      {
//      case 0: strcpy(fake_name [p], "potion of healing"); break;
//      case 1: strcpy(fake_name [p], "potion of extra healing"); break;
//      case 2: strcpy(fake_name [p], "potion of speed"); break;
//      case 3: strcpy(fake_name [p], "potion of might"); break;
//      case 4: strcpy(fake_name [p], "potion of strength"); break;
//      case 5: strcpy(fake_name [p], "potion of dexterity"); break;
//      case 6: strcpy(fake_name [p], "potion of intelligence"); break;
//      case 7: strcpy(fake_name [p], "poison"); break;
//      }

        if (item_type [p] < 4)
        {
        randnum = random3(14);
        switch(randnum)
        {
                case 0: strcpy(fake_name [p], "clear potion");
                icolour [p] = 7; break;
                case 1: strcpy(fake_name [p], "blue potion");
                icolour [p] = 1; break;
                case 2: strcpy(fake_name [p], "black potion");
                icolour [p] = 8;  break;
                case 3: strcpy(fake_name [p], "silvery potion");
                icolour [p] = 7;  break;
                case 4: strcpy(fake_name [p], "cyan potion");
                icolour [p] = 3;  break;
                case 5: strcpy(fake_name [p], "purple potion");
                icolour [p] = 5; break;
                case 6: strcpy(fake_name [p], "orange potion");
                icolour [p] = 12;  break;
                case 7: strcpy(fake_name [p], "inky potion");
                icolour [p] = 1;  break;
                case 8: strcpy(fake_name [p], "red potion");
                icolour [p] = 4; break;
                case 9: strcpy(fake_name [p], "yellow potion");
                icolour [p] = 14; break;
                case 10: strcpy(fake_name [p], "green potion");
                icolour [p] = 2;  break;
                case 11: strcpy(fake_name [p], "brown potion");
                icolour [p] = 6;  break;
                case 12: strcpy(fake_name [p], "pink potion");
                icolour [p] = 13;  break;
                case 13: strcpy(fake_name [p], "white potion");
                icolour [p] = 15; break;
//              case 14: strcpy(fake_name [p], "potion");
//              icolour [p] = 7;  break;
//              case 15: strcpy(fake_name [p], "potion");
//              icolour [p] = 7; break;

        } // end of switch(randnum)

        } else //ie if it's uncommon:
        {
        secchar = random3(13);
        switch(secchar) // secondary characteristic of uncommon wand
        {
                case 0: strcpy(fake_name [p], "bubbling ");
                icolour [p] = 11; break;
                case 1: strcpy(fake_name [p], "lumpy ");
                icolour [p] = 14; break;
                case 2: strcpy(fake_name [p], "fuming ");
                icolour [p] = 7;  break;
                case 3: strcpy(fake_name [p], "smoky ");
                icolour [p] = 6;  break;
                case 4: strcpy(fake_name [p], "fizzy ");
                icolour [p] = 6;  break;
                case 5: strcpy(fake_name [p], "glowing ");
                icolour [p] = 14; break;
                case 6: strcpy(fake_name [p], "sedimented ");
                icolour [p] = 7;  break;
                case 7: strcpy(fake_name [p], "metallic ");
                icolour [p] = 7;  break;
                case 8: strcpy(fake_name [p], "murky ");
                icolour [p] = 11; break;
                case 9: strcpy(fake_name [p], "gluggy ");
                icolour [p] = 14; break;
                case 10: strcpy(fake_name [p], "viscous ");
                icolour [p] = 7;  break;
                case 11: strcpy(fake_name [p], "oily ");
                icolour [p] = 6;  break;
                case 12: strcpy(fake_name [p], "slimy ");
                icolour [p] = 6;  break;
//              case 13: strcpy(fake_name [p], " ");
//              icolour [p] = 14; break;
//              case 14: strcpy(fake_name [p], " ");
//              icolour [p] = 7;  break;
//              case 15: strcpy(fake_name [p], " ");
//              icolour [p] = 7;  break;

        } // end of switch(randnum)


        randnum = random3(14);
        switch(randnum)
        {
                case 0: strcat(fake_name [p], "clear potion");
                icolour [p] = 7; break;
                case 1: strcat(fake_name [p], "blue potion");
                icolour [p] = 1; break;
                case 2: strcat(fake_name [p], "black potion");
                icolour [p] = 8;  break;
                case 3: strcat(fake_name [p], "silvery potion");
                icolour [p] = 7;  break;
                case 4: strcat(fake_name [p], "cyan potion");
                icolour [p] = 3;  break;
                case 5: strcat(fake_name [p], "purple potion");
                icolour [p] = 5; break;
                case 6: strcat(fake_name [p], "orange potion");
                icolour [p] = 12;  break;
                case 7: strcat(fake_name [p], "inky potion");
                icolour [p] = 1;  break;
                case 8: strcat(fake_name [p], "red potion");
                icolour [p] = 4; break;
                case 9: strcat(fake_name [p], "yellow potion");
                icolour [p] = 14; break;
                case 10: strcat(fake_name [p], "green potion");
                icolour [p] = 2;  break;
                case 11: strcat(fake_name [p], "brown potion");
                icolour [p] = 6;  break;
                case 12: strcat(fake_name [p], "pink potion");
                icolour [p] = 13;  break;
                case 13: strcat(fake_name [p], "white potion");
                icolour [p] = 15; break;
//              case 14: strcat(fake_name [p], "potion");
//              icolour [p] = 7;  break;
//              case 15: strcat(fake_name [p], "potion");
//              icolour [p] = 7; break;

        } // end of switch(randnum)
        } // end of else.
        break;

        case 4: // food
        icolour [p] = 6;
        switch(item_type [p])
        {
        case 0: strcpy(fake_name [p], "meat ration"); break;
        case 1: strcpy(fake_name [p], "bread ration"); break;
        case 2: strcpy(fake_name [p], "pear"); break;
        case 3: strcpy(fake_name [p], "apple"); break; // make this less common.
        case 4: strcpy(fake_name [p], "choko"); break;
        case 5: strcpy(fake_name [p], ""); break;
        case 6: strcpy(fake_name [p], ""); break;
        case 7: strcpy(fake_name [p], ""); break;
        } break;

        case 6: // scrolls
        icolour [p] = 7;
        switch(item_type [p])
        {
        case 0: strcpy(fake_name [p], "of identify"); break;
        case 1: strcpy(fake_name [p], "of teleportation"); break;
        case 2: strcpy(fake_name [p], "of fear"); break;
        case 3: strcpy(fake_name [p], "of noise"); break;
        case 4: strcpy(fake_name [p], "of remove curse"); break;
        case 5: strcpy(fake_name [p], "of magic missile"); break;
        case 6: strcpy(fake_name [p], ""); break;
        case 7: strcpy(fake_name [p], ""); break;
        } break;

        case 7: // jewellery
        icolour [p] = 14;
        switch(item_type [p])
        {
        case 0: strcpy(fake_name [p], "ring of regeneration"); break;
        case 1: strcpy(fake_name [p], "ring of protection"); break;
        case 2: strcpy(fake_name [p], ""); break; // make regen rarer.
        case 3: strcpy(fake_name [p], " "); break;
        case 4: strcpy(fake_name [p], " "); break;
        case 5: strcpy(fake_name [p], ""); break;
        case 6: strcpy(fake_name [p], ""); break;
        case 7: strcpy(fake_name [p], ""); break;
        } break;

        }
} // end of if(bp < 0) impossibility bit.

*/


//} // end of for p
// ^ no longer needed


return bp;

} // end of void items()







void special_room(
int igrid [80] [70],
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
unsigned char grid [80] [70])
{

char spec_room_type;

int quant = 1;
long gold_q;
int thing_created = 0;

int glopop = 0;

char depth_max = 1;

depth_max = 3;
char depth = 1;
char depth_c = 0;


char lordx = 0;
char lordy = 0;

/*int x1, x2, y1, y2;
int cx = 0;
int cy = 0;
int sx = 0;
int sy = 0;
int dx = 0;
int dy = 0;
int rd = 0;*/



room_x1 = random3 (55) + 8;
room_y1 = random3 (45) + 8;
room_x2 = room_x1 + random3(4) + 6;
room_y2 = room_y1 + random3(4) + 6;


for (bcount_x = room_x1; bcount_x < room_x2; bcount_x ++)
{
        for (bcount_y = room_y1; bcount_y < room_y2; bcount_y ++)
        {
                grid [bcount_x] [bcount_y] = 105;
        }
}

x1 = room_x1 + 1;
x2 = room_x2 - 1;
y1 = room_y1 + 1;
y2 = room_y2 - 1;


for (bcount_x = x1; bcount_x < x2; bcount_x ++)
{
        for (bcount_y = y1; bcount_y < y2; bcount_y ++)
        {
                grid [bcount_x] [bcount_y] = 117;
        }
}


// placing monsters in special room:

bno_mons = 0;

for (bk = 0; bk < MNST; bk ++)
{
        if (mons_class [bk] != 255) bno_mons++;
}


spec_room_type = random3(4);

if (many_many < 18 && random3(3) != 0) spec_room_type = 3;
if (many_many < 14 && random3(3) != 0) spec_room_type = 0;
if (many_many < 9) spec_room_type = 1;
if (many_many > 15 && spec_room_type == 1) spec_room_type = 0;

switch (spec_room_type)
{

case 0: // orc lair
for (bcount_x = x1; bcount_x < x2; bcount_x ++)
{
        for (bcount_y = y1; bcount_y < y2; bcount_y ++)
        {
                if (bno_mons > 80) return;

                if (random3 (4) == 0) continue;

                for (bk = 0; bk < 85; bk ++)
                {
                        if (mons_class [bk] == 255) break;
                }

                lev_mons = 3;
                if (many_many < 10) lev_mons = 0;
                if (many_many > 9 && many_many < 13) lev_mons = 1;
                if (many_many > 12 && many_many < 19) lev_mons = 2;
                if (many_many > 18) lev_mons = 3;

                switch (lev_mons)
                {
                case 0: mons_alloc [0] = 14;
                mons_alloc [1] = 14;
                mons_alloc [2] = 14;
                mons_alloc [3] = 14;
                mons_alloc [4] = 14;
                mons_alloc [5] = 14;
                mons_alloc [6] = 14;
                mons_alloc [7] = 14;
                mons_alloc [8] = 14;
                mons_alloc [9] = 52; break;

                case 1: mons_alloc [0] = 14;
                mons_alloc [1] = 14;
                mons_alloc [2] = 14;
                mons_alloc [3] = 14;
                mons_alloc [4] = 14;
                mons_alloc [5] = 14;
                mons_alloc [6] = 14;
                mons_alloc [7] = 14;
                mons_alloc [8] = 52;
                mons_alloc [9] = 52; break;

                case 2: mons_alloc [0] = 14;
                mons_alloc [1] = 14;
                mons_alloc [2] = 14;
                mons_alloc [3] = 14;
                mons_alloc [4] = 14;
                mons_alloc [5] = 14;
                mons_alloc [6] = 55;
                mons_alloc [7] = 52;
                mons_alloc [8] = 52;
                mons_alloc [9] = 40; break;

                case 3: mons_alloc [0] = 14;
                mons_alloc [1] = 14;
                mons_alloc [2] = 52;
                mons_alloc [3] = 52;
                mons_alloc [4] = 55;
                mons_alloc [5] = 55;
                mons_alloc [6] = 52;
                mons_alloc [7] = 40;
                mons_alloc [8] = 40;
                mons_alloc [9] = 45; break;

                } // end of switch (many_many)


                mons_class [bk] = mons_alloc [random3(10)];
                define_monster(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_sec, bk);

                mons_x [bk] = bcount_x;
                mons_y [bk] = bcount_y;

                bno_mons++;

//              if (gmon_use [mons_class [bk]] > 0)

give_item(igrid, mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev, mons_speed,
mons_speed_inc, mons_inv, mons_sec, mons_ench_1, mons_ench, no_it,
item_ident, item_class, unique_items, item_type, item_plus, item_dam, item_quant,
item_x, item_y, icolour, xtra_quant, item_link, item_description,
property, mons_x, mons_y, grid);

if (mons_class [bk] == 30 | mons_class [bk] == 2) give_item(igrid, mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev, mons_speed,
mons_speed_inc, mons_inv, mons_sec, mons_ench_1, mons_ench, no_it,
item_ident, item_class, unique_items, item_type, item_plus, item_dam, item_quant,
item_x, item_y, icolour, xtra_quant, item_link, item_description,
property, mons_x, mons_y, grid);


// grid [bcount_x] [bcount_y] = 117;
//              grid [mons_x [bk] - 100] [mons_y [bk] - 100] += 70;

//              if (gmon_use [mons_class [bk] - 100] == 1) give_item();

        }
}
break;




case 1: // kobold lair
lordx = x1 + random3(x2 - x1);
lordy = y1 + random3(y2 - y1);

for (bcount_x = x1; bcount_x < x2; bcount_x ++)
{
        for (bcount_y = y1; bcount_y < y2; bcount_y ++)
        {
                if (bno_mons > 80) return;

                if (random3 (4) == 0) continue;

                for (bk = 0; bk < 85; bk ++)
                {
                        if (mons_class [bk] == 255) break;
                }

                lev_mons = 3;
                if (many_many < 4) lev_mons = 0;
                if (many_many > 3 && many_many < 6) lev_mons = 1;
                if (many_many > 5 && many_many < 9) lev_mons = 2;
                if (many_many > 8 && many_many < 10) lev_mons = 3;

                switch (lev_mons)
                {
                case 0: mons_alloc [0] = 36;
                mons_alloc [1] = 36;
                mons_alloc [2] = 36;
                mons_alloc [3] = 36;
                mons_alloc [4] = 36;
                mons_alloc [5] = 36;
                mons_alloc [6] = 36;
                mons_alloc [7] = 36;
                mons_alloc [8] = 58;
                mons_alloc [9] = 58; break;

                case 1: mons_alloc [0] = 36;
                mons_alloc [1] = 36;
                mons_alloc [2] = 36;
                mons_alloc [3] = 36;
                mons_alloc [4] = 36;
                mons_alloc [5] = 36;
                mons_alloc [6] = 58;
                mons_alloc [7] = 58;
                mons_alloc [8] = 58;
                mons_alloc [9] = 58; break;

                case 2: mons_alloc [0] = 36;
                mons_alloc [1] = 36;
                mons_alloc [2] = 36;
                mons_alloc [3] = 36;
                mons_alloc [4] = 36;
                mons_alloc [5] = 58;
                mons_alloc [6] = 58;
                mons_alloc [7] = 58;
                mons_alloc [8] = 58;
                mons_alloc [9] = 58; break;

                case 3: mons_alloc [0] = 36;
                mons_alloc [1] = 36;
                mons_alloc [2] = 36;
                mons_alloc [3] = 36;
                mons_alloc [4] = 58;
                mons_alloc [5] = 58;
                mons_alloc [6] = 58;
                mons_alloc [7] = 58;
                mons_alloc [8] = 58;
                mons_alloc [9] = 58; break;

                } // end of switch (many_many)

                mons_class [bk] = mons_alloc [random3(10)];

  if (bcount_x == lordx && bcount_y == lordy) // && many_many < 7
  {
    mons_class [bk] = 58;
  }

                define_monster(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_sec, bk);

                mons_x [bk] = bcount_x;
                mons_y [bk] = bcount_y;

                bno_mons++;

//              if (gmon_use [mons_class [bk]] > 0)
give_item(igrid, mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev, mons_speed,
mons_speed_inc, mons_inv, mons_sec, mons_ench_1, mons_ench, no_it,
item_ident, item_class, unique_items, item_type, item_plus, item_dam, item_quant,
item_x, item_y, icolour, xtra_quant, item_link, item_description,
property, mons_x, mons_y, grid);

                if (mons_class [bk] == 30 | mons_class [bk] == 2) give_item(igrid, mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev, mons_speed,
mons_speed_inc, mons_inv, mons_sec, mons_ench_1, mons_ench, no_it,
item_ident, item_class, unique_items, item_type, item_plus, item_dam, item_quant,
item_x, item_y, icolour, xtra_quant, item_link, item_description,
property, mons_x, mons_y, grid);


// grid [bcount_x] [bcount_y] = 117;
//              grid [mons_x [bk] - 100] [mons_y [bk] - 100] += 70;

//              if (gmon_use [mons_class [bk] - 100] == 1) give_item();

        }
}
break;


// Maybe have several types of treasure room?
case 2: // Treasure room! should only appear in deep levels, with a guardian


for (bcount_x = x1; bcount_x < x2; bcount_x ++)
{
        for (bcount_y = y1; bcount_y < y2; bcount_y ++)
        {
/*              depth = random3(depth_max) + 1;

                depth = 1;

                for (depth_c = 0; depth_c < depth; depth_c++)
                {

                if (no_it > 200) return;

                //if (random3 (4) == 0) continue;

                for (bp = 0; bp < 210; bp ++)
                {
                        if (item_class [bp] == 100) break;
                }

                // might as well use lev_mons:
                lev_mons = 3;
                if (many_many < 4) lev_mons = 0;
                if (many_many > 3 && many_many < 6) lev_mons = 1;
                if (many_many > 5 && many_many < 9) lev_mons = 2;
                if (many_many > 8 && many_many < 10) lev_mons = 3;

        int thing_type = random3(7);

        if (depth_c == 0) thing_type = random3(10);

        // ie only one lot of gold pieces in each pile


        //thing_type = random3(7);

//      do
//      {

        switch(thing_type)
        {
        case 0:
        case 1: item_class [bp] = 0;
        break;

        case 2:
        item_class [bp] = 1;
        break;

        case 3:
        case 4: item_class [bp] = 2;
        break;

        case 5: item_class [bp] = 3;
        break;

        case 6: item_class [bp] = 7;
        break;

        default: item_class [bp] = 15;

        }

//      } while (item_class [bp] > 5);

        quant = 1;

        switch(item_class [bp])
        {
                case 0:
                /*
                item_type [bp] = random3(11); // weapons
                if (many_many < 4 | random3(4) == 0) // ie these weapons are more common on any level.
                {
//                      item_type [bp] = random3(5);
                if (random3 (10) > 6) item_plus [bp] = random3 (3);
                        item_plus [bp] += 50;
                        switch(random3(6))
                        {
                                case 0: item_type [bp] = 0; break;
                                case 1: item_type [bp] = 1; break;
                                case 2: item_type [bp] = 3; break;
                                case 3: item_type [bp] = 4; break;
                                case 4: item_type [bp] = 5; break;
                                case 5: item_type [bp] = 14; break;
                        }
                break;
                }
/*              if (many_many >= 4 && many_many < 7)
                {
                        switch(random3(6))
                        {
                                case 0: item_type [bp] = 0; break;
                                case 1: item_type [bp] = 1; break;
                                case 2: item_type [bp] = 3; break;
                                case 3: item_type [bp] = 4; break;
                                case 4: item_type [bp] = 5; break;
                                case 5: item_type [bp] = 14; break;
                        }
                }* /

                if (random3 (10) > 2) item_plus [bp] = random3 (10);
                if (item_plus [bp] < 4) item_plus [bp] += 100;
                item_plus [bp] -= 4;
                item_plus [bp] += 50;

                //if (many_many >= 4) // && many_many < 7)
                //{
                item_type [bp] = random3 (15) + 1;
                //}
                quant = 1;
                break;

                case 1: // missiles
                item_type [bp] = random3(4);
                quant = random3(7) + random3 (6) + 1;// + random3 (10) + 1;
                if (random3 (10) > 7) item_plus [bp] = random3 (5);
                item_plus [bp] += 50;
                break;

                case 2: //item_type [bp] = random3(12); //      armour
                quant = 1;
                if (random3 (10) > 5) item_plus [bp] = random3 (5);
                if (random3 (5) == 0) item_plus [bp] = 99 - random3(5);
                //item_plus [bp] += 50;
                item_type [bp] = random3 (12);
                if (item_type [bp] == 6 && random3(3) == 0) item_type [bp] = 7;
                item_plus [bp] += 50;
                break;

                case 3: // item_type [bp] = random3(9); //      wands
/*              if (many_many < 4 | random3(3) == 0) // ie these wands are more common on any level.
                {
                        item_type [bp] = random3(7);
                        if (item_type [bp] < 2) item_plus [bp] = random3 (10) + random3 (10);
                        if (item_type [bp] >= 2 && item_type [bp] < 7) item_plus [bp] = random3 (5) + random3 (5);
                        // break;
                }        * /
//              if (many_many >= 4 && many_many < 7)
//              {
                        item_type [bp] = random3 (9);
                        if (item_type [bp] < 2) item_plus [bp] = random3 (10) + random3(10) + random3(10);
                        if (item_type [bp] >= 2 && item_type [bp] < 9) item_plus [bp] = random3 (5) + random3 (5) + random3 (5);
//              }
                quant = 1;
                break;



                case 4:
                item_type [bp] = random3 (5); // food
                quant = 1;
                if (random3(4) == 0) quant = random3(3) + 2;
                break;

                case 5: item_class [bp] = 8; // potions // This one not finished
                item_type [bp] = random3 (4);
                item_type [bp] = random3 (10);
                quant = 1;
                break;

                case 6: // item_class [bp] = 15; // scrolls
//              quant = random3 (30) + random3 (many_many * 10);
                item_type [bp] = random3(6);
                if (random3 (5) == 0) item_type [bp] = 0; // identify should be very common
                quant = 1;
                if (random3 (3) == 0) quant = random3 (3) + 1;
                //item_plus [bp] = random3(200);
//              item_plus [bp] = dung1;
//              item_dam [bp] = dung2;
                break;

                case 7: // rings
                item_class [bp] = 7;
                item_type [bp] = random3(12);
//              if (item_type [bp] < 4) item_dam [bp] = random3(13);
//               else item_dam [bp] = random3(13) + (random3(13) * 13);
                quant = 1;
                if (item_type [bp] == 1 | item_type [bp] == 5 | item_type [bp] == 11)
                {
                        do
                        {
                        item_plus [bp] = random3 (3) + random3 (3) + random3 (3);
                        item_plus [bp] -= 4;
                        item_plus [bp] += 50;
                        } while (item_plus [bp] != 50);
                }
                break;
/*
                case 8: // potions
                item_type [bp] = random3(9);
                quant = 1;
                if (random3(4) == 0) quant = random3(3) + 2;
                break;* /

                default: // money - class = 15
                item_class [bp] = 15;
                quant = (random3(10) + random3(3) + 1) * (many_many + 1);
                break;

        }
//      if (item_class [bp] == 4) item_class [bp] = 6;
//      if (item_class [bp] == 5) item_class [bp] = 8;


        //item_class [bp] = 15;
        //item_type [bp] = random3(7);

        //if (item_class [bp] == 15)
        //{
        //gold_q = random3(30000) + 100000;
        //gold_q = 10;









item_quant [bp] = quant;

//      gold_q = quant + 100000;









        //gold_q = 10 + 100000;
//      cout << gold_q << " + ";
                //gold_q = 100000;











        //ltoa(gold_q, item_quant [bp], 10);//remember that the 10 is the base!

//      itoa(gold_q, item_quant [bp], 10);














        // use ltoa because gold_q is a long variable.

//cout << (int) item_class [bp] << ", " << (int) item_type [bp] << " " << (unsigned long) gold_q << " ";
//cprintf(item_quant [bp]);
//cout << "\n";
//if (quant > 40) something = getch();
        //for (bk = 0; bk < 5; bk ++)
        //{

/*      if (gold_q < 10000) item_quant [bp] [0] = 48; // ASCII 48 is 0.
        if (gold_q < 1000) item_quant [bp] [1] = 48;
        if (gold_q < 100) item_quant [p] [2] = 48;
        if (gold_q < 10) item_quant [p] [3] = 48;
        if (gold_q == 0) item_quant [p] [4] = 48;
*/
        //}
        //itoa(damage, damage_print, 10);

//      cprintf(item_quant [p]);
//      cprintf(" * ");
        //}

        //item_quant [p] = 1;
//      item_plus [p] = random3(5);

        //if (item_class [p] != 15) item_class [p] += 100000;

/*      do
        {
                item_x [p] = random3 (80);
                item_y [p] = random3 (70);
        } while (grid [item_x [p]] [item_y [p]] != 17);*/

switch(random3(11))
{
 case 9:
 case 0: glopop = 0; break;
 case 1: glopop = 1; break;
 case 10:
 case 2: glopop = 2; break;
 case 3: glopop = 3; break;
 case 4: glopop = 5; break;
 case 5: glopop = 6; break;
 case 6: glopop = 7; break;
 case 7: glopop = 10; break;
 case 8: glopop = 11; break;
}


thing_created = items(no_it, item_ident, item_class, unique_items, item_type, item_plus, item_dam,
 item_quant, item_x, item_y, icolour, xtra_quant, item_link, item_description,
 grid, glopop, 250, 1, many_many * 2);

        item_x [thing_created] = bcount_x;
        item_y [thing_created] = bcount_y;



//      item_colour(bp, item_class, item_type, item_plus, item_dam, icolour,
//      item_description);





























//      } // end of for (depth_c)








        }

}



                // guardian

                for (bk = 0; bk < MNST; bk ++)
                {
                        if (mons_class [bk] == 255) break;
                        if (bk == MNST - 3) return; // surely this couldn't happen?!
                }

                mons_class [bk] = 39; //mons_alloc [random3(10)];
                define_monster(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_sec, bk);

                mons_x [bk] = x1;// + random3(room_x2 - room_x1 + 1);
                mons_y [bk] = y1;// + random3(room_y2 - room_y1 + 1);

//              cout << (int) mons_y [bk] << "," << (int) mons_x [bk] << " ";
//              getch();

                bno_mons++;

//              if (gmon_use [mons_class [bk]] > 0)
/*give_item(igrid, mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev, mons_speed,
mons_speed_inc, mons_inv, mons_sec, mons_ench_1, mons_ench, no_it,
item_ident, item_class, item_type, item_plus, item_dam, item_quant,
item_x, item_y, icolour, xtra_quant, item_link, item_description,
property, mons_x, mons_y, grid);

                if (mons_class [bk] == 30 | mons_class [bk] == 2) give_item(igrid, mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev, mons_speed,
mons_speed_inc, mons_inv, mons_sec, mons_ench_1, mons_ench, no_it,
item_ident, item_class, item_type, item_plus, item_dam, item_quant,
item_x, item_y, icolour, xtra_quant, item_link, item_description,
property, mons_x, mons_y, grid);*/


// grid [bcount_x] [bcount_y] = 117;
//              grid [mons_x [bk] - 100] [mons_y [bk] - 100] += 70;

//              if (gmon_use [mons_class [bk] - 100] == 1) give_item();

//      }
//}





break; // end of case 2




case 3: // beehive

//break;

beehive(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_x, mons_y,    mons_beh,       mons_inv,
 mons_targ_1_x, mons_targ_1_y,  mons_sec, mons_ench_1, mons_ench,
no_it, item_ident, item_class, unique_items, item_type, item_plus, item_dam, item_quant,
item_x, item_y, icolour, xtra_quant, item_link, item_description, property,
trap_type, trap_x, trap_y, mons_alloc, grid);

break; // end of beehive (case 3)

} // end of switch room_type







/*
if (room_x2 < 47) dx = 1; else dx = -1;
if (room_y2 < 42) dy = 1; else dy = -1;
*/


/*
rd = random3(4);

switch(rd)
{
        case 0: cx = x1 + (random3(x2 - x1));
                cy = y1;
                dx = 0; dy = -1;
                break;
        case 1: cx = x1 + (random3(x2 - x1));
                cy = y2;
                dx = 0; dy = 1;
                break;
        case 2: cy = y1 + (random3(y2 - y1));
                cx = x1;
                dx = -1; dy = 0;
                break;
        case 3: cy = y1 + (random3(y2 - y1));
                cx = x2;
                dx = 1; dy = 0;
                break;
}

sx = cx;
sy = cy;

for (bcount_x = 0; bcount_x < 70; bcount_xy ++)
{

*/



}




/*
for (bcount_x = room_x1; bcount_x < room_x2; bcount_x ++)
{
        for (bcount_y = room_y1; bcount_y < room_y2; bcount_y ++)
        {
                if (grid [bcount_x] [bcount_y] == 105) //               grid [bcount_x] [bcount_y] = 17;
                {
                        k--;
                        time_run ++;
                        return;
                }
        }
}


for (bcount_x = room_x1; bcount_x < room_x2; bcount_x ++)
{
        for (bcount_y = room_y1; bcount_y < room_y2; bcount_y ++)
        {
                if (grid [bcount_x] [bcount_y] < 18) grid [bcount_x] [bcount_y] = 17;
        }
}

*/

//}








void beehive(int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
unsigned char grid [80] [70])
{

//return;

long quant = 0;
long gold_q = 0;

/*
int kx1 = x1;// = room_x1 + 1;
int kx2 = x2;// room_x2 - 1;
int ky1 = y1;// room_y1 + 1;
int ky2 = y2;// room_y2 - 1;
*/

for (bcount_x = x1; bcount_x < x2; bcount_x ++)
{
        for (bcount_y = y1; bcount_y < y2; bcount_y ++)
        {

                if (no_it > 200) return;

                if (random3 (2) == 0) continue;

                for (bp = 0; bp < 210; bp ++)
                {
            if (bp == 200) goto finished_it;
                if (item_class [bp] == 100) break;
                }

        item_class [bp] = 4;

        item_type [bp] = 5;
        if (random3(25) == 0) item_type [bp] = 6; // royal jelly
        quant = 1;

//      gold_q = quant + 100000;

        //ltoa(gold_q, item_quant [bp], 10); // remember that the 10 is the base!

 item_quant [bp] = quant;
//      itoa(gold_q, item_quant [bp], 10);

        item_x [bp] = bcount_x;
        item_y [bp] = bcount_y;

        item_colour(bp, item_class, unique_items, item_type, item_plus, item_dam, icolour,
        item_description);

        no_it++;

        }
}


finished_it : char queen = 1;
int bcount_y2 = 0;
int bcount_x2;


for (bcount_x = x1; bcount_x < x2; bcount_x ++)
{
        for (bcount_y = y1; bcount_y < y2; bcount_y ++)
        {
                if (bno_mons > 100) return;

                if (grid [bcount_x] [bcount_y] != 17 && grid [bcount_x] [bcount_y] != 117) continue; // shouldn't need this, but oh well.
      // 117 for "special rooms"


                //if (random3 (4) == 0) continue;

                for (bk = 0; bk < 85; bk ++)
                {
                        if (mons_class [bk] == 255) break;
                }

                switch(random3(6))
                {
                        case 0:
                                mons_class [bk] = 11;
                        break;

                        case 5:
                        if (queen == 1)
                        {
                                mons_class [bk] = 42;
                                queen = 0;
                                break;
                        } // otherwise fall through


                        case 1:
                        case 2:
                        case 3:
                        case 4:
                                mons_class [bk] = 10;
                        break;
                }

                if (queen == 1 && bcount_x == x2 - 1 && bcount_y == y2 - 1)
                {
                        mons_class [bk] = 42;
                }

                //mons_class [bk] = mons_alloc [random3(10)];
                define_monster(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_sec, bk);

                mons_x [bk] = bcount_x;
                mons_y [bk] = bcount_y;

                grid [mons_x [bk]] [mons_y [bk]] += 70;

                bno_mons++;

                mons_beh [bk] = 100; // should be sleeping

                //if (gmon_use [mons_class [bk] - 100] > 0) give_item();
                //if (mons_class [bk] == 130) give_item();

                }

                }
/*
int x1 = kx1;
int x2 = kx2;
int y1 = ky1;
int y2 = ky2;
*/


} // end of void beehive()










void morgue(int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
unsigned char grid [80] [70])
{

//return;

long quant = 0;
long gold_q = 0;

/*

for (bcount_x = x1; bcount_x < x2; bcount_x ++)
{
        for (bcount_y = y1; bcount_y < y2; bcount_y ++)
        {

                if (no_it > 200) return;

                if (random3 (2) == 0) continue;

                for (bp = 0; bp < 210; bp ++)
                {
            if (bp == 200) goto finished_it;
                if (item_class [bp] == 100) break;
                }

        item_class [bp] = 4;

        item_type [bp] = 5;
        if (random3(25) == 0) item_type [bp] = 6; // royal jelly
        quant = 1;

        gold_q = quant + 100000;

        //ltoa(gold_q, item_quant [bp], 10); // remember that the 10 is the base!


        itoa(gold_q, item_quant [bp], 10);

        item_x [bp] = count_x;
        item_y [bp] = count_y;

        item_colour(p);

        no_it++;

        }
}


finished_it : char queen = 1;
*/


int bcount_y2 = 0;
int bcount_x2;

for (bcount_x = x1; bcount_x < x2; bcount_x ++)
{
        for (bcount_y = y1; bcount_y < y2; bcount_y ++)
        {
                if (bno_mons > 100) return;

                if (grid [bcount_x] [bcount_y] != 17 && grid [bcount_x] [bcount_y] != 117) continue; // shouldn't need this, but oh well.
      // 117 for "special rooms"


                //if (random3 (4) == 0) continue;

                for (bk = 0; bk < 85; bk ++)
                {
                        if (mons_class [bk] == 255) break;
                }

                switch(random3(6))
                {

   case 0:
   case 1:
   case 2:
//   case 3:
   mons_class [bk] = 25;
      define_zombie(3, mons_class,
mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev, mons_speed, mons_speed_inc,
mons_sec);
      //mons_sec [bk] = random3(2);
   break;

   case 3: mons_class [bk] = 60;
   define_monster(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_sec, bk);
   break;


   case 4: mons_class [bk] = 13;
   define_monster(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_sec, bk);
   break;

   case 5: mons_class [bk] = 48;
        if (random3(4) == 0) mons_class [bk] = 47;
   define_monster(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_sec, bk);
   break;

/*   case 6: mons_class [bk] = 48;
   break;*/

                }

                //mons_class [bk] = mons_alloc [random3(10)];
//              define_monster();

                mons_x [bk] = bcount_x;
                mons_y [bk] = bcount_y;

                grid [mons_x [bk]] [mons_y [bk]] += 70;

                bno_mons++;

                mons_beh [bk] = 100; // should be sleeping

                //if (gmon_use [mons_class [bk] - 100] > 0) give_item();
                //if (mons_class [bk] == 130) give_item();

                }

                }
/*
int x1 = kx1;
int x2 = kx2;
int y1 = ky1;
int y2 = ky2;
*/


} // end of void morgue()



void define_zombie(char not_zombsize, int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_sec [MNST]) // 1=2, 2=1, 3=1 or 2
{
int mons_sec2 = 0;

//case 25: // small zombie
//if (mons_sec == 250)
//{

   do
   {
      mons_sec [bk] = random3(52);
   } while (xmons_zombie_size((int) mons_sec [bk]) == 0 | xmons_zombie_size((int) mons_sec [bk]) == not_zombsize);

   mons_sec2 = mons_sec [bk];

//}
//mon_HD = random3(2) + 1;

mons_class [bk] = mons_sec [bk];

define_monster(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_sec, bk);

//mons_class [bk] -= 100;
/*
mons_HD [bk] -= 100;
mons_hp [bk] -= 100;
mons_hp_max [bk] -= 100;
mons_AC [bk] -= 100;
mons_ev [k] -= 100;
mons_speed [k] -= 100;
mons_speed_inc [k] = 70;
mons_speed_inc [k] -= 100 + random3(9);
*/

//mons_HD [k] = mons_define(mons_sec [k], 0, 0);
mons_hp [bk] = (random3((mons_HD [bk]) * 5) + (mons_HD [bk]) * 3);
mons_hp_max [bk] = mons_hp [bk];
mons_AC [bk] = 0;//gmons_AC [k] [2];
mons_ev [bk] = 4;//gmons_ev [k] [2];
mons_speed [bk] = 7;
mons_speed_inc [bk] = 70;
//break;

mons_sec [bk] = mons_sec2;

mons_class [bk] = 25;

if (xmons_zombie_size((int) mons_sec [bk]) == 2)
{
 mons_class [bk] = 51;
 mons_speed [bk] = 6;
}

//mons_class [bk] += 100;
/*
mons_HD [bk] += 100;
mons_hp [k] += 100;
mons_hp_max [k] += 100;
mons_AC [k] += 100;
mons_ev [k] += 100;
mons_speed [k] += 100;
mons_speed_inc [k] = 70;
mons_speed_inc [k] += 100 + random3(9);
*/

}






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
case 13: strcat(gmon_name, "necrophage"); break;// don't know about this one.
case 14: strcat(gmon_name, "orc"); break;
case 15: strcat(gmon_name, "piercer"); break;
case 16: strcat(gmon_name, "quasit"); break;
case 17: strcat(gmon_name, "rat"); break;
case 18: strcat(gmon_name, "snake"); break;
case 19: strcat(gmon_name, "tunneling worm"); break;
case 20: strcat(gmon_name, "unicorn"); break;
case 21: strcat(gmon_name, "vortex"); break;
case 22: strcat(gmon_name, "giant wasp"); break;
case 23: strcat(gmon_name, "abomination"); break;
case 24: strcat(gmon_name, "yellow light"); break;
case 25: strcat(gmon_name, "small zombie"); break;
case 26: strcat(gmon_name, "angel"); break;
case 27: strcat(gmon_name, "giant beetle"); break;
case 28: strcat(gmon_name, "cyclops"); break;
case 29: strcat(gmon_name, "dragon"); break;
case 30: strcat(gmon_name, "elemental"); break;
case 31: strcat(gmon_name, "pit fiend"); break;
case 32: strcat(gmon_name, "giant spore"); break;
case 33: strcat(gmon_name, "hobgoblin"); break;
case 34: strcat(gmon_name, "something invisible"); break;// not a real creature
case 35: strcat(gmon_name, "jelly"); break;
case 36: strcat(gmon_name, "kobold"); break;
case 37: strcat(gmon_name, "lich"); break;
case 38: strcat(gmon_name, "mummy"); break;
case 39: strcat(gmon_name, "naga"); break;
case 40: strcat(gmon_name, "ogre"); break;
case 41: strcat(gmon_name, "plant"); break;
case 42: strcat(gmon_name, "queen bee"); break;
case 43: strcat(gmon_name, "raksasha"); break;
case 44: strcat(gmon_name, "spectre"); break;
case 45: strcat(gmon_name, "troll"); break;
case 46: strcat(gmon_name, "undine"); break;
case 47: strcat(gmon_name, "vampire"); break;
case 48: strcat(gmon_name, "wraith"); break;
case 49: strcat(gmon_name, "abomination"); break;
case 50: strcat(gmon_name, "yak"); break;
case 51: strcat(gmon_name, "large zombie"); break;
case 52: strcat(gmon_name, "orc warrior"); break;
case 53: strcat(gmon_name, "kobold demonologist"); break;
case 54: strcat(gmon_name, "orc wizard"); break;
case 55: strcat(gmon_name, "goblin wizard"); break;
case 56: strcat(gmon_name, "tail section"); break;
57: wyvern
*/








void specr_2(unsigned char grid [80] [70])
{

int bkout = 0;
int g = 0;
int h = 0;


grolko :
if (bkout > 100) return; //0;
rd = random3(4);
switch(rd)
{
        case 0: cx = x1 + (random3(x2 - x1));
                cy = y1;
                dx = 0; dy = -1;
                break;
        case 1: cx = x1 + (random3(x2 - x1));
                cy = y2;
                dx = 0; dy = 1;
                break;
        case 2: cy = y1 + (random3(y2 - y1));
                cx = x1;
                dx = -1; dy = 0;
                break;
        case 3: cy = y1 + (random3(y2 - y1));
                cx = x2;
                dx = 1; dy = 0;
                break;
}

sx = cx;
sy = cy;

for (bcount_x = 0; bcount_x < 100; bcount_x ++)
{
        sx += dx;
        sy += dy;
        if (sx < 6 | sx > 73 | sy < 6 | sy > 63)
        {
                bkout++;
                goto grolko;
        }
/*
        for (g = -1; g < 2; g ++)
        {
                for (h = -1; h < 2; h ++)
                {
                        if (grid [sx + g] [sy + h] == 17) break;
                }
        }
*/
if (bcount_x > 0)
{
        if (grid [sx + 1] [sy] % 50 == 17) break;
        if (grid [sx] [sy + 1] % 50 == 17) break;
        if (grid [sx - 1] [sy] % 50 == 17) break;
        if (grid [sx] [sy - 1] % 50 == 17) break;
}

} // end of for bcount_x

sx = cx;
sy = cy;

for (bcount_y = 0; bcount_y < bcount_x + 2; bcount_y++)
{
//      if (bcount_y == 0)
//      {
                if (grid [sx] [sy] == 105)
                grid [sx] [sy] = 3; // door
//      }

 if (bcount_y > 0 && grid [sx + dx] [sy + dy] % 50 > 1 && grid [sx + dx] [sy + dy] % 50 < 17)
 {
  grid [sx] [sy] = 18;
//  break;
 }

        if (grid [sx] [sy] == 1) grid [sx] [sy] = 106;

        sx += dx;
        sy += dy;
}


// return 1;

} // end of specr_2()








void give_item(int igrid [80] [70],
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
int mons_inv [MNST] [8],
unsigned char mons_sec [MNST],
unsigned char mons_ench_1 [MNST],
unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
unsigned char grid [80] [70]
)
{
unsigned long long_q;
int iquan = 1;
int bp = 0;
int thing_created = 0;
char hand_used = 0; // for Ettins etc.
int xitc = 0;
int xitt = 0;

int force_item = 0;
/*while (item_class [bp] != 100 && bp < ITEMS)
{
        p++;
}*/

for (bp = 0; bp < ITEMS - 100; bp ++)
{
   if (item_quant [bp] == 0) break;
}

//cprintf("X");

//if (mons_class [bk] == 30


if (bp >= ITEMS - 100) return; // already too many.

//item_class [bp] = 0; // hand weapon

item_plus [bp] = 50;
item_dam [bp] = 0;
item_class [bp] = 101;

switch(mons_class [bk])
{

case 36: // kobold
if (random3 (5) < 3) // < 1) // give hand weapon
        {
        item_class [bp] = 0;
        icolour [bp] = 11;
        switch(random3(6))
                {
                        case 0: item_type [bp] = 0; icolour [bp] = 6;
                        break;
                        case 1: item_type [bp] = 3; break;
                        case 2: item_type [bp] = 5; //icolour [bp] = 6;
                        break;
                        case 3: item_type [bp] = 5; break;
                        case 4: item_type [bp] = 3; //icolour [bp] = 6;
                        break;
                }
        } else
              if (random3 (5) < 2)
                {
                        item_class [bp] = 1;
                        item_type [bp] = 3;
                        icolour [bp] = 11;
                        iquan = random3(5) + 1;
                } else goto give_armour;//return;

break;

case 33:  // Hobgoblin
if (random3 (5) < 3) // < 2 // give hand weapon
        {
        item_class [bp] = 0;
        item_type [bp] = 0;
        icolour [bp] = 6;
        } else goto give_armour;//return;
break;

case 6: // goblin
if (random3 (5) < 3) // < 1 // give hand weapon
        {
        item_class [bp] = 0;
        icolour [bp] = 11;
        switch(random3(2))
                {
                        case 0: item_type [bp] = 0; icolour [bp] = 6;
                        break;
                        case 1: item_type [bp] = 3; break;
//                      case 2: item_type [bp] = 12; break;
//                      case 3: item_type [bp] = 5; break;
//                      case 4: item_type [bp] = 14; break;
                }
        } else goto give_armour;//return;
break;


case 60: // Wight
 item_class [bp] = 0;
        icolour [bp] = 11;
 item_type [bp] = 1 + random3(12);
 if (random3(2) == 0)
 {
   item_plus [bp] += random3(3) + 1;
   if (random3(10) == 0) item_dam [bp] = 2;
 }

 if (random3(3) == 0) item_plus [bp] += 100;

break;



case 14: // orc
if (random3 (5) < 4)
        {
        item_class [bp] = 0;
        icolour [bp] = 11;
        switch(random3(8))
                {
                        case 0: item_type [bp] = 2; break;
                        case 1: item_type [bp] = 4; break;
                        case 2: item_type [bp] = 8; break;
                        case 3: item_type [bp] = 9; break;
                        case 4: item_type [bp] = 12; break;
                        case 5: item_type [bp] = 5; break;
                        case 6: item_type [bp] = 3; break;
                        case 7: item_type [bp] = 0; break;
//                      case 8: item_type [bp] = 5; break;
//                      case 9: item_type [bp] = 14; break;
                }
        } else goto give_armour;//return;
break;

case 52: // orc warrior
        item_class [bp] = 0;
        icolour [bp] = 11;
        switch(random3(9))
                {
                        case 0: item_type [bp] = 2; break;
                        case 1: item_type [bp] = 4; break;
                        case 2: item_type [bp] = 8; break;
                        case 3: item_type [bp] = 9; break;
                        case 4: item_type [bp] = 12; break;
                        case 5: item_type [bp] = 5; break;
                        case 6: item_type [bp] = 6; break;
                        case 7: item_type [bp] = 10; break;
                        case 8: item_type [bp] = 17; break;
                }
break;

case 55: // orc knight
        item_class [bp] = 0;
        icolour [bp] = LIGHTCYAN;
        switch(random3(3))
                {
                        case 0: item_type [bp] = 6; break; // long sword
                        case 1: item_type [bp] = 7; break; // great sword
                        case 2: item_type [bp] = 10; break; // battleaxe
                }
 if (random3(3) == 0) item_plus [bp] += random3(4);
break;

case 28:  // Cyclops
//if (random3 (5) < 5) // < 2 // give hand weapon
//      {
        item_class [bp] = 1;
        item_type [bp] = 5;
        icolour [bp] = LIGHTGRAY;
        iquan = random3(5) + 1;
//      } //else goto give_armour;//return;
break;


case 30: // two-headed Ogre
        hand_used = 0;

        if (mons_inv [bk] [0] != 501) hand_used = 1;

        item_class [bp] = 0;
        item_type [bp] = 20; // giant club
        if (random3(3) == 0) item_type [bp] = 21;
        icolour [bp] = BROWN;
        iquan = 1;
break;

case 83: // reaper devil
        item_class [bp] = 0;
        item_type [bp] = 19; // scythe
        icolour [bp] = LIGHTCYAN;
        iquan = 1;
break;


case 40: // Ogre
        item_class [bp] = 0;
        item_type [bp] = 20; // giant club
        if (random3(3) == 0) item_type [bp] = 21;
        icolour [bp] = BROWN;
        iquan = 1;
break;


case 2: // centaur
        if (mons_inv [bk] [0] != 501)
        {
                item_class [bp] = 1;
                item_type [bp] = 1;
                icolour [bp] = BROWN;
                iquan = 3 + random3(10);
                if (random3(3) == 0) item_plus [bp] += random3(4);
        } else
              {
                item_class [bp] = 0;
                item_type [bp] = 14;
                icolour [bp] = BROWN;
                iquan = 1;
                if (random3(3) == 0) item_plus [bp] += random3(4);
              }

break;

case 68: // Efreet
item_class [bp] = 0;
item_type [bp] = 8;
item_dam [bp] = 1;
icolour [bp] = RED;
iquan = 1;
force_item = 1;
break;


case 26: // Angel
item_class [bp] = 0;
item_type [bp] = 6;
item_dam [bp] = 93; // glowing, holy wrath
item_plus [bp] += random3(3) + 1;
icolour [bp] = WHITE;
iquan = 1;
force_item = 1;
break;

} // end of switch(mons_class [bk])


//item_quant [bp] = 1;
//long_q = 100000 + iquan;

if (item_class [bp] == 101)
{
        item_class [bp] = 100;
 goto give_armour;
}

item_quant [bp] = iquan;

//ltoa(long_q, item_quant [bp], 10);//remember that the 10 is the base (radix)!

//itoa(long_q, item_quant [bp], 10);

 //                     Look at this!!!
//item_dam [bp] = 0;

xitc = item_class [bp];
xitt = item_type [bp];
//cprintf("Y");
if (force_item != 0) thing_created = bp;
 else thing_created = items(no_it, item_ident, item_class, unique_items, item_type, item_plus, item_dam,
       item_quant, item_x, item_y, icolour, xtra_quant, item_link, item_description,
       grid, xitc, xitt, 1, many_many);
//cprintf("Z");

item_x [thing_created] = 1; //mons_x [bk];
item_y [thing_created] = 1; //mons_y [bk];
item_ident [thing_created] = 0;
if (item_class [thing_created] == 0) mons_inv [bk] [hand_used] = thing_created; // hand_used = 0 unless Ettin's 2nd hand etc.
if (item_class [thing_created] == 1) mons_inv [bk] [1] = thing_created;
//if (item_class [bp] == 2) mons_inv [bk] [2] = p;
if (item_class [thing_created] == 6) mons_inv [bk] [4] = thing_created;
if (item_class [thing_created] == 15) mons_inv [bk] [4] = thing_created;

if (item_dam [thing_created] % 30 == 7) mons_AC [bk] += 3;
/*
item_x [bp] = 1; //mons_x [bk];
item_y [bp] = 1; //mons_y [bk];
item_ident [bp] = 0;
if (item_class [bp] == 0) mons_inv [bk] [hand_used] = bp; // hand_used = 0 unless Ettin's 2nd hand etc.
if (item_class [bp] == 1) mons_inv [bk] [1] = bp;
//if (item_class [bp] == 2) mons_inv [bk] [2] = p;
if (item_class [bp] == 6) mons_inv [bk] [4] = bp;
if (item_class [bp] == 15) mons_inv [bk] [4] = bp;
*/
item_colour(thing_created, item_class, unique_items, item_type, item_plus, item_dam, icolour,
        item_description);

//name_item();

/*
if (mons_class [bk] == 102) // centaur
{


}*/











give_armour : bp = 0;

while ((item_class [bp] != 100 | item_quant [bp] != 0) && bp < ITEMS)
{
        bp++;
}

if (bp > ITEMS - 100) return; // already too many.

//item_class [bp] = 0; // hand weapon

switch(mons_class [bk])
{
/*
case 36: // kobold
if (random3 (5) < 3) // < 1) // give hand weapon
        {
        item_class [bp] = 0;
        switch(random3(6))
                {
                        case 0: item_type [bp] = 0; break;
                        case 1: item_type [bp] = 3; break;
                        case 2: item_type [bp] = 12; break;
                        case 3: item_type [p] = 5; break;
                        case 4: item_type [p] = 14; break;
                }
        } else
              if (random3 (5) < 2)
                {
                        item_class [p] = 1;
                        item_type [p] = 3;
                        iquan = random3(5) + 1;
                } else return;

break;

case 33:  // Hobgoblin
if (random3 (5) < 3) // < 2 // give hand weapon
        {
        item_class [p] = 0;
        item_type [p] = 0;
        } else return;
break;

case 6: // goblin
if (random3 (5) < 3) // < 1 // give hand weapon
        {
        item_class [p] = 0;
        switch(random3(2))
                {
                        case 0: item_type [p] = 0; break;
                        case 1: item_type [p] = 3; break;
//                      case 2: item_type [p] = 12; break;
//                      case 3: item_type [p] = 5; break;
//                      case 4: item_type [p] = 14; break;
                }
        } else return;
break;

*/


case 14: // orc
//case 36:
iquan = 1;
if (random3 (5) < 4)
        {
        item_class [bp] = 2;
        icolour [bp] = 11;
        switch(random3(8))
                {
                        case 0: item_type [bp] = 1; //break;
                        case 1: item_type [bp] = 1; //break;
                        case 2: item_type [bp] = 1; //break;
                        case 3: item_type [bp] = 1; icolour [bp] = 6;
                        break;
                        case 4: item_type [bp] = 2; break;
                        case 5: item_type [bp] = 2; break;
                        case 6: item_type [bp] = 3; break;
                        case 7: item_type [bp] = 4; break;
//                      case 8: item_type [bp] = 5; break;
//                      case 9: item_type [bp] = 14; break;
                }
        } else return;
break;

case 52: // orc warrior
//case 6:
        item_class [bp] = 2;
        icolour [bp] = 11;
        switch(random3(8))
                {
                        case 0: item_type [bp] = 1; icolour [bp] = 6;
                        break;
                        case 1: item_type [bp] = 1; icolour [bp] = 6;
                        break;
                        case 2: item_type [bp] = 2; break;
                        case 3: item_type [bp] = 2; break;
                        case 4: item_type [bp] = 3; break;
                        case 5: item_type [bp] = 3; break;
                        case 6: item_type [bp] = 4; break;
                        case 7: item_type [bp] = 4; break;
                }
break;

case 55: // orc knight
 item_class [bp] = 2;
 icolour [bp] = 11;
 item_type [bp] = 4 + random3(4);
 iquan = 1;
break;

case 26:
case 60: // Wight & Angel
        item_class [bp] = 2;
        item_type [bp] = 0;
        icolour [bp] = WHITE;
        iquan = 1;

break;


default: return;

} // end of switch(mons_class [bk])

iquan = 1;

//item_quant [bp] = 1;
//long_q = 100000 + iquan;
//item_quant [bp] = iquan;
//ltoa(long_q, item_quant [bp], 10);//remember that the 10 is the base (radix)!

//itoa(long_q, item_quant [bp], 10);

//item_plus [bp] = 0; //                        Look at this!!!

//if (random3(10) == 0) item_plus [bp] = 1;
//if (random3(20) == 0) item_plus [bp] = random3(3) + 1;

//item_plus [bp] += 50;
//item_ident [bp] = 0;
//item_dam [bp] = 0;

xitc = item_class [bp];
xitt = item_type [bp];

thing_created = items(no_it, item_ident, item_class, unique_items, item_type, item_plus, item_dam,
 item_quant, item_x, item_y, icolour, xtra_quant, item_link, item_description,
 grid, xitc, xitt, 1, many_many);


item_x [thing_created] = 1; //mons_x [bk] - 100;
item_y [thing_created] = 1; //mons_y [bk] - 100;
mons_inv [bk] [2] = thing_created;

/*
item_x [bp] = 1; //mons_x [bk] - 100;
item_y [bp] = 1; //mons_y [bk] - 100;
mons_inv [bk] [2] = bp;
*/
if (mons_class [bk] != 60) item_colour(thing_created, item_class, unique_items, item_type, item_plus, item_dam, icolour,
        item_description); // Wights' robes are white
if (mons_class [bk] == 60) icolour [thing_created] = WHITE;
//mons_AC [bk] += property [2] [item_type [bp]] [0];
//mons_ev [bk] += property [2] [item_type [bp]] [1];


/*
        if (item_plus [thing_created] > 130)
                mons_AC [bk] += property [item_class [thing_created]] [item_type [thing_created]] [0] + (item_plus [thing_created] - 150);
         else   mons_AC [bk] += property [item_class [thing_created]] [item_type [thing_created]] [0] + item_plus [thing_created] - 50;

        mons_ev [bk] += property [item_class [thing_created]] [item_type [thing_created]] [1];
 if (mons_ev [bk] < 1) mons_ev [bk] = 1; // This *shouldn't* happen.
*/
        if (item_plus [thing_created] > 130)
                mons_AC [bk] += property [item_class [thing_created]] [item_type [thing_created]] [0] + (item_plus [thing_created] - 150);
         else   mons_AC [bk] += property [item_class [thing_created]] [item_type [thing_created]] [0] + item_plus [thing_created] - 50;

        mons_ev [bk] += property [item_class [thing_created]] [item_type [thing_created]] [1] / 2;
 if (mons_ev [bk] < 1) mons_ev [bk] = 1; // This *shouldn't* happen.


} // end of void give_item()





void item_colour(int p, unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
unsigned char icolour [ITEMS],
unsigned char item_description [5] [30])
{
bp = p;

int secchar;
        int switchnum = 0;


        switch(item_class [bp])
        {
                case 0:
  if (item_dam [bp] > 180)
  {
   switch(item_dam [bp] - 180)
   {
    case 1: icolour [bp] = YELLOW; break;
    case 2: icolour [bp] = RED; break;
    case 3: icolour [bp] = DARKGRAY; break;
    case 4: icolour [bp] = random3(15) + 1; break;
    case 5: icolour [bp] = MAGENTA; break;
   }
  break;
  }

                switch(item_type [bp])
                {

                        case 0:
                        case 13:
                        case 14:
                        case 15:
   case 18:
                        case 20:
                        case 21:
                        icolour [bp] = BROWN;
                        break;

                        default: icolour [bp] = LIGHTCYAN; break;
                }
                break;

                case 1:
                switch(item_type [bp])
                {
                        case 0:
                        case 5:
                        icolour [bp] = BROWN;
                        break;

                        default: icolour [bp] = LIGHTCYAN;
                        break;
                }
                break;

                case 2:
                switch(item_type [bp])
                {
                        case 0:
                        case 9:
                        icolour [bp] = random3(15) + 1;
                        break;

                        case 1:
                        case 11:
                        case 12: // maybe more interesting boot colours?
                        icolour [bp] = BROWN;
                        break;

                        default: icolour [bp] = LIGHTCYAN; break;
                }
                break;

                case 3: item_dam [bp] = item_description [0] [item_type [bp]];
                switch(item_dam [bp] % 12)
                {
                case 0: //strcpy(fake_name [bp], "iron wand");
                icolour [bp] = CYAN; break;
                case 1: //strcpy(fake_name [bp], "brass wand");
                icolour [bp] = YELLOW; break;
                case 2: //strcpy(fake_name [bp], "bone wand");
                icolour [bp] = LIGHTGRAY;  break;
                case 3: //strcpy(fake_name [bp], "wooden wand");
                icolour [bp] = 6;  break;
                case 4: //strcpy(fake_name [bp], "copper wand");
                icolour [bp] = 6;  break;
                case 5: //strcpy(fake_name [bp], "gold wand");
                icolour [bp] = 14; break;
                case 6: //strcpy(fake_name [bp], "silver wand");
                icolour [bp] = WHITE;  break;
                case 7: //strcpy(fake_name [bp], "bronze wand");
                icolour [bp] = 6;  break;
                case 8: //strcpy(fake_name [bp], "ivory wand");
                icolour [bp] = LIGHTGRAY; break;
                case 9: //strcpy(fake_name [bp], "glass wand");
                icolour [bp] = 7; break;
                case 10: //strcpy(fake_name [bp], "lead wand");
                icolour [bp] = LIGHTGRAY;  break;
                case 11: //strcpy(fake_name [bp], "plastic wand");
//              icolour [bp] = DARKGRAY;  break;
  icolour [bp] = random3(15) + 1; break;
                } // end of switch(randnum)

                if (item_dam [bp] / 12 == 9) icolour [bp] = 8;

                // rare wands (eg disintegration - these will be very rare): maybe only 1 thing, like: crystal, shining, etc.
                break;



                case 8:
  item_dam [bp] = item_description [1] [item_type [bp]];

  switch(item_dam [bp] % 14)
  {

                //      if (item_type [bp] < 4)
                //{
                //randnum = random3(14);
                //item_dam [bp] = randnum;
                /*switch(randnum)
                {
  */

   case 0: //strcpy(fake_name [bp], "clear potion");
                        icolour [bp] = 7; break;
                        case 1: //strcpy(fake_name [bp], "blue potion");
                        icolour [bp] = BLUE; break;
                        case 2: //strcpy(fake_name [bp], "black potion");
                        icolour [bp] = DARKGRAY;  break;
                        case 3: //strcpy(fake_name [bp], "silvery potion");
                        icolour [bp] = WHITE;  break;
                        case 4: //strcpy(fake_name [bp], "cyan potion");
                        icolour [bp] = CYAN;  break;
                        case 5: //strcpy(fake_name [bp], "purple potion");
                        icolour [bp] = MAGENTA; break;
                        case 6: //strcpy(fake_name [bp], "orange potion");
                        icolour [bp] = LIGHTRED;  break;
                        case 7: //strcpy(fake_name [bp], "inky potion");
                        icolour [bp] = BLUE;  break;
                        case 8: //strcpy(fake_name [bp], "red potion");
                        icolour [bp] = RED; break;
                        case 9: //strcpy(fake_name [bp], "yellow potion");
                        icolour [bp] = YELLOW; break;
                        case 10: //strcpy(fake_name [bp], "green potion");
                        icolour [bp] = GREEN;  break;
                        case 11: //strcpy(fake_name [bp], "brown potion");
                        icolour [bp] = BROWN;  break;
                        case 12: //strcpy(fake_name [bp], "pink potion");
                        icolour [bp] = LIGHTMAGENTA;  break;
                        case 13: //strcpy(fake_name [bp], "white potion");
                        icolour [bp] = WHITE; break;

   }

  break;

        /*                      case 14: //strcpy(fake_name [bp], "potion");
                        icolour [bp] = 7;  break;
                        case 15: //strcpy(fake_name [bp], "potion");
                        icolour [bp] = 7; break;*/
/*
                } // end of switch(randnum)

                } else //ie if it's uncommon:
                {
                secchar = random3(13);
                switch(secchar) // secondary characteristic of uncommon wand
                {
                        case 0: //strcpy(fake_name [bp], "bubbling ");
                        icolour [bp] = 11; break;
                        case 1: //strcpy(fake_name [bp], "lumpy ");
                        icolour [p] = 14; break;
                        case 2: //strcpy(fake_name [p], "fuming ");
                        icolour [p] = 7;  break;
                        case 3: //strcpy(fake_name [p], "smoky ");
                        icolour [p] = 6;  break;
                        case 4: //strcpy(fake_name [p], "fizzy ");
                        icolour [p] = 6;  break;
                        case 5: //strcpy(fake_name [p], "glowing ");
                        icolour [p] = 14; break;
                        case 6: //strcpy(fake_name [p], "sedimented ");
                        icolour [p] = 7;  break;
                        case 7: //strcpy(fake_name [p], "metallic ");
                        icolour [p] = 7;  break;
                        case 8: //strcpy(fake_name [p], "murky ");
                        icolour [p] = 11; break;
                        case 9: //strcpy(fake_name [p], "gluggy ");
                        icolour [p] = 14; break;
                        case 10: //strcpy(fake_name [p], "viscous ");
                        icolour [p] = 7;  break;
                        case 11: //strcpy(fake_name [p], "oily ");
                        icolour [p] = 6;  break;
                        case 12: //strcpy(fake_name [p], "slimy ");
                        icolour [p] = 6;  break;
        /*              case 13: //strcpy(fake_name [p], " ");
                        icolour [p] = 14; break;
                        case 14: //strcpy(fake_name [p], " ");
                        icolour [p] = 7;  break;
                        case 15: //strcpy(fake_name [p], " ");
                        icolour [p] = 7;  break;*/
/*
                } // end of switch(randnum)


                randnum = random3(14);
                switch(randnum)
                {
                        case 0: //strcat(fake_name [p], "clear potion");
                        icolour [p] = 7; break;
                        case 1: //strcat(fake_name [p], "blue potion");
                        icolour [p] = 1; break;
                        case 2: //strcat(fake_name [p], "black potion");
                        icolour [p] = 8;  break;
                        case 3: //strcat(fake_name [p], "silvery potion");
                        icolour [p] = 7;  break;
                        case 4: //strcat(fake_name [p], "cyan potion");
                        icolour [p] = 3;  break;
                        case 5: //strcat(fake_name [p], "purple potion");
                        icolour [p] = 5; break;
                        case 6: //strcat(fake_name [p], "orange potion");
                        icolour [p] = 12;  break;
                        case 7: //strcat(fake_name [p], "inky potion");
                        icolour [p] = 1;  break;
                        case 8: //strcat(fake_name [p], "red potion");
                        icolour [p] = 4; break;
                        case 9: //strcat(fake_name [p], "yellow potion");
                        icolour [p] = 14; break;
                        case 10: //strcat(fake_name [p], "green potion");
                        icolour [p] = 2;  break;
                        case 11: //strcat(fake_name [p], "brown potion");
                        icolour [p] = 6;  break;
                        case 12: //strcat(fake_name [p], "pink potion");
                        icolour [p] = 13;  break;
                        case 13: //strcat(fake_name [p], "white potion");
                        icolour [p] = 15; break;
        /*              case 14: //strcat(fake_name [p], "potion");
                        icolour [p] = 7;  break;
                        case 15: //strcat(fake_name [p], "potion");
                        icolour [p] = 7; break;*/

//      } // end of switch(randnum)
//      item_dam [p] = (randnum * 13) + secchar;

//      } // end of else.

   //item_dam [p] = item_description [1] [item_type [p]];
//      break;

        case 4: // food
        icolour [bp] = 6;
        switch(item_type [bp])
        {
        case 0: icolour [bp] = BROWN; break; //strcpy(fake_name [bp], "meat ration"); break;
        case 1: icolour [bp] = BROWN; break; //strcpy(fake_name [bp], "bread ration"); break;
        case 2: icolour [bp] = LIGHTGREEN; break; //strcpy(fake_name [bp], "pear"); break;
        case 3: switch(random3(2))
                {
                        case 0: icolour [bp] = GREEN; break;
                        case 1: icolour [bp] = RED; break;
                }
        break;
                //strcpy(fake_name [bp], "apple"); break; // make this less common.
        case 4: icolour [bp] = GREEN; break; //strcpy(fake_name [bp], "choko"); break;
        case 5: icolour [bp] = YELLOW; break; //strcpy(fake_name [bp], "honeycomb"); break;
        case 6: icolour [bp] = YELLOW; break; //strcpy(fake_name [bp], "royal jelly"); break;
        case 7: icolour [bp] = GREEN; break; // snozzcumber//strcpy(fake_name [bp], ""); break;
        case 8: icolour [bp] = YELLOW; break; // pizza???
        case 9: icolour [bp] = LIGHTRED; break; // apricot
        case 10: icolour [bp] = LIGHTRED; break; // orange
        case 11: icolour [bp] = YELLOW; break; // banana
        case 12: icolour [bp] = RED; break; // strawberry
        case 13: icolour [bp] = BROWN; break; // rambutan
        case 14: icolour [bp] = YELLOW; break; // lemon
        case 15: switch(random3(2))
                {
                        case 0: icolour [bp] = GREEN; break;
                        case 1: icolour [bp] = MAGENTA; break; // ???
                }
 break; // grape
        case 16: icolour [bp] = BROWN; break; // sultana
        case 17: icolour [bp] = BROWN; break; // lychee
        case 18: icolour [bp] = BROWN; break; // beef jerky
        case 19: icolour [bp] = YELLOW; break; // cheese
        case 20: icolour [bp] = BROWN; break; // saus
//      case 21: icolour [bp] = BROWN; break;
//      case 22: icolour [bp] = BROWN; break;

        //break;
        } break;



        case 7: // jewellery
        icolour [bp] = 14;

/*
 BLACK        ³  0  ³ Yes   Yes
 BLUE         ³  1    Yes ³ Yes
 GREEN        ³  2  ³ Yes   Yes
 CYAN            3    Yes ³ Yes
 RED          ³  4  ³ Yes   Yes
 MAGENTA         5  ³ Yes ³ Yes
 BROWN        ³  6  ³ Yes ³ Yes
 LIGHTGRAY    ³  7  ³ Yes ³ Yes
 DARKGRAY     ³  8    No  ³ Yes
 LIGHTBLUE       9  ³ No  ³ Yes
 LIGHTGREEN   ³ 10  ³ No    Yes
 LIGHTCYAN    ³ 11  ³ No  ³ Yes
 LIGHTRED     ³ 12  ³ No  ³ Yes
 LIGHTMAGENTA   13    No  ³ Yes
 YELLOW       ³ 14  ³ No    Yes
 WHITE        ³ 15  ³ No  ³ Yes
ÄÄ ÄÄÄ Ä    ÄÄÅÄÄ ÄÄÄÄ
*/


 item_dam [bp] = item_description [3] [item_type [bp]];

//      switchnum = 0;

//      if (item_type [bp] < 4) switchnum = item_dam [bp];
//       else
         {
    switchnum = item_dam [bp] % 13;
         }

        switch(switchnum)
        {
        case 0: icolour [bp] = BROWN; break;
        case 1: icolour [bp] = LIGHTGRAY; break;
        case 2: icolour [bp] = YELLOW; break;
        case 3: icolour [bp] = CYAN; break;
        case 4: icolour [bp] = CYAN; break;
        case 5: icolour [bp] = BROWN; break;
        case 6: icolour [bp] = YELLOW; break;
        case 7: icolour [bp] = BROWN; break;
        case 8: icolour [bp] = LIGHTGRAY; break;
        case 9: icolour [bp] = WHITE; break;
        case 10: icolour [bp] = WHITE; break;
        case 11: icolour [bp] = LIGHTGRAY; break;
        case 12: icolour [bp] = GREEN; break;
        case 13: icolour [bp] = LIGHTCYAN; break;
//      case 14: icolour [bp] = BROWN; break;
//      case 15: icolour [bp] = BROWN; break;
        }

        //if (item_dam [bp] < 4)
        //{
        switch(item_dam [bp] / 13)
        {
                case 5: icolour [bp] = DARKGRAY; break;
        }

        //}

        break;


        case 6: // scrolls
        icolour [bp] = LIGHTGRAY;
   item_dam [bp] = item_description [2] [item_type [bp]];
   item_plus [bp] = item_description [4] [item_type [bp]];
//   item_id [bp] = 0;
        break;

 case 10: // books
 switch(item_dam [bp] % 10)
 {
        case 0: //strcat(glog , "paperback book");
        case 1: //strcat(glog , "hardcover book");
  icolour [bp] = random3(15) + 1;
    break;
        case 2: icolour [bp] = DARKGRAY; //strcat(glog , "leatherbound book");
  if (random3(3) == 0) icolour [bp] = BROWN;
    break;
        case 3: icolour [bp] = CYAN; //strcat(glog , "metal-bound book");
    break;
        case 4: icolour [bp] = LIGHTGRAY; //strcat(glog , "papyrus book");
    break;
        case 5: //strcat(glog , " book");
    break;
        case 6: //strcat(glog , " book");
    break;
 }
        break;

 case 11: // staves
 icolour [bp] = BROWN;
 break;

 case 12: // Magical Orbs of Power
 icolour [bp] = LIGHTBLUE;
 break;

                //}

        }

} // end of item_colour



















 /*
void name_item(void)
{
char secchar = 0;

switch(item_class [bp])
        {

        case 0: // weapons
        icolour [bp] = 11;
        if (item_type [bp] > 8) icolour [p] = 6;
        switch(item_type [p])
        {
        case 0: strcpy(fake_name [p], "club");
        icolour [p] = 6;
        break;
        case 1: strcpy(fake_name [p], "mace"); break;
        case 2: strcpy(fake_name [p], "flail"); break;
        case 3: strcpy(fake_name [p], "dagger"); break;
        case 4: strcpy(fake_name [p], "morningstar"); break;
        case 5: strcpy(fake_name [p], "short sword"); break;
        case 6: strcpy(fake_name [p], "long sword"); break;
        case 7: strcpy(fake_name [p], "great sword"); break;
        case 8: strcpy(fake_name [p], "scimitar"); break;
        case 9: strcpy(fake_name [p], "hand axe"); break;
        case 10: strcpy(fake_name [p], "battleaxe"); break;
        case 12: strcpy(fake_name [p], "spear"); break;
        case 13: strcpy(fake_name [p], "halberd"); break;
        case 14: strcpy(fake_name [p], "sling"); icolour [p] = 6;
        break;
        case 15: strcpy(fake_name [p], "bow"); icolour [p] = 6;
        break;
        case 16: strcpy(fake_name [p], "crossbow"); icolour [p] = 6;
        break;
        }
        break;

        case 1: // ammunition
        icolour [p] = 11;
        switch(item_type [p])
        {
        case 0: strcpy(fake_name [p], "stone"); icolour [p] = 6;
        break;
        case 1: strcpy(fake_name [p], "arrow"); break;
        case 2: strcpy(fake_name [p], "bolt"); break;
        case 3: strcpy(fake_name [p], "dart"); break;
        case 4: strcpy(fake_name [p], ""); break;
        case 5: strcpy(fake_name [p], ""); break;
        case 6: strcpy(fake_name [p], ""); break;
        case 7: strcpy(fake_name [p], ""); break;
        case 8: strcpy(fake_name [p], ""); break;
        case 9: strcpy(fake_name [p], ""); break;
        case 10: strcpy(fake_name [p], ""); break;
        case 12: strcpy(fake_name [p], ""); break;
        case 13: strcpy(fake_name [p], ""); break;
        case 14: strcpy(fake_name [p], ""); break;
        case 15: strcpy(fake_name [p], ""); break;
        case 16: strcpy(fake_name [p], ""); break;
        } break;


        //break;



        case 2: // armour
        icolour [p] = 11;
        switch(item_type [p])
        {
        case 0: strcpy(fake_name [p], "robe");
        icolour [p] = random3(15) + 1;
        case 1: strcpy(fake_name [p], "leather armour");
        icolour [p] = 6;
        break;
        case 2: strcpy(fake_name [p], "ring mail"); break;
        case 3: strcpy(fake_name [p], "scale mail"); break;
        case 4: strcpy(fake_name [p], "chain mail"); break;
        case 5: strcpy(fake_name [p], "splint mail"); break;
        case 6: strcpy(fake_name [p], "banded mail"); break;
        case 7: strcpy(fake_name [p], "plate mail"); break;
        case 8: strcpy(fake_name [p], "shield"); break;
        case 9: strcpy(fake_name [p], "cloak");
        icolour [p] = random3(15) + 1;
        break;
        case 10: strcpy(fake_name [p], "helmet"); break;
        case 11: strcpy(fake_name [p], "pair of gloves");
        icolour [p] = 6;
        break;
        case 12: strcpy(fake_name [p], "pair of boots");
        icolour [p] = 6;
        break;
        } break;


        case 3: // devices
        //icolour [p] = 12;
        //switch(item_type [p])
        //{
        /*case 0: strcpy(fake_name [p], "wand of fire"); break;
        case 1: strcpy(fake_name [p], "wand of cold"); break;
        case 2: strcpy(fake_name [p], "wand of slowing"); break;
        case 3: strcpy(fake_name [p], "wand of magic missile"); break;
        case 4: strcpy(fake_name [p], "wand of healing"); break;
        case 5: strcpy(fake_name [p], "wand of paralysis"); break;
        case 6: strcpy(fake_name [p], ""); break;
        case 7: strcpy(fake_name [p], ""); break;*/
        // Common wands: note: fire & cold will be uncommon, and will be replaced by burning and chilling
        // Lightning will always be blackened
        // If the wand is common:
//      break;

//      case 8: // potions
//      icolour [p] = 13;
/*      switch(item_type [p])
        {
        case 0: strcpy(fake_name [p], "potion of healing"); break;
        case 1: strcpy(fake_name [p], "potion of extra healing"); break;
        case 2: strcpy(fake_name [p], "potion of speed"); break;
        case 3: strcpy(fake_name [p], "potion of might"); break;
        case 4: strcpy(fake_name [p], "potion of strength"); break;
        case 5: strcpy(fake_name [p], "potion of dexterity"); break;
        case 6: strcpy(fake_name [p], "potion of intelligence"); break;
        case 7: strcpy(fake_name [p], "poison"); break;
        }
   //    * /
break;





        case 6: // scrolls
        icolour [p] = 7;
        switch(item_type [p])
        {
        case 0: strcpy(fake_name [p], "of identify"); break;
        case 1: strcpy(fake_name [p], "of teleportation"); break;
        case 2: strcpy(fake_name [p], "of fear"); break;
        case 3: strcpy(fake_name [p], "of noise"); break;
        case 4: strcpy(fake_name [p], "of remove curse"); break;
        case 5: strcpy(fake_name [p], "of magic missile"); break;
        case 6: strcpy(fake_name [p], ""); break;
        case 7: strcpy(fake_name [p], ""); break;
        }



                break;
        }


} // end of name_item()
*/


void place_traps(unsigned char grid [80] [70], unsigned char trap_type [NTRAPS],
 unsigned char trap_x [NTRAPS], unsigned char trap_y [NTRAPS])
{

int num_traps = random3(12) + random3(2);
//unsigned char trap_alloc [5];

for (bi = 0; bi < num_traps; bi++)
{

do
{
        trap_x [bi] = random3(60) + 10;
        trap_y [bi] = random3(50) + 10;
} while (grid [trap_x [bi]] [trap_y [bi]] != 17);
/*
if (many_many < 4)
{
        trap_alloc [0] = 0;
        trap_alloc [1] = 0;
        trap_alloc [2] = 0;
        trap_alloc [3] = 0;
        trap_alloc [4] = 0;
//      cout << " < 4";
//      keyin = getch();
}

if (many_many > 3)
{
        trap_alloc [0] = 0;
        trap_alloc [1] = 1;
        trap_alloc [2] = 2;
        trap_alloc [3] = 3;
        trap_alloc [4] = 1;
//      cout << " > 3";
//      keyin = getch();
}
*/
trap_type [bi] = 0;

if (random3(many_many) > 1) trap_type [bi] = 1;
if (random3(many_many) > 3) trap_type [bi] = 2;
if (random3(many_many) > 5) trap_type [bi] = 1;
if (random3(20) == 0) trap_type [bi] = 4;
if (random3(40) == 0) trap_type [bi] = 5;

//trap_type [bi] = trap_alloc [random3(5)];
grid [trap_x [bi]] [trap_y [bi]] = 28;

} // end of for i

}


void link_items(int mons_inv [MNST] [8], unsigned char item_class [ITEMS], char unique_items [50], unsigned char item_x [ITEMS],
 unsigned char item_x [ITEMS], unsigned char item_y [ITEMS],
 int item_link [ITEMS], int igrid [80] [70], int item_quant [ITEMS])
{

//cout << "Starting link...";

//return;

int b = 0;
int n = 0;
int m = 0;

for (bi = 0; bi < ITEMS; bi ++)
{
        if (item_class [bi] == 100 | item_quant [bi] == 0 | item_x [bi] == 1) continue;

   //if (item_quant [bi] == 0) continue;

        if (igrid [item_x [bi]] [item_y [bi]] == 501)
        {
                igrid [item_x [bi]] [item_y [bi]] = bi;
                //cout << i << ", ";
                //char groth = getch();
                continue;
        } else
        {
        //b = item_link [igrid [item_x [bi]] [item_y [bi]]];
        b = igrid [item_x [bi]] [item_y [bi]];

        while(b != 501)
        {
                m = b;
                n = item_link [b];
                b = n;
        } // end of while

        item_link [m] = bi;

        //cout << "\n\rLinking " << i << " to " << m;
//      char gtot = getch();
        }
} // end of for i

for (bi = 0; bi < MNST; bi ++)
{
        for (bj = 7; bj >= 0; bj --)
        {
      if (mons_inv [bi] [bj] == 501) continue;
      if (igrid [item_x [mons_inv [bi] [bj]]] [item_y [mons_inv [bi] [bj]]] == mons_inv [bi] [bj])
      {
         igrid [item_x [mons_inv [bi] [bj]]] [item_y [mons_inv [bi] [bj]]] = item_link [mons_inv [bi] [bj]];
      }
 }
}

//cout << "Finishing linking....";

}



void roguey_level(
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
unsigned char grid [80] [70],
unsigned char stair_x [10],
unsigned char stair_y [10])
{

unsigned char rox1 [30];
unsigned char rox2 [30];
unsigned char roy1 [30];
unsigned char roy2 [30];
int cn = 0;

for (bcount_y = 0; bcount_y < 5; bcount_y ++)
{

for (bcount_x = 0; bcount_x < 5; bcount_x ++)
{

// rooms:

        rox1 [cn] = bcount_x * 13 + random3(4) + 8;
        roy1 [cn] = bcount_y * 11 + random3(4) + 8;

        rox2 [cn] = rox1 [cn] + random3(8) + 3;
        roy2 [cn] = roy1 [cn] + random3(6) + 3;

        cn++;

} // end of for bcount_y

} // end of for bcount_x

cn = 0;

for (bi = 0; bi < 25; bi ++)
{
        for (bcount_x = rox1 [bi]; bcount_x < rox2 [bi]; bcount_x ++)
        {
                for (bcount_y = roy1 [bi]; bcount_y < roy2 [bi]; bcount_y ++)
                {
                        if (grid [bcount_x] [bcount_y] == 1 && bcount_x < 72);
                        grid [bcount_x] [bcount_y] = 17;
                } // end of for bcount_y
        } // end of for bcount_x
 if (rox2 [bi] - rox1 [bi] > 5 && roy2 [bi] - roy1 [bi] > 5)
   if (random3(100 - many_many) < 3)
   {
                                        if (random3(4) != 0) box_room(rox1 [bi] + 2, rox2 [bi] - 2, roy1 [bi] + 2, roy2 [bi] - 2, grid, random3(2) + 1);
           else box_room(rox1 [bi] + 2, rox2 [bi] - 2, roy1 [bi] + 2, roy2 [bi] - 2, grid, 4);
   }
} // end of for i


// Now, join them together:

char pos [2];
char jpos [2];

char doing = 0;

char last_room = 0;


for (bp = 0; bp < 2; bp ++)
{

for (bi = 0; bi < 25; bi ++)
{
        if (bp == 0 && (bi % 5 == 0 | bi == 0)) continue;

        if (bp == 1 && bi < 5) continue;

        switch(bp)
        {
                case 0:
                last_room = bi - 1;
                pos [0]  = rox1 [bi]; // - 1;
                pos [1]  = roy1 [bi] + random3(roy2 [bi] - roy1 [bi]);

                jpos [0] = rox2 [last_room]; // + 1;
                jpos [1] = roy1 [last_room] + random3(roy2 [last_room] - roy1 [last_room]);
                break;

                case 1:
                last_room = bi - 5;
                pos [1]  = roy1 [bi]; // - 1;
                pos [0]  = rox1 [bi] + random3(rox2 [bi] - rox1 [bi]);

                jpos [1] = roy2 [last_room]; // + 1;
                jpos [0] = rox1 [last_room] + random3(rox2 [last_room] - rox1 [last_room]);
                break;

        } // end of switch p

        while(pos [0] != jpos [0] | pos [1] != jpos [1])
        {
                doing = random3(2);

/*              if (bp == 0)
                {
                        if (pos [1] == jpos [1] + 1 && pos [0] != jpos [0]) doing = 0;
                }*/

                if (pos [doing] < jpos [doing]) pos [doing]++;
                if (pos [doing] > jpos [doing]) pos [doing]--;

                if (grid [pos [0]] [pos [1]] == 1) grid [pos [0]] [pos [1]] = 17;

        } // end of while

        //if (random3(2) == 0 )

//        if (jpos [0

        if ((grid [pos [0] + 1] [pos [1]] == 1 && grid [pos [0] - 1] [pos [1]] == 1) | (grid [pos [0]] [pos [1] + 1] == 1 && grid [pos [0]] [pos [1] - 1] == 1))
        {
                if (grid [pos [0]] [pos [1]] == 17) grid [pos [0]] [pos [1]] = 103;
         //else grid [pos [0]] [pos [1]] = 17;
        }



} // end of for i

} // end of for p


if (many_many > 8 && random3(15) == 0)
//if (random3(2) == 0)
        {

                int spec_room_done = random3(25);
                x1 = rox1 [spec_room_done];
                x2 = rox2 [spec_room_done];
                y1 = roy1 [spec_room_done];
                y2 = roy2 [spec_room_done];
                /*/beehive(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_x, mons_y,    mons_beh,       mons_inv,
 mons_targ_1_x, mons_targ_1_y,  mons_sec, mons_ench_1, mons_ench,
no_it, item_ident, item_class, item_type, item_plus, item_dam, item_quant,
item_x, item_y, icolour, xtra_quant, item_link, item_description, property,
trap_type, trap_x, trap_y, mons_alloc, grid);*/
  morgue(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_x, mons_y,    mons_beh,       mons_inv,
 mons_targ_1_x, mons_targ_1_y,  mons_sec, mons_ench_1, mons_ench,
no_it, item_ident, item_class, unique_items, item_type, item_plus, item_dam, item_quant,
item_x, item_y, icolour, xtra_quant, item_link, item_description, property,
trap_type, trap_x, trap_y, mons_alloc, grid);

                         is_a_specroom = 2; // ????????????????????????????????????????????????
               // should this be here?

                for (bcount_x = x1 - 1; bcount_x < x2; bcount_x++)
                {
                        if (grid [bcount_x] [y1 - 1] == 1) grid [bcount_x] [y1 - 1] = 105;
                        if (grid [bcount_x] [y1 - 1] == 17 | grid [x1 - 1] [bcount_y] == 3) grid [bcount_x] [y1 - 1] = 117;
                        //grid [bcount_x] [y1 - 1] = 105;
                        //grid [bcount_x] [y2] = 105;
                        if (grid [bcount_x] [y2] == 1) grid [bcount_x] [y2] = 105;
                        if (grid [bcount_x] [y2] == 17 | grid [x1 - 1] [bcount_y] == 3) grid [bcount_x] [y2] = 117;

                }

                for (bcount_y = y1 - 1; bcount_y < y2; bcount_y++)
                {
                        if (grid [x1 - 1] [bcount_y] == 1) grid [x1 - 1] [bcount_y] = 105;
                        if (grid [x1 - 1] [bcount_y] == 17 | grid [x1 - 1] [bcount_y] == 3) grid [x1 - 1] [bcount_y] = 117;
                        //grid [x1 - 1] [bcount_y] = 105;
                        if (grid [x2] [bcount_y] == 1) grid [x2] [bcount_y] = 105;
                        if (grid [x2] [bcount_y] == 17 | grid [x2] [bcount_y] == 3) grid [x2] [bcount_y] = 117;
                        //grid [x2] [bcount_y] = 105;

                }

                if (grid [x2] [y2] == 1) grid [x2] [y2] = 105;

        }



//if (skipped == 0) return;

for (bj = 0; bj < 3 + random3(3); bj ++)
{
 for (bi = 18; bi < 20; bi ++)
 {
        if (bi == 20) break; // ?

        do
        {
                pos [0] = random3(60) + 10;
                pos [1] = random3(50) + 10;
        } while (grid [pos [0]] [pos [1]] != 17);

        grid [pos[0]] [pos [1]] = 18; //bi;
  stair_x [bj] = pos [0];
  stair_y [bj] = pos [1];

 } // end for i
} // end for j


} // end of void roguey_level()






void city_level(
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
unsigned char grid [80] [70],
unsigned char stair_x [10],
unsigned char stair_y [10],
int igrid [80] [70]
)
{
int wall_type = 0; // remember, can have many wall types in one level
int xs = 0;
int ys = 0;
int x1 = 0;
int x2 = 0;
int y1 = 0;
int y2 = 0;
char pos [2];
char jpos [2];

if (random3(4) != 0)
{
  wall_type = 1 + random3(2);
} else wall_type = 4;

if (many_many == 60) wall_type = 4;

for (bcount_x = 7; bcount_x < 73; bcount_x ++)
{
        for (bcount_y = 7; bcount_y < 63; bcount_y ++)
        {
                grid [bcount_x] [bcount_y] = 17;
                igrid [bcount_x] [bcount_y] = 501;
        }
}

for (bi = 0; bi < 5; bi ++)
{
 for (bj = 0; bj < 4; bj ++)
 {

 xs = (bi * 13) + 8;
 ys = (bj * 14) + 8;
 x1 = xs + random3(3) + random3(3);
 y1 = ys + random3(3) + random3(3);
 x2 = xs + 11 - random3(3) - random3(3);
 y2 = ys + 12 - random3(3) - random3(3);

 if (random3(7) != 0)
 {
  if (random3(10) != 0) box_room(x1, x2, y1, y2, grid, wall_type);
        else if (random3(4) != 0) box_room(x1, x2, y1, y2, grid, random3(2) + 1);
              else box_room(x1, x2, y1, y2, grid, 4);
  // if (random3(4) == 0 && x2 - x1 > 2 && y2 - y1 > 2) box_room(x1 + (2 + random3(x2 - x1 / 2)), x2 - (2 + random3(x2 - x1 / 2)), y1 + (2 + random3(y2 - y1 / 2)), y2 - (2 + random3(y2 - y1 / 2)), grid, wall_type);
  if (random3(8) == 0 && x2 - x1 > 2 && y2 - y1 > 2) box_room(x1 + 2, x2 - 2, y1 + 2, y2 - 2, grid, wall_type);
 }
 //if (random3(1) == 0) box_room(x1 + 2, x2 - 2, y1 + 2, y2 - 2, grid, wall_type);
 } // end for j
} // end for i

//if (skipped == 0) return;

for (bj = 0; bj < 1 + random3(3); bj ++)
{
 for (bi = 18; bi < 20; bi ++)
 {
        if (bi == 20) break; // ?

        do
        {
                pos [0] = random3(60) + 10;
                pos [1] = random3(50) + 10;
        } while (grid [pos [0]] [pos [1]] != 17);

        grid [pos[0]] [pos [1]] = 18;//bi;
  stair_x [bj] = pos [0];
  stair_y [bj] = pos [1];

 } // end for i
} // end for j



} // end city_level



void box_room(int bx1, int bx2, int by1, int by2, unsigned char grid [80] [70], int wall_type)
{

 for (bcount_x = bx1; bcount_x < bx2 + 1; bcount_x ++)
 {
  grid [bcount_x] [by1] = wall_type;
  grid [bcount_x] [by2] = wall_type;
 } // end bcount_x

 for (bcount_y = by1; bcount_y < by2; bcount_y ++)
 {
  grid [bx1] [bcount_y] = wall_type;
  grid [bx2] [bcount_y] = wall_type;
 } // end bcount_y

if (random3(2) == 0)
{
 if (random3(2) == 0)
 {
  grid [bx1] [by1 + random3(by2 - by1 + 1)] = 3;
 } // end if random
  else grid [bx2] [by1 + random3(by2 - by1 + 1)] = 3;

} // end if random
 else if (random3(2) == 0) grid [bx1 + random3(bx2 - bx1 + 1)] [by1] = 3;
         else grid [bx1 + random3(bx2 - bx1 + 1)] [by2] = 3;

if (random3(3) == 0)
{
 if (random3(2) == 0)
 {
  grid [bx1] [by1 + random3(by2 - by1 + 1)] = 3;
 } // end if random
  else grid [bx2] [by1 + random3(by2 - by1 + 1)] = 3;
} // end if random3(3)

if (random3(3))
{
if (random3(2) == 0) grid [bx1 + random3(by2 - by1 + 1)] [by1] = 3;
         else grid [bx1 + random3(bx2 - bx1 + 1)] [by2] = 3;
}

} // end box_room










int random3(unsigned int rmax)
{
        //static int ugly;

//      return random(rmax); // BC

        if (rmax == 0) return 0;

        return (random() % rmax); // DJGPP

        //ugly ++;

        //if (rmax != 0) return ugly % rmax;

        //return 0;

}



void check_doors(unsigned char grid [80] [70])
{
        char ig;

        for (bcount_x = 0; bcount_x < 80; bcount_x ++)
        {
                for (bcount_y = 0; bcount_y < 70; bcount_y ++)
                {
                        ig = grid [bcount_x] [bcount_y];

                        if (ig != 3 && ig != 103 && ig != 33) continue;



                        if (((grid [bcount_x - 1] [bcount_y] < 10) + (grid [bcount_x + 1] [bcount_y] < 10) + (grid [bcount_x] [bcount_y - 1] < 10) + (grid [bcount_x] [bcount_y + 1] < 10)) < 2)
                                grid [bcount_x] [bcount_y] = 17;
                                        else grid [bcount_x] [bcount_y] = 3;

//                      if ((grid [pos [0] + 1] [pos [1]] == 1 && grid [pos [0] - 1] [pos [1]] == 1) | (grid [pos [0]] [pos [1] + 1] == 1 && grid [pos [0]] [pos [1] - 1] == 1))
                }
        }
}





int xmons_zombie_size(int mclass)
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

return 2; // large

// creatures which can't become zombies return 0:
case 3: // devil
case 5: // fungus
case 8: // imp
case 13: // necr
case 16: // quasit
case 19: // tunneling worm
case 56: // t#
case 21: // vortex
case 23: // abomination??
case 24: // yellow light!!!
case 26: // angel
case 31: // fiend
case 32: // giant spore
case 34: // I?????????????????
case 35: // jelly
case 37: // lich
case 38: // mummy
case 41: // plant
case 43: // Raks
case 44: // S?????????????
case 46: // U
case 47: // Vampire
case 48: // Wraith
case 49: // abomination???

// and, of course:
case 25: // z
case 51: // Z
return 0;

}

return 1; // small

} // end of int xmons_zombie size



void spellbook_template(int sbook_type, int func_pass [10])
{

switch(sbook_type)
{

  case 0: // beginning spellbook 1
       func_pass [0] = 0;
       func_pass [1] = 5; // magic miss
       func_pass [2] = 26; // th fl
       func_pass [3] = 49; // summon sm mam
       func_pass [4] = 59; // blink
       func_pass [5] = 13; // conf fl
       func_pass [6] = 15; // fire bolt
  return;

  case 1: // beginning spellbook 2
       func_pass [0] = 0;
       func_pass [1] = 5; // magic miss
       func_pass [2] = 27; // th fro
       func_pass [3] = 49; // summon sm mam
       func_pass [4] = 59; // blink
       func_pass [5] = 21; // slow
       func_pass [6] = 16; // cold bolt
  return;

  case 2: // beginning spellbook 3
       func_pass [0] = 0;
       func_pass [1] = 5; // magic miss
       func_pass [2] = 49; // summ sm m
       func_pass [3] = 2; // fear
       func_pass [4] = 59; // blink
       func_pass [5] = 24; // conf
       func_pass [6] = 30; // meph cloud
  return;

  case 3: // beginning spellbook 4
       func_pass [0] = 0;
       func_pass [1] = 5; // magic miss
       func_pass [2] = 13; // conj fl
       func_pass [3] = 21; // slow
       func_pass [4] = 59; // blink
       func_pass [5] = 15; // bolt of fire
       func_pass [6] = 30; // meph cloud
  return;

// these are all the same as case 3: so far:

  case 4: // beginning spellbook 4
       func_pass [0] = 0;
       func_pass [1] = 5; // magic miss
       func_pass [2] = 13; // conj fl
       func_pass [3] = 21; // slow
       func_pass [4] = 59; // blink
       func_pass [5] = 15; // bolt of fire
       func_pass [6] = 30; // meph cloud
  return;

  case 5: // beginning spellbook 4
       func_pass [0] = 0;
       func_pass [1] = 5; // magic miss
       func_pass [2] = 13; // conj fl
       func_pass [3] = 21; // slow
       func_pass [4] = 59; // blink
       func_pass [5] = 15; // bolt of fire
       func_pass [6] = 30; // meph cloud
  return;

  case 6: // beginning spellbook 4
       func_pass [0] = 0;
       func_pass [1] = 5; // magic miss
       func_pass [2] = 13; // conj fl
       func_pass [3] = 21; // slow
       func_pass [4] = 59; // blink
       func_pass [5] = 15; // bolt of fire
       func_pass [6] = 30; // meph cloud
  return;


  case 7: // book of summonings
       func_pass [0] = 0;
       func_pass [1] = 49; // summ sm an
       func_pass [2] = 50; // abjur
       func_pass [3] = 51; // sum sco
       func_pass [4] = 61; // swarm
       func_pass [5] = 62; // thing
       func_pass [6] = 120; // NEED SOMETHINGS ELSE!
  break;

 case 8: // book of fire
       func_pass [0] = 0;
       func_pass [1] = 26; // thr fl
       func_pass [2] = 13; // conj fl
       func_pass [3] = 15; // fir bolt
       func_pass [4] = 6; // fireball
       func_pass [5] = 31; // r of fl
       func_pass [6] = 57; // fire storm
  break;

 case 9: // book of ice
       func_pass [0] = 0;
       func_pass [1] = 27; // thr frost
       func_pass [2] = 16; // frost bolt
       func_pass [3] = 29; // freez cl
       func_pass [4] = 120; //
       func_pass [5] = 120; //
       func_pass [6] = 120; //
  break;

 case 10: // book of divinations
       func_pass [0] = 0;
       func_pass [1] = 58; // det trap
       func_pass [2] = 48; // det curse
       func_pass [3] = 64; // magic mapping
       func_pass [4] = 0; // id
       func_pass [5] = 120; //
       func_pass [6] = 120; //
  break;

 case 11: // book of spatial translocation
       func_pass [0] = 0;
       func_pass [1] = 59; // blink
       func_pass [2] = 28; // controlled bl
       func_pass [3] = 1; // tel self
       func_pass [4] = 37; // tel other
       func_pass [5] = 120; //
       func_pass [6] = 120; //
  break;

 case 12: // book of enchantment
       func_pass [0] = 0;
       func_pass [1] = 21; // slow
       func_pass [2] = 22; // haste
       func_pass [3] = 23; // paral
       func_pass [4] = 24; // conf
       func_pass [5] = 52; // levit
       func_pass [6] = 2; // fear
  break;

 case 13: // book of poison
       func_pass [0] = 0;
       func_pass [1] = 30; // meph cl
       func_pass [2] = 35; // venom bolt
       func_pass [3] = 36; // olg tox rad
       func_pass [4] = 40; // cure pois
       func_pass [5] = 56; // pois cloud
       func_pass [6] = 120; //
  break;

 case 14: // book of greater conjurations
       func_pass [0] = 0;
       func_pass [1] = 17; // thunderbolt
       func_pass [2] = 6; // fireball
       func_pass [3] = 54; // throw splint
       func_pass [4] = 55; // bolt of inn
       func_pass [5] = 60; // myst bolt
       func_pass [6] = 57; // fire storm
  break;

 case 15: // book of necromancy - not much here yet (just wait until I do monster corpses!)
       func_pass [0] = 0;
       func_pass [1] = 53; // bolt of draining
       func_pass [2] = 42; // death's door
       func_pass [3] = 120; //
       func_pass [4] = 120; //
       func_pass [5] = 120; //
       func_pass [6] = 120; //
  break;

 case 16: // book of enchantment - another one
       func_pass [0] = 0;
       func_pass [1] = 21; // slow
       func_pass [2] = 63; // enslave
       func_pass [3] = 23; // paral
       func_pass [4] = 24; // conf
       func_pass [5] = 25; // invis
       func_pass [6] = 44; // mass confusion
  break;

 case 17: // book of holy invocations
       func_pass [1] = 46; // repel undead
       func_pass [2] = 4; // remove curse
       func_pass [3] = 45; // smiting
       func_pass [4] = 41; // purificat
       func_pass [5] = 47; // holy word
       func_pass [6] = 120; //
  break;

 case 18: // book of healing
       func_pass [0] = 0;
       func_pass [1] = 32; // rest st
       func_pass [2] = 33; // int
       func_pass [3] = 34; // dex
       func_pass [4] = 38; // l heal
       func_pass [5] = 39; // g heal
       func_pass [6] = 41; // purification
  break;

  case 19: // useful magic
       func_pass [0] = 0;
       func_pass [1] = 4; // remove curse
       func_pass [2] = 14; // dig
       func_pass [3] = 48; // detect curse
       func_pass [4] = 43; // selective amn
       func_pass [5] = 38; // less healing
       func_pass [6] = 3; // noise - but this isn't very useful???
  break;

  case 20: // prayers
       func_pass [0] = 0;
       func_pass [1] = 48; // detect curse
       func_pass [2] = 46; // repel undead
       func_pass [3] = 38; // less healing
       func_pass [4] = 40; // cure poison
       func_pass [5] = 120; //
       func_pass [6] = 120; //
  break;

  case 21: // clouds?
       func_pass [0] = 0;
       func_pass [1] = 30; // mephitic cloud
       func_pass [2] = 13; // conjure flame
       func_pass [3] = 29; // freezing cloud
       func_pass [4] = 56; // poison cloud
       func_pass [5] = 120; //
       func_pass [6] = 120; //
  break;


  // 50+ are magical staves
  case 50: // staff of smiting
       func_pass [0] = 0;
       func_pass [1] = 45; // smiting
       func_pass [2] = 120; //
       func_pass [3] = 120; //
       func_pass [4] = 120; //
       func_pass [5] = 120; //
       func_pass [6] = 120; //
  break;

  case 51: // staff of summoning
       func_pass [0] = 0;
       func_pass [1] = 49; // summon small animals
       func_pass [2] = 51; // summon scorpions
       func_pass [3] = 61; // swarm
       func_pass [4] = 120; //
       func_pass [5] = 120; //
       func_pass [6] = 120; //
  break;

  case 52: // staff of destruction
       func_pass [0] = 0;
       func_pass [1] = 26; // throw fire
       func_pass [2] = 15; // firebolt
       func_pass [3] = 6; // fireball
       func_pass [4] = 120; //
       func_pass [5] = 120; //
       func_pass [6] = 120; //
  break;

  case 53: // staff of destruction
       func_pass [0] = 0;
       func_pass [1] = 27; // throw frost
       func_pass [2] = 16; // frostbolt
       func_pass [3] = 17; // thunderbolt
       func_pass [4] = 120; //
       func_pass [5] = 120; //
       func_pass [6] = 120; //
  break;

  case 54: // staff of destruction
       func_pass [0] = 0;
       func_pass [1] = 6; // fireball
       func_pass [2] = 17; // thunderbolt
       func_pass [3] = 54; // throw splinters
       func_pass [4] = 120; //
       func_pass [5] = 120; //
       func_pass [6] = 120; //
  break;

  case 55: // staff of destruction
       func_pass [0] = 0;
       func_pass [1] = 15; // frost bolt
       func_pass [2] = 16; // firebolt
       func_pass [3] = 55; // bolt of inaccuracy
       func_pass [4] = 120; //
       func_pass [5] = 120; //
       func_pass [6] = 120; //
  break;

  case 56: // staff of warding
       func_pass [0] = 0;
       func_pass [1] = 46; // repel undead
       func_pass [2] = 2; // cause fear
       func_pass [3] = 50; // abjuration
       func_pass [4] = 120; //
       func_pass [5] = 120; //
       func_pass [6] = 120; //
  break;

  case 57: // staff of exploration
       func_pass [0] = 0;
       func_pass [1] = 58; // detect traps
       func_pass [2] = 64; // magic mapping
       func_pass [3] = 120; //
       func_pass [4] = 120; //
       func_pass [5] = 120; //
       func_pass [6] = 120; //
  break;


}

// only 1 to 6 are relevant

// all values must be filled, or at least here.

}

/*
        case 0: strcpy(spln, "Identify"); break;
                case 1: strcpy(spln, "Teleport Self"); break;
                case 2: strcpy(spln, "Cause Fear"); break;
                case 3: strcpy(spln, "Create Noise"); break;
                case 4: strcpy(spln, "Remove Curse"); break;
                case 5: strcpy(spln, "Magic Missile"); break;
                case 6: strcpy(spln, "Fireball"); break;
                case 13: strcpy(spln, "Conjure Flame"); break;
      case 14: strcpy(spln, "Dig"); break;
      case 15: strcpy(spln, "Bolt of Fire"); break;
      case 16: strcpy(spln, "Bolt of Cold"); break;
      case 17: strcpy(spln, "Thunderbolt"); break;
      case 20: strcpy(spln, "Polymorph Other"); break;
      case 21: strcpy(spln, "Slow"); break;
      case 22: strcpy(spln, "Haste"); break;
      case 23: strcpy(spln, "Paralyze"); break;
      case 24: strcpy(spln, "Confuse"); break;
      case 25: strcpy(spln, "Invisibility"); break;
      case 26: strcpy(spln, "Throw Flame"); break;
      case 27: strcpy(spln, "Throw Frost"); break;
      case 28: strcpy(spln, "Controlled Blink"); break;
      case 29: strcpy(spln, "Freezing Cloud"); break;
      case 30: strcpy(spln, "Mephitic Cloud"); break;
      case 31: strcpy(spln, "Ring of Flames"); break;
      case 32: strcpy(spln, "Restore Strength"); break;
      case 33: strcpy(spln, "Restore Intelligence"); break;
      case 34: strcpy(spln, "Restore Dexterity"); break;
      case 35: strcpy(spln, "Venom Bolt"); break;
      case 36: strcpy(spln, "Olgreb's Toxic Radiance"); break;
      case 37: strcpy(spln, "Teleport Creature"); break;
      case 38: strcpy(spln, "Lesser Healing"); break;
      case 39: strcpy(spln, "Greater Healing"); break;
      case 40: strcpy(spln, "Cure Poison"); break;
      case 41: strcpy(spln, "Purification"); break;
      case 42: strcpy(spln, "Death's Door"); break;
      case 43: strcpy(spln, "Selective Amnesia"); break;
      case 44: strcpy(spln, "Mass Confusion"); break;
      case 45: strcpy(spln, "Smiting"); break;
                                                case 46: strcpy(spln, "Repel Undead"); break;
                                                case 47: strcpy(spln, "Holy Word"); break;
      case 48: strcpy(spln, "Detect Curse"); break;
      case 49: strcpy(spln, "Summon Small Mammal"); break;
      case 50: strcpy(spln, "Abjuration"); break;
      case 51: strcpy(spln, "Summon Scorpions"); break;
      case 52: strcpy(spln, "Levitation"); break;
      case 53: strcpy(spln, "Bolt of Draining"); break;
      case 54: strcpy(spln, "Throw Splinters"); break;
      case 55: strcpy(spln, "Bolt of Inaccuracy"); break;
                                          // spelling!
      case 56: strcpy(spln, "Poisonous Cloud"); break;
      case 57: strcpy(spln, "Fire Storm"); break;
      case 58: strcpy(spln, "Detect Traps"); break;
      case 59: strcpy(spln, "Blink"); break;
      case 60: strcpy(spln, "Iskenderun's Mystic Blast"); break;
      case 61: strcpy(spln, "Create Swarm"); break;
      case 62: strcpy(spln, "Create Horrible Thing");
      case 63: enslavement
      case 64: magic mapping
*/


void labyrinth_level(
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
unsigned char grid [80] [70],
unsigned char stair_x [10],
unsigned char stair_y [10],
unsigned char mons_hit [MNST],
char gmon_use [400],
int igrid [80] [70],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS])
{
int lx = 0;
int ly = 0;
int keep_lx = 0;
int keep_ly = 0;
int keep_lx2 = 0;
int keep_ly2 = 0;
char do_2 = 0;
int clear_space = 1;
char start_point_x = 10;
char start_point_y = 10;
char going_x = 1;
char going_y = 0;
char traps_put = 0;
unsigned char traps_put2 = 0;
//cprintf("hhh");

char gk [5];

if (random3(2) == 0) going_x = 1; else going_y = 1;

if (random3(2) == 0)
{
 start_point_x = 70;
 if (going_x == 1) going_x = -1;
}
if (random3(2) == 0)
{
 start_point_y = 60;
 if (going_y == 1) going_y = -1;
}

lx = start_point_x;
ly = start_point_y;

grid [lx] [ly] = 18;

if (going_y != 0) goto do_y;

do_x :// if (going_x != 0)
//{
traps_put2 = 0;
clear_space = 0;//random3(2) + 2;
do
 {
  lx += going_x;
//  itoa(lx, gk, 10);
//  cprintf(gk);
//  cprintf("x");
     if (grid [lx] [ly] == 1) grid [lx] [ly] = 17;
//  cprintf(",");
//  getch();
//if (random3(1000) == 0) goto finishing;
  if (do_2 == 1 && random3(300) == 0 && traps_put2 == 0 && traps_put < 20)
  {
   place_specific_trap(grid, trap_type, trap_x, trap_y, lx,
     ly, 4);
   traps_put++;
   traps_put2++;
  }
 }
 while (grid [lx + going_x * (2 + clear_space)] [ly] == 1 && lx < 72 && lx > 8);
//}
//cprintf("z");
//getch();

doing_y : going_x = 0;

if (ly < 32) going_y = 1;
if (ly > 37) going_y = -1;
if (ly >= 32 && ly <= 37)
{
// cprintf("X");
 goto finishing;
}


do_y :// if (going_y != 0)
//{
//clear_space = random3(2) + 2;
//if (random3(2) == 0)
if (do_2 == 1)
{
lx = keep_lx2;
ly = keep_ly2;
}

clear_space = 1;
// do_2 = 0 is the problem
    if (random3(2) == 0)
    {
     clear_space = 0;
     do_2 = 0;
    } else
          {
           clear_space = 2; //2;
           do_2 = 1;
          }
do
 {
  ly += going_y;
//  itoa(ly, gk, 10);
//  cprintf(gk);
//  cprintf("y");
//  getch();
     if (grid [lx] [ly] == 1) grid [lx] [ly] = 17;
//  cprintf(",");
//  if (random3(1000) == 0)  goto finishing;
 }
 while (grid [lx] [ly + going_y * (2 + clear_space)] == 1 && ly < 62 && ly > 8);
//}
keep_lx = lx;
keep_ly = ly;

//   if (clear_space > 1)
   {
    if (lx < 37) going_x = 1;
    if (lx > 42) going_x = -1;
    if (ly < 33) ly += 2;
    if (ly > 37) ly -= 2;
    if (do_2 == 0)
    {
     clear_space = 6;
    } else
          {
           clear_space = 2;
          }
    do
    {
     lx += going_x;
     if (grid [lx] [ly] == 1) grid [lx] [ly] = 17;
  if (do_2 == 0 && random3(300) == 0 && traps_put2 == 0 && traps_put < 20)
  {
   place_specific_trap(grid, trap_type, trap_x, trap_y, lx,
     ly, 4);
   traps_put++;
   traps_put2++;
  }
    } while (grid [lx + going_x * (2 + clear_space)] [ly] == 1 && lx < 72 && lx > 8);
   }

//if (random3(2) == 10)
if (do_2 == 1) //random3(2) == 0)
{
keep_lx2 = lx;
keep_ly2 = ly;
//do_2 = 1;
} //else do_2 = 0;

{
 lx = keep_lx;
 ly = keep_ly;
}/* else
{
 goto doing_y;
}*/
//cprintf("a");
//getch();

going_y = 0;

if (lx < 37) going_x = 1;
if (lx > 42) going_x = -1;
if (lx >= 37 && lx <= 42)
{
// cprintf("Y");
 goto finishing;
}
goto do_x;

finishing :  start_point_x = random3(60) + 10;
//cprintf("finished.");
/*
for (lx = 0; lx < 20; lx ++)
{
start_point_x = random3(60) + 10;
start_point_y = random3(50) + 10;
// if (grid [start_point_x] [start_point_y] == 17)
grid [start_point_x] [start_point_y] = 18;
}*/

int treasure_item = 0;
int glopop = 0;

for (bcount_x = 0; bcount_x < 16 + random3(5) + random3(5); bcount_x ++)
{
switch(random3(11))
{
 case 9:
 case 0: glopop = 0; break;
 case 1: glopop = 1; break;
 case 10:
 case 2: glopop = 2; break;
 case 3: glopop = 3; break;
 case 4: glopop = 5; break;
 case 5: glopop = 6; break;
 case 6: glopop = 7; break;
 case 7: glopop = 10; break;
 case 8: glopop = 11; break;
}

 treasure_item = items(no_it, item_ident, item_class, unique_items, item_type, item_plus, item_dam,
  item_quant, item_x, item_y, icolour, xtra_quant, item_link, item_description,
  grid, glopop, 250, 1, many_many * 3);
 item_x [treasure_item] = lx;
 item_y [treasure_item] = ly;
}
int passed [2];


int dffgdsg = place_monster(mons_class,
mons_hp,
mons_hp_max,
mons_HD,
mons_AC,
mons_ev,
mons_speed,
mons_speed_inc,
mons_inv,
mons_sec,
mons_ench_1,
mons_ench,
mons_hit,
mons_beh,
no_it,
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
xtra_quant,
item_link,
item_description,
property,
mons_x,
mons_y,
0, //plus_seventy,
74, // typed,
1, //type_place,
lx, //px,
ly, //py,
0, //behaviour,
MHITNOT, //hitting,
0,//1,//allow_bands
gmon_use,
grid,
mons_alloc,
passed,
igrid,
2, //x_pos,
2  //y_pos
);


grid [lx] [ly] = 18; // I think...
// note: must be [lx] [ly] so that you can't be placed on it.

link_items(mons_inv, item_class, unique_items, item_x, item_x, item_y, item_link, igrid, item_quant);



unsigned char floory = 2;
if (random3(5) == 0) floory = 4;
if (random3(15) == 0) floory = 10;

for (bcount_x = 0; bcount_x < 80; bcount_x ++)
{
        for (bcount_y = 0; bcount_y < 70; bcount_y ++)
        {
                if (floory == 10)
  {
    if (grid [bcount_x] [bcount_y] == 1) grid [bcount_x] [bcount_y] = 2 + random3(2) * 2;
  } else if (grid [bcount_x] [bcount_y] == 1) grid [bcount_x] [bcount_y] = floory;
        }
}

//cprintf("finished2");

} // end void labyrinth level



int place_specific_trap(unsigned char grid [80] [70], unsigned char trap_type [NTRAPS],
 unsigned char trap_x [NTRAPS], unsigned char trap_y [NTRAPS], unsigned char spec_x,
unsigned char spec_y, unsigned char spec_type)
{

int tcount = 0;

//  cprintf("x");
//  getch();


for (tcount = 0; tcount < NTRAPS; tcount ++)
{
 if (trap_type [tcount] == 100)
 {
  trap_type [tcount] = spec_type;
  trap_x [tcount] = spec_x;
  trap_y [tcount] = spec_y;
  grid [spec_x] [spec_y] = 28;
  return 1;
 }
 if (tcount >= NTRAPS - 1) return 0;
}

} // end place_specific_trap





/*
THIS ONE WORKS:

void labyrinth_level(
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
unsigned char grid [80] [70],
unsigned char stair_x [10],
unsigned char stair_y [10])
{
int lx = 0;
int ly = 0;
int keep_lx = 0;
int keep_ly = 0;
int keep_lx2 = 0;
int keep_ly2 = 0;
char do_2 = 0;
int clear_space = 1;
char start_point_x = 10;
char start_point_y = 10;
char going_x = 1;
char going_y = 0;

char gk [5];

if (random3(2) == 0) going_x = 1; else going_y = 1;

if (random3(2) == 0)
{
 start_point_x = 70;
 if (going_x == 1) going_x = -1;
}
if (random3(2) == 0)
{
 start_point_y = 60;
 if (going_y == 1) going_y = -1;
}

lx = start_point_x;
ly = start_point_y;

grid [lx] [ly] = 18;

if (going_y != 0) goto do_y;

do_x :// if (going_x != 0)
//{
clear_space = random3(2) + 2;
do
 {
  lx += going_x;
//  itoa(lx, gk, 10);
//  cprintf(gk);
//  cprintf("x");
  grid [lx] [ly] = 17;
//  cprintf(",");
//  getch();
//if (random3(1000) == 0) goto finishing;

 }
 while (grid [lx + going_x * (2 + clear_space)] [ly] == 1 && lx < 72 && lx > 8);
//}
//cprintf("z");
//getch();

doing_y : going_x = 0;

if (ly < 35) going_y = 1;
if (ly > 45) going_y = -1;
if (ly >= 35 && ly <= 45)
{
// cprintf("X");
 goto finishing;
}


do_y :// if (going_y != 0)
//{
//clear_space = random3(2) + 2;
//if (random3(2) == 0)
if (do_2 == 1)
{
lx = keep_lx2;
ly = keep_ly2;
}

clear_space = 1;

    if (random3(2) == 0)
    {
     clear_space = 0;
     do_2 = 0;
    } else
          {
           clear_space = 0; //2;
           do_2 = 1;
          }

do
 {
  ly += going_y;
//  itoa(ly, gk, 10);
//  cprintf(gk);
//  cprintf("y");
//  getch();
  grid [lx] [ly] = 17;
//  cprintf(",");
//  if (random3(1000) == 0)  goto finishing;
 }
 while (grid [lx] [ly + going_y * (2 + clear_space)] == 1 && ly < 62 && ly > 8);
//}
keep_lx = lx;
keep_ly = ly;

//   if (clear_space > 1)
   {
    if (lx < 35) going_x = 1;
    if (lx > 45) going_x = -1;
    if (ly < 35) ly += 2;
    if (ly > 45) ly -= 2;
    if (do_2 == 0)
    {
     clear_space = 4;
    } else
          {
           clear_space = 0;
          }
    do
    {
     lx += going_x;
     grid [lx] [ly] = 17;
    } while (grid [lx + going_x * (2 + clear_space)] [ly] == 1 && lx < 72 && lx > 8);
   }

//if (random3(2) == 10)
if (do_2 == 1) //random3(2) == 0)
{
keep_lx2 = lx;
keep_ly2 = ly;
//do_2 = 1;
} //else do_2 = 0;

{
 lx = keep_lx;
 ly = keep_ly;
}/* else
{
 goto doing_y;
}* /
//cprintf("a");
//getch();

going_y = 0;

if (lx < 35) going_x = 1;
if (lx > 45) going_x = -1;
if (lx >= 35 && lx <= 45)
{
// cprintf("Y");
 goto finishing;
}
goto do_x;

finishing :  start_point_x = random3(60) + 10;
//cprintf("finished.");
/*
for (lx = 0; lx < 20; lx ++)
{
start_point_x = random3(60) + 10;
start_point_y = random3(50) + 10;
// if (grid [start_point_x] [start_point_y] == 17)
grid [start_point_x] [start_point_y] = 18;
}* /

//cprintf("finished2");

} // end void labyrinth level



*/

#define OUTPUT_NO inv_no
//magic_ability(mag_abil, intel)


#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
//#include <math.h>
//#include <time.h>
//#include <alloc.h>
#include <dos.h>
#include <fstream.h>
#include <fcntl.h>
#include <io.h>
#include <SYS\STAT.H>
#include <process.h>

#include <time.h>

//#include <crt0.h>


//#include <dos.h>
#include "globvars.h"
//#include "newacces.h"
//#include "specshad.h"
//#include "moreshad.h"
#include "newacces.h"
#include "newview.h"
#include "monspls5.h"
#include "crawlfnc.h"

#include <assert.h>

//#include "spells.h"

#include "fn2.h"
#include "fn3.h"
#include "invent.h"
#include "monsstat.h"

#include "builder.h"

//#define ITEMS 300



/*

//#include don't!<graphics.h>
// <time.h> - this is for when random numbers are used.
//#include not<iostream.h>
//unsigned char basic_grid [100] [100]; // this is what is _REALLY_ there.
//unsigned char basic_grid_col [100] [100]; // Colour!

//unsigned char grid [140] [140]; // this is what is actually there
//unsigned char show [12] [12]; // this is just what you see

//unsigned char grid_col [100] [100]; // this is the colour of what is shown.
//unsigned char pre_show [12] [12];
//unsigned char show_col [12] [12];
//int x_pos = 10;
//int y_pos = 10;
//int count_x;      many of these variables are in my .h header files
//int count_y;
//int nothing;
int playing = 1;
void viewwindow(void);
//int i;
//int j;                  // REMEMBER directvideo ! ! !
//int k;
//int l;
int m;
int n;
int o;
int x;
int y;

//char keyin;
char keyin_2;
int move_x = 0;
int move_y = 0;
char info [60] [78];  // This is messages sent to the screen via message func.
//int turn; // which turn it is.
int line_no = 0; // which line the message is on.
int info_lines = 0; // how many lines of info displayed this turn.
int info_screens; // how many screens of info displayed this turn.
int counter = 0;
int counter_max = 0;
//int your_colour = 7; // What colour are you?
int can_see; // Are you blind?
char letters [52] [1]; // Holds the letters a-z, A-Z.
int item_got;
int items_here = 0;
int gloggo;
int item_drop_1;
int item_drop_2;
//int hitp; // Hit points!
//int dam; // how much damage you do.
char temp_quant [3]; // used for printing quantities of items.
int quant_drop; // the quantity of a multiple item dropped.
int turnover; // have you done anything this input which uses a turn?
int delay_t = 0;
int delay_doing = 0;
int hunger = 120;
int hung_ch = 0;
int item_wielded = -1;
int item_wield_1;
int item_wield_2;

int armour [0] = -1;
int armour [1] = -1;
int armour [2] = -1;
int armour [3] = -1;
int armour [4] = -1;
int armour [5] = -1;
int armour_wear_1;
int armour_wear_2;

int ring_wear_1;
int ring_wear_2;
int ring [2];
int hand_used;

int beam_range;
int beam_type;
int beam_colour;
int beam_source_x;
int beam_source_y;
int beam_damage;
int beam_hit;
char beam_flavour;

char beam_name [30];

int throw_1;
int throw_2;
int thing_thrown;
int x_plos; int y_plos;

//char killer; // used in monster_die() for what killed the monster
/*
// Character attributes:
int hp = 10; int hp_ch = 1;
int hp_max = 10; int hp_max_ch = 1;
int ep = 3; int ep_ch = 1;
int ep_max = 5; int ep_max_ch = 1;
unsigned int strength = 16; int strength_ch = 1;
unsigned int intel = 13; int intel_ch = 1;
unsigned int dex = 11; int dex_ch = 1;
int AC = 0; int AC_ch = 1; // remember that the AC shown = 10 - AC
int evasion = 10; int evasion_ch = 1;
int damage = 3; int damage_ch = 1;  // remember to change damage back to 1!
int nat_damage = 3; // the damage you do unarmed
float rate_regen = 0.06; float incr_regen = 0;
int xp = 0; int xp_ch = 1;
int xl = 1; int xl_ch = 1;
int gp = 0; int gp_ch = 1;
int clas;

char clasnam [13];

float f_abil = 2; // fighting ability, out of whatever

float speed = 1; // 0.75;
float speed_inc = 0;

//char hp_print [3];
//char hp_max_print [3];
//char ep_print [3];
//char ep_max_print [3];
//char strength_print [3];
//char intel_print [3];
//char dex_print [3];
//char AC_print [2];
//char evasion_print [2];
//char damage_print [2];
char st_prn [8];
//char exl_print [2];

//int your_level = 0;

// these are potion/spell counter variables:
int haste = 0; // haste
int might = 0; // might
int lev = 0; // levitation
int poison = 0; // poison!!


// Resistances: Use (res_whatever > 0) when checking, because if you're
// wearing two rings of something you have res_whatever = 2.
char res_fire = 0;
char res_poison = 0;
char res_cold = 0;


// Monster stuff: gmon = general monster, mons = specific monster

char gmon_name [52] [20]; // The name of the monster
//int gmon_HD [52]; // Hit Dice
int gmon_att [52] [4]; // attacks and damage of each attack
char gmon_use [52];
//int gmon_AC [52];
//int gmon_ev [52]; // evasion
//float gmon_speed [52]; // speed - not in use yet
//int gmon_use [52]; // can it use items?
//int mcolour [52];
/*
int mons_class [100];
int mons_hp [100];
int mons_hp_max [100];
int mons_HD [100];
int mons_AC [100];
int mons_ev [100];
//int mons_att [100] [4]; // (don't) use the gmon value for this.
float mons_speed [100];
float mons_speed_inc [100];
int mons_x [100];
int mons_y [100];
int mons_targ_1_x [100]; // these four can probable be reused each time through the loop too.
int mons_targ_1_y [100];
int mons_targ_2_x [100];
int mons_targ_2_y [100];
*/


//int mmov_x;  // don't need to be arrays because they are reused
//int mmov_y;  // each time through the i loop.


void initialise(void);
//void play(void);
void input(void);
//void more(void);
void move(void);
void run(void);
//void shadowfunc(void);
extern void draw_border(int bord_col);
extern char *ident(char clss, char typ, char plus); // char dam) - not yet
//extern char mons_define(char mclss, char chrct);

//void item(void);
//void message(void);
void item_check(void);
//void inventory(int item_class_inv);
void pickup(void);
void drop(void);
int add_item(void); // adds an item to invent
void item_place(void); // puts an item on the floor somewhere. (used for drop & throw_it (i think))
int conv_lett(int item_drop_3); // converts ASCII number to a number 0-52.
void wield_weapon(void);
void throw_anything(void);
void shoot_thing(void);
void throw_it(void);
void missile(void);

void wear_armour(void);
void takeoff_armour(void);

void eat_food(void);
void food_change(void);
void eaten(int food_eat_3);
void drink(void);
void potion_effect(char pot_eff, int pow);

int check_mons_magres(int mn, int pow);

void puton_ring(void);
void remove_ring(void);

void read_scroll(void);

void magic_missile(char source);
void fireball(char source, int power);

void zap_wand(void);
//void zapping(char ztype);
void zapping(char ztype, int power);
void beam(void);

//void level_change(void);

//void monster_grid(void); // puts monsters into the grid array
void monster(void);
void monster_move(void); // move the monsters around
//void monster_attack(int monster_attacking);
//int inflict_dam (void);
void dragon(void);

void tracer_f(void);

void mons_throw(void);
void mons_pickup(void);
void mons_trap(void);

void plant_spit(void);

//void you_attack(int monster_attacked);
//void monster_die(int monster_killed);
//void load(void);
/*
void save_one(void);
void save_two(void);
void save_three(void);
void save_four(void);
void save_five(void);
void save_six(void);
void save_seven(void);
void save_eight(void);
void save_nine(void);
void save_ten(void);
*/
void you_teleport(void);

//void place_cloud(void);

void open_door(void);
void close_door(void);
void down_stairs(void);
void up_stairs(void);

extern int dir_cursor(char rng);

void cast_a_spell(void);
void your_spells(int spc2, int powc);
void memorise_spell(void);

//extern void message(char inf [60] [78], int inf_lines);



void noisy(void);
void direction(char rnge);

void stethoscope(void);

void mons_cast(void);

void monster_teleport(char monstel);

void summon_demon(void);

//void relay_message(void);

void quit_game(void);


void version(void);
void explosion1(void);
void explosion(void);
void spore_goes_pop(void);

void incrl(void);

void check_links(void);

void fall_into_a_pool(char place, unsigned char grype);
char scramble(void);
void dart_trap(void);
void itrap(void);

void it_name(int itn, char des);
void in_name(int inn, char des);

void burden_change(void);

void torment(void);

void random_uselessness(unsigned char ru);

void mons_in_cloud(void);
void look_around(void);

void identify(char pow);
void remove_curse(void);
void detect_curse(void);

void blink(void);
void random_blink(void);

void manage_shock_shield(void);

void conjure_flame(void);
void stinking_cloud(void);
void stinkcl(char cl_x, char cl_y);
void cast_big_c(int pow, char cty);
void big_cloud(char clouds, char cl_x, char cl_y, int pow);
//void cause_fear(void);
void mass_enchantment(int wh_enchant, int pow);
void cast_ring_of_flames(int power);
void cast_toxic_radiance(void);
void abjuration(int pow);
void cast_fire_storm(int pow);


void cast_lesser_healing(int mabil);
void cast_greater_healing(int mabil);
void cast_cure_poison(int mabil);
void purification(int mabil);
void cast_deaths_door(int pow);
void cast_selective_amnesia(void);
void cast_smiting(int pow);
void turn_undead(int pow);
void holy_word(int pow);

void summon_small_mammals(int pow);
void summon_butter(int pow);
void summon_scorpions(int pow);
void summon_swarm(int pow);
void summon_things(int pow);

void restore_str(void);
void restore_int(void);
void restore_dex(void);

void check_your_resists(void);
void check_mons_resists(void);

void which_spell(void);
int which_spellbook(void);
void read_book(char book_read);

char learn_a_spell(unsigned char splbook, char bitty);

//int mons_ench_f2(char info [78] [78], char info_lines, unsigned char mons_speed [MNST], char mons_ench [MNST] [3], int o, char mons_ench_1 [MNST], int mons_hp [MNST], int mons_hp_max [MNST], int mons_class [MNST], char is_near, char beam_colour, char see_invis, int func_pass [10]);
int mons_ench_f2(int o, char is_near, int func_pass [10]);

char spell_direction(void);

void    direct_effect(void);
void mons_direct_effect(void);

void new_level(void);
void original_name(void);
void special_wielded(void);

void invoke_wielded(void);
void staff_spell(char zap_device_2);

//void mons_speed_adjust(void);


//void item(void);
//void monster_grid(void);


 // items  variables:
        //char all_items [30] [10] [30];
//      int property [4] [30] [3];
//      char mass [10] [30];
// char fake_name [20] [10] [30];This goes with the individual items, I think.
        //int icolour [10] [30];
         /*
         int item_class [200];
         int item_type [200];
         int item_plus [200]; // +, charges, remaining food value
         int item_dam [200]; // damage
         int item_quant [200]; // multiple items
         int item_x [200]; //  x-location
         int item_y [200]; //  y-location
         char fake_name [200] [30]; // fake names of items
         */

/*

         char inv_name [52] [30];
         char inv_class [52];
         char inv_type [52];
         char inv_plus [52];
         char inv_dam [52];
         char inv_col [52];
         int inv_quant [52]; // multiple items, eg ammo or potions
         int burden; // total weight of items carried.
/        char inv_no; // number of items carried.

*/

/*
        float mons_to_hit;
        int damage_taken = 0;
        int runthru = 0;


//       char ench_subj [25];
//       char ench_type [25];
//       char ench_

         // char mons_beh [100]; in view.h

         char mons_ench [100] [3];
         char mons_ench_1 [100]; // is it enchanted at all?
         //unsigned char mons_inv [100] [4];

*/


int main(void)
{
initial(); // in crawlfnc - this must be before initialise();
initialise();
while (playing == 1)
{
        input();
//      if (hp <= 0) end_game(1);
}
return 0;
}







void initialise(void)
{
//                                              damage = 99;

strcpy(str_pass, "");

for (i = 0; i < 10; i ++)
{
        func_pass [i] = 0;
}

for (i = 0; i < 6; i++)
{
        armour [i] = -1;
}


textbackground(0);
your_level = 0;

strcpy(clasnam, "Wizard"); clas = 1;

                        directvideo = 1;


for (i = 0; i < 12; i ++)
{
        row_of_7s [i] = 7;
}



/*
for (count_x = 0; count_x < 120; count_x ++)
{
        for (count_y = 0; count_y < 100; count_y ++)
        {
        if (( grid [count_x] [count_y] = (char *) malloc(10)) == NULL)
        {
           printf("Not enough memory to allocate buffer\n");
           exit(1);  // terminate program if out of memory
        }
        }
}
*/


//randomize(); // BC
srandom(time(NULL));
clrscr();







// There's a lot of junk from here on.




for (i = 1; i < ITEMS; i++)
{
        item_class [i] = -1;
        item_type [i] = -1;
        //strcpy(fake_name [i], " ");
        item_x [i] = 1;
        item_y [i] = 1;
        item_quant [i] = 0;
        item_dam [i] = 0;
        item_plus [i] = 0;
        item_ident [i] = 0;
        item_link [i] = 501;
//      icolour [i] = random(16);
}


for (i = 0; i < 20; i ++)
{
        strcpy(info [i], "");
}

for (i = 0; i < MNST; i++)
{
        mons_class [i] = -1;
        mons_speed_inc [i] = 10;
        mons_targ_1_x [i] = 155;
        mons_ench_1 [i] = 0;
        mons_beh [i] = 0; // patrolling
 mons_hit [i] = MNST + 1; // nothing
        for (j = 0; j < 3; j++)
        {
                mons_ench [i] [j] = 0;
        }
        for (j = 0; j < 8; j++)
        {
                mons_inv [i] [j] = 501;
        }
        mons_sec [i] = 0;
}

for (i = 0; i < PETS; i ++)
{
//  pet_which [i] = 99;
}
//pet_no = 0;

for (i = 0; i < 80; i ++)
{
        for (j = 0; j < 70; j ++)
        {
                igrid [i] [j] = 501;
  map [i] [j] = 0;
        }
}

for (i = 0; i < CLOUDS; i ++)
{

        cloud_x [i] = 0;
        cloud_y [i] = 0;
        cloud_type [i] = 0;
        cloud_decay [i] = 0;
        //int cloud_no;
}
cloud_no = 0;


for (i = 0; i < 400; i ++)
{
        //for (j = 1; j < 4; j ++) gmon_att [i] [j] = 0;
        //gmon_att [i] [0] = 10;
        gmon_use [i] = 0;
}

//gmon_att [29] [1] = 5;
//gmon_att [29] [2] = 35;




for (i = 0; i < 52; i++)
{
//      strcpy(inv_name [i], "");
        inv_quant [i] = 0;
}


for (i = 0; i < 25; i ++)
{
        spells [i] = -1;
        //cout << (int) spells [i] << ",";
}

//getch();

spells [0] = 60;
spells [4] = 6;
spells [12] = 0;
//spell_no = 3;


newc = new_game(your_name);




ring [0] = -1; ring [1] = -1;


//int gogi = newc;


//if (newc < 50) newc -= 10;
//if (newc > 50) newc -= 100;

//cprintf("restoring");

restore_game(); // Must work on saving regeneration, now that speed seems








load();

new_level();

//cprintf("\n\rgot out of load");


/*


//strcpy(gmon_name [0], "giant ant");
mcolour [0] = 8;
//gmon_att [0] [0] = 8;
gmon_use [0] = 0;

//strcpy(gmon_name [1], "giant bat");
mcolour [1] = 8;
//gmon_att [1] [0] = 1;
gmon_use [1] = 0;
gmon_use [1] = 0;

//strcpy(gmon_name [2], "cockatrice");
mcolour [2] = 14;
//gmon_att [2] [0] = 6;
gmon_use [2] = 0;

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
gmon_use [6] = 1;

//strcpy(gmon_name [7], "hound");
mcolour [7] = 6;
//gmon_att [7] [0] = 10;
gmon_use [7] = 0;

//strcpy(gmon_name [8], "imp");
mcolour [8] = 7;
//gmon_att [8] [0] = 4;
gmon_use [8] = 0;

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
gmon_use [12] = 0;

//strcpy(gmon_name [13], "nymph"); // don't know about this one.
mcolour [13] = 10;
//gmon_att [13] [0] = 10;
gmon_use [13] = 1;

//strcpy(gmon_name [14], "orc");
mcolour [14] = 12;
//gmon_att [14] [0] = 5;
gmon_use [14] = 1;
gmon_use [14] = 1;

//strcpy(gmon_name [15], "piercer");
mcolour [15] = 7;
//gmon_att [15] [0] = 5;
gmon_use [15] = 0;

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

//strcpy(gmon_name [18], "snake");
mcolour [18] = 2;
//gmon_att [18] [0] = 6;
gmon_use [18] = 0;

//strcpy(gmon_name [19], "tunneling worm");
mcolour [19] = 12;
//gmon_att [19] [0] = 25;
gmon_use [19] = 0;

//strcpy(gmon_name [20], "unicorn");
mcolour [20] = 15;
//gmon_att [20] [0] = 15;
gmon_use [20] = 0;

//strcpy(gmon_name [21], "vulture");
mcolour [21] = 8;
//gmon_att [21] [0] = 3;
gmon_use [21] = 0;

//strcpy(gmon_name [22], "giant wasp");
mcolour [22] = 14;
//gmon_att [22] [0] = 10;
gmon_use [22] = 0;

//strcpy(gmon_name [23], "xax");
mcolour [23] = 13;
//gmon_att [23] [0] = 5;
gmon_use [23] = 0;

//strcpy(gmon_name [24], "yellow light");
mcolour [24] = 14;
//gmon_att [24] [0] = 0;
gmon_use [24] = 0;

//strcpy(gmon_name [25], "small zombie");
mcolour [25] = 6;
//gmon_att [25] [0] = 15;
gmon_use [25] = 0;

//strcpy(gmon_name [26], "abomination");
mcolour [26] = 11;
//gmon_att [26] [0] = 15;
gmon_use [26] = 0;

//strcpy(gmon_name [27], "giant beetle");
mcolour [27] = 8;
//gmon_att [27] [0] = 25;
gmon_use [27] = 0;

//strcpy(gmon_name [28], "cyclops");
mcolour [28] = 6;
//gmon_att [28] [0] = 35;
gmon_use [28] = 1;

//strcpy(gmon_name [29], "dragon");
mcolour [29] = 2;
//gmon_att [29] [0] = 15;
//gmon_att [29] [1] = 7;
//gmon_att [29] [2] = 7;
gmon_use [29] = 0;

//strcpy(gmon_name [30], "elemental");
mcolour [30] = 4;
gmon_use [30] = 0;

//strcpy(gmon_name [31], "pit fiend");
mcolour [31] = 2;
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
gmon_use [33] = 1;

//strcpy(gmon_name [34], "something invisible"); // not a real creature
mcolour [34] = 15;
gmon_use [34] = 0;

//strcpy(gmon_name [35], "jelly");
mcolour [35] = 6;
//gmon_att [35] [0] = 10;
gmon_use [35] = 0;

//strcpy(gmon_name [36], "kobold");
mcolour [36] = 6;
//gmon_att [36] [0] = 4;
gmon_use [36] = 1;

//strcpy(gmon_name [37], "lich");
mcolour [37] = 15;
//gmon_att [37] [0] = 10;
gmon_use [37] = 1;

//strcpy(gmon_name [38], "mummy");
mcolour [38] = 15;
//gmon_att [38] [0] = 8;
gmon_use [38] = 0;

//strcpy(gmon_name [39], "naga");
mcolour [39] = 11;
//gmon_att [39] [0] = 6;
gmon_use [39] = 0;

//strcpy(gmon_name [40], "ogre");
mcolour [40] = 6;
//gmon_att [40] [0] = 20;
gmon_use [40] = 1;

//strcpy(gmon_name [41], "plant");
mcolour [41] = 2;
gmon_use [41] = 0;

//strcpy(gmon_name [42], "queen bee");
mcolour [42] = 2;
//gmon_att [42] [0] = 13;
gmon_use [42] = 0;

//strcpy(gmon_name [43], "raksasha");
mcolour [43] = 14;
//gmon_att [43] [0] = 7;
gmon_use [43] = 1;

//strcpy(gmon_name [44], "spectre");
mcolour [44] = 2;
//gmon_att [44] [0] = 11;
gmon_use [44] = 0;

//strcpy(gmon_name [45], "troll");
mcolour [45] = 6;
//gmon_att [45] [0] = 15;
//gmon_att [45] [1] = 10;
//gmon_att [45] [2] = 10;
gmon_use [45] = 0;

//strcpy(gmon_name [46], "undine");
mcolour [46] = 6;
//gmon_att [46] [0] = 3;
gmon_use [46] = 0;

//strcpy(gmon_name [47], "vampire");
mcolour [47] = 15;
//gmon_att [47] [0] = 8;
gmon_use [47] = 0;

//strcpy(gmon_name [48], "wraith");
mcolour [48] = 6;
//gmon_att [48] [0] = 7;
gmon_use [48] = 0;

//strcpy(gmon_name [49], "xoryx");
mcolour [49] = 6;
//gmon_att [49] [0] = 20;
//gmon_att [49] [1] = 15;
//gmon_att [49] [2] = 15;
gmon_use [49] = 0;

//strcpy(gmon_name [50], "yak");
mcolour [50] = 15;
//gmon_att [50] [0] = 9;
gmon_use [50] = 0;

//strcpy(gmon_name [51], "large zombie");
mcolour [51] = 15;
//gmon_att [51] [0] = 20;
gmon_use [51] = 0;

//strcpy(gmon_name [52], "orc warrior");
mcolour [52] = 14;
//gmon_att [52] [0] = 12;
gmon_use [52] = 1;

//strcpy(gmon_name [53], "kobold demonologist");
mcolour [53] = 5;
//gmon_att [53] [0] = 4;
gmon_use [53] = 1;

//strcpy(gmon_name [54], "orc wizard");
mcolour [54] = MAGENTA;
//gmon_att [54] [0] = 5;
gmon_use [54] = 1;

/* //strcpy(gmon_name [53], "kobold demonologist");
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
/*mcolour [56] = LIGHTRED;
//gmon_att [56] [0] = 50;
//gmon_att [56] [1] = 15;
//gmon_att [56] [2] = 15;
gmon_use [56] = 0;
*/



mon_init(gmon_use, mcolour);

def_letters(letters);

def_properties(property, mass);

////strcpy(gmon_name [39], "nymph");
//mcolour [39] = 12;
////strcpy(gmon_name [40], "ogre");
//mcolour [40] = 6;
////strcpy(gmon_name [41], "plant");
//mcolour [41] = 2;


/*
for (i = 1; i < 120; i += 4)
{
        for (j = 1; j < 120; j += 4)
        {
                if (grid [i] [j] == 17)
                {
                        mons_x [k] = i;
                        mons_y [k] = j;
                        grid [i] [j] = 18;
                        break;
                }
        }
if (grid [i] [j] == 18) break;
}
*/
//}

/*      case 0: strcat(inv_name [sc_read_2], "robe"); break;
        case 1: strcat(inv_name [sc_read_2], "leather armour"); break;
        case 2: strcat(inv_name [sc_read_2], "ring mail"); break;
        case 3: strcat(inv_name [sc_read_2], "scale mail"); break;
        case 4: strcat(inv_name [sc_read_2], "chain mail"); break;
        case 5: strcat(inv_name [sc_read_2], "splint mail"); break;
        case 6: strcat(inv_name [sc_read_2], "banded mail"); break;
        case 7: strcat(inv_name [sc_read_2], "plate mail"); break;
*/





//cprintf("finished restoring");                       //       to be solved.

if (newc == 1)
{

for (i = 0; i < 80; i ++)
{
        for (j = 0; j < 70; j ++)
        {
                if (grid [i] [j] == 18 | grid [i] [j] == 68)
                {
                        x_pos = i;
                        y_pos = j;
                //      break;
                }
        }
if (grid [i] [j] == 17) break;
}

/*for (i = 0; i < 52; i ++)
{
//      strcpy(inv_name [i], ident(inv_class [i], inv_type [i], inv_plus [i]));
//      inv_ident [i] = 2;

        if (inv_quant [i] < 1) continue;

        if (inv_class [i] < 3)
        {
                burden += mass [inv_class [i]] [inv_type [i]] * inv_quant [i];
        } else
              switch(inv_class [i])
               {
                case 3: burden += (int) 100 * inv_quant [i]; break;
                case 4: burden += (int) 100 * inv_quant [i]; break;
                case 5: burden += (int) 200 * inv_quant [i]; break;
                case 6: burden += (int) 50 * inv_quant [i]; break;
                case 7: burden += (int) 20 * inv_quant [i]; break;
                case 8: burden += (int) 60 * inv_quant [i]; break;
                case 9: burden += (int) 5 * inv_quant [i]; break;
               }
}*/

burden_change();
food_change();


} // end if newc


// traps testing:
/*for (i = 0; i < NTRAPS; i ++)
{
        trap_x [i] = 1;
        trap_y [i] = 1;
        trap_type [i] = 0;
}*/
/*
grid [x_pos - 1] [y_pos] = 28;
grid [x_pos + 1] [y_pos] = 28;
grid [x_pos] [y_pos + 1] = 28;
grid [x_pos] [y_pos - 1] = 28;

trap_x [0] = x_pos - 1;
trap_y [0] = y_pos;
trap_type [0] = 0;

trap_x [1] = x_pos + 1;
trap_y [1] = y_pos;
trap_type [1] = 1;

trap_x [2] = x_pos;
trap_y [2] = y_pos + 1;
trap_type [2] = 2;

trap_x [3] = x_pos;
trap_y [3] = y_pos - 1;
trap_type [3] = 3;

*/

/*
for (i = 0; i < 10; i ++)
{
        do
        {
                count_x = random(60) + 10;
                count_y = random(50) + 10;

        } while (grid [count_x] [count_y] != 17);

        grid [count_x] [count_y] = 28;

        //trap_x [i] = count_x;
        //trap_y [i] = count_y;
        //trap_type [i] = random2(4);
}
*/




/*
for (i = 0; i < MNST; i ++)
{
//      for (j = 0; j < ITEMS; j ++)
//      {
                if (mons_class [i] != -1 && gmon_use [mons_class [i]] > 0)
                {
                        for (j = 0; j < 8; j ++)
                        {
                        if (mons_inv [i] [j] != 501)
                        {
                                item_x [mons_inv [i] [j]] = mons_x [i];
                                item_y [mons_inv [i] [j]] = mons_y [i];
                        }
                        }
                }
//      }

}
*/




//monsters_get(mons_class, mons_inv, item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y, it_no, i, mons_x, mons_y);


        /*
        for (i = 0; i < MNST; i++)
        {
        if (mons_class [i] != -1 && gmon_use [mons_class [i]] != 0)
                {
                        monsters_get(mons_class, mons_inv, item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y, it_no, i, mons_x, mons_y, mons_AC, mons_ev, property);
                        //monsters_find(mons_class, mons_inv, item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y, it_no, i, mons_x, mons_y, property, mons_AC, mons_ev);
                        //restock_monsters(mons_class, mons_inv, item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y, it_no, i, mons_x, mons_y);
                }
        }


        Does nothing.

        */


/*
cout << x_pos;
cprintf("\n\r");
cout << y_pos;
newc = getch();
*/


//draw_border(BROWN);

//getch();

brek = 1;
viewwindow(); // This just puts the view up for the first turn.
brek = 0;

//getch();

item();
/*
// Now, put your stats onto the screen:
window(16,2,75,12);
clrscr();
gotoxy(4,2);
textcolor(7);
cprintf("HP:\n\r");
cprintf("   EP:\n\r");
cprintf("   Strength:\n\r");
cprintf("   Intelligence:\n\r");
cprintf("   Dexterity:\n\r");
cprintf("   AC:\n\r");
cprintf("   EV:\n\r");
cprintf("   Gold:\n\r");
cprintf("   Experience:\r");
*/




}


void input(void)
{

        time_taken = speed + burden_state;

// pet_target = MHITNOT;

        /*turns ++;
        itoa(turns, st_prn, 10);
        strcpy(info [info_lines], st_prn);
        incrl();
        relay_message();*/

int delay_more = 0;
scruggle = 0;
//for (turn = 1; turn < 960; turn ++)
//{
//      viewwindow();
        // First, print out stats:
/*      window(16,1,75,11);
        gotoxy(8,3);
        cout << hp << "(" << hp_max << ")";
        gotoxy(8,4);
        cout << ep << "(" << ep_max << ")";
        gotoxy(14,5);
        cout << strength;
        gotoxy(18,6); cout << intel;
        gotoxy(15,7); cout << dex;
        gotoxy(8,8); cout << AC;
        gotoxy(8,9); cout << evasion;
        gotoxy(12,10); cout << damage;
        gotoxy(16,11); cout << exl << "/" << xp;
*/
        window(36,1,75,16);
        textcolor(7);

// if any of these call the message function indirectly (eg through burden_change)
//  must reset window after.

        if (hp_ch == 1 | hp_max_ch == 1)
        {
 if (hp <= hp_max / 4) textcolor(YELLOW);
 if (hp <= hp_max / 10) textcolor(RED);
        itoa(hp, st_prn, 10);
        gotoxy (8,2);
        cprintf(st_prn);
 textcolor(LIGHTGRAY);
        itoa(hp_max, st_prn, 10);
        cprintf("("); cprintf(st_prn); cprintf(")    ");
        hp_ch = 0; hp_max_ch = 0;
        }

        if (ep_ch == 1 | ep_max_ch == 1)
        {
        itoa(ep, st_prn, 10);
        gotoxy (11,3);
//      gotoxy (8,3);
        cprintf(st_prn);
        itoa(ep_max, st_prn, 10);
        cprintf("("); cprintf(st_prn); cprintf(")   ");
        ep_ch = 0; ep_max_ch = 0;
        }

        if (strength_ch == 1)
        {
 if (strength <= 0) strength = 0;
        itoa(strength, st_prn, 10);
//      gotoxy (14,6);
        gotoxy (9,6);
 if (strength < max_strength) textcolor(YELLOW);
        cprintf(st_prn);
 textcolor(LIGHTGRAY);
        itoa(max_strength, st_prn, 10);
        cprintf("("); cprintf(st_prn); cprintf(")   ");
 strength_ch = 0;
 if (strength == 0) ouch(-9999, 0, 8);
 burden_change(); // because strength can be reduced in monspls3.h
        window(36,1,75,16);
        }

        if (intel_ch == 1)
        {
 if (intel <= 0) intel = 0;
        itoa(intel, st_prn, 10);
//      gotoxy (18,7);
        gotoxy (9,7);
 if (intel < max_intel) textcolor(YELLOW);
        cprintf(st_prn);
 textcolor(LIGHTGRAY);
        itoa(max_intel, st_prn, 10);
        cprintf("("); cprintf(st_prn); cprintf(")   ");
        intel_ch = 0;
 if (intel == 0) ouch(-9999, 0, 7);
        }

        if (dex_ch == 1)
        {
 if (dex <= 0) dex = 0;
        itoa(dex, st_prn, 10);
//      gotoxy (15,8);
        gotoxy (9,8);
 if (dex < max_dex) textcolor(YELLOW);
        cprintf(st_prn);
 textcolor(LIGHTGRAY);
        itoa(max_dex, st_prn, 10);
        cprintf("("); cprintf(st_prn); cprintf(")   ");
        dex_ch = 0;
 if (dex == 0) ouch(-9999, 0, 9);
        }

        if (AC_ch == 1)
        {
        itoa(AC, st_prn, 10);
        gotoxy (8,4);
        cprintf(st_prn);
 if (strlen(st_prn) <= 1) cprintf(" ");
 if (strlen(st_prn) <= 2) cprintf(" ");
 if (strlen(st_prn) <= 3) cprintf(" ");
        cprintf("(");
        itoa(shield_class, st_prn, 10);
        cprintf(st_prn);
 cprintf(")   ");
        AC_ch = 0;
        }

        if (evasion_ch == 1)
        {
        itoa(evasion, st_prn, 10);
        gotoxy (8,5);
        cprintf(st_prn);
        cprintf("  ");
        evasion_ch = 0;
        }
/*

        itoa(your_level, st_prn, 10);
        gotoxy (8,8);
        cprintf(st_prn);
        cprintf("  ");
//      evasion_ch = 0;
*/
        if (gp_ch == 1)
        {
        itoa(gp, st_prn, 10);
        gotoxy (10,9);
        cprintf(st_prn);
        cprintf("  ");
        gp_ch = 0;
        }

        if (xl_ch == 1 | xp_ch == 1)
        {
        itoa(xl, temp_quant, 10);
        //ultoa(xp, st_prn, 10);
        itoa(xp, st_prn, 10);
        gotoxy (16,10);
        cprintf(temp_quant); cprintf("/"); cprintf(st_prn);
        cprintf("      ");
        xl_ch = 0; xp_ch = 0;
        }

        if (hung_ch == 1)
        {
 gotoxy(4,12);
 switch(hung_state)
 {
  case 5: cprintf("Engorged"); break;
  case 4: cprintf("Full    "); break;
  case 3: cprintf("        "); break;
  case 2: cprintf("Hungry  "); break;
  case 1: cprintf("Starving"); break;
 }
 hung_ch = 0;
 } // end of it hung_ch == 1

        if (burden_ch == 1)
        {
        window(36,2,75,15); // was 16, ....
 gotoxy(4,13);
 switch(burden_state)
 {
  case 5: cprintf("Overloaded"); break;
  case 2: cprintf("Encumbered"); break;
  case 0: cprintf("          "); break;
 }
 burden_ch = 0;
 } // end of it burden_ch == 1


 /*
        if (hunger < 1352 + hunger_inc && hunger > 1110)
        {
        gotoxy(4,11);
        cprintf("Hungry   ");
        hung_ch = 0;
        goto get_out_of_hunger;
        }

        if (hunger > 3449)
        {
        gotoxy(4,11);
        cprintf("Full    ");
        hung_ch = 0;
        goto get_out_of_hunger;
        }

        if (hunger > 1351)
        {
        gotoxy(4,11);
        cprintf("        ");
        hung_ch = 0;
        }

        if (hunger <= 500)
        {
        gotoxy(4,11);
        cprintf("Starving    ");
        hung_ch = 0;
        }

        } // end of if hung_ch
*/
//      get_out_of_hunger :

 if (paralysis == 0)
        {

        if (delay_t > 1) delay_t--;
        if (delay_t == 1)
        {
//  mesclr();
                switch(delay_doing)
                {
                case 0: strcpy(info [info_lines], "You finish eating."); incrl(); //relay_message(); break;
  break;

                case 1: strcpy(info [info_lines], "You finish putting on ");
                //item_name(inv_class [armour_wear_2], inv_type [armour_wear_2], inv_dam [armour_wear_2], inv_plus [armour_wear_2], inv_quant [armour_wear_2], inv_ident [armour_wear_2], 5, str_pass);
                in_name(armour_wear_2, 5);
                strcat(info [info_lines], str_pass);
                strcat(info [info_lines], ".");
                incrl(); //relay_message();
  break;

                case 2: strcpy(info [info_lines], "You finish taking off ");
                //item_name(inv_class [armour_wear_2], inv_type [armour_wear_2], inv_dam [armour_wear_2], inv_plus [armour_wear_2], inv_quant [armour_wear_2], inv_ident [armour_wear_2], 5, str_pass);
                in_name(armour_wear_2, 5);
                strcat(info [info_lines], str_pass);
                strcat(info [info_lines], ".");
                incrl(); // test relay_message();
  break;
                        // etc...

      case 3: strcpy(info [info_lines], "You finish memorising.");
      incrl();
      //relay_message();
      break;

                }
        delay_t = 0;
        }

        window(2,2,32,18);
        gotoxy(17,8);

//   flushall();

   if (delay_t == 0)
        {
        _setcursortype(_NORMALCURSOR);

        if (running > 0)
        {
                keyin = 125;
                move_x = run_x;
                move_y = run_y;

      if (kbhit())
      {
         running = 0;
         goto gutch;
      }

                if (run_x == 0 && run_y == 0)
                {
                        running--;
                        keyin = '.';
                }
        } else
               gutch: keyin = getch();

        if (keyin == 0) // alternate ("alt") also works - see ..\KEYTEST.CPP
        {
// mesclr();
        keyin = getch();
        switch(keyin)
        {
                case 'O': move_x = -1; move_y = 1; break;
                case 'P': move_y = 1; move_x = 0; break;
                case 'I': move_x = 1; move_y = -1; break;
                case 'H': move_y = -1; move_x = 0; break;
                case 'G': move_y = -1; move_x = -1; break;
                case 'K': move_x = -1; move_y = 0; break;
                case 'Q': move_y = 1; move_x = 1; break;
                case 'M': move_x = 1; move_y = 0; break;

                case 'S': turnover = 1; move_x = 0; move_y = 0; keyin = '.'; break;
//              Wcheck(); break;
        }

        keyin = 125;
        }
        //speed_inc -= 1;
        }
        else keyin = '.'; // end of if delay_t > 1
        }
        else keyin = '.'; // end of if paralysis == 0


        //_setcursortype(_NOCURSOR);

        if (keyin != 125)
        {
                move_x = 0; move_y = 0; turnover = 0;
        }

 mesclr();
        switch(keyin)
        {

                case 'b': move_x = -1; move_y = 1; break;
                case 'j': move_y = 1; move_x = 0; break;
                case 'u': move_x = 1; move_y = -1; break;
                case 'k': move_y = -1; move_x = 0; break;
                case 'y': move_y = -1; move_x = -1; break;
                case 'h': move_x = -1; move_y = 0; break;
                case 'n': move_y = 1; move_x = 1; break;
                case 'l': move_x = 1; move_y = 0; break;

                case 'B': run_x = -1; run_y = 1; running = 2; break;
                case 'J': run_y = 1; run_x = 0; running = 2; break;
                case 'U': run_x = 1; run_y = -1; running = 2; break;
                case 'K': run_y = -1; run_x = 0; running = 2; break;
                case 'Y': run_y = -1; run_x = -1; running = 2; break;
                case 'H': run_x = -1; run_y = 0; running = 2; break;
                case 'N': run_y = 1; run_x = 1; running = 2; break;
                case 'L': run_x = 1; run_y = 0; running = 2; break;
//              case '5': run_x = 0; run_y = 0; running = 50; break;

                case '1': run_x = -1; run_y = 1; running = 2; break;
                case '2': run_y = 1; run_x = 0; running = 2; break;
                case '9': run_x = 1; run_y = -1; running = 2; break;
                case '8': run_y = -1; run_x = 0; running = 2; break;
                case '7': run_y = -1; run_x = -1; running = 2; break;
                case '4': run_x = -1; run_y = 0; running = 2; break;
                case '3': run_y = 1; run_x = 1; running = 2; break;
                case '6': run_x = 1; run_y = 0; running = 2; break;
                case '5': run_x = 0; run_y = 0; running = 50; break;

                case '<': up_stairs(); break;
                case '>': down_stairs(); break;
                case 'o': open_door(); break;
                case 'c': close_door(); break;
                case 'd': drop(); break;// return;
                case 'i': //relay_message(); //inventory(-1); break;// return;
                nothing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
                break;
                case ',': pickup(); break;// return;
                case ';': item_check(); break;// more();
                         //// test relay_message();
                          //return;
                case 'w': wield_weapon(); break;
                case 't': throw_anything(); break;
  case 'f': shoot_thing(); break;
                case 'W': wear_armour(); break;
                case 'T': takeoff_armour(); break;
                case 'P': puton_ring(); break;
                case 'R': remove_ring(); break;
                case 'z': zap_wand(); break;
  case 'I': invoke_wielded(); move_x = 0; move_y = 0; break;
                case 'e': eat_food(); break;
                case '.':
//  if (hiding != 0 && invis == 0) invis = 1;
  move_x = 0;
  move_y = 0;
  turnover = 1; break;
                case 'q': drink(); break;
                case 'r': read_scroll(); break;
//              case '0': Wcheck(); break; // 0 key uses Wcheck()
                case 'x': look_around(); break;
  case 's': stethoscope(); break; turnover = 1; move_x = 0; move_y = 0; break; //stethoscope(); break; //turnover = 1; break; //stethoscope(); break;
                case 'Z': cast_a_spell(); break;
                case 'M': which_spell(); break;      //memorise_spell(); break;
  case 'X':
  if (level_type == 1)
  {
   strcpy(info [info_lines], "You have no idea where you are!");
   incrl();
   break;
  }
show_map(map, x_pos, y_pos); break;
                case '\\': nothing = check_item_knowledge(); break;
  case 16: replay_messages(); break;
  case '?': list_commands(); break;
  case 'V': original_name(); break;
  case '$': hp += 50; hp_max += 50; hp_ch = 1; break;
//  case '$': identify(0); break; //forget_map(); break; //conf += 50; break; //hp_max += 50; hp_ch = 1;
//  xp += 1000; xp_ch ++; level_change(); break;
  case '%':  if (spell_direction() == -1) break;
   zapping(11, 180);
//   move_x = 0; move_y = 0;
   if (move_x == 0 && move_y == 0)
   {
    strcpy(info [info_lines], "Why would you want to do that?");
    incrl();
    break;
   }
   break;
//grid [x_pos] [y_pos] = 18;
//  case '!': torment(); break;
  case '#': magic_mapping(grid, map, x_pos, y_pos, 80);
  break;/*

//  case 'K': detect_traps(); break;
 //shock_shield = 100; break;
//  case 'k': cast_freezing_c(mag_abil); turnover = 1; break;
  case 16: replay_messages(); break;
  case '?': list_commands(); break;
                case '\\':
  nothing = check_item_knowledge();

/* itoa(x_pos, st_prn, 10);
                strcpy(info [info_lines], st_prn);
                strcat(info [info_lines], ",");
                itoa(y_pos, st_prn, 10);
                strcat(info [info_lines], st_prn);
                incrl();*/
//              break;
                case '`':
   itoa(OUTPUT_NO, st_prn, 10);
                        strcpy(info [info_lines], st_prn);
   incrl();
                        move_x = 0;
                        move_y = 0;
break;
/*   for (i = 0; i < MNST; i ++)
                        {
    if (mons_class [i] != -1) move_x ++;
                        }
                        itoa(move_x, st_prn, 10);
                        strcpy(info [info_lines], st_prn);
                        incrl();*/
//                      move_x = 0;
//                      getch();
//                      break;

//     case '/': grid [x_pos] [y_pos] = 22;
//      break;
//              case 'b': spellbook_contents(22);
//      break;
//  case '|': hp = hp_max; hp_ch = 1;
//  purification(100);
//  break;
//              case '0':

//      grid [x_pos] [y_pos] = 18;


                //window(1, 1, 80, 25);
/*
for (count_x = 0; count_x < 80; count_x ++)
{
        for (count_y = 0; count_y < 70; count_y ++)
        {

        if (igrid [count_x] [count_y] == 501) continue;

        int frx = igrid [count_x] [count_y];

        while(frx != 501)
        {
                //cout << frx; //getch();
                //item_x [frx] = count_x;
                //item_y [frx] = count_y;

                if (frx > ITEMS - 1 | item_quant [frx] < 1)
                {
                        strcpy(info [info_lines], "Problem! ");
                        itoa(item_x [frx], st_prn, 10);
                        strcat(info [info_lines], st_prn);
                        strcat(info [info_lines], ",");
                        itoa(item_y [frx], st_prn, 10);
                        strcat(info [info_lines], st_prn);
                        incrl();
                }

                int fry = item_link [frx];
                frx = fry;
        }


        }
}
*/
/*
for (i = 0; i < NTRAPS; i ++)
{
        if (grid [trap_x [i]] [trap_y [i]] < 25 | (grid [trap_x [i]] [trap_y [i]] > 28 && grid [trap_x [i]] [trap_y [i]] < 75))
        {
                //strcpy(info [info_lines], "Oh no!");
                //incrl();


                //cout << (int) trap_x [i] << "," << (int) trap_y [i] << " ";


                get_ch();
         }

}

strcpy(info [info_lines], "Checked!");
incrl();


*/







/*                      for (count_x = 0; count_x < 80; count_x ++)
                        {
                                for (count_y = 0; count_y < 70; count_y ++)
                                {
                                if (igrid [count_x] [count_y] != 501) textcolor(1);
                                if (igrid [count_x] [count_y] != 501 && item_quant [igrid [count_x] [count_y]] < 1) textcolor(3);
                                //cout << igrid [count_x] [count_y] << ", ";
                                itoa(igrid [count_x] [count_y], st_prn, 10);
                                cprintf(st_prn);
                                cprintf(",");
                                textcolor(7);
                                if (count_y == 65) keyin = getch();
                                }
                        }

                        for (i = 0; i < MNST; i ++)
                        {
                                if (mons_class [i] == 56 | mons_class [i] == 19)
                                {
                                        //itoa(mons_x [i]);
                                        cout << (unsigned int) i << "/" << (int) mons_class [i] << ":" << (int) mons_x [i] << "," << (int) mons_y [i] << " ";
                                }
                        }*/



                        //itoa(burden, st_prn, 10);
                        //itoa(burden, st_prn, 10);
                        //strcpy(info [info_lines], st_prn);
                        //incrl();
//                      break;
//              case 'a': dir_cursor(1); break;
                case 'S': save_game(); break;
                case 'Q': quit_game(); break;
                case 'v': version(); break;
                //case 'g': run(); break;
//              case '[': check_links(); break;
                case 125: break;
/*  case '=': //keyin = curse_an_item(1,0);
  if (curse_an_item(1,0) == 1)
  {
    strcpy(info [info_lines], "success.");
    incrl();
  }
  break;*/

                default: strcpy(info [info_lines], "Unknown command.");
                       //       strcat(info [info_lines], keyin);
                      //        strcat(info [info_lines], "\""); incrl();
                        incrl(); break;//return;

                        // move_x = 0; move_y = 0; break;
        }



if (move_x != 0 | move_y != 0) move();


if (turnover == 0)
{
// test relay_message();
        brek = 1;
        viewwindow();
        brek = 0;
        return;
}

if (special_wield != 0)
{
 special_wielded();
}


if (teleport > 0 && random2(80 / teleport) == 0)
{
        you_teleport();
}



if (grid [x_pos] [y_pos] > 100) in_a_cloud();

// mesclr();

if (invis > 1) invis --;
if (invis == 1)
{
        strcpy(info [info_lines], "You flicker back into view.");
        incrl();
        your_sign = 64;
        invis = 0;
}



if (conf > 1) conf --;
if (conf == 1)
{
        strcpy(info [info_lines], "You feel steadier.");
        incrl();
        conf = 0;
}


if (paralysis > 1) paralysis--;
if (paralysis == 1)
{
        strcpy(info [info_lines], "You can move again.");
        incrl();
        paralysis = 0;
}



if (slow > 1) slow--;
if (slow == 1)
{
        strcpy(info [info_lines], "You feel yourself speed up.");
        incrl();
        slow = 0;
}

if (haste > 1) haste--;
if (haste == 1)
{
        strcpy(info [info_lines], "You feel yourself slow down.");
        incrl();
        haste = 0;
}

if (might > 1) might--;
if (might == 1)
{
        strcpy(info [info_lines], "You feel a little less mighty now.");
        incrl();
        might = 0;
        strength -= 5;
 max_strength -= 5;
        strength_ch = 1;
}

if (berserker > 1) berserker--;
if (berserker == 1)
{
        strcpy(info [info_lines], "You are no longer berserk.");
        incrl();
        strcpy(info [info_lines], "You feel exhausted.");
        incrl();
        berserker = 0;
 paralysis += 4 + random2(4);
 slow += 8 + random2(8) + random2(15);
 hunger -= 700;
 if (hunger <= 50) hunger = 50;

/*      strength -= 5;
 max_strength -= 5;
        strength_ch = 1;*/
}


if (lev > 1)
{
 lev--;
 if (lev == 10)
 {
  strcpy(info [info_lines], "You are starting to lose your buoyancy!");
  incrl();
  lev -= random2(6); // so you never know how much time you have left!
 }
}

if (lev == 1)
{
        strcpy(info [info_lines], "You float gracefully downwards.");
        incrl();
        lev = 0;
 if (grid [x_pos] [y_pos] % 50 == 11 | grid [x_pos] [y_pos] % 50 == 12) fall_into_a_pool(1, grid [x_pos] [y_pos] % 50);
}


if (rotting > 0)
{
        if (random2(20) <= (rotting - 1))
        {
                ouch(1, 0, 1);
                hp_max --;
                hp_ch = 1;
                hp_max_ch = 1;
                strcpy(info [info_lines], "You feel your flesh rotting away.");
                incrl();
                rotting--;
        }

}


if (poison > 0)
{
        if (random2(5) <= (poison - 1))
        {
  if (poison > 10 && random2(poison) >= 8)
                {
                        ouch(random2(10) + 5, 0, 1);
                        hp_ch = 1;
                        strcpy(info [info_lines], "You feel extremely sick.");
                        incrl();
                } else
                if (poison > 5 && random2(2) == 0)
                {
                        ouch(random2(2) + 2, 0, 1);
                        hp_ch = 1;
                        strcpy(info [info_lines], "You feel very sick.");
                        incrl();

                } else
                      {
                        ouch(1, 0, 1);
                        hp_ch = 1;
                        strcpy(info [info_lines], "You feel sick.");// //the poison running through your veins.");
                        incrl();
                      }

        if (random2(8) == 0 | (hp == 1 && random2(3) == 0))
        {
                strcpy(info [info_lines], "You feel a little better.");
                incrl();
                poison--;
        }

        }

//      if (poison > 9) ouch(-9999, 0, 1);
}

if (deaths_door > 0)
{
 if (hp > 0)
 {
  strcpy(info [info_lines], "Your life is in your own hands once again.");
  incrl();
//  strcpy(info [info_lines], "You feel
  paralysis += 5 + random2(5);
  conf += 10 + random2(10);
  hp_max --;
  if (hp > hp_max) hp = hp_max;
  hp_ch = 1;
  deaths_door = 0;
 }
   else deaths_door --;

 if (deaths_door == 10)
 {
  strcpy(info [info_lines], "Your time is quickly running out!");
  incrl();
  deaths_door -= random2(6);
 }

 if (deaths_door == 1)
 {
  strcpy(info [info_lines], "Your time has expired!");
  incrl();
  relay_message();
  more();
  ouch(-9999, 0, 4);
 }

}


if (hunger_inc > 0 && hunger >= 40)
{
 hunger -= hunger_inc;
 hunger -= burden_state;
}

if (hp < hp_max && deaths_door == 0) incr_regen += rate_regen;
if (ep < ep_max) ep_incr_regen += ep_rate_regen;

while(incr_regen >= 100)
{
        hp ++; if (hp > hp_max) hp = hp_max;
        incr_regen -= 100;
        hp_ch = 1;
}

while(ep_incr_regen >= 100)
{
        ep ++; if (ep > ep_max) ep = ep_max;
        ep_incr_regen -= 100;
        ep_ch = 1;
}



//if (speed_inc < 1)
//monster_grid();
losight(show, grid, x_pos, y_pos);
monster(); // used to be monster_move();
//monster_grid();


if (cloud_no > 0)
{
        //cout << "Managing!!!";
        cloud_no = manage_clouds(cloud_x, cloud_y, cloud_type, cloud_decay, cloud_no, grid, time_taken, haste, slow);
}

if (shock_shield > 0)
{
 manage_shock_shield();
}

if (hunger <= 500)
{
if (paralysis == 0 && random2(40) == 0)
{
 strcpy(info [info_lines], "You lose consciousness!");
 incrl();
        paralysis += random2(8) + 5;
 if (paralysis > 13) paralysis = 13;
}
if (hunger <= 100)
{
 strcpy(info [info_lines], "You have starved to death.");
 incrl();
 ouch(-9999, 0, 15);
}
}

if (hung_state == 3 && hunger <= 2600 && hunger_inc > 0) //hunger >= 1351 && hunger <= 1354 + hunger_inc && hunger_ch > 0)
{
        strcpy(info [info_lines], "You are feeling hungry.");
        incrl();
 hung_state = 2;
        hung_ch = 1;
}


if (hung_state == 2 && hunger <= 1000 && hunger_inc > 0) // hunger >= 500 && hunger <= 501 + hunger_inc && hunger_ch > 0)
{
        strcpy(info [info_lines], "You are starving!");
        incrl();
 hung_state = 1;
        hung_ch = 1;
}


if (hung_state == 4 && hunger < 7000 && hunger_inc > 0) //hunger >= 3549 && hunger <= 3554 + hunger_inc && hunger_ch > 0)
{
//      strcpy(info [info_lines], "You are feeling a little less full.");
//      incrl();
 hung_state = 3;
        hung_ch = 1;
}

if (hung_state == 5 && hunger < 11000 && hunger_inc > 0) //hunger >= 3549 && hunger <= 3554 + hunger_inc && hunger_ch > 0)
{
 hung_state = 4;
        hung_ch = 1;
}



if ((delay_t > 0 | paralysis > 0) && scruggle != 0) delay_more = 1;
/*
relay_message();
//info_lines = 0;

if (delay_more == 1)
{
        more();
        delay_more = 0;
}
*/
_setcursortype(_NOCURSOR);

//item();
//monster_grid();
brek = 1;
viewwindow();
brek = 0;


_setcursortype(_NORMALCURSOR);

relay_message();
//info_lines = 0;

if (delay_more == 1)
{
        more();
        delay_more = 0;
 scrloc = 0;
}




        if (no_mons < 150 && random2(50) == 0 && level_type != 1) mons_place(250, 0, 50, 50, 0, MHITNOT); // random2(50)



//if (turnover == 0) return;


// if (info_lines != 0) relay_message();

//item_check();
//      x_pos++; y_pos++;

//if (turnover == 0) return;

// put monsters moving etc here.

//if (speed_inc < 1) speed_inc += speed;

//turnover = 0;

return;

//nothing = getch();
//return 0;
}

/*

void dir_cursor(void)
{
        x = 6; y = 6;
        char grokol [3];


//      gotoxy(count_x - x_pos + 6, count_y - y_pos + 6);

        gotoxy(x,y);
        //keyin = getch();

strcpy(info [info_lines], "Which target?");
incrl();
relay_message();

        window(2,2,12,13);
        gotoxy(x,y);

while (keyin != '.')
{
keyin = getch();
move_x = 0; move_y = 0;
switch(keyin)
{
        case '1': move_x = -1; move_y = 1; break;
        case '2': move_y = 1; move_x = 0;break;
        case '9': move_x = 1; move_y = -1; break;
        case '8': move_y = -1; move_x = 0;break;
        case '7': move_y = -1; move_x = -1; break;
        case '4': move_x = -1; move_y = 0;break;
        case '3': move_y = 1; move_x = 1; break;
        case '6': move_x = 1; move_y = 0;break;

        // put in case '.'

        //default: strcpy(info [info_lines], "What an unusual direction.");
          //    incrl(); relay_message(); window(2,2,12,13); gotoxy(x,y); move_x = 0; move_y = 0;
}

//gettext(x + 1, y + 1, x + 1, y + 1, grokol);
//textbackground(0);
gotoxy(x,y);
//putch(grokol [0]);
//textbackground(7);

if (x + move_x >= 1 && x + move_x < 12) x += move_x;
if (y + move_y >= 1 && y + move_y < 12) y += move_y;

//gettext(x + 1, y + 1, x + 1, y + 1, grokol);
//textbackground(7);
gotoxy(x,y);
//putch(grokol [0]);
//textbackground(0);

} // end of while.
move_x = x - 6;
move_y = y - 6;

}
*/


void move(void)
{
char attacking = 0;
char stepping = 0;

        if (conf > 0)
        {
  if (random2(3) != 0)
  {
    move_x = random2(3) - 1;
                  move_y = random2(3) - 1;
  }

                if (grid [x_pos + move_x] [y_pos + move_y] <= 10)
                {
                        turnover = 1;
                        strcpy(info [info_lines], "Ouch!");
                        incrl();
                        return;
                }

  if ((grid [x_pos + move_x] [y_pos + move_y] % 50 == 11 | grid [x_pos + move_x] [y_pos + move_y] % 50 == 12) && lev == 0)
                {
        fall_into_a_pool(0, grid [x_pos + move_x] [y_pos + move_y] % 50);
   turnover = 1;
                        return;
                }
        } // end of if conf

        if (running > 0 && running != 2 && grid [x_pos + move_x] [y_pos + move_y] != 17 && grid [x_pos + move_x] [y_pos + move_y] != 28)
        {
                running = 0;
                move_x = 0;
                move_y = 0;
                turnover = 0;
      return;
        }

        if (grid [x_pos + move_x] [y_pos + move_y] > 50)
        {
                for (i = 0; i < MNST; i ++)
                {
                        if (mons_x [i] == x_pos + move_x && mons_y [i] == y_pos + move_y && mons_class [i] != -1 && (move_x != 0 | move_y != 0))
                        {
    if (mons_class [i] >= MLAVA0 && mons_sec [i] == 1) break;
    if (mons_beh [i] == 7 && mons_ench [i] [2] != 6 && conf == 0)
    {
      swap_places(i);
      break;
    }
                         you_attack(i);
                         turnover = 1;
    attacking = 1;
                         break;
                        }
                }
        }

 if ((grid [x_pos + move_x] [y_pos + move_y] % 50 == 11 | grid [x_pos + move_x] [y_pos + move_y] % 50 == 12) && attacking == 0 && lev == 0)
        {
  strcpy(info [info_lines], "Do you really want to step there?");
                incrl();
                stepping = get_ch();
  if (stepping == 'y' | stepping == 'Y')
  {
   fall_into_a_pool(0, grid [x_pos + move_x] [y_pos + move_y] % 50);
                turnover = 1;
                return;
  }
  strcpy(info [info_lines], "Okay, then.");
                incrl();
                return;
        }

        if (attacking == 0 && (grid [x_pos + move_x] [y_pos + move_y] % 100 > 10)) // && grid [x_pos + move_x] [y_pos + move_y] < 50) | (grid [x_pos + move_x] [y_pos + move_y] > 110 && grid [x_pos + move_x] [y_pos + move_y] < 150))
        {
                x_pos += move_x;
                y_pos += move_y;
                move_x = 0;
                move_y = 0;
  if (fast_run != 0) time_taken *= 0.6;
                turnover = 1;
                item_check();    // ITEM_CHECK!!

                if (grid [x_pos] [y_pos] > 24 && grid [x_pos] [y_pos] < 29)
                {

                if (grid [x_pos] [y_pos] == 28)
                {
                        //abort();
                        for (i = 0; i < NTRAPS; i ++)
                        {
                                if (trap_x [i] == x_pos && trap_y [i] == y_pos) break;
                        }
                        //cout << "Z" << (int) trap_type [i]; getch();
                        if (trap_type [i] < 4) grid [x_pos] [y_pos] = 25;
   if (trap_type [i] == 4 | trap_type [i] == 5) grid [x_pos] [y_pos] = 26;
                                // else if (trap_type is magic etc
                }


                //if (grid [x_pos] [y_pos] == 25) // later change this to a range for many traps.
                //{
                damage_taken = 0;

 //             switch (grid [x_pos] [y_pos])
//              {
               //       if (grid [x_pos] [y_pos] == 25) // later change this to a range for many traps.
                //{


// maybe later introduce variables, maybe reusing the beam variables, to multiuse this bit for spear etc traps.


                        for (i = 0; i < NTRAPS; i ++)
                        {
                                if (trap_x [i] == x_pos && trap_y [i] == y_pos) break;
                        }


                        switch(trap_type [i])
                        {

                        case 0:
                        strcpy(beam_name, " dart");
                        beam_damage = 4;
                        trapped = i;
                        dart_trap();
                        break;

                        case 1:
                        strcpy(beam_name, "n arrow");
                        beam_damage = 7;
                        trapped = i;
                        dart_trap();
                        break;

                        case 2:
                        strcpy(beam_name, " spear");
                        beam_damage = 10;
                        trapped = i;
                        dart_trap();
                        break;

                        case 3:
                        strcpy(beam_name, "n axe");
                        beam_damage = 15;
                        trapped = i;
                        dart_trap();
                        break;

   case 4:
   strcpy(info [info_lines], "You step on a teleport trap!");
   incrl();
   you_teleport();
   break;

   case 5:
   strcpy(info [info_lines], "You feel momentarily disoriented.");
   incrl();
   forget_map();
   break;

                        } // end of switch


                //} // end of if grid == trap

                } // end of if another grid == trap

                }

        //return;



        //}



        if (grid [x_pos + move_x] [y_pos + move_y] < 10)
        {
                move_x = 0;
                move_y = 0;
                running = 0;
                turnover = 0;
        }


//}

 if (running == 2) running = 1;


} // end of void move()



void fall_into_a_pool(char place, unsigned char grype)
{

char escape = 0;
switch(grype)
{
case 11: strcpy(info [info_lines], "You fall into the lava!"); break;
case 12: strcpy(info [info_lines], "You fall into the water!"); break;
}

incrl();
more();
mesclr();

if (grype == 11 && res_fire == 0)
{
  strcpy(info [info_lines], "You burn to a crisp!");
  incrl();
  ouch(-9999, 0, 5);
}

// a distinction between stepping and falling from lev stops you stepping into a thin stream of lava to get to the other side.

if (place == 1 && scramble() == 1)
{
        if (empty_surrounds(x_pos, y_pos) == 1)
        {
        x_pos = empty_x;
                y_pos = empty_y;
  escape = 1;
        } else goto drowning;
}

if (place == 0 && scramble() == 1)
{
                escape = 1;
}

if (escape == 1)
{
 strcpy(info [info_lines], "You manage to scramble free!");
 incrl();
 if (grype == 11) scrolls_burn(10, 6);
 return;
}

if (place == 0) // don't display this if you fall in from levitating
{
 strcpy(info [info_lines], "You try to escape, but your burden drags you down!");
 incrl();
}

drowning: strcpy(info [info_lines], "You drown...");
incrl();
if (grype == 11) ouch(-9999, 0, 5);
if (grype == 12) ouch(-9999, 0, 6);
// Okay, so you don't trigger a trap when you scramble onto it. I really can't be bothered right now.

} // end of fall_into_a_pool()


char scramble(void)
{
if (((1000 + strength * 200) / 2) + random2((1000 + strength * 200) / 2) <= burden)
                         return 0; // failed

return 1; // success
} // end of scramble



/*void run(void)
{

strcpy(info [info_lines], "Go which direction?");
incrl();
relay_message();

keyin = getch();

if (keyin == 0) // alternate ("alt") also works - see ..\KEYTEST.CPP
        {
        keyin = getch();
        switch(keyin)
        {
                case 'O': move_x = -1; move_y = 1; break;
                case 'P': move_y = 1; move_x = 0; break;
                case 'I': move_x = 1; move_y = -1; break;
                case 'H': move_y = -1; move_x = 0; break;
                case 'G': move_y = -1; move_x = -1; break;
                case 'K': move_x = -1; move_y = 0; break;
                case 'Q': move_y = 1; move_x = 1; break;
                case 'M': move_x = 1; move_y = 0; break;
                //case 'S': turnover = 1; move_x = 0; move_y = 0; break;
        }
        }
         else
             {
                strcpy(info [info_lines], "What an unusual direction!");
                incrl();
                return;
             }

        run_x = move_x;
        run_y = move_y;
        running = 1;

}

*/






/*

void draw_border(int bord_col)
{
textcolor(bord_col);
// this bit draws the borders:
window(1,1,80,25);

for (i = 1; i < 80; i ++)
{
        gotoxy(i,1);
        putch(196);
        gotoxy(i,13);
        putch(196);
        gotoxy(i,25);
        putch(196);
}

for (i = 1; i < 25; i ++)
{
        gotoxy(1,i);
        putch(179);
        gotoxy(79,i);
        putch(179);
        if (i < 14)
        {
                gotoxy(13, i);
                putch(179);
        }
}

gotoxy(1,1); putch(218);
gotoxy(79,1); putch(191);
gotoxy(1,13); putch(195);
gotoxy(13,1); putch(194);
gotoxy(13,13); putch(193);
gotoxy(79,13); putch(180);
gotoxy(1,25); putch(192);
gotoxy(79,25); putch(217);

} // end of void draw_border(int bord_col)




*/


// The message display:
/*
void message(void)
{
int over_check = 0;// stops the printing of too many lines.
int me_c; // loop variable - after all, lots of functions call this one.

window(2, 16, 78, 24);
clrscr();
//info_lines--;
if (info_lines == 0) return;

info_screens = info_lines / 8 + 1;
textcolor (7);

for (me_c = 0; me_c < info_screens; me_c++)
{
        if (info_lines - me_c * 8 > 8) over_check = 8;
                else over_check = info_lines - me_c * 8;


                for (j = 0; j < over_check; j++)
                {
                        gotoxy(1, j + 1);
                        if (info [j + me_c * 8] [0] == 95) // reversing
                        {
                                textcolor(0); textbackground(7);
                                info [j + me_c * 8] [0] = 32;
                        }
                        cprintf(info [j + me_c * 8]);
                        textcolor(7); textbackground(0);
                }
        if (info_screens > me_c + 1 && info_lines - (me_c + 1) * 8 != 0)// && info_screens != 1)
        {
                gotoxy(1 , 8);
                more();
                clrscr();
        }
//if (me_c = 5)
} // end of me_c loop.



info_screens = 0;
info_lines = 0;
//line_no++;
//       if (line_no >9)
//       {
//       clrscr();
//       line_no = 9;
//       }
//if (keyin == 59) more();
} // end of message function




void more(void)
{
char keypress = 0;

window(2, 16, 78, 24);
gotoxy(1,9);
_setcursortype(_NORMALCURSOR);
cprintf("\r--more--");
do
{
        keypress = getch();
} while (keypress != 32 && keypress != 13);

//return keypress;
keypress = 0;
}

*/


void item_check(void)
{
char item_show [50] [50]; // shows what items you're standing on.

/*if (grid [x_pos] [y_pos] != 17)
{
        strcpy(info [info_lines], "There's something here!");
        incrl();
}*/

itc = 0;

counter = 0;


if (grid [x_pos] [y_pos] == 18)
{
        strcpy(info [info_lines], "There is a staircase here.");
 incrl();
}

if (grid [x_pos] [y_pos] == 19)
{
        strcpy(info [info_lines], "There is a staircase here.");
 incrl();
}



        //cout << igrid [x_pos] [y_pos];
        //keyin = getch();






if (igrid [x_pos] [y_pos] == 501) return;

//if (item_link [igrid [x_pos] [y_pos]]

//strcpy(info [info_lines], "Hello!");
//incrl();

//cprintf("Hello!");

int objl = igrid [x_pos] [y_pos];
int hrg = 0;

while(objl != 501)
{
                                counter++;

                                if (counter > 45)
                                {
                                        strcpy(item_show [counter], "Too many items.");
//                                      incrl();
     break;
                                        //return;
                                }

                                if (item_class [objl] == 15)
                                {
                                itoa(item_quant [objl], temp_quant, 10);
//                              strcpy(item_show [counter], " ");
                                strcpy(item_show [counter], temp_quant);

                                if (item_quant [objl] > 1) strcat(item_show [counter], " gold pieces");
                                                       else strcat(item_show [counter], " gold piece");
                                goto linking; //continue;
                                }


                        //item_name(item_class [objl], item_type [objl], item_dam [objl], item_plus [objl], item_quant [objl], item_ident [objl], 3, str_pass);
                        it_name(objl, 3);
                        strcpy(item_show [counter], str_pass);
                        //strcpy(item_show [counter], "grunk");
linking : hrg = item_link [objl];
objl = hrg;
}


//cprintf("huggle");


/*


I've got rid of item_x/y now. I use int igrid [80] [70] to store the item
which is on a space, then item_link [500] to store the number of the next
item in a stack. Much better, especially for mons_pickup.


        for (i = 0; i < ITEMS; i++)
        {

                if (itc > it_no) break;


                if (item_quant [i] > 0)
                {
                itc++;

                if (item_x [i] == x_pos && item_y [i] == y_pos)
                {
                        counter++;
                                if (item_class [i] == 15)
                                {
                                itoa(item_quant [i], temp_quant, 10);
//                              strcpy(item_show [counter], " ");
                                strcpy(item_show [counter], temp_quant);

                                if (item_quant [i] > 1) strcat(item_show [counter], " gold pieces");
                                                       else strcat(item_show [counter], " gold piece");

                                continue;
                                }


                        strcpy(item_show [counter], item_name(item_class [i], item_type [i], item_dam [i], item_plus [i], item_quant [i], item_ident [i], 3));
                        //fake_name [i]);
//                      if (item_quant [i] != 1 && item_class [i] != 6 && item_class [i] != 8) strcat(item_show [counter], "s");
                } // end of if x_pos...
                } // end of if item_quant...
        } // end of for i
*/

itc = 0;
counter_max = counter;
counter = 0;

if (counter_max == 1)
{
        strcpy(info [info_lines], "You see here "); // remember 'an'.
        strcat(info [info_lines], item_show [counter_max]);
        strcat(info [info_lines], ".");
        incrl(); counter++;
        counter_max = 0; // to skip next part.
}


if ((counter_max > 0 && counter_max < 6) || (counter_max > 1 && keyin == 59))
{
        strcpy(info [info_lines], "Things that are here:"); incrl();
        while (counter < counter_max)
        {
                counter++; // this is before the strcpy because item_show start at 1, not 0.
                strcpy(info [info_lines], item_show [counter]);
                incrl();
//              counter++;
        }
        //strcpy(info [info_lines], "   "); incrl();
//if (keyin == 59) more();
}

if (counter_max > 5 && keyin != 59)
        {
                strcpy(info [info_lines], "There are several objects here.");
                incrl(); //lines ++;
        }
//relay_message();
//keyin = 0;
//if (keyin == 59) more();
}






/*
void item_check(void)
{
//char item_show [50] [40]; // shows what items you're standing on.

counter = 0;

for (i = 0; i < ITEMS; i ++)
{
        if (itc > it_no) break;
        if (item_quant [i] != 0)
        {
        itc++;
        if (item_x [i] == x_pos && item_y [i] == y_pos) counter++;
        }
}

if (counter == 0) return;

if (counter == 1)
{

for (i = 0; i < ITEMS; i ++)
{
        if ((item_x [i] != x_pos | item_y [i] != y_pos) | item_quant [i] == 0) continue;

        strcpy(info [info_lines], "You see here");

        if (item_class [i] == 15)
                                {
                                itoa(item_quant [i], temp_quant, 10);
                                strcpy(info [info_lines], " ");
                                strcat(info [info_lines], temp_quant);
                                strcat(info [info_lines], " gold pieces.");
                                incrl();
                                //continue;
                                return;
                                }



                                if (item_quant [i] == 1)
                                {
                                if ((item_class [i] != 6) && (fake_name [i] [0] == 97 ^ fake_name [i] [0] == 101 ^ fake_name [i] [0] == 105 ^ fake_name [i] [0] == 111 ^ fake_name [i] [0] == 117))
                                {
                                        strcat(info [info_lines], " an ");
                                } else strcat(info [info_lines], " a ");
                                if (fake_name [i] [0] < 97) strcat(info [info_lines], " ");

                                if (item_class [i] == 6)
                                strcat(info [info_lines], "scroll ");

                                if (item_class [i] == 8)
                                strcat(info [info_lines], "bottle of ");

                                } else
                                      {
                                      itoa(item_quant [i], temp_quant, 10);
                                      strcat(info [info_lines], " ");
                                      strcat(info [info_lines], temp_quant);
                                      strcat(info [info_lines], " ");
                                      if (item_class [i] == 6)
                                                strcat(info [info_lines], "scrolls ");

                                      if (item_class [i] == 8)
                                                strcat(info [info_lines], "bottles of ");
                                      }



                        strcat(info [info_lines], fake_name [i]);
                        if (item_quant [i] != 1 && item_class [i] != 6 && item_class [i] != 8) strcat(info [info_lines], "s");
                        strcat(info [info_lines], ".");
                        incrl();
                        return;
                } // end of if x_pos...
                } // end of if item_quant...
//      } // end of for i



//      }
//      strcat(info [info_lines], ".");
//      incrl(); counter++;
//      counter_max = 0; // to skip next part.

return;
//} // end of for i

/*



//}



        for (i = 0; i < ITEMS; i++)
        {

                if (itc > it_no) break;


                if (item_quant [i] != 0)
                {
                itc++;

                if (item_x [i] == x_pos && item_y [i] == y_pos)
                {
                        counter++;
                                if (item_class [i] == 15)
                                {
                                itoa(item_quant [i], temp_quant, 10);
                                strcpy(item_show [counter], " ");
                                strcat(item_show [counter], temp_quant);
                                strcat(item_show [counter], " gold pieces");
                                continue;
                                }



                                if (item_quant [i] == 1)
                                {
                                if ((item_class [i] != 6) && (fake_name [i] [0] == 97 ^ fake_name [i] [0] == 101 ^ fake_name [i] [0] == 105 ^ fake_name [i] [0] == 111 ^ fake_name [i] [0] == 117))
                                {
                                        strcpy(item_show [counter], " an ");
                                } else strcpy(item_show [counter], " a ");
                                if (fake_name [i] [0] < 97) strcpy(item_show [counter], " ");

                                if (item_class [i] == 6)
                                strcat(item_show [counter], "scroll ");

                                if (item_class [i] == 8)
                                strcat(item_show [counter], "bottle of ");

                                } else
                                      {
                                      itoa(item_quant [i], temp_quant, 10);
                                      strcpy(item_show [counter], " ");
                                      strcat(item_show [counter], temp_quant);
                                      strcat(item_show [counter], " ");
                                      if (item_class [i] == 6)
                                        strcat(item_show [counter], "scrolls ");

                                      if (item_class [i] == 8)
                                        strcat(item_show [counter], "bottles of ");
                                      }



                        strcat(item_show [counter], fake_name [i]);
                        if (item_quant [i] != 1 && item_class [i] != 6 && item_class [i] != 8) strcat(item_show [counter], "s");
                } // end of if x_pos...
                } // end of if item_quant...
        } // end of for i

itc = 0;
counter_max = counter;
counter = 0;

if (counter_max == 1)
{
        strcpy(info [info_lines], "You see here"); // remember 'an'.
        strcat(info [info_lines], item_show [counter_max]);
        strcat(info [info_lines], ".");
        incrl(); counter++;
        counter_max = 0; // to skip next part.
}


if ((counter_max > 0 && counter_max < 6) || (counter_max > 1 && keyin == 59))
{
        strcpy(info [info_lines], "Things that are here:"); incrl();
        while (counter < counter_max)
        {
                counter++; // this is before the strcpy because item_show start at 1, not 0.
                strcpy(info [info_lines], item_show [counter]); incrl();
//              counter++;
        }
        //strcpy(info [info_lines], "   "); incrl();
//if (keyin == 59) more();
}

if (counter_max > 5 && keyin != 59)
        {
                strcpy(info [info_lines], "There are several objects here.");
                info_lines ++;
        }
//relay_message();
//keyin = 0;
//if (keyin == 59) more();
*/

//}





void pickup(void)
{
gloggo = 0;
items_here = 0;

//int last_item = 501;

if (lev != 0)
{
        strcpy(info [info_lines], "You can't reach the floor from up here.");
        incrl();
        return;
}

if (igrid [x_pos] [y_pos] == 501)
{
        strcpy(info [info_lines], "There isn't anything here.");
        incrl();
        return;
}
//return;




last_item = 501; //igrid [x_pos] [y_pos];




int objl = igrid [x_pos] [y_pos];
int hrg = 0;

while(objl != 501)
{
                                counter++;
                                /*
                                if (item_class [objl] == 15)
                                {
                                //if (item_class [i] == 15 && item_x [i] == x_pos && item_y [i] == y_pos)
                                //{
                                        strcpy(info [info_lines], "You pick up ");
                                        itoa(item_quant [objl], temp_quant, 10);
                                        strcat(info [info_lines], temp_quant);

                                        if (item_quant [objl] > 1) strcat(info [info_lines], " gold pieces.");
                                                else strcat(info [info_lines], " gold piece.");

                                        incrl();
                                        gp += item_quant [objl];
                                        item_quant [objl] = 0;
                                        it_no --;
                                        gp_ch = 1;
                                        if (item_link [objl] == objl) item_link [objl] = 501;
                                        if (last_item != igrid [x_pos] [y_pos])
                                        {
                                                item_link [last_item] = item_link [objl];
                                        }
                                                else igrid [x_pos] [y_pos] = item_link [objl]; // = 501

                                        last_item = objl;
                                        hrg = item_link [objl];
                                        objl = hrg;
                                        continue;
                                //}
                                }
                                */

                        //strcpy(item_show [counter], item_name(item_class [objl], item_type [objl], item_dam [objl], item_plus [objl], item_quant [objl], item_ident [objl], 3));

last_item = objl;

hrg = item_link [objl];
objl = hrg;
items_here++;

if (counter > 1000)
{
 strcpy(info [info_lines], "Oh dear. There appears to be a bug in the program.");
 incrl();
 strcpy(info [info_lines], "I suggest you leave this level then save as soon as possible.");
 incrl();
 return;
}

}


//if (items_here == 1) last_item = 501;







//last_item = 501;







/*
for (i = 0; i < ITEMS; i++)
{

//      if (itc > it_no) break;

        if (item_quant [i] != 0)
        {
        itc++;
        if (item_class [i] == 15 && item_x [i] == x_pos && item_y [i] == y_pos)
        {
        strcpy(info [info_lines], "You pick up ");
        itoa(item_quant [i], temp_quant, 10);
        strcat(info [info_lines], temp_quant);

        if (item_quant [i] > 1) strcat(info [info_lines], " gold pieces.");
                                else strcat(info [info_lines], " gold piece.");

        incrl();
        gp += item_quant [i];
        item_quant [i] = 0;
        it_no --;
        gp_ch = 1;
        continue;
        }
        }
        if (item_x [i] == x_pos && item_y [i] == y_pos && item_quant [i] != 0) items_here++;
}

itc = 0;

if (items_here == 0 && gp_ch != 1)
{
        strcpy(info [info_lines], "There is nothing here to pick up.");
        incrl();
}
*/

















// Anywhere which calls add_item() doesn't need to arrange item_link etc.

// Need to do the "You can only carry some of what's here..." item_link bit.





if (items_here == 1)
{
//      for (i = 0; i < ITEMS; i ++)
//      {
//              if (itc > it_no) break;

//              if (item_quant [i] != 0)
//              {
                //itc ++;
//              if (item_x [i] == x_pos && item_y [i] == y_pos)
//              {
                        item_got = igrid [x_pos] [y_pos];
                        last_item = 501;
   nothing = add_item();
                        if (nothing == 501)
                                {
                                        strcpy(info [info_lines], "You can't carry that many items.");
                                        incrl();
                                } else
                                if (nothing != 1)
                                {
                                        strcpy(info [info_lines], "You can't carry that much weight.");
                                        incrl();
                                }
                                return;
//              }
//              } // end of if item_quant
//      } // end of for i

} // end of if items_here

//itc = 0;


last_item = 501;


if (items_here > 1)
{
        strcpy(info [info_lines], "There are several objects here.");
        incrl();

        o = igrid [x_pos] [y_pos];

        for (k = 0; k < items_here; k++) // use k because of call to relay_message()
        {
//              if (itc > it_no) break;

                //if (item_quant [k] != 0)
                //{
                //itc++;
                //if (item_x [k] == x_pos && item_y [k] == y_pos)
                //{

               //               strcpy(info [info_lines], "There are several objects herehj.");
               //       incrl();

                        if (keyin != 'a')
                        {
                        strcpy(info [info_lines], "pick up ");

                        if (item_class [o] == 15)
                        {
                                itoa(item_quant [o], st_prn, 10);
                                strcat(info [info_lines], st_prn);
                                strcat(info [info_lines], " gold piece");
                                if (item_quant [o] > 1) strcat(info [info_lines], "s");
                        } else
                        {
                                //item_name(item_class [o], item_type [o], item_dam [o], item_plus [o], item_quant [o], item_ident [o], 3, str_pass);
                                it_name(o, 3);
                                strcat(info [info_lines], str_pass);
                        }
                        //fake_name [k]);
                        //if (item_quant [k] > 1) strcat(info [info_lines], "s");
                        strcat(info [info_lines], "\?");
                        incrl();

                        // test relay_message();
                        }

                if (keyin != 'a') keyin = getch();

                if (keyin == 'q')
                {
                        return;
                }
                if (keyin == 'y' | keyin == 'a')
                        {
                        //o = igrid [x_pos] [y_pos];
                        //int h = 0;

                        //while(h < k)

                        item_got = o;

                        int grunk = add_item();

                        if (grunk == 0)
                                {
                                        strcpy(info [info_lines], "You can't carry that much weight.");
                                        incrl();
                                        keyin = 'x'; // resets from 'a'
                                }
                        if (grunk == 501)
                                {
                                        strcpy(info [info_lines], "You can't carry that many items.");
                                        incrl();
                                        keyin = 'x'; // resets from 'a'
                                }


                                if (grunk != 1) // ie if the item picked up is still there.
                                {
                                        last_item = item_got;
                                }

                                //if (grunk == 1)
                                //{

                                //}
                                //else
                                //{
                                //      item_quant [k] = 0;
                                //}
                        //items_here--;
                        }

                        //{

                                if (item_quant [o] > 0) last_item = o;

                                hrg = item_link [o];
                                o = hrg;
                                if (o == 501) return;

                                //h ++;
                        //}



                //} // end of if (item_x/y)
                //} // end of if item_quant
        if (items_here == 0) break;
        } // end of while k loop

        strcpy(info [info_lines], "That's all.");
} // end of if items_here
//item();
itc = 0;















































/*


if (items_here > 1)
{
        strcpy(info [info_lines], "There are several objects here.");
        incrl();
        for (k = 0; k < ITEMS; k++) // use k because of call to relay_message()
        {
//              if (itc > it_no) break;

                if (item_quant [k] != 0)
                {
                itc++;
                if (item_x [k] == x_pos && item_y [k] == y_pos)
                {

               //               strcpy(info [info_lines], "There are several objects herehj.");
               //       incrl();

                        if (keyin != 'a')
                        {
                        strcpy(info [info_lines], "pick up ");
/*
                        if (item_quant [k] == 1)
                        {
                                if ((item_class [k] != 6 && item_class [k] != 8) && (fake_name [k] [0] == 97 ^ fake_name [k] [0] == 101 ^ fake_name [k] [0] == 105 ^ fake_name [k] [0] == 111 ^ fake_name [k] [0] == 117))
                                        {
                                                strcat(info [info_lines], "an ");
                                        } else strcat(info [info_lines], "a ");
                                        // if (fake_name [i] [0] < 97) strcpy(item_show [counter], " ");

                                        if (item_class [k] == 6)
                                        strcat(info [info_lines], "scroll ");

                                        if (item_class [k] == 8)
                                        strcat(info [info_lines], "bottle of ");

                        } else
                              {
                              itoa(item_quant [k], temp_quant, 10);
                              strcat(info [info_lines], temp_quant);
                              strcat(info [info_lines], " ");

                              if (item_class [k] == 6)
                                strcat(info [info_lines], "scrolls ");

                              if (item_class [k] == 8)
                                strcat(info [info_lines], "bottles of ");
                              }

*//*



                        strcat(info [info_lines], item_name(item_class [k], item_type [k], item_dam [k], item_plus [k], item_quant [k], item_ident [k], 3));

                        //fake_name [k]);
                        //if (item_quant [k] > 1) strcat(info [info_lines], "s");
                        strcat(info [info_lines], "\?");
                        incrl();

                        relay_message();
                        }
                if (keyin != 'a') keyin = getch();

                if (keyin == 'q')
                {
                        return;
                }
                if (keyin == 'y' | keyin == 'a')
                        {
                        item_got = k;
                        if (add_item() != 1)
                                {
                                        strcpy(info [info_lines], "You can't carry that much weight.");
                                        incrl();
                                }
                                //else
                                //{
                                //      item_quant [k] = 0;
                                //}
                        items_here--;
                        }
                } // end of if (item_x/y)
                } // end of if item_quant
        if (items_here == 0) break;
        } // end of while k loop

        strcpy(info [info_lines], "That's all.");
} // end of if items_here
//item();
itc = 0;
*/


} // end of pickup() function










int add_item(void)
{

int quant_got = 0;
int item_mass = 0;
int unit_mass = 0;

int retval = 1;

//int pickloop = 0;

//int last_item = 501;

if (inv_no >= 52)
{
// strcpy(info [info_lines], "You can't carry any more items.");
// incrl();
 return 501;
}

if (item_class [item_got] < 3 | item_class [item_got] == 4)
{
        unit_mass = mass [item_class [item_got]] [item_type [item_got]];// * item_quant [item_got];
} else
      switch(item_class [item_got])
       {
        case 3: unit_mass = 100; break; // * item_quant [item_got]; break;
        case 4: unit_mass = 100; break; // * item_quant [item_got]; break;
        case 5: unit_mass = 200; break; //* item_quant [item_got]; break;
        case 6: unit_mass = 50; break; // * item_quant [item_got]; break;
        case 7: unit_mass = 20; break; // * item_quant [item_got]; break;
        case 8: unit_mass = 60; break; // * item_quant [item_got]; break;
        case 9: unit_mass = 5; break; // * item_quant [item_got]; break;
 case 10: unit_mass = 250; break;
 case 11: unit_mass = 130; break;
        case 15: unit_mass = 0; break; // For now!
//      case 10: item_mass = 100 * item_quant [item_got]; break;
       }

//       unit_mass *= 10;


      item_mass = unit_mass * item_quant [item_got];

// if (mass [item_class [item_got]] [item_type [item_got]] + burden > whatever

quant_got = item_quant [item_got];

brek = 0;

if ((int) burden + item_mass > (int) 1000 + strength * 200) // multiply both constants * 10
{
//      strcpy(info [info_lines], "You can't carry that much stuff.");
//      incrl();

        if (item_quant [item_got] == 1) return 0;

        for (m = item_quant [item_got]; m > 0; m --)
        {
                if (m == 0) break;
                if (unit_mass * m + burden <= 1000 + strength * 200)
                {
                        quant_got = m;
                        brek = 1;
                        item_mass = unit_mass * m;
                        strcpy(info [info_lines], "You can only carry some of what is here.");
                        incrl();
                        break;
                }
        }
        if (brek == 0) return 0;
                else retval = 2;

}

brek = 0;

if (item_class [item_got] == 15)
{
        gp += quant_got; gp_ch = 1;
        strcpy(info [info_lines], "You pick up ");
        itoa(quant_got, st_prn, 10);
        strcat(info [info_lines], st_prn);
        strcat(info [info_lines], " gold pieces.");
        incrl();
        turnover = 1;
 alert();
        goto change_igrid;
}

for (m = 0; m < 52; m++)
{

        if ((item_class [item_got] == 1 | item_class [item_got] == 4 | item_class [item_got] == 6 | item_class [item_got] == 8 | item_class [item_got] == 9) && inv_class [m] == item_class [item_got] && inv_type [m] == item_type [item_got] && inv_plus [m] == item_plus [item_got] && inv_dam [m] == item_dam [item_got] && inv_quant [m] > 0)
        {
//      if (strcmp(inv_name [m], fake_name [item_got]) == 0)
        if (item_ident [item_got] == inv_ident [m])
        {
                inv_quant [m] += quant_got;//item_quant [item_got];
                //burden += item_mass; // (mass [item_class [item_got]] [item_type [item_got]]) * item_quant [item_got];

                burden_change();

                strcpy(info [info_lines], " ");
                        strncpy(info [info_lines], letters [m], 1);
                        strcat(info [info_lines], " - ");

/*
                itoa(inv_quant [m], temp_quant, 10);
                     strcat(info [info_lines], temp_quant);
                     strcat(info [info_lines], " ");

                        if (inv_class [m] == 6)
                     strcat(info [info_lines], "scrolls ");

                        if (inv_class [m] == 8)
                     strcat(info [info_lines], "bottles of ");

*/
                        //item_name(inv_class [m], inv_type [m], inv_dam [m], inv_plus [m], inv_quant [m], inv_ident [m], 3, str_pass);
                        in_name(m, 3);
                        strcat(info [info_lines], str_pass);
//                      if (inv_class [m] != 6 && inv_class [m] != 8) strcat(info [info_lines], "s");
                        incrl();

                //item_quant [item_got] -= quant_got;
                //if (item_quant [item_got] == 0) it_no --;
//              grid [item_x [item_got]] [item_y[item_got]] = 249;
                //grid_col [item_x [item_got]] [item_y[item_got]] = 7;

//              item();
                turnover = 1;
//              item();
  alert();
                goto change_igrid;


                //return retval;
        }
        }
} // end of for m loop.

for (m = 0; m < 52; m++)
{


        if (inv_quant [m] == 0)
        {
                //strcpy(inv_name [m], fake_name [item_got]);
                inv_ident [m] = item_ident [item_got];
                inv_class [m] = item_class [item_got];
                inv_type [m] = item_type [item_got];
                inv_plus [m] = item_plus [item_got];
                inv_dam [m] = item_dam [item_got];
                inv_col [m] = icolour [item_got];
                inv_quant [m] = quant_got;//item_quant [item_got];
                //burden += item_mass;
                burden_change();

                        strcpy(info [info_lines], " ");
                        strncpy(info [info_lines], letters [m], 1);
                        strcat(info [info_lines], " - ");

/*
                 if (item_quant [item_got] == 1)
                 //     surely this should be:
                 //if (inv_quant [m] == 1) // ???? I don't know.
                 {

                        if ((inv_class [m] != 6 && inv_class [m] != 8) && (inv_name [m] [0] == 97 ^ inv_name [m] [0] == 101 ^ inv_name [m] [0] == 105 ^ inv_name [m] [0] == 111 ^ inv_name [m] [0] == 117))
                        {
                                strcat(info [info_lines], "an ");
                        } else
//                      if (inv_name [m] [0] != 97 && inv_name [m] [0] != 101 && inv_name [m] [0] != 105 && inv_name [m] [0] != 111 && inv_name [m] [0] != 117 && inv_name [m] [0] > 96)
                        {
                                strcat(info [info_lines], "a ");
                        }

                        if (inv_class [m] == 6)
                        {
                                strcat(info [info_lines], "scroll ");
                        }
                        if (inv_class [m] == 8)
                        {
                                strcat(info [info_lines], "bottle of ");
                        }

                 } else
                     {
                     itoa(inv_quant [m], temp_quant, 10);
                     strcat(info [info_lines], temp_quant);
                     strcat(info [info_lines], " ");

                     if (inv_class [m] == 6)
                     strcat(info [info_lines], "scrolls ");

                     if (inv_class [m] == 8)
                     strcat(info [info_lines], "bottles of ");
                     }
*/

        //item_name(inv_class [m], inv_type [m], inv_dam [m], inv_plus [m], inv_quant [m], inv_ident [m], 3, str_pass);
        in_name(m, 3);
        strcat(info [info_lines], str_pass);

//      if (inv_quant [m] > 1 && inv_class [m] != 6 && inv_class [m] != 8)
//      strcat(info [info_lines], "s");





                        //strcat(info [info_lines], inv_name [m]);
                        //if (inv_quant [m] != 1) strcat(info [info_lines], "s");
                        incrl();

if (item_class [item_got] == 12 && char_direction == 0)
{
 strcpy(info [info_lines], "Now all you have to do is get back to the surface!");
 incrl();
 char_direction = 1;
 grid [x_pos] [y_pos] = 18;
}



                //igrid [x_pos] [y_pos] = 501;





//              it_no --;

                                // Only reset last_item if answered no to a
                                // pickup query, or if didn't pick up all of
                                // an item due to weight.


//              grid [item_x [item_got]] [item_y[item_got]] = 249;
                //grid_col [item_x [item_got]] [item_y[item_got]] = 7;

//              item();

                inv_no++; // increases number of items in inventory


              break;
        }




} // end of for m loopy thing.
turnover = 1;
//item();





        //  Paste here!



change_igrid:


//cout << item_link [item_got] << "XXX"; keyin = getch();

                item_quant [item_got] -= quant_got;//= 0;

                if (item_quant [item_got] == 0)
                {
                        if (last_item == 501)
                        {
                                igrid [x_pos] [y_pos] = item_link [item_got];
                                //cout << igrid [x_pos] [y_pos] << "zzz"; keyin = getch();
                        } else
                        {
                                item_link [last_item] = item_link [item_got];
                                //cout << last_item << "ccc"; keyin = getch();
                        }

                it_no --;
                }









return retval;
} // end of int add_item() function






void drop(void)
{

unsigned char nthing;

x_plos = x_pos;
y_plos = y_pos;

if (inv_no == 0)
        {
        strcpy(info [info_lines], "You aren't carrying anything.");
        incrl();
        return;
        }

query2 : strcpy(info [info_lines], "Drop which item?");
incrl();

// test relay_message();

query : keyin = get_ch();     // <-- Label 'query' here!!!!

if (keyin == '$')
{

quant_drop = gp; // for now.

if (gp == 0)
{
        strcpy(info [info_lines], "You don't have any money.");
        incrl();
        return;
}

strcpy(info [info_lines], "You drop ");
itoa(gp, temp_quant, 10);
strcat(info [info_lines], temp_quant);

if (gp > 1) strcat(info [info_lines], " gold pieces.");
            else strcat(info [info_lines], " gold piece.");

incrl();
//}

if (igrid [x_plos] [y_plos] != 501)
{
        if (item_class [igrid [x_plos] [y_plos]] == 15)
                {
                        item_quant [igrid [x_plos] [y_plos]] += quant_drop;
                        gp -= quant_drop; gp_ch = 1;
                        return;
                }
}


for (i = 0; i < ITEMS; i++)
{
        if (item_quant [i] == 0)
        {
//              strcpy(fake_name [i], inv_name [item_drop_2]);
                item_ident [i] = 0;
                item_class [i] = 15;
                //item_type [i] = inv_type [item_drop_2];
                //item_plus [i] = inv_plus [item_drop_2];
                //item_dam [i] = inv_dam [item_drop_2];
                //icolour [i] = inv_col [item_drop_2];
                //item_x [i] = x_plos; item_y [i] = y_plos; //x place position
                item_quant [i] = quant_drop;
                it_no ++;
                break;
        }
} // end of i loop


m = igrid [x_plos] [y_plos];

igrid [x_plos] [y_plos] = i;
item_link [i] = m;

gp -= quant_drop; gp_ch = 1;

return;

/*
for (i = 0; i < ITEMS; i++)
{
        if (itc > it_no) break;

        if (item_quant [i] != 0)
        {
        itc++;

        if (item_class [i] == 15 && item_x [i] == x_pos && item_y [i] == y_pos)
        {
                item_class [i] = 15;
                item_x [i] = x_pos; item_y [i] = y_pos; //x place position
                item_quant [i] += gp;
                gp = 0; gp_ch = 1;
                //it_no++;
                return;
        }

        }
}
itc = 0;

for (i = 0; i < ITEMS; i++)
{
//if (i > it_no) break;

if (item_quant [i] == 0)
        {
                item_class [i] = 15;
                //item_x [i] = x_pos; item_y [i] = y_pos; //x place position
                item_quant [i] = gp;
                gp = 0; gp_ch = 1;
                it_no++;
                return;
        }

}
*/


}


if (keyin == '?' | keyin == '*')
{
        nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {

           goto query2;

                /*strcpy(info [info_lines], "Drop which item?"); incrl();
                relay_message();
                keyin = get_ch();*/
        }

//      inventory(-1);
//      strcpy(info [info_lines], "Drop which object?");
//      incrl();
//      relay_message();
                //keyin = getch();
//      goto query;
}
item_drop_1 = (int) keyin;

quant_drop = 0;

if (item_drop_1 > 47 && item_drop_1 < 58)
{
        quant_drop = item_drop_1 - 48;
        keyin = get_ch();
        item_drop_1 = (int) keyin;
        if (item_drop_1 > 47 && item_drop_1 < 58)
                {
                quant_drop = (quant_drop * 10 + (item_drop_1 - 48));
                keyin = get_ch();
                item_drop_1 = (int) keyin;
                }
} //else quant_drop = inv_quant [item_drop_1];

if ((item_drop_1 < 65 | (item_drop_1 > 90 && item_drop_1 < 97) | item_drop_1 > 122)) // && item_drop_1 != '$')
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        // maybe a relay_message() call?
        return;
}

//if (item_drop_1 != '$')
//{
        item_drop_2 = conv_lett(item_drop_1);
        if (quant_drop == 0) quant_drop = inv_quant [item_drop_2];


        if (inv_quant [item_drop_2] == 0)
        {
                strcpy(info [info_lines], "You don't have any such object.");
                incrl();
                return;
        }

        //if (item_drop_2 == armour [0])
        for (i = 0; i < 6; i++)
        {
                if (item_drop_2 == armour [i] && armour [i] != -1)
                {
                        strcpy(info [info_lines], "You will have to take that off first.");
                        incrl();
                        return;
                }
        }

        if (item_drop_2 == ring [0] | item_drop_2 == ring [1])
        {
                strcpy(info [info_lines], "You will have to take that off first.");
                incrl();
                return;
        }

        if (item_drop_2 == item_wielded && inv_class [item_drop_2] == 0 && inv_plus [item_drop_2] >= 130)
        {
                strcpy(info [info_lines], "That object is stuck to you!");
                incrl();
                return;
        }

        if (quant_drop > inv_quant [item_drop_2]) quant_drop = inv_quant [item_drop_2];

        strcpy(info [info_lines], "You drop ");

/*

        if (quant_drop == 1)
        {
        if ((inv_class [item_drop_2] != 6 && inv_class [item_drop_2] != 8) && (inv_name [item_drop_2] [0] == 97 ^ inv_name [item_drop_2] [0] == 101 ^ inv_name [item_drop_2] [0] == 105 ^ inv_name [item_drop_2] [0] == 111 ^ inv_name [item_drop_2] [0] == 117))
                {
                        strcat(info [info_lines], "an ");
                } else
//              if (inv_name [item_drop_2] [0] != 97 && inv_name [item_drop_2] [0] != 101 && inv_name [item_drop_2] [0] != 105 && inv_name [item_drop_2] [0] != 111 && inv_name [item_drop_2] [0] != 117 && inv_name [item_drop_2] [0] > 96)
                {
                        strcat(info [info_lines], "a ");
                }
        } else
             {
             itoa(quant_drop, temp_quant, 10);
             strcat(info [info_lines], temp_quant);
             strcat(info [info_lines], " ");
             }

             if (inv_class [item_drop_2] == 6)
             {
             strcat(info [info_lines], "scroll");
             if (inv_quant [item_drop_2] > 1)
             strcat(info [info_lines], "s ");
             else strcat(info [info_lines], " ");
             }

             if (inv_class [item_drop_2] == 8 && quant_drop == 1)
             {
                strcat(info [info_lines], "bottle of ");
             }
             if (inv_class [item_drop_2] == 8 && quant_drop > 1)
             {
                strcat(info [info_lines], "bottles of ");
             }

             //strcat(info [info_lines], inv_name [item_drop_2]);

             //if (inv_quant [item_drop_2] != 1 && inv_class [item_drop_2] != 6 && inv_class [item_drop_2] != 8)

             //strcat(info [info_lines], "s");

*/

        item_name(inv_class [item_drop_2], inv_type [item_drop_2], inv_dam [item_drop_2], inv_plus [item_drop_2], quant_drop, inv_ident [item_drop_2], 3, str_pass);
        //in_name(item_drop_2, 3);
        strcat(info [info_lines], str_pass);

//} // end of if != '$'
        /*else
           {
                item_drop_2 = '$';
                if (quant_drop == 0) quant_drop = gp;
                strcpy(info [info_lines], "You drop ");
                itoa(quant_drop, st_prn, 10);
                strcat(info [info_lines], st_prn);
                strcat(info [info_lines], " gold piece");
                if (quant_drop > 1) strcat(info [info_lines], "s");
                // incrl() is below.
           }*/




//      if (quant_drop != 1 && inv_class [item_drop_2] != 6 && inv_class [item_drop_2] != 8)
//      strcat(info [info_lines], "s");

        strcat(info [info_lines], ".");
        incrl();

        x_plos = x_pos; y_plos = y_pos;
        item_place();

        inv_quant [item_drop_2] -= quant_drop;
//      if (quant_drop == inv_quant [item_drop_2]) inv_no--;
        if (inv_quant [item_drop_2] <= 0) inv_no--;


        burden_change();

        if (item_drop_2 == item_wielded)
        {
  unwield_item(item_drop_2);
                item_wielded = -1;
                strcpy(info [info_lines], "You are empty handed.");
                incrl();
                damage = nat_damage;
                damage_ch = 1;
        }
} // end of drop func.


int conv_lett(int item_drop_3)
{
        if (item_drop_3 > 96 && item_drop_3 < 123)
        {
        item_drop_3 -= 97;
        }

        if (item_drop_3 > 64 && item_drop_3 < 91)
        {
        item_drop_3 -= 39;
        }

return item_drop_3;

} // end of conv_lett func




void item_place(void)
{

int item_mass = 0;
int unit_mass = 0;

if (inv_class [item_drop_2] < 3 | inv_class [item_drop_2] == 4)
{
        unit_mass = mass [inv_class [item_drop_2]] [inv_type [item_drop_2]];
} else
       switch(inv_class [item_drop_2])
       {
        case 3: unit_mass = 100; break; // * item_quant [item_got]; break;
        case 4: unit_mass = 100; break; // * item_quant [item_got]; break;
        case 5: unit_mass = 200; break; //* item_quant [item_got]; break;
        case 6: unit_mass = 50; break; // * item_quant [item_got]; break;
        case 7: unit_mass = 20; break; // * item_quant [item_got]; break;
        case 8: unit_mass = 60; break; // * item_quant [item_got]; break;
        case 9: unit_mass = 5; break; // * item_quant [item_got]; break;
 case 10: unit_mass = 250; break;
 case 11: unit_mass = 130; break;
// case
//      case 10: item_mass = 100 * item_quant [item_got]; break;
       }

//       unit_mass *= 10;
       item_mass = unit_mass * quant_drop;

       //burden -= item_mass;
       // burden_change(); ... later


//while (i != 501)
//{



//cout << "Starting link...";

//int b = 0;
//int n = 0;
//int m = 0;

int b = 0;
//n = 0;
m = 0;


if (igrid [x_plos] [y_plos] != 501)
{
        if ((inv_class [item_drop_2] == 1 | inv_class [item_drop_2] == 4 | inv_class [item_drop_2] == 6 | inv_class [item_drop_2] == 8 | inv_class [item_drop_2] == 9) && inv_class [item_drop_2] == item_class [igrid [x_plos] [y_plos]] && inv_type [item_drop_2] == item_type [igrid [x_plos] [y_plos] ] && inv_plus [item_drop_2] == item_plus [igrid [x_plos] [y_plos]] && inv_dam [item_drop_2] == item_dam [igrid [x_plos] [y_plos]] && item_quant [igrid [x_plos] [y_plos]] > 0)
        {
//      if (strcmp(inv_name [item_drop_2], fake_name [i]) == 0);// && x_plos == item_x [i] && y_plos == item_y [i])
        if (inv_ident [item_drop_2] == item_ident [igrid [x_plos] [y_plos] ])
        {
                item_quant [igrid [x_plos] [y_plos]] += quant_drop;
                //burden_change();
                turnover = 1;
                return;
        }
        }
}




/*
for (b = 0; b < ITEMS; b ++)
{
        if (item_quant [b] != 0)
        {


                break;
        }
} // end of for b*/


for (i = 0; i < ITEMS; i++)
{
 if (i >= 480)
 {
  strcpy(info [info_lines], "The demon of the infinite void grins at you.");
  incrl();
  return;
 }
        if (item_quant [i] == 0)
        {
//              strcpy(fake_name [i], inv_name [item_drop_2]);
                item_ident [i] = inv_ident [item_drop_2];
                item_class [i] = inv_class [item_drop_2];
                item_type [i] = inv_type [item_drop_2];
                item_plus [i] = inv_plus [item_drop_2];
                item_dam [i] = inv_dam [item_drop_2];
                icolour [i] = inv_col [item_drop_2];
                //item_x [i] = x_plos; item_y [i] = y_plos; //x place position
                item_quant [i] = quant_drop;
                it_no ++;
                break;
        }
} // end of i loop


m = igrid [x_plos] [y_plos];

igrid [x_plos] [y_plos] = i;
item_link [i] = m;


/*
//for (i = 0; i < ITEMS; i ++)
//{
        //if (item_quant [i] == 0) continue;

        if (igrid [x_plos] [y_plos] == 501)
        {
                igrid [x_plos] [y_plos] = item_drop_2;
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
                n = item_link [b];
                b = n;
        } // end of while

        item_link [m] = i;

        //cout << "\n\rLinking " << i << " to " << m;
//      char gtot = getch();
        }
//} // end of for i





*/














//} // end of while i













/*
for (i = 0; i < ITEMS; i++)
{
        if ((inv_class [item_drop_2] == 1 | inv_class [item_drop_2] == 4 | inv_class [item_drop_2] == 6 | inv_class [item_drop_2] == 8 | inv_class [item_drop_2] == 9) && (item_x [i] == x_plos) && item_y [i] == y_plos && inv_class [item_drop_2] == item_class [i] && inv_type [item_drop_2] == item_type [i] && inv_plus [item_drop_2] == item_plus [i] && inv_dam [item_drop_2] == item_dam [i] && item_quant [i] > 0)
        {
//      if (strcmp(inv_name [item_drop_2], fake_name [i]) == 0);// && x_plos == item_x [i] && y_plos == item_y [i])
        if (inv_ident [item_drop_2] == item_ident [i])
        {
                item_quant [i] += quant_drop;
                return;
        }
        }
}

*/










//textbackground(3);
//if (i == 199) cout << "huh?";

//item();

turnover = 1;

} // end of item_place.



void burden_change(void)
{

char old_burden = burden_state;

burden = 0;

char bu = 0;

/*for (bu = 0; bu < 52; bu ++)
{
        switch(inv_class [bu]);


}*/


for (bu = 0; bu < 52; bu ++)
{
//      strcpy(inv_name [i], ident(inv_class [i], inv_type [i], inv_plus [i]));
//      inv_ident [i] = 2;

        if (inv_quant [bu] < 1) continue;

        //if (inv_class [bu] < 3)
        //{
                burden += mass [inv_class [bu]] [inv_type [bu]] * inv_quant [bu];
        /*} else
              switch(inv_class [bu])
               {
                case 3: burden += (int) 100 * inv_quant [bu]; break;
                case 4: burden += (int) 100 * inv_quant [bu]; break;
                case 5: burden += (int) 200 * inv_quant [bu]; break;
                case 6: burden += (int) 50 * inv_quant [bu]; break;
                case 7: burden += (int) 20 * inv_quant [bu]; break;
                case 8: burden += (int) 60 * inv_quant [bu]; break;
                case 9: burden += (int) 5 * inv_quant [bu]; break;
               }*/
/*      itoa(bu, st_prn, 10);
        strcpy(info [info_lines], st_prn);
        itoa(burden, st_prn, 10);
        strcat(info [info_lines], ", ");
        strcat(info [info_lines], st_prn);
        incrl();*/

}


burden_state = 0;
burden_ch = 1;

if (burden < 1000 + strength * 200 - 1000)
{
 burden_state = 0; // unencumbered
 return;
}

if (burden < 1000 + strength * 200 - 500)
{
 burden_state = 2; // encumbered
 if (old_burden != burden_state)
 {
  strcpy(info [info_lines], "You are being weighed down by all of your possessions.");
  incrl();
 }
 return;
}

burden_state = 5;
if (old_burden != burden_state)
{
 strcpy(info [info_lines], "You are being crushed by all of your possessions.");
 incrl();
}

}





void wield_weapon(void)
{

unsigned char nthing = 0;
unsigned char i_dam = 0;

if (inv_no == 0)
        {
        strcpy(info [info_lines], "You aren't carrying anything.");
        incrl();
        return;
        }

if (berserker != 0)
{
 strcpy(info [info_lines], "You are too berserk!");
 incrl();
 return;
}

if ((item_wielded != -1) && inv_class [item_wielded] == 0 && inv_plus [item_wielded] > 80)
{
        strcpy(info [info_lines], "You can't unwield your weapon to draw a new one!");
        incrl();
        return;
}


query : strcpy(info [info_lines], "Wield which item (- for none)?");
incrl();

// test relay_message();

keyin = get_ch();

if (keyin == '?')
{
        //inventory(0);
        nthing = invent(0, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
      goto query;
        /*              strcpy(info [info_lines], "Wield which item?"); incrl();
                relay_message();
                keyin = get_ch();*/
        }
}

if (keyin == '*')
{
        //inventory(-1);
        nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
                strcpy(info [info_lines], "Wield which item?"); incrl();
                // test relay_message();
                keyin = get_ch();
        }
}


item_wield_1 = (int) keyin;

if (keyin == '-')
{
 damage = nat_damage;
 if (item_wielded != -1) unwield_item(item_wielded);
 item_wielded = -1;
 strcpy(info [info_lines], "You are empty handed.");
 incrl();
 damage_ch = 1;
 return;
}

if (item_wield_1 < 65 | (item_wield_1 > 90 && item_wield_1 < 97) | item_wield_1 > 122)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

item_wield_2 = conv_lett(item_wield_1);

if (item_wield_2 == item_wielded)
{
        strcpy(info [info_lines], "You are already wielding that!");
        incrl();
        return;
}

if (item_wield_2 == armour [0] | item_wield_2 == armour [5] | item_wield_2 == armour [2] | item_wield_2 == armour [1] | item_wield_2 == armour [4] | item_wield_2 == armour [3]) // later add:   | == helmet etc
{
        strcpy(info [info_lines], "You are wearing that object!");
        incrl();
        return;
}


if (inv_quant [item_wield_2] == 0)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}                                // 2-hands 7,10,12, {20,21}

if (inv_class [item_wield_2] != 0)
{
 if (inv_class [item_wield_2] == 11 && armour [5] != -1)
 {
      strcpy(info [info_lines], "You can't wield that with a shield.");
      incrl();
      return;
 }

        damage = 1;
        //damage = mass [inv_class [item_wield_2]] [inv_type [item_wield_2]] / 100 + 1;
 if (item_wielded != -1) unwield_item(item_wielded);
        item_wielded = item_wield_2;
//      speed =
        // Not finished.
} else
        {

   if (mass [inv_class [item_wield_2]] [inv_type [item_wield_2]] >= 500)
   {
      strcpy(info [info_lines], "That's too heavy for you to wield.");
      incrl();
      return;
   }


   if ((inv_type [item_wield_2] == 7 | inv_type [item_wield_2] == 10 | inv_type [item_wield_2] == 12 | inv_type [item_wield_2] == 17 | inv_type [item_wield_2] == 18 | inv_type [item_wield_2] == 19 | inv_type [item_wield_2] == 20 | inv_type [item_wield_2] == 21) && armour [5] != -1)
   {
      strcpy(info [info_lines], "You can't wield that with a shield.");
      incrl();
      return;
   }

 if (item_wielded != -1) unwield_item(item_wielded);
        item_wielded = item_wield_2;
        }

strcpy(info [info_lines], " ");
                        strncpy(info [info_lines], letters [item_wield_2], 1);
                        strcat(info [info_lines], " - ");

/*

                        if (inv_class [item_wield_2] != 6 && inv_class [item_wield_2] != 8 && (inv_name [item_wield_2] [0] == 97 ^ inv_name [item_wield_2] [0] == 101 ^ inv_name [item_wield_2] [0] == 105 ^ inv_name [item_wield_2] [0] == 111 ^ inv_name [item_wield_2] [0] == 117))
                        {
                                strcat(info [info_lines], "an ");
                        }
                        if ((inv_class [item_wield_2] == 6 | inv_class [item_wield_2] == 8 | inv_name [item_wield_2] [0] != 97 && inv_name [item_wield_2] [0] != 101 && inv_name [item_wield_2] [0] != 105 && inv_name [item_wield_2] [0] != 111 && inv_name [item_wield_2] [0] != 117 && inv_name [item_wield_2] [0] > 96) | inv_name [item_wield_2] [0] == 43 | inv_name [item_wield_2] [0] == 45)
                        {
                                strcat(info [info_lines], "a ");
                        }

                        if (inv_class [item_wield_2] == 6)
                        strcat(info [info_lines], "scroll ");

                        if (inv_class [item_wield_2] == 8)
                        strcat(info [info_lines], "bottle of ");

                        strcat(info [info_lines], inv_name [item_wield_2]);
                        //if (inv_quant [j] != 1) strcat(info [info_lines], "s");

*/
                        //item_name(inv_class [item_wield_2], inv_type [item_wield_2], inv_dam [item_wield_2], inv_plus [item_wield_2], inv_quant [item_wield_2], inv_ident [item_wield_2], 3, str_pass);
                        in_name(item_wield_2, 3);
                        strcat(info [info_lines], str_pass);


                        strcat(info [info_lines], " (weapon in hand)");
                        incrl();




damage_ch = 1;
turnover = 1;

if (inv_plus [item_wield_2] > 80 && inv_class [item_wield_2] == 0)
{
        strcpy(info [info_lines], "It sticks to your hand!");
// if (inv_ident [item_wield_2] == 0)   inv_ident [item_wield_2] = 2;
        incrl();
}

 if (inv_class [item_wield_2] == 0 && inv_ident [item_wield_2] == 0)    inv_ident [item_wield_2] = 2;

if (inv_class [item_wield_2] == 0 && inv_dam [item_wield_2] != 0)
{

if (inv_ident [item_wield_2] == 1) inv_ident [item_wield_2] = 2;

i_dam = inv_dam [item_wield_2];

if (inv_dam [item_wield_2] <= 180)
{
 i_dam = inv_dam [item_wield_2] % 30;
}

switch(i_dam) //inv_dam [item_wield_2] % 30)
{

  case 1:
  strcpy(info [info_lines], "It bursts into flame!");
  incrl();
  break;

  case 2:
  strcpy(info [info_lines], "It glows with a cold blue light!");
  incrl();
  break;

  case 3:
  strcpy(info [info_lines], "It softly glows with a divine radiance!");
  incrl();
  break;

  case 4:
  strcpy(info [info_lines], "You hear the crackle of electricity.");
  incrl();
  break;

  case 5:
  strcpy(info [info_lines], "You feel a sudden desire to kill orcs!");
  incrl();
  break;

  case 6:
  strcpy(info [info_lines], "It begins to drip with poison!");
  incrl();
  break;

  case 7:
  strcpy(info [info_lines], "You feel protected!");
  incrl();
  AC += 3;
  AC_ch = 1;
  break;

  case 8:
  strcpy(info [info_lines], "You sense an unholy aura.");
                incrl();
                break;

  case 181:
  strcpy(info [info_lines], "The Singing Sword hums in delight!");
                incrl();
  special_wield = 1;
                break;

  case 182:
  strcpy(info [info_lines], "You feel bloodthirsty!");
                incrl();
  special_wield = 2;
                break;

  case 183:
  strcpy(info [info_lines], "A shiver runs down your spine.");
                incrl();
  special_wield = 3;
                break;

  case 184:
//  strcpy(info [info_lines], "You feel bloodthirsty!");
//              incrl();
  special_wield = 4;
                break;

  case 185:
  strcpy(info [info_lines], "You feel pruney.");
                incrl();
  special_wield = 0;
                break;


} // end of switch

}

if (inv_class [item_wield_2] == 11)
{
 switch(inv_type [item_wield_2])
        {
 case 0: // wizardry
 mag_abil += 30;
        break;

 case 1: // magic
 ep_max += 13;
 ep_ch = 1;
 magic_battery ++;
 break;

 case 2: // fire
        spec_fire ++;
        break;

        case 3: // cold
 spec_cold ++;
        break;

 case 4:
 spec_poison ++;
        break;

 case 5:
 energy ++;
 break;

 }
}

/*if (inv_dam [item_wield_2] == 6)
{
  strcpy(info [info_lines], "You feel protected.");
  incrl();
  if (inv_ident [item_wield_2] == 1) inv_ident [item_wield_2] = 2;
}*/


} // end of wield_weapon()








void wear_armour(void)
{

unsigned char nthing;

if (inv_no == 0)
        {
        strcpy(info [info_lines], "You aren't carrying anything.");
        incrl();
        return;
        }

if (berserker != 0)
{
 strcpy(info [info_lines], "You are too berserk!");
 incrl();
 return;
}

query : strcpy(info [info_lines], "Wear which item?");
incrl();

// test relay_message();

keyin = get_ch();

if (keyin == '?')
{
        nthing = invent(2, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
   goto query;
        /*              strcpy(info [info_lines], "Wear which item?"); incrl();
                relay_message();
                keyin = get_ch();*/
        }
}

if (keyin == '*')
{
        nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
                strcpy(info [info_lines], "Wear which item?"); incrl();
                // test relay_message();
                keyin = get_ch();
        }
}


armour_wear_1 = (int) keyin; // have to do shield, helmet etc later

/*if (keyin == '-')
{
damage = 1;
item_wielded = -1;
strcpy(info [info_lines], "You are empty handed.");
incrl();
damage_ch = 1;
return;
}*/

if (armour_wear_1 < 65 | (armour_wear_1 > 90 && armour_wear_1 < 97) | armour_wear_1 > 122)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

/*
int armour [0] = -1;
int armour [1] = -1;
int armour [2] = -1;
int armour [3] = -1;
int armour [4] = -1;
int armour [5] = -1;
*/

armour_wear_2 = conv_lett(armour_wear_1);

if (armour_wear_2 == armour [0] | armour_wear_2 == armour [1] | armour_wear_2 == armour [5] | armour_wear_2 == armour [2] | armour_wear_2 == armour [3] | armour_wear_2 == armour [4]) // or armour_wear_w == helmet etc
{
        strcpy(info [info_lines], "You are already wearing that!");
        incrl();
        return;
}

if (inv_quant [armour_wear_2] == 0)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

if (armour_wear_2 == item_wielded)
{
        strcpy(info [info_lines], "You are wielding that object!");
        incrl();
        return;
}

if (inv_class [armour_wear_2] == 2 && inv_type [armour_wear_2] < 8 && armour [1] != -1)
{
        strcpy(info [info_lines], "You can't wear that over your cloak.");
        incrl();
        return;
}


if (inv_class [armour_wear_2] == 2 && inv_type [armour_wear_2] < 8 && armour [0] != -1)
{
        strcpy(info [info_lines], "You are already wearing some body armour.");
        incrl();
        return;
}


if (inv_class [armour_wear_2] == 2 && (inv_type [armour_wear_2] == 8 | inv_type [armour_wear_2] == 14 | inv_type [armour_wear_2] == 13) && armour [5] != -1)
{
        strcpy(info [info_lines], "You are already holding a shield.");
        incrl();
        return;
}

if (item_wielded != -1 && (inv_type [armour_wear_2] == 8 | inv_type [armour_wear_2] == 13 | inv_type [armour_wear_2] == 14) && (inv_class [item_wielded] == 0 && (inv_type [item_wielded] == 7 | inv_type [item_wielded] == 10 | inv_type [item_wielded] == 12 | inv_type [item_wielded] == 17 | inv_type [item_wielded] == 18 | inv_type [item_wielded] == 19 | inv_type [item_wielded] == 20 | inv_type [item_wielded] == 21)) | inv_class [item_wielded] == 11)
{
   strcpy(info [info_lines], "You can't wear a shield with a two-handed weapon.");
   incrl();
   return;
}

if (inv_class [armour_wear_2] == 2 && inv_type [armour_wear_2] == 10 && armour [2] != -1)
{
        strcpy(info [info_lines], "You are already wearing a helmet.");
        incrl();
        return;
}

if (inv_class [armour_wear_2] == 2 && inv_type [armour_wear_2] == 9 && armour [1] != -1)
{
        strcpy(info [info_lines], "You are already wearing a cloak.");
        incrl();
        return;
}

if (inv_class [armour_wear_2] == 2 && inv_type [armour_wear_2] == 11 && armour [3] != -1)
{
        strcpy(info [info_lines], "You are already wearing some gloves.");
        incrl();
        return;
}

if (inv_class [armour_wear_2] == 2 && inv_type [armour_wear_2] == 12 && armour [4] != -1)
{
        strcpy(info [info_lines], "You are already wearing some boots.");
        incrl();
        return;
}


/*
//strcpy(all_items [2] [0], "robe");
        property [2] [0] [0] = 1; // AC
        property [2] [0] [1] = 0; // evasion
        //icolour [2] [0] = 6;

        //strcpy(all_items [2] [1], "leather armour");
        property [2] [1] [0] = 2; // AC
        property [2] [1] [1] = -1; // evasion
        //icolour [2] [1] = 6;

        //strcpy(all_items [2] [2], "ring mail");
        property [2] [2] [0] = 3; // AC
        property [2] [2] [1] = -2; // evasion
        //icolour [2] [2] = 11;

        //strcpy(all_items [2] [2], "scale mail");
        property [2] [2] [0] = 4; // AC
        property [2] [2] [1] = -2; // evasion
        //icolour [2] [2] = 11;

        //strcpy(all_items [2] [3], "chain mail");
        property [2] [3] [0] = 5; // AC
        property [2] [3] [1] = -3; // evasion
        //icolour [2] [3] = 11;

        //strcpy(all_items [2] [4], "splint mail");
        property [2] [4] [0] = 6; // AC
        property [2] [4] [1] = -5; // evasion
        //icolour [2] [4] = 11;

        //strcpy(all_items [2] [5], "banded mail");
        property [2] [5] [0] = 6; // AC
        property [2] [5] [1] = -4; // evasion
        //icolour [2] [5] = 11;

        //strcpy(all_items [2] [6], "plate mail");
        property [2] [6] [0] = 7; // AC
        property [2] [6] [1] = -5; // evasion
        //icolour [2] [6] = 11;

        // other armour:

        // shield:
        property [2] [7] [0] = 1; // AC
        property [2] [7] [1] = 2; // evasion

        // helmet
        property [2] [8] [0] = 1; // AC
        property [2] [8] [1] = 0; // evasion

        // cloak
        property [2] [9] [0] = 1; // AC
        property [2] [9] [1] = 0; // evasion

        // gloves
        property [2] [10] [0] = 1; // AC
        property [2] [10] [1] = 0; // evasion

        // boots
        property [2] [11] [0] = 1; // AC
        property [2] [11] [1] = 0; // evasion
*/

if (inv_class [armour_wear_2] != 2)
{
        strcpy(info [info_lines], "You can't wear that thing.");
        incrl();
        return;
        //damage = 1;
        //damage = mass [inv_class [item_wield_2]] [inv_type [item_wield_2]] / 100 + 1;
        //item_wielded = item_wield_2;
//      speed =
        // Not finished.
} else
 if (inv_type [armour_wear_2] == 8 | inv_type [armour_wear_2] == 13 | inv_type [armour_wear_2] == 14)
 {
   shield_class = 0; // this is later
        if (inv_plus [armour_wear_2] > 130)
                shield_class += inv_plus [armour_wear_2] - 150;
         else
        shield_class += inv_plus [armour_wear_2] - 50;
 } else
        {
        if (inv_plus [armour_wear_2] > 130)
                AC += property [inv_class [armour_wear_2]] [inv_type [armour_wear_2]] [0] + (inv_plus [armour_wear_2] - 150);
         else
        AC += property [inv_class [armour_wear_2]] [inv_type [armour_wear_2]] [0] + inv_plus [armour_wear_2] - 50;

//      evasion += property [inv_class [armour_wear_2]] [inv_type [armour_wear_2]] [1];
if (clas == 0 | clas == 2) // fighter or priest
{
 evasion += property [inv_class [armour_wear_2]] [inv_type [armour_wear_2]] [1] / 2;
// if (inv_type [armour_wear_2] == 8) evasion += 1;
} else
      {
       evasion += property [inv_class [armour_wear_2]] [inv_type [armour_wear_2]] [1];
//       if (inv_type [armour_wear_2] == 8) evasion -= 1;
      }

        //damage = property [inv_class [item_wield_2]] [inv_type [item_wield_2]] [0];
        //item_wielded = item_wield_2;
        }

strcpy(info [info_lines], " ");
                        strncpy(info [info_lines], letters [armour_wear_2], 1);
                        strcat(info [info_lines], " - ");

/*
                        if (inv_name [armour_wear_2] [0] == 97 ^ inv_name [armour_wear_2] [0] == 101 ^ inv_name [armour_wear_2] [0] == 105 ^ inv_name [armour_wear_2] [0] == 111 ^ inv_name [armour_wear_2] [0] == 117)
                        {
                                strcat(info [info_lines], "an ");
                        }
                        if ((inv_name [armour_wear_2] [0] != 97 && inv_name [armour_wear_2] [0] != 101 && inv_name [armour_wear_2] [0] != 105 && inv_name [armour_wear_2] [0] != 111 && inv_name [armour_wear_2] [0] != 117 && inv_name [armour_wear_2] [0] > 96) | inv_name [armour_wear_2] [0] == 43 | inv_name [armour_wear_2] [0] == 45)
                        {
                                strcat(info [info_lines], "a ");
                        }
*/
                        //item_name(inv_class [armour_wear_2], inv_type [armour_wear_2], inv_dam [armour_wear_2], inv_plus [armour_wear_2], inv_quant [armour_wear_2], inv_ident [armour_wear_2], 3, str_pass);



if (inv_type [armour_wear_2] < 8) armour [0] = armour_wear_2;
 else
switch (inv_type [armour_wear_2])
{
case 13:armour [5] = armour_wear_2;
shield_class += 3; // because takes account of plusses above
break;

case 14:armour [5] = armour_wear_2;
shield_class += 7;
break;

case 8: armour [5] = armour_wear_2;
shield_class += 5;
break;
case 9: armour [1] = armour_wear_2; break;
case 10: armour [2] = armour_wear_2; break;
case 11: armour [3] = armour_wear_2; break;
case 12: armour [4] = armour_wear_2; break;
//case 10: armour [5] = armour_wear_2;
}

AC_ch = 1;
evasion_ch = 1;
turnover = 1;
delay_t = property [inv_class [armour_wear_2]] [inv_type [armour_wear_2]] [0];
if (inv_type [armour_wear_2] == 8) delay_t = 0; // shield
delay_doing = 1;

if (inv_ident [armour_wear_2] < 3) inv_ident [armour_wear_2] = 3;

                        in_name(armour_wear_2, 3);
                        strcat(info [info_lines], str_pass);
                        //if (inv_quant [j] != 1) strcat(info [info_lines], "s");

                        strcat(info [info_lines], " (being worn)");
                        incrl();

if (inv_dam [armour_wear_2] != 0)
{

//if (inv_ident [item_wield_2] == 1) inv_ident [item_wield_2] = 2;

//      Should I make these increase food consumption?

switch(inv_dam [armour_wear_2] % 30)
{

  case 1:
  strcpy(info [info_lines], "You feel quick on your feet.");
  incrl();
  if (fast_run > 0) fast_run = 0;
  fast_run --;
  break;

  case 2:
  strcpy(info [info_lines], "You feel resistant to fire."); // What a boring message
  incrl();
  if (res_fire > 0) res_fire = 0;
  res_fire --;
  break;

  case 3:
  strcpy(info [info_lines], "You feel resistant to cold.");
  incrl();
  if (res_cold > 0) res_cold = 0;
  res_cold --;
  break;

  case 4:
  strcpy(info [info_lines], "You feel healthy.");
  incrl();
  if (res_poison > 0) res_poison = 0;
  res_poison --;
  break;

  case 5:
  strcpy(info [info_lines], "You feel perceptive.");
  incrl();
  if (see_invis > 0) see_invis = 0;
  see_invis --;
  break;

  case 6:
 if (invis > 0) invis = 0;
           else if (invis == 0)
           {
                strcpy(info [info_lines], "You fade into invisibility.");
                incrl();
  inv_ident [ring_wear_2] = 3;
           }
        invis --;
        hunger_inc += 7;
 break;

 case 7:
 strcpy(info [info_lines], "You feel stronger.");
 incrl();
 strength += 3;
 max_strength += 3;
 strength_ch = 1;
 break;

 case 8:
 strcpy(info [info_lines], "You feel dextrous.");
 incrl();
 dex += 3;
 max_dex += 3;
 dex_ch = 1;
 break;

 case 9:
 strcpy(info [info_lines], "You feel intelligent."); // inspired messages, aren't they?
 incrl();
 intel += 3;
 max_intel += 3;
 intel_ch = 1;
 break;

 case 10:
 strcpy(info [info_lines], "You feel rather ponderous.");
 speed += 2;
 evasion -= 1;
 evasion_ch = 1;
 break;

        case 11: // levitation
        if (lev > 0) lev = 0;
           else if (lev == 0)
           {
                strcpy(info [info_lines], "You feel very buoyant!");
                incrl();
                strcpy(info [info_lines], "You gently float upwards from the floor.");
                incrl();
           }
        lev --;
        break;

 case 12:
 strcpy(info [info_lines], "You feel resistant to magic.");
 incrl();
 res_magic += 20;
 break;



}

/* case 2: strcat(glog, " of fire resistance"); break;
 case 3: strcat(glog, " of cold resistance"); break;
 case 4: strcat(glog, " of poison resistance"); break;
 case 5: strcat(glog, " of see invisible"); break;
*/

}

/*if (delay_t > 0)
{
 more();
 scrloc = 0;
}
*/

} // end of wear_armour()




void takeoff_armour(void)
{

unsigned char nthing;

if (berserker != 0)
{
 strcpy(info [info_lines], "You are too berserk!");
 incrl();
 return;
}

query : strcpy(info [info_lines], "Take off which item?");
incrl();

// test relay_message();

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = invent(2, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if (keyin == '*') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
    goto query;
        /*              strcpy(info [info_lines], "Take off which item?"); incrl();
                relay_message();
                keyin = get_ch();*/
        }
}

/*if (keyin == '*')
{
        inventory(-1);
        strcpy(info [info_lines], "Take off which item?"); incrl();
        relay_message();
        keyin = get_ch();
}*/


armour_wear_1 = (int) keyin;

if (armour_wear_1 < 65 | (armour_wear_1 > 90 && armour_wear_1 < 97) | armour_wear_1 > 122)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}



/*
if (armour [0] == -1) // or helmet etc
{
        strcpy(info [info_lines], "You aren't wearing any armour.");
        incrl();
        return;
}
*/


armour_wear_2 = conv_lett(armour_wear_1); //armour [0]; // may as well keep on using armour_wear_2

if (inv_class [armour_wear_2] != 2)
{
        strcpy(info [info_lines], "You aren't wearing that!");
        incrl();
        return;
}

if (inv_plus [armour_wear_2] > 130 && (armour_wear_2 == armour [0] | armour_wear_2 == armour [5] | armour_wear_2 == armour [2] | armour_wear_2 == armour [3] | armour_wear_2 == armour [1] | armour_wear_2 == armour [4]))
{
        strcpy(info [info_lines], "That piece of armour seems to be stuck to your body!");
        incrl();
        return;
}

if (inv_type [armour_wear_2] < 8)
{
        if (armour_wear_2 != armour [0])
        {
                strcpy(info [info_lines], "You aren't wearing that!");
                incrl();
                return;
        }

        if (armour [1] != -1)
        {
        strcpy(info [info_lines], "You'll have to take your cloak off first.");
        incrl();
        return;
        }

//      itoa(armour [1], st_prn, 10);
//      strcpy(info [info_lines], st_prn);
//      incrl();

        armour [0] = -1;

} else
switch(inv_type [armour_wear_2])
{

case 13:
case 14:
case 8:
if (armour_wear_2 != armour [5])
{
        strcpy(info [info_lines], "You aren't wearing that!");
        incrl();
        return;
} else armour [5] = -1; break;


case 9:
if (armour_wear_2 != armour [1])
{
        strcpy(info [info_lines], "You aren't wearing that!");
        incrl();
        return;
} else armour [1] = -1; break;


//armour [1] = armour_wear_2; break;
case 10:
if (armour_wear_2 != armour [2])
{
        strcpy(info [info_lines], "You aren't wearing that!");
        incrl();
        return;
} else armour [2] = -1; break;

//armour [2] = armour_wear_2; break;

case 11:
if (armour_wear_2 != armour [3])
{
        strcpy(info [info_lines], "You aren't wearing that!");
        incrl();
        return;
} else armour [3] = -1; break;



//armour [3] = armour_wear_2; break;
case 12:
if (armour_wear_2 != armour [4])
{
        strcpy(info [info_lines], "You aren't wearing that!");
        incrl();
        return;
} else armour [4] = -1; break;


//armour [4] = armour_wear_2; break;

}



//armour [0] = -1;

turnover = 1;
delay_t = property [inv_class [armour_wear_2]] [inv_type [armour_wear_2]] [0];
delay_doing = 2;

strcpy(info [info_lines], "You take off ");
//item_name(inv_class [armour_wear_2], inv_type [armour_wear_2], inv_dam [armour_wear_2], inv_plus [armour_wear_2], inv_quant [armour_wear_2], inv_ident [armour_wear_2], 5, str_pass);
in_name(armour_wear_2, 5);
strcat(info [info_lines], str_pass);
strcat(info [info_lines], ".");
incrl();

unwear_armour(armour_wear_2);

/*if (delay_t > 0)
{
 more();
 scrloc = 0;
}
*/

} // end of void takeoff_armour()


void throw_anything(void)
{
unsigned char nthing;

if (berserker != 0)
{
 strcpy(info [info_lines], "You are too berserk!");
 incrl();
 return;
}

if (inv_no == 0)
        {
        strcpy(info [info_lines], "You aren't carrying anything.");
        incrl();
        return;
        }

query : strcpy(info [info_lines], "Throw which item?");
incrl();

// test relay_message();

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?')
        {
                //nthing = invent(0, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
                nthing = invent(1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        }
        if (keyin == '*') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
    goto query;
        /*              strcpy(info [info_lines], "Throw which item?"); incrl();
                relay_message();
                keyin = get_ch();*/
        }
}

/*if (keyin == '*')
{
        inventory(-1);
        strcpy(info [info_lines], "Throw which item?"); incrl();
        relay_message();
        keyin = getch();
}*/


throw_1 = (int) keyin;

if (throw_1 < 65 | (throw_1 > 90 && throw_1 < 97) | throw_1 > 122)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

throw_2 = conv_lett(throw_1);

if (throw_2 == ring [0] | throw_2 == ring [1] | throw_2 == armour [0] | throw_2 == armour [5] | throw_2 == armour [2] | throw_2 == armour [1] | throw_2 == armour [4] | throw_2 == armour [3])
{
        strcpy(info [info_lines], "You are wearing that object!");
        incrl();
        return;
}

if (throw_2 == item_wielded && inv_plus [item_wielded] > 80)
{
        strcpy(info [info_lines], "That thing is stuck to your hand!");
        incrl();
        return;
}

if (inv_quant [throw_2] == 0)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

throw_it();

} // end of throw_anything

void shoot_thing(void)
{

if (berserker != 0)
{
 strcpy(info [info_lines], "You are too berserk!");
 incrl();
 return;
}

int type_wanted = 0;

if (item_wielded == -1) goto find;

if (inv_class [item_wielded] != 0 | inv_type [item_wielded] < 13 | inv_type [item_wielded] > 15)
{
        type_wanted = 3;
} else
                                                {
       type_wanted = inv_type [item_wielded] - 13;
                                                }

find: for (i = 0; i < 53; i ++)
{
  if (i == 52)
                {
   strcpy(info [info_lines], "You have nothing suitable.");
                        incrl();
                        return;
                }
  if (inv_quant [i] == 0) continue;
                if (inv_class [i] == 1 && type_wanted == inv_type [i])
  {
                                throw_2 = i;
    break;
  }
}

throw_it();

} // end shoot_thing


void throw_it(void)
{
char shoot = 0;

strcpy(info [info_lines], "Which direction? (* to target)");
incrl();
// test relay_message();
//keyin_2 = getch();
/*
switch(keyin_2)
{
        case '1': move_x = -1; move_y = 1; break;
        case '2': move_y = 1; move_x = 0;break;
        case '9': move_x = 1; move_y = -1; break;
        case '8': move_y = -1; move_x = 0;break;
        case '7': move_y = -1; move_x = -1; break;
        case '4': move_x = -1; move_y = 0;break;
        case '3': move_y = 1; move_x = 1; break;
        case '6': move_x = 1; move_y = 0;break;
        case '5': dir_cursor(); break;


        // put in case '.', '>' '<' etc

        default: strcpy(info [info_lines], "What an unusual direction.");
                incrl(); move_x = 0; move_y = 0; return;
}
*/

//itoa(dir_cursor(), temp_quant, 10);
//move_x = (temp_quant [0] / 10) - 6;
//move_y = temp_quant [1] - 6;
/*
nothing = dir_cursor();
if (nothing == -1)
{
        strcpy(info [info_lines], "What an unusual direction.");
        incrl();
        return;
}
move_x = ((int) (nothing / 10)) - 6;
move_y = nothing - (((int) nothing / 10) * 10) - 6;
*/

direction(1);

if (nothing == -1) return;


if (conf != 0)
{
 move_x = random2(13) - 6;
 move_y = random2(13) - 6;
}

beam_range = 2; // provisional until mass variable used

beam_flavour = 0;

//if (inv_class [throw_2

switch (inv_class [throw_2])
{
//beam_type = inv_class [throw_2];
        case 0: beam_type = 41;
 beam_range = 5;
 break;
        case 1: beam_type = 41;
 beam_range = 6;
 break;
        case 2: beam_type = 91; break;
        case 3: beam_type = 47; break;
        case 4: beam_type = 37; break;
        case 5: beam_type = 43; break;
        case 6: beam_type = 63; break;
        case 7: beam_type = 61; break;
        case 8: beam_type = 33; break;
        case 9: beam_type = 42; break;
        case 10: beam_type = 40; break;
        case 11: beam_type = 37; break;
}

//beam_type = inv_class [throw_2]; // character put on screen

beam_source_x = x_pos;
beam_source_y = y_pos;
beam_colour = inv_col [throw_2];//icolour [inv_class [throw_2]] [inv_type [throw_2]];
item_name(inv_class [throw_2], inv_type [throw_2], inv_dam [throw_2], inv_plus [throw_2], 1, inv_ident [throw_2], 6, str_pass);
//in_name(throw_2, 6);    <-- Don't use this: "the 10 +0 darts hit the x"
strcpy(beam_name, str_pass);

//beam_hit = (dex / 5) + (thr_abil / 10); // look at this!!!

beam_hit = (thr_abil * dex) / 50;

//(dex + thr_abil) / 5;












// NEED TO WORK ON BEAM_HIT!!!























if (inv_class [throw_2] == 1 && inv_type [throw_2] == 3) // darts
{
   beam_hit += 3;
}

if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1)
{
        beam_damage = property [inv_class [throw_2]] [inv_type [throw_2]] [0];
//      if (inv_class [item_wielded] == 0 && inv_type [item_wielded] == 13 && inv_type [throw_2] == 1)
//      {
//              beam_damage
//      }

//      Missile devices eg bows
        if (inv_class [item_wielded] == 0 && inv_type [item_wielded] == inv_type [throw_2] + 13 && inv_type [throw_2] <= 16)
        {
                beam_damage = property [inv_class [throw_2]] [inv_type [throw_2]] [1];

  if (inv_type [item_wielded] == 15) time_taken *= 1.5;

                int hoggl = inv_plus [item_wielded] - 50;

                if (inv_plus [item_wielded] > 80) hoggl -= 100;
                //damage_done += inv_plus [item_wielded];

                //hoggl = (hoggl % 10);

                beam_damage += hoggl;
  beam_range += 5;



                int ghoggl = inv_plus [item_wielded] - 50;

                if (inv_plus [item_wielded] > 80) ghoggl -= 100;
                //damage_done += inv_plus [item_wielded];

                //ghoggl = (ghoggl / 10);

                beam_hit += ghoggl + 3;

                beam_range += 3;


//              beam_damage += inv_plus [item_wielded];
//              beam_hit += inv_plus [item_wielded];
                if (inv_plus [item_wielded] > 80)
                {
                        beam_damage -= 100;
                        beam_hit -= 100;
                }
                shoot = 1;
                strcpy(info [info_lines], "You shoot ");
                /*item_name(inv_class [throw_2], inv_type [throw_2], inv_dam [throw_2], inv_plus [throw_2], 1, inv_ident [throw_2], 3, str_pass);
                strcat(info [info_lines], str_pass);
                strcat(info [info_lines], ".");
                incrl();*/
        }
/*
        13 = sling
        14 = bow
        15 = crossbow - gotta put loading in
        16 = hand crossbow (but not yet)
        17 = arbalest - with spears?
*/


        int hoggl = inv_plus [throw_2] - 50;
        if (inv_plus [throw_2] > 80) hoggl -= 100;
        //damage_done += inv_plus [throw_2];
        hoggl = (hoggl % 10);
        beam_damage += hoggl;


        int ghoggl = inv_plus [throw_2] - 50;
        if (inv_plus [throw_2] > 80) ghoggl -= 100;
        //damage_done += inv_plus [throw_2];
        ghoggl = (ghoggl / 10);
        beam_hit += ghoggl;



        if (inv_class [throw_2] == 0)
        {
                if (inv_type [throw_2] == 3 | inv_type [throw_2] == 9 | inv_type [throw_2] == 11)
                {
//                      beam_hit += 2;
                        beam_damage += 1;
                } else
                {
                        beam_damage -= 1;
                        beam_hit -= 3;
                }
        }

 beam_damage += thr_abil / 10;

        //beam_damage += inv_plus [throw_2];
        //if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1) beam_hit += inv_plus [throw_2];



       //       if (inv_plus [throw_2] > 80) beam_damage -= 100;
}
 else beam_damage = 2; // later put mass thing here.


// shooting: if (inv_class [throw_2] ==

//beam_hit *= random2(200) / 100;
if (shoot == 0) strcpy(info [info_lines], "You throw ");
// if shoot == 1, it has already strcpy'd "You shoot "
        item_name(inv_class [throw_2], inv_type [throw_2], inv_dam [throw_2], inv_plus [throw_2], 1, inv_ident [throw_2], 3, str_pass);
        strcat(info [info_lines], str_pass);
        strcat(info [info_lines], ".");
        incrl();

 if (item_wielded == throw_2) unwield_item(throw_2);

thing_thrown = 1;
missile();
inv_quant [throw_2] --;
if (inv_quant [throw_2] == 0)
{
        inv_no --;
        if (item_wielded == throw_2)
        {
                item_wielded = -1;
                strcpy(info [info_lines], "You are now empty handed.");
                incrl();
        }
}

burden_change();

move_x = 0; move_y = 0;

alert();

turnover = 1;

} // end of void throw_it()




void missile(void)
{
// note: using beam variables to save memory
//float beam_tot = 0; // total increments
//float beam_inc = 0; // move_x / move_y
int beam_sign_x = 0;
int beam_sign_y = 0;
//int bmox = 0;
float bmoy = 0;
int bmax = 0;
float bmay = 0;
beam_x = beam_source_x;
beam_y = beam_source_y;
int bmhit = beam_hit;

int damage_done = 0;

brek = 1;
//viewwindow();
brek = 0;

tracer = 0;

//strcpy(info [info_lines], "You throw it!");
//incrl();
//if (move_x == 0) move_y = 1;
//if (move_y == 0) move_x = 1;
/*
bmax = 1;

bmay = (float) move_y / move_x;
*/

/*

if (move_x > 1 | move_y > 1)
{
        if (move_x > 0) beam_sign_x = 1;
        if (move_x < 0)
        {
                beam_sign_x = -1;
                move_x *= -1;
        }

        if (move_y > 0) beam_sign_y = 1;
        if (move_y < 0)
        {
                beam_sign_y = -1;
                move_y *= -1;
        }

        beam_inc = (float) move_y / move_x;
} // else beam_inc = 0;
*/

if (move_x > 1 | move_y > 1 | move_x < -1 | move_y < -1)
{
/*      bmox = move_x;
        beam_inc = 1;
        bmoy = move_y;
        move_x = 0; move_y = 0;*/

        if (move_x > 0) beam_sign_x = 1;
        if (move_x < 0)
        {
                beam_sign_x = -1;
                move_x *= -1;
        }

        if (move_y > 0) beam_sign_y = 1;
        if (move_y < 0)
        {
                beam_sign_y = -1;
                move_y *= -1;
        }

        if (move_x > move_y)
        {
        if (beam_sign_x == 1) bmay = (float) (move_y) / (move_x + 1);
        if (beam_sign_x == -1) bmay = (float) (move_y) / (move_x + 1);
        bmax = 1;
        } else
              {
              if (beam_sign_y == 1) bmay = (float) (move_x) / (move_y + 1);
              if (beam_sign_y == -1) bmay = (float) (move_x) / (move_y + 1);
              bmax = 2;
              }
        bmoy = 1;



} //else beam_inc = 0;



for (n = 1; n < beam_range; n++)
{


        beam_hit = bmhit; // random2(bmhit); // * random2(200) / 100;


        //if (beam_sign_x == 1) //< 1)
//        if (bmoy >

        if (bmax == 1)
        {
                move_x = beam_sign_x;
                bmoy += bmay;
                if (bmoy >= 1)
                {
                        move_y = beam_sign_y;
                        bmoy -= 1;
                } else move_y = 0;
        }



        if (bmax == 2)
        {
        move_y = beam_sign_y;
        bmoy += bmay;
        if (bmoy >= 1)
        {
                move_x = beam_sign_x;
                bmoy -= 1;
        } else move_x = 0;
        }









     /* if (beam_inc == 1)
        {


        //do
        {
                bmax += 1; bmay += 1;
                if (bmax == bmox && bmay == bmoy)
                {
                bmay = 0;
                        bmax = 0;
                }
                if (bmax <= bmox) move_x = beam_sign_x; else move_x = 0;
                if (bmay <= bmoy) move_y = beam_sign_y; else move_y = 0;
        } //while (move_x == 0 && move_y == 0);



        }
        /*
        if (beam_inc != 0)
        {
                beam_tot += beam_inc;
                //if (beam_tot >= 1) beam_y ++;
                //while (beam_tot >= 1)
                //{
                if (beam_tot >= 1)
                {
                move_y = beam_sign_y;
                beam_tot -= 1;
                } else move_y = 0;
                //}


                if (beam_tot < 1) move_x = beam_sign_x; else move_x = 0;

        }// else
           //   {
            //  beam_x += move_x; beam_y += move_y;
             // }
        */



if (grid [beam_x + move_x] [beam_y + move_y] > 10)
{
       /*       if (beam_inc != 0)
        {
                beam_tot += beam_inc;
                //if (beam_tot >= 1) beam_y ++;
                //while (beam_tot >= 1)
                //{
                if (beam_tot >= 1)
                {
                beam_y += beam_sign_y;
                beam_tot -= 1;
                }
                //}


                if (beam_tot < 1) beam_x += beam_sign_x;

        } else
              {*/
                beam_x += move_x; beam_y += move_y;
              //}

// This bit is just too fast! Can't see missiles. I might put it in as
//  an option later.

/*
if (beam_x - move_x > x_pos - 6 && beam_x - move_x < x_pos + 6 && beam_y - move_y > y_pos - 6 && beam_y - move_y < y_pos + 6 && show [beam_x - move_x - x_pos + 6] [beam_y - move_y - y_pos + 6] != 0)
        {
                window(2,2,12,13);
//              textcolor(show_col [beam_x - move_x - x_pos + 6] [beam_y - move_y - y_pos + 6]);
                gotoxy(beam_x - move_x - x_pos + 6, beam_y - move_y - y_pos + 6);
//              putch(show [beam_x - move_x - x_pos + 6] [beam_y - move_y - y_pos + 6]);
                count_x = beam_x - move_x;
                count_y = beam_y - move_y;
                put_show();
        }
*/

        if (beam_x == target_x && beam_y == target_y)
        {
      if (beam_flavour == 10)
      {
        if (thing_thrown == 3) thing_thrown = 1;
                                                                if (thing_thrown == 4) thing_thrown = 2;
                explosion1();
        aim_down = 0;
                      return;
      }

      if (beam_flavour == 8) // cloud
      {
            aim_down = 0;
            return;
      }

  }

  if (strcmp(beam_name, "orb of energy") == 0)
                {
        place_cloud(7, beam_x, beam_y, random2(5) + 1);
                }

  if (strcmp(beam_name, "ball of steam") == 0)
                {
        place_cloud(8, beam_x, beam_y, random2(5) + 1);
                }

if (beam_x > x_pos - 7 && beam_x < x_pos + 7 && beam_y > y_pos - 7 && beam_y < y_pos + 7 && show [beam_x - x_pos + 7] [beam_y - y_pos + 7] != 0)
        {
                brek = 1;
//              brek = 0;
                viewwindow();
                brek = 0;
                window(2,2,34,15);
                textcolor(beam_colour);
                gotoxy(beam_x - x_pos + 17, beam_y - y_pos + 8);
                putch(beam_type);
                delay(30);
//              delay(30);
//              delay(30);
        }

if (beam_x == x_pos && beam_y == y_pos)
{
 // I find it more logical that there can be virtually unavoidable missiles than missiles that can almost always be avoided.
// if (beam_hit >=


if (shield_class > 0 && random2(beam_hit * 5) <= random2(shield_class))
{
 strcpy(info [info_lines], "You block the ");
        strcat(info [info_lines], beam_name);
 strcat(info [info_lines], ".");
 incrl();
// blocked = 1;
// hit = 0;
 if (beam_flavour == 10)
 {
        explosion1();
  aim_down = 0;
        return;
 }
 break;
} // end of block


        if (beam_hit >= random2(evasion) + random2(dex) / 3 - 2)// | random2(6) == 0)
        {

        strcpy(info [info_lines], "The ");
        strcat(info [info_lines], beam_name);
        strcat(info [info_lines], " hits you!");
        incrl();
        if (thing_thrown == 3 | thing_thrown == 4)
        {
        if (beam_x > x_pos - 7 && beam_x < x_pos + 7 && beam_y > y_pos - 7 && beam_y < y_pos + 7 && show [beam_x - x_pos + 7] [beam_y - y_pos + 7] != 0)
        {
                brek = 1;
                viewwindow();
                brek = 0;
                window(2,2,34,15);
                textcolor(beam_colour);
                //show [beam_x - x_pos + 6] [beam_y - y_pos + 6] = 35;
                gotoxy(beam_x - x_pos + 17, beam_y - y_pos + 8);
                putch(35);
        }
        }
/*
        switch(beam_flavour)
        {
                case 2: // fire
                if (res_fire != 0)
                {
                        strcpy(info [info_lines], "... But you don't get burned!");
                        incrl();
                        n++;
                }
                break;

                case 3: // cold
                if (res_cold != 0)
                {
                        strcpy(info [info_lines], "... But you don't feel cold!");
                        incrl();
                        n++;
                }
                break;

  case 6: // plant spit
  splash_with_acid(10);
  break;

        }
*/
hurted = 0;
        if (beam_damage > 100)
        {
                hurted += random2(beam_damage - 100);
                hurted += random2(beam_damage - 100);
                hurted += random2(beam_damage - 100);
        } else hurted += random2(beam_damage);

 check_your_resists();

//      hp -= damage_done;

        if (beam_flavour == 20 | beam_flavour == 20 | (beam_flavour == 2 && strcmp(beam_name, "ball of steam") != 0) | stricmp(beam_name, "hellfire") == 0) scrolls_burn(2, 6);
 if (beam_flavour == 3) scrolls_burn(2, 8);


        ouch(hurted, 0, 3);

        hp_ch = 1;

        break;
        //n++; // reduces beam's range
        } else
        {
        strcpy(info [info_lines], "The ");
        strcat(info [info_lines], beam_name);
        strcat(info [info_lines], " misses you!");
        incrl();
        if (move_x == 0 && move_y == 0) break;
        //break;
        }

if (beam_flavour == 10)
{
        explosion1();
 aim_down = 0;
        return;
}

}



if (grid [beam_x] [beam_y] >= 50)
{
count_x = 0;


for (o = 0; o < MNST; o++)
{
        if (mons_x [o] == beam_x && mons_y [o] == beam_y && mons_class [o] != -1)
        {
                //your_to_hit = strength + (0.5 * dex) + (f_abil);
                //if (inv_class [item_wielded] == 0) your_to_hit += inv_plus [item_wielded];
                //your_to_hit *= random2(200) / 100;

  // this won't display a "you miss it" if you throw an aim_down thing at a lava worm, only if you hit.
  if (beam_hit >= random2(mons_ev [o]) && ((mons_class [o] < MLAVA0 | mons_sec [o] == 0) | (beam_x == target_x && beam_y == target_y && aim_down == 1))) // | random2(6) == 0)
                {
  hurted = 0;

                if (beam_damage > 100)
                {
                        hurted += random2(beam_damage - 100);
                        hurted += random2(beam_damage - 100);
                        hurted += random2(beam_damage - 100);
//                      mons_hp [o] -= random2(beam_damage - 100);
                } else hurted += random2(beam_damage);

  if (mons_near(o) == 1)
  {
                strcpy(info [info_lines], "The ");
                //if (beam_type == 63) strcat(info [info_lines], "scroll ");
                //if (beam_type == 33) strcat(info [info_lines], "bottle of ");
                strcat(info [info_lines], beam_name);
                strcat(info [info_lines], " hits ");
                strcat(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 1)); //gmon_name [mons_class [o]]);
                strcat(info [info_lines], ".");
                incrl();
  }

  check_mons_resists();

  mons_hp [o] -= hurted;

  if ((thing_thrown == 1 | thing_thrown == 3) && mons_beh [o] == 7 && hurted != 0) mons_beh [o] = 1;

                if (mons_beh [o] == 0) mons_beh [o] = 1;


                if (thing_thrown == 3 | thing_thrown == 4)
                {
                if (beam_x > x_pos - 7 && beam_x < x_pos + 7 && beam_y > y_pos - 7 && beam_y < y_pos + 7 && show [beam_x - x_pos + 7] [beam_y - y_pos + 7] != 0)
                {
                        brek = 1;
                        viewwindow();
                        brek = 0;
                        window(2,2,34,15);
                        textcolor(beam_colour);
                        gotoxy(beam_x - x_pos + 17, beam_y - y_pos + 8);
                        putch(35);
                }
                }
                if (mons_hp [o] <= 0)
                {
                        switch(thing_thrown)
                        {
                                case 1: killer = 3; break; // You threw
                                case 2: killer = 4; break; // mons threw
                                case 3: killer = 3; break; // You cast a missile spell which doesn't leave an item.
                                case 4: killer = 4; break; // mons cast a missile spell which doesn't leave an item.
                        }

                        monster_die(o);
                }
                count_x = 1;

                if (beam_flavour == 10)
                {
                        explosion1();
   aim_down = 0;
                        return;
                }


                break;
                } else
                if (thing_thrown != 2 && thing_thrown != 4 && mons_near(o) == 1 && (mons_class [o] < MLAVA0 | mons_sec [o] == 0)) // No message if monster missile misses
                {
                strcpy(info [info_lines], "The ");
                //if (beam_type == 63) strcat(info [info_lines], "scroll ");
                //if (beam_type == 33) strcat(info [info_lines], "bottle of ");
                strcat(info [info_lines], beam_name);
                strcat(info [info_lines], " misses ");
                strcat(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 1)); //gmon_name [mons_class [o]]);
                strcat(info [info_lines], ".");
                incrl();

                if (invis != 0 && mons_beh [o] == 0) mons_beh [o] = 1;

                //if (mons_hp [o] <= 0) monster_die(o);
                //count_x = 1;
                //break;
                }

                //n++;
        }
} // end of for o

} // end of if (grid ... >= 60)



if (count_x == 1)
{
count_x = 0;
break;
}

} // end of if.

  if (beam_x == target_x && beam_y == target_y && aim_down == 1)
                {
                goto landed;
                }


} // end of for n loop

landed : if (beam_flavour == 10)
{
        explosion1();
 aim_down = 0;
        return;
}


if (thing_thrown == 1) // ie if you threw it.
{
if (grid [beam_x] [beam_y] % 50 != 11 && grid [beam_x] [beam_y] % 50 != 12)
{
 item_drop_2 = throw_2;
 x_plos = beam_x; y_plos = beam_y;
 quant_drop = 1;
 if (inv_class [throw_2] != 1 | random2(2) == 0) item_place();
}

if (throw_2 == item_wielded)
        {
                item_wielded = -1;
                strcpy(info [info_lines], "You are empty handed.");
                incrl();
                damage = nat_damage;
                damage_ch = 1;
        }
}


if (thing_thrown == 2 && (grid [beam_x] [beam_y] % 50 != 11 && grid [beam_x] [beam_y] % 50 != 12) && random2(2) != 0) // monster threw it.
{

if (item_quant [hand_used] == 0) // 0 instead of 1 because the item_quant is reduced by 1 when it is thrown.
{
//      item_x [hand_used] = beam_x;
//      item_y [hand_used] = beam_y;
} else
     {

        if (igrid [beam_x] [beam_y] != 501)
        {
                if ((item_class [hand_used] == 1 | item_class [hand_used] == 4 | item_class [hand_used] == 6 | item_class [hand_used] == 8 | item_class [hand_used] == 9) && item_class [hand_used] == item_class [igrid [beam_x] [beam_y]] && item_type [hand_used] == item_type [igrid [beam_x] [beam_y]] && item_plus [hand_used] == item_plus [igrid [beam_x] [beam_y]] && item_dam [hand_used] == item_dam [igrid [beam_x] [beam_y]])
                {
                        item_quant [igrid [beam_x] [beam_y]] ++;
   aim_down = 0;
                        return;
                }
        } // end of if igrid != 501


/*
        for (o = 0; o < ITEMS; o++)
        {
                if ((beam_x == item_x [o] && beam_y == item_y [o] && item_class [hand_used] == 1 | item_class [hand_used] == 4 | item_class [hand_used] == 6 | item_class [hand_used] == 8 | item_class [hand_used] == 9) && item_class [hand_used] == item_class [o] && item_type [hand_used] == item_type [o] && item_plus [hand_used] == item_plus [o] && item_dam [hand_used] == item_dam [o])
                {
                //if (strcmp(fake_name [hand_used], fake_name [o]) == 0);// && beam_x == item_x [o] && beam_y == item_y [o])
                if (item_ident [hand_used] == item_ident [o])
                {
                        item_quant [o] += 1;
                        return; //break; //return;
                }
                }
        }
*/
        for (o = 0; o < ITEMS; o++)
        {
                if (item_quant [o] == 0)
                {
                        //strcpy(fake_name [o], fake_name [hand_used]);
                        item_ident [o] = item_ident [hand_used];
                        item_class [o] = item_class [hand_used];
                        item_type [o] = item_type [hand_used];
                        item_plus [o] = item_plus [hand_used];
                        item_dam [o] = item_dam [hand_used];
                        icolour [o] = icolour [hand_used];
                        //item_x [o] = beam_x; item_y [o] = beam_y; //x place position
                        item_quant [o] = 1;
                        item_link [o] = 501;
                        it_no ++;
                        break;
                }
        } // end of o loop

        if (igrid [beam_x] [beam_y] == 501)
        {
                igrid [beam_x] [beam_y] = o;
        } else
        {
                int hug = igrid [beam_x] [beam_y];
                igrid [beam_x] [beam_y] = o;
                item_link [o] = hug;
        }

     } // end of else

} // if (thing_throw == 2) ...


aim_down = 0;
// ^ must have this before every return in this function!

} // end of missile()









void puton_ring(void)
{

unsigned char nthing;

if (inv_no == 0)
        {
        strcpy(info [info_lines], "You aren't carrying anything.");
        incrl();
        return;
        }

if (berserker != 0)
{
 strcpy(info [info_lines], "You are too berserk!");
 incrl();
 return;
}

query : strcpy(info [info_lines], "Put on which item?");
incrl();

// test relay_message();

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = invent(7, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if (keyin == '*') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
       goto query;

/*              strcpy(info [info_lines], "Put on which item?"); incrl();
                relay_message();
                keyin = get_ch();*/
        }
}
/*
if (keyin == '*')
{
        inventory(-1);
        strcpy(info [info_lines], "Put on which item?"); incrl();
        relay_message();
        keyin = getch();
}
*/

ring_wear_1 = (int) keyin; // have to do amulets etc later

/*if (keyin == '-')
{
damage = 1;
item_wielded = -1;
strcpy(info [info_lines], "You are empty handed.");
incrl();
damage_ch = 1;
return;
}*/

if (ring_wear_1 < 65 | (ring_wear_1 > 90 && ring_wear_1 < 97) | ring_wear_1 > 122)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

ring_wear_2 = conv_lett(ring_wear_1);

if (ring_wear_2 == ring [0] | ring_wear_2 == ring [1])
{
        strcpy(info [info_lines], "You are already wearing that!");
        incrl();
        return;
}

if (inv_quant [ring_wear_2] == 0)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

if (ring_wear_2 == item_wielded)
{
        strcpy(info [info_lines], "You are wielding that object.");
        incrl();
        return;
}

if (armour [3] != -1 && inv_plus [armour [3]] > 80)
{
        strcpy(info [info_lines], "You can't take your gloves off to wear a ring!");
        incrl();
        return;
}

if (inv_class [ring_wear_2] == 7 && ring [0] != -1 && ring [1] != -1) // and you are trying to wear body armour.
{
        strcpy(info [info_lines], "You are already wearing a ring on each hand.");
        incrl();
        return;
}



if (inv_class [ring_wear_2] != 7)
{
        strcpy(info [info_lines], "You can't put that on your finger!");
        incrl();
        return;
}

if (ring [0] != -1) hand_used = 1;
if (ring [1] != -1) hand_used = 0;

if (ring [0] == -1 && ring [1] == -1)
{
        strcpy(info [info_lines], "Put on which hand (l or r)?");
        incrl(); // test relay_message();

        keyin = get_ch();

        if (keyin != 'r' && keyin != 'l')
        {
                strcpy(info [info_lines], "You don't have such a hand!");
                incrl();
                return;
        }

        if (keyin == 'l') hand_used = 0;
        if (keyin == 'r') hand_used = 1;
}

ring [hand_used] = ring_wear_2;



strcpy(info [info_lines], " ");
                        strncpy(info [info_lines], letters [ring_wear_2], 1);
                        strcat(info [info_lines], " - ");

/*
                        if (inv_name [ring_wear_2] [0] == 97 ^ inv_name [ring_wear_2] [0] == 101 ^ inv_name [ring_wear_2] [0] == 105 ^ inv_name [ring_wear_2] [0] == 111 ^ inv_name [ring_wear_2] [0] == 117)
                        {
                                strcat(info [info_lines], "an ");
                        }
                        if (inv_name [ring_wear_2] [0] != 97 && inv_name [ring_wear_2] [0] != 101 && inv_name [ring_wear_2] [0] != 105 && inv_name [ring_wear_2] [0] != 111 && inv_name [ring_wear_2] [0] != 117 && inv_name [ring_wear_2] [0] > 96)
                        {
                                strcat(info [info_lines], "a ");
                        }
*/
                        item_name(inv_class [ring_wear_2], inv_type [ring_wear_2], inv_dam [ring_wear_2], inv_plus [ring_wear_2], inv_quant [ring_wear_2], inv_ident [ring_wear_2], 3, str_pass);
                        strcat(info [info_lines], str_pass);
                        //if (inv_quant [j] != 1) strcat(info [info_lines], "s");

                        if (hand_used == 0) strcat(info [info_lines], " (on left hand)");
                        if (hand_used == 1) strcat(info [info_lines], " (on right hand)");
                        incrl();


switch(inv_type [ring_wear_2])
{

        case 0: // ring of regeneration
        rate_regen += 100;
        hunger_inc += 4;
        break;

        case 1: // protection
        if (inv_plus [ring_wear_2] > 130) AC -= 100;
        AC += inv_plus [ring_wear_2];
        AC -= 50;
        AC_ch = 1;
        hunger_inc += 1;
 if (inv_plus [ring_wear_2] != 0 && inv_plus [ring_wear_2] != 100) inv_ident [ring_wear_2] = 3;
        break;

        case 2: // fire resistance
//      strcpy(info [info_lines], "Fire resist!");
        //incrl();
        res_fire -= 1;
        hunger_inc += 1;
        break;

        case 3: // poison resistance
        res_poison += 1;
        hunger_inc += 1;
        break;

        case 4: // cold resistance
//      strcpy(info [info_lines], "Cold resist!");
        //incrl();
        res_cold -= 1;
        hunger_inc += 1;
        break;

        case 5: // increase strength
        if (inv_plus [ring_wear_2] > 130)
 {
 strength -= 100;
 max_strength -= 100;
 }
        strength += inv_plus [ring_wear_2] - 50;
        max_strength += inv_plus [ring_wear_2] - 50;
        strength_ch = 1;
        hunger_inc += 1;
 burden_change();
 if (inv_plus [ring_wear_2] != 0 && inv_plus [ring_wear_2] != 100) inv_ident [ring_wear_2] = 3;
 break;

        case 6: // combat?
        f_abil += 30;
        hunger_inc += 1;
        break;

        case 7: // see invisible
        see_invis --;
        hunger_inc += 1;
        break;

        case 8: // invisibility
        if (invis > 0) invis = 0;
           else if (invis == 0)
           {
                strcpy(info [info_lines], "You flicker for a moment, then turn invisible!");
                incrl();
  inv_ident [ring_wear_2] = 3;
           }
//      your_sign = 73;
        invis --;
        hunger_inc += 7;
        break;

        case 9: // hunger
        hunger_inc += 4;
        break;

        case 10: // teleportation
        hunger_inc += 1;
        teleport += 1;
        break;

        case 11: // evasion
        hunger_inc += 1;
        if (inv_plus [ring_wear_2] > 130) evasion -= 100;
        evasion += inv_plus [ring_wear_2];
        evasion -= 50;
        evasion_ch = 1;
 if (inv_plus [ring_wear_2] != 0 && inv_plus [ring_wear_2] != 100) inv_ident [ring_wear_2] = 3;
        break;

        case 12:
 hunger_inc += 1;
 sust_abil++;
 break;

 case 13:
 hunger_inc -= 2;
 break;

 case 14: // increase dex
        if (inv_plus [ring_wear_2] > 130)
 {
  max_dex -= 100;
  dex -= 100;
 }
        dex += inv_plus [ring_wear_2] - 50;
        max_dex += inv_plus [ring_wear_2] - 50;
        dex_ch = 1;
        hunger_inc += 1;
 if (inv_plus [ring_wear_2] != 0 && inv_plus [ring_wear_2] != 100) inv_ident [ring_wear_2] = 3;
        break;

 case 15: // increase intel
        if (inv_plus [ring_wear_2] > 130)
 {
  max_intel -= 100;
  intel -= 100;
 }
        intel += inv_plus [ring_wear_2] - 50;
        max_intel += inv_plus [ring_wear_2] - 50;
        intel_ch = 1;
        hunger_inc += 1;
 if (inv_plus [ring_wear_2] != 0 && inv_plus [ring_wear_2] != 100) inv_ident [ring_wear_2] = 3;
        break;

 case 16: // wizardry
 mag_abil += 20;
 hunger_inc += 1;
 break;

 case 17: // magic
 ep_max += 9;
 hunger_inc ++;
 ep_ch = 1;
 magic_battery ++;
 break;

        case 18: // levitation
        if (lev > 0) lev = 0;
           else if (lev == 0)
           {
                strcpy(info [info_lines], "You feel very buoyant!");
                incrl();
                strcpy(info [info_lines], "You gently float upwards from the floor.");
                incrl();
       inv_ident [ring_wear_2] = 3;
           }
        lev --;
        hunger_inc += 1;
        break;

 case 19: // prot_life
 prot_life ++;
        hunger_inc ++;
        break;

 case 20: // magic resistance
 res_magic += 30;
 hunger_inc += 1;
 break;

}
//armour [0] = armour_wear_2;
//AC_ch = 1;
//evasion_ch = 1;
turnover = 1;

} // end of puton_ring()





void remove_ring(void)
{

unsigned char nthing;

if (ring [0] == -1 && ring [1] == -1)
{
        strcpy(info [info_lines], "You aren't wearing any rings.");
        incrl();
        return;
}

if (berserker != 0)
{
 strcpy(info [info_lines], "You are too berserk!");
 incrl();
 return;
}

if (armour [3] != -1 && inv_plus [armour [3]] > 80)
{
        strcpy(info [info_lines], "You can't take your gloves off to remove your rings!");
        incrl();
        return;
}


if (ring [0] != -1) hand_used = 0;
if (ring [1] != -1) hand_used = 1;


if (ring [0] != -1 && ring [1] != -1)
{
        query : strcpy(info [info_lines], "Remove from which hand (l or r)?");
        incrl(); // test relay_message();

        keyin = get_ch();

        if (keyin == '?' | keyin == '*')
        {
        if (keyin == '?') nthing = invent(7, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if (keyin == '*') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
       goto query;
/*              strcpy(info [info_lines], "Remove from which hand?"); incrl();
                relay_message();
                keyin = get_ch();*/
        }
/*              inventory(7);
                strcpy(info [info_lines], "Remove which item?"); incrl();
                relay_message();
                goto query;
*/
        }

        if (keyin != 'r' && keyin != 'l')
        {
                strcpy(info [info_lines], "You don't have such a hand!");
                incrl();
                return;
        }

        if (keyin == 'l') hand_used = 0;
        if (keyin == 'r') hand_used = 1;

}

if (inv_plus [ring [hand_used]] > 80) // cursed
{
        strcpy(info [info_lines], "This ring seems to be stuck to your finger!");
        incrl();
 if (inv_ident [ring [hand_used]] == 0) inv_ident [ring [hand_used]] = 1;
 return;
}

strcpy(info [info_lines], "You remove your ");
item_name(inv_class [ring [hand_used]], inv_type [ring [hand_used]], inv_dam [ring [hand_used]], inv_plus [ring [hand_used]], inv_quant [ring [hand_used]], inv_ident [ring [hand_used]], 8, str_pass);
strcat(info [info_lines], str_pass);
strcat(info [info_lines], ".");
incrl();

ring_wear_2 = ring [hand_used]; // I'll still use ring_wear_2 here.


switch(inv_type [ring_wear_2])
{

        case 0: // regen
        rate_regen -= 100;
        hunger_inc -= 4;
        break;

        case 1: // prot
        if (inv_plus [ring_wear_2] > 130) AC += 100;
        AC -= inv_plus [ring_wear_2];
        AC += 50;
        AC_ch = 1;
        hunger_inc -= 1;
        break;

        case 2: // fire resistance
        res_fire += 1;
        hunger_inc -= 1;
        break;

        case 3: // poison resistance
        res_poison -= 1;
        hunger_inc -= 1;
        break;

        case 4: // cold resistance
        res_cold += 1;
        hunger_inc -= 1;
        break;

        case 5: // increase strength
        if (inv_plus [ring_wear_2] > 130)
 {
  strength += 100;
  max_strength += 100;
 }
        strength -= inv_plus [ring_wear_2];
 max_strength -= inv_plus [ring_wear_2];
 strength += 50;
 max_strength += 50;
        strength_ch = 1;
        hunger_inc -= 1;
 burden_change();
        break;

        case 6: // slaying
        f_abil -= 30;
        hunger_inc -= 1;
        break;

        case 7: // see invisible
        see_invis ++;
        hunger_inc -= 1;
        break;

        case 8: // invisibility
        invis ++;
        if (invis == 0) invis = 1;
        hunger_inc -= 7;
        break;

        case 9: // hunger
        hunger_inc -= 4;
        break;

        case 10: // teleportation
        hunger_inc -= 1;
        teleport -= 1;
        break;

        case 11: // evasion
        hunger_inc -= 1;
        if (inv_plus [ring_wear_2] > 130) evasion += 100;
        evasion -= inv_plus [ring_wear_2];
        evasion += 50;
        evasion_ch = 1;
        break;

 case 12: // sust_abil
 hunger_inc -= 1;
 sust_abil -= 1;
 break;

 case 13: // sustenance
 hunger_inc += 2;
 break;

 case 14: // increase dex
        if (inv_plus [ring_wear_2] > 130)
 {
  max_dex += 100;
  dex += 100;
 }
        dex -= inv_plus [ring_wear_2] - 50;
 max_dex -= inv_plus [ring_wear_2] - 50;
        dex_ch = 1;
        hunger_inc -= 1;
        break;

 case 15: // increase intel
        if (inv_plus [ring_wear_2] > 130)
 {
  max_intel += 100;
  intel += 100;
 }
        intel -= inv_plus [ring_wear_2] - 50;
        max_intel -= inv_plus [ring_wear_2] - 50;
        intel_ch = 1;
        hunger_inc -= 1;
        break;

 case 16: // wizardry
 mag_abil -= 20;
 hunger_inc -= 1;
 break;

 case 17: // magic
 ep_max -= 9;
 if (ep > ep_max) ep = ep_max;
 hunger_inc --;
 ep_ch = 1;
 magic_battery --;
 break;

        case 18: // levitation
        lev ++;
        if (lev == 0) lev = 1;
        hunger_inc -= 1;
        break;

 case 19: // prot_life
 prot_life --;
        hunger_inc --;
        break;

 case 20: // magic resistance
 res_magic -= 30;
 hunger_inc -= 1;
 break;

} // end of switch




ring [hand_used] = -1;


turnover = 1;


} // end of remove_ring



void zap_wand(void)
{
int zap_device_1;
int zap_device_2;

unsigned char nthing;

wand_id = 0;

if (inv_no == 0)
        {
        strcpy(info [info_lines], "You aren't carrying anything.");
        incrl();
        return;
        }

if (berserker != 0)
{
 strcpy(info [info_lines], "You are too berserk!");
 incrl();
 return;
}


query : strcpy(info [info_lines], "Zap which item?");
incrl();

// test relay_message();

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = invent(3, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if (keyin == '*') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
   goto query;

        /*              strcpy(info [info_lines], "Zap which item?"); incrl();
                relay_message();
                keyin = get_ch();*/
        }
/*
        inventory(3);
        strcpy(info [info_lines], "Zap which item?"); incrl();
        relay_message();
        keyin = getch();*/
}
/*
if (keyin == '*')
{
        inventory(-1);
        strcpy(info [info_lines], "Zap which item?"); incrl();
        relay_message();
        keyin = getch();
}
*/

zap_device_1 = (int) keyin; // have to do amulets etc later



if (zap_device_1 < 65 | (zap_device_1 > 90 && zap_device_1 < 97) | zap_device_1 > 122)
{
        Dont_have : strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

zap_device_2 = conv_lett(zap_device_1);

if (inv_quant [zap_device_2] == 0)
{
        goto Dont_have;
}

/*if (inv_class [zap_device_2] == 11 && item_wielded == zap_device_2)
{
 staff_spell(zap_device_2);
 return; // turnover is set in staff_spell()
} else*/
if (inv_class [zap_device_2] != 3 | inv_plus [zap_device_2] <= 0)
{
        strcpy(info [info_lines], "Nothing appears to happen.");
        incrl();
        turnover = 1;
        return;
}

strcpy(info [info_lines], "Which direction? (* to target)");
incrl();
// test relay_message();
//keyin_2 = getch();
/*
switch(keyin_2)
{
        case '1': move_x = -1; move_y = 1; break;
        case '2': move_y = 1; move_x = 0;break;
        case '9': move_x = 1; move_y = -1; break;
        case '8': move_y = -1; move_x = 0;break;
        case '7': move_y = -1; move_x = -1; break;
        case '4': move_x = -1; move_y = 0;break;
        case '3': move_y = 1; move_x = 1; break;
        case '6': move_x = 1; move_y = 0;break;
        case '5': dir_cursor(); break;
        // put in case '.', '>' '<' etc

        default: strcpy(info [info_lines], "What an unusual direction.");
                incrl(); move_x = 0; move_y = 0; return;
}
*/

direction(1);

if (nothing == -1) return;

if (conf != 0)
{
 move_x = random2(13) - 6;
 move_y = random2(13) - 6;
}

//if (property [inv_class [zap_device_2]] [inv_type [zap_device_2]] [1] > 0)
//{ // ie if the beam is visible.
//beam_range = random2(10) + 5;
//beam_type = inv_class [zap_device_2];

beam_source_x = x_pos; beam_source_y = y_pos;

//int thing_to_do =
//zapping(inv_type [zap_device_2], beam_colour, beam_name, beam_range, beam_damage, beam_flavour, beam_type, thing_thrown, beam_hit);

//cout << "col: " << beam_colour << "hit " << beam_hit << "dam " << beam_damage << "ty " << beam_type << " " << beam_name;

//getch();

//switch(thing_to_do)
//{
        //case 1:
//      missile(); //break;
//      case 2: beam(); break;
//}


//zapping(char ztype, char power);

char type_zapped = inv_type [zap_device_2];
if (type_zapped == 16) type_zapped = 21;

zapping(type_zapped, 80); // 40??

if (wand_id == 1 | inv_type [zap_device_2] == 12)
{
        set_id(inv_class [zap_device_2], inv_type [zap_device_2], 1);
}

/*
switch(inv_type [zap_device_2])
{
//if (inv_type [zap_device_2] == 0)
//{
        case 0: // flame
        beam_source_x = x_pos;
        beam_source_y = y_pos;
        beam_colour = 4;
        strcpy(beam_name, "puff of flame");
        beam_range = random2(5) + 7;
        beam_damage = 8; // should this be the same as magic missile?
        beam_hit = 30;
        beam_type = 42; // maybe make missile() put a # on wherever it hits?
        thing_thrown = 3;
        missile();
        beam_flavour = 2; // fire
        break;


        case 1: // frost
        beam_source_y = y_pos;
        beam_colour = 15;
        strcpy(beam_name, "ball of frost");
        beam_range = random2(5) + 7;
        beam_damage = 8; // should this be the same as magic missile?
        beam_hit = 30;
        beam_type = 42; // maybe make missile() put a # on wherever it hits?
        thing_thrown = 3;
        missile();
        beam_flavour = 3; // cold
        break;

        case 2: // slow monster
        strcpy(beam_name, "0");
        beam_range = random2(5) + 6;
        beam_type = 0;
        beam_colour = 0;
        thing_thrown = 1;
        beam();
        beam_flavour = 4; // magic
        break;

        case 3: // hasting
        strcpy(beam_name, "0");
        beam_range = random2(5) + 6;
        beam_type = 0;
        beam_colour = 1;
        thing_thrown = 1;
        beam();
        beam_flavour = 4; // magic
        break;

        case 4: // magic missile
        beam_source_x = x_pos;
        beam_source_y = y_pos;
        beam_colour = 13;//inv_col [throw_2];//icolour [inv_class [throw_2]] [inv_type [throw_2]];
        strcpy(beam_name, "magic missile");// inv_name [throw_2]);
//      if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1)
//      {
//              beam_damage = property [inv_class [throw_2]] [inv_type [throw_2]] [0];
//              beam_damage += inv_plus [throw_2];
//      }
//       else beam_damage = 2; // later put mass thing here.
        beam_range = random2(5) + 7;
        beam_damage = 15;
        beam_hit = 50;
        beam_type = 35;
//      beam_hit = strength + (0.5 * dex) + (f_abil);
//      if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1) beam_hit += inv_plus [throw_2];
        //beam_hit *= random2(200) / 100;
        thing_thrown = 3;
        missile();
        beam_flavour = 1; // a magical missile
        break;

        case 5: // healing
        strcpy(beam_name, "0");
        beam_range = random2(5) + 6;
        beam_type = 0;
        beam_colour = 2;
        thing_thrown = 1;
        beam();
        beam_flavour = 4; // magic
        break;

        case 6: // paralysis
        strcpy(beam_name, "0");
        beam_range = random2(5) + 6;
        beam_type = 0;
        beam_colour = 3;
        thing_thrown = 1;
        beam();
        beam_flavour = 4; // magic
        break;

        case 7:
        strcpy(beam_name, "bolt of fire");
        beam_range = random2(10) + 5;
        beam_damage = 10;
        beam_colour = 4;
        beam_type = 35; // 35
        thing_thrown = 1;
        beam();
        beam_flavour = 2; // fire
        break;
//}


//if (inv_type [zap_device_2] == 1)
//{
        case 8:
        strcpy(beam_name, "bolt of cold");
        beam_range = random2(10) + 5;
        beam_damage = 34;
        beam_colour = 15;
        beam_type = 35;
        thing_thrown = 1;
        beam();
        beam_flavour = 3; // cold
        break;
//}
}
*/

inv_plus [zap_device_2] --;
//}

turnover = 1;

alert();

move_x = 0; move_y = 0;


//if (inv_dam [zap_device_2] == 1) // for devices, this means it's identified.
//{

//      Something about inv_ident here!!!

/*
if (inv_name [zap_device_2] [0] == '+' | inv_name [zap_device_2] [0] == '-')
{
                //if ((inv_class [zap_device_2] > -1 && inv_class [zap_device_2] < 3))
                //{
                if (inv_plus [zap_device_2] < 0) strcpy(inv_name [zap_device_2], "-");
                        else strcpy(inv_name [zap_device_2], "+");
                itoa(inv_plus [zap_device_2] , temp_quant, 10);
                strcat(inv_name [zap_device_2], temp_quant);
                strcat(inv_name [zap_device_2], " ");
                //}
                //strcat(inv_name [zap_device_2], all_items [inv_class [zap_device_2]] [inv_type [zap_device_2]]);



                // need to put in a switch for identifying wands

                switch(inv_type [zap_device_2])
                {
                case 0: strcat(inv_name [zap_device_2], "wand of flame"); break;
                case 1: strcat(inv_name [zap_device_2], "wand of frost"); break;
                case 2: strcat(inv_name [zap_device_2], "wand of slowing"); break;
                case 3: strcat(inv_name [zap_device_2], "wand of hasting"); break;
                case 4: strcat(inv_name [zap_device_2], "wand of magic missile"); break;
                case 5: strcat(inv_name [zap_device_2], "wand of healing"); break;
                case 6: strcat(inv_name [zap_device_2], "wand of paralysis"); break;
                case 7: strcat(inv_name [zap_device_2], "wand of fire"); break;
                case 8: strcat(inv_name [zap_device_2], "wand of cold"); break;
                case 9: strcat(inv_name [zap_device_2], "wand of confusion"); break;
                }
}
*/

} // end of void zap_wand();





void zapping(char ztype, int power)
{

ench_power = power;

func_pass [2] = power;
func_pass [8] = power;

beam_damage = power;

//if (ztype == 16) ztype = 21;

char luggy = zappy(func_pass, str_pass, ztype);

if (ztype == 14)
{
   strcpy(info [info_lines], "You hear a mighty clap of thunder!");
   incrl();
   x_plos = x_pos;
   y_plos = y_pos;
   noisy();
}

strcpy(beam_name, str_pass);

beam_colour = func_pass [0];
beam_range = func_pass [1];
beam_damage = func_pass [2];
if (func_pass [2] == 0) beam_damage = power;
beam_hit = func_pass [3];
beam_type = func_pass [4];
beam_flavour = func_pass [5];
thing_thrown = func_pass [6];
wand_id = func_pass [7];

func_pass [8] = power;

if (beam_name [0] == 48) beam_damage = power;

thing_thrown = 3;

switch(luggy)
{
        case 1: missile(); break;
        case 2: beam(); break;
}





/*
switch(ztype)
{
//if (inv_type [zap_device_2] == 0)
//{
        case 0: // flame
//      beam_source_x = x_pos;
//      beam_source_y = y_pos;
        beam_colour = 4;
        strcpy(beam_name, "puff of flame");
        beam_range = random2(5) + 7;
        beam_damage = 108; // should this be the same as magic missile?
        beam_hit = 60;
        beam_type = 42; // maybe make missile() put a # on wherever it hits?
        thing_thrown = 3;
        beam_flavour = 2; // fire
        missile();
        break;


        case 1: // frost
//      beam_source_y = y_pos;
        beam_colour = 15;
        strcpy(beam_name, "puff of frost");
        beam_range = random2(5) + 7;
        beam_damage = 108; // should this be the same as magic missile?
        beam_hit = 60;
        beam_type = 42; // maybe make missile() put a # on wherever it hits?
        thing_thrown = 3;
        //missile();
        beam_flavour = 3; // cold
        missile();
        break;

        case 2: // slow monster
        strcpy(beam_name, "0");
        beam_range = random2(5) + 6;
        beam_type = 0;
        beam_colour = 0;
        thing_thrown = 1;
        //beam();
        beam_flavour = 4; // magic
        beam();
        break;

        case 3: // hasting
        strcpy(beam_name, "0");
        beam_range = random2(5) + 6;
        beam_type = 0;
        beam_colour = 1;
        thing_thrown = 1;
        //beam();
        beam_flavour = 4; // magic
        beam();
        break;

        case 4: // magic missile
        strcpy(info [info_lines], "You hurl a ball of sizzling energy!");
        incrl();
//      beam_source_x = x_pos;
//      beam_source_y = y_pos;
        beam_colour = 13;//inv_col [throw_2];//icolour [inv_class [throw_2]] [inv_type [throw_2]];
        strcpy(beam_name, "magic missile");// inv_name [throw_2]);
//      if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1)
//      {
//              beam_damage = property [inv_class [throw_2]] [inv_type [throw_2]] [0];
//              beam_damage += inv_plus [throw_2];
//      }
//       else beam_damage = 2; // later put mass thing here.
        beam_range = random2(5) + 7;
        beam_damage = 107;
        beam_hit = 80;
        beam_type = 35;
//      beam_hit = strength + (0.5 * dex) + (f_abil);
//      if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1) beam_hit += inv_plus [throw_2];
        //beam_hit *= random2(200) / 100;
        thing_thrown = 3;
        beam_flavour = 1; // a magical missile
        missile();
        break;

        case 5: // healing
        strcpy(beam_name, "0");
        beam_range = random2(5) + 6;
        beam_type = 0;
        beam_colour = 2;
        thing_thrown = 1;
        beam_flavour = 4; // magic
        beam();
        break;

        case 6: // paralysis
        strcpy(beam_name, "0");
        beam_range = random2(5) + 6;
        beam_type = 0;
        beam_colour = 3;
        thing_thrown = 1;
        beam_flavour = 4; // magic
        beam();
        break;

        case 7:
        strcpy(beam_name, "bolt of fire");
        beam_range = random2(10) + 5;
        beam_damage = 112;
        beam_colour = 4;
        beam_type = 35; // 35
        thing_thrown = 1;
        beam_flavour = 2; // fire
        beam();
        break;
//}


//if (inv_type [zap_device_2] == 1)
//{
        case 8:
        strcpy(beam_name, "bolt of cold");
        beam_range = random2(10) + 5;
        beam_damage = 112;
        beam_colour = 15;
        beam_type = 35;
        thing_thrown = 1;
        beam_flavour = 3; // cold
        beam();
        break;

        case 9: // confusion
        strcpy(beam_name, "0");
        beam_range = random2(5) + 6;
        beam_type = 0;
        beam_colour = 4;
        thing_thrown = 1;
        beam_flavour = 4; // magic
        beam();
        break;

        case 10: // make invisible
        strcpy(beam_name, "0");
        beam_range = random2(5) + 6;
        beam_type = 0;
        beam_colour = 5;
        thing_thrown = 1;
        beam_flavour = 4; // magic
        beam();
        break;

        case 11: // digging
        strcpy(beam_name, "0");
        beam_range = random2(5) + 6;
        beam_type = 0;
        beam_colour = 6;
        thing_thrown = 1;
        beam_flavour = 4;
        beam();
        break;


        case 12: // fireball
//      beam_source_x = x_pos;
//      beam_source_y = y_pos;
        beam_colour = 4;
        strcpy(beam_name, "fireball");
        beam_range = random2(5) + 7;
        beam_damage = 15; // should this be the same as magic missile?
        beam_hit = 40;
        beam_type = 42; // maybe make missile() put a # on wherever it hits?
        thing_thrown = 3;
        beam_flavour = 10; // fire
        missile();
        break;

        case 13: // make invisible
        strcpy(beam_name, "0");
        beam_range = random2(5) + 6;
        beam_type = 0;
        beam_colour = 7; // 6 is used by digging
        thing_thrown = 1;
        beam_flavour = 4; // magic
        beam();
        break;


} // end of switch
*/


}


void beam(void)
{
//float beam_tot = 0; // total increments
//char beam_inc = 0; // move_x / move_y
int beam_sign_x = 0;
int beam_sign_y = 0;
//int bmox = 0;
float bmoy = 0;
int bmax = 0;
float bmay = 0;

char clouty = 0;

hurted = 0;

//int beam_x = beam_source_x;
//int beam_y = beam_source_y;

beam_x = beam_source_x;
beam_y = beam_source_y;

        brek = 1;
//      viewwindow(); // this resets the window so that dragons don't appear to do weird things.
        brek = 0;

        if (move_x > 0) beam_sign_x = 1;
        if (move_x < 0)
        {
                beam_sign_x = -1;
                move_x *= -1;
        }

        if (move_y > 0) beam_sign_y = 1;
        if (move_y < 0)
        {
                beam_sign_y = -1;
                move_y *= -1;
        }



if (move_x > 1 | move_y > 1 | move_x < -1 | move_y < -1)
{

//      beam_inc = 1;
        bmoy = move_y;
        //move_x = 0; move_y = 0;



        if (move_x > move_y)
        {
        if (beam_sign_x == 1) bmay = (float) (move_y) / (move_x + 1);
        if (beam_sign_x == -1) bmay = (float) (move_y) / (move_x + 1);
        bmax = 1;
        } else
              {
              if (beam_sign_y == 1) bmay = (float) (move_x) / (move_y + 1);
              if (beam_sign_y == -1) bmay = (float) (move_x) / (move_y + 1);
              bmax = 2;
              }
        bmoy = 1;



} //else beam_inc = 0;



for (n = 1; n < beam_range; n++)
{

move_x = beam_sign_x; move_y = beam_sign_y;


if (bmax == 1)
        {
                move_x = beam_sign_x;
                bmoy += bmay;
                if (bmoy >= 1)
                {
                        move_y = beam_sign_y;
                        bmoy -= 1;
                } else move_y = 0;
        }



if (bmax == 2)
        {
                move_y = beam_sign_y;
                bmoy += bmay;
                if (bmoy >= 1)
                {
                        move_x = beam_sign_x;
                        bmoy -= 1;
                } else move_x = 0;
        }


        if (beam_x == target_x && beam_y == target_y && beam_flavour == 10)
        {
                explosion1();
  aim_down = 0;
                return;
        }



if (beam_colour == 6)
{
        beam_x += move_x;
        beam_y += move_y;
        if (grid [beam_x] [beam_y] == 2 | grid [beam_x] [beam_y] == 4)
 {
   aim_down = 0;
   return;
 }
        if (grid [beam_x] [beam_y] == 1) grid [beam_x] [beam_y] = 17;
        if (grid [beam_x] [beam_y] == 51) grid [beam_x] [beam_y] = 67;
} else
       if (grid [beam_x + move_x] [beam_y + move_y] > 10)
{
        beam_x += move_x; beam_y += move_y;
} else
        {

                if (beam_flavour == 10)
                {
                        explosion1();
   aim_down = 0;
                        return;
                }

                if (beam_name [0] == 48 | beam_flavour != 5)
                // ie non-bouncing enchantments, non-electrical beams etc
                {
   aim_down = 0;
                        return;
                }

                strcpy(info [info_lines], "The ");
                strcat(info [info_lines], beam_name);
                strcat(info [info_lines], " bounces.");
                incrl();
                // test relay_message();
  //more();
                //nothing = getch();
                n++; // reduces beam's range by 1
                if (move_x == 0 ^ move_y == 0) //   ^ XOR
                {
                        beam_sign_x *= -1;
                        beam_sign_y *= -1;
                        //move_x *=  -1;
                        //move_y *= -1;
                }
                if (move_x != 0 && move_y != 0)
                {
                        if (grid [beam_x] [beam_y + move_y] > 10 && grid [beam_x + move_x] [beam_y] > 10)
                        {
                                beam_sign_x *= -1; beam_sign_y *= -1;
                                //move_y *= -1; move_x *= -1;
                        }
                        if (grid [beam_x + move_x] [beam_y] <= 10 && grid [beam_x] [beam_y + move_y] > 10)
                        {
                                beam_sign_x *= -1;
                                //move_x *= -1;
                        }
                        if (grid [beam_x] [beam_y + move_y] <= 10 && grid [beam_x + move_x] [beam_y] > 10)
                        {
                                beam_sign_y *= -1;
                                //move_y *= -1;
                        }
                        if ((grid [beam_x] [beam_y + move_y] <= 10 && grid [beam_x + move_x] [beam_y] <= 10))// | (grid [beam_x] [beam_y + move_y] > 10 && grid [beam_x + move_x] [beam_y]))
                        {
                                beam_sign_x *= -1; beam_sign_y *= -1;
                                //move_y *= -1; move_x *= -1;
                        }




                }
                // and more stuff here.
        }

        if (beam_x > x_pos - 7 && beam_x < x_pos + 7 && beam_y > y_pos - 7 && beam_y < y_pos + 7 && (show [beam_x - x_pos + 7] [beam_y - y_pos + 7] != 0 | (beam_x == x_pos && beam_y == y_pos)) && beam_type != 0)
        {
                if (beam_range != n)
                {
                        //brek = 1;
                        //viewwindow;
                        window(2,2,34,15);
                        textcolor(beam_colour);
                        //show [beam_x - x_pos + 7] [beam_y - y_pos + 7] = 35;
                        gotoxy(beam_x - x_pos + 17, beam_y - y_pos + 8);
                        putch(beam_type);
                        delay(30);
                }
        }

if (grid [beam_x] [beam_y] >= 100) // hit a cloud
{

if (beam_colour == 200 | beam_name [0] == 48) goto out_of_cloud_bit;

clouty = which_cloud(beam_x, beam_y);

//1 = fire, 3 = cold, 2 = stinking

if ((cloud_type [clouty] == 3 && beam_flavour == 2) | (cloud_type [clouty] == 1 && beam_flavour == 3))
{
 if (see_grid(beam_x, beam_y) == 1)
 {
  strcpy(info [info_lines], "You hear a sizzling sound!");
  incrl();
 }

   cloud_type [clouty] = 0;
   grid [cloud_x [clouty]] [cloud_y [clouty]] -= 100;
   cloud_decay [clouty] = 0;
   cloud_no--;

   n -= 3;

}

//switch(cloud_type [which_cloud

}

out_of_cloud_bit: if (beam_x == x_pos && beam_y == y_pos && beam_colour != 6) // have to do something about enchantments here.
{                                                    // ^^^ digging
        if (beam_colour == 200)
        {
  tracer = 1;
//  aim_down = 0;
  goto out_of_hit_you;
                //return;
        }

        if (beam_flavour == 10)
        {
                explosion1();
    aim_down = 0;
                return;
          }


                if (beam_name [0] != 48) // ie enchantments always hit
  {
   if (beam_hit < random2(evasion) + random2(dex) / 3 - 2)
   {
        strcpy(info [info_lines], "The ");
     strcat(info [info_lines], beam_name);
     strcat(info [info_lines], " misses you.");
     incrl();
     continue;
   }
  }




        if (beam_name [0] == 48)
        {
 aim_down = 0;
 if (beam_colour != 1 && beam_colour != 5 && beam_colour != 2)
 if ((thing_thrown == 1 | thing_thrown == 3) && you_resist_magic(ench_power) == 1)
 {
        strcpy(info [info_lines], "You resist.");
                incrl();
                return;
 }
        switch(beam_colour)
        {
                case 0: potion_effect(9, 40); return; // slow
                case 1: potion_effect(2, 0); return; // haste
                case 2: potion_effect(1, 0); return; // heal (heal wounds potion eff)
                case 3: potion_effect(10, 5); return; // paralysis
                case 4: potion_effect(11, 10); return; // confusion
                case 5: potion_effect(12, 0); return; // invisibility
                // 6 is used by digging
                case 7: you_teleport(); return;
  case 9: potion_effect(11, 10); return; // confusion?
                //case 6: break; // dig
        } // end of switch (beam_colour)

        } // end of if beam_name [0] == 48



        strcpy(info [info_lines], "The ");
        strcat(info [info_lines], beam_name);
        strcat(info [info_lines], " hits you");
//      incrl();

        hurted = 0;

        if (beam_damage > 100)
        {
                hurted += random2(beam_damage - 100);
                hurted += random2(beam_damage - 100);
                hurted += random2(beam_damage - 100);
        } else hurted += random2(beam_damage);


        check_your_resists();

        strcat(info [info_lines], "!");
        incrl();

        if (beam_flavour == 2 | stricmp(beam_name, "hellfire") == 0) scrolls_burn(3, 6); // also above
 if (beam_flavour == 3) scrolls_burn(3, 8);

        ouch(hurted, 0, 3);

        hp_ch = 1;

        n++; // reduces beam's range

 if (move_x == 0 && move_y == 0) break;

//      continue;
goto check_aimed;
}

out_of_hit_you :

if (target_x == beam_x && target_y == beam_y && aim_down == 1 && ((grid [beam_x] [beam_y] % 50 == 11 && beam_flavour == 3) | (grid [beam_x] [beam_y] % 50 == 12 && beam_flavour == 2)))
  place_cloud(8, beam_x, beam_y, 2 + random2(5)); // steam

if (grid [beam_x] [beam_y] >= 50)
{


for (o = 0; o < MNST; o++)
{
        if (mons_x [o] == beam_x && mons_y [o] == beam_y && mons_class [o] != -1 && beam_name [0] != 48)
        {

// tracer = 0 = run out of range (impossible as yet)
// tracer = 1 = hits you in range
// tracer_mons = 0 = hits no monsters
//       "     = 1 = hits monster before you (possibly also after)
//       "     = 2 = hits monster after but not before
//       "     = 3 = hits tamed monster(s) but no others
//       "     = 4 = hits tamed monster(s) + possibly other things

  if (mons_class [o] >= MLAVA0 && mons_sec [o] == 1 && (beam_x != target_x | beam_y != target_y | aim_down != 1)) goto check_aimed;

                if (beam_name [0] != 48) // ie enchantments always hit
  {
   if (beam_hit < random2(mons_ev [o]))
   {
        strcpy(info [info_lines], "The ");
     strcat(info [info_lines], beam_name);
     strcat(info [info_lines], " misses ");
                strcat(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 1)); //gmon_name [mons_class [o]]);
     strcat(info [info_lines], ".");
     incrl();
     goto check_aimed;
   }
  }



  strcpy(info [info_lines], "The ");
                strcat(info [info_lines], beam_name);
                strcat(info [info_lines], " hits ");
                strcat(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 1)); //gmon_name [mons_class [o]]);
                strcat(info [info_lines], ".");
                incrl();

                hurted = 0;

                if (beam_damage > 100)
                {
                        hurted += random2(beam_damage - 100);
                        hurted += random2(beam_damage - 100);
                        hurted += random2(beam_damage - 100);
                } else hurted += random2(beam_damage);

                check_mons_resists();

                /*
                switch(beam_flavour)
                {
                        case 2: // fire
                        if (mons_res_fire(mons_class [o]) > 0)
                        {
                                //strcpy(info [info_lines], "The ");
                                strcpy(info [info_lines], monam (mons_class [o], mons_ench [o] [2], 1));
                                strcat(info [info_lines], " appears unharmed.");
                                incrl();
                                hurted = 0;
                        }
                        break;

                }


                if (stricmp(beam_name, "hellfire") == 0)
                {
                        if (mons_res_fire(mons_class [o]) == 2)
                        {
                                strcpy(info [info_lines], monam (mons_class [o], mons_ench [o] [2], 1));
                                strcat(info [info_lines], " appears unharmed.");
                                incrl();
                                hurted = 0;
                        }
                        if (mons_res_fire(mons_class [o]) == 1)
                        {
                                strcpy(info [info_lines], monam (mons_class [o], mons_ench [o] [2], 1));
                                strcat(info [info_lines], " partially resists.");
                                incrl();
                                hurted /= 2;
                        }
                }
                            */

                mons_hp [o] -= hurted;

//              if (invis != 0 && mons_beh [o] == 0) mons_beh [o] = 1;
                if (mons_beh [o] == 0) mons_beh [o] = 1;
  if ((thing_thrown == 1 | thing_thrown == 3) && mons_beh [o] == 7 && hurted != 0) mons_beh [o] = 1;

                if (mons_hp [o] <= 0)
                {
                switch(thing_thrown)
                {
                        case 1: killer = 3; break; // your beam
                        case 3: killer = 3; break; //  "    "
                        case 2: killer = 4; break; // dragon breath &c
                }
                monster_die(o);
                }

                if (beam_flavour == 10)
                {
                        explosion1();
   aim_down = 0;
                        return;
                }

                n++;
        }

        if (mons_x [o] == beam_x && mons_y [o] == beam_y && mons_class [o] != -1 && beam_name [0] == 48)
        {
//abort();
// tracer:
  if (beam_colour == 200)
  {
   if (trac_targ != MHITYOU)
                        {


                                                   if (mons_beh [o] == 7 && tracer_mons == 0)
                                                   {
                                                                         tracer_mons = 3;
          trac_hit_tamed = 1;
                                             goto check_aimed;
                                                   }
         if (mons_beh [o] == 7 && mons_beh [i] == 7)
                                                                        {
                tracer_mons = 4;
                                                                                goto check_aimed;
                                                                        }



         if (o == mons_hit [i])
         {
          trac_hit_mons = 1;
          goto check_aimed;
         }

                                                   if (tracer_mons == 0) //(mons_beh [o] == 7 && tracer_mons == 0)
                                                   {
                                                                         tracer_mons = 3;
                                             goto check_aimed;
                                                   }
                                                   if (trac_hit_mons == 1 && tracer_mons != 1) tracer_mons = 2;// else
//                                                                                                                                                                                                                                                                                                                       tracer_mons = 1;
                                                   if (trac_hit_mons == 0) tracer_mons = 1;
                        } else // Aimed at you:
        {
//         if (mons_beh [o] == 7) trac_hit_tamed = 1;

                                                                                        /// WAS HERE!!!

                                                   if (tracer == 1 && tracer_mons != 1) tracer_mons = 2;// else
//                                                                                                                                                                                                                                                                                                                       tracer_mons = 1;
                                                   if (tracer == 0) tracer_mons = 1;
        }
//   outp(tracer_mons);
//   abort();
//   cprintf(tracer_mons + 97);
//   getch();
                                                        goto check_aimed;

  }


                brek = 0;

                if (invis != 0 && mons_beh [o] == 0) mons_beh [o] = 1;


                // cannot shift this to another module
                if (beam_colour ==  7) //: // teleportation
                {
                        if (mons_near(o) == 1 && mons_ench [o] [2] != 6)
                        {
                                strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));
                                strcat(info [info_lines], " disappears.");
                                incrl();
                        }
                        monster_teleport(o);
   aim_down = 0;
                        return;
                }

      if (beam_colour == 8)
      {
            monster_polymorph(o, 250, 100);
            aim_down = 0;
            return;
      }

                //mons_ench_f2(info, info_lines, mons_speed, mons_ench, o, mons_ench_1, mons_hp, mons_hp_max, mons_class, mons_near(o), see_invis);

//      func_pass [2] = beam_damage;

//              mons_ench_f2(info, info_lines, mons_speed, mons_ench, o, mons_ench_1, mons_hp, mons_hp_max, mons_class, mons_near(o), beam_colour, see_invis, func_pass);

//itoa(beam_damage, st_prn, 10);
//cprintf(st_prn);
//cprintf(",");
//itoa(mag_abil, st_prn, 10);
//cprintf(st_prn);
//cprintf(":");

      if (check_mons_magres(o, ench_power) == 0 && beam_colour != 1 && beam_colour != 2 && beam_colour != 5)
      {
         strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));
         strcat(info [info_lines], " resists.");
         incrl();
         aim_down = 0;
         return;
      }

      mons_ench_f2(o, mons_near(o), func_pass);

//              info_lines = func_pass [0];

                wand_id = func_pass [1];
  aim_down = 0;
                return;

                //mons_ench_f2( );

        } // end of if ...


} // end of for o

}

check_aimed : if (beam_x == target_x && beam_y == target_y && aim_down == 1)
{
 goto landed;
}



} // end of for n

landed : if (beam_x == target_x && beam_y == target_y && aim_down == 1)
// had if beam == tracer
{
//      tracer = 0;
 aim_down = 0;
        return;
}

if (beam_flavour == 10)
{
        explosion1();
 aim_down = 0;
        return;
}


if (beam_name [0] == 48 && beam_colour != 6 && beam_colour != 200)
{
        strcpy(info [info_lines], "Nothing appears to happen.");
        incrl();
}

aim_down = 0;
// ^ must have!

}  // end of void beam(void);









void check_your_resists(void)
{

        switch(beam_flavour)
        {
                case 2: // fire
                if (res_fire != 0)
                {
                        strcat(info [info_lines], "... But it doesn't seem to burn you!");
                        //incrl();
                        hurted = 0;
                        //n++;
                        //scrolls_burn(3); // also below
                        //continue;
                }
                break;

                case 3: // cold
                if (res_cold != 0)
                {
                        strcat(info [info_lines], "... But you don't feel cold!");
                        //incrl();
                        hurted = 0;
                        //n++;
                        //continue;
                }
                break;

 case 6: // poison
 if (res_poison > 0)
 {
  strcat(info [info_lines], "... but you resist!");
  hurted = 0;
 } else poison ++;
        break;

 case 7: // negativity
 if (prot_life != 0)
        {
  strcat(info [info_lines], "... but you resist!");
                hurted = 0;
                break;
        }
 incrl();
 strcpy(info [info_lines], "You feel drained!");
        incrl();
 lose_level();
        break;

// 8 is stinking cloud

 } // end switch

        if (stricmp(beam_name, "hellfire") == 0 | beam_flavour == 20) // 20 is lava
        {
                if (res_fire != 0)
                {
                        strcat(info [info_lines], "... But you partially resist it");
                        //incrl();
                        //n++;
                        hurted /= 2;
                }
                //break;
        }



}





void check_mons_resists(void)
{

                switch(beam_flavour)
                {
                        case 2: // fire
                        if (mons_res_fire(mons_class [o]) > 0 | (mons_inv [o] [2] != 501 && item_dam [mons_inv [o] [2]] % 30 == 2))
                        {
                                //strcpy(info [info_lines], "The ");
    if (mons_near(o) == 1 && mons_ench [o] [2] != 6)
    {
                        strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));
                                strcat(info [info_lines], " appears unharmed.");
                                incrl();
    }
    hurted = 0;
                        }

   if (mons_res_fire(mons_class [o]) == -1 && (mons_inv [o] [2] == 501 | item_dam [mons_inv [o] [2]] % 30 != 2))
   {
     hurted *= 1.5;
   }
   break;


   case 3: // cold
                        if (mons_res_cold(mons_class [o]) > 0 | (mons_inv [o] [2] != 501 && item_dam [mons_inv [o] [2]] % 30 == 3))
                        {
                                //strcpy(info [info_lines], "The ");
    if (mons_near(o) == 1 && mons_ench [o] [2] != 6)
    {
     strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));
                                strcat(info [info_lines], " appears unharmed.");
                                incrl();
    }
                                hurted = 0;
                        }

   if (mons_res_cold(mons_class [o]) == -1 && (mons_inv [o] [2] == 501 | item_dam [mons_inv [o] [2]] % 30 != 3))
   {
     hurted *= 1.5;
   }
   break;


   case 6: // poison
   if (random2(3) != 0)
   {
    switch(thing_thrown)
    {
     case 1:
     case 3:
     poison_monster(o, 0);
     break;

     case 2:
     case 4:
     poison_monster(o, 1);
     break;
    }
   }

   if (mons_res_poison(mons_class [o]) != 0)
   {
    if (mons_near(o) == 1 && mons_ench [o] [2] != 6)
    {
                                strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));
                                strcat(info [info_lines], " appears unharmed.");
                                incrl();
    }
                                hurted = 0;
   }
   break;

   // 8: acid???

                        case 7: // negativity
                        if (mons_holiness(mons_class [o]) > 0)
                        {
                                //strcpy(info [info_lines], "The ");
    if (mons_near(o) == 1 && mons_ench [o] [2] != 6)
    {
                        strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));
                                strcat(info [info_lines], " appears unharmed.");
                                incrl();
    }
    hurted = 0;
                        } else
                         {
      if (mons_near(o) == 1)
      {
       strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));
       strcat(info [info_lines], " is drained.");
       incrl();
      }
      mons_HD [o] --;
      mons_hp_max [o] -= 3 + random2(6);
      mons_hp [o] -= 3 + random2(6);
      if (mons_hp [o] >= mons_hp_max [o]) mons_hp [o] = mons_hp_max [o];
      if (mons_HD [o] <= 0) mons_hp [o] = 0;
                                } // end else

//   if (mons_res_neg(mons_class [o]) == -1)
//   {
//     hurted *= 1.5;
//   }
   break;


                } // end of switch


                if (stricmp(beam_name, "hellfire") == 0 | beam_flavour == 20)
                {
                        if (mons_res_fire(mons_class [o]) == 2)
                        {
    if (mons_near(o) == 1 && mons_ench [o] [2] != 6)
    {
                                 strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));
                                 strcat(info [info_lines], " appears unharmed.");
                                 incrl();
    }
                                hurted = 0;
                        }
                        if (mons_res_fire(mons_class [o]) == 1 | (mons_inv [o] [2] != 501 && item_dam [mons_inv [o] [2]] % 30 == 2))
                        {
    if (mons_near(o) == 1 && mons_ench [o] [2] != 6)
    {
                                strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));
                                strcat(info [info_lines], " partially resists.");
                                incrl();
    }
                        hurted /= 2;
                        }
   if (mons_res_fire(mons_class [o]) == -1 && (mons_inv [o] [2] == 501 | item_dam [mons_inv [o] [2]] % 30 != 2))
   {
     hurted *= 1.2;// hellfire
   }
   }


}


int check_mons_magres(int mn, int pow)
{

int mrs = mon_resist_mag(mons_class [mn], mons_HD [mn]);

if (mrs == 5000) return 0;

//if (random2(pow) + random2(pow) + random2(pow) >= mrs) return 1;
if (random2(pow) / 3 + random2(pow) / 3 + random2(pow) / 3 >= mrs) return 1;
/*
itoa(pow, st_prn, 10);
cprintf(st_prn);
cprintf(",");
itoa(mrs, st_prn, 10);
cprintf(st_prn);
*/
return 0;

} // end of check_mons_magres


void eat_food(void)
{
int food_eat_1;
int food_eat_2;

unsigned char nthing;

if (inv_no == 0)
        {
        strcpy(info [info_lines], "You aren't carrying anything.");
        incrl();
        return;
        }

query : strcpy(info [info_lines], "Eat which item?");
incrl();

// test relay_message();

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = invent(4, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if (keyin == '*') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
       goto query;

        /*              strcpy(info [info_lines], "Eat which item?"); incrl();
                relay_message();
                keyin = get_ch();*/
        }

}
/*
if (keyin == '*')
{
        inventory(-1);
        strcpy(info [info_lines], "Eat which item?"); incrl();
        relay_message();
        keyin = getch();
}
*/
food_eat_1 = (int) keyin;

if (food_eat_1 < 65 | (food_eat_1 > 90 && food_eat_1 < 97) | food_eat_1 > 122)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

food_eat_2 = conv_lett(food_eat_1);

if (inv_quant [food_eat_2] == 0)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}


if (inv_class [food_eat_2] != 4)
{
        strcpy(info [info_lines], "You can't eat that!");
        incrl();
        return;
}

inv_quant [food_eat_2] --;

if (inv_quant [food_eat_2] == 0)
{
        inv_no --;
        if (item_wielded == food_eat_2)
        {
                item_wielded = -1;
                strcpy(info [info_lines], "You are now empty handed.");
                incrl();
        }
}


eaten(inv_type [food_eat_2]); // put different values (ie above 30) for eating things not in class 4, eg corpses.

if (hunger > 12000) hunger = 12000;

/*if (hunger > 3449)
{
        strcpy(info [info_lines], "You feel full.");
        incrl();
        hung_ch = 1;
}*/

food_change();

turnover = 1;

burden_change();

/*if (delay_t > 0)
{
 more();
 scrloc = 0;
}*/

} // end of void eat_food()



void food_change(void)
{
hung_state = 0;

if (hunger <= 1000)
{
 hung_state = 1; // starving
 return;
}
if (hunger <= 2600)
{
 hung_state = 2; // hungry
 return;
}
if (hunger < 7000)
{
 hung_state = 3; // -
 return;
}
if (hunger < 11000)
{
 hung_state = 4; // Full
 return;
}
hung_state = 5; // Engorged

} // end of void food_change()


void eaten(int food_eat_3) // this contains results of eating things.
{

hung_ch = 1;

//int *hungry;

//hungry = &hunger;

func_pass [0] = hunger;
func_pass [1] = delay_t;
func_pass [2] = delay_doing;


int gloggj = eating(info, info_lines, func_pass, food_eat_3);

hunger = func_pass [0];
delay_t = func_pass [1];
delay_doing = func_pass [2];


incrl();

if (food_eat_3 == 6)
{
 restore_str();
 restore_int();
 restore_dex();
}

/*
switch(food_eat_3)
{
        case 0:
        strcpy(info [info_lines], "That meat ration really hit the spot!");
        incrl();
        hunger += 2500;
        delay_t = 4;
        delay_doing = 0;
        hung_ch = 1;
        return;

        case 1:
        strcpy(info [info_lines], "That bread ration really hit the spot!");
        incrl();
        hunger += 2200;
        delay_t = 4;
        delay_doing = 0;
        hung_ch = 1;
        return;

        case 3: // apple
        strcpy(info [info_lines], "Mmmm... Yummy apple.");
        incrl();
        hunger += 300;
        delay_t = 1;
        delay_doing = 0;
        hung_ch = 1;
        return;

        case 2: // pear
        strcpy(info [info_lines], "Mmmm... Yummy pear.");
        incrl();
        hunger += 300;
        delay_t = 1;
        delay_doing = 0;
        hung_ch = 1;
        return;

        case 4:
        strcpy(info [info_lines], "That choko was very bland.");
        incrl();
        hunger += 300;
        delay_t = 1;
        delay_doing = 0;
        hung_ch = 1;
        return;

} // end of switch
*/

} // end of void eaten (int food_eat_3)


// Probably put the other item use code here.



void drink(void)
{
int drink_1;
int drink_2;
unsigned char nthing;

if (inv_no == 0)
        {
        strcpy(info [info_lines], "You aren't carrying anything.");
        incrl();
        return;
        }

if (berserker != 0)
{
 strcpy(info [info_lines], "You are too berserk!");
 incrl();
 return;
}

query : strcpy(info [info_lines], "Drink which item?");
incrl();

// test relay_message();

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = invent(8, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if (keyin == '*') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
    goto query;
        /*              strcpy(info [info_lines], "Drink which item?"); incrl();
                relay_message();
                keyin = get_ch();*/
        }
}
/*
if (keyin == '*')
{
        inventory(-1);
        strcpy(info [info_lines], "Drink which item?"); incrl();
        relay_message();
        keyin = getch();
}
*/
drink_1 = (int) keyin;

if (drink_1 < 65 | (drink_1 > 90 && drink_1 < 97) | drink_1 > 122)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

drink_2 = conv_lett(drink_1);

if (inv_quant [drink_2] == 0)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}


if (inv_class [drink_2] != 8)
{
        strcpy(info [info_lines], "You can't drink that!");
        incrl();
        return;
}

inv_quant [drink_2] --;

if (inv_quant [drink_2] == 0)
{
        inv_no --;
        if (item_wielded == drink_2)
        {
                item_wielded = -1;
                strcpy(info [info_lines], "You are now empty handed.");
                incrl();
        }
}

burden_change();
//burden -= 60;

turnover = 1;

potion_effect(inv_type [drink_2], 40);

hunger += 40;

//strcpy(inv_name [drink_2], ident(inv_class [drink_2], inv_type [drink_2], inv_plus [drink_2]));
inv_ident [drink_2] = 1;

set_id(inv_class [drink_2], inv_type [drink_2], 1);

if (hunger > 12000) hunger = 12000;

food_change();

} // end of void drink()



void potion_effect(char pot_eff, int pow)
{

char abil_change = 0;

switch(pot_eff)
{
//if (inv_type [drink_2] == 0) // potion of healing

//{
        case 0:
        strcpy(info [info_lines], "You feel better.");
        incrl();
        hp += random2(7) + 5; if (hp > hp_max) hp = hp_max;
        hp_ch = 1;
        //if (poison > 0)
        //{
                poison = 0;
                rotting = 0;
        //}
        wand_id = 1;
        break;
//}

//if (inv_type [drink_2] == 1) // potion of extra healing
//{
        case 1:
        strcpy(info [info_lines], "You feel much better.");
        incrl();
        hp += random2(10) + random2(10) + random2(10) + 10;
/* if (hp > hp_max)
 {
  hp_max += (hp - hp_max) / 10 + 1;
  hp = hp_max;
 }*/
 if (hp > hp_max) hp = hp_max;
        hp_ch = 1;
        wand_id = 1;
        break;
//}

//if (inv_type [drink_2] == 2) // potion of speed
//{
        case 2:
        if (haste == 0) strcpy(info [info_lines], "You feel yourself speed up.");
                else strcpy(info [info_lines], "Your high speed feels more durable.");
        incrl();
//      haste += random2(50) + 40;
        if (slow > 0)
        {
                slow = 0;
        } else haste += random2(50) + 40;
        wand_id = 1;
 if (haste > 90) haste = 90;

 break;
//}

        case 3: // potion of might
        if (might == 0) strcpy(info [info_lines], "You feel very mighty all of a sudden.");
        else
        {
                if (might < 150) might += random2(40) + 35;
                strcpy(info [info_lines], "You're still pretty mighty.");
                incrl();
                return;
        }
        incrl();
        might += random2(40) + 35;
        strength += 5;
 max_strength += 5;
        strength_ch = 1;
 if (might > 75) might = 75;
        break;

        case 4: // potion of increase strength
        strcpy(info [info_lines], "You feel strong!");
        incrl();
        abil_change = random2 (2) + 1;
 strength += abil_change;
 if (strength > max_strength) max_strength = strength; //+= abil_change;
        strength_ch = 1;
        break;

        case 5: // potion of increase dexterity
        strcpy(info [info_lines], "You feel dextrous!");
        incrl();
        abil_change = random2 (2) + 1;
 dex += abil_change;
 if (dex > max_dex) max_dex = dex; // += abil_change;
        dex_ch = 1;
        break;

        case 6: // potion of increase intelligence
        strcpy(info [info_lines], "You feel intelligent!");
        incrl();
        abil_change = random2 (2) + 1;
 intel += abil_change;
 if (intel > max_intel) max_intel = intel; // += abil_change;
        intel_ch = 1;
        break;

        case 7: // levitation
        if (lev == 0)
        {
                strcpy(info [info_lines], "You feel very buoyant!");
                incrl();
                strcpy(info [info_lines], "You gently float upwards from the floor.");
                incrl();
        } else
              {
              strcpy(info [info_lines], "You feel more buoyant.");
              incrl();
              }
        if (lev >= 0) lev += random2 (20) + 25;
 if (lev > 45) lev = 45;
        break;

        case 8: // poison!!
        if (res_poison > 0)
        {
                strcpy (info [info_lines], "You feel slightly nauseous.");
                incrl();
                break;
        }
        if (poison > 0)
        {
                strcpy(info [info_lines], "You feel even sicker.");
                info_lines ++;
        } else
              {
              strcpy (info [info_lines], "That potion tasted very nasty...");
              info_lines ++;
              }
        poison += random2(3) + random2(3) + 1;
        wand_id = 1;
        break;

        case 9: // slow
        if (slow == 0) strcpy(info [info_lines], "You feel yourself slow down.");
                else strcpy(info [info_lines], "Your low speed feels more durable.");
        incrl();
        if (haste > 0)
        {
                haste = 0;
        } else slow += random2(pow) + pow;
        wand_id = 1;
 if (slow > 90) slow = 90;
        break;

        case 10: // paralysis
        if (paralysis == 0) strcpy(info [info_lines], "You suddenly lose the ability to move!");
                else strcpy(info [info_lines], "You still can't move!");
        incrl();
        paralysis += random2(8) + 5;
        wand_id = 1;
 if (paralysis > 13) paralysis = 13;
        break;

        case 11: // confusion
        if (conf == 0) strcpy(info [info_lines], "You feel confused.");
                else strcpy(info [info_lines], "You feel very confused.");
        incrl();
        conf += random2(20) + 15;
        wand_id = 1;
 if (conf > 40) conf = 40;
        break;

        case 12: // Invisibility (can I do it?)
        if (invis == 0) strcpy(info [info_lines], "You flicker for a moment, then turn invisible!");
                else strcpy(info [info_lines], "You feel hidden.");
        incrl();
        if (invis >= 0) invis += random2(15) + 35;
//      your_sign = 73;
        wand_id = 1;
 if (invis > 100) invis = 100;
        break;

        case 13: // oatmeal - always gluggy white/gray?
        strcpy(info [info_lines], "That potion was really gluggy!");
        incrl();
        hunger += 6000;
        hung_ch = 1;
 if (hunger > 12000) hunger = 12000;
        break;

        case 14: // disease
        strcpy(info [info_lines], "There was something very wrong with that potion! ");
        incrl();
 if (sust_abil != 0)
 {
  strcpy(info [info_lines], "You feel slightly ill for a moment.");
  incrl();
  break;
 }
 else
        switch(random2(3))
        {
                case 0: strcpy(info [info_lines], "You feel weakened.");
                incrl();
                strength -= random2(3) + random2(2) + 1;
  if (strength < 3) strength = 3;
                strength_ch = 1;
                burden_change();
                break;

                case 1: strcpy(info [info_lines], "You feel clumsy.");
                incrl();
                dex -= random2(3) + random2(2) + 1;
  if (dex < 3) dex = 3;
                dex_ch = 1;
                break;

                case 2: strcpy(info [info_lines], "You feel dopey.");
                incrl();
                intel -= random2(3) + random2(2) + 1;
  if (intel < 3) intel = 3;
                intel_ch = 1;
                break;

                /*
                case 3: strcpy(info [info_lines], "You feel less resilient.");
                incrl();
                hp_max -= random2(3) + random2(3) + 1;
                if (hp > hp_max) hp = hp_max;
                hp_max_ch = 1;
                hp_ch = 1;
                break;
                */
        }
        break;

        // Don't generate random2ly - should be rare and interesting
        case 15: // decay
        strcpy(info [info_lines], "You feel your flesh start to rot away!");
        incrl();
        rotting += random2(10) + 10;
        break;

 case 16: // water
 strcpy(info [info_lines], "This tastes like water.");
 incrl();
 hunger += 20;
 break;

 case 17: // experience
 strcpy(info [info_lines], "You feel more experienced!");
 incrl();
 xp = exp_needed(xl + 2) + 1;
 level_change();
 break;

 case 18: // magic
 strcpy(info [info_lines], "You feel magical!");
 incrl();
        ep += random2(10) + random2(10) + 5;
 if (ep > ep_max)
 {
  ep_max += (ep - ep_max) / 4 + 1;
  ep = ep_max;
 }
        ep_ch = 1;
 break;

 case 19: // restore abilities
 strcpy(info [info_lines], "You feel refreshed."); // ...can't think of anything better right now
 incrl();
 restore_str();
 restore_int();
 restore_dex();
 break;

        case 20: // strong poison!!
        if (res_poison > 0)
        {
                strcpy (info [info_lines], "You feel very nauseous.");
                incrl();
                break;
        }
        if (poison > 0)
        {
                strcpy(info [info_lines], "You feel even sicker.");
                info_lines ++;
        } else
              {
              strcpy (info [info_lines], "That potion tasted extremely nasty...");
              info_lines ++;
              }
        poison += random2(7) + random2(7) + 3;
        wand_id = 1;
        break;

} // end of switch (pot_eff)



} // end of void potion_effect(pot_eff)







void read_scroll(void)
{
//int sc_read_1;
//int sc_read_2; Now in globvars.h because of random2 uselessness

int id_used = 0;
unsigned char nthing;

// some variables used inside the switch:
char affected = 0;



if (inv_no == 0)
        {
        strcpy(info [info_lines], "You aren't carrying anything.");
        incrl();
        return;
        }

if (berserker != 0)
{
 strcpy(info [info_lines], "You are too berserk!");
 incrl();
 return;
}


query : strcpy(info [info_lines], "Read which item?");
incrl();

// test relay_message();

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = invent(6, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if (keyin == '*') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
      goto query;

/*              strcpy(info [info_lines], "Read which item?"); incrl();
                relay_message();
                keyin = get_ch();*/
        }
/*
        inventory(6);
        strcpy(info [info_lines], "Read which item?"); incrl();
        relay_message();
        keyin = get_ch();
*/
}
/*
if (keyin == '*')
{
        inventory(-1);
        strcpy(info [info_lines], "Read which item?"); incrl();
        relay_message();
        keyin = get_ch();
}
*/
sc_read_1 = (int) keyin;

if (sc_read_1 < 65 | (sc_read_1 > 90 && sc_read_1 < 97) | sc_read_1 > 122)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

sc_read_2 = conv_lett(sc_read_1);

if (inv_quant [sc_read_2] == 0)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

if (inv_class [sc_read_2] == 10)
{
//      strcpy(info [info_lines], "That's a book, not a scroll.");
//      incrl();
 read_book(sc_read_2);
        return;
}

if (inv_class [sc_read_2] != 6)
{
        strcpy(info [info_lines], "You can't read that!");
        incrl();
        return;
}

if (inv_type [sc_read_2] == 15)
{
  strcpy(info [info_lines], "This scroll appears to be blank.");
  incrl();
  turnover = 1;
  set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
  return;
}




inv_quant [sc_read_2] --;

if (inv_quant [sc_read_2] == 0)
{
        inv_no --;
        if (item_wielded == sc_read_2)
        {
                item_wielded = -1;
                strcpy(info [info_lines], "You are now empty handed.");
                incrl();
        }
}

turnover = 1;

count_y = sc_read_2;

strcpy(info [info_lines], "As you read the scroll, it crumbles to dust.");
incrl();
burden_change();
//burden -= 50;

if (conf != 0)
{
 random_uselessness(random2(9));
 return;
}
// The scroll effects:

switch(inv_type [sc_read_2])
{


case 0: // identify
inv_ident [count_y] = 2;
set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
identify(1);
return;


case 1: // teleport
        you_teleport();
        set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
return;

case 2: // fear
strcpy(info [info_lines], "You feel scary!");
incrl();
set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
 mass_enchantment(4, 50);
break;

case 3: // noise
strcpy(info [info_lines], "You hear a loud clanging noise!");
incrl();
x_plos = x_pos;
y_plos = y_pos;
noisy();
set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
return;

case 4: // scroll of remove curse
        remove_curse();
        set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
break;

case 5: // detect curse
 detect_curse();
set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
break;

case 6: // summoning
strcpy(info [info_lines], "A horrible Thing appears!");
incrl();
create_monster(49, 25, 7, x_pos, y_pos, MHITNOT);
set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
break;


case 7: // scroll of enchant weapon
if (item_wielded == -1)
{
        strcpy(info [info_lines], "Nothing appears to happen.");
        incrl();
        return;
}


if (inv_class [item_wielded] == 0 | inv_class [item_wielded] == 1) // | inv_class [item_wielded] == 1)
{

if (inv_class [item_wielded] == 0 && inv_dam [item_wielded] > 180)
{
        strcpy(info [info_lines], "Nothing appears to happen.");
        incrl();
        return;
}

 if (inv_dam [item_wielded] == 4) // electrocution
 {
   if (inv_plus [item_wielded] > 130)
   {
    inv_plus [item_wielded] -= 100;
   }
                item_name(inv_class [item_wielded], inv_type [item_wielded], inv_dam [item_wielded], inv_plus [item_wielded], inv_quant [item_wielded], inv_ident [item_wielded], 4, str_pass);

   inv_plus [item_wielded] += 3 + random2(3) + random2(3);
   if (inv_plus [item_wielded] >= 59) inv_plus [item_wielded] = 59;

   strcpy(info [info_lines], str_pass);
   strcat(info [info_lines], " glows blue for a moment.");
   incrl();
   return;
 }


        if (inv_plus [item_wielded] >= 154)
        {
                if (random2(9) >= (inv_plus [item_wielded] - 150))
                {
                item_name(inv_class [item_wielded], inv_type [item_wielded], inv_dam [item_wielded], inv_plus [item_wielded], inv_quant [item_wielded], inv_ident [item_wielded], 4, str_pass);

                strcpy(info [info_lines], str_pass);
                strcat(info [info_lines], " glows silver for a moment.");
                incrl();
                inv_plus [item_wielded] -= 100;
                set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
                return;
                }
        }

        if (inv_plus [item_wielded] - 50 >= 4 && inv_plus [item_wielded] < 130)
        {
                if (random2(9) < (inv_plus [item_wielded] - 50)) goto nothing_happened;
        }



        // vVvVv    This is *here* for a reason!
        item_name(inv_class [item_wielded], inv_type [item_wielded], inv_dam [item_wielded], inv_plus [item_wielded], inv_quant [item_wielded], inv_ident [item_wielded], 4, str_pass);

        inv_plus [item_wielded] ++;
        if (inv_plus [item_wielded] > 130) inv_plus [item_wielded] -= 100;
        //strcpy(info [info_lines], "Your ");
        //in_name(item_wielded, 4);

        strcpy(info [info_lines], str_pass);
        strcat(info [info_lines], " glows green for a moment.");
        incrl();
        set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
        return;

//        strcat(info [info_lines],
}

nothing_happened: strcpy(info [info_lines], "Nothing appears to happen.");
incrl();
return; // end of case 7: enc weapon


case 8: // ench armour
for (i = 0; i < 7; i ++)
{
        if (i == 6)
        {
                strcpy(info [info_lines], "Nothing appears to happen.");
                incrl();
                return;
        }

        if (armour [i] != -1) break;
}

do
{
        affected = random2(6);
} while (armour [affected] == -1);




        if (inv_plus [armour [affected]] >= 153)
        {
                if (random2(8) >= (inv_plus [armour [affected]] - 150))
                {
                item_name(inv_class [armour [affected]], inv_type [armour [affected]], inv_dam [armour [affected]], inv_plus [armour [affected]], inv_quant [armour [affected]], inv_ident [armour [affected]], 4, str_pass);

                inv_plus [armour [affected]] -= 100;
                set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);

                strcpy(info [info_lines], str_pass);
                strcat(info [info_lines], " glows silver for a moment.");
                incrl();
                return;
                }
        }

        if (inv_plus [armour [affected]] - 50 >= 3 && inv_plus [armour [affected]] < 130)
        {
                if (random2(8) < (inv_plus [armour [affected]] - 50)) goto nothing_happened_2;
        }



        // vVvVv    This is *here* for a reason!
        item_name(inv_class [armour [affected]], inv_type [armour [affected]], inv_dam [armour [affected]], inv_plus [armour [affected]], inv_quant [armour [affected]], inv_ident [armour [affected]], 4, str_pass);

        inv_plus [armour [affected]] ++;
        if (inv_plus [armour [affected]] > 130) inv_plus [armour [affected]] -= 100;
        //strcpy(info [info_lines], "Your ");
        //in_name(armour [affected], 4);

        set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);

        strcpy(info [info_lines], str_pass);
        strcat(info [info_lines], " glows green for a moment.");
 if (affected != 5) AC ++; else shield_class ++;
 AC_ch = 1;
        incrl();
        return; // end of ench armour

case 9: // torment
        beam_x = x_pos;
        beam_y = y_pos;
        torment();
 set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
        return;

case 10: // random uselessness
        random_uselessness(random2(9)); //random2 (1));
        return;

case 11: // curse weapon
        if (item_wielded == -1 | inv_class [item_wielded] > 0 | inv_plus [item_wielded] > 130)
        {
                goto nothing_happened_2;
        }
                in_name(item_wielded, 4);
                strcpy(info [info_lines], str_pass);
                strcat(info [info_lines], " glows black for a moment.");
                incrl();
                inv_plus [item_wielded] += 100;
        set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
                return;


case 12: // curse armour
for (i = 0; i < 7; i ++)
{
        if (i == 6)
        {
                goto nothing_happened_2;
        }
        if (armour [i] != -1 && inv_plus [armour [i]] < 130) break;
}
do
{
        affected = random2(6);
} while (armour [affected] == -1 | inv_plus [armour [affected]] > 130);

        // vVvVv    This is *here* for a reason!
        item_name(inv_class [armour [affected]], inv_type [armour [affected]], inv_dam [armour [affected]], inv_plus [armour [affected]], inv_quant [armour [affected]], inv_ident [armour [affected]], 4, str_pass);
        inv_plus [armour [affected]] += 100;
        set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
        strcpy(info [info_lines], str_pass);
        strcat(info [info_lines], " glows black for a moment.");
        incrl();
        return;


case 13: // immolation
strcpy(info [info_lines], "The scroll explodes in your hands!");
incrl();

        beam_type = 43;
        beam_damage = 107;
        beam_flavour = 2; // <=- not sure about this
        beam_x = x_pos;
        beam_y = y_pos;
        strcpy(beam_name, "fiery explosion");
        beam_colour = RED;
        thing_thrown = 1; // your explosion, (not someone else's explosion)
        explosion();
 set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
break;

case 14: // blink

   blink();

   set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);

break;

// This is never called; it is dealt with earlier:
case 15: // paper
break;

case 16: // magic mapping
if (level_type == 1)
{
 strcpy(info [info_lines], "You feel momentarily disoriented.");
 incrl();
 set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
 break;
}
strcpy(info [info_lines], "You feel aware of your surroundings.");
incrl();
magic_mapping(grid, map, x_pos, y_pos, 21);
set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
break;

case 17: // forgetting
strcpy(info [info_lines], "You feel momentarily disoriented.");
incrl();
forget_map();
set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
break;


} // end of switch


return;

nothing_happened_2: strcpy(info [info_lines], "Nothing appears to happen.");
incrl();
return;


} // end of void read_scroll()


void blink(void)
{

strcpy(info [info_lines], "Blink to where?");
incrl();
// test relay_message();

direction(100);

if (nothing == -1)
{
        strcpy(info [info_lines], "The spell fizzles.");
        incrl();
        return;
}

if (grid [target_x] [target_y] <= 10 | (grid [target_x] [target_y] >= 50 && (grid [target_x] [target_y] <= 110 | grid [target_x] [target_y] >= 150)))
{
  strcpy(info [info_lines], "Your body is wracked with pain!");
  incrl();
  if (deaths_door == 0) hp = (hp / 2) + 1;
  hp_ch = 1;
  you_teleport();
  return;
}

x_pos = target_x;
y_pos = target_y;
//x_pos += move_x;
//y_pos += move_y;

move_x = 0;
move_y = 0;

}

void random_blink(void)
{
 int passed [2];
 if (random_near_space(passed) == 0 | (x_pos == passed [0] && y_pos == passed [1]))
        {
  strcpy(info [info_lines], "You feel rather strange for a moment.");
                incrl();
                return;
        }

 strcpy(info [info_lines], "You blink.");
        incrl();

        x_pos = passed [0];
        y_pos = passed [1];
}

void mass_enchantment(int wh_enchant, int pow)
{

brek = 0; viewwindow();
        for (i = 0; i < MNST; i ++)
        {
                if (mons_class [i] == -1) continue;

                if (mons_x [i] > x_pos - 7 && mons_x [i] < x_pos + 7 && mons_y [i] > y_pos - 7 && mons_y [i] < y_pos + 7 && show [mons_x [i] - x_pos + 7] [mons_y [i] - y_pos + 7] != 0)
                {
                        if (mons_ench_1 [i] == 1)
                                for (p = 0; p < 3; p ++)
                                {
                                        if (mons_ench [i] [p] == wh_enchant) brek = 1;
                                } // end of for p

                        for (p = 0; p < 3; p ++)
                        {
                        if (brek == 1)
                        {
                                brek = 0;
                                break;
                        }
                        if (mons_ench [i] [p] == 0) // | p == 2) // replaces 3rd enchantment if all full.
                                {
                                        mons_ench [i] [p] = wh_enchant;
                                        //mons_beh [i] = 3;
                                        mons_ench_1 [i] = 1;
        //                              strcpy(info [info_lines], "Scared!");
        //                                incrl();
                                        break;
                                }
                        } // end of for p
                } // end of if visible
        } // end of for i


} // end of void cause_fear()


void fireball(char source, int power)
{

strcpy(info [info_lines], "Which direction? (* to target)");
incrl();
// test relay_message();

direction(1);

//set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);

if (nothing == -1)
{
        strcpy(info [info_lines], "The spell fizzles.");
        incrl();
        return;
}


beam_source_x = x_pos; beam_source_y = y_pos;

//if (source == 1)

zapping(12, power); // cast a spell

//   else zapping(12, 40); // wand

        //zapping(12);
//      int thing_to_do =

//cout << "col: " << beam_colour << "hit " << beam_hit << "dam " << beam_damage << "ty " << beam_type << "nam:" << beam_name;
//      zapping(12, beam_colour, beam_name, beam_range, beam_damage, beam_flavour, beam_type, thing_thrown, beam_hit);
        //missile();

        move_x = 0; move_y = 0;

}

void cast_fire_storm(int powc)
{

   char stx = 0;
   char sty = 0;
   char cl_x = 0;
                        char cl_y = 0;
   int summd = 0;
strcpy(info [info_lines], "Where?");
incrl();

direction(100);

if (nothing == -1)
{
        strcpy(info [info_lines], "The spell fizzles.");
        incrl();
        return;
}

cl_x = target_x;
cl_y = target_y;

strcpy(info [info_lines], "A raging storm of fire appears!");
incrl();

beam_colour = RED;

//beam_colour = WHITE;
//beam_range = 3 + random2(3);
//beam_damage = 0;
//beam_hit = 20;
//beam_type = 2;
//beam_flavour = 7; // exploding gas on target // 7; // gas?
//thing_thrown = 4;//? ???? ?

 for (stx = -2; stx < 3; stx++)
   {

      for (sty = -2; sty < 3; sty++)
      {
          if ((stx == -2 && sty == -2) | (stx == -2 && sty == 2) | (stx == 2 && sty == -2) | (stx == 2 && sty == 2))
                                                                                                                continue;

                beam_range = 1 + random2(5) + random2(powc) / 50;
                       // was 1
          if (beam_range > 20) beam_range = 20 + random2(4);
            beam_type = 1;

            target_x = cl_x + stx;
            target_y = cl_y + sty;

            if (random2(4) == 0 && grid [target_x] [target_y] == 17) // > 10 && grid [target_x] [target_y] < 100 && (target_x != x_pos | target_y != y_pos))
            {
                   summd = mons_place(21, 1, target_x, target_y, 4, MHITNOT);
//int mons_place(int typed, int type_place, int px, int py, char behaviour, int hitting)
//  create_monster(21, 22, 3, target_x, target_y, MHITNOT);
//                   continue;
            }

            if (grid [target_x] [target_y] > 10 && grid [target_x] [target_y] < 100 && (target_x != x_pos | target_y != y_pos)) //100)
            {
//               place_cloud();
               place_cloud(1, target_x, target_y, beam_range);
            }

//      cprintf("x");

      } // end of sty

   } // end of stx

//} // end of freezing_cloud(c,c)

brek = 1;
viewwindow();

} // end of cast_fire_storm


char spell_direction(void)
{

strcpy(info [info_lines], "Which direction? (* to target)");
incrl();
// test relay_message();

direction(1);

//set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);

if (nothing == -1)
{
        strcpy(info [info_lines], "The spell fizzles.");
        incrl();
        return -1;
}

beam_source_x = x_pos; beam_source_y = y_pos;
return 1;

}




void identify(char pow)
{

//      strcpy(info [info_lines], "You cast a spell of identification.");
//      incrl();

        //strcpy(inv_name [count_y], "of identify");

   int id_used = 1;

        if (pow == 1 && random2(3) == 0) id_used = random2 (3) + 1;
        char nthing;

        do
        {
        query : strcpy(info [info_lines], "Identify which item?");
        incrl();
        // test relay_message();

        keyin = get_ch();

        if (keyin == '*' | keyin == '?')
        {
                if (keyin == '?') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
                if (keyin == '*') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
                if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
                {
                                keyin = nthing;
                } else
                {
                        /*strcpy(info [info_lines], "Identify which item?"); incrl();
                        relay_message();
                        keyin = get_ch();*/
         goto query;
                }
                /*
                inventory(-1);
                relay_message();
                keyin = get_ch();
                */
        }

   if (keyin == 27) return;

        sc_read_1 = (int) keyin;

        if (sc_read_1 < 65 | (sc_read_1 > 90 && sc_read_1 < 97) | sc_read_1 > 122)
        {
                strcpy(info [info_lines], "You don't have any such object.");
                incrl();
                //id_used = 0;
                continue;
        }

        sc_read_2 = conv_lett(sc_read_1);

        if (inv_quant [sc_read_2] == 0)
        {
                strcpy(info [info_lines], "You don't have any such object.");
                incrl();
                //id_used = 0;
                continue;
        }
        //strcpy(inv_name [sc_read_2], "");

/*      if ((inv_class [sc_read_2] > -1 && inv_class [sc_read_2] < 4))
                {
                //strcat(inv_name [sc_read_2], " ");
//              if (inv_plus [sc_read_2] < 0) strcat(inv_name [sc_read_2], "-");

                if (inv_plus [sc_read_2] >= 0 && (inv_plus [sc_read_2] <= 80 | inv_plus [sc_read_2] >= 100)) strcat(inv_name [sc_read_2], "+");
                itoa(inv_plus [sc_read_2] , temp_quant, 10);
                if (inv_plus [sc_read_2] > 80) itoa((inv_plus [sc_read_2] - 100), temp_quant, 10);
                strcat(inv_name [sc_read_2], temp_quant);
                strcat(inv_name [sc_read_2], " ");
                }
*/
        //strcat(inv_name [sc_read_2], all_items [inv_class [sc_read_2]] [inv_type [sc_read_2]]);


//      burden -= 50;




        //strcpy(inv_name [sc_read_2], ident(inv_class [sc_read_2], inv_type [sc_read_2], inv_plus [sc_read_2]));
        set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);
        inv_ident [sc_read_2] = 3;

        strcpy(info [info_lines], " ");
                        strncpy(info [info_lines], letters [sc_read_2], 1);
                        strcat(info [info_lines], " - ");

/*
                                if (inv_quant [sc_read_2] == 1)
                        {
                        if ((inv_class [sc_read_2] != 6 && inv_class [sc_read_2] != 8) && (inv_name [sc_read_2] [0] == 97 ^ inv_name [sc_read_2] [0] == 101 ^ inv_name [sc_read_2] [0] == 105 ^ inv_name [sc_read_2] [0] == 111 ^ inv_name [sc_read_2] [0] == 117))
                        {
                                strcat(info [info_lines], "an ");
                        } else
//                      if ((inv_name [sc_read_2] [0] != 97 && inv_name [sc_read_2] [0] != 101 && inv_name [sc_read_2] [0] != 105 && inv_name [sc_read_2] [0] != 111 && inv_name [sc_read_2] [0] != 117 && inv_name [sc_read_2] [0] > 96) | inv_name [sc_read_2] [0] == 43 | inv_name [sc_read_2] [0] == 45)
                        {
                                strcat(info [info_lines], "a ");
                        }

                        } else
                              {
                              itoa(inv_quant [sc_read_2], temp_quant, 10);
                              strcat(info [info_lines], temp_quant);
                              strcat(info [info_lines], " ");
                              }
                        if (inv_class [sc_read_2] == 6)
                        {
                                strcat(info [info_lines], "scroll");
                                if (inv_quant [sc_read_2] > 1)
                                        strcat(info [info_lines], "s ");
                                        else strcat(info [info_lines], " ");
                        }

                        if (inv_class [sc_read_2] == 8 && inv_quant [sc_read_2] == 1)
                        {
                                strcat(info [info_lines], "bottle of ");
                        }

                        if (inv_class [sc_read_2] == 8 && inv_quant [sc_read_2] > 1)
                        {
                                strcat(info [info_lines], "bottles of ");
                        }

*/

                        item_name(inv_class [sc_read_2], inv_type [sc_read_2], inv_dam [sc_read_2], inv_plus [sc_read_2], inv_quant [sc_read_2], inv_ident [sc_read_2], 3, str_pass);
                        strcat(info [info_lines], str_pass);

//                      if (inv_quant [sc_read_2] != 1 && inv_class [sc_read_2] != 6 && inv_class [sc_read_2] != 8)
//                              strcat(info [info_lines], "s");


                        if (sc_read_2 == item_wielded) strcat(info [info_lines], " (weapon in hand)");
                        if (sc_read_2 == armour [0]) strcat(info [info_lines], " (being worn)");

                        if (sc_read_2 == ring [0]) strcat(info [info_lines], " (on left hand)");
                        if (sc_read_2 == ring [1]) strcat(info [info_lines], " (on right hand)");

                        incrl();


        id_used -= 1;

        } while (id_used > 0);

} // end of identify


void conjure_flame(void)
{

strcpy(info [info_lines], "You cast a flaming cloud spell!");
incrl();

strcpy(info [info_lines], "Where?");
incrl();
// test relay_message();

direc: direction(100);

if (nothing == -1)
{
        strcpy(info [info_lines], "The spell fizzles.");
        incrl();
        return;
}

char ig = grid [target_x] [target_y];

if (see_grid(target_x, target_y) == 0)
{
   strcpy(info [info_lines], "You can't see that place!");
   incrl();
   // test relay_message();
   goto direc;
}


if (ig <= 10 | ig >= 50)
{
   strcpy(info [info_lines], "There's already something there!");
   incrl();
   // test relay_message();
   goto direc;
}

// Hmmm... This should really take account of invis monsters

beam_source_x = x_pos; beam_source_y = y_pos;

//      zapping(12);
//      int thing_to_do =

//cout << "col: " << beam_colour << "hit " << beam_hit << "dam " << beam_damage << "ty " << beam_type << "nam:" << beam_name;
//      zapping(12, beam_colour, beam_name, beam_range, beam_damage, beam_flavour, beam_type, thing_thrown, beam_hit);
        //missile();

        beam_range = 15;
        beam_type = 1;

//      place_cloud();
 place_cloud(1, target_x, target_y, beam_range);

move_x = 0;
move_y = 0;
}



void stinking_cloud(void)
{

//strcpy(info [info_lines], "You cast a  cloud spell!");
//incrl();

strcpy(info [info_lines], "Which direction? (* to target)");
incrl();
// test relay_message();

direction(1);

if (nothing == -1)
{
        strcpy(info [info_lines], "The spell fizzles.");
        incrl();
        return;
}


beam_source_x = x_pos; beam_source_y = y_pos;

strcpy(beam_name, "ball of vapour");
beam_colour = GREEN;
beam_range = 7;
beam_damage = 0;
beam_hit = 20;
beam_type = 42;
beam_flavour = 8; // exploding gas on target // 7; // gas?
thing_thrown = 4;//? ???? ?

missile();


//      zapping(12);
//      int thing_to_do =

//cout << "col: " << beam_colour << "hit " << beam_hit << "dam " << beam_damage << "ty " << beam_type << "nam:" << beam_name;
//      zapping(12, beam_colour, beam_name, beam_range, beam_damage, beam_flavour, beam_type, thing_thrown, beam_hit);
        //missile();

beam_damage = magic_ability(mag_abil, intel);

stinkcl(beam_x, beam_y);
move_x = 0;
move_y = 0;
}


void stinkcl(char cl_x, char cl_y)
{

   char stx = 0;
   char sty = 0;

        for (stx = -1; stx < 2; stx++)
   {

      for (sty = -1; sty < 2; sty++)
      {

                beam_range = random2(5) + 1 + random2(beam_damage / 50);
          if (beam_range > 10) beam_range = 10 + random2(3);
            beam_type = 2;

            target_x = cl_x + stx;
            target_y = cl_y + sty;

            if (grid [target_x] [target_y] > 10 && grid [target_x] [target_y] < 100)
            {
//               place_cloud();
                 place_cloud(2, target_x, target_y, beam_range);
            }

      } // end of sty

   } // end of stx


}



void cast_big_c(int pow, char cty) // <- is this relevant?
{

//strcpy(info [info_lines], "You cast a freezing cloud spell!");
//incrl();

strcpy(info [info_lines], "Where do you want to put it?");
incrl();
// test relay_message();

direction(100);

if (nothing == -1)
{
        strcpy(info [info_lines], "The spell fizzles.");
        incrl();
        return;
}


//beam_source_x = x_pos; beam_source_y = y_pos;

beam_damage = pow;

big_cloud(cty, target_x, target_y, pow);

move_x = 0; move_y = 0;

}


void big_cloud(char clouds, char cl_x, char cl_y, int pow)
{
   char stx = 0;
   char sty = 0;

if (clouds % 100 == 3) beam_colour = WHITE;
if (clouds % 100 == 4) beam_colour = LIGHTGREEN;
//beam_colour = WHITE;
//beam_range = 3 + random2(3);
//beam_damage = 0;
//beam_hit = 20;
//beam_type = 2;
//beam_flavour = 7; // exploding gas on target // 7; // gas?
//thing_thrown = 4;//? ???? ?

 for (stx = -1; stx < 2; stx++)
   {

      for (sty = -1; sty < 2; sty++)
      {

                beam_range = 9 + random2(3) + pow / 50;
                       // was 1
          if (beam_range > 20) beam_range = 20 + random2(4);
            beam_type = clouds;

            target_x = cl_x + stx;
            target_y = cl_y + sty;

            if (grid [target_x] [target_y] > 10 && grid [target_x] [target_y] < 100 && (target_x != x_pos | target_y != y_pos)) //100)
            {
//               place_cloud();
                 place_cloud(clouds, target_x, target_y, beam_range);
            }

      } // end of sty

   } // end of stx

} // end of freezing_cloud(c,c)



void cast_lesser_healing(int mabil)
{

if (hp == hp_max)
{
strcpy(info [info_lines], "Nothing appears to happen.");
incrl();
return;
}

strcpy(info [info_lines], "You feel healed.");
incrl();

hp += 5 + random2(4) + random2(4) + random2(mabil / 20);
hp_ch = 1;
//deaths_door = 0;

if (hp > hp_max) hp = hp_max;

} // end of lesser healing

void cast_greater_healing(int mabil)
{

if (hp == hp_max)
{
 strcpy(info [info_lines], "Nothing appears to happen.");
 incrl();
 return;
}

strcpy(info [info_lines], "You feel greatly healed!");
incrl();

hp += 15 + random2(15) + random2(15) + random2(mabil / 10);
hp_ch = 1;

if (hp > hp_max) hp = hp_max;

} // end of void cast_greater_healing

void cast_cure_poison(int mabil)
{

if (poison == 0)
{
 strcpy(info [info_lines], "Nothing appears to happen.");
 incrl();
 return;
}

poison -= 2 + random2(mabil / 30) + random2(3);

if (poison <= 0)
{
 strcpy(info [info_lines], "You feel the poison leave your system.");
 incrl();
 poison = 0;
 return;
}

strcpy(info [info_lines], "You feel most of the poison leave your system.");
incrl();

} // end of cast_cure_poison

void purification(int mabil)
{

strcpy(info [info_lines], "You feel purified!");
incrl();

poison = 0;
rotting = 0;
conf = 0;
slow = 0;
paralysis = 0; // ?

// this doesn't seem very useful, does it?

}

void cast_deaths_door(int pow)
{

if (deaths_door > 0)
{
 strcpy(info [info_lines], "Your appeal for an extension has been denied.");
 incrl();
 return;
}

strcpy(info [info_lines], "You feel invincible!");
incrl();
strcpy(info [info_lines], "You seem to hear sand running through an hourglass...");
incrl();

deaths_door = 12 + random2(5) + random2(5) + random2(5) + random2(pow) / 10;
hp = 0;
hp_ch = 1;

}

void abjuration(int pow)
{
// can't use beam variables here, because of monster_die and the puffs of smoke

int ab = 0;

for (ab = 0; ab < MNST; ab ++)
{
 if (mons_class [ab] == -1) continue;
 if (mons_near(ab) == 0) continue;
 if (mons_ench_1 [ab] == 0 | mons_ench [ab] [0] < 20 | mons_ench [ab] [0] > 25) continue;

 mons_ench [ab] [0] -= 2 + random2(3);
 if (mons_ench [ab] [0] <= 19)
 {
  killer = 6;
  monster_die(ab);
  continue;
 }
 strcpy(info [info_lines], monam (mons_sec [ab], mons_class [ab], mons_ench [ab] [2], 0));
 strcat(info [info_lines], " shudders.");
 incrl();

} // end of for ab

} // end of void abjuration

void summon_small_mammals(int pow)
{
if (random2(2) == 0) create_monster(17, 22, 7, x_pos, y_pos, MHITNOT);
   else create_monster(1, 22, 7, x_pos, y_pos, MHITNOT);
} // end of summon_small_mammals

void summon_scorpions(int pow)
{
int numsc = 1 + random2(pow) / 20;
int scount = 0;

if (numsc > 8) numsc = 8;

for (scount = 0; scount < numsc; scount ++)
{
  create_monster(18, 22, 7, x_pos, y_pos, MHITNOT);
}

} // end of summon_scopions

void summon_swarm(int pow)
{
int numsc = 1 + random2(pow) / 7;
int scount = 0;
int thing_called = 0;

if (numsc > 8) numsc = 8;

for (scount = 0; scount < numsc; scount ++)
{
 switch(random2(14))
 {
  case 10:
  case 12:
  case 0: thing_called = 0; break; // giant ant
  case 13:
  case 1: thing_called = 10; break; // killer bee
  case 2: thing_called = 18; break; //   " larva
  case 3: thing_called = 22; break; // scorpion
  case 11:
  case 4: thing_called = 24; break; // wasp
  case 5: thing_called = 27; break; // B
  case 6: thing_called = 42; break; // Q
  case 8: thing_called = 62; break; // wolf spider
  case 9: thing_called = 66; break; // butterfly
 } // end switch
 create_monster(thing_called, 22, 7, x_pos, y_pos, MHITNOT);
}
 strcpy(info [info_lines], "You call forth a swarm of pestilential beasts!");
 incrl();
} // end of summon_swarm

void summon_things(int pow)
{
int numsc = 1 + random2(pow) / 20;
int big_things = 0;
//int little_things = 0;
int scount = 0;

while(numsc > 2)
{
        if (random2(3) == 0) break;
 numsc -= 2;
        big_things ++;
}

if (numsc > 8) numsc = 8;
if (big_things > 8) big_things = 8;

//for (scount = 0; scount < numsc; scount ++)
while (numsc > 0)
{
  create_monster(23, 22, 7, x_pos, y_pos, MHITNOT);
  numsc--;
}

while (big_things > 0)
{
  create_monster(49, 22, 7, x_pos, y_pos, MHITNOT);
  big_things--;
}

strcpy(info [info_lines], "Something answered your call!");
incrl();

intel--;
intel_ch = 1;
strcpy(info [info_lines], "Your brain shrivels slightly.");
incrl();

} // end of summon_things

void summon_butter(int pow)
{
int scount = 0;
for (scount = 0; scount < 8; scount ++)
{
  create_monster(66, 21, 7, x_pos, y_pos, MHITNOT);
}
// maybe in some rare circumstances produce caterpillars?
} // end butterflies


void cast_selective_amnesia(void)
{

char spc = 0;
char spc2 = 0;
char ep_gain = 0;

if (spell_no == 0)
{
        strcpy(info [info_lines], "You don't know any spells."); // can this happen?
        incrl();
        return;
}

query : strcpy(info [info_lines], "Forget which spell?");
incrl();

// test relay_message();

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        //inventory(0);
        char unthing = spell_list(spells); //invent(0, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);

//      cout << (int) unthing;

//        unthing = getch();

        if (unthing == 2) return; //27) return;

        if ((unthing >= 65 && unthing <= 90) | (unthing >= 97 && unthing <= 122))
        {
                keyin = unthing;
        } else
        {
                goto query;
        }
}

spc = (int) keyin;

if (spc < 65 | (spc > 90 && spc < 97) | spc > 122)
{
        unknown : strcpy(info [info_lines], "You don't know that spell.");
        incrl();
        return;
}

spc2 = conv_lett(spc);

if (spells [spc2] == -1)
{
        goto unknown;
}

if (random2(25) == 0)
{
 strcpy(info [info_lines], "Oops! This spell sure is a blunt instrument.");
 incrl();
 forget_map();
 return;
}

ep_gain = spell_value(spells [spc2]);
//if (random2(3) == 0) ep_gain -= random2(spell_value(spells [spc2])) + 1;
if (ep_gain < 0) ep_gain = 0;

if (ep_gain == spell_value(spells [spc2]))
{
 strcpy(info [info_lines], "The spell releases its latent energy back to you as it unravels.");
}/* else
       {
       if (ep_gain == 0) strcpy(info [info_lines], "The spell is erased from your mind.");
          else strcpy(info [info_lines], "The spell releases some of its latent energy back to you as it unravels.");
       }*/

incrl();

spell_levels += ep_gain;
ep_ch = 1;
spell_no --;

spells [spc2] = -1;

} // end of cast_selective_amn


void remove_curse(void)
{

if (item_wielded != -1)
        {
        if (inv_plus [item_wielded] > 130) inv_plus [item_wielded] -= 100;
        }

        if (ring [0] != -1)
        {
        if (inv_plus [ring [0]] > 80) inv_plus [ring [0]] -= 100;
        }

        if (ring [1] != -1)
        {
        if (inv_plus [ring [1]] > 130) inv_plus [ring [1]] -= 100;
        }

        for (i = 0; i < 6; i ++)
        {
                if (inv_plus [armour [i]] > 130) inv_plus [armour [i]] -= 100;
        }


/*      strcpy(info [info_lines], "You cast a remove curse spell!");
        incrl();*/
        strcpy(info [info_lines], "You feel as if something is helping you.");
        incrl();
        //strcpy(inv_name [count_y], "of remve curse");
        //inv_ident [count_y] = 1;
//      set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);


} // end of remove_curse()


void detect_curse(void)
{

int curse_found = 0;

for (i = 0; i < 52; i ++)
{

   if (inv_class [i] == -1 | inv_quant [i] == 0) continue;

   if (inv_class [i] == 0 | inv_class [i] == 2 | inv_class [i] == 7)
   {
      if (inv_ident [i] == 0)
      {
        curse_found ++;
                                                                inv_ident [i] = 1;
      }
   }
} // end of for i

//if (curse_found > 0)
//{
// strcpy(info [info_lines], "You sense a malignant aura.");
//} else
strcpy(info [info_lines], "You sense the presence of curses on your possessions.");

 incrl();

}

void cast_smiting(int pow)
{

strcpy(info [info_lines], "Smite whom?");
incrl();
// test relay_message();

direction(100);

if (nothing == -1 | grid [target_x] [target_y] < 50 | (grid [target_x] [target_y] >= 100 && grid [target_x] [target_y] < 150))
{
        strcpy(info [info_lines], "The spell fizzles.");
        incrl();
        return;
}

for (i = 0; i < MNST; i ++)
{
  if (mons_x [i] == target_x && mons_y [i] == target_y) break;
}

strcpy(info [info_lines], "You smite ");
strcat(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 1));
strcat(info [info_lines], "!");
incrl();

mons_hp [i] -= random2(8) + random2(pow) / 10;
mons_hp [i] -= random2(8) + random2(pow) / 10;
mons_hp [i] -= random2(8) + random2(pow) / 10;

killer = 1;
if (mons_hp [i] <= 0) monster_die(i);

move_x = 0;
move_y = 0;

} // end of smiting

/*
void magic_missile(char source)
{

strcpy(info [info_lines], "Which direction?");
incrl();
// test relay_message();

direction(1);

//set_id(inv_class [sc_read_2], inv_type [sc_read_2], 1);

if (nothing == -1)
{
        strcpy(info [info_lines], "The spell fizzles.");
        incrl();
        return;
}

beam_source_x = x_pos; beam_source_y = y_pos;

char xmove_x = move_x;
char xmove_y = move_y;

//for (i = 0; i < (mag_abil / 40) + 1; i ++)
//{

if (source == 1) zapping(4, magic_ability(mag_abil, intel)); // spell
   else zapping(4, 40); // wand

        //delay(500);

//int thing_to_do =
//      zapping(4, beam_colour, beam_name, beam_range, beam_damage, beam_flavour, beam_type, thing_thrown, beam_hit);
//cout << "col: " << beam_colour << "hit " << beam_hit << "dam " << beam_damage << "ty " << beam_type << " " << beam_name;
        move_x = xmove_x;
        move_y = xmove_y;
        //missile();
//}
move_x = 0; move_y = 0;

} // end of magic_missile()
*/














void you_teleport(void)
{

        strcpy(info [info_lines], "Your surroundings suddenly seem different.");
        incrl();
        // don't show this when blinded

        do
        {
                x_pos = random2(70) + 10;
                y_pos = random2(60) + 10;
        } while (grid [x_pos] [y_pos] != 17);

} // end of you_teleport



void torment(void)
{
 int dmi = 0;

        strcpy(info [info_lines], "Your body is wracked with pain!");
        incrl();
        //hp -= (hp / 2) - 1;
        int tormdam = (hp / 2) - 1;
        ouch(tormdam, 0, 0);
        hp_ch = 1;

        for (dmi = 0; dmi < MNST; dmi++)
        {
                if (mons_class [dmi] == -1) continue;

                if (mons_near(dmi) != 1) continue;

//              int mres = random2(100);
//              int mres2 = mon_resist_mag(mons_class [dmi], mons_HD [dmi]);

//              if (mres < mres2) continue;

  if (mons_holiness(mons_class [dmi]) > 0) continue;

                // maybe an exemption for undead?
                // maybe do something fun for magic circles?

                mons_hp [dmi] -= (mons_hp [dmi] / 2) - 1;

                if (mons_ench [dmi] [2] == 6) continue; // can't see it

                strcpy(info [info_lines], monam (mons_sec [dmi], mons_class [dmi], mons_ench [dmi] [2], 0));
                strcat(info [info_lines], " convulses!");
                incrl();
        }

}


void random_uselessness(unsigned char ru)
{

switch(ru)
{
        case 0:
        strcpy(info [info_lines], "The dust glows a ");
        strcat(info [info_lines], weird_colours(random2(256)));
        strcat(info [info_lines], " colour!");
        incrl();
        break;

        case 1:
        strcpy(info [info_lines], "The scroll reassembles itself in your hand!");
        inv_quant [sc_read_2]++;
        incrl();
 burden_change();
        break;

        case 2:
        if (item_wielded != -1)
        {
        in_name(item_wielded, 4);
        strcpy(info [info_lines], str_pass);
        strcat(info [info_lines], " glows ");
        strcat(info [info_lines], weird_colours(random2(256)));
        strcat(info [info_lines], " for a moment.");
        incrl();
        return;
        //strcat(info
        } else
             {
                strcpy(info [info_lines], "Nothing appears to happen.");
                incrl();
                return;
             }
//      break;

        case 3:
        strcpy(info [info_lines], "You hear the distant roaring of an enraged ");
                switch(random2(8))
                {
                        case 0: strcat(info [info_lines], "frog!"); break;
                        case 1: strcat(info [info_lines], "pill bug!"); break;
                        case 2: strcat(info [info_lines], "millipede!"); break;
                        case 3: strcat(info [info_lines], "eggplant!"); break;
                        case 4: strcat(info [info_lines], "albino dragon!"); break;
                        case 5: strcat(info [info_lines], "dragon!"); break;
                        case 6: strcat(info [info_lines], "human!"); break;
                        case 7: strcat(info [info_lines], "slug!"); break;
                }
        incrl();
        return;

        case 4:
        strcpy(info [info_lines], "You smell ");
                switch(random2(8))
                {
                        case 0: strcat(info [info_lines], "coffee."); break;
                        case 1: strcat(info [info_lines], "salt."); break;
                        case 2: strcat(info [info_lines], "burning hair!"); break;
                        case 3: strcat(info [info_lines], "baking bread."); break;
                        case 4: strcat(info [info_lines], "something weird."); break;
                        case 5: strcat(info [info_lines], "wet wool."); break;
                        case 6: strcat(info [info_lines], "sulphur."); break;
                        case 7: strcat(info [info_lines], "fire and brimstone!"); break;
                }
        incrl();
        return;



        case 5:
        strcpy(info [info_lines], "You experience a momentary feeling of inescapable doom!");
        incrl();
        return;

        case 6:
        strcpy(info [info_lines], "Your ");
 switch(random2(3))
 {
                        case 0: strcat(info [info_lines], "ears itch."); break;
                        case 1: strcat(info [info_lines], "brain hurts!"); break;
                        case 2: strcat(info [info_lines], "nose twitches suddenly!"); break;
 }
 incrl();
        return;

 case 7:
 strcpy(info [info_lines], "You hear the tinkle of a tiny bell.");
 incrl();
 summon_butter(357);
 return;

        case 8:
        strcpy(info [info_lines], "You hear ");
                switch(random2(9))
                {
                        case 0: strcat(info [info_lines], "snatches of song."); break;
                        case 1: strcat(info [info_lines], "a voice call someone else's name."); break;
                        case 2: strcat(info [info_lines], "a very strange noise."); break;
                        case 3: strcat(info [info_lines], "roaring flame."); break;
                        case 4: strcat(info [info_lines], "a very strange noise indeed."); break;
                        case 5: strcat(info [info_lines], "the chiming of a distant gong."); break;
                        case 6: strcat(info [info_lines], "the bellowing of a yak."); break;
                        case 7: strcat(info [info_lines], "a crunching sound."); break;
                        case 8: strcat(info [info_lines], "the tinkle of an enormous bell."); break;
                }
        incrl();
        return;


}

//abort();

}




void open_door(void)
{

strcpy(info [info_lines], "Which direction?");
incrl();
// test relay_message();
//keyin_2 = getch();
/*
switch(keyin_2)
{
        case '1': move_x = -1; move_y = 1; break;
        case '2': move_y = 1; move_x = 0;break;
        case '9': move_x = 1; move_y = -1; break;
        case '8': move_y = -1; move_x = 0;break;
        case '7': move_y = -1; move_x = -1; break;
        case '4': move_x = -1; move_y = 0;break;
        case '3': move_y = 1; move_x = 1; break;
        case '6': move_x = 1; move_y = 0;break;

        // put in case '.', '>' '<' etc

        default: strcpy(info [info_lines], "What an unusual direction.");
                incrl(); move_x = 0; move_y = 0; return;
}
*/

direction(0);

if (nothing == -1) return;

if (move_x > 1 | move_y > 1)
{
        strcpy(info [info_lines], "I'm afraid your arm isn't that long.");
        incrl();
        return;
}

if (grid [x_pos + move_x] [y_pos + move_y] == 3)
{
        if (lev != 0)
        {
                strcpy(info [info_lines], "You reach down and open the door.");
                incrl();
                grid [x_pos + move_x] [y_pos + move_y] = 20;
                turnover = 1;
        } else
              {
                if (random2(6) == 0)
                {
                        strcpy(info [info_lines], "As you open the door, it creaks loudly!");
                        x_plos = x_pos;
                        y_plos = y_pos;
                } else strcpy(info [info_lines], "You open the door.");

                incrl();
                grid [x_pos + move_x] [y_pos + move_y] = 20;
                turnover = 1;
              }
} else
        {
        strcpy(info [info_lines], "There isn't anything that you can open there.");
        incrl();
        //relay_message();
        //return;
        }

move_x = 0; move_y = 0;
} // end of void open_door()



void close_door(void)
{

strcpy(info [info_lines], "Which direction?");
incrl();
// test relay_message();
//keyin_2 = getch();

/*
switch(keyin_2)
{
        case '1': move_x = -1; move_y = 1; break;
        case '2': move_y = 1; move_x = 0;break;
        case '9': move_x = 1; move_y = -1; break;
        case '8': move_y = -1; move_x = 0;break;
        case '7': move_y = -1; move_x = -1; break;
        case '4': move_x = -1; move_y = 0;break;
        case '3': move_y = 1; move_x = 1; break;
        case '6': move_x = 1; move_y = 0;break;

        // put in case '.', '>' '<' etc

        default: strcpy(info [info_lines], "What an unusual direction.");
                incrl(); move_x = 0; move_y = 0; return;
}
*/

direction(0);

if (nothing == -1) return;

if (move_x > 1 | move_y > 1)
{
        strcpy(info [info_lines], "I'm afraid your arm isn't that long.");
        incrl();
        return;
}

if (move_x == 0 && move_y == 0)
{
        strcpy(info [info_lines], "You can't close the door on yourself!");
        incrl();
        return;
}

if (grid [x_pos + move_x] [y_pos + move_y] == 70)
{
        // Need to make sure that turnover = 1 if creature is invisible

        strcpy(info [info_lines], "There's a creature in the doorway!");
        incrl();
        move_x = 0; move_y = 0;
        return;
}



if (grid [x_pos + move_x] [y_pos + move_y] == 20)
{
        //for (i = 0; i < ITEMS; i++)
        //{

                //if (itc > it_no) break;


                //if (item_quant [i] != 0)
                //{
                //itc++;

                //if (item_x [i] == x_pos + move_x && item_y [i] == y_pos + move_y)

                if (igrid [x_pos + move_x] [y_pos + move_y] != 501)
                {
                strcpy(info [info_lines], "There's something blocking the doorway.");
                incrl();
                move_x = 0; move_y = 0;
                itc = 0;
                return;
                }
                //}
        //}
        //itc = 0;

        if (lev != 0)
        {
        strcpy(info [info_lines], "You reach down and close the door.");
        incrl();
        grid [x_pos + move_x] [y_pos + move_y] = 3;
        turnover = 1;
        } else
              {
                if (random2(6) == 0)
                {
                        strcpy(info [info_lines], "As you close the door, it creaks loudly!");
                        x_plos = x_pos;
                        y_plos = y_pos;
                } else strcpy(info [info_lines], "You close the door.");

                incrl();
                grid [x_pos + move_x] [y_pos + move_y] = 3;
                turnover = 1;
              }
} else
        {
        strcpy(info [info_lines], "There isn't anything that you can close there!");
        incrl();
        //relay_message();
        //return;
}

move_x = 0; move_y = 0;
} // end of void open_door()







void up_stairs(void)
{

char stair_find = grid [x_pos] [y_pos];


if (stair_find != 18 && stair_find != 21 && stair_find != 23)
{
        strcpy(info [info_lines], "You can't go up here.");
        incrl();
        return;
}

if (char_direction == 0)
{
        strcpy(info [info_lines], "This staircase only leads down!");
 incrl();
        return;
}

if (burden_state == 5)
{
        strcpy(info [info_lines], "You are carrying too much to climb the stairs.");
 incrl();
        turnover = 1;
        return;
}

// if load is too heavy... else...

/*

char mni;

//mni = random2(40) + 50; // Change this!!!

mni = 95;

mni -= no_mons;


if (mni > 0)
{
        for (i = 0; i < mni; i++)
        {
                mons_place(250, 0, 50, 50, 0, MHITNOT);
        }
} // end of if mi


//if (mni > 0)


// Putting this bit in made it crash once (abnormal prog term). Maybe it will again.
{
        for (i = 0; i < MNST; i++)
        {
        if (mons_class [i] != -1 && gmon_use [mons_class [i]] > 0)
                {
                        //monsters_get(mons_class, mons_inv, item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y, it_no, i, mons_x, mons_y, mons_AC, mons_ev, property);
                        monsters_find(mons_class, mons_inv, item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y, it_no, i, mons_x, mons_y, property, mons_AC, mons_ev);
                        //it_no += restock_monsters(mons_class, mons_inv, item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y, it_no, i, mons_x, mons_y, item_link);
                }
        }
} // end of if mi


for (i = 0; i < 10; i ++)
{
        if (stair_x [i] == x_pos && stair_y [i] == y_pos)
 {
  stair_taken = i;
  break;
 }
}

*/

//                              save_level();


//unidentify(fake_name, item_class, item_type, item_dam);

/*
char keyg;
window(1,1,80,25);
gotoxy(1,1);

for (i = 0; i < ITEMS; i ++)
{

cprintf(fake_name [i]);
cprintf("\n\r");

if (i % 25 == 0)
{
        keyg = getch();
}

} // end of for i

keyg = getch();
*/


/*switch(your_level)
{
        case 0: save_one(); break;
        case 1: save_two();  break;
        case 2: save_three(); break;
        case 3: save_four(); break;
        case 4: save_five(); break;
        case 5: save_six(); break;
        case 6: save_seven(); break;
        case 7: save_eight(); break;
        case 8: save_nine(); break;
        case 9: save_ten(); break;
}*/






your_level--;


if (your_level == -1)
{
strcpy(info [info_lines], "You have escaped!");
incrl();
for (i = 0; i < 52; i++)
{
 if (inv_quant [i] > 0 && inv_class [i] == 12) ouch(-9999, 0, 12);
}

ouch(-9999, 0, 11);

}

if (where_are_you == 2)
{
 strcpy(info [info_lines], "You hear a howl of diabolical anger from deep beneath your feet!");
 incrl();
 more();
 where_are_you = 0;
 your_level = 28;
}

load();
//save_one();
/*
do
{

for (i = 0; i < 80; i ++)
{
        for (j = 0; j < 70; j ++)
        {
                if (grid [i] [j] == stair_find + 1 && grid [i] [j] != 1)
                {
                        x_pos = i;
                        y_pos = j;
                        break;
                }
        }
if (grid [i] [j] == stair_find + 1 && grid [i] [j] != 1) break;
}

} while (grid [i] [j] != stair_find + 1);
*/
/*
x_pos = stair_x [stair_taken];
y_pos = stair_y [stair_taken];
*/
        do
        {
                x_pos = random2(70) + 10;
                y_pos = random2(60) + 10;
        } while (grid [x_pos] [y_pos] != 17);

/*
for (i = 0; i < MNST; i ++)
{
//      for (j = 0; j < ITEMS; j ++)
//      {
                if (mons_class [i] != -1 && gmon_use [mons_class [i]] > 0)
                {
                        for (j = 0; j < 8; j ++)
                        {
                        if (mons_inv [i] [j] != 501)
                        {
                                item_x [mons_inv [i] [j]] = mons_x [i];
                                item_y [mons_inv [i] [j]] = mons_y [i];
                        }
                        }
                }
//      }

}
*/






new_level();


if (lev != 0)
{

        strcpy(info [info_lines], "You float up the stairs... And bob straight up to the ceiling!");
        incrl();
 strcpy(info [info_lines], "The staircase disappears!");
 incrl();
        return;
}

strcpy(info [info_lines], "You climb up the stairs.");
incrl();
strcpy(info [info_lines], "The staircase disappears!");
incrl();


// if there is a monster on the staircase, instead of relocating it relocate character to nearest
// empty square - "You jump from the stairs to the nearest patch of floor"?? or different for levitating.

} // end of void up_stairs(void)






void down_stairs(void)
{
char stair_find = grid [x_pos] [y_pos];

if (grid [x_pos] [y_pos] != 18 && grid [x_pos] [y_pos] != 19 && grid [x_pos] [y_pos] != 21)
{
        strcpy(info [info_lines], "You can't go down here!");
 incrl();
 return;
}

if (char_direction == 1)
{
        strcpy(info [info_lines], "These stairs only lead upwards.");
 incrl();
 return;
}

/*if (stair_find != 19 && stair_find != 22 && stair_find != 24)
{
        strcpy(info [info_lines], "You can't go down here.");
        incrl();
        return;
}*/

if (lev != 0)
{
        strcpy(info [info_lines], "You can't reach the stairs from up here.");
        incrl();
        return;
}
// if load is too heavy... else...

if (where_are_you == 1) where_are_you = 2;

level_type = 0;

if (grid [x_pos] [y_pos] == 19)
{
 where_are_you = 1; // go to hell!
 strcpy(info [info_lines], "Welcome to Hell!");
 incrl();
 strcpy(info [info_lines], "Please enjoy your stay.");
 incrl();
 more();
 your_level = 59;
}


if (your_level > 12 && your_level <= 25 && random2(20) == 0 && level_type == 0) //grid [x_pos] [y_pos] == 21)
{
 level_type = 1;
// strcpy(info [info_lines], "You enter a dark and forbidding labyrinth!");
// incrl();
// more();
// your_level = 59;
}

/*
char mni;

mni = random2(40) + 50; // Change this!!!

mni = 90;

mni -= no_mons;

if (mni > 0)
{
        for (i = 0; i < mni; i++)
        {
                mons_place(250, 0, 50, 50, 0, MHITNOT);
        }
} // end of if mi



if (mni > 0)
{
        for (i = 0; i < mni; i++)
        {
                //it_no += restock_monsters(mons_class, mons_inv, item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y, it_no, i, mons_x, mons_y, item_link);
                //monsters_get(mons_class, mons_inv, item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y, it_no, i, mons_x, mons_y, mons_AC, mons_ev, property);

        }
} // end of if mi

for (i = 0; i < 10; i ++)
{
        if (stair_x [i] == x_pos && stair_y [i] == y_pos)
 {
  stair_taken = i;
  break;
 }
}


save_level();
*/

//unidentify(fake_name, item_class, item_type, item_dam);


/*
char keyg;
window(1,1,80,25);
gotoxy(1,1);

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

if (level_type != 1) your_level++;

//if (your_level == 10) your_level = 0; // wrap around for now.

load();
//save_one();
/*
do
{

for (i = 1; i < 80; i ++)
{
        for (j = 1; j < 70; j ++)
        {
                if (grid [i] [j] == stair_find - 1 && grid [i] [j] != 1)
                {
                        x_pos = i;
                        y_pos = j;
                        break;
                }
        }
if (grid [i] [j] == stair_find - 1 && grid [i] [j] != 1) break;
}

} while (grid [x_pos] [y_pos] != stair_find - 1);
*/

//x_pos = stair_x [stair_taken];
//y_pos = stair_y [stair_taken];

int grid_wanted = 18;

if (your_level == 26 | your_level == 65) grid_wanted = 17;

        do
        {
                x_pos = random2(70) + 10;
                y_pos = random2(60) + 10;
        } while (grid [x_pos] [y_pos] != grid_wanted);


/*
for (i = 0; i < MNST; i ++)
{
//      for (j = 0; j < ITEMS; j ++)
//      {
                if (mons_class [i] != -1 && gmon_use [mons_class [i]] > 0)
                {
                        for (j = 0; j < 8; j ++)
                        {
                        if (mons_inv [i] [j] != 501)
                        {
                                item_x [mons_inv [i] [j]] = mons_x [i];
                                item_y [mons_inv [i] [j]] = mons_y [i];
                        }
                        }
                }
//      }

}
*/

if (level_type == 1)
{
 strcpy(info [info_lines], "You enter a dark and forbidding labyrinth.");
} else
strcpy(info [info_lines], "You climb down the stairs.");
incrl();

strcpy(info [info_lines], "The staircase disappears!");
incrl();
grid [x_pos] [y_pos] = 17;

new_level();

turnover = 1;


} // end of void up_stairs(void)











void monster(void)
{
char bat = 0;
int monc = 0;

int msecc = 0;

//cprintf("og");




//itoa((int) time_taken, st_prn, 10);
//cprintf(st_prn);

//getch();
//delay(500);

brek = 0;
//viewwindow(); // only once; before the monster function
losight(show, grid, x_pos, y_pos);

brek = 0;
//mnc = 0;



//viewwindow(); // only once; before the monster function


//itoa(no_mons, st_prn, 10);
//mons_speed [i], st_prn, 10);
//strcpy(info [info_lines], st_prn);
//strcat(info [info_lines], ",");
//itoa(mons_speed_inc [i], st_prn, 10);
//strcat(info [info_lines], st_prn);
//incrl();

no_mons ++;

for (i = 0; i < MNST; i++)
{
/*
if (mons_near(i) == 9)
{
itoa(mnc, st_prn, 10);
//mons_speed [i], st_prn, 10);
strcpy(info [info_lines], st_prn);
//strcat(info [info_lines], ",");
//itoa(mons_speed_inc [i], st_prn, 10);
//strcat(info [info_lines], st_prn);
incrl();
}
*/

//if (mons_class [i] == -1) continue;


//if (monc > no_mons) break;





/*
if (mons_near(i) == 1)
{
itoa(mnc, st_prn, 10);
//mons_speed [i], st_prn, 10);
strcpy(info [info_lines], st_prn);
//strcat(info [info_lines], ",");
//itoa(mons_speed_inc [i], st_prn, 10);
//strcat(info [info_lines], st_prn);
incrl();
}
*/

/*
if (mons_near(i))
{
        strcpy(info [info_lines], "z");
        incrl();
}
*/

if (mons_class [i] != -1) no_mons ++;

//if (mons_class [i] == -1) continue; // if monster isn't there, don't bother.
if (mons_class [i] != -1 && mons_class [i] != 56)
{

monc ++;
//{
//grid [mons_x [i]] [mons_y [i]] = basic_grid [mons_x [i]] [mons_y [i]];
//grid_col [mons_x [i]] [mons_y [i]] = basic_grid_col [mons_x [i]] [mons_y [i]];

if (mons_class [i] == 32 && mons_hp [i] < 1)
{
        for (j = 0; j < 3; j++)
                {
                        mons_ench [i] [j] = 0;
                }

        mons_ench_1 [i] = 0;
        //itc = 0;
        no_mons --;
        mons_class [i] = -1;
        if (grid [mons_x [i]] [mons_y [i]] >= 50) grid [mons_x [i]] [mons_y [i]] -= 50;
        spore_goes_pop(); // has to be after the above, so that spore isn't killed twice.
        continue;
} // end of if giant spore



//if (mons_hp [i] <= (mons_hp_max [i] / 4 - 1)) mons_beh [i] = 3;
// move this ^^^

/*
// enchantments:

if (mons_ench_1 [i] != 0)
{

for (p = 0; p < 3; p ++)
{
switch (mons_ench [i] [p])
{
        case 1: // slow
        if (random2(60) == 0)
        {
                mons_speed [i] *= 2;
                mons_ench [i] [p] = 0;
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                        mons_ench_1 [i] = 0;
        }
        break;

        case 2: // haste
        if (random2(60) == 0)
        {
                mons_speed [i] /= 2;
                mons_ench [i] [p] = 0;
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                        mons_ench_1 [i] = 0;
        }
        break;


        case 3: // paralysis
        //mons_targ_1_x [i] = mons_x [i];
        //mons_targ_1_y [i] = mons_y [i];
        //mons_beh [i] = 0;
        //mons_speed [i] += 100;
        if (random2(20) == 0)
        {
                mons_speed [i] -= 100;
                //mons_ev [i] += 5;
                mons_ench [i] [p] = 0;
                //mons_speed [i] /= 2;
                //mons_ench [i] [p] = 0;
                //mons_beh [i] =
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                        mons_ench_1 [i] = 0;
        }
        break;


        case 4: // fear
        mons_beh [i] = 3;
        if (random2 (40) == 0)
        {
        mons_beh [i] = 1; // reset to monster's original behaviour
        mons_ench [i] [p] = 0;
        if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                        mons_ench_1 [i] = 0;
        }
        break;

} // end of switch
} // end of for p
if (mons_speed [i] >= 100) continue;
} // end of if
*/


if (haste == 0) mons_speed_inc [i] += (mons_speed [i] * time_taken / 10);
        else mons_speed_inc [i] += (mons_speed [i] * (time_taken / 2) / 10);

if (slow > 0) mons_speed_inc [i] += (mons_speed [i] * time_taken / 10);

//if (mons_speed [i] >= 100) mons_speed_inc [i] -= 100;


// Note: Put regeneration before enchantments so that paralysed creatures still regenerate.

/*
if (grid [mons_x [i]] [mons_y [i]] >= 100)
{
        mons_in_cloud();
// abort();
 if (mons_class [i] == -1)
        {
                mons_speed_inc [i] = 1;
                break;
        }
//      cout << "/" << (int) grid [mons_x [i]] [mons_y [i]] << "/";
//      getch();
}
*/



/*
if (mons_near(i))
{
        strcpy(info [info_lines], "z");
        incrl();
}
*/







while (mons_speed_inc [i] >= 80) // The continues & breaks are WRT this.
{

if (mons_class [i] != -1 && mons_hp [i] <= 0)
{
 killer = 5;
        monster_die(i);
        continue;
}

mons_speed_inc [i] -= 10;

//abort();

if (grid [mons_x [i]] [mons_y [i]] > 100)
{
 if (mons_class [i] >= MLAVA0 && mons_sec [i] == 1) break;
 if (mons_class [i] == -1) break; // problem with vortices
        mons_in_cloud();
        if (mons_class [i] == -1)
        {
                mons_speed_inc [i] = 1;
                break;
        }
//      cout << "/" << (int) grid [mons_x [i]] [mons_y [i]] << "/";
//      getch();
}







/*
if (mons_near(i))
{
        strcpy(info [info_lines], "x");
        incrl();
}
//keyin = getch(); //cprintf("x");
*/




if (invis != 0 && mons_targ_1_x [i] == mons_x [i] && mons_targ_1_y [i] == mons_y [i])
        mons_beh [i] = 2;



//if (mons_beh [i] == 0) continue; // if it hasn't found you yet.

switch (mons_beh [i])
{
case 0:
mons_targ_1_x [i] = mons_x [i];
mons_targ_1_y [i] = mons_y [i];
break;

case 1: // chasing YOU
//losight(show, grid, x_pos, y_pos);
//if ((mons_near(i) == 1 && invis == 0) | mons_class [i] == 19)// && random2 (6) != 0))
if ((mons_near(i) == 1 && invis == 0) | (mons_near(i) == 0 && mons_targ_1_x [i] == mons_x [i] && mons_targ_1_y [i] == mons_y [i]))
{
        mons_targ_1_x [i] = x_pos;
        mons_targ_1_y [i] = y_pos;
}
break;

case 2: if ((mons_targ_1_x [i] == mons_x [i] && mons_targ_1_y [i] == mons_y [i]) | random2(20) == 0)
{
        mons_targ_1_x [i] = random2(80);
        mons_targ_1_y [i] = random2(70);
}
break; // wandering

case 3: // fleeing
if (mons_near(i) == 1 && (invis == 0 | mons_see_invis(mons_class [i]) > 0))
{
        mons_targ_1_x [i] = x_pos;
        mons_targ_1_y [i] = y_pos;
}
if (mons_hp [i] > (mons_hp_max [i] / 2)) mons_beh [i] = 1;
// change the '= 1' to whatever monster used to be doing
break;

// 4 is used by confusion

case 6: // attacking other monster
     if (mons_hit [i] == MHITNOT)
     {
       mons_beh [i] = 2; // ?
       break;
     }
     mons_targ_1_x [i] = mons_x [mons_hit [i]];
     mons_targ_1_y [i] = mons_y [mons_hit [i]];
break;

case 7: // friendly
//To be nice (although not strictly accurate) they
//should follow you even when they can't see you.
if (pet_target != MHITNOT)
{
 if (mons_hit [i] == MHITNOT)
 {
  mons_hit [i] = pet_target;
 }
}

if (mons_hit [i] == i | mons_beh [mons_hit [i]] == 7) mons_hit [i] = MHITNOT;
// I don't know why I have to do that.

if (mons_hit [i] != MHITNOT && ((mons_near(i) == 1 && mons_near(mons_hit [i]) == 1) | distance(mons_x [i], mons_x [mons_hit [i]], mons_y [i], mons_y [mons_hit [i]]) == 1))
{
  mons_targ_1_x [i] = mons_x [mons_hit [i]];
  mons_targ_1_y [i] = mons_y [mons_hit [i]];
}
 else
    if (mons_near(i) == 1)
    {
     if (random2(3) == 0)
      {
        mons_targ_1_x [i] = random2(80);
        mons_targ_1_y [i] = random2(70);
      } else
             {
              mons_targ_1_x [i] = x_pos;
              mons_targ_1_y [i] = y_pos;
             }
    } else
          {
            mons_targ_1_x [i] = x_pos;
            mons_targ_1_y [i] = y_pos;
          }
break;


/*
case 4: //if ((mons_targ_1_x [i] == mons_x [i] && mons_targ_1_y [i] == mons_y [i]) | random2(50) == 0)
mons_targ_1_x [i] = random2(80);
mons_targ_1_y [i] = random2(70);
break; // confused
*/
// A confused monster shouldn't reset its target - just move random2ly.




} // end of switch (mons_beh)




// enchantments:

if (mons_ench_1 [i] != 0)
{

        //mons_ench_f ( );

for (p = 0; p < 3; p ++)
{
switch (mons_ench [i] [p])
{
        case 1: // slow
        if (random2(60) < mons_HD [i])
        {
                if (mons_speed [i] >= 100)
                {
                        mons_speed [i] = ((mons_speed [i] - 100) * 2) + 100;
                } else mons_speed [i] *= 2;
                mons_ench [i] [p] = 0;
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                        mons_ench_1 [i] = 0;
        }
        break;

        case 2: // haste
        if (random2(60) == 0)
        {
                if (mons_speed [i] >= 100)
                {
                        mons_speed [i] = ((mons_speed [i] - 100) / 2) + 100;
                } else mons_speed [i] /= 2;
                mons_ench [i] [p] = 0;
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                        mons_ench_1 [i] = 0;
        }
        break;

/*
        case 3: // paralysis
        //mons_targ_1_x [i] = mons_x [i];
        //mons_targ_1_y [i] = mons_y [i];
        //mons_beh [i] = 0;
        //mons_speed [i] += 100;
        if (random2(20) == 0 < mons_HD [i])
        {
//      cprintf("iggy");
//      getch();
//         mons_speed [i] -= 100;
                //mons_ev [i] += 5;
                mons_ench [i] [p] = 0;
  mons_speed_inc [i] = 0;
                //mons_speed [i] /= 2;
                //mons_ench [i] [p] = 0;
                //mons_beh [i] =
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                        mons_ench_1 [i] = 0;
        } else
       {
        mons_speed_inc [i] = 0; // yuck!
       }
        break;
*/

        case 4: // fear
        mons_beh [i] = 3;
        if (random2 (150) <= mons_HD [i] + 10)
        {
        mons_beh [i] = 1; // reset to monster's original behaviour
        mons_ench [i] [p] = 0;
        if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                        mons_ench_1 [i] = 0;
        }
        break;


        case 5: // confusion
        mons_beh [i] = 4;
        if (random2 (40) < mons_HD [i])
        {
  if (mons_class [i] == 66 | mons_class [i] == 21) continue; // butterfly, fire vortex
                mons_beh [i] = 1;
                mons_ench [i] [p] = 0;
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                mons_ench_1 [i] = 0;
        }
        break;


        case 6: // invisibility
        if (random2 (40) == 0 | (mons_class [i] >= MLAVA0 && mons_sec [i] == 0))
        {
  if (mons_class [i] == 46) continue;
  if (mons_class [i] >= MLAVA0 && mons_sec [i] == 1) continue; //grid [mons_x [i]] [mons_y [i]] == 61) continue;
                mons_ench [i] [p] = 0;
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                mons_ench_1 [i] = 0;
        }
        break;



   case 57: // poison
   case 58: // worse poison
   case 59: // worser poison
   case 60: // worsest poison
   case 7: // your poison
   case 8: // your worse poison
   case 9: // your worser poison
   case 10: // your worsest poison
   if (random2(2) == 0) mons_hp [i] -= random2((mons_ench [i] [p] % 50) - 5) + 1;
   if (mons_hp [i] <= 0)
   {
      if (mons_ench [i] [p] < 50) killer = 1; // ie you
         else killer = 5;
      monster_die(i);
      goto endbat;
   }
   if (random2(4) == 0) //30 - mons_HD [i]) <= 0) // LOOK AT THIS!!!
   {
      mons_ench [i] [p] --; //= 0;
      if (mons_ench [i] [p] % 50 == 6)
      {
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
        {
                           mons_ench_1 [i] = 0;
        }
      }
   }
   break;

// 19 is taken by summoning:
// If these are changed, must also change abjuration
   case 25:
   case 24:
   case 23:
   case 22:
   case 21:
   case 20:
   if (random2(10) == 0)
   {
     mons_ench [i] [p] --;
   }
   if (mons_ench [i] [p] == 19)
   {
     killer = 6;
     monster_die(i);
     goto endbat;
   }
   break;

 case 30: // charm monster
        if (mons_beh [i] != 7 | random2 (150) <= mons_HD [i] + 10)
        {
        mons_beh [i] = 1; // reset to monster's original behaviour
        mons_ench [i] [p] = 0;
        if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                        mons_ench_1 [i] = 0;
        }
 break;

} // end of switch
} // end of for p












































        //mons_ench_f(mons_ench, mons_speed, mons_ench_1, mons_beh, i);


















} // end of if


if (mons_speed [i] >= 100) continue;

if (mons_hp [i] < mons_hp_max [i] && (random2(17) == 0 | mons_class [i] == 8 | mons_class [i] == 45 | mons_class [i] == 76)) mons_hp [i] ++;



//if (gmon_use [mons_class [i]] == 1)
//{
//if ((mons_inv [i] [0] != 255 && mons_inv [i] [1] != 255 && mons_inv [i] [2] != 255 && mons_inv [i] [3] != 255) | random2(10) < 5)
        //continue;


//              if (gmon_use [mons_class [i]] > 3)// && random2(3) == 0) Not used right now -
// no monster currently has gmon_use > 3. To speed up, maybe give
// some advanced monsters ability to use items, but not kobolds etc.

if (igrid [mons_x [i]] [mons_y [i]] != 501 && (gmon_use [mons_class [i]] == 3 | mons_class [i] == 35))
{
        mons_pickup();
}

//              {
//              mons_pickup();

/*
for (o = 0; o < ITEMS; o ++)
{
        if (itc > it_no) break;


        if (item_quant [o] != 0)
        {
        itc++;

        //if (gmon_use [mons_class [i]] == 0) break; //continue;

        if (item_x [o] == mons_x [i] && item_y [o] == mons_y [i] && item_quant [o] > 0)
        {

        if ((mons_inv [i] [0] == 501 | item_quant [mons_inv [i] [0]] == 0) && item_class [o] == 0 && mons_inv [i] [0] != o)
        {
                mons_inv [i] [0] = o; // picking up weapons.
                brek = 1;
                break;
        //strcpy(info [info_lines], "Hello!"); incrl(); more();
        }
        if ((mons_inv [i] [1] == 501 | item_quant [mons_inv [i] [1]] == 0) && item_class [o] == 1 && mons_inv [i] [1] != o)
        {
                mons_inv [i] [1] = o;
                brek = 1;
                break;
        }
        //strcpy(info [info_lines], "Hello2!"); incrl(); more();
        }

        }
}
itc = 0;
*/
//              if (brek == 1)
//              {
//              brek = 0;
//              continue;
//              }
//              } // end of if gmon_use


/*
for (o = 0; o < 200; o ++)
{
        if (item_x [o] == mons_x [i] && item_y [o] == mons_y [i] && item_quant [o] > 0)
        {

        if (mons_inv [i] [0] == 255 && (item_class [o] == 0 | item_class [o] == 1))
        {
                mons_inv [i] [0] = o; // picking up weapons.
        //strcpy(info [info_lines], "Hello!"); incrl(); more();
        }
        //strcpy(info [info_lines], "Hello2!"); incrl(); more();
        }

}

*/

//} // end of if (gmon_use ...)

// if monster can see you...

// use a switch here....


if (mons_beh [i] == 3) // fleeing
{
if (mons_targ_1_x [i] < mons_x [i]) mmov_x = 1; else mmov_x = 0;
if (mons_targ_1_x [i] > mons_x [i]) mmov_x = -1;
if (mons_targ_1_y [i] < mons_y [i]) mmov_y = 1; else mmov_y = 0;
if (mons_targ_1_y [i] > mons_y [i]) mmov_y = -1;
} else
      {
        if (mons_targ_1_x [i] < mons_x [i]) mmov_x = -1; else mmov_x = 0;
        if (mons_targ_1_x [i] > mons_x [i]) mmov_x = 1;
        if (mons_targ_1_y [i] < mons_y [i]) mmov_y = -1; else mmov_y = 0;
        if (mons_targ_1_y [i] > mons_y [i]) mmov_y = 1;
      }


char brkk = 0;

if (mons_beh [i] == 4) // confused
{
        mmov_x = random2(3) - 1;
        mmov_y = random2(3) - 1;
        if ((grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] % 100) >= 50 && (mmov_x != 0 | mmov_y != 0))
        {
                for (p = 0; p < MNST; p ++)
                {
                if (mons_x [p] == mons_x [i] + mmov_x && mons_y [p] == mons_y [i] + mmov_y && mons_class [p] > -1)
                {
                        if (monsters_fight(i, p) == 1)
   {
                        mmov_x = 0; mmov_y = 0;
                        brkk = 1;
                        break;
   }
                }
                }
        }
} // end of if confused
if (brkk == 1) continue;


//if ((mons_class [i] != 1 && mons_class [i] != 46) | bat == 0)
//{

// MONS FIGHT WAS HERE!!!!

// for monsters who use spec abils when next to you
if (mons_near(i) == 1)
{

switch(mons_class [i])
{

case 59:
if (random2(2) == 0 && (mons_ench [i] [2] != 6 | see_invis != 0))
{
  strcpy(info [info_lines], "The giant eyeball stares at you!");
  incrl();
  if (paralysis < 10) paralysis += 2 + random2(3);
}
break;

case 65:
if (random2(2) == 0 && (mons_ench [i] [2] != 6 | see_invis != 0))
{
  strcpy(info [info_lines], "The eye of draining stares at you!");
  incrl();
  ep -= random2(5) + random2(5) + random2(5) + 2;
  if (ep < 0) ep = 1;
  ep_ch = 1;
  mons_hp [i] += 5;
  if (mons_hp [i] >= mons_hp_max [i]) mons_hp [i] = mons_hp_max [i];
}
break;

case MLAVA0:
case MLAVA1:
case MLAVA2:
case MLAVA3:
case MLAVA4:
case MWATER0:
case MWATER1:
case MWATER2:
case MWATER3:
case MWATER4:
case MWATER5:
if (mons_sec [i] == 0) // visible
{
 if (random2(5) == 0)
 {
   mons_sec [i] = 1;
   mons_ench [i] [2] = 6;
   mons_ench_1 [i] = 1;

 }
} else
{
 if (random2(10) == 0 && (mons_x [i] != x_pos | mons_y [i] != y_pos)) mons_sec [i] = 0;
}
break;

} // end of switch

} // end of if mons_near



// ATTACK WAS HERE!!!








if (((mons_beh [i] == 1 | mons_beh [i] == 3) && mons_near(i) == 1) | ((mons_beh [i] == 6 | mons_beh [i] == 7) && mons_hit [i] != MHITNOT));
{

if ((mons_beh [i] == 1 | mons_beh [i] == 3) && mons_near(i) == 1) pet_target = i;

if ((mons_beh [i] == 6 | mons_beh [i] == 7) && (mons_hit [i] == MHITNOT | mons_hit [i] == MHITYOU))
{
 goto end_switch;
// break;
}

if (mons_beh [i] == 6 | mons_beh [i] == 7)
{
        trac_targ = mons_hit [i];
        trac_targ_x = mons_x [mons_hit [i]];
        trac_targ_y = mons_y [mons_hit [i]];
} else
{
 trac_targ = MHITYOU;
        trac_targ_x = x_pos;
        trac_targ_y = y_pos;
}

if (trac_targ == MHITYOU && mons_near(i) == 0) goto end_switch; //continue;

// Hmmm... monsters will only fight with missiles/spells if they can see you!
//    big problem!

// Could solve: make mons_near(i) == 1 only necess if attacking you;
// if not attacking you, rely on tracer. This will be VERY slow.

switch(mons_class [i])
{

case MLAVA2:
if (invis != 0 && mons_see_invis(mons_class [i]) == 0) break;
if (random2(2) == 0 | mons_sec [i] == 1 | mons_ench [i] [2] == 6) break;
// viewwindow was here.
if (show [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] != 0)
{
move_x = 0; move_y = 0;
//move_x = x_pos - mons_x [i];
//move_y = y_pos - mons_y [i];

move_x = trac_targ_x - mons_x [i];
move_y = trac_targ_y - mons_y [i];
if (move_x != 0 | move_y != 0)
{
brek = 1;
viewwindow();
target_x = trac_targ_x;
target_y = trac_targ_y;
aim_down = 1;
        beam_range = random2(10) + 5;
        beam_source_x = mons_x [i]; beam_source_y = mons_y [i];
//              strcpy(beam_name, monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 4)); //gmon_name [mons_class [i]]);
                strcpy(beam_name, "glob of lava");
                beam_damage = 108;
                beam_colour = RED;
                beam_type = '#'; // hash #
                beam_flavour = 20; // lava
  beam_hit = 20;
                strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
                strcat(info [info_lines], " spits lava!");
                incrl();
        thing_thrown = 4;
        missile();
}
}
break;

case MWATER2:
if (invis != 0 && mons_see_invis(mons_class [i]) == 0) break;
if (mons_sec [i] == 1 | mons_ench [i] [2] == 6) break;
if (show [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] != 0)
{
move_x = 0; move_y = 0;
move_x = trac_targ_x - mons_x [i];
move_y = trac_targ_y - mons_y [i];
if (move_x != 0 | move_y != 0)
{
target_x = trac_targ_x;
target_y = trac_targ_y;
aim_down = 1;
brek = 1;
viewwindow();
        beam_range = random2(10) + 5;
        beam_source_x = mons_x [i]; beam_source_y = mons_y [i];
//              strcpy(beam_name, monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 4)); //gmon_name [mons_class [i]]);
                strcpy(beam_name, "bolt of electricity");
                beam_damage = 105;
                beam_colour = LIGHTCYAN;
                beam_type = 35; // hash #
                beam_flavour = 5; // elec
  beam_hit = 150;
                strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
                strcat(info [info_lines], " shoots out a bolt of electricity!");
                incrl();
        thing_thrown = 2;
        beam();
}
}
break;

case 61:
if (invis != 0 && mons_see_invis(mons_class [i]) == 0) break;
if (random2(3) == 0) // o plant
{
  plant_spit();
}
break;

case 31:
if (random2(4) == 0) // fiend!
{
        switch(random2(4))
        {
                case 0:
  if (mons_near(i) == 0 | mons_beh [i] == 7) break;
  if (mons_ench [i] [2] != 6)
  {
        strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0));
        strcat(info [info_lines], " calls on the powers of Hell!");
        incrl();
  }
                mmov_x = 0; mmov_y = 0;
                torment(); break;

                case 1:
                case 2:
                case 3:
  if (trac_targ == MHITYOU && mons_near(i) == 0) break;
  tracer_f();
  if (mons_beh [i] == 7 && tracer == 1) break;
                if (tracer != 0 | (trac_targ != MHITYOU && trac_hit_mons != 0)) // doesn't need to worry about haste
                {
   if (mons_ench [i] [2] != 6)
   {
        strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0));
        strcat(info [info_lines], " makes a gesture!");
        incrl();
   }
                spell_cast = 49;
                        mons_cast();
                }
                break;

        }

        mmov_x = 0; mmov_y = 0;

}
break;


// Kobold demonologist
case 53:
if (random2(15) == 0 && no_mons < 90)
{
        //summon_demon();
        mmov_x = 0;
        continue;
}
break;

case 15:
if (random2(10) == 0) // phantom
{
 if (random2(3) == 0)
 {
   strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0));
   strcat(info [info_lines], " blinks.");
   incr();
   monster_blink(i);
 }
}
break;

// orc & goblin wizards
//if ((mons_class [i] == 54 | mons_class [i] == 55) /*&& mons_inv [i] [4] != 501*/ && random2(4) == 0)
//{
//      spell_cast = random2 (7);
        /*switch(item_type [mons_inv [i] [4]])
        {
                case 5: spell_cast = 0;
                default: spell_cast = 0;
        }*/

        //spell_cast = 100 + random2(8);

// 0,4,5


/*
case 80: strcat(gmon_name, "burning devil"); break;
case 81: strcat(gmon_name, "rotting devil"); break;
case 82: strcat(gmon_name, "yellow devil"); break;
case 83: strcat(gmon_name, "grey devil"); break;
case 84: strcat(gmon_name, "shadow devil"); break;
case 85: strcat(gmon_name, "hairy devil"); break;
86: ice
87: blue
*/

/*
case 80: // burning devil
//if (random2(3) == 0)
        switch(random2(3))
 {
        case 0: mons_sec [i] = RED; break;
        case 1: mons_sec [i] = LIGHTRED; break;
        case 2: mons_sec [i] = YELLOW; break;
 }
if (random2(4) != 0) break;
if (trac_targ == MHITYOU)
{
 if (mons_near(i) == 0) break;
 if (mons_ench [i] [2] != 6)
 {
        strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
        strcat(info [info_lines], " points at you.");
        incrl();
 }
 beam_type = 0;
        direct_effect();
}
break;
*/

//case 68:
//place_cloud(101, mons_x [i], mons_y [i], 2 + random2(4));
//break;

case 37:
case 39:
case 43: // R
case 47: // vampire
case 54:
case 68: // efreet
case 69: // brain worm
case 70: // giant orange brain
case 79: // great orb of eyes
case 80:
case 101: // steam dragon
case 103: // orc sorceror
if (mons_class [i] == 80)
 switch(random2(3))
 {
        case 0: mons_sec [i] = RED; break;
        case 1: mons_sec [i] = LIGHTRED; break;
        case 2: mons_sec [i] = YELLOW; break;
 }

if (invis != 0 && mons_see_invis(mons_class [i]) == 0) break;

if (random2(4) != 0) break;

msecc = mons_sec [i];
if (mons_class [i] == 80) msecc = 30; // burning devil

//aim_down = 1;

/*splist [0] = 8; // bolt spell
splist [1] = 6; // enchantment
splist [2] = 11; // self_ench
splist [3] = 0; // misc*/

// tracer = 0 = run out of range
// tracer = 1 = hits you in range
// tracer_mons = 0 = hits no monsters
//       "     = 1 = hits monster before you (possibly also after)
//       "     = 2 = hits monster after but not before
//       "     = 3 = hits tamed monster(s) but no others

if (trac_targ == MHITYOU && mons_near(i) == 0) break;

tracer_f();

if (mons_spell_list(msecc, 4) == 18 && mons_beh [i] == 1 && trac_targ == MHITYOU && distance(x_pos, mons_x [i], y_pos, mons_y [i]) < 30)
{
        spell_cast = 18;
        goto casted;
}

//   if (tracer != 0 | (trac_targ != MHITYOU && trac_hit_mons != 0))

if (mons_beh [i] == 7 && trac_hit_tamed == 1) break;

if (mons_beh [i] != 3)
{
 //outp(tracer_mons);
 if (tracer_mons == 0 && tracer == 1 | ((trac_targ != MHITYOU && trac_hit_mons != 0)))
 {
        spell_cast = mons_spell_list(msecc, random2(4));
  goto casted;
 }
 if (tracer_mons == 1 && trac_targ == MHITYOU | random2(10) == 0) // | (tracer == 0 && trac_targ == MHITYOU))
 {
  if (random2(2) == 0 | (mons_ench_1 [i] == 1)) break;
  spell_cast = mons_spell_list(msecc, 2);
  goto casted;
 }
 //if (tracer == 1 && tracer_mons == 3) spell_cast = mons_spell_list(msecc, 0);
 if ((tracer == 1 && (tracer_mons != 1)) | (trac_targ != MHITYOU))// && trac_hit_mons == 1)) //tracer_mons == 2 | tracer_mons == 3))
 {
        switch(random2(4))
        {
    case 0: spell_cast = mons_spell_list(msecc, 0);
    if (tracer_mons == 2) aim_down = 1;
    break;
                        case 1: spell_cast = mons_spell_list(msecc, 1); break;
    case 2: spell_cast = mons_spell_list(msecc, 3); break;
    case 3: spell_cast = mons_spell_list(msecc, 4); break;
        }
 }
} // end if
 else
        {
  if (random2(5) != 0) break;
        spell_cast = mons_spell_list(msecc, 5);
 }

casted : if (spell_cast == 100) break; // nothing cast

if (mons_near(i) == 1)
{
        strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
if (mons_class [i] != 69) // brain worm
if (mons_class [i] == 103 && spell_cast == 27 && your_level < 50)
{
  if (mons_ench [i] [2] != 6)
                        {
                strcat(info [info_lines], " opens a gate to Hell!");
                incrl();
                        }
} else
if (mons_class [i] == 101) // steam dragon
 {
  if (mons_ench [i] [2] != 6)
                        {
                strcat(info [info_lines], " breathes.");
                incrl();
                        }
 } else

 if (mons_class [i] == 79) // great orb of eyes
 {
  if (mons_ench [i] [2] != 6)
                        {
                strcat(info [info_lines], " gazes.");
                incrl();
                        }
 } else
 if (mons_class [i] == 70) // giant orange brain
 {
  if (mons_ench [i] [2] != 6)
                        {
                strcat(info [info_lines], " pulsates.");
                incrl();
                        }
 } else
 if ((mons_class [i] >= 80 && mons_class [i] <= 90) | mons_class [i] == 68)
        {
                strcat(info [info_lines], " gestures.");
                incrl();
        } else
        {
                strcat(info [info_lines], " casts a spell.");
                incrl();
       }
} // end of if mons_near

if (spell_cast == 16)
{
 if (mons_near(i) == 1)
        {
//              strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
//              strcat(info [info_lines], " casts a spell!");
//              incrl();
                strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
                strcat(info [info_lines], " blinks!");
                incrl();
  monster_blink(i);
  continue;
 }
}

/* switch(random2(3))
 {
  case 0: spell_cast = 0; break;
  case 1: spell_cast = 4; break;
  case 2: spell_cast = 5; break;
  case 3: spell_cast = 1; break;
 }
*/

// spell_cast = random2(13);

        //if (spell_cast == 105) spell_cast = 104;

//      if (spell_cast != 5 && spell_cast != 11)
//      {
//              tracer_f();
//      }

 beam_damage = 5 * mons_HD [i]; // really???

//      if (spell_cast == 5 | spell_cast == 11 | spell_cast == 13 | spell_cast == 14) // doesn't need to worry about haste
//      {
                mons_cast();
//      }

//      item_quant [mons_inv [i] [4]] --;
//      if (item_quant [mons_inv [i] [4]] == 0) mons_inv [i] [4] = 501; // = item_link [mons_inv [i] [4]] ???
        mmov_x = 0;
 trac_hit_tamed = 0;
        continue;
//}
break;



case 12:
if (invis != 0 && mons_see_invis(mons_class [i]) == 0) break;
if (mons_inv [i] [1] != 501) // manticore
{
                hand_used = mons_inv [i] [1];
                beam_range = 6;
                //              strcpy(beam_name, fake_name [hand_used]);//item_type [mons_inv [i] [1]]]);
                item_name(item_class [hand_used], item_type [hand_used], item_dam [hand_used], item_plus [hand_used], 1, item_ident [hand_used], 6, str_pass);
                strcpy(beam_name, str_pass);   //"dart");
                mons_throw();
                //mmov_x = 0;
                if (brek == 1)
                {
                        brek = 0;
                        continue;
                }
}
break;


// dragon breath weapon:
case 29:
case 73: // hellhound
case 75:
if (invis != 0 && mons_see_invis(mons_class [i]) == 0) break;
if ((mons_class [i] != 73 && random2(13) < 3) | random2(10) == 0)
{
if (mons_beh [i] == 6 | mons_beh [i] == 7)
{
 trac_targ = mons_hit [i];
 tracer_f();
 if (trac_hit_mons == 0) break;
}
dragon();
mmov_x = 0;
//mmov_y = 0;
continue;
//break;
} // end of dragon breathe
break;

} // end switch??


end_switch : if (gmon_use [mons_class [i]] > 0 && mons_inv [i] [1] != 501 && mons_class [i] != 30 && (invis == 0 | mons_see_invis(mons_class [i]) != 0)) // 2-h ogre
{

hand_used = mons_inv [i] [1];
if (random2(10) < 7)
{
        if (item_quant [hand_used] > 0)
        {
/*      brek = 0;
        beam_colour = 200;
        beam_name [0] = 48;
        beam_type = 0;
        beam_range = 6; // should be same as range is going to be for real missile.
        move_x = trac_targ_x - mons_x [i];
        move_y = trac_targ_y - mons_y [i];
        beam_source_x = mons_x [i];
        beam_source_y = mons_y [i];
        beam();*/
  if (trac_targ == MHITYOU && tracer_mons == 1) goto end_throw;
  if (trac_targ == MHITYOU && mons_near(i) == 0) goto end_throw;
  tracer_f();
  if (mons_beh [i] == 7 && tracer == 1 && tracer_mons != 1) goto end_throw;

                if (tracer != 0 | (trac_targ != MHITYOU && trac_hit_mons != 0)) // doesn't need to worry about haste
                {
                beam_range = 6;
//              strcpy(beam_name, fake_name [hand_used]);//item_type [mons_inv [i] [1]]]);
                item_name(item_class [hand_used], item_type [hand_used], item_dam [hand_used], item_plus [hand_used], 1, item_ident [hand_used], 6, str_pass);
                strcpy(beam_name, str_pass);
                mons_throw();
                //mmov_x = 0;
                if (brek == 1)
                {
                        brek = 0;
                        continue;
                }
                }

//      if (tracer != 0)
//      {
                //break;
//      } // end of if brek
        }
}

}

} // end if mons_near



end_throw : if ((mons_beh [i] == 7 | mons_beh [i] == 6) && (mmov_x != 0 | mmov_y != 0) && grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] % 100 >= 50)
{
                for (p = 0; p < MNST; p ++)
                {
                if (mons_x [p] == mons_x [i] + mmov_x && mons_y [p] == mons_y [i] + mmov_y && mons_class [p] > -1)
                {
                        if (monsters_fight(i, p) == 1)
   {
    if (mons_class [i] == 1 | mons_class [i] == 46) mons_speed_inc [i] -= mons_speed [i];
                  mmov_x = 0; mmov_y = 0;
                        brkk = 1;
                        break;
   }
                }
                }
} // end of if mons_beh [i] == 7
//} // end of bat

if (brkk == 1) continue;





if (mons_x [i] + mmov_x == x_pos && mons_y [i] + mmov_y == y_pos)
{
//o = 4;//mons_class [i];
//j = 5;//item_class [mons_inv [i] [0]];
//k = 6;//item_type [mons_inv [i] [0]];
//p = 7;//property [j] [k] [0];
//p = 8;
//mmov_y = 1;
//mmov_x = property [item_class [mmov_y]] [item_type [mmov_y]] [0];
//mmov_x = property [item_class [mons_inv [i] [0]]] [item_type [mons_inv [i] [0]]] [0];


mmov_x = mons_inv [i] [0];

if (mons_class [i] == 1 | mons_class [i] == 46 && bat == 0) monster_attack(i);

if ((mons_class [i] == 1 | mons_class [i] == 46) && mons_beh [i] != 7) // giant bat
{
        mons_beh [i] = 2;
        bat = 1;
}



if (mons_class [i] != 1 && mons_class [i] != 46) monster_attack(i);


if (mons_class [i] == 32 && mons_hp [i] < 1)
{
        for (j = 0; j < 3; j++)
                {
                        mons_ench [i] [j] = 0;
                }

        mons_ench_1 [i] = 0;
        //itc = 0;
        no_mons --;
        mons_class [i] = -1;
        if (grid [mons_x [i]] [mons_y [i]] >= 50) grid [mons_x [i]] [mons_y [i]] -= 50;
        spore_goes_pop(); // has to be after the above, so that spore isn't killed twice.
        continue;
} // end of if giant spore



mmov_x = 0; mmov_y = 0; continue; //break;
}



if (mons_class [i] == -1 | mons_class [i] == 61) continue;
                         // o plant
monster_move();



/*
if (mons_x [i] > x_pos - 6 && mons_x [i] < x_pos + 6 && mons_y [i] > y_pos - 6 && mons_y [i] < y_pos + 6 && show [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] != 0)
{
brek = 1;
viewwindow();
}
*/

if (mons_beh [i] == 1)//: // chasing YOU
if (mons_near(i) == 1 && (invis == 0 | mons_see_invis(mons_class [i]) != 0))// && random2 (6) != 0))
{
        mons_targ_1_x [i] = x_pos;
        mons_targ_1_y [i] = y_pos;
}

if (mons_hp [i] <= (mons_hp_max [i] / 4 - 1) && mons_intel(mons_class [i]) > 1) mons_beh [i] = 3;



}
endbat : bat = 0;

if (mons_beh [i] == 6 && random2(3) != 0) mons_beh [i] == 1; // I think?

} // end of if (mons_class != -1)

}
//}
//}

//if (mons_near(i) == 1)
{
//itoa(monc, st_prn, 10);
//mons_speed [i], st_prn, 10);
//strcpy(info [info_lines], st_prn);
//strcat(info [info_lines], ",");
//itoa(mons_speed_inc [i], st_prn, 10);
//strcat(info [info_lines], st_prn);
//incrl();
}



} // end of void monster()


void tracer_f(void)
{

  trac_hit_mons = 0;
  tracer = 0;
                tracer_mons = 0;
                brek = 0;
                beam_colour = 200;
                beam_name [0] = 48;
                beam_type = 0;
        beam_range = 10;
                move_x = trac_targ_x - mons_x [i];
                move_y = trac_targ_y - mons_y [i];
                beam_source_x = mons_x [i];
                beam_source_y = mons_y [i];
  aim_down = 0;
                beam();
//  trac_targ_x = 0;
//              trac_targ_y = 0;
//  trac_targ = MHITNOT;
}





void mons_pickup(void)
{

if (mons_class [i] == 35) // Jelly!
{
//      if (random2(3) == 0) return;

   // need to make sure it doesn't eat artefacts etc.

   int hps_gained;
   int quant_eated;

   quant_eated = random2(10) + 1;

   if (quant_eated > item_quant [igrid [mons_x [i]] [mons_y [i]]]) quant_eated = item_quant [igrid [mons_x [i]] [mons_y [i]]];

   if (item_class [igrid [mons_x [i]] [mons_y [i]]] == 0 && item_dam [igrid [mons_x [i]] [mons_y [i]]] > 180)
     return; // unique items

   if (item_class [igrid [mons_x [i]] [mons_y [i]]] == 1 && (item_type [igrid [mons_x [i]] [mons_y [i]]] == 0 | item_type [igrid [mons_x [i]] [mons_y [i]]] == 5))
   {
     // shouldn't eat stone things.
     // - but what about stone wands & rings?
     return;
   }

   if (item_class [igrid [mons_x [i]] [mons_y [i]]] < 15)
   {
      hps_gained = quant_eated * mass [item_class [igrid [mons_x [i]] [mons_y [i]]]] [item_type [igrid [mons_x [i]] [mons_y [i]]]];
      hps_gained /= 20;
//        hps_gained = random2(10) + 3;
   } else
   {
      hps_gained = 1;
   }

   if (hps_gained < 1) hps_gained = 1;
   if (hps_gained > 50) hps_gained = 50;


   item_quant [igrid [mons_x [i]] [mons_y [i]]] -= quant_eated;
   if (item_quant [igrid [mons_x [i]] [mons_y [i]]] <= 0) igrid [mons_x [i]] [mons_y [i]] = item_link [igrid [mons_x [i]] [mons_y [i]]];


   mons_hp [i] += hps_gained;
   if (mons_hp [i] > mons_hp_max [i]) mons_hp_max [i] = mons_hp [i];
   // adjust for mass of object!!!

   if (mons_near(i) == 1)
   {
      strcpy(info [info_lines], "You hear a slurping noise.");
      incrl();
   } else
         {
            strcpy(info [info_lines], "You hear a distant slurping noise.");
            incrl();
         }


if (mons_hp [i] >= 50)
{

         if (jelly_divide(i) == 0) mons_hp [i] = 50; //abort();

         //jelly_divide(i);

         /*no_mons ++;
         mn_no ++;

         mons_beh [k] = 0; // Look at this!*/

}

   return;

}




switch(item_class [igrid [mons_x [i]] [mons_y [i]]])
{
        case 0:
        if (mons_inv [i] [0] != 501)
                return;
 if (item_dam [igrid [mons_x [i]] [mons_y [i]]] > 180) return; // too tricky for now
        mons_inv [i] [0] = igrid [mons_x [i]] [mons_y [i]];
        igrid [mons_x [i]] [mons_y [i]] = item_link [igrid [mons_x [i]] [mons_y [i]]];
 item_link [mons_inv [i] [0]] = 501;
 if (item_dam [mons_inv [i] [0]] % 30 == 7) mons_AC [i] += 3;
        //strcpy(info [info_lines], "Pickup: ");
        //itoa(i, st_prn, 10);
        //strcat(info [info_lines], st_prn);
        //incrl();
        mmov_x = 0; mmov_y = 0;
        break;

        case 1:
        if (mons_inv [i] [1] != 501 && item_type [mons_inv [i] [1]] == item_type [igrid [mons_x [i]] [mons_y [i]]] && item_plus [mons_inv [i] [1]] == item_plus [igrid [mons_x [i]] [mons_y [i]]] && item_dam [mons_inv [i] [1]] == item_dam [igrid [mons_x [i]] [mons_y [i]]])
        {
                item_quant [mons_inv [i] [1]] += item_quant [igrid [mons_x [i]] [mons_y [i]]];
                item_quant [igrid [mons_x [i]] [mons_y [i]]] = 0;
                igrid [mons_x [i]] [mons_y [i]] = item_link [igrid [mons_x [i]] [mons_y [i]]];
  item_link [mons_inv [i] [0]] = 501;
                return;
        }
        if (mons_inv [i] [0] != 501 | item_quant [igrid [mons_x [i]] [mons_y [i]]] == 1)
                return;
        mons_inv [i] [1] = igrid [mons_x [i]] [mons_y [i]];
        igrid [mons_x [i]] [mons_y [i]] = item_link [igrid [mons_x [i]] [mons_y [i]]];
 item_link [mons_inv [i] [0]] = 501;
        //strcat(info [info_lines], st_prn);
        //incrl();
        mmov_x = 0; mmov_y = 0;
        break;

}


/*
for (o = 0; o < ITEMS; o ++)
{
        if (itc > it_no) break;


        if (item_quant [o] != 0)
        {
        itc++;

        //if (gmon_use [mons_class [i]] == 0) break; //continue;

        if (item_x [o] == mons_x [i] && item_y [o] == mons_y [i] && item_quant [o] > 0)
        {

        if ((mons_inv [i] [0] == 501 | item_quant [mons_inv [i] [0]] == 0) && item_class [o] == 0 && mons_inv [i] [0] != o)
        {
                mons_inv [i] [0] = o; // picking up weapons.
                brek = 1;
                break;
        //strcpy(info [info_lines], "Hello!"); incrl(); more();
        }
        if ((mons_inv [i] [1] == 501 | item_quant [mons_inv [i] [1]] == 0) && item_class [o] == 1 && mons_inv [i] [1] != o)
        {
                mons_inv [i] [1] = o;
                brek = 1;
                break;
        }
        //strcpy(info [info_lines], "Hello2!"); incrl(); more();
        }

        }
}
itc = 0;
*/
}






void monster_move(void)
{
char which_first = random2(2);

// whether x or y comes first.
//char mmov2_x [120];
//char mmov2_y [120];
//int s; // a loop variable. not


//for (i = 0; i < 120; i++)
//{
/*
if (mons_class [i] != -1)  // if monster isn't there, don't bother.
{
//grid [mons_x [i]] [mons_y [i]] = basic_grid [mons_x [i]] [mons_y [i]];
//grid_col [mons_x [i]] [mons_y [i]] = basic_grid_col [mons_x [i]] [mons_y [i]];


// enchantments:

if (mons_ench [i] != 0)
{

switch (mons_ench [i])
{
        case 1: // slow
        if (random2(100) == 0)
        {
                mons_speed [i] *= 2;
                mons_ench [i] = 0;
        }
        break;

}

}

if (haste == 0) mons_speed_inc [i] += (mons_speed [i] * speed);
else mons_speed_inc [i] += (mons_speed [i] * (speed / 2));


while (mons_speed_inc [i] >= 8)
{
mons_speed_inc [i] -= 1;


if (mons_targ_1_x [i] == 155) continue; // if it hasn't found you yet.


//if (gmon_use [mons_class [i]] == 1)
//{
//if ((mons_inv [i] [0] != 255 && mons_inv [i] [1] != 255 && mons_inv [i] [2] != 255 && mons_inv [i] [3] != 255) | random2(10) < 5)
        //continue;


for (o = 0; o < 200; o ++)
{
        if (item_x [o] == mons_x [i] && item_y [o] == mons_y [i] && item_quant [o] > 0)
        {

        if (mons_inv [i] [0] == 255 && (item_class [o] == 0 | item_class [o] == 1))
        {
                mons_inv [i] [0] = o; // picking up weapons.
        //strcpy(info [info_lines], "Hello!"); incrl(); more();
        }
        //strcpy(info [info_lines], "Hello2!"); incrl(); more();
        }

}

//} // end of if (gmon_use ...)

// if monster can see you...

mons_targ_1_x [i] = x_pos; mons_targ_1_y [i] = y_pos;


if (mons_targ_1_x [i] < mons_x [i]) mmov_x = -1; else mmov_x = 0;
if (mons_targ_1_x [i] > mons_x [i]) mmov_x = 1;

if (mons_targ_1_y [i] < mons_y [i]) mmov_y = -1; else mmov_y = 0;
if (mons_targ_1_y [i] > mons_y [i]) mmov_y = 1;
//}




if (mons_x [i] + mmov_x == x_pos && mons_y [i] + mmov_y == y_pos)
{
//o = 4;//mons_class [i];
//j = 5;//item_class [mons_inv [i] [0]];
//k = 6;//item_type [mons_inv [i] [0]];
//p = 7;//property [j] [k] [0];
//p = 8;
//mmov_y = 1;
//mmov_x = property [item_class [mmov_y]] [item_type [mmov_y]] [0];
mmov_x = property [item_class [mons_inv [i] [0]]] [item_type [mons_inv [i] [0]]] [0];
monster_attack(i);
mmov_x = 0; mmov_y = 0; break;
}


// dragon breath weapon:

if (random2(10) < 5 && mons_class [i] == 29)
{

dragon();
break;
/*

if (mons_x [i] > x_pos - 6 && mons_x [i] < x_pos + 6 && mons_y [i] > y_pos - 6 && mons_y [i] < y_pos + 6)
{
viewwindow();
if (show [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] != 0)
{
move_x = 0; move_y = 0;

if (mons_targ_1_x [i] == mons_x [i])
{
if (mons_targ_1_y [i] < mons_y [i]) move_y = -1; else move_y = 1;
}

if (mons_targ_1_y [i] == mons_y [i])
{
if (mons_targ_1_x [i] < mons_x [i]) move_x = -1; else move_x = 1;
}

if (mons_targ_1_x [i] - mons_x [i] == mons_targ_1_y [i] - mons_y [i])
{
if (mons_targ_1_y [i] < mons_y [i]) move_y = -1; else move_y = 1;
if (mons_targ_1_y [i] == mons_y [i]) move_y = 0;

if (mons_targ_1_x [i] < mons_x [i]) move_x = -1; else move_x = 1;
if (mons_targ_1_x [i] == mons_x [i]) move_x = 0;
}

if (move_x != 0 | move_y != 0)
{
        beam_range = random2(10) + 5;
        //beam_type = inv_class [zap_device_2];

        beam_source_x = mons_x [i]; beam_source_y = mons_y [i];

        //if (inv_type [zap_device_2] == 0)
        //{
                strcpy(beam_name, gmon_name [mons_class [i]]);
                strcat(beam_name, "'s blast of flame");
                beam_damage = 10;
                beam_colour = 4;
                beam_type = 35; // hash #
        //}
                strcpy(info [info_lines], "The ");
                strcat(info [info_lines], gmon_name [mons_class [i]]);
                strcat(info [info_lines], " breathes fire.");
                incrl();
        //viewwindow();
        beam(); break;
}
}
}

} // end of dragon breathe
*/



int okmove = 16;
if (mons_flies(mons_class [i]) > 0 | mons_class [i] >= MLAVA0) okmove = 10;




// some monsters opening doors: change the gmon_use == 1 to gmon_use > 0 maybe?
        if (grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] == 3)
   {
      if (mons_class [i] == 25 | mons_class [i] == 51)
      {
        if (gmon_use [mons_sec [i]] > 0)
        {
                        grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] = 20;
//hp -= 5; hp_ch = 1;
          return;
        }
      } else
            if (gmon_use [mons_class [i]] > 0)
            {
                grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] = 20;
                   return;
            }

      if (mons_class [i] == 35)
      {
         grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] = 17;
         strcpy(info [info_lines], "You hear a slurping sound.");
         incrl();
         mons_hp [i] += 5;
         if (mons_hp [i] > mons_hp_max [i]) mons_hp_max [i] = mons_hp [i];
      }


   }
//      if (grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] == 3






// have a variable equal to the minimum value the monster can move through!

// equivalent of your move() for monsters:
if (mmov_x != 0 && mmov_y != 0)
{
                                                       // 11
if (grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] < okmove | grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] >= 50)
{
//if (grid [mons_x [i] + mmov_x] [mons_y [i]] >= 3 && grid [mons_x [i] + mmov_x] [mons_y [i]] < 30)



        // some monsters opening doors
        if (gmon_use [mons_class [i]] > 0 && grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] == 3)
        {
   if (mons_class [i] == 25 | mons_class [i] == 51)
   {
     if (gmon_use [mons_sec [i]] > 0)
     {
         grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] = 20;
        return;
     }
   } else
         {
                grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] = 20;
                return;
         }
        }


        //mmov_y = 0;
if (which_first == 1)
{

        if (grid [mons_x [i] + mmov_x] [mons_y [i]] > okmove && grid [mons_x [i] + mmov_x] [mons_y [i]] < 50)
        {
                mmov_y = 0;
        } else
   //mmov_y = random2 (3) - 1;
        if (grid [mons_x [i]] [mons_y [i] + mmov_y] > okmove && grid [mons_x [i]] [mons_y [i] + mmov_y] < 50)
        {
                mmov_x = 0;
        }

}  // end if which_first

        //mmov_x = random2 (3) - 1;

            //          if (grid [mons_x [i]] [mons_y [i] + mmov_y] >= 3 && grid [mons_x [i]] [mons_y [i] + mmov_y] < 30)
                if (mmov_x != 0 && mmov_y != 0)
                {
                mmov_x = random2 (3) - 1;
                mmov_y = random2 (3) - 1;
                }

}
//mmov_x = 0; mmov_y = 0; // put something more intelligent later.
} else // end of: if (mmov_x !=0 etc
{

        if (mmov_x != 0 ^ mmov_y != 0) // ^ = XOR, | = OR(inc)
        {                                                             // 11
        if (mmov_x != 0 && (grid [mons_x [i] + mmov_x] [mons_y [i]] < okmove | grid [mons_x [i] + mmov_x] [mons_y [i]] >= 50))
        {
                //mmov_x = 0;



if (random2(2) == 0)
{

                if (grid [mons_x [i]] [mons_y [i] - 1] > okmove && grid [mons_x [i]] [mons_y [i] - 1] < 50)
                {
                        mmov_y = -1;
   mmov_x = 0;
                        //mmov_y = random2 (3) - 1;
                } else
                {
                        if (grid [mons_x [i]] [mons_y [i] + 1] > okmove && grid [mons_x [i]] [mons_y [i] + 1] < 50)
                        {
                                mmov_y = 1;
    mmov_x = 0;
                                //mmov_y = random2 (3) - 1;
                        }
  }
} else
 {
                if (grid [mons_x [i]] [mons_y [i] + 1] > okmove && grid [mons_x [i]] [mons_y [i] + 1] < 50)
                {
                        mmov_y = 1;
   mmov_x = 0;
                        //mmov_y = random2 (3) - 1;
                } else
                {
                        if (grid [mons_x [i]] [mons_y [i] - 1] > okmove && grid [mons_x [i]] [mons_y [i] - 1] < 50)
                        {
                                mmov_y = -1;
    mmov_x = 0;
                                //mmov_y = random2 (3) - 1;
                        }
                }
 } // end if random2(2) else







        }  else

                                              // was 11
        if ((grid [mons_x [i]] [mons_y [i] + mmov_y] < okmove | grid [mons_x [i]] [mons_y [i] + mmov_y] >= 50) && mmov_y != 0)
        {
                //mmov_y = 0;

if (random2(2) == 0)
 {

                if (grid [mons_x [i] - 1] [mons_y [i]] > okmove && grid [mons_x [i] - 1] [mons_y [i]] < 50)
                        {
                                mmov_x = -1;
    mmov_y = 0;
    //mmov_x = random2 (3) - 1;
                        } else
                        if (grid [mons_x [i] + 1] [mons_y [i]] > okmove && grid [mons_x [i] + 1] [mons_y [i]] < 50)
                        {
                                mmov_x = 1;
    mmov_y = 0;
                                //mmov_x = random2 (3) - 1;
                        }
 } else
   {


                if (grid [mons_x [i] + 1] [mons_y [i]] > okmove && grid [mons_x [i] + 1] [mons_y [i]] < 50)
                        {
                                mmov_x = 1;
    mmov_y = 0;
    //mmov_x = random2 (3) - 1;
                        } else
                        if (grid [mons_x [i] - 1] [mons_y [i]] > okmove && grid [mons_x [i] - 1] [mons_y [i]] < 50)
                        {
                                mmov_x = -1;
    mmov_y = 0;
                                //mmov_x = random2 (3) - 1;
                        }

    }  // end random2(2) else








        }

         // end of 2nd else


        //}











if (mmov_x != 0 && mmov_y != 0)
{                                                     // was 11.
if (grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] < okmove | grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] >= 50)
{
//if (grid [mons_x [i] + mmov_x] [mons_y [i]] >= 3 && grid [mons_x [i] + mmov_x] [mons_y [i]] < 30)

/*      // some monsters opening doors: change the gmon_use == 1 to gmon_use > 0 maybe?
        if (gmon_use [mons_class [i]] == 1 && grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] == 3)
        {
        grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] = 20;
        return;
        } */


        {
        //mmov_y = 0;
                mmov_y = random2 (3) - 1;
                mmov_x = random2 (3) - 1;
        } //else

            //          if (grid [mons_x [i]] [mons_y [i] + mmov_y] >= 3 && grid [mons_x [i]] [mons_y [i] + mmov_y] < 30)

                        //mmov_x = 0;
                //      mmov_x = random2 (3) - 1;
                //      mmov_y = random2 (3) - 1;


}
//mmov_x = 0; mmov_y = 0; // put something more intelligent later.
}




/* This is what I've just removed


 else // end of: if (mmov_x !=0 etc
{

        if (mmov_x != 0 ^ mmov_y != 0) // ^ = XOR, | = OR(inc)
        {
        if ((grid [mons_x [i] + mmov_x] [mons_y [i]] < 11 | grid [mons_x [i] + mmov_x] [mons_y [i]] >= 50) && mmov_x != 0)
        {
                //mmov_x = 0;
                if (grid [mons_x [i]] [mons_y [i] + 1] > 10 && grid [mons_x [i]] [mons_y [i] + 1] < 70)
                {
                        //mmov_y = 1;
                        mmov_y = random2 (3) - 1;
                } else
                {
                        if (grid [mons_x [i]] [mons_y [i] - 1] > 10 && grid [mons_x [i]] [mons_y [i] - 1] < 70)
                        {
                                mmov_x = random2 (3) - 1;
                        }
                }
        }  else



        if ((grid [mons_x [i]] [mons_y [i] + mmov_y] < 11 | grid [mons_x [i]] [mons_y [i] + mmov_y] >= 70) && mmov_y != 0)
        {
                //mmov_y = 0;
                if (grid [mons_x [i] + 1] [mons_y [i]] > 10 && grid [mons_x [i] + 1] [mons_y [i]] < 70)
                        {
                                mmov_x = random2 (3) - 1;
                        } else
                        if (grid [mons_x [i] - 1] [mons_y [i]] > 10 && grid [mons_x [i] - 1] [mons_y [i]] < 70)
                        {
                                mmov_x = random2 (3) - 1;
                        }
        }


} // end of if (mmov_x != 0 ^ etc

}

*/


//} // end of 1st else.   the end of this else appears after this comment:
//if (mmov_x == 0 && mmov_y == 0)
//{

//for (j = -1; j < 2; j++)
//{
//for (k = -1; k < 2; k++)
//{
//if (grid [mons_x [i] + j



//} // end of if (mmov_x == 0 && etc
//} // end of for k
//} // end of for j

} // end of if (mmov_x != 0 ^ etc for the 2nd time
}  // end of else above


if (which_first == 0)
{

        if (grid [mons_x [i] + mmov_x] [mons_y [i]] > okmove && grid [mons_x [i] + mmov_x] [mons_y [i]] < 50)
        {
                mmov_y = 0;
        } else
   //mmov_y = random2 (3) - 1;
        if (grid [mons_x [i]] [mons_y [i] + mmov_y] > okmove && grid [mons_x [i]] [mons_y [i] + mmov_y] < 50)
        {
                mmov_x = 0;
        }

}  // end if which_first




// the attack part used to be here...

//if (mons_class [i] == 1 && mons_beh [i] == 2)
/*
if (mons_beh [i] == 2)
{
if (mons_x [i] + mmov_x == x_pos && mons_y [i] + mmov_y == y_pos)
        {
                mmov_x = 0;
                mmov_y = 0;
        }
}
*/
/*
if (mons_beh [i] == 2 && mmov_y == 0 && mmov_y == 0)
{
        mons_targ_1_x [i] = random2(80);
        mons_targ_1_y [i] = random2(70);
}
*/

// Need to have two wandering (mons_beh = 2); one friendly, one not.


// maybe: (makes worms stupid, though)
if (mons_class [i] == 19 && mmov_x != 0 && mmov_y != 0)
{
        if (random2(2) == 0) mmov_x = 0; else mmov_y = 0;
        if (grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] == 1 && mons_x [i] + mmov_x > 6 && mons_x [i] + mmov_x < 73 && mons_y [i] + mmov_y > 6 && mons_y [i] + mmov_y < 63)
                grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] = 17;
}

unsigned char grik = grid [mons_x [i] + mmov_x] [mons_y [i] + mmov_y] % 100;

if ((grid [mons_x [i]] [mons_y [i]] >= 50 && grid [mons_x [i]] [mons_y [i]] < 100) | (grid [mons_x [i]] [mons_y [i]] >= 150 && grid [mons_x [i]] [mons_y [i]] < 200)) grid [mons_x [i]] [mons_y [i]] -= 50;

if (grik > okmove && grik < 50)// | grik > 100 + okmove)
{

        if (mons_x [i] + mmov_x == x_pos && mons_y [i] + mmov_y == y_pos)
        {
                monster_attack(i);
                mmov_x = 0;
                mmov_y = 0;
        }
/*
        if (mmov_x != 0 | mmov_y != 0)
        {*/
                vacated_x = mons_x [i];
                vacated_y = mons_y [i];
/*      } else
        {
                vacated_x = 0;
        }*/

 if (mons_class [i] >= MLAVA0 && mons_class [i] < MWATER0 && grik != 11)
 {
  mmov_x = 0;
  mmov_y = 0;
 }
 if (mons_class [i] >= MWATER0 && grik != 12)
 {
  mmov_x = 0;
  mmov_y = 0;
 }

 if (mons_class [i] == 67 && see_grid(mons_x [i] + mmov_x, mons_y [i] + mmov_y) == 1)
 {
  mmov_x = 0;
  mmov_y = 0;
 }

//if (mons_class [i] == 68 && (mmov_x != 0 | mmov_y != 0)) place_cloud(101, mons_x [i], mons_y [i], 2 + random2(4));
if (mons_class [i] == 68) place_cloud(101, mons_x [i], mons_y [i], 2 + random2(4));

 // this appears to be the real one:
        mons_x [i] += mmov_x;
        mons_y [i] += mmov_y;

        if (mons_class [i] == 19 && grid [vacated_x] [vacated_y] < 50 && (mmov_x != 0 | mmov_y != 0)) //(mmov_x != 0 | mmov_y != 0)) // worm
        {
                char vac_x_2 = vacated_x; //mons_x [i];
                char vac_y_2 = vacated_y; //mons_y [i];

                //if (grid [mons_x [i]] [mons_y [i]] <= 50)
                grid [mons_x [i]] [mons_y [i]] += 50;
      // ^^^ don't know about this

                i++;

                if (mons_class [i] != 56)
                {
                        //i--;
                        goto out_of_worm;
                }

                while(mons_class [i] == 56)
                {
                        //assert(grid [vac_x_2] [vac_y_2] < 50);

                        if (grid [mons_x [i]] [mons_y [i]] >= 50) grid [mons_x [i]] [mons_y [i]] -= 50;
//grid [mons_x [i]] [mons_y [i]] -= 50;

                        vacated_x = mons_x [i];
                        vacated_y = mons_y [i];

                        mons_x [i] = vac_x_2;
                        mons_y [i] = vac_y_2;

                        vac_x_2 = vacated_x;
                        vac_y_2 = vacated_y;

                        //if (grid [mons_x [i]] [mons_y [i]] < 50)
                        grid [mons_x [i]] [mons_y [i]] += 50;
         // ^^^ don't know about this

                        i ++;
                }

                out_of_worm: i--;

                return; //continue;// hopefully will do next monster...

        }



/*
        if (gmon_use [mons_class [i]] == 1)
        {
                for (o = 0; o < 8; o ++)
                {
                        if (mons_inv [i] [o] != 501)
                        {

                        //if (item_x [mons_inv [i] [o]] == mons_x [i] && item_y [mons_inv [i] [o]] == mons_y [i])
                        //{
                                //item_x [mons_inv [i] [o]] += mmov_x;
                                //item_y [mons_inv [i] [o]] += mmov_y;
                                item_x [mons_inv [i] [o]] = mons_x [i];
                                item_y [mons_inv [i] [o]] = mons_y [i];
                        //} else mons_inv [i] [o] = 501;

                        }

                }
        }
*/
} else
      {
//                      if (mons_beh [i] == 1) mons_beh [i] = 2;











                                      // Look!
      }

// need to put in something so that monster picks up multiple items (eg ammunition) identical to those it's carrying.

if (grid [mons_x [i]] [mons_y [i]] < 50 | (grid [mons_x [i]] [mons_y [i]] >= 100 && grid [mons_x [i]] [mons_y [i]] < 150))
         grid [mons_x [i]] [mons_y [i]] += 50;
// ^^ don't know

/*
if (grid [mons_x [i]] [mons_y [i]] > 100)
{
        mons_in_cloud();
        if (mons_class [i] == -1) mons_speed_inc [i] = 1;
}
*/

// monsters stepping on traps:
if (grid [mons_x [i]] [mons_y [i]] > 74 && grid [mons_x [i]] [mons_y [i]] < 79 && (mmov_x != 0 | mmov_y != 0))
// remember that all of the grid values are + 50.
// later include flying monsters
{
if (mons_flies(mons_class [i]) == 0) mons_trap();

} // end of if monster steps on trap


} // end of monster_move()



/*
char grid_okay(int mc, int grd)
{

}
*/

void plant_spit(void)
{
if (mons_near(i) == 0) return;

strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0));
  strcat(info [info_lines], " spits at you.");
  incrl();

move_x = trac_targ_x - mons_x [i];
move_y = trac_targ_y - mons_y [i];
 strcpy(beam_name, "plant spit");

        beam_type = '#';
        beam_source_x = mons_x [i];
        beam_source_y = mons_y [i];
 beam_range = 10;
        beam_colour = YELLOW;
 beam_flavour = 8;

        beam_damage = 107; //property [item_class [hand_used]] [item_type [hand_used]] [0] + item_plus [hand_used] - 50;

        beam_hit = 20 + mons_HD [i] * 3;  //strength + (0.5 * dex) + (f_abil);
//      beam_hit += item_plus [hand_used] - 50;

        thing_thrown = 4;

 missile();
}








void mons_in_cloud(void)
{

int wc = which_cloud(mons_x [i], mons_y [i]);

switch(cloud_type [wc] % 100)
{
        case 99:
        cprintf("Fatal error: monster steps on nonexistant cloud!");
        exit(0);
   break;

        case 1: // fire
  if (mons_class [i] == 21 | mons_class [i] == 68) break; // fire vortex and efreet
        strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
        strcat(info [info_lines], " is engulfed in flame!");
        if (mons_near(i) == 1 && mons_class [i] != 68) incrl(); // efreet
   if (mons_res_fire(mons_class [i]) > 0 | (mons_inv [i] [2] != 501 && item_dam [mons_inv [i] [2]] % 30 == 2)) break;
        mons_hp [i] -= ((random2(5) + random2(5) + random2(5) + 3) * (float) mons_speed [i]) / 10;
 if (mons_res_fire(mons_class [i]) < 0 && !(mons_inv [i] [2] != 501 && item_dam [mons_inv [i] [2]] % 30 == 2)) mons_hp [i] -= random2(15) * (float) (mons_speed [i] / 10);
 // remember that the above is in addition to the other damage.
        if (mons_hp [i] < 1)
        {
                if (cloud_type [wc] >= 100) killer = 5; else killer = 1; // your cloud!
                monster_die(i);
                mons_speed_inc [i] = 1;
                if (grid [mons_x [i]] [mons_y [i]] < 100) grid [mons_x [i]] [mons_y [i]] += 50;
                //grid [mons_x [i]] [mons_y [i]] += 50;
        }
        break;

   case 2: // stinking cloud
   strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0));
   strcat(info [info_lines], " is engulfed in noxious gasses!");
   if (mons_near(i) == 1) incrl();

   if (mons_res_poison(mons_class [i]) > 0) return;

   beam_colour = 4;
   if (random2(5) + 1 >= mons_HD [i]) mons_ench_f2(i, mons_near(i), func_pass);

   mons_hp [i] -= (random2(3) * (float) mons_speed [i]) / 10; // 3
   if (mons_hp [i] < 1)
   {
//       killer = 5;
                if (cloud_type [wc] >= 100) killer = 5; else killer = 1; // your cloud!
       monster_die(i);
                 if (grid [mons_x [i]] [mons_y [i]] < 100) grid [mons_x [i]] [mons_y [i]] += 50;
   }

//   mons_ench


//      strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0));


//      mons_ench_f2(info, info_lines, mons_speed, mons_ench, i, mons_ench_1, mons_hp, mons_hp_max, mons_class, mons_near(i), 4, see_invis, func_pass);

//              info_lines = func_pass [0];


   break;




 case 3: // cold
        strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
        strcat(info [info_lines], " is engulfed in freezing vapours!");
 if (mons_near(i) == 1) incrl();
   if (mons_res_cold(mons_class [i]) > 0 | (mons_inv [i] [2] != 501 && item_dam [mons_inv [i] [2]] % 30 == 3)) break;
        mons_hp [i] -= ((random2(5) + random2(5) + random2(5) + 3) * (float) mons_speed [i]) / 10;
 if (mons_res_cold(mons_class [i]) < 0 && !(mons_inv [i] [2] != 501 && item_dam [mons_inv [i] [2]] % 30 == 3)) mons_hp [i] -= random2(15) * (float) (mons_speed [i] / 10);
 // remember that the above is in addition to the other damage.
        if (mons_hp [i] < 1)
        {
//              killer = 5;
                if (cloud_type [wc] >= 100) killer = 5; else killer = 1; // your cloud!
                monster_die(i);
                mons_speed_inc [i] = 1;
                if (grid [mons_x [i]] [mons_y [i]] < 100) grid [mons_x [i]] [mons_y [i]] += 50;
                //grid [mons_x [i]] [mons_y [i]] += 50;
        }
        break;

   case 4: // poison cloud
   strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0));
   strcat(info [info_lines], " is engulfed in a cloud of poison!");
   if (mons_near(i) == 1) incrl();
   if (mons_res_poison(mons_class [i]) > 0) return;
   beam_colour = 4;
   if (cloud_type [wc] >= 100) poison_monster(i, 1); // something else
                                                                else poison_monster(i, 0);

   mons_hp [i] -= (random2(3) * (float) mons_speed [i]) / 10; // 3
   if (mons_hp [i] < 1)
   {
                if (cloud_type [wc] >= 100) killer = 5; else killer = 1; // your cloud!
       monster_die(i);
                 if (grid [mons_x [i]] [mons_y [i]] < 100) grid [mons_x [i]] [mons_y [i]] += 50;
   }
   break;

// 5,6,7 are harmless smoke

  case 8: // steam
  if (mons_class [i] == 101) break;
        strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
        strcat(info [info_lines], " is engulfed in steam!");
        if (mons_near(i) == 1 && mons_class [i] != 68) incrl(); // efreet
   if (mons_res_fire(mons_class [i]) > 0 | (mons_inv [i] [2] != 501 && item_dam [mons_inv [i] [2]] % 30 == 2)) break;
        mons_hp [i] -= ((random2(6)) * (float) mons_speed [i]) / 10;
 if (mons_res_fire(mons_class [i]) < 0 && !(mons_inv [i] [2] != 501 && item_dam [mons_inv [i] [2]] % 30 == 2)) mons_hp [i] -= random2(6) * (float) (mons_speed [i] / 10);
        if (mons_hp [i] < 1)
        {
                if (cloud_type [wc] >= 100) killer = 5; else killer = 1; // your cloud!
                monster_die(i);
                mons_speed_inc [i] = 1;
                if (grid [mons_x [i]] [mons_y [i]] < 100) grid [mons_x [i]] [mons_y [i]] += 50;
                //grid [mons_x [i]] [mons_y [i]] += 50;
        }
  break;

}


}











void mons_trap(void)
{

        // maybe later introduce variables, maybe reusing the beam variables, to multiuse this bit for spear etc traps.


                        int tr;

                        for (tr = 0; tr < NTRAPS; tr ++)
                        {
                                if (trap_x [tr] == mons_x [i] && trap_y [tr] == mons_y [i]) break;
                                //if (tr == NTRAPS - 1) abort();
                        }

                        if (mons_near(i) == 1 && grid [trap_x [tr]] [trap_y [tr]] % 50 == 28)
   {
//    if (trap_type [tr] < 4)
    grid [trap_x [tr]] [trap_y [tr]] -= 3;
    if (trap_type [tr] >= 4) grid [trap_x [tr]] [trap_y [tr]] ++;
   }
                        //      grid [mons_x [i]] [mons_y [i]] = 75;

                                // etc for magic, pit traps...


                        switch(trap_type [tr])
                        {

                        case 0:
                        strcpy(beam_name, " dart");
                        beam_damage = 4;
                        beam_colour = 1;
                        beam_type = 3;
                        break;

                        case 1:
                        strcpy(beam_name, "n arrow");
                        beam_damage = 7;
                        beam_colour = 1;
                        beam_type = 1;
                        break;

                        case 2:
                        strcpy(beam_name, " spear");
                        beam_damage = 10;
                        beam_colour = 0;
                        beam_type = 11;
                        break;

                        case 3:
                        strcpy(beam_name, "n axe");
                        beam_damage = 15;
                        beam_colour = 0;
                        beam_type = 9;
                        break;

   case 4:
   if (mons_class [i] == 19 | mons_class [i] == 56) return;
   if (mons_near(i) == 1)
        {
                strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0));
     strcat(info [info_lines], " suddenly disappears!");
     incrl();
   }
   monster_teleport(i);
   return;

   case 5:
        if (mons_near(i) == 1)
        {
                strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0));
     strcat(info [info_lines], " looks momentarily disoriented.");
     incrl();
   }
   return;

                        } // end of switch






 /*     if (mons_near(i) == 1)
        {
        strcpy(info [info_lines], "A");
        strcat(info [info_lines], beam_name);
        strcat(info [info_lines], " suddenly fires out at ");
        strcat(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 1)); //gmon_name [mons_class [i]]);
        strcat(info [info_lines], "!");
        incrl();
        }*/

        if ((20 + curr_level * 2) * random2(200) / 100 >= mons_ev [i]);
        {
                damage_taken = random2(beam_damage);
                damage_taken -= random2 (mons_AC [i] + 1);
                if (damage_taken > 0) mons_hp [i] -= damage_taken;
        }
        if (damage_taken > 0)
        {
                if (mons_near(i) == 1)
                {
                        strcpy(info [info_lines], "A");
                        strcat(info [info_lines], beam_name);
                        strcat(info [info_lines], " hits ");
                        strcat(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 1)); //gmon_name [mons_class [i]]);
                        strcat(info [info_lines], "!");
                        incrl();
                }
                if (mons_hp [i] <= 0)
                {
                        killer = 5;
                        monster_die(i);
                        mons_speed_inc [i] = 1;
                }
        } else
        if (mons_near(i) == 1)
        {
                strcpy(info [info_lines], "A");
                strcat(info [info_lines], beam_name);
                strcat(info [info_lines], " misses ");
                strcat(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 1)); //gmon_name [mons_class [i]]);
                strcat(info [info_lines], "!");
                incrl();
        }




        if (random2(10) > 6 && it_no > 300) return;

        beam_x = mons_x [i];
        beam_y = mons_y [i];

        trapped = tr;

        itrap();

        //it_no += trap_item(it_no, icolour, item_ident, item_dam, item_plus, item_type, item_class, item_link, item_quant, igrid, beam_colour, beam_type, mons_x [i], mons_y [i]);





/*
        if (random2(10) + damage_taken < 7)
        {
        for (p = 0; p < ITEMS; p++)
        {
                if (item_class [p] == 1 && item_type [p] == 0 && item_x [p] == mons_x [i] && item_y [p] == mons_y [i] && item_type [p] == 0 && item_plus [p] == 50 && item_dam [p] == 0)
                {
                //if (strcmp("dart", fake_name [p]) == 0);
                if (item_ident [p] == 0)
                {
                        item_quant [p] += 1;
                        return;
                }
                }
        }


                        for (p = 0; p < ITEMS; p++)
                        {
                                if (item_quant [p] == 0)
                                {
                                        //strcpy(fake_name [p], "dart");
                                        item_ident [p] = 0;
                                        item_class [p] = 1;
                                        item_type [p] = 0;
                                        item_plus [p] = 50;
                                        item_dam [p] = 0;
                                        icolour [p] = 11;
                                        item_x [p] = mons_x [i]; item_y [p] = mons_y [i];
                                        item_quant [p] = 1;
                                        it_no ++;
                                        break;
                                }
                        } // end of p loop
                        } // end of if random2(10) ...

*/

} // end of mons_trap()




void mons_cast(void)
{
int sumcount = 0;
int sumcount2;
int summonik = 0;
//char sumbeh = 0;
//int sumtarg = 0;

if (spell_cast == 20 | spell_cast == 23) // etc...
{
        switch(spell_cast)
        {
  case 20: beam_type = 0; break; // burst of h-fire
  case 23: beam_type = 2; break; // brain feed
        } // end switch
if (trac_targ == MHITYOU) direct_effect();
   else mons_direct_effect();
return;
} // end of if (spell_cast)/

if (spell_cast == 21) // vampire summoning
{
 sumcount2 = random2(4) + 1;
 for (sumcount = 0; sumcount < sumcount2; sumcount ++)
 if (random2(3) != 0) create_monster(1, 24, mons_beh [i], mons_x [i], mons_y [i], mons_hit [i]);
    else create_monster(17, 24, mons_beh [i], mons_x [i], mons_y [i], mons_hit [i]);
        return;
}

if (spell_cast == 24) // summon anything
{
 sumcount2 = random2(4) + 1;
 for (sumcount = 0; sumcount < sumcount2; sumcount ++)
                        create_monster(random2(70), 24, mons_beh [i], mons_x [i], mons_y [i], mons_hit [i]);
    return;
}

if (spell_cast == 25) // summon fake R
{
  sumcount2 = random2(2) + 1;
 for (sumcount = 0; sumcount < sumcount2; sumcount ++)
                        create_monster(78, 24, mons_beh [i], mons_x [i], mons_y [i], mons_hit [i]);
    return;
}

if (spell_cast == 27) // summon devil
{
    summonik = random2(8) + 80;
    if (random2(4) == 0 | (summonik == 84 && random2(3) != 0)) summonik = 4;
                        create_monster(summonik, 23, mons_beh [i], mons_x [i], mons_y [i], mons_hit [i]);
    return;
}

//char create_monster(int cls, int dur, int beha, int cr_x, int cr_y, int hitting)

brek = 1;
viewwindow();


move_x = trac_targ_x - mons_x [i];
move_y = trac_targ_y - mons_y [i];

target_x = trac_targ_x;
target_y = trac_targ_y;

/*move_x = 0;
move_y = 0;*/

beam_source_x = mons_x [i];
beam_source_y = mons_y [i];

//spell_cast = 107;

//if (spell_cast != 49)
//{

                //strcpy(info [info_lines], "The ");
//              strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
//              strcat(info [info_lines], " reads a scroll!");
//              incrl();
//      }
//}

// Need to correct this for power of spellcaster

int ufdg = mons_spells(spell_cast, func_pass, beam_name);

ench_power = beam_damage; // !!!
beam_colour = func_pass [0];
beam_range = func_pass [1];
beam_damage = func_pass [2];
beam_hit = func_pass [3];
beam_type = func_pass [4];
beam_flavour = func_pass [5];
thing_thrown = func_pass [6];

ench_power = 5 * mons_HD [i];

if (spell_cast == 5 | spell_cast == 11 | spell_cast == 13 | spell_cast == 14) // haste/invis
{
        move_x = 0;
        move_y = 0;
}
//aim_down = 1;

if (spell_cast == 20) // burst of hellfire
{

 return;
}

switch(ufdg)
{
        case 0: missile(); break;
        case 1: beam(); break;
//      default: cprintf("Fatal error: monster casts nothing?!");
        //cout << "\n\r spell cast = " << (int) spell_cast << ", ufdg = " << ufdg << "     ";
//      exit(0);
}


/*

switch(spell_cast)
{
case 0: // magic missile
//strcpy(info [info_lines], "The ");
strcpy(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
strcat(info [info_lines], " hurls a ball of sizzling energy!");
incrl();
beam_colour = 13;//inv_col [throw_2];//icolour [inv_class [throw_2]] [inv_type [throw_2]];
strcpy(beam_name, "magic missile");// inv_name [throw_2]);
beam_range = random2(5) + 7;
beam_damage = 105;
beam_hit = 80;
beam_type = 35;
thing_thrown = 4;
//missile();
beam_flavour = 1; // a magical missile
missile();
break;

case 1: // flame
//strcpy(info [info_lines], "The ");
strcpy(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
strcat(info [info_lines], " hurls a puff of flame!");
incrl();
beam_colour = 4;
strcpy(beam_name, "puff of flame");
beam_range = random2(5) + 7;
beam_damage = 108; // should this be the same as magic missile?
beam_hit = 60;
beam_type = 42; // maybe make missile() put a # on wherever it hits?
thing_thrown = 4;
beam_flavour = 2; // fire
missile();
break;


case 2: // frost
//strcpy(info [info_lines], "The ");
strcpy(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
strcat(info [info_lines], " hurls a puff of frost!");
incrl();
beam_colour = 15;
strcpy(beam_name, "puff of frost");
beam_range = random2(5) + 7;
beam_damage = 108; // should this be the same as magic missile?
beam_hit = 60;
beam_type = 42; // maybe make missile() put a # on wherever it hits?
thing_thrown = 4;
beam_flavour = 3; // cold
missile();
break;

case 3: // paralysis
strcpy(beam_name, "0");
beam_range = random2(5) + 6;
beam_type = 0;
beam_colour = 3;
thing_thrown = 4;
beam_flavour = 4; // magic
beam();
break;

case 4: // slow monster
strcpy(beam_name, "0");
beam_range = random2(5) + 6;
beam_type = 0;
beam_colour = 0;
thing_thrown = 4;
beam_flavour = 4; // magic
beam();
break;

case 5: // haste
for (p = 0; p < 3; p ++)
{
        if (mons_ench [i] [p] == 2) return;
        if (mons_ench [i] [p] == 1)
        {
                mons_ench [i] [p] = 0;
                if (mons_speed [i] >= 100)
                {
                        mons_speed [i] = ((mons_speed [i] - 100) * 2) + 100;
                        } else mons_speed [i] *= 2;
                if (mons_ench [i] [0] == 0 && mons_ench [i] [1] == 0 && mons_ench [i] [2] == 0)
                        mons_ench_1 [i] = 0;
                brek = 1;
                break;
        }
} // end of for p
        if (brek == 0)
        for (p = 0; p < 3; p ++)
        {
        if (mons_ench [i] [p] == 0) // | p == 2) // replaces 3rd enchantment
                {
                mons_ench [i] [p] = 2;
                if (mons_speed [i] >= 100)
                {
                        mons_speed [i] = ((mons_speed [i] - 100) * 2) + 100;
                } else mons_speed [i] *= 2;
                mons_ench_1 [i] = 1;
                break;
                }
        }
//strcpy(info [info_lines], "The ");
strcpy(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
strcat(info [info_lines], " seems to speed up.");
incrl();
return;// break;


case 6: // slow monster
strcpy(beam_name, "0");
beam_range = random2(5) + 6;
beam_type = 0;
beam_colour = 4;
thing_thrown = 4;
beam_flavour = 4; // magic
beam();
break;




case 8:
strcpy(beam_name, "bolt of fire");
beam_range = random2(10) + 5;
beam_damage = 112;
beam_colour = 4;
beam_type = 35; // 35
thing_thrown = 1;
beam_flavour = 2; // fire
beam();
break;


case 9:
strcpy(beam_name, "bolt of cold");
beam_range = random2(10) + 5;
beam_damage = 112;
beam_colour = 15;
beam_type = 35;
thing_thrown = 1;
beam_flavour = 3; // cold
beam();
break;

}

*/

/*
void zapping(char ztype)
{

switch(ztype)
{
//if (inv_type [zap_device_2] == 0)
//{
        case 0: // flame
//      beam_source_x = x_pos;
//      beam_source_y = y_pos;
        beam_colour = 4;
        strcpy(beam_name, "puff of flame");
        beam_range = random2(5) + 7;
        beam_damage = 108; // should this be the same as magic missile?
        beam_hit = 60;
        beam_type = 42; // maybe make missile() put a # on wherever it hits?
        thing_thrown = 3;
        missile();
        beam_flavour = 2; // fire
        break;


        case 1: // frost
//      beam_source_y = y_pos;
        beam_colour = 15;
        strcpy(beam_name, "puff of frost");
        beam_range = random2(5) + 7;
        beam_damage = 108; // should this be the same as magic missile?
        beam_hit = 60;
        beam_type = 42; // maybe make missile() put a # on wherever it hits?
        thing_thrown = 3;
        missile();
        beam_flavour = 3; // cold
        break;


        case 3: // hasting
        strcpy(beam_name, "0");
        beam_range = random2(5) + 6;
        beam_type = 0;
        beam_colour = 1;
        thing_thrown = 1;
        beam();
        beam_flavour = 4; // magic
        break;

        case 4: // magic missile
        strcpy(info [info_lines], "You hurl a ball of sizzling energy!");
        incrl();
//      beam_source_x = x_pos;
//      beam_source_y = y_pos;
        beam_colour = 13;//inv_col [throw_2];//icolour [inv_class [throw_2]] [inv_type [throw_2]];
        strcpy(beam_name, "magic missile");// inv_name [throw_2]);
//      if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1)
//      {
//              beam_damage = property [inv_class [throw_2]] [inv_type [throw_2]] [0];
//              beam_damage += inv_plus [throw_2];
//      }
//       else beam_damage = 2; // later put mass thing here.
        beam_range = random2(5) + 7;
        beam_damage = 107;
        beam_hit = 80;
        beam_type = 35;
//      beam_hit = strength + (0.5 * dex) + (f_abil);
//      if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1) beam_hit += inv_plus [throw_2];
        //beam_hit *= random2(200) / 100;
        thing_thrown = 3;
        missile();
        beam_flavour = 1; // a magical missile
        break;

        case 5: // healing
        strcpy(beam_name, "0");
        beam_range = random2(5) + 6;
        beam_type = 0;
        beam_colour = 2;
        thing_thrown = 1;
        beam();
        beam_flavour = 4; // magic
        break;


        case 7:
        strcpy(beam_name, "bolt of fire");
        beam_range = random2(10) + 5;
        beam_damage = 10;
        beam_colour = 4;
        beam_type = 35; // 35
        thing_thrown = 1;
        beam();
        beam_flavour = 2; // fire
        break;
//}


//if (inv_type [zap_device_2] == 1)
//{
        case 8:
        strcpy(beam_name, "bolt of cold");
        beam_range = random2(10) + 5;
        beam_damage = 34;
        beam_colour = 15;
        beam_type = 35;
        thing_thrown = 1;
        beam();
        beam_flavour = 3; // cold
        break;
} // end of switch



}
*/


















} // end of void mons_cast()




void monster_teleport(char monstel)
{

grid [mons_x [monstel]] [mons_y [monstel]] -= 50;

char ogrid = 17;

if (mons_class [monstel] >= MLAVA0) ogrid = 11;
if (mons_class [monstel] >= MWATER0) ogrid = 12;

do
{
        mons_x [monstel] = random2(60) + 10;
        mons_y [monstel] = random2(50) + 10;
} while (grid [mons_x [monstel]] [mons_y [monstel]] != ogrid);

grid [mons_x [monstel]] [mons_y [monstel]] += 50;

} // end of void monster_teleport





/*
void summon_demon(void)
{
char passout = 0;

k = 0;

do
{
k ++;
} while (mons_class [k] != -1 && k < 90);

if (k >= 90) return;

//strcpy(info [info_lines], "The ");
strcpy(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
strcat(info [info_lines], " speaks a magical formula.");
incrl();

mons_class [k] = 54;//mons_alloc [random2(10)];

if (random2(4) == 0) mons_class [k] = 55;
if (random2(7) == 0) mons_class [k] = 56;

//strcpy(info [info_lines], "A "); // check for "An"
strcat(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 2)); //gmon_name [mons_class [k]]);
strcat(info [info_lines], " appears!");
incrl();

get_monster();

mons_speed [k] /= 10;

no_mons ++;

mons_beh [k] = 1; // Look at this!

mons_x [k] = mons_x [i];
mons_y [k] = mons_y [i];

passout = 0;
while (passout == 0)
{
//mons_x [k] = 10;// + random2(120) + 100;
//mons_y [k] = 10 + random2(60);// + 100;
//mons_x [k] += random2 (70);
if (random2(4) == 0 | mons_x [k] < 5 | mons_x [k] > 75 | mons_y [k] < 5 | mons_y [k] > 65)
{
        mons_x [k] = mons_x [i];
        mons_y [k] = mons_y [i];
}
mons_x [k] += random2(3) - 1;
mons_y [k] += random2(3) - 1;

if (grid [mons_x [k]] [mons_y [k]] == 17) passout = 1;
}


grid [mons_x [k]] [mons_y [k]] += 50;


} // end of summon_demon()
*/


void dragon(void)
{

brek = 1;
viewwindow();
//if (show [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] != 0)
//{
move_x = 0; move_y = 0;
/*
if (mons_targ_1_x [i] == mons_x [i])
{
if (mons_targ_1_y [i] < mons_y [i]) move_y = -1; else move_y = 1;
}

if (mons_targ_1_y [i] == mons_y [i])
{
if (mons_targ_1_x [i] < mons_x [i]) move_x = -1; else move_x = 1;
}

if (mons_targ_1_x [i] - mons_x [i] == mons_targ_1_y [i] - mons_y [i])
{
if (mons_targ_1_y [i] < mons_y [i]) move_y = -1; else move_y = 1;
if (mons_targ_1_y [i] == mons_y [i]) move_y = 0;

if (mons_targ_1_x [i] < mons_x [i]) move_x = -1; else move_x = 1;
if (mons_targ_1_x [i] == mons_x [i]) move_x = 0;
}
*/

move_x = trac_targ_x - mons_x [i];
move_y = trac_targ_y - mons_y [i];



if (move_x != 0 | move_y != 0)
{
        beam_range = random2(10) + 5;
        //beam_type = inv_class [zap_device_2];

        beam_source_x = mons_x [i]; beam_source_y = mons_y [i];

        //if (inv_type [zap_device_2] == 0)
        //{
                strcpy(beam_name, monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 4)); //gmon_name [mons_class [i]]);
  switch(mons_class [i])
  {
  case 73:
  case 29:
        strcat(beam_name, "'s blast of flame");
  beam_flavour = 2;
  beam_colour = RED;
  break;
  case 75:
  strcat(beam_name, "'s blast of cold");
  beam_flavour = 3;
  beam_colour = WHITE;
  break;
  }

                beam_damage = 100 + mons_HD [i];
//              beam_colour = 4;
                beam_type = 35; // hash #
//              beam_flavour = 2; // fire
  beam_hit = 30;

      //        strcpy(info [info_lines], "The ");
  if (mons_near(i) == 1)
                {
                strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
                strcat(info [info_lines], " breathes.");
                incrl();
  }
        //viewwindow();
        thing_thrown = 2;
        beam(); //break;
//}
}
} // end of dragon();




void mons_throw(void)
{

char shoot = 0;

if (mons_x [i] > x_pos - 6 && mons_x [i] < x_pos + 6 && mons_y [i] > y_pos - 6 && mons_y [i] < y_pos + 6)
{
//brek = 0;
//viewwindow();
if (show [mons_x [i] - x_pos + 6] [mons_y [i] - y_pos + 6] != 0)
{
move_x = 0; move_y = 0;
/*
if (mons_targ_1_x [i] == mons_x [i])
{
if (mons_targ_1_y [i] < mons_y [i]) move_y = -1; else move_y = 1;
}

if (mons_targ_1_y [i] == mons_y [i])
{
if (mons_targ_1_x [i] < mons_x [i]) move_x = -1; else move_x = 1;
}

if (mons_targ_1_x [i] - mons_x [i] == mons_targ_1_y [i] - mons_y [i])
{
if (mons_targ_1_y [i] < mons_y [i]) move_y = -1; else move_y = 1;
if (mons_targ_1_y [i] == mons_y [i]) move_y = 0;

if (mons_targ_1_x [i] < mons_x [i]) move_x = -1; else move_x = 1;
if (mons_targ_1_x [i] == mons_x [i]) move_x = 0;
}
*/


move_x = trac_targ_x - mons_x [i];
move_y = trac_targ_y - mons_y [i];

if (move_x != 0 | move_y != 0)
{
        beam_type = 41;
        beam_source_x = mons_x [i];
        beam_source_y = mons_y [i];
        beam_colour = icolour [hand_used];
 beam_flavour = 0;
                //strcpy(beam_name, fake_name [mons_inv [i] [1]]);//item_type [mons_inv [i] [1]]]);
        //if (inv_class [throw_2] == 0 | inv_class [throw_2] == 1)
        beam_damage = property [item_class [hand_used]] [item_type [hand_used]] [0] + item_plus [hand_used] - 50;
        //[item_class [throw_2]] [inv_type [throw_2]] [0];
        // else beam_damage = 10; // later put mass thing here.
        //beam_hit = 20 + mons_HD [i] * 3;  //strength + (0.5 * dex) + (f_abil);
 beam_hit = 7;
        beam_hit += item_plus [hand_used] - 50;

        //if (inv_class [item_wielded] == 0) mons_to_hit += inv_plus [item_wielded];
        //beam_hit *= random2(200) / 100;

        thing_thrown = 2;
//      strcpy(info [info_lines], "The ");

        if (mons_class [i] == 12)
        {
                strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
                strcat(info [info_lines], " flicks its tail!");
                beam_damage = 10;
                beam_hit *= 2;
        }





        if (mons_class [i] != 12)
        {

        if (item_class [mons_inv [i] [0]] == 0 && item_type [mons_inv [i] [0]] == item_type [mons_inv [i] [1]] + 13 && item_type [mons_inv [i] [1]] <= 16)
        {

                beam_damage = property [item_class [mons_inv [i] [1]]]    [item_type [mons_inv [i] [1]]]    [1];

                int hoggl = item_plus [mons_inv [i] [0]] - 50;

                if (item_plus [mons_inv [i] [0]] > 80) hoggl -= 100;

                //damage_done += inv_plus [item_wielded];

                //hoggl = (hoggl % 10);

                beam_damage += hoggl;




                int ghoggl = item_plus [mons_inv [i] [0]] - 50;

                if (item_plus [mons_inv [i] [0]] > 80) ghoggl -= 100;

                //damage_done += inv_plus [item_wielded];

                //ghoggl = (ghoggl / 10);

                beam_hit += ghoggl;

                beam_range += 3;


//              beam_damage += inv_plus [item_wielded];
//              beam_hit += inv_plus [item_wielded];

                if (item_plus [mons_inv [i] [0]] > 80)
                {
                        beam_damage -= 100;
                        beam_hit -= 100;
                }
                shoot = 1;
                /*strcpy(info [info_lines], "You shoot ");
                item_name(inv_class [throw_2], inv_type [throw_2], inv_dam [throw_2], inv_plus [throw_2], 1, inv_ident [throw_2], 3, str_pass);
                strcat(info [info_lines], str_pass);
                strcat(info [info_lines], ".");
                incrl();*/
        }


                strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);

                if (shoot == 1)
                {
                        strcat(info [info_lines], " shoots ");
                } else strcat(info [info_lines], " throws ");//a");

                item_name(item_class [hand_used], item_type [hand_used], item_dam [hand_used], item_plus [hand_used], 1, item_ident [hand_used], 3, str_pass);
                strcat(info [info_lines], str_pass);
        /*
                if (fake_name [hand_used] [0] == 97 ^ fake_name [hand_used] [0] == 101 ^ fake_name [hand_used] [0] == 105 ^ fake_name [hand_used] [0] == 111 ^ fake_name [hand_used] [0] == 117)
                strcat(info [info_lines], "n");
                strcat(info [info_lines], " ");
                strcat(info [info_lines], fake_name [hand_used]);
        */
                strcat(info [info_lines], ".");

        }

        incrl();
        missile();
        item_quant [hand_used] --;
        if (item_quant [hand_used] == 0)
        {
        mons_inv [i] [1] = 501;
        it_no --;
        }
        brek = 1;
        //if (item_quant [mons_inv [i] [1]] == 0) inv_no --;
        //move_x = 0; move_y = 0;
}
}
}

if (mons_class [i] == 2) mons_speed_inc [i] -= 10;


}









/*


void load(void)
{

switch (your_level)
{
        case 0: ifstream f1 ("lev000.dat");
        case 1: ifstream f1 ("lev001.dat");
        case 2: ifstream f1 ("lev002.dat");
        case 3: ifstream f1 ("lev003.dat");
        case 4: ifstream f1 ("lev004.dat");
}

if (!f1) cerr << "Cannot open level file.";

for (count_x = 0; count_x < 140; count_x ++)
{
        for (count_y = 0; count_y < 140; count_y ++)
        {
                f1.get(grid [count_x] [count_y]);
        }
}


} // end of void load();


*/

void level_change(void) // Look at this !!!!
{
int prtry = 0;
int prtry2 = 0;
int prtry3 = 0;

//while (xp > (20 * ((xl * 0.6) * (xl * 0.7)) + 10) && xl < 27)
while (xp > exp_needed(xl + 2) && xl < 27) //(20 * xl * xl)) // && xl < 27)
//while(xp > exp_needed(xl))
{
        xl++;
        strcpy(info [info_lines], "You are now a level ");
        itoa(xl, temp_quant, 10);
        strcat(info [info_lines], temp_quant);
        strcat(info [info_lines], " ");
        strcat(info [info_lines], clasnam);
        strcat(info [info_lines], "!");
        incrl();
//      relay_message();
        more();
 scrloc = 0;
switch(clas)
{
case 0: // fighter
        brek = random2(5) + 4;
        hp += brek;
        hp_max += brek;
        brek = 1; //random2(3);
        ep += brek;
        ep_max += brek;
        f_abil += 10; // 15
        mag_abil += 3;
 thr_abil += 5;
 res_magic += 3;
 spell_levels++;
//      speed -= .02;                         No change in speed???
break;

case 1: // wizard
        brek = random2(4) + 3;
        hp += brek;
        hp_max += brek;
        brek = random2(2) + 2;
        ep += brek;
        ep_max += brek;
        f_abil += 3;
        mag_abil += 7;
 thr_abil += 2;
 res_magic += 5;
 spell_levels += 5 - brek;
//      speed -= .02;
break;

case 3: // thief
        brek = random2(4) + 4;
        hp += brek;
        hp_max += brek;
        brek = 1 + random2(2); //random2(3);
        ep += brek;
        ep_max += brek;
        f_abil += 7; // 15
        mag_abil += 4;
 thr_abil += 8;
 res_magic += 3;
 spell_levels++;
 if (xl > max_level && xl % 3 == 0) evasion ++;
break;

case 2: // priest
        brek = random2(4) + 4;
        hp += brek;
        hp_max += brek;
        brek = random2(2) + 1;
        ep += brek;
        ep_max += brek;
        f_abil += 6; // 15
        mag_abil += 5;
 thr_abil += 1;
 res_magic += 5;
 spell_levels += 1;
if (xl > max_level && xl % 3 == 0)
{
 if (spell_no >= 21)
 {
  goto infuse;
 }
prtry2 = xl / 3;
if (xl > 12) prtry2 = (xl - 12) / 2 + 4;
switchy : switch(prtry2)
{
 case 1: prtry = 38; break; // lesser healing
 case 2: prtry = 46; break; // repel undead
 case 3: prtry = 65; break; // heal other
 case 4: prtry = 41; break; // purification
 case 5: prtry = 39; break; // greater healing
 case 6: prtry = 45; break; // smiting
 case 7: prtry = 47; break; // holy word
 default: goto infuse;
}
for (prtry3 = 0; prtry3 < 25; prtry3 ++)
{
 if (spells [prtry3] == prtry)
 {
  prtry2++;
  if (prtry2 >= 7)
  {
    infuse : strcpy(info [info_lines], "You have been granted an infusion of magical power!");
    incrl();
    spell_levels += 1 + random2(4);
    goto get_out;
  }
  goto switchy;
 }
}
for (prtry3 = 0; prtry3 < 25; prtry3 ++)
{
 if (spells [prtry3] == -1)
 {
  spells [prtry3] = prtry;
  spell_no ++;
  break;
 }
}
strcpy(info [info_lines], "You have been granted a spell!");
incrl();
}
get_out : break;


}

 if (xl > max_level) max_level = xl;

// NEED to look at this! =oregeneration uses addition - can't mix!


if (xl < 5) rate_regen += 2;
if (xl > 4 && xl <= 10 && xl % 2 == 0) rate_regen += 2;
if (xl > 10 && xl % 2 == 0) rate_regen ++;
//if (xl > 4 && xl < 11) rate_regen += 1;
//if (xl > 10) rate_regen += 1;


xl_ch = 1;
/*
if (xp > (20 * pow(xl, 2)))
{
        xp = (20 * pow(xl, 2)) - 1;
}
*/
hp_ch = 1; ep_ch = 1;

}

//if (xl_ch == 1)
//{
// more();
// scrloc = 0;
//}

xp_ch = 1;

}







void noisy(void) // x_plos & y_plos must have values for this function.
{

// remember the other noisy in newview.h

for (p = 0; p < MNST; p ++)
{
        if (mons_x [p] >= x_plos - 20 && mons_x [p] <= x_plos + 20 && mons_y [p] >= y_plos - 20 && mons_y [p] <= y_plos + 20)
        {
        if (mons_beh [p] == 0) // && random2(3) < 2 // and the monster can hear you.
                mons_beh [p] = 1;

      mons_targ_1_x [p] = x_pos;
      mons_targ_1_y [p] = y_pos;

   }


}

} // end of void noisy()




void direction(char rnge)
{

char ink = 0;

nothing = dir_cursor(rnge);

if (nothing > 50000)
{
        nothing -= 50000;
        aim_down = 1;
}

if (nothing > 10000)
{
        nothing -= 10000;
        ink = 1;
}

if (nothing == -1)
{
        strcpy(info [info_lines], "What an unusual direction.");
        incrl();
        return;
}

if (nothing == 253)
{
        if (prev_targ == 500)
        {
                strcpy(info [info_lines], "You haven't got a target.");
                incrl();
                nothing = -1;
                return;
        }

        if (mons_near(prev_targ) != 1 | (mons_ench [prev_targ] [2] == 6 && see_invis == 0))
        {
                strcpy(info [info_lines], "You can't see that creature any more.");
                incrl();
                nothing = -1;
                return;
        }

        move_x = mons_x [prev_targ] - x_pos;
        move_y = mons_y [prev_targ] - y_pos;
//      move_x = ;
//      move_y = ;
        target_x = move_x + x_pos;
        target_y = move_y + y_pos;
        return;
}

if (nothing == 254)
{
//      strcpy(info [info_lines], "I'm afraid your arm isn't that long.");
//      incrl();
        return;
}


move_x = ((int) (nothing / 100)) - 7;
move_y = nothing - (((int) nothing / 100) * 100) - 7;

target_x = move_x + x_pos;
target_y = move_y + y_pos;

if (ink == 1)
{
        target_x = 1;
        target_y = 1;
}


if (move_x == 0)
{
        if (move_y > 0) move_y = 1;
        if (move_y < 0) move_y = -1;
}

if (move_y == 0)
{
        if (move_x > 0) move_x = 1;
        if (move_x < 0) move_x = -1;
}

if (move_x == move_y | move_x * -1 == move_y)
{
        if (move_y > 0) move_y = 1;
        if (move_y < 0) move_y = -1;
        if (move_x > 0) move_x = 1;
        if (move_x < 0) move_x = -1;
}

        int v;

        if (grid [target_x] [target_y] >= 50)
        {

                for (v = 0; v < MNST; v++)
                {
                        if (mons_x [v] == target_x && mons_y [v] == target_y && rnge != 100)
                        {
                                prev_targ = v; break;
                        }
                }
        }

} // end of direction()



void stethoscope(void)
{
// STETH can't examine spaces in cardinal directions more than 1 space from you


char eggplant;

strcpy(info [info_lines], "Which monster?");
incrl();
// test relay_message();

direction(1);

if (nothing == -1) return;

/*
itoa(grid [x_pos + move_x] [y_pos + move_y], st_prn, 10);
//strcat(info [info_lines], st_prn);
//strcat(info [info_lines], "/");
//itoa(mons_hp [i], st_prn, 10);
strcpy(info [info_lines], st_prn);
incrl();


        move_x = 0;
        move_y = 0;
        return;

//return;
*/

if (grid [x_pos + move_x] [y_pos + move_y] <= 50)
{
//      strcpy(info [info_lines], "There isn't anything there.!");









itoa((int) item_dam [igrid [x_pos + move_x] [y_pos + move_y]], st_prn, 10);
strcpy(info [info_lines], st_prn);
//strcat(info [info_lines], "/");
//itoa(mons_hp [i], st_prn, 10);
//strcpy(info [info_lines], st_prn);
incrl();






        //incrl();
        move_x = 0;
        move_y = 0;
        return;
}

for (i = 0; i < MNST; i++)
{
        if (mons_x [i] == x_pos + move_x && mons_y [i] == y_pos + move_y && mons_class [i] != -1)
                break;
}

strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
strcat(info [info_lines], ": ");

//monster_blink(i);

//for (j = 0; j < 8; j ++)
{
/*
itoa(mons_inv [i] [j], st_prn, 10);
strcpy(info [info_lines], st_prn);
strcat(info [info_lines], ",");
//itoa(mons_ev [i], st_prn, 10);
//strcat(info [info_lines], st_prn);
incrl();
/*/
//itoa(10 - mons_AC [i], st_prn, 10);


itoa(i, st_prn, 10);
strcat(info [info_lines], st_prn);
strcat(info [info_lines], ":");

itoa(mons_hp [i], st_prn, 10);
strcat(info [info_lines], st_prn);
strcat(info [info_lines], "/");
itoa(mons_hp_max [i], st_prn, 10);
strcat(info [info_lines], st_prn);
strcat(info [info_lines], "   ");

itoa(mons_beh [i], st_prn, 10);
strcat(info [info_lines], st_prn);
strcat(info [info_lines], "/");
itoa(mons_hit [i], st_prn, 10);
strcat(info [info_lines], st_prn);
incrl();

strcpy(info [info_lines], "Speed:");
itoa(mons_speed [i], st_prn, 10);
strcat(info [info_lines], st_prn);
strcat(info [info_lines], "/");
itoa(mons_speed_inc [i], st_prn, 10);
strcat(info [info_lines], st_prn);
incrl();




//eggplant = mons_near((unsigned char) i);

//if (eggplant == 0) strcat(info [info_lines], "0"); else strcat(info [info_lines], "1");
//incrl();




//strcat(info [info_lines], (char) eggplant);
}
move_x = 0;
move_y = 0;

return;

} // end of stethoscope()


/*
void relay_message(void)
{
//char eggl [5];

        if (info_lines > 0) running = 0;
        message(info, info_lines);
        info_lines = 0;
}
*/



void quit_game(void)
{

strcpy(info [info_lines], "Really quit?");
incrl();

// test relay_message();

keyin = get_ch();

if (keyin == 'y' | keyin == 'Y')
{
ouch(-9999, 0, 13);

//      end_game(0);
/*
        clrscr();
        window(1,1,80,25);
        clrscr();
        cprintf("Goodbye, ");
        cprintf(your_name);
        cprintf(".");
        cprintf("\n\n\r");
        exit(0);
*/
}

} // end of void quit_game




void version(void)
{
strcpy(info [info_lines], "This is Dungeon Crawl v1.05. (Last build 7/10/97)");
incrl();
}








void explosion1(void)
{

if (stricmp(beam_name, "hellfire") == 0)
{
        strcpy(info [info_lines], "The hellfire explodes!");
        incrl();
//      more();
        beam_type = 43;
        beam_damage = 120;
        beam_flavour = 13; // hellfire
}


if (stricmp(beam_name, "fireball") == 0)
{
        strcpy(info [info_lines], "The fireball explodes!");
        incrl();
//      more();
        beam_type = 43;
        beam_damage = 110;
        beam_flavour = 2;
}


explosion();

}









void explosion(void)
{

char clouty = 0;

hurted = 0;

//float beam_tot = 0; // total increments
//char beam_inc = 0; // move_x / move_y
//int beam_sign_x = 0;
//int beam_sign_y = 0;
//int bmox = 0;
//float bmoy = 0;
//int bmax = 0;
//float bmay = 0;
//int beam_x = beam_source_x;
//int beam_y = beam_source_y;

//int beam_x;
//int beam_y;

beam_source_x = beam_x;
beam_source_y = beam_y;



        brek = 1;
//      viewwindow(); // this resets the window so that dragons don't appear to do weird things.
        brek = 0;

int q = 0;

for (n = -1; n < 2; n++) // x
{

for (q = -1; q < 2; q++) // y
{

beam_x = beam_source_x + n;
beam_y = beam_source_y + q;


        if (grid [beam_x] [beam_y] < 10) continue;

        if (beam_x > x_pos - 7 && beam_x < x_pos + 7 && beam_y > y_pos - 7 && beam_y < y_pos + 7 && (show [beam_x - x_pos + 7] [beam_y - y_pos + 7] != 0 | (beam_x == x_pos && beam_y == y_pos)))// && beam_type != 0)
        {
                //viewwindow;
                window(2,2,34,15);
                textcolor(beam_colour);
                //show [beam_x - x_pos + 7] [beam_y - y_pos + 7] = 35;
                gotoxy(beam_x - x_pos + 17, beam_y - y_pos + 8);
                putch(35);
        }

}
}

//delay(500);
// test relay_message();

scrloc = 0;
more();

for (n = -1; n < 2; n++) // x
{

for (q = -1; q < 2; q++) // y
{

beam_x = beam_source_x + n;
beam_y = beam_source_y + q;




//      }

if (grid [beam_x] [beam_y] < 10) continue;


if (beam_x == x_pos && beam_y == y_pos) // && beam_colour != 6) // have to do something about enchantments here.
{                                                       // ^^^ digging

        if (beam_colour == 200)
        {
                tracer = 1;
                return;
        }



        strcpy(info [info_lines], "The ");
        strcat(info [info_lines], beam_name);
        strcat(info [info_lines], " engulfs you");
//      incrl();

        hurted = 0;

        if (beam_damage > 100)
        {
                hurted += random2(beam_damage - 100);
                hurted += random2(beam_damage - 100);
                hurted += random2(beam_damage - 100);
        } else hurted += random2(beam_damage);

        check_your_resists();

        /*
        switch(beam_flavour)
        {
                case 2: // fire
                if (res_fire > 0)
                {
                        strcat(info [info_lines], "... But you don't get burnt");
                        incrl();
                        n++;
                        continue;
                }
                break;

                case 3: // cold
                if (res_cold > 0)
                {
                        strcat(info [info_lines], "... But you don't feel cold");
                        incrl();
                        n++;
                        continue;
                }
                break;

                case 13: // hellfire
                if (res_fire > 0)
                {
                        strcat(info [info_lines], "... But you partially resist it");
                        incrl();
                        n++;
                        hurted /= 2;
                }
                break;
        }
        */
        strcat(info [info_lines], "!");
//      incrl();

 if (beam_flavour == 2 | stricmp(beam_name, "hellfire") == 0) scrolls_burn(5, 6);
 if (beam_flavour == 3) scrolls_burn(5, 8);
 if (beam_flavour == 11) scrolls_burn(2, 4); // spores!

        ouch(hurted, 0, 3);

        hp_ch = 1;

        //n++; // reduces beam's range
}


if (grid [beam_x] [beam_y] >= 100) // hit a cloud
{

//if (beam_colour == 200 | beam_name [0] == 48) goto out_of_cloud_bit;

clouty = which_cloud(beam_x, beam_y);

//1 = fire, 3 = cold, 2 = stinking

if ((cloud_type [clouty] == 3 && beam_flavour == 2) | (cloud_type [clouty] == 1 && beam_flavour == 3))
{
 if (see_grid(beam_x, beam_y) == 1)
 {
  strcpy(info [info_lines], "You hear a sizzling sound!");
  incrl();
 }

   cloud_type [clouty] = 0;
   grid [cloud_x [clouty]] [cloud_y [clouty]] -= 100;
   cloud_decay [clouty] = 0;
   cloud_no--;

//   n -= 3;

}
}




if (grid [beam_x] [beam_y] % 100 >= 50)
{
/*
if (beam_colour == 200)
{
        tracer = 0; // later, have tracer = number of monster it hits.
        return;
}
*/


for (o = 0; o < MNST; o++)
{
        if (mons_x [o] == beam_x && mons_y [o] == beam_y && mons_class [o] != -1 && beam_name [0] != 48)
        {

  if (mons_class [o] >= MLAVA0 && mons_sec [o] == 1) continue;

                hurted = 0;

                if (beam_damage > 100)
                {
                        hurted += random2(beam_damage - 100);
                        hurted += random2(beam_damage - 100);
                        hurted += random2(beam_damage - 100);
                } else hurted += random2(beam_damage);

                strcpy(info [info_lines], "The ");
                strcat(info [info_lines], beam_name);
                strcat(info [info_lines], " engulfs ");
                strcat(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 1)); //gmon_name [mons_class [o]]);
                strcat(info [info_lines], ".");
                incrl();

                check_mons_resists();

                /*
                switch(beam_flavour)
                {
                        case 2:
                        if (mons_res_fire(mons_class [o]) > 0)
                        {
                                strcpy(info [info_lines], monam (mons_class [o], mons_ench [o] [2], 1));
                                strcat(info [info_lines], " appears unharmed.");
                                incrl();
                                hurted = 0;
                        }
                        break;

                        case 13:
                        if (mons_res_fire(mons_class [o]) == 2)
                        {
                                strcpy(info [info_lines], monam (mons_class [o], mons_ench [o] [2], 1));
                                strcat(info [info_lines], " appears unharmed.");
                                incrl();
                                hurted = 0;
                        }
                        if (mons_res_fire(mons_class [o]) == 1)
                        {
                                strcpy(info [info_lines], monam (mons_class [o], mons_ench [o] [2], 1));
                                strcat(info [info_lines], " partially resists.");
                                incrl();
                                hurted /= 2;
                        }
                        break;

                }
                */


                mons_hp [o] -= hurted;


//              if (invis != 0 && mons_beh [o] == 0) mons_beh [o] = 1;

  mons_beh [o] = 1;

                if (mons_hp [o] <= 0)
                {
                switch(thing_thrown)
                {
                        case 1: killer = 3; break; // your explosion
                        case 3: killer = 3; break; //  "    "
                        case 2: killer = 4; break; // someone else's

                }
                monster_die(o);
                }
                //n++;
        }


} // end of for o

}

} // end of for q
} // end of for n
/*
if (beam_colour == 200)
{
        tracer = 0;
        return;
}



if (beam_name [0] == 48 && beam_colour != 6)
{
        strcpy(info [info_lines], "Nothing appears to happen.");
        incrl();
}
*/

//}  // end of void beam(void);


//more();






} // end of void explosion


void spore_goes_pop(void) // should really do something about mons_hit, but can't be bothered
{
        //incrl();
if (mons_near(i) == 1)
{
 brek = 1;
        viewwindow();
        strcpy(info [info_lines], "The giant spore explodes!");
        incrl();
}
//      more();
        beam_type = 43;
        beam_damage = 115;
        beam_flavour = 11; // <=- not sure about this
        beam_x = mons_x [i];
        beam_y = mons_y [i];
        strcpy(beam_name, "explosion of spores");
        beam_colour = LIGHTGRAY;
        thing_thrown = 2; // someone else's explosion
        explosion();
}




void incrl(void)
{

info_lines++;
scrloc++;
scruggle = 1;

relay_message();
//char oggy;

//incr();

//oggy = incrline();
/*
if (info_lines >= 75)
{
relay_message();
        more();
        info_lines = 0;
        //zeroinf();
}
*/
return;
}

// Blackadder curse: onion falls on head.

void check_links(void)
{
window(40,1,80,25);
gotoxy(1,1);
/*
for (i = 0; i < ITEMS; i++)
{
        if (item_x [i] == x_pos && item_y [i] == y_pos)
        {
//              cout << i << "," << item_link [i] << " ; ";
        }

}
*/

}





void cast_a_spell(void)
{

char spc = 0;
char spc2 = 0;
int spellh = 0;
int powc = magic_ability(mag_abil, intel);

if (spell_no == 0)
{
        strcpy(info [info_lines], "You don't know any spells.");
        incrl();
        return;
}

if (berserker != 0)
{
 strcpy(info [info_lines], "You are too berserk!");
 incrl();
 return;
}

query : strcpy(info [info_lines], "Cast which spell?");
incrl();

// test relay_message();

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        //inventory(0);
        char unthing = spell_list(spells); //invent(0, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);

//      cout << (int) unthing;

//        unthing = getch();

        if (unthing == 2) return; //27) return;

        if ((unthing >= 65 && unthing <= 90) | (unthing >= 97 && unthing <= 122))
        {
                keyin = unthing;
        } else
        {
                goto query;
        }
}

spc = (int) keyin;

//if (spc < 65 | (spc > 90 && spc < 97) | spc > 122)
if (spc < 97 | spc > 121)
{
        unknown : strcpy(info [info_lines], "You don't know that spell.");
        incrl();
        return;
}

spc2 = conv_lett(spc);

if (spells [spc2] == -1)
{
        goto unknown;
}

if (spell_value(spells [spc2]) > ep)
{
        strcpy(info [info_lines], "You don't have enough magic to cast that spell.");
        incrl();
        return;
}

if (hunger <= spell_hunger(spell_value(spells [spc2]), spells [spc2]) | hunger <= 500)
{
 strcpy(info [info_lines], "You don't have the energy to cast that spell.");
 incrl();
 turnover = 1; // ??
 return;
}

ep -= spell_value(spells [spc2]);
ep_ch = 1;

//hunger -= spell_value(spells [spc2]) * 100;

if (energy == 0)
{
 spellh = spell_hunger(spell_value(spells [spc2]), spells [spc2]);
 spellh -= magic_ability(mag_abil, intel);
 if (spellh < 10) spellh = 10;
 hunger -= spellh;
 hung_ch = 1;
 how_hungered(spellh);
 //food_change();
}

thing_thrown = 3;

turnover = 1;
alert();

if (conf != 0)
{
 random_uselessness(2 + random2(7));
 return;
}

your_spells(spells [spc2], powc);

turnover = 1;


} // end of void cast_a_spell


void your_spells(int spc2, int powc)
{

switch(spc2)
{
        case 0: identify(0); return; // will only ever id one item
        case 1: you_teleport(); return;
        case 2: mass_enchantment(4, powc); return;
        case 3:
                strcpy(info [info_lines], "You hear a voice call your name!");
                incrl();
                x_plos = x_pos;
                y_plos = y_pos;
                noisy();
                break;
        case 4: remove_curse(); return;

        case 5:
 if (spell_direction() == -1) return;
 zapping(4, powc);
 move_x = 0; move_y = 0;
 return; // magic missile

        case 6:
 powc *= spec_fire + 1;
 powc /= spec_cold + 1;
 fireball(1, powc); return;

   case 13: conjure_flame(); return;

   case 14:
   if (spell_direction() == -1) return;
   if (move_x == 0 && move_y == 0)
   {
    strcpy(info [info_lines], "Why would you want to do that?");
    incrl();
    return;
   }
   zapping(11, powc);
   move_x = 0; move_y = 0;
   return; // dig

   case 15: if (spell_direction() == -1) return;
   powc *= spec_fire + 1;
   powc /= spec_cold + 1;
   zapping(7, powc);
   move_x = 0; move_y = 0;
   return; // fire bolt

   case 16: if (spell_direction() == -1) return;
   powc *= spec_cold + 1;
   powc /= spec_fire + 1;
   zapping(8, powc);
   move_x = 0; move_y = 0;
   return; // frost bolt


   case 17: if (spell_direction() == -1) return;
   zapping(14, powc);
   move_x = 0; move_y = 0;
   return; // lightning bolt


   case 20: if (spell_direction() == -1) return;
   if (move_x == 0 && move_y == 0)
   {
    strcpy(info [info_lines], "Sorry, it doesn't work like that.");
    incrl();
    return;
   }
   zapping(15, powc);
   move_x = 0; move_y = 0;
   return; // polymorph

   case 21: if (spell_direction() == -1) return;
   zapping(2, powc);
   move_x = 0; move_y = 0;
   return; // slow

   case 22: if (spell_direction() == -1) return;
   zapping(3, powc);
   move_x = 0; move_y = 0;
   return; // haste

   case 23: if (spell_direction() == -1) return;
   zapping(6, powc);
   move_x = 0; move_y = 0;
   return; // paralyze

   case 24: if (spell_direction() == -1) return;
   zapping(9, powc);
   move_x = 0; move_y = 0;
   return; // confuse

   case 25: if (spell_direction() == -1) return;
   zapping(10, powc);
   move_x = 0; move_y = 0;
   return; // invis


   case 26: if (spell_direction() == -1) return;
   powc *= spec_fire + 1;
   powc /= spec_cold + 1;
   zapping(0, powc);
   move_x = 0; move_y = 0;
   return; // throw flame

   case 27: if (spell_direction() == -1) return;
   powc *= spec_cold + 1;
   powc /= spec_fire + 1;
   zapping(1, powc);
   move_x = 0; move_y = 0;
   return; // throw frost

   case 28:
   blink();
   return;

   case 29:
   powc *= spec_cold + 1;
   powc /= spec_fire + 1;
   cast_big_c(powc, 3);
   return;

   case 30:
   stinking_cloud();
   return;

   case 31:
   powc *= spec_fire + 1;
   powc /= spec_cold + 1;
   cast_ring_of_flames(powc);
   return;

   case 32:
   restore_str();
   return;

   case 33:
   restore_int();
   return;

   case 34:
   restore_dex();
   return;

   case 35: if (spell_direction() == -1) return;
   powc *= spec_poison + 1;
   zapping(16, powc);
   move_x = 0; move_y = 0;
   return; // venom bolt

   case 36: cast_toxic_radiance();
   return;

   case 37: if (spell_direction() == -1) return;
   if (move_x == 0 && move_y == 0)
   {
    strcpy(info [info_lines], "Sorry, it doesn't work like that.");
    incrl();
    return;
   }
   zapping(13, powc);
   move_x = 0; move_y = 0;
   break; // teleport creature (I think)

   case 38:
   cast_lesser_healing(powc);
   break;

   case 39:
   cast_greater_healing(powc);
   break;

   case 40:
   powc *= spec_poison + 1;
   cast_cure_poison(powc);
   break;

   case 41:
   purification(powc);
   break;

   case 42:
   cast_deaths_door(powc);
   break;

   case 43:
   cast_selective_amnesia();
   break;

   case 44: // mass conf
   mass_enchantment(5, powc);
   break;

   case 45: // smite
   cast_smiting(powc);
   break;

   case 46: // repel undead
   turn_undead(50);
   break;

   case 47: // holy word
   holy_word(50);
   conf += random2(3) + 2;
   break;

   case 48:
   detect_curse();
   break;

   case 49:
   summon_small_mammals(powc);
   break;

   case 50:
   abjuration(powc);
   break;

   case 51:
   summon_scorpions(powc);
   break;

   case 52: // levitation
   potion_effect(7, 40);
   break;

   case 53:  if (spell_direction() == -1) return;
   zapping(17, powc);
   move_x = 0; move_y = 0;
   return; // bolt of draining

   case 54: if (spell_direction() == -1) return;
   zapping(18, powc);
   move_x = 0; move_y = 0;
   return; // spray of metal

   case 55: if (spell_direction() == -1) return;
   zapping(19, powc);
   move_x = 0; move_y = 0;
   return; // bolt of inaccuracy

   case 56: // poison cloud
   powc *= spec_poison + 1;
   cast_big_c(powc, 4);
   return;

   case 57: // fire storm
   powc *= spec_fire + 1;
   powc /= spec_cold + 1;
   cast_fire_storm(powc);
   move_x = 0; move_y = 0;
   return;

   case 58: // detect_traps
   if (detect_traps() > 0)
                        {
        strcpy(info [info_lines], "You detect some traps!");
                        } else strcpy(info [info_lines], "You detect nothing.");
                        incrl();
                        return;

   case 59: // random blink
   random_blink();
   return;

   case 60: // Mystic blast
   if (spell_direction() == -1) return;
   zapping(20, powc);
   move_x = 0; move_y = 0;
                        return;

   case 61: // Swarm
   summon_swarm(powc);
                        return;

   case 62: // Summon Things
   if (sust_abil != 0 | intel == 3)
                        {
        strcpy(info [info_lines], "Your call goes unanswered.");
                                incrl();
    return;
                        }
   summon_things(powc);
                        return;

   case 63: if (spell_direction() == -1) return;
   zapping(21, powc);
   move_x = 0; move_y = 0;
   return; // enslavement

   case 64: // magic mapping
   if (level_type == 1)
   {
    strcpy(info [info_lines], "You feel momentarily disoriented.");
    incrl();
    return;
   }
   strcpy(info [info_lines], "You feel aware of your surroundings.");
   incrl();
   magic_mapping(grid, map, x_pos, y_pos, (powc / 3 + 5));
   return;

   case 65: // heal other
   if (spell_direction() == -1) return;
   if (move_x == 0 && move_y == 0)
   {
    strcpy(info [info_lines], "Sorry, it doesn't work like that.");
    incrl();
    return;
   }
   zapping(5, powc);
   move_x = 0; move_y = 0;
   return;

default:
strcpy(info [info_lines], "Invalid spell!");
incrl();
break;
} // end switch



} // end spell_effects



void memorise_spell(void)
{




int id_used = 0;
unsigned char nthing;

if (conf != 0)
{
        strcpy(info [info_lines], "You're too confused to memorise anything!");
        incrl();
        return;
}

if (inv_no == 0)
        {
        strcpy(info [info_lines], "You aren't carrying anything.");
        incrl();
        return;
        }

query : strcpy(info [info_lines], "Memorise from what?");
incrl();

// test relay_message();

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = invent(6, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if (keyin == '*') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
                //strcpy(info [info_lines], "Mem?"); incrl();
                //relay_message();
                //keyin = get_ch();
                goto query;
        }
/*
        inventory(6);
        strcpy(info [info_lines], "Read which item?"); incrl();
        relay_message();
        keyin = get_ch();
*/
}
/*
if (keyin == '*')
{
        inventory(-1);
        strcpy(info [info_lines], "Read which item?"); incrl();
        relay_message();
        keyin = get_ch();
}
*/
sc_read_1 = (int) keyin;

if (sc_read_1 < 65 | (sc_read_1 > 90 && sc_read_1 < 97) | sc_read_1 > 122)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

sc_read_2 = conv_lett(sc_read_1);

if (inv_quant [sc_read_2] == 0)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}
/*
if (inv_class [sc_read_2] == 5)
{
        strcpy(info [info_lines], "That's a book, not a scroll.");
        incrl();
        return;
}*/

if (inv_class [sc_read_2] != 6)
{
        strcpy(info [info_lines], "You can't read that!");
        incrl();
        return;
}

count_y = sc_read_2;

turnover = 1;
/*
if (ep_max <= spell_value(inv_type [sc_read_2]))
{
        strcpy(info [info_lines], "You don't have enough mana to memorise that spell!");
        incrl();
        turnover = 1;
        return;
}*/

for (i = 0; i < 25; i ++)
{
        if (spells [i] == inv_type [sc_read_2])
        {
                strcpy(info [info_lines], "You already know that spell!");
                incrl();
                return;
        }
}


//if (//learn_success() == 1)
{
        for (i = 0; i < 25; i ++)
        {
                if (spells [i] == -1) break;
        }
} /*else
{
        strcpy(info [info_lines], "You fail to memorize the spell.");
        incrl();
        return;
}*/

spells [i] = inv_type [sc_read_2];

inv_quant [sc_read_2]--;

if (item_wielded == sc_read_2 && inv_quant [sc_read_2] == 0)
{
        strcpy(info [info_lines], "You are now empty handed.");
        incrl();
        item_wielded = -1;
}

ep_max -= spell_value(spells [i]);
if (ep > ep_max) ep = ep_max;
ep_ch = 1;
ep_max_ch = 1;

/*inv_quant [sc_read_2] --;

if (inv_quant [sc_read_2] == 0)
{
        inv_no --;
        if (item_wielded == sc_read_2)
        {
                item_wielded = -1;
                strcpy(info [info_lines], "You are now empty handed.");
                incrl();
        }
}*/              // Maybe don't have the scroll destroyed?

turnover = 1;


}













void dart_trap(void)
{



                        if (random2(10) < 2)
                        {
                                strcpy(info [info_lines], "You avoid triggering a");
                                strcat(info [info_lines], beam_name);
                                strcat(info [info_lines], " trap.");
                                incrl();
                                return;
                        }
                        strcpy(info [info_lines], "A");
                        strcat(info [info_lines], beam_name);
                        strcat(info [info_lines], " shoots out and ");
                        //incrl();
   if (random2(50) < shield_class)
   {
    strcat(info [info_lines], "hits your shield.");
    incrl();
    goto out_of_trap;
   }

                        if ((20 + curr_level * 2) * random2(200) / 100 >= evasion + random2(dex) / 3 - 2)
                        {
                                damage_taken = random2(beam_damage);
                                damage_taken -= random2 (AC + 1);
                                if (damage_taken > 0) ouch(damage_taken, 0, 10);
//                              hp -= damage_taken;
//                              hp_ch = 1;
                        }
                        if (damage_taken > 0)
                        {
                                strcat(info [info_lines], "hits you!");
                                incrl();
                        } else
                        {
                                strcat(info [info_lines], "misses you.");
                                incrl();
                        }


                        out_of_trap : beam_x = x_pos;
                        beam_y = y_pos;

                        if (random2(2) != 0) itrap();


                        if (random2(10) + damage_taken < 7)
                        {

                        //trap_item(it_no, icolour, item_ident, item_dam, item_plus, item_type, item_class, item_link, item_quant, igrid, 1, 1, beam_x, beam_y);







/*                      for (i = 0; i < ITEMS; i++)
                        {
/*                              if (item_class [i] == 1 && item_type [i] == 0 && item_x [i] == x_pos && item_y [i] == y_pos && item_type [i] == 0 && item_plus [i] == 50 && item_dam [i] == 0)
                                {
//                              if (strcmp("dart", item_name(item_class [i], item_type [i], item_dam [i], item_plus [i], item_quant [i], item_ident [i], 3)) == 0);
                                /*if (item_ident [i] == 0)
                                {
                                        item_quant [i] += 1;
                                        return;
                                }//* /
                                }
                        }
*/
                        /*
                        for (i = 0; i < ITEMS; i++)
                        {
                                if (item_quant [i] == 0)
                                {
                                        //strcpy(fake_name [i], "dart");
                                        item_ident [i] = 0;
                                        item_class [i] = 1;
                                        item_type [i] = 0;
                                        item_plus [i] = 50;
                                        item_dam [i] = 0;
                                        icolour [i] = 11;
                                        item_x [i] = x_pos; item_y [i] = y_pos;
                                        item_quant [i] = 1;
                                        it_no ++;
                                        break;
                                }
                        } // end of i loop
                        */

                        //} // end of if random2(10) ...
//              } // end of switch
               //       } // end of if grid = 25
                } // end of if grid is a trap
                return; // This is to avoid going through the you_attack bit.
        }



//}








void itrap(void)
{

                        if (random2(10) > 6 && it_no > 300) return;

                        switch(trap_type [trapped])
                        {
                                case 0: //dart
                                beam_colour = 1; // class
                                beam_damage = 3; // type
                                break;

                                case 1: // arrow
                                beam_colour = 1;
                                beam_damage = 1;
                                break;

                                case 2: //spear
                                beam_colour = 0;
                                beam_damage = 11;
                                break;

                                case 3: // (hand) axe
                                beam_colour = 0;
                                beam_damage = 9;
                                break;

                                //default: abort();

                                default:
                                //beam_colour = 1;
                                //beam_damage = 3;
                                //cout << (int) trapped << "x" << (int) trap_type [trapped] << " ";
                                getch();
                                break;

                        }

                        //if (random2(10) < 7 && it_no < 300)
                        //{
                                it_no += trap_item(it_no, icolour, item_ident, item_dam, item_plus, item_type, item_class, item_link, item_quant, igrid, beam_colour, beam_damage, beam_x, beam_y);
                        //}



}






void it_name(int itn, char des)
{
                item_name(item_class [itn], item_type [itn], item_dam [itn], item_plus [itn], item_quant [itn], item_ident [itn], des, str_pass);
}

void in_name(int inn, char des)
{
                item_name(inv_class [inn], inv_type [inn], inv_dam [inn], inv_plus [inn], inv_quant [inn], inv_ident [inn], des, str_pass);
}
















char learn_a_spell(unsigned char splbook, char bitty)
{


char strungy [9];

unsigned int numbo;

int j;
int h;
int multip = 1;

int found_a_one = 0;

int icky = 0;


/*
      strcpy(info [info_lines], "Learn from which spellbook?");
      incrl();
*/


      bitty ++;




      numbo = inv_plus [splbook];


      itoa(numbo, strungy, 2);




      // Put changes here:
      /*
      if (strlen(strungy) < bitty + 1 | strungy [bitty] != '1')
      {
         return 0;
      }*/

      int kl = 0;

      for (h = 1; h < 7; h ++)
      {

      if (strungy [h] == '1') kl ++;

      if (kl == bitty)
      {
//         strungy [h] = '0';
         learned = h;
         goto succes;
      }

      //   strungy [bitty] = '0';

      }




      return 0;







      succes : multip = 1;
      numbo = 0;
      found_a_one = 0;
















      icky = strlen(strungy);

      multip = 1;
      numbo = 0;

      for (j = icky; j >= 1; j --)
      {
         /*if (j != 0 && (strungy [j] == '0' | strungy [j] == '1') && (strungy [j - 1] != '1' && strungy [j - 1] != '0'))
         {
            continue;
         }*/

         if (strungy [j] != '1' && strungy [j] != '0') continue;
         if (strungy [j] == '0')
         {
            multip *= 2;
//            cprintf("found");
            continue;

         }

         if (strungy [j] == '1')
         {
               numbo += multip;
               multip *= 2;
//               cprintf("found");
///               found_a_one = 1;

         }
       }



      strcpy(strungy, "");

//      itoa(numbo, strungy, 10);

//      cprintf(strungy);

//      return numbo;

inv_plus [splbook] = numbo + 64;

   return 1;

} // end of change spellbook


int which_spellbook(void)
{
unsigned char nthing;

if (spell_levels <= 0)
{
 strcpy(info [info_lines], "You can't memorise any more spells yet.");
 incrl();
 return 0;
}

if (inv_no == 0)
        {
        strcpy(info [info_lines], "You aren't carrying anything.");
        incrl();
        return 0;
        }

query :
strcpy(info [info_lines], "You can memorise ");
itoa(spell_levels, st_prn, 10);
strcat(info [info_lines], st_prn);
strcat(info [info_lines], " more levels of spells.");
incrl();
strcpy(info [info_lines], "Memorise from which spellbook?");
incrl();

// test relay_message();

keyin = get_ch();
//relay_message();
if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = invent(10, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if (keyin == '*') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
                goto query;
        }

}


sc_read_1 = (int) keyin;

if (sc_read_1 < 65 | (sc_read_1 > 90 && sc_read_1 < 97) | sc_read_1 > 122)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return 0;
}

sc_read_2 = conv_lett(sc_read_1);

if (inv_quant [sc_read_2] == 0)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return 0;
}

if (inv_class [sc_read_2] != 10)
{
        strcpy(info [info_lines], "That isn't a spellbook!");
        incrl();
        return 0;
}

count_y = sc_read_2;

//turnover = 1;

/*
for (i = 0; i < 25; i ++)
{
        if (spells [i] == inv_type [sc_read_2])
        {
                strcpy(info [info_lines], "You already know that spell!");
                incrl();
                return;
        }
}*/

read_book(count_y);

if (inv_plus [count_y] == 64)
{
   strcpy(info [info_lines], "That spellbook is empty.");
   incrl();
   return 0;
}

window(2, 16, 78, 24);
clrscr();
scrloc = 0;
//info_lines = 0;
return 1;

}

void read_book(char book_read)
{

//if (which_spellbook() == 0) return;

// remember that this function is called from staff spells as well.
if (inv_class [book_read] == 11) keyin = spellbook_contents(inv_plus [book_read], inv_type [book_read] + 40);
 else keyin = spellbook_contents(inv_plus [book_read], inv_type [book_read]);
// Put special book effects in another function which can be called from
// memorise as well
if (inv_ident [book_read] == 0) turnover = 1;
inv_ident [book_read] = 1;

return;

}


void which_spell(void)
{
int id_used = 0;

/*
int id_used = 0;
unsigned char nthing;

if (inv_no == 0)
        {
        strcpy(info [info_lines], "You aren't carrying anything.");
        incrl();
        return;
        }

query : strcpy(info [info_lines], "Memorise from which spellbook?");
incrl();

// test relay_message();

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = invent(10, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if (keyin == '*') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
                goto query;
        }

}


sc_read_1 = (int) keyin;

if (sc_read_1 < 65 | (sc_read_1 > 90 && sc_read_1 < 97) | sc_read_1 > 122)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

sc_read_2 = conv_lett(sc_read_1);

if (inv_quant [sc_read_2] == 0)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

if (inv_class [sc_read_2] != 10)
{
        strcpy(info [info_lines], "That isn't a spellbook!");
        incrl();
        return;
}

count_y = sc_read_2;

//turnover = 1;

/*
for (i = 0; i < 25; i ++)
{
        if (spells [i] == inv_type [sc_read_2])
        {
                strcpy(info [info_lines], "You already know that spell!");
                incrl();
                return;
        }
}* /



if (inv_plus [count_y] == 64)
{
   strcpy(info [info_lines], "That spellbook is empty.");
   incrl();
   return;
}
*/

if (which_spellbook() == 0) return;



//query_2 : strcpy(info [info_lines], "Memorise which spell?");
//incrl();

// test relay_message();

//keyin = spellbook_contents(inv_plus [count_y]);
// this is done in read_book(...)



//keyin = get_ch();

sc_read_1 = (int) keyin;


if (sc_read_1 < 65 | (sc_read_1 > 90 && sc_read_1 < 97) | sc_read_1 > 122)
{
        whatt : strcpy(info [info_lines], "What?");
        incrl();
        return;
}


sc_read_2 = conv_lett(sc_read_1);


if (sc_read_2 > 7)
{
   goto whatt;
}


if (learn_a_spell(count_y, sc_read_2) != 1)
{
   goto whatt;
}



char specspell = which_spell_in_book(inv_type [count_y], learned);


if (spell_no == 21) //if changed, must also change for priest in level_change.
{
 strcpy(info [info_lines], "Your head is already too full of spells!");
 incrl();
 return;
}

for (i = 0; i < 25; i ++)
{
        if (spells [i] == specspell)
   {
      strcpy(info [info_lines], "Oops, you already know that spell!");
      incrl();
      turnover = 1;
      return;
   }
}

/*if (magic_battery != 0)
{
 strcpy(info [info_lines], "Something stops you memorising the spell.");
 incrl();
 turnover = 1;
 return;
}*/


if (spell_levels < spell_value(specspell) | xl < spell_value(specspell))
{
   strcpy(info [info_lines], "Your brain hurts!");
   incrl();
   conf += 2 + random2(4);
   if (conf > 40) conf = 40;
   turnover = 1;
   return;
}

for (i = 0; i < 25; i ++)
{
        if (spells [i] == -1) break;
}


spells [i] = specspell;




/*ep_max -= spell_value(spells [i]);
if (ep > ep_max) ep = ep_max;
ep_ch = 1;
ep_max_ch = 1;
spell_no ++;*/

spell_levels -= spell_value(spells [i]);
spell_no ++;

//time_taken *= spell_value(spells [i]);

delay_t = spell_value(spells [i]); // * 3;
delay_doing = 3;
scruggle = 0;

turnover = 1;

}


void manage_shock_shield(void)
{
shock_shield --;
if (shock_shield == 0) return;

   char stx = 0;
   char sty = 0;

beam_colour = LIGHTCYAN;

 for (stx = -1; stx < 2; stx++)
   {

      for (sty = -1; sty < 2; sty++)
      {
          if (sty == 0 && stx == 0) continue;

                beam_range = 1 + random2(2);
          if (random2(3) == 0) beam_range ++;
//          if (beam_range > 20) beam_range = 20 + random2(4);
            beam_type = 1;

            target_x = x_pos + stx;
            target_y = y_pos + sty;

            if (grid [target_x] [target_y] > 10 && grid [target_x] [target_y] < 100)// && (target_x != x_pos | target_y != y_pos)) //100)
            {
//               place_cloud();
               place_cloud(1, target_x, target_y, beam_range);
            }

      } // end of sty

   } // end of stx

}

void cast_ring_of_flames(int power)
{
 shock_shield += random2(power) / 10 + 4;
 if (shock_shield > 25) shock_shield = 25;
 strcpy(info [info_lines], "The air around you leaps into flame!");
 incrl();
}


void restore_str(void)
{
 strength = max_strength;
 strength_ch = 1;
}

void restore_int(void)
{
 intel = max_intel;
 intel_ch = 1;
}

void restore_dex(void)
{
 dex = max_dex;
 dex_ch = 1;
}

void turn_undead(int pow) // what should I use for pow?
{

int tu = 0;

for (tu = 0; tu < MNST; tu ++)
{
 if (mons_class [tu] == -1 | mons_near(tu) == 0) continue;

 if (mons_holiness(mons_class [tu]) > 0)
 {
  mons_hp [tu] -= random2(5) + random2(pow) / 20;
  killer = 1;
  if (mons_hp [tu] <= 0)
  {
   monster_die(tu);
   continue;
  }



                        if (mons_ench_1 [tu] == 1)
                                for (p = 0; p < 3; p ++)
                                {
                                        if (mons_ench [tu] [p] == 4) brek = 1;
                                } // end of for p

                        for (p = 0; p < 3; p ++)
                        {
                        if (brek == 1)
                        {
                                brek = 0;
                                break;
                        }
                        if (mons_ench [tu] [p] == 0) // | p == 2) // replaces 3rd enchantment if all full.
                                {
                                        mons_ench [tu] [p] = 4;
                                        //mons_beh [i] = 3;
                                        mons_ench_1 [tu] = 1;
        //                              strcpy(info [info_lines], "Scared!");
        //                                incrl();
                                        break;
                                }
   }

  } // end of if mons_holiness

} // end of for tu

} // end of turn_undead


void holy_word(int pow)
{

int tu = 0;

strcpy(info [info_lines], "You speak a Word of immense power!");
incrl();

for (tu = 0; tu < MNST; tu ++)
{
 if (mons_class [tu] == -1 | mons_near(tu) == 0) continue;

 if (mons_holiness(mons_class [tu]) > 0)
 {
  mons_hp [tu] -= random2(15) + random2(15) + random2(pow) / 10;
  killer = 1;
  if (mons_hp [tu] <= 0)
  {
   monster_die(tu);
   continue;
  }

    if (mons_speed_inc [tu] >= 25) mons_speed_inc [tu] -= 20;

                        if (mons_ench_1 [tu] == 1)
                                for (p = 0; p < 3; p ++)
                                {
                                        if (mons_ench [tu] [p] == 4) brek = 1;
                                } // end of for p

                        for (p = 0; p < 3; p ++)
                        {
                        if (brek == 1)
                        {
                                brek = 0;
                                break;
                        }
                        if (mons_ench [tu] [p] == 0) // | p == 2) // replaces 3rd enchantment if all full.
                                {
                                        mons_ench [tu] [p] = 4;
                                        //mons_beh [i] = 3;
                                        mons_ench_1 [tu] = 1;
        //                              strcpy(info [info_lines], "Scared!");
        //                                incrl();
                                        break;
                                }
   }

  } // end of if mons_holiness

} // end of for tu



} // end of holy word


void cast_toxic_radiance(void)
{

unsigned char toxy = 0;

strcpy(info [info_lines], "A sickly green light radiates from you!");
incrl();
show_green = GREEN;
brek = 1;
viewwindow();
brek = 0;
if (invis != 0)
{
 strcpy(info [info_lines], "The light passes straight through your body!");
 incrl();
} else
       if (res_poison == 0)
  {
   poison += 2;
   strcpy(info [info_lines], "You feel rather sick.");
   incrl();
  }


for (toxy = 0; toxy < MNST; toxy ++)
{
  if (mons_class [toxy] == -1) continue;
  if (mons_near(toxy) == 1)
  {
    if (mons_ench [toxy] [2] != 6) poison_monster(toxy, 0); // assumes only you can cast this spell (or would want to)
  }

} // end loop

} // end toxic rad


void look_around(void)
{

        int xps = 17;
 int yps = 7;
        char gotch;
        char mve_x, mve_y;
        char bk = 0;
        char keyy;

 strcpy(info [info_lines], "Move the cursor around.");
 incrl();
 // test relay_message();

        cursoring: gotoxy(xps,yps);

        window(2,3,34,15);
        gotoxy(xps,yps);

//while (gotch != '.')
do
{
//gotch = getch();



        gotch = getch();

//   if (keyy == 'p') return 253;

        if (gotch != 0 && gotch != 13)
        {
        //gotch = getch();
  switch(gotch)
  {
                case 'b': mve_x = -1; mve_y = 1; break;
                case 'j': mve_y = 1; mve_x = 0; break;
                case 'u': mve_x = 1; mve_y = -1; break;
                case 'k': mve_y = -1; mve_x = 0; break;
                case 'y': mve_y = -1; mve_x = -1; break;
                case 'h': mve_x = -1; mve_y = 0; break;
                case 'n': mve_y = 1; mve_x = 1; break;
                case 'l': mve_x = 1; mve_y = 0; break;
    default:    return;
  }
 goto gotchy;
 }

        if (gotch != 13) gotch = getch();

        mve_x = 0; mve_y = 0;

        switch(gotch)
        {
                case 13: gotch = 'S'; break;
                case 'O': mve_x = -1; mve_y = 1; break;
                case 'P': mve_y = 1; mve_x = 0; break;
                case 'I': mve_x = 1; mve_y = -1; break;
                case 'H': mve_y = -1; mve_x = 0; break;
                case 'G': mve_y = -1; mve_x = -1; break;
                case 'K': mve_x = -1; mve_y = 0; break;
                case 'Q': mve_y = 1; mve_x = 1; break;
                case 'M': mve_x = 1; mve_y = 0; break;
                case 'S': break;
                //case 'L': bk = 0; break;

                // need <, > etc

                default: return; //strcpy(info [info_lines], "What an unusual direction.");
                        //info_lines++; mve_x = 0; mve_y = 0; return 99;
        }

gotchy : gotoxy(xps,yps);
//putch(grokol [0]);
//textbackground(7);

if (xps + mve_x >= 11 && xps + mve_x < 24) xps += mve_x;
if (yps + mve_y >= 1 && yps + mve_y < 14) yps += mve_y;

//gettext(x + 1, y + 1, x + 1, y + 1, grokol);
//textbackground(7);
mesclr();

if (show [xps - 10] [yps] == 0 && (xps != 17 | yps != 7))
{
 strcpy(info [info_lines], "You can't see that place.");
 incrl();
 goto glogokh;
}


if ((grid [x_pos + xps - 17] [y_pos + yps - 7] >= 50 && grid [x_pos + xps - 17] [y_pos + yps - 7] < 100) | grid [x_pos + xps - 17] [y_pos + yps - 7] >= 150)
{
for (i = 0; i < MNST; i++)
{
        if (mons_x [i] == x_pos + xps - 17 && mons_y [i] == y_pos + yps - 7 && mons_class [i] != -1)
     break;
}

if (mons_ench [i] [2] == 6 && see_invis == 0) goto look_clouds;

strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 2));
strcat(info [info_lines], ".");
incrl();
//if (mons_inv [i] [0] != 501)
//{
// strcpy(info [info_lines], "It is wielding
//}

if (mons_class [i] == 106)
{
 strcpy(info [info_lines], "It has ");
 itoa(mons_sec [i], st_prn, 10);
 strcat(info [info_lines], st_prn);
 strcat(info [info_lines], " heads.");
 incrl();
}

if (mons_beh [i] == 7)
{
 strcpy(info [info_lines], "It is friendly.");
 incrl();
}
}

look_clouds:
if (grid [x_pos + xps - 17] [y_pos + yps - 7] >= 100)
{
 switch(cloud_type [which_cloud(x_pos + xps - 17, y_pos + yps - 7)] % 100)
 {
  case 1: strcpy(info [info_lines], "There is a cloud of flame here."); break;
  case 2: strcpy(info [info_lines], "There is a cloud of noxious fumes here."); break;
  case 3: strcpy(info [info_lines], "There is a cloud of freezing vapour here."); break;
  case 4: strcpy(info [info_lines], "There is a cloud of poison gas here."); break;
  case 5: strcpy(info [info_lines], "There is a cloud of smoke here."); break;
  case 6: strcpy(info [info_lines], "There is a cloud of blue smoke here."); break;
  case 7: strcpy(info [info_lines], "There is a cloud of purple smoke here."); break;
  case 8: strcpy(info [info_lines], "There is a cloud of steam here."); break;
 }
incrl();
} // end of look_clouds:

if (igrid [x_pos + xps - 17] [y_pos + yps - 7] != 501)
{
 if (item_link [igrid [x_pos + xps - 17] [y_pos + yps - 7]] != 501 | item_quant [igrid [x_pos + xps - 17] [y_pos + yps - 7]] > 1)
 {
  strcpy(info [info_lines], "You see a pile of items here.");
 } else strcpy(info [info_lines], "You see an item here.");

 incrl();
}

switch(grid [x_pos + xps - 17] [y_pos + yps - 7] % 50)
{
  case 2:
  case 1: strcpy(info [info_lines], "A rock wall.");
  break;
  case 3: strcpy(info [info_lines], "A closed door.");
  break;
  case 4: strcpy(info [info_lines], "A metal wall.");
  break;
  case 11: strcpy(info [info_lines], "Some lava.");
  break;
  case 12: strcpy(info [info_lines], "Some water.");
  break;
  case 28: // undiscovered trap
  case 17: strcpy(info [info_lines], "A piece of floor.");
  break;
  case 20: strcpy(info [info_lines], "An open door.");
  break;
  case 18: strcpy(info [info_lines], "A staircase.");
  break;
  case 19: strcpy(info [info_lines], "A staircase to hell.");
  break;
  case 21: strcpy(info [info_lines], "A staircase to a branch level.");
  break;
  case 25: strcpy(info [info_lines], "A trap.");
  break;
  case 26: strcpy(info [info_lines], "A magical trap.");
  break;
  case 27: strcpy(info [info_lines], "A trap.");
  break;
}
incrl();

glogokh:        // test relay_message();

//itoa((int) grid [x_pos + xps - 17] [y_pos + yps - 7], st_prn, 10);
//strcpy(info [info_lines], st_prn);
//incrl();

window(2,3,34,15);
gotoxy(xps,yps);
//putch(grokol [0]);
//textbackground(0);

} while (gotch != 'S');

// end of while.

mve_x = xps - 7;
mve_y = yps - 7;

return;


} // end of look_around




void direct_effect(void)
{


switch(beam_type)
{
 case 0: // hellfire
 strcpy(info [info_lines], "You are engulfed in a burst of hellfire!");
        incrl();
 hurted = random2(10) + random2(5) + 5;
 beam_flavour == 20; // lava, but it's hellfire anyway
 strcpy(beam_name, "hellfire"); // for ouch
 check_your_resists();
 scrolls_burn(4, 6);
        ouch(hurted, 0, 3);
 hp_ch = 1;
 break;

 case 1: // smiting
 strcpy(info [info_lines], "Something smites you!");
        incrl();
 hurted = random2(10) + random2(5) + 5;
 strcpy(beam_name, "smiting"); // for ouch
        ouch(hurted, 0, 3);
 hp_ch = 1;
 break;

 case 2: // brain feed
      if (random2(3) == 0 && intel > 3 && sust_abil == 0)
      {
        strcpy(info [info_lines], "Something feeds on your intelligence!");
        incrl();
        strcpy(info [info_lines], "You feel stupid.");
        incrl();
        intel --;
        intel_ch = 1;
      } else
                                                                                                {
                strcpy(info [info_lines], "Something tries to feed on your intelligence!");
                                                                                                                                incrl();
                                                                                                }
 break;



}

return;

}


void mons_direct_effect(void)
{


switch(beam_type)
{


  case 0:
  if (mons_near(mons_hit [i]) == 1)
                {
                strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));
                strcat(info [info_lines], " is engulfed in hellfire.");
                incrl();
  }
  strcpy(beam_name, "hellfire");
  beam_flavour = 20;
                hurted = 0;
  hurted += random2(10) + random2(5) + 5;
                check_mons_resists();
                mons_hp [mons_hit [i]] -= hurted;

                if (mons_beh [o] == 0) mons_beh [o] = 1;

                if (mons_hp [o] <= 0)
                {
   killer = 4;
                monster_die(o);
                }
  break;

  case 2: // brain feed - not implemented here (nor, probably, can be)
                break;

} // end switch

} // end monster_direct_effect()



void new_level(void)
{
        window(36,1,75,15);
        textcolor(7);
 itoa(your_level + 1, temp_quant, 10);
        gotoxy (11,11);
 floor_colour = LIGHTGRAY;
 rock_colour = BROWN;
 if (level_type == 1)
 {
  cprintf("a labyrinth");
 }
 else if (where_are_you == 1)
 {
  floor_colour = CYAN;
  cprintf("Dis");
 }
 else if (where_are_you == 2)
 {
   floor_colour = LIGHTRED;
   rock_colour = RED;
   cprintf("Hell");
 }
  else
        cprintf(temp_quant);
        cprintf("      ");
}

void original_name(void)
{
char drink_1;
char drink_2;
char inn = 0;
char nthing = 0;

query : strcpy(info [info_lines], "Examine which item?");
incrl();

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '?') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if (keyin == '*') nthing = invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
    goto query;
        }
}

drink_1 = (int) keyin;

if (drink_1 < 65 | (drink_1 > 90 && drink_1 < 97) | drink_1 > 122)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

drink_2 = conv_lett(drink_1);

if (inv_quant [drink_2] == 0)
{
        strcpy(info [info_lines], "You don't have any such object.");
        incrl();
        return;
}

inn = drink_2;

   strcpy(info [info_lines], " ");
                        strncpy(info [info_lines], letters [item_wield_2], 1);
                        strcat(info [info_lines], " - ");
                 item_name(inv_class [inn], inv_type [inn], inv_dam [inn], inv_plus [inn], inv_quant [inn], 0, 3, str_pass);
//                      in_name(item_wield_2, 3);
                        strcat(info [info_lines], str_pass);
   incrl();
} // end original_name


void special_wielded(void)
{

switch(special_wield)
{
case 1:
if (random2(20) == 0)
{
 strcpy(info [info_lines], "The Singing Sword ");
 switch(random2(32))
 {
  case 0: strcat(info [info_lines], "hums a little tune."); incrl(); break;
  case 1: strcat(info [info_lines], "breaks into glorious song!"); incrl(); break;
  case 2: strcat(info [info_lines], "sings."); incrl(); break;
  case 3: strcat(info [info_lines], "sings loudly."); incrl(); break;
  case 4: strcat(info [info_lines], "chimes melodiously."); incrl(); break;
  case 5: strcat(info [info_lines], "makes a horrible noise."); incrl(); break;
  case 6: strcat(info [info_lines], "sings off-key."); incrl(); break;
  case 7: strcat(info [info_lines], "sings 'tra-la-la'."); incrl(); break;
  case 8: strcat(info [info_lines], "burbles away merrily."); incrl(); break;
  case 9: strcat(info [info_lines], "gurgles."); incrl(); break;
  case 10: strcat(info [info_lines], "suddenly shrieks!"); incrl(); break;
  case 11: strcat(info [info_lines], "cackles."); incrl(); break;
  case 12: strcat(info [info_lines], "warbles."); incrl(); break;
  case 13: strcat(info [info_lines], "chimes harmoniously."); incrl(); break;
  case 14: strcat(info [info_lines], "makes beautiful music."); incrl(); break;
  case 15: strcat(info [info_lines], "produces a loud orchestral chord."); incrl(); break;
  case 16: strcat(info [info_lines], "whines plaintively."); incrl(); break;
  case 17: strcat(info [info_lines], "tinkles."); incrl(); break;
  case 18: strcat(info [info_lines], "rings like a bell."); incrl(); break;
  case 19: strcat(info [info_lines], "wails mournfully."); incrl(); break;
  case 20: strcat(info [info_lines], "practices its scales."); incrl(); break;
  case 21: strcat(info [info_lines], "lilts tunefully."); incrl(); break;
  case 22: strcat(info [info_lines], "hums tunelessly."); incrl(); break;
  case 23: strcat(info [info_lines], "sighs."); incrl(); break;
  case 24: strcat(info [info_lines], "makes a deep moaning sound."); incrl(); break;
  case 25: strcat(info [info_lines], "makes a popping sound."); incrl(); break;
  case 26: strcat(info [info_lines], "sings a sudden stoccato note."); incrl(); break;
  case 27: strcat(info [info_lines], "says 'Hi! I'm the Singing Sword!'."); incrl(); break;
  case 28: strcat(info [info_lines], "whispers something."); incrl(); break;
  case 29: strcat(info [info_lines], "speaks gibberish."); incrl(); break;
  case 30: strcat(info [info_lines], "raves incoherently."); incrl(); break;
  case 31: strcat(info [info_lines], "yells in some weird language."); incrl(); break;
 }
   x_plos = x_pos;
   y_plos = y_pos;
   noisy();
}
break;


case 3: // scythe of Curses
if (random2(30) == 0) curse_an_item(0, 0);
break;

case 4: // mace of variability
if (inv_plus [item_wielded] > 100) inv_plus [item_wielded] -= 100;
if (random2(3) != 0) break;
inv_plus [item_wielded] += random2(2);
inv_plus [item_wielded] -= random2(2);
if (inv_plus [item_wielded] < 46) inv_plus [item_wielded] = 46;
if (inv_plus [item_wielded] > 57) inv_plus [item_wielded] = 57;
inv_col [item_wielded] = random2(15) + 1;
break;

// 5 - glaive

} // end switch

} // end void special_wielded






void invoke_wielded(void)
{

if (item_wielded == -1)
{
 strcpy(info [info_lines], "You aren't wielding anything!");
 incrl();
 return;
}

switch(inv_class [item_wielded])
{
 case 11:
 staff_spell(item_wielded);
 break;

 default: strcpy(info [info_lines], "Nothing appears to happen.");
 incrl();
 break;

}

turnover = 1;

} // end invoke_wielded




















void staff_spell(char zap_device_2)
{
int id_used = 0;
char staff_zapped = zap_device_2;
int spellh;

if (inv_plus [zap_device_2] == 64 | inv_type [zap_device_2] < 7)
{
   strcpy(info [info_lines], "That staff has no spells in it.");
   incrl();
   return;
}


read_book(zap_device_2);
unsigned char specspell;
int powc = magic_ability(mag_abil, intel);

//keyin = getch();
sc_read_1 = (int) keyin; // keyin is got in read_book

if (sc_read_1 < 65 | (sc_read_1 > 90 && sc_read_1 < 97) | sc_read_1 > 122)
{
        whatt : strcpy(info [info_lines], "What?");
        incrl();
        return;
}


sc_read_2 = conv_lett(sc_read_1);


if (sc_read_2 > 7)
{
   goto whattt;
}


if (learn_a_spell(zap_device_2, sc_read_2) != 1)
{
   goto whattt;
}



specspell = which_spell_in_book(inv_type [item_wielded] + 40, sc_read_2 + 1);

if (ep < spell_value(specspell) | xl < spell_value(specspell))
{
   strcpy(info [info_lines], "Your brain hurts!");
   incrl();
   conf += 2 + random2(4);
   if (conf > 40) conf = 40;
   turnover = 1;
   return;
}

// note that spell staves cannot have empty spaces in strungy before the last real spell.

your_spells(specspell, powc);

ep -= spell_value(specspell);
ep_ch = 1;

if (inv_type [item_wielded] != 10) // staff of smiting uses no food
if (energy == 0)
{
 spellh = spell_hunger(spell_value(specspell), specspell);
 spellh -= magic_ability(mag_abil, intel);
 if (spellh < 10) spellh = 10;
 hunger -= spellh;
 hung_ch = 1;
 how_hungered(spellh);
}

turnover = 1;

return;

whattt:
strcpy(info [info_lines], "What?");
incrl();

} // end staff_spell

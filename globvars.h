#define ITEMS 500
#define MNST 200
#define NTRAPS 30
#define CLOUDS 30
#define PETS 20
#define MHITNOT 201
#define MHITYOU 202
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

//#define strcpy(info[info_lines], print

// change MHIT??? if I change MNST

// unfortunately, pets are too stupid too be real pets, but
// clever enough to be summoned creatures which don't last long.

char teleport = 0;
char see_invis = 0;
char res_fire = 1;
char res_poison = 0;
char res_cold = 0;
char fast_run = 0;
char sust_abil = 0;
char deaths_door = 0;
//char hiding = 1;
char magic_battery = 0;
char spec_fire = 0;
char spec_cold = 0;
char spec_poison = 0;
char prot_life = 0;
char special_wield = 0;
char berserker = 0;
char energy = 0;

char level_type = 0;
// 0 = normal dungeon level
// 1 = labyrinth

int res_magic = 0;
char max_level = 1;

char show_green = 0;

int x_pos = 10;
int y_pos = 10;
int count_x;
int count_y;
unsigned char map [80] [70];
unsigned char grid [80] [70]; // this is what is actually there
                        // Have the two dimensions different (ie 80 & 70) so that the Replace command can distinguish them.
//unsigned char show [15] [15]; // this is just what you see
unsigned int show [15] [15]; // I'd love this not to be an int. Oh well.
unsigned char buffy [1500]; //[800]; //392];


int mons_class [MNST];
int mons_hp [MNST];
//unsigned char mons_hp_max [MNST];
int mons_hp_max [MNST];
unsigned char mons_HD [MNST];
int mons_AC [MNST];
char mons_ev [MNST];
//int mons_att [MNST] [4]; // (don't) use the gmon value for this.
/*float mons_speed [MNST];
float mons_speed_inc [MNST];*/

/*unsigned char mons_speed [MNST];
unsigned char mons_speed_inc [MNST];*/

unsigned char mons_speed [MNST];
unsigned char mons_speed_inc [MNST];

unsigned char mons_x [MNST];
unsigned char mons_y [MNST];
unsigned char mons_targ_1_x [MNST]; // these four can probable be reused each time through the loop too.
unsigned char mons_targ_1_y [MNST];
//int mons_targ_2_x [MNST];
//int mons_targ_2_y [MNST];
int mons_inv [MNST] [8];
unsigned char mons_beh [MNST];
        unsigned char no_mons = 0;
        unsigned char mn_no = 0;
        unsigned char mons_sec [MNST];
 unsigned char mons_hit [MNST]; // is it hitting another monster?
        //unsigned char mnc = 0;


         unsigned char item_class [ITEMS];
         unsigned char item_type [ITEMS];
         unsigned char item_plus [ITEMS]; // +, charges, remaining food value
         unsigned char item_dam [ITEMS]; // damage
         unsigned int item_quant [ITEMS]; // multiple items
         unsigned char item_x [ITEMS]; //  x-location
         unsigned char item_y [ITEMS]; //  y-location
//       char fake_name [ITEMS] [30]; // fake names of items
         char thing_quant [16];
         unsigned long temp_long;
         int it_no = 0; int itc = 0;
         unsigned char icolour [ITEMS];
         char item_ident [ITEMS];

         int item_link [ITEMS];

  char unique_items [50];

int i;
int j;                  // REMEMBER directvideo ! ! !
int p;
int l;
int nothing;
/*#include <fstream.h>
#include <fcntl.h>
#include <io.h>
#include <SYS\STAT.H>*/


char keyin;


int curr_level = 0;
int your_level;

unsigned char show_col [15] [15];
int your_colour = LIGHTGRAY; // What colour are you?
char your_sign = 64;
//int icolour [500];// [30];
char mcolour [400];
//char mons_beh [MNST];
int brek; // used in monster() etc
//int itcheck = 0;

char row_of_7s [12];

int k;

char killer; // used in monster_die() for what killed the monster

int invis = 0;
int conf = 0;
int paralysis = 0;
int slow = 0;
int haste = 0; // haste
int might = 0; // might
int lev = 0; // levitation
int poison = 0; // poison!!
int rotting = 0;
unsigned char shock_shield = 0;


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
char info [78] [78];  // This is messages sent to the screen via message func.
char scrloc = 0;
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
int hunger = 6000;
char hunger_inc = 4;
char hung_ch = 0;
char hung_state = 3; // normal
char burden_ch = 0;

int item_wielded = -1;
int item_wield_1;
int item_wield_2;
/*
int body_armour = -1;
int head_armour = -1;
int cloak_armour = -1;
int hand_armour = -1;
int foot_armour = -1;
int shield_armour = -1;
*/

char armour [6];

int armour_wear_1;
int armour_wear_2;

int ring_wear_1;
int ring_wear_2;
char ring [2];
int hand_used;

int beam_range;
int beam_type;
int beam_colour;
int beam_source_x;
int beam_source_y;
int beam_damage;
int ench_power = 0;
int beam_hit;
int beam_x;
int beam_y;
int target_x;
int target_y;
char beam_flavour;

char beam_name [30];

int hurted;

int throw_1;
int throw_2;
int thing_thrown;
int x_plos; int y_plos;

//char killer; // used in monster_die() for what killed the monster

// Character attributes:
int hp = 10; int hp_ch = 1;
int hp_max = 10; int hp_max_ch = 1;
int ep = 3; int ep_ch = 1;
int ep_max = 5; int ep_max_ch = 1;

char strength = 16; char strength_ch = 1;
char intel = 13; char intel_ch = 1;
char dex = 11; char dex_ch = 1;

char max_strength = 16;
char max_intel = 13;
char max_dex = 11;

int AC = 0; int AC_ch = 1; // remember that the AC shown = 10 - AC
int evasion = 10; int evasion_ch = 1;
char shield_class = 0;
int damage = 2; int damage_ch = 1;  // remember to change damage back to 1!
int nat_damage = 2; // the damage you do unarmed
//float rate_regen = 0.06; float incr_regen = 0;
//float ep_rate_regen = 0.06; float ep_incr_regen = 0;
char rate_regen = 6; unsigned char incr_regen = 0;
char ep_rate_regen = 6; char ep_incr_regen = 0;


//unsigned long xp = 0; int xp_ch = 1;
unsigned int xp = 0; int xp_ch = 1;
int xl = 1; int xl_ch = 1;
int gp = 0; int gp_ch = 1;
int clas;

char clasnam [13];

int f_abil = 10; // fighting ability, out of whatever
int mag_abil = 10; // magic ability, out of whatever
int thr_abil = 10; // throw_abil

//float speed = 1; // 0.75;


char speed = 10; // 0.75;


int speed_inc = 0; // This variable isn't used - must remove it sometime.

char time_taken = 0;

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
//int gmon_AC [52];
//int gmon_ev [52]; // evasion
//float gmon_speed [52]; // speed - not in use yet
//int gmon_use [52]; // can it use items?
//int mcolour [52];
/*
int mons_class [120];
int mons_hp [120];
int mons_hp_max [120];
int mons_HD [120];
int mons_AC [120];
int mons_ev [120];
//int mons_att [120] [4]; // (don't) use the gmon value for this.
float mons_speed [120];
float mons_speed_inc [120];
int mons_x [120];
int mons_y [120];
int mons_targ_1_x [120]; // these four can probably be reused each time through the loop too.
int mons_targ_1_y [120];
int mons_targ_2_x [120];
int mons_targ_2_y [120];
*/
int mmov_x;  // don't need to be arrays because they are reused
int mmov_y;  // each time through the i loop.

        int mons_to_hit;
        int damage_taken = 0;
        int runthru = 0;



//      char inv_name [52] [30];
        char inv_class [52];
        char inv_type [52];
        unsigned char inv_plus [52];
        unsigned char inv_dam [52];
        char inv_col [52];
        int inv_quant [52]; // multiple items, eg ammo or potions
        int burden = 1000; // total weight of items carried.
 char burden_state = 0;
        char inv_no; // number of items carried.
        char inv_ident [52];


        // Monster stuff: gmon = general monster, mons = specific monster

//char gmon_name [100] [20]; // The name of the monster
//int gmon_HD [52]; // Hit Dice
//int gmon_att [100] [4]; // attacks and damage of each attack
char gmon_use [400];

char your_name [9];







//       char ench_subj [25];
//       char ench_type [25];
//       char ench_

         // char mons_beh [120]; in view.h

         unsigned char mons_ench [MNST] [3];
         unsigned char mons_ench_1 [MNST]; // is it enchanted at all?
         //unsigned char mons_inv [120] [4];

         int property [4] [30] [3];
         int mass [15] [30];





char spell_cast = 0; // used in mons_cast

char tracer = 0; // used in missile
char trac_hit_mons = 0;
char tracer_mons = 0;
char tracer_range = 0;

int trac_targ = MHITNOT;
int trac_targ_x = 0;
int trac_targ_y = 0;

char trac_hit_tamed = 0;

char levels_exist [30];

//char cha_no = 0;

int newc = 0;

unsigned int turns = 0;

char running = 0;
char run_x = 0;
char run_y = 0;

int prev_targ = 500;

int igrid [80] [70];

int last_item = 501;



unsigned char trap_x [NTRAPS];
unsigned char trap_y [NTRAPS];
unsigned char trap_type [NTRAPS];
int trapped;



int func_pass [10];
char str_pass [50];

char vacated_x; // for tunneling worms etc
char vacated_y;

char sc_read_1;
char sc_read_2;

char death_string [80];


unsigned char cloud_x [CLOUDS];
unsigned char cloud_y [CLOUDS];
unsigned char cloud_type [CLOUDS];
int cloud_decay [CLOUDS];
unsigned char cloud_no;

char spells [25]; // can't change to unsigned 'cause empty is -1 (D'oh!)
char spell_levels = 50;

char spell_no = 0;

char wand_id = 0;

unsigned char mons_alloc [20];

int learned = -1;

unsigned char item_description [5] [30];

char scruggle = 0;
char aim_down = 0;

unsigned char pet_target = 0; // I should probably save this in the save-file

unsigned char empty_x;
unsigned char empty_y;

unsigned char stair_x [10];
unsigned char stair_y [10];
unsigned char stair_taken = 0;
unsigned char char_direction = 0;
// 0 = going down
// 1 = going up!
unsigned char where_are_you = 0;
// 0 = normal dungeon level
// 1 = dis
// 2 = hell
long points = 0;

unsigned char floor_colour = LIGHTGRAY;
unsigned char rock_colour = BROWN;
/*char pet_no = 0;
char is_pet_no = 0;
unsigned char pet_which [PETS];
unsigned char pet_target [PETS];
*/
// Add -lemu to link for floating point emulation

// Carbolic smoke ball
// Trollhide armour - ++ regen, but fire/acid do permanent damage?

// VITAL!!! Must notify author of CWSDPMI.EXE, also Rhide thing?

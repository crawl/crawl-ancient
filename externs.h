#include "defines.h"
#include "message.h"

extern char info [200];
extern char st_prn [20];

extern char str_pass [80];

extern char gmon_use [1000];
extern char mcolour [1000];
extern unsigned char show_green;


struct dist
{
 char move_x;
 char move_y;
 char target_x;
 char target_y;
 char prev_targ;
 long nothing;
};

struct bolt
{
 int range, type, colour, flavour, source_x, source_y;
 int damage, ench_power, hit, bx, by, target_x;
 int target_y;
/* char beam_flavour, wand_id;*/
 char wand_id;
 char aim_down;
 char thing_thrown;
/*, beam_type;*/
 char move_x, move_y;
 int trac_targ;
 int tracer_mons;
 int trac_hit_tamed;
 int trac_hit_mons;
 int tracer;
 int trac_targ_x, trac_targ_y;
/* int beam_hit;*/

 /* If a monster fired it, which monster? */
 int beam_source;

 char beam_name [40];

};

/*struct level
{
 unsigned char grid [GXM] [GYM];
 unsigned char mgrid [GXM] [GYM];
 unsigned int igrid [GXM] [GYM];
 unsigned char map [GXM] [GYM];
};*/

struct player
{
char turnover;
unsigned char prev_targ;

char your_name [30];

unsigned char species;
char run_x;
char run_y;
char running;

/*
Make these all functions:
char teleport;
char see_invis;
char res_fire;
char res_poison;
char res_cold;
char fast_run;
char sust_abil;
char deaths_door;
char magic_battery;
char spec_fire;
char spec_cold;
char spec_poison;
char prot_life;
char berserker;
char energy;*/
char special_wield;
char deaths_door;
char shock_shield;

unsigned char corpse_count;
unsigned char disease;

/* Function
int res_magic ;
*/
char max_level;

int x_pos;
int y_pos;

int hunger;
char hunger_inc;
char equip [NO_EQUIP];
/* list in player.cc */

int hp;
int hp_max;
int base_hp; // this is what is affected by necrophages etc
int base_hp2;
int ep;
int ep_max;
int base_ep;
int base_ep2;

char strength;
char intel;
char dex;
char max_strength;
char max_intel;
char max_dex;


/* xxx_ch variables : determine whether new value is printed next turn.
Set to 1 if value is changed. */
char hung_ch;
char hung_state; /* normal */
char burden_ch;
char hp_ch;
char ep_ch;
char strength_ch;
char intel_ch;
char dex_ch;
char xp_ch;
char AC_ch; /* remember that the AC shown = 10 - AC */
char gp_ch;
char evasion_ch;

/* Functions:
int AC;
int evasion;
char shield_class;
char rate_regen;
char ep_rate_regen;
*/

unsigned char incr_regen;
unsigned char ep_incr_regen;

unsigned long xp;
int xl;
unsigned int gp;
int clas;
char clasnam [30];
char speed;
int time_taken;

        unsigned char inv_class [52];
        unsigned char inv_type [52];
        unsigned char inv_plus [52];
        unsigned char inv_plus2 [52];
        unsigned char inv_dam [52];
        unsigned char inv_ident [52];
        char inv_col [52];
        int inv_quant [52];
        char inv_no; // number of items carried.

 int burden;
 char burden_state;
unsigned char spells [25];
char spell_no;
char spell_levels;
unsigned char char_direction ;
// 0 = going down
// 1 = going up!
unsigned char pet_target;

int your_level;

unsigned char duration [30]; // lots of durational things. Why didn't I do this for haste etc right from the start? Oh well.
/* There's a list of these in player.cc */

int invis ;
int conf;
int paralysis;
int slow ;
int haste ; // haste
int might ; // might
int lev ; // levitation
int poison ; // poison!!
int rotting ;
int berserker;

unsigned char attribute [10]; // various attributes, eg resist lightning
/* list in player.cc */

char is_undead;
/* 0 - is alive
   1 - is undead, but can still eat/drink etc (eg vampire)
   2 - is undead (mummy etc)*/

char delay_doing;
char delay_t;

unsigned char skills [50];
unsigned char practise_skill [50];
unsigned int skill_points [50];
int exp_available;

unsigned char item_description [5] [50];
char unique_items [50];
char unique_creatures [50];
char level_type;



char where_are_you;

unsigned char branch_stairs [20];

char religion;
unsigned char piety;
unsigned char gods [20];

unsigned char mutation [50];
unsigned char mpower;

unsigned char betrayal;

};

extern struct player you [1];


struct monsters
{
 int m_class;
 int m_hp;
 int m_hp_max;
 unsigned char m_HD;
 int m_AC;
 char m_ev;
 unsigned char m_speed;
 unsigned char m_speed_inc;
 unsigned char m_x;
 unsigned char m_y;
 unsigned char m_targ_1_x;
 unsigned char m_targ_1_y;
 int m_inv [8];
 unsigned char m_beh;
 unsigned char m_sec;
 unsigned char m_hit;
 unsigned char m_ench [3];
 unsigned char m_ench_1;
};

/* extern struct monsters mons [MNST]; */

struct item_struct
{
 unsigned char iclass [ITEMS]; /* basic class (eg weapon) */
 unsigned char itype [ITEMS]; /* type within that class (eg dagger) */
 unsigned char iplus [ITEMS]; /* hit+, charges */
 unsigned char iplus2 [ITEMS]; /* dam+ etc */
 unsigned char idam [ITEMS]; /* special stuff */
 unsigned int iquant [ITEMS]; /* multiple items */
 unsigned char ix [ITEMS]; /*  x-location */
 unsigned char iy [ITEMS]; /* y-location */
 unsigned char icol [ITEMS]; /* colour */
 unsigned char iid [ITEMS]; /* identification */
 unsigned int ilink [ITEMS]; /* next item in stack */
};

/*extern struct item_struct it [1]; */


struct environ
{
 item_struct it [1];
 monsters mons [MNST];
 unsigned char grid [GXM] [GYM];
 unsigned char mgrid [GXM] [GYM];
 int igrid [GXM] [GYM];
 unsigned char map [GXM] [GYM];
 char cgrid [GXM] [GYM];
 unsigned int show [19] [19];
 unsigned char show_col [19] [19];
 unsigned char rock_colour;
 unsigned char floor_colour;

 unsigned char cloud_x [CLOUDS];
 unsigned char cloud_y [CLOUDS];
 unsigned char cloud_type [CLOUDS];
 int cloud_decay [CLOUDS];
 unsigned char cloud_no;

 unsigned char keeper_name [5] [3];
 unsigned char sh_x [5];
 unsigned char sh_y [5];
 unsigned char sh_greed [5];
 unsigned char sh_type [5];
 unsigned char sh_level [5];

 unsigned char trap_type [NTRAPS];
 unsigned char trap_x [NTRAPS];
 unsigned char trap_y [NTRAPS];
 int mons_alloc [20];
 int trap_known;

};

extern struct environ env [1]; // make this global to main module

struct ghost_struct
{
 char gname [20];
 unsigned char ghs [20];
};

extern struct ghost_struct ghost;

extern void (*viewwindow)(char);

#ifndef EXTERNS_H
#define EXTERNS_H

#include "defines.h"
#include "message.h"

extern char info[200];
extern char st_prn[20];

extern char str_pass[80];

extern char gmon_use[1000];
extern char mcolour[1000];
extern unsigned char show_green;

#ifdef SHORT_FILE_NAMES
const int kNameLen = 30;
const int kFileNameLen = 6;
const int kFileNameSize = 5 + kFileNameLen;

#else
#ifdef SAVE_DIR_PATH
        // file name length has to be able to cover full paths -- bwross
const int kNameLen = 30;
const int kFileNameLen = 250;
const int kFileNameSize = 5 + kFileNameLen;

#else
const int kNameLen = 30;
const int kFileNameLen = 28;
const int kFileNameSize = 5 + kFileNameLen;

#endif
#endif

// Length of Path + File Name
const int kPathLen = 256;

// This value is used to mark that the current berserk is free from
// penalty (Xom's granted or from a deck of cards).
#define NO_BERSERK_PENALTY    -1


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
    char wand_id;
    char aim_down;
    char thing_thrown;
    char move_x, move_y;
    int trac_targ;
    int tracer_mons;
    int trac_hit_tamed;
    int trac_hit_mons;
    int tracer;
    int trac_targ_x, trac_targ_y;

    /* If a monster fired it, which monster? */
    int beam_source;

    char beam_name[40];
};


struct player
{
    char turn_is_over;          // flag signaling that player has performed a timed action

    unsigned char prev_targ;

    char your_name[kNameLen];

    unsigned char species;
    char run_x;
    char run_y;
    char running;

    char special_wield;
    char deaths_door;
    char shock_shield;

    double elapsed_time;        // total amount of elapsed time in the game

    unsigned char synch_time;   // amount of time to wait before calling handle_time

    unsigned char disease;

    char max_level;

    int x_pos;
    int y_pos;

    int hunger;
    // char hunger_inc;         // this is now handled by player_hunger_rate
    char equip[NO_EQUIP];
    /* list in player.cc */

    int hp;
    int hp_max;
    int base_hp;                // this is what is affected by necrophages etc

    int base_hp2;
    int magic_points;
    int max_magic_points;
    int base_magic_points;
    int base_magic_points2;

    char strength;
    char intel;
    char dex;
    char max_strength;
    char max_intel;
    char max_dex;

    /* xxx_ch variables : determine whether new value is printed next turn.
       Set to 1 if value is changed. */
    char redraw_hunger;
    char hunger_state;          /* normal */
    char redraw_burden;
    char redraw_hit_points;
    char redraw_magic_points;
    char redraw_strength;
    char redraw_intelligence;
    char redraw_dexterity;
    char redraw_experience;
    char redraw_armor_class;    /* remember that the AC shown = 10 - AC */
    char redraw_gold;
    char redraw_evasion;

    unsigned char hit_points_regeneration;
    unsigned char magic_points_regeneration;

    unsigned long experience;
    int experience_level;
    unsigned int gold;
    int char_class;
    char class_name[30];
    char speed;
    int time_taken;

    char shield_blocks;         // number of shield blocks since last action
                                // this field is transient, its not saved
                                // out but is set to zero at the start of the
                                // input loop.

    unsigned char inv_class[52];
    unsigned char inv_type[52];
    unsigned char inv_plus[52];
    unsigned char inv_plus2[52];
    unsigned char inv_dam[52];
    unsigned char inv_ident[52];
    char inv_colour[52];
    int inv_quantity[52];
    char num_inv_items;         // number of items carried.

    int burden;
    char burden_state;
    unsigned char spells[25];
    char spell_no;
    // char spell_levels;       // handled in player.cc now
    unsigned char char_direction;
    // 0 = going down
    // 1 = going up!
    unsigned char pet_target;

    int your_level;

    int duration[30];           // lots of durational things. Why didn't I do this for haste etc right from the start? Oh well.
    /* There's a list of these in player.cc */

    int invis;
    int conf;
    int paralysis;
    int slow;
    int haste;
    int might;
    int levitation;             // levitation

    int poison;
    int rotting;
    int berserker;

    int exhausted;              // fatigue counter for berserk
    int berserk_penalty;        // pelnalty for moving while berserk

    unsigned char attribute[30];  // various attributes, eg resist lightning
    /* list in player.cc */

    char is_undead;
    /* 0 - is alive
       1 - is undead, but can still eat/drink etc (eg vampire)
       2 - is undead (mummy etc) */

    char delay_doing;
    char delay_t;

    unsigned char skills[50];
    unsigned char practise_skill[50];
    unsigned int skill_points[50];
    int exp_available;

    unsigned char item_description[5][50];
    char unique_items[50];
    char unique_creatures[50];
    char level_type;

    char where_are_you;

    unsigned char branch_stairs[30];

    char religion;
    unsigned char piety;
    unsigned char gods[100];

    unsigned char mutation[100];
    unsigned char demon_pow[100];
    unsigned char magic_contamination;

    unsigned char had_item[50];

    unsigned char betrayal;
};

extern struct player you;


struct monsters
{
    int type;
    int hit_points;
    int max_hit_points;
    unsigned char hit_dice;
    int armor_class;
    char evasion;
    unsigned char speed;
    unsigned char speed_increment;
    unsigned char x;
    unsigned char y;
    unsigned char target_x;
    unsigned char target_y;
    int inv[8];
    unsigned char behavior;
    unsigned char number;
    unsigned char monster_foe;
    unsigned char enchantment[3];
    unsigned char enchantment1;
};


struct item_struct
{
    unsigned char base_type[ITEMS];     /* basic class (eg weapon) */
    unsigned char sub_type[ITEMS];      /* type within that class (eg dagger) */
    unsigned char pluses[ITEMS];        /* hit+, charges */
    unsigned char pluses2[ITEMS];       /* dam+ etc */
    unsigned char special[ITEMS];       /* special stuff */
    unsigned int quantity[ITEMS];       /* multiple items */
    unsigned char x[ITEMS];     /*  x-location */
    unsigned char y[ITEMS];     /* y-location */
    unsigned char colour[ITEMS];        /* colour */
    unsigned char id[ITEMS];    /* identification */
    unsigned int link[ITEMS];   /* next item in stack */
};


struct environ
{
    item_struct it[1];
    monsters mons[MNST];
    unsigned char grid[GXM][GYM];
    unsigned char mgrid[GXM][GYM];
    int igrid[GXM][GYM];
    unsigned char map[GXM][GYM];
    char cgrid[GXM][GYM];
    unsigned int show[19][19];
    unsigned char show_col[19][19];
    unsigned char rock_colour;
    unsigned char floor_colour;

    unsigned char cloud_x[CLOUDS];
    unsigned char cloud_y[CLOUDS];
    unsigned char cloud_type[CLOUDS];
    int cloud_decay[CLOUDS];
    unsigned char cloud_no;

    unsigned char keeper_name[5][3];
    unsigned char sh_x[5];
    unsigned char sh_y[5];
    unsigned char sh_greed[5];
    unsigned char sh_type[5];
    unsigned char sh_level[5];

    unsigned char trap_type[NTRAPS];
    unsigned char trap_x[NTRAPS];
    unsigned char trap_y[NTRAPS];
    int mons_alloc[20];
    int trap_known;
};

extern struct environ env;      // make this global to main module


struct ghost_struct
{
    char name[20];
    unsigned char values[20];
};

extern struct ghost_struct ghost;


extern void (*viewwindow) (char, bool);


struct system_environment
{
    char  *crawl_name;
    char  *crawl_pizza;
    char  *crawl_rc;
    char  *crawl_dir;
};

extern system_environment  sys_env;

#endif // EXTERNS_H

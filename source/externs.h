/*
 *  File:       externs.h
 *  Summary:    Fixed size 2D vector class that asserts if you do something bad.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 * <3>     7/29/00    JDJ   Renamed sh_x, sh_y, sh_greed, sh_type, sh_level so
 *                          they start with shop.
 * <2>     7/29/00    JDJ   Switched to using bounds checked array classes.
 *                          Made a few char arrays unsigned char arrays.
 */

#ifndef EXTERNS_H
#define EXTERNS_H

#include "defines.h"
#include "enum.h"
#include "FixAry.h"
#include "message.h"

extern char info[200];               // defined in acr.cc {dlb}
extern char st_prn[20];              // defined in acr.cc {dlb}
extern char str_pass[80];            // defined in acr.cc {dlb}
extern unsigned char show_green;     // defined in view.cc {dlb}
extern FixedVector<unsigned char, 1000> mcolour;  // defined in mon-util.cc -- w/o this screen redraws *really* slow {dlb}

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

// output from direction() function:
struct dist
{
    bool isValid;       // valid target chosen?
    bool isTarget;      // target (true), or direction (false)?
    bool isMe;          // selected self (convenience: tx == you.x_pos,
                        // ty == you.y_pos)
    bool isCancel;      // user cancelled (usually <ESC> key)
    int  tx,ty;         // target x,y or logical extension of beam to map edge
    int  dx,dy;         // delta x and y if direction - always -1,0,1

    // internal use - ignore
    int  prev_target;   // previous target
};

struct bolt
{
    // INPUT parameters set by caller
    int range;                  // minimum range
    int rangeMax;               // maximum range
    int type;                   // missile gfx
    int colour;
    int flavour;
    int source_x, source_y;     // beam origin
    int damage, ench_power, hit;
    int target_x, target_y;     // intended target
    char thrower;               // what kind of thing threw this?
    char ex_size;               // explosion radius (0==none)
    int beam_source;            // NON_MONSTER or monster index #
    char beam_name[40];
    bool isBeam;                // beams? (can hits multiple targets?)


    // OUTPUT parameters (tracing, ID)
    bool obviousEffect;         // did an 'obvious' effect happen?
    int fr_count, foe_count;    // # of times a friend/foe is "hit"
    int fr_power, foe_power;    // total levels/hit dice affected

    // INTERNAL use - should not usually be set outside of beam.cc
    bool isTracer;              // is this a tracer?
    bool aimedAtFeet;           // this was aimed at self!
    bool msgGenerated;          // an appropriate msg was already mpr'd
    bool isExplosion;           // explosion phase (as opposed to beam phase)
    bool smartMonster;          // tracer firer can guess at other mons. resists?
    bool canSeeInvis;           // tracer firer can see invisible?
    bool isFriendly;            // tracer firer is enslaved or pet
    int  foeRatio;              // 100* foe ratio (see mons_should_fire())
};

struct player
{
  char turn_is_over; // flag signaling that player has performed a timed action

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

  unsigned char synch_time;   // amount to wait before calling handle_time

  unsigned char disease;

  char max_level;

  int x_pos;
  int y_pos;

  int hunger;
  FixedVector<char, NUM_EQUIP> equip;

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

  char redraw_hunger;
  char hunger_state;

  char redraw_burden;
  char redraw_hit_points;
  char redraw_magic_points;
  char redraw_strength;
  char redraw_intelligence;
  char redraw_dexterity;
  char redraw_experience;
  char redraw_armor_class;   // great -- more mixed american/proper spelling

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

  FixedVector<unsigned char, ENDOFPACK> inv_class;
  FixedVector<unsigned char, ENDOFPACK> inv_type;
  FixedVector<unsigned char, ENDOFPACK> inv_plus;
  FixedVector<unsigned char, ENDOFPACK> inv_plus2;
  FixedVector<unsigned char, ENDOFPACK> inv_dam;
  FixedVector<unsigned char, ENDOFPACK> inv_ident;
  FixedVector<unsigned char, ENDOFPACK> inv_colour;
  FixedVector<int, ENDOFPACK> inv_quantity;
//  num_inv_items is deprecated -- use inv_count() instead!
//  char num_inv_items;                    // number of items carried

  int burden;
  char burden_state;
  FixedVector<unsigned char, 25> spells;
  char spell_no;
  unsigned char char_direction;          //

  unsigned char pet_target;

  int your_level; // offset by one (-1 == 0, 0 == 1, etc.) for display

  FixedVector<int, NUM_DURATIONS> duration; // durational things. Why didn't I do this for haste etc right from the start? Oh well.

  int invis;
  int conf;
  int paralysis;
  int slow;
  int haste;
  int might;
  int levitation;

  int poison;
  int rotting;
  int berserker;

  int exhausted;                      // fatigue counter for berserk

  int berserk_penalty;                // pelnalty for moving while berserk

  FixedVector<unsigned char, 30> attribute;        // see ATTRIBUTES in enum.h

  char is_undead;                     // see UNDEAD_STATES in enum.h

  char delay_doing;
  char delay_t;

  FixedVector<unsigned char, 50>  skills;
  FixedVector<unsigned char, 50>  practise_skill;
  FixedVector<unsigned int, 50>  skill_points;
  int exp_available;

  FixedArray<unsigned char, 5, 50> item_description;
  FixedVector<unsigned char, 50> unique_items;
  FixedVector<unsigned char, 50> unique_creatures;
  char level_type;

  char where_are_you;

  FixedVector<unsigned char, 30> branch_stairs;

  char religion;
  unsigned char piety;
  unsigned char gift_timeout;
  FixedVector<unsigned char, 100> penance;

  FixedVector<unsigned char, 100> mutation;
  FixedVector<unsigned char, 100> demon_pow;
  unsigned char magic_contamination;

  char confusing_touch;
  char sure_blade;

  FixedVector<unsigned char, 50> had_item;

  unsigned char betrayal;
  unsigned char normal_vision;        // how far the species gets to see
  unsigned char current_vision;       // current sight radius (cells)

  unsigned char hell_exit;            // which level plyr goes to on hell exit.

  int passwall_x;       // coord for passwall, shouldn't need saving, since
  int passwall_y;       // the player should never get an action.
};

extern struct player you;


struct monsters
{
    int type;
    int hit_points;
    int max_hit_points;
    unsigned int hit_dice;
    int armor_class;       // great -- more mixed american/proper spelling
    int evasion;
    unsigned int speed;
    unsigned int speed_increment;
    unsigned char x;
    unsigned char y;
    unsigned char target_x;
    unsigned char target_y;
    FixedVector<int, 8> inv;
    unsigned char attitude;            // from MONS_ATTITUDE
    unsigned int behavior;
    unsigned int foe;
    FixedVector<unsigned int, NUM_MON_ENCHANTS> enchantment;
    unsigned char flags;               // bitfield of boolean flags
    unsigned int number;               // #heads (hydra), etc.
    int foe_memory;                    // how long to 'remember' foe x,y
                                       // once they go out of sight
};


struct item_struct // argh! why a struct of arrays and not an array of structs? {dlb}
{
  FixedVector<unsigned char, MAX_ITEMS> base_type; /* basic class (eg weapon) */
  FixedVector<unsigned char, MAX_ITEMS> sub_type;  /* type within that class (eg dagger) */
  FixedVector<unsigned char, MAX_ITEMS> pluses;       /* hit+, charges */
  FixedVector<unsigned char, MAX_ITEMS> pluses2;       /* dam+ etc */
  FixedVector<unsigned char, MAX_ITEMS> special;       /* special stuff */
  FixedVector<unsigned int, MAX_ITEMS> quantity;       /* multiple items */
  FixedVector<unsigned char, MAX_ITEMS> x;             /* x-location */
  FixedVector<unsigned char, MAX_ITEMS> y;             /* y-location */
  FixedVector<unsigned char, MAX_ITEMS> colour;        /* colour */
  FixedVector<unsigned char, MAX_ITEMS> id;            /* identification */
  FixedVector<unsigned int, MAX_ITEMS> link;           /* next item in stack */
};


struct environ
{
    item_struct it[1];
    FixedVector<monsters, MAX_MONSTERS> mons;
    FixedArray<unsigned char, GXM, GYM> grid;
    FixedArray<unsigned char, GXM, GYM> mgrid;
    FixedArray<int, GXM, GYM> igrid;
    FixedArray<unsigned char, GXM, GYM> map;
    FixedArray<unsigned char, GXM, GYM> cgrid;
    FixedArray<unsigned int, 19, 19> show;
    FixedArray<unsigned char, 19, 19> show_col;
    unsigned char rock_colour;
    unsigned char floor_colour;

    FixedVector<unsigned char, MAX_CLOUDS> cloud_x;
    FixedVector<unsigned char, MAX_CLOUDS> cloud_y;
    FixedVector<unsigned char, MAX_CLOUDS> cloud_type;
    FixedVector<int, MAX_CLOUDS> cloud_decay;
    unsigned char cloud_no;

    FixedArray<unsigned char, 5, 3> keeper_name;
    FixedVector<unsigned char, 5> shop_x;
    FixedVector<unsigned char, 5> shop_y;
    FixedVector<unsigned char, 5> shop_greed;
    FixedVector<unsigned char, 5> shop_type;
    FixedVector<unsigned char, 5> shop_level;

    FixedVector<unsigned char, MAX_TRAPS> trap_type;
    FixedVector<unsigned char, MAX_TRAPS> trap_x;
    FixedVector<unsigned char, MAX_TRAPS> trap_y;
    FixedVector<int, 20> mons_alloc;
    int trap_known;
    double elapsed_time;             // used during level load
};

extern struct environ env;


struct ghost_struct
{
    char name[20];
    FixedVector<unsigned char, 20> values;
};


extern struct ghost_struct ghost;


extern void (*viewwindow) (char, bool);


struct system_environment
{
    char *crawl_name;
    char *crawl_pizza;
    char *crawl_rc;
    char *crawl_dir;
    char *home;
};

extern system_environment SysEnv;


struct game_options {
    long        autopickups;    // items to autopickup
    bool        verbose_dump;   // make character dumps contain more detail
    bool        colour_map;     // add colour to the map
    bool        clean_map;      // remove unseen clouds/monsters
    bool        show_uncursed;  // label known uncursed items as "uncursed"
    bool        always_greet;   // display greeting message when reloading
    bool        easy_open;      // open doors with movement
    bool        easy_armour;    // allow auto-removing of armour
    bool        easy_butcher;   // open doors with movement
    int         easy_confirm;   // make yesno() confirming easier
    int         colour[16];     // macro fg colours to other colours
    int         background;     // select default background colour
    int         channels[NUM_MESSAGE_CHANNELS];  // msg channel colouring
    int         weapon;         // auto-choose weapon for character
    int         chaos_knight;   // choice of god for Chaos Knights (Xom/Makleb)
    int         death_knight;   // choice of god/necromancy for Death Knights
    int         priest;         // choice of god for priests (Zin/Yred)
    bool        random_pick;    // randomly generate character
    int         hp_warning;     // percentage hp for danger warning
    char        race;           // preselected race
    char        cls;            // preselected class
    int         sc_entries;     // # of score entries
};

extern game_options  Options;

struct tagHeader {
    short tagID;
    long offset;
};

struct scorefile_entry {
    char version;
    char release;
    long points;
    char name[kNameLen];
    long uid;                // for multiuser systems
    char race;
    char cls;
    char race_class_name[5]; // overrides race & cls if non-null
    char lvl;                // player level.
    char best_skill;         // best skill #
    char best_skill_lvl;     // best skill level
    int death_type;
    int death_source;       // 0 or monster TYPE
    int mon_num;            // sigh...
    char death_source_name[40];    // overrides death_source
    char dlvl;               // dungeon level (relative)
    char level_type;         // what kind of level died on..
    char branch;             // dungeon branch
    int final_hp;
    char wiz_mode;
};

#endif // EXTERNS_H

/*
 *  File:       externs.h
 *  Summary:    Various game structures
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

#include <queue>
#include <vector>
#include <string>

#include <time.h>

#include "defines.h"
#include "enum.h"
#include "FixAry.h"
#include "Kills.h"
#include "message.h"

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

struct coord_def
{
    int         x;
    int         y;

    // coord_def( int x_in = 0, int y_in = 0 ) : x(x_in), y(y_in) {};
    bool operator == (const coord_def &other) const
    {
        return (x == other.x && y == other.y);
    }

    bool operator != (const coord_def &other) const
    {
        return (x != other.x || y != other.y);
    }
};

struct dice_def
{
    int         num;
    int         size;

    dice_def( int n = 0, int s = 0 ) : num(n), size(s) {}
};

// output from direction() function:
struct dist
{
    bool isValid;       // valid target chosen?
    bool isTarget;      // target (true), or direction (false)?
    bool isMe;          // selected self (convenience: tx == you.x_pos,
                        // ty == you.y_pos)
    bool isCancel;      // user cancelled (usually <ESC> key)
    int  tx, ty;        // target x,y or logical extension of beam to map edge
    int  dx, dy;        // delta x and y if direction - always -1,0,1
    bool dir;           // represents a direction, not a specific target

    // internal use - ignore
    int  prev_target;   // previous target
};


struct bolt
{
    // INPUT parameters set by caller
    int         range;                 // minimum range
    int         rangeMax;              // maximum range
    int         type;                  // missile gfx
    int         colour;
    int         flavour;
    int         source_x, source_y;    // beam origin
    dice_def    damage;
    int         ench_power;
    int         hit;
    int         target_x, target_y;    // intended target
    int         thrower;               // what kind of thing threw this?
    int         ex_size;               // explosion radius (0==none)
    int         beam_source;           // NON_MONSTER or monster index #
    char        name[ITEMNAME_SIZE];   // name of beam
    bool        is_beam;               // beams? (can hits multiple targets?)
    bool        is_explosion;          // beam explodes at endpoint
    bool        is_big_cloud;          // expands into big_cloud at endpoint
    bool        is_enchant;            // no block/dodge, but mag resist
    bool        is_energy;             // mostly energy/non-physical attack
    bool        is_launched;           // was fired from launcher?
    bool        is_thrown;             // was thrown from hand?
    bool        target_first;          // targeting by direction
    const char *aux_source;            // source of KILL_MISC beams

    // OUTPUT parameters (tracing, ID)
    bool        obvious_effect;         // did an 'obvious' effect happen?
    int         fr_count, foe_count;   // # of times a friend/foe is "hit"
    int         fr_power, foe_power;   // total levels/hit dice affected

    // INTERNAL use - should not usually be set outside of beam.cc
    bool        is_tracer;          // is this a tracer?
    bool        aimed_at_feet;      // this was aimed at self!
    bool        msg_generated;      // an appropriate msg was already mpr'd
    bool        in_explosion_phase; // explosion phase (as opposed to beam phase)
    bool        smart_monster;  // tracer firer can guess at other mons. resists?
    bool        can_see_invis;  // tracer firer can see invisible?
    bool        is_friendly;    // tracer firer is enslaved or pet
    int         foe_ratio;      // 100* foe ratio (see mons_should_fire())

    // A contstructor to try and fix some of the bugs that occur because
    // this struct never seems to be properly initialized.  Definition
    // is over in misc.cc for lack of a better place (short of inlining
    // it here).
    bolt();
};


struct run_check_dir
{
    unsigned char   grid;
    char            dx;
    char            dy;
};

struct delay_queue_item
{
    int             type;
    int             duration;
    int             parm1;
    int             parm2;
};

// XXX: note that source currently is only accurate in that it's either
// MHITYOU or something else (ie it doesn't track monster ids for poison, etc)
struct mon_enchant_def
{
    enchant_type    type;      // ENCH_ type
    int             source;    // MHITYOU, MHITNOT, monster id
    int             duration;  // in clock ticks
    int             work;      // partial work tracking
};

struct item_def
{
    unsigned char  base_type;  // basic class (ie OBJ_WEAPON)
    unsigned char  sub_type;   // type within that class (ie WPN_DAGGER)
    short          plus;       // +to hit, charges, corpse mon id
    short          plus2;      // +to dam, sub-sub type for boots and helms
    unsigned long  special;    // special stuff
    unsigned char  colour;     // item colour
    unsigned long  flags;      // item statuc flags
    short          quantity;   // number of items

    int            ra_props[ RAP_NUM_PROPERTIES ];

    short  x;          // x-location;         for inventory items = -1
    short  y;          // y-location;         for inventory items = -1
    short  link;       // link to next item
    short  slot;       // for inventory items = slot
};

struct player_def
{
  activity_type activity;   // The current multiturn activity,
                            // usually set to ACT_NONE

  bool turn_is_over; // flag signaling that player has performed a timed action

  unsigned char prev_targ;
  char your_name[kNameLen];

  unsigned char species;

  char run_x;
  char run_y;
  FixedVector< run_check_dir, 3 > run_check; // array of grids to check
  signed char running;
  bool stepping;

  char special_wield;
  char deaths_door;
  char fire_shield;

  double elapsed_time;        // total amount of elapsed time in the game
  unsigned char synch_time;   // amount to wait before calling handle_time

  int x_pos;
  int y_pos;

  int hunger;
  FixedVector<char, NUM_EQUIP> equip;

  int hp;
  int hp_max;
  int base_hp;                // temporary max HP loss (rotting)
  int base_hp2;               // base HPs from levels (and permanent loss)

  int magic_points;
  int max_magic_points;
  int base_magic_points;      // temporary max MP loss? (currently unused)
  int base_magic_points2;     // base MPs from levels and potions of magic

  char str;
  char intel;
  char dex;
  char max_str;
  char max_intel;
  char max_dex;

  char hunger_state;

  bool wield_change;          // redraw weapon

  unsigned long redraw_status_flags;

  unsigned char symbol;
  unsigned char colour;
  unsigned char flash_colour;

  bool autopickup_on;
  int stealth_cache;
  FixedVector< char, NUM_STATUE_TYPES >  visible_statue;

  unsigned char hit_points_regeneration;
  unsigned char magic_points_regeneration;

  unsigned long xp;
  int xp_level;
  int max_xp_level;

  unsigned int gold;

  int char_class;
  char class_name[30];

  int time_taken;

  short shield_blocks;         // number of shield blocks since last action

  FixedVector< item_def, ENDOFPACK > inv;

  short nemelex_altar_index;

  int burden;
  char burden_state;
  FixedVector<unsigned char, 25>  spells;
  char spell_no;
  unsigned char char_direction;

  unsigned char pet_target;

  int depth; // offset by one (-1 == 0, 0 == 1, etc.) for display

  // durational things. Why didn't I do this for haste etc
  // right from the start? Oh well.
  FixedVector<int, NUM_DURATIONS> duration;

  int disease;
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

  FixedVector<unsigned char, 30>  attribute;        // see ATTRIBUTES in enum.h

  char is_undead;                     // see UNDEAD_STATES in enum.h

  std::queue< delay_queue_item >  delay_queue;  // pending actions
  int                             delay_work;   // current work counter

  FixedVector<unsigned char, MAX_SKILLS>  skills;
  FixedVector<unsigned char, MAX_SKILLS>  practise_skill;
  FixedVector<unsigned int,  MAX_SKILLS>   skill_points;
  FixedVector<unsigned char, MAX_SKILLS>  skill_order;
  int   skill_cost_level;
  long  total_skill_points;
  int   exp_available;

  FixedArray<long, NUM_IDESC, MAX_SUBTYPES>  item_description;

  FixedVector<unsigned char, MAX_UNIQ_ITEMS> unique_items;
  FixedVector<unsigned char, MAX_UNIQ_MONS>  unique_creatures;

  Kills     kills;

  char      level_type;
  char      where_are_you;

  FixedVector<unsigned char, 30>  branch_stairs;

  char religion;
  unsigned char piety;
  unsigned char gift_timeout;
  FixedVector<unsigned char, MAX_NUM_GODS>  penance;
  FixedVector<unsigned char, MAX_NUM_GODS>  worshipped;
  FixedVector<unsigned short, MAX_NUM_GODS>  num_gifts;

  FixedVector<unsigned char, MAX_MUTATIONS> mutation;
  FixedVector<unsigned char, MAX_MUTATIONS> demon_pow;
  int  magic_contamination;

  char confusing_touch;
  char sure_blade;

  FixedVector<unsigned char, MAX_SUBTYPES>  had_book;

  unsigned char betrayal;
  unsigned char normal_vision;        // how far the species gets to see
  unsigned char current_vision;       // current sight radius (cells)

  unsigned char hell_exit;            // which level plyr goes to on hell exit.

  // This field is here even in non-WIZARD compiles, since the
  // player might have been playing previously under wiz mode.
  bool          wizard;               // true if player has entered wiz mode.
  time_t        birth_time;           // start time of game

  time_t        start_time;           // start time of session
  long          real_time;            // real time played (in seconds)
  long          num_turns;            // number of turns taken

  int           old_hunger;  // used for hunger delta-meter (see output.cc)

  bool          naked_okay;  // used for ignoring body armour checks on run

  // Warning: these two are quite different.
  //
  // The spell table is an index to a specific spell slot (you.spells).
  // The ability table lists the ability (ABIL_*) which prefers that letter.
  //
  // In other words, the spell table contains hard links and the ability
  // table contains soft links.
  FixedVector<int, 52>  spell_letter_table;   // ref to spell by slot
  FixedVector<int, 52>  ability_letter_table; // ref to ability by enum
};

struct monsters
{
    int                 type;
    int                 hit_points;
    int                 max_hit_points;
    int                 hit_dice;
    int                 armour_class;
    int                 evasion;

    unsigned int        speed;
    unsigned int        energy;
    unsigned int        wait;
    unsigned int        shield_blocks;

    int                 x;
    int                 y;
    int                 target_x;
    int                 target_y;

    FixedVector<int, 8> inv;
    FixedVector<int, 6> spells;

    unsigned long       flags;          // bitfield of boolean flags
    unsigned char       attitude;       // from MONS_ATTITUDE
    beh_type            behaviour;
    int                 foe;
    int                 foe_memory;     // how long to 'remember' foe x,y
                                        // once they go out of sight

    int                 colour;
    unsigned int        number;         // #heads (hydra), etc.

    FixedVector< mon_enchant_def, NUM_MON_ENCHANTS >  ench;
};

struct cloud_struct
{
    unsigned char       x;
    unsigned char       y;
    unsigned char       type;
    short               decay;
};

struct shop_struct
{
    unsigned char       x;
    unsigned char       y;
    unsigned char       greed;
    unsigned char       type;
    unsigned char       level;

    char keeper_name[ kNameLen ];
};

struct trap_struct
{
    int                 x;
    int                 y;
    int                 type;
    int                 ammo;
};

struct ghost_struct
{
    char name[ kNameLen ];
    FixedVector< short, NUM_GHOST_VALUES > values;
};

struct crawl_environment
{
    unsigned char rock_colour;
    unsigned char floor_colour;

    FixedVector< item_def, MAX_ITEMS >       item;  // item list
    FixedVector< monsters, MAX_MONSTERS >    mons;  // monster list

    FixedArray< unsigned char, GXM, GYM >    grid;  // terrain grid
    FixedArray< unsigned char, GXM, GYM >    mgrid; // monster grid
    FixedArray< int, GXM, GYM >              igrid; // item grid
    FixedArray< unsigned char, GXM, GYM >    cgrid; // cloud grid

    FixedArray< unsigned short, GXM, GYM >   map;   // discovered terrain

    // These two are really low level, so I'm trying to keep
    // them mostly isolated in view.cc.
    FixedArray< unsigned int, 19, 19>        show;      // view window char
    FixedArray< unsigned short, 19, 19>      show_col;  // view window colour

    FixedVector< cloud_struct, MAX_CLOUDS >  cloud; // cloud list
    unsigned char cloud_no;

    FixedVector< shop_struct, MAX_SHOPS >    shop;  // shop list
    FixedVector< trap_struct, MAX_TRAPS >    trap;  // trap list

    FixedVector< int, 20 >   mons_alloc;
    int                      trap_known;
    double                   elapsed_time; // used during level load

    struct ghost_struct      ghost;        // ghost/demonlord definition

    // tracks which level files have been written:
    FixedArray< bool, MAX_LEVELS, MAX_BRANCHES >           level_files;

    // these are used for the overmap feature:
    FixedArray< unsigned char, MAX_LEVELS, MAX_BRANCHES >  altars_present;
    FixedVector< char, MAX_BRANCHES >                      stair_level;
    FixedArray< unsigned char, MAX_LEVELS, MAX_BRANCHES >  feature;
};

struct system_environment
{
    char *crawl_name;
    char *crawl_pizza;
    char *crawl_rc;
    char *crawl_macro;
    char *crawl_dir;
    char *crawl_morgue;
    int   lines;
    char *home;                 // only used by MULTIUSER systems
    bool  board_with_nail;      // Easter Egg silliness
};

struct message_filter
{
    int         channel;        // Use -1 to match any channel.
    std::string substring;      // Use empty string to match against channel
                                // alone.

    message_filter( int ch, const std::string &s )
        : channel(ch), substring(s)
    { }

    message_filter( const std::string &s ) : channel(-1), substring(s) { }

    bool is_filtered( int ch, const std::string &s ) const
    {
        bool channel_match = (ch == channel || channel == -1);

        if (!channel_match || substring.length() == 0)
            return (channel_match);

        return (s.find(substring) != std::string::npos);
    }
};


struct game_options
{
    long        autopickups;    // items to autopickup
    bool        autopickup_on_look; // ';' causes autopickup
    bool        verbose_dump;   // make character dumps contain more detail
    bool        detailed_stat_dump;  // add detailed stats and resist info.
    bool        clean_map;      // remove unseen clouds/monsters
    bool        colour_map;     // add colour to the map
    int         colour_rotten;  // change rotten meat to colour
    bool        show_uncursed;  // label known uncursed items as "uncursed"
    bool        invent_weights; // show weights in inventory
    bool        invent_colours; // show colours in inventory
    bool        always_greet;   // display greeting message when reloading
    bool        easy_open;      // open doors with movement
    bool        easy_armour;    // allow auto-removing of armour
    bool        easy_butcher;   // open doors with movement
    bool        easy_reach;     // reaching works automatic with CTRL-move
    int         easy_confirm;   // make yesno() confirming easier
    int         easy_quit_item_prompts; // make item prompts quitable on space
    FixedVector<int, 16>  colour;  // macro fg colours to other colours
    int         background;     // select default background colour
    FixedVector<int, NUM_MESSAGE_CHANNELS> channels;  // msg channel colouring
    int         weapon;         // auto-choose weapon for character
    int         chaos_knight;   // choice of god for Chaos Knights (Xom/Makleb)
    int         death_knight;   // choice of god/necromancy for Death Knights
    int         priest;         // choice of god for priests (Zin/Yred)
    bool        random_pick;    // randomly generate character
    int         hp_warning;     // percentage hp for danger warning
    int         hp_attention;   // percentage hp for danger attention
    char        race;           // preselected race
    char        cls;            // preselected class
    bool        terse_hand;     // use terse description for wielded item
    bool        delay_message_clear; // avoid clearing messages each turn
    unsigned int friend_brand;  // Attribute for branding friendly monsters
    bool        no_dark_brand;  // Attribute for branding friendly monsters

    bool        colourful_corpses;  // corpse colour based off mon, not species
    bool        prompt_move_modifiers;   // prompts for * / and : commands

    bool        old_skill_countdown;  // use old skill countdown
    bool        skill_countdown;      // countdown/up to next skill level

    int         fire_items_start; // index of first item for fire command
    FixedVector<int, NUM_FIRE_TYPES>  fire_order; // order for 'f' command

    bool        auto_list;      // automatically jump to appropriate item lists

    FixedVector<bool, NUM_FLUSH_REASONS> flush_input;// when to flush input buff
    bool        lowercase_invocations;          // prefer lowercase invocations

    FixedVector<int, NUM_OBJECT_CLASSES> list_order; // inventory class order
    int         number_walk_items;         // #items to display on walkover

    char_set_type  char_set;
    FixedVector<unsigned char, NUM_DCHAR_TYPES> char_table;

    std::vector<std::string> banned_objects;  // Objects we'll never pick up
    bool        pickup_thrown;  // Pickup thrown missiles
    bool        pickup_dropped; // Pickup dropped objects
    int         travel_delay;   // How long to pause between travel moves

    std::vector<message_filter> stop_travel;  // Messages that stop travel

    int         stash_tracking; // How stashes are tracked

    bool        travel_colour;  // Colour levelmap using travel information?
    int         travel_stair_cost;

    bool        item_colour;    // Colour items on level map

    bool        explore_item_stop; // Stop exploring if a previously unseen
                                   // item comes into view

    std::vector< FixedVector<std::string, 2> > sound_mappings;

    int         dump_kill_places; // How to dump place information for kills.
    int         dump_message_count; // How many old messages to dump

    int         drop_mode;          // Controls whether single or multidrop
                                    // is the default.

    bool        easy_exit_menu;     // Menus are easier to get out of

    std::vector<std::string> drop_filter;

    FixedVector< unsigned int, ACT_ACTIVITY_COUNT > activity_interrupts;

#ifdef WIZARD
    int         wiz_mode;       // yes, no, never in wiz mode to start
#endif

    // internal use only:
    int         sc_entries;     // # of score entries
    int         sc_format;      // Format for score entries

};

struct tagHeader
{
    short tagID;
    long offset;
};

struct scorefile_entry
{
    int         version;
    int         release;
    long        points;
    char        name[kNameLen];
    long        uid;                // for multiuser systems
    int         race;
    int         cls;
    char        race_class_name[5]; // overrides race & cls if non-null
    int         lvl;                // player level.
    int         best_skill;         // best skill #
    int         best_skill_lvl;     // best skill level
    int         death_type;
    int         death_source;       // 0 or monster TYPE
    int         mon_num;            // sigh...
    char        death_source_name[40];    // overrides death_source
    char        auxkilldata[ITEMNAME_SIZE]; // weapon wielded, spell cast, etc
    char        dlvl;               // dungeon level (relative)
    char        level_type;         // what kind of level died on..
    char        branch;             // dungeon branch
    int         final_hp;           // actual current HPs (probably <= 0)
    int         final_max_hp;       // net HPs after rot
    int         final_max_max_hp;   // gross HPs before rot
    int         damage;             // damage of final attack
    int         str;                // final str (useful for nickname)
    int         intel;              // final int
    int         dex;                // final dex (useful for nickname)
    int         god;                // god
    int         piety;              // piety
    int         penance;            // penance
    bool        wiz_mode;           // character used wiz mode
    time_t      birth_time;         // start time of character
    time_t      death_time;         // end time of character
    long        real_time;          // real playing time in seconds
    long        num_turns;          // number of turns taken
    int         num_diff_runes;     // number of rune types in inventory
    int         num_runes;          // total number of runes in inventory
    long        skill_bonus_level;  // total skill points when Orb picked up
};

#endif // EXTERNS_H

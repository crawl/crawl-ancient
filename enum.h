/*
 *  File:       enum.h
 *  Summary:    Global (ick) enums.
 *  Written by: Daniel Ligon
 *
 *  Change History (most recent first):
 *
 *      <9>    10-Jan-2000      dlb    commented many things (line #s, etc.)
 *                                     added HUNGER_STATES
 *                                     added BURDEN_STATES
 *                                     added UNDEAD_STATES
 *                                     trivial clean-up
 *      <8>    11/04/99         cdl    added killed_by
 *      <7>     9/29/99         BCR    Added comments showing where uniques are
 *      <6>     9/25/99         CDL    Added commands
 *      <5>     9/9/99          BWR    Removed Great Swords skill
 *      <4>     8/6/99          BWR    added branch and level types
 *      <3>     6/2/99          DML    beams, clouds, ench, ms, kill,
 *                                     other minor changes
 *      <2>     5/26/99         JDJ    Added a header guard.
 *      <1>     -/--/--         CDL    Created
 */

#ifndef ENUM_H
#define ENUM_H


enum ABILITIES
{
    ABIL_SPIT_POISON = 1,                   //    1
    ABIL_MAPPING,
    ABIL_TELEPORTATION,
    ABIL_BREATHE_FIRE,
    ABIL_BLINK,
    ABIL_BREATHE_FROST,
    ABIL_BREATHE_POISON,
    ABIL_BREATHE_LIGHTNING,
    ABIL_SPIT_ACID,
    ABIL_BREATHE_POWER,                     //   10
    ABIL_GO_BERSERK_I,
    ABIL_BREATHE_STICKY_FLAME,
    ABIL_BREATHE_STEAM,
    ABIL_FLY,
    ABIL_SUMMON_MINOR_DEMON,
    ABIL_SUMMON_DEMON,
    ABIL_HELLFIRE,
    ABIL_TORMENT,
    ABIL_RAISE_DEAD,
    ABIL_CONTROL_DEMON,                     //   20
    ABIL_TO_PANDEMONIUM,
    ABIL_CHANNELING,
    ABIL_THROW_FLAME,
    ABIL_THROW_FROST,
    ABIL_BOLT_OF_DRAINING,                  //   25
    ABIL_TURN_INVISIBLE = 51,               //   51
    ABIL_TURN_VISIBLE,
    ABIL_LEVITATE,
    ABIL_STOP_LEVITATING,
    ABIL_END_TRANSFORMATION,                //   55
    ABIL_REPEL_UNDEAD_I = 110,              //  110
    ABIL_MINOR_HEALING,
    ABIL_PESTILENCE,
    ABIL_HOLY_WORD,
    ABIL_SUMMON_GUARDIAN,                   //  114
    ABIL_REPEL_UNDEAD_II = 120,             //  120
    ABIL_SMITING,
    ABIL_ANNIHILATE_UNDEAD,
    ABIL_THUNDERBOLT,
    ABIL_SUMMON_DAEVA,                      //  124
    ABIL_RECALL_UNDEAD_SLAVES = 130,        //  130
    ABIL_ENSLAVE_UNDEAD = 132,              //  132
    ABIL_INVOKE_DEATH,                      //  133
    ABIL_ANIMATE_CORPSE = 140,              //  140
    ABIL_RECALL_UNDEAD,
    ABIL_ANIMATE_DEAD,
    ABIL_DRAIN_LIFE,
    ABIL_CONTROL_UNDEAD,                    //  144
    ABIL_CHANNEL_ENERGY = 160,              //  160
    ABIL_MIGHT_I = 170,                     //  170
    ABIL_HEALING_I,
    ABIL_HASTE,                             //  172
    ABIL_MINOR_DESTRUCTION = 180,           //  180
    ABIL_LESSER_SERVANT_OF_MAKHLEB,
    ABIL_MAJOR_DESTRUCTION,
    ABIL_GREATER_SERVANT_OF_MAKHLEB,        //  183
    ABIL_FORGET_SPELL = 190,                //  190
    ABIL_GO_BERSERK_II = 200,               //  200
    ABIL_MIGHT_II,
    ABIL_HASTE_SELF,                        //  202
    ABIL_LESSER_HEALING = 220,              //  220
    ABIL_PURIFICATION,
    ABIL_HEALING_II,
    ABIL_RESTORATION,
    ABIL_GREATER_HEALING,                   //  224
    ABIL_RENOUNCE_RELIGION = 250            //  250
};

enum AMMUNITION_DESCRIPTIONS
{
    DAMMO_ORCISH = 3,                       //    3
    DAMMO_ELVEN,
    DAMMO_DWARVEN                           //    5
};

enum ARMOUR
{
    ARM_ROBE,                               //    0
    ARM_LEATHER_ARMOUR,
    ARM_RING_MAIL,
    ARM_SCALE_MAIL,
    ARM_CHAIN_MAIL,
    ARM_SPLINT_MAIL,                        //    5
    ARM_BANDED_MAIL,
    ARM_PLATE_MAIL,
    ARM_SHIELD,
    ARM_CLOAK,
    ARM_HELMET,                             //   10
    ARM_GLOVES,
    ARM_BOOTS,
    ARM_BUCKLER,
    ARM_LARGE_SHIELD,
    ARM_DRAGON_HIDE,                        //   15
    ARM_TROLL_HIDE,
    ARM_CRYSTAL_PLATE_MAIL,
    ARM_DRAGON_ARMOUR,
    ARM_TROLL_LEATHER_ARMOUR,
    ARM_ICE_DRAGON_HIDE,                    //   20
    ARM_ICE_DRAGON_ARMOUR,
    ARM_STEAM_DRAGON_HIDE,
    ARM_STEAM_DRAGON_ARMOUR,
    ARM_MOTTLED_DRAGON_HIDE,
    ARM_MOTTLED_DRAGON_ARMOUR,              //   25
    ARM_STORM_DRAGON_HIDE,
    ARM_STORM_DRAGON_ARMOUR,
    ARM_GOLD_DRAGON_HIDE,
    ARM_GOLD_DRAGON_ARMOUR,
    ARM_ANIMAL_SKIN,                        //   30
    ARM_SWAMP_DRAGON_HIDE,
    ARM_SWAMP_DRAGON_ARMOUR
};

enum ARMOUR_DESCRIPTIONS
{
    DARM_EMBROIDERED_SHINY = 1,             //    1
    DARM_RUNED,
    DARM_GLOWING,
    DARM_ELVEN,
    DARM_DWARVEN,                           //    5
    DARM_ORCISH
};

enum ARMOUR_PROPERTIES
{
    PARM_AC,                                //    0
    PARM_EVASION
};

enum ATTRIBUTES
{
    ATTR_DIVINE_LIGHTNING_PROTECTION,       //    0
    ATTR_SPEC_AIR,
    ATTR_SPEC_EARTH,
    ATTR_CONTROL_TELEPORT,
    ATTR_WALK_SLOWLY,
    ATTR_TRANSFORMATION,                    //    5
    ATTR_CARD_COUNTDOWN,
    ATTR_CARD_TABLE,
    ATTR_NUM_DEMONIC_POWERS
};

enum BEAMS                                  // beam[].flavour
{
    BEAM_MISSILE,                           //    0
    BEAM_MMISSILE,                          // used for similarly unresistable things
    BEAM_FIRE,
    BEAM_COLD,
    BEAM_MAGIC,
    BEAM_ELECTRICITY,                       //    5
    BEAM_POISON,
    BEAM_NEG,
    BEAM_CLOUD,                             //    8
    BEAM_EXPLOSION = 10,                    //   10
    BEAM_SPORE,                             //   11
    BEAM_HELLFIRE = 13,                     //   13: found 11jan2000 {dlb}
    BEAM_HOLY = 18,                         //   18: aka beam of cleansing, golden flame
    BEAM_FRAG,
    BEAM_LAVA,                              //   20
    BEAM_ICE = 23,                          //   23
    BEAM_NUKE = 27                          //   27
};

enum BOOKS
{
    BOOK_MINOR_MAGIC_I,                     //    0
    BOOK_MINOR_MAGIC_II,
    BOOK_MINOR_MAGIC_III,
    BOOK_CONJURATIONS_I,
    BOOK_CONJURATIONS_II,
    BOOK_FLAMES,
    BOOK_FROST,
    BOOK_INVOCATIONS,
    BOOK_FIRE,
    BOOK_ICE,
    BOOK_SURVEYANCES,                       //   10
    BOOK_SPATIAL_TRANSLOCATIONS,
    BOOK_ENCHANTMENTS,
    BOOK_YOUNG_POISONERS,
    BOOK_TEMPESTS,
    BOOK_DEATH,
    BOOK_HINDERANCE,
    BOOK_CHANGES,
    BOOK_TRANSFIGURATIONS,
    BOOK_PRACTICAL_MAGIC,
    BOOK_WAR_CHANTS,                        //   20
    BOOK_CLOUDS,
    BOOK_HEALING,
    BOOK_NECROMANCY,
    BOOK_NECRONOMICON,
    BOOK_SUMMONINGS,
    BOOK_CHARMS,
    BOOK_DEMONOLOGY,
    BOOK_AIR,
    BOOK_SKY,
    BOOK_DIVINATIONS,                       //   30
    BOOK_WARP,
    BOOK_TOXINS,
    BOOK_ANNIHILATIONS,
    BOOK_UNLIFE,
    BOOK_DESTRUCTION,
    BOOK_CONTROL,
    BOOK_MUTATIONS,
    BOOK_TUKIMA,
    BOOK_GEOMANCY,
    BOOK_EARTH,                             //   40
    BOOK_MANUAL,
    BOOK_WIZARDRY,
    BOOK_POWER,
    BOOK_CANTRIPS,                          //   44 - added 04jan2000 {jmf}
    BOOK_PARTY_TRICKS,                      //   45 - added 04jan2000 {jmf}
    NUM_BOOKS                               // must always be last member in enum {dlb}
};

enum BRANCHES                               // you.where_are_you
{
    BRANCH_MAIN_DUNGEON,                    //    0
    BRANCH_DIS,
    BRANCH_GEHENNA,
    BRANCH_VESTIBULE_OF_HELL,
    BRANCH_COCYTUS,
    BRANCH_TARTARUS,
    BRANCH_INFERNO,
    BRANCH_THE_PIT,                         //    7
    BRANCH_ORCISH_MINES = 10,               //   10
    BRANCH_HIVE,
    BRANCH_LAIR,
    BRANCH_SLIME_PITS,
    BRANCH_VAULTS,
    BRANCH_CRYPT,
    BRANCH_HALL_OF_BLADES,
    BRANCH_HALL_OF_ZOT,
    BRANCH_ECUMENICAL_TEMPLE,
    BRANCH_SNAKE_PIT,
    BRANCH_ELVEN_HALLS,                     //   20
    BRANCH_TOMB,
    BRANCH_SWAMP
};

enum BRANCH_STAIRS                          // you.branch_stairs[]
{
    STAIRS_ORCISH_MINES,                    //    0
    STAIRS_HIVE,
    STAIRS_LAIR,
    STAIRS_SLIME_PITS,
    STAIRS_VAULTS,
    STAIRS_CRYPT,
    STAIRS_HALL_OF_BLADES,
    STAIRS_HALL_OF_ZOT,
    STAIRS_ECUMENICAL_TEMPLE,
    STAIRS_SNAKE_PIT,
    STAIRS_ELVEN_HALLS,                     //   10
    STAIRS_TOMB,
    STAIRS_SWAMP
};

enum BURDEN_STATES                          // you.burden_state
{
    BS_UNENCUMBERED,                        //    0
    BS_ENCUMBERED = 2,                      //    2
    BS_OVERLOADED = 5                       //    5
};

enum CLOUD_TYPES                            // cloud_type[], place_cloud(), big_cloud()
{
    CLOUD_NONE,                             //    0
    CLOUD_FIRE,                             //    1
    CLOUD_STINK,                            //    2
    CLOUD_COLD,                             //    3
    CLOUD_POISON,                           //    4
    CLOUD_GREY_SMOKE = 5,                   //    5: found 11jan2000 {dlb}
    CLOUD_BLUE_SMOKE = 6,                   //    6: found 11jan2000 {dlb}
    CLOUD_PURP_SMOKE = 7,                   //    7: was: CLOUD_ENERGY and wrong 19jan2000 {dlb}
    CLOUD_STEAM,                            //    8
    CLOUD_MIASMA = 9,                       //    9: found 11jan2000 {dlb}
    CLOUD_BLACK_SMOKE = 10,                 //   10: was: CLOUD_STICKY_FLAME and wrong 19jan2000 {dlb}
    CLOUD_DEBUGGING = 99,                   //   99: used once as 'nonexistent cloud' {dlb}
// if env.cloud_type > 100, it is a monster's cloud {dlb}
    CLOUD_FIRE_MON = 101,                   //  101: found 11jan2000 {dlb}
    CLOUD_STINK_MON = 102,                  //  102: found 11jan2000 {dlb}
    CLOUD_COLD_MON = 103,                   //  103: added 11jan2000 {dlb}
    CLOUD_POISON_MON = 104,                 //  104
    CLOUD_GREY_SMOKE_MON = 105,             //  105: found 11jan2000 {dlb}
    CLOUD_BLUE_SMOKE_MON = 106,             //  106: found 11jan2000 {dlb}
    CLOUD_PURP_SMOKE_MON = 107,             //  107:
    CLOUD_STEAM_MON = 108,                  //  108: added 11jan2000 {dlb}
    CLOUD_MIASMA_MON = 109,                 //  109: added 11jan2000 {dlb}
    CLOUD_BLACK_SMOKE_MON = 110             //  110: added 19jan2000 {dlb}
};

enum COMMANDS
{
    CMD_NO_CMD = 1000,                      // 1000
    CMD_MOVE_NOWHERE,
    CMD_MOVE_LEFT,
    CMD_MOVE_DOWN,
    CMD_MOVE_UP,
    CMD_MOVE_RIGHT,
    CMD_MOVE_UP_LEFT,
    CMD_MOVE_DOWN_LEFT,
    CMD_MOVE_UP_RIGHT,
    CMD_MOVE_DOWN_RIGHT,
    CMD_RUN_LEFT,                           // 1000 +  10
    CMD_RUN_DOWN,
    CMD_RUN_UP,
    CMD_RUN_RIGHT,
    CMD_RUN_UP_LEFT,
    CMD_RUN_DOWN_LEFT,
    CMD_RUN_UP_RIGHT,
    CMD_RUN_DOWN_RIGHT,
    CMD_OPEN_DOOR_LEFT,
    CMD_OPEN_DOOR_DOWN,
    CMD_OPEN_DOOR_UP,                       // 1000 +  20
    CMD_OPEN_DOOR_RIGHT,
    CMD_OPEN_DOOR_UP_LEFT,
    CMD_OPEN_DOOR_DOWN_LEFT,
    CMD_OPEN_DOOR_UP_RIGHT,
    CMD_OPEN_DOOR_DOWN_RIGHT,
    CMD_OPEN_DOOR,
    CMD_CLOSE_DOOR,
    CMD_REST,
    CMD_GO_UPSTAIRS,
    CMD_GO_DOWNSTAIRS,                      // 1000 +  30
    CMD_TOGGLE_AUTOPICKUP,
    CMD_PICKUP,
    CMD_DROP,
    CMD_BUTCHER,
    CMD_INSPECT_FLOOR,
    CMD_EXAMINE_OBJECT,
    CMD_INVOKE,
    CMD_WIELD_WEAPON,
    CMD_WEAPON_SWAP,
    CMD_THROW,                              // 1000 +  40
    CMD_FIRE,
    CMD_WEAR_ARMOUR,
    CMD_REMOVE_ARMOUR,
    CMD_WEAR_JEWELLERY,
    CMD_REMOVE_JEWELLERY,
    CMD_LIST_WEAPONS,
    CMD_LIST_ARMOUR,
    CMD_LIST_JEWELLERY,
    CMD_ZAP_WAND,
    CMD_CAST_SPELL,                         // 1000 +  50
    CMD_MEMORISE_SPELL,
    CMD_USE_ABILITY,
    CMD_PRAY,
    CMD_EAT,
    CMD_QUAFF,
    CMD_READ,
    CMD_LOOK_AROUND,
    CMD_SEARCH,
    CMD_SHOUT,
    CMD_DISARM_TRAP,                        // 1000 +  60
    CMD_CHARACTER_DUMP,
    CMD_DISPLAY_COMMANDS,
    CMD_DISPLAY_INVENTORY,
    CMD_DISPLAY_KNOWN_OBJECTS,
    CMD_DISPLAY_MUTATIONS,
    CMD_DISPLAY_SKILLS,
    CMD_DISPLAY_MAP,
    CMD_DISPLAY_OVERMAP,
    CMD_DISPLAY_RELIGION,
    CMD_DISPLAY_CHARACTER_STATUS,           // 1000 +  70
    CMD_EXPERIENCE_CHECK,
    CMD_GET_VERSION,
    CMD_ADJUST_INVENTORY,
    CMD_REPLAY_MESSAGES,
    CMD_REDRAW_SCREEN,
    CMD_MACRO_ADD,
    CMD_MACRO_SAVE,
    CMD_SAVE_GAME,
    CMD_SAVE_GAME_NOW,
    CMD_SUSPEND_GAME,                       // 1000 +  80
    CMD_QUIT,
    CMD_WIZARD
};

enum CORPSES
{
    CORPSE_BODY,                            //    0
    CORPSE_SKELETON
};

enum DELAY
{
    DELAY_EAT,                              //    0
    DELAY_ARMOR_ON,
    DELAY_ARMOR_OFF,
    DELAY_MEMORIZE,
    DELAY_BUTCHER,
    DELAY_AUTOPICKUP
};

enum DEMON_BEAMS
{
    DMNBM_HELLFIRE,                         //    0
    DMNBM_SMITING,
    DMNBM_BRAIN_FEED,
    DMNBM_MUTATION
};

enum DEMON_CLASSES                          // summon_any_demon()
{
    DEMON_LESSER,                           //    0: Class V
    DEMON_COMMON,                           //    1: Class II-IV
    DEMON_GREATER                           //    2: Class I
};

enum DUNGEON_FEATURES                       // grd[][]
{
    DNGN_UNSEEN,                            //    0
    DNGN_ROCK_WALL,
    DNGN_STONE_WALL,
    DNGN_CLOSED_DOOR,
    DNGN_METAL_WALL,
    DNGN_SECRET_DOOR,
    DNGN_GREEN_CRYSTAL_WALL,
    DNGN_ORCISH_IDOL,
    DNGN_WAX_WALL,                          //    8
    DNGN_LAVA_X = 11,                       //   11
    DNGN_WATER_X,                           //   12
    DNGN_SILVER_STATUE = 21,                //   21
    DNGN_GRANITE_STATUE,
    DNGN_ORANGE_CRYSTAL_STATUE,             //   23
    DNGN_STATUE_35 = 35,                    //   35
    DNGN_STATUE_39 = 39,                    //   39
    DNGN_LAVA = 61,                         //   61
    DNGN_DEEP_WATER,                        //   62
    DNGN_SHALLOW_WATER = 65,                //   65
    DNGN_WATER_STUCK,
    DNGN_FLOOR,                             //   67
    DNGN_ENTER_HELL = 69,                   //   69
    DNGN_OPEN_DOOR,
    DNGN_BRANCH_STAIRS,                     //   71
    DNGN_TRAP_I = 75,                       //   75
    DNGN_TRAP_II,
    DNGN_TRAP_III,
    DNGN_UNDISCOVERED_TRAP,                 //   78
    DNGN_ENTER_SHOP = 80,                   //   80
    DNGN_ENTER_LABYRINTH,
    DNGN_STONE_STAIRS_DOWN_I,
    DNGN_STONE_STAIRS_DOWN_II,
    DNGN_STONE_STAIRS_DOWN_III,
    DNGN_ROCK_STAIRS_DOWN,
    DNGN_STONE_STAIRS_UP_I,
    DNGN_STONE_STAIRS_UP_II,
    DNGN_STONE_STAIRS_UP_III,
    DNGN_ROCK_STAIRS_UP,                    //   89
    DNGN_ENTER_DIS = 92,                    //   92
    DNGN_ENTER_GEHENNA,
    DNGN_ENTER_COCYTUS,
    DNGN_ENTER_TARTARUS,
    DNGN_ENTER_ABYSS,
    DNGN_EXIT_ABYSS,
    DNGN_STONE_ARCH,
    DNGN_ENTER_PANDEMONIUM,
    DNGN_EXIT_PANDEMONIUM,                  //  100
    DNGN_TRANSIT_PANDEMONIUM,               //  101
    DNGN_ENTER_ORCISH_MINES = 110,          //  110
    DNGN_ENTER_HIVE,
    DNGN_ENTER_LAIR_I,
    DNGN_ENTER_SLIME_PITS,
    DNGN_ENTER_VAULTS,
    DNGN_ENTER_CRYPT_I,
    DNGN_ENTER_HALL_OF_BLADES,
    DNGN_ENTER_ZOT,
    DNGN_ENTER_TEMPLE,
    DNGN_ENTER_SNAKE_PIT,
    DNGN_ENTER_ELVEN_HALLS,                 //  120
    DNGN_ENTER_TOMB,
    DNGN_ENTER_SWAMP,                       //  122
    DNGN_RETURN_DUNGEON_I = 130,            //  130
    DNGN_RETURN_DUNGEON_II,
    DNGN_RETURN_DUNGEON_III,
    DNGN_RETURN_LAIR_II,
    DNGN_RETURN_DUNGEON_IV,
    DNGN_RETURN_VAULTS,
    DNGN_RETURN_CRYPT_II,
    DNGN_EXIT_ZOT,
    DNGN_RETURN_DUNGEON_V,
    DNGN_RETURN_LAIR_III,
    DNGN_RETURN_MINES,                      //  140
    DNGN_RETURN_CRYPT_III,
    DNGN_RETURN_LAIR_IV,                    //  142
    DNGN_ALTAR_ZIN = 180,                   //  180
    DNGN_ALTAR_SHINING_ONE,
    DNGN_ALTAR_KIKUBAAQUDGHA,
    DNGN_ALTAR_YREDELEMNUL,
    DNGN_ALTAR_XOM,
    DNGN_ALTAR_VEHUMET,
    DNGN_ALTAR_OKAWARU,
    DNGN_ALTAR_MAKHLEB,
    DNGN_ALTAR_SIF_MUNA,
    DNGN_ALTAR_TROG,
    DNGN_ALTAR_NEMELEX_XOBEH,               //  190
    DNGN_ALTAR_ELYVILON,                    //  191
    DNGN_BLUE_FOUNTAIN = 200,               //  200
    DNGN_DRY_FOUNTAIN_I,
    DNGN_SPARKLING_FOUNTAIN,                // aka 'Magic Fountain' {dlb}
    DNGN_DRY_FOUNTAIN_II,
    DNGN_DRY_FOUNTAIN_III,
    DNGN_DRY_FOUNTAIN_IV,
    DNGN_DRY_FOUNTAIN_V,
    DNGN_DRY_FOUNTAIN_VI,
    DNGN_DRY_FOUNTAIN_VII,
    DNGN_DRY_FOUNTAIN_VIII,
    DNGN_PERMADRY_FOUNTAIN = 210            //  210 - added (from dungeon.cc / maps.cc) 22jan2000 {dlb}
};

enum DURATIONS                              // you.duration[]
{
    DUR_LIQUID_FLAMES,                      //    0
    DUR_ICY_ARMOUR,
    DUR_REPEL_MISSILES,
    DUR_PRAYER,
    DUR_REGENERATION,
    DUR_VORPAL_BLADE,
    DUR_FIRE_BRAND,
    DUR_ICE_BRAND,
    DUR_LETHAL_INFUSION,
    DUR_SWIFTNESS,
    DUR_INSULATION,                         //   10
    DUR_STONEMAIL,
    DUR_CONTROLLED_FLIGHT,
    DUR_TELEPORT,
    DUR_CONTROL_TELEPORT,
    DUR_POISON_WEAPON,
    DUR_RESIST_POISON,
    DUR_BREATH_WEAPON,
    DUR_TRANSFORMATION,
    DUR_DEATH_CHANNEL,
    DUR_DEFLECT_MISSILES                    //   20
};

enum ENCHANTMENT                            // menv[].enchantment[]
{
    ENCH_NONE,                              //    0
    ENCH_SLOW,
    ENCH_HASTE,                             //    2
    ENCH_FEAR = 4,                          //    4
    ENCH_CONFUSION,
    ENCH_INVIS,
    ENCH_YOUR_POISON_I,
    ENCH_YOUR_POISON_II,
    ENCH_YOUR_POISON_III,
    ENCH_YOUR_POISON_IV,                    //   10
    ENCH_SUMMON = 19,                       //   19
    ENCH_ABJ_I,                             //   20
    ENCH_ABJ_II,
    ENCH_ABJ_III,
    ENCH_ABJ_IV,
    ENCH_ABJ_V,
    ENCH_ABJ_VI,                            //   25
    ENCH_CHARM = 30,                        //   30
    ENCH_YOUR_STICKY_FLAME_I,
    ENCH_YOUR_STICKY_FLAME_II,
    ENCH_YOUR_STICKY_FLAME_III,
    ENCH_YOUR_STICKY_FLAME_IV,              //   34
    ENCH_GLOWING_SHAPESHIFTER = 38,         //   38
    ENCH_SHAPESHIFTER,
    ENCH_TP_I,                              //   40
    ENCH_TP_II,
    ENCH_TP_III,
    ENCH_TP_IV,                             //   43
    ENCH_POISON_I = 57,                     //   57
    ENCH_POISON_II,
    ENCH_POISON_III,
    ENCH_POISON_IV,                         //   60
    ENCH_STICKY_FLAME_I,
    ENCH_STICKY_FLAME_II,
    ENCH_STICKY_FLAME_III,
    ENCH_STICKY_FLAME_IV,
    ENCH_FRIEND_ABJ_I,
    ENCH_FRIEND_ABJ_II,
    ENCH_FRIEND_ABJ_III,
    ENCH_FRIEND_ABJ_IV,
    ENCH_FRIEND_ABJ_V,
    ENCH_FRIEND_ABJ_VI,                     //   70
    ENCH_CREATED_FRIENDLY
};

enum EQUIPMENT
{
    EQ_WEAPON,                              //    0
    EQ_CLOAK,
    EQ_HELMET,
    EQ_GLOVES,
    EQ_BOOTS,
    EQ_SHIELD,                              //    5
    EQ_BODY_ARMOUR,
    EQ_LEFT_RING,
    EQ_RIGHT_RING,
    EQ_AMULET,
    NUM_EQUIP                               //   10 - must always be last member in enum {dlb}
};

enum FOODS                                  // mitm[].sub_type[]
{
    FOOD_MEAT_RATION,                       //    0
    FOOD_BREAD_RATION,
    FOOD_PEAR,
    FOOD_APPLE,
    FOOD_CHOKO,
    FOOD_HONEYCOMB,
    FOOD_ROYAL_JELLY,
    FOOD_SNOZZCUMBER,
    FOOD_PIZZA,
    FOOD_APRICOT,
    FOOD_ORANGE,                            //   10
    FOOD_BANANA,
    FOOD_STRAWBERRY,
    FOOD_RAMBUTAN,
    FOOD_LEMON,
    FOOD_GRAPE,
    FOOD_SULTANA,
    FOOD_LYCHEE,
    FOOD_BEEF_JERKY,
    FOOD_CHEESE,
    FOOD_SAUSAGE,                           //   20
    FOOD_CHUNK
};

enum GODS
{
    GOD_NO_GOD,                             //    0
    GOD_ZIN,
    GOD_SHINING_ONE,
    GOD_KIKUBAAQUDGHA,
    GOD_YREDELEMNUL,
    GOD_XOM,
    GOD_VEHUMET,
    GOD_OKAWARU,
    GOD_MAKHLEB,
    GOD_SIF_MUNA,
    GOD_TROG,                               //   10
    GOD_NEMELEX_XOBEH,
    GOD_ELYVILON
};

enum GOOD_THINGS
{
    GOOD_KILLED_LIVING = 1,                 //    1
    GOOD_KILLED_UNDEAD,
    GOOD_KILLED_DEMON,
    GOOD_KILLED_ANGEL_I,
    GOOD_KILLED_ANGEL_II,
    GOOD_HACKED_CORPSE,
    GOOD_OFFER_STUFF,
    GOOD_OFFER_CORPSE,
    GOOD_SLAVES_KILL_LIVING,
    GOOD_SERVANTS_KILL,                     //   10
    GOOD_CARDS
};

enum HANDS_REQUIRED
{
    HANDS_ONE_HANDED = 1,                   //    1
    HANDS_TWO_HANDED,
    HANDS_ONE_OR_TWO_HANDED
};

enum HELMET_DESCRIPTIONS
{
    DHELM_WINGED = 1,                       //    1
    DHELM_HORNED,
    DHELM_CRESTED,
    DHELM_PLUMED,
    DHELM_SPIKED,
    DHELM_VISORED,
    DHELM_JEWELLED
};

enum HUNGER_STATES                          // you.hunger_state
{
    HS_RAVENOUS,                            //    0: only used in food_change()
    HS_STARVING,
    HS_HUNGRY,
    HS_SATIATED,                            // "not hungry" state
    HS_FULL,
    HS_ENGORGED                             //    5
};

enum JEWELLERY
{
    RING_REGENERATION,                      //    0
    RING_PROTECTION,
    RING_PROTECTION_FROM_FIRE,
    RING_POISON_RESISTANCE,
    RING_PROTECTION_FROM_COLD,
    RING_STRENGTH,
    RING_SLAYING,
    RING_SEE_INVISIBLE,
    RING_INVISIBILITY,
    RING_HUNGER,
    RING_TELEPORTATION,                     //   10
    RING_EVASION,
    RING_SUSTAIN_ABILITIES,
    RING_SUSTENANCE,
    RING_DEXTERITY,
    RING_INTELLIGENCE,
    RING_WIZARDRY,
    RING_MAGICAL_POWER,
    RING_LEVITATION,
    RING_LIFE_PROTECTION,
    RING_PROTECTION_FROM_MAGIC,             //   20
    RING_FIRE,
    RING_ICE,
    RING_TELEPORT_CONTROL,                  //   23
    AMU_RAGE = 35,                          //   35
    AMU_RESIST_SLOW,
    AMU_CLARITY,
    AMU_WARDING,
    AMU_RESIST_CORROSION,
    AMU_THE_GOURMAND,                       //   40
    AMU_CONSERVATION,
    AMU_CONTROLLED_FLIGHT,
    AMU_INACCURACY,
    AMU_RESIST_MUTATION
};

enum JOB
{
    JOB_FIGHTER,                            //    0
    JOB_WIZARD,
    JOB_PRIEST,
    JOB_THIEF,
    JOB_GLADIATOR,
    JOB_NECROMANCER,
    JOB_PALADIN,
    JOB_ASSASSIN,
    JOB_BERSERKER,
    JOB_HUNTER,
    JOB_CONJURER,                           //   10
    JOB_ENCHANTER,
    JOB_FIRE_ELEMENTALIST,
    JOB_ICE_ELEMENTALIST,
    JOB_SUMMONER,
    JOB_AIR_ELEMENTALIST,
    JOB_EARTH_ELEMENTALIST,
    JOB_CRUSADER,
    JOB_DEATH_KNIGHT,
    JOB_VENOM_MAGE,
    JOB_CHAOS_KNIGHT,                       //   20
    JOB_TRANSMUTER,
    JOB_HEALER,                             //   22
    JOB_REAVER = 24,                        //   24
    JOB_STALKER,
    JOB_MONK,
    JOB_WARPER
};

enum KILLBY
{
    KILLED_BY_MONSTER,                      //    0
    KILLED_BY_POISON,
    KILLED_BY_CLOUD,
    KILLED_BY_BEAM,                         //    3
    KILLED_BY_LAVA = 5,                     //    5
    KILLED_BY_WATER,
    KILLED_BY_STUPIDITY,
    KILLED_BY_WEAKNESS,
    KILLED_BY_CLUMSINESS,
    KILLED_BY_TRAP,                         //   10
    KILLED_BY_LEAVING,
    KILLED_BY_WINNING,
    KILLED_BY_QUITTING,
    KILLED_BY_DRAINING,
    KILLED_BY_STARVATION,                   //   15
    KILLED_BY_FREEZING,
    KILLED_BY_BURNING,
    KILLED_BY_WILD_MAGIC,
    KILLED_BY_XOM,
    KILLED_BY_STATUE,                       //   20
    KILLED_BY_ROTTING,
    KILLED_BY_TARGETTING,
    KILLED_BY_SPORE
};

enum KILLER                                 // monster_die(), thing_thrown
{
     KILL_YOU = 1,                          //    1
     KILL_MON,
     KILL_YOU_MISSILE,
     KILL_MON_MISSILE,
     KILL_MISC,
     KILL_RESET                             // abjuration, etc.
};

#define YOU_KILL(x) ((x) == KILL_YOU || (x) == KILL_YOU_MISSILE)

enum LEVEL_TYPES                            // you.level_type
{
    LEVEL_DUNGEON,                          //    0
    LEVEL_LABYRINTH,
    LEVEL_ABYSS,
    LEVEL_PANDEMONIUM
};

enum MISCELLANY                             // mitm.sub_type[]
{
    MISC_BOTTLED_EFREET,                    //    0
    MISC_CRYSTAL_BALL_OF_SEEING,
    MISC_AIR_ELEMENTAL_FAN,
    MISC_LAMP_OF_FIRE,
    MISC_STONE_OF_EARTH_ELEMENTALS,
    MISC_LANTERN_OF_SHADOWS,                //    5
    MISC_HORN_OF_GERYON,
    MISC_BOX_OF_BEASTS,
    MISC_DECK_OF_WONDERS,
    MISC_DECK_OF_SUMMONINGS,
    MISC_CRYSTAL_BALL_OF_ENERGY,            //   10
    MISC_EMPTY_EBONY_CASKET,
    MISC_CRYSTAL_BALL_OF_FIXATION,
    MISC_DISC_OF_STORMS,
    MISC_RUNE_OF_ZOT,
    MISC_DECK_OF_TRICKS,                    //   15
    MISC_DECK_OF_POWER,
    MISC_PORTABLE_ALTAR_OF_NEMELEX
};

enum MISSILES
{
    MI_STONE,                               //    0
    MI_ARROW,
    MI_BOLT,
    MI_DART,
    MI_EGGPLANT,
    MI_LARGE_ROCK,                          //    5
    MI_AXE = 9,                             //    9
    MI_SPEAR = 11                           //   11
};

enum MONSTERS                               // menv[].type
{
    MONS_GIANT_ANT,                         //    0
    MONS_GIANT_BAT,
    MONS_CENTAUR,
    MONS_RED_DEVIL,
    MONS_ETTIN,
    MONS_FUNGUS,
    MONS_GOBLIN,
    MONS_HOUND,
    MONS_IMP,
    MONS_JACKAL,
    MONS_KILLER_BEE,                        //   10
    MONS_KILLER_BEE_LARVA,
    MONS_MANTICORE,
    MONS_NECROPHAGE,
    MONS_ORC,
    MONS_PHANTOM,                           //   15
    MONS_QUASIT,
    MONS_RAT,
    MONS_SCORPION,
    MONS_TUNNELING_WORM,
    MONS_UGLY_THING,                        //   20
    MONS_FIRE_VORTEX,
    MONS_WORM,
    MONS_ABOMINATION_SMALL,
    MONS_YELLOW_WASP,
    MONS_ZOMBIE_SMALL,                      //   25
    MONS_ANGEL,
    MONS_GIANT_BEETLE,
    MONS_CYCLOPS,
    MONS_DRAGON,
    MONS_TWO_HEADED_OGRE,                   //   30
    MONS_FIEND,
    MONS_GIANT_SPORE,
    MONS_HOBGOBLIN,
    MONS_ICE_BEAST,
    MONS_JELLY,
    MONS_KOBOLD,
    MONS_LICH,
    MONS_MUMMY,
    MONS_GUARDIAN_NAGA,
    MONS_OGRE,                              //   40
    MONS_PLANT,
    MONS_QUEEN_BEE,
    MONS_RAKSHASA,
    MONS_SNAKE,
    MONS_TROLL,
    MONS_UNSEEN_HORROR,
    MONS_VAMPIRE,
    MONS_WRAITH,
    MONS_ABOMINATION_LARGE,
    MONS_YAK,                               //   50
    MONS_ZOMBIE_LARGE,
    MONS_ORC_WARRIOR,
    MONS_KOBOLD_DEMONOLOGIST,
    MONS_ORC_WIZARD,
    MONS_ORC_KNIGHT,                        //   55
    MONS_WORM_TAIL,
    MONS_WYVERN,
    MONS_BIG_KOBOLD,
    MONS_GIANT_EYEBALL,
    MONS_WIGHT,                             //   60
    MONS_OKLOB_PLANT,
    MONS_WOLF_SPIDER,
    MONS_SHADOW,
    MONS_HUNGRY_GHOST,
    MONS_EYE_OF_DRAINING,                   //   65
    MONS_BUTTERFLY,
    MONS_WANDERING_MUSHROOM,
    MONS_EFREET,
    MONS_BRAIN_WORM,
    MONS_GIANT_ORANGE_BRAIN,                //   70
    MONS_BOULDER_BEETLE,
    MONS_FLYING_SKULL,
    MONS_HELL_HOUND,
    MONS_MINOTAUR,
    MONS_ICE_DRAGON,
    MONS_SLIME_CREATURE,
    MONS_FREEZING_WRAITH,
    MONS_RAKSHASA_FAKE,
    MONS_GREAT_ORB_OF_EYES,
    MONS_HELLION,                           //   80
    MONS_ROTTING_DEVIL,
    MONS_TORMENTOR,
    MONS_REAPER,
    MONS_SOUL_EATER,
    MONS_HAIRY_DEVIL,
    MONS_ICE_DEVIL,
    MONS_BLUE_DEVIL,
    MONS_BEAST,
    MONS_IRON_DEVIL,                        //   89
    MONS_GLOWING_SHAPESHIFTER = 98,         //   98
    MONS_SHAPESHIFTER,
    MONS_GIANT_MITE,                        //  100
    MONS_STEAM_DRAGON,
    MONS_VERY_UGLY_THING,
    MONS_ORC_SORCEROR,
    MONS_HIPPOGRIFF,
    MONS_GRIFFON,
    MONS_HYDRA,
    MONS_SMALL_SKELETON,
    MONS_LARGE_SKELETON,
    MONS_HELL_KNIGHT,
    MONS_NECROMANCER,                       //  110
    MONS_WIZARD,
    MONS_ORC_PRIEST,
    MONS_ORC_HIGH_PRIEST,
    MONS_HUMAN,
    MONS_GNOLL,
    MONS_CLAY_GOLEM,
    MONS_WOOD_GOLEM,
    MONS_STONE_GOLEM,
    MONS_IRON_GOLEM,
    MONS_CRYSTAL_GOLEM,                     //  120
    MONS_TOENAIL_GOLEM,
    MONS_MOTTLED_DRAGON,
    MONS_EARTH_ELEMENTAL,
    MONS_FIRE_ELEMENTAL,
    MONS_AIR_ELEMENTAL,
    MONS_ICE_FIEND,
    MONS_SHADOW_FIEND,
    MONS_BROWN_SNAKE,
    MONS_GIANT_LIZARD,
    MONS_SPECTRAL_WARRIOR,                  //  130
    MONS_PULSATING_LUMP,
    MONS_STORM_DRAGON,
    MONS_YAKTAUR,
    MONS_DEATH_YAK,
    MONS_ROCK_TROLL,
    MONS_STONE_GIANT,
    MONS_FLAYED_GHOST,
    MONS_BUMBLEBEE,
    MONS_REDBACK,
    MONS_INSUBSTANTIAL_WISP,                //  140
    MONS_VAPOUR,
    MONS_OGRE_MAGE,
    MONS_SPINY_WORM,
    MONS_DANCING_WEAPON,
    MONS_TITAN,
    MONS_GOLDEN_DRAGON,
    MONS_ELF,
    MONS_LINDWORM,
    MONS_ELEPHANT_SLUG,
    MONS_WAR_DOG,                           //  150
    MONS_GREY_RAT,
    MONS_GREEN_RAT,
    MONS_ORANGE_RAT,
    MONS_BLACK_SNAKE,
    MONS_SHEEP,
    MONS_GHOUL,
    MONS_HOG,
    MONS_GIANT_MOSQUITO,
    MONS_GIANT_CENTIPEDE,
    MONS_IRON_TROLL,                        //  160
    MONS_NAGA,
    MONS_FIRE_GIANT,
    MONS_FROST_GIANT,
    MONS_FIREDRAKE,
    MONS_SHADOW_DRAGON,
    MONS_YELLOW_SNAKE,
    MONS_GREY_SNAKE,
    MONS_DEEP_TROLL,
    MONS_GIANT_BLOWFLY,
    MONS_RED_WASP,                          //  170
    MONS_SWAMP_DRAGON,
    MONS_SWAMP_DRAKE,
    MONS_SOLDIER_ANT,
    MONS_HILL_GIANT,
    MONS_QUEEN_ANT,
    MONS_ANT_LARVA,
    MONS_GIANT_FROG,
    MONS_GIANT_BROWN_FROG,
    MONS_SPINY_FROG,
    MONS_BLINK_FROG,                        //  180
    MONS_GIANT_COCKROACH,
    MONS_SMALL_SNAKE,                       //  182
    MONS_WHITE_IMP = 220,                   //  220
    MONS_LEMURE,
    MONS_UFETUBUS,
    MONS_MANES,
    MONS_MIDGE,
    MONS_NEQOXEC,
    MONS_ORANGE_DEMON,
    MONS_HELLWING,
    MONS_SMOKE_DEMON,
    MONS_YNOXINUL,
    MONS_EXECUTIONER,                       //  230
    MONS_GREEN_DEATH,
    MONS_BLUE_DEATH,
    MONS_BALRUG,
    MONS_CACODEMON,
    MONS_DEMONIC_CRAWLER,                   //  235
    MONS_SUN_DEMON,
    MONS_SHADOW_IMP,
    MONS_SHADOW_DEMON,
    MONS_LOROCYPROCA,
    MONS_SHADOW_WRAITH,                     //  240
    MONS_GIANT_AMOEBA,
    MONS_GIANT_SLUG,
    MONS_GIANT_SNAIL,
    MONS_SPATIAL_VORTEX,
    MONS_PIT_FIEND,
    MONS_BORING_BEETLE,
    MONS_GARGOYLE,
    MONS_METAL_GARGOYLE,
    MONS_MOLTEN_GARGOYLE,
    MONS_PROGRAM_BUG,                       //  250
// BCR - begin first batch of uniques.
    MONS_MNOLEG,
    MONS_LOM_LOBON,
    MONS_CEREBOV,
    MONS_GLOORX_VLOQ,                       //  254
// BCR - End first batch of uniques.
    MONS_NAGA_MAGE = 260,                   //  260
    MONS_NAGA_WARRIOR,
    MONS_ORC_WARLORD,
    MONS_DEEP_ELF_SOLDIER,
    MONS_DEEP_ELF_FIGHTER,
    MONS_DEEP_ELF_KNIGHT,
    MONS_DEEP_ELF_MAGE,
    MONS_DEEP_ELF_SUMMONER,
    MONS_DEEP_ELF_CONJURER,
    MONS_DEEP_ELF_PRIEST,
    MONS_DEEP_ELF_HIGH_PRIEST,              //  270
    MONS_DEEP_ELF_DEMONOLOGIST,
    MONS_DEEP_ELF_ANNIHILATOR,
    MONS_DEEP_ELF_SORCEROR,
    MONS_DEEP_ELF_DEATH_MAGE,
    MONS_BROWN_OOZE,
    MONS_AZURE_JELLY,
    MONS_DEATH_OOZE,
    MONS_ACID_BLOB,
    MONS_ROYAL_JELLY,
// BCR - begin second batch of uniques.
    MONS_TERENCE,                           //  280
    MONS_JESSICA,
    MONS_IJYB,
    MONS_SIGMUND,
    MONS_BLORK_THE_ORC,
    MONS_EDMUND,
    MONS_PSYCHE,
    MONS_EROLCHA,
    MONS_DONALD,
    MONS_URUG,
    MONS_MICHAEL,                           //  290
    MONS_JOSEPH,
    MONS_SNORG,                             // was Anita - Snorg is correct 16jan2000 {dlb}
    MONS_ERICA,
    MONS_JOSEPHINE,
    MONS_HAROLD,
    MONS_NORBERT,
    MONS_JOZEF,
    MONS_AGNES,
    MONS_MAUD,
    MONS_LOUISE,                            //  300
    MONS_FRANCIS,
    MONS_FRANCES,
    MONS_RUPERT,
    MONS_WAYNE,
    MONS_DUANE,
    MONS_XTAHUA,
    MONS_NORRIS,
    MONS_ADOLF,
    MONS_MARGERY,
    MONS_BORIS,                             //  310
// BCR - end second batch of uniques.
    MONS_GERYON = 340,                      //  340
    MONS_DISPATER,
    MONS_ASMODEUS,
    MONS_ANTAEUS,
    MONS_ERESHKIGAL,                        //  344
    MONS_ANCIENT_LICH = 356,                //  356
    MONS_OOZE,                              //  357
    MONS_VAULT_GUARD = 360,                 //  360
    MONS_CURSE_SKULL,
    MONS_VAMPIRE_KNIGHT,
    MONS_VAMPIRE_MAGE,
    MONS_SHINING_EYE,
    MONS_ORB_GUARDIAN,                      //  365
    MONS_DAEVA,
    MONS_SPECTRAL_THING,
    MONS_GREATER_NAGA,
    MONS_SKELETAL_DRAGON,
    MONS_TENTACLED_MONSTROSITY,             //  370
    MONS_SPHINX,
    MONS_ROTTING_HULK,
    MONS_GUARDIAN_MUMMY,
    MONS_GREATER_MUMMY,
    MONS_MUMMY_PRIEST,
    MONS_CENTAUR_WARRIOR,
    MONS_YAKTAUR_CAPTAIN,
    MONS_KILLER_KLOWN,
    MONS_GUARDIAN_ROBOT,
    MONS_DORGI,                             //  380 - deprecated? {dlb}
    MONS_SWORD,                             //  381 - deprecated? {dlb}
    MONS_QUOKKA,                            //  382 - deprecated? {dlb}
    MONS_EYE_OF_DEVASTATION = 385,          //  385
    MONS_MOTH_OF_WRATH,
    MONS_DEATH_COB,
    MONS_CURSE_TOE,
    MONS_GOLD_MIMIC,
    MONS_WEAPON_MIMIC,                      //  390
    MONS_ARMOUR_MIMIC,
    MONS_SCROLL_MIMIC,
    MONS_POTION_MIMIC,
    MONS_HELL_HOG,
    MONS_SERPENT_OF_HELL,
    MONS_BOGGART,
    MONS_QUICKSILVER_DRAGON,
    MONS_IRON_DRAGON,
    MONS_SKELETAL_WARRIOR,
    MONS_PLAYER_GHOST,                      //  400
    MONS_PANDEMONIUM_DEMON,                 //  401
    MONS_LAVA_WORM = 420,                   //  420
    MONS_LAVA_FISH,
    MONS_LAVA_SNAKE,
    MONS_ANOTHER_LAVA_THING,                //  423
    MONS_BIG_FISH = 430,                    //  430
    MONS_GIANT_GOLDFISH,
    MONS_ELECTRICAL_EEL,
    MONS_JELLYFISH,
    MONS_WATER_ELEMENTAL,
    MONS_SWAMP_WORM
};

enum MONSTER_BEHAVIOR                       // create_monster()
{
    BEH_SLEEP,                              //    0
    BEH_CHASING_I,
    BEH_WANDER,
    BEH_FLEE,
    BEH_CONFUSED,                           //    4
    BEH_FIGHT = 6,                          //    6
    BEH_ENSLAVED,                           //    7
    BEH_CHASING_II = 100                    //  100
};

enum MONSTER_HOLINESS
{
    MH_NORMAL,                              //    0
    MH_UNDEAD,
    MH_DEMONIC
};

enum MONSTER_SPELLS                         // mons_cast(), spell_list[], mons_spells()
{
    MS_MMISSILE,                            //    0
    MS_FLAME,
    MS_FROST,
    MS_PARALYSIS,
    MS_SLOW,
    MS_HASTE,
    MS_CONFUSE,                             //    6 - do not deprecate!!! 13jan2000 {dlb}
    MS_VENOM_BOLT,
    MS_FIRE_BOLT,
    MS_COLD_BOLT,
    MS_LIGHTNING_BOLT,                      //   10
    MS_INVIS,
    MS_FIREBALL,
    MS_HEAL,
    MS_TELEPORT,
    MS_TELEPORT_OTHER,
    MS_BLINK,
    MS_CRYSTAL_SPEAR,
    MS_DIG,
    MS_NEGATIVE_BOLT,
    MS_HELLFIRE_BURST,                      //   20
    MS_VAMPIRE_SUMMON,
    MS_ORB_ENERGY,
    MS_BRAIN_FEED,
    MS_LEVEL_SUMMON,
    MS_FAKE_RAKSHASA_SUMMON,
    MS_STEAM_BALL,
    MS_SUMMON_DEMON,
    MS_ANIMATE_DEAD,
    MS_PAIN,
    MS_SMITE,                               //   30
    MS_STICKY_FLAME,
    MS_POISON_BLAST,
    MS_SUMMON_DEMON_LESSER,
    MS_SUMMON_UFETUBUS,
    MS_PURPLE_BLAST,
    MS_SUMMON_BEAST,                        // MS_GERYON was not descriptive - renamed 13jan2000 {dlb}
    MS_ENERGY_BOLT,
    MS_STING,
    MS_IRON_BOLT,
    MS_STONE_ARROW,                         //   40
    MS_POISON_SPLASH,
    MS_SUMMON_UNDEAD,
    MS_MUTATION,
    MS_ZULZER,
    MS_DISINTEGRATE,
    MS_MARSH_GAS,
    MS_QUICKSILVER_BOLT,
    MS_TORMENT,
    MS_HELLFIRE,
    MS_METAL_SPLINTERS,                     //   50
    MS_SUMMON_DEMON_GREATER,                // _1 was confusing - renamed 13jan2000 {dlb}
    MS_BANISHMENT
};

enum MUTATIONS
{
    MUT_TOUGH_SKIN,                         //    0
    MUT_STRONG,
    MUT_CLEVER,
    MUT_AGILE,
    MUT_GREEN_SCALES,
    MUT_BLACK_SCALES,                       //    5
    MUT_GREY_SCALES,
    MUT_BONEY_PLATES,
    MUT_REPULSION_FIELD,
    MUT_POISON_RESISTANCE,
    MUT_CARNIVOROUS,                        //   10
    MUT_HERBIVOROUS,
    MUT_HEAT_RESISTANCE,
    MUT_COLD_RESISTANCE,
    MUT_SHOCK_RESISTANCE,
    MUT_REGENERATION,                       //   15
    MUT_FAST_METABOLISM,
    MUT_SLOW_METABOLISM,
    MUT_WEAK,
    MUT_DOPEY,
    MUT_CLUMSY,                             //   20
    MUT_TELEPORT_CONTROL,
    MUT_TELEPORT,
    MUT_MAGIC_RESISTANCE,
    MUT_FAST,
    MUT_ACUTE_VISION,                       //   25
    MUT_DEFORMED,
    MUT_TELEPORT_AT_WILL,
    MUT_SPIT_POISON,
    MUT_MAPPING,
    MUT_BREATHE_FLAMES,                     //   30
    MUT_BLINK,
    MUT_HORNS,
    MUT_STRONG_STIFF,
    MUT_FLEXIBLE_WEAK,
    MUT_LOST,                               //   35
    MUT_CLARITY,
    MUT_BERSERK,
    MUT_DETERIORATION,
    MUT_BLURRY_VISION,
    MUT_MUTATION_RESISTANCE,                //   40
    MUT_FRAIL,
    MUT_ROBUST,
    MUT_TORMENT_RESISTANCE,
    MUT_NEGATIVE_ENERGY_RESISTANCE,
    MUT_SUMMON_MINOR_DEMONS,                //   45
    MUT_SUMMON_DEMONS,
    MUT_HURL_HELLFIRE,
    MUT_CALL_TORMENT,
    MUT_RAISE_DEAD,
    MUT_CONTROL_DEMONS,                     //   50
    MUT_PANDEMONIUM,
    MUT_DEATH_STRENGTH,
    MUT_CHANNEL_HELL,
    MUT_DRAIN_LIFE,
    MUT_THROW_FLAMES,                       //   55
    MUT_THROW_FROST,
    MUT_SMITE,                              //   57
    MUT_RED_SCALES = 70,                    //   70
    MUT_NACREOUS_SCALES,
    MUT_GREY2_SCALES,
    MUT_METALLIC_SCALES,
    MUT_BLACK2_SCALES,
    MUT_WHITE_SCALES,                       //   75
    MUT_YELLOW_SCALES,
    MUT_BROWN_SCALES,
    MUT_BLUE_SCALES,
    MUT_PURPLE_SCALES,
    MUT_SPECKLED_SCALES,                    //   80
    MUT_ORANGE_SCALES,
    MUT_INDIGO_SCALES,
    MUT_RED2_SCALES,
    MUT_IRIDESCENT_SCALES,
    MUT_PATTERNED_SCALES,                   //   85
    NUM_MUTATIONS                           //   86 - must always be last member in enum {dlb}
};

enum NAMED_WEAPONS                          // mitm.special[]
{
    NWPN_SINGING_SWORD = 181,               //  181
    NWPN_WRATH_OF_TROG,
    NWPN_SCYTHE_OF_CURSES,
    NWPN_MACE_OF_VARIABILITY,
    NWPN_GLAIVE_OF_PRUNE,
    NWPN_SCEPTRE_OF_TORMENT,
    NWPN_SWORD_OF_ZONGULDROK,
    NWPN_SWORD_OF_CEREBOV,
    NWPN_STAFF_OF_DISPATER,
    NWPN_SCEPTRE_OF_ASMODEUS,               //  190
    NWPN_SWORD_OF_POWER,
    NWPN_KNIFE_OF_ACCURACY,
    NWPN_STAFF_OF_OLGREB,
    NWPN_VAMPIRES_TOOTH,
    NWPN_STAFF_OF_WUCAD_MU
};

enum NAUGHTY_THINGS
{
    NAUGHTY_NECROMANCY = 1,                 //    1
    NAUGHTY_UNHOLY,
    NAUGHTY_KILLING,
    NAUGHTY_ATTACK_HOLY,
    NAUGHTY_ATTACK_FRIEND,
    NAUGHTY_FRIEND_DIES,
    NAUGHTY_BUTCHER,
    NAUGHTY_STABBING,
    NAUGHTY_SPELLCASTING,
    NAUGHTY_POISON                          //   10: jmf added - 04jan2000
};

enum OBJECT_CLASSES                         // mitm.base_type[]
{
    OBJ_WEAPONS,                            //    0
    OBJ_MISSILES,
    OBJ_ARMOUR,
    OBJ_WANDS,
    OBJ_FOOD,                               //    4
    OBJ_UNKNOWN_I = 5,                      //    5 - in use, unknown - labeled as books in invent.cc {dlb}
    OBJ_SCROLLS = 6,                        //    6
    OBJ_JEWELLERY,
    OBJ_POTIONS,                            //    8
    OBJ_UNKNOWN_II = 9,                     //    9 - in use, unknown stackable - labeled as gems in invent.cc {dlb}
    OBJ_BOOKS = 10,                         //   10
    OBJ_STAVES,
    OBJ_ORBS,
    OBJ_MISCELLANY,
    OBJ_CORPSES,
    OBJ_GOLD,                               //   15
    OBJ_GEMSTONES                           // found in itemname.cc, labeled as miscellaneous in invent.cc {dlb}
};

enum ORBS
{
    ORB_ZOT                                 //    0
};

enum POTIONS
{
    POT_HEALING,                            //    0
    POT_HEAL_WOUNDS,
    POT_SPEED,
    POT_MIGHT,
    POT_GAIN_STRENGTH,
    POT_GAIN_DEXTERITY,
    POT_GAIN_INTELLIGENCE,
    POT_LEVITATION,
    POT_POISON,
    POT_SLOWING,
    POT_PARALYSIS,                          //   10
    POT_CONFUSION,
    POT_INVISIBILITY,
    POT_PORRIDGE,
    POT_DEGENERATION,
    POT_DECAY,
    POT_WATER,
    POT_EXPERIENCE,
    POT_MAGIC,
    POT_RESTORE_ABILITIES,
    POT_STRONG_POISON,                      //   20
    POT_BERSERK_RAGE,
    POT_CURE_MUTATION,
    POT_MUTATION
};

enum RANDART_PROP
{
    RAP_BRAND,                              //    0
    RAP_AC,
    RAP_EVASION,
    RAP_STRENGTH,
    RAP_INTELLIGENCE,
    RAP_DEXTERITY,
    RAP_FIRE,
    RAP_COLD,
    RAP_ELECTRICITY,
    RAP_POISON,
    RAP_NEGATIVE_ENERGY,                    //   10
    RAP_MAGIC,
    RAP_EYESIGHT,
    RAP_INVISIBLE,
    RAP_LEVITATE,
    RAP_BLINK,
    RAP_CAN_TELEPORT,
    RAP_BERSERK,
    RAP_MAPPING,
    RAP_NOISES,
    RAP_PREVENT_SPELLCASTING,               //   20
    RAP_CAUSE_TELEPORTATION,
    RAP_PREVENT_TELEPORTATION,
    RAP_ANGRY,
    RAP_METABOLISM,
    RAP_MUTAGENIC,
    RAP_ACCURACY,
    RAP_DAMAGE
};

enum SCROLLS
{
    SCR_IDENTIFY,                           //    0
    SCR_TELEPORTATION,
    SCR_FEAR,
    SCR_NOISE,
    SCR_REMOVE_CURSE,
    SCR_DETECT_CURSE,
    SCR_SUMMONING,
    SCR_ENCHANT_WEAPON_I,
    SCR_ENCHANT_ARMOUR,
    SCR_TORMENT,
    SCR_RANDOM_USELESSNESS,                 //   10
    SCR_CURSE_WEAPON,
    SCR_CURSE_ARMOUR,
    SCR_IMMOLATION,
    SCR_BLINKING,
    SCR_PAPER,
    SCR_MAGIC_MAPPING,
    SCR_FORGETFULNESS,
    SCR_ACQUIREMENT,
    SCR_ENCHANT_WEAPON_II,
    SCR_VORPALISE_WEAPON,                   //   20
    SCR_RECHARGING,
    SCR_ENCHANT_WEAPON_III
};

enum SHOPS                                  // env.sh_type[], item_in_shop(), in_a_shop()
{
    SHOP_WEAPON,                            //    0
    SHOP_ARMOUR,
    SHOP_WEAPON_ANTIQUE,
    SHOP_ARMOUR_ANTIQUE,
    SHOP_GENERAL_ANTIQUE,
    SHOP_JEWELLERY,                         //    5
    SHOP_WAND,
    SHOP_BOOK,
    SHOP_FOOD,
    SHOP_DISTILLERY,
    SHOP_SCROLL,                            //   10
    SHOP_GENERAL
};

enum SKILLS
{
    SK_FIGHTING,                            //    0
    SK_SHORT_BLADES,
    SK_LONG_SWORDS,
    SK_UNUSED_1,                            // SK_GREAT_SWORDS - now unused
    SK_AXES,
    SK_MACES_FLAILS,
    SK_POLEARMS,
    SK_STAVES,
    SK_SLINGS,
    SK_BOWS,
    SK_CROSSBOWS,                           //   10
    SK_DARTS,
    SK_THROWING,
    SK_ARMOUR,
    SK_DODGING,
    SK_STEALTH,
    SK_STABBING,
    SK_SHIELDS,
    SK_TRAPS_DOORS,
    SK_UNARMED_COMBAT,                      //   19
    SK_SPELLCASTING = 25,                   //   25
    SK_CONJURATIONS,
    SK_ENCHANTMENTS,
    SK_SUMMONINGS,
    SK_NECROMANCY,
    SK_TRANSLOCATIONS,                      //   30
    SK_TRANSMIGRATION,
    SK_DIVINATIONS,
    SK_FIRE_MAGIC,
    SK_ICE_MAGIC,
    SK_AIR_MAGIC,
    SK_EARTH_MAGIC,
    SK_POISON_MAGIC,
    SK_INVOCATIONS
};

enum SPECIAL_ARMOR
{
    SPARM_NORMAL,                           //    0
    SPARM_RUNNING,
    SPARM_FIRE_RESISTANCE,
    SPARM_COLD_RESISTANCE,
    SPARM_POISON_RESISTANCE,
    SPARM_SEE_INVISIBLE,
    SPARM_DARKNESS,
    SPARM_STRENGTH,
    SPARM_DEXTERITY,
    SPARM_INTELLIGENCE,
    SPARM_PONDEROUSNESS,                    //   10
    SPARM_LEVITATION,
    SPARM_MAGIC_RESISTANCE,
    SPARM_PROTECTION,
    SPARM_STEALTH,
    SPARM_RESISTANCE,
    SPARM_POSITIVE_ENERGY,
    SPARM_ARCHMAGI,
    SPARM_PRESERVATION
};

enum SPECIAL_WEAPONS
{
    SPWPN_NORMAL,                           //    0
    SPWPN_FLAMING,
    SPWPN_FREEZING,
    SPWPN_HOLY_WRATH,
    SPWPN_ELECTROCUTION,
    SPWPN_ORC_SLAYING,
    SPWPN_VENOM,
    SPWPN_PROTECTION,
    SPWPN_DRAINING,
    SPWPN_SPEED,
    SPWPN_VORPAL,                           //   10
    SPWPN_FLAME,
    SPWPN_FROST,
    SPWPN_VAMPIRICISM,
    SPWPN_DISRUPTION,
    SPWPN_PAIN,
    SPWPN_DISTORTION,
    SPWPN_REACHING,                         //   17
    SPWPN_RANDART_I = 25,                   //   25
    SPWPN_RANDART_II,
    SPWPN_RANDART_III,
    SPWPN_RANDART_IV,
    SPWPN_RANDART_V
};

enum SPECIAL_WIELD
{
    SPWLD_NONE,                             //    0
    SPWLD_SING,
    SPWLD_TROG,
    SPWLD_CURSE,
    SPWLD_VARIABLE,                         //    4
    SPWLD_TORMENT = 6,                      //    6
    SPWLD_ZONGULDROK,
    SPWLD_POWER,
    SPWLD_WUCAD_MU,                         //    9
    SPWLD_SHADOW = 50                       //   50
};

enum SPECIES
{
    SP_HUMAN = 1,                           //    1
    SP_ELF,
    SP_HIGH_ELF,
    SP_GREY_ELF,
    SP_DEEP_ELF,
    SP_SLUDGE_ELF,
    SP_HILL_DWARF,
    SP_MOUNTAIN_DWARF,
    SP_HALFLING,
    SP_HILL_ORC,                            //   10
    SP_KOBOLD,
    SP_MUMMY,
    SP_NAGA,
    SP_GNOME,
    SP_OGRE,
    SP_TROLL,
    SP_OGRE_MAGE,
    SP_RED_DRACONIAN,
    SP_WHITE_DRACONIAN,
    SP_GREEN_DRACONIAN,                     //   20
    SP_GOLDEN_DRACONIAN,
    SP_GREY_DRACONIAN,
    SP_BLACK_DRACONIAN,
    SP_PURPLE_DRACONIAN,
    SP_MOTTLED_DRACONIAN,
    SP_PALE_DRACONIAN,
    SP_UNK0_DRACONIAN,
    SP_UNK1_DRACONIAN,
    SP_UNK2_DRACONIAN,
    SP_CENTAUR,                             //   30
    SP_DEMIGOD,
    SP_SPRIGGAN,
    SP_MINOTAUR,
    SP_DEMONSPAWN,
    SP_GHOUL,
    SP_KENKU
};

enum SPELLBOOK_CONTENTS                     // you.inv_plus[] - it's a binary thing ...
{                                           // assuming all from start, no empty slots 22jan2000 {dlb}
    SPBK_NO_SLOTS = 64,                     //   64
    SPBK_ONE_SLOT = 96,                     //   96
    SPBK_TWO_SLOTS = 112,                   //  112
    SPBK_THREE_SLOTS = 120,                 //  120
    SPBK_FOUR_SLOTS = 124,                  //  124
    SPBK_FIVE_SLOTS = 126,                  //  126
    SPBK_SIX_SLOTS = 127                    //  127
};

enum SPELLS
{
    SPELL_IDENTIFY,                         //    0
    SPELL_TELEPORT_SELF,
    SPELL_CAUSE_FEAR,
    SPELL_CREATE_NOISE,
    SPELL_REMOVE_CURSE,
    SPELL_MAGIC_DART,
    SPELL_FIREBALL,                         //    6
    SPELL_CONJURE_FLAME = 13,               //   13
    SPELL_DIG,
    SPELL_BOLT_OF_FIRE,
    SPELL_BOLT_OF_COLD,
    SPELL_LIGHTNING_BOLT,                   //   17
    SPELL_POLYMORPH_OTHER = 20,             //   20
    SPELL_SLOW,
    SPELL_HASTE,
    SPELL_PARALYZE,
    SPELL_CONFUSE,
    SPELL_INVISIBILITY,                     //   25
    SPELL_THROW_FLAME,
    SPELL_THROW_FROST,
    SPELL_CONTROLLED_BLINK,
    SPELL_FREEZING_CLOUD,
    SPELL_MEPHITIC_CLOUD,                   //   30
    SPELL_RING_OF_FLAMES,
    SPELL_RESTORE_STRENGTH,
    SPELL_RESTORE_INTELLIGENCE,
    SPELL_RESTORE_DEXTERITY,
    SPELL_VENOM_BOLT,                       //   35
    SPELL_OLGREBS_TOXIC_RADIANCE,
    SPELL_TELEPORT_OTHER,
    SPELL_LESSER_HEALING,
    SPELL_GREATER_HEALING,
    SPELL_CURE_POISON_I,                    //   40
    SPELL_PURIFICATION,
    SPELL_DEATHS_DOOR,
    SPELL_SELECTIVE_AMNESIA,
    SPELL_MASS_CONFUSION,
    SPELL_SMITING,                          //   45
    SPELL_REPEL_UNDEAD,
    SPELL_HOLY_WORD,
    SPELL_DETECT_CURSE,
    SPELL_SUMMON_SMALL_MAMMAL,
    SPELL_ABJURATION_I,                     //   50
    SPELL_SUMMON_SCORPIONS,
    SPELL_LEVITATION,
    SPELL_BOLT_OF_DRAINING,
    SPELL_LEHUDIBS_CRYSTAL_SPEAR,
    SPELL_BOLT_OF_INACCURACY,               //   55
    SPELL_POISONOUS_CLOUD,
    SPELL_FIRE_STORM,
    SPELL_DETECT_TRAPS,
    SPELL_BLINK,
    SPELL_ISKENDERUNS_MYSTIC_BLAST,         //   60
    SPELL_SWARM,
    SPELL_SUMMON_HORRIBLE_THINGS,
    SPELL_ENSLAVEMENT,
    SPELL_MAGIC_MAPPING,
    SPELL_HEAL_OTHER,                       //   65
    SPELL_ANIMATE_DEAD,
    SPELL_PAIN,
    SPELL_EXTENSION,
    SPELL_CONTROL_UNDEAD,
    SPELL_ANIMATE_SKELETON,                 //   70
    SPELL_VAMPIRIC_DRAINING,
    SPELL_SUMMON_WRAITHS,
    SPELL_DETECT_ITEMS,
    SPELL_BORGNJORS_REVIVIFICATION,
    SPELL_BURN,                             //   75
    SPELL_FREEZE,
    SPELL_SUMMON_ELEMENTAL,
    SPELL_OZOCUBUS_REFRIGERATION,
    SPELL_STICKY_FLAME,
    SPELL_SUMMON_ICE_BEAST,                 //   80
    SPELL_OZOCUBUS_ARMOUR,
    SPELL_CALL_IMP,
    SPELL_REPEL_MISSILES,
    SPELL_BERSERKER_RAGE,
    SPELL_DISPEL_UNDEAD,                    //   85
    SPELL_GUARDIAN,
    SPELL_PESTILENCE,
    SPELL_THUNDERBOLT,
    SPELL_FLAME_OF_CLEANSING,
    SPELL_SHINING_LIGHT,                    //   90
    SPELL_SUMMON_DAEVA,
    SPELL_ABJURATION_II,                    //   92
    SPELL_TWISTED_RESURRECTION = 110,       //  110
    SPELL_REGENERATION,
    SPELL_BONE_SHARDS,
    SPELL_BANISHMENT,
    SPELL_CIGOTUVIS_DEGENERATION,
    SPELL_STING,                            //  115
    SPELL_SUBLIMATION_OF_BLOOD,
    SPELL_TUKIMAS_DANCE,
    SPELL_HELLFIRE,
    SPELL_SUMMON_DEMON,
    SPELL_DEMONIC_HORDE,                    //  120
    SPELL_SUMMON_GREATER_DEMON,
    SPELL_CORPSE_ROT,
    SPELL_TUKIMAS_VORPAL_BLADE,
    SPELL_FIRE_BRAND,
    SPELL_FREEZING_AURA,                    //  125
    SPELL_LETHAL_INFUSION,
    SPELL_CRUSH,
    SPELL_BOLT_OF_IRON,
    SPELL_STONE_ARROW,
    SPELL_TOMB_OF_DOROKLOHE,                //  130
    SPELL_STONEMAIL,
    SPELL_SHOCK,
    SPELL_SWIFTNESS,
    SPELL_FLY,
    SPELL_INSULATION,                       //  135
    SPELL_ORB_OF_ELECTROCUTION,
    SPELL_DETECT_CREATURES,
    SPELL_CURE_POISON_II,
    SPELL_CONTROL_TELEPORT,
    SPELL_POISON_AMMUNITION,                //  140
    SPELL_POISON_WEAPON,
    SPELL_RESIST_POISON,
    SPELL_PROJECTED_NOISE,
    SPELL_ALTER_SELF,
    SPELL_DEBUGGING_RAY,                    //  145
    SPELL_RECALL,
    SPELL_PORTAL,
    SPELL_AGONY,
    SPELL_SPIDER_FORM,
    SPELL_DISRUPT,                          //  150
    SPELL_DISINTEGRATE,
    SPELL_BLADE_HANDS,
    SPELL_STATUE_FORM,
    SPELL_ICE_FORM,
    SPELL_DRAGON_FORM,                      //  155
    SPELL_NECROMUTATION,
    SPELL_DEATH_CHANNEL,
    SPELL_SYMBOL_OF_TORMENT,
    SPELL_DEFLECT_MISSILES,
    SPELL_ORB_OF_FRAGMENTATION,             //  160
    SPELL_ICE_BOLT,
    SPELL_ICE_STORM,
    SPELL_ARC,
    SPELL_AIRSTRIKE,
    SPELL_SHADOW_CREATURES,                 //  165
    SPELL_NO_SPELL = 210                    //  210 - added 22jan2000 {dlb}
};

enum SPELL_TYPES
{
    SPTYP_CONJURATION = 11,                 //   11
    SPTYP_ENCHANTMENT,
    SPTYP_FIRE,
    SPTYP_ICE,
    SPTYP_TRANSMIGRATION,                   //   15
    SPTYP_NECROMANCY,
    SPTYP_HOLY,
    SPTYP_SUMMONING,
    SPTYP_DIVINATION,
    SPTYP_TRANSLOCATION,                    //   20
    SPTYP_POISON,
    SPTYP_EARTH,
    SPTYP_AIR
};

enum STATS
{
    STAT_STRENGTH,                          //    0
    STAT_DEXTERITY,
    STAT_INTELLIGENCE
};

enum STAVES
{
    STAFF_WIZARDRY,                         //    0
    STAFF_POWER,
    STAFF_FIRE,
    STAFF_COLD,
    STAFF_POISON,
    STAFF_ENERGY,                           //    5
    STAFF_DEATH,
    STAFF_CONJURATION,
    STAFF_ENCHANTMENT,
    STAFF_SUMMONING_I,
    STAFF_SMITING,                          //   10
    STAFF_SUMMONING_II,
    STAFF_DESTRUCTION_I,
    STAFF_DESTRUCTION_II,
    STAFF_DESTRUCTION_III,
    STAFF_DESTRUCTION_IV,                   //   15
    STAFF_WARDING,
    STAFF_DISCOVERY,
    STAFF_DEMONOLOGY,                       //   18
    STAFF_AIR = 25,                         //   25
    STAFF_EARTH,
    STAFF_CHANNELING
};

enum SYMBOLS                                // beam[].type
{
    SYM_MISSILE = ')',
    SYM_SPORE = '+',
    SYM_ZAP = '#',
    SYM_DEBUG = 'X'
};

enum TRANSFORMATIONS
{
    TRAN_NONE,                              //    0
    TRAN_SPIDER,
    TRAN_BLADE_HANDS,
    TRAN_STATUE,
    TRAN_ICE_BEAST,
    TRAN_DRAGON,                            //    5
    TRAN_LICH
};

enum TRAPS
{
    TRAP_DART,                              //    0
    TRAP_ARROW,
    TRAP_SPEAR,
    TRAP_AXE,
    TRAP_TELEPORT,
    TRAP_AMNESIA,                           //    5
    TRAP_BLADE,
    TRAP_BOLT,
    TRAP_ZOT
};

enum UNARMED_ATTACKS
{
    UNAT_NO_ATTACK,                         //    0
    UNAT_KICK,
    UNAT_HEADBUTT,
    UNAT_TAILSLAP,
    UNAT_PUNCH
};

enum UNDEAD_STATES                          // you.is_undead
{
    US_ALIVE,                               //    0
    US_HUNGRY_DEAD,
    US_UNDEAD
};

enum VORPAL_DESCRIPTIONS
{
    DVORP_CRUSHING,                         //    0
    DVORP_SLICING,
    DVORP_PIERCING,
    DVORP_CHOPPING
};

enum WANDS
{
    WAND_FLAME,                             //    0
    WAND_FROST,
    WAND_SLOWING,
    WAND_HASTING,
    WAND_MAGIC_DARTS,
    WAND_HEALING,                           //    5
    WAND_PARALYSIS,
    WAND_FIRE,
    WAND_COLD,
    WAND_CONFUSION,
    WAND_INVISIBILITY,                      //   10
    WAND_DIGGING,
    WAND_FIREBALL,
    WAND_TELEPORTATION,
    WAND_LIGHTNING,
    WAND_POLYMORPH_OTHER,                   //   15
    WAND_ENSLAVEMENT,
    WAND_DRAINING,
    WAND_RANDOM_EFFECTS,
    WAND_DISINTEGRATION
};

enum WEAPONS
{
// Base weapons
    WPN_CLUB,                               //    0
    WPN_MACE,
    WPN_FLAIL,
    WPN_DAGGER,
    WPN_MORNINGSTAR,
    WPN_SHORT_SWORD,                        //    5
    WPN_LONG_SWORD,
    WPN_GREAT_SWORD,
    WPN_SCIMITAR,
    WPN_HAND_AXE,
    WPN_BATTLEAXE,                          //   10
    WPN_SPEAR,
    WPN_HALBERD,
    WPN_SLING,
    WPN_BOW,
    WPN_CROSSBOW,                           //   15
    WPN_HAND_CROSSBOW,
    WPN_GLAIVE,
    WPN_QUARTERSTAFF,
// these three not created ordinarily
    WPN_SCYTHE,
    WPN_GIANT_CLUB,                         //   20
    WPN_GIANT_SPIKED_CLUB,
// "rare" weapons - some have special cases and are uncommon
    WPN_EVENINGSTAR,
    WPN_QUICK_BLADE,
    WPN_KATANA,
    WPN_EXECUTIONERS_AXE,                   //   25
    WPN_DOUBLE_SWORD,
    WPN_TRIPLE_SWORD,
    WPN_HAMMER,
    WPN_ANCUS,
    WPN_WHIP,                               //   30
    WPN_SABRE,
    WPN_DEMON_BLADE,
    WPN_DEMON_WHIP,
    WPN_DEMON_TRIDENT,
    WPN_BROAD_AXE,                          //   35
// base items (continued)
    WPN_AXE,
    WPN_TRIDENT,
    WPN_SPIKED_FLAIL,
    WPN_GREAT_MACE,
    WPN_GREAT_FLAIL,                        //   40
    WPN_KNIFE,
// special cases
    WPN_UNARMED = 500,                      //  500
    WPN_UNKNOWN = 1000                      // 1000
};

enum WEAPON_DESCRIPTIONS
{
    DWPN_RUNED = 1,                         //    1
    DWPN_GLOWING,
    DWPN_ORCISH,
    DWPN_ELVEN,
    DWPN_DWARVEN                            //    5
};

enum WEAPON_PROPERTIES
{
    PWPN_DAMAGE,                            //    0
    PWPN_HIT,
    PWPN_SPEED
};

enum ZAPS                                   // zapping(), zappy()
{
    ZAP_FLAME,                              //    0
    ZAP_FROST,
    ZAP_SLOWING,
    ZAP_HASTING,
    ZAP_MAGIC_DARTS,
    ZAP_HEALING,                            //    5
    ZAP_PARALYSIS,
    ZAP_FIRE,
    ZAP_COLD,
    ZAP_CONFUSION,
    ZAP_INVISIBILITY,                       //   10
    ZAP_DIGGING,
    ZAP_FIREBALL,
    ZAP_TELEPORTATION,
    ZAP_LIGHTNING,
    ZAP_POLYMORPH_OTHER,                    //   15
    ZAP_VENOM_BOLT,
    ZAP_NEGATIVE_ENERGY,
    ZAP_CRYSTAL_SPEAR,
    ZAP_BEAM_OF_ENERGY,
    ZAP_ORB_OF_ENERGY,                      //   20
    ZAP_ENSLAVEMENT,
    ZAP_PAIN,
    ZAP_STICKY_FLAME,
    ZAP_DISPEL_UNDEAD,
    ZAP_CLEANSING_FLAME,                    //   25
    ZAP_BONE_SHARDS,
    ZAP_BANISHMENT,
    ZAP_DEGENERATION,
    ZAP_STING,
    ZAP_HELLFIRE,                           //   30
    ZAP_IRON_BOLT,
    ZAP_PEBBLE,
    ZAP_STONE_ARROW,
    ZAP_ELECTRICITY,
    ZAP_ORB_OF_ELECTRICITY,                 //   35
    ZAP_SPIT_POISON,
    ZAP_DEBUGGING_RAY,
    ZAP_BREATHE_FIRE,
    ZAP_BREATHE_FROST,
    ZAP_BREATHE_ACID,                       //   40
    ZAP_BREATHE_POISON,
    ZAP_BREATHE_POWER,
    ZAP_ENSLAVE_UNDEAD,
    ZAP_AGONY,
    ZAP_DISRUPTION,                         //   45
    ZAP_DISINTEGRATION,                     //   46
    ZAP_ISKS_CROSS,                         //   47: Isk's Cross (commented out) {dlb}
    ZAP_BREATHE_STEAM = 48,                 //   48
    ZAP_CONTROL_DEMON,
    ZAP_ORB_OF_FRAGMENTATION,               //   50
    ZAP_ICE_BOLT,
    ZAP_ICE_STORM
};


#endif          // ENUM_H

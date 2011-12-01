/*
 *  File:       defines.h
 *  Summary:    Various definess used by Crawl.
 *  Written by: Linley Henzel
 *
 *      Abstract:       A variety of miscellaneous constant values are found here.
 *                      I think we should move the colors into an enum or something
 *                      because there are in numerical order.  But I'm too lazy to
 *                      do it myself.
 *
 *  Copyright © 1999 Brian Robinson.  // Me?  How come?
 *
 *  Change History (most recent first):
 *
 *       <4>     7/29/00        JDJ     Renamed MNG NON_MONSTER, MNST MAX_MONSTERS, ITEMS MAX_ITEMS,
 *                                      ING NON_ITEM, CLOUDS MAX_CLOUDS, CNG EMPTY_CLOUD, NTRAPS MAX_TRAPS.
 *       <3>     9/25/99        CDL     linuxlib -> liblinux
 *       <2>     6/17/99        BCR     indented and added header
 *       <1>     --/--/--       LRH     Created
 */

#ifndef DEFINES_H
#define DEFINES_H

#ifndef NULL
    #define NULL 0
#endif

// ... was 5, 12, 21
#define HIT_WEAK 7
#define HIT_MED 18
#define HIT_STRONG 36

// These two depths are artificial... they've just always been these values.
#define ABYSS_DEPTH                     51
#define PANDEMONIUM_DEPTH               52

// Depth of hell demonlord levels (starting from 26 from the vestibule)
#define HELL_DEPTH                      33

#define INFO_SIZE       200          // size of message buffers
#define ITEMNAME_SIZE   200          // size of item names/shop names/etc
#define BEAMNAME_SIZE   40           // size of beam names
#define HIGHSCORE_SIZE  800          // <= 10 Lines for long format scores

#define MAX_LEVELS                      50
#define MAX_BRANCHES                    30

#define MAX_SKILL_ORDER                 100

#define MAX_ITEM_CREATE_LEVEL           40

// Note: MAX SUBTYPES, UNIQ_ITEMS, UNIQ_MONS, SKILLS, MUTATIONS are important
// to savefiles and so cannot easily be changed.

// max number of subtypes for item_description
#define MAX_SUBTYPES                    50

// NOTE: Max Unique items must == max subtypes because of the way they're saved
#define MAX_UNIQ_ITEMS                  MAX_SUBTYPES
#define MAX_UNIQ_MONS                   50

#define MAX_SKILLS                      50
#define MAX_MUTATIONS                   100

#define MAX_RAND_NAME_LEN               20

// Maximum charge level for rods
#define MAX_ROD_CHARGE                  9
#define ROD_CHARGE_MULT                 100

// Maximum enchantment on weapons/armour/secondary armours
// Note: use armour_max_enchant(item) to get the correct limit for item
#define MAX_WPN_ENCHANT                 5
#define MAX_ARM_ENCHANT                 5
#define MAX_SEC_ENCHANT                 2

#define MAX_RING_PLUS                   5

// Maximum number of spells the character can learn (ignoring Selective Amnesia)
#define MAX_LEARNT_SPELLS               20

// Some defines for item adjective/material descriptors
#define NUM_WANDS_MAT                   12
#define NUM_WANDS_ADJ                   16

#define NUM_POTIONS_MAT                 14
#define NUM_POTIONS_ADJ                 17

#define NUM_RINGS_MAT                   16
#define NUM_RINGS_ADJ                   14

#define NUM_BOOKS_MAT                    6
#define NUM_BOOKS_ADJ                    9

#define NUM_STAVE_ADJ                   10


// This value is used to make test_hit checks always succeed
#define AUTOMATIC_HIT                   1500

// This is the percentage of the time that hit/miss is decided by coin toss.
#define MIN_HIT_MISS_PERCENTAGE         5

// Value of a plus to hit:
#define TOHIT_PLUS_MULTIPLE             5


// This value is used to mark immune levels of MR
#define MAG_IMMUNE                      5000

// This is the damage amount used to signal insta-death
#define INSTANT_DEATH                   -9999

// grids that monsters can see
#define MONSTER_LOS_RANGE               8

// most items allowed in a shop
#define MAX_SHOP_ITEMS                  16

#define MAX_NUM_GODS                    21

// sound level standards
// mininum is the base, we add mult * radius to it:
#define SL_EXPLODE_MIN                  10
#define SL_EXPLODE_MULT                 10

// #define SL_BOW                          3
#define SL_TRAP_CLICK                   3
#define SL_HISS                         6
#define SL_BUZZ                         6
#define SL_GROWL                        8
#define SL_MOAN                         8
#define SL_SPLASH                       8
#define SL_CREAK                        8
#define SL_CROAK                        8
#define SL_BARK                         10
#define SL_YELL                         10
#define SL_TRAP_JAM                     12
#define SL_SHRIEK                       12
#define SL_ROAR                         15
#define SL_DIG                          15
#define SL_NOISY_WEAPON                 20
#define SL_HORN                         25
#define SL_NOISE_SCROLL                 30
#define SL_THUNDER                      30
#define SL_PROJECTED_NOISE              30
#define SL_EARTHQUAKE                   30
#define SL_TRAP_ZOT                     30

// some key points for you.hunger
#define HUNGER_MAX_FOOD                 12000
#define HUNGER_STARTING_LEVEL           (HUNGER_MAX_FOOD / 2)
// note that these levels mark boundaraies centered on satiated
#define HUNGER_STARVING                 1000
#define HUNGER_HUNGRY                   2600
#define HUNGER_FULL                     (HUNGER_MAX_FOOD / 2 + 1000)
#define HUNGER_ENGORGED                 (HUNGER_MAX_FOOD - 1000)

// sub-levels of starvation:
#define HUNGER_PARALYSIS_LEVEL          (HUNGER_STARVING / 2)
#define HUNGER_STARVATION_DEATH         100

#define NUM_MONSTER_SPELL_SLOTS         6

// most ansi-friendly way I can think of defining this.
#define ESCAPE '\x1b'

// there's got to be a better way...
#ifdef _LIBLINUX_IMPLEMENTATION
#elif macintosh
#else
    #ifndef TRUE
     #define TRUE 1
    #endif

    #ifndef FALSE
     #define FALSE 0
    #endif
#endif

#define MINIMUM( xxx, yyy )     (((xxx) < (yyy)) ? (xxx) : (yyy))
#define MAXIMUM( xxx, yyy )     (((xxx) > (yyy)) ? (xxx) : (yyy))

#define BOUNDS( xxx, yyy, zzz ) (((xxx) < (yyy)) ? (yyy) : ((xxx) > (zzz)) ? (zzz) : (xxx))

// Convert capital letters into mystic numbers representing
// CTRL sequences.  This is a macro because a lot of the time
// it wants to be used in case labels.
#define CONTROL( xxx )          (xxx - 'A' + 1)

// max size of inventory array {dlb}:
#define ENDOFPACK               52

// amount of energy monster needs to act:
#define ENERGY_THRESHOLD        80

// value of you.exhaustion where fatigue becomes axhaustion
#define EXHAUSTION_THRESHOLD    20

// minimum value for strength required on armour and weapons
#define STR_REQ_THRESHOLD       10

// max size of monter array {dlb} (do not change, see below)
#define MAX_MONSTERS            200
// number of monster enchantments
#define NUM_MON_ENCHANTS        6
// non-monster for mgrd[][] -- (MNST + 1) {dlb}:
#define NON_MONSTER             (MAX_MONSTERS + 1)

// These are used in the same context as monster indices, and is
// stored and passed around.  The result being that you shouldn't
// change these (or MAX_MONSTERS).
#define MHITNOT                 201
#define MHITYOU                 202


// max size of item list {dlb}:
#define MAX_ITEMS               500
// non-item -- (ITEMS + 1) {dlb}
#define NON_ITEM                (MAX_ITEMS + 1)

// max size of cloud array {dlb}:
#define MAX_CLOUDS              100

// empty cloud -- (CLOUDS + 1) {dlb}:
#define EMPTY_CLOUD             (MAX_CLOUDS + 1)

// max x-bound for level generation {dlb}
#define GXM                     80
// max y-bound for level generation {dlb}
#define GYM                     70

// this is the size of the border around the playing area (see in_bounds())
#define BOUNDARY_BORDER         4

// Now some defines about the actual play area:
// Note: these boundaries are exclusive for the zone the player can move/dig,
// and are inclusive for the area that we display on the map.
// Note: that the right (bottom) boundary is one smaller here.
#define X_BOUND_1               BOUNDARY_BORDER
#define X_BOUND_2               (GXM - BOUNDARY_BORDER)
#define X_WIDTH                 (X_BOUND_2 - X_BOUND_1 + 1)

#define Y_BOUND_1               BOUNDARY_BORDER
#define Y_BOUND_2               (GYM - BOUNDARY_BORDER)
#define Y_WIDTH                 (Y_BOUND_2 - Y_BOUND_1 + 1)

// these mark the center zone where the player moves without shifting
#define ABYSS_SHIFT_RADIUS      10

#define X_ABYSS_1               (X_BOUND_1 + ABYSS_SHIFT_RADIUS)
#define X_ABYSS_2               (GXM - X_ABYSS_1)
#define X_ABYSS_WIDTH           (X_ABYSS_2 - X_ABYSS_1 + 1)
#define X_ABYSS_CENTER          (X_ABYSS_1 + X_ABYSS_WIDTH / 2)

#define Y_ABYSS_1               (Y_BOUND_1 + ABYSS_SHIFT_RADIUS)
#define Y_ABYSS_2               (GYM - Y_ABYSS_1)
#define Y_ABYSS_WIDTH           (Y_ABYSS_2 - Y_ABYSS_1 + 1)
#define Y_ABYSS_CENTER          (Y_ABYSS_1 + Y_ABYSS_WIDTH / 2)

// max traps per level
#define MAX_TRAPS               30

// max shops per level
#define MAX_SHOPS               5

// vision range
#define MAX_VISION_RANGE        8
#define VISION_RANGE_DEFAULT    8

// This value is used to mark that the current berserk is free from
// penalty (Xom's granted or from a deck of cards).
#define NO_BERSERK_PENALTY    -1

// used to mark monster enchantments as indefinate duration.
#define PERM_MON_ENCH    -1


// some shortcuts:
#define menv   env.mons
#define mitm   env.item
#define grd    env.grid
#define mgrd   env.mgrid
#define igrd   env.igrid

// colors, such pretty colors ...
#ifndef DOS
    #define BLACK               0
    #define BLUE                1
    #define GREEN               2
    #define CYAN                3
    #define RED                 4
    #define MAGENTA             5
    #define BROWN               6
    #define LIGHTGREY           7
    #define DARKGREY            8
    #define LIGHTBLUE           9
    #define LIGHTGREEN          10
    #define LIGHTCYAN           11
    #define LIGHTRED            12
    #define LIGHTMAGENTA        13
    #define YELLOW              14
    #define WHITE               15

    #define LIGHTGRAY           LIGHTGREY
    #define DARKGRAY            DARKGREY
#else
    #include <conio.h>
    #define LIGHTGREY           LIGHTGRAY
    #define DARKGREY            DARKGRAY
#endif

// Colour options... these are used as bit flags along with the colour
// value in the low byte.

#ifdef USE_CURSES

// This is used to signal curses (which has seven base colours) to
// try to get a brighter version using recommisioned attribute flags.
#define COLFLAG_CURSES_BRIGHTEN          0x0080

#endif

#define COLFLAG_FRIENDLY_MONSTER         0x0100

enum char_attributes
{
    CHATTR_NORMAL,
    CHATTR_STANDOUT,
    CHATTR_BOLD,
    CHATTR_BLINK,
    CHATTR_UNDERLINE,
    CHATTR_REVERSE,
    CHATTR_DIM
};

#endif

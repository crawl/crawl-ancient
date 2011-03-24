/*
 *  File:       randart.cc
 *  Summary:    Random and unrandom artefact functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *   <8>     19 Jun 99   GDL    added IBMCPP support
 *   <7>     14/12/99    LRH    random2 -> random5
 *   <6>     11/06/99    cdl    random4 -> random2
 *
 *   <1>     -/--/--     LRH    Created
 */

#include "AppHdr.h"
#include "randart.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "dungeon.h"
#include "globals.h"
#include "externs.h"
#include "itemname.h"
#include "itemprop.h"
#include "shopping.h"
#include "stuff.h"

/*
   The initial generation of a randart is very simple - it occurs
   in dungeon.cc and consists of giving it a few random things - plus & plus2
   mainly.
*/
static const char *Rand_Wpn_Names[] =
{
    " of Blood",
    " of Death",
    " of Bloody Death",
    " of Pain",
    " of Painful Death",
    " of Pain & Death",
    " of Infinite Pain",
    " of Eternal Torment",
    " of Power",
    " of Wrath",
/* 10: */
    " of Doom",
    " of Tender Mercy",
    " of the Apocalypse",
    " of the Jester",
    " of the Ring",
    " of the Fool",
    " of the Gods",
    " of the Imperium",
    " of Destruction",
    " of Armageddon",
/* 20: */
    " of Cruel Justice",
    " of Righteous Anger",
    " of Might",
    " of the Orb",
    " of Makhleb",
    " of Trog",
    " of Xom",
    " of the Ancients",
    " of Mana",
    " of Nemelex Xobeh",
/* 30: */
    " of the Magi",
    " of the Archmagi",
    " of the King",
    " of the Queen",
    " of the Spheres",
    " of Circularity",
    " of Linearity",
    " of Conflict",
    " of Battle",
    " of Honour",
/* 40: */
    " of the Butterfly",
    " of the Wasp",
    " of the Frog",
    " of the Weasel",
    " of the Troglodytes",
    " of the Pill-Bug",
    " of Sin",
    " of Vengeance",
    " of Execution",
    " of Arbitration",
/* 50: */
    " of the Seeker",
    " of Truth",
    " of Lies",
    " of the Eggplant",
    " of the Turnip",
    " of Chance",
    " of Curses",
    " of Hell's Wrath",
    " of the Undead",
    " of Chaos",
/* 60: */
    " of Law",
    " of Life",
    " of the Old World",
    " of the New World",
    " of the Middle World",
    " of Crawl",
    " of Unpleasantness",
    " of Discomfort",
    " of Brutal Revenge",
    " of Triumph",
/* 70: */
    " of Evisceration",
    " of Dismemberment",
    " of Terror",
    " of Fear",
    " of Pride",
    " of the Volcano",
    " of Blood-Lust",
    " of Division",
    " of Eternal Harmony",
    " of Peace",
/* 80: */
    " of Quick Death",
    " of Instant Death",
    " of Misery",
    " of the Whale",
    " of the Lobster",
    " of the Whelk",
    " of the Penguin",
    " of the Puffin",
    " of the Mushroom",
    " of the Toadstool",
/* 90: */
    " of the Little People",
    " of the Puffball",
    " of Spores",
    " of Optimality",
    " of Pareto-Optimality",
    " of Greatest Utility",
    " of Anarcho-Capitalism",
    " of Ancient Evil",
    " of the Revolution",
    " of the People",
/* 100: */
    " of the Elves",
    " of the Dwarves",
    " of the Orcs",
    " of the Humans",
    " of Sludge",
    " of the Naga",
    " of the Trolls",
    " of the Ogres",
    " of Equitable Redistribution",
    " of Wealth",
/* 110: */
    " of Poverty",
    " of Reapportionment",
    " of Fragile Peace",
    " of Reinforcement",
    " of Beauty",
    " of the Slug",
    " of the Snail",
    " of the Gastropod",
    " of Corporal Punishment",
    " of Capital Punishment",
/* 120: */
    " of the Beast",
    " of Light",
    " of Darkness",
    " of Day",
    " of the Day",
    " of Night",
    " of the Night",
    " of Twilight",
    " of the Twilight",
    " of Dawn",
/* 130: */
    " of the Dawn",
    " of the Sun",
    " of the Moon",
    " of Distant Worlds",
    " of the Unseen Realm",
    " of Pandemonium",
    " of the Abyss",
    " of the Nexus",
    " of the Gulag",
    " of the Crusades",
/* 140: */
    " of Proximity",
    " of Wounding",
    " of Peril",
    " of the Eternal Warrior",
    " of the Eternal War",
    " of Evil",
    " of Pounding",
    " of Oozing Pus",
    " of Pestilence",
    " of Plague",
/* 150: */
    " of Negation",
    " of the Saviour",
    " of Infection",
    " of Defence",
    " of Protection",
    " of Defence by Offence",
    " of Expedience",
    " of Reason",
    " of Unreason",
    " of the Heart",
/* 160: */
    " of Offence",
    " of the Leaf",
    " of Leaves",
    " of Winter",
    " of Summer",
    " of Autumn",
    " of Spring",
    " of Midsummer",
    " of Midwinter",
    " of Eternal Night",
/* 170: */
    " of Shrieking Terror",
    " of the Lurker",
    " of the Crawling Thing",
    " of the Thing",
    " \"Thing\"",
    " of the Sea",
    " of the Forest",
    " of the Trees",
    " of Earth",
    " of the World",
/* 180: */
    " of Bread",
    " of Yeast",
    " of the Amoeba",
    " of Deformation",
    " of Guilt",
    " of Innocence",
    " of Ascent",
    " of Descent",
    " of Music",
    " of Brilliance",
/* 190: */
    " of Disgust",
    " of Feasting",
    " of Sunlight",
    " of Starshine",
    " of the Stars",
    " of Dust",
    " of the Clouds",
    " of the Sky",
    " of Ash",
    " of Slime",
/* 200: */
    " of Clarity",
    " of Eternal Vigilance",
    " of Purpose",
    " of the Moth",
    " of the Goat",
    " of Fortitude",
    " of Equivalence",
    " of Balance",
    " of Unbalance",
    " of Harmony",
/* 210: */
    " of Disharmony",
    " of the Inferno",
    " of the Omega Point",
    " of Inflation",
    " of Deflation",
    " of Supply",
    " of Demand",
    " of Gross Domestic Product",
    " of Unjust Enrichment",
    " of Detinue",
/* 220: */
    " of Conversion",
    " of Anton Piller",
    " of Mandamus",
    " of Frustration",
    " of Breach",
    " of Fundamental Breach",
    " of Termination",
    " of Extermination",
    " of Satisfaction",
    " of Res Nullius",
/* 230: */
    " of Fee Simple",
    " of Terra Nullius",
    " of Context",
    " of Prescription",
    " of Freehold",
    " of Tortfeasance",
    " of Omission",
    " of Negligence",
    " of Pains",
    " of Attainder",
/* 240: */
    " of Action",
    " of Inaction",
    " of Truncation",
    " of Defenestration",
    " of Desertification",
    " of the Wilderness",
    " of Psychosis",
    " of Neurosis",
    " of Fixation",
    " of the Open Hand",
/* 250: */
    " of the Tooth",
    " of Honesty",
    " of Dishonesty",
    " of Divine Compulsion",
    " of the Invisible Hand",
    " of Freedom",
    " of Liberty",
    " of Servitude",
    " of Domination",
    " of Tension",
/* 260: */
    " of Monotheism",
    " of Atheism",
    " of Agnosticism",
    " of Existentialism",
    " of the Good",
    " of Relativism",
    " of Absolutism",
    " of Absolution",
    " of Abstinence",
    " of Abomination",
/* 270: */
    " of Mutilation",
    " of Stasis",
    " of Wonder",
    " of Dullness",
    " of Dim Light",
    " of the Shining Light",
    " of Immorality",
    " of Amorality",
    " of Precise Incision",
    " of Orthodoxy",
/* 280: */
    " of Faith",
    " of Untruth",
    " of the Augurer",
    " of the Water Diviner",
    " of the Soothsayer",
    " of Punishment",
    " of Amelioration",
    " of Sulphur",
    " of the Egg",
    " of the Globe",
/* 290: */
    " of the Candle",
    " of the Candelabrum",
    " of the Vampires",
    " of the Orcs",
    " of the Halflings",
    " of World's End",
    " of Blue Skies",
    " of Red Skies",
    " of Orange Skies",
    " of Purple Skies",
/* 300: */
    " of Articulation",
    " of the Mind",
    " of the Spider",
    " of the Lamprey",
    " of the Beginning",
    " of the End",
    " of Severance",
    " of Sequestration",
    " of Mourning",
    " of Death's Door",
/* 310: */
    " of the Key",
    " of Earthquakes",
    " of Failure",
    " of Success",
    " of Intimidation",
    " of the Mosquito",
    " of the Gnat",
    " of the Blowfly",
    " of the Turtle",
    " of the Tortoise",
/* 320: */
    " of the Pit",
    " of the Grave",
    " of Submission",
    " of Dominance",
    " of the Messenger",
    " of Crystal",
    " of Gravity",
    " of Levity",
    " of the Slorg",
    " of Surprise",
/* 330: */
    " of the Maze",
    " of the Labyrinth",
    " of Divine Intervention",
    " of Rotation",
    " of the Spinneret",
    " of the Scorpion",
    " of Demonkind",
    " of the Genius",
    " of Bloodstone",
    " of Grontol",
/* 340: */
    " \"Grim Tooth\"",
    " \"Widowmaker\"",
    " \"Widowermaker\"",
    " \"Lifebane\"",
    " \"Conservator\"",
    " \"Banisher\"",
    " \"Tormentor\"",
    " \"Secret Weapon\"",
    " \"String\"",
    " \"Stringbean\"",
/* 350: */
    " \"Blob\"",
    " \"Globulus\"",
    " \"Hulk\"",
    " \"Raisin\"",
    " \"Starlight\"",
    " \"Giant's Toothpick\"",
    " \"Pendulum\"",
    " \"Backscratcher\"",
    " \"Brush\"",
    " \"Murmur\"",
/* 360: */
    " \"Sarcophage\"",
    " \"Concordance\"",
    " \"Dragon's Tongue\"",
    " \"Arbiter\"",
    " \"Gram\"",
    " \"Grom\"",
    " \"Grim\"",
    " \"Grum\"",
    " \"Rummage\"",
    " \"Omelette\"",
/* 370: */
    " \"Egg\"",
    " \"Aubergine\"",
    " \"Z\"",
    " \"X\"",
    " \"Q\"",
    " \"Ox\"",
    " \"Death Rattle\"",
    " \"Tattletale\"",
    " \"Fish\"",
    " \"Bung\"",
/* 380: */
    " \"Arcanum\"",
    " \"Mud Pie of Death\"",
    " \"Transmigrator\"",
    " \"Ultimatum\"",
    " \"Earthworm\"",
    " \"Worm\"",
    " \"Worm's Wrath\"",
    " \"Xom's Favour\"",
    " \"Bingo\"",
    " \"Leviticus\"",
// Not yet possible...
/* 390: */
    " of Joyful Slaughter",
};

static const int Num_Wpn_Names = sizeof( Rand_Wpn_Names ) / sizeof( char * );

static const char *Rand_Arm_Names[] =
{
/* 0: */
    " of Shielding",
    " of Grace",
    " of Impermeability",
    " of the Onion",
    " of Life",
    " of Defence",
    " of Nonsense",
    " of Eternal Vigilance",
    " of Fun",
    " of Joy",
/* 10: */
    " of Death's Door",
    " of the Gate",
    " of Watchfulness",
    " of Integrity",
    " of Bodily Harmony",
    " of Harmony",
    " of the Untouchables",
    " of Grot",
    " of Grottiness",
    " of Filth",
/* 20: */
    " of Wonder",
    " of Wondrous Power",
    " of Power",
    " of Vlad",
    " of the Eternal Fruit",
    " of Invincibility",
    " of Hide-and-Seek",
    " of the Mouse",
    " of the Saviour",
    " of Plasticity",
/* 30: */
    " of Baldness",
    " of Terror",
    " of the Arcane",
    " of Resist Death",
    " of Anaesthesia",
    " of the Guardian",
    " of Inviolability",
    " of the Tortoise",
    " of the Turtle",
    " of the Armadillo",
/* 40: */
    " of the Echidna",
    " of the Armoured One",
    " of Weirdness",
    " of Pathos",
    " of Serendipity",
    " of Loss",
    " of Hedging",
    " of Indemnity",
    " of Limitation",
    " of Exclusion",
/* 50: */
    " of Repulsion",
    " of Untold Secrets",
    " of the Earth",
    " of the Turtledove",
    " of Limited Liability",
    " of Responsibility",
    " of Hadjma",
    " of Glory",
    " of Preservation",
    " of Conservation",
/* 60: */
    " of Protective Custody",
    " of the Clam",
    " of the Barnacle",
    " of the Lobster",
    " of Hairiness",
    " of Supple Strength",
    " of Space",
    " of the Vacuum",
    " of Compression",
    " of Decompression",

/* 70: */
    " of the Loofah",
    " \"Sarcophagus\"",
};

static const int Num_Arm_Names = sizeof( Rand_Arm_Names ) / sizeof( char * );

enum randart_desc_colours
{
    RDC_PLAIN,                  // does not need special colour processing
    RDC_BRIGHTLY_GLOWING,
    RDC_STEAMING,
    RDC_SMOKING,
    RDC_SHIMMERING,
    RDC_CRYSTAL,
    RDC_JEWELLED_IRIDESCENT,
    RDC_TRANSPARENT,
    RDC_ENCRUSTED,
    RDC_SLIMY,
    RDC_BLOODSTAINED,
    RDC_ICHOR_STAINED
};

struct randart_desc_adj
{
    const char             *str;
    randart_desc_colours    colour;
    appearance_type         appear;
    bool                    check; // not_shiny for arm, ring/amu for jewellery
};

#define NUM_DESC_ADJ        26

static const randart_desc_adj Wpn_Desc_Adj[ NUM_DESC_ADJ ] =
{
    { "brightly glowing ", RDC_BRIGHTLY_GLOWING, APP_SPECIAL, false },
    { "steaming ",          RDC_STEAMING, APP_SPECIAL, false },
    { "smoking ", RDC_SMOKING, APP_SPECIAL, false },
    { "bloodstained ", RDC_BLOODSTAINED, APP_SPECIAL, false },
    { "twisted ", RDC_PLAIN, APP_SPECIAL, false },
    { "shimmering ", RDC_PLAIN, APP_SPECIAL, false },
    { "warped ", RDC_PLAIN, APP_SPECIAL, false },
    { "crystal ", RDC_CRYSTAL, APP_SPECIAL, false },
    { "jewelled ", RDC_JEWELLED_IRIDESCENT, APP_SPECIAL, false },
    { "transparent ", RDC_TRANSPARENT, APP_SPECIAL, false },
    { "encrusted ", RDC_ENCRUSTED, APP_SPECIAL, false },
    { "pitted ", RDC_PLAIN, APP_SPECIAL, false },
    { "slimy ", RDC_SLIMY, APP_SPECIAL, false },
    { "polished ", RDC_PLAIN, APP_SPECIAL, false },
    { "fine ", RDC_PLAIN, APP_SPECIAL, false },
    { "crude ", RDC_PLAIN, APP_SPECIAL, false },
    { "ancient ", RDC_PLAIN, APP_SPECIAL, false },
    { "ichor-stained ", RDC_ICHOR_STAINED, APP_SPECIAL, false },
    { "faintly glowing ", RDC_PLAIN, APP_SPECIAL, false },
    { "runed ", RDC_PLAIN, APP_FANCY, false },
    { "runed ", RDC_PLAIN, APP_FANCY, false },
    { "shiny ", RDC_PLAIN, APP_FANCY, false },
    { "shiny ", RDC_PLAIN, APP_FANCY, false },
    { "glowing ", RDC_PLAIN, APP_FANCY, false },
    { "glowing ", RDC_PLAIN, APP_FANCY, false },
    { "", RDC_PLAIN, APP_PLAIN, false },
};

static const randart_desc_adj Arm_Desc_Adj[ NUM_DESC_ADJ ] =
{
    { "brightly glowing ", RDC_BRIGHTLY_GLOWING, APP_SPECIAL, false },
    { "steaming ", RDC_STEAMING, APP_SPECIAL, false },
    { "smoking ", RDC_SMOKING, APP_SPECIAL, false },
    { "bloodstained ", RDC_BLOODSTAINED, APP_SPECIAL, false },
    { "twisted ", RDC_PLAIN, APP_SPECIAL, false },
    { "shimmering ", RDC_PLAIN, APP_SPECIAL, false },
    { "warped ", RDC_PLAIN, APP_SPECIAL, false },
    { "heavily runed ", RDC_PLAIN, APP_SPECIAL, false },
    { "jeweled ", RDC_JEWELLED_IRIDESCENT, APP_SPECIAL, false },
    { "transparent ", RDC_TRANSPARENT, APP_SPECIAL, false },
    { "encrusted ", RDC_ENCRUSTED, APP_SPECIAL, false },
    { "pitted ", RDC_PLAIN, APP_SPECIAL, false },
    { "slimy ", RDC_SLIMY, APP_SPECIAL, false },
    { "faintly glowing ", RDC_PLAIN, APP_SPECIAL, false },
    { "fine ", RDC_PLAIN, APP_SPECIAL, false },
    { "crude ", RDC_PLAIN, APP_SPECIAL, false },
    { "ancient ", RDC_PLAIN, APP_SPECIAL, false },
    { "ichor-stained ", RDC_ICHOR_STAINED, APP_SPECIAL, false },
    { "polished ", RDC_PLAIN, APP_SPECIAL, true }, // sleek for non-shiny
    { "runed ", RDC_PLAIN, APP_FANCY, false },
    { "runed ", RDC_PLAIN, APP_FANCY, false },
    { "glowing ", RDC_PLAIN, APP_FANCY, false },
    { "glowing ", RDC_PLAIN, APP_FANCY, false },
    { "shiny ", RDC_PLAIN, APP_FANCY, true },      // embroidered for non-shiny
    { "shiny ", RDC_PLAIN, APP_FANCY, true },      // embroidered for non-shiny
    { "", RDC_PLAIN, APP_PLAIN, false },
};

static const randart_desc_adj Ring_Desc_Adj[ NUM_DESC_ADJ ] =
{
    { "brightly glowing ", RDC_BRIGHTLY_GLOWING, APP_SPECIAL, false },
    { "steaming ", RDC_STEAMING, APP_SPECIAL, false },
    { "smoking ", RDC_SMOKING, APP_SPECIAL, false },
    { "brilliant ", RDC_PLAIN, APP_SPECIAL, false },
    { "faintly glowing ", RDC_PLAIN, APP_SPECIAL, false },
    { "shimmering ", RDC_PLAIN, APP_SPECIAL, false },
    { "warped ", RDC_PLAIN, APP_SPECIAL, false },
    { "diamond inlaid ", RDC_PLAIN, APP_SPECIAL, false },
    { "iridescent ", RDC_JEWELLED_IRIDESCENT, APP_SPECIAL, false },
    { "transparent ", RDC_TRANSPARENT, APP_SPECIAL, false },
    { "pitted ", RDC_PLAIN, APP_SPECIAL, false },
    { "slimy ", RDC_SLIMY, APP_SPECIAL, false },
    { "polished ", RDC_PLAIN, APP_SPECIAL, false },
    { "fine ", RDC_PLAIN, APP_SPECIAL, false },
    { "crude ", RDC_PLAIN, APP_SPECIAL, false },
    { "ancient ", RDC_PLAIN, APP_SPECIAL, false },
    { "plain ", RDC_PLAIN, APP_SPECIAL, false },

    // these count as special if they're amulets
    { "encrusted ", RDC_ENCRUSTED, APP_PLAIN, true },
    { "shiny ", RDC_PLAIN, APP_PLAIN, true },

    // these appear plain for both rings and amulets
    { "runed ", RDC_PLAIN, APP_PLAIN, false },
    { "runed ", RDC_PLAIN, APP_PLAIN, false },
    { "twisted ", RDC_PLAIN, APP_PLAIN, false },
    { "twisted ", RDC_PLAIN, APP_PLAIN, false },
    { "glowing ", RDC_PLAIN, APP_PLAIN, false },
    { "glowing ", RDC_PLAIN, APP_PLAIN, false },
    { "", RDC_PLAIN, APP_PLAIN, false },
};

// Remember: disallow unrandart creation in abyss/pan

/*
   The following unrandart bits were taken from $pellbinder's mon-util code
   (see mon-util.h & mon-util.cc) and modified (LRH). They're in randart.cc and
   not randart.h because they're only used in this code module.
*/

#if defined(MAC) || defined(__IBMCPP__) || defined(__BCPLUSPLUS__)
#define PACKED
#else
#define PACKED __attribute__ ((packed))
#endif

//int unranddatasize;

#ifdef __IBMCPP__
#pragma pack(push)
#pragma pack(1)
#endif

struct unrandart_entry
{
    const char *name;        // true name of unrandart (max 31 chars)
    const char *unid_name;   // un-id'd name of unrandart (max 31 chars)

    int ura_cl;        // class of ura
    int ura_ty;        // type of ura
    int ura_pl;        // plus of ura
    int ura_pl2;       // plus2 of ura
    int ura_col;       // colour of ura

    int prpty[RAP_NUM_PROPERTIES];

    // special description added to 'v' command output (max 31 chars)
    const char *spec_descrip1;
    // special description added to 'v' command output (max 31 chars)
    const char *spec_descrip2;
    // special description added to 'v' command output (max 31 chars)
    const char *spec_descrip3;
};

#ifdef __IBMCPP__
#pragma pack(pop)
#endif

static struct unrandart_entry unranddata[] = {
#include "unrand.h"
};

char art_n[ ITEMNAME_SIZE ];
static FixedVector < char, NO_UNRANDARTS > unrandart_exist;

static struct unrandart_entry *seekunrandart( const item_def &item );

void set_unrandart_exist(int whun, char is_exist)
{
    unrandart_exist[whun] = is_exist;
}

char does_unrandart_exist(int whun)
{
    return (unrandart_exist[whun]);
}

// returns true is item is a pure randart or an unrandart
bool is_random_artefact( const item_def &item )
{
    return (item.flags & ISFLAG_ARTEFACT_MASK);
}

// returns true if item in an unrandart
bool is_unrandom_artefact( const item_def &item )
{
    return (item.flags & ISFLAG_UNRANDART);
}

// returns true if item is one of the origional fixed artefacts
bool is_fixed_artefact( const item_def &item )
{
    if (!is_random_artefact( item )
        && item.base_type == OBJ_WEAPONS
        && item.special >= SPWPN_SINGING_SWORD)
    {
        return (true);
    }

    return (false);
}

int get_unique_item_status( int base_type, int art )
{
    // Note: for weapons "art" is in item.special,
    //       for orbs it's the sub_type.
    if (base_type == OBJ_WEAPONS)
    {
        if (art >= SPWPN_SINGING_SWORD && art <= SPWPN_SWORD_OF_ZONGULDROK)
            return (you.unique_items[ art - SPWPN_SINGING_SWORD ]);
        else if (art >= SPWPN_SWORD_OF_POWER && art <= SPWPN_STAFF_OF_WUCAD_MU)
            return (you.unique_items[ art - SPWPN_SWORD_OF_POWER + 24 ]);
    }
    else if (base_type == OBJ_ORBS)
    {
        if (art >= 4 && art <= 19)
            return (you.unique_items[ art + 3 ]);

    }

    return (UNIQ_NOT_EXISTS);
}

void set_unique_item_status( int base_type, int art, int status )
{
    // Note: for weapons "art" is in item.special,
    //       for orbs it's the sub_type.
    if (base_type == OBJ_WEAPONS)
    {
        if (art >= SPWPN_SINGING_SWORD && art <= SPWPN_SWORD_OF_ZONGULDROK)
            you.unique_items[ art - SPWPN_SINGING_SWORD ] = status;
        else if (art >= SPWPN_SWORD_OF_POWER && art <= SPWPN_STAFF_OF_WUCAD_MU)
            you.unique_items[ art - SPWPN_SWORD_OF_POWER + 24 ] = status;
    }
    else if (base_type == OBJ_ORBS)
    {
        if (art >= 4 && art <= 19)
            you.unique_items[ art + 3 ] = status;

    }
}

// which == 0 (default) gives random fixed artefact.
// Returns true if successful.
bool make_item_fixed_artefact( item_def &item, bool in_abyss, int which )
{
    bool  force = true;  // we force any one asked for specifically

    if (!which)
    {
        // using old behaviour... try only once. -- bwr
        force = false;

        which = SPWPN_SINGING_SWORD + random2(12);
        if (which >= SPWPN_SWORD_OF_CEREBOV)
            which += 3; // skip over Cerebov's, Dispater's, and Asmodeus' weapons
    }

    int status = get_unique_item_status( OBJ_WEAPONS, which );

    if ((status == UNIQ_EXISTS
            || (in_abyss && status == UNIQ_NOT_EXISTS)
            || (!in_abyss && status == UNIQ_LOST_IN_ABYSS))
        && !force)
    {
        return (false);
    }

    switch (which)
    {
    case SPWPN_SINGING_SWORD:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_LONG_SWORD;
        item.plus  = 7;
        item.plus2 = 6;
        break;

    case SPWPN_WRATH_OF_TROG:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_BATTLEAXE;
        item.plus  = 3;
        item.plus2 = 11;
        break;

    case SPWPN_SCYTHE_OF_CURSES:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_SCYTHE;
        item.plus  = 13;
        item.plus2 = 13;
        break;

    case SPWPN_MACE_OF_VARIABILITY:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_MACE;
        item.plus  = random2(16) - 4;
        item.plus2 = random2(16) - 4;
        break;

    case SPWPN_GLAIVE_OF_PRUNE:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_GLAIVE;
        item.plus  = 0;
        item.plus2 = 12;
        break;

    case SPWPN_SCEPTRE_OF_TORMENT:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_MACE;
        item.plus  = 7;
        item.plus2 = 6;
        break;

    case SPWPN_SWORD_OF_ZONGULDROK:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_LONG_SWORD;
        item.plus  = 9;
        item.plus2 = 9;
        break;

    case SPWPN_SWORD_OF_POWER:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_GREAT_SWORD;
        item.plus  = 0; // set on wield
        item.plus2 = 0; // set on wield
        break;

    case SPWPN_KNIFE_OF_ACCURACY:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_DAGGER;
        item.plus  = 27;
        item.plus2 = -1;
        break;

    case SPWPN_STAFF_OF_OLGREB:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_STAFF;
        item.plus  = 0; // set on wield
        item.plus2 = 0; // set on wield
        break;

    case SPWPN_VAMPIRES_TOOTH:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_DAGGER;
        item.plus  = 3;
        item.plus2 = 4;
        break;

    case SPWPN_STAFF_OF_WUCAD_MU:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_STAFF;
        item.plus  = 0; // set on wield
        item.plus2 = 0; // set on wield
        break;

    case SPWPN_SWORD_OF_CEREBOV:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_GREAT_SWORD;
        item.plus  = 6;
        item.plus2 = 6;
        item.colour = YELLOW;
        do_curse_item( item );
        break;

    case SPWPN_STAFF_OF_DISPATER:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_STAFF;
        item.plus  = 4;
        item.plus2 = 4;
        item.colour = YELLOW;
        break;

    case SPWPN_SCEPTRE_OF_ASMODEUS:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = WPN_STAFF;
        item.plus  = 7;
        item.plus2 = 7;
        item.colour = RED;
        break;

    default:
        DEBUGSTR( "Trying to create illegal fixed artefact!" );
        return (false);
    }

    // If we get here, we've made the artefact
    item.special = which;
    item.quantity = 1;

    // Items originally generated in the abyss and not found will be
    // shifted to "lost in abyss", and will only be found there. -- bwr
    set_unique_item_status( OBJ_WEAPONS, which, UNIQ_EXISTS );

    return (true);
}

static long calc_seed( const item_def &item )
{
    return (item.special & RANDART_SEED_MASK);
}

static int random5( int randmax )
{
    if (randmax <= 1)
        return (0);

    return (static_cast<int>(rand()) / (RAND_MAX / randmax + 1));
    // must use random (not rand) for the predictable-results-from-known
    //  -srandom-seeds thing to work.
}

void old_randart_properties( item_def &item )
{
    ASSERT( is_random_artefact( item ) );

    int i = 0;
    int power_level = 0;

    if (is_unrandom_artefact( item ))
    {
        struct unrandart_entry *unrand = seekunrandart( item );

        for (i = 0; i < RAP_NUM_PROPERTIES; i++)
            item.ra_props[i] = unrand->prpty[i];

        return;
    }

    // long seed = item.base_type * adam + item.sub_type * (aplus % 100) + aplus2 * 100;
    long seed = calc_seed( item );
    long randstore = rand();
    srand( seed );

    if (item.base_type == OBJ_ARMOUR)
        power_level = item.plus / 2 + 2;
    else if (item.base_type == OBJ_JEWELLERY)
        power_level = 1 + random5(3) + random5(2);
    else // OBJ_WEAPON
        power_level = item.plus / 3 + item.plus2 / 3;

    if (power_level < 0)
        power_level = 0;

    for (i = 0; i < RAP_NUM_PROPERTIES; i++)
        item.ra_props[i] = 0;

    if (item.base_type == OBJ_WEAPONS)  /* Only weapons get brands, of course */
    {
        item.ra_props[RAP_BRAND] = SPWPN_FLAMING + random5(15);

        if (random5(6) == 0)
            item.ra_props[RAP_BRAND] = SPWPN_FLAMING + random5(2);

        if (random5(6) == 0)
            item.ra_props[RAP_BRAND] = SPWPN_ORC_SLAYING + random5(4);

        if (random5(6) == 0)
            item.ra_props[RAP_BRAND] = SPWPN_VORPAL;

        if (item.ra_props[RAP_BRAND] == SPWPN_UNUSED_I
            || item.ra_props[RAP_BRAND] == SPWPN_UNUSED_II)
        {
            item.ra_props[RAP_BRAND] = SPWPN_NORMAL;
        }

        if (item.ra_props[RAP_BRAND] == SPWPN_PROTECTION)
            item.ra_props[RAP_BRAND] = SPWPN_NORMAL;

        if (item.ra_props[RAP_BRAND] == SPWPN_DISRUPTION
            && !(item.sub_type == WPN_MACE || item.sub_type == WPN_GREAT_MACE
                || item.sub_type == WPN_HAMMER))
        {
            item.ra_props[RAP_BRAND] = SPWPN_NORMAL;
        }

        // is this happens, things might get broken -- bwr
        if (item.ra_props[RAP_BRAND] == SPWPN_SPEED
            && item.sub_type == WPN_QUICK_BLADE)
        {
            item.ra_props[RAP_BRAND] = SPWPN_NORMAL;
        }

        if (is_range_weapon( item ))
        {
            item.ra_props[RAP_BRAND] = SPWPN_NORMAL;

            if (random5(3) == 0)
            {
                int tmp = random5(20);

                item.ra_props[RAP_BRAND] = (tmp >= 18) ? SPWPN_SPEED :
                                           (tmp >= 14) ? SPWPN_PROTECTION :
                                           (tmp >= 10) ? SPWPN_VENOM
                                                       : SPWPN_FLAMING + (tmp % 2);
            }
        }


        if (is_demonic( item ))
        {
            switch (random5(9))
            {
            case 0:
                item.ra_props[RAP_BRAND] = SPWPN_DRAINING;
                break;
            case 1:
                item.ra_props[RAP_BRAND] = SPWPN_FLAMING;
                break;
            case 2:
                item.ra_props[RAP_BRAND] = SPWPN_FREEZING;
                break;
            case 3:
                item.ra_props[RAP_BRAND] = SPWPN_ELECTROCUTION;
                break;
            case 4:
                item.ra_props[RAP_BRAND] = SPWPN_VAMPIRICISM;
                break;
            case 5:
                item.ra_props[RAP_BRAND] = SPWPN_PAIN;
                break;
            case 6:
                item.ra_props[RAP_BRAND] = SPWPN_VENOM;
                break;
            default:
                power_level -= 2;
            }
            power_level += 2;
        }
        else if (random5(3) == 0)
            item.ra_props[RAP_BRAND] = SPWPN_NORMAL;
        else
            power_level++;
    }

    if (random5(5) == 0)
        goto skip_mods;

    /* AC mod - not for armours or rings of protection */
    if (random5(4 + power_level) == 0
        && item.base_type != OBJ_ARMOUR
        && (item.base_type != OBJ_JEWELLERY || item.sub_type != RING_PROTECTION))
    {
        item.ra_props[RAP_AC] = 1 + random5(3) + random5(3) + random5(3);
        power_level++;
        if (random5(4) == 0)
        {
            item.ra_props[RAP_AC] -= 1 + random5(3) + random5(3) + random5(3);
            power_level--;
        }
    }

    /* ev mod - not for rings of evasion */
    if (random5(4 + power_level) == 0
        && (item.base_type != OBJ_JEWELLERY || item.sub_type != RING_EVASION))
    {
        item.ra_props[RAP_EVASION] = 1 + random5(3) + random5(3) + random5(3);
        power_level++;
        if (random5(4) == 0)
        {
            item.ra_props[RAP_EVASION] -= 1 + random5(3) + random5(3) + random5(3);
            power_level--;
        }
    }

    /* str mod - not for rings of strength */
    if (random5(4 + power_level) == 0
        && (item.base_type != OBJ_JEWELLERY || item.sub_type != RING_STRENGTH))
    {
        item.ra_props[RAP_STRENGTH] = 1 + random5(3) + random5(2);
        power_level++;
        if (random5(4) == 0)
        {
            item.ra_props[RAP_STRENGTH] -= 1 + random5(3) + random5(3) + random5(3);
            power_level--;
        }
    }

    /* int mod - not for rings of intelligence */
    if (random5(4 + power_level) == 0
        && (item.base_type != OBJ_JEWELLERY || item.sub_type != RING_INTELLIGENCE))
    {
        item.ra_props[RAP_INTELLIGENCE] = 1 + random5(3) + random5(2);
        power_level++;
        if (random5(4) == 0)
        {
            item.ra_props[RAP_INTELLIGENCE] -= 1 + random5(3) + random5(3) + random5(3);
            power_level--;
        }
    }

    /* dex mod - not for rings of dexterity */
    if (random5(4 + power_level) == 0
        && (item.base_type != OBJ_JEWELLERY || item.sub_type != RING_DEXTERITY))
    {
        item.ra_props[RAP_DEXTERITY] = 1 + random5(3) + random5(2);
        power_level++;
        if (random5(4) == 0)
        {
            item.ra_props[RAP_DEXTERITY] -= 1 + random5(3) + random5(3) + random5(3);
            power_level--;
        }
    }

  skip_mods:
    if (random5(15) < power_level
        || item.base_type == OBJ_WEAPONS
        || (item.base_type == OBJ_JEWELLERY && item.sub_type == RING_SLAYING))
    {
        goto skip_combat;
    }

    /* Weapons and rings of slaying can't get these */
    if (random5(4 + power_level) == 0)  /* to-hit */
    {
        item.ra_props[RAP_ACCURACY] = 1 + random5(3) + random5(2);
        power_level++;
        if (random5(4) == 0)
        {
            item.ra_props[RAP_ACCURACY] -= 1 + random5(3) + random5(3) + random5(3);
            power_level--;
        }
    }

    if (random5(4 + power_level) == 0)  /* to-dam */
    {
        item.ra_props[RAP_DAMAGE] = 1 + random5(3) + random5(2);
        power_level++;
        if (random5(4) == 0)
        {
            item.ra_props[RAP_DAMAGE] -= 1 + random5(3) + random5(3) + random5(3);
            power_level--;
        }
    }

  skip_combat:
    if (random5(12) < power_level)
        goto finished_powers;

/* res_fire */
    if (random5(4 + power_level) == 0
        && (item.base_type != OBJ_JEWELLERY
            || (item.sub_type != RING_PROTECTION_FROM_FIRE
                && item.sub_type != RING_FIRE
                && item.sub_type != RING_ICE))
        && (item.base_type != OBJ_ARMOUR
            || (item.sub_type != ARM_DRAGON_ARMOUR
                && item.sub_type != ARM_ICE_DRAGON_ARMOUR
                && item.sub_type != ARM_GOLD_DRAGON_ARMOUR)))
    {
        item.ra_props[RAP_FIRE] = 1;
        if (random5(5) == 0)
            item.ra_props[RAP_FIRE]++;
        power_level++;
    }

    /* res_cold */
    if (random5(4 + power_level) == 0
        && (item.base_type != OBJ_JEWELLERY
            || (item.sub_type != RING_PROTECTION_FROM_COLD
                && item.sub_type != RING_FIRE
                && item.sub_type != RING_ICE))
        && (item.base_type != OBJ_ARMOUR
            || (item.sub_type != ARM_DRAGON_ARMOUR
                && item.sub_type != ARM_ICE_DRAGON_ARMOUR
                && item.sub_type != ARM_GOLD_DRAGON_ARMOUR)))
    {
        item.ra_props[RAP_COLD] = 1;
        if (random5(5) == 0)
            item.ra_props[RAP_COLD]++;
        power_level++;
    }

    if (random5(12) < power_level || power_level > 7)
        goto finished_powers;

    /* res_elec */
    if (random5(4 + power_level) == 0
        && (item.base_type != OBJ_ARMOUR
            || item.sub_type != ARM_STORM_DRAGON_ARMOUR))
    {
        item.ra_props[RAP_ELECTRICITY] = 1;
        power_level++;
    }

/* res_poison */
    if (random5(5 + power_level) == 0
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_POISON_RESISTANCE)
        && (item.base_type != OBJ_ARMOUR
            || (item.sub_type != ARM_GOLD_DRAGON_ARMOUR
                && item.sub_type != ARM_SWAMP_DRAGON_ARMOUR)))
    {
        item.ra_props[RAP_POISON] = 1;
        power_level++;
    }

    /* prot_life - no necromantic brands on weapons allowed */
    if (random5(4 + power_level) == 0
        && (item.base_type != OBJ_JEWELLERY || item.sub_type != RING_TELEPORTATION)
        && item.ra_props[RAP_BRAND] != SPWPN_DRAINING
        && item.ra_props[RAP_BRAND] != SPWPN_VAMPIRICISM
        && item.ra_props[RAP_BRAND] != SPWPN_PAIN)
    {
        item.ra_props[RAP_NEGATIVE_ENERGY] = 1;
        power_level++;
    }

    /* res magic */
    if (random5(4 + power_level) == 0
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_PROTECTION_FROM_MAGIC))
    {
        item.ra_props[RAP_MAGIC] = 20 + random5(40);
        power_level++;
    }

    /* see_invis */
    if (random5(4 + power_level) == 0
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_INVISIBILITY))
    {
        item.ra_props[RAP_EYESIGHT] = 1;
        power_level++;
    }

    if (random5(12) < power_level || power_level > 10)
        goto finished_powers;

    /* turn invis */
    if (random5(10) == 0
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_INVISIBILITY))
    {
        item.ra_props[RAP_INVISIBLE] = 1;
        power_level++;
    }

    /* levitate */
    if (random5(10) == 0
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_LEVITATION))
    {
        item.ra_props[RAP_LEVITATE] = 1;
        power_level++;
    }

    if (random5(10) == 0)       /* blink */
    {
        item.ra_props[RAP_BLINK] = 1;
        power_level++;
    }

    /* teleport */
    if (random5(10) == 0
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_TELEPORTATION))
    {
        item.ra_props[RAP_CAN_TELEPORT] = 1;
        power_level++;
    }

    /* go berserk */
    if (random5(10) == 0
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != AMU_RAGE))
    {
        item.ra_props[RAP_BERSERK] = 1;
        power_level++;
    }

    if (random5(10) == 0)       /* sense surr */
    {
        item.ra_props[RAP_MAPPING] = 1;
        power_level++;
    }


  finished_powers:
    /* Armours get less powers, and are also less likely to be
       cursed that wpns */
    if (item.base_type == OBJ_ARMOUR)
        power_level -= 4;

    if (random5(17) >= power_level || power_level < 2)
        goto finished_curses;

    switch (random5(9))
    {
    case 0:                     /* makes noise */
        if (item.base_type != OBJ_WEAPONS)
            break;

        item.ra_props[RAP_NOISES] = 1 + random5(4);
        break;

    case 1:                     /* no magic */
        item.ra_props[RAP_PREVENT_SPELLCASTING] = 1;
        break;

    case 2:                     /* random teleport */
        if (item.base_type != OBJ_WEAPONS)
            break;

        item.ra_props[RAP_CAUSE_TELEPORTATION] = 5 + random5(15);
        break;

    case 3:   /* no teleport - doesn't affect some instantaneous teleports */
        if (item.base_type == OBJ_JEWELLERY
            && (item.sub_type == RING_TELEPORTATION
                || item.sub_type == RING_TELEPORT_CONTROL))
        {
            break;
        }

        item.ra_props[RAP_BLINK] = 0;
        item.ra_props[RAP_CAN_TELEPORT] = 0;
        item.ra_props[RAP_PREVENT_TELEPORTATION] = 1;
        break;

    case 4:                     /* berserk on attack */
        if (item.base_type != OBJ_WEAPONS)
            break;

        item.ra_props[RAP_ANGRY] = 1 + random5(8);
        break;

    case 5:                     /* susceptible to fire */
        if (item.base_type == OBJ_JEWELLERY
            && (item.sub_type == RING_PROTECTION_FROM_FIRE
                || item.sub_type == RING_FIRE
                || item.sub_type == RING_ICE))
        {
            break;
        }

        if (item.base_type == OBJ_ARMOUR
            && (item.sub_type == ARM_DRAGON_ARMOUR
                || item.sub_type == ARM_ICE_DRAGON_ARMOUR
                || item.sub_type == ARM_GOLD_DRAGON_ARMOUR))
        {
            break;
        }

        item.ra_props[RAP_FIRE] = -1;
        break;

    case 6:                     /* susceptible to cold */
        if (item.base_type == OBJ_JEWELLERY
            && (item.sub_type == RING_PROTECTION_FROM_COLD
                || item.sub_type == RING_FIRE
                || item.sub_type == RING_ICE))
        {
            break;              /* already does this or something */
        }

        if (item.base_type == OBJ_ARMOUR
            && (item.sub_type == ARM_DRAGON_ARMOUR
                || item.sub_type == ARM_ICE_DRAGON_ARMOUR
                || item.sub_type == ARM_GOLD_DRAGON_ARMOUR))
        {
            break;
        }

        item.ra_props[RAP_COLD] = -1;
        break;

    case 7:                     /* speed metabolism */
        if (item.base_type == OBJ_JEWELLERY && item.sub_type == RING_HUNGER)
            break;

        if (item.base_type == OBJ_JEWELLERY && item.sub_type == RING_SUSTENANCE)
            break;

        item.ra_props[RAP_METABOLISM] = 1 + random5(3);
        break;

    case 8:   /* emits mutagenic radiation - increases magic_contamination */
        /* property is chance (1 in ...) of increasing magic_contamination */
        item.ra_props[RAP_MUTAGENIC] = 2 + random5(4);
        break;
    }

/*
   26 - +to-hit (no wpns)
   27 - +to-dam (no wpns)
 */

finished_curses:
    if (random5(10) == 0
        && (item.base_type != OBJ_ARMOUR
            || (item.sub_type != ARM_CLOAK && item.sub_type != ARM_BOOTS)
            || get_equip_race( item ) != ISFLAG_ELVEN))
    {
        power_level++;
        item.ra_props[RAP_STEALTH] = 10 + random5(70);

        if (random5(4) == 0)
        {
            item.ra_props[RAP_STEALTH] = -item.ra_props[RAP_STEALTH] - random5(20);
            power_level--;
        }
    }

    if ((power_level < 2 && random5(5) == 0) || random5(30) == 0)
        item.ra_props[RAP_CURSED] = 1;

    srand(randstore);
}

static void set_randart_properties( item_def &item )
{
    ASSERT( is_random_artefact( item ) );

    int tmp;
    int i = 0;
    int power_level = 0;
    int cursed = 0;

    if (is_unrandom_artefact( item ))
    {
        struct unrandart_entry *unrand = seekunrandart( item );

        for (i = 0; i < RAP_NUM_PROPERTIES; i++)
            item.ra_props[i] = unrand->prpty[i];

        return;
    }

    if (item.base_type == OBJ_ARMOUR)
        power_level = item.plus / 2 + 2;
    else if (item.base_type == OBJ_WEAPONS)
        power_level = item.plus / 3 + item.plus2 / 3;
    else if (item.base_type == OBJ_JEWELLERY)
    {
        const int pluses = ring_has_pluses(item);

        power_level = 1 + random2(3) + random2(2);

        if (item.sub_type == RING_FIRE || item.sub_type == RING_ICE)
        {
            power_level += 1;
        }
        else if (item.sub_type == RING_HUNGER || item.sub_type == AMU_INACCURACY)
        {
            power_level -= 2;
        }
        else if (pluses)
        {
            power_level += item.plus / 3;

            if (pluses == 2)
                power_level += item.plus2 / 3;
        }
    }

    if (power_level < 0)
        power_level = 0;

    for (i = 0; i < RAP_NUM_PROPERTIES; i++)
        item.ra_props[i] = 0;

    /* Only weapons get brands, of course */
    if (item.base_type == OBJ_WEAPONS)
    {
        // all but distortion and reaching
        item.ra_props[RAP_BRAND] = SPWPN_FLAMING + random2(15);

        if (one_chance_in(6))
        {
            item.ra_props[RAP_BRAND] = coinflip() ? SPWPN_FLAMING
                                                  : SPWPN_FREEZING;
        }

        // orc slaying, venom, protection, draining
        if (one_chance_in(6))
            item.ra_props[RAP_BRAND] = SPWPN_ORC_SLAYING + random2(4);

        if (one_chance_in(6))
            item.ra_props[RAP_BRAND] = SPWPN_VORPAL;

        if (is_range_weapon( item ))
        {
            item.ra_props[RAP_BRAND] = SPWPN_NORMAL;

            if (one_chance_in(3))
            {
                tmp = random2(20);

                item.ra_props[RAP_BRAND] = (tmp == 19) ? SPWPN_SPEED :
                                           (tmp >= 16) ? SPWPN_VORPAL :
                                           (tmp >= 12) ? SPWPN_PROTECTION :
                                           (tmp >=  8) ? SPWPN_VENOM :
                                           (tmp >=  4) ? SPWPN_FLAMING
                                                       : SPWPN_FREEZING;
            }
        }

        if (is_demonic( item ))
        {
            switch (random2(9))
            {
            case 0:
                item.ra_props[RAP_BRAND] = SPWPN_DRAINING;
                break;
            case 1:
                item.ra_props[RAP_BRAND] = SPWPN_FLAMING;
                break;
            case 2:
                item.ra_props[RAP_BRAND] = SPWPN_FREEZING;
                break;
            case 3:
                item.ra_props[RAP_BRAND] = SPWPN_ELECTROCUTION;
                break;
            case 4:
                item.ra_props[RAP_BRAND] = SPWPN_VAMPIRICISM;
                break;
            case 5:
                item.ra_props[RAP_BRAND] = SPWPN_PAIN;
                break;
            case 6:
                item.ra_props[RAP_BRAND] = SPWPN_VENOM;
                break;
            default:
                power_level -= 2;
            }
            power_level += 2;
        }
        else if (one_chance_in(3))
            item.ra_props[RAP_BRAND] = SPWPN_NORMAL;
        else
            power_level++;

        // Now that brand is settled, fix "illegal" brands (no random here!):

        // These two were missile brands:
        if (item.ra_props[RAP_BRAND] == SPWPN_UNUSED_I
            || item.ra_props[RAP_BRAND] == SPWPN_UNUSED_II)
        {
            if (item.sub_type == WPN_WHIP
                || item.sub_type == WPN_DEMON_WHIP
                || weapon_skill( item ) == SK_POLEARMS)
            {
                item.ra_props[RAP_BRAND] = SPWPN_REACHING;
            }
            else
            {
                item.ra_props[RAP_BRAND] = SPWPN_NORMAL;
            }
        }

        // Only maces (and appearently artefact hammers?) of disruption
        if (item.ra_props[RAP_BRAND] == SPWPN_DISRUPTION
            && !(item.sub_type == WPN_MACE
                || item.sub_type == WPN_GREAT_MACE
                || item.sub_type == WPN_HAMMER))
        {
            item.ra_props[RAP_BRAND] = SPWPN_DISTORTION;
        }
    }

    if (one_chance_in(5))
        goto skip_mods;

    /* AC mod - not for non-shield armours or weapons/rings of protection */
    if (one_chance_in( 4 + power_level )
        && (item.base_type != OBJ_WEAPONS
            || item.ra_props[RAP_BRAND] != SPWPN_PROTECTION)
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_PROTECTION)
        && (item.base_type != OBJ_ARMOUR
            || get_armour_slot( item ) == EQ_SHIELD))
    {
        item.ra_props[RAP_AC] += 1 + random2(3) + random2(2) + random2(2);
        power_level++;

        if (one_chance_in(4))
        {
            item.ra_props[RAP_AC] -= 2 + random2(4) + random2(3) + random2(3);
            power_level--;
            cursed -= item.ra_props[RAP_AC];
        }
    }

    /* ev mod - not for rings of evasion */
    if (one_chance_in( 4 + power_level )
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_EVASION))
    {
        item.ra_props[RAP_EVASION] = 1 + random2(3) + random2(2) + random2(2);
        power_level++;
        if (one_chance_in(4))
        {
            item.ra_props[RAP_EVASION] -= 2 + random2(4) + random2(3) + random2(3);
            power_level--;
            cursed -= item.ra_props[RAP_EVASION];
        }
    }

    /* str mod - not for rings of strength */
    if (one_chance_in( 4 + power_level )
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_STRENGTH))
    {
        item.ra_props[RAP_STRENGTH] = 1 + random2(3) + random2(2);
        power_level++;
        if (one_chance_in(4))
        {
            item.ra_props[RAP_STRENGTH] -= 2 + random2(3) + random2(3) + random2(3);
            power_level--;
            cursed -= 2 * item.ra_props[RAP_STRENGTH];
        }
    }

    /* int mod - not for rings of intelligence */
    if (one_chance_in( 4 + power_level )
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_INTELLIGENCE))
    {
        item.ra_props[RAP_INTELLIGENCE] = 1 + random2(3) + random2(2);
        power_level++;
        if (one_chance_in(4))
        {
            item.ra_props[RAP_INTELLIGENCE] -= 2 + random2(3) + random2(3) + random2(3);
            power_level--;
            cursed -= 2 * item.ra_props[RAP_INTELLIGENCE];
        }
    }

    /* dex mod - not for rings of dexterity */
    if (one_chance_in( 4 + power_level )
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_DEXTERITY))
    {
        item.ra_props[RAP_DEXTERITY] = 1 + random2(3) + random2(2);
        power_level++;
        if (one_chance_in(4))
        {
            item.ra_props[RAP_DEXTERITY] -= 2 + random2(3) + random2(3) + random2(3);
            power_level--;
            cursed -= 2 * item.ra_props[RAP_DEXTERITY];
        }
    }

  skip_mods:
    if (random2(15) < power_level
        || item.base_type == OBJ_WEAPONS
        || (item.base_type == OBJ_JEWELLERY && item.sub_type == RING_SLAYING))
    {
        goto skip_combat;
    }

    /* Weapons and rings of slaying can't get these */
    if (one_chance_in( 4 + power_level ))
    {
        item.ra_props[RAP_ACCURACY] = 1 + random2(3) + random2(2);
        power_level++;
        if (one_chance_in(4))
        {
            item.ra_props[RAP_ACCURACY] -= 1 + random2(3) + random2(3) + random2(3);
            power_level--;
            cursed -= item.ra_props[RAP_ACCURACY];
        }
    }

    if (one_chance_in( 4 + power_level ))
    {
        item.ra_props[RAP_DAMAGE] = 1 + random2(3) + random2(2);
        power_level++;
        if (one_chance_in(4))
        {
            item.ra_props[RAP_DAMAGE] -= 1 + random2(3) + random2(3) + random2(3);
            power_level--;
            cursed -= item.ra_props[RAP_DAMAGE];
        }
    }

  skip_combat:
    if (random2(12) < power_level)
        goto finished_powers;

/* res_fire */
    if (one_chance_in( 4 + power_level )
        && (item.base_type != OBJ_JEWELLERY
            || (item.sub_type != RING_PROTECTION_FROM_FIRE
                && item.sub_type != RING_FIRE
                && item.sub_type != RING_ICE))
        && (item.base_type != OBJ_ARMOUR
            || (item.sub_type != ARM_DRAGON_ARMOUR
                && item.sub_type != ARM_ICE_DRAGON_ARMOUR
                && item.sub_type != ARM_GOLD_DRAGON_ARMOUR)))
    {
        item.ra_props[RAP_FIRE] = 1;
        power_level++;

        if (one_chance_in(5))
        {
            item.ra_props[RAP_FIRE]++;
            power_level++;
        }
    }

    /* res_cold */
    if (one_chance_in( 4 + power_level )
        && (item.base_type != OBJ_JEWELLERY
            || (item.sub_type != RING_PROTECTION_FROM_COLD
                && item.sub_type != RING_FIRE
                && item.sub_type != RING_ICE))
        && (item.base_type != OBJ_ARMOUR
            || (item.sub_type != ARM_DRAGON_ARMOUR
                && item.sub_type != ARM_ICE_DRAGON_ARMOUR
                && item.sub_type != ARM_GOLD_DRAGON_ARMOUR)))
    {
        item.ra_props[RAP_COLD] = 1;
        power_level++;

        if (one_chance_in(5))
        {
            item.ra_props[RAP_COLD]++;
            power_level++;
        }
    }

    if (random2(12) < power_level || power_level > 7)
        goto finished_powers;

    /* res_elec */
    if (one_chance_in( 4 + power_level )
        && (item.base_type != OBJ_ARMOUR
            || item.sub_type != ARM_STORM_DRAGON_ARMOUR))
    {
        item.ra_props[RAP_ELECTRICITY] = 1;
        power_level++;
    }

/* res_poison */
    tmp = random2( 3 * (5 + power_level) );
    if (tmp < 3
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_POISON_RESISTANCE)
        && (item.base_type != OBJ_ARMOUR
            || (item.sub_type != ARM_GOLD_DRAGON_ARMOUR
                && item.sub_type != ARM_SWAMP_DRAGON_ARMOUR)))
    {
        item.ra_props[RAP_POISON] = 1 + (tmp == 0);
        power_level++;
    }

    /* prot_life - no necromantic brands on weapons allowed */
    tmp = random2( 3 * (5 + power_level) );
    if (tmp < 3
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_LIFE_PROTECTION)
        && item.ra_props[RAP_BRAND] != SPWPN_DRAINING
        && item.ra_props[RAP_BRAND] != SPWPN_VAMPIRICISM
        && item.ra_props[RAP_BRAND] != SPWPN_PAIN)
    {
        item.ra_props[RAP_NEGATIVE_ENERGY] = 1 + (tmp == 0);
        power_level++;
    }

    /* res magic */
    if (one_chance_in( 4 + power_level )
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_PROTECTION_FROM_MAGIC))
    {
        item.ra_props[RAP_MAGIC] = 20 + random2(41);
        power_level++;
    }

    /* see_invis */
    if (one_chance_in( 4 + power_level )
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_SEE_INVISIBLE))
    {
        item.ra_props[RAP_EYESIGHT] = 1;
        power_level++;
    }

    if (random2(12) < power_level || power_level > 10)
        goto finished_powers;

    /* turn invis */
    if (one_chance_in(10)
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_INVISIBILITY))
    {
        item.ra_props[RAP_INVISIBLE] = 1;
        power_level++;
    }

    /* levitate */
    if (one_chance_in(10)
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_LEVITATION))
    {
        item.ra_props[RAP_LEVITATE] = 1;
        power_level++;
    }

    /* blink */
    if (one_chance_in(10))
    {
        item.ra_props[RAP_BLINK] = 1;
        power_level++;
    }

    /* teleport */
    if (one_chance_in(10)
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != RING_TELEPORTATION))
    {
        item.ra_props[RAP_CAN_TELEPORT] = 1;
        power_level++;
    }

    /* go berserk */
    if (one_chance_in(10)
        && (item.base_type != OBJ_JEWELLERY
            || item.sub_type != AMU_RAGE))
    {
        item.ra_props[RAP_BERSERK] = 1;
        power_level++;
    }

    /* sense surr */
    if (one_chance_in(10))
    {
        item.ra_props[RAP_MAPPING] = 1;
        power_level++;
    }


  finished_powers:
    /* Armours get less powers, and are also less likely to be
       cursed that wpns */
    if (item.base_type == OBJ_ARMOUR)
        power_level -= 4;

    if (random2(17) >= power_level || power_level < 2)
        goto finished_curses;

    tmp = random2(18);

    switch (tmp)
    {
    case 2:                     /* no magic */
    case 3:
        item.ra_props[RAP_PREVENT_SPELLCASTING] = 1;
        cursed += 30;
        break;

    case 4:                     /* random teleport */
    case 5:
        item.ra_props[RAP_CAUSE_TELEPORTATION] = 5 + random2(15);
        cursed += 30;
        break;

    case 0:                     /* makes noise */
    case 1:
        // only for weapons currently because it's done via "special wield"
        if (item.base_type == OBJ_WEAPONS)
        {
            item.ra_props[RAP_NOISES] = SPWLD_HUM + random2(4);
            cursed += 3;
            break;
        }
        // fall-through for non-weapons

    case 6:   /* no teleport - doesn't affect some instantaneous teleports */
    case 7:
        if (item.base_type == OBJ_JEWELLERY
            && (item.sub_type == RING_TELEPORTATION
                || item.sub_type == RING_TELEPORT_CONTROL))
        {
            break;
        }

        item.ra_props[RAP_BLINK] = 0;
        item.ra_props[RAP_CAN_TELEPORT] = 0;
        item.ra_props[RAP_PREVENT_TELEPORTATION] = 1;
        cursed += 30;
        break;

    case 10:                     /* susceptible to fire */
    case 11:
        if ((item.base_type == OBJ_JEWELLERY
                && (item.sub_type == RING_PROTECTION_FROM_FIRE
                    || item.sub_type == RING_FIRE
                    || item.sub_type == RING_ICE))
            || (item.base_type == OBJ_ARMOUR
                && (item.sub_type == ARM_DRAGON_ARMOUR
                    || item.sub_type == ARM_ICE_DRAGON_ARMOUR
                    || item.sub_type == ARM_GOLD_DRAGON_ARMOUR)))
        {
            break;              /* already does this or something */
        }

        ASSERT( item.ra_props[RAP_FIRE] >= 0 && item.ra_props[RAP_FIRE] <= 2 );
        item.ra_props[RAP_FIRE] = -1 - item.ra_props[RAP_FIRE];
        cursed += 30;
        break;

    case 12:                     /* susceptible to cold */
    case 13:
        if ((item.base_type == OBJ_JEWELLERY
                && (item.sub_type == RING_PROTECTION_FROM_COLD
                    || item.sub_type == RING_FIRE
                    || item.sub_type == RING_ICE))
            || (item.base_type == OBJ_ARMOUR
                && (item.sub_type == ARM_DRAGON_ARMOUR
                    || item.sub_type == ARM_ICE_DRAGON_ARMOUR
                    || item.sub_type == ARM_GOLD_DRAGON_ARMOUR)))
        {
            break;
        }

        ASSERT( item.ra_props[RAP_COLD] >= 0 && item.ra_props[RAP_COLD] <= 2 );
        item.ra_props[RAP_COLD] = -1 - item.ra_props[RAP_COLD];
        cursed += 30;
        break;

    case 8:                     /* berserk on attack */
    case 9:
        if (item.base_type == OBJ_WEAPONS)
        {
            item.ra_props[RAP_ANGRY] = 1 + random2(8);
            cursed += 5;
            break;
        }
        // fall-through for non-weapons

    case 14:                     /* speed metabolism */
    case 15:
        if (item.base_type == OBJ_JEWELLERY
            && (item.sub_type == RING_HUNGER || item.sub_type == RING_SUSTENANCE))
        {
            break;
        }

        item.ra_props[RAP_METABOLISM] = 1 + random2(3);
        cursed += item.ra_props[RAP_METABOLISM] * 10;
        break;

    default:
        // will hit mutagenic below like other breaks above
        break;
    }

    if (!cursed)
    {
        if (tmp % 2 == 0
            && (item.base_type != OBJ_JEWELLERY
                || item.sub_type != RING_POISON_RESISTANCE)
            && (item.base_type != OBJ_ARMOUR
                || (item.sub_type != ARM_SWAMP_DRAGON_ARMOUR
                    && item.sub_type != ARM_GOLD_DRAGON_ARMOUR)))
        {
            ASSERT( item.ra_props[RAP_POISON] >= 0 && item.ra_props[RAP_POISON] <= 2 );
            item.ra_props[RAP_POISON] = -1 - item.ra_props[RAP_POISON];
            cursed += 15;
        }
        else
        {
            /* emits mutagenic radiation - increases magic_contamination */
            /* chance is (1 in ...) of increasing magic_contamination */
            item.ra_props[RAP_MUTAGENIC] = 2 + random2(4);
            cursed += 30;
        }
    }

/*
   26 - +to-hit (no wpns)
   27 - +to-dam (no wpns)
 */

finished_curses:
    if (one_chance_in(10)
        && (item.base_type != OBJ_ARMOUR
            || (item.sub_type != ARM_CLOAK && item.sub_type != ARM_BOOTS)
            || get_equip_race( item ) != ISFLAG_ELVEN))
    {
        power_level++;
        item.ra_props[RAP_STEALTH] = 10 + random2(71);

        if (one_chance_in(4))
        {
            item.ra_props[RAP_STEALTH] = -item.ra_props[RAP_STEALTH] - random2(21);
            power_level--;
            cursed += 3;
        }
    }

    // marks if randart should start cursed
    if (cursed > 0)
        item.ra_props[RAP_CURSED] = cursed;
    else if (one_chance_in(30))
        item.ra_props[RAP_CURSED] = 1;
}

static const randart_desc_adj & get_desc_adj( const item_def &item )
{
    ASSERT( item.base_type == OBJ_JEWELLERY
            || item.base_type == OBJ_WEAPONS
            || item.base_type == OBJ_ARMOUR );

    ASSERT( is_random_artefact( item ) );

    const int index = ((item.special & 0xff000000) >> 24) % NUM_DESC_ADJ;

    if (item.base_type == OBJ_WEAPONS)
        return (Wpn_Desc_Adj[ index ]);
    else if (item.base_type == OBJ_ARMOUR)
        return (Arm_Desc_Adj[ index ]);

    ASSERT( item.base_type == OBJ_JEWELLERY );
    return (Ring_Desc_Adj[ index ]);
}

// returns how the item looks: plain, fancy, or special
// NOTE: needs to be kept in sync with the artefact name functions!
appearance_type item_appearance( const item_def &item )
{
    appearance_type ret = APP_PLAIN;

    if (item.base_type != OBJ_JEWELLERY
        && item.base_type != OBJ_WEAPONS
        && item.base_type != OBJ_ARMOUR)
    {
        if (item.base_type == OBJ_MISSILES
            && get_equip_desc( item ) != ISFLAG_NO_DESC)
        {
            return (APP_FANCY);
        }

        return (ret);
    }

    if (is_unrandom_artefact( item ) || is_fixed_artefact( item ))
    {
        // XXX: these artefacts need better handling
        ret = APP_SPECIAL;
    }
    else if (is_random_artefact( item ))
    {
        if (item_ident( item, ISFLAG_KNOW_TYPE ))
            ret = APP_SPECIAL;
        else
        {
            const randart_desc_adj &adj = get_desc_adj( item );

            ret = adj.appear;

            // handle special cases for amulets.
            if (item.base_type == OBJ_JEWELLERY
                && (adj.check && jewellery_is_amulet( item )))
            {
                ret = APP_SPECIAL;
            }
        }
    }
    else if (item.base_type != OBJ_JEWELLERY
            && get_equip_desc( item ) != ISFLAG_NO_DESC)
    {
        ret = APP_FANCY;
    }

    return (ret);
}

// Get suggestion for colour of randart from adjective (but can get overridden)
// Returns BLACK for no particular suggestion.
int randart_suggested_colour( const item_def &item )
{
    ASSERT( is_random_artefact( item ) );

    // Leave jewellery to it's natural material colour more often:
    if (item.base_type == OBJ_JEWELLERY && !one_chance_in(3))
        return (BLACK);

    int ret = BLACK;

    const randart_desc_adj &adj = get_desc_adj( item );

    switch (adj.colour)
    {
    case RDC_BRIGHTLY_GLOWING:
        ret = element_colour( EC_MAGIC );
        break;

    case RDC_STEAMING:
        ret = LIGHTGREY;
        break;

    case RDC_SMOKING:
        ret = element_colour( EC_SMOKE );
        break;

    case RDC_BLOODSTAINED:
    case RDC_ICHOR_STAINED:
        if (item.base_type != OBJ_JEWELLERY)
            ret = element_colour( EC_BLOOD );
        break;

    case RDC_SHIMMERING:
    case RDC_JEWELLED_IRIDESCENT:
        ret = element_colour( EC_JEWEL );
        break;

    case RDC_CRYSTAL:
        if (item.base_type == OBJ_WEAPONS)
            ret = element_colour( EC_CRYSTAL );
        else if (item.base_type == OBJ_JEWELLERY)
            ret = element_colour( EC_JEWEL );
        break;

    case RDC_TRANSPARENT:
        ret = (coinflip() ? LIGHTGREY : DARKGREY);
        break;

    case RDC_ENCRUSTED:
        if (coinflip())
            ret = element_colour( EC_EARTH );       // dirt, rust
        else if (coinflip())
            ret = element_colour( EC_BLOOD );       // blood
        else
            ret = element_colour( EC_SLIME );       // slime
        break;

    case RDC_SLIMY:
        ret = element_colour( EC_SLIME );
        break;

    default:
        break;
    }

    return (ret);
}

// Used to get the material factor out of the seed and pass it
// over to item_colour so the ring colour can match it's material
// properly.
int randart_ring_material( const item_def &item )
{
    ASSERT( item.base_type == OBJ_JEWELLERY && is_random_artefact( item ) );

    // Using a different set of bits than the desc to maintain independance.
    return ((item.special & 0x00f00000) >> 20);
}

// Note: weapons only - armour and jewellery are below
const char *randart_name( const item_def &item )
{
    ASSERT( item.base_type == OBJ_WEAPONS );

    if (is_unrandom_artefact( item ))
    {
        struct unrandart_entry *unrand = seekunrandart( item );

        return (item_ident(item, ISFLAG_KNOW_TYPE) ? unrand->name
                                                   : unrand->unid_name);
    }

    char buff[ ITEMNAME_SIZE ];
    art_n[0] = '\0';            // clear name buffer

    if (!item_ident( item, ISFLAG_KNOW_TYPE ))
    {
        const randart_desc_adj &adj = get_desc_adj( item );
        strcat( art_n, adj.str );

        standard_name_weap( item.sub_type, buff );
        strcat(art_n, buff);

        return (art_n);
    }

    standard_name_weap( item.sub_type, buff );
    strcat( art_n, buff );

    if (item.special & 0x80000000)
        strcat( art_n, Rand_Wpn_Names[ item.special % Num_Wpn_Names ] );
    else
    {
        make_name( item.special, false, buff );

        if (item.special % 3)
        {
            strcat(art_n, " \"");
            strcat(art_n, buff);
            strcat(art_n, "\"");
        }
        else
        {
            strcat(art_n, " of ");
            strcat(art_n, buff);
        }
    }

    return (art_n);
}

const char *randart_armour_name( const item_def &item )
{
    ASSERT( item.base_type == OBJ_ARMOUR );

    if (is_unrandom_artefact( item ))
    {
        struct unrandart_entry *unrand = seekunrandart( item );

        return (item_ident(item, ISFLAG_KNOW_TYPE) ? unrand->name
                                                   : unrand->unid_name);
    }

    char buff[ ITEMNAME_SIZE ];
    art_n[0] = '\0';            // clear name buffer

    if (!item_ident( item, ISFLAG_KNOW_TYPE ))
    {
        const randart_desc_adj &adj = get_desc_adj( item );

        if (adj.check && armour_not_shiny( item ))
        {
            if (adj.appear == APP_SPECIAL)
                strcat( art_n, "sleek " );
            else // APP_FANCY
                strcat( art_n, "embroidered " );
        }
        else
        {
            strcat( art_n, adj.str );
        }

        standard_name_armour( item, buff );
        strcat( art_n, buff );

        return (art_n);
    }

    standard_name_armour(item, buff);
    strcat(art_n, buff);

    if (item.special & 0x80000000)
        strcat( art_n, Rand_Arm_Names[ item.special % Num_Arm_Names ] );
    else
    {
        make_name( item.special, false, buff );

        if (item.special % 3)
        {
            strcat(art_n, " \"");
            strcat(art_n, buff);
            strcat(art_n, "\"");
        }
        else
        {
            strcat(art_n, " of ");
            strcat(art_n, buff);
        }
    }

    return (art_n);
}

const char *randart_ring_name( const item_def &item )
{
    ASSERT( item.base_type == OBJ_JEWELLERY );

    char buff[ITEMNAME_SIZE];

    art_n[0] = '\0';            // clear name buffer

    const int pluses = ring_has_pluses(item);

    // display pluses in name, at least it helps with telling artefacts apart.
    if (pluses && item_ident( item, ISFLAG_KNOW_PLUSES ))
    {
        char tmp_quant[20];

        if (item.plus >= 0)
            strncat( art_n, "+", ITEMNAME_SIZE );

        itoa( item.plus, tmp_quant, 10 );
        strncat( art_n, tmp_quant, ITEMNAME_SIZE );

        if (pluses == 2)
        {
            strncat( art_n, ",", ITEMNAME_SIZE );

            if (item.plus2 >= 0)
                strncat( art_n, "+", ITEMNAME_SIZE );

            itoa( item.plus2, tmp_quant, 10 );
            strncat( art_n, tmp_quant, ITEMNAME_SIZE );
        }

        strncat( art_n, " ", ITEMNAME_SIZE );
    }

    if (is_unrandom_artefact( item ))
    {
        struct unrandart_entry *unrand = seekunrandart( item );

        strncat( art_n, item_ident(item, ISFLAG_KNOW_TYPE) ? unrand->name
                                                           : unrand->unid_name,
                        ITEMNAME_SIZE );
        return (art_n);
    }

    if (!item_ident( item, ISFLAG_KNOW_TYPE ))
    {
        const randart_desc_adj &adj = get_desc_adj( item );
        strcat( art_n, adj.str );

        const int mat = randart_ring_material( item );

        if (jewellery_is_amulet( item ))
        {
            switch (mat)
            {
            case  0: strcat( art_n, "zirconium" ); break;
            case  1: strcat( art_n, "sapphire" ); break;
            case  2: strcat( art_n, "gold" ); break;
            case  3: strcat( art_n, "emerald" ); break;
            case  4: strcat( art_n, "garnet" ); break;
            case  5: strcat( art_n, "bronze" ); break;
            case  6: strcat( art_n, "brass" ); break;
            case  7: strcat( art_n, "copper" ); break;
            case  8: strcat( art_n, "ruby" ); break;
            case  9: strcat( art_n, "ivory" ); break;
            case 10: strcat( art_n, "bone" ); break;
            case 11: strcat( art_n, "platinum" ); break;
            case 12: strcat( art_n, "jade" ); break;
            case 13: strcat( art_n, "plastic" ); break;
            case 14: strcat( art_n, "orichalcum" ); break;
            case 15: strcat( art_n, "electrum" ); break;
            }

            strcat( art_n, " amulet" );
        }
        else // is a ring
        {
            switch (mat)
            {
            case  0: strcat( art_n, "wooden" ); break;
            case  1: strcat( art_n, "silver" ); break;
            case  2: strcat( art_n, "gold" ); break;
            case  3: strcat( art_n, "iron" ); break;
            case  4: strcat( art_n, "steel" ); break;
            case  5: strcat( art_n, "bronze" ); break;
            case  6: strcat( art_n, "brass" ); break;
            case  7: strcat( art_n, "copper" ); break;
            case  8: strcat( art_n, "granite" ); break;
            case  9: strcat( art_n, "ivory" ); break;
            case 10: strcat( art_n, "bone" ); break;
            case 11: strcat( art_n, "marble" ); break;
            case 12: strcat( art_n, "jade" ); break;
            case 13: strcat( art_n, "glass" ); break;
            case 14: strcat( art_n, "lapis lazuli" ); break;
            case 15: strcat( art_n, "obsidian" ); break;
            }

            strcat( art_n, " ring" );
        }

        return (art_n);
    }

    strcat( art_n, jewellery_is_amulet( item ) ?  "amulet" : "ring" );

    if ((item.special & 0xc0000000) == 0)
        strcat( art_n, Rand_Arm_Names[ item.special % Num_Arm_Names ] );
    else
    {
        make_name( item.special, false, buff );

        if (item.special % 3)
        {
            strcat(art_n, " \"");
            strcat(art_n, buff);
            strcat(art_n, "\"");
        }
        else
        {
            strcat(art_n, " of ");
            strcat(art_n, buff);
        }
    }

    return (art_n);
}                               // end randart_ring_name()

bool make_item_randart( item_def &item )
{
    if ((item.base_type != OBJ_WEAPONS
            && item.base_type != OBJ_ARMOUR
            && item.base_type != OBJ_JEWELLERY)
        || is_unrandom_artefact( item )
        || is_fixed_artefact( item ))
    {
        return (false);
    }

    item.flags |= ISFLAG_RANDART;
    item.special = random_int();

    int i, j;

    // XXX: We shouldn't need this anymore:
    //
    // Try a few times to get an artefact that does something, but leave
    // a chance that we'll get the occasional one that doesn't for fun
    // (artefact weapons and armour at least have the power that they
    // never corrode).  Note that if we run out of trys we may also
    // be taking an exceptionally good item, which is the counterbalance.
    for (i = 0; i < 10; i++)
    {
        set_randart_properties( item );

        for (j = 0; j < RAP_NUM_PROPERTIES; j++)
        {
            if (item.ra_props[j])
                break;
        }

        // If it does something, but isn't too good we'll take it now...
        // not that although this appears to strongly favour creating
        // bad artefacts, but truly bad artefacts aren't that common
        // (value 0 artefacts can be quite usuable actually)... which
        // balances things out.
        if (j < RAP_NUM_PROPERTIES && random2(1000) >= randart_value(item) + 50)
            break;
    }

    item_colour( item );

    return (true);
}

bool unmake_item_randart( item_def &item )
{
    if (!is_random_artefact( item ) || is_unrandom_artefact( item ))
        return (false);

    if (item.base_type != OBJ_WEAPONS
        && item.base_type != OBJ_ARMOUR
        && item.base_type != OBJ_JEWELLERY)
    {
        return (false);
    }

    item.flags &= ~ISFLAG_RANDART;
    item.special = SPWPN_NORMAL;

    return (true);
}

static struct unrandart_entry *seekunrandart( const item_def &item )
{
    int x = 0;

    while (x < NO_UNRANDARTS)
    {
        if (unranddata[x].ura_cl == item.base_type
            && unranddata[x].ura_ty == item.sub_type
            && unranddata[x].ura_pl == item.plus
            && unranddata[x].ura_pl2 == item.plus2)
        {
            return (&unranddata[x]);
        }

        x++;
    }

    return (&unranddata[0]);  // Dummy object
}                               // end seekunrandart()

int find_unrandart_index(int item_number)
{
    int x;

    for(x=0; x < NO_UNRANDARTS; x++)
    {
        if (unranddata[x].ura_cl == mitm[item_number].base_type
            && unranddata[x].ura_ty == mitm[item_number].sub_type
            && unranddata[x].ura_pl == mitm[item_number].plus
            && unranddata[x].ura_pl2 == mitm[item_number].plus2)
        {
            return (x);
        }
    }

    return (-1);
}

int find_okay_unrandart(unsigned char aclass, unsigned char atype)
{
    int x, count;
    int ret = -1;

    for (x = 0, count = 0; x < NO_UNRANDARTS; x++)
    {
        if (unranddata[x].ura_cl == aclass
            && (atype == OBJ_RANDOM || atype == unranddata[x].ura_ty)
            && !does_unrandart_exist(x))
        {
            count++;

            if (random2(count) == 0)
                ret = x;
        }
    }

    return (ret);
}                               // end find_okay_unrandart()

bool make_item_unrandart( item_def &item, int unrand_index )
{
    item.base_type = unranddata[unrand_index].ura_cl;
    item.sub_type  = unranddata[unrand_index].ura_ty;
    item.plus      = unranddata[unrand_index].ura_pl;
    item.plus2     = unranddata[unrand_index].ura_pl2;
    item.colour    = unranddata[unrand_index].ura_col;

    item.flags |= ISFLAG_UNRANDART;

    for (int i = 0; i < RAP_NUM_PROPERTIES; i++)
        item.ra_props[i] = unranddata[unrand_index].prpty[i];

    item.special = unranddata[unrand_index].prpty[RAP_BRAND];

    if (unranddata[unrand_index].prpty[RAP_CURSED])
        do_curse_item( item );

    set_unrandart_exist( unrand_index, 1 );

    return (true);
}                               // end make_item_unrandart()

const char *unrandart_descrip( char which_descrip, const item_def &item )
{

    // Eventually it would be great to have randomly generated
    // descriptions for randarts.
    struct unrandart_entry *unrand = seekunrandart( item );

    return ((which_descrip == 0) ? unrand->spec_descrip1 :
            (which_descrip == 1) ? unrand->spec_descrip2 :
            (which_descrip == 2) ? unrand->spec_descrip3 : "Unknown.");

}                               // end unrandart_descrip()

void standard_name_weap( unsigned char item_typ, char glorg[ITEMNAME_SIZE] )
{
    strcpy(glorg,  (item_typ == WPN_CLUB) ? "club" :
                   (item_typ == WPN_MACE) ? "mace" :
                   (item_typ == WPN_FLAIL) ? "flail" :
                   (item_typ == WPN_KNIFE) ? "knife" :
                   (item_typ == WPN_DAGGER) ? "dagger" :
                   (item_typ == WPN_MORNINGSTAR) ? "morningstar" :
                   (item_typ == WPN_SHORT_SWORD) ? "short sword" :
                   (item_typ == WPN_LONG_SWORD) ? "long sword" :
                   (item_typ == WPN_GREAT_SWORD) ? "great sword" :
                   (item_typ == WPN_SCIMITAR) ? "scimitar" :
                   (item_typ == WPN_HAND_AXE) ? "hand axe" :
                   (item_typ == WPN_BATTLEAXE) ? "battleaxe" :
                   (item_typ == WPN_SPEAR) ? "spear" :
                   (item_typ == WPN_TRIDENT) ? "trident" :
                   (item_typ == WPN_HALBERD) ? "halberd" :
                   (item_typ == WPN_SLING) ? "sling" :
                   (item_typ == WPN_BOW) ? "bow" :
                   (item_typ == WPN_LONGBOW) ? "longbow" :
                   (item_typ == WPN_BLOWGUN) ? "blowgun" :
                   (item_typ == WPN_CROSSBOW) ? "crossbow" :
                   (item_typ == WPN_HAND_CROSSBOW) ? "hand crossbow" :
                   (item_typ == WPN_GLAIVE) ? "glaive" :
                   (item_typ == WPN_QUARTERSTAFF) ? "quarterstaff" :
                   (item_typ == WPN_STAFF) ? "staff" :
                   (item_typ == WPN_SCYTHE) ? "scythe" :
                   (item_typ == WPN_EVENINGSTAR) ? "eveningstar" :
                   (item_typ == WPN_QUICK_BLADE) ? "quick blade" :
                   (item_typ == WPN_KATANA) ? "katana" :
                   (item_typ == WPN_LAJATANG) ? "lajatang" :
                   (item_typ == WPN_EXECUTIONERS_AXE) ? "executioner's axe" :
                   (item_typ == WPN_DOUBLE_SWORD) ? "double sword" :
                   (item_typ == WPN_TRIPLE_SWORD) ? "triple sword" :
                   (item_typ == WPN_HAMMER) ? "hammer" :
                   (item_typ == WPN_ANCUS) ? "ancus" :
                   (item_typ == WPN_WHIP) ? "whip" :
                   (item_typ == WPN_SABRE) ? "sabre" :
                   (item_typ == WPN_DEMON_BLADE) ? "demon blade" :
                   (item_typ == WPN_DEMON_WHIP) ? "demon whip" :
                   (item_typ == WPN_DEMON_TRIDENT) ? "demon trident" :
                   (item_typ == WPN_BROAD_AXE) ? "broad axe" :
                   (item_typ == WPN_WAR_AXE) ? "war axe" :
                   (item_typ == WPN_SPIKED_FLAIL) ? "spiked flail" :
                   (item_typ == WPN_GREAT_MACE) ? "great mace" :
                   (item_typ == WPN_DIRE_FLAIL) ? "dire flail" :
                   (item_typ == WPN_FALCHION) ? "falchion" :
                   (item_typ == WPN_BLESSED_BLADE) ? "blessed blade" :
                   (item_typ == WPN_LOCHABER_AXE) ? "lochaber axe" :

           (item_typ == WPN_GIANT_CLUB)
                           ? (SysEnv.board_with_nail ? "two-by-four"
                                                     : "giant club") :

           (item_typ == WPN_GIANT_SPIKED_CLUB)
                           ? (SysEnv.board_with_nail ? "board with nail"
                                                     : "giant spiked club")

                                   : "unknown weapon");
}                               // end standard_name_weap()

void standard_name_armour( const item_def &item, char glorg[ITEMNAME_SIZE] )
{
    short helm_type, dhelm;

    glorg[0] = '\0';

    switch (item.sub_type)
    {
    case ARM_ROBE:
        strcat(glorg, "robe");
        break;

    case ARM_LEATHER_ARMOUR:
        strcat(glorg, "leather armour");
        break;

    case ARM_STUDDED_LEATHER_ARMOUR:
        strcat(glorg, "studded leather armour");
        break;

    case ARM_RING_MAIL:
        strcat(glorg, "ring mail");
        break;

    case ARM_SCALE_MAIL:
        strcat(glorg, "scale mail");
        break;

    case ARM_CHAIN_MAIL:
        strcat(glorg, "chain mail");
        break;

    case ARM_SPLINT_MAIL:
        strcat(glorg, "splint mail");
        break;

    case ARM_BANDED_MAIL:
        strcat(glorg, "banded mail");
        break;

    case ARM_PLATE_MAIL:
        strcat(glorg, "plate mail");
        break;

    case ARM_SHIELD:
        strcat(glorg, "shield");
        break;

    case ARM_CLOAK:
        strcat(glorg, "cloak");
        break;

    case ARM_CAP:
    case ARM_HELMET:
        dhelm = get_helmet_desc( item );

        if (dhelm != THELM_DESC_PLAIN)
        {
            strcat( glorg,
                    (dhelm == THELM_DESC_WINGED)   ? "winged " :
                    (dhelm == THELM_DESC_HORNED)   ? "horned " :
                    (dhelm == THELM_DESC_CRESTED)  ? "crested " :
                    (dhelm == THELM_DESC_PLUMED)   ? "plumed " :
                    (dhelm == THELM_DESC_SPIKED)   ? "spiked " :
                    (dhelm == THELM_DESC_VISORED)  ? "visored " :
                    (dhelm == THELM_DESC_JEWELLED) ? "jeweled "
                                                   : "buggy " );
        }

        helm_type = get_helmet_type( item );
        if (helm_type == THELM_HELM)
            strcat(glorg, "helm");
        else if (helm_type == THELM_CAP)
            strcat(glorg, "cap");
        else if (helm_type == THELM_WIZARD_HAT)
            strcat(glorg, "wizard's hat");
        else
            strcat(glorg, "helmet");
        break;

    case ARM_GLOVES:
        strcat(glorg, "gloves");
        break;

    case ARM_NAGA_BARDING:
        strcat(glorg, "naga barding");
        break;

    case ARM_CENTAUR_BARDING:
        strcat(glorg, "centaur barding");
        break;

    case ARM_BOOTS:
        strcat(glorg, "boots");
        break;

    case ARM_BUCKLER:
        strcat(glorg, "buckler");
        break;

    case ARM_LARGE_SHIELD:
        strcat(glorg, "large shield");
        break;

    case ARM_DRAGON_HIDE:
        strcat(glorg, "dragon hide");
        break;

    case ARM_TROLL_HIDE:
        strcat(glorg, "troll hide");
        break;

    case ARM_CRYSTAL_PLATE_MAIL:
        strcat(glorg, "crystal plate mail");
        break;

    case ARM_DRAGON_ARMOUR:
        strcat(glorg, "dragon armour");
        break;

    case ARM_TROLL_LEATHER_ARMOUR:
        strcat(glorg, "troll leather armour");
        break;

    case ARM_ICE_DRAGON_HIDE:
        strcat(glorg, "ice dragon hide");
        break;

    case ARM_ICE_DRAGON_ARMOUR:
        strcat(glorg, "ice dragon armour");
        break;

    case ARM_STEAM_DRAGON_HIDE:
        strcat(glorg, "steam dragon hide");
        break;

    case ARM_STEAM_DRAGON_ARMOUR:
        strcat(glorg, "steam dragon armour");
        break;

    case ARM_MOTTLED_DRAGON_HIDE:
        strcat(glorg, "mottled dragon hide");
        break;

    case ARM_MOTTLED_DRAGON_ARMOUR:
        strcat(glorg, "mottled dragon armour");
        break;

    case ARM_STORM_DRAGON_HIDE:
        strcat(glorg, "storm dragon hide");
        break;

    case ARM_STORM_DRAGON_ARMOUR:
        strcat(glorg, "storm dragon armour");
        break;

    case ARM_GOLD_DRAGON_HIDE:
        strcat(glorg, "gold dragon hide");
        break;

    case ARM_GOLD_DRAGON_ARMOUR:
        strcat(glorg, "gold dragon armour");
        break;

    case ARM_ANIMAL_SKIN:
        strcat(glorg, "animal skin");
        break;

    case ARM_SWAMP_DRAGON_HIDE:
        strcat(glorg, "swamp dragon hide");
        break;

    case ARM_SWAMP_DRAGON_ARMOUR:
        strcat(glorg, "swamp dragon armour");
        break;
    }
}                               // end standard_name_armour()

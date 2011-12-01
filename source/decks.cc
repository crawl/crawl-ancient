/*
 *  File:       decks.cc
 *  Summary:    Functions with decks of cards.
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "decks.h"

#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "externs.h"

#include "decks.h"
#include "effects.h"
#include "food.h"
#include "it_use2.h"
#include "items.h"
#include "itemprop.h"
#include "misc.h"
#include "monplace.h"
#include "mutation.h"
#include "ouch.h"
#include "player.h"
#include "religion.h"
#include "spells1.h"
#include "spells3.h"
#include "spells4.h"
#include "spl-cast.h"
#include "stuff.h"


enum CARDS                      // (unsigned char) deck_of_foo[]
{
    CARD_BLANK = 0,             //    0  less with more evocations
    CARD_BUTTERFLY,             //       friendly butterflies
    CARD_WRAITH,                //       lose level, drain exp
    CARD_EXPERIENCE,            //       potion of experience
    CARD_WEALTH,                //       gain $$$
    CARD_INTELLIGENCE,          //    5  gain INT mutation
    CARD_STRENGTH,              //       gain STR mutation
    CARD_QUICKSILVER,           //       gain DEX mutation
    CARD_STUPIDITY,             //       lose INT mutation
    CARD_WEAKNESS,              //       lose STR mutation
    CARD_SLOTH,                 //   10  lose DEX mutation
    CARD_SHUFFLE,               //       shuffle stats
    CARD_FREAK,                 //       gain nonstat mutations
    CARD_DEATH,                 //       unfriendly reapers (no abj)
    CARD_NORMALITY,             //       remove nonstat mutations
    CARD_SHADOW,                //   15  unfriendly shadow (no abj)
    CARD_GATE,                  //       banish to abyss
    CARD_STATUE,                //       friendly statue (no abj)
    CARD_ACQUISITION,           //       aquirement
    CARD_HASTEN,                //       potion of speed
    CARD_DEMON_LESSER,          //   20  friendly lesser demons
    CARD_DEMON_COMMON,          //       friendly common demons
    CARD_DEMON_GREATER,         //       charmed greater demons
    CARD_DEMON_SWARM,           //       hostile lesser demons
    CARD_YAK,                   //       friendly yaks
    CARD_FIEND,                 //   25  charmed fiend
    CARD_DRAGON,                //       friendly dragon
    CARD_GOLEM,                 //       friendly golem
    CARD_THING_FUGLY,           //       friendly ugly thing
    CARD_LICH,                  //       hostile lich (no abj)
    CARD_HORROR_UNSEEN,         //   30  unfriendly unseen horror
    CARD_BLINK,                 //       blink
    CARD_TELEPORT,              //       teleport
    CARD_TELEPORT_NOW,          //       instant teleport
    CARD_RAGE,                  //       berserk
    CARD_LEVITY,                //   35  levitation
    CARD_VENOM,                 //       poison
    CARD_XOM,                   //       Xom effect
    CARD_SLOW,                  //       slow
    CARD_DECAY,                 //       decay
    CARD_HEALING,               //   40  healing
    CARD_HEAL_WOUNDS,           //       heal wounds
    CARD_TORMENT,               //       torment
    CARD_FOUNTAIN,              //       create fountain
    CARD_ALTAR,                 //       create altar to you.religion
    CARD_FAMINE,                //   45  set hunger to 500
    CARD_FEAST,                 //       set hunger to 12000
    CARD_WILD_MAGIC,            //       random miscast effect
    CARD_VIOLENCE,              //       aquire weapon
    CARD_PROTECTION,            //       aquire armour
    CARD_KNOWLEDGE,             //   50  aquire book
    CARD_MAZE,                  //       banish to labyrinth
    CARD_PANDEMONIUM,           //       banish to pandemonium
    CARD_IMPRISONMENT,          //       entomb character
    CARD_RULES_FOR_BRIDGE,      //   54  sometimes replaces blank
    NUM_CARDS,                  // must remain last regular member {dlb}
    CARD_RANDOM = 255           // must remain final member {dlb}
};

// Deck theme: hide in corner and alter character.
static int deck_of_wonders[] =
{
    CARD_BLANK,
    CARD_BUTTERFLY,
    CARD_EXPERIENCE,            // XP
    CARD_WRAITH,
    CARD_WEALTH,                // $$$
    CARD_INTELLIGENCE,          // stats
    CARD_STRENGTH,
    CARD_QUICKSILVER,
    CARD_STUPIDITY,
    CARD_WEAKNESS,
    CARD_SLOTH,
    CARD_SHUFFLE,
    CARD_FREAK,                 // mutations
    CARD_NORMALITY,
    CARD_DEATH,
    CARD_SHADOW,
    CARD_GATE,
    CARD_VIOLENCE,              // stuff of specific types
    CARD_PROTECTION,
    CARD_KNOWLEDGE,
    CARD_LICH,
    CARD_XOM,
    CARD_DECAY,
    CARD_ALTAR,                 // altar to our god
    CARD_FOUNTAIN,
    CARD_MAZE,                  // labyrinth
    CARD_PANDEMONIUM
};

static const int DECK_WONDERS_SIZE = sizeof(deck_of_wonders) / sizeof(int);

// Deck theme: summon friends in combat
static int deck_of_summoning[] =
{
    CARD_BLANK,
    CARD_STATUE,
    CARD_BUTTERFLY,
    CARD_DEMON_LESSER,
    CARD_DEMON_COMMON,
    CARD_DEMON_SWARM,
    CARD_FIEND,
    CARD_YAK,
    CARD_DRAGON,
    CARD_GOLEM,
    CARD_THING_FUGLY,
    CARD_HORROR_UNSEEN
};

static const int DECK_SUMMONING_SIZE = sizeof(deck_of_summoning) / sizeof(int);

// Deck theme: helpful effects in combat
static int deck_of_tricks[] =
{
    CARD_BLANK,
    CARD_BUTTERFLY,
    CARD_BLINK,
    CARD_TELEPORT,
    CARD_TELEPORT_NOW,
    CARD_RAGE,
    CARD_LEVITY,
    CARD_HEALING,
    CARD_HEAL_WOUNDS,
    CARD_WILD_MAGIC,
    CARD_DEMON_LESSER,
    CARD_HASTEN
};

static const int DECK_TRICKS_SIZE = sizeof(deck_of_tricks) / sizeof(int);

// Deck theme: mixed bag of combat effects and some aquirement (items and food)
static int deck_of_power[] =
{
    CARD_BLANK,
    CARD_DEMON_COMMON,          // combat stuff
    CARD_DEMON_GREATER,
    CARD_BLINK,
    CARD_TELEPORT_NOW,
    CARD_VENOM,
    CARD_XOM,
    CARD_HEAL_WOUNDS,
    CARD_STATUE,                // golem friends
    CARD_FEAST,                 // food
    CARD_FAMINE,
    CARD_ACQUISITION,           // good stuff
    CARD_WILD_MAGIC,
    CARD_HASTEN,
    CARD_TORMENT,
    CARD_DEMON_SWARM,
    CARD_SLOW
};

static const int DECK_POWER_SIZE = sizeof(deck_of_power) / sizeof(int);

// Supposed to be bad, small chance of OK... Nemelex wouldn't like a game
// that didn't have some chance of "losing".
static int deck_of_punishment[] =
{
    CARD_BLANK,
    CARD_BUTTERFLY,
    CARD_WRAITH,
    CARD_STUPIDITY,
    CARD_WEAKNESS,
    CARD_SLOTH,
    CARD_SHUFFLE,
    CARD_FREAK,
    CARD_DEATH,
    CARD_NORMALITY,
    CARD_SHADOW,
    CARD_GATE,
    CARD_DEMON_LESSER,
    CARD_DEMON_COMMON,
    CARD_DEMON_GREATER,
    CARD_FIEND,
    CARD_YAK,
    CARD_DRAGON,
    CARD_GOLEM,
    CARD_THING_FUGLY,
    CARD_HORROR_UNSEEN,
    CARD_LICH,
    CARD_VENOM,
    CARD_SLOW,
    CARD_DECAY,
    CARD_TORMENT,
    CARD_FAMINE,
    CARD_WILD_MAGIC,
    CARD_MAZE,
    CARD_PANDEMONIUM
};

static const int DECK_PUNISHMENT_SIZE = sizeof(deck_of_punishment) / sizeof(int);

static bool card_gain_stat( int stat )
{
    bool ret = false;

    const int amount = 1 + (random2(15) <= you.skills[SK_EVOCATIONS])
                         + (random2(30) <= you.skills[SK_EVOCATIONS])
                         + (random2(30) <= you.skills[SK_EVOCATIONS]);

    const int stat_mut = (stat == STAT_STRENGTH)        ? MUT_STRONG :
                         (stat == STAT_INTELLIGENCE)    ? MUT_CLEVER
                                                        : MUT_AGILE;

    for (int i = 0; i < amount; i++)
    {
        if (mutate( stat_mut, true ))
            ret = true;
    }

    return (ret);
}

static bool card_lose_stat( int stat )
{
    bool ret = false;

    const int amount = roll_dice(2, 2) - (random2(30) <= you.skills[SK_EVOCATIONS]);

    const int stat_mut = (stat == STAT_STRENGTH)        ? MUT_WEAK :
                         (stat == STAT_INTELLIGENCE)    ? MUT_DOPEY
                                                        : MUT_CLUMSY;

    for (int i = 0; i < amount; i++)
    {
        if (mutate( stat_mut, true ))
            ret = true;
    }

    return (ret);
}

static void shuffle_stats( void )
{
    FixedVector< int, NUM_STATS > base;         // base max w/o modifications
    FixedVector< int, NUM_STATS > mod;          // diff of base and net max
    FixedVector< int, NUM_STATS > drain;        // temp loss of stat
    int stat;

    // retrieve unmodified max stats
    base[STAT_STRENGTH] = player_base_stat( STAT_STRENGTH );
    base[STAT_INTELLIGENCE] = player_base_stat( STAT_INTELLIGENCE );
    base[STAT_DEXTERITY] = player_base_stat( STAT_DEXTERITY );

    // calculate modification
    mod[STAT_STRENGTH] = you.max_str - base[STAT_STRENGTH];
    mod[STAT_INTELLIGENCE] = you.max_intel - base[STAT_INTELLIGENCE];
    mod[STAT_DEXTERITY] = you.max_dex - base[STAT_DEXTERITY];

    // calculate amount that stat is drained
    drain[STAT_STRENGTH] = you.max_str - you.str;
    drain[STAT_INTELLIGENCE] = you.max_intel - you.intel;
    drain[STAT_DEXTERITY] = you.max_dex - you.dex;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS,
          "initial str = %d + %d - %d; int = %d + %d - %d; dex = %d + %d - %d",
          base[STAT_STRENGTH], mod[STAT_STRENGTH], drain[STAT_STRENGTH],
          base[STAT_INTELLIGENCE], mod[STAT_INTELLIGENCE], drain[STAT_INTELLIGENCE],
          base[STAT_DEXTERITY], mod[STAT_DEXTERITY], drain[STAT_DEXTERITY] );

#endif

    you.str = 101;
    you.intel = 101;
    you.dex = 101;

    do
    {
        stat = random2(NUM_STATS);

        if (base[stat] == 101)
            continue;

        if (you.str == 101)
        {
            you.max_str = base[stat] + mod[STAT_STRENGTH];
            you.str = you.max_str - drain[STAT_STRENGTH];
        }
        else if (you.intel == 101)
        {
            you.max_intel = base[stat] + mod[STAT_INTELLIGENCE];
            you.intel = you.max_intel - drain[STAT_INTELLIGENCE];
        }
        else if (you.dex == 101)
        {
            you.max_dex = base[stat] + mod[STAT_DEXTERITY];
            you.dex = you.max_dex - drain[STAT_DEXTERITY];
        }

        base[stat] = 101;
    }
    while (base[STAT_STRENGTH] != 101 || base[STAT_DEXTERITY] != 101
                                       || base[STAT_INTELLIGENCE] != 101);

    set_redraw_status( REDRAW_STRENGTH );
    set_redraw_status( REDRAW_INTELLIGENCE );
    set_redraw_status( REDRAW_DEXTERITY );
    burden_change();
}

void deck_of_cards( int type )
{

    // I really am not fond of how all of this works, the
    // decks ought to be stored (possibly) in an array of
    // pointers to int or as discrete arrays of int using
    // the sizeof operator to determine upper bounds, and
    // not defines, which is a bit clumsy given that you
    // have to update two things presently (the array and
    // the corresponding define) in order to add things to
    // decks ... someone fix this, or I will {dlb}
    int *card = deck_of_wonders;
    int  max_card = 0;
    int  i = 0;
    int  brownie_points = 0;     // for passing to did_god_conduct() {dlb}

    const int deck = ((type == DECK_OF_PUNISHMENT) ? -1 : get_inv_in_hand());

    ASSERT( type == DECK_OF_PUNISHMENT || deck != -1 );

    mpr("You draw a card...");

    switch (type)
    {
    case DECK_OF_WONDERS:
        card = deck_of_wonders;
        max_card = DECK_WONDERS_SIZE;
        break;
    case DECK_OF_SUMMONING:
        card = deck_of_summoning;
        max_card = DECK_SUMMONING_SIZE;
        break;
    case DECK_OF_TRICKS:
        card = deck_of_tricks;
        max_card = DECK_TRICKS_SIZE;
        break;
    case DECK_OF_POWER:
        card = deck_of_power;
        max_card = DECK_POWER_SIZE;
        break;
    case DECK_OF_PUNISHMENT:
        card = deck_of_punishment;
        max_card = DECK_PUNISHMENT_SIZE;
        break;
    }

    i = card[ random2(max_card) ];

    if (one_chance_in(250))
    {
        mpr("This card doesn't seem to belong here.");
        i = random2(NUM_CARDS);
    }

    if (i == CARD_BLANK && you.skills[SK_EVOCATIONS] > random2(30))
        i = card[ random2(max_card) ];

    cards( i, (type == DECK_OF_PUNISHMENT) );

    // Decks of punishment aren't objects in the game,
    // it's just Nemelex's form of punishment... currently
    // they're the only non-deck based card effect coming
    // through here. -- bwr
    if (deck != -1)
    {
        you.inv[deck].plus--;

        if (you.inv[deck].plus == 0)
        {
            mpr("The deck of cards disappears in a puff of smoke.");
            dec_inv_item_quantity( deck, 1 ); // calls unwield_item()

            // these bonuses happen only when the deck expires {dlb}:
            brownie_points = (coinflip() ? 2 : 1);

            if (type == DECK_OF_WONDERS)
                brownie_points += 2;
            else if (type == DECK_OF_POWER)
                brownie_points++;
        }

        // this bonus happens with every use {dlb}:
        if (type == DECK_OF_WONDERS || one_chance_in(3))
            brownie_points++;

        did_god_conduct( DID_CARDS, brownie_points );
    }
}                               // end deck_of_cards()

void cards( int which_card, bool punish )
{
    int altar = 0;
    int x, y;
    int loopy = 0;              // general purpose loop variable {dlb}
    bool success;               // for summoning messages {dlb}
    bool failMsg;
    int summ_dur, summ_num, summ_mon;
    beh_type summ_beh;

    if (which_card == CARD_BLANK && one_chance_in(10))
        which_card = CARD_RULES_FOR_BRIDGE;

    switch (which_card)
    {
    default:
    case CARD_BLANK:
        mpr("It is blank.");
        break;

    case CARD_BUTTERFLY:
        mpr("You have drawn the Butterfly.");

        if (cast_summon_butterflies( 10 + you.skills[SK_EVOCATIONS] * 5 ))
            mpr("Brightly coloured insects fly from the card!");
        break;

    case CARD_WRAITH:
        mpr("You have drawn the Wraith.");

        lose_level();
        drain_exp();
        break;

    case CARD_EXPERIENCE:
        mpr( "You have drawn Experience." );
        potion_effect( POT_EXPERIENCE, 0 );
        break;

    case CARD_WEALTH:
        mpr("You have drawn Wealth.");

        you.gold += 120 + roll_dice( 2, 20 * you.skills[SK_EVOCATIONS] );
        set_redraw_status( REDRAW_GOLD );
        break;

    case CARD_INTELLIGENCE:
        mpr("You have drawn the Brain!");

        card_gain_stat( STAT_INTELLIGENCE );
        set_redraw_status( REDRAW_INTELLIGENCE );
        break;

    case CARD_STRENGTH:
        mpr("You have drawn Strength!");

        card_gain_stat( STAT_STRENGTH );
        set_redraw_status( REDRAW_STRENGTH );
        break;

    case CARD_QUICKSILVER:
        mpr("You have drawn the Quicksilver card.");

        card_gain_stat( STAT_DEXTERITY );
        set_redraw_status( REDRAW_DEXTERITY );
        break;

    case CARD_STUPIDITY:
        mpr("You have drawn Stupidity!");

        card_lose_stat( STAT_INTELLIGENCE );

        if (punish && random2(30) > you.skills[SK_EVOCATIONS])
            you.max_intel--;

        set_redraw_status( REDRAW_INTELLIGENCE );
        break;

    case CARD_WEAKNESS:
        mpr("You have drawn Weakness.");

        card_lose_stat( STAT_STRENGTH );

        if (punish && random2(30) > you.skills[SK_EVOCATIONS])
            you.max_str--;

        set_redraw_status( REDRAW_STRENGTH );
        break;

    case CARD_SLOTH:
        mpr("You have drawn the Slug.");

        card_lose_stat( STAT_DEXTERITY );

        if (punish && random2(30) > you.skills[SK_EVOCATIONS])
            you.max_dex--;

        set_redraw_status( REDRAW_DEXTERITY );
        break;

    case CARD_SHUFFLE:          // shuffle stats
        mpr("You have drawn the Shuffle card!");
        shuffle_stats();
        break;

    case CARD_FREAK:
        mpr("You have drawn the Freak!");

        failMsg = true;
        for (loopy = 0; loopy < 6; loopy++)
        {
            if (punish && !one_chance_in(5))
                success = (give_bad_mutation( true, false ) != MUT_NONE);
            else
                success = mutate( PICK_RANDOM_MUTATION, false, false );

            if (success)
                failMsg = false;
        }

        if (failMsg)
            mpr("You feel odd for a moment.");
        break;

    case CARD_DEATH:
        mpr("Oh no! You have drawn the Death card!");

        if (you.duration[DUR_TELEPORT])
            you_teleport();

        for (loopy = 0; loopy < 5; loopy++)
        {
            create_monster( MONS_REAPER );
        }
        break;

    case CARD_NORMALITY:
        mpr("You have drawn Normalisation.");

        for (loopy = 0; loopy < 6; loopy++)
        {
            delete_mutation( (punish) ? PICK_RANDOM_MUTATION
                                      : PICK_NONSTAT_MUTATION );
        }
        break;

    case CARD_SHADOW:
        mpr("You have drawn the Shadow.");
        create_monster( MONS_SOUL_EATER );
        break;

    case CARD_GATE:
        mpr("You have drawn the Gate!");
        more();

        if (you.level_type == LEVEL_ABYSS)
            banished( DNGN_EXIT_ABYSS );
        else if (you.level_type == LEVEL_LABYRINTH)
            canned_msg( MSG_NOTHING_HAPPENS );
        else
            banished( DNGN_ENTER_ABYSS );
        break;

    case CARD_STATUE:
        mpr("You have drawn the Crystal Statue.");
        create_monster( MONS_CRYSTAL_GOLEM, BEH_FRIENDLY );
        break;

    case CARD_ACQUISITION:
        mpr( "You have drawn Acquisition!" );
        mpr( "The card unfolds to form a scroll of paper." );
        acquirement( OBJ_RANDOM );
        break;

    case CARD_HASTEN:
        mpr("You have drawn Haste.");
        potion_effect( POT_SPEED, 5 * you.skills[SK_EVOCATIONS] );
        break;

    case CARD_DEMON_LESSER:
        mpr("On the card is a picture of a little demon.");

        summ_mon = rand_demon( DEMON_LESSER );

        summ_dur = 1 + random2(3) + you.skills[SK_EVOCATIONS] / 3;
        if (summ_dur > 6)
            summ_dur = 6;

        summ_beh = ((punish) ? BEH_HOSTILE : BEH_FRIENDLY);

        if (create_monster( summ_mon, summ_beh, summ_dur ) != -1)
            mpr("The picture comes to life!");
        break;

    case CARD_DEMON_COMMON:
        mpr("On the card is a picture of a demon.");

        summ_mon = rand_demon( DEMON_COMMON );

        summ_dur = 1 + random2(3) + you.skills[SK_EVOCATIONS] / 4;
        if (summ_dur > 6)
            summ_dur = 6;

        summ_beh = ((punish) ? BEH_HOSTILE : BEH_FRIENDLY);

        if (create_monster( summ_mon, summ_beh, summ_dur ) != -1)
            mpr("The picture comes to life!");
        break;

    case CARD_DEMON_GREATER:
        mpr("On the card is a picture of a huge demon.");

        summ_mon = rand_demon( DEMON_GREATER );

        if (punish)
            summ_beh = BEH_HOSTILE;
        else
        {
            summ_beh = (you.skills[SK_EVOCATIONS] > random2(30)) ? BEH_FRIENDLY
                                                                 : BEH_CHARMED;
        }

        if (summ_beh == BEH_CHARMED)
            mpr( "You don't feel so good about this..." );

        if (create_monster( summ_mon, summ_beh, 5 ) != -1)
            mpr("The picture comes to life!");
        break;

    case CARD_DEMON_SWARM:
        mpr("On the card is a picture of a swarm of little demons.");

        success = false;

        summ_num = 7 + random2(6);

        for (loopy = 0; loopy < summ_num; loopy++)
        {
            summ_mon = rand_demon( DEMON_LESSER );

            if (create_monster( summ_mon, BEH_HOSTILE, 6 ) != -1)
                 success = true;
        }

        if (success)
            mpr("The picture comes to life!");
        break;

    case CARD_YAK:
        mpr("On the card is a picture of a huge shaggy yak.");

        summ_dur = 2 + you.skills[SK_EVOCATIONS] / 2;
        if (summ_dur > 6)
            summ_dur = 6;

        summ_mon = (random2(you.skills[SK_EVOCATIONS]) < 5) ? MONS_YAK
                                                            : MONS_DEATH_YAK;

        summ_beh = ((punish) ? BEH_HOSTILE : BEH_FRIENDLY);

        if (one_chance_in(50))
            summ_mon = MONS_SHEEP;

        if (create_monster( summ_mon, summ_beh, summ_dur ) != -1)
        {
            mpr("The picture comes to life!");

            if (summ_mon == MONS_SHEEP)
                mpr("Hey! That's not a yak!");
        }
        break;

    case CARD_FIEND:
        mpr("On the card is a picture of a huge scaly devil.");

        summ_dur = 1 + random2(3) + you.skills[SK_EVOCATIONS] / 6;
        if (summ_dur > 6)
            summ_dur = 6;

        if (punish)
            summ_beh = BEH_HOSTILE;
        else
        {
            summ_beh = (you.skills[SK_EVOCATIONS] > random2(30)) ? BEH_FRIENDLY
                                                                 : BEH_CHARMED;
        }

        if (summ_beh == BEH_CHARMED)
            mpr( "You don't feel so good about this..." );

        if (create_monster( MONS_FIEND, summ_beh, summ_dur ) != -1)
            mpr("The picture comes to life!");
        break;

    case CARD_DRAGON:
        mpr("On the card is a picture of a huge scaly dragon.");

        summ_dur = 3 + you.skills[SK_EVOCATIONS] / 6;
        if (summ_dur > 6)
            summ_dur = 6;

        summ_mon = coinflip() ? MONS_DRAGON : MONS_ICE_DRAGON;
        summ_beh = ((punish) ? BEH_HOSTILE : BEH_FRIENDLY);

        if (create_monster( summ_mon, summ_beh, summ_dur ) != -1)
            mpr("The picture comes to life!");
        break;

    case CARD_GOLEM:
        mpr("On the card is a picture of a statue.");

        summ_dur = 2 + you.skills[SK_EVOCATIONS] / 4;
        if (summ_dur > 6)
            summ_dur = 6;

        summ_mon = MONS_CLAY_GOLEM + random2(6);
        summ_beh = ((punish) ? BEH_HOSTILE : BEH_FRIENDLY);

        if (create_monster( summ_mon, summ_beh, summ_dur ) != -1)
            mpr("The picture comes to life!");
        break;

    case CARD_THING_FUGLY:
        mpr("On the card is a picture of an ugly thing.");

        summ_dur = 2 + you.skills[SK_EVOCATIONS] / 4;
        if (summ_dur > 6)
            summ_dur = 6;

        summ_mon = (random2(you.skills[SK_EVOCATIONS]) < 5) ? MONS_UGLY_THING
                                                            : MONS_VERY_UGLY_THING;
        summ_beh = ((punish) ? BEH_HOSTILE : BEH_FRIENDLY);

        if (create_monster( summ_mon, summ_beh, summ_dur ) != -1)
            mpr("The picture comes to life!");
        break;

    case CARD_LICH:
        mpr( "On the card is a picture of a very irate looking skeletal being." );

        if (create_monster( MONS_LICH ) != -1)
            mpr("The picture comes to life!");
        break;

    case CARD_HORROR_UNSEEN:
        if (!player_see_invis())
        {
            mpr("It is blank.");
            summ_beh = BEH_HOSTILE;
        }
        else
        {
            mpr("On the card is a picture of a hideous abomination.");
            summ_beh = ((punish) ? BEH_HOSTILE : BEH_FRIENDLY);
        }

        if (summ_beh == BEH_HOSTILE)
            summ_dur = 0;
        else
        {
            summ_dur = 2 + you.skills[SK_EVOCATIONS] / 4;
            if (summ_dur > 6)
                summ_dur = 6;
        }

        if (create_monster( MONS_UNSEEN_HORROR, summ_beh, summ_dur ) != -1)
        {
            if (player_see_invis())
                mpr("The picture comes to life!");
        }
        break;

    case CARD_BLINK:
        mpr("You have drawn Blink.");

        if (one_chance_in( you.skills[SK_EVOCATIONS] ))
            random_blink( 8 + skill_bump( SK_EVOCATIONS ) * 3 );
        else
            blink();
        break;

    case CARD_TELEPORT:
        mpr("You have drawn the Portal of Delayed Transposition.");
        you_teleport();
        break;

    case CARD_TELEPORT_NOW:
        mpr( "You have drawn the Portal of Instantaneous Transposition." );
        you_teleport2( true, true );  // in abyss, always to new area
        break;

    case CARD_RAGE:
        mpr("You have drawn Rage.");

        if (!go_berserk(false))
            canned_msg(MSG_NOTHING_HAPPENS);
        else
        {
            you.berserk_penalty = NO_BERSERK_PENALTY;

            // Being really nice here.  Since the traditional
            // way to make good use this berserk is to drop the
            // deck immediately and go unarmed, we'll autodisarm
            // the character in the same action.
            if (!punish && you.equip[EQ_WEAPON] != -1)
            {
                ASSERT(you.inv[you.equip[EQ_WEAPON]].base_type == OBJ_MISCELLANY
                       && (you.inv[you.equip[EQ_WEAPON]].sub_type == MISC_DECK_OF_WONDERS
                           || you.inv[you.equip[EQ_WEAPON]].sub_type == MISC_DECK_OF_SUMMONINGS
                           || you.inv[you.equip[EQ_WEAPON]].sub_type == MISC_DECK_OF_TRICKS
                           || you.inv[you.equip[EQ_WEAPON]].sub_type == MISC_DECK_OF_POWER));

                unwield_item();
            }
        }
        break;

    case CARD_LEVITY:
        mpr("You have drawn Levity.");
        potion_effect( POT_LEVITATION, 5 * you.skills[SK_EVOCATIONS] );
        break;

    case CARD_VENOM:
        mpr("You have drawn Venom.");
        if (punish)
            poison_player( 2 + random2(7) );
        else
            poison_player( 2 + random2(7 - you.skills[SK_EVOCATIONS] / 5) );
        break;

    case CARD_XOM:
        mpr("You have drawn the card of Xom!");
        Xom_acts( true, 5 + random2( you.skills[SK_EVOCATIONS] ), true );
        break;

    case CARD_SLOW:
        mpr("You have drawn Slowness.");
        if (punish)
            potion_effect( POT_SLOWING, 100 );
        else
            potion_effect( POT_SLOWING, 100 - 2 * you.skills[SK_EVOCATIONS] );
        break;

    case CARD_DECAY:
        mpr("You have drawn Decay.");

        if (you.is_undead)
            mpr("You feel terrible.");
        else
        {
            if (punish)
                rot_player( 2 + random2(7) );
            else
                rot_player( 2 + random2(7 - you.skills[SK_EVOCATIONS] / 4) );
        }
        break;

    case CARD_HEALING:
        mpr("You have drawn the Elixir of Health.");
        potion_effect( POT_HEALING, 30 + 3 * you.skills[SK_EVOCATIONS] );
        break;

    case CARD_HEAL_WOUNDS:
        mpr("You have drawn the Symbol of Immediate Regeneration.");
        potion_effect( POT_HEAL_WOUNDS, 30 + 3 * you.skills[SK_EVOCATIONS] );
        break;

    case CARD_TORMENT:
        mpr("You have drawn the Symbol of Torment.");
        torment( you.x_pos, you.y_pos );
        break;

    // what about checking whether there are items there, too? {dlb}
    case CARD_FOUNTAIN:
        mpr("You have drawn the Fountain.");

        if (grd[you.x_pos][you.y_pos] == DNGN_FLOOR)
        {
            strcpy( info, "A beautiful fountain of clear blue water grows "
                          "from the floor " );

            strcat( info, (you.species == SP_NAGA || you.species == SP_CENTAUR)
                                ? "before you!" : "at your feet!" );
            mpr(info);
            grd[you.x_pos][you.y_pos] = DNGN_BLUE_FOUNTAIN;
        }
        else
        {
            canned_msg(MSG_NOTHING_HAPPENS);
        }
        break;

    case CARD_ALTAR:
        mpr("You have drawn the Altar.");

        if (you.religion == GOD_NO_GOD)
            canned_msg(MSG_NOTHING_HAPPENS);
        else
        {
            altar = DNGN_ALTAR_ZIN + you.religion - 1;

            if (grd[you.x_pos][you.y_pos] == DNGN_FLOOR)
            {
                strcpy(info, "An altar grows from the floor ");
                strcat(info,
                        (you.species == SP_NAGA || you.species == SP_CENTAUR)
                                            ? "before you!" : "at your feet!");
                mpr(info);
                grd[you.x_pos][you.y_pos] = altar;
            }
            else
            {
                random_in_bounds( x, y );
                grd[x][y] = altar;

                mpr( "You sense divine power!" );
            }
        }
        break;

    case CARD_FAMINE:
        mpr("You have drawn Famine.");

        if (you.is_undead == US_UNDEAD)
            mpr("You feel rather smug.");
        else
            set_hunger(500, true);
        break;

    case CARD_FEAST:
        mpr("You have drawn the Feast.");

        if (you.is_undead == US_UNDEAD)
            mpr("You feel a horrible emptiness.");
        else
            set_hunger(12000, true);
        break;

    case CARD_WILD_MAGIC:
        mpr( "You have drawn Wild Magic." );
        miscast_effect( SPTYP_RANDOM, random2(15) + 5, random2(250), 0 );
        break;

    case CARD_VIOLENCE:
        mpr("You have drawn Violence.");
        acquirement( OBJ_WEAPONS );
        break;

    case CARD_PROTECTION:
        mpr("You have drawn Protection.");
        acquirement( OBJ_ARMOUR );
        break;

    case CARD_KNOWLEDGE:
        mpr("You have drawn Knowledge.");
        acquirement( OBJ_BOOKS );
        break;

    case CARD_MAZE:
        mpr("You have drawn the Maze!");
        more();

        if (you.level_type == LEVEL_DUNGEON)
            banished( DNGN_ENTER_LABYRINTH );
        break;

    case CARD_PANDEMONIUM:
        mpr("You have drawn the Pandemonium card!");
        more();

        if (you.level_type == LEVEL_PANDEMONIUM)
            banished( DNGN_EXIT_PANDEMONIUM );
        else if (you.level_type == LEVEL_LABYRINTH)
            canned_msg( MSG_NOTHING_HAPPENS );
        else
            banished( DNGN_ENTER_PANDEMONIUM );
        break;

    case CARD_RULES_FOR_BRIDGE:
        mpr("You have drawn the rules for contract bridge.");
        mpr("How intriguing!");
        break;

    case CARD_IMPRISONMENT:
        mpr("You have drawn the Prison!");
        cast_entomb(1000);
        break;
    }

    return;
}                               // end cards()

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

#include <string.h>

#include "externs.h"
#include "enum.h"

#include "dot_h.h"
#include "mutation.h"
#include "religion.h"

/* Number of cards. This number includes the first (number zero) card */
#define DECK_WONDERS 27
#define DECK_SUMMONING 11
#define DECK_TRICKS 11
#define DECK_POWER 17

#define TOTAL_CARDS 55

char mutate(int which_mutation);

unsigned char deck_of_wonders[] =
{
    0,                          /* Blank */
    1,                          /* Butterfly */
    2,                          /* Wraith */
    3,                          /* Xp */
    4,                          /* Wealth */
    5,                          /* Brain */
    6,                          /* Strength */
    7,                          /* Quicksilver */
    8,                          /* Stupid */
    9,                          /* Weak */
    10,                         /* Slug */
    11,                         /* Shuffle */
    12,                         /* Freak */
    13,                         /* Death */
    14,                         /* Normalisation */
    15,                         /* Shadow */
    16,                         /* Gate */
    17,                         /* Statue */
    18,                         /* Acquisition */
    19,                         /* Haste */
    29,                         /* Lich */
    37,                         /* Xom */
    39,                         /* Decay */
    44,                         /* Altar */
    43,                         /* Fountain */
    51,                         /* Maze */
    52,                         /* Pandemonium */
    0,                          /* Sanity buffer... */
    0                           /* */
};

unsigned char deck_of_summoning[] =
{
    17,                         /* Statue */
    20,                         /* Little demon */
    21,                         /* Bigger demon */
    22,                         /* Very big dem */
    23,                         /* Demon swarm */
    24,                         /* Yak */
    25,                         /* Fiend */
    26,                         /* Dragon */
    27,                         /* golem */
    28,                         /* Very ugly */
    30,                         /* Unseen horror */
    0,                          /* Sanity buffer... */
    0                           /* */

};

unsigned char deck_of_tricks[] =
{
    0,                          /* Blank */
    1,                          /* Butterfly */
    31,                         /* Blink */
    32,                         /* Teleport */
    33,                         /* Inst teleport */
    34,                         /* Rage */
    35,                         /* Levity */
                                                                  //38, /* Slowness */
    40,                         /* Healing */
    47,                         /* Wild Magic */
    20,                         /* Little demon */
    19,                         /* Haste */
    0,                          /* Sanity buffer... */
    0                           /* */

};

unsigned char deck_of_power[] =
{
    0,                          /* Blank */
    21,                         /* Demon */
    22,                         /* Big demon */
    33,                         /* Inst teleport */
    36,                         /* Venom */
    37,                         /* Xom */
    41,                         /* Heal wounds */
    45,                         /* Famine */
    46,                         /* Feast */
    47,                         /* Wild Magic */
    48,                         /* Violence */
    49,                         /* Protection */
    50,                         /* Knowledge */
    19,                         /* Haste */
    42,                         /* Torment */
    23,                         /* Demon Swarm (bad) */
    38,                         /* Slowness */
    0,                          /* Sanity buffer... */
    0                           /* */
};

void cards(unsigned char which_card);


void deck_of_cards(unsigned char which_deck)
{
    int card[50];
    unsigned char max_card = 0;
    int i = 0;

    strcpy(info, "You draw a card...");
    mpr(info);

    switch (which_deck)
    {
    case 0:
        for (i = 0; i < DECK_WONDERS; i++)
        {
            card[i] = deck_of_wonders[i];
        }
        max_card = DECK_WONDERS;
        break;
    case 1:
        for (i = 0; i < DECK_SUMMONING; i++)
        {
            card[i] = deck_of_summoning[i];
        }
        max_card = DECK_SUMMONING;
        break;
    case 2:
        for (i = 0; i < DECK_TRICKS; i++)
        {
            card[i] = deck_of_tricks[i];
        }
        max_card = DECK_TRICKS;
        break;
    case 3:
        for (i = 0; i < DECK_POWER; i++)
        {
            card[i] = deck_of_power[i];
        }
        max_card = DECK_POWER;
        break;
    }

    i = card[random2(max_card)];

    if (random2(250) == 0)
    {
        mpr("This card doesn't seem to belong here.");
        i = random2(TOTAL_CARDS);
    }

    cards(i);

    you.inv_plus[you.equip[EQ_WEAPON]]--;
    if (you.inv_plus[you.equip[EQ_WEAPON]] == 0)
    {
        strcpy(info, "The deck of cards disappears in a puff of smoke.");
        mpr(info);
        unwield_item(you.equip[EQ_WEAPON]);
        you.inv_quantity[you.equip[EQ_WEAPON]]--;
        if (you.inv_quantity[you.equip[EQ_WEAPON]] == 0)        // can this be false?

        {
            you.num_inv_items--;
            you.equip[EQ_WEAPON] = -1;
            strcpy(info, "You are now empty handed.");
            mpr(info);
        }
        done_good(11, 1 + random2(2));
        if (which_deck == 0)
            done_good(11, 2);
        if (which_deck == 3)
            done_good(11, 1);
    }
    burden_change();

    if (random2(3) == 0 || which_deck == 0)
        done_good(11, 1);
    /* Using cards pleases Nemelex */

}


void cards(unsigned char which_card)
{

    int dvar[5];
    int dvar1 = 0;

    switch (which_card)
    {
    default:
    case 0:
        strcpy(info, "It is blank.");
        mpr(info);
        break;
    case 1:
        mpr("You have drawn the Butterfly.");
        mpr("A brightly coloured insect flies from the card!");
        create_monster(MONS_BUTTERFLY, 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
        break;
    case 2:
        mpr("You have drawn the Wraith.");
        lose_level();
        drain_exp();
        break;
    case 3:
        mpr("You have drawn Experience.");
        if (you.experience < 27)
            you.experience = exp_needed(you.experience_level + 2, you.species) + 1;
        level_change();
        break;
    case 4:
        mpr("You have drawn Wealth.");
        you.gold += 800 + random2(500) + random2(500);
        you.redraw_gold = 1;
        break;
    case 5:
        mpr("You have drawn the Brain!");
        you.max_intel += 1 + random2(2) + random2(2);
        you.redraw_intelligence = 1;    // note that only the maximum is increased

        break;
    case 6:
        mpr("You have drawn Strength!");
        you.max_strength += 1 + random2(2) + random2(2);
        you.redraw_strength = 1;
        break;
    case 7:
        mpr("You have drawn the Quicksilver card.");
        you.max_dex += 1 + random2(2) + random2(2);
        you.redraw_dexterity = 1;
        break;
    case 8:
        mpr("You have drawn Stupidity!");
        you.intel -= 2 + random2(2) + random2(2);
        if (you.intel <= 3)
            you.intel = 0;
        you.redraw_intelligence = 1;
        break;
    case 9:
        mpr("You have drawn Weakness.");
        you.strength -= 2 + random2(2) + random2(2);
        if (you.strength <= 3)
            you.strength = 0;
        you.redraw_strength = 1;
        break;
    case 10:
        mpr("You have drawn the Slug.");
        you.dex -= 2 + random2(2) + random2(2);
        if (you.dex <= 3)
            you.dex = 0;
        you.redraw_dexterity = 1;
        break;
    case 11:                    // shuffle stats

        mpr("You have drawn the Shuffle card!");
        //you.strength = you.max_strength;
        //you.intel = you.max_intel;
        //you.dex = you.max_dex;
        dvar[0] = you.strength;
        dvar[1] = you.intel;
        dvar[2] = you.dex;
        you.strength = 101;
        you.intel = 101;
        you.dex = 101;
        do
        {
            dvar1 = random2(3);
            if (dvar[dvar1] == 101)
            {
                continue;
            }

            if (you.strength == 101)
            {
                you.strength = dvar[dvar1];
                you.max_strength = dvar[dvar1];
            }
            else if (you.intel == 101)
            {
                you.intel = dvar[dvar1];
                you.max_intel = dvar[dvar1];
            }
            else if (you.dex == 101)
            {
                you.dex = dvar[dvar1];
                you.max_dex = dvar[dvar1];
            }
            dvar[dvar1] = 101;
        }
        while (dvar[0] != 101 || dvar[1] != 101 || dvar[2] != 101);
        you.redraw_strength = 1;
        you.redraw_intelligence = 1;
        you.redraw_dexterity = 1;
        burden_change();
        break;
    case 12:                    // mutation

        mpr("You have drawn the Freak!");
        mutate(100);
        mutate(100);
        mutate(100);
        mutate(100);
        mutate(100);
        mutate(100);
        break;
    case 13:                    // reaper

        strcpy(info, "Oh no! You have drawn the Death card!");
        if (you.duration[DUR_TELEPORT] != 0)
            you_teleport();     /* heh heh heh */
        create_monster(MONS_REAPER, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);
        create_monster(MONS_REAPER, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);
        create_monster(MONS_REAPER, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);
        create_monster(MONS_REAPER, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);
        create_monster(MONS_REAPER, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);
        break;
    case 14:                    // lose mutations

        mpr("You have drawn Normalisation.");
        delete_mutation(100);
        delete_mutation(100);
        delete_mutation(100);
        delete_mutation(100);
        delete_mutation(100);
        delete_mutation(100);
        break;
    case 15:                    // soul eater

        mpr("You have drawn the Shadow.");
        create_monster(MONS_SOUL_EATER, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);
        break;
    case 16:                    // gate to abyss

        mpr("You have drawn the Gate!");
        more();
        if (you.level_type == 2)
        {
            banished(97);
            break;
        }
        if (you.level_type == 1)
        {
            mpr("Nothing appears to happen.");
            break;
        }
        banished(96);
        break;
    case 17:                    // pet

        mpr("You have drawn the Crystal Statue.");
        create_monster(MONS_CRYSTAL_GOLEM, 0, 7, you.x_pos, you.y_pos, you.pet_target, 250);
        break;
    case 18:                    // acquirement

        mpr("You have drawn Acquisition!");
        mpr("The card unfolds to form a scroll of paper.");
        acquirement(250);
        break;
    case 19:                    // Haste

        mpr("You have drawn Haste.");
        potion_effect(POT_SPEED, 150);
        break;
    case 20:
        strcpy(info, "On the card is a picture of a little demon.");
        mpr(info);
        strcpy(info, "The picture comes to life!");
        mpr(info);
        create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 7, you.x_pos, you.y_pos, MHITYOU, 250);
        break;
    case 21:
        strcpy(info, "On the card is a picture of a demon.");
        mpr(info);
        strcpy(info, "The picture comes to life!");
        mpr(info);
        create_monster(MONS_NEQOXEC + random2(5), 21 + random2(3), 7, you.x_pos, you.y_pos, MHITYOU, 250);
        break;
    case 22:
        strcpy(info, "On the card is a picture of a huge demon.");
        mpr(info);
        strcpy(info, "The picture comes to life!");
        mpr(info);
        create_monster(MONS_NEQOXEC + random2(5), 20 + random2(3), 7, you.x_pos, you.y_pos, MHITYOU, 250);
        break;
    case 23:
        strcpy(info, "On the card is a picture of a swarm of little demons.");
        mpr(info);
        strcpy(info, "The picture comes to life!");
        mpr(info);
        create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
        create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
        create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
        create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
        create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
        create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
        create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
        break;
    case 24:
        strcpy(info, "On the card is a picture of a huge shaggy yak.");
        mpr(info);
        strcpy(info, "The picture comes to life!");
        mpr(info);
        create_monster(MONS_DEATH_YAK, 25, 7, you.x_pos, you.y_pos, MHITYOU, 250);
        break;
    case 25:
        strcpy(info, "On the card is a picture of a huge scaly devil.");
        mpr(info);
        strcpy(info, "The picture comes to life!");
        mpr(info);
        create_monster(MONS_FIEND, 21, 7, you.x_pos, you.y_pos, MHITYOU, 250);
        break;
    case 26:
        strcpy(info, "On the card is a picture of a huge scaly dragon.");
        mpr(info);
        strcpy(info, "The picture comes to life!");
        mpr(info);
        if (random2(2) == 0)
            create_monster(MONS_DRAGON, 22, 7, you.x_pos, you.y_pos, MHITYOU, 250);
        else
            create_monster(MONS_ICE_DRAGON, 22, 7, you.x_pos, you.y_pos, MHITYOU, 250);
        break;
    case 27:
        strcpy(info, "On the card is a picture of a statue.");
        mpr(info);
        strcpy(info, "The picture comes to life!");
        mpr(info);
        create_monster(MONS_CLAY_GOLEM + random2(6), 21, 7, you.x_pos, you.y_pos, MHITYOU, 250);
        break;
    case 28:
        strcpy(info, "On the card is a picture of a very ugly thing.");
        mpr(info);
        strcpy(info, "The picture comes to life!");
        mpr(info);
        create_monster(MONS_VERY_UGLY_THING, 21, 7, you.x_pos, you.y_pos, MHITYOU, 250);
        break;
    case 29:
        strcpy(info, "On the card is a picture of a very irritated-looking skeletal thing.");
        mpr(info);
        strcpy(info, "The picture comes to life!");
        mpr(info);
        create_monster(MONS_LICH, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);
        break;
    case 30:                    // unseen horror

        if (player_see_invis() == 0)
        {
            strcat(info, "It is blank");
            mpr(info);
        }
        else
        {
            strcpy(info, "On the card is a picture of a hideous abomination.");
            mpr(info);
            strcpy(info, "The picture comes to life!");
            mpr(info);
        }
        create_monster(MONS_UNSEEN_HORROR, 21, 7, you.x_pos, you.y_pos, MHITYOU, 250);
        break;
    case 31:                    // Blink

        mpr("You have drawn Blink.");
        random_blink();
        break;
    case 32:
        mpr("You have drawn the Portal of Delayed Transposition.");
        you_teleport();
        break;
    case 33:
        mpr("You have drawn the Portal of Instantaneous Transposition.");
        you_teleport2(1);
        break;
    case 34:
        mpr("You have drawn Rage.");
        if (go_berserk() == 0) {
            mpr("Nothing appears to happen.");
        }
#ifdef USE_NEW_BERSERK
        else {
            // No penalty if you go berserk from a card
            you.berserk_penalty = NO_BERSERK_PENALTY;
        }
#endif
        break;
    case 35:
        mpr("You have drawn Levity.");
        potion_effect(POT_LEVITATION, 100);
        break;
    case 36:
        mpr("You have drawn Venom.");
        if (player_res_poison() == 0)
        {
            you.poison += 2 + random2(3);
            mpr("You feel poison flood through your system.");
        }
        break;
    case 37:
        mpr("You have drawn the card of Xom!");
        Xom_acts(1, 5 + random2(12), 1);
        break;
    case 38:
        mpr("You have drawn Slowness.");
        potion_effect(POT_SLOWING, 100);
        break;
    case 39:
        mpr("You have drawn Decay.");
        if (you.is_undead != 0)
        {
            strcpy(info, "You feel terrible.");
            mpr(info);
            break;
        }
        strcpy(info, "You feel your flesh start to rot away!");
        mpr(info);
        you.rotting += random2(5) + 4;
        break;
    case 40:
        mpr("You have drawn the Elixir of Health.");
        potion_effect(POT_HEALING, 100);
        break;
    case 41:
        mpr("You have drawn the Symbol of Immediate Regeneration.");
        potion_effect(POT_HEALING, 100);
        break;
    case 42:
        mpr("You have drawn the Symbol of Torment.");
        torment();
        break;
    case 43:
        mpr("You have drawn the Fountain.");
        if (grd[you.x_pos][you.y_pos] == 67)
        {
            mpr("A beautiful fountain of clear blue water grows from the floor at your feet.");
            grd[you.x_pos][you.y_pos] = 200;
        }
        else
            mpr("Nothing appears to happen.");
        break;
    case 44:
        mpr("You have drawn the Altar.");
        if (you.religion == GOD_NO_GOD)
        {
            mpr("Nothing appears to happen.");
            break;
        }
        dvar1 = 179 + you.religion;
        if (grd[you.x_pos][you.y_pos] == 67)
        {
            mpr("An altar grows from the floor at your feet!");
            grd[you.x_pos][you.y_pos] = dvar1;
        }
        else
        {
            do
            {
                dvar[0] = 10 + random2(70);
                dvar[1] = 10 + random2(60);
            }
            while (grd[dvar[0]][dvar[1]] != 67);
            grd[dvar[0]][dvar[1]] = dvar1;
            mpr("An altar waits for you somewhere on this level!");
        }
        break;
    case 45:
        mpr("You have drawn Famine.");
        if (you.is_undead == 2)
        {
            mpr("You feel rather smug.");
            break;
        }
        you.hunger = 500;
        food_change();
        break;
    case 46:
        mpr("You have drawn the Feast.");
        if (you.is_undead == 2)
        {
            mpr("You feel a horrible emptiness.");
            break;
        }
        you.hunger = 12000;
        food_change();
        break;
    case 47:
        mpr("You have drawn Wild Magic.");
        miscast_effect(10 + random2(15), random2(15) + 5, random2(250), 0);
        break;
    case 48:
        mpr("You have drawn Violence.");
        acquirement(0);
        break;
    case 49:
        mpr("You have drawn Protection.");
        acquirement(2);
        break;
    case 50:
        mpr("You have drawn Knowledge.");
        acquirement(10);
        break;
    case 51:
        mpr("You have drawn the Maze!");
        more();
        if (you.level_type != 0)
            break;
        banished(81);
        break;
    case 52:
        mpr("You have drawn the Pandemonium card!");
        more();
        if (you.level_type == 3)
        {
            banished(100);
            break;
        }
        if (you.level_type == 1)
        {
            mpr("Nothing appears to happen.");
            break;
        }
        banished(99);
        break;
    case 53:
        mpr("You have drawn the rules for contract bridge.");
        mpr("How intriguing!");
        break;
    case 54:
        mpr("You have drawn the Prison!");
        entomb();
        break;

        // Fear, immolation, Holy Word

    }

}

//30, /* */
//30, /* */
//30, /* */
//30, /* */
//30, /* */
//30, /* */


/*
   Index:
   0 - Blank
   1 - Butterfly
   2 - Wraith
   3 - Experience
   4 - Wealth
   5 - Brain
   6 - Strength
   7 - Quicksilver
   8 - Stupidity
   9 - Weakness
   10- The Slug
   11- Shuffle
   12- Freak
   13- Death
   14- Normalisation
   15- Shadow
   16- Gate
   17- Statue
   18- Acquisition
   19- Haste
   20- Little demon
   21- Bigger demon
   22- Very big demon
   23- Demon swarm (hostile)
   24- Yak
   25- Fiend
   26- Dragon
   27- Golem
   28- V. ugly
   29- Lich (hostile)
   30- U horror
   31- Blink
   32- Teleport
   33- Inst teleport
   34- Rage
   35- Levity
   36- Venom
   37- Card of Xom
   38- Slowness
   39- Decay
   40- Healing
   41- Heal Wounds
   42- Torment
   43- Fountain
   44- Altar
   45- Famine
   46- Feast
   47- Wild Magic
   48- Violence
   49- Protection
   50- Knowledge
   51- Maze
   52- Pandemonium
   53- rules for bridge (joke)
   54- Prison
   55-
   56-
   57-
   58-
   59-
   60-
   61-
   62-
   63-
   64-
   65-
   66-
   67-
   68-
   69-

 */





/*void deck_of_summonings(void)
   {

   strcpy(info, "You draw a card from the deck...");
   mpr(info);

   switch(random2(13))
   {
   case 0:
   strcpy(info, "It is blank.");
   mpr(info);
   break;
   case 1:
   strcpy(info, "It turns into a brightly coloured butterfly!");
   mpr(info);
   create_monster(MONS_BUTTERFLY, 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
   break;
   case 2:
   strcpy(info, "On the card is a picture of a little demon.");
   mpr(info);
   strcpy(info, "The picture comes to life!");
   mpr(info);
   create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 7, you.x_pos, you.y_pos, MHITYOU, 250);
   break;
   case 3:
   strcpy(info, "On the card is a picture of a demon.");
   mpr(info);
   strcpy(info, "The picture comes to life!");
   mpr(info);
   create_monster(MONS_NEQOXEC + random2(5), 21 + random2(3), 7, you.x_pos, you.y_pos, MHITYOU, 250);
   break;
   case 4:
   strcpy(info, "On the card is a picture of a huge demon.");
   mpr(info);
   strcpy(info, "The picture comes to life!");
   mpr(info);
   create_monster(MONS_NEQOXEC + random2(5), 20 + random2(3), 7, you.x_pos, you.y_pos, MHITYOU, 250);
   break;
   case 5:
   strcpy(info, "On the card is a picture of a swarm of little demons.");
   mpr(info);
   strcpy(info, "The picture comes to life!");
   mpr(info);
   create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
   create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
   create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
   create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
   create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
   create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
   create_monster(MONS_WHITE_IMP + random2(5), 21 + random2(4), 1, you.x_pos, you.y_pos, MHITYOU, 250);
   break;
   case 6:
   strcpy(info, "On the card is a picture of a huge shaggy yak.");
   mpr(info);
   strcpy(info, "The picture comes to life!");
   mpr(info);
   create_monster(MONS_DEATH_YAK, 25, 7, you.x_pos, you.y_pos, MHITYOU, 250);
   break;
   case 7:
   strcpy(info, "On the card is a picture of a huge scaly devil.");
   mpr(info);
   strcpy(info, "The picture comes to life!");
   mpr(info);
   create_monster(MONS_FIEND, 21, 7, you.x_pos, you.y_pos, MHITYOU, 250);
   break;
   case 8:
   strcpy(info, "On the card is a picture of a huge scaly dragon.");
   mpr(info);
   strcpy(info, "The picture comes to life!");
   mpr(info);
   if (random2(2) == 0)
   create_monster(MONS_DRAGON, 22, 7, you.x_pos, you.y_pos, MHITYOU, 250);
   else create_monster(MONS_ICE_DRAGON, 22, 7, you.x_pos, you.y_pos, MHITYOU, 250);
   break;
   case 9:
   strcpy(info, "On the card is a picture of a statue.");
   mpr(info);
   strcpy(info, "The picture comes to life!");
   mpr(info);
   create_monster(MONS_CLAY_GOLEM + random2(6), 21, 7, you.x_pos, you.y_pos, MHITYOU, 250);
   break;
   case 10:
   strcpy(info, "On the card is a picture of a very ugly thing.");
   mpr(info);
   strcpy(info, "The picture comes to life!");
   mpr(info);
   create_monster(MONS_VERY_UGLY_THING, 21, 7, you.x_pos, you.y_pos, MHITYOU, 250);
   break;
   case 11:
   strcpy(info, "On the card is a picture of a very irritated-looking skeletal thing.");
   mpr(info);
   strcpy(info, "The picture comes to life!");
   mpr(info);
   create_monster(MONS_LICH, 0, 1, you.x_pos, you.y_pos, MHITYOU, 250);
   break;
   case 12: // unseen horror
   if (player_see_invis() == 0)
   {
   strcat(info, "It is blank");
   mpr(info);
   } else
   {
   strcpy(info, "On the card is a picture of a hideous abomination.");
   mpr(info);
   strcpy(info, "The picture comes to life!");
   mpr(info);
   }
   create_monster(MONS_UNSEEN_HORROR, 21, 7, you.x_pos, you.y_pos, MHITYOU, 250);
   break;

   }

   you.inv_plus [you.equip [EQ_WEAPON]] --;
   if (you.inv_plus [you.equip [EQ_WEAPON]] == 0)
   {
   strcpy(info, "The deck of cards disappears in a puff of smoke.");
   mpr(info);
   unwield_item(you.equip [EQ_WEAPON]);
   you.inv_quantity [you.equip [EQ_WEAPON]] --;
   if (you.inv_quantity [you.equip [EQ_WEAPON]] == 0) // can this be false?
   {
   you.num_inv_items --;
   you.equip [EQ_WEAPON] = -1;
   strcpy(info, "You are now empty handed.");
   mpr(info);
   }
   }
   burden_change();
   } */

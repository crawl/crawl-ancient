/*
 *  File:       religion.cc
 *  Summary:    Misc religion related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *
 *   <5>   11/15/99      cdl    Fixed Daniel's yellow Xom patch  :)
 *                              Xom will sometimes answer prayers
 *   <4>   10/11/99      BCR    Added Daniel's yellow Xom patch
 *   <3>    6/13/99      BWR    Vehumet book giving code.
 *   <2>    5/20/99      BWR    Added screen redraws
 *   <1>    -/--/--      LRH    Created
 */

#include "AppHdr.h"
#include "religion.h"

#include <stdlib.h>
#include <string.h>

#include "externs.h"
#include "enum.h"

#include "bang.h"
#include "describe.h"
#include "dungeon.h"
#include "effects.h"
#include "itemname.h"
#include "it_use2.h"
#include "misc.h"
#include "monplace.h"
#include "mutation.h"
#include "ouch.h"
#include "player.h"
#include "shopping.h"
#include "spells.h"
#include "spells1.h"
#include "spells3.h"
#include "stuff.h"

char mutate(int which_mutation);


char *sacrifice[] =
{
    {" glows silver and disappears."},
    {" glows a brilliant golden colour and disappears."},
    {" rots away in an instant."},
    {" crumbles to dust."},
    {" is eaten by a bug."},    /* Xom - no sacrifices */
    {" explodes into nothingness."},
    {" is consumed in a burst of flame."},
    {" is consumed in a roaring column of flame."},
    {" glows faintly for a moment, then is gone."},
    {" is consumed in a roaring column of flame."},
    {" glows with a rainbow of weird colours and disappears."},
    {" evaporates."}
};

char *god_name(int which_god);
char *god_name_long(int which_god);
void gain_piety(char pgn);
void naughty(char type_naughty, int naughtiness);
void excommunication(void);
void god_pitch(unsigned char which_god);
void altar_prayer();
void lose_piety(char pgn);

void pray(void)
{

    unsigned char was_praying = you.duration[DUR_PRAYER];

    // all prayers take time
    you.turn_is_over = 1;

    if (you.religion != GOD_NO_GOD && grd[you.x_pos][you.y_pos] == 179 + you.religion)
    {
        altar_prayer();
    }
    else if (grd[you.x_pos][you.y_pos] >= 180 && grd[you.x_pos][you.y_pos] <= 199)
    {
        if (you.species == SP_DEMIGOD)
        {
            mpr("Sorry, a being of your status cannot worship here.");
            return;             /* demigod */
        }
        god_pitch(grd[you.x_pos][you.y_pos] - 179);
        return;
    }


    if (you.religion == GOD_NO_GOD)
    {
        if (you.is_undead != 0)
            mpr("You spend a moment contemplating the meaning of death.");
        else
            mpr("You spend a moment contemplating the meaning of life.");
        return;
    }

    if (you.religion == GOD_XOM)
    {
        if ( random2( 10 ) )
        {
          mpr("Xom ignores you.");
        }
        else
        {
          // Every now and then, Xom listens
          // This is for flavor, not effect, so praying should not be
          // encouraged.

          // Xom is nicer to experienced players  (0 is bad, 1 is nice)
          char nice  = 27 <= random2( 27 + you.experience_level ) ;

          // and he's not very nice even then
          int  sever = ( nice ) ? random2( random2( you.experience_level ) )
                                : you.experience_level;

          // bad results are enforced, good are not
          char force = ! nice;

          Xom_acts( nice, 1+sever, force );
        }
        return;
    }
    strcpy(info, "You offer a prayer to ");
    strcat(info, god_name(you.religion));
    strcat(info, ".");
    mpr(info);

    you.duration[DUR_PRAYER] = 9 + random2(you.piety) / 20 + random2(you.piety) / 20;

//if (you.char_class == 2 || you.char_class == 6) you.duration [3] = 4 + random2(you.piety) / 20 + random2(you.piety) / 20 + random2(you.piety) / 20;

    strcpy(info, god_name(you.religion));

    if (you.piety <= 5)
    {
        strcat(info, " is displeased.");
    }
    else if (you.piety <= 20)
    {
        strcat(info, " is noncommittal.");
    }
    else if (you.piety <= 40)
    {
        strcat(info, " is pleased with you.");
    }
    else if (you.piety <= 70)
    {
        strcat(info, " is most pleased with you.");
    }
    else if (you.piety <= 100)
    {
        strcat(info, " is greatly pleased with you.");
        you.duration[DUR_PRAYER] *= 2;
    }
    else if (you.piety <= 130)
    {
        strcat(info, " is extremely pleased with you.");
        you.duration[DUR_PRAYER] *= 2;
    }
    else
    {
        strcat(info, " is exalted by your worship!");
        you.duration[DUR_PRAYER] *= 3;
    }

    mpr(info);

/* itoa(you.piety, st_prn, 10);
   strcpy(info, "Debug info - Piety: ");
   strcat(info, st_prn);
   mpr(info); */


    if (was_praying == 0)
    {
/*
   Remember to check for water/lava
 */

        if (you.religion == GOD_NEMELEX_XOBEH && random2(200) <= you.piety
                    && (random2(3) == 0 || you.attribute[ATTR_CARD_TABLE] == 0)
                    && you.attribute[ATTR_CARD_COUNTDOWN] == 0
                    && grd[you.x_pos][you.y_pos] != 61
                    && grd[you.x_pos][you.y_pos] != 62)
        {
            int Nemelex = 0;
            int gift_type = MISC_DECK_OF_TRICKS;

            if (you.attribute[ATTR_CARD_TABLE] == 0)
            {
                Nemelex = items(1, OBJ_MISCELLANY,
                                MISC_PORTABLE_ALTAR_OF_NEMELEX, 1, 1, 250);
                you.attribute[ATTR_CARD_TABLE] = 1;
            }
            else
            {
                if (random2(200) <= you.piety && random2(4) == 0)
                    gift_type = MISC_DECK_OF_SUMMONINGS;
                if (random2(200) <= you.piety && random2(2) == 0)
                    gift_type = MISC_DECK_OF_WONDERS;
                if (random2(200) <= you.piety && random2(4) == 0)
                    gift_type = MISC_DECK_OF_POWER;
                Nemelex = items(1, OBJ_MISCELLANY, gift_type, 1, 1, 250);
            }

            if (you.species != SP_NAGA)
                mpr("Something appears at your feet!");
            else
                mpr("Something appears before you!");

            more();

            int what_was_theren = igrd[you.x_pos][you.y_pos];

            mitm.link[Nemelex] = what_was_theren;
            igrd[you.x_pos][you.y_pos] = Nemelex;

            you.attribute[ATTR_CARD_COUNTDOWN] = 10;
            lose_piety(5 + random2(5) + random2(5));
        }

        if ((you.religion == GOD_OKAWARU || you.religion == GOD_TROG)
                    && you.piety > 130 && random2(you.piety) > 120
                    && random2(4) == 0 && grd[you.x_pos][you.y_pos] != 61
                    && grd[you.x_pos][you.y_pos] != 62)
        {
            strcpy(info, god_name(you.religion));
            strcat(info, " grants you a gift!");
            mpr(info);
            more();
            if (you.religion == GOD_TROG
                        || (you.religion == GOD_OKAWARU && random2(2) == 0))
                acquirement(OBJ_WEAPONS);
            else
                acquirement(OBJ_ARMOUR);
            lose_piety(30 + random2(10) + random2(10));
        }

        if (you.religion == GOD_YREDELEMNUL && random2(you.piety) > 80
                                                    && random2(10) == 0)
        {
            strcpy(info, god_name(you.religion));
            strcat(info, " grants you an undead servant!");
            mpr(info);
            more();
            int und_serv = MONS_WRAITH;         /* wraith */

            if (random2(7) == 0)
                und_serv = MONS_MUMMY;
            if (random2(7) == 0)
                und_serv = MONS_VAMPIRE;
            if (random2(10) == 0)
                und_serv = MONS_FLAYED_GHOST;
            if (random2(10) == 0)
                und_serv = MONS_GHOUL;
            if (random2(7) == 0)
                und_serv = MONS_ROTTING_HULK;
            if (random2(9) == 0)
                und_serv = MONS_SKELETAL_WARRIOR;
            if (random2(7) == 0)
                und_serv = MONS_SPECTRAL_WARRIOR;
            if (random2(7) == 0)
                und_serv = MONS_WIGHT;
            create_monster(und_serv, 0, 7, you.x_pos, you.y_pos,
                                                        you.pet_target, 250);

            lose_piety(4 + random2(4) + random2(4));
        }


        if ((you.religion == GOD_KIKUBAAQUDGHA || you.religion == GOD_SIF_MUNA
                            || you.religion == GOD_VEHUMET)
                    && you.piety > 160 && random2(you.piety) > 100)
        {
            int gift = BOOK_MINOR_MAGIC_I;

            switch (you.religion) {
            case GOD_KIKUBAAQUDGHA:
                // Kiku - gives death books
                if (you.had_item[BOOK_NECROMANCY] == 0)
                    gift = BOOK_NECROMANCY;

                else if (you.had_item[BOOK_DEATH] == 0)
                    gift = BOOK_DEATH;

                else if (you.had_item[BOOK_UNLIFE] == 0)
                    gift = BOOK_UNLIFE;

                else if (you.had_item[BOOK_NECRONOMICON] == 0)
                    gift = BOOK_NECRONOMICON;

                break;

            case GOD_SIF_MUNA:
                gift = 250;     // Sif Muna - gives any
                break;

            // Vehumet - gives conj/summ. Gives bks first for whichever
            // skill is higher
            case GOD_VEHUMET:
                if (you.had_item[BOOK_CONJURATIONS_I] == 0)
                    gift = BOOK_CONJURATIONS_I;
                else if (you.had_item[BOOK_ANNIHILATIONS] == 0)
                    gift = BOOK_ANNIHILATIONS;      // conj bks

                if (you.skills[SK_CONJURATIONS] < you.skills[SK_SUMMONINGS]
                                            || gift == BOOK_MINOR_MAGIC_I)
                {
                    if (you.had_item[BOOK_SUMMONINGS] == 0)
                        gift = BOOK_SUMMONINGS;
                    else if (you.had_item[BOOK_INVOCATIONS] == 0)
                        gift = BOOK_INVOCATIONS;
                    else if (you.had_item[BOOK_DEMONOLOGY] == 0)
                        gift = BOOK_DEMONOLOGY;         // summoning bks
                }
                break;
            }

            // shouldn't give you something if it's just going to fall in a pool
            if (gift != BOOK_MINOR_MAGIC_I && (grd[you.x_pos][you.y_pos] != 61
                                        && grd[you.x_pos][you.y_pos] != 62))
            {
                strcpy(info, god_name(you.religion));
                strcat(info, " grants you a gift!");
                mpr(info);
                more();

                if (gift == 250)
                {
                    acquirement(OBJ_BOOKS);
                }
                else
                {
                    int thing_created = items(1, OBJ_BOOKS, gift, 1, 1, 250);

                    if (you.species != SP_NAGA)
                        mpr("Something appears at your feet!");
                    else
                        mpr("Something appears before you!");
                    more();
                    int what_was_there = igrd[you.x_pos][you.y_pos];

                    mitm.link[thing_created] = what_was_there;
                    igrd[you.x_pos][you.y_pos] = thing_created;
                }

                lose_piety(40 + random2(10) + random2(10));

                // Vehumet gives books less readily
                if (you.religion == GOD_VEHUMET)
                    lose_piety(10 + random2(10));
            }
        }
    }
}

char *god_name(int which_god)
{

    switch (which_god)
    {
    case GOD_NO_GOD:
        return "You aren't religious!";
    case GOD_ZIN:
        return "Zin";
    case GOD_SHINING_ONE:
        return "The Shining One";
    case GOD_KIKUBAAQUDGHA:
        return "Kikubaaqudgha";
    case GOD_YREDELEMNUL:
        return "Yredelemnul";
    case GOD_XOM:
        return "Xom";
    case GOD_VEHUMET:
        return "Vehumet";
    case GOD_OKAWARU:
        return "Okawaru";
    case GOD_MAKHLEB:
        return "Makhleb";
    case GOD_SIF_MUNA:
        return "Sif Muna";
    case GOD_TROG:
        return "Trog";
    case GOD_NEMELEX_XOBEH:
        return "Nemelex Xobeh";
    case GOD_ELYVILON:
        return "Elyvilon";
    }

    return "Illegal god";

}


char *god_name_long(int which_god)
{

    switch (which_god)
    {
    case GOD_NO_GOD:
        return "Nobody";
    case GOD_ZIN:
        return "Zin the Law-Giver";
    case GOD_SHINING_ONE:
        return "The Shining One";
    case GOD_KIKUBAAQUDGHA:
        return "Kikubaaqudgha";
    case GOD_YREDELEMNUL:
        return "Yredelemnul";
    case GOD_XOM:
        return "Xom of Chaos";
    case GOD_VEHUMET:
        return "Vehumet";
    case GOD_OKAWARU:
        return "Okawaru";
    case GOD_MAKHLEB:
        return "Makhleb";
    case GOD_SIF_MUNA:
        return "Sif Muna";
    case GOD_TROG:
        return "Trog";
    case GOD_NEMELEX_XOBEH:
        return "Nemelex Xobeh";
    case GOD_ELYVILON:
        return "Elyvilon the Healer";
    }

    return "Illegal God";

}


void Xom_acts(char niceness, int sever, char force_sever)
{

/*
   niceness = 0 - bad, 1 - nice
 */

    struct bolt beam[1];

    if (sever <= 1)
        sever = 1;

    if (force_sever == 0)
        sever = random2(sever);

    if (sever == 0)
        return;

#ifdef XOM_ACTS_YELLOW
    set_colour( YELLOW );
#endif

okay_try_again:
    if (niceness == 0 || random2(3) == 0)
    {
        /* bad things */

        // this should always be first - it will often be called
        // deliberately, with a low sever value
        if (random2(sever) <= 2)
        {
            switch (random2(4))
            {
            case 0:
                mpr("Xom notices you.");
                break;
            case 1:
                mpr("Xom's attention turns to you for a moment.");
                break;
            case 2:
                mpr("Xom's power touches on you for a moment.");
                break;
            case 3:
                mpr("You hear Xom's maniacal laughter.");
                break;
            }
            miscast_effect(10 + random2(15), random2(10) + 5, random2(100), 0);
            return;
        }

        if (random2(sever) <= 2)
        {
            switch (random2(4))
            {
            case 0:
                mpr("\"Suffer!\"");
                break;
            case 1:
                mpr("Xom's malign attention turns to you for a moment.");
                break;
            case 2:
                mpr("Xom's power touches on you for a moment.");
                break;
            case 3:
                mpr("You hear Xom's maniacal laughter.");
                break;
            }
            lose_stat(100, 1 + random2(3));
            return;
        }

        if (random2(sever) <= 2)
        {
            switch (random2(4))
            {
            case 0:
                mpr("Xom notices you.");
                break;
            case 1:
                mpr("Xom's attention turns to you for a moment.");
                break;
            case 2:
                mpr("Xom's power touches on you for a moment.");
                break;
            case 3:
                mpr("You hear Xom's maniacal laughter.");
                break;
            }
            miscast_effect(10 + random2(15), random2(15) + 5, random2(250), 0);
            return;
        }

        if (random2(sever) <= 3 && you.is_undead == 0)
        {
            switch (random2(4))
            {
            case 0:
                mpr("\"You need some minor adjustments, mortal!\"");
                break;
            case 1:
                mpr("\"Let me alter your pitiful body.\"");
                break;
            case 2:
                mpr("Xom's power touches on you for a moment.");
                break;
            case 3:
                mpr("You hear Xom's maniacal laughter.");
                break;
            }
            mpr("Your body is suffused with distortional energy.");
            you.hp = random2(you.hp) + 1;
            if (you.hp < you.hp_max / 2)
                you.hp = you.hp_max / 2;
            you.redraw_hit_points = 1;
            if (mutate(100) == 0)
                if (mutate(100) == 0)
                    if (mutate(100) == 0)
                        if (mutate(100) == 0)
                            mpr("Nothing seems to happen.");
            return;
        }

        if (random2(sever) <= 3 && you.is_undead == 0)
        {
            switch (random2(4))
            {
            case 0:
                mpr("\"You have displeased me, mortal.\"");
                break;
            case 1:
                mpr("\"You have grown too confident for your meagre worth.\"");
                break;
            case 2:
                mpr("Xom's power touches on you for a moment.");
                break;
            case 3:
                mpr("You hear Xom's maniacal laughter.");
                break;
            }
            if (random2(4) == 0)
            {
                drain_exp();
                if (random2(sever) > 3)
                    drain_exp();
                if (random2(sever) > 3)
                    drain_exp();
                return;
            }
            else
            {
                mpr("A wave of agony tears through your body!");
                you.hp = you.hp / 2 + 1;
                you.redraw_hit_points = 1;
            }
        }

        if (random2(sever) <= 3)
        {
            switch (random2(3))
            {
            case 0:
                mpr("\"Time to have some fun!\"");
                break;
            case 1:
                mpr("\"Fight to survive, mortal.\"");
                break;
            case 2:
                mpr("You hear Xom's maniacal laughter.");
                break;
            }
            if (random2(4) != 0)
            {
                create_monster(MONS_NEQOXEC + random2(5), 22, 1,
                                        you.x_pos, you.y_pos, MHITNOT, 250);
                if (random2(3) == 0)
                    create_monster(MONS_NEQOXEC + random2(5), 22, 1,
                                        you.x_pos, you.y_pos, MHITNOT, 250);

                if (random2(4) == 0)
                    create_monster(MONS_NEQOXEC + random2(5), 22, 1,
                                        you.x_pos, you.y_pos, MHITNOT, 250);

                if (random2(3) == 0)
                    create_monster(MONS_HELLION + random2(10), 22, 1,
                                        you.x_pos, you.y_pos, MHITNOT, 250);

                if (random2(4) == 0)
                    create_monster(MONS_HELLION + random2(10), 22, 1,
                                        you.x_pos, you.y_pos, MHITNOT, 250);
            }
            else
                dancing_weapon(100, 1);         /* nasty, but fun */

            return;
        }

        /* And, finally: (remember, miscast_effect can cause this) */
        if (you.your_level == 0)
        {
            switch (random2(3))
            {
            case 0:
                mpr("\"You have grown too comfortable in your little world, mortal!\"");
                break;
            case 1:
                mpr("Xom casts you into the Abyss!");
                break;
            case 2:
                mpr("The world seems to spin as Xom's maniacal laughter rings in your ears.");
                break;
            }
            banished(96);
            return;
        }

        if (random2(4) != 0)
            goto okay_try_again;

        mpr("You hear Xom's maniacal laughter.");
        return;
    }
/* ouch(x, 19, y); for killed by Xom message */

/* Okay, now for the nicer stuff (note: these things are not necess nice): */
    if (random2(sever) <= 2)
    {
        switch (random2(4))
        {
        case 0:
            mpr("\"Go forth and destroy!\"");
            break;
        case 1:
            mpr("\"Go forth and destroy, mortal!\"");
            break;
        case 2:
            mpr("Xom grants you a minor favour.");
            break;
        case 3:
            mpr("Xom smiles on you.");
            break;
        }

        switch (random2(7))
        {
        case 0:
            potion_effect(POT_HEALING, 150);
            break;
        case 1:
            potion_effect(POT_HEAL_WOUNDS, 150);
            break;
        case 2:
            potion_effect(POT_SPEED, 150);
            break;
        case 3:
            potion_effect(POT_MIGHT, 150);
            break;
        case 4:
            potion_effect(POT_INVISIBILITY, 150);
            break;
        case 5:
            if (random2(6) == 0)
            {
                potion_effect(POT_EXPERIENCE, 150);
            }
            else
            {
                you.berserk_penalty = NO_BERSERK_PENALTY;
                potion_effect(POT_BERSERK_RAGE, 150);
            }
            break;
        case 6:
            you.berserk_penalty = NO_BERSERK_PENALTY;
            potion_effect(POT_BERSERK_RAGE, 150);
            break;
        }
        return;
    }

    if (random2(sever) <= 4)
    {
        switch (random2(3))
        {
        case 0:
            mpr("\"Serve the mortal, my children!\"");
            break;
        case 1:
            mpr("Xom grants you some temporary aid.");
            break;
        case 2:
            mpr("Xom opens a gate.");
            break;
        }
        create_monster(MONS_NEQOXEC + random2(5), 22, 7, you.x_pos, you.y_pos, you.pet_target, 250);
        create_monster(MONS_NEQOXEC + random2(5), 22, 7, you.x_pos, you.y_pos, you.pet_target, 250);
        if (random2(you.experience_level) >= 8)
            create_monster(MONS_NEQOXEC + random2(5), 22, 7, you.x_pos, you.y_pos, you.pet_target, 250);
        if (random2(you.experience_level) >= 8)
            create_monster(MONS_HELLION + random2(10), 22, 7, you.x_pos, you.y_pos, you.pet_target, 250);
        if (random2(you.experience_level) >= 8)
            create_monster(MONS_HELLION + random2(10), 22, 7, you.x_pos, you.y_pos, you.pet_target, 250);
        return;
    }

    if (random2(sever) <= 3)
    {
        switch (random2(3))
        {
        case 0:
            mpr("\"Take this token of my esteem.\"");
            break;
        case 1:
            mpr("Xom grants you a gift!");
            break;
        case 2:
            mpr("Xom's generous nature manifests itself.");
            break;
        }
        if (grd[you.x_pos][you.y_pos] == 61 || grd[you.x_pos][you.y_pos] == 62)
        {
            mpr("You hear a splash.");  /* How unfortunate. I'll bet Xom feels sorry for you. */
            return;
        }
        int thing_created = items(1, 250, 250, 1, you.experience_level * 3, 250);

        if (you.species != SP_NAGA)
            mpr("Something appears at your feet!");
        else
            mpr("Something appears before you!");
        int what_was_there = igrd[you.x_pos][you.y_pos];

        mitm.link[thing_created] = what_was_there;
        igrd[you.x_pos][you.y_pos] = thing_created;
        return;
    }

    if (random2(sever) <= 4)
    {
        switch (random2(3))
        {
        case 0:
            mpr("\"Serve the mortal, my child!\"");
            break;
        case 1:
            mpr("Xom grants you a demonic servitor.");
            break;
        case 2:
            mpr("Xom opens a gate.");
            break;
        }
        if (random2(you.experience_level) <= 5)
            create_monster(MONS_WHITE_IMP + random2(5), 0, 7, you.x_pos, you.y_pos, you.pet_target, 250);
        else
            create_monster(MONS_NEQOXEC + random2(5), 0, 7, you.x_pos, you.y_pos, you.pet_target, 250);
        return;
    }

    if (random2(sever) <= 4)
    {
        switch (random2(4))
        {
        case 0:
            mpr("\"Take this instrument of destruction!\"");
            break;
        case 1:
            mpr("\"You have earned yourself a gift.\"");
            break;
        case 2:
            mpr("Xom grants you an implement of death.");
            break;
        case 3:
            mpr("Xom smiles on you.");
            break;
        }
        acquirement(OBJ_WEAPONS);
        return;
    }

    if (random2(sever) <= 5 && you.is_undead == 0)
    {
        switch (random2(4))
        {
        case 0:
            mpr("\"You need some minor adjustments, mortal!\"");
            break;
        case 1:
            mpr("\"Let me alter your pitiful body.\"");
            break;
        case 2:
            mpr("Xom's power touches on you for a moment.");
            break;
        case 3:
            mpr("You hear Xom's maniacal chuckling.");
            break;
        }
        mpr("Your body is suffused with distortional energy.");
        you.hp = random2(you.hp) + 1;
        if (you.hp < you.hp_max / 2)
            you.hp = you.hp_max / 2;
        you.redraw_hit_points = 1;
        give_good_mutation();
        return;
    }

    if (random2(sever) <= 2)
    {
        you.attribute[ATTR_DIVINE_LIGHTNING_PROTECTION] = 1;
        mpr("Xom hurls a blast of lightning!");
        beam[0].beam_source = MNG;
        beam[0].type = 43;
        beam[0].damage = 130;
        beam[0].flavour = 5;
        beam[0].bx = you.x_pos;
        beam[0].by = you.y_pos;
        strcpy(beam[0].beam_name, "blast of lightning");
        beam[0].colour = LIGHTCYAN;
        beam[0].thing_thrown = 1;       /* your explosion */
        explosion(1, beam);
        you.attribute[ATTR_DIVINE_LIGHTNING_PROTECTION] = 0;
        return;
    }

    if (random2(4) != 0)
        goto okay_try_again;

    mpr("You hear Xom's maniacal laughter.");

}


/*
   Thing_done:
   1 - killed a living monster in god's name
   2 - killed an undead in god's name
   3 - killed a demon in god's name
   4 - killed an angel (any time)
   5 - killed an angel in god's name (all of these pass HD of monster as pgain)
   6 - hacked up a corpse in god's name
   7 - offered inanimate stuff at an altar
   8 - offered stuff, including at least one corpse at an altar
   9 - undead slaves killed a living thing
   10 - any servants kill anything
   11 - cards (Nemelex)
 */
void done_good(char thing_done, int pgain)
{

    if (you.religion == GOD_NO_GOD)
        return;

    switch (thing_done)
    {
    case GOOD_KILLED_LIVING:    /* killed a living monster in god's name */
        switch (you.religion)
        {
        case GOD_ELYVILON:
            mpr("Elyvilon did not appreciate that!");
            naughty(NAUGHTY_KILLING, 10);
            break;
        case GOD_KIKUBAAQUDGHA:
        case GOD_YREDELEMNUL:
        case GOD_VEHUMET:
        case GOD_OKAWARU:
        case GOD_MAKHLEB:
        case GOD_TROG:
            strcpy(info, god_name(you.religion));
            strcat(info, " accepts your kill.");
            mpr(info);
            if (random2(pgain + 18) > 5)
                gain_piety(1);
            break;
        }
        break;

    case GOOD_KILLED_UNDEAD:    /* killed an undead in god's name */
        switch (you.religion)
        {
        case GOD_ZIN:
        case GOD_SHINING_ONE:
        case GOD_VEHUMET:
        case GOD_MAKHLEB:
            strcpy(info, god_name(you.religion));
            strcat(info, " accepts your kill.");
            mpr(info);
            if (random2(pgain + 18) > 4)
                gain_piety(1);
            break;
        }
        break;

    case GOOD_KILLED_DEMON:     /* killed a demon in god's name */
        switch (you.religion)
        {
        case GOD_ZIN:
        case GOD_SHINING_ONE:
        case GOD_VEHUMET:
        case GOD_MAKHLEB:
            strcpy(info, god_name(you.religion));
            strcat(info, " accepts your kill.");
            mpr(info);
            if (random2(pgain + 18) > 3)
                gain_piety(1);
            break;
        }
        break;

    case GOOD_KILLED_ANGEL_I:
    case GOOD_KILLED_ANGEL_II:
        switch (you.religion)
        {
            case GOD_ZIN:
            case GOD_SHINING_ONE:
        case GOD_ELYVILON:
            strcpy(info, god_name(you.religion));
            strcat(info, " did not appreciate that!");
            mpr(info);
            if (you.conf != 0)
                naughty(NAUGHTY_ATTACK_HOLY, 3);
            else
                naughty(NAUGHTY_ATTACK_HOLY, pgain * 3);
            break;
        }
        break;

    case GOOD_HACKED_CORPSE:    /* Note - pgain is you.experience_level, maybe */
        switch (you.religion)
        {
        case GOD_KIKUBAAQUDGHA:
        case GOD_OKAWARU:
        case GOD_MAKHLEB:
        case GOD_TROG:
            strcpy(info, god_name(you.religion));
            strcat(info, " accepts your offering.");
            mpr(info);
            if (random2(pgain + 10) > 5)
                gain_piety(1);
            break;
        case GOD_ZIN:
        case GOD_SHINING_ONE:
        case GOD_ELYVILON:
            strcpy(info, god_name(you.religion));
            strcat(info, " did not appreciate that!");
            mpr(info);
            naughty(NAUGHTY_BUTCHER, 8);
            break;
        }
        break;

    case GOOD_OFFER_STUFF:
        strcpy(info, god_name(you.religion));
        strcat(info, " is pleased with your offering.");
        mpr(info);
        gain_piety(1);
        break;

    case GOOD_SLAVES_KILL_LIVING:       /* undead slaves killed a living monster */
        switch (you.religion)
        {
        case GOD_KIKUBAAQUDGHA:
        case GOD_YREDELEMNUL:
        case GOD_VEHUMET:
            strcpy(info, god_name(you.religion));
            strcat(info, " accepts your slave's kill.");
            mpr(info);
            if (random2(pgain + 18) > 5)
                gain_piety(1);
            break;
        }
        break;

    case GOOD_SERVANTS_KILL:    /* servants killed any monster */
        switch (you.religion)
        {
        case GOD_VEHUMET:
        case GOD_MAKHLEB:
            strcpy(info, god_name(you.religion));
            strcat(info, " accepts your collateral kill.");
            mpr(info);
            if (random2(pgain + 18) > 5)
                gain_piety(1);
            break;
        }
        break;

    case GOOD_CARDS:            /* cards */
        switch (you.religion)
        {
        case GOD_NEMELEX_XOBEH:
            gain_piety(pgain);
            break;
        }
        break;

/*
   Offering at altars is covered in another function.
 */

    }

}

void gain_piety(char pgn)
{

    if (you.piety > 100 && random2(3) == 0)
        return;
    if (you.piety > 150 && random2(3) == 0)
        return;
    if (you.piety > 199)
        return;

    int old_piety = you.piety;

    you.piety += pgn;

    if (you.piety >= 30 && old_piety < 30)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
        case GOD_SHINING_ONE:
            mpr("You can now repel the undead.");
            break;
        case GOD_KIKUBAAQUDGHA:
            mpr("You can now recall your undead servants at will.");
            break;
        case GOD_YREDELEMNUL:
            mpr("You can now animate corpses.");
            break;
        case GOD_VEHUMET:
            mpr("You can gain power by killing in Vehumet's name, or by your servants' kills.");
            break;
        case GOD_OKAWARU:
            mpr("You can now give your body great, if temporary, strength.");
            break;
        case GOD_MAKHLEB:
            mpr("You can now gain power by killing in Makhleb's name.");
            break;
        case GOD_TROG:
            mpr("You can now go berserk at will.");
            break;
        case GOD_ELYVILON:
            mpr("You now have minor healing powers.");
            break;
        }
    }

    if (you.piety >= 50 && old_piety < 50)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
            mpr("You can now call upon Zin for minor healing.");
            break;
        case GOD_SHINING_ONE:
            mpr("You can now smite your foes.");
            break;
        case GOD_KIKUBAAQUDGHA:
            mpr("You now have some protection from the side-effects of necromantic magic.");
            break;
        case GOD_YREDELEMNUL:
            mpr("You can now recall your undead slaves.");
            break;
        case GOD_VEHUMET:
            mpr("Your destructive magics cast in Vehumet's name will rarely fail.");
            break;
        case GOD_OKAWARU:
            mpr("You can now call upon Okawaru for minor healing.");
            break;
        case GOD_MAKHLEB:
            mpr("You can now call on Makhleb's destructive powers.");
            break;
        case GOD_SIF_MUNA:
            mpr("You can now forget spells at will.");
            break;
        case GOD_TROG:
            mpr("You can now give your body great, if temporary, strength.");
            break;
        case GOD_ELYVILON:
            mpr("You now have the power to cure poison, disease and decay.");
            break;
        }
    }

    if (you.piety >= 75 && old_piety < 75)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
            mpr("You can now call a plague of pestilential beasts upon your enemies!");
            break;
        case GOD_SHINING_ONE:
            mpr("You can now dispel the undead.");
            break;
        case GOD_KIKUBAAQUDGHA:
            mpr("You can now permanently enslave the undead.");
            break;
        case GOD_YREDELEMNUL:
            mpr("You can now animate whole legions of the undead.");
            break;
        case GOD_VEHUMET:
            mpr("During prayer you are protected from summoned creatures.");
            break;
        case GOD_MAKHLEB:
            mpr("You can now summon a lesser servant of Makhleb.");
            break;
        case GOD_ELYVILON:
            mpr("You now have more powerful healing abilities.");
            break;
        }
    }

    if (you.piety >= 100 && old_piety < 100)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
            mpr("You can now speak a Holy Word.");
            break;
        case GOD_SHINING_ONE:
            mpr("You can now hurl bolts of divine anger.");
            break;
        case GOD_YREDELEMNUL:
            mpr("You can now drain life from all living things around you.");
            break;
        case GOD_VEHUMET:
            mpr("You can channel ambient magical energy for your own uses.");
            break;
        case GOD_MAKHLEB:
            mpr("You can now call on the greater of Makhleb's destructive powers.");
            break;
        case GOD_SIF_MUNA:
            mpr("You now have some protection from the side-effects of magic.");
            break;
        case GOD_TROG:
            mpr("You can now haste yourself.");
            break;
        case GOD_ELYVILON:
            mpr("You have gained the power of Restoration.");
            break;
        }
    }

    if (you.piety >= 120 && old_piety < 120)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
            mpr("You can now summon a guardian angel.");
            break;
        case GOD_SHINING_ONE:
            mpr("You can now summon a divine warrior!");
            break;
        case GOD_KIKUBAAQUDGHA:
            mpr("You can now summon an emissary of Death!");    /* ie a reaper */
            break;
        case GOD_YREDELEMNUL:
            mpr("You can now control the undead.");
            break;
        case GOD_OKAWARU:
            mpr("You can now haste yourself.");
            break;
        case GOD_MAKHLEB:
            mpr("You can now summon a greater servant of Makhleb.");
            break;
        case GOD_ELYVILON:
            mpr("You now have incredible healing powers.");
            break;
        }
    }

}

/*

 */
void naughty(char type_naughty, int naughtiness)
{
/*
   1 = using necromancy (spell or device)
   2 = using unholy stuff (eg call imp, summon horrible things)
   3 = killing in the name of peaceful diety
   4 = attacking holy things
   5 = attacking friendly
   6 = allowing friendly to die
   7 = butchering in the name of peaceful diety
   8 = stabbing
   9 = spellcasting (Trog hates this)
 */

    int piety_loss = 0;

    switch (type_naughty)
    {
    case NAUGHTY_NECROMANCY:
    case NAUGHTY_UNHOLY:
    case NAUGHTY_ATTACK_HOLY:
        switch (you.religion)
        {
        case GOD_ZIN:
        case GOD_SHINING_ONE:
        case GOD_ELYVILON:
            piety_loss = naughtiness;
            break;
        }
        break;

    case NAUGHTY_KILLING:
        switch (you.religion)
        {
        case GOD_ELYVILON:
            piety_loss = naughtiness;
            break;
        }
        break;

    case NAUGHTY_ATTACK_FRIEND:
    case NAUGHTY_FRIEND_DIES:
        switch (you.religion)
        {
        case GOD_ZIN:
        case GOD_SHINING_ONE:
        case GOD_ELYVILON:
        case GOD_OKAWARU:
            piety_loss = naughtiness;
            break;
        }
        break;

    case NAUGHTY_BUTCHER:       /* butchering in the name of */
        switch (you.religion)
        {
        case GOD_ZIN:
        case GOD_SHINING_ONE:
        case GOD_ELYVILON:
            piety_loss = naughtiness;
            break;
        }
        break;

    case NAUGHTY_STABBING:      /* stabbing */
        switch (you.religion)
        {
        case GOD_SHINING_ONE:
            piety_loss = naughtiness;
            break;
        }
        break;

    case NAUGHTY_SPELLCASTING:  /* spellcasting */
        switch (you.religion)
        {
        case GOD_TROG:
            piety_loss = naughtiness;
            break;
        }
        break;


    }

    if (piety_loss == 0)
        return;

    if (piety_loss == 1)
        mpr("You feel a little guilty.");
    else if (piety_loss <= 4)
        mpr("You feel guilty.");
    else if (piety_loss <= 9)
        mpr("You feel very guilty.");
    else
        mpr("You feel extremely guilty.");

    lose_piety(piety_loss);

    if (you.piety <= 0)
        excommunication();

}


void lose_piety(char pgn)
{

    int old_piety = you.piety;

    if (you.piety - pgn < 0)
        you.piety = 0;
    else
        you.piety -= pgn;

    if (you.piety < 120 && old_piety >= 120)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
            mpr("You can no longer summon a guardian angel.");
            break;
        case GOD_SHINING_ONE:
            mpr("You can no longer summon a divine warrior.");
            break;
        case GOD_KIKUBAAQUDGHA:
            mpr("You can no longer summon an emissary of Death.");      /* ie a reaper */
            break;
        case GOD_YREDELEMNUL:
            mpr("You can no longer control the undead.");
            break;
        case GOD_MAKHLEB:
            mpr("You can no longer summon a greater servant of Makhleb.");
            break;
        case GOD_ELYVILON:
            mpr("You no longer have incredible healing powers.");
            break;
        }
    }

    if (you.piety < 100 && old_piety >= 100)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
            mpr("You can no longer speak a Holy Word.");
            break;
        case GOD_SHINING_ONE:
            mpr("You can no longer hurl bolts of divine anger.");
            break;
        case GOD_YREDELEMNUL:
            mpr("You can no longer drain life.");
            break;
        case GOD_VEHUMET:
            mpr("You have lost your channelling abilities.");
            break;
        case GOD_MAKHLEB:
            mpr("You can no longer call on Makhleb's greater destructive powers.");
            break;
        case GOD_SIF_MUNA:
            mpr("You no longer have special protection from the side-effects of magic.");
            break;
        case GOD_TROG:
            mpr("You can no longer haste yourself.");
            break;
        case GOD_ELYVILON:
            mpr("You have lost the power of Restoration.");
            break;
        }
    }


    if (you.piety < 75 && old_piety >= 75)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
            mpr("You can no longer call a plague.");
            break;
        case GOD_SHINING_ONE:
            mpr("You can no longer dispel the undead.");
            break;
        case GOD_KIKUBAAQUDGHA:
            mpr("You can no longer permanently enslave the undead.");
            break;
        case GOD_YREDELEMNUL:
            mpr("You can no longer animate legions of the dead.");
            break;
        case GOD_VEHUMET:
            mpr("You feel vulnerable.");
            break;
        case GOD_MAKHLEB:
            mpr("You can no longer summon any servants of Makhleb.");
            break;
        case GOD_ELYVILON:
            mpr("You lose your powerful healing abilities.");
            break;
        }
    }


    if (you.piety < 50 && old_piety >= 50)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
            mpr("You can no longer call upon Zin for minor healing.");
            break;
        case GOD_SHINING_ONE:
            mpr("You can no longer smite your foes.");
            break;
        case GOD_KIKUBAAQUDGHA:
            mpr("You no longer have any special protection from miscast necromancy.");
            break;
        case GOD_YREDELEMNUL:
            mpr("You can no longer recall your undead slaves.");
            break;
        case GOD_VEHUMET:
            mpr("You feel fallible.");
            break;
        case GOD_MAKHLEB:
            mpr("You can no longer call on Makhleb's destructive powers.");
            break;
        case GOD_SIF_MUNA:
            mpr("You can no longer forget spells at will.");
            break;
        case GOD_TROG:
            mpr("You can no longer give your body strength.");
            break;
        case GOD_ELYVILON:
            mpr("You have lost the power of Purification.");
            break;
        }
    }


    if (you.piety < 30 && old_piety >= 30)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
        case GOD_SHINING_ONE:
            mpr("You can no longer repel the undead.");
            break;
        case GOD_KIKUBAAQUDGHA:
            mpr("You can no longer recall your undead servants at will.");
            break;
        case GOD_YREDELEMNUL:
            mpr("You can no longer animate corpses.");
            break;
        case GOD_VEHUMET:
        case GOD_MAKHLEB:
            mpr("You can no longer gain power from your kills.");
            break;
        case GOD_TROG:
            mpr("You can no longer go berserk at will.");
            break;
        case GOD_ELYVILON:
            mpr("You have lost your minor healing powers.");
            break;
        }
    }


}




void excommunication(void)
{
    mpr("You have lost your religion!");
    more();
    if (you.religion == GOD_XOM)
        Xom_acts(0, you.experience_level * 2, 1);
    if (you.religion == GOD_KIKUBAAQUDGHA || you.religion == GOD_YREDELEMNUL)
    {
        strcpy(info, god_name(you.religion));
        strcat(info, " does not appreciate desertion!");
        mpr(info);
        miscast_effect(SPTYP_NECROMANCY, 5 + you.experience_level, random2(30) + random2(30) + random2(30), 100);
    }
    if (you.religion == GOD_VEHUMET || you.religion == GOD_MAKHLEB)
    {
        strcpy(info, god_name(you.religion));
        strcat(info, " does not appreciate desertion!");
        mpr(info);
        miscast_effect(SPTYP_CONJURATION + (random2(2) * 7), 8 + you.experience_level, random2(40) + random2(30) + random2(30), 100);
    }
    if (you.religion == GOD_TROG)
    {
        strcpy(info, god_name(you.religion));
        strcat(info, " does not appreciate desertion!");
        mpr(info);
        miscast_effect(SPTYP_FIRE, 8 + you.experience_level, random2(40) + random2(30) + random2(30), 100);
    }                           /* Trog uses fire wild magic - is this right? */
    you.religion = GOD_NO_GOD;
    you.piety = 0;
}

void altar_prayer(void)
{

    int item_sacr;
    int i, j;
    char subst_id[4][50];


    item_sacr = 501;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 50; j++)
        {
            subst_id[i][j] = 1;
        }
    }

    if (igrd[you.x_pos][you.y_pos] == 501 || you.religion == GOD_SHINING_ONE || you.religion == GOD_XOM)
    {
        mpr("You kneel at the altar and pray.");
        return;
    }

    mpr("You kneel at the altar and pray.");

    i = igrd[you.x_pos][you.y_pos];

    do
    {
        if (random2(1000) == 0)
            break;
        j = mitm.link[i];
        switch (you.religion)
        {
        case GOD_ZIN:
        case GOD_OKAWARU:
        case GOD_MAKHLEB:
        case GOD_NEMELEX_XOBEH:
            it_name(i, 0, str_pass);
            strcpy(info, str_pass);
            strcat(info, sacrifice[you.religion - 1]);
            mpr(info);
            if (mitm.base_type[i] == OBJ_CORPSES || random2(item_value(mitm.base_type[i], mitm.sub_type[i], mitm.special[i], mitm.pluses[i], mitm.pluses2[i], mitm.quantity[i], 3, subst_id)) >= 50)
                gain_piety(1);
            destroy_item(i);
            break;

        case GOD_SIF_MUNA:
            it_name(i, 0, str_pass);
            strcpy(info, str_pass);
            strcat(info, sacrifice[you.religion - 1]);
            mpr(info);
            if (item_value(mitm.base_type[i], mitm.sub_type[i], mitm.special[i], mitm.pluses[i], mitm.pluses2[i], mitm.quantity[i], 3, subst_id) >= 150)
                gain_piety(1 + random2(3));
            destroy_item(i);
            break;

        case GOD_KIKUBAAQUDGHA:
        case GOD_TROG:
            if (mitm.base_type[i] != OBJ_CORPSES)
                break;
            it_name(i, 0, str_pass);
            strcpy(info, str_pass);
            strcat(info, sacrifice[you.religion - 1]);
            mpr(info);
            gain_piety(1);
            destroy_item(i);
            break;

        case GOD_ELYVILON:
            if (mitm.base_type[i] != OBJ_WEAPONS && mitm.base_type[i] != OBJ_MISSILES)
                break;
            it_name(i, 0, str_pass);
            strcpy(info, str_pass);
            strcat(info, sacrifice[you.religion - 1]);
            mpr(info);

            if (random2(item_value(mitm.base_type[i], mitm.sub_type[i],
                            mitm.special[i], mitm.pluses[i], mitm.pluses2[i],
                            mitm.quantity[i], 3, subst_id)) >= random2(50))
            {
                gain_piety(1);
            }
            destroy_item(i);
            break;

        }
        i = j;
    }
    while (i != 501);

}



void god_pitch(unsigned char which_god)
{

    strcpy(info, "You kneel at the altar of ");
    strcat(info, god_name(which_god));
    strcat(info, ".");
    mpr(info);

    more();

    if ((you.is_undead != 0 || you.species == SP_DEMONSPAWN) && (which_god == GOD_ZIN || which_god == GOD_SHINING_ONE || which_god == GOD_ELYVILON))
    {
        strcpy(info, god_name(which_god));
        strcat(info, " does not accept worship from those such as you!");
        mpr(info);
        return;
    }

    describe_god(which_god);

    mpr("Do you wish to join this religion?");
    char keyi = get_ch();

    if (keyi != 'Y' && keyi != 'y')
    {
#ifdef PLAIN_TERM
        redraw_screen();
#endif
        return;
    }

    mpr("Are you sure?");
    keyi = get_ch();
    if (keyi != 'Y' && keyi != 'y')
    {
#ifdef PLAIN_TERM
        redraw_screen();
#endif
        return;
    }


    if (you.religion != GOD_NO_GOD)
        excommunication();

    strcpy(info, god_name(which_god));
    strcat(info, " welcomes you!");
    mpr(info);
    more();
#ifdef PLAIN_TERM
    redraw_screen();
#endif

    you.religion = which_god;
    you.piety = 15;

}


void offer_corpse(int corpse)
{
    it_name(corpse, 0, str_pass);
    strcpy(info, str_pass);
    strcat(info, sacrifice[you.religion - 1]);
    mpr(info);
    done_good(GOOD_HACKED_CORPSE, 10);
}

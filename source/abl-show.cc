/*
 *  File:       abl-show.cc
 *  Summary:    Functions related to special abilities.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *  <6>    19mar2000     jmf    added elvish Glamour
 *  <5>     11/06/99     cdl    reduced power of minor destruction
 *
 *  <4>      9/25/99     cdl    linuxlib -> liblinux
 *
 *  <3>      5/20/99     BWR    Now use scan_randarts to
 *                              check for flags, rather than
 *                              only checking the weapon.
 *
 *  <2>      5/20/99     BWR    Extended screen line support
 *
 *  <1>      -/--/--     LRH             Created
 */

#include "AppHdr.h"
#include "abl-show.h"

#include <string.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "beam.h"
#include "effects.h"
#include "food.h"
#include "it_use2.h"
#include "message.h"
#include "misc.h"
#include "monplace.h"
#include "player.h"
#include "religion.h"
#include "skills.h"
#include "skills2.h"
#include "spell.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "spells4.h"
#include "stuff.h"
#include "transfor.h"
#include "view.h"

#ifdef MACROS
#include "macro.h"
#endif

#ifdef LINUX
#include "liblinux.h"
#endif

//jmf: brent sez:
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason ...
#ifdef USE_CURSES
#include <curses.h>
#endif

// this all needs to be split into data/util/show files
// and the struct mechanism here needs to be rewritten (again)
// along with the display routine to piece the strings
// together dynamically ... I'm getting to it now {dlb}

// it makes more sense to think of them as an array
// of structs than two arrays that share common index
// values -- well, doesn't it? {dlb}
struct talent
{
    int which;
    int fail;
    bool is_invocation;
};


static char show_abilities(struct talent *p_abils);
static bool generate_abilities(struct talent *p_abils);
static bool insert_ability(int which_ability, struct talent **p_pabils);

/*
   Activates a menu which gives player access to all of their non-spell
   special abilities - Eg naga's spit poison, or the Invocations you get
   from worshipping. Generated dynamically - the function checks to see which
   abilities you have every time.
 */
void activate_ability(void)
/*************************/
{
    int loopy = 0;              // general purpose looping variable {dlb}

    // see elsewhere for comments - 80 is *way* generous here {dlb}
    FixedVector < talent, 80 > abil_now;

    unsigned char keyin = 0;
    char unthing = 0;
    unsigned char spc, spc2;

    int power;
    struct dist abild;
    struct bolt beam;
    struct dist spd;

    unsigned char abil_used;

    // early returns prior to generation of ability list {dlb}:
    if (you.conf)
    {
        mpr("You're too confused!");
        return;
    }

    if (you.berserker)
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }

    // fill array of structs with "empty" values {dlb}:
    for (loopy = 0; loopy < 80; loopy++)
    {
        abil_now[loopy].which = -1;
        abil_now[loopy].fail = -1;
        abil_now[loopy].is_invocation = false;
    }

    // populate the array of structs {dlb}:
    if (!generate_abilities(&abil_now[0]))
    {
        mpr("Sorry, you're not good enough to have a special ability.");
        return;
    }

    for (;;)
    {
        mpr("Use which ability? (? or * to list)", MSGCH_PROMPT);

        keyin = get_ch();

        if (keyin == '?' || keyin == '*')
        {
            unthing = show_abilities(&abil_now[0]);

            redraw_screen();

            if (unthing == 2)
                return;

            if ((unthing >= '@' && unthing <= '[')
                || (unthing >= 'a' && unthing <= 'z'))
            {
                keyin = unthing;
                break;
            }
            else
            {
                mesclr();
            }
        }
        else
        {
            break;
        }
    }


    spc = (int) keyin;

    // shouldn't that 'y' be a 'z' ??? {dlb}
    if ((spc < 'a' || spc > 'y') && (spc < 'A' || spc > 'Z'))
    {
        mpr("You can't do that.");
        return;
    }

    spc2 = letter_to_index(spc);

    if (abil_now[spc2].which == -1)
    {
        mpr("You can't do that.");
        return;
    }

    abil_used = spc2;

    // some abilities don't need a hunger check
    bool hungerCheck = true;
    switch (abil_now[abil_used].which)
    {
        case ABIL_RENOUNCE_RELIGION:
        case ABIL_STOP_LEVITATING:
        case ABIL_TURN_VISIBLE:
        case ABIL_END_TRANSFORMATION:
            hungerCheck = false;
            break;
        default:
            break;
    }

    if (hungerCheck && you.hunger_state < HS_HUNGRY)
    {
        mpr("You're too hungry.");
        return;
    }

    // no turning back now... {dlb}
    you.turn_is_over = 1;

    if (random2avg(98, 3) <= abil_now[abil_used].fail)
    {
        mpr("You fail to use your ability.");
        return;
    }

    switch (abil_now[abil_used].which)
    {
#ifdef USE_ELVISH_GLAMOUR_ABILITY
    case ABIL_GLAMOUR:
        if (!enough_mp(5, false))
            return;

        else if (you.duration[DUR_GLAMOUR])
        {
            canned_msg(MSG_CANNOT_DO_YET);
            return;
        }

        mpr("You use your Elvish wiles.");

        cast_glamour(10 + random2(you.experience_level / 2)
                            + random2(you.experience_level / 2));

        dec_mp(5);
        make_hungry(40 + random2avg(79, 2), true);
        you.duration[DUR_GLAMOUR] = 20 + random2avg(13, 3);
        you.turn_is_over = 1;
        break;
#endif

    case ABIL_SPIT_POISON:      // Naga + spit poison mutation
        if (you.duration[DUR_BREATH_WEAPON])
        {
            canned_msg(MSG_CANNOT_DO_YET);
        }
        else if (spell_direction(abild, beam) == -1)
        {
            canned_msg(MSG_OK);
        }
        else
        {
            mpr("You spit poison.");

            zapping( ZAP_SPIT_POISON,
                    you.experience_level + you.mutation[MUT_SPIT_POISON] * 5,
                    beam );

            make_hungry(40 + random2avg(79, 2), false);
            you.duration[DUR_BREATH_WEAPON] += 3 + random2(5);
            you.turn_is_over = 1;
        }
        break;

    case ABIL_MAPPING:          // Gnome + sense surrounds mut
        mpr("You sense your surroundings.");

        magic_mapping( 3 + random2(you.experience_level)
                            + random2(you.experience_level)
                            + you.mutation[MUT_MAPPING] * 10,
                      40 + random2(you.experience_level)
                            + random2(you.experience_level)
                            + random2(you.experience_level) );

        make_hungry( 30 + random2avg(59, 2), true );
        you.turn_is_over = 1;
        break;

    case ABIL_TELEPORTATION:    // ring of teleport, + teleport mut
        if (!enough_mp(3, false))
            return;

        dec_mp(3);
        make_hungry(200 + random2avg(399, 2), true);

        if (you.mutation[MUT_TELEPORT_AT_WILL] == 3)
            you_teleport2(true);
        else
            you_teleport();
        break;

    case ABIL_BREATHE_FIRE:
    case ABIL_BREATHE_FROST:
    case ABIL_BREATHE_POISON:
    case ABIL_BREATHE_LIGHTNING:
    case ABIL_SPIT_ACID:
    case ABIL_BREATHE_POWER:
    case ABIL_BREATHE_STICKY_FLAME:
    case ABIL_BREATHE_STEAM:
        if (you.duration[DUR_BREATH_WEAPON]
            && abil_now[abil_used].which != ABIL_SPIT_ACID)
        {
            canned_msg(MSG_CANNOT_DO_YET);
            return;
        }
        else if (spell_direction(abild, beam) == -1)
        {
            canned_msg(MSG_OK);
            return;
        }

        switch (abil_now[abil_used].which)
        {
        case ABIL_BREATHE_FIRE:
            power = you.experience_level;
            power += you.mutation[MUT_BREATHE_FLAMES] * 4;
            if (you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
                power += 12;
            // don't check for hell serpents - they get hell fire,
            // never regular fire (GDL)

            mpr("You breathe fire" + (power < 10)?".":"!");

            zapping( ZAP_BREATHE_FIRE, power, beam);
            break;

        case ABIL_BREATHE_FROST:
            mpr("You exhale a wave of freezing cold.");
            zapping(ZAP_BREATHE_FROST, you.experience_level, beam);
            break;

        case ABIL_BREATHE_POISON:
            mpr("You exhale a blast of poison gas.");
            zapping(ZAP_BREATHE_POISON, you.experience_level, beam);
            break;

        case ABIL_BREATHE_LIGHTNING:
            mpr("You spit a bolt of lightning.");
            zapping(ZAP_LIGHTNING, (you.experience_level * 2), beam);
            break;

        case ABIL_SPIT_ACID:
            mpr("You spit acid.");
            zapping(ZAP_BREATHE_ACID, you.experience_level, beam);
            break;

        case ABIL_BREATHE_POWER:
            mpr("You spit a bolt of incandescent energy.");
            zapping(ZAP_BREATHE_POWER, you.experience_level, beam);
            break;

        case ABIL_BREATHE_STICKY_FLAME:
            mpr("You spit a glob of burning liquid.");
            zapping(ZAP_STICKY_FLAME, you.experience_level, beam);
            break;

        case ABIL_BREATHE_STEAM:
            mpr("You exhale a blast of scalding steam.");
            zapping(ZAP_BREATHE_STEAM, you.experience_level, beam);
            break;

        }

        if (abil_now[abil_used].which != ABIL_SPIT_ACID)
        {
            you.duration[DUR_BREATH_WEAPON] +=
                        random2(5) + 3 + random2(30 - you.experience_level);
        }

        if (abil_now[abil_used].which == ABIL_BREATHE_STEAM)
        {
            you.duration[DUR_BREATH_WEAPON] /= 2;
        }

        make_hungry(200 + random2avg(99, 2) -
                                (you.experience_level * random2(5)), false);
        you.turn_is_over = 1;
        break;

    case ABIL_BLINK:
        if (!enough_mp(1, false))
            return;

        dec_mp(1);
        make_hungry(50 + random2avg(99, 2), true);
        random_blink(true);
        break;

    case ABIL_GO_BERSERK_I:
        if (you.hunger_state < HS_SATIATED)
        {
            mpr("You're too hungry to berserk.");
            return;
        }

        go_berserk(true);
        break;

    // fly (kenku) -- eventually becomes permanent (see acr.cc)
    case ABIL_FLY:
        if (!enough_mp(3, false))
            return;

        cast_fly( you.experience_level * 4 );

        dec_mp(3);
        make_hungry(100 + random2avg(199, 2), true);

        if (you.experience_level > 14)
        {
            mpr("You feel very comfortable in the air.");
            you.levitation = 100;
            you.duration[DUR_CONTROLLED_FLIGHT] = 100;
        }
        break;

    case ABIL_FLY_II:           // Fly (Draconians, or anything else with wings)
        if (you.exhausted)
        {
            mpr("You're too exhausted to fly.");
        }
        else if (you.burden_state != BS_UNENCUMBERED)
        {
            mpr("You're carrying too much weight to fly.");
        }
        else
        {
            cast_fly( you.experience_level * 2 );

            make_hungry(25 + random2avg(49, 2), true);
            you.attribute[ATTR_EXPENSIVE_FLIGHT] = 1;
        }
        break;

    // DEMONIC POWERS:
    case ABIL_SUMMON_MINOR_DEMON:
        if (!enough_mp(3, false) || !enough_hp(4, false))
            return;

        summon_ice_beast_etc( you.experience_level * 4,
                                     summon_any_demon(DEMON_LESSER) );

        dec_mp(3);
        dec_hp(3, false);
        make_hungry(50 + random2avg(199, 2), true);
        break;

    case ABIL_SUMMON_DEMON:
        if (!enough_mp(5, false) || !enough_hp(6, false))
            return;

        summon_ice_beast_etc( you.experience_level * 4,
                                     summon_any_demon(DEMON_COMMON) );

        dec_mp(5);
        dec_hp(5, false);
        make_hungry(100 + random2avg(399, 2), true);
        break;

    case ABIL_HELLFIRE:
        if (!enough_mp(8, false) || !enough_hp(9, false))
            return;

        your_spells(SPELL_HELLFIRE, 20 + you.experience_level, false);

        dec_mp(8);
        dec_hp(8, false);
        make_hungry(200 + random2avg(399, 2), true);
        break;

    case ABIL_TORMENT:
        if (you.is_undead)
        {
            mpr("The unliving cannot use this ability.");
            return;
        }

        if (!enough_mp(9, false))
            return;

        torment(you.x_pos, you.y_pos);

        dec_mp(9);
        make_hungry(200 + random2avg(599, 2), true);
        break;

    case ABIL_RAISE_DEAD:
        if (!enough_mp(5, false) || !enough_hp(6, false))
            return;

        your_spells(SPELL_ANIMATE_DEAD, you.experience_level * 5, false);

        dec_mp(5);
        dec_hp(5, false);
        make_hungry(100 + random2avg(399, 2), true);
        break;

    case ABIL_CONTROL_DEMON:
        if (!enough_mp(4, false) || !enough_hp(5, false))
            return;

        if (spell_direction(abild, beam) == -1)
        {
            canned_msg(MSG_OK);
            return;
        }

        zapping(ZAP_CONTROL_DEMON, you.experience_level * 5, beam);

        dec_mp(4);
        dec_hp(4, false);
        make_hungry(100 + random2avg(99, 2), true);
        break;

    case ABIL_TO_PANDEMONIUM:
        if (you.level_type == LEVEL_PANDEMONIUM)
        {
            mpr("You're already here.");
            return;
        }

        if (!enough_mp(7, false))
            return;

        banished(DNGN_ENTER_PANDEMONIUM);

        dec_mp(7);
        make_hungry(200 + random2avg(399, 2), true);
        break;

    case ABIL_CHANNELING:
        if (!enough_hp(1, false))
            return;

        mpr("You channel some magical energy.");

        inc_mp(1 + random2(5), false);
        dec_hp(1, false);
        make_hungry(20 + random2avg(59, 2), true);
        break;

    case ABIL_THROW_FLAME:
    case ABIL_THROW_FROST:
        if (!enough_mp(1, false) || !enough_hp(2, false))
            return;

        if (spell_direction(abild, beam) == -1)
        {
            canned_msg(MSG_OK);
            return;
        }

        dec_mp(1);
        dec_hp(1, false);
        make_hungry(50 + random2avg(99, 2), true);

        zapping( (abil_now[abil_used].which == ABIL_THROW_FLAME
                        ? ZAP_FLAME : ZAP_FROST),
                    you.experience_level * 3,
                    beam );
        break;

    case ABIL_BOLT_OF_DRAINING:
        if (!enough_mp(4, false) || !enough_hp(5, false))
            return;

        if (spell_direction(abild, beam) == -1)
        {
            canned_msg(MSG_OK);
            return;
        }

        zapping(ZAP_NEGATIVE_ENERGY, you.experience_level * 6, beam);

        dec_mp(4);
        dec_hp(4, false);
        make_hungry(100 + random2avg(199, 2), true);
        break;

        // MAX_ITEMS, etc.:
    case ABIL_TURN_INVISIBLE:
        if (you.hunger_state < HS_SATIATED)
        {
            mpr("You're too hungry to turn invisible.");
            return;
        }

        if (!enough_mp(2, false))
            return;

        potion_effect(POT_INVISIBILITY, you.experience_level + 10);

        dec_mp(2);
        make_hungry(250 + random2avg(499, 2), true);
        break;

    case ABIL_TURN_VISIBLE:
        mpr("You feel less transparent.");
        you.invis = 1;
        break;

    case ABIL_LEVITATE:
        if (!enough_mp(1, false))
            return;

        potion_effect(POT_LEVITATION, you.experience_level + 10);

        dec_mp(1);
        make_hungry(100 + random2avg(199, 2), true);
        break;

    case ABIL_STOP_LEVITATING:
        mpr("You feel heavy.");
        you.levitation = 1;
        break;

    case ABIL_END_TRANSFORMATION:
        mpr("You feel almost normal.");
        you.duration[DUR_TRANSFORMATION] = 2;
        break;

    // INVOCATIONS:
    case ABIL_REPEL_UNDEAD_I:
    case ABIL_REPEL_UNDEAD_II:
        if (!enough_mp(1, false))
            return;

        turn_undead(you.piety);

        dec_mp(1);
        exercise(SK_INVOCATIONS, 1);
        break;

    case ABIL_MINOR_HEALING:
        if (!enough_mp(2, false))
            return;

        dec_mp(2);

        if (!cast_lesser_healing())
            break;

        lose_piety(1);
        make_hungry(50 + random2avg(99, 2), true);
        exercise(SK_INVOCATIONS, 1 + random2(3));
        break;

    case ABIL_PESTILENCE:
        if (!enough_mp(3, false))
            return;

        summon_swarm(you.piety);

        dec_mp(3);
        lose_piety((coinflip()? 3 : 2));
        make_hungry(100 + random2avg(199, 2), true);
        exercise(SK_INVOCATIONS, 2 + random2(4));
        break;

    case ABIL_HOLY_WORD:
        if (!enough_mp(6, false))
            return;

        holy_word(you.piety);

        dec_mp(6);
        lose_piety(coinflip()? 4 : 3);
        make_hungry(150 + random2avg(299, 2), true);
        exercise(SK_INVOCATIONS, 3 + random2(5));
        break;

    case ABIL_SUMMON_GUARDIAN:
        if (!enough_mp(7, false))
            return;

        summon_ice_beast_etc(you.skills[SK_INVOCATIONS] * 4, MONS_ANGEL);

        dec_mp(7);
        lose_piety(3 + random2(4));
        make_hungry(150 + random2avg(299, 2), true);
        exercise(SK_INVOCATIONS, 8 + random2(10));
        break;

    case ABIL_SMITING:
        if (!enough_mp(3, false))
            return;

        cast_smiting(you.skills[SK_INVOCATIONS] * 6);

        dec_mp(3);
        lose_piety(1);
        make_hungry(50 + random2avg(99, 2), true);
        exercise(SK_INVOCATIONS, (coinflip()? 3 : 2));
        break;

    case ABIL_ANNIHILATE_UNDEAD:
        if (!enough_mp(3, false))
            return;

        if (spell_direction(spd, beam) == -1)
        {
            canned_msg(MSG_OK);
            return;
        }

        zapping(ZAP_DISPEL_UNDEAD, you.skills[SK_INVOCATIONS] * 6, beam);

        dec_mp(3);
        lose_piety((coinflip()? 2 : 1));
        make_hungry(50 + random2avg(99, 2), true);
        exercise(SK_INVOCATIONS, 2 + random2(4));
        break;

    case ABIL_THUNDERBOLT:
        if (!enough_mp(5, false))
            return;

        if (spell_direction(spd, beam) == -1)
        {
            canned_msg(MSG_OK);
            return;
        }

        zapping(ZAP_LIGHTNING, you.skills[SK_INVOCATIONS] * 6, beam);

        dec_mp(5);
        lose_piety((coinflip()? 2 : 1));
        make_hungry(100 + random2avg(199, 2), true);
        exercise(SK_INVOCATIONS, 3 + random2(6));
        break;

    case ABIL_SUMMON_DAEVA:
        if (!enough_mp(8, false))
            return;

        summon_ice_beast_etc(you.skills[SK_INVOCATIONS] * 4, MONS_DAEVA);

        dec_mp(8);
        lose_piety(3 + random2(4));
        make_hungry(150 + random2avg(299, 2), true);
        exercise(SK_INVOCATIONS, 8 + random2(10));
        break;

    case ABIL_RECALL_UNDEAD_SLAVES:
        if (!enough_mp(1, false))
            return;

        recall(1);

        dec_mp(1);
        exercise(SK_INVOCATIONS, 1);
        break;

    case ABIL_ENSLAVE_UNDEAD:
        if (!enough_mp(4, false))
            return;

        if (spell_direction(spd, beam) == -1)
        {
            canned_msg(MSG_OK);
            return;
        }

        zapping(ZAP_ENSLAVE_UNDEAD, you.skills[SK_INVOCATIONS] * 8,
                beam);

        dec_mp(4);
        lose_piety((coinflip()? 3 : 2));
        make_hungry(150 + random2avg(299, 2), true);
        exercise(SK_INVOCATIONS, 5 + random2(5));
        break;

    case ABIL_INVOKE_DEATH:
        if (!enough_mp(4, false))
            return;

        summon_ice_beast_etc(20 + you.skills[SK_INVOCATIONS] * 3,
                             MONS_REAPER);

        dec_mp(4);
        lose_piety(3 + random2(4));
        make_hungry(350 + random2avg(699, 2), true);
        exercise(SK_INVOCATIONS, 10 + random2(14));
        break;

    case ABIL_ANIMATE_CORPSE:
        if (!enough_mp(3, false))
            return;

        mpr("You call on the dead to walk for you...");

        animate_a_corpse(you.x_pos, you.y_pos, BEH_ENSLAVED, you.pet_target,
                         CORPSE_BODY);

        dec_mp(3);
        make_hungry(50 + random2avg(99, 2), true);
        exercise(SK_INVOCATIONS, 2 + random2(4));
        break;

    case ABIL_RECALL_UNDEAD:
        if (!enough_mp(4, false))
            return;

        recall(1);

        dec_mp(4);
        make_hungry(100 + random2avg(199, 2), true);
        exercise(SK_INVOCATIONS, 2 + random2(4));
        break;

    case ABIL_ANIMATE_DEAD:
        if (!enough_mp(7, false))
            return;

        mpr("You call on the dead to walk for you...");

        animate_dead(1 + you.skills[SK_INVOCATIONS], BEH_ENSLAVED,
                     you.pet_target, 1);

        dec_mp(7);
        lose_piety((coinflip()? 2 : 1));
        make_hungry(150 + random2avg(299, 2), true);
        exercise(SK_INVOCATIONS, 2 + random2(4));
        break;

    case ABIL_DRAIN_LIFE:
        if (!enough_mp(6, false))
            return;

        drain_life(you.skills[SK_INVOCATIONS]);

        dec_mp(6);
        lose_piety(1 + random2(3));
        make_hungry(150 + random2avg(299, 2), true);
        exercise(SK_INVOCATIONS, 2 + random2(4));
        break;

    case ABIL_CONTROL_UNDEAD:
        if (!enough_mp(5, false))
            return;

        mass_enchantment(30, you.skills[SK_INVOCATIONS] * 8);

        dec_mp(5);
        lose_piety(2 + random2(3));
        make_hungry(150 + random2avg(399, 2), true);
        exercise(SK_INVOCATIONS, 3 + random2(4));
        break;

    case ABIL_CHANNEL_ENERGY:
        mpr("You channel some magical energy.");

        inc_mp(1 + random2(you.skills[SK_INVOCATIONS] / 4 + 2), false);
        make_hungry(50 + random2avg(99, 2), true);
        exercise(SK_INVOCATIONS, 1 + random2(3));
        break;

    case ABIL_MIGHT_I:
        if (!enough_mp(2, false))
            return;

        potion_effect(POT_MIGHT, you.skills[SK_INVOCATIONS] * 8);

        dec_mp(2);
        lose_piety(1);
        make_hungry(50 + random2avg(99, 2), true);
        exercise(SK_INVOCATIONS, 1 + random2(3));
        break;

    case ABIL_HEALING_I:
        if (!enough_mp(2, false))
            return;

        dec_mp(2);

        if (!cast_lesser_healing())
            break;

        lose_piety(1);
        make_hungry(100 + random2avg(99, 2), true);
        exercise(SK_INVOCATIONS, 2 + random2(5));
        break;

    case ABIL_HASTE:
        if (!enough_mp(5, false))
            return;

        potion_effect(POT_SPEED, you.skills[SK_INVOCATIONS] * 8);

        dec_mp(5);
        lose_piety(2 + random2(3));
        make_hungry(100 + random2avg(199, 2), true);
        exercise(SK_INVOCATIONS, 3 + random2(7));
        break;

    case ABIL_MINOR_DESTRUCTION:
        if (!enough_mp(1, false))
            return;

        if (spell_direction(spd, beam) == -1)
        {
            canned_msg(MSG_OK);
            return;
        }

        power = random2avg((you.skills[SK_INVOCATIONS] * 6), 2);

        switch (random2(5))
        {
        case 0: zapping( ZAP_FLAME,        power,     beam ); break;
        case 1: zapping( ZAP_PAIN,         power,     beam ); break;
        case 2: zapping( ZAP_STONE_ARROW,  power,     beam ); break;
        case 3: zapping( ZAP_ELECTRICITY,  power,     beam ); break;
        case 4: zapping( ZAP_BREATHE_ACID, power / 2, beam ); break;
        }

        dec_mp(1);
        make_hungry(20 + random2avg(39, 2), true);
        exercise(SK_INVOCATIONS, (coinflip()? 2 : 1));
        break;

    case ABIL_LESSER_SERVANT_OF_MAKHLEB:
        if (!enough_mp(2, false))
            return;

        summon_ice_beast_etc(20 + you.skills[SK_INVOCATIONS] * 3,
                             MONS_NEQOXEC + random2(5));

        dec_mp(2);
        lose_piety((coinflip()? 2 : 1));
        make_hungry(50 + random2avg(99, 2), true);
        exercise(SK_INVOCATIONS, 2 + random2(3));
        break;

    case ABIL_MAJOR_DESTRUCTION:
        if (!enough_mp(4, false))
            return;

        if (spell_direction(spd, beam) == -1)
        {
            canned_msg(MSG_OK);
            return;
        }

        power = you.skills[SK_INVOCATIONS] * 5;

        switch (random2(8))
        {
        case 0: zapping( ZAP_FIRE,               power, beam ); break;
        case 1: zapping( ZAP_FIREBALL,           power, beam ); break;
        case 2: zapping( ZAP_LIGHTNING,          power, beam ); break;
        case 3: zapping( ZAP_NEGATIVE_ENERGY,    power, beam ); break;
        case 4: zapping( ZAP_STICKY_FLAME,       power, beam ); break;
        case 5: zapping( ZAP_IRON_BOLT,          power, beam ); break;
        case 6: zapping( ZAP_ORB_OF_ELECTRICITY, power, beam ); break;

        case 7:
            mpr("You feel temporarily insulated.");
            you.attribute[ATTR_DIVINE_LIGHTNING_PROTECTION] = 1;
            mpr("Makhleb hurls a blast of lightning!");

            // make a divine lightning bolt...
            beam.beam_source = NON_MONSTER;
            beam.type = SYM_BURST;
            beam.damage = 130;
            beam.flavour = BEAM_ELECTRICITY;
            beam.target_x = you.x_pos;
            beam.target_y = you.y_pos;
            strcpy(beam.beam_name, "blast of lightning");
            beam.colour = LIGHTCYAN;
            beam.thrower = KILL_YOU;
            beam.ex_size = 1 + you.skills[SK_INVOCATIONS] / 8;
            beam.isTracer = false;

            // ... and fire!
            explosion(beam);
            // protection down
            you.attribute[ATTR_DIVINE_LIGHTNING_PROTECTION] = 0;
            break;
        }

        dec_mp(3);
        lose_piety((coinflip()? 2 : 1));
        make_hungry(100 + random2avg(99, 2), true);
        exercise(SK_INVOCATIONS, 3 + random2(5));
        break;

    case ABIL_GREATER_SERVANT_OF_MAKHLEB:
        if (!enough_mp(6, false))
            return;

        summon_ice_beast_etc(20 + you.skills[SK_INVOCATIONS] * 3,
                             MONS_EXECUTIONER + random2(5));

        dec_mp(4);
        lose_piety((coinflip()? 3 : 2));
        make_hungry(100 + random2avg(99, 2), true);
        exercise(SK_INVOCATIONS, 6 + random2(6));
        break;

    case ABIL_GO_BERSERK_II:
        if (you.hunger_state < HS_SATIATED)
        {
            mpr("You're too hungry to berserk.");
            return;
        }

        go_berserk(true);

        make_hungry(100 + random2avg(199, 2), true);
        //exercise(SK_INVOCATIONS, 1 + random2(3));
        break;

    case ABIL_MIGHT_II:
        potion_effect(POT_MIGHT, you.skills[SK_INVOCATIONS] * 6);

        lose_piety((coinflip()? 2 : 1));
        make_hungry(200 + random2avg(399, 2), true);
        //exercise(SK_INVOCATIONS, 2 + random2(5));
        break;

    case ABIL_HASTE_SELF:
        potion_effect(POT_SPEED, you.skills[SK_INVOCATIONS] * 6);

        lose_piety(2 + random2(3));
        make_hungry(250 + random2avg(499, 2), true);
        //exercise(SK_INVOCATIONS, 4 + random2(6));
        break;

    case ABIL_FORGET_SPELL:
        cast_selective_amnesia(true);
        //exercise(SK_INVOCATIONS, 1);
        break;

    case ABIL_LESSER_HEALING:
        if (!enough_mp(1, false))
            return;

        dec_mp(1);

        if (!cast_lesser_healing())
            break;

        make_hungry(100 + random2avg(199, 2), true);
        exercise(SK_INVOCATIONS, (coinflip()? 2 : 1));
        break;

    case ABIL_PURIFICATION:
        if (!enough_mp(1, false))
            return;

        purification();

        dec_mp(1);
        lose_piety(1);
        make_hungry(150 + random2avg(299, 2), true);
        exercise(SK_INVOCATIONS, 2 + random2(3));
        break;

    case ABIL_HEALING_II:
        if (!enough_mp(2, false))
            return;

        if (!cast_greater_healing())
            break;

        lose_piety(1 + random2(3));

        dec_mp(2);
        make_hungry(250 + random2avg(499, 2), true);
        exercise(SK_INVOCATIONS, 3 + random2(5));
        break;

    case ABIL_RESTORATION:
        if (!enough_mp(3, false))
            return;

        restore_stat(STAT_ALL, false);

        if (you.base_hp < 5000)
            you.base_hp = 5000;

        calc_hp();
        dec_mp(3);
        lose_piety(2 + random2(3));
        make_hungry(400 + random2avg(899, 2), true);
        exercise(SK_INVOCATIONS, 4 + random2(6));
        break;

    case ABIL_GREATER_HEALING:
        if (!enough_mp(6, false))
            return;

        if (!cast_greatest_healing())
            break;

        //cast_purification(you.piety);

        dec_mp(6);
        lose_piety(4 + random2(4));
        make_hungry(600 + random2avg(1099, 2), true);
        exercise(SK_INVOCATIONS, 6 + random2(10));
        break;

    //jmf: intended as invocations from evil god(s):
    case ABIL_CHARM_SNAKE:
        if (!enough_mp(6, false))
            return;

        cast_snake_charm( you.experience_level * 2
                            + you.skills[SK_INVOCATIONS] * 3 );

        dec_mp(6);
        lose_piety(3 + random2(6));
        make_hungry(200 + random2avg(300, 3), true);
        exercise(SK_INVOCATIONS, 2 + random2(4));
        break;

    case ABIL_TRAN_SERPENT_OF_HELL:
        if (!enough_mp(16, false))
            return;

        transform(10 + (you.experience_level * 2) +
                  (you.skills[SK_INVOCATIONS] * 3), TRAN_SERPENT_OF_HELL);

        dec_mp(16);
        lose_piety(8 + random2(4));
        exercise(SK_INVOCATIONS, 6 + random2(9));
        break;

    case ABIL_BREATHE_HELLFIRE:
        if (!enough_hp(9, false))
            return;

        if (you.duration[DUR_BREATH_WEAPON])
        {
            canned_msg(MSG_CANNOT_DO_YET);
            return;
        }

        if (spell_direction(spd, beam) == -1)
            break;

        zapping(ZAP_HELLFIRE, 20 + you.experience_level, beam);

        dec_hp(8, false);
        make_hungry( 200 + random2avg(99, 2) -
                            (you.experience_level * random2(5)), false );

        you.duration[DUR_BREATH_WEAPON] +=
                        3 + random2(5) + random2(30 - you.experience_level);
        break;

    case ABIL_ROTTING:
        if (!enough_mp(4, false) || !enough_hp(5, false))
            return;

        cast_rotting(you.experience_level * 2 + you.skills[SK_INVOCATIONS] * 3);

        dec_mp(4);
        lose_piety(1 + random2(4));
        //make_hungry(100 + random2avg(199,2), true);
        exercise(SK_INVOCATIONS, 2 + random2(4));
        break;

    case ABIL_TORMENT_II:
        if (!enough_mp(9, false))
            return;

        torment(you.x_pos, you.y_pos);

        dec_mp(9);
        lose_piety(2 + random2(4));
        //make_hungry(100 + random2avg(199,2), true);
        exercise(SK_INVOCATIONS, 2 + random2(4));
        break;

    case ABIL_SHUGGOTH_SEED:
        if (!enough_mp(12, false) || !enough_hp(9, false))
            return;

        if (you.duration[DUR_SHUGGOTH_SEED_RELOAD])
        {
            canned_msg(MSG_CANNOT_DO_YET);
            return;
        }

        cast_shuggoth_seed( you.experience_level * 2
                                + you.skills[SK_INVOCATIONS] * 3 );

        dec_mp(12);
        lose_piety(5 + random2(6));
        //make_hungry(200 + random2avg(299,2), true);
        you.duration[DUR_SHUGGOTH_SEED_RELOAD] = 10 + random2avg(39, 2);
        exercise(SK_INVOCATIONS, 2 + random2(4));
        break;

    case ABIL_RENOUNCE_RELIGION:
        if (yesno("Really renounce your faith, foregoing its fabulous benefits?")
            && yesno( "Are you sure you won't change your mind later?" ))
        {
            excommunication();
        }
        else
        {
            canned_msg(MSG_OK);
        }
        break;

    default:
        mpr("Sorry, you can't do that.");
        break;
    }

    return;
}  // end activate_ability()


// Lists any abilities the player may possess
static char show_abilities(struct talent *p_abils)
/************************************************/
{
    struct talent *zeroindex = p_abils;

    int loopy = 0;
    char lines = 0;
    unsigned char anything = 0;
    char strng[5] = "";         // is this really necessary? {dlb}
    char ft, ki;
    bool can_invoke = false;

    for (loopy = 0; loopy < 80; loopy++)
    {
        if ((p_abils + loopy)->is_invocation)
        {
            can_invoke = true;
            break;
        }
    }


#ifdef DOS_TERM
    char buffer[4800];

    gettext(1, 1, 80, 25, buffer);
    window(1, 1, 80, 25);
#endif

    clrscr();
    cprintf
        ("  Ability                           Cost                    Success");
    lines++;

    /* ******************************************************************
       // BEGIN: does this block ever come into play? {dlb}

       if ( lines > NUMBER_OF_LINES - 3 )
       {
       gotoxy(1, NUMBER_OF_LINES);
       cprintf("-more-");

       ki = getch();

       if ( ki == 27 )
       {
       #ifdef DOS_TERM
       puttext(1, 1, 80, 25, buffer);
       #endif
       return 27;
       }

       if (ki >= 'A' && ki <= 'z')
       {
       #ifdef DOS_TERM
       puttext(1, 1, 80, 25, buffer);
       #endif
       return ki;
       }

       if ( ki == 0 )
       ki = getch();

       lines = 0;
       clrscr();
       gotoxy(1, 1);
       anything = 0;

       }

       if ( lines > 1 )
       cprintf(EOL " ");

       // END: does this block ever come into play? {dlb}
       ****************************************************************** */

    for (loopy = 0; loopy < 80; loopy++)
    {
        p_abils = (zeroindex + loopy);

        if (lines > NUMBER_OF_LINES - 2)
        {
            gotoxy(1, NUMBER_OF_LINES);
            cprintf("-more-");

            ki = getch();

            if (ki == 27)
            {
#ifdef DOS_TERM
                puttext(1, 1, 80, 25, buffer);
#endif
                return 27;
            }

            if (ki >= 'A' && ki <= 'z')
            {
#ifdef DOS_TERM
                puttext(1, 1, 80, 25, buffer);
#endif
                return ki;
            }

            if (ki == 0)
                ki = getch();

            lines = 0;
            clrscr();
            gotoxy(1, 1);
            anything = 0;
        }

        if (loopy == 25 && can_invoke)
        {
            anything++;
            textcolor(BLUE);
            cprintf(EOL "    Invocations - ");
            textcolor(LIGHTGREY);
            lines++;
            continue;
        }

        if (p_abils->which != -1)
        {
            anything++;

            if (lines > 0)
                cprintf(EOL);

            lines++;

            cprintf(" ");

            ft = index_to_letter(loopy);

            strng[0] = ft;
            cprintf(strng);
            cprintf(" - ");

            switch (p_abils->which)
            {
#ifdef USE_ELVISH_GLAMOUR_ABILITY
            case ABIL_GLAMOUR:
                cprintf("Glamour                         5 Magic, Food");
                break;
#endif
            case ABIL_SPIT_POISON:
                cprintf("Spit Poison                     Food");
                break;
            case ABIL_MAPPING:
                cprintf("Sense Surroundings              Food");
                break;
            case ABIL_TELEPORTATION:
                cprintf("Teleportation                   3 Magic, Food");
                break;
            case ABIL_BREATHE_FIRE:
                cprintf("Breathe Fire                    Food, Delay");
                break;
            case ABIL_BLINK:
                cprintf("Blink                           1 Magic, Food ");
                break;
            case ABIL_BREATHE_FROST:
                cprintf("Breathe Frost                   Food, Delay");
                break;
            case ABIL_BREATHE_POISON:
                cprintf("Breathe Poison Gas              Food, Delay");
                break;
            case ABIL_BREATHE_LIGHTNING:
                cprintf("Breathe Lightning               Food, Delay");
                break;
            case ABIL_SPIT_ACID:
                cprintf("Spit Acid                       Food");
                break;
            case ABIL_BREATHE_POWER:
                cprintf("Breathe Power                   Food, Delay");
                break;
            case ABIL_GO_BERSERK_I:
                cprintf("Go Berserk                      None");
                break;
            case ABIL_BREATHE_STICKY_FLAME:
                cprintf("Breathe Sticky Flame            Food, Delay");
                break;
            case ABIL_BREATHE_STEAM:
                cprintf("Breathe Steam                   Food, Delay");
                break;
            case ABIL_BREATHE_HELLFIRE:
                cprintf("Breathe Hellfire                8 hp, Food, Delay");
                break;
            case ABIL_FLY:
                cprintf("Fly                             3 Magic, Food");
                break;
            case ABIL_FLY_II:
                cprintf("Fly                             Food, Exhaustion");
                break;

            //Demonic powers:
            case ABIL_SUMMON_MINOR_DEMON:
                cprintf
                    ("Summon Minor Demon              3 Magic, 3 hp, Food");
                break;
            case ABIL_SUMMON_DEMON:
                cprintf
                    ("Summon Demon                    5 Magic, 5 hp, Food");
                break;
            case ABIL_HELLFIRE:
                cprintf
                    ("Hellfire                        8 Magic, 8 hp, Food");
                break;
            case ABIL_TORMENT:
                cprintf
                    ("Torment                         9 Magic, Food, Pain");
                break;
            case ABIL_RAISE_DEAD:
                cprintf
                    ("Raise Dead                      5 Magic, 5 hp, Food");
                break;
            case ABIL_CONTROL_DEMON:
                cprintf
                    ("Control Demon                   4 Magic, 4 hp, Food");
                break;
            case ABIL_TO_PANDEMONIUM:
                //jmf: was "Open Gate to Pandemonium"
                cprintf("Gate Yourself to Pandemonium    7 Magic, Food");
                break;
            case ABIL_CHANNELING:
                cprintf("Channeling                      1 hp, Food");
                break;
            case ABIL_THROW_FLAME:
                cprintf
                    ("Throw Flame                     1 Magic, 1 hp, Food");
                break;
            case ABIL_THROW_FROST:
                cprintf
                    ("Throw Frost                     1 Magic, 1 hp, Food");
                break;
            case ABIL_BOLT_OF_DRAINING:
                cprintf
                    ("Bolt of Draining                4 Magic, 4 hp, Food");
                break;

            // Most of the following are from items:
            case ABIL_TURN_INVISIBLE:
                cprintf("Turn Invisible                  2 Magic, Food");
                break;
            case ABIL_TURN_VISIBLE:
                cprintf("Turn Visible                    None");
                break;
            case ABIL_LEVITATE:
                cprintf("Levitate                        1 Magic, Food");
                break;
            case ABIL_STOP_LEVITATING:
                cprintf("Stop Levitating                 None");
                break;

            case ABIL_END_TRANSFORMATION:
                cprintf("End Transformation              None");
                break;

            // Invocations:
            case ABIL_REPEL_UNDEAD_I:
            case ABIL_REPEL_UNDEAD_II:
                cprintf("Repel Undead                    1 Magic, Food");
                break;
            case ABIL_MINOR_HEALING:
                cprintf
                    ("Minor Healing                   2 Magic, Food, Piety");
                break;
            case ABIL_PESTILENCE:
                cprintf
                    ("Pestilence                      3 Magic, Food, Piety");
                break;
            case ABIL_HOLY_WORD:
                cprintf
                    ("Holy Word                       6 Magic, Food, Piety");
                break;
            case ABIL_SUMMON_GUARDIAN:
                cprintf
                    ("Summon Guardian                 7 Magic, Food, Piety");
                break;
            case ABIL_SMITING:
                cprintf
                    ("Smiting                         3 Magic, Food, Piety");
                break;
            case ABIL_ANNIHILATE_UNDEAD:
                cprintf
                    ("Annihilate Undead               3 Magic, Food, Piety");
                break;
            case ABIL_THUNDERBOLT:
                cprintf
                    ("Thunderbolt                     5 Magic, Food, Piety");
                break;
            case ABIL_SUMMON_DAEVA:
                cprintf
                    ("Summon Daeva                    8 Magic, Food, Piety");
                break;
            case ABIL_RECALL_UNDEAD_SLAVES:
                cprintf("Recall Undead Slaves            1 Magic");
                break;
            case ABIL_ENSLAVE_UNDEAD:
                cprintf
                    ("Enslave Undead                  2 Magic, Food, Piety");
                break;
            case ABIL_INVOKE_DEATH:
                cprintf
                    ("Invoke Death                    3 Magic, Food, Piety");
                break;
            case ABIL_ANIMATE_CORPSE:
                cprintf("Animate Corpse                  3 Magic, Food");
                break;
            case ABIL_RECALL_UNDEAD:
                cprintf("Recall Undead                   4 Magic, Food");
                break;
            case ABIL_ANIMATE_DEAD:
                cprintf
                    ("Animate Dead                    7 Magic, Food, Piety");
                break;
            case ABIL_DRAIN_LIFE:
                cprintf
                    ("Drain Life                      6 Magic, Food, Piety");
                break;
            case ABIL_CONTROL_UNDEAD:
                cprintf
                    ("Control Undead                  5 Magic, Food, Piety");
                break;
            case ABIL_CHANNEL_ENERGY:
                cprintf("Channel Energy                  Food");
                break;
            case ABIL_MIGHT_I:
                cprintf
                    ("Might                           2 Magic, Food, Piety");
                break;
            case ABIL_HEALING_I:
                cprintf
                    ("Healing                         2 Magic, Food, Piety");
                break;
            case ABIL_HASTE:
                cprintf
                    ("Haste                           5 Magic, Food, Piety");
                break;
            case ABIL_MINOR_DESTRUCTION:
                cprintf("Minor Destruction               1 Magic, Food");
                break;
            case ABIL_LESSER_SERVANT_OF_MAKHLEB:
                cprintf
                    ("Lesser Servant of Makhleb       2 Magic, Food, Piety");
                break;
            case ABIL_MAJOR_DESTRUCTION:
                cprintf
                    ("Major Destruction               4 Magic, Food, Piety");
                break;
            case ABIL_GREATER_SERVANT_OF_MAKHLEB:
                cprintf
                    ("Greater Servant of Makhleb      6 Magic, Food, Piety");
                break;
            case ABIL_GO_BERSERK_II:
                cprintf("Go Berserk                      Food");
                break;
            case ABIL_MIGHT_II:
                cprintf("Might                           Food, Piety");
                break;
            case ABIL_HASTE_SELF:
                cprintf("Haste Self                      Food, Piety");
                break;
            case ABIL_FORGET_SPELL:
                cprintf("Forget Spell                    5 Magic, Piety");
                break;
            case ABIL_LESSER_HEALING:
                cprintf("Lesser Healing                  1 Magic, Food");
                break;
            case ABIL_PURIFICATION:
                cprintf
                    ("Purification                    2 Magic, Food, Piety");
                break;
            case ABIL_HEALING_II:
                cprintf
                    ("Healing                         2 Magic, Food, Piety");
                break;
            case ABIL_RESTORATION:
                cprintf
                    ("Restoration                     3 Magic, Food, Piety");
                break;
            case ABIL_GREATER_HEALING:
                cprintf
                    ("Greater Healing                 6 Magic, Food, Piety");
                break;
            case ABIL_CHARM_SNAKE:
                cprintf
                    ("Charm Snake                     6 Magic, Food, Piety");
            case ABIL_TRAN_SERPENT_OF_HELL:
                cprintf
                    ("Turn into Demonic Serpent      16 Magic, Food, Piety");
                break;
            case ABIL_ROTTING:
                cprintf
                    ("Rotting                         4 Magic, 4 hp, Piety");
                break;
            case ABIL_TORMENT_II:
                cprintf
                    ("Call Torment                    9 Magic, Piety, Pain");
                break;
            case ABIL_SHUGGOTH_SEED:
                cprintf
                    ("Sow Shuggoth Seed              12 Magic, 8 hp, Piety");
                break;
            case ABIL_RENOUNCE_RELIGION:
                cprintf("Renounce Religion               None");
                break;
            }

            gotoxy(35, wherey());

            gotoxy(60, wherey());

            int spell_f = p_abils->fail;

            cprintf( (spell_f == 100) ? "Useless"   :
                     (spell_f >   90) ? "Terrible"  :
                     (spell_f >   80) ? "Cruddy"    :
                     (spell_f >   70) ? "Bad"       :
                     (spell_f >   60) ? "Very Poor" :
                     (spell_f >   50) ? "Poor"      :
                     (spell_f >   40) ? "Fair"      :
                     (spell_f >   30) ? "Good"      :
                     (spell_f >   20) ? "Very Good" :
                     (spell_f >   10) ? "Great"     :
                     (spell_f >    0) ? "Excellent" :
                                        "Perfect" );

            gotoxy(70, wherey());
        }                              // end if conditional
    }                                  // end "for loopy"

    if (anything > 0)
    {
        ki = getch();

        if (ki >= 'A' && ki <= 'z')
        {
#ifdef DOS_TERM
            puttext(1, 1, 80, 25, buffer);
#endif
            return ki;
        }

        if (ki == 0)
            ki = getch();

#ifdef DOS_TERM
        puttext(1, 1, 80, 25, buffer);
#endif

        return anything;
    }

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif

    ki = getch();

    return ki;
}                               // end show_abilities()


static bool generate_abilities(struct talent *p_abils)
/****************************************************/
{

    int ability = -1;                   // used with draconian checks {dlb}
    struct talent *zeroindex = p_abils; // we may need this later {dlb}

    // checking for species-related abilities and mutagenic counterparts {dlb}:
#ifdef USE_ELVISH_GLAMOUR_ABILITY
    if (you.species == SP_GREY_ELF)
    {
        if (you.experience_level > 4
            && you.attribute[ATTR_TRANSFORMATION] == TRAN_NONE)
        {
            insert_ability(ABIL_GLAMOUR, &p_abils);
        }
    }
#endif

    //jmf: alternately put check elsewhere
    if ((you.level_type == LEVEL_DUNGEON
                && (you.species == SP_GNOME
                    || you.mutation[MUT_MAPPING]
                    || scan_randarts(RAP_MAPPING)))

        || (you.level_type == LEVEL_PANDEMONIUM
                && you.mutation[MUT_MAPPING] == 3))
    {
            insert_ability(ABIL_MAPPING, &p_abils);
    }


    //jmf: check for breath weapons -- they're exclusive of each other I hope!
    //     better make better ones first.
    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_SERPENT_OF_HELL)
    {
        insert_ability(ABIL_BREATHE_HELLFIRE, &p_abils);
    }
    else if (you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON
                                        || you.mutation[MUT_BREATHE_FLAMES])
    {
        insert_ability(ABIL_BREATHE_FIRE, &p_abils);
    }

    if (you.species == SP_NAGA)
    {
        if (you.mutation[MUT_BREATHE_POISON])
            insert_ability(ABIL_BREATHE_POISON, &p_abils);
        else
            insert_ability(ABIL_SPIT_POISON, &p_abils);
    }
    else if (you.mutation[MUT_SPIT_POISON])
    {
        insert_ability(ABIL_SPIT_POISON, &p_abils);
    }

    if (player_genus(GENPC_DRACONIAN))
    {
        if (you.experience_level > 6)
        {
            ability = (
                (you.species == SP_GREEN_DRACONIAN)  ? ABIL_BREATHE_POISON :
                (you.species == SP_RED_DRACONIAN)    ? ABIL_BREATHE_FIRE :
                (you.species == SP_WHITE_DRACONIAN)  ? ABIL_BREATHE_FROST :
                (you.species == SP_GOLDEN_DRACONIAN) ? ABIL_SPIT_ACID :
                (you.species == SP_BLACK_DRACONIAN)  ? ABIL_BREATHE_LIGHTNING :
                (you.species == SP_PURPLE_DRACONIAN) ? ABIL_BREATHE_POWER :
                (you.species == SP_PALE_DRACONIAN)   ? ABIL_BREATHE_STEAM :
                (you.species == SP_MOTTLED_DRACONIAN)? ABIL_BREATHE_STICKY_FLAME:
                                                     -1);

            if (ability != -1)
                insert_ability(ability, &p_abils);
        }
    }

    if (!you.duration[DUR_CONTROLLED_FLIGHT] && !you.levitation)
    {
        // kenku can fly, but only from the ground
        // (until levitation 15, when it becomes permanent until revoked)
        //jmf: "upgrade" for draconians -- expensive flight
        if (you.species == SP_KENKU && you.experience_level > 4)
            insert_ability(ABIL_FLY, &p_abils);
        else if (player_genus(GENPC_DRACONIAN) && you.mutation[MUT_BIG_WINGS])
            insert_ability(ABIL_FLY_II, &p_abils);
    }

    // demonic powers {dlb}:
    if (you.mutation[MUT_SUMMON_MINOR_DEMONS])
        insert_ability(ABIL_SUMMON_MINOR_DEMON, &p_abils);

    if (you.mutation[MUT_SUMMON_DEMONS])
        insert_ability(ABIL_SUMMON_DEMON, &p_abils);

    if (you.mutation[MUT_HURL_HELLFIRE])
        insert_ability(ABIL_HELLFIRE, &p_abils);

    if (you.mutation[MUT_CALL_TORMENT])
        insert_ability(ABIL_TORMENT, &p_abils);

    if (you.mutation[MUT_RAISE_DEAD])
        insert_ability(ABIL_RAISE_DEAD, &p_abils);

    if (you.mutation[MUT_CONTROL_DEMONS])
        insert_ability(ABIL_CONTROL_DEMON, &p_abils);

    if (you.mutation[MUT_PANDEMONIUM])
        insert_ability(ABIL_TO_PANDEMONIUM, &p_abils);

    if (you.mutation[MUT_CHANNEL_HELL])
        insert_ability(ABIL_CHANNELING, &p_abils);

    if (you.mutation[MUT_THROW_FLAMES])
        insert_ability(ABIL_THROW_FLAME, &p_abils);

    if (you.mutation[MUT_THROW_FROST])
        insert_ability(ABIL_THROW_FROST, &p_abils);

    if (you.mutation[MUT_SMITE])
        insert_ability(ABIL_BOLT_OF_DRAINING, &p_abils);

    // transformation-only powers {dlb}:
    //jmf: uh, and dragon-transformation breath above is what? fixed 24jul2000
    if (you.duration[DUR_TRANSFORMATION])
        insert_ability(ABIL_END_TRANSFORMATION, &p_abils);

    // Most abilities from items are displaced by 50, so that abilities obtained
    // naturally or from mutations can be put earlier, to avoid the menu letters
    // always changing around
    if (you.mutation[MUT_BLINK] || scan_randarts(RAP_BLINK))
        insert_ability(ABIL_BLINK, &p_abils);

    if (wearing_amulet(AMU_RAGE) || scan_randarts(RAP_BERSERK))
        insert_ability(ABIL_GO_BERSERK_I, &p_abils);

    if (you.invis)
    {
        insert_ability(ABIL_TURN_VISIBLE, &p_abils);
    }
    else
    {
        if ((you.equip[EQ_LEFT_RING] != -1
                && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_INVISIBILITY)
            || (you.equip[EQ_RIGHT_RING] != -1
                && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_INVISIBILITY)
            || (you.equip[EQ_CLOAK] != -1
                && you.inv_dam[you.equip[EQ_CLOAK]] % 30 == SPARM_DARKNESS)
            || (you.equip[EQ_HELMET] != -1
                && you.inv_dam[you.equip[EQ_HELMET]] % 30 == SPARM_DARKNESS)
            || scan_randarts(RAP_INVISIBLE))
        {
            insert_ability(ABIL_TURN_INVISIBLE, &p_abils);
        }
    }

    //jmf: "upgrade" for draconians -- expensive flight
    if (you.levitation)
    {
        insert_ability(ABIL_STOP_LEVITATING, &p_abils);
    }
    else
    {
        if ((you.equip[EQ_LEFT_RING] != -1
                && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_LEVITATION)
            || (you.equip[EQ_RIGHT_RING] != -1
                && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_LEVITATION)
            || (you.equip[EQ_BOOTS] != -1
                && you.inv_dam[you.equip[EQ_BOOTS]] % 30 == SPARM_LEVITATION)
            || scan_randarts(RAP_LEVITATE))
        {
            insert_ability(ABIL_LEVITATE, &p_abils);
        }
    }


    if ((you.equip[EQ_LEFT_RING] != -1
            && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_TELEPORTATION)
        || (you.equip[EQ_RIGHT_RING] != -1
            && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_TELEPORTATION)
        || you.mutation[MUT_TELEPORT_AT_WILL]
        || scan_randarts(RAP_CAN_TELEPORT))
    {
        insert_ability(ABIL_TELEPORTATION, &p_abils);
    }

    // look at output routine for why this is done {dlb}
    // -- god abilities are capitalized and this would see to do that
    //    magic... however, XXX: it does mean that if a player has a huge
    //    number of abilities, they may be overwriten, so this should
    //    be looked into -- bwr
    p_abils = (zeroindex + 26);

    // gods take abilities away until penance completed -- bwr
    if (!player_under_penance() && !silenced(you.x_pos, you.y_pos))
    {
        switch (you.religion)
        {
        case GOD_ZIN:
            if (you.piety >= 30)
                insert_ability(ABIL_REPEL_UNDEAD_I, &p_abils);
            if (you.piety >= 50)
                insert_ability(ABIL_MINOR_HEALING, &p_abils);
            if (you.piety >= 75)
                insert_ability(ABIL_PESTILENCE, &p_abils);
            if (you.piety >= 100)
                insert_ability(ABIL_HOLY_WORD, &p_abils);
            if (you.piety >= 120)
                insert_ability(ABIL_SUMMON_GUARDIAN, &p_abils);
            break;

        case GOD_SHINING_ONE:
            if (you.piety >= 30)
                insert_ability(ABIL_REPEL_UNDEAD_II, &p_abils);
            if (you.piety >= 50)
                insert_ability(ABIL_SMITING, &p_abils);
            if (you.piety >= 75)
                insert_ability(ABIL_ANNIHILATE_UNDEAD, &p_abils);
            if (you.piety >= 100)
                insert_ability(ABIL_THUNDERBOLT, &p_abils);
            if (you.piety >= 120)
                insert_ability(ABIL_SUMMON_DAEVA, &p_abils);
            break;

        case GOD_YREDELEMNUL:
            if (you.piety >= 30)
                insert_ability(ABIL_ANIMATE_CORPSE, &p_abils);
            if (you.piety >= 50)
                insert_ability(ABIL_RECALL_UNDEAD, &p_abils);
            if (you.piety >= 75)
                insert_ability(ABIL_ANIMATE_DEAD, &p_abils);
            if (you.piety >= 100)
                insert_ability(ABIL_DRAIN_LIFE, &p_abils);
            if (you.piety >= 120)
                insert_ability(ABIL_CONTROL_UNDEAD, &p_abils);
            break;

        case GOD_ELYVILON:
            if (you.piety >= 30)
                insert_ability(ABIL_LESSER_HEALING, &p_abils);
            if (you.piety >= 50)
                insert_ability(ABIL_PURIFICATION, &p_abils);
            if (you.piety >= 75)
                insert_ability(ABIL_HEALING_II, &p_abils);
            if (you.piety >= 100)
                insert_ability(ABIL_RESTORATION, &p_abils);
            if (you.piety >= 120)
                insert_ability(ABIL_GREATER_HEALING, &p_abils);
            break;

        case GOD_MAKHLEB:
            if (you.piety >= 50)
                insert_ability(ABIL_MINOR_DESTRUCTION, &p_abils);
            if (you.piety >= 75)
                insert_ability(ABIL_LESSER_SERVANT_OF_MAKHLEB, &p_abils);
            if (you.piety >= 100)
                insert_ability(ABIL_MAJOR_DESTRUCTION, &p_abils);
            if (you.piety >= 120)
                insert_ability(ABIL_GREATER_SERVANT_OF_MAKHLEB, &p_abils);
            break;

        case GOD_KIKUBAAQUDGHA:
            if (you.piety >= 30)
                insert_ability(ABIL_RECALL_UNDEAD_SLAVES, &p_abils);
            if (you.piety >= 75)
                insert_ability(ABIL_ENSLAVE_UNDEAD, &p_abils);
            if (you.piety >= 120)
                insert_ability(ABIL_INVOKE_DEATH, &p_abils);
            break;

        case GOD_OKAWARU:
            if (you.piety >= 30)
                insert_ability(ABIL_MIGHT_I, &p_abils);
            if (you.piety >= 50)
                insert_ability(ABIL_HEALING_I, &p_abils);
            if (you.piety >= 120)
                insert_ability(ABIL_HASTE, &p_abils);
            break;

        case GOD_TROG:
            if (you.piety >= 30)
                insert_ability(ABIL_GO_BERSERK_II, &p_abils);
            if (you.piety >= 50)
                insert_ability(ABIL_MIGHT_II, &p_abils);
            if (you.piety >= 100)
                insert_ability(ABIL_HASTE_SELF, &p_abils);
            break;

        case GOD_SIF_MUNA:
            if (you.piety >= 50)
                insert_ability(ABIL_FORGET_SPELL, &p_abils);
            break;

        case GOD_VEHUMET:
            if (you.piety >= 100)
                insert_ability(ABIL_CHANNEL_ENERGY, &p_abils);
            break;

        default:
            break;
        }
    }


    // and finally, the ability to opt-out of your faith {dlb}:
    p_abils = (zeroindex + 49);

    if (you.religion != GOD_NO_GOD && !silenced(you.x_pos, you.y_pos))
        insert_ability(ABIL_RENOUNCE_RELIGION, &p_abils);

    // this is a shameless kludge for the time being {dlb}:
    for (p_abils = zeroindex; (p_abils - zeroindex) < 80; p_abils++)
    {
        if (p_abils->which != -1)
            return true;
    }

    return false;
}                               // end generate_abilities()


static bool insert_ability(int which_ability, struct talent **p_pabils)
/*********************************************************************/
{
    int failure = 0;
    bool invoc = false;

    (*p_pabils)->which = which_ability;

    switch (which_ability)
    {
    // begin species abilities - some are mutagenic, too {dlb}
#ifdef USE_ELVISH_GLAMOUR_ABILITY
    case ABIL_GLAMOUR:
        failure = 50 - (you.experience_level * 2);
        break;
#endif

    case ABIL_SPIT_POISON:
        failure = (you.species == SP_NAGA) ? 20 - you.experience_level
            : 30 - (10 * you.mutation[MUT_SPIT_POISON] +
                    you.experience_level);
        break;

    case ABIL_MAPPING:
        failure = (you.species == SP_GNOME) ? 20 - you.experience_level
            : 30 - (10 * you.mutation[MUT_MAPPING] + you.experience_level);
        break;

    case ABIL_BREATHE_FIRE:
        failure = (you.species == SP_RED_DRACONIAN)
                        ? failure = 30 - you.experience_level
                        : failure = 50 - (10 * you.mutation[MUT_BREATHE_FLAMES]
                                                    + you.experience_level);

        if (you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
            failure -= 20;
        break;

    case ABIL_BREATHE_FROST:
    case ABIL_BREATHE_POISON:
    case ABIL_SPIT_ACID:
    case ABIL_BREATHE_LIGHTNING:
    case ABIL_BREATHE_POWER:
    case ABIL_BREATHE_STICKY_FLAME:
        failure = 30 - you.experience_level;

        if (you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
            failure -= 20;
        break;

    case ABIL_BREATHE_STEAM:
        failure = 20 - you.experience_level;

        if (you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON)
            failure -= 20;
        break;

    case ABIL_FLY:              // this is for kenku {dlb}
        failure = 45 - (3 * you.experience_level);
        break;

    case ABIL_FLY_II:           // this is for draconians {dlb}
        failure = 45 - (you.experience_level + you.strength);
        break;
        // end species abilties (some mutagenic)

        // begin demonic powers {dlb}
    case ABIL_THROW_FLAME:
    case ABIL_THROW_FROST:
        failure = 10 - you.experience_level;
        break;

    case ABIL_SUMMON_MINOR_DEMON:
        failure = 27 - you.experience_level;
        break;

    case ABIL_CHANNELING:
    case ABIL_BOLT_OF_DRAINING:
        failure = 30 - you.experience_level;
        break;

    case ABIL_CONTROL_DEMON:
        failure = 35 - you.experience_level;
        break;

    case ABIL_SUMMON_DEMON:
        failure = 40 - you.experience_level;
        break;

    case ABIL_TO_PANDEMONIUM:
        failure = 57 - (you.experience_level * 2);
        break;

    case ABIL_HELLFIRE:
    case ABIL_RAISE_DEAD:
        failure = 50 - you.experience_level;
        break;

    case ABIL_TORMENT:
        failure = 60 - you.experience_level;
        break;
        // end demonic powers {dlb}

        // begin transformation abilities {dlb}
    case ABIL_END_TRANSFORMATION:
        failure = 0;
        break;

    case ABIL_BREATHE_HELLFIRE:
        failure = 32 - you.experience_level;
        break;
        // end transformation abilities {dlb}
        // begin item abilities - some possibly mutagenic {dlb}
    case ABIL_BLINK:
        failure = 30 - (10 * you.mutation[MUT_BLINK]);
        break;

    case ABIL_TURN_INVISIBLE:
    case ABIL_LEVITATE:
        failure = 50 - (you.experience_level * 2);
        break;

    case ABIL_TURN_VISIBLE:
    case ABIL_STOP_LEVITATING:
        failure = 0;
        break;

    case ABIL_TELEPORTATION:
        failure = (you.mutation[MUT_TELEPORT_AT_WILL] > 1) ? 20 : 50;
        break;

    case ABIL_GO_BERSERK_I:
        failure = 50 - (you.experience_level * 2);

        if (player_genus(GENPC_DWARVEN) || you.species == SP_HILL_ORC
                                                    || you.species == SP_OGRE)
        {
            failure -= 10;
        }
        else if (you.species == SP_TROLL)
        {
            failure -= 30;
        }
        break;
        // end item abilities - some possibly mutagenic {dlb}

        // begin invocations {dlb}
    case ABIL_PURIFICATION:
        invoc = true;
        failure = 20 - (you.piety / 10) - (5 * you.skills[SK_INVOCATIONS]);
        break;

    case ABIL_REPEL_UNDEAD_I:
    case ABIL_REPEL_UNDEAD_II:
    case ABIL_RECALL_UNDEAD_SLAVES:
    case ABIL_MIGHT_I:
    case ABIL_LESSER_HEALING:
        invoc = true;
        failure = 30 - (you.piety / 10) - (6 * you.skills[SK_INVOCATIONS]);
        break;

    // These three are Trog abilities... Invocations means nothing -- bwr
    case ABIL_GO_BERSERK_II:    // piety >= 30
        invoc = true;
        failure = 30 - you.piety;       // starts at 0%
        break;

    case ABIL_MIGHT_II:         // piety >= 50
        invoc = true;
        failure = 80 - you.piety;       // starts at 30%
        break;

    case ABIL_HASTE_SELF:       // piety >= 100
        invoc = true;
        failure = 160 - you.piety;      // starts at 60%
        break;

    case ABIL_ANIMATE_CORPSE:
        invoc = true;
        failure = 40 - (you.piety / 10) - (3 * you.skills[SK_INVOCATIONS]);
        break;

    case ABIL_MINOR_HEALING:
    case ABIL_SMITING:
    case ABIL_HEALING_I:
    case ABIL_MINOR_DESTRUCTION:
    case ABIL_FORGET_SPELL:
    case ABIL_ENSLAVE_UNDEAD:
    case ABIL_ANIMATE_DEAD:
    case ABIL_LESSER_SERVANT_OF_MAKHLEB:
    case ABIL_HEALING_II:
        invoc = true;
        failure = 40 - (you.piety / 10) - (5 * you.skills[SK_INVOCATIONS]);
        break;

    case ABIL_CHANNEL_ENERGY:
        invoc = true;
        failure = 40 - you.intel - you.skills[SK_INVOCATIONS];
        break;

    case ABIL_RECALL_UNDEAD:
        invoc = true;
        failure = 50 - (you.piety / 10) - (you.skills[SK_INVOCATIONS] * 4);
        break;

    case ABIL_PESTILENCE:
    case ABIL_ANNIHILATE_UNDEAD:
        invoc = true;
        failure = 60 - (you.piety / 10) - (5 * you.skills[SK_INVOCATIONS]);
        break;

    case ABIL_MAJOR_DESTRUCTION:
    case ABIL_DRAIN_LIFE:
        invoc = true;
        failure = 60 - (you.piety / 15) - (you.skills[SK_INVOCATIONS] * 4);
        break;

    case ABIL_HOLY_WORD:
    case ABIL_THUNDERBOLT:
    case ABIL_RESTORATION:
    case ABIL_CONTROL_UNDEAD:
    case ABIL_HASTE:
    case ABIL_GREATER_SERVANT_OF_MAKHLEB:
        invoc = true;
        failure = 70 - (you.piety / 15) - (you.skills[SK_INVOCATIONS] * 4);
        break;

    case ABIL_SUMMON_GUARDIAN:
    case ABIL_SUMMON_DAEVA:
    case ABIL_INVOKE_DEATH:
    case ABIL_GREATER_HEALING:
        invoc = true;
        failure = 80 - (you.piety / 15) - (you.skills[SK_INVOCATIONS] * 4);
        break;

        //jmf: following for to-be-created gods
    case ABIL_CHARM_SNAKE:
        invoc = true;
        failure = 40 - (you.piety / 10) - (3 * you.skills[SK_INVOCATIONS]);
        break;

    case ABIL_TRAN_SERPENT_OF_HELL:
        invoc = true;
        failure = 80 - (you.piety / 15) - (you.skills[SK_INVOCATIONS] * 4);
        break;

    case ABIL_ROTTING:
        invoc = true;
        failure = 60 - (you.piety / 10) - (5 * you.skills[SK_INVOCATIONS]);
        break;

    case ABIL_TORMENT_II:
        invoc = true;
        failure = 70 - (you.piety / 15) - (you.skills[SK_INVOCATIONS] * 4);
        break;

    case ABIL_SHUGGOTH_SEED:
        invoc = true;
        failure = 85 - (you.piety / 15) - (you.skills[SK_INVOCATIONS] * 3);
        break;

    case ABIL_RENOUNCE_RELIGION:
        invoc = true;
        failure = 0;
        break;

        // end invocations {dlb}
    default:
        failure = -1;
        break;
    }

    // this will provide at least a slim chance of failure
    // note that I'm deliberately excluding "failure == 0"
    // cases here, as it seems unfair to penalize things like
    // turning visible or ending levitation {dlb}:
    if (failure < 0)
        failure = 1;

    (*p_pabils)->fail = failure;
    (*p_pabils)->is_invocation = invoc;
    (*p_pabils)++;

    return true;
}                               // end insert_ability()

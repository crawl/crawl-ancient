/*
 *  File:       spells.cc
 *  Summary:    Some spell related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 * <3> 19mar2000     jmf     Added silenced() calls; changed many "You conjure
 *                           up a <foo> explosion!" messages to "You are caught
 *                           in a <foo> explosion!" -- since they may have been
 *                           generated in Xom_acts()
 * <2>   10/1/99     BCR     Added messages for failed animate dead
 * <1>   -/--/--     LRH     Created
 */

/* This determines how likely it is that more powerful wild magic effects
 * will occur. Set to 100 for the old probabilities (although the individual
 * effects have been made much nastier since then).
 */
#define WILD_MAGIC_NASTINESS 150

#include "AppHdr.h"
#include "spells.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "beam.h"
#include "effects.h"
#include "it_use2.h"
#include "monplace.h"
#include "monstuff.h"
#include "mutation.h"
#include "ouch.h"
#include "player.h"
#include "spells1.h"
#include "spells3.h"
#include "stuff.h"
#include "view.h"

#ifdef MACROS
#include "macro.h"
#endif

bool miscast_effect(unsigned int sp_type, int mag_pow,
                    int mag_fail, int force_effect)
{
/*  sp_type is the type of the spell
 *  mag_pow is overall power of the spell or effect (ie its level)
 *  mag_fail is the degree to which you failed
 *  force_effect forces a certain effect to occur. Currently unused.
 */
    struct bolt beam;
    bool failMsg = true;

    int loopj = 0;
    int spec_effect = 0;
    int hurted = 0;

    if (sp_type == SPTYP_RANDOM)
        sp_type = 1 << (random2(12));

    spec_effect = (mag_pow * mag_fail * (10 + mag_pow) / 7
                                                * WILD_MAGIC_NASTINESS) / 100;

    if (force_effect == 100
        && random2(40) > spec_effect && random2(40) > spec_effect)
    {
        canned_msg(MSG_NOTHING_HAPPENS);
        return false;
    }

    // setup beam
    beam.isTracer = false;

    spec_effect = spec_effect / 100;

#ifdef WIZARD
    strcpy(info, "Sptype: ");
    itoa(sp_type, st_prn, 10);
    strcat(info, st_prn);
    strcat(info, ", failure1: ");
    itoa(spec_effect, st_prn, 10);
    strcat(info, st_prn);
#endif

    spec_effect = random2(spec_effect);

    if (spec_effect > 3)
        spec_effect = 3;
    else if (spec_effect < 0)
        spec_effect = 0;

#ifdef WIZARD
    strcat(info, ", failure2: ");
    itoa(spec_effect, st_prn, 10);
    strcat(info, st_prn);
    mpr(info);
#endif

    if (force_effect != 100)
        spec_effect = force_effect;

    switch (sp_type)
    {
    case SPTYP_CONJURATION:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
            switch (random2(10))
            {
            case 0:
                mpr("Sparks fly from your hands!");
                break;
            case 1:
                mpr("The air around you crackles with energy!");
                break;
            case 2:
                mpr("Wisps of smoke drift from your fingertips.");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("You are momentarily dazzled by a flash of light!");
                break;
            case 5:
                mpr("Strange energies run through your body.");
                break;
            case 6:
                mpr("Your skin tingles.");
                break;
            case 7:
                mpr("Your skin glows momentarily.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                // josh declares mummies cannot smell {dlb}
                if (you.species != SP_MUMMY)
                    mpr("You smell something strange.");
                break;
            }
            break;

        case 1:         // a bit less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("Smoke pours from your fingertips!");
                big_cloud(CLOUD_GREY_SMOKE, you.x_pos, you.y_pos, 20,
                          7 + random2(7));
                break;
            case 1:
                mpr("A wave of violent energy washes through your body!");
                mpr("Ouch.");
                ouch(6 + random2avg(7, 2), 0, KILLED_BY_WILD_MAGIC);
                break;
            }
            break;

        case 2:         // rather less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("Energy rips through your body!");
                mpr("Ouch!");
                ouch(9 + random2avg(17, 2), 0, KILLED_BY_WILD_MAGIC);
                break;
            case 1:
                mpr("You are caught in a violent explosion!");
                mpr("Oops.");
                beam.type = SYM_BURST;
                beam.damage = 112;
                beam.flavour = BEAM_MISSILE; // unsure about this
                // BEAM_EXPLOSION instead? {dlb}

                beam.target_x = you.x_pos;
                beam.target_y = you.y_pos;
                strcpy(beam.beam_name, "explosion");
                beam.colour = random_colour();
                beam.thrower = KILL_YOU;
                beam.ex_size = 1;

                explosion(beam);
                break;
            }
            break;

        case 3:         // considerably less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("You are blasted with magical energy!");
                ouch(12 + random2avg(29, 2), 0, KILLED_BY_WILD_MAGIC);
                break;
            case 1:
                mpr("There is a sudden explosion of magical energy!");
                beam.type = SYM_BURST;
                beam.damage = 120;
                beam.flavour = BEAM_MISSILE; // unsure about this
                // BEAM_EXPLOSION instead? {dlb}
                beam.target_x = you.x_pos;
                beam.target_y = you.y_pos;
                strcpy(beam.beam_name, "explosion");
                beam.colour = random_colour();
                beam.thrower = KILL_YOU;
                beam.ex_size = coinflip()?1:2;

                explosion(beam);
                break;
            }
            break;
        }
        break;                  // end conjuration

    case SPTYP_ENCHANTMENT:
        switch (spec_effect)
        {
        case 0:         // harmless messages only
            switch (random2(10))
            {
            case 0:
                mpr("Your hands glow momentarily.");
                break;
            case 1:
                mpr("The air around you crackles with energy!");
                break;
            case 2:
                mpr("Multicolored lights dance before your eyes!");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("Waves of light ripple over your body.");
                break;
            case 5:
                mpr("Strange energies run through your body.");
                break;
            case 6:
                mpr("Your skin tingles.");
                break;
            case 7:
                mpr("Your skin glows momentarily.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr("You hear something strange.");
                else
                    mpr("Your skull vibrates slightly.");
                break;
            }
            break;

        case 1:         // slightly annoying
            switch (random2(2))
            {
            case 0:
                potion_effect(POT_LEVITATION, 20);
                break;
            case 1:
                random_uselessness(2 + random2(7), 0);
                break;
            }
            break;

        case 2:         // much more annoying
            switch (random2(7))
            {
            case 0:
            case 1:
            case 2:
                mpr("You sense a malignant aura.");
                curse_an_item(0, 0);
                break;
            case 3:
            case 4:
            case 5:
                potion_effect(POT_SLOWING, 10);
                break;
            case 6:
                potion_effect(POT_BERSERK_RAGE, 10);
                break;
            }
            break;

        case 3:         // potentially lethal
            switch (random2(4))
            {
            case 0:
                do
                {
                    curse_an_item(0, 0);
                    loopj = random2(3);
                }
                while (loopj != 0);

                mpr("You sense an overwhelmingly malignant aura!");
                break;
            case 1:
                potion_effect(POT_PARALYSIS, 10);
                break;
            case 2:
                potion_effect(POT_CONFUSION, 10);
                break;
            case 3:
                mpr("You feel saturated with unharnessed energies!");
                you.magic_contamination += random2avg(19,3);
                break;
            }
            break;
        }
        break;                  // end enchantments

    case SPTYP_TRANSLOCATION:
        switch (spec_effect)
        {
        case 0:         // harmless messages only
            switch (random2(10))
            {
            case 0:
                mpr("Space warps around you.");
                break;
            case 1:
                mpr("The air around you crackles with energy!");
                break;
            case 2:
                mpr("You feel a wrenching sensation.");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("You spin around.");
                break;
            case 5:
                mpr("Strange energies run through your body.");
                break;
            case 6:
                mpr("Your skin tingles.");
                break;
            case 7:
                mpr("The world appears momentarily distorted!");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                mpr("You feel uncomfortable.");
                break;
            }
            break;

        case 1:         // mostly harmless
            switch (random2(6))
            {
            case 0:
            case 1:
            case 2:
                mpr("You are caught in a localised field of spatial distortion.");
                mpr("Ouch!");
                ouch(4 + random2avg(9, 2), 0, KILLED_BY_WILD_MAGIC);
                break;
            case 3:
            case 4:
                mpr("Space bends around you!");
                random_blink(false);
                ouch(4 + random2avg(7, 2), 0, KILLED_BY_WILD_MAGIC);
                break;
            case 5:
                mpr("Space twists in upon itself!");
                create_monster(MONS_SPATIAL_VORTEX, ENCH_ABJ_III, BEH_HOSTILE,
                               you.x_pos, you.y_pos, MHITNOT, 250);
                break;
            }
            break;

        case 2:         // less harmless
            switch (random2(6))
            {
            case 0:
            case 1:
            case 2:
                mpr("You are caught in a strong localised spatial distortion.");
                mpr("Ouch!!");
                ouch(9 + random2avg(23, 2), 0, KILLED_BY_WILD_MAGIC);
                break;
            case 3:
            case 4:
                mpr("Space warps around you!");
                if (one_chance_in(3))
                    you_teleport2(true);
                else
                    random_blink(false);
                ouch(5 + random2avg(9, 2), 0, KILLED_BY_WILD_MAGIC);
                potion_effect(POT_CONFUSION, 10);
                break;
            case 5:
                mpr("Space twists in upon itself!");

                for (loopj = 0; loopj < 2 + random2(3); loopj++)
                {
                    create_monster(MONS_SPATIAL_VORTEX, ENCH_ABJ_III, BEH_HOSTILE,
                                   you.x_pos, you.y_pos, MHITNOT, 250);
                }
                break;
            }
            break;

        case 3:         // much less harmless

            switch (random2(4))
            {
            case 0:
                mpr("You are caught in an extremely strong localised spatial distortion!");
                mpr("Ouch!!!");
                ouch(15 + random2avg(29, 2), 0, KILLED_BY_WILD_MAGIC);
                break;
            case 1:
                mpr("Space warps crazily around you!");
                you_teleport2(true);
                ouch(9 + random2avg(17, 2), 0, KILLED_BY_WILD_MAGIC);
                potion_effect(POT_CONFUSION, 30);
                break;
            case 2:
                mpr("You are cast into the Abyss!");
                more();
                banished(DNGN_ENTER_ABYSS);     // sends you to the abyss
                break;
            case 3:
                mpr("You feel saturated with unharnessed energies!");
                you.magic_contamination += random2avg(19,3);
                break;
            }
            break;
        }
        break;                  // end translocations

    case SPTYP_SUMMONING:
        switch (spec_effect)
        {
        case 0:         // harmless messages only
            switch (random2(10))
            {
            case 0:
                mpr("Shadowy shapes form in the air around you, then vanish.");
                break;
            case 1:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr("You hear strange voices.");
                else
                    mpr("You feel momentarily dizzy.");
                break;
            case 2:
                mpr("Your head hurts.");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("Your brain hurts!");
                break;
            case 5:
                mpr("Strange energies run through your body.");
                break;
            case 6:
                mpr("The world appears momentarily distorted.");
                break;
            case 7:
                mpr("Space warps around you.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                mpr("Distant voices call out to you!");
                break;
            }
            break;

        case 1:         // a little bad
            switch (random2(6))
            {
            case 0:             // identical to translocation
            case 1:
            case 2:
                mpr("You are caught in a localised spatial distortion.");
                mpr("Ouch!");
                ouch(5 + random2avg(9, 2), 0, KILLED_BY_WILD_MAGIC);
                break;

            case 3:
                mpr("Space twists in upon itself!");
                create_monster(MONS_SPATIAL_VORTEX, ENCH_ABJ_III, BEH_HOSTILE,
                               you.x_pos, you.y_pos, MHITNOT, 250);
                break;

            case 4:
            case 5:
                if (create_monster(summon_any_demon(DEMON_LESSER), ENCH_ABJ_V,
                                    BEH_HOSTILE, you.x_pos, you.y_pos,
                                    MHITNOT, 250) != -1)
                {
                    mpr("Something appears in a flash of light!");
                }
                break;
            }

        case 2:         // more bad
            switch (random2(6))
            {
            case 0:
                mpr("Space twists in upon itself!");

                for (loopj = 0; loopj < 2 + random2(3); loopj++)
                {
                    create_monster(MONS_SPATIAL_VORTEX, ENCH_ABJ_III, BEH_HOSTILE,
                                   you.x_pos, you.y_pos, MHITNOT, 250);
                }
                break;

            case 1:
            case 2:
                if (create_monster(summon_any_demon(DEMON_COMMON), ENCH_ABJ_V,
                                    BEH_HOSTILE, you.x_pos, you.y_pos,
                                    MHITNOT, 250) != -1)
                {
                    mpr("Something forms out of thin air!");
                }
                break;

            case 3:
            case 4:
            case 5:
                mpr("A chorus of chattering voices calls out to you!");
                create_monster(summon_any_demon(DEMON_LESSER), ENCH_ABJ_V,
                               BEH_HOSTILE, you.x_pos, you.y_pos, MHITNOT,
                               250);

                create_monster(summon_any_demon(DEMON_LESSER), ENCH_ABJ_V,
                               BEH_HOSTILE, you.x_pos, you.y_pos, MHITNOT,
                               250);

                if (coinflip())
                {
                    create_monster(summon_any_demon(DEMON_LESSER), ENCH_ABJ_V,
                                   BEH_HOSTILE, you.x_pos, you.y_pos,
                                   MHITNOT, 250);
                }

                if (coinflip())
                {
                    create_monster(summon_any_demon(DEMON_LESSER), ENCH_ABJ_V,
                                   BEH_HOSTILE, you.x_pos, you.y_pos,
                                   MHITNOT, 250);
                }
                break;
            }
            break;

        case 3:         // more bad
            switch (random2(4))
            {
            case 0:
                if (create_monster(MONS_ABOMINATION_SMALL, 0, BEH_HOSTILE,
                                    you.x_pos, you.y_pos, MHITNOT, 250) != -1)
                {
                    mpr("Something forms out of thin air.");
                }
                break;

            case 1:
                mpr("You sense a hostile presence.");
                create_monster(summon_any_demon(DEMON_GREATER), 0,
                                BEH_HOSTILE, you.x_pos, you.y_pos,
                                MHITNOT, 250);
                break;

            case 2:
                mpr("Something turns its malign attention towards you...");

                create_monster(summon_any_demon(DEMON_COMMON), ENCH_ABJ_III,
                               BEH_HOSTILE, you.x_pos, you.y_pos, MHITNOT,
                               250);

                create_monster(summon_any_demon(DEMON_COMMON), ENCH_ABJ_III,
                               BEH_HOSTILE, you.x_pos, you.y_pos, MHITNOT,
                               250);

                if (coinflip())
                    create_monster(summon_any_demon(DEMON_COMMON), ENCH_ABJ_III,
                                   BEH_HOSTILE, you.x_pos, you.y_pos,
                                   MHITNOT, 250);
                break;

            case 3:
                mpr("You are cast into the Abyss!");
                banished(DNGN_ENTER_ABYSS);
                break;
            }
            break;
        }                       // end Summonings
        break;

    case SPTYP_DIVINATION:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
            switch (random2(10))
            {
            case 0:
                mpr("Weird images run through your mind.");
                break;
            case 1:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr("You hear strange voices.");
                else
                    mpr("Your nose twitches.");
                break;
            case 2:
                mpr("Your head hurts.");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("Your brain hurts!");
                break;
            case 5:
                mpr("Strange energies run through your body.");
                break;
            case 6:
                mpr("Everything looks hazy for a moment.");
                break;
            case 7:
                mpr("You seem to have forgotten something, but you can't remember what it was!");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                mpr("You feel uncomfortable.");
                break;
            }
            break;

        case 1:         // more annoying things
            switch (random2(2))
            {
            case 0:
                mpr("You feel slightly disoriented.");
                forget_map(10 + random2(10));
                break;
            case 1:
                potion_effect(POT_CONFUSION, 1);
                break;
            }
            break;

        case 2:         // even more annoying things
            switch (random2(2))
            {
            case 0:
                if (you.is_undead)
                    mpr("You suddenly recall your previous life!");
                else if (lose_stat(STAT_INTELLIGENCE, 1 + random2(3)))
                    mpr("You have damaged your brain!");
                else
                    mpr("You have a terrible headache.");
                break;
            case 1:
                mpr("You feel lost.");
                forget_map(40 + random2(40));
                break;
            }
            potion_effect(POT_CONFUSION, 1);  // common to all cases here {dlb}
            break;

        case 3:         // nasty
            switch (random2(3))
            {
            case 0:
                mpr( forget_spell() ? "You have forgotten a spell!"
                                    : "You get a splitting headache." );
                break;
            case 1:
                mpr("You feel completely lost.");
                forget_map(100);
                break;
            case 2:
                if (you.is_undead)
                    mpr("You suddenly recall your previous life.");
                else if (lose_stat(STAT_INTELLIGENCE, 3 + random2(3)))
                    mpr("You have damaged your brain!");
                else
                    mpr("You have a terrible headache.");
                break;
            }
            potion_effect(POT_CONFUSION, 1);  // common to all cases here {dlb}
            break;
        }
        break;                  // end divinations

    case SPTYP_NECROMANCY:
        if (you.religion == GOD_KIKUBAAQUDGHA
            && (!player_under_penance() && you.piety >= 50
                && random2(150) <= you.piety))
        {
            canned_msg(MSG_NOTHING_HAPPENS);
            break;
        }

        switch (spec_effect)
        {
        case 0:
            switch (random2(10))
            {
            case 0:
                // josh declares mummies cannot smell {dlb}
                if (you.species != SP_MUMMY)
                    mpr("You smell decay.");
                break;
            case 1:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr("You hear strange and distant voices.");
                else
                    mpr("You feel homesick.");
                break;
            case 2:
                mpr("Pain shoots through your body.");
                break;
            case 3:
                mpr("Your bones ache.");
                break;
            case 4:
                mpr("The world around you seems to dim momentarily.");
                break;
            case 5:
                mpr("Strange energies run through your body.");
                break;
            case 6:
                mpr("You shiver with cold.");
                break;
            case 7:
                mpr("You sense a malignant aura.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                mpr("You feel very uncomfortable.");
                break;
            }
            break;

        case 1:         // a bit nasty
            switch (random2(3))
            {
            case 0:
                if (you.is_undead)
                {
                    mpr("You feel weird for a moment.");
                    break;
                }
                mpr("Pain shoots through your body!");
                ouch(5 + random2avg(15, 2), 0, KILLED_BY_WILD_MAGIC);
                break;
            case 1:
                mpr("You feel horribly lethargic.");
                potion_effect(POT_SLOWING, 15);
                break;
            case 2:
                // josh declares mummies cannot smell, and they do not rot{dlb}
                if (you.species != SP_MUMMY)
                {
                    mpr("You smell decay."); // identical to a harmless message
                    you.rotting++;
                }
                break;
            }
            break;

        case 2:         // much nastier
            switch (random2(3))
            {
            case 0:
                mpr("Flickering shadows surround you.");

                create_monster(MONS_SHADOW, ENCH_ABJ_II, BEH_HOSTILE, you.x_pos,
                               you.y_pos, MHITNOT, 250);

                if (coinflip())
                    create_monster(MONS_SHADOW, ENCH_ABJ_II, BEH_HOSTILE, you.x_pos,
                                   you.y_pos, MHITNOT, 250);

                if (coinflip())
                    create_monster(MONS_SHADOW, ENCH_ABJ_II, BEH_HOSTILE, you.x_pos,
                                   you.y_pos, MHITNOT, 250);
                break;

            case 1:
                if (!player_prot_life() && one_chance_in(3))
                {
                    drain_exp();
                    break;
                }               // otherwise it just flows through...

            case 2:
                if (you.is_undead)
                {
                    mpr("You feel weird for a moment.");
                    break;
                }
                mpr("You convulse helplessly as pain tears through your body!");
                ouch(15 + random2avg(23, 2), 0, KILLED_BY_WILD_MAGIC);
                break;
            }
            break;

        case 3:         // even nastier
            switch (random2(6))
            {
            case 0:
                if (you.is_undead)
                {
                    mpr("Something just walked over your grave. No, really!");
                    break;
                }
                mpr("Your body is wracked with pain!");

                dec_hp((you.hp / 2) - 1, false);
                break;

            case 1:
                mpr("You are engulfed in negative energy!");

                if (!player_prot_life())
                {
                    drain_exp();
                    break;
                }               // otherwise it just flows through...

            case 2:
                lose_stat(STAT_RANDOM, 1 + random2avg(7, 2));
                break;

            case 3:
                if (you.is_undead)
                {
                    mpr("You feel terrible.");
                    break;
                }
                mpr("You feel your flesh start to rot away!");
                you.rotting += random2avg(7, 2) + 1;
                break;

            case 4:
                if (create_monster(MONS_SOUL_EATER, ENCH_ABJ_IV, BEH_HOSTILE,
                                    you.x_pos, you.y_pos, MHITNOT, 250) != -1)
                {
                    mpr("Something reaches out for you...");
                }
                break;

            case 5:
                if (create_monster(MONS_REAPER, ENCH_ABJ_IV, BEH_HOSTILE,
                                    you.x_pos, you.y_pos, MHITNOT, 250) != -1)
                {
                    mpr("Death has come for you...");
                }
                break;
            }
            break;
        }
        break;                  // end necromancy

    case SPTYP_TRANSMIGRATION:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
            switch (random2(10))
            {
            case 0:
                mpr("Your hands glow momentarily.");
                break;
            case 1:
                mpr("The air around you crackles with energy!");
                break;
            case 2:
                mpr("Multicolored lights dance before your eyes!");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("Waves of light ripple over your body.");
                break;
            case 5:
                mpr("Strange energies run through your body.");
                break;
            case 6:
                mpr("Your skin tingles.");
                break;
            case 7:
                mpr("Your skin glows momentarily.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                // josh declares mummies cannot smell {dlb}
                if (you.species != SP_MUMMY)
                    mpr("You smell something strange.");
                break;
            }
            break;

        case 1:         // slightly annoying
            switch (random2(2))
            {
            case 0:
                mpr("Your body is twisted painfully.");
                ouch(1 + random2avg(11, 2), 0, KILLED_BY_WILD_MAGIC);
                break;
            case 1:
                random_uselessness(2 + random2(7), 0);
                break;
            }
            break;

        case 2:         // much more annoying
            switch (random2(4))
            {
            case 0:
                mpr("Your body is twisted very painfully!");
                ouch(3 + random2avg(23, 2), 0, KILLED_BY_WILD_MAGIC);
                break;
            case 1:
                mpr("You feel saturated with unharnessed energies!");
                you.magic_contamination += random2avg(19,3);
                break;
            case 2:
                potion_effect(POT_PARALYSIS, 10);
                break;
            case 3:
                potion_effect(POT_CONFUSION, 10);
                break;
            }
            break;

        case 3:         // even nastier

            switch (random2(3))
            {
            unsigned char i;
            case 0:
                mpr("Your body is flooded with distortional energies!");
                you.magic_contamination += random2avg(35, 3);

                ouch(3 + random2avg(18, 2), 0, KILLED_BY_WILD_MAGIC);
                break;

            case 1:
                mpr("You feel very strange.");
                delete_mutation(100);
                ouch(5 + random2avg(23, 2), 0, KILLED_BY_WILD_MAGIC);
                break;

            case 2:
                mpr("Your body is distorted in a weirdly horrible way!");
                failMsg = !give_bad_mutation();
                if (one_chance_in(2))
                    give_bad_mutation(failMsg);

                ouch(5 + random2avg(23, 2), 0, KILLED_BY_WILD_MAGIC);
                break;
            }
            break;
        }
        break;                  // end transmigrations

    case SPTYP_FIRE:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
            switch (random2(10))
            {
            case 0:
                mpr("Sparks fly from your hands!");
                break;
            case 1:
                mpr("The air around you burns with energy!");
                break;
            case 2:
                mpr("Wisps of smoke drift from your fingertips.");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                // josh declares mummies cannot smell {dlb}
                if (you.species != SP_MUMMY)
                    mpr("You smell smoke.");
                break;
            case 5:
                mpr("Heat runs through your body.");
                break;
            case 6:
                mpr("You feel uncomfortably hot.");
                break;
            case 7:
                mpr("Lukewarm flames ripple over your body.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr("You hear a sizzling sound.");
                else
                    mpr("You feel like you have heartburn.");
                break;
            }
            break;

        case 1:         // a bit less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("Smoke pours from your fingertips!");
                big_cloud(CLOUD_GREY_SMOKE + random2(3), you.x_pos, you.y_pos,
                          20, 7 + random2(7));
                break;
            case 1:
                mpr("Flames sear your flesh.");
                scrolls_burn(3, OBJ_SCROLLS);

                if (player_res_fire() < 100)
                    ouch(2 + random2avg(13, 2), 0, KILLED_BY_WILD_MAGIC);
                break;
            }
            break;

        case 2:         // rather less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("You are blasted with fire.");

                ouch(check_your_resists(5 + random2avg(29, 2), 2), 0,
                     KILLED_BY_WILD_MAGIC);

                scrolls_burn(5, OBJ_SCROLLS);
                break;
            case 1:
                mpr("You are caught a fiery explosion!");
                mpr("Oops.");

                beam.type = SYM_BURST;
                beam.damage = 114;
                beam.flavour = BEAM_FIRE;
                beam.target_x = you.x_pos;
                beam.target_y = you.y_pos;
                strcpy(beam.beam_name, "explosion");
                beam.colour = RED;
                beam.thrower = KILL_YOU;
                beam.ex_size = 1;

                explosion(beam);
                break;
            }
            break;

        case 3:         // considerably less harmless stuff
            switch (random2(3))
            {
            case 0:
                mpr("You are blasted with searing flames!");

                ouch(check_your_resists(9 + random2avg(33, 2), 2), 0,
                     KILLED_BY_WILD_MAGIC);

                scrolls_burn(10, OBJ_SCROLLS);
                break;
            case 1:
                mpr("There is a sudden and violent explosion of flames!");

                beam.type = SYM_BURST;
                beam.damage = 120;
                beam.flavour = BEAM_FIRE;
                beam.target_x = you.x_pos;
                beam.target_y = you.y_pos;
                strcpy(beam.beam_name, "fireball");
                beam.colour = RED;
                beam.thrower = KILL_YOU;
                beam.ex_size = coinflip()?1:2;

                explosion(beam);
                break;

            case 2:
                mpr("You are covered in liquid fire!");
                you.duration[DUR_LIQUID_FLAMES] += random2avg(7, 3) + 1;
                break;
            }
            break;
        }
        break;                  // end fire

    case SPTYP_ICE:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
            switch (random2(10))
            {
            case 0:
                mpr("You shiver with cold.");
                break;
            case 1:
                mpr("A chill runs through your body.");
                break;
            case 2:
                mpr("Wisps of condensation drift from your fingertips.");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("Your hands feel numb with cold.");
                break;
            case 5:
                mpr("A chill runs through your body.");
                break;
            case 6:
                mpr("You feel uncomfortably cold.");
                break;
            case 7:
                mpr("Frost covers your body.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr("You hear a crackling sound.");
                else
                    mpr("A snowflake lands on your nose.");
                break;
            }
            break;

        case 1:         // a bit less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("You feel extremely cold.");
                break;
            case 1:
                mpr("You are covered in a thin layer of ice");
                scrolls_burn(2, OBJ_POTIONS);

                if (player_res_cold() < 100)
                    ouch(4 + random2avg(5, 2), 0, KILLED_BY_WILD_MAGIC);
                break;
            }
            break;

        case 2:         // rather less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("Heat is drained from your body.");

                ouch(check_your_resists(5 + random2(6) + random2(7), 3), 0,
                     KILLED_BY_WILD_MAGIC);

                scrolls_burn(4, OBJ_POTIONS);
                break;

            case 1:
                mpr("You are caught in an explosion of ice and frost!");
                mpr("Oops.");

                beam.type = SYM_BURST;
                beam.damage = 111;
                beam.flavour = BEAM_COLD;
                beam.target_x = you.x_pos;
                beam.target_y = you.y_pos;
                strcpy(beam.beam_name, "explosion");
                beam.colour = WHITE;
                beam.thrower = KILL_YOU;
                beam.ex_size = 1;

                explosion(beam);
                break;
            }
            break;

        case 3:         // less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("You are blasted with ice!");

                ouch(check_your_resists(9 + random2avg(23, 2), 3), 0,
                     KILLED_BY_WILD_MAGIC);

                scrolls_burn(9, OBJ_POTIONS);
                break;
            case 1:
                mpr("Freezing gasses pour from your hands!");

                big_cloud(CLOUD_COLD, you.x_pos, you.y_pos, 20,
                          8 + random2(4));
                break;
            }
            break;
        }
        break;                  // end ice

    case SPTYP_EARTH:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
        case 1:
            switch (random2(10))
            {
            case 0:
                mpr("You feel earthy.");
                break;
            case 1:
                mpr("You are showered with tiny particles of grit.");
                break;
            case 2:
                mpr("Sand pours from your fingers.");
                break;
            case 3:
                mpr("You feel a surge of energy from the ground.");
                break;
            case 4:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr("You hear a distant rumble.");
                else
                    mpr("You sympathise with the stones.");
                break;
            case 5:
                mpr("You feel gritty.");
                break;
            case 6:
                mpr("You feel momentarily lethargic.");
                break;
            case 7:
                mpr("Motes of dust swirl before your eyes.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                strcpy(info, "Your ");
                strcat(info, (you.species == SP_NAGA)    ? "underbelly feels" :
                             (you.species == SP_CENTAUR) ? "hooves feel"
                                                         : "feet feel");
                strcat(info, " warm.");
                mpr(info);
                break;
            }
            break;

        case 2:         // slightly less harmless stuff
            switch (random2(1))
            {
            case 0:
                switch (random2(3))
                {
                case 0:
                    mpr("You are hit by flying rocks!");
                    break;
                case 1:
                    mpr("You are blasted with sand!");
                    break;
                case 2:
                    mpr("Rocks fall onto you out of nowhere!");
                    break;
                }

                hurted = random2avg(13, 2) + 10;
                hurted -= random2(1 + player_AC());

                ouch(hurted, 0, KILLED_BY_WILD_MAGIC);
                break;
            }
            break;

        case 3:         // less harmless stuff
            switch (random2(1))
            {
            case 0:
                mpr("You are caught in an explosion of flying shrapnel!");
                mpr("Oops.");

                beam.type = SYM_BURST;
                beam.damage = 115;
                beam.flavour = BEAM_FRAG;
                beam.target_x = you.x_pos;
                beam.target_y = you.y_pos;
                strcpy(beam.beam_name, "explosion");
                beam.colour = CYAN;

                if (one_chance_in(5))
                    beam.colour = BROWN;
                if (one_chance_in(5))
                    beam.colour = LIGHTCYAN;

                beam.thrower = KILL_YOU;
                beam.ex_size = 1;

                explosion(beam);
                break;
            }
            break;
        }
        break;                  // end Earth

    case SPTYP_AIR:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
            switch (random2(10))
            {
            case 0:
                mpr("Ouch! You gave yourself an electric shock.");
                break;
            case 1:
                mpr("You feel momentarily weightless.");
                break;
            case 2:
                mpr("Wisps of vapour drift from your fingertips.");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("You feel electric!");
                break;
            case 5:
                mpr("Sparks of electricity dance on your fingertips.");
                break;
            case 6:
                mpr("You are blasted with air!");
                break;
            case 7:
                // josh declares mummies cannot smell {dlb}
                if (!silenced(you.x_pos, you.y_pos))
                    mpr("You hear a whooshing sound.");
                else if (you.species != SP_MUMMY)
                    mpr("You smell ozone.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                // josh declares mummies cannot smell {dlb}
                if (!silenced(you.x_pos, you.y_pos))
                    mpr("You hear a crackling sound.");
                else if (you.species != SP_MUMMY)
                    mpr("You smell something musty.");
                break;
            }
            break;

        case 1:         // a bit less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("There is a shower of sparks.");
                break;
            case 1:
                mpr("The wind howls around you!");
                break;
            }
            break;

        case 2:         // rather less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("Electricity courses through your body.");
                ouch(check_your_resists(4 + random2avg(9, 2), 5), 0,
                     KILLED_BY_WILD_MAGIC);
                break;
            case 1:
                mpr("Noxious gasses pour from your hands!");
                big_cloud(CLOUD_STINK, you.x_pos, you.y_pos, 20,
                          9 + random2(4));
                break;
            }
            break;

        case 3:         // less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("You are caught in an explosion of electrical discharges!");
                mpr("Oops.");

                beam.type = SYM_BURST;
                beam.damage = 108;
                beam.flavour = BEAM_ELECTRICITY;
                beam.target_x = you.x_pos;
                beam.target_y = you.y_pos;
                strcpy(beam.beam_name, "explosion");
                beam.colour = LIGHTBLUE;
                beam.thrower = KILL_YOU;
                beam.ex_size = one_chance_in(4)?1:2;

                explosion(beam);
                break;
            case 1:
                mpr("Venomous gasses pour from your hands!");
                big_cloud( CLOUD_POISON, you.x_pos, you.y_pos, 20,
                           8 + random2(5) );
                break;
            }
            break;
        }
        break;                  // end air

    case SPTYP_POISON:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
            switch (random2(10))
            {
            case 0:
                mpr("You feel mildly nauseous.");
                break;
            case 1:
                mpr("You feel slightly ill.");
                break;
            case 2:
                mpr("Wisps of poison gas drift from your fingertips.");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("You feel faint for a moment.");
                break;
            case 5:
                mpr("You feel sick.");
                break;
            case 6:
                mpr("You feel odd.");
                break;
            case 7:
                mpr("You feel weak for a moment.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr("You hear a slurping sound.");
                else
                    mpr("You taste almonds.");
                break;
            }
            break;

        case 1:         // a bit less harmless stuff
            switch (random2(2))
            {
            case 0:
                if (player_res_poison())
                {
                    canned_msg(MSG_NOTHING_HAPPENS);
                    return false;
                }

                mpr("You feel sick.");
                you.poison += 2 + random2(3);
                break;
            case 1:
                mpr("Noxious gasses pour from your hands!");
                place_cloud(CLOUD_STINK, you.x_pos, you.y_pos,
                            2 + random2(4));
                break;
            }
            break;

        case 2:         // rather less harmless stuff
            switch (random2(3))
            {
            case 0:
                if (player_res_poison())
                {
                    canned_msg(MSG_NOTHING_HAPPENS);
                    return false;
                }

                mpr("You feel very sick.");
                you.poison += 3 + random2avg(9, 2);
                break;

            case 1:
                mpr("Noxious gasses pour from your hands!");
                big_cloud(CLOUD_STINK, you.x_pos, you.y_pos, 20,
                          8 + random2(5));
                break;

            case 2:
                if (player_res_poison())
                {
                    canned_msg(MSG_NOTHING_HAPPENS);
                    return false;
                }
                lose_stat(STAT_RANDOM, 1);
                break;
            }
            break;

        case 3:         // less harmless stuff
            switch (random2(3))
            {
            case 0:
                if (player_res_poison())
                {
                    canned_msg(MSG_NOTHING_HAPPENS);
                    return false;
                }

                mpr("You feel incredibly sick.");
                you.poison += 10 + random2avg(19, 2);
                break;
            case 1:
                mpr("Venomous gasses pour from your hands!");
                big_cloud(CLOUD_POISON, you.x_pos, you.y_pos, 20,
                          7 + random2(7));
                break;
            case 2:
                if (player_res_poison())
                {
                    canned_msg(MSG_NOTHING_HAPPENS);
                    return false;
                }

                lose_stat(STAT_RANDOM, 1 + random2avg(5, 2));
                break;
            }
            break;
        }
        break;                  // end poison
    }

    return true;
}                               // end miscast_effect()

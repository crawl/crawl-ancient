/*
 *  File:       skills.cc
 *  Summary:    Skill exercising functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *       <3>     8/08/99        BWR             Increased skill cost in midgame
 *
 *       <2>     7/31/99        BWR             Inc skill_point granularity,
 *                                                  added MAX_SPENDING_LIMIT
 *       <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "skills.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "externs.h"

#include "macro.h"
#include "player.h"
#include "skills2.h"
#include "stuff.h"


// MAX_COST_LIMIT is the maximum XP amount it will cost to raise a skill
//                by 10 skill points (ie one standard practise).
//
// MAX_SPENDING_LIMIT is the maximum XP amount we allow the player to
//                    spend on a skill in a single raise.
//
// Note that they don't have to be equal, but it is important to make
// sure that they're set so that the spending limit will always allow
// for 1 skill point to be earned.
#define MAX_COST_LIMIT           250
#define MAX_SPENDING_LIMIT       250

static int exercise2( int exsk );

// These values were calculated by running a simulation of gaining skills.
// The goal is to try and match the old cost system which used the player's
// experience level (which has a number of problems) so things shouldn't
// seem too different to the player... but we still try to err on the
// high side for the lower levels.  This system has a nice tradeoff
// property to it... classes which start with less skills get a longer
// period at a cheaper rate to compensate. -- bwr
int skill_cost_needed( int level )
{
    // The average starting skill total is actually lower, but
    // some classes get about 2200, and they would probably
    // start around skill cost level 3 if we used the average.  -- bwr
    int ret = 2200;

    switch (level)
    {
    case 1: ret = 0; break;

    case 2:  ret +=   250; break; //  250 -- big because of initial 25 pool
    case 3:  ret +=   350; break; //  100
    case 4:  ret +=   550; break; //  200
    case 5:  ret +=   900; break; //  350
    case 6:  ret +=  1300; break; //  400
    case 7:  ret +=  1900; break; //  600
    case 8:  ret +=  2800; break; //  900
    case 9:  ret +=  4200; break; // 1400
    case 10: ret +=  5900; break; // 1700
    case 11: ret +=  9000; break; // 3100

    default:
        ret += 9000 + (4000 * (level - 11));
        break;
    }

    return (ret);
}

void calc_total_skill_points( void )
{
    int i;

    you.total_skill_points = 0;

    for (i = 0; i < NUM_SKILLS; i++)
    {
        you.total_skill_points += you.skill_points[i];
    }

    for (i = 1; i <= 27; i++)
    {
        if (you.total_skill_points < skill_cost_needed(i))
            break;
    }

    you.skill_cost_level = i - 1;

#if DEBUG_DIAGNOSTICS
    set_redraw_status( REDRAW_EXPERIENCE );
#endif
}

// skill_cost_level makes skills more expensive for more experienced characters
// skill_level      makes higher skills more expensive
static int calc_skill_cost( int skill_cost_level, int skill_level )
{
    int ret = 1 + skill_level;

    // does not yet allow for loss of skill levels.
    if (skill_level > 9)
    {
        ret *= (skill_level - 7);
        ret /= 3;
    }

    if (skill_cost_level > 4)
        ret += skill_cost_level - 4;
    if (skill_cost_level > 7)
        ret += skill_cost_level - 7;
    if (skill_cost_level > 10)
        ret += skill_cost_level - 10;
    if (skill_cost_level > 13)
        ret += skill_cost_level - 13;
    if (skill_cost_level > 16)
        ret += skill_cost_level - 16;

    if (skill_cost_level > 10)
    {
        ret *= (skill_cost_level - 5);
        ret /= 5;
    }

    if (skill_level > 7)
        ret += 1;
    if (skill_level > 9)
        ret += 2;
    if (skill_level > 11)
        ret += 3;
    if (skill_level > 13)
        ret += 4;
    if (skill_level > 15)
        ret += 5;

    if (ret > MAX_COST_LIMIT)
        ret = MAX_COST_LIMIT;

    return (ret);
}

// returns total number of skill points gained
int exercise( int exsk, int deg )
{
    int ret = 0;

    while (deg > 0)
    {
        if (you.exp_available <= 0 || you.skills[exsk] >= 27)
            break;

        if (you.practise_skill[exsk] || one_chance_in(4))
            ret += exercise2( exsk );

        deg--;
    }

    return (ret);
}                               // end exercise()

// returns true
static int exercise2( int exsk )
{
    int deg = 1;
    int bonus = 0;
    int old_best_skill = best_skill( SK_FIGHTING, (NUM_SKILLS - 1) );

    int skill_cost = calc_skill_cost( you.skill_cost_level, you.skills[exsk] );
    int i;

    if (exsk < SK_RANGED_COMBAT)
    {
        // being good at some weapons makes others easier to learn:
        static const int transferable[SK_RANGED_COMBAT][2] =
        {
            { SK_NONE,          SK_NONE },          // SK_FIGHTING
            { SK_LONG_SWORDS,   SK_NONE },          // SK_SHORT_BLADES
            { SK_SHORT_BLADES,  SK_NONE },          // SK_LONG_SWORDS
            { SK_NONE,          SK_NONE },          // SK_UNUSED_1
            { SK_POLEARMS,      SK_MACES_FLAILS },  // SK_AXES
            { SK_AXES,          SK_NONE },          // SK_MACES_FLAILS
            { SK_AXES,          SK_STAVES },        // SK_POLEARMS
            { SK_POLEARMS,      SK_NONE },          // SK_STAVES
            { SK_DARTS,         SK_NONE },          // SK_SLINGS
            { SK_CROSSBOWS,     SK_NONE },          // SK_BOWS
            { SK_BOWS,          SK_NONE },          // SK_CROSSBOWS
            { SK_SLINGS,        SK_NONE },          // SK_DARTS
        };

        for (i = 0; i < 2; i++)
        {
            if (transferable[exsk][i] == SK_NONE)
                break;

            if (you.skills[ transferable[exsk][i] ] > you.skills[exsk])
            {
                bonus += random2(3);
                break;               // only given once, even if more apply
            }
        }
    }
    else if (exsk >= SK_SPELLCASTING)
    {
        // spell casting is cheaper early on, and elementals hinder each other
        if (you.skill_cost_level < 5)
        {
            skill_cost /= 2;
        }
        else if (you.skill_cost_level < 15)
        {
            skill_cost *= (10 + (you.skill_cost_level - 5));
            skill_cost /= 20;
        }

        // being good at elemental magic makes other elements harder to learn:
        if (exsk >= SK_FIRE_MAGIC && exsk <= SK_EARTH_MAGIC)
        {
            static const int elemental_ring[4] =
                { SK_FIRE_MAGIC, SK_EARTH_MAGIC, SK_ICE_MAGIC, SK_AIR_MAGIC };

            // any element greater than exsk cancels 1 in 3
            int id = -1;
            for (i = 0; i < 4; i++)
            {
                if (exsk == elemental_ring[i])
                {
                    id = i;
                }
                else if (you.skills[ elemental_ring[i] ] > you.skills[exsk]
                        && one_chance_in(3))
                {
                    return (0);
                }
            }

            ASSERT( id != -1 );

            // opposing element (2 away on ring) greater cancels 2 in 3
            if (you.skills[ (elemental_ring[id] + 2) % 4 ] > you.skills[exsk]
                && !one_chance_in(3))
            {
                return (0);
            }
        }
        else if (exsk >= SK_CONJURATIONS && exsk <= SK_DIVINATIONS)
        {
            // experimental restriction to encourage some specialization -- bwr
            int skill_rank = 1;

            // seven skills: conj, ench, summ, necro, tloc, tmig, div
            for (i = SK_CONJURATIONS; i <= SK_DIVINATIONS; i++)
            {
                if (you.skills[exsk] < you.skills[i])
                    skill_rank++;
            }

            // Things get progressively harder, but not harder than
            // the Fire-Air or Ice-Earth level.
            if (skill_rank > 3 && one_chance_in(10 - skill_rank))
                return (0);
        }
    }

    int fraction = 0;
    int spending_limit = (you.exp_available < MAX_SPENDING_LIMIT)
                                    ? you.exp_available : MAX_SPENDING_LIMIT;

    // handle fractional learning
    if (skill_cost > spending_limit)
    {
        // This system is a bit hard on missile weapons in the late game
        // since they require expendable ammo in order to practise.
        // Increasing the "deg"ree of exercise would make missile
        // weapons too easy earlier on, so instead we're giving them
        // a special case here.
        if ((exsk != SK_DARTS && exsk != SK_BOWS && exsk != SK_CROSSBOWS)
                                        || skill_cost > you.exp_available)
        {
            fraction = (spending_limit * 10) / skill_cost;
            skill_cost = (skill_cost * fraction) / 10;

            deg = (deg * fraction) / 10;

            if (deg == 0)
                bonus = (bonus * fraction) / 10;
        }
        else
        {
            if ((skill_cost / 2) > MAX_SPENDING_LIMIT)
            {
                deg = 0;
                fraction = 5;
            }
            else
            {
                deg = 1;
            }

            skill_cost = spending_limit;
        }
    }

    skill_cost -= random2(5);

    // No free lunch, this is a problem now that we don't
    // have overspending.
    if (skill_cost < 1)
        skill_cost = 1;

    int skill_inc = (deg + bonus) * 10 + fraction;

    // Starting to learn skills is easier if the appropriate stat is high
    if (you.skills[exsk] == 0)
    {
        if ((exsk >= SK_FIGHTING && exsk <= SK_STAVES
                && exsk != SK_SHORT_BLADES)
            || exsk == SK_ARMOUR || exsk == SK_SHIELDS)
        {
            skill_inc *= ((you.str < 5) ? 5 : you.str);
            skill_inc /= 10;
        }
        else if ((exsk >= SK_SLINGS && exsk <= SK_UNARMED_COMBAT)
            || exsk == SK_SHORT_BLADES)
        {
            // Note: Armour and shields are handled above.
            skill_inc *= ((you.dex < 5) ? 5 : you.dex);
            skill_inc /= 10;
        }
        else if ((exsk >= SK_SPELLCASTING && exsk <= SK_POISON_MAGIC)
            || exsk == SK_EVOCATIONS)
        {
            skill_inc *= ((you.intel < 5) ? 5 : you.intel);
            skill_inc /= 10;
        }
        else if (exsk == SK_INVOCATIONS)
        {
            // Bonus just for those odd balls who don't have invocations
            // yet have managed to really please an invocation wanting god.
            skill_inc *= ((you.piety < 50) ? 10 : (5 + you.piety / 10));
            skill_inc /= 10;
        }
    }

    // no need to go further if
    if (skill_inc <= 0)
        return (0);

    // Can safely return at any stage before this
    you.skill_points[exsk] += skill_inc;
    you.exp_available -= skill_cost;

    you.total_skill_points += skill_inc;

    if (you.skill_cost_level < 27
       && you.total_skill_points >= skill_cost_needed(you.skill_cost_level + 1))
    {
        you.skill_cost_level++;
    }

    if (you.exp_available < 0)
        you.exp_available = 0;

    set_redraw_status( REDRAW_EXPERIENCE );

/*
    New (LH): debugging bit: when you exercise a skill, displays the skill
    exercised and how much you spent on it. Too irritating to be a regular
    WIZARD feature.

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Exercised %s * %d for %d xp.",
         skill_name(exsk), skill_inc, skill_cost );
#endif
*/

    if (you.skill_points[exsk] >= (skill_exp_needed(you.skills[exsk] + 2)
                                    * species_skills(exsk, you.species) / 100))
    {
        you.skills[exsk]++;

        if (you.skills[exsk] == 27)
        {
            snprintf( info, INFO_SIZE, "You have mastered %s!",
                      skill_name( exsk ) );
        }
        else if (you.skills[exsk] == 1)
        {
            snprintf( info, INFO_SIZE, "You have gained %s skill!",
                      skill_name( exsk ) );
        }
        else
        {
            snprintf( info, INFO_SIZE, "Your %s skill increases to level %d!",
                      skill_name( exsk ), you.skills[exsk] );
        }

        mpr( MSGCH_INTRINSIC_GAIN, info );

        // Recalculate this skill's order for tie breaking skills
        // at its new level.   See skills2.cc::init_skill_order()
        // for more details.  -- bwr
        you.skill_order[exsk] = 0;
        for (i = SK_FIGHTING; i < NUM_SKILLS; i++)
        {
            if (i == exsk)
                continue;

            if (you.skills[i] >= you.skills[exsk])
                you.skill_order[exsk]++;
        }

        if (exsk == SK_FIGHTING)
            calc_hp_max();

        if (exsk == SK_INVOCATIONS || exsk == SK_EVOCATIONS
            || exsk == SK_SPELLCASTING)
        {
            calc_mp_max();
        }

        if (exsk == SK_DODGING || exsk == SK_ARMOUR)
            set_redraw_status( REDRAW_EVASION );

        if (exsk == SK_ARMOUR || exsk == SK_SHIELDS
                || exsk == SK_ICE_MAGIC || exsk == SK_EARTH_MAGIC
                || you.duration[ DUR_TRANSFORMATION ] > 0)
        {
            set_redraw_status( REDRAW_ARMOUR_CLASS );
        }

        const int best = best_skill( SK_FIGHTING, (NUM_SKILLS - 1) );
        const int best_spell = best_skill( SK_SPELLCASTING, SK_POISON_MAGIC );

        if (exsk == SK_SPELLCASTING && you.skills[exsk] == 1
            && best_spell == SK_SPELLCASTING)
        {
            mpr("You're starting to get the hang of this magic thing.");
        }

        if (best != old_best_skill || old_best_skill == exsk)
            set_redraw_status( REDRAW_SKILL );
    }

    return (skill_inc);
}                               // end exercise2()

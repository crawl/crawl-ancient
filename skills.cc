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
#include <stdlib.h>

#include "externs.h"

#include "player.h"
#include "skills2.h"
#include "stuff.h"

#ifdef MACROS
#include "macro.h"
#endif


#define MAX_COST_LIMIT           250
#define MAX_SPENDING_LIMIT       250


static void exercise2(char exsk, char deg);




void exercise( char exsk, int deg )
{

    if ( you.exp_available > 0 && you.skills[exsk] < 27 )
    {
        while (deg > 0)
        {
            if ( !you.practise_skill[exsk] && !one_chance_in(4) )
              break;
            else
            {
                exercise2(exsk, 1);
                deg--;
            }
        }
    }

    return;

}          // end exercise()




static void exercise2( char exsk, char deg )
{

    int skill_change = deg * (1 + you.skills[exsk]);    // but "deg" always equals 1 ... {dlb}

    int bonus = 0;

    char title[40];
    char old_best_skill = best_skill(SK_FIGHTING, (NUM_SKILLS - 1), 99);

// does not yet allow for loss of skill levels.
    if ( you.skills[exsk] > 9 )
      skill_change *= (you.skills[exsk] - 7) / 3;

    if ( you.experience_level > 4 )
      skill_change += you.experience_level - 4;
    if ( you.experience_level > 7 )
      skill_change += you.experience_level - 7;
    if ( you.experience_level > 9 )
      skill_change += you.experience_level - 9;
    if ( you.experience_level > 10 )
      skill_change += you.experience_level - 10;
    if ( you.experience_level > 11 )
      skill_change += you.experience_level - 11;

    if ( you.experience_level > 7 )    // was 12 -- bwross
      skill_change *= (you.experience_level - 6) / 2;

    if ( you.skills[exsk] > 7 )
      skill_change += 1;
    if ( you.skills[exsk] > 9 )
      skill_change += 2;
    if ( you.skills[exsk] > 11 )
      skill_change += 3;
    if ( you.skills[exsk] > 13 )
      skill_change += 4;
    if ( you.skills[exsk] > 15 )
      skill_change += 5;

/* New (LH): Reduced the spending cap from 500. I'm not sure, but the
 * MAX_SPENDING_LIMIT doesn't seem to affect the actual xp cost per
 * skill_point. Am I misreading this?
 */
    if ( skill_change > MAX_COST_LIMIT )
      skill_change = MAX_COST_LIMIT;

/*
 * New (LH): If the pool is filling up, you use more xp. I think this is
 * quite reasonable, as it will just get ignored past the 20K cut-off anyway.
 * Actually, trying to apply mathematical/logical rigour to the xp pool
 * system doesn't work - it's meant to be arbitrary. The only reason for its
 * existence is to give players a reason to go out and kill things and
 * explore instead of sitting around practising their skills.
 */
    if ( you.exp_available >= 1000 )
      skill_change += 20;
    if ( you.exp_available >= 2500 )
      skill_change += 50;
    if ( you.exp_available >= 5000 )
      skill_change += 75;
    if ( you.exp_available >= 8000 )
      skill_change += 110;
    if ( you.exp_available >= 12000 )
      skill_change += 150;
    if ( you.exp_available >= 15000 )
      skill_change += 200;

// being good at some weapons makes others easier to learn:
    if ( exsk < SK_SLINGS )
    {
        /* blades to blades */
        if ( ( exsk == SK_SHORT_BLADES || exsk == SK_LONG_SWORDS )
              && ( you.skills[SK_SHORT_BLADES] > you.skills[exsk] || you.skills[SK_LONG_SWORDS] > you.skills[exsk] ) )
        {
            bonus += random2(3);
        }

        /* Axes and Polearms */
        if ( ( exsk == SK_AXES || exsk == SK_POLEARMS )
            && ( you.skills[SK_AXES] > you.skills[exsk] || you.skills[SK_POLEARMS] > you.skills[exsk] ) )
        {
            bonus += random2(3);
        }

        /* Polearms and Staves */
        if ( ( exsk == SK_POLEARMS || exsk == SK_STAVES )
            && ( you.skills[SK_POLEARMS] > you.skills[exsk] || you.skills[SK_STAVES] > you.skills[exsk] ) )
        {
            bonus += random2(3);
        }

        /* Axes and Maces */
        if ( ( exsk == SK_AXES || exsk == SK_MACES_FLAILS )
            && ( you.skills[SK_AXES] > you.skills[exsk] || you.skills[SK_MACES_FLAILS] > you.skills[exsk] ) )
        {
            bonus += random2(3);
        }
    }

// Quick fix for the fact that stealth can't be gained fast enough to
// keep up with the levels of monsters, this should speed its advancement
    if ( exsk == SK_STEALTH )
      bonus += random2(3);

    if ( exsk >= SK_SPELLCASTING )
      {
        if ( you.experience_level < 5 )
        {
            skill_change /= 2;
        }
        else if ( you.experience_level < 15 )
        {
            skill_change *= (10 + (you.experience_level - 5));
            skill_change /= 20;
        }

        // being good at elemental magic makes other elements harder to learn:
        if ( exsk >= SK_FIRE_MAGIC && exsk <= SK_EARTH_MAGIC
            && ( you.skills[SK_FIRE_MAGIC] > you.skills[exsk]
                  || you.skills[SK_ICE_MAGIC] > you.skills[exsk]
                  || you.skills[SK_AIR_MAGIC] > you.skills[exsk]
                  || you.skills[SK_EARTH_MAGIC] > you.skills[exsk] ) )
          {
            if ( one_chance_in(3) )
              return;
          }

        // some are direct opposites
        if ( ( exsk == SK_FIRE_MAGIC || exsk == SK_ICE_MAGIC )
            && ( you.skills[SK_FIRE_MAGIC] > you.skills[exsk]
                  || you.skills[SK_ICE_MAGIC] > you.skills[exsk] ) )
          {
            // of course, this is cumulative with the one above.
            if (!one_chance_in(3))
              return;
          }

        if ( ( exsk == SK_AIR_MAGIC || exsk == SK_EARTH_MAGIC )
            && ( you.skills[SK_AIR_MAGIC] > you.skills[exsk]
                  || you.skills[SK_EARTH_MAGIC] > you.skills[exsk] ) )
          {
            if (!one_chance_in(3))
              return;
          }
      }

    int fraction = 0;

    // Have to be careful not to cause skills to become stagnant here.  The
    // limit of 500 (250 for spells) above for skill_change will make the
    // limit of 100 count for more than 1/10 so the spending_limit should
    // never result in a skill never being able to advance.
    int spending_limit = (you.exp_available < MAX_SPENDING_LIMIT) ? you.exp_available
                                                                  : MAX_SPENDING_LIMIT;

    if (skill_change > spending_limit)
    {
        // This system is a bit hard on missile weapons in the late game
        // since they require expendable ammo in order to practise.
        // Increasing the "deg"ree of exercise would make missile
        // weapons too easy earlier on, so instead we're giving them
        // a special case here.
        if ((exsk != SK_DARTS && exsk != SK_BOWS && exsk != SK_CROSSBOWS)
            || skill_change > you.exp_available)
        {
            fraction = (spending_limit * 10) / skill_change;
            skill_change = (skill_change * fraction) / 10;

            deg = (deg * fraction) / 10;

            if ( deg == 0 )
              bonus = (bonus * fraction) / 10;
        }
        else
        {
            if ((skill_change / 2) > MAX_SPENDING_LIMIT)
            {
                deg = 0;
                fraction = 5;
            }
            else
            {
                deg = 1;
            }

            skill_change = spending_limit;
        }
    }

    skill_change -= random2(5);

    if ( skill_change < 1 )
    {
        // No free lunch, this is a problem now that we don't
        // have overspending.
        if (deg > 0 || fraction > 0 || bonus > 0)
          skill_change = 1;
        else
          skill_change = 0;
    }

// Can safely return at any stage before this

    int skill_inc = (deg + bonus) * 10 + fraction;

    // Starting to learn skills is easier if the appropriate stat is high
    if ( you.skills[exsk] == 0 )
    {
        if ((exsk >= SK_FIGHTING && exsk <= SK_STAVES) || exsk == SK_ARMOUR)
          {
            // These skills are easier for the strong
            skill_inc *= ((you.strength < 5) ? 5 : you.strength);
            skill_inc /= 10;
          }
        else if (exsk >= SK_SLINGS && exsk <= SK_UNARMED_COMBAT)
          {
            // These skills are easier for the dexterous
            // Note: Armour is handled above.
            skill_inc *= ((you.dex < 5) ? 5 : you.dex);
            skill_inc /= 10;
          }
        else if (exsk >= SK_SPELLCASTING && exsk <= SK_POISON_MAGIC)
          {
            // These skills are easier for the smart
            skill_inc *= ((you.intel < 5) ? 5 : you.intel);
            skill_inc /= 10;
          }
    }

    you.skill_points[exsk] += skill_inc;
    you.exp_available -= skill_change;

    if (you.exp_available < 0)
      you.exp_available = 0;

    you.redraw_experience = 1;

/*
   New (LH): debugging bit: when you exercise a skill, displays the skill
   exercised and how much you spent on it. Too irritating to be a regular WIZARD
   feature.

   strcpy(info, "Exercised ");
   strcat(info, skill_name(exsk));
   strcat(info, " * ");
   itoa(skill_inc, st_prn, 10);
   strcat(info, st_prn);
   strcat(info, " for ");
   itoa(skill_change, st_prn, 10);
   strcat(info, st_prn);
   strcat(info, " xp.");
   mpr(info);
*/


    if ( you.skill_points[exsk] > (skill_exp_needed(you.skills[exsk] + 2) * species_skills(exsk, you.species) / 100) )
    {
        strcpy(info, "Your ");
        strcat(info, skill_name(exsk));
        strcat(info, " skill increases!");
        mpr(info);

        you.skills[exsk]++;

        if ( exsk == SK_FIGHTING )
          calc_hp();

        if ( exsk == SK_INVOCATIONS || exsk == SK_SPELLCASTING )
          calc_mp();

        if ( exsk == SK_DODGING || exsk == SK_ARMOUR )
          you.redraw_evasion = 1;

        if ( exsk == SK_ARMOUR || exsk == SK_SHIELDS || exsk == SK_ICE_MAGIC || exsk == SK_EARTH_MAGIC )    // checked {dlb}
          you.redraw_armor_class = 1;

        if ( exsk == SK_SPELLCASTING )
          if ( you.skills[exsk] == 1 && best_skill(SK_SPELLCASTING, SK_POISON_MAGIC, 99) == SK_SPELLCASTING )
            mpr("You're starting to get the hang of this magic thing.");

        if ( best_skill(SK_FIGHTING, (NUM_SKILLS - 1), 99) != old_best_skill || old_best_skill == exsk )
        {
            strcpy(title, skill_title(best_skill(SK_FIGHTING, (NUM_SKILLS - 1), 99), you.skills[best_skill(SK_FIGHTING, (NUM_SKILLS - 1), 99)]));
            redraw_skill(you.your_name, title);
        }
    }

}          // end exercise2()

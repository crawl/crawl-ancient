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
#include "enum.h"
#include "player.h"
#include "skills2.h"
#include "stuff.h"

#ifdef MACROS
  #include "macro.h"
#endif

#define MAX_SPENDING_LIMIT       100

#ifdef CLASSES
void exercise2(char exsk, char deg, char cutting);

#else
void exercise2(char exsk, char deg);

#endif


void exercise(char exsk, int deg)
{

    while (deg > 0)
    {
#ifdef CLASSES
        exercise2(exsk, 1, 0);
#else
        exercise2(exsk, 1);
#endif
        deg--;
    }

}


#ifdef CLASSES
void exercise2(char exsk, char deg, char cutting)
#else
void exercise2(char exsk, char deg)
#endif
{

    int skill_change = deg * (you.skills[exsk] + 1);    // + 3;
    int bonus = 0;

    char title[40];
    char old_best_skill = best_skill(0, 50, 99);

/*                itoa(skill_change, st_prn, 10);
   strcpy(info, "skill_change (0): ");
   strcat(info, st_prn);
   mpr(info); */


#ifdef CLASSES
    if (cutting == 1)
        goto cut_through;
#endif

    // does not yet allow for loss of skill levels.
    if (you.exp_available <= 0)
        return;
    if (you.skills[exsk] == 27)
        return;                 // already maximum

    if (you.practise_skill[exsk] == 0 && random2(4) != 0)
        return;


    if (you.skills[exsk] >= 10)
        skill_change *= (you.skills[exsk] - 7) / 3;

    if (you.experience_level > 4)
        skill_change += you.experience_level - 4;
    if (you.experience_level > 7)
        skill_change += you.experience_level - 7;
    if (you.experience_level > 9)
        skill_change += you.experience_level - 9;
    if (you.experience_level > 10)
        skill_change += you.experience_level - 10;
    if (you.experience_level > 11)
        skill_change += you.experience_level - 11;

    // Used to be 12 -- bwross
    if (you.experience_level >= 8)
        skill_change *= (you.experience_level - 6) / 2;

    if (you.skills[exsk] >= 8)
        skill_change += 1;
    if (you.skills[exsk] >= 10)
        skill_change += 2;
    if (you.skills[exsk] >= 12)
        skill_change += 3;
    if (you.skills[exsk] >= 14)
        skill_change += 4;
    if (you.skills[exsk] >= 16)
        skill_change += 5;

    if (skill_change > 500)
        skill_change = 500;

// being good at some weapons makes others easier to learn:
    if (exsk < SK_SLINGS)
    {
        /* blades to blades */
        if (
               (exsk == SK_SHORT_BLADES
                || exsk == SK_LONG_SWORDS
                || exsk == SK_GREAT_SWORDS
               )
               &&
               (you.skills[SK_SHORT_BLADES] > you.skills[exsk]
                || you.skills[SK_LONG_SWORDS] > you.skills[exsk]
                || you.skills[SK_GREAT_SWORDS] > you.skills[exsk]
               )
            )
        {
            bonus += random2(3);
        }

        /* Axes and Polearms */
        if (
               (exsk == SK_AXES || exsk == SK_POLEARMS)
               &&
               (you.skills[SK_AXES] > you.skills[exsk]
                || you.skills[SK_POLEARMS] > you.skills[exsk]
               )
            )
        {
            bonus += random2(3);
        }

        /* Polearms and Staves */
        if (
               (exsk == SK_POLEARMS || exsk == SK_STAVES)
               &&
               (you.skills[SK_POLEARMS] > you.skills[exsk]
                || you.skills[SK_STAVES] > you.skills[exsk]
               )
            )
        {
            bonus += random2(3);
        }

        /* Axes and Maces */
        if (
               (exsk == SK_AXES || exsk == SK_MACES_FLAILS)
               &&
               (you.skills[SK_AXES] > you.skills[exsk]
                || you.skills[SK_MACES_FLAILS] > you.skills[exsk]
               )
            )
        {
            bonus += random2(3);
        }
    }


    if (exsk >= SK_SPELLCASTING)
    {
        if (you.experience_level < 5)
        {
            skill_change /= 2;
        }
        else if (you.experience_level < 15)
        {
            skill_change *= (10 + (you.experience_level - 5));
            skill_change /= 20;
        }

        // being good at elemental magic makes other elements harder to learn:
        if (exsk >= SK_FIRE_MAGIC && exsk <= SK_EARTH_MAGIC
            && (you.skills[SK_FIRE_MAGIC] > you.skills[exsk]
                || you.skills[SK_ICE_MAGIC] > you.skills[exsk]
                || you.skills[SK_AIR_MAGIC] > you.skills[exsk]
                || you.skills[SK_EARTH_MAGIC] > you.skills[exsk]
            )
            )
        {
            if (random2(3) == 0)
                return;
        }

        // some are direct opposites
        if ((exsk == SK_FIRE_MAGIC || exsk == SK_ICE_MAGIC)
            && (you.skills[SK_FIRE_MAGIC] > you.skills[exsk]
                || you.skills[SK_ICE_MAGIC] > you.skills[exsk]
            )
            )
        {
            // of course, this is cumulative with the one above.
            if (random2(3) != 0)
                return;
        }

        if ((exsk == SK_AIR_MAGIC || exsk == SK_EARTH_MAGIC)
            && (you.skills[SK_AIR_MAGIC] > you.skills[exsk]
                || you.skills[SK_EARTH_MAGIC] > you.skills[exsk]
            )
            )
        {
            if (random2(3) != 0)
                return;
        }
    }


    int fraction = 0;

    // Have to be careful not to cause skills to become stagnant here.  The
    // limit of 500 (250 for spells) above for skill_change will make the
    // limit of 100 count for more than 1/10 so the spending_limit should
    // never result in a skill never being able to advance.
    int spending_limit = (you.exp_available < MAX_SPENDING_LIMIT) ?
                                        you.exp_available : MAX_SPENDING_LIMIT;

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
            if (deg == 0)
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

    if (skill_change <= 0)
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
    if (you.skills[exsk] == 0)
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
    you.redraw_experience = 1;

#ifdef CLASSES
cut_through:
// if (you.skill_points [exsk] > (skill_exp_needed(you.skills [exsk] + 2) * species_skills(exsk, you.species) / 40) && you.skills [exsk] < 27)
#endif
//#else
    if (you.skill_points[exsk] > (skill_exp_needed(you.skills[exsk] + 2) * species_skills(exsk, you.species) / 100) && you.skills[exsk] < 27)
//#endif
    {
/*        if (exsk == SK_DODGING) player_evasion(you) -= ev_mod(); */

        you.skills[exsk]++;
        strcpy(info, "Your ");
        strcat(info, skill_name(exsk));
        strcat(info, " skill increases!");
        mpr(info);

        calc_hp();
        you.redraw_hit_points = 1;

        if (exsk == SK_DODGING)
            you.redraw_evasion = 1;

        if (exsk == SK_SHIELDS || exsk == SK_ARMOUR
            || exsk == SK_ICE_MAGIC || exsk == SK_EARTH_MAGIC)
        {
            you.redraw_armor_class = 1;
        }                       /* ice and earth magic because this can change effects of the
                                   armour spells. */
        if (exsk == SK_INVOCATIONS)
        {
            you.redraw_magic_points = 1;
            calc_ep();
        }

        if (exsk == SK_SPELLCASTING)
        {
            // you.spell_levels += 2;
            you.redraw_magic_points = 1;
            calc_ep();
            if (you.skills[exsk] == 1 && best_skill(SK_SPELLCASTING, SK_POISON_MAGIC, 99) == SK_SPELLCASTING)
                mpr("You're starting to get the hang of this magic thing.");
        }

        if (best_skill(0, 50, 99) != old_best_skill || old_best_skill == exsk)
        {
            strcpy(title, skill_title(best_skill(0, 50, 99), you.skills[best_skill(0, 50, 99)], you.char_class, you.experience_level));
            redraw_skill(you.your_name, title);
//       draw_border(BROWN, you.your_name, title);
        }
    }

    if (you.exp_available <= 0)
        you.exp_available = 0;
}

/*
 *  File:       skills.cc
 *  Summary:    Skill exercising functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
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

#ifdef CLASSES
void exercise2(char exsk, char deg, char cutting);

#else
void exercise2(char exsk, char deg);

#endif


void exercise(char exsk, char deg)
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
    if (you.exp_available <= 0 && (exsk != SK_SPELLCASTING || you.skills[SK_SPELLCASTING] > 0))
        return;
    if (you.skills[exsk] == 27)
        return;                 // already maximum

    if (you.practise_skill[exsk] == 0 && random2(4) != 0)
        return;

    if (you.skills[exsk] >= 10)
        skill_change *= (you.skills[exsk] - 9) / 3;
//if (you.skills [exsk] >= 20) skill_change *= you.skills [exsk] - 19;
    //if (you.experience_level >= 25) skill_change *= you.experience_level - 24;

    if (you.experience_level > 4)
        skill_change += you.experience_level - 4;
    if (you.experience_level > 7)
        skill_change += you.experience_level - 8;
    if (you.experience_level > 9)
        skill_change += you.experience_level - 9;
    if (you.experience_level > 10)
        skill_change += you.experience_level - 10;
    if (you.experience_level > 11)
        skill_change += you.experience_level - 11;

    if (you.experience_level >= 12)
        skill_change *= (you.experience_level - 10) / 2;
/* if (you.experience_level > 10) skill_change ++;
   if (you.experience_level > 13) skill_change += 2;
   if (you.experience_level > 15) skill_change += 5;
   if (you.experience_level > 17) skill_change += 10;
   if (you.experience_level > 19) skill_change ++;
   if (you.experience_level > 20) skill_change ++;
   if (you.experience_level > 21) skill_change ++;
   if (you.experience_level > 22) skill_change ++; */
// if (you.skills [exsk] >= 4) skill_change ++;
    // if (you.skills [exsk] >= 6) skill_change ++;
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

    if (exsk == SK_SPELLCASTING && you.skills[SK_SPELLCASTING] < 1)
        skill_change = 0;       /* makes it easier for non-spellcasters to learn
                                   spellcasting by reading scrolls. */

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
            deg += random2(3);
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
            deg += random2(3);
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
            deg += random2(3);
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
            deg += random2(3);
        }
    }

    if (exsk >= SK_SPELLCASTING)
    {
        skill_change /= 2;

        // being good at elemental magic makes other elements harder to learn:
        if (exsk >= SK_FIRE_MAGIC && exsk <= SK_EARTH_MAGIC
            && (you.skills[SK_FIRE_MAGIC] > you.skills[exsk]
                || you.skills[SK_ICE_MAGIC] > you.skills[exsk]
                || you.skills[SK_AIR_MAGIC] > you.skills[exsk]
                || you.skills[SK_EARTH_MAGIC] > you.skills[exsk]
            )
            )
        {
//   mpr("Bad element 1.");
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
//   mpr("Bad element 2.");
            if (random2(3) != 0)
                return;         // of course, this is cumulative with the one above.

        }

        if ((exsk == SK_AIR_MAGIC || exsk == SK_EARTH_MAGIC)
            && (you.skills[SK_AIR_MAGIC] > you.skills[exsk]
                || you.skills[SK_EARTH_MAGIC] > you.skills[exsk]
            )
            )
        {
//   mpr("Bad element 3.");
            if (random2(3) != 0)
                return;
        }

    }
    skill_change -= random2(5);

/*                itoa(skill_change, st_prn, 10);
   strcpy(info, "skill_change (2): ");
   strcat(info, st_prn);
   mpr(info); */


    if (skill_change <= 0)
        skill_change = 0;

// Can safely return at any stage before this

    you.skill_points[exsk] += deg;
    you.exp_available -= skill_change;
    you.redraw_experience = 1;
/*                itoa(skill_change, st_prn, 10);
   strcpy(info, "skill_change (3): ");
   strcat(info, st_prn);
   mpr(info);
 */

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
            || exsk == SK_ICE_MAGIC || exsk == SK_EARTH_MAGIC
            )
        {
            you.redraw_armor_class = 1;
        }                       /* ice and earth magic because this can change effects of the
                                   armour spells. */
        if (exsk == SK_SPELLCASTING)
        {
            you.spell_levels += 2;
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

#include "config.h"

#include <string.h>

#include "externs.h"
#include "macro.h"
#include "player.h"
#include "skills.h"
#include "skills2.h"
#include "stuff.h"


#ifdef CLASSES
void exercise2(char exsk, char deg, char cutting);
#else
void exercise2(char exsk, char deg);
#endif


void exercise(char exsk, char deg)
{

do
{
#ifdef CLASSES
 exercise2(exsk, 1, 0);
#else
 exercise2(exsk, 1);
#endif
 deg --;
} while (deg > 0);

}


#ifdef CLASSES
void exercise2(char exsk, char deg, char cutting)
#else
void exercise2(char exsk, char deg)
#endif
{

 int skill_change = deg * (you[0].skills [exsk] + 1);// + 3;
 char title [40];
 char old_best_skill = best_skill(0, 50, 99);


#ifdef CLASSES
if (cutting == 1) goto cut_through;
#endif

// does not yet allow for loss of skill levels.
 if (you[0].exp_available <= 0) return;
 if (you[0].skills [exsk] == 27) return; // already maximum

 if (you[0].practise_skill [exsk] == 0 && random2(4) != 0) return;

if (you[0].skills [exsk] >= 10) skill_change *= (you[0].skills [exsk] - 9) / 3;
//if (you[0].skills [exsk] >= 20) skill_change *= you[0].skills [exsk] - 19;
//if (you[0].xl >= 25) skill_change *= you[0].xl - 24;

 if (you[0].xl > 4) skill_change += you[0].xl - 4;
 if (you[0].xl > 7) skill_change += you[0].xl - 8;
 if (you[0].xl > 9) skill_change += you[0].xl - 9;
 if (you[0].xl > 10) skill_change += you[0].xl - 10;
 if (you[0].xl > 11) skill_change += you[0].xl - 11;

 if (you[0].xl >= 12) skill_change *= (you[0].xl - 11) / 2;
/* if (you[0].xl > 10) skill_change ++;
 if (you[0].xl > 13) skill_change += 2;
 if (you[0].xl > 15) skill_change += 5;
 if (you[0].xl > 17) skill_change += 10;
 if (you[0].xl > 19) skill_change ++;
 if (you[0].xl > 20) skill_change ++;
 if (you[0].xl > 21) skill_change ++;
 if (you[0].xl > 22) skill_change ++;*/
// if (you[0].skills [exsk] >= 4) skill_change ++;
// if (you[0].skills [exsk] >= 6) skill_change ++;
 if (you[0].skills [exsk] >= 8) skill_change += 1;
 if (you[0].skills [exsk] >= 10) skill_change += 2;
 if (you[0].skills [exsk] >= 12) skill_change += 3;
 if (you[0].skills [exsk] >= 14) skill_change += 4;
 if (you[0].skills [exsk] >= 16) skill_change += 5;

if (exsk < 8) // being good at some weapons makes others easier to learn:
{
 if ((exsk == 1 || exsk == 2 || exsk == 3) && (you[0].skills [1] > you[0].skills [exsk] || you[0].skills [2] > you[0].skills [exsk] || you[0].skills [2] > you[0].skills [exsk]))
 {
  deg += random2(3);
 }
 if ((exsk == 4 || exsk == 6) && (you[0].skills [4] > you[0].skills [exsk] || you[0].skills [6] > you[0].skills [exsk]))
 {
  deg += random2(3);
 }
 if ((exsk == 6 || exsk == 7) && (you[0].skills [6] > you[0].skills [exsk] || you[0].skills [7] > you[0].skills [exsk]))
 {
  deg += random2(3);
 }
 if ((exsk == 4 || exsk == 5) && (you[0].skills [4] > you[0].skills [exsk] || you[0].skills [5] > you[0].skills [exsk]))
 {
  deg += random2(3);
 }
}

 if (exsk >= 25)
 {
  skill_change /= 2;

// being good at elemental magic makes other elements harder to learn:
  if (exsk >= 33 && exsk <= 36 && (you[0].skills [33] > you[0].skills [exsk] || you[0].skills [34] > you[0].skills [exsk] || you[0].skills [35] > you[0].skills [exsk] || you[0].skills [36] > you[0].skills [exsk]))
   if (random2(3) == 0) return;

  if (exsk == 33 || exsk == 34 && (you[0].skills [33] > you[0].skills [exsk] || you[0].skills [34] > you[0].skills [exsk]))
  {
   if (random2(3) != 0) return; // of course, this is cumulative with the one above.
  }
  if (exsk == 35 || exsk == 36 && (you[0].skills [35] > you[0].skills [exsk] || you[0].skills [36] > you[0].skills [exsk]))
  {
   if (random2(3) != 0) return;
  }

 }
 skill_change -= random2(5);

 if (skill_change <= 0) skill_change = 0;

// Can safely return at any stage before this

 you[0].skill_points [exsk] += deg;
 you[0].exp_available -= skill_change;

#ifdef CLASSES
 cut_through:
// if (you[0].skill_points [exsk] > (skill_exp_needed(you[0].skills [exsk] + 2) * species_skills(exsk, you[0].species) / 40) && you[0].skills [exsk] < 27)
#endif
//#else
 if (you[0].skill_points [exsk] > (skill_exp_needed(you[0].skills [exsk] + 2) * species_skills(exsk, you[0].species) / 100) && you[0].skills [exsk] < 27)
//#endif
 {
/*        if (exsk == 14) player_evasion(you) -= ev_mod();*/

        you[0].skills [exsk] ++;
        strcpy(info, "Your ");
        strcat(info, skill_name(exsk));
        strcat(info, " skill increases!");
        mpr(info);

        calc_hp();
        you[0].hp_ch = 1;

/*        if (exsk == 13) increase_armour_skill();
        if (exsk == 14) player_evasion(you) += ev_mod();*/
        if (exsk == 14) you[0].evasion_ch = 1;

        if (exsk == 17 || exsk == 13)
        {
/*         you[0].shield_class = get_shield_class();*/
         you[0].AC_ch = 1;
        }
        if (exsk == 25)
        {
         you[0].spell_levels += 2;
//         if (you[0].clas == 2 || you[0].clas == 6) you[0].spell_levels -= 2;
//         you[0].ep_max ++;
//         you[0].ep ++;
         you[0].ep_ch = 1;
         calc_ep();
        }
/*        if (exsk == 27)
         you[0].res_magic += 2;*/

        if (best_skill(0, 50, 99) != old_best_skill || old_best_skill == exsk)
        {
         strcpy(title, skill_title(best_skill(0, 50, 99), you[0].skills [best_skill(0, 50, 99)], you[0].clas, you[0].xl));
         redraw_skill(you[0].your_name, title);
//       draw_border(BROWN, you[0].your_name, title);
        }
 }
 if (you[0].exp_available <= 0) you[0].exp_available = 0;


}

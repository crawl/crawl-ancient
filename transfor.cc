#include "config.h"

#include <string.h>

#include "externs.h"
#include "enum.h"

#include "it_use2.h"
#include "itemname.h"
#include "skills2.h"
#include "stuff.h"

int remove_equipment(char remove_stuff [8]);
void extra_hp(int amount_extra);
void untransform(void);


extern unsigned char your_sign; /* these two are defined in view.cc */
extern unsigned char your_colour;

int remove_equipment(char remove_stuff [8])
{

int i;

if (remove_stuff [EQ_WEAPON] == 1 && you[0].equip [EQ_WEAPON] != -1)
{
  unwield_item(you[0].equip [EQ_WEAPON]);
  you[0].equip [EQ_WEAPON] = -1;
  mpr("You are empty handed.");
}

for (i = EQ_CLOAK; i < EQ_LEFT_RING; i ++)
{
 if (remove_stuff [i] == 0) continue;
 if (you[0].equip [i] == -1) continue;
 in_name(you[0].equip [i], 4, str_pass);
 strcpy(info, str_pass);
 strcat(info, " falls away.");
 mpr(info);
 unwear_armour(you[0].equip [i]);
 you[0].equip [i] = -1;
}

return 1;

}

/* armour list
0 - wielded

1 - cloak
2 - helmet
3 - gloves
4 - boots
5 - shield
6 - body armour

7 - ring 0
8 - ring 1
9 - amulet
*/


char transform(int pow, char which_trans)
{

if (you[0].attribute [ATTR_TRANSFORMATION] != TRAN_NONE) untransform();

if (you[0].is_undead != 0)
{
 mpr("Your dead flesh cannot be transformed in this way.");
 return 0;
}

char rem_stuff [8];
int i = EQ_WEAPON;
for (i = EQ_WEAPON; i < EQ_RIGHT_RING; i ++)
{
 rem_stuff [i] = 1;
}

you[0].strength_ch = 1;
you[0].intel_ch = 1;
you[0].dex_ch = 1;
you[0].evasion_ch = 1;
you[0].AC_ch = 1;

if (you[0].species == SP_NAGA)
 if (which_trans != TRAN_BLADE_HANDS && which_trans != TRAN_LICH) you[0].attribute [ATTR_WALK_SLOWLY] --;

/* Remember, it can still fail in the switch below... */

switch(which_trans)
{
 case TRAN_SPIDER: /* also AC + 2, ev + 3, fast_run */
 mpr("You turn into a venomous arachnid creature.");
 remove_equipment(rem_stuff);
 you[0].dex += 5;
 you[0].max_dex += 5;
 you[0].attribute [ATTR_TRANSFORMATION] = TRAN_SPIDER;
 you[0].duration [DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);
 if (you[0].duration [DUR_TRANSFORMATION] > 100) you[0].duration [DUR_TRANSFORMATION] = 100;
 your_sign = 's';
 your_colour = DARKGREY;
 return 1;

 case TRAN_BLADE_HANDS:
 rem_stuff [EQ_CLOAK] = 0; rem_stuff [EQ_HELMET] = 0; rem_stuff [EQ_BOOTS] = 0; rem_stuff [EQ_BODY_ARMOUR] = 0;
 remove_equipment(rem_stuff);
 mpr("Your hands turn into razor-sharp scythe blades.");
 you[0].attribute [ATTR_TRANSFORMATION] = TRAN_BLADE_HANDS;
 you[0].duration [DUR_TRANSFORMATION] = 10 + random2(pow);
 if (you[0].duration [DUR_TRANSFORMATION] > 100) you[0].duration [DUR_TRANSFORMATION] = 100;
 return 1;

 case TRAN_STATUE: /* also AC + 20, ev - 5 */
 mpr("You turn into a living statue of rough stone.");
 rem_stuff [EQ_WEAPON] = 0; /* can still hold a weapon */
 remove_equipment(rem_stuff);
 you[0].dex -= 2;
 you[0].max_dex -= 2;
 you[0].strength += 2;
 you[0].max_strength += 2;
 you[0].attribute [ATTR_TRANSFORMATION] = TRAN_STATUE;
 you[0].duration [DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);
 if (you[0].duration [DUR_TRANSFORMATION] > 100) you[0].duration [DUR_TRANSFORMATION] = 100;
 your_sign = '8';
 your_colour = LIGHTGREY;
 extra_hp(15);
 return 1;

 case TRAN_ICE_BEAST: /* also AC + 2, res_cold * 3, -1 * res_fire */
 mpr("You turn into a creature of crystalline ice.");
 remove_equipment(rem_stuff);
 you[0].attribute [ATTR_TRANSFORMATION] = TRAN_ICE_BEAST;
 you[0].duration [DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);
 if (you[0].duration [DUR_TRANSFORMATION] > 100) you[0].duration [DUR_TRANSFORMATION] = 100;
 your_sign = 'I';
 your_colour = WHITE;
 extra_hp(12);
 return 1;

 case TRAN_DRAGON: /* also AC + 7, ev - 3, -1 * res_cold, 2 * res_fire */
 mpr("You turn into a fearsome dragon!");
 remove_equipment(rem_stuff);
 you[0].attribute [ATTR_TRANSFORMATION] = TRAN_DRAGON;
 you[0].duration [DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);
 if (you[0].duration [DUR_TRANSFORMATION] > 100) you[0].duration [DUR_TRANSFORMATION] = 100;
 you[0].strength += 10;
 you[0].max_strength += 10;
 your_sign = 'D';
 your_colour = GREEN;
 extra_hp(16);
 return 1;

 case TRAN_LICH: /* also AC + 3, 1 * res_cold, prot_life, res_poison, is_undead, res_magic, drain attack (if empty-handed) */
 if (you[0].deaths_door != 0)
 {
  mpr("The transformation conflicts with an enchantment already in effect.");
  return 0;
 }
 mpr("Your body is suffused with negative energy!");
 /* no remove_equip */
 you[0].attribute [ATTR_TRANSFORMATION] = TRAN_LICH;
 you[0].duration [DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);
 if (you[0].duration [DUR_TRANSFORMATION] > 100) you[0].duration [DUR_TRANSFORMATION] = 100;
 you[0].strength += 3;
 you[0].max_strength += 3;
 your_sign = 'L';
 your_colour = LIGHTGREY;
 you[0].is_undead = 1;
 return 1;


}

return 0;

}

void untransform(void)
{

unsigned char was_transformed = you[0].attribute [ATTR_TRANSFORMATION];

char rem_stuff [8];
int i = 0;
for (i = EQ_WEAPON; i < EQ_RIGHT_RING; i ++)
{
 rem_stuff [i] = 0;
}

you[0].strength_ch = 1;
you[0].intel_ch = 1;
you[0].dex_ch = 1;
you[0].evasion_ch = 1;
you[0].AC_ch = 1;

your_sign = '@';
your_colour = LIGHTGREY;

switch(you[0].attribute [ATTR_TRANSFORMATION])
{
 case TRAN_SPIDER:
 mpr("Your transformation has ended.");
 you[0].dex -= 5;
 you[0].max_dex -= 5;
 break;

 case TRAN_BLADE_HANDS:
 mpr("Your hands shrink back to their normal proportions.");
 break;

 case TRAN_STATUE:
 mpr("You revert to your normal fleshy form.");
 you[0].dex += 2;
 you[0].max_dex += 2;
 you[0].strength -= 2;
 you[0].max_strength -= 2;
 break;

 case TRAN_ICE_BEAST:
 mpr("You warm up again.");
 break;

 case TRAN_DRAGON:
 you[0].strength -= 10;
 you[0].max_strength -= 10;
 mpr("Your transformation has ended.");
 break;

 case TRAN_LICH:
 you[0].strength -= 3;
 you[0].max_strength -= 3;
 mpr("You feel yourself come back to life.");
 you[0].is_undead = 0;
 break;


} /* end switch */

you[0].attribute [ATTR_TRANSFORMATION] = TRAN_NONE;
you[0].duration [DUR_TRANSFORMATION] = 0;
if (you[0].species == SP_NAGA && you[0].equip [EQ_BOOTS] != -1 && you[0].inv_plus2 [you[0].equip [EQ_BOOTS]] != 1) /* If nagas wear boots while transformed, they fall off again afterwards */
{
 rem_stuff [EQ_BOOTS] = 1;
 remove_equipment(rem_stuff);
 if (was_transformed != TRAN_BLADE_HANDS && was_transformed != TRAN_LICH) you[0].attribute [ATTR_WALK_SLOWLY] ++;
}

calc_hp();

}

char can_equip(char use_which)
{

if (you[0].attribute [ATTR_TRANSFORMATION] == TRAN_NONE || you[0].attribute [ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS || you[0].attribute [ATTR_TRANSFORMATION] == TRAN_LICH) /* or it's a transformation which doesn't change overall shape */
/* if more cases are added to this if must also change in item_use for naga barding */
{
 if (you[0].species == SP_NAGA && use_which == EQ_BOOTS) return 0;
 if (you[0].species == SP_CENTAUR && use_which == EQ_BOOTS) return 0; /* Neither nagas or centaurs can use boots */
 if ((you[0].species == SP_MINOTAUR || you[0].species == SP_KENKU) && use_which == EQ_HELMET) return 0; /* Minotaurs/Kenku can't wear headgear */
 if (you[0].species == SP_KENKU && use_which == EQ_BOOTS) return 0; /* Kenku can't wear footgear */
}

if (use_which == EQ_HELMET && you[0].mutation [MUT_HORNS] != 0) return 0; /* horns prevent wearing a helmet */

switch(you[0].attribute [ATTR_TRANSFORMATION])
{
 case TRAN_NONE: return 1;
 case TRAN_SPIDER: /* spider - can't wear anything */
 return 0;
 case TRAN_BLADE_HANDS: /* scythe hands */
 if (use_which == EQ_WEAPON || use_which == EQ_GLOVES || use_which == EQ_SHIELD) return 0;
 return 1;
 case TRAN_STATUE: /* statue */
 if (use_which == EQ_WEAPON) return 1;
 return 0;
 case TRAN_LICH:
 return 1;
 default: return 0;
}

return 1;

}

void extra_hp(int amount_extra) /* must also set in calc_hp */
{

  calc_hp();
  you[0].hp *= amount_extra;
  you[0].hp /= 10;
  you[0].hp_ch = 1;
  if (you[0].hp >= you[0].hp_max) you[0].hp = you[0].hp_max;

}

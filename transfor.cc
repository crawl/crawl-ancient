/*
 *  File:       transfor.cc
 *  Summary:    Misc function related to player transformations.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *              <2>             5/26/99         JDJ             transform() and untransform() set wield_change so
 *                                                                      the weapon line updates.
 *              <1>             -/--/--         LRH             Created
 */

#include "AppHdr.h"
#include "transfor.h"

#include <string.h>

#include "externs.h"

#include "it_use2.h"
#include "itemname.h"
#include "skills2.h"
#include "stuff.h"

int remove_equipment(char remove_stuff[8]);
void extra_hp(int amount_extra);
void untransform(void);


extern unsigned char your_sign; /* these two are defined in view.cc */
extern unsigned char your_colour;

extern char wield_change;

int remove_equipment(char remove_stuff[8])
{
    int i;

    if (remove_stuff[EQ_WEAPON] == 1 && you.equip[EQ_WEAPON] != -1)
    {
        unwield_item(you.equip[EQ_WEAPON]);
        you.equip[EQ_WEAPON] = -1;
        mpr("You are empty handed.");
    }

    for (i = EQ_CLOAK; i < EQ_LEFT_RING; i++)
    {
        if (remove_stuff[i] == 0)
            continue;
        if (you.equip[i] == -1)
            continue;
        in_name(you.equip[i], 4, str_pass);
        strcpy(info, str_pass);
        strcat(info, " falls away.");
        mpr(info);
        unwear_armour(you.equip[i]);
        you.equip[i] = -1;
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
    if (you.attribute[ATTR_TRANSFORMATION] != TRAN_NONE)
        untransform();

    if (you.is_undead)
    {
        mpr("Your unliving flesh cannot be transformed in this way.");
        return 0;
    }

    char rem_stuff[8];
    int i = EQ_WEAPON;

    for (i = EQ_WEAPON; i < EQ_RIGHT_RING; i++)
    {
        rem_stuff[i] = 1;
    }

    you.redraw_strength = 1;
    you.redraw_intelligence = 1;
    you.redraw_dexterity = 1;
    you.redraw_evasion = 1;
    you.redraw_armor_class = 1;
    wield_change = 1;

    if (you.species == SP_NAGA)
        if (which_trans != TRAN_BLADE_HANDS && which_trans != TRAN_LICH)
            you.attribute[ATTR_WALK_SLOWLY]--;

/* Remember, it can still fail in the switch below... */

    switch (which_trans)
    {
    case TRAN_SPIDER:           /* also AC + 2, ev + 3, fast_run */
        mpr("You turn into a venomous arachnid creature.");
        remove_equipment(rem_stuff);
        you.dex += 5;
        you.max_dex += 5;
        you.attribute[ATTR_TRANSFORMATION] = TRAN_SPIDER;
        you.duration[DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);
        if (you.duration[DUR_TRANSFORMATION] > 100)
            you.duration[DUR_TRANSFORMATION] = 100;
        your_sign = 's';
        your_colour = DARKGREY;
        return 1;

    case TRAN_BLADE_HANDS:
        rem_stuff[EQ_CLOAK] = 0;
        rem_stuff[EQ_HELMET] = 0;
        rem_stuff[EQ_BOOTS] = 0;
        rem_stuff[EQ_BODY_ARMOUR] = 0;
        remove_equipment(rem_stuff);
        mpr("Your hands turn into razor-sharp scythe blades.");
        you.attribute[ATTR_TRANSFORMATION] = TRAN_BLADE_HANDS;
        you.duration[DUR_TRANSFORMATION] = 10 + random2(pow);
        if (you.duration[DUR_TRANSFORMATION] > 100)
            you.duration[DUR_TRANSFORMATION] = 100;
        return 1;

    case TRAN_STATUE:           /* also AC + 20, ev - 5 */
        if (you.species != SP_GNOME)
        {
            mpr("You turn into a living statue of rough stone.");
        }
        else
        {
            mpr("Look, a garden gnome. How cute is that?");
        }
        rem_stuff[EQ_WEAPON] = 0;       /* can still hold a weapon */
        remove_equipment(rem_stuff);
        you.dex -= 2;
        you.max_dex -= 2;
        you.strength += 2;
        you.max_strength += 2;
        you.attribute[ATTR_TRANSFORMATION] = TRAN_STATUE;
        you.duration[DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);
        if (you.duration[DUR_TRANSFORMATION] > 100)
            you.duration[DUR_TRANSFORMATION] = 100;
        your_sign = '8';
        your_colour = LIGHTGREY;
        extra_hp(15);
        return 1;

    case TRAN_ICE_BEAST:        /* also AC + 2, res_cold * 3, -1 * res_fire */
        mpr("You turn into a creature of crystalline ice.");
        remove_equipment(rem_stuff);
        you.attribute[ATTR_TRANSFORMATION] = TRAN_ICE_BEAST;
        you.duration[DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);
        if (you.duration[DUR_TRANSFORMATION] > 100)
            you.duration[DUR_TRANSFORMATION] = 100;
        your_sign = 'I';
        your_colour = WHITE;
        extra_hp(12);
        return 1;

    case TRAN_DRAGON:           /* also AC + 7, ev - 3, -1 * res_cold, 2 * res_fire */
        mpr("You turn into a fearsome dragon!");
        remove_equipment(rem_stuff);
        you.attribute[ATTR_TRANSFORMATION] = TRAN_DRAGON;
        you.duration[DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);
        if (you.duration[DUR_TRANSFORMATION] > 100)
            you.duration[DUR_TRANSFORMATION] = 100;
        you.strength += 10;
        you.max_strength += 10;
        your_sign = 'D';
        your_colour = GREEN;
        extra_hp(16);
        return 1;

    case TRAN_LICH:             /* also AC + 3, 1 * res_cold, prot_life, res_poison, is_undead, res_magic, drain attack (if empty-handed) */
        if (you.deaths_door != 0)
        {
            mpr("The transformation conflicts with an enchantment already in effect.");
            return 0;
        }
        mpr("Your body is suffused with negative energy!");
        /* no remove_equip */
        you.attribute[ATTR_TRANSFORMATION] = TRAN_LICH;
        you.duration[DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);
        if (you.duration[DUR_TRANSFORMATION] > 100)
            you.duration[DUR_TRANSFORMATION] = 100;
        you.strength += 3;
        you.max_strength += 3;
        your_sign = 'L';
        your_colour = LIGHTGREY;
        you.is_undead = US_HUNGRY_DEAD;
        return 1;
    }

    return 0;
}


void untransform()
{
    unsigned char was_transformed = you.attribute[ATTR_TRANSFORMATION];

    char rem_stuff[8];
    int i = 0;

    for (i = EQ_WEAPON; i < EQ_RIGHT_RING; i++)
    {
        rem_stuff[i] = 0;
    }

    you.redraw_strength = 1;
    you.redraw_intelligence = 1;
    you.redraw_dexterity = 1;
    you.redraw_evasion = 1;
    you.redraw_armor_class = 1;
    wield_change = 1;

    your_sign = '@';
    your_colour = LIGHTGREY;

    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_SPIDER:
        mpr("Your transformation has ended.");
        you.dex -= 5;
        you.max_dex -= 5;
        break;

    case TRAN_BLADE_HANDS:
        mpr("Your hands shrink back to their normal proportions.");
        wield_change = 1;
        break;

    case TRAN_STATUE:
        mpr("You revert to your normal fleshy form.");
        you.dex += 2;
        you.max_dex += 2;
        you.strength -= 2;
        you.max_strength -= 2;
        break;

    case TRAN_ICE_BEAST:
        mpr("You warm up again.");
        break;

    case TRAN_DRAGON:
        you.strength -= 10;
        you.max_strength -= 10;
        mpr("Your transformation has ended.");
        break;

    case TRAN_LICH:
        you.strength -= 3;
        you.max_strength -= 3;
        mpr("You feel yourself come back to life.");
        you.is_undead = US_ALIVE;
        break;
    }

    you.attribute[ATTR_TRANSFORMATION] = TRAN_NONE;
    you.duration[DUR_TRANSFORMATION] = 0;
    if (you.species == SP_NAGA && you.equip[EQ_BOOTS] != -1 && you.inv_plus2[you.equip[EQ_BOOTS]] != 1)         /* If nagas wear boots while transformed, they fall off again afterwards */
    {
        rem_stuff[EQ_BOOTS] = 1;
        remove_equipment(rem_stuff);
        if (was_transformed != TRAN_BLADE_HANDS && was_transformed != TRAN_LICH)
            you.attribute[ATTR_WALK_SLOWLY]++;
    }

    calc_hp();
}


char can_equip(char use_which)
{
    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_NONE || you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS || you.attribute[ATTR_TRANSFORMATION] == TRAN_LICH)   /* or it's a transformation which doesn't change overall shape */
/* if more cases are added to this if must also change in item_use for naga barding */
    {
        if (you.species == SP_NAGA && use_which == EQ_BOOTS)
            return 0;
        if (you.species == SP_CENTAUR && use_which == EQ_BOOTS)
            return 0;           /* Neither nagas or centaurs can use boots */
        if ((you.species == SP_MINOTAUR || you.species == SP_KENKU) && use_which == EQ_HELMET)
            return 0;           /* Minotaurs/Kenku can't wear headgear */
        if (you.species == SP_KENKU && use_which == EQ_BOOTS)
            return 0;           /* Kenku can't wear footgear */
    }

    if (use_which == EQ_HELMET && you.mutation[MUT_HORNS] != 0)
        return 0;               /* horns prevent wearing a helmet */

    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_NONE:
        return 1;
    case TRAN_SPIDER:           // spider - can't wear anything

        return 0;
    case TRAN_BLADE_HANDS:
        if (use_which == EQ_WEAPON || use_which == EQ_GLOVES || use_which == EQ_SHIELD)
            return 0;
        return 1;
    case TRAN_STATUE:
        if (use_which == EQ_WEAPON)
            return 1;
        return 0;
    case TRAN_LICH:
        return 1;
    default:
        return 0;
    }

    return 1;
}


void extra_hp(int amount_extra) /* must also set in calc_hp */
{
    calc_hp();
    you.hp *= amount_extra;
    you.hp /= 10;
    you.redraw_hit_points = 1;
    if (you.hp >= you.hp_max)
        you.hp = you.hp_max;
}

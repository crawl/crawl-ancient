/*
 *  File:       transfor.cc
 *  Summary:    Misc function related to player transformations.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 * <2>  5/26/99  JDJ  transform() and untransform() set wield_change so
 *                    the weapon line updates.
 * <1> -/--/--   LRH  Created
 */

#include "AppHdr.h"
#include "transfor.h"

#include <string.h>

#include "externs.h"

#include "it_use2.h"
#include "itemname.h"
#include "items.h"
#include "misc.h"
#include "player.h"
#include "skills2.h"
#include "stuff.h"

extern unsigned char your_sign; // defined in view.cc
extern unsigned char your_colour;       // defined in view.cc
extern bool wield_change;       // defined in output.cc
void drop_everything(void);
void extra_hp(int amount_extra);

bool remove_equipment(FixedVector < char, 8 > &remove_stuff)
{
    if (remove_stuff[EQ_WEAPON] == 1 && you.equip[EQ_WEAPON] != -1)
    {
        unwield_item(you.equip[EQ_WEAPON]);
        you.equip[EQ_WEAPON] = -1;
        mpr("You are empty handed.");
    }

    for (int i = EQ_CLOAK; i < EQ_LEFT_RING; i++)
    {
        if (remove_stuff[i] == 0 || you.equip[i] == -1)
            continue;

        in_name(you.equip[i], 4, str_pass);
        strcpy(info, str_pass);
        strcat(info, " falls away.");
        mpr(info);
        unwear_armour(you.equip[i]);
        you.equip[i] = -1;
    }

    return true;
}                               // end remove_equipment()

bool transform(int pow, char which_trans)
{
    //jmf: silently discard this enchantment
    you.duration[DUR_STONESKIN] = 0;

    if (you.species && player_in_water())
    {
        // This might by overkill, but it's okay because obviously
        // whatever magical ability that let's them walk on land is
        // removed when they're in water (in this case, their natural
        // form is completely over-riding any other... goes well with
        // the forced transform when entering water)... things like
        // allowing merfolk to transform into dragons and fly out of
        // the water can be done later -- bwr
        mpr("You cannot tranform while you're in the water.");
        return (false);
    }

    if (you.attribute[ATTR_TRANSFORMATION] != TRAN_NONE)
        untransform();

    if (you.is_undead)
    {
        mpr("Your unliving flesh cannot be transformed in this way.");
        return false;
    }

    FixedVector < char, 8 > rem_stuff;

    for (int i = EQ_WEAPON; i < EQ_RIGHT_RING; i++)
        rem_stuff[i] = 1;

    you.redraw_evasion = 1;
    you.redraw_armor_class = 1;
    wield_change = true;

#if 0
    // Now handled in player_movement_speed()
    if (you.species == SP_NAGA
        && (which_trans != TRAN_BLADE_HANDS && which_trans != TRAN_LICH))
    {
        you.attribute[ATTR_WALK_SLOWLY]--;
    }
#endif

    /* Remember, it can still fail in the switch below... */
    switch (which_trans)
    {
    case TRAN_SPIDER:           /* also AC + 2, ev + 3, fast_run */
        mpr("You turn into a venomous arachnid creature.");
        remove_equipment(rem_stuff);
        modify_stat(STAT_DEXTERITY, 5, true);
        you.attribute[ATTR_TRANSFORMATION] = TRAN_SPIDER;
        you.duration[DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);

        if (you.duration[DUR_TRANSFORMATION] > 100)
            you.duration[DUR_TRANSFORMATION] = 100;

        your_sign = 's';
        your_colour = BROWN;
        return true;

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

        return true;

    case TRAN_STATUE:           /* also AC + 20, ev - 5 */
        if (you.species == SP_GNOME && coinflip())
            mpr("Look, a garden gnome. How cute is that?");
        else if (player_genus(GENPC_DWARVEN) && one_chance_in(10))
            mpr("You inwardly fear your resemblance to a lawn ornament.");
        else
            mpr("You turn into a living statue of rough stone.");

        rem_stuff[EQ_WEAPON] = 0;       /* can still hold a weapon */
        remove_equipment(rem_stuff);
        modify_stat(STAT_DEXTERITY, -2, true);
        modify_stat(STAT_STRENGTH, 2, true);
        you.attribute[ATTR_TRANSFORMATION] = TRAN_STATUE;
        you.duration[DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);

        if (you.duration[DUR_TRANSFORMATION] > 100)
            you.duration[DUR_TRANSFORMATION] = 100;

        your_sign = '8';
        your_colour = LIGHTGREY;
        extra_hp(15);
        return true;

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
        return true;

    case TRAN_DRAGON:   /* also AC + 7, ev - 3, -1 * res_cold, 2 * res_fire */
        mpr("You turn into a fearsome dragon!");
        remove_equipment(rem_stuff);
        you.attribute[ATTR_TRANSFORMATION] = TRAN_DRAGON;
        you.duration[DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);

        if (you.duration[DUR_TRANSFORMATION] > 100)
            you.duration[DUR_TRANSFORMATION] = 100;

        modify_stat(STAT_STRENGTH, 10, true);
        your_sign = 'D';
        your_colour = GREEN;
        extra_hp(16);
        return true;

    case TRAN_LICH:
        // also AC + 3, 1 * res_cold, prot_life, res_poison, is_undead,
        // res_magic, drain attack (if empty-handed)
        if (you.deaths_door)
        {
            mpr( "The transformation conflicts with an enchantment "
                 "already in effect." );

            return false;
        }

        mpr("Your body is suffused with negative energy!");

        /* no remove_equip */
        you.attribute[ATTR_TRANSFORMATION] = TRAN_LICH;
        you.duration[DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);

        if (you.duration[DUR_TRANSFORMATION] > 100)
            you.duration[DUR_TRANSFORMATION] = 100;

        modify_stat(STAT_STRENGTH, 3, true);
        your_sign = 'L';
        your_colour = LIGHTGREY;
        you.is_undead = US_HUNGRY_DEAD;
        return true;

    case TRAN_AIR:
        mpr("You feel diffuse...");
        remove_equipment(rem_stuff);
        drop_everything();
        you.attribute[ATTR_TRANSFORMATION] = TRAN_AIR;
        you.duration[DUR_TRANSFORMATION] = 35 + random2(pow) + random2(pow);

        if (you.duration[DUR_TRANSFORMATION] > 150)
            you.duration[DUR_TRANSFORMATION] = 150;

        modify_stat(STAT_DEXTERITY, 8, true);
        your_sign = '#';
        your_colour = DARKGREY;
        return true;

    case TRAN_SERPENT_OF_HELL:
        mpr("You transform into a huge demonic serpent!");
        remove_equipment(rem_stuff);
        you.attribute[ATTR_TRANSFORMATION] = TRAN_SERPENT_OF_HELL;
        you.duration[DUR_TRANSFORMATION] = 20 + random2(pow) + random2(pow);

        if (you.duration[DUR_TRANSFORMATION] > 120)
            you.duration[DUR_TRANSFORMATION] = 120;

        modify_stat(STAT_STRENGTH, 13, true);
        your_sign = 'S';
        your_colour = RED;
        extra_hp(17);
        return true;
    }

    return false;
}                               // end transform()

void untransform(void)
{
    unsigned char was_transformed = you.attribute[ATTR_TRANSFORMATION];

    FixedVector < char, 8 > rem_stuff;

    for (int i = EQ_WEAPON; i < EQ_RIGHT_RING; i++)
        rem_stuff[i] = 0;

    you.redraw_evasion = 1;
    you.redraw_armor_class = 1;
    wield_change = true;

    your_sign = '@';
    your_colour = LIGHTGREY;

    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_SPIDER:
        mpr("Your transformation has ended.", MSGCH_DURATION);
        modify_stat(STAT_DEXTERITY, -5, true);
        break;

    case TRAN_BLADE_HANDS:
        mpr("Your hands revert to their normal proportions.", MSGCH_DURATION);
        wield_change = true;
        break;

    case TRAN_STATUE:
        mpr("You revert to your normal fleshy form.", MSGCH_DURATION);
        modify_stat(STAT_DEXTERITY, 2, true);
        modify_stat(STAT_STRENGTH, -2, true);
        break;

    case TRAN_ICE_BEAST:
        mpr("You warm up again.", MSGCH_DURATION);
        break;

    case TRAN_DRAGON:
        mpr("Your transformation has ended.", MSGCH_DURATION);
        modify_stat(STAT_STRENGTH, -10, true);
        break;

    case TRAN_LICH:
        mpr("You feel yourself come back to life.", MSGCH_DURATION);
        modify_stat(STAT_STRENGTH, -3, true);
        you.is_undead = US_ALIVE;
        break;

    case TRAN_AIR:
        mpr("Your body solidifies.", MSGCH_DURATION);
        modify_stat(STAT_DEXTERITY, -8, true);
        break;

    case TRAN_SERPENT_OF_HELL:
        mpr("Your transformation has ended.", MSGCH_DURATION);
        modify_stat(STAT_STRENGTH, -13, true);
        break;
    }

    you.attribute[ATTR_TRANSFORMATION] = TRAN_NONE;
    you.duration[DUR_TRANSFORMATION] = 0;

    // If nagas wear boots while transformed, they fall off again afterwards:
    if (you.species == SP_NAGA
            && you.equip[EQ_BOOTS] != -1
            && you.inv_plus2[you.equip[EQ_BOOTS]] != 1)
    {
        rem_stuff[EQ_BOOTS] = 1;
        remove_equipment(rem_stuff);

#if 0
        // handled in player_movement_speed()
        if (was_transformed != TRAN_BLADE_HANDS && was_transformed != TRAN_LICH)
        {
            you.attribute[ATTR_WALK_SLOWLY]++;
        }
#endif
    }

    calc_hp();
}                               // end untransform()

bool can_equip(char use_which)
{

    // if more cases are added to this if must also change in
    // item_use for naga barding
    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_NONE
        || you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS
        || you.attribute[ATTR_TRANSFORMATION] == TRAN_LICH)
        /* or a transformation which doesn't change overall shape */
    {
        if (use_which == EQ_BOOTS)
        {
            switch (you.species)
            {
            case SP_NAGA:
            case SP_CENTAUR:
            case SP_KENKU:
                return false;
            default:
                break;
            }
        }
        else if (use_which == EQ_HELMET)
        {
            switch (you.species)
            {
            case SP_MINOTAUR:
            case SP_KENKU:
                return false;
            default:
                break;
            }
        }
    }

    if (use_which == EQ_HELMET && you.mutation[MUT_HORNS])
        return false;

    switch (you.attribute[ATTR_TRANSFORMATION])
    {
    case TRAN_NONE:
    case TRAN_LICH:
        return true;
        break;

    case TRAN_BLADE_HANDS:
        if (use_which == EQ_WEAPON || use_which == EQ_GLOVES
                                                || use_which == EQ_SHIELD)
        {
            return false;
        }
        else
            return true;
        break;

    case TRAN_STATUE:
        return (use_which == EQ_WEAPON);
        break;

    default:
        return false;
        break;
    }

    return true;
}                               // end can_equip()

void extra_hp(int amount_extra) // must also set in calc_hp
{
    calc_hp();

    you.hp *= amount_extra;
    you.hp /= 10;

    deflate_hp(you.hp_max, false);
}                               // end extra_hp()

void drop_everything(void)
{
    int i = 0;

    if (you.num_inv_items < 1)
        return;

    mpr( "You find yourself unable to carry your posessions!" );

    while (you.num_inv_items && i < ENDOFPACK)
    {
        if (you.inv_quantity[i] > 0)
        {
            item_place(i, you.x_pos, you.y_pos, you.inv_quantity[i]);
            you.inv_quantity[i] = 0;
            you.num_inv_items--;
        }

        i++;
    }

    return;
}                               // end drop_everything()

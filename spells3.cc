/*
 *  File:       spells3.cc
 *  Summary:    Implementations of some additional spells.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <2>     8/05/99        BWR             Added allow_control_teleport
 *      <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "spells3.h"

#include <string.h>

#include "externs.h"
#include "enum.h"

#include "beam.h"
#include "direct.h"
#include "fight.h"
#include "itemname.h"
#include "it_use2.h"
#include "items.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mstruct.h"
#include "mons_lev.h"
#include "player.h"
#include "abyss.h"
#include "randart.h"
#include "spells0.h"
#include "spells1.h"
#include "stuff.h"
#include "view.h"

extern char wield_change;       /* defined in output.cc */


void cast_selective_amnesia(void)
{
    char spc = 0;
    char spc2 = 0;
    char ep_gain = 0;
    int keyin = 0;

    if (you.spell_no == 0)
    {
        strcpy(info, "You don't know any spells.");     // can this happen?

        mpr(info);
        return;
    }

query:
    strcpy(info, "Forget which spell?");
    mpr(info);

// test relay_message();

    keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        char unthing = spell_list();    //invent(0, you.inv_quantity, you.inv_dam, you.inv_class, you.inv_type, you.inv_plus, you.inv_ident, you.equip [0], you.equip [6], you.equip [5], you.equip [2], you.equip [1], you.equip [3], you.equip [4], you.ring);

#ifdef PLAIN_TERM
        redraw_screen();
#endif

        if (unthing == 2)
            return;

        if ((unthing >= 65 && unthing <= 90) || (unthing >= 97 && unthing <= 122))
        {
            keyin = unthing;
        }
        else
        {
            mesclr();
            goto query;
        }
    }

    spc = (int) keyin;

    if (spc < 65 || (spc > 90 && spc < 97) || spc > 122)
    {
unknown:
        strcpy(info, "You don't know that spell.");
        mpr(info);
        return;
    }

    spc2 = conv_lett(spc);

    if (spc2 >= 25 || you.spells[spc2] == 210)
    {
        goto unknown;
    }

    if (you.religion != GOD_SIF_MUNA
                    && random2(you.skills[SK_SPELLCASTING])
                                < random2(spell_value(you.spells[spc2])))
    {
        strcpy(info, "Oops! This spell sure is a blunt instrument.");
        mpr(info);
        forget_map(20 + random2(50));
        return;
    }

    ep_gain = spell_value(you.spells[spc2]);
    if (ep_gain < 0)
        ep_gain = 0;

    if (ep_gain == spell_value(you.spells[spc2]))
    {
        strcpy(info, "The spell releases its latent energy back to you as it unravels.");
    }

    mpr(info);

    // you.spell_levels += ep_gain;
    you.redraw_magic_points = 1;
    you.spell_no--;

    you.spells[spc2] = 210;
}                               /* end of cast_selective_amn */


void remove_curse(void)
{

    if (you.equip[EQ_WEAPON] != -1 && you.inv_class[you.equip[EQ_WEAPON]] == 0)
    {
        if (you.inv_plus[you.equip[EQ_WEAPON]] > 130)
            you.inv_plus[you.equip[EQ_WEAPON]] -= 100;
        wield_change = 1;
    }

    if (you.equip[EQ_LEFT_RING] != -1)
    {
        if (you.inv_plus[you.equip[EQ_LEFT_RING]] > 130)
            you.inv_plus[you.equip[EQ_LEFT_RING]] -= 100;
    }

    if (you.equip[EQ_RIGHT_RING] != -1)
    {
        if (you.inv_plus[you.equip[EQ_RIGHT_RING]] > 130)
            you.inv_plus[you.equip[EQ_RIGHT_RING]] -= 100;
    }

    if (you.equip[EQ_AMULET] != -1)
    {
        if (you.inv_plus[you.equip[EQ_AMULET]] > 130)
            you.inv_plus[you.equip[EQ_AMULET]] -= 100;
    }
    int i;

    for (i = 1; i < 7; i++)
    {
        if (you.inv_plus[you.equip[i]] > 130)
            you.inv_plus[you.equip[i]] -= 100;
    }


    strcpy(info, "You feel as if something is helping you.");
    mpr(info);

}                               /* end of remove_curse() */


void detect_curse(void)
{

    int curse_found = 0;
    int i;

    for (i = 0; i < 52; i++)
    {

        if (you.inv_quantity[i] == 0)
            continue;

        if (you.inv_class[i] == 0 || you.inv_class[i] == 2 || you.inv_class[i] == 7)
        {
            if (you.inv_ident[i] == 0)
            {
                curse_found++;
                you.inv_ident[i] = 1;
            }
        }
    }                           /* end of for i */

//if (curse_found > 0)
    //{
    // strcpy(info, "You sense a malignant aura.");
    //} else
    strcpy(info, "You sense the presence of curses on your possessions.");

    mpr(info);

}

void cast_smiting(int pow)
{

    struct dist beam[1];
    int i;

    strcpy(info, "Smite whom?");
    mpr(info);

    direction(100, beam);

    if (beam[0].nothing == -1
        || mgrd[beam[0].target_x][beam[0].target_y] == MNG
        || (beam[0].target_x == you.x_pos && beam[0].target_y == you.y_pos))
    {
        strcpy(info, "The spell fizzles.");
        mpr(info);
        return;
    }

    i = mgrd[beam[0].target_x][beam[0].target_y];

    strcpy(info, "You smite ");
    strcat(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 1));
    strcat(info, "!");
    mpr(info);

    menv[i].hit_points -= random2(8) + random2(pow) / 3;

    // Now reduced -- bwross
    // menv[i].hit_points -= random2(8) + random2(pow) / 3;
    // menv[i].hit_points -= random2(8) + random2(pow) / 3;

    if (menv[i].hit_points <= 0)
        monster_die(i, 1, 0);
    else
        print_wounds(i);

}                               /* end of smiting */

void airstrike(int pow)
{
    struct dist beam[1];
    int i;
    int hurted = 0;

    strcpy(info, "Strike whom?");
    mpr(info);

    direction(100, beam);

    if (beam[0].nothing == -1 || mgrd[beam[0].target_x][beam[0].target_y] == MNG)
    {
        strcpy(info, "The spell fizzles.");
        mpr(info);
        return;
    }

    i = mgrd[beam[0].target_x][beam[0].target_y];

    strcpy(info, "The air twists around and strikes ");
    strcat(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 1));
    strcat(info, "!");
    mpr(info);

    hurted += random2(12) + random2(pow) / 6 + random2(pow) / 7;

    hurted -= random2(menv[i].armor_class + 1);
    if (hurted < 0)
        hurted = 0;

    menv[i].hit_points -= hurted;

    if (menv[i].hit_points <= 0)
        monster_die(i, 1, 0);
    else
        print_wounds(i);

}



void cast_bone_shards(int power)
{

    if (you.equip[EQ_WEAPON] == -1 || you.inv_class[you.equip[EQ_WEAPON]] != 14)        //  || you.inv_type [you.equip [0]] != 1)

    {
        strcpy(info, "The spell fails.");
        mpr(info);
        return;
    }

    if (you.inv_type[you.equip[EQ_WEAPON]] != 1)
    {
        strcpy(info, "The corpse collapses into a mass of pulpy flesh.");
        mpr(info);

        return;
    }

    power *= 15;
    power += mons_weight(you.inv_plus[you.equip[EQ_WEAPON]]);

    struct dist spelld[1];
    struct bolt beam[1];

    if (spell_direction(spelld, beam) == -1)
        return;

    strcpy(info, "The skeleton explodes into sharp fragments of bone!");
    mpr(info);

    unwield_item(you.equip[EQ_WEAPON]);
    you.inv_quantity[you.equip[EQ_WEAPON]]--;
    if (you.inv_quantity[you.equip[EQ_WEAPON]] == 0)    /* can this be false? */
    {
        you.num_inv_items--;
        you.equip[EQ_WEAPON] = -1;
        strcpy(info, "You are now empty handed.");
        mpr(info);
    }

    zapping(ZAP_BONE_SHARDS, power, beam);
/*   zapping(26, power); */
    burden_change();

}


void sublimation(int pow)
{

    unsigned char was_wielded = 0;

    if (you.equip[EQ_WEAPON] == -1 || you.inv_class[you.equip[EQ_WEAPON]] != 4 || you.inv_type[you.equip[EQ_WEAPON]] != 21)
    {
        if (you.deaths_door != 0)
        {
            strcpy(info, "A conflicting enchantment prevents the spell from coming into effect.");
            mpr(info);
            return;
        }

        if (you.hp <= 1)
        {
            strcpy(info, "Your attempt to draw power from your own body fails.");
            mpr(info);
            return;
        }

        strcpy(info, "You draw magical energy from your own body!");
        mpr(info);

        while (you.magic_points < you.max_magic_points && you.hp > 1)
        {
            you.magic_points++;
            you.hp--;
            if (random2(pow) < 6 && you.hp > 1)
                you.hp--;
            if (random2(pow) < 6 && you.hp > 1)
                you.hp--;
            if (random2(pow) < 6 && you.hp > 1)
                you.hp--;
            if (random2(pow) < 6)
                break;
        }

        you.redraw_hit_points = 1;
        you.redraw_magic_points = 1;

        return;
    }

    strcpy(info, "The chunk of flesh you are holding crumbles to dust.");
    mpr(info);
    strcpy(info, "A flood of magical energy pours into your mind!");
    mpr(info);

    if (pow > 100)
        pow = 100;

    you.magic_points += 7 + random2(7);
/*you.magic_points += random2(pow) / 3;
   you.magic_points += random2(pow) / 3;
   you.magic_points += random2(pow) / 3; */

    was_wielded = you.equip[EQ_WEAPON];
    unwield_item(you.equip[EQ_WEAPON]);
    you.equip[EQ_WEAPON] = -1;

    you.inv_quantity[was_wielded]--;
    if (you.inv_quantity[was_wielded] <= 0)
    {
        you.inv_quantity[was_wielded] = 0;
        you.num_inv_items--;
    }
    burden_change();

    if (you.magic_points > you.max_magic_points)
        you.magic_points = you.max_magic_points;
    you.redraw_magic_points = 1;

}

void dancing_weapon(int pow, char force_hostile)
{
    int numsc = 21 + random2(pow) / 5;

    if (numsc > 25)
        numsc = 25;

    int summs = 0;
    int i = 0;
    char behavi = 7;
    char empty[2];

    if (empty_surrounds(you.x_pos, you.y_pos, 67, 0, empty) == 0)
    {
failed_spell:
        strcpy(info, "You hear a popping sound.");
        mpr(info);
        return;
    }

    if (you.equip[EQ_WEAPON] == -1 || you.inv_class[you.equip[EQ_WEAPON]] != 0 || (you.inv_type[you.equip[EQ_WEAPON]] >= 13 && you.inv_type[you.equip[EQ_WEAPON]] <= 16) || you.inv_dam[you.equip[EQ_WEAPON]] >= 180)
    {
        goto failed_spell;
    }

    if (you.inv_plus[you.equip[EQ_WEAPON]] >= 100 || force_hostile == 1)
        behavi = 1;             /* a cursed weapon becomes hostile */

    summs = create_monster(MONS_DANCING_WEAPON, numsc, behavi, empty[0], empty[1], you.pet_target, 1);

    if (summs == -1)
        goto failed_spell;

    for (i = 0; i < ITEMS; i++)
    {
        if (i >= 480)
        {
            strcpy(info, "The demon of the infinite void grins at you.");
            mpr(info);
            return;
        }
        if (mitm.quantity[i] == 0)
        {
            mitm.id[i] = you.inv_ident[you.equip[EQ_WEAPON]];
            mitm.base_type[i] = you.inv_class[you.equip[EQ_WEAPON]];
            mitm.sub_type[i] = you.inv_type[you.equip[EQ_WEAPON]];
            mitm.pluses[i] = you.inv_plus[you.equip[EQ_WEAPON]];
            mitm.pluses2[i] = you.inv_plus2[you.equip[EQ_WEAPON]];
            mitm.special[i] = you.inv_dam[you.equip[EQ_WEAPON]];
            mitm.colour[i] = you.inv_colour[you.equip[EQ_WEAPON]];
            mitm.quantity[i] = 1;
            mitm.link[i] = 501;
            /* it_no ++; */
            break;
        }
    }                           /* end of i loop */

    item_name(you.inv_plus2[you.equip[EQ_WEAPON]], you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_quantity[you.equip[EQ_WEAPON]], you.inv_ident[you.equip[EQ_WEAPON]], 4, str_pass);
    strcpy(info, str_pass);
    strcat(info, " dances into the air!");
    mpr(info);

    unwield_item(you.equip[EQ_WEAPON]);

    you.inv_quantity[you.equip[EQ_WEAPON]] = 0;
    you.equip[EQ_WEAPON] = -1;
    you.num_inv_items--;

    menv[summs].inv[0] = i;
    menv[summs].number = mitm.colour[i];
}

bool monster_on_level(int monster)
{
    for (int i = 0; i < MNST; i++)
    {
        if (menv[i].type == monster)
           return true;
    }

    return false;
}

//
// This function returns true if the player can used controlled
// teleport here.
//
bool allow_control_teleport()
{
    bool ret = true;

    if (you.level_type == LEVEL_ABYSS || you.level_type == LEVEL_LABYRINTH)
    {
        ret = false;
    }
    else
    {
        switch (you.where_are_you) {
        case BRANCH_TOMB:
            // The tomb is a layed out maze, it'd be a shame if the player
            // just teleports through any of it.
            ret = false;
            break;

        case BRANCH_SLIME_PITS:
            // Cannot teleport into the slime pit vaults until the royal
            // jelly is gone.
            if (monster_on_level(MONS_ROYAL_JELLY))
            {
                ret = false;
            }
            break;

        case BRANCH_ELVEN_HALLS:
            // Cannot raid the elven halls vaults with teleport
            if (you.branch_stairs[STAIRS_ELVEN_HALLS]
                        + branch_depth(STAIRS_ELVEN_HALLS) == you.your_level)
            {
                ret = false;
            }
            break;

        case BRANCH_HALL_OF_ZOT:
            // Cannot teleport to the Orb
            if (you.branch_stairs[STAIRS_HALL_OF_ZOT]
                        + branch_depth(STAIRS_HALL_OF_ZOT) == you.your_level)
            {
                ret = false;
            }
            break;
        }
    }

    // Tell the player why if they have teleport control.
    if (ret == false && you.attribute[ATTR_CONTROL_TELEPORT] != 0)
        mpr("A powerful magic prevents you from controlling you teleport.");

    return ret;
}

void you_teleport()
{
    if (scan_randarts(RAP_PREVENT_TELEPORTATION))
    {
        mpr("You feel a weird sense of stasis.");
        return;
    }

    if (you.duration[DUR_TELEPORT] != 0)
    {
        mpr("You feel strangely stable.");
        you.duration[DUR_TELEPORT] = 0;
        return;
    }

    mpr("You feel strangely unstable.");
    you.duration[DUR_TELEPORT] = 4 + random2(3);
}


void you_teleport2(char allow_control)
{
    if (you.attribute[ATTR_CONTROL_TELEPORT] != 0 && allow_control_teleport()
                                        && you.conf == 0 && allow_control == 1)
    {
        mpr("You may choose your destination (press '.' or delete to select).");
        mpr("Expect minor deviation; teleporting into an open area is recommended.");
        more();

        int plox[2];

        plox[0] = 1;
        show_map(plox);

#ifdef PLAIN_TERM
        redraw_screen();
#endif

        plox[0] += random2(3) - 1;
        plox[1] += random2(3) - 1;

        if (random2(4) == 0)
        {
            plox[0] += random2(3) - 1;
            plox[1] += random2(3) - 1;
        }

#ifdef WIZARD
        strcpy(info, "Target square: ");
        itoa(plox[0], st_prn, 10);
        strcat(info, st_prn);
        strcat(info, ", ");
        itoa(plox[1], st_prn, 10);
        strcat(info, st_prn);
        mpr(info);
#endif

        if (plox[0] < 6 || plox[1] < 6 || plox[0] > 75 || plox[1] > 65)
        {
            mpr("Nearby solid objects disrupt your rematerialisation!");
            goto random_teleport;
        }

        you.x_pos = plox[0];
        you.y_pos = plox[1];

        if ((grd[you.x_pos][you.y_pos] != 67
                                    && grd[you.x_pos][you.y_pos] != 65)
                    || mgrd[you.x_pos][you.y_pos] != MNG
                    || env.cgrid[you.x_pos][you.y_pos] != CNG)
        {
            mpr("Oops!");
            goto random_teleport;
        }

    }
    else
    {
random_teleport:
        mpr("Your surroundings suddenly seem different.");

        do
        {
            you.x_pos = random2(70) + 10;
            you.y_pos = random2(60) + 10;
        }
        while ((grd[you.x_pos][you.y_pos] != 67
                                    && grd[you.x_pos][you.y_pos] != 65)
                        || mgrd[you.x_pos][you.y_pos] != MNG
                        || env.cgrid[you.x_pos][you.y_pos] != CNG);
    }

    if (you.level_type == 2)
    {
        abyss_teleport();
        env.cloud_no = 0;
        you.pet_target = MHITNOT;
    }
}                               /* end of you_teleport */


void entomb()
{
    char srx = 0;
    char sry = 0;

    char chance_found = 4;

    char number_built = 0;

//if (you.char_class == 3) chance_found = 2;
    for (srx = you.x_pos - 1; srx < you.x_pos + 2; srx++)
    {
        for (sry = you.y_pos - 1; sry < you.y_pos + 2; sry++)
        {

            if (srx == you.x_pos && sry == you.y_pos)
                continue;

            if (grd[srx][sry] != 67 && grd[srx][sry] != 65 && grd[srx][sry] != 70 && (grd[srx][sry] < 75 || grd[srx][sry] > 78))
                continue;
            if (mgrd[srx][sry] != MNG)
                continue;

            int objl = igrd[srx][sry];
            int hrg = 0;

            while (objl != 501)
            {
                if (mitm.base_type[objl] == 12)
                    goto finish_loop;   // Wouldn't want to do this (ie destroy an Orb)

                hrg = mitm.link[objl];
                objl = hrg;
            }

            objl = igrd[srx][sry];
            hrg = 0;
//   int objl2;

            while (objl != 501)
            {
//    objl2 = objl;
                hrg = mitm.link[objl];
                destroy_item(objl);
                objl = hrg;
//    strcpy(info, "Destroying.");
                //    mpr(info);
            }

            if (env.cgrid[srx][sry] != CNG)
            {
                env.cloud_type[env.cgrid[srx][sry]] = 0;
                env.cgrid[env.cloud_x[env.cgrid[srx][sry]]][env.cloud_y[env.cgrid[srx][sry]]] = CNG;
                env.cloud_decay[env.cgrid[srx][sry]] = 0;
                env.cloud_no--;
            }

            grd[srx][sry] = 1;
            number_built++;
          finish_loop:chance_found++;
                                // this does nothing

        }
    }

    if (number_built > 0)
    {
        strcpy(info, "Walls grow out of the floor around you!");
    }
    else
        strcpy(info, "Nothing appears to happen.");
    mpr(info);

}


void cast_poison_ammo(void)
{

    if (you.equip[EQ_WEAPON] == -1 || you.inv_class[you.equip[EQ_WEAPON]] != 1 || you.inv_dam[you.equip[EQ_WEAPON]] != 0 || you.inv_type[you.equip[EQ_WEAPON]] == 0 || you.inv_type[you.equip[EQ_WEAPON]] == 5)
    {
        mpr("Nothing appears to happen.");
        return;
    }

    item_name(you.inv_plus2[you.equip[EQ_WEAPON]], you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]], you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_quantity[you.equip[EQ_WEAPON]], you.inv_ident[you.equip[EQ_WEAPON]], 4, str_pass);
    strcpy(info, str_pass);
    if (you.inv_quantity[you.equip[EQ_WEAPON]] == 1)
        strcat(info, " is covered in a thin film of poison.");
    else
        strcat(info, " are covered in a thin film of poison.");
    mpr(info);

    you.inv_dam[you.equip[EQ_WEAPON]] = 3;
    wield_change = 1;

}


void create_noise2()
{

    int plox[2];

    strcpy(info, "Choose the noise's source (press '.' or delete to select).");
    mpr(info);
    more();
    plox[0] = 1;
    show_map(plox);

#ifdef PLAIN_TERM
    redraw_screen();
#endif

#ifdef WIZARD
    strcpy(info, "Target square: ");
    itoa(plox[0], st_prn, 10);
    strcat(info, st_prn);
    strcat(info, ", ");
    itoa(plox[1], st_prn, 10);
    strcat(info, st_prn);
    mpr(info);
#endif

    if (plox[0] < 1 || plox[1] < 1 || plox[0] > 78 || plox[1] > 68)
    {
        mpr("You hear a muffled thud.");
        return;
    }

    mpr("You hear a distant voice call your name.");

    noisy(30, plox[0], plox[1]);


}

/*
   Type recalled:
   0 = anything
   1 = undead only (Kiku religion ability)
 */
void recall(char type_recalled)
{
    int i;
    int j = 0;
    int k = 1;
    int l = MNST - 1;
    char recalled = 0;

    if (random2(2) == 0)
    {
        j = MNST - 1;
        l = 0;
        k = -1;
    }                           /* sometimes goes through monster list backwards */

    char empty[2];

    for (i = j; i != l; i += k)
    {
        if (menv[i].type == -1)
            continue;
        if (menv[i].behavior != 7)
            continue;
        if (menv[i].type >= MLAVA0)
            continue;

        if (type_recalled == 1)
        {
            if (!((menv[i].type == 23 || menv[i].type == 49) && menv[i].number == BROWN || menv[i].number == RED || menv[i].number == LIGHTRED))        /* abominations created by twisted res, although it gets others too */
                if (!(menv[i].type == 83))      /* reapers */
                    if (mons_holiness(menv[i].type) != 1)
                        continue;
        }

        if (empty_surrounds(you.x_pos, you.y_pos, 67, 0, empty) == 0)
        {
            break;              /* time to stop */
        }
        else
        {
            mgrd[menv[i].x][menv[i].y] = MNG;
            menv[i].x = empty[0];
            menv[i].y = empty[1];
            mgrd[menv[i].x][menv[i].y] = i;
            recalled++;
            if (menv[i].enchantment[2] != 6 || player_see_invis() != 0)
            {
                strcpy(info, "You recall your ");
                strcat(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 4));
                strcat(info, ".");
                mpr(info);
            }
            else
                recalled--;     /* you're not informed if you've recalled an invis creature */
        }                       /* end else */

    }

    if (recalled == 0)
        mpr("Nothing appears to have answered your call.");

}


void portal(void)
{

    char dir_sign = 0;

    if (you.where_are_you != 0) // && you.where_are_you < 10)

    {
        mpr("This spell doesn't work here.");
        return;
    }
    if (you.level_type != 0)
    {
        mpr("This spell doesn't work here.");
        return;
    }

    if (grd[you.x_pos][you.y_pos] != 67)
    {
        mpr("You must be on a piece of normal floor to cast this spell.");
        return;
    }

    mpr("Which direction ('<' for up, '>' for down, 'x' to quit)?");

get_dir:
    char keyi = get_ch();

    if (keyi != '<' && keyi != '>' && keyi != 'x')
        goto get_dir;
    if (keyi == '<' && you.your_level == 0)
    {
        mpr("You can't go any further upwards with this spell.");
        goto get_dir;
    }
    if (keyi == '>' && you.your_level == 35)
    {
        mpr("You can't go any further downwards with this spell.");
        goto get_dir;
    }

    if (keyi == 'x')
    {
        mpr("Okay, then.");
        return;
    }

    if (keyi == '>')
        dir_sign = 1;
    else
        dir_sign = -1;

    mpr("How many levels (1 - 9, 'x' to quit)?");
  get_dir2:keyi = get_ch();
    if (keyi < 49 && keyi > 57 && keyi != 'x')
        goto get_dir2;

    if (keyi == 'x')
    {
        mpr("Okay, then.");
        return;
    }

    keyi -= 48;

    int target_level = you.your_level + keyi * dir_sign;

    if (you.where_are_you == 0)
    {
        if (target_level < 0)
            target_level = 0;
        if (target_level > 35)
            target_level = 35;
    }

    mpr("You fall through a mystic portal, and materialise at the foot of a staircase.");
    more();
    int old_lev = you.your_level;

    you.your_level = target_level - 1;
    grd[you.x_pos][you.y_pos] = 82;

    down_stairs(1, old_lev);

}

void cast_death_channel(int pow)
{

    mpr("You feel a great wave of evil energy pour through your body.");

    you.duration[DUR_DEATH_CHANNEL] += 15 + random2((pow / 3) + 1);
    if (you.duration[DUR_DEATH_CHANNEL] > 30)
        you.duration[DUR_DEATH_CHANNEL] = 30;

}

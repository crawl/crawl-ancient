/*
 *  File:       spells1.cc
 *  Summary:    Implementations of some additional spells.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <3>      9/11/99        LRH             Can't blink in the Abyss
 *      <3>      6/22/99        BWR             Removed teleport control from
 *                                              random_blink().
 *      <2>      5/20/99        BWR             Increased greatest healing.
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"

#include <string.h>
#include <stdlib.h>

#include "externs.h"
#include "enum.h"

#include "beam.h"
#include "direct.h"
#include "fight.h"
#include "invent.h"
#include "itemname.h"
#include "items.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mstruct.h"
#include "player.h"
#include "abyss.h"
#include "randart.h"
#include "skills.h"
#include "spells1.h"
#include "spells0.h"
#include "spells2.h"
#include "spells3.h"
#include "stuff.h"
#include "view.h"
#include "it_use2.h"

extern char wield_change;       /* defined in output.cc */

void blink(void);
void random_blink(void);
void stinkcl(char cl_x, char cl_y, struct bolt beam[1]);

void cast_big_c(int pow, char cty);
void big_cloud(char clouds, char cl_x, char cl_y, int pow);
char cast_lesser_healing(int mabil);
char cast_greater_healing(int mabil);
char cast_greatest_healing(int mabil);
char healing_spell(int healed);
void cast_revivification(int mabil);
void cast_cure_poison(int mabil);
void purification(void);
void cast_deaths_door(int pow);
void abjuration(int pow);
void extension(int pow);
void ice_armour(int pow, char extending);
void stone_scales(int pow);
void missile_prot(int pow);
void deflection(int pow);
void cast_regen(int pow);
void cast_berserk(void);
void cast_swiftness(int power);
void cast_fly(int power);
void cast_insulation(int power);
void cast_teleport_control(int power);
void cast_resist_poison(int power);


void blink(void)
{

    struct dist beam[1];

    if (scan_randarts(RAP_PREVENT_TELEPORTATION))
    {
        mpr("You feel a weird sense of stasis.");
        return;
    }

    if (!allow_control_teleport())
    {
        mpr("A powerful magic interfers with your control of the blink.");
        random_blink();
        return;
    }

        if (you.level_type == LEVEL_ABYSS && random2(3) != 0)
    {
        mpr("The power of the Abyss keeps you in your place!");
        return;
    }

    if (you.conf != 0)
    {
        random_blink();
        return;
    }

start_blink:
    strcpy(info, "Blink to where?");
    mpr(info);

    direction(100, beam);

    if (beam[0].nothing == -1)
    {
        strcpy(info, "The spell fizzles.");
        mpr(info);
        return;
    }

    if (see_grid(beam[0].target_x, beam[0].target_y) == 0)
    {
        strcpy(info, "You can't blink there!");
        mpr(info);
        goto start_blink;
    }

    if (grd[beam[0].target_x][beam[0].target_y] <= 10 || mgrd[beam[0].target_x][beam[0].target_y] != MNG)
    {
        strcpy(info, "Oops! Maybe something was there already.");
        mpr(info);
        random_blink();
        return;
    }

    you.x_pos = beam[0].target_x;
    you.y_pos = beam[0].target_y;


    if (you.level_type == 2)
    {
        abyss_teleport();
        env.cloud_no = 0;
        you.pet_target = MHITNOT;
    }

}

void random_blink(void)
{
    int passed[2];

    if (scan_randarts(RAP_PREVENT_TELEPORTATION))
    {
        mpr("You feel a weird sense of stasis.");
        return;
    }

    if (random_near_space(passed) == 0
                    || (you.x_pos == passed[0] && you.y_pos == passed[1]))
    {
        strcpy(info, "You feel rather strange for a moment.");
        mpr(info);
        return;
    }

        if (you.level_type == LEVEL_ABYSS && random2(3) != 0)
    {
        mpr("The power of the Abyss keeps you in your place!");
        return;
    }



    strcpy(info, "You blink.");
    mpr(info);

    you.x_pos = passed[0];
    you.y_pos = passed[1];

    if (you.level_type == 2)
    {
        abyss_teleport();
        env.cloud_no = 0;
        you.pet_target = MHITNOT;
    }
}



void fireball(int power)
{
    strcpy(info, "Which direction? (*/+ to target)");
    mpr(info);

    if (you.prev_targ != MHITNOT && you.prev_targ < MNST)
        if (mons_near(you.prev_targ) && (menv[you.prev_targ].enchantment[2] != 6 || player_see_invis() != 0))
        {
            strcpy(info, "You are currently targetting ");
            strcat(info, monam(menv[you.prev_targ].number, menv[you.prev_targ].type, menv[you.prev_targ].enchantment[2], 1));
            strcat(info, " (p to target).");
            mpr(info);
        }
        else
            mpr("You have no current target.");


    struct dist fire_ball[1];

    direction(1, fire_ball);

    if (fire_ball[0].nothing == -1)
    {
        strcpy(info, "The spell fizzles.");
        mpr(info);
        return;
    }

    struct bolt beam[1];

    beam[0].source_x = you.x_pos;
    beam[0].source_y = you.y_pos;
    beam[0].move_x = fire_ball[0].move_x;
    beam[0].move_y = fire_ball[0].move_y;
    beam[0].target_x = fire_ball[0].target_x;
    beam[0].target_y = fire_ball[0].target_y;

    zapping(ZAP_FIREBALL, power, beam);

}

void cast_fire_storm(int powc)
{

    char stx = 0;
    char sty = 0;
    char cl_x = 0;
    char cl_y = 0;
    int summd = 0;

    strcpy(info, "Where?");
    mpr(info);

    struct dist fire_storm[1];
    struct bolt beam[1];

    direction(100, fire_storm);

    beam[0].move_x = fire_storm[0].move_x;
    beam[0].move_y = fire_storm[0].move_y;
    beam[0].target_x = fire_storm[0].target_x;
    beam[0].target_y = fire_storm[0].target_y;

    if (fire_storm[0].nothing == -1)
    {
        strcpy(info, "The spell fizzles.");
        mpr(info);
        return;
    }

    cl_x = beam[0].target_x;
    cl_y = beam[0].target_y;

    strcpy(info, "A raging storm of fire appears!");
    mpr(info);

    beam[0].colour = RED;

    for (stx = -2; stx < 3; stx++)
    {

        for (sty = -2; sty < 3; sty++)
        {
            if ((stx == -2 && sty == -2) || (stx == -2 && sty == 2) || (stx == 2 && sty == -2) || (stx == 2 && sty == 2))
                continue;

            beam[0].range = 1 + random2(5) + random2(powc) / 5 + random2(powc) / 5;
            /* was 1 */
            if (beam[0].range > 20)
                beam[0].range = 20 + random2(4);
            beam[0].type = 1;

            beam[0].target_x = cl_x + stx;
            beam[0].target_y = cl_y + sty;

            if (random2(4) == 0 && grd[beam[0].target_x][beam[0].target_y] == 67 && mgrd[beam[0].target_x][beam[0].target_y] == MNG)    // > 10 && grd [beam[0].target_x] [beam[0].target_y] < 100 && (beam[0].target_x != you.x_pos || beam[0].target_y != you.y_pos))

            {
                summd = mons_place(21, 1, beam[0].target_x, beam[0].target_y, 4, MHITNOT, 250, you.your_level);
            }

            if (grd[beam[0].target_x][beam[0].target_y] > 10 && (beam[0].target_x != you.x_pos || beam[0].target_y != you.y_pos))
            {
                place_cloud(1, beam[0].target_x, beam[0].target_y, beam[0].range);
            }

        }                       /* end of sty */

    }                           /* end of stx */

    viewwindow(1, false);

}                               /* end of cast_fire_storm */



char spell_direction(struct dist spelld[1], struct bolt beam[1])
{

    strcpy(info, "Which direction? (*/+ to target)");
    mpr(info);

    if (you.prev_targ != MHITNOT && you.prev_targ < MNST)
        if (mons_near(you.prev_targ) && (menv[you.prev_targ].enchantment[2] != 6 || player_see_invis() != 0))
        {
            strcpy(info, "You are currently targetting ");
            strcat(info, monam(menv[you.prev_targ].number, menv[you.prev_targ].type, menv[you.prev_targ].enchantment[2], 1));
            strcat(info, " (p to target).");
            mpr(info);
        }
        else
            mpr("You have no current target.");

    direction(1, spelld);

    if (spelld[0].nothing == -1)
    {
        strcpy(info, "The spell fizzles.");
        mpr(info);
        return -1;
    }
    beam[0].move_x = spelld[0].move_x;
    beam[0].move_y = spelld[0].move_y;
    beam[0].target_y = spelld[0].target_y;
    beam[0].target_x = spelld[0].target_x;
    beam[0].source_x = you.x_pos;
    beam[0].source_y = you.y_pos;
    return 1;

}




void identify(char pow)
{
    int id_used = 1;

    if (pow == 1 && random2(3) == 0)
        id_used = random2(3) + 1;
    unsigned char nthing = 0;

    do
    {
      query:strcpy(info, "Identify which item?");
        mpr(info);

        unsigned char keyin = get_ch();

        if (keyin == '*' || keyin == '?')
        {
            if (keyin == '?')
                nthing = get_invent(-1);
            if (keyin == '*')
                nthing = get_invent(-1);
            if ((nthing >= 65 && nthing <= 90) || (nthing >= 97 && nthing <= 122))
            {
                keyin = nthing;
            }
            else
            {
/*       mesclr(); */
                goto query;
            }
        }

        if (keyin == 27)
            return;

        int sc_read_1 = (int) keyin;

        if (sc_read_1 < 65 || (sc_read_1 > 90 && sc_read_1 < 97) || sc_read_1 > 122)
        {
            strcpy(info, "You don't have any such object.");
            mpr(info);
            continue;
        }

        int sc_read_2 = conv_lett(sc_read_1);

        if (you.inv_quantity[sc_read_2] == 0)
        {
            strcpy(info, "You don't have any such object.");
            mpr(info);
            continue;
        }


        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        you.inv_ident[sc_read_2] = 3;

        strcpy(info, " ");

        if (sc_read_2 <= 25)
            info[0] = sc_read_2 + 97;
        else
            info[0] = sc_read_2 + 39;
        info[1] = 0;            /* This null-terminates it, right? */
        strcat(info, " - ");
        item_name(you.inv_plus2[sc_read_2], you.inv_class[sc_read_2], you.inv_type[sc_read_2], you.inv_dam[sc_read_2], you.inv_plus[sc_read_2], you.inv_quantity[sc_read_2], you.inv_ident[sc_read_2], 3, str_pass);
        strcat(info, str_pass);

        if (sc_read_2 == you.equip[EQ_WEAPON])
        {
            strcat(info, " (weapon in hand)");
            wield_change = 1;
        }
        if (sc_read_2 == you.equip[EQ_BODY_ARMOUR])
            strcat(info, " (being worn)");

        if (sc_read_2 == you.equip[EQ_LEFT_RING])
            strcat(info, " (on left hand)");
        if (sc_read_2 == you.equip[EQ_RIGHT_RING])
            strcat(info, " (on right hand)");
        if (sc_read_2 == you.equip[EQ_AMULET])
            strcat(info, " (around neck)");

        mpr(info);

        id_used -= 1;

    }
    while (id_used > 0);

}                               /* end of identify */


void conjure_flame(int pow)
{

    struct dist spelld[1];

    strcpy(info, "You cast a flaming cloud spell!");
    mpr(info);

    strcpy(info, "Where?");
    mpr(info);

direc:
    direction(100, spelld);

    if (spelld[0].nothing == -1)
    {
        strcpy(info, "The spell fizzles.");
        mpr(info);
        return;
    }

    char ig = grd[spelld[0].target_x][spelld[0].target_y];

    if (see_grid(spelld[0].target_x, spelld[0].target_y) == 0)
    {
        strcpy(info, "You can't see that place!");
        mpr(info);
        goto direc;
    }


    if (ig <= 10 || mgrd[spelld[0].target_x][spelld[0].target_y] != MNG || env.cgrid[spelld[0].target_x][spelld[0].target_y] != CNG)
    {
        strcpy(info, "There's already something there!");
        mpr(info);
        goto direc;
    }

/* Hmmm... This should really take account of you.invis monsters */

/*beam[0].source_x = you.x_pos; beam[0].source_y = you.y_pos; */

    int durat = 5 + random2(pow) / 2 + random2(pow) / 2;

    if (durat > 23)
        durat = 23;

/*      beam[0].range = 5 + random2(pow) / 2 + random2(pow) / 2;
   if (beam[0].range > 23) beam[0].range = 23;
   beam[0].type = 1; */

    place_cloud(1, spelld[0].target_x, spelld[0].target_y, durat);

}



void stinking_cloud(void)
{

    struct dist spelld[1];
    struct bolt beam[1];

    strcpy(info, "Which direction? (*/+ to target)");
    mpr(info);

    if (you.prev_targ != MHITNOT && you.prev_targ < MNST)
        if (mons_near(you.prev_targ) && (menv[you.prev_targ].enchantment[2] != 6 || player_see_invis() != 0))
        {
            strcpy(info, "You are currently targetting ");
            strcat(info, monam(menv[you.prev_targ].number, menv[you.prev_targ].type, menv[you.prev_targ].enchantment[2], 1));
            strcat(info, " (p to target).");
            mpr(info);
        }
        else
            mpr("You have no current target.");

    direction(1, spelld);

    if (spelld[0].nothing == -1)
    {
        strcpy(info, "The spell fizzles.");
        mpr(info);
        return;
    }

    beam[0].move_x = spelld[0].move_x;
    beam[0].move_y = spelld[0].move_y;
    beam[0].target_x = spelld[0].target_x;
    beam[0].target_y = spelld[0].target_y;


    beam[0].source_x = you.x_pos;
    beam[0].source_y = you.y_pos;

    strcpy(beam[0].beam_name, "ball of vapour");
    beam[0].colour = GREEN;
    beam[0].range = 7;
    beam[0].damage = 0;
    beam[0].hit = 20;
    beam[0].type = '#';
    beam[0].flavour = 8;        // exploding gas on target // 7; // gas?

    beam[0].thing_thrown = 4;   //? ???? ?

    missile(beam, -1);

    beam[0].damage = magic_ability(30, you.intel);

    stinkcl(beam[0].bx, beam[0].by, beam);
}


void stinkcl(char cl_x, char cl_y, struct bolt beam[1])
{

    char stx = 0;
    char sty = 0;

    for (stx = -1; stx < 2; stx++)
    {

        for (sty = -1; sty < 2; sty++)
        {

            beam[0].range = random2(5) + 1 + random2(beam[0].damage / 50);
            if (beam[0].range > 10)
                beam[0].range = 10 + random2(3);
            beam[0].type = 2;

            beam[0].target_x = cl_x + stx;
            beam[0].target_y = cl_y + sty;

            if (grd[beam[0].target_x][beam[0].target_y] > 10)
            {
//               place_cloud();
                place_cloud(2, beam[0].target_x, beam[0].target_y, beam[0].range);
            }

        }                       // end of sty

    }                           // end of stx


}




void cast_big_c(int pow, char cty)
{

    strcpy(info, "Where do you want to put it?");
    mpr(info);

    struct dist cdis[1];

    direction(100, cdis);

    if (cdis[0].nothing == -1)
    {
        strcpy(info, "The spell fizzles.");
        mpr(info);
        return;
    }


//beam[0].damage = pow;

//big_cloud(cty, beam[0].target_x, beam[0].target_y, pow); //, beam);
    big_cloud(cty, cdis[0].target_x, cdis[0].target_y, pow);

}


void big_cloud(char clouds, char cl_x, char cl_y, int pow)      //, struct bolt beam [1])
 {
    char stx = 0;
    char sty = 0;
    int lasts = 0;

/*if (clouds % 100 == 3) beam[0].colour = WHITE;
   if (clouds % 100 == 4) beam[0].colour = LIGHTGREEN; */

    for (stx = -1; stx < 2; stx++)
    {

        for (sty = -1; sty < 2; sty++)
        {

            lasts = 2 + random2(5) + pow / 2;

            if (lasts > 12)
                lasts = 12 + random2(4);

            if (grd[cl_x + stx][cl_y + sty] > 10)
            {
                place_cloud(clouds, cl_x + stx, cl_y + sty, lasts);
            }

        }                       // end of sty

    }                           // end of stx

}                               // end of freezing_cloud(c,c)



char cast_lesser_healing(void)
{

    return healing_spell(5 + random2(4) + random2(4));  // + random2(mabil / 5));

}                               // end of lesser healing

char cast_greater_healing(void)
{

    return healing_spell(15 + random2(15) + random2(15));       // + random2(mabil / 2));

}                               // end of void cast_greater_healing

char cast_greatest_healing(void)
{

    return healing_spell(50 + random2(25) + random2(25));       // + random2(mabil) + random2(mabil));

}                               // end of void cast_greater_healing



char healing_spell(int healed)
{
    int mgr = 0;
    struct dist bmove[1];

dirc:
    strcpy(info, "Which direction?");
    mpr(info);
    direction(0, bmove);

    mgr = mgrd[you.x_pos + bmove[0].move_x][you.y_pos + bmove[0].move_y];

    if (bmove[0].nothing == -1)
    {
        strcpy(info, "Huh?!");
        mpr(info);
        return 0;
    }

    if (bmove[0].move_x > 1 || bmove[0].move_y > 1)
    {
        strcpy(info, "This spell doesn't reach that far.");
        mpr(info);
        goto dirc;
    }

    if (bmove[0].move_x == 0 && bmove[0].move_y == 0)
    {
        you.hp += healed;
        you.redraw_hit_points = 1;
        if (you.hp > you.hp_max)
            you.hp = you.hp_max;
        mpr("You are healed.");
        return 1;
    }

    if (mgr == MNG)
    {
        strcpy(info, "There isn't anything there!");
        mpr(info);
        return -1;
    }

    strcpy(info, "You heal ");
    strcat(info, monam(menv[mgr].number, menv[mgr].type, menv[mgr].enchantment[2], 1));
    strcat(info, ".");
    mpr(info);

    menv[mgr].hit_points += healed;

    if (menv[mgr].hit_points >= menv[mgr].max_hit_points)
    {
        menv[mgr].hit_points = menv[mgr].max_hit_points;
        strcpy(info, monam(menv[mgr].number, menv[mgr].type, menv[mgr].enchantment[2], 0));
        strcat(info, " is completely healed.");
        mpr(info);
    }
    else
        print_wounds(mgr);

    return 1;
}


void cast_revivification(int mabil)
{
    if (you.hp == you.hp_max)
    {
        strcpy(info, "Nothing appears to happen.");
        mpr(info);
        return;
    }

    if (you.hp_max <= 20)
    {
        strcpy(info, "You lack the resilience to cast this spell.");
        mpr(info);
        return;
    }

    you.base_hp -= 2;
    if (random2(mabil) < 8)
        you.base_hp -= 1;
    if (random2(mabil) < 8)
        you.base_hp -= 1;
    if (random2(mabil) < 8)
        you.base_hp -= 1;
    if (random2(mabil) < 8)
        you.base_hp -= 1;
    if (random2(mabil) < 8)
        you.base_hp -= 1;
    if (random2(mabil) < 8)
        you.base_hp -= 1;
    if (random2(mabil) < 8)
        you.base_hp -= 1;
    if (random2(mabil) < 8)
        you.base_hp -= 1;
    if (random2(mabil) < 8)
        you.base_hp -= 1;
//if (you.hp_max <= 1) you.hp_max = 1;

    calc_hp();

/*if (you.deaths_door != 0)
   {
   strcpy(info, "Your body is healed in an excruciatingly painful way!");
   mpr(info);
   strcpy(info, "The spell is only partially successful.");
   mpr(info);
   you.hp = you.hp_max / 2 + 1;
   if (you.hp >= you.hp_max) you.hp = you.hp_max;
   you.redraw_hit_points = 1;
   return;
   } */
    strcpy(info, "Your body is healed in an amazingly painful way.");
    mpr(info);

    you.hp = you.hp_max;
    you.redraw_hit_points = 1;

}                               // end of void cast_greater_healing

void cast_cure_poison(int mabil)
{

    if (you.poison == 0)
    {
        strcpy(info, "Nothing appears to happen.");
        mpr(info);
        return;
    }

    you.poison -= 2 + random2(mabil) + random2(3);

    if (you.poison <= 0)
    {
        strcpy(info, "You feel the poison leave your system.");
        mpr(info);
        you.poison = 0;
        return;
    }

    strcpy(info, "You feel most of the poison leave your system.");
    mpr(info);

}                               // end of cast_cure_poison

void purification(void)
{

    strcpy(info, "You feel purified!");
    mpr(info);

    you.poison = 0;
    you.rotting = 0;
    you.conf = 0;
    you.slow = 0;
    you.paralysis = 0;          // ?

    you.disease = 0;
// this doesn't seem very useful, does it?

}

void cast_deaths_door(int pow)
{

    if (you.is_undead != 0)
    {
        strcpy(info, "You're already dead!");
        mpr(info);
        return;
    }

    if (you.deaths_door > 0)
    {
        strcpy(info, "Your appeal for an extension has been denied.");
        mpr(info);
        return;
    }

    strcpy(info, "You feel invincible!");
    mpr(info);
    strcpy(info, "You seem to hear sand running through an hourglass...");
    mpr(info);

    you.deaths_door = 8 + random2(5) + random2(5) + random2(5) + random2(pow) / 10;
    you.hp = you.skills[SK_NECROMANCY] + (you.religion == GOD_KIKUBAAQUDGHA) * 13;
    if (you.hp > you.hp_max)
        you.hp = you.hp_max;
    you.redraw_hit_points = 1;

}

void abjuration(int pow)
{
/* can't use beam variables here, because of monster_die and the puffs of smoke */

    int ab = 0;

    strcpy(info, "Send 'em back where they came from!");
    mpr(info);

    for (ab = 0; ab < MNST; ab++)
    {
        if (menv[ab].type == -1)
            continue;
        if (!mons_near(ab))
            continue;
        if (menv[ab].behavior == 7)
            continue;
        if (menv[ab].enchantment1 == 0 || menv[ab].enchantment[1] < 20 || menv[ab].enchantment[1] > 25)
            continue;

        menv[ab].enchantment[1] -= 1 + random2(pow) / 3;
        if (menv[ab].enchantment[1] <= 19)
        {
            monster_die(ab, 6, 0);
            continue;
        }
        strcpy(info, monam(menv[ab].number, menv[ab].type, menv[ab].enchantment[2], 0));
        strcat(info, " shudders.");
        mpr(info);

    }                           // end of for ab

}                               // end of void abjuration

void extension(int pow)
{

//strcpy(info, "You feel magical!");
    //mpr(info);

    if (you.haste > 0)
    {
        potion_effect(POT_SPEED, pow);
    }

    if (you.might > 0)
    {
        potion_effect(POT_MIGHT, pow);
    }

    if (you.levitation > 0)
    {
        potion_effect(POT_LEVITATION, pow);
    }

    if (you.invis > 0)
    {
        potion_effect(POT_INVISIBILITY, pow);
    }

    if (you.duration[DUR_ICY_ARMOUR] > 0)
    {
        ice_armour(pow, 1);
    }

    if (you.duration[DUR_REPEL_MISSILES] > 0)
    {
        missile_prot(pow);
    }

    if (you.duration[DUR_REGENERATION] > 0)
    {
        cast_regen(pow);
    }

    if (you.duration[DUR_DEFLECT_MISSILES] > 0)
    {
        deflection(pow);
    }

    if (you.shock_shield > 0)
    {
        you.shock_shield += random2(pow) + 4;
        if (you.shock_shield > 25)
            you.shock_shield = 25;
        strcpy(info, "Your ring of flames spell is extended.");
        mpr(info);
    }

    if (you.duration[DUR_VORPAL_BLADE] > 0 && you.duration[DUR_VORPAL_BLADE] < 80)
        you.duration[DUR_VORPAL_BLADE] += 10 + random() % 10;
    if (you.duration[DUR_FIRE_BRAND] > 0 && you.duration[DUR_FIRE_BRAND] < 80)
        you.duration[DUR_FIRE_BRAND] += 10 + random() % 10;
    if (you.duration[DUR_ICE_BRAND] > 0 && you.duration[DUR_ICE_BRAND] < 80)
        you.duration[DUR_ICE_BRAND] += 10 + random() % 10;
    if (you.duration[DUR_LETHAL_INFUSION] > 0 && you.duration[DUR_LETHAL_INFUSION] < 80)
        you.duration[DUR_LETHAL_INFUSION] += 10 + random() % 10;

    if (you.duration[DUR_SWIFTNESS] > 0)
        cast_swiftness(pow);
    if (you.duration[DUR_INSULATION] > 0)
        cast_insulation(pow);
    if (you.duration[DUR_STONEMAIL] > 0)
        stone_scales(pow);
    if (you.duration[DUR_CONTROLLED_FLIGHT] > 0)
        cast_fly(pow);
// 13 is teleport countdown
    if (you.duration[DUR_CONTROL_TELEPORT] > 0)
        cast_teleport_control(pow);
    if (you.duration[DUR_RESIST_POISON] > 0)
        cast_resist_poison(pow);
/* 17 is breath */
    if (you.duration[DUR_TRANSFORMATION] > 0)
    {
        mpr("Your transformation has been extended.");
        you.duration[DUR_TRANSFORMATION] += 10 + random2(pow);
        if (you.duration[DUR_TRANSFORMATION] > 100)
            you.duration[DUR_TRANSFORMATION] = 100;
    }
    if (you.duration[DUR_DEATH_CHANNEL] > 0)
        cast_death_channel(pow);

}                               // end extension

void ice_armour(int pow, char extending)
{

//if (pow > 100) pow = 100;

    int dur_change = 0;

    if (you.equip[EQ_BODY_ARMOUR] != -1 && extending == 0)
    {
        if (you.inv_type[you.equip[EQ_BODY_ARMOUR]] > 1 && you.inv_type[you.equip[EQ_BODY_ARMOUR]] != 16 && you.inv_type[you.equip[EQ_BODY_ARMOUR]] != 19 && (you.inv_type[you.equip[EQ_BODY_ARMOUR]] < 22 || you.inv_type[you.equip[EQ_BODY_ARMOUR]] > 25))
        {
            strcpy(info, "You are wearing too much armour.");
            mpr(info);
            return;
        }
    }

    if (you.duration[DUR_STONEMAIL] != 0)
    {
        strcpy(info, "The spell conflicts with another spell still in effect.");
        mpr(info);
        return;
    }

    if (you.duration[DUR_ICY_ARMOUR] == 0)
    {
        strcpy(info, "A film of ice covers your body!");
/* player_AC(you) += 6; */
        you.redraw_armor_class = 1;
    }
    else
        strcpy(info, "Your icy armour thickens.");
    mpr(info);


    dur_change = 20 + random2(pow) + random2(pow);

    if (dur_change + you.duration[DUR_ICY_ARMOUR] >= 100)
        you.duration[DUR_ICY_ARMOUR] = 100;
    else
        you.duration[DUR_ICY_ARMOUR] += dur_change;
/*
   you.duration [DUR_ICY_ARMOUR] += 20 + random2(pow) / 2 + random2(pow) / 2;

   if (you.duration [DUR_ICY_ARMOUR] >= 100) you.duration [DUR_ICY_ARMOUR] = 100;
 */
}

void stone_scales(int pow)
{

//if (pow > 100) pow = 100;

    int dur_change = 0;

    if (you.duration[DUR_ICY_ARMOUR] != 0)
    {
        strcpy(info, "The spell conflicts with another spell still in effect.");
        mpr(info);
        return;
    }

    if (you.duration[DUR_STONEMAIL] == 0)
    {
        strcpy(info, "A set of stone scales covers your body!");
/* player_AC(you) += 7;
   player_evasion(you) -= 2; */
        you.redraw_evasion = 1;
        you.redraw_armor_class = 1;
    }
    else
        strcpy(info, "Your scaly armour looks firmer.");
    mpr(info);


    dur_change = 20 + random2(pow) + random2(pow);

    if (dur_change + you.duration[DUR_STONEMAIL] >= 100)
        you.duration[DUR_STONEMAIL] = 100;
    else
        you.duration[DUR_STONEMAIL] += dur_change;

    burden_change();

}


void missile_prot(int pow)
{

    if (pow > 100)
        pow = 100;

    strcpy(info, "You feel protected from missiles.");
    mpr(info);

    you.duration[DUR_REPEL_MISSILES] += 10 + random2(pow) + random2(pow);

    if (you.duration[DUR_REPEL_MISSILES] >= 100)
        you.duration[DUR_REPEL_MISSILES] = 100;

}

void deflection(int pow)
{

    if (pow > 100)
        pow = 100;

    strcpy(info, "You feel very safe from missiles.");
    mpr(info);

    you.duration[DUR_DEFLECT_MISSILES] += 15 + random2(pow);

    if (you.duration[DUR_DEFLECT_MISSILES] >= 100)
        you.duration[DUR_DEFLECT_MISSILES] = 100;

}

void cast_regen(int pow)
{

    int dur_change = 0;

//if (pow > 150) pow = 150;

    strcpy(info, "Your skin crawls.");
    mpr(info);

    // Now handled by player_hunger_rate()
    // if (you.duration[DUR_REGENERATION] == 0)
    // {
/* you.rate_regen += 100; */
    //     you.hunger_inc += 4;
    // }

    dur_change = 5 + random2(pow) + random2(pow) + random2(pow);

    if (dur_change + you.duration[DUR_REGENERATION] >= 100)
        you.duration[DUR_REGENERATION] = 100;
    else
        you.duration[DUR_REGENERATION] += dur_change;

// if (you.duration [4] >= 100) you.duration [1] = 100;

}

void cast_berserk(void)
{
    if (go_berserk() == 0)
    {
        strcpy(info, "You fail to go berserk.");
        mpr(info);
    }
}


void cast_swiftness(int power)
{

    int dur_incr = 0;

    if (you.duration[DUR_SWIFTNESS] == 0)
    {
        if (player_fast_run() != 0)
        {
            mpr("You can already move quickly.");
            return;
        }
//  you.fast_run ++;
    }

    if (you.species != SP_NAGA)
        strcpy(info, "You feel quick on your feet.");
    else
        strcpy(info, "You feel quick.");
    mpr(info);

    dur_incr = random2(power) + random2(power) + 20;

    if (dur_incr + you.duration[DUR_SWIFTNESS] > 100)
        you.duration[DUR_SWIFTNESS] = 100;
    else
        you.duration[DUR_SWIFTNESS] += dur_incr;

}



void cast_fly(int power)
{

    int dur_change = 0;

    if (you.levitation == 0)
    {
        strcpy(info, "You fly up into the air.");
        mpr(info);
    }
    else
    {
        strcpy(info, "You feel more buoyant.");
        mpr(info);
    }
    dur_change = random2(power) + random2(power) + 25;
    if (you.levitation + dur_change > 100)
        you.levitation = 100;
    else
        you.levitation += dur_change;

    if (you.duration[DUR_CONTROLLED_FLIGHT] + dur_change > 100)
        you.duration[DUR_CONTROLLED_FLIGHT] = 100;
    else
        you.duration[DUR_CONTROLLED_FLIGHT] += dur_change;
    /* duration [12] makes the game think you're wearing an amulet of controlled flight */

    burden_change();

}

void cast_insulation(int power)
{

    int dur_incr = 0;

    // handled by player_res_electrity() now
    // if (you.duration[DUR_INSULATION] == 0)
    // {
    //     you.attribute[ATTR_RESIST_LIGHTNING]++;
    // }

    strcpy(info, "You feel insulated.");
    mpr(info);

    dur_incr = random2(power) + 10;

    if (dur_incr + you.duration[DUR_INSULATION] > 100)
        you.duration[DUR_INSULATION] = 100;
    else
        you.duration[DUR_INSULATION] += dur_incr;

}

void cast_resist_poison(int power)
{

    int dur_incr = 0;

    strcpy(info, "You feel resistant to poison.");
    mpr(info);

    dur_incr = random2(power) + 10;

    if (dur_incr + you.duration[DUR_RESIST_POISON] > 100)
        you.duration[DUR_RESIST_POISON] = 100;
    else
        you.duration[DUR_RESIST_POISON] += dur_incr;

}

void cast_teleport_control(int power)
{

    int dur_incr = 0;

    if (you.duration[DUR_CONTROL_TELEPORT] == 0)
    {
        you.attribute[ATTR_CONTROL_TELEPORT]++;
    }

    strcpy(info, "You feel controlled.");
    mpr(info);

    dur_incr = random2(power) + 10;

    if (dur_incr + you.duration[DUR_CONTROL_TELEPORT] > 100)
        you.duration[DUR_CONTROL_TELEPORT] = 100;
    else
        you.duration[DUR_CONTROL_TELEPORT] += dur_incr;

}


void cast_ring_of_flames(int power)
{
    you.shock_shield += random2(power) / 10 + 4;
    if (you.shock_shield > 25)
        you.shock_shield = 25;
    strcpy(info, "The air around you leaps into flame!");
    mpr(info);
    manage_shock_shield();
}

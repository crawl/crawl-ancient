/*
 *  File:       spells1.cc
 *  Summary:    Implementations of some additional spells.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <4>      06-mar-2000    bwr             confusing_touch, sure_blade
 *      <3>      9/11/99        LRH             Can't blink in the Abyss
 *      <3>      6/22/99        BWR             Removed teleport control from
 *                                              random_blink().
 *      <2>      5/20/99        BWR             Increased greatest healing.
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "spells1.h"

#include <string.h>
#include <stdlib.h>

#include "externs.h"

#include "abyss.h"
#include "beam.h"
#include "direct.h"
#include "fight.h"
#include "invent.h"
#include "it_use2.h"
#include "itemname.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mon-util.h"
#include "player.h"
#include "skills2.h"
#include "spells3.h"
#include "spells4.h"
#include "stuff.h"
#include "view.h"
#include "wpn-misc.h"

extern bool wield_change;       // defined in output.cc

char healing_spell(int healed);

void blink(void)
{
    struct dist beam;

    // yes, there is a logic to this ordering {dlb}:
    if (scan_randarts(RAP_PREVENT_TELEPORTATION))
        mpr("You feel a weird sense of stasis.");
    else if (you.level_type == LEVEL_ABYSS && !one_chance_in(3))
        mpr("The power of the Abyss keeps you in your place!");
    else if (you.conf)
        random_blink(false);
    else if (!allow_control_teleport())
    {
        mpr("A powerful magic interferes with your control of the blink.");
        random_blink(false);
    }
    else
    {
        // query for location {dlb}:
        for (;;)
        {
            mpr("Blink to where?", MSGCH_PROMPT);

            direction(beam, DIR_TARGET);

            if (!beam.isValid)
            {
                canned_msg(MSG_SPELL_FIZZLES);
                return;         // early return {dlb}
            }

            if (see_grid(beam.tx, beam.ty))
                break;
            else
            {
                mesclr();
                mpr("You can't blink there!");
            }
        }

        if (grd[beam.tx][beam.ty] <= DNGN_LAST_SOLID_TILE
            || mgrd[beam.tx][beam.ty] != NON_MONSTER)
        {
            mpr("Oops! Maybe something was there already.");
            random_blink(false);
        }
        else
        {
            you.x_pos = beam.tx;
            you.y_pos = beam.ty;

            if (you.level_type == LEVEL_ABYSS)
            {
                abyss_teleport();
                env.cloud_no = 0;
                you.pet_target = MHITNOT;
            }
        }
    }

    return;
}                               // end blink()

void random_blink(bool allow_partial_control)
{
    int tx, ty;

    if (scan_randarts(RAP_PREVENT_TELEPORTATION))
        mpr("You feel a weird sense of stasis.");
    else if (you.level_type == LEVEL_ABYSS && !one_chance_in(3))
    {
        mpr("The power of the Abyss keeps you in your place!");
    }
    else if (!random_near_space(you.x_pos, you.y_pos, tx, ty))
    {
        mpr("You feel jittery for a moment.");
    }

#ifdef USE_SEMI_CONTROLLED_BLINK
    //jmf: add back control, but effect is cast_semi_controlled_blink(pow)
    else if (you.attribute[ATTR_CONTROL_TELEPORT] && !you.conf
             && allow_partial_control && allow_control_teleport())
    {
        mpr("You may select the general direction of your translocation.");
        cast_semi_controlled_blink(100);
    }
#endif

    else
    {
        mpr("You blink.");

        you.x_pos = tx;
        you.y_pos = ty;

        if (you.level_type == LEVEL_ABYSS)
        {
            abyss_teleport();
            env.cloud_no = 0;
            you.pet_target = MHITNOT;
        }
    }

    return;
}                               // end random_blink()

void fireball(int power)
{
    struct dist fire_ball;

    mpr("Which direction? (*/+ to target)", MSGCH_PROMPT);

    message_current_target();

    direction(fire_ball);

    if (!fire_ball.isValid)
        canned_msg(MSG_SPELL_FIZZLES);
    else
    {
        struct bolt beam;

        beam.source_x = you.x_pos;
        beam.source_y = you.y_pos;
        beam.target_x = fire_ball.tx;
        beam.target_y = fire_ball.ty;

        zapping(ZAP_FIREBALL, power, beam);
    }

    return;
}                               // end fireball()

void cast_fire_storm(int powc)
{
    char stx = 0, sty = 0;
    char cl_x = 0, cl_y = 0;

    struct bolt beam;
    struct dist fire_storm;

    mpr("Where?");

    direction(fire_storm, DIR_TARGET);

    beam.target_x = fire_storm.tx;
    beam.target_y = fire_storm.ty;

    if (!fire_storm.isValid)
    {
        canned_msg(MSG_SPELL_FIZZLES);
        return;
    }

    cl_x = beam.target_x;
    cl_y = beam.target_y;

    mpr("A raging storm of fire appears!");

    beam.colour = RED;

    for (stx = -2; stx < 3; stx++)
    {
        for (sty = -2; sty < 3; sty++)
        {
            if ((stx == -2 && sty == -2) || (stx == -2 && sty == 2)
                    || (stx == 2 && sty == -2) || (stx == 2 && sty == 2))
            {
                continue;
            }

            beam.range = 1 + random2(5) +
                                (random2(powc) / 5) + (random2(powc) / 5);

            if (beam.range > 20)
                beam.range = 20 + random2(4);

            beam.type = 1;
            beam.target_x = cl_x + stx;
            beam.target_y = cl_y + sty;

            if (grd[beam.target_x][beam.target_y] == DNGN_FLOOR
                    && mgrd[beam.target_x][beam.target_y] == NON_MONSTER
                    && one_chance_in(4))
            {
                mons_place( MONS_FIRE_VORTEX, BEH_WANDER, MHITNOT, true,
                    beam.target_x, beam.target_y);
            }

            if (grd[beam.target_x][beam.target_y] > DNGN_LAST_SOLID_TILE
                    && (beam.target_x != you.x_pos
                                        || beam.target_y != you.y_pos))
            {
                place_cloud( CLOUD_FIRE, beam.target_x, beam.target_y,
                                                            beam.range );
            }
        }                       // end "for stx, sty"
    }

    viewwindow(1, false);
}                               // end cast_fire_storm()

char spell_direction(struct dist &spelld, struct bolt &pbolt, int restrict)
{
    if (restrict == DIR_TARGET)
        mpr("Choose a target (+/- for next/prev monster)");
    else
        mpr("Which direction? (*/+ to target)");

    message_current_target();

    direction(spelld, restrict);

    if (!spelld.isValid)
    {
        // check for user cancel
        canned_msg(MSG_SPELL_FIZZLES);
        return -1;
    }

    pbolt.target_x = spelld.tx;
    pbolt.target_y = spelld.ty;
    pbolt.source_x = you.x_pos;
    pbolt.source_y = you.y_pos;

    return 1;
}                               // end spell_direction()

void identify(int power)
{
    int id_used = 1;
    unsigned char nthing = 0;

    // scrolls of identify *may* produce "extra" identifications {dlb}:
    if (power == -1 && one_chance_in(5))
        id_used += (coinflip()? 1 : 2);

    do
    {
      query:
        mpr("Identify which item?", MSGCH_PROMPT);

        unsigned char keyin = get_ch();

        if (keyin == '*' || keyin == '?')
        {
            if (keyin == '?')
                nthing = get_invent(-1);

            if (keyin == '*')
                nthing = get_invent(-1);

            if ((nthing >= 'A' && nthing <= 'Z')
                                        || (nthing >= 'a' && nthing <= 'z'))
            {
                keyin = nthing;
            }
            else
            {
                //mesclr();
                goto query;
            }
        }

        if (keyin == 27)
            return;

        int sc_read_1 = (int) keyin;

        if (sc_read_1 < 'A'
                || (sc_read_1 > 'Z' && sc_read_1 < 'a')
                || sc_read_1 > 'z')
        {
            mpr("You don't have any such object.");
            continue;
        }

        int sc_read_2 = letter_to_index(sc_read_1);

        if (you.inv_quantity[sc_read_2] == 0)
        {
            mpr("You don't have any such object.");
            continue;
        }

        set_id(you.inv_class[sc_read_2], you.inv_type[sc_read_2], 1);
        you.inv_ident[sc_read_2] = 3;

        strcpy(info, " ");

        info[0] = index_to_letter(sc_read_2);
        info[1] = '\0';

        strcat(info, " - ");
        in_name(sc_read_2, 3, str_pass);
        strcat(info, str_pass);

        strcat( info,
               (sc_read_2 == you.equip[EQ_WEAPON])      ? " (weapon in hand)" :
               (sc_read_2 == you.equip[EQ_BODY_ARMOUR]) ? " (being worn)" :
               (sc_read_2 == you.equip[EQ_LEFT_RING])   ? " (on left hand)" :
               (sc_read_2 == you.equip[EQ_RIGHT_RING])  ? " (on right hand)" :
               (sc_read_2 == you.equip[EQ_AMULET])      ? " (around neck)"
                                                        : "" );
        mpr(info);

        if (sc_read_2 == you.equip[EQ_WEAPON])
            wield_change = true;

        id_used--;
    }
    while (id_used > 0);
}                               // end identify()

void conjure_flame(int pow)
{
    struct dist spelld;

    mpr("You cast a flaming cloud spell! But where?", MSGCH_PROMPT);

  direc:
    direction(spelld, DIR_TARGET);

    if (!spelld.isValid)
    {
        canned_msg(MSG_SPELL_FIZZLES);
        return;
    }

    if (!see_grid(spelld.tx, spelld.ty))
    {
        mpr("You can't see that place!");
        goto direc;
    }

    if (grd[spelld.tx][spelld.ty] <= DNGN_LAST_SOLID_TILE
        || mgrd[spelld.tx][spelld.ty] != NON_MONSTER
        || env.cgrid[spelld.tx][spelld.ty] != EMPTY_CLOUD)
    {
        mpr("There's already something there!");
        goto direc;
    }

    int durat = 5 + (random2(pow) / 2) + (random2(pow) / 2);

    if (durat > 23)
        durat = 23;

    place_cloud(CLOUD_FIRE, spelld.tx, spelld.ty, durat);
}                               // end cast_conjure_flame()

void stinking_cloud(void)
{
    struct dist spelld;
    struct bolt beem;

    mpr("Which direction? (*/+ to target)", MSGCH_PROMPT);

    message_current_target();

    direction(spelld);

    if (!spelld.isValid)
    {
        canned_msg(MSG_SPELL_FIZZLES);
        return;
    }

    beem.target_x = spelld.tx;
    beem.target_y = spelld.ty;

    beem.source_x = you.x_pos;
    beem.source_y = you.y_pos;

    strcpy(beem.beam_name, "ball of vapour");
    beem.colour = GREEN;
    beem.range = 6;
    beem.rangeMax = 6;
    beem.damage = 0;
    beem.hit = 20;
    beem.type = SYM_ZAP;
    beem.flavour = BEAM_ACID;
    beem.ench_power = 3 * you.intel;
    beem.thrower = KILL_YOU;
    beem.isBeam = false;
    beem.isTracer = false;

    beam(beem);
}                               // end stinking_cloud()

void cast_big_c(int pow, char cty)
{
    struct dist cdis;

    mpr("Where do you want to put it?", MSGCH_PROMPT);
    direction(cdis, DIR_TARGET);

    if (!cdis.isValid)
    {
        canned_msg(MSG_SPELL_FIZZLES);
        return;
    }

    big_cloud( cty, cdis.tx, cdis.ty, pow, 8 + random2(3) );
}                               // end cast_big_c()

void big_cloud(char clouds, char cl_x, char cl_y, int pow, int size)
{
#ifdef USE_NEW_CLOUD_CODE
    apply_area_cloud(make_a_normal_cloud, cl_x, cl_y, pow, size, clouds);
#else

    char stx = 0;
    char sty = 0;
    int lasts = 0;

    for (stx = -1; stx < 2; stx++)
    {
        for (sty = -1; sty < 2; sty++)
        {
            lasts = 2 + random2(5) + (pow / 2);

            if (lasts > 12)
                lasts = 12 + random2(4);

            if (grd[cl_x + stx][cl_y + sty] > DNGN_LAST_SOLID_TILE)
                place_cloud( clouds, cl_x + stx, cl_y + sty, lasts );

        }                       // end of sty
    }
#endif
}                               // end big_cloud()

char cast_lesser_healing(void)
{
    return healing_spell(5 + random2avg(7, 2));
}                               // end lesser healing()

char cast_greater_healing(void)
{
    return healing_spell(15 + random2avg(29, 2));
}                               // end cast_greater_healing()

char cast_greatest_healing(void)
{
    return healing_spell(50 + random2avg(49, 2));
}                               // end cast_greatest_healing()

char healing_spell(int healed)
{
    int mgr = 0;
    struct monsters *monster = 0;       // NULL {dlb}
    struct dist bmove;

  dirc:
    mpr("Which direction?", MSGCH_PROMPT);
    direction(bmove, DIR_DIR);

    if (!bmove.isValid)
    {
        mpr("Huh?!");
        return 0;
    }

    mgr = mgrd[you.x_pos + bmove.dx][you.y_pos + bmove.dy];

    if (bmove.dx == 0 && bmove.dy == 0)
    {
        mpr("You are healed.");
        inc_hp(healed, false);
        return 1;
    }

    if (mgr == NON_MONSTER)
    {
        mpr("There isn't anything there!");
        return -1;
    }

    monster = &menv[mgr];

    if (heal_monster(monster, healed, false))
    {
        strcpy(info, "You heal ");
        strcat(info, ptr_monam( monster, 1 ));
        strcat(info, ".");
        mpr(info);

        if (monster->hit_points == monster->max_hit_points)
            simple_monster_message( monster, " is completely healed." );
        else
            print_wounds(monster);
    }
    else
    {
        canned_msg(MSG_NOTHING_HAPPENS);
    }

    return 1;
}                               // end healing_spell()

bool cast_revivification(int power)
{
    int loopy = 0;              // general purpose loop variable {dlb}
    bool success = false;

    if (you.hp == you.hp_max)
        canned_msg(MSG_NOTHING_HAPPENS);
    else if (you.hp_max < 21)
        mpr("You lack the resilience to cast this spell.");
    else
    {
        mpr("Your body is healed in an amazingly painful way.");

        you.base_hp -= 2;

        for (loopy = 0; loopy < 9; loopy++)
        {
            if (random2(power) < 8)
                you.base_hp--;
        }

        calc_hp();
        set_hp(you.hp_max, false);
        success = true;
    }

    return (success);
}                               // end cast_revivification()

void cast_cure_poison(int mabil)
{
    if (!you.poison)
        canned_msg(MSG_NOTHING_HAPPENS);
    else
    {
        you.poison -= 2 + random2(mabil) + random2(3);

        if (you.poison < 1)
        {
            mpr("You feel the poison leave your system.");
            you.poison = 0;
        }
        else
        {
            mpr("You feel most of the poison leave your system.");
        }
    }

    return;
}                               // end cast_cure_poison()

void purification(void)
{
    mpr("You feel purified!");

    you.poison = 0;
    you.rotting = 0;
    you.conf = 0;
    you.slow = 0;
    you.paralysis = 0;          // ?
    you.disease = 0;            // this doesn't seem very useful, does it?

    // disease is quite useful (food cost of waiting around for disease
    // to go away so you can heal normally, plus protecting stats from
    // decay), paralysis isn't (how do you cast this?) -- bwr
}                               // end purification()

void cast_deaths_door(int pow)
{
    if (you.is_undead)
        mpr("You're already dead!");
    else if (you.deaths_door)
        mpr("Your appeal for an extension has been denied.");
    else
    {
        mpr("You feel invincible!");
        mpr("You seem to hear sand running through an hourglass...");

        you.deaths_door = 8 + random2avg(13, 3) + (random2(pow) / 10);

        // making this cap a bit unpredicable.
        if (you.deaths_door > 25)
            you.deaths_door = 25 + random2(3);

        set_hp( you.skills[SK_NECROMANCY], false );

        // Kikubaaqudgha gives out a little gift depending on how
        // good you are. -- bwr
        if (you.religion == GOD_KIKUBAAQUDGHA && !player_under_penance())
            inc_hp( you.piety / 15, false );

        deflate_hp( you.hp_max, false );
    }

    return;
}

// can't use beam variables here, because of monster_die and the puffs of smoke
void abjuration(int pow)
{
    struct monsters *monster = 0;       // NULL {dlb}

    mpr("Send 'em back where they came from!");

    for (int ab = 0; ab < MAX_MONSTERS; ab++)
    {
        monster = &menv[ab];

        int abjLevel;

        if (monster->type == -1 || !mons_near(monster))
            continue;

        if (mons_friendly(monster))
            continue;

        abjLevel = mons_del_ench(monster, ENCH_ABJ_I, ENCH_ABJ_VI);
        if (abjLevel != ENCH_NONE)
        {
            abjLevel -= 1 + (random2(pow) / 3);

            if (abjLevel < ENCH_ABJ_I)
                monster_die(monster, KILL_RESET, 0);
            else
            {
                simple_monster_message(monster, " shudders.");
                mons_add_ench(monster, abjLevel);
            }
        }
    }
}                               // end abjuration()

// Antimagic is sort of an anti-extension... it sets a lot of magical
// durations to 1 so it's very nasty at times (and potentially lethal,
// that's why we reduce levitation to 2, so that the player has a chance
// to stop insta-death... sure the others could lead to death, but that's
// not as direct as falling into deep water) -- bwr
void antimagic( void )
{
    if (you.haste)
        you.haste = 1;

    if (you.might)
        you.might = 1;

    if (you.levitation > 2)
        you.levitation = 2;

    if (you.invis)
        you.invis = 1;

    if (you.duration[DUR_ICY_ARMOUR])
        you.duration[DUR_ICY_ARMOUR] = 1;

    if (you.duration[DUR_REPEL_MISSILES])
        you.duration[DUR_REPEL_MISSILES] = 1;

    if (you.duration[DUR_REGENERATION])
        you.duration[DUR_REGENERATION] = 1;

    if (you.duration[DUR_DEFLECT_MISSILES])
        you.duration[DUR_DEFLECT_MISSILES] = 1;

    if (you.shock_shield)
        you.shock_shield = 1;

    // This one isn't really so appropriate, and extension has
    // more than enough already -- bwr
    /*
    if ( !(you.duration[DUR_WEAPON_BRAND] < 1
                 || you.duration[DUR_WEAPON_BRAND] > 80) )
    {
        you.duration[DUR_WEAPON_BRAND] += 10 + random2(10);
    }
    */

    if (you.duration[DUR_SWIFTNESS])
        you.duration[DUR_SWIFTNESS] = 1;

    if (you.duration[DUR_INSULATION])
        you.duration[DUR_INSULATION] = 1;

    if (you.duration[DUR_STONEMAIL])
        you.duration[DUR_STONEMAIL] = 1;

    if (you.duration[DUR_CONTROLLED_FLIGHT])
        you.duration[DUR_CONTROLLED_FLIGHT] = 1;

    if (you.duration[DUR_CONTROL_TELEPORT])
        you.duration[DUR_CONTROL_TELEPORT] = 1;

    if (you.duration[DUR_RESIST_POISON])
        you.duration[DUR_RESIST_POISON] = 1;

    if (you.duration[DUR_TRANSFORMATION])
        you.duration[DUR_TRANSFORMATION] = 1;

    //jmf: added following
    if (you.duration[DUR_STONESKIN])
        you.duration[DUR_STONESKIN] = 1;

    if (you.duration[DUR_FORESCRY])
        you.duration[DUR_FORESCRY] = 1;

    if (you.duration[DUR_SEE_INVISIBLE])
        you.duration[DUR_SEE_INVISIBLE] = 1;

    if (you.duration[DUR_SILENCE])
        you.duration[DUR_SILENCE] = 1;

    if (you.duration[DUR_CONDENSATION_SHIELD])
        you.duration[DUR_CONDENSATION_SHIELD] = 1;

    if (you.duration[DUR_DEATH_CHANNEL])
        you.duration[DUR_DEATH_CHANNEL] = 1;
}                               // end extension()

void extension(int pow)
{
    if (you.haste)
        potion_effect(POT_SPEED, pow);

    if (you.might)
        potion_effect(POT_MIGHT, pow);

    if (you.levitation)
        potion_effect(POT_LEVITATION, pow);

    if (you.invis)
        potion_effect(POT_INVISIBILITY, pow);

    if (you.duration[DUR_ICY_ARMOUR])
        ice_armour(pow, true);

    if (you.duration[DUR_REPEL_MISSILES])
        missile_prot(pow);

    if (you.duration[DUR_REGENERATION])
        cast_regen(pow);

    if (you.duration[DUR_DEFLECT_MISSILES])
        deflection(pow);

    if (you.shock_shield)
    {
        you.shock_shield += random2(pow) + 4;

        if (you.shock_shield > 25)
            you.shock_shield = 25;

        mpr("Your ring of flames spell is extended.");
    }

    // This one isn't really so appropriate, and extension has
    // more than enough already -- bwr
    /*
    if ( !(you.duration[DUR_WEAPON_BRAND] < 1
                 || you.duration[DUR_WEAPON_BRAND] > 80) )
    {
        you.duration[DUR_WEAPON_BRAND] += 10 + random2(10);
    }
    */

    if (you.duration[DUR_SWIFTNESS])
        cast_swiftness(pow);

    if (you.duration[DUR_INSULATION])
        cast_insulation(pow);

    if (you.duration[DUR_STONEMAIL])
        stone_scales(pow);

    if (you.duration[DUR_CONTROLLED_FLIGHT])
        cast_fly(pow);

    if (you.duration[DUR_CONTROL_TELEPORT])
        cast_teleport_control(pow);

    if (you.duration[DUR_RESIST_POISON])
        cast_resist_poison(pow);

    if (you.duration[DUR_TRANSFORMATION])
    {
        mpr("Your transformation has been extended.");
        you.duration[DUR_TRANSFORMATION] += 10 + random2(pow);
        if (you.duration[DUR_TRANSFORMATION] > 100)
            you.duration[DUR_TRANSFORMATION] = 100;
    }

    //jmf: added following
    if (you.duration[DUR_STONESKIN])
        cast_stoneskin(pow);

    if (you.duration[DUR_FORESCRY])
        cast_forescry(pow);

    if (you.duration[DUR_SEE_INVISIBLE])
        cast_see_invisible(pow);

    if (you.duration[DUR_SILENCE])   //how precisely did you cast extension?
        cast_silence(pow);

    if (you.duration[DUR_CONDENSATION_SHIELD])
        cast_condensation_shield(pow);

    if (you.duration[DUR_DEATH_CHANNEL])
        cast_death_channel(pow);
}                               // end extension()

void ice_armour(int pow, bool extending)
{
    if (you.equip[EQ_BODY_ARMOUR] != -1 && !extending)
    {
        // XXX: This should be expressed with enums -- bwr
        if (you.inv_type[you.equip[EQ_BODY_ARMOUR]] > 1
            && you.inv_type[you.equip[EQ_BODY_ARMOUR]] != 16
            && you.inv_type[you.equip[EQ_BODY_ARMOUR]] != 19
            && (you.inv_type[you.equip[EQ_BODY_ARMOUR]] < 22
                || you.inv_type[you.equip[EQ_BODY_ARMOUR]] > 25))
        {
            mpr("You are wearing too much armour.");
            return;
        }
    }

    if (you.duration[DUR_STONEMAIL] || you.duration[DUR_STONESKIN])
    {
        mpr("The spell conflicts with another spell still in effect.");
        return;
    }

    if (you.duration[DUR_ICY_ARMOUR])
        mpr("Your icy armour thickens.");
    else
    {
        mpr("A film of ice covers your body!");
        you.redraw_armor_class = 1;
    }

    you.duration[DUR_ICY_ARMOUR] += 20 + random2(pow) + random2(pow);

    if (you.duration[DUR_ICY_ARMOUR] > 50)
        you.duration[DUR_ICY_ARMOUR] = 50;
/*
 you.duration [DUR_ICY_ARMOUR] += 20 + random2(pow) / 2 + random2(pow) / 2;
 if (you.duration [DUR_ICY_ARMOUR] >= 100) you.duration [DUR_ICY_ARMOUR] = 100;
*/
}                               // end ice_armour()

void stone_scales(int pow)
{
    //if (pow > 100)
    //  pow = 100;
    int dur_change = 0;

    if (you.duration[DUR_ICY_ARMOUR] || you.duration[DUR_STONESKIN])
    {
        mpr("The spell conflicts with another spell still in effect.");
        return;
    }

    if (you.duration[DUR_STONEMAIL])
        mpr("Your scaly armour looks firmer.");
    else
    {
        mpr("A set of stone scales covers your body!");
        you.redraw_evasion = 1;
        you.redraw_armor_class = 1;
    }

    dur_change = 20 + random2(pow) + random2(pow);

    if (dur_change + you.duration[DUR_STONEMAIL] >= 50)
        you.duration[DUR_STONEMAIL] = 50;
    else
        you.duration[DUR_STONEMAIL] += dur_change;

    burden_change();
}                               // end stone_scales()

void missile_prot(int pow)
{
    if (pow > 100)
        pow = 100;

    mpr("You feel protected from missiles.");

    you.duration[DUR_REPEL_MISSILES] += 10 + random2avg(2 * (pow - 1) + 1, 2);

    if (you.duration[DUR_REPEL_MISSILES] > 100)
        you.duration[DUR_REPEL_MISSILES] = 100;
}                               // end missile_prot()

void deflection(int pow)
{
    if (pow > 100)
        pow = 100;

    mpr("You feel very safe from missiles.");

    you.duration[DUR_DEFLECT_MISSILES] += 15 + random2(pow);

    if (you.duration[DUR_DEFLECT_MISSILES] > 100)
        you.duration[DUR_DEFLECT_MISSILES] = 100;
}                               // end cast_deflection()

void cast_regen(int pow)
{
    //if (pow > 150) pow = 150;
    mpr("Your skin crawls.");

    you.duration[DUR_REGENERATION] += 5 + random2avg(3 * (pow - 1) + 1, 3);

    if (you.duration[DUR_REGENERATION] > 100)
        you.duration[DUR_REGENERATION] = 100;
}                               // end cast_regen()

void cast_berserk(void)
{
    go_berserk(true);
}                               // end cast_berserk()

void cast_swiftness(int power)
{
    int dur_incr = 0;

    if (you.species == SP_MERFOLK && player_in_water())
    {
        mpr("This spell will not benefit you while you're swimming!");
        return;
    }

    if (!you.duration[DUR_SWIFTNESS] && player_movement_speed() <= 6)
    {
        mpr("You can already move quickly.");
        return;
    }

    // Reduced the duration (it's easy to chain even in tight situations):
    // dur_incr = random2(power) + random2(power) + 20;
    dur_incr = random2(power) + random2(power) + random2(10) + 10;

    // Centaurs do have feet and shouldn't get here anyways -- bwr
    strcpy(info, "You feel quick");
    strcat(info, (you.species == SP_NAGA) ? "." : " on your feet.");
    mpr(info);

    if (dur_incr + you.duration[DUR_SWIFTNESS] > 100)
        you.duration[DUR_SWIFTNESS] = 100;
    else
        you.duration[DUR_SWIFTNESS] += dur_incr;
}                               // end cast_swiftness()

void cast_fly(int power)
{
    int dur_change = 25 + random2(power) + random2(power);

    if (!you.levitation)
        mpr("You fly up into the air.");
    else
        mpr("You feel more buoyant.");

    if (you.levitation + dur_change > 100)
        you.levitation = 100;
    else
        you.levitation += dur_change;

    if (you.duration[DUR_CONTROLLED_FLIGHT] + dur_change > 100)
        you.duration[DUR_CONTROLLED_FLIGHT] = 100;
    else
        you.duration[DUR_CONTROLLED_FLIGHT] += dur_change;

    // duration[DUR_CONTROLLED_FLIGHT] makes the game think player wears an amulet of controlled flight

    burden_change();
}

void cast_insulation(int power)
{
    int dur_incr = random2(power) + 10;

    mpr("You feel insulated.");

    if (dur_incr + you.duration[DUR_INSULATION] > 100)
        you.duration[DUR_INSULATION] = 100;
    else
        you.duration[DUR_INSULATION] += dur_incr;
}                               // end cast_insulation()

void cast_resist_poison(int power)
{
    int dur_incr = random2(power) + 10;

    mpr("You feel resistant to poison.");

    if (dur_incr + you.duration[DUR_RESIST_POISON] > 100)
        you.duration[DUR_RESIST_POISON] = 100;
    else
        you.duration[DUR_RESIST_POISON] += dur_incr;
}                               // end cast_resist_poison()

void cast_teleport_control(int power)
{
    int dur_incr = random2(power) + 10;

    if (you.duration[DUR_CONTROL_TELEPORT] == 0)
        you.attribute[ATTR_CONTROL_TELEPORT]++;

    mpr("You feel in control.");

    if (dur_incr + you.duration[DUR_CONTROL_TELEPORT] >= 50)
        you.duration[DUR_CONTROL_TELEPORT] = 50;
    else
        you.duration[DUR_CONTROL_TELEPORT] += dur_incr;
}                               // end cast_teleport_control()

void cast_ring_of_flames(int power)
{
    you.shock_shield += 4 + (random2(power) / 10);

    if (you.shock_shield > 25)
        you.shock_shield = 25;

    mpr("The air around you leaps into flame!");

    manage_shock_shield();
}                               // end cast_ring_of_flames()

void cast_confusing_touch(int power)
{

    if (!you.confusing_touch)
        mpr("Your hands begin to glow red.");
    else if (you.confusing_touch < 50)
        mpr("Your hands begin to glow brighter.");

    you.confusing_touch += 5 + (random2(power) / 5);

    if (you.confusing_touch > 50)
        you.confusing_touch = 50;

}                               // end cast_confusing_touch()

bool cast_sure_blade(int power)
{
    bool success = false;

    if (you.equip[EQ_WEAPON] == -1)
        mpr("You aren't wielding a weapon!");
    else if (weapon_skill( you.inv_class[you.equip[EQ_WEAPON]],
                     you.inv_type[you.equip[EQ_WEAPON]]) != SK_SHORT_BLADES)
    {
        mpr("You cannot bond with this weapon.");
    }
    else
    {
        if (!you.sure_blade)
            mpr("You become one with your weapon.");
        else if (you.sure_blade < 25)
            mpr("Your bond becomes stronger.");

        you.sure_blade += 8 + (random2(power) / 10);

        if (you.sure_blade > 25)
            you.sure_blade = 25;

        success = true;
    }

    return (success);
}                               // end cast_sure_blade()

void manage_shock_shield(void)
{
    you.shock_shield--;

    if (!you.shock_shield)
        return;

    char stx = 0, sty = 0;

    for (stx = -1; stx < 2; stx++)
    {
        for (sty = -1; sty < 2; sty++)
        {
            if (sty == 0 && stx == 0)
                continue;

            //if ( one_chance_in(3) ) beam.range ++;

            if (grd[you.x_pos + stx][you.y_pos + sty] > DNGN_LAST_SOLID_TILE
                && env.cgrid[you.x_pos + stx][you.y_pos + sty] == EMPTY_CLOUD)
            {
                place_cloud( CLOUD_FIRE, you.x_pos + stx, you.y_pos + sty,
                                                (one_chance_in(6) ? 3 : 1) );
            }
        }
    }
}                               // end manage_shock_shield()

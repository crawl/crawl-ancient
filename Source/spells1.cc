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


extern bool wield_change;    // defined in output.cc


char healing_spell(int healed);
void stinkcl(char cl_x, char cl_y, struct bolt beam[1]);




void blink( void )
{

    struct dist beam[1];

// yes, there is a logic to this ordering {dlb}:
    if ( scan_randarts(RAP_PREVENT_TELEPORTATION) )
      mpr("You feel a weird sense of stasis.");
    else if ( you.level_type == LEVEL_ABYSS && !one_chance_in(3) )
      mpr("The power of the Abyss keeps you in your place!");
    else if ( you.conf )
      random_blink(false);
    else if ( !allow_control_teleport() )
    {
        mpr("A powerful magic interferes with your control of the blink.");
        random_blink(false);
    }
    else
    {
    // query for location {dlb}:
        for (;;)
        {
            mpr("Blink to where?");

            direction(100, beam);

            if ( beam[0].nothing == -1 )
            {
                canned_msg(MSG_SPELL_FIZZLES);
                return;                           // early return {dlb}
            }

            if ( see_grid(beam[0].target_x, beam[0].target_y) )
              break;
            else
            {
                mesclr();
                mpr("You can't blink there!");
            }
        }

        if ( grd[beam[0].target_x][beam[0].target_y] <= DNGN_LAST_SOLID_TILE
              || mgrd[beam[0].target_x][beam[0].target_y] != NON_MONSTER )
        {
            mpr("Oops! Maybe something was there already.");
            random_blink(false);
        }
        else
        {
            you.x_pos = beam[0].target_x;
            you.y_pos = beam[0].target_y;

            if ( you.level_type == LEVEL_ABYSS )
            {
                abyss_teleport();
                env.cloud_no = 0;
                you.pet_target = MHITNOT;
            }
        }
    }

    return;

}          // end blink()




void random_blink( bool allow_partial_control )
{

    FixedVector<int, 2> passed;

    if ( scan_randarts(RAP_PREVENT_TELEPORTATION) )
      mpr("You feel a weird sense of stasis.");
    else if ( !random_near_space(passed) || ( you.x_pos == passed[0] && you.y_pos == passed[1] ) )
      mpr("You feel rather strange for a moment.");
    else if ( you.level_type == LEVEL_ABYSS && !one_chance_in(3) )
      mpr("The power of the Abyss keeps you in your place!");

#ifdef USE_SEMI_CONTROLLED_BLINK
//jmf: add back control, but effect is cast_semi_controlled_blink(pow)
    else if ( you.attribute[ATTR_CONTROL_TELEPORT] && !you.conf && allow_partial_control && allow_control_teleport() )
    {
        mpr("You may select the general direction of your translocation.");
        cast_semi_controlled_blink(100);
    }
#endif

    else
    {
        mpr("You blink.");

        you.x_pos = passed[0];
        you.y_pos = passed[1];

        if ( you.level_type == LEVEL_ABYSS )
        {
            abyss_teleport();
            env.cloud_no = 0;
            you.pet_target = MHITNOT;
        }
    }

    return;

}          // end random_blink()




void fireball( int power )
{

    struct dist fire_ball[1];

    mpr("Which direction? (*/+ to target)");

    message_current_target();

    direction(1, fire_ball);

    if ( fire_ball[0].nothing == -1 )
      canned_msg(MSG_SPELL_FIZZLES);
    else
    {
        struct bolt beam[1];

        beam[0].source_x = you.x_pos;
        beam[0].source_y = you.y_pos;
        beam[0].move_x = fire_ball[0].move_x;
        beam[0].move_y = fire_ball[0].move_y;
        beam[0].target_x = fire_ball[0].target_x;
        beam[0].target_y = fire_ball[0].target_y;

        zapping(ZAP_FIREBALL, power, &beam[0]);
    }

    return;

}          // end fireball()




void cast_fire_storm( int powc )
{

    char stx = 0, sty = 0;
    char cl_x = 0, cl_y = 0;

    struct bolt beam[1];
    struct dist fire_storm[1];

    mpr("Where?");

    direction(100, fire_storm);

    beam[0].move_x = fire_storm[0].move_x;
    beam[0].move_y = fire_storm[0].move_y;
    beam[0].target_x = fire_storm[0].target_x;
    beam[0].target_y = fire_storm[0].target_y;

    if ( fire_storm[0].nothing == -1 )
    {
        canned_msg(MSG_SPELL_FIZZLES);
        return;
    }

    cl_x = beam[0].target_x;
    cl_y = beam[0].target_y;

    mpr("A raging storm of fire appears!");

    beam[0].colour = RED;

    for (stx = -2; stx < 3; stx++)
      for (sty = -2; sty < 3; sty++)
      {
          if ( ( stx == -2 && sty == -2 ) || ( stx == -2 && sty == 2 )
                  || ( stx == 2 && sty == -2 ) || ( stx == 2 && sty == 2 ) )
            continue;

          beam[0].range = 1 + random2(5) + ( random2(powc) / 5 ) + ( random2(powc) / 5 );     // = 1;

          if ( beam[0].range > 20 )
            beam[0].range = 20 + random2(4);

          beam[0].type = 1;
          beam[0].target_x = cl_x + stx;
          beam[0].target_y = cl_y + sty;

          if ( grd[beam[0].target_x][beam[0].target_y] == DNGN_FLOOR
              && mgrd[beam[0].target_x][beam[0].target_y] == NON_MONSTER
              && one_chance_in(4) )
            mons_place(MONS_FIRE_VORTEX, true, beam[0].target_x, beam[0].target_y, BEH_CONFUSED, MHITNOT, 250, you.your_level);

          if ( grd[beam[0].target_x][beam[0].target_y] > DNGN_LAST_SOLID_TILE
              && ( beam[0].target_x != you.x_pos || beam[0].target_y != you.y_pos ) )
            place_cloud(CLOUD_FIRE, beam[0].target_x, beam[0].target_y, beam[0].range);

      }        // end "for stx, sty"

    viewwindow(1, false);

}          // end cast_fire_storm()




char spell_direction( struct dist spelld[1], struct bolt *pbolt )
{

    mpr("Which direction? (*/+ to target)");

    message_current_target();

    direction(1, spelld);

    if ( spelld[0].nothing == -1 )
    {
        canned_msg(MSG_SPELL_FIZZLES);
        return -1;
    }

    pbolt->move_x = spelld[0].move_x;
    pbolt->move_y = spelld[0].move_y;
    pbolt->target_y = spelld[0].target_y;
    pbolt->target_x = spelld[0].target_x;
    pbolt->source_x = you.x_pos;
    pbolt->source_y = you.y_pos;

    return 1;

}          // end spell_direction()




void identify( int power )
{

    int id_used = 1;
    unsigned char nthing = 0;

// scrolls of identify *may* produce "extra" identifications {dlb}:
    if ( power == -1 && one_chance_in(5) )
      id_used += ( coinflip() ? 1 : 2 );

    do
    {
query:
        mpr("Identify which item?");

        unsigned char keyin = get_ch();

        if ( keyin == '*' || keyin == '?' )
        {
            if ( keyin == '?' )
              nthing = get_invent(-1);

            if ( keyin == '*' )
              nthing = get_invent(-1);

            if ( ( nthing >= 'A' && nthing <= 'Z' ) || ( nthing >= 'a' && nthing <= 'z' ) )
              keyin = nthing;
            else
            {
                //mesclr();
                goto query;
            }
        }

        if ( keyin == 27 )
          return;

        int sc_read_1 = (int) keyin;

        if ( sc_read_1 < 'A' || ( sc_read_1 > 'Z' && sc_read_1 < 'a' ) || sc_read_1 > 'z' )
        {
            mpr("You don't have any such object.");
            continue;
        }

        int sc_read_2 = letter_to_index(sc_read_1);

        if ( you.inv_quantity[sc_read_2] == 0 )
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

        strcat(info, (sc_read_2 == you.equip[EQ_WEAPON])      ? " (weapon in hand)" :
                     (sc_read_2 == you.equip[EQ_BODY_ARMOUR]) ? " (being worn)" :
                     (sc_read_2 == you.equip[EQ_LEFT_RING])   ? " (on left hand)" :
                     (sc_read_2 == you.equip[EQ_RIGHT_RING])  ? " (on right hand)" :
                     (sc_read_2 == you.equip[EQ_AMULET])      ? " (around neck)"
                                                              : "" );

        mpr(info);

        if ( sc_read_2 == you.equip[EQ_WEAPON] )
          wield_change = true;

        id_used--;

    }
    while (id_used > 0);

}                               // end identify()




void conjure_flame( int pow )
{

    struct dist spelld[1];

    mpr("You cast a flaming cloud spell! But where?");

direc:
    direction(100, spelld);

    if ( spelld[0].nothing == -1 )
    {
        canned_msg(MSG_SPELL_FIZZLES);
        return;
    }

    if ( !see_grid(spelld[0].target_x, spelld[0].target_y) )
    {
        mpr("You can't see that place!");
        goto direc;
    }

    if ( grd[spelld[0].target_x][spelld[0].target_y] <= DNGN_LAST_SOLID_TILE
          || mgrd[spelld[0].target_x][spelld[0].target_y] != NON_MONSTER
          || env.cgrid[spelld[0].target_x][spelld[0].target_y] != EMPTY_CLOUD )
    {
        mpr("There's already something there!");
        goto direc;
    }

    int durat = 5 + ( random2(pow) / 2 ) + ( random2(pow) / 2 );

    if ( durat > 23 )
      durat = 23;

    place_cloud(CLOUD_FIRE, spelld[0].target_x, spelld[0].target_y, durat);

}          // end cast_conjure_flame()




void stinking_cloud( void )
{

    struct dist spelld[1];
    struct bolt beam[1];

    mpr("Which direction? (*/+ to target)");

    message_current_target();

    direction(1, spelld);

    if ( spelld[0].nothing == -1 )
    {
        canned_msg(MSG_SPELL_FIZZLES);
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
    beam[0].type = SYM_ZAP;
    beam[0].flavour = BEAM_ACID;       // exploding gas on target // 7; // <- I wonder if 7 == (new) BEAM_CLOUD {dlb}
    beam[0].thing_thrown = KILL_MON_MISSILE;           //? ???? ?

    missile(&beam[0], -1);

    beam[0].damage = 3 * you.intel;    // yep, that's what it broke down to {dlb}

    stinkcl(beam[0].bx, beam[0].by, beam);

}          // end stinking_cloud()




void stinkcl( char cl_x, char cl_y, struct bolt beam[1] )
{

    char stx = 0;
    char sty = 0;

    for (stx = -1; stx < 2; stx++)
      for (sty = -1; sty < 2; sty++)
      {
          beam[0].range = 1 + random2(5) + random2(beam[0].damage / 50);

          if ( beam[0].range > 10 )
            beam[0].range = 10 + random2(3);

          beam[0].type = 2;               // which is ?

          beam[0].target_x = cl_x + stx;
          beam[0].target_y = cl_y + sty;

          if ( grd[beam[0].target_x][beam[0].target_y] > DNGN_LAST_SOLID_TILE )
            place_cloud(CLOUD_STINK, beam[0].target_x, beam[0].target_y, beam[0].range);

      }        // end "for stx, sty"

}          // end stinkcl()




void cast_big_c( int pow, char cty )
{

    mpr("Where do you want to put it?");

    struct dist cdis[1];

    direction(100, cdis);

    if ( cdis[0].nothing == -1 )
      {
        canned_msg(MSG_SPELL_FIZZLES);
        return;
      }


    //beam[0].damage = pow;

    big_cloud(cty, cdis[0].target_x, cdis[0].target_y, pow, 8 + random2(3));

}          // end cast_big_c()



void big_cloud( char clouds, char cl_x, char cl_y, int pow, int size )
{

#ifdef USE_NEW_CLOUD_CODE

    apply_area_cloud(make_a_normal_cloud, cl_x, cl_y, pow, size, clouds);

#else

    char stx = 0;
    char sty = 0;
    int lasts = 0;

    for (stx = -1; stx < 2; stx++)
      for (sty = -1; sty < 2; sty++)
      {

          lasts = 2 + random2(5) + ( pow / 2 );

          if ( lasts > 12 )
            lasts = 12 + random2(4);

          if ( grd[cl_x + stx][cl_y + sty] > DNGN_LAST_SOLID_TILE )
            place_cloud(clouds, cl_x + stx, cl_y + sty, lasts);

      }                       // end of sty

#endif

}          // end big_cloud()




char cast_lesser_healing( void )
{

    return healing_spell(5 + random2avg(7,2));

}          // end lesser healing()




char cast_greater_healing( void )
{

    return healing_spell(15 + random2avg(29,2));

}          // end cast_greater_healing()




char cast_greatest_healing( void )
{

    return healing_spell(50 + random2avg(49,2));

}          // end cast_greatest_healing()




char healing_spell( int healed )
{

    int mgr = 0;
    struct monsters *monster = 0;    // NULL {dlb}
    struct dist bmove[1];

dirc:
    mpr("Which direction?");
    direction(0, bmove);

    mgr = mgrd[you.x_pos + bmove[0].move_x][you.y_pos + bmove[0].move_y];

    if ( bmove[0].nothing == -1 )
    {
        mpr("Huh?!");
        return 0;
    }

    if ( bmove[0].move_x > 1 || bmove[0].move_y > 1 )
    {
        mpr("This spell doesn't reach that far.");
        goto dirc;
    }

    if ( bmove[0].move_x == 0 && bmove[0].move_y == 0 )
    {
        mpr("You are healed.");
        inc_hp(healed, false);
        return 1;
    }

    if ( mgr == NON_MONSTER )
    {
        mpr("There isn't anything there!");
        return -1;
    }

    monster = &menv[mgr];

    if ( heal_monster(monster, healed, false) )
    {
        strcpy(info, "You heal ");
        strcat(info, monam(monster->number, monster->type, monster->enchantment[2], 1));
        strcat(info, ".");
        mpr(info);

        if ( monster->hit_points == monster->max_hit_points )
          simple_monster_message(monster, " is completely healed.");
        else
          print_wounds(monster);
    }
    else
    {
        canned_msg(MSG_NOTHING_HAPPENS);
    }

    return 1;

}          // end healing_spell()




bool cast_revivification( int power )
{

    int loopy = 0;           // general purpose loop variable {dlb}
    bool success = false;

    if ( you.hp == you.hp_max )
      canned_msg(MSG_NOTHING_HAPPENS);
    else if ( you.hp_max < 21 )
      mpr("You lack the resilience to cast this spell.");
    else
    {
        mpr("Your body is healed in an amazingly painful way.");

        you.base_hp -= 2;

        for (loopy = 0; loopy < 9; loopy++)
          if ( random2(power) < 8 )
            you.base_hp--;

        calc_hp();

        set_hp(you.hp_max, false);

        success = true;
    }

    return ( success );

}          // end cast_revivification()




void cast_cure_poison( int mabil )
{

    if ( !you.poison )
      canned_msg(MSG_NOTHING_HAPPENS);
    else
    {
        you.poison -= 2 + random2(mabil) + random2(3);

        if ( you.poison < 1 )
        {
            mpr("You feel the poison leave your system.");
            you.poison = 0;
        }
        else
          mpr("You feel most of the poison leave your system.");
    }

    return;

}          // end cast_cure_poison()




void purification( void )
{

    mpr("You feel purified!");

    you.poison = 0;
    you.rotting = 0;
    you.conf = 0;
    you.slow = 0;
    you.paralysis = 0;          // ?
    you.disease = 0;            // this doesn't seem very useful, does it?

}          // end purification()




void cast_deaths_door( int pow )
{

    if ( you.is_undead )
      mpr("You're already dead!");
    else if ( you.deaths_door )
      mpr("Your appeal for an extension has been denied.");
    else
    {
        mpr("You feel invincible!");
        mpr("You seem to hear sand running through an hourglass...");

        you.deaths_door = 8 + random2avg(13,3) + (random2(pow) / 10);

        set_hp(you.skills[SK_NECROMANCY], false);

        if ( you.religion == GOD_KIKUBAAQUDGHA )       // add penance check, too? {dlb}
          inc_hp(13, false);                           // should scale to piety w/ kikub? {dlb}

        deflate_hp(you.hp_max, false);
    }

    return;

}




// can't use beam variables here, because of monster_die and the puffs of smoke
void abjuration( int pow )
{

    struct monsters *monster = 0;    // NULL {dlb}

    mpr("Send 'em back where they came from!");

    for (int ab = 0; ab < MAX_MONSTERS; ab++)
    {
        monster = &menv[ab];

        if ( monster->type == -1 || !mons_near(monster) )
          continue;

        if ( monster->behavior == BEH_ENSLAVED )
          continue;

        if ( monster->enchantment1 == 0
              || monster->enchantment[1] < ENCH_ABJ_I
              || ( monster->enchantment[1] > ENCH_ABJ_VI && monster->enchantment[1] < ENCH_FRIEND_ABJ_I )
              || monster->enchantment[1] > ENCH_FRIEND_ABJ_VI )
          continue;

        monster->enchantment[1] -= 1 + (random2(pow) / 3);

        if ( monster->enchantment[1] <= ENCH_SUMMON )
          monster_die(monster, KILL_RESET, 0);
        else
          simple_monster_message(monster, " shudders.");
    }

}          // end abjuration()




void extension( int pow )
{

    if ( you.haste )
      potion_effect(POT_SPEED, pow);

    if ( you.might )
      potion_effect(POT_MIGHT, pow);

    if ( you.levitation )
      potion_effect(POT_LEVITATION, pow);

    if ( you.invis )
      potion_effect(POT_INVISIBILITY, pow);

    if ( you.duration[DUR_ICY_ARMOUR] )
      ice_armour(pow, true);

    if ( you.duration[DUR_REPEL_MISSILES] )
      missile_prot(pow);

    if ( you.duration[DUR_REGENERATION] )
      cast_regen(pow);

    if ( you.duration[DUR_DEFLECT_MISSILES] )
      deflection(pow);

    if ( you.shock_shield )
    {
        you.shock_shield += random2(pow) + 4;

        if ( you.shock_shield > 25 )
          you.shock_shield = 25;

        mpr("Your ring of flames spell is extended.");
    }

    if ( !(you.duration[DUR_WEAPON_BRAND] < 1 || you.duration[DUR_WEAPON_BRAND] > 80) )
      you.duration[DUR_WEAPON_BRAND] += 10 + random2(10);

    if ( you.duration[DUR_SWIFTNESS] )
      cast_swiftness(pow);

    if ( you.duration[DUR_INSULATION] )
      cast_insulation(pow);

    if ( you.duration[DUR_STONEMAIL] )
      stone_scales(pow);

    if ( you.duration[DUR_CONTROLLED_FLIGHT] )
      cast_fly(pow);

// 13 is teleport countdown

    if ( you.duration[DUR_CONTROL_TELEPORT] )
      cast_teleport_control(pow);

    if ( you.duration[DUR_RESIST_POISON] )
      cast_resist_poison(pow);

// 17 is breath

    if ( you.duration[DUR_TRANSFORMATION] )
    {
        mpr("Your transformation has been extended.");
        you.duration[DUR_TRANSFORMATION] += 10 + random2(pow);
        if (you.duration[DUR_TRANSFORMATION] > 100)
            you.duration[DUR_TRANSFORMATION] = 100;
    }

//jmf: added following
    if ( you.duration[DUR_FORESCRY] )
      cast_forescry(pow);
    if ( you.duration[DUR_SEE_INVISIBLE] )
      cast_see_invisible(pow);
    if ( you.duration[DUR_SILENCE] ) //how precisely did you cast extension?
      cast_silence(pow);
    if ( you.duration[DUR_CONDENSATION_SHIELD] )
      cast_condensation_shield(pow);
    if ( you.duration[DUR_DEATH_CHANNEL] )
      cast_death_channel(pow);

}          // end extension()




void ice_armour( int pow, bool extending )
{

    //if (pow > 100) pow = 100;

    int dur_change = 0;

    if (you.equip[EQ_BODY_ARMOUR] != -1 && !extending )
    {
        if (you.inv_type[you.equip[EQ_BODY_ARMOUR]] > 1 && you.inv_type[you.equip[EQ_BODY_ARMOUR]] != 16 && you.inv_type[you.equip[EQ_BODY_ARMOUR]] != 19 && (you.inv_type[you.equip[EQ_BODY_ARMOUR]] < 22 || you.inv_type[you.equip[EQ_BODY_ARMOUR]] > 25))
        {
            mpr("You are wearing too much armour.");
            return;
        }
    }

    if ( you.duration[DUR_STONEMAIL] )
    {
        mpr("The spell conflicts with another spell still in effect.");
        return;
    }

    if ( !you.duration[DUR_ICY_ARMOUR] )
    {
        mpr("A film of ice covers your body!");
        you.redraw_armor_class = 1;
    }
    else
      mpr("Your icy armour thickens.");

    dur_change = 20 + random2(pow) + random2(pow);

    if ( dur_change + you.duration[DUR_ICY_ARMOUR] >= 100 )
      you.duration[DUR_ICY_ARMOUR] = 100;
    else
      you.duration[DUR_ICY_ARMOUR] += dur_change;
/*
 you.duration [DUR_ICY_ARMOUR] += 20 + random2(pow) / 2 + random2(pow) / 2;
 if (you.duration [DUR_ICY_ARMOUR] >= 100) you.duration [DUR_ICY_ARMOUR] = 100;
*/

}          // end ice_armour()




void stone_scales( int pow )
{

    //if (pow > 100)
    //  pow = 100;

    int dur_change = 0;

    if ( you.duration[DUR_ICY_ARMOUR] )
    {
      mpr("The spell conflicts with another spell still in effect.");
      return;
    }

    if ( !you.duration[DUR_STONEMAIL] )
    {
      mpr("A set of stone scales covers your body!");
      you.redraw_evasion = 1;
      you.redraw_armor_class = 1;
    }
    else
      mpr("Your scaly armour looks firmer.");

    dur_change = 20 + random2(pow) + random2(pow);

    if (dur_change + you.duration[DUR_STONEMAIL] >= 100)
      you.duration[DUR_STONEMAIL] = 100;
    else
      you.duration[DUR_STONEMAIL] += dur_change;

    burden_change();

}          // end stone_scales()




void missile_prot( int pow )
{

    if ( pow > 100 )
      pow = 100;

    mpr("You feel protected from missiles.");

    you.duration[DUR_REPEL_MISSILES] += 10 + random2avg( 2*(pow-1) + 1, 2 );

    if ( you.duration[DUR_REPEL_MISSILES] > 100 )
      you.duration[DUR_REPEL_MISSILES] = 100;

}          // end missile_prot()




void deflection( int pow )
{

    if ( pow > 100 )
      pow = 100;

    mpr("You feel very safe from missiles.");

    you.duration[DUR_DEFLECT_MISSILES] += 15 + random2(pow);

    if ( you.duration[DUR_DEFLECT_MISSILES] > 100 )
      you.duration[DUR_DEFLECT_MISSILES] = 100;

}          // end cast_deflection()




void cast_regen( int pow )
{

    //if (pow > 150) pow = 150;

    mpr("Your skin crawls.");

    you.duration[DUR_REGENERATION] += 5 + random2avg( 3*(pow-1) + 1, 3 );

    if ( you.duration[DUR_REGENERATION] > 100 )
      you.duration[DUR_REGENERATION] = 100;

}          // end cast_regen()




void cast_berserk( void )
{

    if ( !go_berserk() )    // I don't like how this will stack messages {dlb}
      mpr("You fail to go berserk.");

}          // end cast_berserk()




void cast_swiftness( int power )
{

    int dur_incr = 0;

    if ( !you.duration[DUR_SWIFTNESS] )
    {
        if ( player_fast_run() )
        {
            mpr("You can already move quickly.");
            return;
        }
    }

    dur_incr = random2(power) + random2(power) + 20;

    strcpy(info, "You feel quick");
    strcat(info, (you.species == SP_NAGA || you.species == SP_CENTAUR) ? "." : " on your feet.");
    mpr(info);

    if ( dur_incr + you.duration[DUR_SWIFTNESS] > 100 )
      you.duration[DUR_SWIFTNESS] = 100;
    else
      you.duration[DUR_SWIFTNESS] += dur_incr;

}          // end cast_swiftness()




void cast_fly( int power )
{

    int dur_change = 25 + random2(power) + random2(power);

    if (!you.levitation)
      mpr("You fly up into the air.");
    else
      mpr("You feel more buoyant.");

    if ( you.levitation + dur_change > 100 )
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




void cast_insulation( int power )
{

    int dur_incr = random2(power) + 10;

    mpr("You feel insulated.");

    if (dur_incr + you.duration[DUR_INSULATION] > 100)
        you.duration[DUR_INSULATION] = 100;
    else
        you.duration[DUR_INSULATION] += dur_incr;

}          // end cast_insulation()




void cast_resist_poison( int power )
{

    int dur_incr = random2(power) + 10;

    mpr("You feel resistant to poison.");

    if (dur_incr + you.duration[DUR_RESIST_POISON] > 100)
        you.duration[DUR_RESIST_POISON] = 100;
    else
        you.duration[DUR_RESIST_POISON] += dur_incr;

}          // end cast_resist_poison()




void cast_teleport_control( int power )
{

    int dur_incr = random2(power) + 10;

    if ( you.duration[DUR_CONTROL_TELEPORT] == 0 )
      you.attribute[ATTR_CONTROL_TELEPORT]++;

    mpr("You feel in control.");

    if ( dur_incr + you.duration[DUR_CONTROL_TELEPORT] >= 100 )
      you.duration[DUR_CONTROL_TELEPORT] = 100;
    else
      you.duration[DUR_CONTROL_TELEPORT] += dur_incr;

}          // end cast_teleport_control()




void cast_ring_of_flames( int power )
{

    you.shock_shield += 4 + (random2(power) / 10);

    if (you.shock_shield > 25)
      you.shock_shield = 25;

    mpr("The air around you leaps into flame!");

    manage_shock_shield();

}          // end cast_ring_of_flames()




void cast_confusing_touch( int power )
{

    if ( !you.confusing_touch )
      mpr( "Your hands begin to glow red." );
    else if ( you.confusing_touch < 50 )
      mpr( "Your hands begin to glow brighter." );

    you.confusing_touch += 5 + (random2(power) / 5);

    if ( you.confusing_touch > 50 )
      you.confusing_touch = 50;

}          // end cast_confusing_touch()




bool cast_sure_blade( int power )
{

    bool success = false;

    if ( you.equip[EQ_WEAPON] == -1 )
      mpr("You aren't wielding a weapon!");
    else if ( weapon_skill(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]]) != SK_SHORT_BLADES )
      mpr("You cannot bond with this weapon.");
    else
    {
        if ( !you.sure_blade )
          mpr("You become one with your weapon.");
        else if ( you.sure_blade < 25 )
          mpr("Your bond becomes stronger.");

        you.sure_blade += 8 + (random2( power ) / 10);

        if ( you.sure_blade > 25 )
          you.sure_blade = 25;

        success = true;
    }


    return ( success );

}          // end cast_sure_blade()




void manage_shock_shield( void )
{

    you.shock_shield--;

    if ( !you.shock_shield )
      return;

    char stx = 0, sty = 0;

    for (stx = -1; stx < 2; stx++)
      for (sty = -1; sty < 2; sty++)
      {
          if ( sty == 0 && stx == 0 )
            continue;

          //if ( one_chance_in(3) ) beam[0].range ++;

          if ( grd[you.x_pos + stx][you.y_pos + sty] > DNGN_LAST_SOLID_TILE
              && env.cgrid[you.x_pos + stx][you.y_pos + sty] == EMPTY_CLOUD )
            place_cloud(CLOUD_FIRE, you.x_pos + stx, you.y_pos + sty, ( one_chance_in(6) ? 3 : 1 ));
      }

}          // end manage_shock_shield()

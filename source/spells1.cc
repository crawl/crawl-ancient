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
#include <stdio.h>

#include "globals.h"
#include "externs.h"

#include "abyss.h"
#include "beam.h"
#include "cloud.h"
#include "direct.h"
#include "invent.h"
#include "it_use2.h"
#include "itemname.h"
#include "itemprop.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mon-util.h"
#include "ouch.h"
#include "player.h"
#include "skills2.h"
#include "spells2.h"
#include "spells3.h"
#include "spells4.h"
#include "spl-util.h"
#include "stuff.h"
#include "view.h"

bool something_there( int x, int y )
{
    return ((x == you.x_pos && y == you.y_pos)
            || grid_is_solid( grd[x][y] )
            || (mgrd[x][y] != NON_MONSTER
                && !mons_is_submerged( &menv[ mgrd[x][y] ] ))
            || is_cloud( x, y ));
}

// Not actually used for the fireball spell, but for delayed fireball ability.
int fireball( int power )
{
    struct dist fire_ball;

    mpr( MSGCH_PROMPT, STD_DIRECTION_PROMPT );
    message_current_target();

    direction( fire_ball, DIR_NONE, TARG_ENEMY );

    if (!fire_ball.isValid)
    {
        canned_msg(MSG_SPELL_FIZZLES);
        return (SPRET_ABORT);
    }

    struct bolt beam;

    beam.source_x = you.x_pos;
    beam.source_y = you.y_pos;
    beam.target_x = fire_ball.tx;
    beam.target_y = fire_ball.ty;

    zapping( ZAP_FIREBALL, power, beam );

    return (SPRET_SUCCESS);
}                               // end fireball()

void cast_fire_storm( int powc, struct bolt &beam )
{
    beam.ex_size = 2 + (random2(powc) > 75);
    beam.flavour = BEAM_LAVA;
    beam.type = SYM_ZAP;
    beam.colour = RED;
    beam.beam_source = MHITYOU;
    beam.thrower = KILL_YOU_MISSILE;
    beam.aux_source = NULL;
    beam.obvious_effect = false;
    beam.is_beam = false;
    beam.is_explosion = true;
    beam.is_enchant = false;
    beam.is_tracer = false;
    beam.is_energy = true;
    beam.ench_power = powc;     // used for radius
    strcpy( beam.name, "great blast of fire" );
    beam.hit = 100;
    beam.damage = calc_dice( 3, 15 + (powc * 3) / 5 );

    explosion( beam );
    mpr("A raging storm of fire appears!");

    viewwindow(true, false);
}                               // end cast_fire_storm()

void cast_chain_lightning( int powc )
{
    struct bolt beam;

    // initialize beam structure
    strcpy( beam.name, "lightning arc" );
    beam.aux_source = "chain lightning";
    beam.beam_source = MHITYOU;
    beam.thrower = KILL_YOU_MISSILE;
    beam.range = 8;
    beam.rangeMax = 8;
    beam.hit = AUTOMATIC_HIT;
    beam.type = SYM_ZAP;
    beam.flavour = BEAM_ELECTRICITY;
    beam.obvious_effect = true;
    beam.is_beam = false;            // since we want to stop at our target
    beam.is_enchant = false;
    beam.is_explosion = false;
    beam.is_tracer = false;
    beam.is_energy = true;

    int sx, sy;
    int tx, ty;
    int i;

    for (sx = you.x_pos, sy = you.y_pos;
         powc > 0;
         powc -= 8 + random2(13), sx = tx, sy = ty)
    {
        // infinity as far as this spell is concerned
        // (Range - 1) is used because the distance is randomized and
        // may be shifted by one.
        int min_dist = MONSTER_LOS_RANGE - 1;

        int dist;
        int count = 0;

        tx = -1;
        ty = -1;

        for (i = 0; i < MAX_MONSTERS; i++)
        {
            struct monsters *monster = &menv[i];

            if (monster->type == -1)
                continue;

            dist = grid_distance( sx, sy, monster->x, monster->y );

            // check for the source of this arc
            if (!dist)
                continue;

            // randomize distance (arcs don't care about a couple of feet)
            dist += (random2(3) - 1);

            // always ignore targets further than current one
            if (dist > min_dist)
                continue;

            if (!check_line_of_sight( sx, sy, monster->x, monster->y ))
                continue;

            count++;

            if (dist < min_dist)
            {
                // switch to looking for closer targets (but not always)
                if (!one_chance_in(10))
                {
                    min_dist = dist;
                    tx = monster->x;
                    ty = monster->y;
                    count = 0;
                }
            }
            else if (tx == -1 || one_chance_in( count ))
            {
                // either first target, or new selected target at min_dist
                tx = monster->x;
                ty = monster->y;

                // need to set min_dist for first target case
                if (dist < min_dist)
                    min_dist = dist;
            }
        }

        // now check if the player is a target:
        dist = grid_distance( sx, sy, you.x_pos, you.y_pos );

        if (dist)       // ie player was not the source
        {
            // distance randomized (as above)
            dist += (random2(3) - 1);

            // select player if only, closest, or randomly selected
            if ((tx == -1
                    || dist < min_dist
                    || (dist == min_dist && one_chance_in( count + 1 )))
                && check_line_of_sight( sx, sy, you.x_pos, you.y_pos ))
            {
                tx = you.x_pos;
                ty = you.y_pos;
            }
        }

        const bool see_source = see_grid( sx, sy );
        const bool see_targ   = see_grid( tx, ty );

        if (tx == -1)
        {
            if (see_source)
                mpr( "The lightning grounds out." );

            break;
        }

        // Trying to limit message spamming here so we'll only mention
        // the thunder when it's out of LoS.
        if (noisy( SL_THUNDER, sx, sy ) && !see_source)
            mpr( "You hear a mighty clap of thunder!" );

        if (see_source && !see_targ)
            mpr( "The lightning arcs out of your line of sight!" );
        else if (!see_source && see_targ)
            mpr( "The lightning arc suddenly appears!" );

        beam.source_x = sx;
        beam.source_y = sy;
        beam.target_x = tx;
        beam.target_y = ty;
        beam.colour = element_colour( EC_ELECTRICITY );
        beam.damage = calc_dice( 2, 10 + powc / 2 );        // from beam.cc
        fire_beam( beam );
    }

    more();
}

void identify(int power)
{
    int id_used = 1;
    int item_slot;
    char str_pass[ ITEMNAME_SIZE ];

    // scrolls of identify *may* produce "extra" identifications {dlb}:
    if (power == -1 && one_chance_in(5))
        id_used += (coinflip()? 1 : 2);

    do
    {
        item_slot = prompt_invent_item( "Identify which item?", -1, true,
                                        false, false );
        if (item_slot == PROMPT_ABORT)
        {
            canned_msg( MSG_OK );
            return;
        }

        set_ident_type( you.inv[item_slot].base_type,
                        you.inv[item_slot].sub_type, ID_KNOWN_TYPE );

        set_ident_flags( you.inv[item_slot], ISFLAG_IDENT_MASK );

        // output identified item
        in_name( item_slot, DESC_INVENTORY_EQUIP, str_pass );
        mpr( str_pass );

        if (item_slot == you.equip[EQ_WEAPON])
            set_redraw_status( REDRAW_WIELD );

        id_used--;
    }
    while (id_used > 0);
}                               // end identify()

int cast_small_cloud( int pow, struct bolt &beam, int cloud )
{
    if (something_there( beam.target_x, beam.target_y ))
    {
        mpr( "There's already something there!" );
        canned_msg( MSG_SPELL_FIZZLES );
        return (SPRET_FAIL);
    }

    int durat = 5 + (random2(pow) / 2) + (random2(pow) / 2);

    if (durat > 23)
        durat = 23;

    place_cloud( cloud, beam.target_x, beam.target_y, durat );

    return (SPRET_SUCCESS);
}

void stinking_cloud( int pow, struct bolt &beam )
{
    strcpy( beam.name, "ball of vapour" );
    beam.colour = GREEN;
    beam.range = 7;
    beam.rangeMax = 7;
    beam.damage = dice_def( 1, 0 );
    beam.hit = 20 + pow / 5;
    beam.type = SYM_ZAP;
    beam.flavour = BEAM_MMISSILE;
    beam.ench_power = pow;
    beam.beam_source = MHITYOU;
    beam.thrower = KILL_YOU;
    beam.aux_source = NULL;
    beam.is_beam = false;
    beam.is_enchant = false;
    beam.is_energy = true;
    beam.is_tracer = false;
    beam.is_explosion = true;

    fire_beam( beam );
}                               // end stinking_cloud()

void big_cloud( char clouds, char cl_x, char cl_y, int pow, int size )
{
    apply_area_cloud( make_a_normal_cloud, cl_x, cl_y, pow, size, clouds );
}                               // end big_cloud()

void cast_big_cloud( int pow, struct bolt &beam, int cloud )
{
    big_cloud( cloud, beam.target_x, beam.target_y, pow / 10, 8 + random2(3) );
}                               // end cast_big_c()

static int healing_spell( int healed )
{
    int mgr = 0;
    struct monsters *monster = 0;       // NULL {dlb}
    struct dist bmove;

    mpr(MSGCH_PROMPT,"Which direction?" );
    direction( bmove, DIR_DIR, TARG_FRIEND );

    if (!bmove.isValid)
    {
        canned_msg( MSG_HUH );
        return (SPRET_ABORT);
    }

    mgr = mgrd[you.x_pos + bmove.dx][you.y_pos + bmove.dy];

    if (bmove.dx == 0 && bmove.dy == 0)
    {
        mpr( "You are healed." );
        inc_hp( healed, false );
        you.duration[DUR_CUT] = 0;
        return (SPRET_SUCCESS);
    }

    if (mgr == NON_MONSTER)
    {
        mpr("There isn't anything there!");
        return (SPRET_FAIL);  // not ABORT since could be trying for invis mon
    }

    monster = &menv[mgr];

    if (heal_monster( monster, healed, false ))
    {
        mon_msg( monster, "You heal %s.", false );

        if (monster->hit_points == monster->max_hit_points)
            mon_msg( monster, " is completely healed." );
        else
            print_wounds( monster );
    }
    else
    {
        canned_msg( MSG_NOTHING_HAPPENS );
    }

    return (SPRET_SUCCESS);
}                               // end healing_spell()

int cast_healing( int pow )
{
    if (pow > 50)
        pow = 50;

    return (healing_spell( pow + roll_dice(2, pow) ));
}

bool cast_revivification(int power)
{
    bool success = false;

    if (you.hp == you.hp_max)
        canned_msg( MSG_NOTHING_HAPPENS );
    else if (you.hp_max < 21)
        mpr( "You lack the resilience to cast this spell." );
    else
    {
        mpr( "Your body is healed in an amazingly painful way." );

        const int dam = you.hp_max - you.hp;

        int loss = 3 + random2(4);
        const int max_extra = 9 + random2(dam) / 5;

        for (int loopy = 0; loopy < max_extra; loopy++)
        {
            if (random2(power) < 8)
                loss++;
        }

        dec_max_hp( loss );
        set_hp( you.hp_max, false );
        success = true;
    }

    return (success);
}                               // end cast_revivification()

void cast_cure_poison(int mabil)
{
    if (!you.poison)
        canned_msg( MSG_NOTHING_HAPPENS );
    else
        reduce_poison_player( 2 + random2(mabil) + random2(3) );

    return;
}                               // end cast_cure_poison()

void purification(void)
{
    mpr("You feel purified!");

    you.poison = 0;
    you.rotting = 0;
    you.conf = 0;
    you.slow = 0;
    you.disease = 0;
    you.paralysis = 0;          // can't currently happen -- bwr
    you.duration[DUR_STUN] = 0;
}                               // end purification()

int allowed_deaths_door_hp(void)
{
    int hp = you.skills[SK_NECROMANCY] / 2;

    if (you.religion == GOD_KIKUBAAQUDGHA && !player_under_penance())
        hp += you.piety / 15;

    return (hp);
}

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

        const int hp = allowed_deaths_door_hp();

        if (you.hp > hp)
            set_hp( hp, false );

        you.deaths_door = 7 + roll_dice(3,5) + (random2(pow) / 10);

        if (you.deaths_door > 25)
            you.deaths_door = 23 + random2(5);
    }

    return;
}

// can't use beam variables here, because of monster_die and the puffs of smoke
void abjuration(int pow)
{
    mpr( "Send 'em back where they came from!" );

    for (int ab = 0; ab < MAX_MONSTERS; ab++)
    {
        struct monsters *const mon = &menv[ab];

        if (mon->type == -1
            || !mons_near(mon)
            || mons_friendly(mon))
        {
            continue;
        }

        mon_enchant_def *const ench = mons_has_ench( mon, ENCH_SUMMONED );

        if (!ench)
            continue;

        const int dur = levels_to_dur( ENCH_SUMMONED, 1 + (random2(pow) / 8) );

        if (!mons_dec_ench_ptr( mon, ench, dur ))
            mon_msg( mon, " shudders." );
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

    if (you.slow)
        you.slow = 1;

    if (you.conf)
        you.conf = 1;

    if (you.might)
        you.might = 1;

    if (you.levitation > 2)
        you.levitation = 2;

    if (you.invis)
        you.invis = 1;

    if (you.duration[DUR_WEAPON_BRAND])
        you.duration[DUR_WEAPON_BRAND] = 1;

    if (you.duration[DUR_ICY_ARMOUR])
        you.duration[DUR_ICY_ARMOUR] = 1;

    if (you.duration[DUR_REPEL_MISSILES])
        you.duration[DUR_REPEL_MISSILES] = 1;

    if (you.duration[DUR_REGENERATION])
        you.duration[DUR_REGENERATION] = 1;

    if (you.duration[DUR_DEFLECT_MISSILES])
        you.duration[DUR_DEFLECT_MISSILES] = 1;

    if (you.fire_shield)
        you.fire_shield = 1;

    if (you.confusing_touch)
        you.confusing_touch = 1;

    if (you.sure_blade)
        you.sure_blade = 1;

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

    contaminate_player( -1 * (1+random2(5)));
}                               // end antimagic()

void extension( int pow )
{
    int contamination = random2(2);

    if (you.haste)
    {
        potion_effect( POT_SPEED, pow );
        contamination++;
    }

    if (you.slow)
        potion_effect( POT_SLOWING, pow );

#if 0
    if (you.conf)
        potion_effect(POT_CONFUSION, pow);  // how did you cast extension?
#endif

    if (you.might)
    {
        potion_effect( POT_MIGHT, pow );
        contamination++;
    }

    if (you.invis)
    {
        potion_effect( POT_INVISIBILITY, pow );
        contamination++;
    }

    if (you.duration[DUR_ICY_ARMOUR])
        ice_armour( pow, true );

    if (you.duration[DUR_REPEL_MISSILES])
        missile_prot( pow );

    if (you.duration[DUR_DEFLECT_MISSILES])
        deflection( pow );

    if (you.duration[DUR_REGENERATION])
        cast_regen( pow );

    if (you.fire_shield)
    {
        you.fire_shield += 5 + random2(pow / 10);

        if (you.fire_shield > 50)
            you.fire_shield = 50;

        mpr( "Your ring of flames roars with new vigour!" );
    }

    const int wpn = get_inv_wielded();

    if (you.duration[DUR_WEAPON_BRAND] && wpn != -1)
        brand_weapon( get_weapon_brand( you.inv[wpn] ), pow );

    if (you.confusing_touch)
        cast_confusing_touch( pow );

    if (you.sure_blade)
        cast_sure_blade( pow );

    if (you.duration[DUR_SWIFTNESS])
        cast_swiftness( pow );

    if (you.duration[DUR_INSULATION])
        cast_insulation( pow );

    if (you.duration[DUR_STONEMAIL])
        stone_scales( pow );

    if (you.duration[DUR_CONTROLLED_FLIGHT])
        cast_fly( pow );
    else if (you.levitation)
        potion_effect( POT_LEVITATION, pow );

    if (you.duration[DUR_CONTROL_TELEPORT])
        cast_teleport_control( pow );

    if (you.duration[DUR_RESIST_POISON])
        cast_resist_poison( pow );

    if (you.duration[DUR_TRANSFORMATION])
    {
        mpr("Your transformation has been extended.");
        you.duration[DUR_TRANSFORMATION] += 5 + random2( pow );
        if (you.duration[DUR_TRANSFORMATION] > 100)
            you.duration[DUR_TRANSFORMATION] = 100;
    }

    //jmf: added following
    if (you.duration[DUR_STONESKIN])
        cast_stoneskin( pow );

    if (you.duration[DUR_FORESCRY])
        cast_forescry( pow );

    if (you.duration[DUR_SEE_INVISIBLE])
        cast_see_invisible( pow );

    if (you.duration[DUR_SILENCE])   //how precisely did you cast extension?
        cast_silence( pow );

    if (you.duration[DUR_CONDENSATION_SHIELD])
        cast_condensation_shield( pow );

    if (contamination)
        contaminate_player( contamination );
}                               // end extension()

void ice_armour( int pow, bool extending )
{
    if (!player_light_armour())
    {
        if (!extending)
            mpr("You are wearing too much armour.");

        return;
    }

    if (you.duration[DUR_STONEMAIL] || you.duration[DUR_STONESKIN])
    {
        if (!extending)
            mpr("The spell conflicts with another spell still in effect.");

        return;
    }

    if (you.duration[DUR_ICY_ARMOUR])
        mpr( "Your icy armour thickens." );
    else
    {
        if (you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST)
            mpr( "Your icy body feels more resilient." );
        else
            mpr( "A film of ice covers your body!" );

        set_redraw_status( REDRAW_ARMOUR_CLASS );
    }

    you.duration[DUR_ICY_ARMOUR] += 20 + random2(pow) + random2(pow);

    if (you.duration[DUR_ICY_ARMOUR] > 50)
        you.duration[DUR_ICY_ARMOUR] = 50;
}                               // end ice_armour()

void stone_scales(int pow)
{
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
        if (you.attribute[ATTR_TRANSFORMATION] == TRAN_STATUE)
            mpr( "Your stone body feels more resilient." );
        else
            mpr( "A set of stone scales covers your body!" );

        set_redraw_status( REDRAW_EVASION );
        set_redraw_status( REDRAW_ARMOUR_CLASS );
    }

    dur_change = 20 + random2(pow) + random2(pow);

    if (dur_change + you.duration[DUR_STONEMAIL] >= 100)
        you.duration[DUR_STONEMAIL] = 100;
    else
        you.duration[DUR_STONEMAIL] += dur_change;

    burden_change();
}                               // end stone_scales()

void missile_prot(int pow)
{
    mpr("You feel protected from missiles.");

    you.duration[DUR_REPEL_MISSILES] += 8 + roll_dice( 2, pow );

    if (you.duration[DUR_REPEL_MISSILES] > 100)
        you.duration[DUR_REPEL_MISSILES] = 100;
}                               // end missile_prot()

void deflection(int pow)
{
    mpr("You feel very safe from missiles.");

    you.duration[DUR_DEFLECT_MISSILES] += 15 + random2(pow);

    if (you.duration[DUR_DEFLECT_MISSILES] > 100)
        you.duration[DUR_DEFLECT_MISSILES] = 100;
}                               // end cast_deflection()

void cast_regen( int pow )
{
    if (you.hunger <= HUNGER_HUNGRY)
    {
        mpr( "Your system is currently too weak to regenerate." );
        return;
    }

    mpr("Your skin crawls.");

    you.duration[DUR_REGENERATION] += 5 + roll_dice( 2, pow / 3 + 1 );

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

    if (player_in_water())
    {
        if (you.species == SP_MERFOLK)
            mpr("This spell will not benefit you while you're swimming!");
        else
            mpr("This spell will not benefit you while you're in water!");

        return;
    }

    if (!you.duration[DUR_SWIFTNESS] && player_movement_speed() <= 6)
    {
        mpr( "You can't move any more quickly." );
        return;
    }

    // Reduced the duration:  -- bwr
    // dur_incr = random2(power) + random2(power) + 20;
    dur_incr = 20 + random2( power );

    // Centaurs do have feet and shouldn't get here anyways -- bwr
    mpr( "You feel quick%s",  (you.species == SP_NAGA) ? "." : " on your feet." );

    if (dur_incr + you.duration[DUR_SWIFTNESS] > 100)
        you.duration[DUR_SWIFTNESS] = 100;
    else
        you.duration[DUR_SWIFTNESS] += dur_incr;
}                               // end cast_swiftness()

void cast_fly(int power)
{
    int dur_change = 25 + random2(power) + random2(power);

    if (!player_is_levitating())
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

    // duration[DUR_CONTROLLED_FLIGHT] makes the game think player
    // wears an amulet of controlled flight

    burden_change();
}

void cast_insulation(int power)
{
    int dur_incr = 10 + random2(power);

    mpr("You feel insulated.");

    if (dur_incr + you.duration[DUR_INSULATION] > 100)
        you.duration[DUR_INSULATION] = 100;
    else
        you.duration[DUR_INSULATION] += dur_incr;
}                               // end cast_insulation()

void cast_resist_poison(int power)
{
    int dur_incr = 10 + roll_dice(2, power);

    mpr("You feel resistant to poison.");

    if (dur_incr + you.duration[DUR_RESIST_POISON] > 100)
        you.duration[DUR_RESIST_POISON] = 100;
    else
        you.duration[DUR_RESIST_POISON] += dur_incr;
}                               // end cast_resist_poison()

void cast_teleport_control(int power)
{
    int dur_incr = 10 + random2(power);

    mpr("You feel in control.");

    if (dur_incr + you.duration[DUR_CONTROL_TELEPORT] >= 50)
        you.duration[DUR_CONTROL_TELEPORT] = 50;
    else
        you.duration[DUR_CONTROL_TELEPORT] += dur_incr;
}                               // end cast_teleport_control()

void cast_ring_of_flames(int power)
{
    you.fire_shield += 5 + (power / 10) + (random2(power) / 5);

    if (you.fire_shield > 50)
        you.fire_shield = 50;

    mpr("The air around you leaps into flame!");

    manage_fire_shield();
}                               // end cast_ring_of_flames()

void cast_confusing_touch(int power)
{
    mpr( "Your %s begin to glow %s.", your_hand(true),
            (you.confusing_touch ? "brighter" : "red") );

    you.confusing_touch += 5 + (random2(power) / 5);

    if (you.confusing_touch > 30)
        you.confusing_touch = 30;
}                               // end cast_confusing_touch()

bool cast_sure_blade(int power)
{
    bool success = false;

    const int wpn = get_inv_wielded();

    if (wpn == -1)
        mpr("You aren't wielding a weapon!");
    else if (weapon_skill( you.inv[wpn] ) != SK_SHORT_BLADES)
        mpr("You cannot bond with this weapon.");
    else
    {
        if (!you.sure_blade)
        {
            mpr("You become one with your weapon.");
            set_redraw_status( REDRAW_WIELD );
        }
        else if (you.sure_blade < 25)
            mpr("Your bond becomes stronger.");
        else
            mpr("Your bond remains the same.");

        you.sure_blade += 8 + (random2(power) / 5);

        if (you.sure_blade > 25)
            you.sure_blade = 25;

        success = true;
    }

    return (success);
}                               // end cast_sure_blade()

void manage_fire_shield(void)
{
    you.fire_shield--;

    if (!you.fire_shield)
        return;

    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_ICE_BEAST)
    {
        int res = player_res_fire();

        int dam = (roll_dice(3,10) + 7) * you.time_taken;

        if (res < 0)
            dam += (roll_dice(2,10) + 1) * you.time_taken;

        dam = (dam + 5) / 10;

        if (dam > 0)
        {
            mpr( MSGCH_WARN, "You're melting!" );
            ouch( dam, 0, KILLED_BY_MELTING );
        }
    }

    int stx = 0, sty = 0;

    for (stx = -1; stx < 2; stx++)
    {
        for (sty = -1; sty < 2; sty++)
        {
            if (sty == 0 && stx == 0)
                continue;

            const int x = you.x_pos + stx;
            const int y = you.y_pos + sty;

            if (!grid_is_solid( grd[x][y] ) && !is_cloud( x, y ))
                place_cloud( CLOUD_FIRE, x, y, 1 + random2(6) );
        }
    }
}                               // end manage_fire_shield()

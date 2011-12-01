/*
 *  File:       spells2.cc
 *  Summary:    Implementations of some additional spells.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *     <4>    03jan1999    jmf     Changed summon_small_mammals so at
 *                                 higher levels it indeed summons in plural.
 *                                 Removed some IMHO unnecessary failure msgs.
 *                                 (from e.g. animate_dead).
 *                                 Added protection by special deities.
 *     <3>     10/11/99    BCR     fixed range bug in burn_freeze,
 *                                 vamp_drain, and summon_elemental
 *     <2>     5/26/99     JDJ     detect_items uses '~' instead of '*'.
 *     <1>     -/--/--     LRH     Created
 */

#include "AppHdr.h"
#include "spells2.h"

#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "externs.h"

#include "beam.h"
#include "cloud.h"
#include "direct.h"
#include "effects.h"
#include "itemname.h"
#include "itemprop.h"
#include "items.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mon-util.h"
#include "ouch.h"
#include "player.h"
#include "randart.h"
#include "spells4.h"
#include "spl-cast.h"
#include "spl-util.h"
#include "stuff.h"
#include "view.h"

int raise_corpse( int corps, int corx, int cory, beh_type corps_beh,
                  int corps_hit, int actual );

int detect_traps( int pow )
{
    // it's bad to detect in areas with non-standard mapping
    if (you.level_type == LEVEL_LABYRINTH || you.level_type == LEVEL_ABYSS)
        return (0);

    int traps_found = 0;

    if (pow > 50)
        pow = 50;

    const int range = 8 + random2(8) + pow;

    for (int i = 0; i < MAX_TRAPS; i++)
    {
        if (env.trap[i].type == TRAP_UNASSIGNED)
            continue;

        const int etx = env.trap[i].x;
        const int ety = env.trap[i].y;

        // Used to just be visible screen:
        // if (etx > you.x_pos - 15 && etx < you.x_pos + 15
        //     && ety > you.y_pos - 8 && ety < you.y_pos + 8)

        if (grid_distance( you.x_pos, you.y_pos, etx, ety ) < range)
        {
            if (grd[etx][ety] == DNGN_UNDISCOVERED_TRAP)
            {
                traps_found++;

                grd[etx][ety] = trap_category( env.trap[i].type );
                set_envmap_char( etx, ety, get_magicmap_char( grd[etx][ety] ) );
                set_terrain_mapped( etx, ety );
            }
        }
    }

    return (traps_found);
}                               // end detect_traps()

int detect_items( int pow )
{
    // it's bad to detect in areas with non-standard mapping
    if (you.level_type == LEVEL_LABYRINTH || you.level_type == LEVEL_ABYSS)
        return (0);

    if (pow > 50)
        pow = 50;

    const int map_radius = 8 + random2(8) + pow;
    int items_found = 0;

    for (int i = you.x_pos - map_radius; i < you.x_pos + map_radius; i++)
    {
        for (int j = you.y_pos - map_radius; j < you.y_pos + map_radius; j++)
        {
            if (!in_bounds( i, j ))
                continue;

            if (igrd[i][j] != NON_ITEM)
                set_envmap_char( i, j, get_magicmap_char(DNGN_ITEM_DETECTED) );
        }
    }

    return (items_found);
}                               // end detect_items()

int detect_creatures( int pow )
{
    // it's bad to detect in areas with non-standard mapping
    if (you.level_type == LEVEL_LABYRINTH || you.level_type == LEVEL_ABYSS)
        return (0);

    if (pow > 50)
        pow = 50;

    unsigned char creatures_found = 0;
    const int     map_radius = 8 + random2(8) + pow;

    for (int i = you.x_pos - map_radius; i < you.x_pos + map_radius; i++)
    {
        for (int j = you.y_pos - map_radius; j < you.y_pos + map_radius; j++)
        {
            if (!in_bounds( i, j ))
                continue;

            if (mgrd[i][j] != NON_MONSTER)
            {
                struct monsters *mon = &menv[ mgrd[i][j] ];

                if (mon->type < 0)
                    continue;

                if (!player_monster_visible( mon ))
                    continue;

                set_envmap_char( i, j, mons_char( mon->type ) );

                if (mons_intel( mon->type ) == I_HIGH
                    && mons_class_flag( mon->type, M_SPELLCASTER ))
                {
                    behaviour_event( mon, ME_DISTURB, MHITYOU,
                                     you.x_pos, you.y_pos );
                }
            }
        }
    }

    return (creatures_found);
}                               // end detect_creatures()

int corpse_rot(int power)
{
    UNUSED( power );

    char adx = 0;
    char ady = 0;

    char minx = you.x_pos - 6;
    char maxx = you.x_pos + 7;
    char miny = you.y_pos - 6;
    char maxy = you.y_pos + 7;
    char xinc = 1;
    char yinc = 1;

    if (coinflip())
    {
        minx = you.x_pos + 6;
        maxx = you.x_pos - 7;
        xinc = -1;
    }

    if (coinflip())
    {
        miny = you.y_pos + 6;
        maxy = you.y_pos - 7;
        yinc = -1;
    }

    for (adx = minx; adx != maxx; adx += xinc)
    {
        if (adx == 7 || adx == -7)
            return 0;

        for (ady = miny; ady != maxy; ady += yinc)
        {
            if (see_grid(adx, ady))
            {
                if (igrd[adx][ady] == NON_ITEM || is_cloud( adx, ady ))
                    continue;

                int objl = igrd[adx][ady];
                int hrg = 0;

                while (objl != NON_ITEM)
                {
                    if (mitm[objl].base_type == OBJ_CORPSES
                        && mitm[objl].sub_type == CORPSE_BODY)
                    {
                        if (!mons_skeleton(mitm[objl].plus))
                            destroy_item(objl);
                        else
                        {
                            mitm[objl].sub_type = CORPSE_SKELETON;
                            mitm[objl].special = 200;
                            mitm[objl].colour = LIGHTGREY;
                        }

                        place_cloud( CLOUD_MIASMA, adx, ady,
                                     11 + roll_dice(3,6) );

                        goto out_of_raise;
                    }

                    hrg = mitm[objl].link;
                    objl = hrg;
                }

              out_of_raise:
                objl = 1;
            }
        }
    }

    if (player_can_smell())
        mpr("You smell decay.");

    // should make zombies decay into skeletons

    return 0;
}                               // end corpse_rot()

// attempt to raise the top corpse of each stack visible from (cx, cy)
int animate_dead( int power, int cx, int cy, beh_type corps_beh, int corps_hit,
                  int actual )
{
    int adx = 0;
    int ady = 0;

    int minx = cx - 6;
    int maxx = cx + 6;
    int miny = cy - 6;
    int maxy = cy + 6;
    int xinc = 1;
    int yinc = 1;

    const int range = power / 10;

    int number_raised = 0;

    // randomly reverse x scan
    if (coinflip())
    {
        minx = cx + 6;
        maxx = cx - 6;
        xinc = -1;
    }

    // randomly reverse y scan
    if (coinflip())
    {
        miny = cy + 6;
        maxy = cy - 6;
        yinc = -1;
    }

    for (adx = minx; adx != maxx; adx += xinc)
    {
        for (ady = miny; ady != maxy; ady += yinc)
        {
            if (!in_bounds( adx, ady ))
                continue;

            if (grid_distance( cx, cy, adx, ady ) <= range
                && check_line_of_sight( cx, cy, adx, ady ))
            {
                // Note: Since we break after the first we don't have to worry
                // about storing the link (for when the object goes away).
                for (int o = igrd[adx][ady]; o != NON_ITEM; o = mitm[o].link)
                {
                    ASSERT( is_valid_item( mitm[o] ) );

                    if (mitm[o].base_type == OBJ_CORPSES)
                    {
                        number_raised += raise_corpse( o, adx, ady, corps_beh,
                                                       corps_hit, actual );
                        break;
                    }
                }
            }
        }
    }

    if (actual == 0)
        return (number_raised);

    if (number_raised > 0)
        mpr("The dead are walking!");

    return (number_raised);
}                               // end animate_dead()


bool animate_a_corpse( int axps,  int ayps, beh_type corps_beh, int corps_hit,
                       int anim_type )
{
    bool ret = false;

    for (int obj = igrd[axps][ayps]; obj != NON_ITEM; obj = mitm[obj].link)
    {
        if (mitm[obj].base_type != OBJ_CORPSES)
            continue;

        if (anim_type == CORPSE_SKELETON
             && mitm[obj].sub_type != CORPSE_SKELETON)
        {
            continue;
        }

        if (raise_corpse( obj, axps, ayps, corps_beh, corps_hit, 1 ) > 0)
        {
            mpr("The dead are walking!");
            ret = true;
            break;
        }
    }

    return (ret);
}                               // end animate_a_corpse()


int raise_corpse( int corps, int corx, int cory, beh_type corps_beh,
                  int corps_hit, int actual )
{
    int returnVal = 0;

    if (mons_zombie_size(mitm[corps].plus) && actual != 0)
    {
        int type;
        if (mitm[corps].sub_type == CORPSE_BODY)
        {
            if (mons_zombie_size(mitm[corps].plus) == Z_SMALL)
                type = MONS_ZOMBIE_SMALL;
            else
                type = MONS_ZOMBIE_LARGE;
        }
        else
        {
            if (mons_zombie_size(mitm[corps].plus) == Z_SMALL)
                type = MONS_SKELETON_SMALL;
            else
                type = MONS_SKELETON_LARGE;
        }

        if (create_monster( type, corps_beh, 0, corx, cory, corps_hit,
                            mitm[corps].plus, true ) != -1)
        {
            returnVal = 1;
            destroy_item(corps);
        }
    }

    return (returnVal);
}                               // end raise_corpse()

void cast_twisted( int power, beh_type corps_beh, int corps_hit )
{
    int total_mass = 0;
    int num_corpses = 0;
    int type_resurr = MONS_ABOMINATION_SMALL;
    char colour;

    unsigned char rotted = 0;

    if (igrd[you.x_pos][you.y_pos] == NON_ITEM)
    {
        mpr("There's nothing here!");
        return;
    }

    int objl = igrd[you.x_pos][you.y_pos];
    int next;

    while (objl != NON_ITEM)
    {
        next = mitm[objl].link;

        if (mitm[objl].base_type == OBJ_CORPSES
                && mitm[objl].sub_type == CORPSE_BODY)
        {
            total_mass += random2( mons_weight( mitm[objl].plus ) );

            num_corpses++;
            if (mitm[objl].special < 100)
                rotted++;

            destroy_item( objl );
        }

        objl = next;
    }

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Mass for abomination: %d", total_mass );
#endif

    // This is what the old statement pretty much boils down to,
    // the average will be approximately 10 * power (or about 1000
    // at the practical maximum).  That's the same as the mass
    // of a hippogriff, a spiny frog, or a steam dragon.  Thus,
    // material components are far more important to this spell. -- bwr
    total_mass += roll_dice( 20, power );

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Mass including power bonus: %d", total_mass);
#endif

    if (total_mass < 400 + roll_dice( 2, 500 )
        || num_corpses < (coinflip() ? 3 : 2))
    {
        mpr("The spell fails.");
        mpr("The corpses collapse into a pulpy mess.");
        return;
    }

    if (total_mass > 500 + roll_dice( 3, 1000 )
        && num_corpses > (coinflip() ? 2 : 3))
    {
        type_resurr = MONS_ABOMINATION_LARGE;
    }

    if (rotted == num_corpses)
        colour = BROWN;
    else if (rotted >= random2( num_corpses ))
        colour = RED;
    else
        colour = LIGHTRED;

    int mon = create_monster( type_resurr, corps_beh, 0, you.x_pos, you.y_pos,
                              corps_hit, colour );

    if (mon == -1)
        mpr("The corpses collapse into a pulpy mess.");
    else
    {
        mpr("The heap of corpses melds into an agglomeration of writhing flesh!");
        if (type_resurr == MONS_ABOMINATION_LARGE)
        {
            const int div = (colour == LIGHTRED) ? 750 :
                            (colour == RED)      ? 1500
                                                 : 2500;

            menv[mon].hit_dice = 8 + div_rand_round( total_mass, div );

            if (menv[mon].hit_dice > 20)
                menv[mon].hit_dice = 20;

            // XXX: No convenient way to get the hit dice size right now.
            menv[mon].hit_points = hit_points( menv[mon].hit_dice, 2, 5 );
            menv[mon].max_hit_points = menv[mon].hit_points;

            if (colour == LIGHTRED)
                menv[mon].armour_class += total_mass / 1000;
        }
    }
}                               // end cast_twisted()

bool brand_weapon( int which_brand, int power )
{
    int temp_rand;              // probability determination {dlb}
    int duration_affected = 0;  //jmf: NB: now HOW LONG, not WHICH BRAND.

    const int wpn = get_inv_wielded();
    const bool extend = (you.duration[DUR_WEAPON_BRAND] > 0);

    if (wpn == -1)
        return (false);

    if (you.inv[wpn].base_type != OBJ_WEAPONS)
        return (false);

    // XXX; bows should have their own versions of these spells?
    const bool bow   = is_range_weapon( you.inv[wpn] );
    const int  brand = get_weapon_brand( you.inv[wpn] );

    if (is_fixed_artefact( you.inv[wpn] )
        || is_random_artefact( you.inv[wpn] )
        || (extend && brand != which_brand)
        || (!extend && brand != SPWPN_NORMAL ))
    {
        return (false);
    }

    char str_pass[ ITEMNAME_SIZE ];
    in_name( wpn, DESC_CAP_YOUR, str_pass );
    strcpy( info, str_pass );

    switch (which_brand)        // use SPECIAL_WEAPONS here?
    {
    case SPWPN_FLAMING:
        if (extend)
            strcat( info, (bow) ? " glows brighter!" : " burns brighter!" );
        else
            strcat( info, (bow) ? " glows red!" : " bursts into flame!" );

        duration_affected = 7;
        break;

    case SPWPN_FREEZING:
        if (extend)
            strcat( info, " glows brighter." );
        else
            strcat( info, " glows blue." );

        duration_affected = 7;
        break;

    case SPWPN_VENOM:
        // weapon must cut in some way to work (slice, piece, whip slash)
        if (!does_damage_type(you.inv[wpn], DAM_SLICE | DAM_PIERCE | DAM_WHIP))
            return (false);

        if (extend)
            strcat( info, " drips poison." );
        else
            strcat( info, " starts dripping with poison." );

        duration_affected = 15;
        break;

    case SPWPN_DRAINING:
        if (bow)
            return (false);

        if (extend)
            strcat( info, " crackles loudly." );
        else
            strcat( info, " crackles with unholy energy." );

        duration_affected = 12;
        break;

    case SPWPN_VORPAL:  // Tukima's Vorpal Blade
        // must be able a vorpal slicing weapon (ie swords and scythes)
        if (get_vorpal_type( you.inv[wpn] ) != DVORP_SLICING)
            return (false);

        if (extend)
            strcat( info, " glows silver." );
        else
            strcat( info, " glows silver and looks extremely sharp." );

        duration_affected = 10;
        break;

    case SPWPN_DISTORTION:      //jmf: added for Warp Weapon
        if (bow)
            return  (false);

        strcat(info, " seems to ");

        temp_rand = random2(6);
        strcat(info, (temp_rand == 0) ? "twist" :
                     (temp_rand == 1) ? "bend" :
                     (temp_rand == 2) ? "vibrate" :
                     (temp_rand == 3) ? "flex" :
                     (temp_rand == 4) ? "wobble"
                                      : "twang");

        strcat( info, coinflip() ? " oddly." : " strangely." );
        duration_affected = 5;

        // This brand is insanely powerful, this isn't even really
        // a start to balancing it, but it needs something. -- bwr
        miscast_effect(SPTYP_TRANSLOCATION, 9, 90, 100, "a distortion effect");
        break;

    case SPWPN_DUMMY_CRUSHING:  //jmf: added for Maxwell's Silver Hammer
        // must be a bludgeoning weapon
        if (!does_damage_type( you.inv[wpn], DAM_BLUDGEON ))
            return (false);

        which_brand = SPWPN_VORPAL;

        if (extend)
            strcat( info, " glows silver." );
        else
            strcat( info, " glows silver and feels heavier." );

        duration_affected = 7;
        break;
    }

    set_item_ego_type( you.inv[wpn], OBJ_WEAPONS, which_brand );

    mpr(info);
    set_redraw_status( REDRAW_WIELD );

    int dur_change = duration_affected + roll_dice(2, power);

    you.duration[DUR_WEAPON_BRAND] += dur_change;

    if (you.duration[DUR_WEAPON_BRAND] > 100)
        you.duration[DUR_WEAPON_BRAND] = 100;

    return (true);
}                               // end brand_weapon()

void turn_undead( int pow )
{
    struct monsters *monster;

    mpr("You attempt to repel the undead.");

    for (int tu = 0; tu < MAX_MONSTERS; tu++)
    {
        monster = &menv[tu];

        if (monster->type == -1 || !mons_near(monster))
            continue;

        // used to inflict random2(5) + (random2(pow) / 20) damage,
        // in addition {dlb}
        if (mons_holiness( monster ) == MH_UNDEAD)
        {
            if (check_mons_resist_magic( monster, pow ))
            {
                mon_msg( monster, " resists." );
                continue;
            }

            if (mons_add_ench( monster, ENCH_FEAR, MHITYOU ))
                mon_msg( monster, " is repelled!" );
        }                       // end "if mons_holiness"
    }                           // end "for tu"
}                               // end turn_undead()

void holy_word( int pow, bool silent )
{
    if (!silent)
        mpr("You speak a Word of immense power!");

    // doubt this will ever happen, but it's here as a safety -- bwr
    if (pow > 300)
        pow = 300;

    for (int tu = 0; tu < MAX_MONSTERS; tu++)
    {
        struct monsters *const monster = &menv[tu];

        if (monster->type == -1 || !mons_near( monster ))
            continue;

        const bool unholy = mons_is_unholy( monster );
        const bool evil = mons_is_evil( monster );

        if (unholy || evil)
        {
            int dam = roll_dice( 3, 10 + pow / 6 );

            if (unholy) // probably also evil
            {
                mon_msg( monster, " convulses!" );

                mons_add_ench( monster, ENCH_STUN, MHITYOU );
                mons_add_ench( monster, ENCH_FEAR, MHITYOU );
            }
            else // just evil
            {
                mon_msg( monster, " partially resists!" );
                dam /= 2;
            }

            if (you_hurt_monster( monster, dam ))
                continue;
        }
        else if (mons_holiness( monster ) == MH_HOLY
                || (testbits( monster->flags, MF_GOD_GIFT ) && !unholy && !evil))
        {
            if (heal_monster( monster, 5 + roll_dice( 2, 2 + pow / 20 ), false ))
                mon_msg( monster, " looks healthier." );
        }
    }

    if (you.hp < you.hp_max)
    {
        mpr("You feel better.");
        inc_hp( 5 + roll_dice( 2, 2 + pow / 20 ), false );
    }
}                               // end holy_word()

// poisonous light passes right through invisible players
// and monsters, and so, they are unaffected by this spell --
// assumes only you can cast this spell (or would want to)
void cast_toxic_radiance(void)
{
    struct monsters *monster;

    mpr("You radiate a sickly green light!");

    you.flash_colour = GREEN;
    viewwindow(true, false);
    more();
    mesclr();

    // determine whether the player is hit by the radiance: {dlb}
    if (you.invis)
    {
        mpr("The light passes straight through your body.");
    }
    else if (player_res_poison() < 2)
    {
        mpr("You feel rather sick.");
        poison_player(2);
    }

    // determine which monsters are hit by the radiance: {dlb}
    for (int toxy = 0; toxy < MAX_MONSTERS; toxy++)
    {
        monster = &menv[toxy];

        if (monster->type != -1 && mons_near(monster))
        {
            if (!mons_is_invisible( monster ))
            {
                poison_monster(monster, true);

                if (coinflip()) // 50-50 chance for a "double hit" {dlb}
                    poison_monster(monster, true);

            }
            else if (player_see_invis())
            {
                mon_msg( monster, "The light passes through %s." );
            }
        }
    }
}                               // end cast_toxic_radiance()

void cast_refrigeration(int pow)
{
    int hurted = 0;
    struct bolt beam;
    int toxy;

    beam.flavour = BEAM_COLD;

    const dice_def  dam_dice( 3, 5 + pow / 10 );

    mpr("The heat is drained from your surroundings.");

    you.flash_colour = LIGHTCYAN;
    viewwindow(true, false);
    more();
    mesclr();

    // Do the player:
    hurted = roll_dice( dam_dice );
    hurted = check_your_resists( hurted, static_cast<beam_type>(beam.flavour) );

    if (hurted > 0)
    {
        mpr("You feel very cold.");
        ouch( hurted, 0, KILLED_BY_FREEZING );

        // Note: this used to be 12!... and it was also applied even if
        // the player didn't take damage from the cold, so we're being
        // a lot nicer now.  -- bwr
        expose_player_to_element( BEAM_COLD, 5 );
    }

    // Now do the monsters:
    for (toxy = 0; toxy < MAX_MONSTERS; toxy++)
    {
        struct monsters *const monster = &menv[toxy];

        if (monster->type == -1)
            continue;

        if (mons_near( monster ))
        {
            mon_msg( monster, "You freeze %s." );

            hurted = roll_dice( dam_dice );
            hurted = mons_adjust_flavoured( monster, beam, hurted );

            if (!you_hurt_monster( monster, hurted ))
            {
                //jmf: "slow snakes" finally available
                if (mons_class_flag(monster->type, M_COLD_BLOOD) && coinflip())
                    mons_add_ench( monster, ENCH_SLOW );
            }
        }
    }
}                               // end cast_refrigeration()

// This is a Yred ability only.
void drain_life( int pow )
{
    int hp_gain = 0;
    int hurted = 0;
    int num = 0;

    mpr( "You draw life from your surroundings." );

    // Incoming power to this function is skill in INVOCATIONS, so
    // we'll add an assert here to warn anyone who tries to use
    // this function with spell level power.
    ASSERT( pow <= 27 );

    you.flash_colour = DARKGREY;
    viewwindow(true, false);
    more();
    mesclr();

    for (int toxy = 0; toxy < MAX_MONSTERS; toxy++)
    {
        struct monsters *const monster = &menv[toxy];

        if (monster->type == -1)
            continue;

        if (mons_res_negative_energy( monster ))
            continue;

        if (mons_near(monster))
        {
            mon_msg( monster, "You draw life from %s.", false );

            hurted = 3 + random2(7) + random2( pow / 4 );
            hp_gain += hurted;
            num++;

            you_hurt_monster( monster, hurted );
        }
    }

    if (num == 0 && !you.is_undead)
    {
        mpr( "Aaaarggghhhhh!" );
        dec_hp( 10 + roll_dice(2,20), false );
        return;
    }

    if (num > 1)
        hp_gain = (hp_gain + 1) / 2;

    if (hp_gain > pow * 2)
        hp_gain = pow * 2;

    if (hp_gain)
    {
        mpr( "You feel life flooding into your body." );
        inc_hp( hp_gain, false );
    }
}                               // end drain_life()

int vampiric_drain( int pow, struct bolt &beam )
{
    int inflicted = 0;

    const int mgr = mgrd[beam.target_x][beam.target_y];

    if (mgr == NON_MONSTER)
    {
        mpr("There isn't anything there!");
        return (SPRET_FAIL); // could be aiming at invis mon, so not ABORT
    }

    struct monsters *const monster = &menv[mgr];

    const int holy = mons_holiness( monster );

    if (!you.is_undead
        && (holy == MH_UNDEAD || holy == MH_DEMONIC))
    {
        mpr("Aaaarggghhhhh!");
        dec_hp( 10 + roll_dice(2,20), false );
        return (SPRET_FAIL);
    }

    if (mons_res_negative_energy( monster ))
    {
        canned_msg( MSG_NOTHING_HAPPENS );
        return (SPRET_FAIL);
    }

    // The practical maximum of this is about 25 (pow @ 100).  -- bwr
    inflicted = 1 + roll_dice(2,5) + random2(pow) / 7;

    if (inflicted >= monster->hit_points)
        inflicted = monster->hit_points;

    if (inflicted >= you.hp_max - you.hp)
        inflicted = you.hp_max - you.hp;

    if (inflicted == 0)
    {
        canned_msg( MSG_NOTHING_HAPPENS );
        return (SPRET_FAIL);
    }

    you_hurt_monster( monster, inflicted );

    inflicted /= 2;
    if (inflicted)
    {
        mon_msg( monster, "You feel life coursing from %s into your body!", false );
        inc_hp( inflicted, false );
    }

    return (SPRET_SUCCESS);
}                               // end vampiric_drain()

// Note: this function is currently only used for Freeze. -- bwr
int burn_freeze( int pow, struct bolt &beam, int flavour )
{
    if (pow > 25)
        pow = 25;

    const int mgr = mgrd[beam.target_x][beam.target_y];

    if (mgr == NON_MONSTER)
    {
        mpr("There isn't anything there!");
        return (SPRET_FAIL);
    }

    struct monsters *const monster = &menv[mgr];

    int tmp;
    int colour = LIGHTMAGENTA;
    const char *desc = "______";

    switch (flavour)
    {
    case BEAM_FIRE:
        tmp = random2(3);
        colour = element_colour( EC_FIRE );
        desc = "burn";
        break;

    case BEAM_COLD:
        tmp = random2(3);
        colour = element_colour( EC_ICE );
        desc = "freeze";
        break;

    case BEAM_MISSILE:
        colour = element_colour( EC_EARTH );;
        desc = "crush";
        break;

    case BEAM_ELECTRICITY:
        colour = element_colour( EC_ELECTRICITY );
        desc = "zap";
        break;

    default:
        break;
    }

    zap_animation( colour, monster, true );

    mpr( "You %s %s.", desc, ptr_monam( monster, DESC_NOCAP_THE ) );

    int hurted = roll_dice( 1, 3 + pow / 3 );

    beam.flavour = flavour;

    if (flavour != BEAM_MISSILE)
        hurted = mons_adjust_flavoured( monster, beam, hurted );

    if (!you_hurt_monster( monster, hurted ))
    {
        if (flavour == BEAM_COLD)
        {
            if (mons_class_flag( monster->type, M_COLD_BLOOD ) && coinflip())
                mons_add_ench( monster, ENCH_SLOW );

            const int cold_res = mons_res_cold( monster );

            if (cold_res <= 0)
                monster->energy += cold_res * random2( 2 + pow / 5 );
        }
    }

    return (SPRET_SUCCESS);
}                               // end burn_freeze()

// 'unfriendly' is percentage chance summoned elemental goes
//              postal on the caster (after taking into account
//              chance of that happening to unskilled casters
//              anyway)
int summon_elemental( int pow, int force_type, int unfriendly, bool charm )
{
    int type_summoned = MONS_PROGRAM_BUG;       // error trapping {dlb}
    struct dist smove;
    int mon;

    int dir_x;
    int dir_y;
    int targ_x;
    int targ_y;

    int numsc = 2 + (random2(pow) / 5);

    if (numsc > 6)
        numsc = 6;

    mpr( MSGCH_PROMPT, "Summon from material in which direction?" );

    direction( smove, DIR_DIR );

    if (!smove.isValid)
    {
        canned_msg( MSG_NOTHING_HAPPENS );
        return (-2);
    }

    dir_x  = smove.dx;
    dir_y  = smove.dy;
    targ_x = you.x_pos + dir_x;
    targ_y = you.y_pos + dir_y;

    if (!in_bounds( targ_x, targ_y )
        || mgrd[targ_x][targ_y] != NON_MONSTER
        || (dir_x == 0 && dir_y == 0))
    {
        canned_msg( MSG_NOTHING_HAPPENS );
        return (-1);
    }

    const int grid  = grd[targ_x][targ_y];
    const int cid   = env.cgrid[targ_x][targ_y];
    const int ctype = get_cloud_type( targ_x, targ_y );

    if (grid == DNGN_ROCK_WALL
            && (!force_type || force_type == MONS_EARTH_ELEMENTAL))
    {
        type_summoned = MONS_EARTH_ELEMENTAL;

        if (in_bounds( targ_x, targ_y ))
            grd[ targ_x ][ targ_y ] = DNGN_FLOOR;
    }
    else if ((ctype == CLOUD_FIRE || ctype == CLOUD_FIRE_MON)
            && (!force_type || force_type == MONS_FIRE_ELEMENTAL))
    {
        type_summoned = MONS_FIRE_ELEMENTAL;
        delete_cloud( cid );
    }
    else if (grid == DNGN_LAVA
            && (!force_type || force_type == MONS_FIRE_ELEMENTAL))
    {
        type_summoned = MONS_FIRE_ELEMENTAL;
    }
    else if ((grid == DNGN_DEEP_WATER
                || grid == DNGN_SHALLOW_WATER
                || grid == DNGN_BLUE_FOUNTAIN)
            && (!force_type || force_type == MONS_WATER_ELEMENTAL))
    {
        type_summoned = MONS_WATER_ELEMENTAL;
    }
    else if ((grid >= DNGN_FLOOR && ctype == CLOUD_NONE)
            && (!force_type || force_type == MONS_AIR_ELEMENTAL))
    {
        type_summoned = MONS_AIR_ELEMENTAL;
    }

    // found something to summon
    if (type_summoned == MONS_PROGRAM_BUG)
    {
        canned_msg( MSG_NOTHING_HAPPENS );
        return (-1);
    }

    // silly - ice for water? 15jan2000 {dlb}
    // little change here to help with the above... and differentiate
    // elements a bit... {bwr}
    // - Water elementals are now harder to be made reliably friendly
    // - Air elementals are harder because they're more dynamic/dangerous
    // - Earth elementals are more static and easy to tame (as before)
    // - Fire elementals fall in between the two (10 is still fairly easy)
    if ((type_summoned == MONS_FIRE_ELEMENTAL
            && random2(10) >= you.skills[SK_FIRE_MAGIC])

        || (type_summoned == MONS_WATER_ELEMENTAL
            && random2((you.species == SP_MERFOLK) ? 5 : 15)
                           >= you.skills[SK_ICE_MAGIC])

        || (type_summoned == MONS_AIR_ELEMENTAL
            && random2(15) >= you.skills[SK_AIR_MAGIC])

        || (type_summoned == MONS_EARTH_ELEMENTAL
            && random2(5)  >= you.skills[SK_EARTH_MAGIC])

        || random2(100) < unfriendly)
    {
        mon = create_monster( type_summoned, (charm) ? BEH_CHARMED : BEH_HOSTILE,
                              numsc, targ_x, targ_y, MHITYOU, 250 );

        if (mon != -1)
            mpr( "The elemental doesn't seem to appreciate being summoned." );
    }
    else
    {
        mon = create_monster( type_summoned, BEH_FRIENDLY, numsc,
                              targ_x, targ_y, you.pet_target, 250 );
    }

    return (mon);
}                               // end summon_elemental()

//jmf: beefed up higher-level casting of this (formerly lame) spell
void summon_small_mammals( int pow )
{
    int thing_called = MONS_PROGRAM_BUG;        // error trapping{dlb}

    int pow_spent = 0;
    int pow_left = pow + 1;
    int summoned = 0;
    int summoned_max = pow / 16;

    if (summoned_max > 5)
        summoned_max = 5;
    if (summoned_max < 1)
        summoned_max = 1;

    while (pow_left > 0 && summoned < summoned_max)
    {
        summoned++;
        pow_spent = 1 + random2( pow_left );
        pow_left -= pow_spent;

        switch (pow_spent)
        {
        case 75:
        case 74:
        case 38:
            thing_called = MONS_ORANGE_RAT;
            break;

        case 65:
        case 64:
        case 63:
        case 27:
        case 26:
        case 25:
            thing_called = MONS_GREEN_RAT;
            break;

        case 57:
        case 56:
        case 55:
        case 54:
        case 53:
        case 52:
        case 20:
        case 18:
        case 16:
        case 14:
        case 12:
        case 10:
            thing_called = coinflip() ? MONS_QUOKKA : MONS_GREY_RAT;
            break;

        default:
            thing_called = coinflip() ? MONS_GIANT_BAT : MONS_RAT;
            break;
        }

        create_monster( thing_called, BEH_FRIENDLY, 3 );
    }
}                               // end summon_small_mammals()

void summon_scorpions(int pow)
{
    int numsc = 1 + random2(pow) / 10 + random2(pow) / 10;

    numsc = stepdown_value(numsc, 2, 2, 6, 8);  //see stuff.cc - 12jan2000 {dlb}

    for (int scount = 0; scount < numsc; scount++)
    {
        if (random2(pow) <= 3)
        {
            if (create_monster( MONS_SCORPION, BEH_HOSTILE, 3 ) != -1)
                mpr("A scorpion appears. It doesn't look very happy.");
        }
        else
        {
            if (create_monster( MONS_SCORPION, BEH_FRIENDLY, 3 ) != -1)
                mpr("A scorpion appears.");
        }
    }
}                               // end summon_scorpions()

void call_imp( int pow )
{
    // imps from SK_CONJURATION to SK_POISON
    static const monster_type imp_list[] =
    {
        MONS_DART_IMP, MONS_INVIS_IMP, MONS_IMP, MONS_SHADOW_IMP,
        MONS_BLINK_IMP, MONS_MORPH_IMP, MONS_BRAIN_IMP,
        MONS_FLAME_IMP, MONS_WHITE_IMP, MONS_SWIFT_IMP, MONS_STONE_IMP,
        MONS_POISON_IMP,
    };
    const int num_imps = sizeof(imp_list)  / sizeof(int);

    ASSERT( SK_POISON_MAGIC - SK_CONJURATIONS + 1 == num_imps );

    monster_type  mc = MONS_IMP;
    int           total_skills = 0;

    for (int i = 0; i < num_imps; i++)
    {
        // XXX: used to have divinations at skill / 2... how important
        // is that really?
        const int skill = you.skills[ SK_CONJURATIONS + i ];

        total_skills += skill;
        if (random2( total_skills ) < skill)
            mc = imp_list[i];
    }

    summon_ice_beast_etc( pow, mc );

    if (mc == MONS_IMP && random2(pow) > 10)
        summon_ice_beast_etc( pow, mc );
}

void summon_ice_beast_etc( int pow, int type, beh_type behav )
{
    int dur = 2 + (random2(pow) / 4);

    if (dur > 6)
        dur = 6;

    switch (type)
    {
    case MONS_ICE_BEAST:
        mpr("A chill wind blows around you.");
        break;

    case MONS_IMP:
        mpr("A beastly little devil appears in a puff of smoke.");
        break;

    case MONS_WHITE_IMP:
        mpr("A beastly little devil appears in a puff of frigid air.");
        break;

    case MONS_POISON_IMP:
        mpr("A beastly little green devil appears in a puff of noxious fumes.");
        break;

    case MONS_FLAME_IMP:
        mpr("A beastly little devil appears in a puff of flame.");
        break;

    case MONS_BLINK_IMP:
        mpr("A beastly little devil blinks into existance.");
        break;

    case MONS_BRAIN_IMP:
        mpr("A beastly little devil with a large brain appears.");
        break;

    case MONS_SWIFT_IMP:
        mpr("A beastly little devil starts streaking around the room.");
        break;

    case MONS_STONE_IMP:
        mpr("A beastly little devil rises out of the ground.");
        break;

    case MONS_INVIS_IMP:
        mpr("A beastly little devil fades into existance.");
        break;

    case MONS_DART_IMP:
        mpr("A beastly little devil appears with a bang and a puff of purple smoke.");
        break;

    case MONS_SWARM_IMP:
        mpr("A beastly little devil appears and starts to screech for help.");
        break;

    case MONS_MORPH_IMP:
        mpr("A small devilish figure appears with a giggle, and grows to larger size.");
        break;


    case MONS_SHADOW_IMP:
        mpr("A shadowy apparition takes form in the air.");
        break;

    case MONS_ANGEL:
        mpr("You open a gate to the realm of Zin!");
        break;

    case MONS_DAEVA:
        mpr("You are momentarily dazzled by a brilliant golden light.");
        break;

    default:
        mpr("A demon appears!");
        if (random2(pow) < 4)
        {
            behav = BEH_HOSTILE;
            mpr("It doesn't look very happy.");
        }
        break;
    }


    create_monster( type, behav, dur );
}                               // end summon_ice_beast_etc()

// this is the demonspawn mutation ability (level is level of mutation/effect)
void summon_demons( int pow, bool group )
{
    summon_ice_beast_etc( pow, rand_demon( (group) ? DEMON_COMMON
                                                   : DEMON_LESSER ) );
    if (group)
    {
        const int num = roll_dice(1,3);
        for (int i = 0; i < num; i++)
        {
            summon_ice_beast_etc( pow, rand_demon( DEMON_LESSER ) );
        }
    }
}

struct insect_def
{
    monster_type    mon;
    int             rare;
    int             cost;
};

bool summon_swarm( int pow, bool unfriendly, bool god_gift )
{
    bool summoned = false;
    int tx = you.x_pos, ty = you.y_pos;
    int target = MHITYOU;

    int numsc = 2 + roll_dice( 2, 1 + pow / 20 );
    if (numsc > 12)
        numsc = 12;

    if (god_gift)
    {
        struct dist  spd;
        struct bolt  beam;

        mpr( MSGCH_PROMPT, "Where do you wish to summon creeping doom?" );

        if (!spell_direction( spd, beam, DIR_GRID, TARG_ENEMY ))
            return (false);

        tx = beam.target_x;
        ty = beam.target_y;

        if (mgrd[tx][ty] != NON_MONSTER)
            target = mgrd[tx][ty];
    }

    // XXX: Add more low level insects for this function so we can remove
    // some of the non-insects (although it's probably okay to leave it
    // as creepy crawlies) or simply improve the distribution?
    const insect_def insects[] =
    {
        // ordered by hit dice:
        { MONS_GIANT_COCKROACH,    10,  0 },
        { MONS_BUTTERFLY,           8,  0 },
        { MONS_GIANT_MOSQUITO,      8,  1 },

        { MONS_GIANT_MITE,          8,  2 },
        { MONS_GIANT_CENTIPEDE,     8,  2 },

        { MONS_GIANT_ANT,           8,  5 },
        { MONS_KILLER_BEE,          5, 10 },
        { MONS_SCORPION,            5, 15 },

        { MONS_YELLOW_WASP,         1, 35 },

        { MONS_GIANT_BEETLE,        3, 15 },
        { MONS_GIANT_BLOWFLY,       5, 10 },

        { MONS_REDBACK,             3, 20 },
        { MONS_SOLDIER_ANT,         5, 15 },

        { MONS_BUMBLEBEE,           3, 15 },

        { MONS_WOLF_SPIDER,         3, 25 },
        { MONS_RED_WASP,            1, 50 },
    };

    const int num_insects = sizeof( insects ) / sizeof( insect_def );

    for (int i = 0; i < numsc; i++)
    {
        int count = 0;
        int pick = -1;

        if (pow <= 0)
            break;

        for (int j = 0; j < num_insects; j++)
        {
            if (pow >= insects[j].cost)
            {
                count += insects[j].rare;
                if (random2(count) < insects[j].rare)
                    pick = j;
            }
        }

        if (pick == -1)
            continue;

        beh_type behaviour = BEH_HOSTILE;  // default to unfriendly

        // Note: friendly, non-god_gift means spell.
        if (god_gift)
            behaviour = BEH_GOD_GIFT;
        else if (!unfriendly && random2(pow) > 7)
            behaviour = BEH_FRIENDLY;

        if (create_monster( insects[pick].mon, behaviour, 2, tx, ty, target ))
        {
            summoned = true;
            pow -= insects[pick].cost;
        }
    }

    return (summoned);
}                               // end summon_swarm()

void summon_undead(int pow)
{
    int temp_rand = 0;
    int thing_called = MONS_PROGRAM_BUG;        // error trapping {dlb}

    int numsc = 1 + random2(pow) / 30 + random2(pow) / 30;
    numsc = stepdown_value(numsc, 2, 2, 6, 8);  //see stuff.cc {dlb}

    mpr("You call on the undead to aid you!");

    for (int scount = 0; scount < numsc; scount++)
    {
        temp_rand = random2(25);

        thing_called = ((temp_rand > 8) ? MONS_WRAITH :          // 64%
                        (temp_rand > 3) ? MONS_SPECTRAL_WARRIOR  // 20%
                                        : MONS_FREEZING_WRAITH); // 16%

        beh_type behav = (random2(pow) < 6) ? BEH_HOSTILE : BEH_FRIENDLY;

        if (create_monster( thing_called, behav, 5 ) != -1)
        {
            if (behav == BEH_HOSTILE)
                mpr("You sense a hostile presence.");
            else
                mpr("An insubstantial figure forms in the air.");
        }
    }                           // end for loop

    //jmf: Kiku sometimes deflects this
    if (!you.is_undead
        && !(you.religion == GOD_KIKUBAAQUDGHA
             && (!player_under_penance()
                 && you.piety >= 100 && random2(200) <= you.piety)))
    {
        disease_player( 25 + random2(50) );
    }
}                               // end summon_undead()

void summon_things( int pow )
{
    int big_things = 0;
    int numsc = 2 + (random2(pow) / 10) + (random2(pow) / 10);

    if (one_chance_in(3) && !lose_stat( STAT_INTELLIGENCE, 1, true ))
        mpr("Your call goes unanswered.");
    else
    {
        numsc = stepdown_value( numsc, 2, 2, 6, -1 );

        while (numsc > 2)
        {
            if (one_chance_in(4))
                break;

            numsc -= 2;
            big_things++;
        }

        if (numsc > 8)
            numsc = 8;

        if (big_things > 8)
            big_things = 8;

        while (big_things > 0)
        {
            create_monster( MONS_TENTACLED_MONSTROSITY, BEH_FRIENDLY );
            big_things--;
        }

        while (numsc > 0)
        {
            create_monster( MONS_ABOMINATION_LARGE, BEH_FRIENDLY );
            numsc--;
        }

        mpr( "Some Thing%s answered your call!",
              (numsc + big_things > 1) ? "s" : "" );

    }

    return;
}

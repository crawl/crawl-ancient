/*
 *  File:       mstuff2.cc
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <5>     31 July 2000   JDJ      Fixed mon_throw to use lnchType.
 *      <4>     29 July 2000   JDJ      Tweaked mons_throw so it doesn't index past
 *                                      the end of the array when monsters don't have
 *                                      a weapon equipped.
 *      <3>     25 July 2000   GDL      Fixed Manticores
 *      <2>     28 July 2000   GDL      Revised monster throwing
 *      <1>     -/--/--        LRH      Created
 */

#include "AppHdr.h"
#include "mstuff2.h"

#include <string>
#include <string.h>
#include <stdio.h>

#include "globals.h"
#include "externs.h"

#include "beam.h"
#include "debug.h"
#include "effects.h"
#include "fight.h"
#include "itemname.h"
#include "itemprop.h"
#include "items.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mon-util.h"
#include "player.h"
#include "spells2.h"
#include "spells4.h"
#include "spl-cast.h"
#include "stuff.h"
#include "view.h"

static int monster_abjuration(int pow, bool test);

// XXX: must fix species abils to not use duration 15
// -- ummm ... who wrote this? {dlb}

// NB: only works because grid location already verified
//     to be some sort of trap prior to function call: {dlb}
void mons_trap( struct monsters *monster )
{
    int temp_rand = 0;          // probability determination {dlb}

    // single calculation permissible {dlb}
    bool monsterNearby = mons_near(monster);

    // new function call {dlb}
    int which_trap = trap_at_xy(monster->x, monster->y);
    if (which_trap == -1)
        return;

    bool trapKnown = (grd[monster->x][monster->y] != DNGN_UNDISCOVERED_TRAP);
    bool revealTrap = false;    // more sophisticated trap uncovering {dlb}
    bool ammo_trap = false;
    int damage_taken = -1;      // must initialize at -1 for this f(x) {dlb}

    struct bolt beem;


    // flying monsters neatly avoid mechanical traps
    // and may actually exit this function early: {dlb}
    if (mons_flies(monster))
    {
        if (trap_category(env.trap[which_trap].type) == DNGN_TRAP_MECHANICAL)
        {
            if (trapKnown)
                mon_msg(monster, " flies safely over a trap.");

            return;             // early return {dlb}
        }
    }

    //
    // Trap damage to monsters is not a function of level, beacuse they
    // are fairly stupid and tend to have fewer hp than players -- this
    // choice prevents traps from easily killing large monsters fairly
    // deep within the dungeon.
    //
    switch (env.trap[which_trap].type)
    {
    case TRAP_DART:
        ammo_trap = true;
        strcpy(beem.name, " dart");
        beem.damage = dice_def( 1, 4 );
        beem.colour = OBJ_MISSILES;
        beem.type = MI_DART;
        break;
    case TRAP_NEEDLE:
        ammo_trap = true;
        strcpy(beem.name, " needle");
        beem.damage = dice_def( 1, 0 );
        beem.colour = OBJ_MISSILES;
        beem.type = MI_NEEDLE;
        break;
    case TRAP_ARROW:
        ammo_trap = true;
        strcpy(beem.name, "n arrow");
        beem.damage = dice_def( 1, 7 );
        beem.colour = OBJ_MISSILES;
        beem.type = MI_ARROW;
        break;
    case TRAP_SPEAR:
        ammo_trap = true;
        strcpy(beem.name, " spear");
        beem.damage = dice_def( 1, 10 );
        beem.colour = OBJ_WEAPONS;
        beem.type = WPN_SPEAR;
        break;
    case TRAP_BOLT:
        ammo_trap = true;
        strcpy(beem.name, " bolt");
        beem.damage = dice_def( 1, 13 );
        beem.colour = OBJ_MISSILES;
        beem.type = MI_BOLT;
        break;
    case TRAP_AXE:
        ammo_trap = true;
        strcpy(beem.name, "n axe");
        beem.damage = dice_def( 1, 15 );
        beem.colour = OBJ_WEAPONS;
        beem.type = WPN_HAND_AXE;
        break;
    // teleport traps are *never* revealed through
    // the triggering action of a monster, as any
    // number of factors could have been in play: {dlb}
    case TRAP_TELEPORT:
        monster_teleport( monster );
        break;
    // amnesia traps do not affect monsters (yet) and
    // only monsters of normal+ IQ will direct a msg
    // to the player - also, *never* revealed: {dlb}
    case TRAP_AMNESIA:
        if (mons_intel(monster->type) > I_ANIMAL)
            mon_msg(monster, " seems momentarily disoriented.");
        break;
    // blade traps sometimes fail to trigger altogether,
    // resulting in an "early return" from this f(x) for
    // some - otherwise, blade *always* revealed: {dlb}
    case TRAP_BLADE:
        if (one_chance_in(5))
        {
            if (trapKnown)
            {
                mon_msg(monster, " fails to trigger a blade trap.");
            }
            return;             // early return {dlb}
        }
        else if (random2(monster->evasion) > 3)
        {
            if (monsterNearby && !mon_msg(monster, " avoids a huge, swinging blade."))
            {
                mpr("A huge blade swings out!");
            }

            damage_taken = -1;  // just to be certain {dlb}
        }
        else
        {
            if (monsterNearby)
            {
                strcpy(info, "A huge blade swings out");

                if (player_monster_visible( monster ))
                {
                    strcat(info, " and slices into ");
                    strcat(info, ptr_monam( monster, DESC_NOCAP_THE ));
                }

                strcat(info, "!");
                mpr(info);
            }

            damage_taken = 10 + roll_zdice(2,10);
            damage_taken = apply_mons_armour( damage_taken, monster );

            if (damage_taken < 0)
                damage_taken = 0;
        }

        revealTrap = true;
        break;

    // zot traps are out to get *the player*! Hostile monsters
    // benefit and friendly monsters suffer - such is life - on
    // rare occasion, the trap affects nearby players, triggering
    // an "early return" - zot traps are *never* revealed - instead,
    // enchantment messages serve as clues to the trap's presence: {dlb}
    case TRAP_ZOT:
        if (monsterNearby)
        {
            if (one_chance_in(5))
            {
                mpr("The power of Zot is invoked against you!");
                miscast_effect( SPTYP_RANDOM, 10 + random2(30),
                                75 + random2(100), 0, "the power of Zot" );
                return;         // early return {dlb}
            }
        }

        snprintf( info, INFO_SIZE, "You hear a %s \"Zot\"!",
                  (monsterNearby) ? "loud" : "distant" );
        noisy( SL_TRAP_ZOT, monster->x, monster->y, info );

        // determine trap effects upon monster, based upon
        // whether it is naughty or nice to the player: {dlb}
        temp_rand = random2(16);

        beem.thrower = KILL_MON;        // probably unnecessary
        beem.aux_source = NULL;
        beem.is_launched = true;

        if (mons_friendly(monster))
        {
            beem.flavour = ((temp_rand < 3) ? BEAM_PARALYSIS :// paralyze 3/16
                            (temp_rand < 7) ? BEAM_CONFUSION  //  confuse 4/16
                                            : BEAM_SLOW);     //     slow 9/16
        }
        else
        {
            beem.flavour = ((temp_rand < 3) ? BEAM_HASTE :      // haste 3/16
                            (temp_rand < 7) ? BEAM_INVISIBILITY // invis 4/16
                                            : BEAM_HEALING);    //  heal 9/16
        }

        mons_ench_f2( monster, beem );
        damage_taken = 0;      // just to be certain {dlb}
        break;
    }


    // go back and handle projectile traps: {dlb}
    if (ammo_trap)
    {
        const int level = (you.level_type == LEVEL_DUNGEON) ? you.depth
                                                            : 36;
        const int avoid = 5 + 8 * monster->hit_dice;
        const int diff = 10 + level / 2;

        const int hit = 20 + 3 * level;

        if (!env.trap[which_trap].ammo)
        {
            revealTrap = false;
            noisy( SL_TRAP_CLICK, monster->x, monster->y,
                   "You hear something go click." );
        }
        else if (random2(avoid) >= diff)
        {
            // monster avoids trap
            revealTrap = false;
        }
        else
        {
            env.trap[which_trap].ammo--;

            // projectile traps *always* revealed after "firing": {dlb}
            revealTrap = true;

            // determine whether projectile hits, calculate damage: {dlb}
#if DEBUG_DIAGNOSTICS
            mpr( MSGCH_DIAGNOSTICS, "Monster activated ammo trap:" );
#endif

            const bool can_block = mons_can_block( monster, NULL, true );
            const int  shield_class = mons_shield_class( monster );
            const int  con_block = 3 + level / 2 + mons_blocks_penalty( monster );

            if (can_block && test_block( shield_class, con_block ))
            {
                handle_mon_shield_blocks( monster );

                if (monsterNearby)
                {
                    mpr( "%s blocks a%s!",
                          ptr_monam( monster, DESC_CAP_THE ),
                          beem.name );
                }

                damage_taken = -1;  // negative damage marks a miss
            }
            else if (test_hit( hit, monster->evasion ))
            {
                bool apply_poison = false;

                damage_taken = roll_dice( beem.damage );
                damage_taken = apply_mons_armour( damage_taken, monster );

                if (damage_taken < 0)
                    damage_taken = 0;

                if (beem.colour == OBJ_MISSILES
                    && beem.type == MI_NEEDLE
                    && random2( monster->armour_class + 1 ) < 2)
                {
                    apply_poison = true;
                }

                if (monsterNearby)
                {
                    mpr( "A%s hits %s%s!",
                          beem.name,
                          ptr_monam( monster, DESC_NOCAP_THE ),
                          (damage_taken == 0) ? ", but does no damage" : "" );
                }

                if (apply_poison)
                    poison_monster( monster, false );
            }
            else
            {
                if (monsterNearby)
                {
                    mpr( "A%s misses %s!",
                          beem.name,
                          ptr_monam( monster, DESC_NOCAP_THE ) );
                }

                damage_taken = -1;  // negative damage marks a miss
            }

            drop_trap_item( which_trap, monster->x, monster->y, 1 );
        }
    }


    // reveal undiscovered traps, where appropriate: {dlb}
    if (monsterNearby && !trapKnown && revealTrap)
        grd[monster->x][monster->y] = trap_category(env.trap[which_trap].type);

    // apply damage and handle death, where appropriate: {dlb}
    hurt_monster_to_kill( monster, damage_taken, KILL_MISC, 0 );

    return;
}                               // end mons_trap()

void mons_cast( struct monsters *monster, struct bolt &pbolt, int spell_cast )
{
    // always do setup.  It might be done already, but it doesn't
    // hurt to do it again (cheap).  In fact it's needed for some.
    setup_mons_cast( monster, pbolt, spell_cast );

    // single calculation permissible {dlb}
    bool monsterNearby = mons_near(monster);

    int sumcount = 0;
    int sumcount2;
    int summonik = 0;
    int duration = 0;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Mon #%d casts %s (#%d)", monster_index(monster),
             mons_spell_name( spell_cast ), spell_cast );
#endif

    if (spell_cast == MS_HELLFIRE_BURST || spell_cast == MS_BRAIN_FEED
        || spell_cast == MS_SMITE || spell_cast == MS_MUTATION)
    {                           // etc.
        if (monster->foe == MHITYOU || monster->foe == MHITNOT)
        {
            if (monsterNearby)
                direct_effect( pbolt );
        }
        else
        {
            mons_direct_effect( pbolt, monster_index(monster) );
        }
        return;
    }

    switch (spell_cast)
    {
    case MS_VAMPIRE_SUMMON:
        sumcount2 = 3 + random2(3) + monster->hit_dice / 5;

        for (sumcount = 0; sumcount < sumcount2; sumcount++)
        {
            int mons = MONS_GIANT_BAT;

            if (!one_chance_in(3))
            {
                switch (random2(4))
                {
                case 0:
                    mons = MONS_ORANGE_RAT;
                    break;

                case 1:
                    mons = MONS_GREEN_RAT;
                    break;

                case 2:
                    mons = MONS_GREY_RAT;
                    break;

                case 3:
                default:
                    mons = MONS_RAT;
                    break;
                }
            }

            create_monster( mons, SAME_ATTITUDE(monster), 5,
                            monster->x, monster->y, monster->foe );
        }
        return;

    case MS_LEVEL_SUMMON:       // summon anything appropriate for level
        if (!mons_friendly(monster) && monsterNearby
            && monster_abjuration(1, true) > 0 && coinflip())
        {
            monster_abjuration( monster->hit_dice * 10, false );
            return;
        }

        sumcount2 = 1 + random2(4) + random2( monster->hit_dice / 7 + 1 );

        for (sumcount = 0; sumcount < sumcount2; sumcount++)
        {
            create_monster( RANDOM_MONSTER, SAME_ATTITUDE(monster), 5,
                            monster->x, monster->y, monster->foe );
        }
        return;

    case MS_FAKE_RAKSHASA_SUMMON:
        sumcount2 = (coinflip() ? 2 : 3);

        for (sumcount = 0; sumcount < sumcount2; sumcount++)
        {
            create_monster( MONS_RAKSHASA_FAKE, SAME_ATTITUDE(monster), 3,
                            monster->x, monster->y, monster->foe );
        }
        return;

    case MS_SUMMON_DEMON: // class 2-4 demons
        if (!mons_friendly(monster) && monsterNearby
            && monster_abjuration(1, true) > 0 && coinflip())
        {
            monster_abjuration(monster->hit_dice * 10, false);
            return;
        }

        sumcount2 = 1 + random2(2) + random2( monster->hit_dice / 10 + 1 );

        duration  = 2 + monster->hit_dice / 10;
        if (duration > 6)
            duration = 6;

        for (sumcount = 0; sumcount < sumcount2; sumcount++)
        {
            create_monster( rand_demon(DEMON_COMMON),
                            SAME_ATTITUDE(monster), duration,
                            monster->x, monster->y,
                            monster->foe );
        }
        return;

    case MS_ANIMATE_DEAD:
        // see special handling in monstuff::handle_spell {dlb}
        animate_dead( 100, monster->x, monster->y,
                      SAME_ATTITUDE(monster), monster->foe, 1 );
        return;

    case MS_SUMMON_DEMON_LESSER: // class 5 demons
        sumcount2 = 1 + random2(3) + random2( monster->hit_dice / 5 + 1 );

        duration  = 2 + monster->hit_dice / 5;
        if (duration > 6)
            duration = 6;

        for (sumcount = 0; sumcount < sumcount2; sumcount++)
        {
            create_monster( rand_demon(DEMON_LESSER),
                            SAME_ATTITUDE(monster), duration,
                            monster->x, monster->y,
                            monster->foe );
        }
        return;

    case MS_SUMMON_UFETUBUS:
        sumcount2 = 2 + random2(2) + random2( monster->hit_dice / 5 + 1 );

        duration  = 2 + monster->hit_dice / 5;
        if (duration > 6)
            duration = 6;

        for (sumcount = 0; sumcount < sumcount2; sumcount++)
        {
            create_monster( MONS_UFETUBUS, SAME_ATTITUDE(monster), duration,
                            monster->x, monster->y, monster->foe );
        }
        return;

    case MS_SUMMON_BEAST:       // Geryon
        create_monster( MONS_BEAST, SAME_ATTITUDE(monster), 4,
                        monster->x, monster->y, monster->foe );
        return;

    case MS_SUMMON_UNDEAD:      // summon undead around player
        if (!mons_friendly(monster) && monsterNearby
            && monster_abjuration( 1, true ) > 0 && coinflip())
        {
            monster_abjuration( monster->hit_dice * 10, false );
            return;
        }

        sumcount2 = 2 + random2(2) + random2( monster->hit_dice / 4 + 1 );

        duration  = 2 + monster->hit_dice / 5;
        if (duration > 6)
            duration = 6;

        for (sumcount = 0; sumcount < sumcount2; sumcount++)
        {
            do
            {
                summonik = random2(241);        // hmmmm ... {dlb}
            }
            while (mons_class_holiness( summonik ) != MH_UNDEAD);

            create_monster( summonik, SAME_ATTITUDE(monster), duration,
                            you.x_pos, you.y_pos, monster->foe );
        }
        return;

    case MS_TORMENT:
        if (!monsterNearby || mons_friendly(monster))
            return;

        mon_msg( monster, " calls on the powers of Hell!" );
        torment( monster->x, monster->y );
        return;

    case MS_DISPEL_UNDEAD:
        if (!monsterNearby || mons_friendly(monster))
            return;

        dispel_undead_area( monster->x, monster->y,
                            10 * monster->hit_dice,
                            monster_index( monster ) );
        return;


    case MS_SUMMON_DEMON_GREATER:
        if (!mons_friendly(monster) && !monsterNearby &&
            monster_abjuration(1, true) > 0 && coinflip())
        {
            monster_abjuration(monster->hit_dice * 10, false);
            return;
        }

        sumcount2 = 1 + random2( monster->hit_dice / 10 + 1 );

        duration  = 2 + monster->hit_dice / 10;
        if (duration > 6)
            duration = 6;

        for (sumcount = 0; sumcount < sumcount2; sumcount++)
        {
            create_monster( rand_demon(DEMON_GREATER),
                            SAME_ATTITUDE(monster), duration,
                            monster->x, monster->y, monster->foe );
        }
        return;

    // Journey -- Added in Summon Lizards and Draconian
    case MS_SUMMON_LIZARDS:
        if (!mons_friendly( monster ) && !monsterNearby &&
            monster_abjuration( 1, true ) > 0 && coinflip())
        {
            monster_abjuration( monster->hit_dice * 10, false );
            return;
        }

        sumcount2 = 1 + random2(3) + random2( monster->hit_dice / 5 + 1 );

        duration  = 2 + monster->hit_dice / 5;
        if (duration > 6)
            duration = 6;

        for (sumcount = 0; sumcount < sumcount2; sumcount++)
        {
            create_monster( rand_dragon( DRAGON_LIZARD ),
                            SAME_ATTITUDE(monster), duration,
                            monster->x, monster->y, monster->foe );
        }
        break;

    case MS_CANTRIP:
        // Monster spell of uselessness, just prints a message.
        // This spell exists so that some monsters with really strong
        // spells (ie orc priest) can be toned down a bit. -- bwr
        //
        // XXX: Needs expansion, and perhaps different priest/mage flavours.

        // XXX add some non-player target effects (friendly player too?)
        if (monster->foe != MHITYOU || mons_friendly( monster ))
            return;

        switch (random2(7))
        {
        case 0:
            zap_animation( element_colour( EC_MAGIC ), monster );
            mon_msg( monster, " glows brightly for a moment.", true,
                                    MSGCH_MONSTER_ENCHANT );
            break;
        case 1:
            zap_animation( element_colour( EC_UNHOLY ) );
            mpr( "You feel troubled." );
            break;
        case 2:
            zap_animation( element_colour( EC_UNHOLY ) );
            mpr( "You feel a wave of unholy energy pass over you." );
            break;
        case 3:
            zap_animation( element_colour( EC_MAGIC ), monster );
            mon_msg( monster, " looks stronger.", true, MSGCH_MONSTER_ENCHANT );
            break;
        case 4:
            zap_animation( element_colour( EC_MAGIC ), monster );
            mon_msg( monster, " becomes somewhat translucent.", true,
                                    MSGCH_MONSTER_ENCHANT );
            break;
        case 5:
            zap_animation( element_colour( EC_MAGIC ), monster );
            mon_msg( monster, "'s eyes start to glow.", true,
                                    MSGCH_MONSTER_ENCHANT );
            break;
        case 6:
        default:
            zap_animation( element_colour( EC_MAGIC ) );
            if (one_chance_in(20))
                mpr( "You resist (whatever that was supposed to do)." );
            else
                mpr( "You resist." );
            break;
        }
        return;
    }

    fire_beam( pbolt );
}                               // end mons_cast()


/*
 * setup bolt structure for monster spell casting.
 *
 */

void setup_mons_cast( struct monsters *monster, struct bolt &pbolt,
                      int spell_cast )
{
    // Need to correct this for power of spellcaster
    const int power = 12 * monster->hit_dice;

    // Always set ench_power and beam_source -- they are used by things
    // other than fire_beam().  Also note that ench_power will be modified
    // and reduced by mons_spells.
    pbolt.ench_power = (spell_cast == MS_TELEPORT) ? 2000 : power;
    pbolt.beam_source = monster_index( monster );

    switch (spell_cast)
    {
    case MS_VAMPIRE_SUMMON:
    case MS_LEVEL_SUMMON:       // summon anything appropriate for level
    case MS_FAKE_RAKSHASA_SUMMON:
    case MS_SUMMON_DEMON:
    case MS_ANIMATE_DEAD:
    case MS_SUMMON_DEMON_LESSER:
    case MS_SUMMON_UFETUBUS:
    case MS_SUMMON_BEAST:       // Geryon
    case MS_SUMMON_UNDEAD:      // summon undead around player
    case MS_SUMMON_LIZARDS:
    case MS_TORMENT:
    case MS_DISPEL_UNDEAD:
    case MS_SUMMON_DEMON_GREATER:
    case MS_CANTRIP:
        // these are no-ops since they don't involve direct_effect,
        // fire_tracer, or beam.
        return;

    case MS_HELLFIRE_BURST:     // direct effect
        pbolt.type = DMNBM_HELLFIRE;
        return;
    case MS_BRAIN_FEED:         // direct effect
        pbolt.type = DMNBM_BRAIN_FEED;
        return;
    case MS_SMITE:              // direct effect
        pbolt.type = DMNBM_SMITING;
        return;
    case MS_MUTATION:           // direct effect
        pbolt.type = DMNBM_MUTATION;
        return;
    default:
        break;
    }

    mons_spells( spell_cast, power, pbolt );

    pbolt.aux_source = NULL;
    pbolt.is_launched = false;
    pbolt.is_thrown = false;
    pbolt.source_x = monster->x;
    pbolt.source_y = monster->y;
    pbolt.is_tracer = false;

    if (pbolt.name[0])
        pbolt.aux_source = pbolt.name;
    else
        pbolt.aux_source = NULL;

    if (spell_cast == MS_HASTE
        || spell_cast == MS_INVIS
        || spell_cast == MS_HEAL
        || spell_cast == MS_TELEPORT)
    {
        // At some point we might want to consider having monsters
        // use these on others (ie orc priests healing other orcs).
        pbolt.target_x = monster->x;
        pbolt.target_y = monster->y;
    }
    else if (spell_cast == MS_DIG)
    {
        // assuming monsters with dig have some sort of magical perception
        if (monster->foe == MHITYOU)
        {
            pbolt.target_x = you.x_pos;
            pbolt.target_y = you.y_pos;
        }
        else if (monster->foe != MHITNOT)
        {
            pbolt.target_x = menv[monster->foe].x;
            pbolt.target_y = menv[monster->foe].y;
        }
    }
}                               // end setup_mons_cast()


void monster_teleport( struct monsters *monster, int delay, bool silent,
                       bool away )
{
    delay = (delay < 1) ? 0 : ((delay > 4) ? 4 : delay);

    if (delay)
    {
        if (mons_del_ench( monster, ENCH_TELEPORT ))
        {
            if (!silent)
                mon_msg(monster, " seems more stable.");
        }
        else
        {
            mons_add_ench( monster, ENCH_TELEPORT, MHITNOT, -1, delay );
        }

        return;
    }

    if (!silent)
        mon_msg(monster, " disappears!");

    char ogrid = monster_habitat(monster->type);

    int newx, newy;

    for (;;)
    {
        random_in_bounds( newx, newy );

        // don't land on top of another monster
        if (mgrd[newx][newy] != NON_MONSTER)
            continue;

        // ... or the player
        if (newx == you.x_pos && newy == you.y_pos)
            continue;

        // ... or too close to the player when we're not allowing that
        if (away && grid_distance( you.x_pos, you.y_pos, newx, newy ) <= 8)
            continue;

        // monster's going to the same habitat
        if (ogrid == grd[newx][newy])
            break;

        // DEEP_WATER monsters can be teleported to SHALLOW_WATER
        if (ogrid == DNGN_DEEP_WATER && grd[newx][newy] == DNGN_SHALLOW_WATER)
            break;
    }

    move_monster_to_grid( monster_index( monster ), newx, newy );

    // Mimics change form/colour when teleported
    if (mons_is_mimic( monster->type ))
    {
        monster->type = MONS_GOLD_MIMIC + random2(5);
        monster->colour = get_mimic_colour( monster );
    }
}                               // end monster_teleport()

void setup_dragon( struct monsters *monster, struct bolt &pbolt )
{
    const int type = (mons_genus( monster->type ) == MONS_DRACONIAN)
                            ? draco_subspecies( monster ) : monster->type;

    switch (type)
    {
    case MONS_FIRE_DRAKE:
    case MONS_HELL_HOUND:
    case MONS_DRAGON:
    case MONS_LINDWURM:
    case MONS_XTAHUA:
        snprintf( pbolt.name, ITEMNAME_SIZE, "%s's blast of flame",
                  ptr_monam( monster, DESC_PLAIN ) );

        pbolt.flavour = BEAM_FIRE;
        pbolt.colour = RED;
        pbolt.aux_source = "blast of fiery breath";
        break;

    case MONS_ICE_DRAGON:
    case MONS_FROST_DRAKE:
        snprintf( pbolt.name, ITEMNAME_SIZE, "%s's blast of cold",
                  ptr_monam( monster, DESC_PLAIN ) );

        pbolt.flavour = BEAM_COLD;
        pbolt.colour = WHITE;
        pbolt.aux_source = "blast of icy breath";
        break;

    case MONS_RED_DRACONIAN:
        snprintf( pbolt.name, ITEMNAME_SIZE, "%s's flame breath",
                  ptr_monam( monster, DESC_PLAIN ) );

        pbolt.flavour = BEAM_FIRE;
        pbolt.colour = RED;
        pbolt.aux_source = "blast of flame breath";
        break;

    case MONS_WHITE_DRACONIAN:
        snprintf( pbolt.name, ITEMNAME_SIZE, "%s's frost breath",
                  ptr_monam( monster, DESC_PLAIN ) );

        mpr( MSGCH_DIAGNOSTICS, "bolt name: '%s'", pbolt.name );

        pbolt.flavour = BEAM_COLD;
        pbolt.colour = WHITE;
        pbolt.aux_source = "blast of frost breath";
        break;

    default:
        DEBUGSTR("Bad monster class in setup_dragon()");
    }

    pbolt.range = 8 + random2(5);
    pbolt.rangeMax = 8;
    pbolt.damage = dice_def( 3, (monster->hit_dice * 2) );
    pbolt.type = SYM_ZAP;
    pbolt.hit = 40 + (monster->hit_dice * 5);
    pbolt.beam_source = monster_index(monster);
    pbolt.thrower = KILL_MON;
    pbolt.is_beam = true;
    pbolt.is_enchant = false;
    pbolt.is_launched = false;
    pbolt.is_thrown = false;
}                               // end setup_dragon();

void setup_generic_throw(struct monsters *monster, struct bolt &pbolt)
{
    pbolt.range = 8;
    pbolt.rangeMax = 8;
    pbolt.beam_source = monster_index(monster);
    pbolt.hit = 1500;   // used first for tracer, so assume we'll hit

    pbolt.type = SYM_MISSILE;
    pbolt.flavour = BEAM_MISSILE;
    pbolt.thrower = KILL_MON_MISSILE;
    pbolt.aux_source = NULL;
    pbolt.is_beam = false;
    pbolt.is_enchant = false;
}

bool mons_throw( struct monsters *mon, struct bolt &pbolt, int ammo_id )
{
    // this was assumed during cleanup down below:
    ASSERT( ammo_id == mon->inv[MSLOT_MISSILE] );

    // XXX: ugly hack, but avoids adding dynamic allocation to this code
    static char throw_buff[ ITEMNAME_SIZE ];

    int baseHit = 0, baseDam = 0;       // from thrown or ammo
    int ammoHitBonus = 0, ammoDamBonus = 0;     // from thrown or ammo
    int lnchHitBonus = 0, lnchDamBonus = 0;     // special add from launcher
    int exHitBonus = 0, exDamBonus = 0; // 'extra' bonus from skill/dex/str

    int hitMult = 0;
    int damMult = 0;

    item_def ammo = mitm[ammo_id];  // copy changed for venom launchers
    ammo.quantity = 1;

    pbolt.range = 8;
    pbolt.rangeMax = 8;
    pbolt.beam_source = monster_index(mon);

    pbolt.type = SYM_MISSILE;
    pbolt.colour = ammo.colour;
    pbolt.flavour = BEAM_MISSILE;
    pbolt.thrower = KILL_MON_MISSILE;
    pbolt.aux_source = NULL;

    // extract weapon/ammo bonuses due to magic
    ammoHitBonus = ammo.plus;
    ammoDamBonus = ammo.plus2;

    // figure out if we're thrown or launched
    const launch_retval launched = is_launched( monster_index(mon), mitm[ammo_id] );

    if (launched == LRET_LAUNCHED)
    {
        const int wpn = mon->inv[MSLOT_WEAPON];
        ASSERT( wpn != NON_ITEM && mitm[wpn].base_type == OBJ_WEAPONS );

        lnchHitBonus = mitm[wpn].plus;
        lnchDamBonus = mitm[wpn].plus2;

        switch (mitm[wpn].sub_type)
        {
        case WPN_BLOWGUN:
            baseHit = 20;
            hitMult = 60;
            damMult = 0;
            lnchDamBonus = 0;
            break;
        case WPN_BOW:
        case WPN_LONGBOW:
            baseHit = 10;
            hitMult = 80;
            damMult = 35;
            // monsters get half the launcher damage bonus,
            // which is about as fair as I can figure it.
            lnchDamBonus = (lnchDamBonus + 1) / 2;
            break;
        case WPN_CROSSBOW:
            baseHit = 20;
            hitMult = 50;
            damMult = 30;
            break;
        case WPN_HAND_CROSSBOW:
            baseHit = 20;
            hitMult = 50;
            damMult = 20;
            break;
        case WPN_SLING:
            baseHit = 10;
            hitMult = 40;
            damMult = 20;
            // monsters get half the launcher damage bonus,
            // which is about as fair as I can figure it.
            lnchDamBonus /= 2;
            break;
        }

        baseDam = property( ammo, PWPN_HIT );

        // missiles don't have pluses2;  use hit bonus
        ammoDamBonus = ammoHitBonus;

        exHitBonus = (hitMult * mon->hit_dice + 5) / 10;
        exDamBonus = (damMult * mon->hit_dice + 5) / 10;

        // elven bow w/ elven arrow, also orcish
        if (get_equip_race( mitm[wpn] ) != ISFLAG_NO_RACE
            && get_equip_race( mitm[wpn] ) == get_equip_race( ammo ))
        {
            baseHit += 5;
            baseDam++;
        }

        // elven launcher bonuses:
        if (get_equip_race( mitm[wpn] ) == ISFLAG_ELVEN)
        {
            baseHit += 5;

            if (mons_species( mon->type ) == MONS_ELF)
                baseHit += 5;
        }

        // monsters no longer gain unfair advantages with weapons of fire/ice
        // and incorrect ammo.  They now have same restriction as players.
        const int bow_brand = get_weapon_brand( mitm[wpn] );

        const int ammo_brand = get_ammo_brand( ammo );
        const bool poison = (ammo_brand == SPMSL_POISONED
                            || ammo_brand == SPMSL_POISONED_UNUSED);

        // POISON brand launchers poison ammo
        if (bow_brand == SPWPN_VENOM && ammo_brand == SPMSL_NORMAL)
            set_item_ego_type( ammo, OBJ_MISSILES, SPMSL_POISONED );


        // WEAPON or AMMO of FIRE
        if ((bow_brand == SPWPN_FLAMING || ammo_brand == SPMSL_FLAME)
            && bow_brand != SPWPN_FREEZING && ammo_brand != SPMSL_ICE)
        {
            baseHit += 5;
            exDamBonus += 6;
            pbolt.flavour = BEAM_FIRE;
            strcpy(pbolt.name, "bolt of ");

            if (poison)
                strcat(pbolt.name, "poison ");

            strcat(pbolt.name, "flame");
            pbolt.colour = RED;
            pbolt.type = SYM_ZAP;
        }

        // WEAPON or AMMO of FROST
        if ((bow_brand == SPWPN_FREEZING || ammo_brand == SPMSL_ICE)
            && bow_brand != SPWPN_FLAMING && ammo_brand != SPMSL_FLAME)
        {
            baseHit += 5;
            exDamBonus += 6;
            pbolt.flavour = BEAM_COLD;
            strcpy(pbolt.name, "bolt of ");

            if (poison)
                strcat(pbolt.name, "poison ");

            strcat(pbolt.name, "frost");
            pbolt.colour = WHITE;
            pbolt.type = SYM_ZAP;
        }

        // Note: we already have 10 energy taken off.  -- bwr
        if (mitm[wpn].sub_type == WPN_CROSSBOW)
            mon->energy += ((bow_brand == SPWPN_SPEED) ? 4 : -2);
        else if (bow_brand == SPWPN_SPEED)
            mon->energy += 5;
    }
    else if (launched == LRET_THROWN)
    {
        if (ammo.base_type == OBJ_MISSILES && ammo.sub_type == MI_DART)
        {
            baseHit = 20;
            hitMult = 40;
            damMult = 25;
        }
        else
        {
            baseHit = 15;
            hitMult = 30;
            damMult = 25;
        }

        baseDam = property( ammo, PWPN_DAMAGE );

        // ammo damage needs adjusting here - OBJ_MISSILES
        // don't get separate tohit/damage bonuses!
        if (ammo.base_type == OBJ_MISSILES)
            ammoDamBonus = ammoHitBonus;

        // give monster "skill" bonuses based on HD
        exHitBonus = (hitMult * mon->hit_dice + 5) / 10;
        exDamBonus = (damMult * mon->hit_dice + 5) / 10;
    }

    // monster intelligence bonus
    if (mons_intel( mon->type ) == I_HIGH)
        exHitBonus += 10;

    // now,  if a monster is, for some reason,  throwing something really
    // stupid,  it will have baseHit of 0 and damage of 0.  Ah well.
    strcpy(info, ptr_monam( mon, DESC_CAP_THE) );

    strcat(info, (launched == LRET_LAUNCHED) ? " shoots " : " throws ");

    if (strlen(pbolt.name) > 0)
    {
        strcat(info, "a ");
        strcat(info, pbolt.name);
    }
    else
    {
        // build shoot message
        char str_pass[ ITEMNAME_SIZE ];
        item_name( ammo, DESC_NOCAP_A, str_pass );
        strcat(info, str_pass);

        // build beam name
        item_name( ammo, DESC_PLAIN, str_pass );
        strcpy(pbolt.name, str_pass);
    }

    strcat(info, ".");
    mpr(info);


    if (launched == LRET_LAUNCHED)
    {
        snprintf( throw_buff, sizeof(throw_buff), "Shot with a%s %s by %s",
                  is_vowel(pbolt.name[0]) ? "n" : "", pbolt.name,
                  ptr_monam( mon, DESC_NOCAP_A ) );
    }
    else
    {
        snprintf( throw_buff, sizeof(throw_buff), "Hit by a%s %s thrown by %s",
                  is_vowel(pbolt.name[0]) ? "n" : "", pbolt.name,
                  ptr_monam( mon, DESC_NOCAP_A ) );
    }

    pbolt.aux_source = throw_buff;

    // add everything up.
    pbolt.hit = baseHit + exHitBonus + 5 * ammoHitBonus;
    pbolt.damage = dice_def( 1, baseDam + random2(exDamBonus + 1) + ammoDamBonus );

    if (launched == LRET_LAUNCHED)
    {
        // pluses from launcher
        pbolt.hit += 5 * lnchHitBonus;
        pbolt.damage.size += lnchDamBonus;
    }

    fire_beam( pbolt, &ammo );

    if (dec_mitm_item_quantity( ammo_id, 1 ))
        mon->inv[MSLOT_MISSILE] = NON_ITEM;

    return (true);
}                               // end mons_throw()

// should really do something about mons_hit, but can't be bothered
void spore_goes_pop( struct monsters *monster )
{
    struct bolt beam;
    int type = monster->type;

    if (monster == NULL)
        return;

    beam.is_tracer = false;
    beam.is_enchant = false;
    beam.is_explosion = true;
    beam.beam_source = monster_index( monster );
    beam.type = SYM_BURST;
    beam.target_x = monster->x;
    beam.target_y = monster->y;
    beam.thrower = KILL_MON;    // someone else's explosion
    beam.aux_source = NULL;

    if (type == MONS_GIANT_SPORE)
    {
        beam.flavour = BEAM_SPORE;
        strcpy(beam.name, "explosion of spores");
        beam.colour = LIGHTGREY;
        beam.damage = dice_def( 3, 15 );
        beam.ex_size = 2;
        strcpy( info, "The giant spore explodes!" );
    }
    else
    {
        beam.flavour = BEAM_ELECTRICITY;
        strcpy(beam.name, "blast of lightning");
        beam.colour = LIGHTCYAN;
        beam.damage = dice_def( 3, 20 );
        beam.ex_size = coinflip() ? 3 : 2;
        strcpy( info, "The ball lightning explodes!" );
    }

    if (mons_near( monster ))
    {
        viewwindow( true, false );
        mpr( info );
    }

    explosion( beam );
}                               // end spore_goes_pop()

void mons_spells( int spell_cast, int power, struct bolt &beam )
{
    ASSERT(power > 0);

    // initialize to some bogus values so we can catch problems
    strcpy( beam.name, "****" );
    beam.colour = 1000;
    beam.hit = -1;
    beam.damage = dice_def( 1, 0 );
    beam.type = -1;
    beam.flavour = -1;
    beam.thrower = KILL_MON_MISSILE;
    beam.is_enchant = false;
    beam.is_beam = false;
    beam.is_explosion = false;
    beam.range = 8;
    beam.rangeMax = 8;

    // XXX: Reducing ench_power back towards it's more traditional 3.30
    // level (which was 5 per level... here it's 6 per level, and before
    // this change it was equal to power at 12 per level).  This may
    // or may not be a good thing... needs testing.
    beam.ench_power = power / 2;

    switch (spell_cast)
    {
    case MS_MMISSILE:
        beam.colour = LIGHTMAGENTA;     //inv_colour [throw_2];
        strcpy( beam.name, "magic dart" );       // inv_name [throw_2]);
        beam.damage = dice_def( 2, 6 + power / 16 );
        beam.hit = AUTOMATIC_HIT;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;
        beam.is_beam = false;            // hey, it's blockable! cool!
        break;

    case MS_FLAME:
        beam.colour = RED;
        strcpy( beam.name, "puff of flame" );

        // should this be the same as magic missile?
        // No... magic missile is special in that it has a really
        // high to-hit value, so these should do more damage -- bwr
        beam.damage = dice_def( 2, 7 + power / 12 );

        beam.hit = 60 + power / 40;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON_MISSILE;
        beam.flavour = BEAM_FIRE;
        beam.is_beam = false;
        break;

    case MS_FROST:
        beam.colour = WHITE;
        strcpy( beam.name, "puff of frost" );

        // should this be the same as magic missile?
        // see MS_FLAME -- bwr
        beam.damage = dice_def( 2, 7 + power / 12 );

        beam.hit = 60 + power / 40;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON_MISSILE;
        beam.flavour = BEAM_COLD;
        beam.is_beam = false;
        break;

    case MS_PARALYSIS:
        beam.name[0] = '\0';
        beam.type = 0;
        beam.flavour = BEAM_PARALYSIS;
        beam.thrower = KILL_MON_MISSILE;
        beam.is_beam = true;
        beam.is_enchant = true;
        break;

    case MS_SLOW:
        beam.name[0] = '\0';
        beam.type = 0;
        beam.flavour = BEAM_SLOW;
        beam.thrower = KILL_MON_MISSILE;
        beam.is_beam = true;
        beam.is_enchant = true;
        break;

    case MS_HASTE:              // (self)
        beam.name[0] = '\0';
        beam.type = 0;
        beam.flavour = BEAM_HASTE;
        beam.thrower = KILL_MON_MISSILE;
        beam.is_beam = true;
        beam.is_enchant = true;
        break;

    case MS_CONFUSE:
        beam.name[0] = '\0';
        beam.type = 0;
        beam.flavour = BEAM_CONFUSION;
        beam.thrower = KILL_MON_MISSILE;
        beam.is_beam = true;
        beam.is_enchant = true;
        break;

    case MS_VENOM_BOLT:
        strcpy( beam.name, "bolt of poison" );
        beam.damage = dice_def( 3, 7 + power / 13 );
        beam.colour = LIGHTGREEN;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON;
        beam.flavour = BEAM_POISON;
        beam.hit = 80 + power / 20;
        beam.is_beam = true;
        break;

    case MS_POISON_ARROW:
        strcpy( beam.name, "poison arrow" );
        beam.damage = dice_def( 4, 5 + power / 4 );
        beam.colour = LIGHTGREEN;
        beam.type = SYM_MISSILE;
        beam.thrower = KILL_MON;
        beam.flavour = BEAM_POISON_ARROW;
        beam.hit = 40 + power / 5;
        break;

    case MS_FIRE_BOLT:
        strcpy( beam.name, "bolt of fire" );
        beam.range = 8 + random2(5);
        beam.damage = dice_def( 3, 9 + power / 11 );
        beam.colour = RED;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON;
        beam.flavour = BEAM_FIRE;
        beam.hit = 80 + power / 20;
        beam.is_beam = true;
        break;

    case MS_COLD_BOLT:
        strcpy( beam.name, "bolt of cold" );
        beam.range = 8 + random2(5);
        beam.damage = dice_def( 3, 9 + power / 11 );
        beam.colour = WHITE;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON;
        beam.flavour = BEAM_COLD;
        beam.hit = 80 + power / 20;     // hit
        beam.is_beam = true;
        break;

    case MS_LIGHTNING_BOLT:
        strcpy( beam.name, "bolt of lightning" );
        beam.range = 8 + random2(10);
        beam.damage = dice_def( 2, 12 + power / 6 );
        beam.colour = LIGHTCYAN;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON;
        beam.flavour = BEAM_ELECTRICITY;
        beam.hit = 70 + power / 20;
        beam.is_beam = true;
        break;

    case MS_INVIS:
        beam.name[0] = '\0';
        beam.type = 0;
        beam.flavour = BEAM_INVISIBILITY;
        beam.thrower = KILL_MON;
        beam.is_beam = true;
        beam.is_enchant = true;
        break;

    case MS_FIREBALL:
        beam.colour = RED;
        strcpy( beam.name, "fireball" );
        beam.damage = dice_def( 3, 8 + power / 10 );
        beam.hit = 100;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON;
        beam.flavour = BEAM_FIRE;
        beam.is_beam = false;
        beam.is_explosion = true;
        break;

    case MS_HEAL:
        beam.name[0] = '\0';
        beam.type = 0;
        beam.flavour = BEAM_HEALING;
        beam.thrower = KILL_MON;
        beam.damage = dice_def( 2, 5 + power / 3 );
        beam.is_beam = true;
        beam.is_enchant = true;
        break;

    case MS_TELEPORT:
        beam.name[0] = '\0';
        beam.type = 0;
        beam.flavour = BEAM_TELEPORT;        // 6 is used by digging
        beam.thrower = KILL_MON;
        beam.is_beam = true;
        beam.is_enchant = true;
        break;

    case MS_TELEPORT_OTHER:
        beam.name[0] = '\0';
        beam.type = 0;
        beam.flavour = BEAM_TELEPORT;        // 6 is used by digging
        beam.thrower = KILL_MON;
        beam.is_beam = true;
        beam.is_enchant = true;
        break;

    case MS_BLINK:
    case MS_CONTROLLED_BLINK:
        beam.is_beam = false;
        break;

    case MS_CRYSTAL_SPEAR:      // was splinters
        strcpy( beam.name, "crystal spear" );
        beam.damage = dice_def( 4, 9 + power / 10 );
        beam.colour = WHITE;
        beam.type = SYM_MISSILE;
        beam.thrower = KILL_MON;
        beam.flavour = BEAM_MMISSILE;
        beam.hit = 80 + power / 20;
        beam.is_beam = false;
        break;

    case MS_DIG:
        beam.name[0] = '\0';
        beam.range = 8 + random2( power / 5 ) + random2(5);
        beam.rangeMax = 24;
        beam.type = 0;
        beam.flavour = BEAM_DIGGING;
        beam.thrower = KILL_MON;
        beam.is_beam = true;
        beam.is_enchant = true;
        break;

    case MS_NEGATIVE_BOLT:      // negative energy
        strcpy( beam.name, "bolt of negative energy" );
        beam.range = 8 + random2(5);
        beam.damage = dice_def( 3, 8 + power / 13 );
        beam.colour = DARKGREY;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON;
        beam.flavour = BEAM_NEG;
        beam.hit = 80 + power / 20;
        beam.is_beam = true;
        break;

    // 20, 21 are used

    case MS_ORB_ENERGY: // mystic blast
        beam.colour = LIGHTMAGENTA;
        strcpy( beam.name, "orb of energy" );
        beam.damage = dice_def( 3, 8 + power / 14 );
        beam.hit = 100 + power / 20;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;
        beam.is_beam = false;
        break;

    // 23 is brain feed

    case MS_STEAM_BALL:
        beam.colour = LIGHTGREY;
        strcpy( beam.name, "ball of steam" );
        beam.damage = dice_def( 3, 6 + power / 15 );
        beam.hit = 150;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON_MISSILE;
        beam.flavour = BEAM_STEAM;
        beam.is_beam = false;
        break;

    // 27 is summon devils
    // 28 is animate dead

    case MS_PAIN:
        beam.name[0] = '\0';
        beam.type = 0;
        beam.flavour = BEAM_PAIN;     // pain
        beam.thrower = KILL_MON;
        beam.damage = dice_def( 2, 7 + power / 10 );
        beam.is_beam = true;
        beam.is_enchant = true;
        break;

    // 30 is smiting

    case MS_STICKY_FLAME:
        beam.colour = RED;
        strcpy( beam.name, "sticky flame" );
        beam.damage = dice_def( 3, 8 + power / 15 );
        beam.hit = 50 + power / 15;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON_MISSILE;
        beam.flavour = BEAM_NAPALM;
        beam.is_beam = false;
        break;

    case MS_POISON_BLAST:
        strcpy( beam.name, "blast of poison" );
        beam.range = 8 + random2(5);
        beam.damage = dice_def( 3, 8 + power / 15 );
        beam.colour = LIGHTGREEN;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON;
        beam.flavour = BEAM_POISON;
        beam.hit = 100 + power / 20;
        beam.is_beam = true;
        beam.is_big_cloud = true;
        break;

    case MS_PURPLE_BLAST:       // purple bang thing
        beam.colour = LIGHTMAGENTA;
        strcpy( beam.name, "orb of energy" );
        beam.damage = dice_def( 3, 10 + power / 12 );
        beam.hit = 100 + power / 20;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;
        beam.is_beam = false;
        beam.is_explosion = true;
        break;

    case MS_ENERGY_BOLT:        // eye of devastation
        beam.colour = YELLOW;
        strcpy( beam.name, "bolt of energy" );
        beam.range = 8 + random2(10);
        beam.damage = dice_def( 3, 20 + power / 12 );
        beam.hit = 100 + power / 20;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON_MISSILE;
        beam.flavour = BEAM_NUKE;     // a magical missile which destroys walls
        beam.is_beam = true;
        break;

    case MS_STING:              // sting
        beam.colour = GREEN;
        strcpy( beam.name, "sting" );
        beam.damage = dice_def( 2, 6 + power / 10 );
        beam.hit = 60 + power / 20;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON_MISSILE;
        beam.flavour = BEAM_POISON;
        beam.is_beam = false;
        break;

    case MS_IRON_BOLT:
        beam.colour = LIGHTCYAN;
        strcpy( beam.name, "iron bolt" );
        beam.range = 5 + random2(4);
        beam.damage = dice_def( 4, 6 + power / 12 );
        beam.hit = 60 + power / 20;
        beam.type = SYM_MISSILE;
        beam.thrower = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;   // similarly unresisted thing
        beam.is_beam = false;
        break;

    case MS_STONE_ARROW:
        beam.colour = LIGHTGREY;
        strcpy( beam.name, "stone arrow" );
        beam.damage = dice_def( 3, 5 + power / 10 );
        beam.hit = 40 + power / 20;
        beam.type = SYM_MISSILE;
        beam.thrower = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;   // similarly unresisted thing
        beam.is_beam = false;
        break;

    case MS_POISON_SPLASH:
        beam.colour = GREEN;
        strcpy( beam.name, "splash of poison" );
        beam.damage = dice_def( 2, 4 + power / 6 );
        beam.hit = 30 + power / 20;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON_MISSILE;
        beam.flavour = BEAM_POISON;
        beam.is_beam = false;
        break;

    case MS_DISINTEGRATE:
        beam.name[0] = '\0';
        beam.type = 0;
        beam.flavour = BEAM_DISINTEGRATION;
        beam.thrower = KILL_MON;
        beam.damage = dice_def( 2, 15 + power / 6 );
        beam.is_beam = true;
        beam.is_enchant = true;
        break;

    case MS_MARSH_GAS:          // swamp drake
        strcpy( beam.name, "noisome vapour" );
        beam.damage = dice_def( 3, 5 + power / 15 );
        beam.colour = GREEN;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON;
        beam.flavour = BEAM_POISON;
        beam.hit = 80 + power / 20;
        beam.is_beam = false;
        beam.is_big_cloud = true;
        break;

    case MS_MIASMA:            // death drake
        strcpy( beam.name, "foul vapour" );
        beam.damage = dice_def( 3, 5 + power / 24 );
        beam.colour = DARKGREY;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON;
        beam.flavour = BEAM_MIASMA;
        beam.hit = 80 + power / 20;
        beam.is_beam = false;
        break;

    case MS_QUICKSILVER_BOLT:   // Quicksilver dragon
        beam.colour = element_colour( EC_RANDOM );
        strcpy( beam.name, "bolt of energy" );
        beam.damage = dice_def( 3, 25 + power / 24 );
        beam.hit = 100;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;
        beam.is_beam = false;
        break;

    case MS_HELLFIRE:           // fiend's hellfire
        strcpy( beam.name, "hellfire" );
        beam.colour = RED;
        beam.range = 8 + random2(10);
        beam.damage = dice_def( 3, 25 + power / 24 );
        beam.hit = 100;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON;
        beam.flavour = BEAM_HELLFIRE;
        beam.is_beam = true;
        beam.is_explosion = true;
        break;

    case MS_HELLFROST:           // fiend's hellfrost
        strcpy( beam.name, "hellfrost" );
        beam.colour = WHITE;
        beam.range = 8 + random2(10);
        beam.damage = dice_def( 3, 25 + power / 24 );
        beam.hit = 100;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON;
        beam.flavour = BEAM_HELLFROST;
        beam.is_beam = true;
        beam.is_explosion = true;
        break;

    case MS_METAL_SPLINTERS:
        strcpy( beam.name, "spray of metal splinters" );
        beam.range = 8 + random2(10);
        beam.damage = dice_def( 3, 20 + power / 12 );
        beam.colour = CYAN;
        beam.type = SYM_ZAP;
        beam.thrower = KILL_MON;
        beam.flavour = BEAM_FRAG;
        beam.hit = 150 + power / 20;
        beam.is_beam = true;
        break;

    case MS_BANISHMENT:
        beam.name[0] = '\0';
        beam.type = 0;
        beam.flavour = BEAM_BANISH;
        beam.thrower = KILL_MON_MISSILE;
        beam.is_beam = true;
        beam.is_enchant = true;
        break;

    case MS_BLINK_OTHER:
        beam.name[0] = '\0';
        beam.type = 0;
        beam.flavour = BEAM_BLINK;
        beam.thrower = KILL_MON;
        beam.is_beam = true;
        beam.is_enchant = true;
        break;

    default:
        DEBUGSTR("Unknown spell");
    }
}                               // end mons_spells()

static int monster_abjuration( int pow, bool test )
{
    int result = 0;

    if (!test)
        mpr( "Send 'em back where they came from!" );

    for (int ab = 0; ab < MAX_MONSTERS; ab++)
    {
        struct monsters *mon = &menv[ab];

        if (mon->type == -1
            || !mons_near( mon )
            || !mons_friendly( mon ))
        {
            continue;
        }

        mon_enchant_def *const ench = mons_has_ench( mon, ENCH_SUMMONED );

        if (!ench)
            continue;

        result++;

        if (test)
            continue;

        if (pow > 60)
            pow = 60;

        const int dur = levels_to_dur( ENCH_SUMMONED, 1 + (random2(pow) / 3) );

        if (!mons_dec_ench_ptr( mon, ench, dur ))
            mon_msg( mon, " shudders." );
    }

    return (result);
}                               // end monster_abjuration()

/*
 *  File:       mstuff2.cc
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "mstuff2.h"

#include <string>
#include <string.h>

#include "externs.h"

#include "bang.h"
#include "beam.h"
#include "debug.h"
#include "effects.h"
#include "fight.h"
#include "itemname.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mon-util.h"
#include "player.h"
#include "spells.h"
#include "spells2.h"
#include "stuff.h"
#include "view.h"
#include "wpn-misc.h"


static unsigned char monster_abjuration(int pow, bool test);


//NOTE: must fix species abils to not use duration 15 -- ummm ... who wrote this? {dlb}




// NB: only works because grid location already verified
//     to be some sort of trap prior to function call: {dlb}
void mons_trap( struct monsters *monster )
{

    int temp_rand = 0;                    // probability determination {dlb}
    bool monsterNearby = mons_near(monster);    // single calculation permissible {dlb}
    int which_trap = trap_at_xy(monster->x, monster->y);    // new function call {dlb}
    bool trapKnown = ( grd[monster->x][monster->y] != DNGN_UNDISCOVERED_TRAP );
    bool revealTrap = false;              // more sophisticated trap uncovering {dlb}
    bool projectileFired = false;         // <sigh> I had to do it, I swear {dlb}
    int damage_taken = -1;                // must initialize at -1 for this f(x) {dlb}

    struct bolt beem[1];


// flying monsters neatly avoid mechanical traps
// and may actually exit this function early: {dlb}
    if ( mons_flies(monster->type) )
    {
        if ( trap_category(env.trap_type[which_trap]) == DNGN_TRAP_MECHANICAL )
        {
            if ( trapKnown )
              simple_monster_message(monster, " flies safely over a trap.");

            return;    // early return {dlb}
        }
    }


//
// Trap damage to monsters is not a function of level, beacuse they
// are fairly stupid and tend to have fewer hp than players -- this
// choice prevents traps from easily killing large monsters fairly
// deep within the dungeon.
//
    switch ( env.trap_type[which_trap] )
    {
      case TRAP_DART:
        projectileFired = true;
        strcpy(beem[0].beam_name, " dart");
        beem[0].damage = 4;
        beem[0].colour = OBJ_MISSILES;
        beem[0].type = MI_DART;
        break;
      case TRAP_ARROW:
        projectileFired = true;
        strcpy(beem[0].beam_name, "n arrow");
        beem[0].damage = 7;
        beem[0].colour = OBJ_MISSILES;
        beem[0].type = MI_ARROW;
        break;
      case TRAP_SPEAR:
        projectileFired = true;
        strcpy(beem[0].beam_name, " spear");
        beem[0].damage = 10;
        beem[0].colour = OBJ_WEAPONS;
        beem[0].type = WPN_SPEAR;
        break;
      case TRAP_BOLT:
        projectileFired = true;
        strcpy(beem[0].beam_name, " bolt");
        beem[0].damage = 13;
        beem[0].colour = OBJ_MISSILES;
        beem[0].type = MI_BOLT;
        break;
      case TRAP_AXE:
        projectileFired = true;
        strcpy(beem[0].beam_name, "n axe");
        beem[0].damage = 15;
        beem[0].colour = OBJ_WEAPONS;
        beem[0].type = WPN_HAND_AXE;
        break;
// teleport traps are *never* revealed through
// the triggering action of a monster, as any
// number of factors could have been in play: {dlb}
      case TRAP_TELEPORT:
        monster_teleport(monster, true);
        break;
// amnesia traps do not affect monsters (yet) and
// only monsters of normal+ IQ will direct a msg
// to the player - also, *never* revealed: {dlb}
      case TRAP_AMNESIA:
        if ( mons_intel(monster->type) > I_ANIMAL )
          simple_monster_message(monster, " seems momentarily disoriented.");
        break;
// blade traps sometimes fail to trigger altogether,
// resulting in an "early return" from this f(x) for
// some - otherwise, blade *always* revealed: {dlb}
      case TRAP_BLADE:
        if ( one_chance_in(5) )
        {
            if ( trapKnown )
              simple_monster_message(monster, " fails to trigger a blade trap.");

            return;    // early return {dlb}
        }
        else if ( random2(monster->evasion) > 8 )
        {
            if ( monsterNearby
                && !simple_monster_message(monster, " avoids a huge, swinging blade.") )
              mpr("A huge blade swings out!");

            damage_taken = -1;    // just to be certain {dlb}
        }
        else
        {
            if ( monsterNearby )
            {
                strcpy(info, "A huge blade swings out");

                if ( monster->enchantment[2] != ENCH_INVIS || player_see_invis() )
                {
                    strcat(info, " and slices into ");
                    strcat(info, monam(monster->number, monster->type, monster->enchantment[2], 1));
                }

                strcat(info, "!");
                mpr(info);
            }

            damage_taken = 10 + random2avg(29,2);
            damage_taken -= random2(1 + monster->armor_class);
        }

        revealTrap = true;
        break;
// zot traps are out to get *the player*! Hostile monsters
// benefit and friendly monsters suffer - such is life - on
// rare occasion, the trap affects nearby players, triggering
// an "early return" - zot traps are *never* revealed - instead,
// enchantment messages serve as clues to the trap's presence: {dlb}
      case TRAP_ZOT:
        if ( monsterNearby )
        {
            if ( one_chance_in(5) )
            {
                mpr("Something invokes the power of Zot against you.");
                miscast_effect(SPTYP_RANDOM, 10 + random2(30), 75 + random2(100), 0);
                return;     // early return {dlb}
            }
        }

    // output triggering message to player, where appropriate: {dlb}
        if ( !silenced(monster->x, monster->y) )
        {
            strcpy(info, "You hear");
            if ( !monsterNearby )
              strcat(info, " in the distance");
            strcat(info, ": \"Zot!\"");
            mpr(info);
        }

    // determine trap effects upon monster, based upon
    // whether it is naughty or nice to the player: {dlb}

    // NB: beem[].colour values are mislabeled as colours (?) -
    //     cf. mons_ench_f2() [which are also mislabeled] {dlb}
        temp_rand = random2(16);

        beem[0].thing_thrown = KILL_MON;    // probably unnecessary

        if ( monster->behavior == BEH_ENSLAVED || monster->behavior == BEH_FLEE_FRIEND )
        {
            beem[0].colour = ( (temp_rand < 3) ? CYAN :      //paralyze - 3 in 16 {dlb}
                               (temp_rand < 7) ? RED         // confuse - 4 in 16 {dlb}
                                               : BLACK );    //    slow - 9 in 16 {dlb}
        }
        else
        {
            beem[0].colour = ( (temp_rand < 3) ? BLUE :      //haste - 3 in 16 {dlb}
                               (temp_rand < 7) ? MAGENTA     //invis - 4 in 16 {dlb}
                                               : GREEN );    // heal - 9 in 16 {dlb}
        }

        mons_ench_f2(monster, &beem[0]);
        damage_taken = -1;    // just to be certain {dlb}
        break;
    }


// go back and handle projectile traps: {dlb}
    if ( projectileFired )
    {
    // projectile traps *always* revealed after "firing": {dlb}
        revealTrap = true;

    // determine whether projectile hits, calculate damage: {dlb}
        if ( ((20 + (you.your_level << 1)) * random2(200)) / 100 >= monster->evasion )
        {
            damage_taken = random2(beem[0].damage);
            damage_taken -= random2(1 + monster->armor_class);
        }
        else
        {
            damage_taken = -1;    // just to be certain {dlb}
        }

    // output appropriate message, non-positive damage == "miss": {dlb}
        if ( monsterNearby )
        {
            strcpy(info, "A");
            strcat(info, beem[0].beam_name);
            strcat(info, (damage_taken > 0) ? " hits " : " misses ");
            strcat(info, monam(monster->number, monster->type, monster->enchantment[2], 1));
            strcat(info, "!");
            mpr(info);
        }

    // generate "fallen" projectile, where appropriate: {dlb}
        if ( random2(10) < 7 )
        {
            beem[0].bx = monster->x;
            beem[0].by = monster->y;
            itrap(&beem[0], which_trap);
        }
    }


// reveal undiscovered traps, where appropriate: {dlb}
    if ( monsterNearby && !trapKnown && revealTrap )
      grd[monster->x][monster->y] = trap_category(env.trap_type[which_trap]);


// apply damage and handle death, where appropriate: {dlb}
    if ( damage_taken > 0 )
    {
        hurt_monster(monster, damage_taken);

        if ( monster->hit_points < 1 )
        {
            monster_die(monster, KILL_MISC, 0);
            monster->speed_increment = 1;
        }
    }

    return;

}          // end mons_trap()




void mons_cast( struct monsters *monster, struct bolt *pbolt, int spell_cast )
{

    bool monsterNearby = mons_near(monster);    // single calculation permissible {dlb}

    int sumcount = 0;
    int sumcount2;
    int summonik = 0;

    pbolt->beam_source = monster_index(monster);

    if ( spell_cast == MS_HELLFIRE_BURST
          || spell_cast == MS_BRAIN_FEED
          || spell_cast == MS_SMITE
          || spell_cast == MS_MUTATION )
    {                           // etc.

        switch (spell_cast)
        {
          case MS_HELLFIRE_BURST:
            pbolt->type = DMNBM_HELLFIRE;
            break;
          case MS_BRAIN_FEED:
            pbolt->type = DMNBM_BRAIN_FEED;
            break;
          case MS_SMITE:
            pbolt->type = DMNBM_SMITING;
            break;
          case MS_MUTATION:
            pbolt->type = DMNBM_MUTATION;
            break;
        }

        if ( pbolt->trac_targ == MHITYOU || pbolt->trac_targ == MHITNOT )
        {
            if ( monsterNearby )
              direct_effect(pbolt);

            return;
        }

        mons_direct_effect(pbolt, monster_index(monster));
        return;
    }

    switch ( spell_cast )
    {
      case MS_VAMPIRE_SUMMON:
        sumcount2 = 1 + random2(4);

        for (sumcount = 0; sumcount < sumcount2; sumcount++)
          create_monster((one_chance_in(3) ? MONS_RAT : MONS_GIANT_BAT), 24, monster->behavior, monster->x, monster->y, monster->monster_foe, 250);
        return;

      case MS_LEVEL_SUMMON:       // summon anything appropriate for level
        if ( monster->behavior == BEH_CHASING_I    // was menv[0] {dlb}
            && monsterNearby
            && monster_abjuration(1, true) > 0
            && coinflip() )
        {
            monster_abjuration(monster->hit_dice * 10, false);
            return;
        }

        sumcount2 = 1 + random2(4);

        for (sumcount = 0; sumcount < sumcount2; sumcount++)
          create_monster(RANDOM_MONSTER, 24, monster->behavior, monster->x, monster->y, monster->monster_foe, 250);
        return;

      case MS_FAKE_RAKSHASA_SUMMON:
        sumcount2 = ( coinflip() ? 2 : 1 );

        for (sumcount = 0; sumcount < sumcount2; sumcount++)
          create_monster(MONS_RAKSHASA_FAKE, 21, monster->behavior, monster->x, monster->y, monster->monster_foe, 250);
        return;

      case MS_SUMMON_DEMON:
        if ( monster->behavior == BEH_CHASING_I    // was menv[0] {dlb}
            && monsterNearby
            && monster_abjuration(1, true) > 0
            && coinflip() )
        {
            monster_abjuration(monster->hit_dice * 10, false);
            return;
        }

        create_monster(summon_any_demon(DEMON_COMMON), 22, monster->behavior, monster->x, monster->y, monster->monster_foe, 250);
        return;

      case MS_ANIMATE_DEAD:
     // see special handling in monstuff::handle_spell {dlb}
        animate_dead(5 + random2(5), monster->behavior, monster->monster_foe, 1);
        return;

      case MS_SUMMON_DEMON_LESSER:
        create_monster(summon_any_demon(DEMON_LESSER), 21, monster->behavior, monster->x, monster->y, monster->monster_foe, 250);
        return;

      case MS_SUMMON_UFETUBUS:
        create_monster(MONS_UFETUBUS, 21, monster->behavior, monster->x, monster->y, monster->monster_foe, 250);
        return;

      case MS_SUMMON_BEAST:       // Geryon
        create_monster(MONS_BEAST, 23, monster->behavior, monster->x, monster->y, monster->monster_foe, 250);
        return;

      case MS_SUMMON_UNDEAD:      // summon undead around player
        if ( monster->behavior == BEH_CHASING_I    // was menv[0] {dlb}
            && monsterNearby
            && monster_abjuration(1, true) > 0
            && coinflip() )
        {
            monster_abjuration(monster->hit_dice * 10, false);
            return;
        }

        sumcount2 = 2 + random2(3);

        for (sumcount = 0; sumcount < sumcount2; sumcount++)
        {
            do
            {
                summonik = random2(241);     // hmmmm ... {dlb}
            }
            while (mons_holiness(summonik) != MH_UNDEAD);

            create_monster(summonik, 21, monster->behavior, you.x_pos, you.y_pos, monster->monster_foe, 250);
        }
        return;

      case MS_TORMENT:
        if ( !monsterNearby || monster->behavior == BEH_ENSLAVED )
          return;

        simple_monster_message(monster, " calls on the powers of Hell!");

        torment(monster->x, monster->y);
        return;

      case MS_SUMMON_DEMON_GREATER:
        if ( monster->behavior == BEH_CHASING_I    // was menv[0] {dlb}
            && !monsterNearby
            && monster_abjuration(1, true) > 0
            && coinflip() )
        {
            monster_abjuration(monster->hit_dice * 10, false);
            return;
        }

        create_monster(summon_any_demon(DEMON_GREATER), 21, monster->behavior, monster->x, monster->y, monster->monster_foe, 250);
        return;
    }

//out_of_spec:
    viewwindow(1, false);

    pbolt->move_x = pbolt->trac_targ_x - monster->x;
    pbolt->move_y = pbolt->trac_targ_y - monster->y;

    pbolt->target_x = pbolt->trac_targ_x;
    pbolt->target_y = pbolt->trac_targ_y;

    pbolt->source_x = monster->x;
    pbolt->source_y = monster->y;

// Need to correct this for power of spellcaster
    int power = 12 * monster->hit_dice;

    struct SBeam theBeam = mons_spells(spell_cast, power);

    pbolt->colour = theBeam.colour;
    pbolt->range = theBeam.range;
    pbolt->damage = theBeam.damage;
    pbolt->hit = theBeam.hit;
    pbolt->type = theBeam.type;
    pbolt->flavour = theBeam.flavour;
    pbolt->thing_thrown = theBeam.thrown;

    strcpy(pbolt->beam_name, theBeam.name.c_str());

    pbolt->ench_power = pbolt->damage;        // !!!

    pbolt->ench_power = 5 * monster->hit_dice;

    if ( spell_cast == MS_HASTE
          || spell_cast == MS_INVIS
          || spell_cast == MS_HEAL
          || spell_cast == MS_TELEPORT )
    {
        pbolt->move_x = 0;
        pbolt->move_y = 0;
    }

    if ( spell_cast == MS_TELEPORT )
      pbolt->ench_power = 2000;

    if ( spell_cast == MS_HELLFIRE_BURST )
      return;

    if ( theBeam.isBeam )
      beam(pbolt);
    else
      missile(pbolt, 0);

}          // end mons_cast()




void monster_teleport( struct monsters *monster, bool instan )
{

    int p = 0;            // loop variable
    int temp_rand = 0;    // probability determination {dlb}

    if ( !instan )
    {
        for (p = 0; p < 3; p++)
        {
            if ( monster->enchantment[p] >= ENCH_TP_I && monster->enchantment[p] <= ENCH_TP_IV )
            {
                monster->enchantment[p] = ENCH_NONE;

                if ( monster->enchantment[0] == ENCH_NONE
                    && monster->enchantment[1] == ENCH_NONE
                    && monster->enchantment[2] == ENCH_NONE )
                  monster->enchantment1 = 0;

                simple_monster_message(monster, " seems more stable.");

                return;
            }
        }                       // end of for p

        for (p = 0; p < 3; p++)
        {
            if (monster->enchantment[p] == ENCH_NONE)
            {
                monster->enchantment[p] = ( coinflip() ? ENCH_TP_III : ENCH_TP_IV );
                monster->enchantment1 = 1;
                break;
            }
        }
        return;
    }

    simple_monster_message(monster, " disappears!");

    mgrd[monster->x][monster->y] = MNG;

    char ogrid = monster_habitat(monster->type);

// the while portion seems fishy to me for some reason {dlb}
    do
    {
        monster->x = 10 + random2(GXM - 20);
        monster->y = 10 + random2(GYM - 20);
    }
    while ((grd[monster->x][monster->y] != ogrid && (ogrid != DNGN_DEEP_WATER || grd[monster->x][monster->y] != DNGN_SHALLOW_WATER)) || mgrd[monster->x][monster->y] != MNG);

    mgrd[monster->x][monster->y] = monster_index(monster);

/* Mimics change form/colour when t'ported */
    if ( mons_category(monster->type) == MC_MIMIC )
    {
        monster->type = MONS_GOLD_MIMIC + random2(5);
        monster->number = random_colour();

        if (monster->type == MONS_WEAPON_MIMIC || monster->type == MONS_ARMOUR_MIMIC)
        {
            temp_rand = random2(100);

            monster->number = ( (temp_rand > 34) ? BROWN :               // 65% chance {dlb}
                                (temp_rand > 14) ? LIGHTCYAN :           // 20% chance {dlb}
                                (temp_rand >  4) ? CYAN                  // 10% chance {dlb}
                                                 : random_colour() );    //  5% chance {dlb}
        }
    }

}          // end monster_teleport()



void dragon( struct monsters *monster, struct bolt *pbolt )
{

    viewwindow(1, false);

    pbolt->move_x = 0;
    pbolt->move_y = 0;

    pbolt->move_x = pbolt->trac_targ_x - monster->x;
    pbolt->move_y = pbolt->trac_targ_y - monster->y;

    if ( pbolt->move_x || pbolt->move_y )
    {
        pbolt->range = 5 + random2(10);

        pbolt->source_x = monster->x;
        pbolt->source_y = monster->y;

        strcpy(pbolt->beam_name, monam(monster->number, monster->type, monster->enchantment[2], 4));

        switch ( monster->type )
        {
          case MONS_FIREDRAKE:
          case MONS_HELL_HOUND:
          case MONS_DRAGON:
          case MONS_LINDWORM:
          case MONS_XTAHUA:
            strcat(pbolt->beam_name, "'s blast of flame");
            pbolt->flavour = BEAM_FIRE;
            pbolt->colour = RED;
            break;

          case MONS_ICE_DRAGON:
            strcat(pbolt->beam_name, "'s blast of cold");
            pbolt->flavour = BEAM_COLD;
            pbolt->colour = WHITE;
            break;

          default:
            DEBUGSTR("Bad monster class in dragon()");
        }

        pbolt->damage = 100 + (monster->hit_dice << 1);
        pbolt->type = SYM_ZAP;
        pbolt->hit = 30;
        pbolt->beam_source = monster_index(monster);
        pbolt->thing_thrown = KILL_MON;

        simple_monster_message(monster, " breathes.");

        beam(pbolt);
    }

}          // end dragon();




void mons_throw( struct monsters *monster, struct bolt *pbolt, int hand_used )
{

    bool shoot = false;

    pbolt->range = 9;
    pbolt->beam_source = monster_index(monster);

    if ( mons_near(monster) )           /* This is obviously a problem; no throwing unless mons on screen. */
    {
        pbolt->move_x = 0;
        pbolt->move_y = 0;

        pbolt->move_x = pbolt->trac_targ_x - monster->x;
        pbolt->move_y = pbolt->trac_targ_y - monster->y;

        if ( pbolt->move_x != 0 || pbolt->move_y != 0 )
        {
            pbolt->type = SYM_MISSILE;
            pbolt->source_x = monster->x;
            pbolt->source_y = monster->y;
            pbolt->colour = mitm.colour[hand_used];
            pbolt->flavour = BEAM_MISSILE;
            pbolt->damage = property(mitm.base_type[hand_used], mitm.sub_type[hand_used], PWPN_DAMAGE) + mitm.pluses[hand_used] - 50;
            pbolt->hit = 7;
            pbolt->hit += mitm.pluses[hand_used] - 50;
            pbolt->thing_thrown = KILL_MON;

            if ( monster->type == MONS_MANTICORE )
            {
                simple_monster_message(monster, " flicks its tail!");
                pbolt->damage = 10;
                pbolt->hit <<= 1;
            }
            else
            {

            // how dose this interact with two-headed ogres having two melee weapons ? {dlb}
                if ( monster->inv[MSLOT_WEAPON] != ING
                    && mitm.base_type[monster->inv[MSLOT_WEAPON]] == OBJ_WEAPONS
                    && launches_things(mitm.sub_type[monster->inv[MSLOT_WEAPON]])
                    && mitm.sub_type[monster->inv[MSLOT_MISSILE]] == launched_by(mitm.sub_type[monster->inv[MSLOT_WEAPON]]) )
                {

                    pbolt->damage = property(mitm.base_type[monster->inv[MSLOT_MISSILE]], mitm.sub_type[monster->inv[MSLOT_MISSILE]], PWPN_HIT);    // replaced value was "1" is this what was intended?

                    int hoggl = mitm.pluses2[monster->inv[MSLOT_WEAPON]] - 50;

                    if ( mitm.pluses2[monster->inv[MSLOT_WEAPON]] > 80 )
                      hoggl -= 100;

                    pbolt->damage += hoggl;
                    pbolt->damage += monster->hit_dice;


                    /* elven bow w/ elven arrow, also orcish */
                    if ( mitm.special[monster->inv[MSLOT_WEAPON]] / 30 == mitm.special[monster->inv[MSLOT_MISSILE]] / 30 )
                    {
                        pbolt->hit++;
                        pbolt->damage++;
                        if ( mitm.special[monster->inv[MSLOT_WEAPON]] / 30 == DWPN_ELVEN )
                          pbolt->hit++;
                    }


                    int ghoggl = mitm.pluses[monster->inv[MSLOT_WEAPON]] - 50;

                    if (mitm.pluses[monster->inv[MSLOT_WEAPON]] > 80)
                        ghoggl -= 100;

                    pbolt->hit += ghoggl;

                    pbolt->range += 5;

                    if (mitm.pluses[monster->inv[MSLOT_WEAPON]] > 80)
                    {
                        pbolt->damage -= 100;
                        pbolt->hit -= 100;
                    }

                    shoot = true;

                // notice here, that monsters shoot flame or ice, even if the ammo does not match {dlb}:
                    if ( mitm.special[monster->inv[MSLOT_WEAPON]] % 30 == SPWPN_FLAME )
                    {
                        pbolt->hit += 2;
                        pbolt->damage += 1 + random2(5);
                        pbolt->flavour = BEAM_FIRE;
                        strcpy(pbolt->beam_name, "bolt of ");
                        if ( mitm.special[hand_used] % 30 == SPMSL_POISONED || mitm.special[hand_used] % 30 == SPMSL_POISONED_II )
                          strcat(pbolt->beam_name, "poison ");
                        strcat(pbolt->beam_name, "flame");
                        pbolt->colour = RED;
                        pbolt->type = SYM_ZAP;
                        pbolt->thing_thrown = KILL_MON_MISSILE;
                    }

                    if ( mitm.special[monster->inv[MSLOT_WEAPON]] % 30 == SPWPN_FROST )
                    {
                        pbolt->hit += 2;
                        pbolt->damage += 1 + random2(5);
                        pbolt->flavour = BEAM_COLD;
                        strcpy(pbolt->beam_name, "bolt of ");
                        if ( mitm.special[hand_used] % 30 == SPMSL_POISONED || mitm.special[hand_used] % 30 == SPMSL_POISONED_II )
                          strcat(pbolt->beam_name, "poison ");
                        strcat(pbolt->beam_name, "frost");
                        pbolt->colour = WHITE;
                        pbolt->type = SYM_ZAP;
                        pbolt->thing_thrown = KILL_MON_MISSILE;
                    }

                }

                strcpy(info, monam(monster->number, monster->type, monster->enchantment[2], 0));
                strcat(info, (shoot) ? " shoots " : " throws ");

                item_name(mitm.pluses2[hand_used], mitm.base_type[hand_used], mitm.sub_type[hand_used], mitm.special[hand_used], mitm.pluses[hand_used], 1, mitm.id[hand_used], 3, str_pass);

                strcat(info, str_pass);
                strcat(info, ".");
                mpr(info);

            }

            missile(pbolt, hand_used);
            mitm.quantity[hand_used]--;

            if ( mitm.quantity[hand_used] == 0 )
              monster->inv[MSLOT_MISSILE] = ING;
        }
    }


// shouldn't this be within the "mons_near(monster)" conditional? {dlb}:
    if ( monster->type == MONS_CENTAUR )
      monster->speed_increment -= 10;

}          // end mons_throw()




// should really do something about mons_hit, but can't be bothered
void spore_goes_pop( struct monsters *monster )
{

    struct bolt beam[1];

    if ( mons_near(monster) )
    {
        viewwindow(1, false);
        mpr("The giant spore explodes!");
    }

    beam[0].beam_source = monster_index(monster);
    beam[0].type = SYM_BURST;
    beam[0].damage = 115;
    beam[0].flavour = BEAM_SPORE;       // unsure about this
    beam[0].bx = monster->x;
    beam[0].by = monster->y;
    strcpy(beam[0].beam_name, "explosion of spores");
    beam[0].colour = LIGHTGREY;
    beam[0].thing_thrown = KILL_MON;    // someone else's explosion

    explosion(true, &beam[0]);

}          // end spore_goes_pop()




struct SBeam mons_spells( char spell_cast, int power )
{

    ASSERT(power > 0);

    struct SBeam beam;

#if DEBUG
    beam.name = "****";         // initialize to some bogus values so we can catch problems
    beam.colour = 1000;
    beam.range = -1;
    beam.damage = -1;
    beam.hit = -1;
    beam.type = -1;
    beam.flavour = -1;
    beam.thrown = -1;
#endif

    switch (spell_cast)
    {
    case MS_MMISSILE:
        beam.colour = LIGHTMAGENTA;     //inv_colour [throw_2];//icolour [inv_class [throw_2]] [inv_type [throw_2]];
        beam.name = "magic dart";       // inv_name [throw_2]);
        beam.range = 7 + random2(5);
        beam.damage = 104 + (power / 100);
        beam.hit = 1500;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;
        beam.isBeam = false;
        break;

    case MS_FLAME:
        beam.colour = RED;
        beam.name = "puff of flame";
        beam.range = 7 + random2(5);
        beam.damage = 105 + (power / 40);         // should this be the same as magic missile?
        beam.hit = 60;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_FIRE;
        beam.isBeam = false;
        break;

    case MS_FROST:
        beam.colour = WHITE;
        beam.name = "puff of frost";
        beam.range = 7 + random2(5);
        beam.damage = 105 + (power / 40);         // should this be the same as magic missile?
        beam.hit = 60;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_COLD;
        beam.isBeam = false;
        break;

    case MS_PARALYSIS:
        beam.name = "0";
        beam.range = 6 + random2(5);
        beam.type = 0;
        beam.colour = CYAN;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MAGIC;
        beam.isBeam = true;
        break;

    case MS_SLOW:
        beam.name = "0";
        beam.range = 6 + random2(5);
        beam.type = 0;
        beam.colour = BLACK;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MAGIC;
        beam.isBeam = true;
        break;

    case MS_HASTE:              // (self)
        beam.name = "0";
        beam.range = 6 + random2(5);
        beam.type = 0;
        beam.colour = BLUE;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MAGIC;
        beam.isBeam = true;
        break;

    case MS_CONFUSE:
        beam.name = "0";
        beam.range = 6 + random2(5);
        beam.type = 0;
        beam.colour = RED;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MAGIC;
        beam.isBeam = true;
        break;

    case MS_VENOM_BOLT:
        beam.name = "bolt of poison";
        beam.range = 8 + random2(10);
        beam.damage = 106 + power / 13;
        beam.colour = LIGHTGREEN;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_POISON;
        beam.hit = 7 + random2(power) / 80;
        beam.isBeam = true;
        break;

    case MS_FIRE_BOLT:
        beam.name = "bolt of fire";
        beam.range = 5 + random2(10);
        beam.damage = 108 + power / 11;
        beam.colour = RED;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_FIRE;
        beam.hit = 8 + random2(power) / 80;     // hit
        beam.isBeam = true;
        break;

    case MS_COLD_BOLT:
        beam.name = "bolt of cold";
        beam.range = 5 + random2(10);
        beam.damage = 108 + power / 11;
        beam.colour = WHITE;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_COLD;
        beam.hit = 8 + random2(power) / 80;     // hit
        beam.isBeam = true;
        break;

    case MS_LIGHTNING_BOLT:
        beam.name = "bolt of lightning";
        beam.range = random2(10) + 8;
        beam.damage = 110 + power / 9;
        beam.colour = LIGHTCYAN;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_ELECTRICITY;
        beam.hit = 10 + random2(power) / 40;
        beam.isBeam = true;
        break;

    case MS_INVIS:
        beam.name = "0";
        beam.range = random2(5) + 6;
        beam.type = 0;
        beam.colour = MAGENTA;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_MAGIC;
        beam.isBeam = true;
        break;

    case MS_FIREBALL:
        beam.colour = RED;
        beam.name = "fireball";
        beam.range = random2(5) + 7;
        beam.damage = 7 + beam.damage / 10;
        beam.hit = 40;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_YOU_MISSILE;         //DML: ???
        beam.flavour = BEAM_EXPLOSION;  // why not BEAM_FIRE? {dlb}
        beam.isBeam = false;
        break;

    case MS_HEAL:
        beam.name = "0";
        beam.range = random2(5) + 6;
        beam.type = 0;
        beam.colour = GREEN;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_MAGIC;
        beam.isBeam = true;
        break;

    case MS_TELEPORT:
        beam.name = "0";
        beam.range = random2(5) + 6;
        beam.type = 0;
        beam.colour = LIGHTGREY;        // 6 is used by digging
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_MAGIC;
        beam.isBeam = true;
        break;

    case MS_TELEPORT_OTHER:
        beam.name = "0";
        beam.range = random2(5) + 6;
        beam.type = 0;
        beam.colour = LIGHTGREY;        // 6 is used by digging
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_MAGIC;
        beam.isBeam = true;
        break;

    case MS_BLINK:
        beam.isBeam = false;
        break;

    case MS_CRYSTAL_SPEAR:      // was splinters
        beam.name = "crystal spear";
        beam.range = random2(10) + 8;
        beam.damage = 112 + power / 10;
        beam.colour = WHITE;
        beam.type = SYM_MISSILE;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_MMISSILE;
        beam.hit = 6;           // + random2(power) / 10;
        beam.isBeam = false;
        break;

    case MS_DIG:
        beam.name = "0";
        beam.range = random2(5) + 4 + random2(power) / 10;
        beam.type = 0;
        beam.colour = BROWN;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_MAGIC;
        beam.isBeam = true;
        break;

    case MS_NEGATIVE_BOLT:      // negative energy
        beam.name = "bolt of negative energy";
        beam.range = random2(10) + 8;
        beam.damage = 106 + power / 13;
        beam.colour = DARKGREY;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_NEG;
        beam.hit = 7 + random2(power) / 80;
        beam.isBeam = true;
        break;

// 20, 21 are used

    case MS_ORB_ENERGY: // mystic blast
        beam.colour = LIGHTMAGENTA;
        beam.name = "orb of energy";
        beam.range = random2(5) + 7;
        beam.damage = 107 + (power / 14);
        beam.hit = 10 + (power / 20);
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;
        beam.isBeam = false;
        break;

// 23 is brain feed

    case MS_STEAM_BALL:
        beam.colour = LIGHTGREY;
        beam.name = "ball of steam";
        beam.range = random2(5) + 7;
        beam.damage = 106;
        beam.hit = 11;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_FIRE;       // fire - I think this is appropriate
        beam.isBeam = false;
        break;

// 27 is summon devils
// 28 is animate dead

    case MS_PAIN:
        beam.name = "0";
        beam.range = random2(8) + 8;
        beam.type = 0;
        beam.colour = LIGHTMAGENTA;     // pain
        beam.thrown = KILL_MON;
        beam.damage = 50;
        beam.hit = 7 + (power / 20);
        beam.flavour = BEAM_MAGIC;
        beam.isBeam = true;
        break;

// 30 is smiting

    case MS_STICKY_FLAME:
        beam.colour = RED;
        beam.name = "sticky flame";
        beam.range = random2(5) + 7;
        beam.damage = 103 + power / 50;
        beam.hit = 8 + power / 15;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_FIRE;
        beam.isBeam = false;
        break;

    case MS_POISON_BLAST:       // demon
        beam.name = "blast of poison";
        beam.range = random2(10) + 8;
        beam.damage = 103 + power / 25;
        beam.colour = LIGHTGREEN;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_POISON;
        beam.hit = 7 + random2(power) / 80;
        beam.isBeam = true;
        break;

    case MS_PURPLE_BLAST:       // purple bang thing
        beam.colour = LIGHTMAGENTA;
        beam.name = "blast";
        beam.range = random2(5) + 7;
        beam.damage = 106;
        beam.hit = 9;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_EXPLOSION;  // an exploding magical missile
        beam.isBeam = false;
        break;

    case MS_ENERGY_BOLT:        // eye of devastation
        beam.colour = YELLOW;
        beam.name = "bolt of energy";
        beam.range = 10 + random2(15);
        beam.damage = 120;
        beam.hit = 9;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_NUKE;       // a magical missile which destroys walls
        beam.isBeam = true;
        break;

    case MS_STING:              // sting
        beam.colour = GREEN;
        beam.name = "sting";
        beam.range = random2(5) + 9;
        beam.damage = 6 + power / 25;
        beam.hit = 60;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_POISON;
        beam.isBeam = false;
        break;

    case MS_IRON_BOLT:
        beam.colour = LIGHTCYAN;
        beam.name = "iron bolt";
        beam.range = random2(5) + 5;
        beam.damage = 108 + (power / 9);
        beam.hit = 6 + (power / 25);
        beam.type = SYM_MISSILE;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;   // a magical missile - or similarly unresisted thing
        beam.isBeam = false;
        break;

    case MS_STONE_ARROW:
        beam.colour = LIGHTGREY;
        beam.name = "stone arrow";
        beam.range = random2(5) + 9;
        beam.damage = 105 + (power / 10);
        beam.hit = 5 + power / 47;
        beam.type = SYM_MISSILE;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;   // a magical missile - or similarly unresisted thing
        beam.isBeam = false;
        break;

    case MS_POISON_SPLASH:
        beam.colour = GREEN;
        beam.name = "splash of poison";
        beam.range = random2(6) + 6;
        beam.damage = 4 + power / 10;
        beam.hit = 9;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_POISON;
        break;
        beam.isBeam = false;           // something seem a little odd to you all about this coming after the break? 12mar2000 {dlb}

    case MS_DISINTEGRATE:
        beam.name = "0";
        beam.range = random2(8) + 8;
        beam.type = 0;
        beam.colour = WHITE;
        beam.thrown = KILL_MON;
        beam.damage = 50;
        beam.hit = 30 + (power / 10);
        beam.flavour = BEAM_MAGIC;
        beam.isBeam = true;
        break;

    case MS_MARSH_GAS:          // swamp drake
        beam.name = "foul vapour";
        beam.range = random2(10) + 8;
        beam.damage = 102 + power / 25;
        beam.colour = GREEN;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_POISON;
        beam.hit = 7 + random2(power) / 80;
        beam.isBeam = false;
        break;

    case MS_QUICKSILVER_BOLT:   // Quicksilver dragon
        beam.colour = random_colour();
        beam.name = "bolt of energy";
        beam.range = 10 + random2(15);
        beam.damage = 125;
        beam.hit = 9;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;
        beam.isBeam = false;
        break;

    case MS_HELLFIRE:           // fiend's hellfire
        beam.name = "hellfire";
        beam.colour = RED;
        beam.range = random2(10) + 5;
        beam.damage = 25;
        beam.hit = 20;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_EXPLOSION;  // hellfire - why not BEAM_HELLFIRE? {dlb}
        beam.isBeam = true;
        break;

    case MS_METAL_SPLINTERS:
        beam.name = "spray of metal splinters";
        beam.range = random2(10) + 8;
        beam.damage = 120 + power / 20;
        beam.colour = CYAN;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_FRAG;
        beam.hit = 15 + random2(power) / 50;
        beam.isBeam = true;
        break;

    case MS_BANISHMENT:
        beam.name = "0";
        beam.range = random2(5) + 6;
        beam.type = 0;
        beam.colour = LIGHTGREEN;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MAGIC;
        beam.isBeam = true;
        break;

    default:
        DEBUGSTR("Unknown spell");
    }

    return beam;

}          // end mons_spells()




static unsigned char monster_abjuration( int pow, bool test )
{

    unsigned char result = 0;
    struct monsters *monster = 0;    // NULL {dlb}

    if ( !test )
      mpr("Send 'em back where they came from!");

    for (int ab = 0; ab < MNST; ab++)
    {
        monster = &menv[ab];

        if ( monster->type == -1 || !mons_near(monster) )
          continue;

        if ( monster->behavior != BEH_ENSLAVED )
          continue;

        if ( monster->enchantment1 == 0
              || ( ( monster->enchantment[1] < ENCH_ABJ_I || monster->enchantment[1] > ENCH_ABJ_VI )
                  && ( monster->enchantment[1] < ENCH_FRIEND_ABJ_I || monster->enchantment[1] > ENCH_FRIEND_ABJ_VI ) ) )
          continue;

        result++;

        if ( test )
          continue;

        if ( pow > 60 )
          pow = 60;

        monster->enchantment[1] -= 1 + (random2(pow) / 3);

        if (monster->enchantment[1] < ENCH_ABJ_I || (monster->enchantment[1] > ENCH_ABJ_VI && monster->enchantment[1] < ENCH_FRIEND_ABJ_I))
          monster_die(monster, KILL_RESET, 0);
        else
          simple_monster_message(monster, " shudders.");
    }

    return result;

}          // end monster_abjuration()

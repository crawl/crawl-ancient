/*
 *  File:       beam.cc
 *  Summary:    Functions related to ranged attacks.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *   <7>    21mar2001    GDL    Replaced all FP arithmetic with integer*100 math
 *   <6>    07jan2001    GDL    complete rewrite.
 *   <5>    22July2000   GDL    allowed 'dummy' missiles from monsters
 *   <4>    11/14/99     cdl    evade beams with random40(ev) vice random2(ev)
 *                              all armour now protects against shrapnel
 *   <3>     6/ 2/99     DML    Added enums
 *   <2>     5/20/99     BWR    Added refreshs for curses
 *   <1>     -/--/--     LRH    Created
 */

#include "AppHdr.h"
#include "beam.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef DOS
#include <dos.h>
#include <conio.h>
#endif
#if DEBUG_DIAGNOSTICS
#include <stdio.h>
#endif

#include "globals.h"
#include "externs.h"

#include "cloud.h"
#include "delay.h"
#include "effects.h"
#include "enum.h"
#include "fight.h"
#include "it_use2.h"
#include "itemname.h"
#include "itemprop.h"
#include "items.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mon-util.h"
#include "mstuff2.h"
#include "ouch.h"
#include "player.h"
#include "religion.h"
#include "skills.h"
#include "skills2.h"
#include "spells1.h"
#include "spells3.h"
#include "spells4.h"
#include "stuff.h"
#include "view.h"

#define BEAM_STOP       1000        // all beams stopped by subtracting this
                                    // from remaining range
#define MON_RESIST      0           // monster resisted
#define MON_UNAFFECTED  1           // monster unaffected
#define MON_AFFECTED    2           // monster was unaffected

static int spreadx[] = { 0, 0, 1, -1 };
static int spready[] = { -1, 1, 0, 0 };
static int opdir[]   = { 2, 1, 4, 3 };
static FixedArray < bool, 19, 19 > explode_map;

// helper functions (some of these, esp. affect(),  should probably
// be public):
static void sticky_flame_monster( int mn, bool source, int hurt_final );
static bool affectsWalls( struct bolt &beam );
static int affect( struct bolt &beam, int x, int y );
static bool isBouncy( struct bolt &beam );
static void beam_drop_object( struct bolt &beam, item_def *const item, int x, int y );
static bool beam_term_on_target( struct bolt &beam );
static void beam_explodes( struct bolt &beam, int x, int y );
static int bounce( int &step1, int &step2, int w1, int w2, int &n1, int &n2,
    int l1, int l2, int &t1, int &t2, bool topBlocked, bool sideBlocked );
static bool fuzzyLine( int nx, int ny, int &tx, int &ty, int lx, int ly,
    int stepx, int stepy, bool roundX, bool roundY );
static int  affect_wall( struct bolt &beam, int x, int y );
static int  affect_place_clouds( struct bolt &beam, int x, int y );
static void affect_place_explosion_clouds( struct bolt &beam, int x, int y );
static int  affect_player( struct bolt &beam );
static void affect_items( struct bolt &beam, int x, int y );
static int  affect_monster( struct bolt &beam, struct monsters *mon );
static int  affect_monster_enchantment( struct bolt &beam, struct monsters *mon );
static int  range_used_on_hit( struct bolt &beam );
static void explosion1( struct bolt &pbolt );
static void explosion_map( struct bolt &beam, int x, int y, int count, int dir, int r );
static void explosion_cell( struct bolt &beam, int x, int y, bool drawOnly );

static void ench_animation( int flavour, struct monsters *mon = NULL, bool force = false);

static void zappy( char z_type, int power, struct bolt &pbolt );

static bool beam_is_blockable( struct bolt &pbolt )
{
    // BEAM_ELECTRICITY is added here because chain lighting is not
    // a true beam (stops at the first target it gets to and redirects
    // from there)... but we don't want it shield blockable.
    return (!pbolt.is_beam && !pbolt.is_explosion && pbolt.flavour != BEAM_ELECTRICITY);
}

// simple animated flash from Rupert Smith (and expanded to be more generic):
void zap_animation( int colour, struct monsters *mon, bool force )
{
    int x = you.x_pos, y = you.y_pos;

    // default to whatever colour magic is today
    if (colour == -1)
        colour = element_colour( EC_MAGIC );

    if (mon)
    {
        if (!force && !player_monster_visible( mon ))
            return;

        x = mon->x;
        y = mon->y;
    }

    if (!see_grid( x, y ))
        return;

    const int drawx = x - you.x_pos + 18;
    const int drawy = y - you.y_pos + 9;

    if (drawx > 8 && drawx < 26 && drawy > 0 && drawy < 18)
    {
        textcolor( colour );
        gotoxy( drawx, drawy );
        putch( SYM_ZAP );

#ifdef LINUX
        update_screen();
#endif

        delay(50);
    }
}

// special front function for zap_animation to interpret enchantment flavours
static void ench_animation( int flavour, struct monsters *mon, bool force )
{
    const int elem = (flavour == BEAM_HEALING)       ? EC_HEAL :
                     (flavour == BEAM_PAIN)          ? EC_UNHOLY :
                     (flavour == BEAM_DISPEL_UNDEAD) ? EC_HOLY :
                     (flavour == BEAM_POLYMORPH)     ? EC_MUTAGENIC :
                     (flavour == BEAM_TELEPORT
                          || flavour == BEAM_BANISH
                          || flavour == BEAM_BLINK)  ? EC_WARP
                                                     : EC_ENCHANT;
    zap_animation( element_colour( elem ), mon, force );
}

void zapping( char ztype, int power, struct bolt &pbolt )
{
#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "zapping:  power=%d", power );
#endif

    // all of the following might be changed by zappy():
    pbolt.range = 8;                    // "power", gets reduced on hits
    pbolt.rangeMax = 8;                 // "distance", cannot go further
    pbolt.hit = 0;                      // default for enchant beams (I think)
    pbolt.damage = dice_def( 1, 0 );    // default for enchant beams (I think)
    pbolt.type = 0;                     // default for enchant beams
    pbolt.flavour = BEAM_MAGIC;         // default for enchant beams
    pbolt.ench_power = power / 2;
    pbolt.obvious_effect = false;
    pbolt.is_beam = false;               // default for all beams.
    pbolt.is_enchant = false;            // default for all beams.
    pbolt.is_energy = true;              // default for magical beams.
    pbolt.is_tracer = false;             // default for all player beams
    pbolt.thrower = KILL_YOU_MISSILE;    // missile from player
    pbolt.beam_source = MHITYOU;         //    "      "   "
    pbolt.aux_source = NULL;             // additional source info, unused
    pbolt.is_launched = false;
    pbolt.is_thrown = false;

    // fill in the bolt structure
    zappy( ztype, power, pbolt );

    if (ztype == ZAP_LIGHTNING)
    {
        noisy( SL_THUNDER, pbolt.source_x, pbolt.source_y,
               "You hear a mighty clap of thunder!" );
    }

    fire_beam( pbolt );

    return;
}                               // end zapping()

dice_def calc_dice( int num_dice, int max_damage )
{
    dice_def    ret( num_dice, 0 );

    if (num_dice <= 1)
    {
        ret.num  = 1;
        ret.size = max_damage;
    }
    else if (max_damage <= num_dice)
    {
        ret.num  = max_damage;
        ret.size = 1;
    }
    else
    {
        // Divied the damage among the dice, and add one
        // occasionally to make up for the fractions. -- bwr
        ret.size = div_rand_round( max_damage, num_dice );
    }

    return (ret);
}

// *do not* call this function directly (duh - it's static), need to
// see zapping() for default values not set within this function {dlb}
static void zappy( char z_type, int power, struct bolt &pbolt )
{
    int temp_rand = 0;          // probability determination {dlb}

    switch (z_type)
    {
    // level 1
    //
    // This cap is to keep these easy and very cheap spells from
    // becoming too powerful.
    case ZAP_STRIKING:
    case ZAP_MAGIC_DARTS:
    case ZAP_STING:
    case ZAP_ELECTRICITY:
    case ZAP_FLAME_TONGUE:
    case ZAP_SMALL_SANDBLAST:
    case ZAP_DISRUPTION:                // ench_power boosted below
    case ZAP_PAIN:                      // ench_power boosted below
        if (power > 25)
            power = 25;
        break;

    // level 2/3
    //
    // The following examples should make it clear that in the
    // early game this cap is only limiting to serious spellcasters
    case ZAP_SANDBLAST:
    case ZAP_FLAME:             // also ability (pow = lev * 2)
    case ZAP_FROST:             // also ability (pow = lev * 2)
    case ZAP_STONE_ARROW:
        if (power > 50)
            power = 50;
        break;

    // natural/mutant breath/spit powers (power ~= characer level)
    case ZAP_SPIT_POISON:               // lev + mut * 5
    case ZAP_BREATHE_FIRE:              // lev + mut * 4 + 12 (if dragonform)
    case ZAP_BREATHE_FROST:             // lev
    case ZAP_BREATHE_ACID:              // lev (or invoc * 3 from minor destr)
    case ZAP_BREATHE_POISON:            // lev
    case ZAP_BREATHE_POWER:             // lev
    case ZAP_BREATHE_STEAM:             // lev
        if (power > 50)
            power = 50;
        break;

    // enchantments and other resistable effects
    case ZAP_SLOWING:
    case ZAP_HASTING:
    case ZAP_PARALYSIS:
    case ZAP_PETRIFY:
    case ZAP_BACKLIGHT:
    case ZAP_SLEEP:
    case ZAP_CONFUSION:
    case ZAP_INVISIBILITY:
    case ZAP_ENSLAVEMENT:
    case ZAP_TELEPORTATION:
    case ZAP_DIGGING:
    case ZAP_POLYMORPH_OTHER:
    case ZAP_DEGENERATION:
    case ZAP_BANISHMENT:
        // This is the only power that matters.  We magnify it apparently
        // to get values that work better with magic resistance checks...
        // those checks will scale down this value and max it out at 120.
        // pbolt.ench_power *= 3;
        // pbolt.ench_power /= 2;
        break;

    default:
        break;
    }

    // Note:  I'm only displaying the top damage and such here, that's
    // because it's really not been known before (since the above caps
    // didn't exist), so they were all pretty much unlimited before.
    // Also note, that the high end damage occurs at the cap, only
    // players that are that powerful can get that damage... and
    // although these numbers might seem small, you should remember
    // that Dragons in this game are 60-90 hp monsters, and very
    // few monsters have more than 100 hp (and that 1d5 damage is
    // still capable of taking a good sized chunk (and possibly killing)
    // any monster you're likely to meet in the first three levels). -- bwr

    // Note: damage > 100 signals that "random2(damage - 100)" will be
    // applied three times, which not only ups the damage but gives
    // a more normal distribution.
    switch (z_type)
    {
    case ZAP_STRIKING:                                  // cap 25
        strcpy(pbolt.name, "force bolt");
        pbolt.colour = BLACK;
        pbolt.damage = dice_def( 1, 5 );                // dam: 5
        pbolt.hit = 30 + power / 5;
        pbolt.type = SYM_SPACE;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable
        pbolt.obvious_effect = true;
        break;

    case ZAP_MAGIC_DARTS:                               // cap 25
        strcpy(pbolt.name, "magic dart");
        pbolt.colour = LIGHTMAGENTA;
        pbolt.damage = dice_def( 1, 3 + power / 5 );    // 25: 1d8
        pbolt.hit = AUTOMATIC_HIT;                      // hits always
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable
        pbolt.obvious_effect = true;
        break;

    case ZAP_STING:                                     // cap 25
        strcpy(pbolt.name, "sting");
        pbolt.colour = GREEN;
        pbolt.damage = dice_def( 1, 3 + power / 5 );    // 25: 1d8
        pbolt.hit = 30 + power / 5;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_POISON;                    // extra damage

        pbolt.obvious_effect = true;
        break;

    case ZAP_ELECTRICITY:                               // cap 20
        strcpy(pbolt.name, "zap");
        pbolt.colour = LIGHTCYAN;
        pbolt.range = 6 + random2(8);                   // extended in beam
        pbolt.damage = dice_def( 1, 3 + random2(power) / 2 ); // 25: 1d11
        pbolt.hit = 20 + power / 7;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_ELECTRICITY;               // beams & reflects

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_DISRUPTION:                                // cap 25
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_DISINTEGRATION;
        pbolt.damage = dice_def( 1, 4 + power / 5 );    // 25: 1d9
        pbolt.ench_power *= 3;
        pbolt.is_enchant = true;
        break;

    case ZAP_PAIN:                                      // cap 25
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_PAIN;
        pbolt.damage = dice_def( 1, 4 + power / 5 );    // 25: 1d9
        pbolt.ench_power *= 7;
        pbolt.ench_power /= 2;
        pbolt.is_enchant = true;
        break;

    case ZAP_FLAME_TONGUE:                              // cap 25
        strcpy(pbolt.name, "flame");
        pbolt.colour = RED;

        pbolt.rangeMax = 4;
        pbolt.range = 1 + random2(2) + random2(power) / 10;
        if (pbolt.range > 4)
            pbolt.range = 4;

        pbolt.damage = dice_def( 1, 8 + power / 4 );    // 25: 1d14
        pbolt.hit = 50 + power / 5;
        pbolt.type = SYM_BOLT;
        pbolt.flavour = BEAM_FIRE;

        pbolt.obvious_effect = true;
        break;

    case ZAP_SMALL_SANDBLAST:                           // cap 25
        strcpy(pbolt.name, "blast of ");

        temp_rand = random2(4);

        strcat(pbolt.name, (temp_rand == 0) ? "dust" :
                                (temp_rand == 1) ? "dirt" :
                                (temp_rand == 2) ? "grit" : "sand");

        pbolt.colour = element_colour( EC_EARTH );
        pbolt.rangeMax = 2;
        pbolt.range = (random2(power) > random2(30)) ? 2 : 1;
        pbolt.damage = dice_def( 1, 7 + power / 5 );    // 25: 1d12
        pbolt.hit = 40 + power / 5;
        pbolt.type = SYM_BOLT;
        pbolt.flavour = BEAM_FRAG;                      // extra AC resist
        pbolt.is_energy = false;

        pbolt.obvious_effect = true;
        break;

    case ZAP_SANDBLAST:                                 // cap 50
        strcpy(pbolt.name, coinflip() ? "blast of rock" : "rocky blast");
        pbolt.colour = element_colour( EC_EARTH );

        pbolt.rangeMax = 4;
        pbolt.range = 2 + random2(power) / 20;
        if (pbolt.range > 4)
            pbolt.range = 4;

        pbolt.damage = dice_def( 2, 4 + power / 4 );    // 25: 2d10
        pbolt.hit = 80 + power / 5;
        pbolt.type = SYM_BOLT;
        pbolt.flavour = BEAM_FRAG;                      // extra AC resist
        pbolt.is_energy = false;

        pbolt.obvious_effect = true;
        break;

    case ZAP_BONE_SHARDS:
        strcpy(pbolt.name, "spray of bone shards");
        pbolt.colour = LIGHTGREY;
        pbolt.range = 8 + random2(10);

        pbolt.damage = calc_dice( 3, 6 + power / 2 );
        pbolt.hit = 80 + power / 5;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_FRAG;                       // unresisted
        pbolt.is_energy = false;

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_FLAME:                                     // cap 50
        strcpy(pbolt.name, "puff of flame");
        pbolt.colour = RED;
        pbolt.damage = dice_def( 2, 4 + power / 6 );    // 50: 2d12
        pbolt.hit = 60 + power / 5;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_FIRE;

        pbolt.obvious_effect = true;
        break;

    case ZAP_FROST:                                     // cap 50
        strcpy(pbolt.name, "puff of frost");
        pbolt.colour = WHITE;
        pbolt.damage = dice_def( 2, 4 + power / 6 );    // 50: 2d12
        pbolt.hit = 60 + power / 5;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_COLD;

        pbolt.obvious_effect = true;
        break;

    case ZAP_STONE_ARROW:                               // cap 50
        strcpy(pbolt.name, "stone arrow");
        pbolt.colour = LIGHTGREY;
        pbolt.damage = dice_def( 2, 4 + power / 5 );    // 50: 2d14
        pbolt.hit = 20 + power / 2;
        pbolt.type = SYM_MISSILE;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable
        pbolt.is_energy = false;

        pbolt.obvious_effect = true;
        break;

    case ZAP_STICKY_FLAME:
        strcpy(pbolt.name, "sticky flame");             // extra damage
        pbolt.colour = RED;
        pbolt.damage = dice_def( 2, 3 + power / 12 );   // 50: 2d7  100: 2d11
        pbolt.hit = 50 + power / 5;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_NAPALM;

        pbolt.obvious_effect = true;
        break;

    case ZAP_MYSTIC_BLAST:
        strcpy(pbolt.name, "orb of energy");
        pbolt.colour = LIGHTMAGENTA;
        pbolt.damage = calc_dice( 2, 5 + (power * 2) / 5 );
        pbolt.hit = 80 + power / 5;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable
        pbolt.obvious_effect = true;
        break;

    case ZAP_ICE_BOLT:
        strcpy(pbolt.name, "bolt of ice");
        pbolt.colour = WHITE;
        pbolt.damage = calc_dice( 3, 10 + (power * 2) / 5 );
        pbolt.hit = 30 + power;
        pbolt.type = SYM_MISSILE;
        pbolt.flavour = BEAM_ICE;                       // half resistable
        pbolt.is_energy = false;
        break;

    case ZAP_MAGMA:
        strcpy(pbolt.name, "bolt of magma");
        pbolt.colour = RED;
        pbolt.range = 8 + random2(5);
        pbolt.damage = calc_dice( 3, 10 + power / 2 );
        pbolt.hit = 50 + power / 5;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_LAVA;
        pbolt.is_energy = false;

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_DISPEL_UNDEAD:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_DISPEL_UNDEAD;
        pbolt.damage = calc_dice( 3, 20 + (power * 3) / 5 );
        pbolt.ench_power *= 3;
        pbolt.ench_power /= 2;
        pbolt.is_enchant = true;
        break;

    case ZAP_FIRE:
        strcpy(pbolt.name, "bolt of fire");
        pbolt.colour = RED;
        pbolt.range = 8 + random2(10);
        pbolt.damage = calc_dice( 3, 15 + power / 2 );
        pbolt.hit = 80 + power / 5;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_FIRE;

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_COLD:
        strcpy(pbolt.name, "bolt of cold");
        pbolt.colour = WHITE;
        pbolt.range = 8 + random2(10);
        pbolt.damage = calc_dice( 3, 15 + power / 2 );
        pbolt.hit = 80 + power / 5;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_COLD;

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_VENOM_BOLT:
        strcpy(pbolt.name, "bolt of poison");
        pbolt.colour = LIGHTGREEN;
        pbolt.range = 8 + random2(10);
        pbolt.damage = calc_dice( 3, 15 + power / 2 );
        pbolt.hit = 80 + power / 5;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_POISON;                    // extra damage

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_NEGATIVE_ENERGY:
        strcpy(pbolt.name, "bolt of negative energy");
        pbolt.colour = DARKGREY;
        pbolt.range = 8 + random2(10);
        pbolt.damage = calc_dice( 3, 15 + (power * 3) / 5 );
        pbolt.hit = 80 + power / 5;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_NEG;                       // drains levels

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_IRON_BOLT:
        strcpy(pbolt.name, "iron bolt");
        pbolt.colour = LIGHTCYAN;
        pbolt.range = 5 + random2(4);
        pbolt.damage = calc_dice( 4, 15 + (power * 3) / 5 );
        pbolt.hit = 40 + power / 2;
        pbolt.type = SYM_MISSILE;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable
        pbolt.obvious_effect = true;
        pbolt.is_energy = false;
        break;

    case ZAP_POISON_ARROW:
        strcpy(pbolt.name, "poison arrow");
        pbolt.colour = LIGHTGREEN;
        pbolt.range = 8;
        pbolt.damage = calc_dice( 3, 15 + (power * 3) / 5 );
        pbolt.hit = 40 + power;
        pbolt.type = SYM_MISSILE;
        pbolt.flavour = BEAM_POISON_ARROW;              // extra damage
        pbolt.obvious_effect = true;
        pbolt.is_energy = false;
        break;

    case ZAP_DISINTEGRATION:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_DISINTEGRATION;
        pbolt.range = 8;
        pbolt.damage = calc_dice( 3, 15 + (power * 3) / 5 );
        pbolt.ench_power *= 5;
        pbolt.ench_power /= 2;
        pbolt.is_beam = true;
        pbolt.is_enchant = true;
        break;

    case ZAP_LIGHTNING:
        // also for breath (at pow = lev * 2; max dam: 33)
        strcpy(pbolt.name, "bolt of lightning");
        pbolt.colour = LIGHTCYAN;
        pbolt.range = 8 + random2(10);
        pbolt.damage = calc_dice( 2, 15 + power / 2 );
        pbolt.hit = 70 + power / 5;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_ELECTRICITY;               // beams & reflects

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_FIREBALL:
        strcpy(pbolt.name, "fireball");
        pbolt.colour = RED;
        pbolt.range = 7;
        pbolt.damage = calc_dice( 3, 10 + (power * 2) / 5 );
        pbolt.hit = 100;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_FIRE;
        pbolt.is_explosion = true;
        break;

    case ZAP_ORB_OF_ELECTRICITY:
        strcpy(pbolt.name, "orb of electricity");
        pbolt.colour = LIGHTBLUE;
        pbolt.range = 7;
        pbolt.damage = calc_dice( 2, 10 + (power * 3) / 4 );
        pbolt.hit = 100;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_ELECTRICITY;
        pbolt.is_explosion = true;
        break;

    case ZAP_ORB_OF_FRAGMENTATION:
        strcpy(pbolt.name, "metal orb");
        pbolt.colour = CYAN;
        pbolt.range = 7;
        pbolt.damage = calc_dice( 4, 20 + (power * 3) / 4 );
        pbolt.hit = 100;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_FRAG;                      // extra AC resist
        pbolt.is_explosion = true;
        break;

    case ZAP_CLEANSING_FLAME:
        strcpy(pbolt.name, "golden flame");
        pbolt.colour = YELLOW;
        pbolt.range = 7;
        pbolt.damage = calc_dice( 3, 30 + (power * 3) / 4 );
        pbolt.hit = 150;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_HOLY;

        pbolt.obvious_effect = true;
        pbolt.is_explosion = true;
        break;

    case ZAP_CRYSTAL_SPEAR:
        strcpy(pbolt.name, "crystal spear");
        pbolt.colour = WHITE;
        pbolt.range = 8;
        pbolt.damage = calc_dice( 4, 30 + (power * 3) / 4 );
        pbolt.hit = 60 + power;
        pbolt.type = SYM_MISSILE;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable
        pbolt.is_energy = false;

        pbolt.obvious_effect = true;
        break;

    case ZAP_HELLFIRE:
        // also power 50 serpent form breath (unused)
        strcpy(pbolt.name, "hellfire");
        pbolt.colour = RED;
        pbolt.range = 7;
        pbolt.damage = calc_dice( 3, 10 + (power * 3) / 4 );
        pbolt.hit = 100;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_HELLFIRE;

        pbolt.obvious_effect = true;
        pbolt.is_explosion = true;
        break;

    case ZAP_HELLFROST:
        // also power 50 serpent form breath (unused)
        strcpy(pbolt.name, "hellfrost");
        pbolt.colour = WHITE;
        pbolt.range = 7;
        pbolt.damage = calc_dice( 3, 10 + (power * 3) / 4 );
        pbolt.hit = 100;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_HELLFROST;

        pbolt.obvious_effect = true;
        pbolt.is_explosion = true;
        break;

    case ZAP_ICE_STORM:
        strcpy(pbolt.name, "great blast of cold");
        pbolt.colour = BLUE;
        pbolt.range = 8;
        pbolt.damage = calc_dice( 3, 15 + (power * 3) / 5 );
        pbolt.hit = 100;
        pbolt.ench_power = power;                       // used for radius
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_ICE;                       // half resisted
        pbolt.is_explosion = true;
        break;

    case ZAP_BEAM_OF_ENERGY:    // bolt of innacuracy
        strcpy(pbolt.name, "narrow beam of energy");
        pbolt.colour = YELLOW;
        pbolt.range = 8;
        pbolt.damage = calc_dice( 2, 40 + power );
        pbolt.hit = 2;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_ENERGY;                    // unresisted

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_SPIT_POISON:                               // cap 50
        // max pow = 50 (30 from level, 10/15/20 from mutation)
        strcpy(pbolt.name, "splash of poison");
        pbolt.colour = GREEN;

        pbolt.range = 2 + random2(power + 5) / 10;
        pbolt.damage = dice_def( 2, 4 + power / 2 );    // max: 58/38
        pbolt.hit = 30 + power;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_POISON;
        pbolt.obvious_effect = true;
        break;

    case ZAP_BREATHE_FIRE:                              // cap 50
        // max pow = 50 (50 dragon-form OR 30 level, 10/15/20 mutation)
        strcpy(pbolt.name, "fiery breath");
        pbolt.colour = RED;

        pbolt.range = 2 + random2(power + 5) / 10;
        pbolt.damage = dice_def( 3, 4 + power / 3 );    // max: 60/42
        pbolt.hit = 30 + power;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_FIRE;

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_BREATHE_FROST:                             // cap 50
        // max power = 30 (level)
        strcpy(pbolt.name, "freezing breath");
        pbolt.colour = WHITE;
        pbolt.range = 2 + random2(power + 5) / 10;
        pbolt.damage = dice_def( 3, 4 + power / 3 );    // max: 42
        pbolt.hit = 30 + power;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_COLD;

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_BREATHE_LIGHTNING:                             // cap 50
        // max power = 30 (level)
        strcpy(pbolt.name, "bolt of lightning");
        pbolt.colour = LIGHTCYAN;
        pbolt.range = 2 + random2(power + 5) / 5;
        pbolt.damage = dice_def( 2, 5 + power / 2 );        // max: 40
        pbolt.hit = 30 + power;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_ELECTRICITY;               // beams & reflects

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_BREATHE_ACID:                              // cap 50
        // max power = 30 for ability, 50 for minor destruction (max dam: 57)
        strcpy(pbolt.name, "acid");
        pbolt.colour = YELLOW;

        pbolt.range = 2 + random2(power + 5) / 10;
        pbolt.damage = dice_def( 3, 3 + (power + 2) / 4 );   // max: 48/33
        pbolt.hit = 30 + power;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_ACID;

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_BREATHE_POISON:                                // cap 50
        // max power = 30 (level)
        strcpy(pbolt.name, "poison gas");          // leaves poison clouds
        pbolt.colour = GREEN;

        pbolt.range = 2 + random2(power + 5) / 10;
        pbolt.damage = dice_def( 3, 3 + power / 3 );    // dam: 39
        pbolt.hit = 40 + power;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_POISON;

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_BREATHE_STICKY_FLAME:                              // cap 50
        // max power = 30 (level)
        strcpy(pbolt.name, "sticky flame");        // extra damage
        pbolt.colour = RED;

        pbolt.range = 2 + random2(power + 5) / 10;
        pbolt.damage = dice_def( 3, 2 + power / 6 );    // max: 21
        pbolt.hit = 35 + power / 2;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_NAPALM;

        pbolt.obvious_effect = true;
        break;

    case ZAP_BREATHE_POWER:                             // cap 50
        // max power = 30 (level)
        strcpy(pbolt.name, "bolt of energy");

        pbolt.colour = BLUE;
        if (random2(power) >= 8)
            pbolt.colour = LIGHTBLUE;
        if (random2(power) >= 12)
            pbolt.colour = MAGENTA;
        if (random2(power) >= 17)
            pbolt.colour = LIGHTMAGENTA;

        pbolt.range = 2 + random2(power + 5) / 10;
        pbolt.damage = dice_def( 3, 3 + (power + 2) / 4 ); // max dam: 33
        pbolt.hit = 35 + power / 2;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_BREATHE_STEAM:                             // cap 50
        // max power = 30 (level)
        strcpy(pbolt.name, "ball of steam");       // leaves steam clouds
        pbolt.colour = LIGHTGREY;

        pbolt.range = 4 + random2(power + 5) / 10;
        pbolt.damage = dice_def( 3, 3 + power / 3 );    // max dam: 39
        pbolt.hit = 50 + power;
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_STEAM;

        pbolt.obvious_effect = true;
        pbolt.is_beam = true;
        break;

    case ZAP_SLOWING:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_SLOW;
        pbolt.is_enchant = true;
        break;

    case ZAP_HASTING:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_HASTE;
        pbolt.is_enchant = true;
        break;

    case ZAP_PARALYSIS:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_PARALYSIS;
        pbolt.is_enchant = true;
        break;

    case ZAP_PETRIFY:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_PETRIFY;
        pbolt.is_enchant = true;
        break;

    case ZAP_CONFUSION:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_CONFUSION;
        pbolt.is_enchant = true;
        break;

    case ZAP_INVISIBILITY:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_INVISIBILITY;
        pbolt.is_enchant = true;
        break;

    case ZAP_HEALING:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_HEALING;
        pbolt.damage = dice_def( 1, 7 + power / 3 );
        pbolt.is_enchant = true;
        break;

    case ZAP_DIGGING:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_DIGGING;
        pbolt.range = 3 + random2( power / 5 ) + random2(5);
        pbolt.rangeMax = 24;
        pbolt.is_beam = true;
        pbolt.is_enchant = true;
        break;

    case ZAP_TELEPORTATION:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_TELEPORT;
        pbolt.is_enchant = true;
        break;

    case ZAP_POLYMORPH_OTHER:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_POLYMORPH;
        pbolt.is_enchant = true;
        break;

    case ZAP_ENSLAVEMENT:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_CHARM;
        pbolt.is_enchant = true;
        break;

    case ZAP_BANISHMENT:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_BANISH;
        pbolt.is_enchant = true;
        break;

    case ZAP_DEGENERATION:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_DEGENERATE;
        pbolt.is_enchant = true;
        break;

    case ZAP_ENSLAVE_UNDEAD:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_ENSLAVE_UNDEAD;
        pbolt.is_enchant = true;
        break;

    case ZAP_AGONY:
        strcpy(pbolt.name, "agony");
        pbolt.flavour = BEAM_PAIN;
        pbolt.ench_power *= 5;
        pbolt.is_enchant = true;
        break;

    case ZAP_CONTROL_DEMON:
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_ENSLAVE_DEMON;
        pbolt.ench_power *= 3;
        pbolt.ench_power /= 2;
        pbolt.is_enchant = true;
        break;

    case ZAP_SLEEP:             //jmf: added
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_SLEEP;
        pbolt.is_enchant = true;
        break;

    case ZAP_BACKLIGHT: //jmf: added
        pbolt.name[0] = '\0';
        pbolt.flavour = BEAM_BACKLIGHT;
        pbolt.colour = BLUE;
        pbolt.is_enchant = true;
        break;

    case ZAP_DEBUGGING_RAY:
        strcpy( pbolt.name, "debugging ray" );
        pbolt.colour = element_colour( EC_RANDOM );
        pbolt.damage = dice_def( 1500, 1 );             // dam: 1500
        pbolt.hit = AUTOMATIC_HIT;
        pbolt.type = SYM_DEBUG;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable

        pbolt.obvious_effect = true;
        break;

    default:
        strcpy(pbolt.name, "buggy beam");
        pbolt.colour = element_colour( EC_RANDOM );
        pbolt.damage = dice_def( 1, 0 );
        pbolt.hit = 60;
        pbolt.type = SYM_DEBUG;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable

        pbolt.obvious_effect = true;
        break;
    }                           // end of switch
}                               // end zappy()

/*  NEW (GDL):
 *  Now handles all beamed/thrown items and spells,  tracers, and their effects.
 *  item is used for items actually thrown/launched
 *
 *  if item is NULL,  there is no physical object being thrown that could
 *  land on the ground.
 */


/*
 * Beam pseudo code:
 *
 * 1. Calculate stepx and stepy - algorithms depend on finding a step axis
 *    which results in a line of rise 1 or less (ie 45 degrees or less)
 * 2. Calculate range.  Tracers always have max range, otherwise the beam
 *    will have somewhere up to rangeMax, but limited by range and num hits
 * 3. Loop tracing out the line:
 *      3a. Check for walls and wall affecting beams
 *      3b. If no valid move is found, try a fuzzy move
 *      3c. If no valid move is yet found, try bouncing
 *      3d. If no valid move or bounce is found, break
 *      4. Check for beam termination on target
 *      5. Affect the cell which the beam just moved into -> affect()
 *      6. Decrease remaining range appropriately
 *      7. Check for early out due to aimed_at_feet
 *      8. Draw the beam
 * 9. Drop an object where the beam 'landed'
 *10. Beams explode where the beam 'landed'
 *11. If no message generated yet, send "nothing happens" (enchantments only)
 *
 */


void fire_beam( struct bolt &pbolt, item_def *const item )
{
    int lx, ly;             // last affected x,y
    int stepx, stepy;       // x,y increment - FP
    int wx, wy;             // 'working' x,y - FP
    bool beamTerminate;     // has beam been 'stopped' by something?
    int nx, ny;             // test(new) x,y - FP
    int tx, ty;             // test(new) x,y - integer
    bool roundX, roundY;    // which to round?
    int rangeRemaining;
    bool fuzzyOK;           // fuzzification resulted in OK move
    bool sideBlocked, topBlocked, random_beam;
    int dist_trav;

#if DEBUG_DIAGNOSTICS
    // LoS traces are too annoying in some cases.
    if (pbolt.flavour != BEAM_LINE_OF_SIGHT)
    {
        mpr( MSGCH_DIAGNOSTICS,
             "%s%s%s (%d,%d) to (%d,%d): sym=%d col=%d flav=%d pow=%d hit=%d dam=%dd%d laun=%d",
                 (pbolt.is_beam) ? "beam" : "missile",
                 (pbolt.is_explosion) ? "*" :
                 (pbolt.is_big_cloud) ? "+" : "",
                 (pbolt.is_tracer) ? " tracer" : "",
                 pbolt.source_x, pbolt.source_y,
                 pbolt.target_x, pbolt.target_y,
                 pbolt.type, pbolt.colour, pbolt.flavour, pbolt.ench_power,
                 pbolt.hit, pbolt.damage.num, pbolt.damage.size, pbolt.is_launched );
    }
#endif

    // init
    pbolt.aimed_at_feet = false;
    pbolt.msg_generated = false;
    roundY = false;
    roundX = false;

    // first, calculate beam step
    const int dx = pbolt.target_x - pbolt.source_x;
    const int dy = pbolt.target_y - pbolt.source_y;

    const int  ax = abs(dx);
    const int  ay = abs(dy);
    const bool cardinal = (dx == 0 || dy == 0 || ax == ay);

    // check for aim at feet
    if (dx == 0 && dy == 0)
    {
        pbolt.aimed_at_feet = true;
        stepx = 0;
        stepy = 0;
        tx = pbolt.source_x;
        ty = pbolt.source_y;
    }
    else
    {
        if (ax >= ay)
        {
            stepx = (dx > 0) ? 100 : -100;
            stepy = (100 * dy) / ax;
            roundY = true;
        }
        else
        {
            stepy = (dy > 0) ? 100 : -100;
            stepx = (100 * dx) / ay;
            roundX = true;
        }
    }

    // give chance for beam to affect one cell even if aimed_at_feet.
    beamTerminate = false;

    // setup working coords
    lx = pbolt.source_x;
    wx = 100 * lx;
    ly = pbolt.source_y;
    wy = 100 * ly;

    // setup range
    rangeRemaining = pbolt.range;

    // before we start drawing the beam, turn buffering off
#ifdef WIN32CONSOLE
    bool oldValue = true;
    if (!pbolt.is_tracer)
        oldValue = setBuffering(false);
#endif

    // cannot use source_x, source_y, target_x, target_y during
    // step algorithm due to bouncing.
    dist_trav = 0;

    // now, one step at a time, try to move towards target.
    while (!beamTerminate)
    {
        nx = wx + stepx;
        ny = wy + stepy;

        if (roundY)
        {
            tx = nx / 100;
            ty = (ny + 50) / 100;
        }

        if (roundX)
        {
            ty = ny / 100;
            tx = (nx + 50) / 100;
        }

        // check that tx, ty are valid.  If not,  set to last
        // x,y and break.
        if (!in_bounds( tx, ty ))
        {
            tx = lx;
            ty = ly;
            break;
        }

        // see if tx, ty is blocked by something
        if (grid_is_solid( grd[tx][ty] ))
        {
            // first, check to see if this beam affects walls.
            if (affectsWalls( pbolt ))
            {
                // should we ever get a tracer with a wall-affecting
                // beam (possible I suppose),  we'll quit tracing now.
                if (!pbolt.is_tracer)
                    rangeRemaining -= affect( pbolt, tx, ty );

                // if it's still a wall, quit.
                if (grid_is_solid( grd[tx][ty] ))
                    break;      // breaks from line tracing
            }
            else if (!cardinal)
            {
                // BEGIN fuzzy line algorithm... but not for the eight
                // cardinal directions where it doesn't make sense (there
                // isn't a floor/ceiling pair of lines to choose from for
                // direct orthagonal or diagonal beams)... in fact, for
                // diagonals allowing fuzzyLine is downright buggy and
                // allows for shots to pass through walls or skip squares.
                fuzzyOK = fuzzyLine(nx,ny,tx,ty,lx,ly,stepx,stepy,roundX,roundY);
                if (!fuzzyOK)
                {
                    // BEGIN bounce case
                    if (!isBouncy(pbolt))
                    {
                        tx = lx;
                        ty = ly;
                        break;          // breaks from line tracing
                    }

                    sideBlocked = false;
                    topBlocked = false;

                    // BOUNCE -- guaranteed to return reasonable tx, ty.
                    // if it doesn't, we'll quit in the next if stmt anyway.
                    if (roundY)
                    {
                        if (grid_is_solid( grd[lx + stepx / 100][ly] ))
                            sideBlocked = true;

                        if (dy != 0)
                        {
                            if (grid_is_solid( grd[lx][ly + ((stepy>0)?1:-1)] ))
                                topBlocked = true;
                        }

                        dist_trav = 0;
                        rangeRemaining -= bounce(stepx, stepy, wx, wy, nx, ny,
                            lx, ly, tx, ty, topBlocked, sideBlocked);
                    }
                    else
                    {
                        if (grid_is_solid( grd[lx][ly + stepy / 100] ))
                            sideBlocked = true;

                        if (dx != 0)
                        {
                            if (grid_is_solid( grd[lx + ((stepx>0)?1:-1)][ly] ))
                                topBlocked = true;
                        }

                        dist_trav = 0;
                        rangeRemaining -= bounce(stepy, stepx, wy, wx, ny, nx,
                            ly, lx, ty, tx, topBlocked, sideBlocked);
                    }
                    // END bounce case - range check

                    if (rangeRemaining < 1)
                    {
                        tx = lx;
                        ty = ly;
                        break;
                    }
                }
            } // end else - beam doesn't affect walls
        } // endif - is tx, ty wall?

        // at this point, if grd[tx][ty] is still a wall, we
        // couldn't find any path: bouncy, fuzzy, or not - so break.
        if (grid_is_solid( grd[tx][ty] ))
        {
            tx = lx;
            ty = ly;
            break;
        }

        // check for "target termination"
        // occurs when beam can be targetted at empty
        // cell (e.g. a mage wants an explosion to happen
        // between two monsters)

        // in this case,  don't affect the cell - players
        // /monsters have no chance to dodge or block such
        // a beam, and we want to avoid silly messages.
        if (tx == pbolt.target_x && ty == pbolt.target_y)
            beamTerminate = beam_term_on_target( pbolt );

        // affect the cell,  except in the special case noted
        // above -- affect() will early out if something gets
        // hit and the beam is type 'term on target'.  Note that
        // we don't allow explosions to affect the target here
        // (since that would give a double hit), but we do allow
        // big_cloud terminals to affect the target (so that the
        // target actually gets affected).
        if (!beamTerminate || !pbolt.is_explosion)
        {
            // random beams: randomize before affect
            random_beam = false;
            if (pbolt.flavour == BEAM_RANDOM)
            {
                random_beam = true;
                pbolt.flavour = BEAM_FIRE + random2(7);
            }

            rangeRemaining -= affect( pbolt, tx, ty );

            if (random_beam)
                pbolt.flavour = BEAM_RANDOM;
        }

        // always decrease range by 1
        rangeRemaining -= 1;
        dist_trav += 1;

        // check for range termination
        if (rangeRemaining <= 0 || dist_trav >= pbolt.rangeMax)
            beamTerminate = true;

        // special case - beam was aimed at feet
        if (pbolt.aimed_at_feet)
            beamTerminate = true;

        // This is a bit of kludgy paranoia... if we ever get in a situation
        // where the move ends up jumping, we stop.  This can theoretically
        // happen because of the fuzzy code (and in fact does happen if
        // we allow diagonals to use the fuzzyLine code).  Stopping here
        // shouldn't be a problem because it's trying to navigate the
        // "correct" side of a so9lid tile that gets us here... in these
        // rare cases, we'll assume it's tricky enough that the answer is no.
        if (grid_distance( lx, ly, tx, ty ) != 1)
        {
            tx = lx;
            ty = ly;
            break;
        }

        // actually draw the beam/missile/whatever,
        // if the player can see the cell.
        if (!pbolt.is_tracer && !pbolt.is_enchant && see_grid( tx,ty ))
        {
            // we don't clean up the old position.
            // first, most people like seeing the full path,
            // and second, it is hard to do it right with
            // respect to killed monsters, cloud trails, etc.

            // draw new position
            int drawx = tx - you.x_pos + 18;
            int drawy = ty - you.y_pos + 9;

            // bounds check
            if (drawx > 8 && drawx < 26 && drawy > 0 && drawy < 18)
            {
                if (pbolt.colour == BLACK)
                    textcolor( element_colour( EC_RANDOM ) );
                else
                    textcolor( pbolt.colour );

                gotoxy( drawx, drawy );
                putch( pbolt.type );

#ifdef LINUX
                // get curses to update the screen so we can see the beam
                update_screen();
#endif

                delay(15);

#ifdef MISSILE_TRAILS_OFF
                if (!pbolt.is_beam)
                    viewwindow(true,false); // mv: added. It's not optimal but
                                         // is usually enough
#endif
            }
        }

        // set some stuff up for the next iteration
        lx = tx;
        ly = ty;

        wx = nx;
        wy = ny;

    } // end- while !beamTerminate

    // the beam has finished,  and terminated at tx, ty

    // leave an object, if applicable
    if (item)
        beam_drop_object( pbolt, item, tx, ty );

    // check for explosion.  NOTE that for tracers, we have to make a copy
    // of target co'ords and then reset after calling this -- tracers should
    // never change any non-tracers fields in the beam structure. -- GDL
    int ox = pbolt.target_x;
    int oy = pbolt.target_y;

    beam_explodes( pbolt, tx, ty );

    if (pbolt.is_tracer)
    {
        pbolt.target_x = ox;
        pbolt.target_y = oy;
    }

    // canned msg for enchantments that affected no-one
    if (pbolt.is_enchant
        && pbolt.flavour != BEAM_DIGGING
        && pbolt.beam_source == MHITYOU
        && !pbolt.is_tracer
        && !pbolt.msg_generated
        && !pbolt.obvious_effect)
    {
        canned_msg( MSG_NOTHING_HAPPENS );
    }

    // that's it!
#ifdef WIN32CONSOLE
    if (!pbolt.is_tracer)
        setBuffering( oldValue );
#endif
}                               // end fire_beam();


// returns damage taken by a monster from a "flavoured" (fire, ice, etc.)
// attack -- damage from clouds and branded weapons handled elsewhere.
int mons_adjust_flavoured( struct monsters *monster, struct bolt &pbolt,
                           int hurted, bool doFlavouredEffects )
{
    // if we're not doing flavored effects,  must be preliminary
    // damage check only;  do not print messages or apply any side
    // effects!
    int resist;
    int flav = pbolt.flavour;

    // these two are subtypes of fire
    if (pbolt.flavour == BEAM_NAPALM)
    {
        if (monster->type == MONS_MOTTLED_DRAGON)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " appears unharmed.");

            return (0);
        }

        flav = BEAM_FIRE;
    }
    else if (pbolt.flavour == BEAM_STEAM)
    {
        if (monster->type == MONS_STEAM_DRAGON)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " appears unharmed.");

            return (0);
        }

        flav = BEAM_FIRE;
    }

    switch (flav)
    {
    case BEAM_FIRE:
        resist = mons_res_fire(monster);
        if (resist > 1)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " appears unharmed.");

            hurted = 0;
        }
        else if (resist == 1)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " resists.");

            hurted /= 3;
        }
        else if (resist < 0)
        {
            if (monster->type == MONS_ICE_BEAST
                || monster->type == MONS_SIMULACRUM_SMALL
                || monster->type == MONS_SIMULACRUM_LARGE)
            {
                if (doFlavouredEffects)
                    mon_msg(monster, " melts!");
            }
            else
            {
                if (doFlavouredEffects)
                    mon_msg(monster, " is burned terribly!");
            }

            hurted *= 15;
            hurted /= 10;
        }
        break;


    case BEAM_COLD:
        resist = mons_res_cold(monster);
        if (resist > 1)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " appears unharmed.");

            hurted = 0;
        }
        else if (resist == 1)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " resists.");

            hurted /= 3;
        }
        else if (resist < 0)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " is frozen!");

            hurted *= 15;
            hurted /= 10;
        }
        break;

    case BEAM_ELECTRICITY:
        if (mons_res_elec(monster) > 0)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " appears unharmed.");

            hurted = 0;
        }
        break;


    case BEAM_POISON:
        if (mons_res_poison(monster) > 0)
        {
            if (doFlavouredEffects)
                mon_msg( monster, " appears unharmed." );

            hurted = 0;
        }
        else if (doFlavouredEffects && !one_chance_in(3))
        {
            poison_monster( monster, YOU_KILL(pbolt.thrower) );
        }
        break;

    case BEAM_POISON_ARROW:
        if (mons_res_poison(monster) > 0)
        {
            if (doFlavouredEffects)
            {
                mon_msg( monster, " partially resists." );

                // Poison arrow can poison any living thing regardless of
                // poison resistance. -- bwr
                if (mons_has_lifeforce( monster ))
                    poison_monster( monster, YOU_KILL(pbolt.thrower), 2, true );
            }

            hurted /= 2;
        }
        else if (doFlavouredEffects)
        {
            poison_monster( monster, YOU_KILL(pbolt.thrower), 4 );
        }
        break;

    case BEAM_NEG:
        if (mons_res_negative_energy(monster) > 0)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " appears unharmed.");

            hurted = 0;
        }
        else
        {
            // early out for tracer/no side effects
            if (!doFlavouredEffects)
                return (hurted);

            drain_monster( monster );

            if (monster->hit_dice < 1)
                monster->hit_points = 0;
        }                       // end else
        break;

    case BEAM_MIASMA:
        if (mons_res_negative_energy( monster ) >= 3)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " appears unharmed.");

            hurted = 0;
        }
        else
        {
            // early out for tracer/no side effects
            if (!doFlavouredEffects)
                return (hurted);

            if (mons_res_poison( monster ) <= 0)
                poison_monster( monster, YOU_KILL(pbolt.thrower) );

            if (one_chance_in( 3 + 2 * mons_res_negative_energy(monster) ))
            {
                struct bolt beam;
                beam.flavour = BEAM_SLOW;
                mons_ench_f2( monster, beam );
            }
        }
        break;

    case BEAM_HOLY:             // flame of cleansing
        if (mons_is_unholy( monster ))
        {
            if (doFlavouredEffects)
                mon_msg( monster, " writhes in agony!" );

            hurted = (hurted * 3) / 2;
        }
        else if (!mons_is_evil( monster ))
        {
            if (doFlavouredEffects)
                mon_msg( monster, " appears unharmed." );

            hurted = 0;
        }
        break;

    case BEAM_ACID:
        if (mons_res_acid( monster ))
        {
            if (doFlavouredEffects)
                mon_msg(monster, " appears unharmed.");

            hurted = 0;
        }
        break;

    case BEAM_ICE:
        /* ice - about 50% of damage is cold, other 50% is impact and
           can't be resisted (except by AC, of course) */
        resist = mons_res_cold(monster);
        if (resist > 0)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " partially resists.");

            hurted /= 2;
        }
        else if (resist < 0)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " is frozen!");

            hurted *= 13;
            hurted /= 10;
        }
        break;
    }                           /* end of switch */

    if (flav == BEAM_LAVA)    //jmf: lava != hellfire
    {
        resist = mons_res_fire(monster);
        if (resist > 0)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " partially resists.");

            hurted /= 2;
        }
        else if (resist < 0)
        {
            if (monster->type == MONS_ICE_BEAST
                || monster->type == MONS_SIMULACRUM_SMALL
                || monster->type == MONS_SIMULACRUM_LARGE)
            {
                if (doFlavouredEffects)
                    mon_msg(monster, " melts!");
            }
            else
            {
                if (doFlavouredEffects)
                    mon_msg(monster, " is burned terribly!");
            }

            hurted *= 12;
            hurted /= 10;
        }
    }
    else if (stricmp(pbolt.name, "hellfire") == 0)
    {
        resist = mons_res_fire(monster);
        if (resist > 2)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " appears unharmed.");

            hurted = 0;
        }
        else if (resist > 0)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " partially resists.");

            hurted /= 2;
        }
        else if (resist < 0)
        {
            if (monster->type == MONS_ICE_BEAST
                || monster->type == MONS_SIMULACRUM_SMALL
                || monster->type == MONS_SIMULACRUM_LARGE)
            {
                if (doFlavouredEffects)
                    mon_msg(monster, " melts!");
            }
            else
            {
                if (doFlavouredEffects)
                    mon_msg(monster, " is burned terribly!");
            }

            hurted *= 12;       /* hellfire */
            hurted /= 10;
        }
    }
    else if (stricmp(pbolt.name, "hellfrost") == 0)
    {
        resist = mons_res_cold(monster);
        if (resist > 2)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " appears unharmed.");

            hurted = 0;
        }
        else if (resist > 0)
        {
            if (doFlavouredEffects)
                mon_msg(monster, " partially resists.");

            hurted /= 2;
        }
        else if (resist < 0)
        {
            if (doFlavouredEffects)
                mon_msg( monster, " is frozen!" );

            hurted *= 12;       /* hellfire */
            hurted /= 10;
        }
    }

    return ((hurted < 0) ? 0 : hurted);
}                               // end mons_adjust_flavoured()


// Enchants all monsters in player's sight.
bool mass_enchantment( enchant_type ench, int pow, int origin, bool show_resists )
{
    // as these are the only ones this code is supporting, we'll assert them:
    ASSERT( ench == ENCH_FEAR || ench == ENCH_CHARM || ench == ENCH_CONFUSION );

    bool msg_generated = false;

    viewwindow( false, false );

    for (int i = 0; i < MAX_MONSTERS; i++)
    {
        struct monsters *const mon = &menv[i];

        if (mon->type == -1 || !mons_near( mon ))
            continue;

        // assuming that the only mass charm is control undead:
        if (ench == ENCH_CHARM)
        {
            if (mons_friendly(mon))
                continue;

            if (mons_holiness( mon ) != MH_UNDEAD)
                continue;

            if (check_mons_resist_magic( mon, pow ))
            {
                if (show_resists)
                {
                    mon_msg( mon, " resists." );
                    msg_generated = false;
                }
                continue;
            }
        }
        else if (mons_holiness( mon ) == MH_NATURAL)
        {
            if (check_mons_resist_magic( mon, pow ))
            {
                if (show_resists)
                {
                    mon_msg( mon, " resists." );
                    msg_generated = false;
                }
                continue;
            }
        }
        else  // trying to enchant an unnatural creature doesn't work
        {
            if (show_resists)
            {
                mon_msg( mon, " is unaffected." );
                msg_generated = false;
            }
            continue;
        }

        if (mons_has_ench( mon, ench ))
        {
            if (show_resists)
            {
                msg_generated = true;
                if (ench == ENCH_FEAR)
                    mon_msg( mon, " is already frightened." );
                else if (ench == ENCH_CONFUSION)
                    mon_msg( mon, " is already confused." );
                else
                    msg_generated = false;
            }
            continue;
        }

        if (mons_add_ench( mon, ench, origin ))
        {
            if (player_monster_visible( mon ))
            {
                // turn message on
                msg_generated = true;
                switch (ench)
                {
                case ENCH_FEAR:
                    mon_msg( mon, " looks frightened!" );
                    break;
                case ENCH_CONFUSION:
                    mon_msg( mon, " looks rather confused." );
                    break;
                case ENCH_CHARM:
                    mon_msg( mon, " submits to your will." );
                    break;
                default:
                    msg_generated = false; // oops, I guess not!
                    break;
                }
            }
        }
    }                           // end "for i"

    if (!msg_generated)
        canned_msg( MSG_NOTHING_HAPPENS );

    return (msg_generated);
}                               // end mass_enchantmenet()

/*
   Monster has probably failed save, now it gets enchanted somehow.

   returns MON_RESIST if monster is unaffected due to magic resist.
   returns MON_UNAFFECTED if monster is immune to enchantment
   returns MON_AFFECTED in all other cases (already enchanted, etc)
 */
int mons_ench_f2(struct monsters *monster, struct bolt &pbolt)
{
    bool is_near = mons_near(monster);  // single caluclation permissible {dlb}
    char buff[ ITEMNAME_SIZE ];

    bool had_condition = false;         // used for messages

    switch (pbolt.flavour)      /* put in magic resistance */
    {
    case BEAM_SLOW:         /* 0 = slow monster */
        // try to remove haste,  if monster is hasted
        if (mons_del_ench( monster, ENCH_HASTE ))
        {
            if (mon_msg(monster, " is no longer moving quickly."))
                pbolt.obvious_effect = true;

            return (MON_AFFECTED);
        }

        had_condition = mons_has_ench( monster, ENCH_SLOW );

        // not hasted,  slow it
        if (mons_add_ench( monster, ENCH_SLOW ) && !had_condition)
        {
            if (mon_msg(monster, " seems to slow down."))
                pbolt.obvious_effect = true;
        }
        return (MON_AFFECTED);

    case BEAM_HASTE:                  // 1 = haste
        if (mons_del_ench( monster, ENCH_SLOW ))
        {
            if (mon_msg(monster, " is no longer moving slowly."))
                pbolt.obvious_effect = true;

            return (MON_AFFECTED);
        }

        had_condition = mons_has_ench( monster, ENCH_HASTE );

        // not slowed, haste it
        if (mons_add_ench( monster, ENCH_HASTE ) && !had_condition)
        {
            if (mon_msg(monster, " seems to speed up."))
                pbolt.obvious_effect = true;
        }
        return (MON_AFFECTED);

    case BEAM_HEALING:         /* 2 = healing */
        if (monster->hit_points < monster->max_hit_points
            && heal_monster( monster, 5 + roll_dice( pbolt.damage ), false ))
        {
            if (monster->hit_points == monster->max_hit_points)
            {
                if (mon_msg( monster, "'s wounds heal themselves!" ))
                    pbolt.obvious_effect = true;
            }
            else
            {
                if (mon_msg(monster, " is healed somewhat."))
                    pbolt.obvious_effect = true;
            }
        }
        return (MON_AFFECTED);

    case BEAM_PARALYSIS:                  /* 3 = paralysis */
    case BEAM_PETRIFY:
        if (!mons_has_ench( monster, ENCH_PARALYSIS )
            && !mons_has_ench( monster, ENCH_PETRIFY ))
        {
            bool obv = false;

            if (pbolt.flavour == BEAM_PARALYSIS)
            {
                mons_add_ench( monster, ENCH_PARALYSIS );
                obv = mon_msg( monster, " suddenly stops moving!" );
            }
            else
            {
                mons_add_ench( monster, ENCH_PETRIFY );
                obv = mon_msg( monster, " suddenly turns to stone!" );
            }

            pbolt.obvious_effect = obv;

            if (grd[monster->x][monster->y] == DNGN_LAVA
                || grid_is_water( grd[monster->x][monster->y] ))
            {
                if (mons_flies(monster) == FLY_POWERED)
                {
                    // don't worry about invisibility - you should be able to
                    // see if something has fallen into the lava
                    if (is_near)
                    {
                        strcpy(info, ptr_monam(monster, DESC_CAP_THE));
                        strcat(info, " falls into the ");
                        strcat(info, (grd[monster->x][monster->y] == DNGN_LAVA)
                                     ?  "lava" : "water");
                        strcat(info, "!");
                        mpr(info);
                    }

                    if (grd[monster->x][monster->y] == DNGN_SHALLOW_WATER)
                        mpr( "Splash!" );
                    else
                    {
                        switch (pbolt.thrower)
                        {
                        case KILL_YOU:
                        case KILL_YOU_MISSILE:
                            monster_die(monster, KILL_YOU, pbolt.beam_source);
                            break;      /*  "    " */

                        case KILL_MON:
                        case KILL_MON_MISSILE:
                            monster_die(monster, KILL_MON_MISSILE, pbolt.beam_source);
                            break;      /* dragon breath &c */
                        }
                    }
                }
            }
        }
        return (MON_AFFECTED);

    case BEAM_CONFUSION:                   /* 4 = confusion */
        had_condition = mons_is_confused( monster );

        if (mons_add_ench( monster, ENCH_CONFUSION ) && !had_condition)
        {
            // put in an exception for fungi, plants and other things you won't
            // notice becoming confused.
            if (mon_msg(monster, " appears confused."))
                pbolt.obvious_effect = true;
        }
        return (MON_AFFECTED);

    case BEAM_INVISIBILITY:               /* 5 = invisibility */
        // Store the monster name before it becomes an "it" -- bwr
        strncpy( buff, ptr_monam( monster, DESC_CAP_THE ), sizeof(buff) );

        had_condition = mons_is_invisible( monster );
        if (mons_add_ench( monster, ENCH_INVIS ) && !had_condition)
        {
            if (mons_near( monster ))
            {
                mpr( "%s flickers %s", buff,
                         player_see_invis() ? "for a moment."
                                            : "and vanishes!" );
            }

            pbolt.obvious_effect = true;
        }
        return (MON_AFFECTED);

    case BEAM_CHARM:             /* 9 = charm */
        had_condition = mons_has_ench( monster, ENCH_CHARM );

        if (mons_add_ench( monster, ENCH_CHARM ) && !had_condition)
        {
            // put in an exception for fungi, plants and other things you won't
            // notice becoming charmed.
            if (mon_msg(monster, " is charmed."))
                pbolt.obvious_effect = true;
        }
        return (MON_AFFECTED);

    default:
        break;
    }                           /* end of switch (beam_colour) */

    return (MON_AFFECTED);
}                               // end mons_ench_f2()

// actually poisons a monster (w/ message)
void poison_monster( struct monsters *mon, bool fromPlayer, int levels,
                     bool force )
{
    int source = (fromPlayer) ? MHITYOU : MHITNOT;

    if (mon->type == -1)
        return;

    if (!force && mons_res_poison(mon) > 0)
        return;

    // actually do the poisoning
    enchant_retval rval = mons_inc_ench_levels( mon, ENCH_POISON, source, levels );

    if (rval == ERV_NEW)
        mon_msg( mon, " looks ill." );
    else if (rval == ERV_INCREASED)
        mon_msg( mon, " looks even sicker." );

    // finally, take care of deity preferences
    if (fromPlayer && rval != ERV_FAIL)
        did_god_conduct( DID_POISON, 5 + random2(3) );
}                               // end poison_monster()

// actually napalms a monster (w/ message)
void sticky_flame_monster( int mn, bool fromPlayer, int levels )
{
    int source = (fromPlayer) ? MHITYOU : MHITNOT;
    struct monsters *const mon = &menv[mn];

    if (mon->type == -1 || mon->type == MONS_MOTTLED_DRAGON)
        return;

    levels -= mons_res_fire( mon );
    if (levels < 1)
        return;

    // actually do the sticky flame
    enchant_retval rval = mons_inc_ench_levels( mon, ENCH_STICKY_FLAME,
                                                source, levels );

    if (rval != ERV_FAIL)
        mon_msg( mon, " is covered in liquid fire!" );
}                               // end sticky_flame_monster

/*
 * Used by monsters in "planning" which spell to cast. Fires off a "tracer"
 * which tells the monster what it'll hit if it breathes/casts etc.
 *
 * The output from this tracer function is four variables in the beam struct:
 * fr_count, foe_count: a count of how many friends and foes will (probably)
 * be hit by this beam
 * fr_power, foe_power: a measure of how many 'friendly' hit dice it will
 *   affect,  and how many 'unfriendly' hit dice.
 *
 * note that beam properties must be set,  as the tracer will take them
 * into account,  as well as the monster's intelligence.
 *
 */
void fire_tracer(struct monsters *monster, struct bolt &pbolt)
{
    // don't fiddle with any input parameters other than tracer stuff!
    pbolt.is_tracer = true;
    pbolt.source_x = monster->x;    // always safe to do.
    pbolt.source_y = monster->y;
    pbolt.beam_source = monster_index(monster);
    pbolt.can_see_invis = (mons_see_invis(monster) != 0);
    pbolt.smart_monster = (mons_intel(monster->type) == I_HIGH ||
                          mons_intel(monster->type) == I_NORMAL);
    pbolt.is_friendly = mons_friendly(monster);

    // init tracer variables
    pbolt.foe_count = pbolt.fr_count = 0;
    pbolt.foe_power = pbolt.fr_power = 0;

    // default - see mons_should_fire()
    pbolt.foe_ratio = 60;                        // all stupid monsters

    if (pbolt.smart_monster)
    {
        // intelligent charmed summons/monsters are more likely to hit friends
        if (monster->attitude != ATT_FRIENDLY && mons_friendly( monster ))
            pbolt.foe_ratio = 25;
        else // other intelligent monsters are more careful
            pbolt.foe_ratio = 80;
    }

    // fire!
    fire_beam( pbolt );

    // unset tracer flag (convenience)
    pbolt.is_tracer = false;
}                               // end tracer_f()

bool check_line_of_sight( int sx, int sy, int tx, int ty )
{
    struct bolt  pbolt;

    const int dist = grid_distance( sx, sy, tx, ty );

    // can always see one square away
    if (dist <= 1)
        return (true);

    // currently we limit the range to 8
    if (dist > MONSTER_LOS_RANGE)
        return (false);

    // Redirect player centered LoS to the old method (using display table)...
    // note that this assumes that viewwindow() has been called if needed
    // before we get here (ie this won't work very well if this function gets
    // called between moving the player and updating the display).
    if (sx == you.x_pos && sy == you.y_pos)
        return (see_grid( tx, ty ));
    else if (tx == you.x_pos && ty == you.y_pos)
        return (see_grid( sx, sy ));

    // Okay, no easy way... set up a LoS beam between the points
    pbolt.flavour = BEAM_LINE_OF_SIGHT;
    pbolt.is_tracer = true;
    pbolt.source_x = sx;
    pbolt.source_y = sy;
    pbolt.target_x = tx;
    pbolt.target_y = ty;
    pbolt.range = MONSTER_LOS_RANGE;
    pbolt.rangeMax = MONSTER_LOS_RANGE;

    // setting these just to be safe:
    pbolt.hit = 0;
    pbolt.type = 0;
    pbolt.damage = dice_def( 0, 1 );
    pbolt.colour = BLACK;
    pbolt.is_beam = true;
    pbolt.is_enchant = false;

    // init tracer variables (used to tell if we "hit" the target)
    pbolt.foe_count = pbolt.fr_count = 0;
    pbolt.foe_power = pbolt.fr_power = 0;

    // fire!
    fire_beam( pbolt );

    // got to target?
    return (pbolt.foe_count == 1);
}


/*
   When a mimic is hit by a ranged attack, it teleports away (the slow way)
   and changes its appearance - the appearance change is in monster_teleport
   in mstuff2.
 */
void mimic_alert(struct monsters *mimic)
{
    if (mons_has_ench( mimic, ENCH_TELEPORT ))
        return;

    monster_teleport( mimic, random2(3) );
}                               // end mimic_alert()

static bool isBouncy(struct bolt &beam)
{
    // XXX: why 'non-enchantment'?
    // at present, only non-enchantment eletrcical beams bounce.
    //if (!beam.is_enchant && beam.flavour == BEAM_ELECTRICITY)

    // beaming (non-arc) lightning is now the only thing bouncing
    if (beam.is_beam && beam.flavour == BEAM_ELECTRICITY)
        return (true);

    return (false);
}

// Okay, this is fast and dirty but at least it should partially
// fix the badly broken poison blast and noisome vapour monster attacks.
// This currently only checks the likely squares, and assumes poisonous
// clouds.
static void trace_big_cloud( struct bolt &beam, int x, int y )
{
    for (int tx = x - 1; tx <= x + 1; tx++)
    {
        for (int ty = y - 1; ty <= y + 1; ty++)
        {
            if (!in_bounds( tx, ty ))
                continue;

            if (you.x_pos == tx && you.y_pos == ty)
            {
                if (beam.is_friendly)
                {
                    beam.fr_count += 1;
                    beam.fr_power += you.xp_level;
                }
                else
                {
                    beam.foe_count += 1;
                    beam.foe_power += you.xp_level;
                }
            }

            const int mon = mgrd[tx][ty];

            if (mon != NON_MONSTER)
            {
                // Making the assumption that these will be largely
                // used by monsters in areas where poison resistance
                // is common (green deaths and summoned demons, swamp
                // dragons in the swamp)... so we can assume a bit
                // of knowledge.
                if (mons_res_poison( &menv[mon] ) > 0)
                    continue;

                if (beam.is_friendly ^ mons_friendly( &menv[mon] ))
                {
                    beam.foe_count += 1;
                    beam.foe_power += mons_power(mon);
                }
                else
                {
                    beam.fr_count += 1;
                    beam.fr_power += mons_power(mon);
                }
            }
        }
    }
}

static void beam_explodes( struct bolt &beam, int x, int y )
{
    int size, type, dur;

    // this will be the last thing this beam does.. set target_x
    // and target_y to hold explosion co'ords.

    beam.target_x = x;
    beam.target_y = y;

    // generic explosion
    if (beam.is_explosion)
    {
        explosion1( beam );
    }
    else if (beam.flavour >= BEAM_POTION_STINKING_CLOUD
            && beam.flavour <= BEAM_POTION_RANDOM)
    {
        type = CLOUD_NONE;
        size = 1 + beam.ench_power / 25 + random2(2 + beam.ench_power / 15);
        dur = 1 + beam.ench_power / 25;

        switch (beam.flavour)
        {
        case BEAM_POTION_STINKING_CLOUD:
            type = CLOUD_STINK;
            break;

        case BEAM_POTION_POISON:
            type = CLOUD_POISON;
            break;

        case BEAM_POTION_MIASMA:
            type = CLOUD_MIASMA;
            break;

        case BEAM_POTION_BLACK_SMOKE:
            type = CLOUD_BLACK_SMOKE;
            break;

        case BEAM_POTION_STEAM:
            type = CLOUD_STEAM;
            break;

        case BEAM_POTION_FIRE:
            type = CLOUD_FIRE;
            break;

        case BEAM_POTION_COLD:
            type = CLOUD_COLD;
            break;

        case BEAM_POTION_BLUE_SMOKE:
            type = CLOUD_BLUE_SMOKE;
            break;

        case BEAM_POTION_PURP_SMOKE:
            type = CLOUD_PURP_SMOKE;
            break;

        case BEAM_POTION_RANDOM:
            type = CLOUD_RANDOM;
            break;

        default:
            break;
        }

        if (type != CLOUD_NONE)
            big_cloud( type, x, y, dur, size );
    }
    else if (strcmp( beam.name, "blast of poison" ) == 0)
    {
        // cloud producer -- POISON BLAST
        type = YOU_KILL( beam.thrower ) ? CLOUD_POISON : CLOUD_POISON_MON;
        dur = roll_dice( 1, 1 + beam.ench_power / 10 );
        size = 5 + beam.ench_power / 20 + random2(3);

        if (!beam.is_tracer)
            big_cloud( type, x, y, dur, size );
        else
            trace_big_cloud( beam, x, y );
    }
    else if (strcmp( beam.name, "noisome vapour" ) == 0)
    {
        // cloud producer -- NOISOME VAPOR (SWAMP DRAKE?)
        type = YOU_KILL( beam.thrower ) ? CLOUD_STINK : CLOUD_STINK_MON;
        dur = roll_dice( 1, 1 + beam.ench_power / 10 );
        size = 5 + beam.ench_power / 20 + random2(3);

        if (!beam.is_tracer)
            big_cloud( type, x, y, dur, size );
        else
            trace_big_cloud( beam, x, y );
    }
}

static bool beam_term_on_target( struct bolt &beam )
{
    // Signal that beam successfully got to the target for LoS beams:
    if (beam.flavour == BEAM_LINE_OF_SIGHT)
    {
        beam.foe_count++;
        return (true);
    }

    if (beam.is_explosion || beam.is_big_cloud)
        return (true);

    return (false);
}

static void beam_drop_object( struct bolt &beam, item_def *const item, int x, int y )
{
    ASSERT( item != NULL );

    // conditions: beam is missile and not tracer.
    if (beam.is_tracer || beam.flavour != BEAM_MISSILE)
        return;

    // ammo is always lost in lava and deep water
    if (grd[x][y] == DNGN_LAVA || grd[x][y] == DNGN_DEEP_WATER)
        return;

    // No longer matters who threw the ammo, what matters is the
    // PC's ability to fletch and recover that ammo.
    if (fired_ammo_recoverable( *item ))
        copy_item_to_grid( *item, x, y, 1 );
}

// somewhat complicated BOUNCE function
// returns # of times beam bounces during routine (usually 1)
//
// step 1 is always the step value from the stepping direction.
#define B_HORZ      1
#define B_VERT      2
#define B_BOTH      3

static int bounce(int &step1, int &step2, int w1, int w2, int &n1, int &n2,
    int l1, int l2, int &t1, int &t2, bool topBlocked, bool sideBlocked)
{
    int bounceType = 0;
    int bounceCount = 1;

    if (topBlocked) bounceType = B_HORZ;
    if (sideBlocked) bounceType = B_VERT;
    if (topBlocked && sideBlocked)
    {
        // check for veritcal bounce only
        if ((w2 + step2 - 50)/100 == (w2 - 50)/100)
            bounceType = B_VERT;
        else
            bounceType = B_BOTH;
    }

    switch (bounceType)
    {
        case B_VERT:            // easiest
            n1 = w1;
            n2 = w2 + step2;
            step1 = -step1;
            t1 = n1 / 100;
            t2 = (n2 + 50)/100;
            // check top
            if (t2 != n2/100 && topBlocked)
                t2 = n2/100;
            break;
        case B_HORZ:            // a little tricky
            if (step2 > 0)
                n2 = (100 + 200*(w2/100)) - (w2 + step2);
            else
                n2 = (100 + 200*((w2 - 50)/100)) - (w2 + step2);
            n1 = w1 + step1;
            t1 = n1 /100;
            t2 = (n2 + 50) / 100;
            step2 = -step2;
            break;
        case B_BOTH:
            // vertical:
            n1 = w1;
            t1 = l1;
            t2 = l2;
            // horizontal:
            if (step2 > 0)
                n2 = (100 + 200*(w2/100)) - (w2 + step2);
            else
                n2 = (100 + 200*((w2 - 50)/100)) - (w2 + step2);
            // reverse both directions
            step1 =- step1;
            step2 =- step2;
            bounceCount = 2;
            break;
        default:
            bounceCount = 0;
            break;
    }

    return (bounceCount);
}

static bool fuzzyLine( int nx, int ny, int &tx, int &ty, int lx, int ly,
                       int stepx, int stepy, bool roundX, bool roundY )
{
    bool fuzzyOK = false;
    int fx, fy;                 // fuzzy x,y

    // BEGIN fuzzy line algorithm
    fx = tx;
    fy = ty;

    if (roundY)
    {
        // try up
        fy = (ny + 100) / 100;

        // check for monotonic
        if (fy != ty && ((stepy > 0 && fy >= ly) || (stepy < 0 && fy <= ly)))
            fuzzyOK = true;

        // see if up try is blocked
        if (fuzzyOK && grid_is_solid( grd[tx][fy] ))
            fuzzyOK = false;

        // try down
        if (!fuzzyOK)
            fy = ny / 100;

        // check for monotonic
        if (fy != ty && ((stepy > 0 && fy >= ly) || (stepy < 0 && fy <= ly)))
            fuzzyOK = true;

        if (fuzzyOK && grid_is_solid( grd[tx][fy] ))
            fuzzyOK = false;
    }

    if (roundX)
    {
        // try up
        fx = (nx + 100) / 100;

        // check for monotonic
        if (fx != tx && ((stepx > 0 && fx >= lx) || (stepx < 0 && fx <= lx)))
            fuzzyOK = true;

        // see if up try is blocked
        if (fuzzyOK && grid_is_solid( grd[fx][ty] ))
            fuzzyOK = false;

        // try down
        if (!fuzzyOK)
            fx = nx / 100;

        // check for monotonic and one square from last
        if (fx != tx && ((stepx > 0 && fx >= lx) || (stepx < 0 && fx <= lx)))
            fuzzyOK = true;

        if (fuzzyOK && grid_is_solid( grd[fx][ty] ))
            fuzzyOK = false;
    }
    // END fuzzy line algorithm

    // must be one away from the last point, else beams can quantuum tunnel
    if (grid_distance( lx, ly, fx, fy ) != 1)
        fuzzyOK = false;

    if (fuzzyOK)
    {
        tx = fx;
        ty = fy;
    }

    return (fuzzyOK);
}

// affects a single cell.
// returns the amount of extra range 'used up' by this beam
// during the affectation.
//
// pseudo-code:
//
// 1. If wall, and wall affecting non-tracer, affect the wall.
//  1b.  If for some reason the wall-affect didn't make it into
//      a non-wall, return                      affect_wall()
// 2. for non-tracers, produce cloud effects    affect_place_clouds()
// 3. if cell holds player, affect player       affect_player()
// 4. if cell holds monster, affect monster     affect_monster()
// 5. return range used affectation.

static int affect( struct bolt &beam, int x, int y )
{
    // extra range used by hitting something
    int rangeUsed = 0;

    // line of sight never affects anything
    if (beam.flavour == BEAM_LINE_OF_SIGHT)
        return (0);

    if (grid_is_solid( grd[x][y] ))
    {
        if (beam.is_tracer)          // tracers always stop on walls.
            return (BEAM_STOP);

        if (affectsWalls(beam))
        {
            rangeUsed += affect_wall( beam, x, y );
        }
        // if it's still a wall,  quit - we can't do anything else to
        // a wall.  Otherwise effects (like clouds, etc) are still possible.
        if (grid_is_solid( grd[x][y] ))
            return (rangeUsed);
    }

    // grd[x][y] will NOT be a wall for the remainder of this function.

    // if not a tracer, place clouds
    if (!beam.is_tracer)
        rangeUsed += affect_place_clouds( beam, x, y );

    // if player is at this location,  try to affect unless term_on_target
    if (x == you.x_pos && y == you.y_pos)
    {
        // Done this way so that poison blasts affect the target once (via
        // place_cloud) and explosion spells only affect the target once
        // (during the explosion phase, not an initial hit during the
        // beam phase).
        if (!beam.is_big_cloud
            && (!beam.is_explosion || beam.in_explosion_phase))
        {
            rangeUsed += affect_player( beam );
        }

        if (beam_term_on_target( beam ))
            return (BEAM_STOP);
    }

    // if there is a monster at this location,  affect it
    // submerged monsters aren't really there -- bwr
    const int mid = mgrd[x][y];
    if (mid != NON_MONSTER && !mons_is_submerged( &menv[mid] ))
    {
        if (!beam.is_big_cloud
            && (!beam.is_explosion || beam.in_explosion_phase))
        {
            rangeUsed += affect_monster( beam, &menv[mid] );
        }

        if (beam_term_on_target( beam ))
            return (BEAM_STOP);
    }

    return (rangeUsed);
}

static bool affectsWalls(struct bolt &beam)
{
    // digging
    if (beam.flavour == BEAM_DIGGING)
        return (true);

    // Isn't this much nicer than the hack to remove ice bolts, disrupt,
    // and needles (just because they were also coloured "white") -- bwr
    if (beam.flavour == BEAM_DISINTEGRATION && beam.damage.num >= 3)
        return (true);

    // eye of devestation?
    if (beam.flavour == BEAM_NUKE)
        return (true);

    return (false);
}

// return amount of extra range used up by affectation of this wall.
static int affect_wall(struct bolt &beam, int x, int y)
{
    int rangeUsed = 0;

    if (!in_bounds( x, y ))
        return (BEAM_STOP);

    const int targ_grid = grd[x][y];
    const bool in_sight = see_grid( x, y );

    if (beam.flavour == BEAM_DIGGING)
    {
        switch (targ_grid)
        {
        case DNGN_STONE_WALL:
        case DNGN_METAL_WALL:
        case DNGN_PERMAROCK_WALL:
            return (BEAM_STOP);

        case DNGN_SECRET_DOOR:
            grd[x][y] = DNGN_CLOSED_DOOR;

            if (!in_sight)
                set_terrain_changed( x, y );

            if (!beam.msg_generated)
            {
                if (in_sight)
                {
                    mpr( "You found a secret door!" );
                    beam.obvious_effect = true;
                }

                beam.msg_generated = true;
            }
            return (BEAM_STOP);

        case DNGN_ROCK_WALL:
            grd[x][y] = DNGN_FLOOR;

            if (!in_sight)
                set_terrain_changed( x, y );

            noisy( SL_DIG, x, y );

            if (!beam.msg_generated)
            {
                if (player_can_hear( x, y ))
                {
                    mpr(MSGCH_SOUND,"You hear a grinding noise." );
                    beam.obvious_effect = true;
                }

                beam.msg_generated = true;
            }
            break;

        default:
            break;
        }
    }
    else if (beam.flavour == BEAM_DISINTEGRATION || beam.flavour == BEAM_NUKE)
    {
        if (targ_grid == DNGN_ROCK_WALL
            || targ_grid == DNGN_CLOSED_DOOR
            || targ_grid == DNGN_SECRET_DOOR
            || targ_grid == DNGN_WAX_WALL)
        {
            grd[x][y] = DNGN_FLOOR;

            if (!in_sight)
                set_terrain_changed( x, y );

            if (noisy( SL_DIG, x, y, "You hear a grinding noise." ))
                beam.obvious_effect = true;
        }
        else if (targ_grid == DNGN_ORCISH_IDOL
                || targ_grid == DNGN_SILVER_STATUE
                || targ_grid == DNGN_GRANITE_STATUE
                || targ_grid == DNGN_ORANGE_CRYSTAL_STATUE)
        {
            grd[x][y] = DNGN_FLOOR;

            if (!in_sight)
                set_terrain_changed( x, y );

            if (player_can_hear( x, y ))
            {
                if (!see_grid( x, y ))
                    mpr(MSGCH_SOUND,"You hear a hideous screaming!" );
                else
                    mpr(MSGCH_SOUND,"The statue screams as its substance crumbles away!" );
            }
            else
            {
                if (see_grid(x,y))
                    mpr("The statue twists and shakes as its substance crumbles away!");
            }

            if (targ_grid == DNGN_SILVER_STATUE)
                you.visible_statue[ STATUE_SILVER ] = 0;
            else if (targ_grid == DNGN_ORANGE_CRYSTAL_STATUE)
                you.visible_statue[ STATUE_ORANGE_CRYSTAL ] = 0;

            beam.obvious_effect = 1;
        }

        return (BEAM_STOP);
    }

    return (rangeUsed);
}

static int affect_place_clouds( struct bolt &beam, int x, int y )
{
    int cloud;

    if (beam.in_explosion_phase)
    {
        affect_place_explosion_clouds( beam, x, y );
        return (0);       // return value irrelevant for explosions
    }

    // check for CLOUD HITS
    if (is_cloud( x, y ))
    {
        // polymorph randomly changes clouds in its path
        if (beam.flavour == BEAM_POLYMORPH)
            env.cloud[ env.cgrid[x][y] ].type = 1 + random2(8);

        // now exit (all enchantments)
        if (beam.is_enchant)
            return (0);

        const int cid = env.cgrid[x][y];
        const int ctype = get_cloud_type( x, y );

        // fire cancelling cold & vice versa
        if (((ctype == CLOUD_COLD || ctype == CLOUD_COLD_MON)
                && (beam.flavour == BEAM_FIRE || beam.flavour == BEAM_LAVA))
            || ((ctype == CLOUD_FIRE || ctype == CLOUD_FIRE_MON)
                && beam.flavour == BEAM_COLD))
        {
            if (player_can_hear( x, y ))
                mpr(MSGCH_SOUND,"You hear a sizzling sound!" );

            delete_cloud( cid );
            return (5);
        }
    }

    // POISON BLAST
    if (strcmp( beam.name, "blast of poison" ) == 0)
    {
        cloud = YOU_KILL(beam.thrower) ? CLOUD_POISON : CLOUD_POISON_MON;
        place_cloud( cloud, x, y, random2(4) + 2 );
    }

    // NOISOME VAPOR (SWAMP DRAKE?)
    if (strcmp( beam.name, "noisome vapour" ) == 0)
    {
        cloud = YOU_KILL(beam.thrower) ? CLOUD_STINK : CLOUD_STINK_MON;
        place_cloud( cloud, x, y, random2(4) + 2 );
    }

    // FIRE/COLD over water/lava
    if ((grd[x][y] == DNGN_LAVA && beam.flavour == BEAM_COLD)
        || (grid_is_water( grd[x][y] ) && beam.flavour == BEAM_FIRE))
    {
        cloud = YOU_KILL(beam.thrower) ? CLOUD_STEAM : CLOUD_STEAM_MON;
        place_cloud( cloud, x, y, 2 + random2(5) );
    }

    // ORB OF ENERGY
    if (strcmp( beam.name, "orb of energy" ) == 0)
        place_cloud( CLOUD_PURP_SMOKE, x, y, random2(5) + 1 );

    // GREAT BLAST OF COLD
    if (strcmp( beam.name, "great blast of cold" ) == 0)
        place_cloud( CLOUD_COLD, x, y, random2(5) + 3 );


    // BALL OF STEAM
    if (beam.flavour == BEAM_STEAM)
    {
        cloud = YOU_KILL( beam.thrower ) ? CLOUD_STEAM : CLOUD_STEAM_MON;
        place_cloud( cloud, x, y, random2(5) + 2 );
    }

    // MIASMA
    if (beam.flavour == BEAM_MIASMA)
    {
        cloud = YOU_KILL( beam.thrower ) ? CLOUD_MIASMA : CLOUD_MIASMA_MON;
        place_cloud( cloud, x, y, random2(5) + 2 );
    }

    // STICKY FLAME
    if (beam.flavour == BEAM_NAPALM)
        place_cloud( CLOUD_BLACK_SMOKE, x, y, random2(4) + 2 );

    // POISON GAS
    if (strcmp(beam.name, "poison gas") == 0)
    {
        cloud = YOU_KILL( beam.thrower ) ? CLOUD_POISON : CLOUD_POISON_MON;
        place_cloud( cloud, x, y, random2(4) + 3 );
    }

    return (0);
}

// following two functions used with explosions:
static void affect_place_explosion_clouds( struct bolt &beam, int x, int y )
{
    int cloud;
    int duration;

    // Note: Calling place_cloud with "false" for affect_grid so that we
    // don't get a double message and hit... the first hit is for the blast
    // damage in the spell, the first cloud hit will happen on the next
    // action.  Spells using big_cloud instead of explosion work the other
    // way (they pretty much need the cloud to affect the grid immediately).

    // first check: FIRE/COLD over water/lava
    if ((grd[x][y] == DNGN_LAVA && beam.flavour == BEAM_COLD)
        || (grid_is_water( grd[x][y] ) && beam.flavour == BEAM_FIRE))
    {
        cloud = YOU_KILL( beam.thrower ) ? CLOUD_STEAM : CLOUD_STEAM_MON;
        place_cloud( cloud, x, y, 2 + random2(5), false );
        return;
    }

    // Then check for more specific explosion cloud types.
    if (stricmp( beam.name, "ice storm" ) == 0)
        place_cloud( CLOUD_COLD, x, y, roll_dice(2,3), false );

    if (stricmp( beam.name, "stinking cloud" ) == 0)
    {
        duration =  1 + random2(4) + random2( (beam.ench_power / 50) + 1 );
        place_cloud( CLOUD_STINK, x, y, duration, false );
    }

    if (strcmp( beam.name, "great blast of fire" ) == 0)
    {
        duration = 1 + random2(5) + roll_dice( 2, beam.ench_power / 5 );

        if (duration > 20)
            duration = 20 + random2(4);

        place_cloud( CLOUD_FIRE, x, y, duration, false );

        if (grd[x][y] == DNGN_FLOOR && mgrd[x][y] == NON_MONSTER
            && one_chance_in(4))
        {
            mons_place( MONS_FIRE_VORTEX, BEH_HOSTILE, MHITNOT, true, x, y );
        }
    }
}

static void affect_items(struct bolt &beam, int x, int y)
{
    beam_type flavour = static_cast<beam_type>( beam.flavour );

    if (stricmp( beam.name, "hellfire" ) == 0)
        flavour = BEAM_FIRE;
    else if (stricmp( beam.name, "hellfrost" ) == 0)
        flavour = BEAM_COLD;

    expose_floor_to_element( flavour, x, y );
}

// A little helper function to handle the calling of ouch()...
static void beam_ouch( int dam, struct bolt &beam )
{
    // The order of this is important.
    if (YOU_KILL( beam.thrower ) && !beam.aux_source)
    {
        ouch( dam, 0, KILLED_BY_TARGETTING );
    }
    else if (MON_KILL( beam.thrower ))
    {
        if (beam.flavour == BEAM_SPORE)
            ouch( dam, beam.beam_source, KILLED_BY_SPORE );
        else
            ouch( dam, beam.beam_source, KILLED_BY_BEAM, beam.aux_source );
    }
    else // KILL_MISC || (YOU_KILL && aux_source)
    {
        ouch( dam, beam.beam_source, KILLED_BY_WILD_MAGIC, beam.aux_source );
    }
}

// return amount of extra range used up by affectation of the player
static int affect_player( struct bolt &beam )
{
    // digging -- don't care.
    if (beam.flavour == BEAM_DIGGING)
        return (0);

    // check for tracer
    if (beam.is_tracer)
    {
        // check can see player
        // XXX: note the cheat to allow for ME_ALERT to target the player...
        // replace this with a time since alert system, rather than just
        // peeking to see if the character is still there. -- bwr
        if (beam.can_see_invis || !you.invis
            || (you.x_pos == beam.target_x && you.y_pos == beam.target_y))
        {
            if (beam.is_friendly)
            {
                beam.fr_count += 1;
                beam.fr_power += you.xp_level;
            }
            else
            {
                beam.foe_count += 1;
                beam.foe_power += you.xp_level;
            }
        }

        return (range_used_on_hit(beam));
    }

    if (beam.is_enchant)
    {
        // BEGIN enchantment beam
        if (beam.flavour != BEAM_HASTE
            && beam.flavour != BEAM_INVISIBILITY
            && beam.flavour != BEAM_HEALING
            && ((beam.flavour != BEAM_TELEPORT && beam.flavour != BEAM_BANISH)
                || !beam.aimed_at_feet)
            && you_resist_magic( beam.ench_power ))
        {
            canned_msg(MSG_YOU_RESIST);
            return (range_used_on_hit(beam));
        }

        ench_animation( beam.flavour );

        switch (beam.flavour)
        {
        case BEAM_SLOW:
            potion_effect( POT_SLOWING, beam.ench_power );
            beam.obvious_effect = true;
            break;     // slow

        case BEAM_HASTE:
            potion_effect( POT_SPEED, beam.ench_power );
            contaminate_player( 1 );
            beam.obvious_effect = true;
            break;     // haste

        case BEAM_HEALING:
            potion_effect( POT_HEAL_WOUNDS, beam.ench_power );
            beam.obvious_effect = true;
            break;     // heal (heal wounds potion eff)

        case BEAM_PARALYSIS:
        case BEAM_PETRIFY:
            potion_effect( POT_PARALYSIS, beam.ench_power );
            beam.obvious_effect = true;
            break;     // paralysis

        case BEAM_CONFUSION:
            potion_effect( POT_CONFUSION, beam.ench_power );
            beam.obvious_effect = true;
            break;     // confusion

        case BEAM_INVISIBILITY:
            potion_effect( POT_INVISIBILITY, beam.ench_power );
            contaminate_player( 1 + random2(2) );
            beam.obvious_effect = true;
            break;     // invisibility

            // 6 is used by digging

        case BEAM_TELEPORT:
            you_teleport();
            beam.obvious_effect = true;
            break;

        case BEAM_BLINK:
            random_blink(0);
            beam.obvious_effect = true;
            break;

        case BEAM_POLYMORPH:
            mpr("This is polymorph other only!");
            beam.obvious_effect = true;
            break;

        case BEAM_CHARM:
            potion_effect( POT_CONFUSION, beam.ench_power );
            beam.obvious_effect = true;
            break;     // enslavement - confusion?

        case BEAM_BANISH:
            banished( DNGN_ENTER_ABYSS );
            beam.obvious_effect = true;
            break;     // banishment to the abyss

        case BEAM_PAIN:      // pain
            if (you.is_undead || you.mutation[MUT_TORMENT_RESISTANCE])
            {
                mpr("You are unaffected.");
                break;
            }

            mpr("Pain shoots through your body!");

            if (!beam.aux_source)
                beam.aux_source = "by nerve-wracking pain";

            beam_ouch( roll_dice( beam.damage ), beam );
            beam.obvious_effect = true;
            break;

        case BEAM_DISPEL_UNDEAD:
            if (!you.is_undead)
            {
                mpr("You are unaffected.");
                break;
            }

            mpr( "You convulse!" );

            if (!beam.aux_source)
                beam.aux_source = "by dispel undead";

            beam_ouch( roll_dice( beam.damage ), beam );
            beam.obvious_effect = true;
            break;

        case BEAM_DISINTEGRATION:
            mpr("You are blasted!");

            if (!beam.aux_source)
                beam.aux_source = "disintegration bolt";

            beam_ouch( roll_dice( beam.damage ), beam );
            beam.obvious_effect = true;
            break;

        default:
            // _all_ enchantments should be enumerated here!
            mpr("Software bugs nibble your toes!");
            break;
        }               // end of switch (beam.colour)

        // regardless of affect, we need to know if this is a stopper
        // or not - it seems all of the above are.
        return (range_used_on_hit( beam ));

        // END enchantment beam
    }

    // THE BEAM IS NOW GUARANTEED TO BE A NON-ENCHANTMENT
    ASSERT( !beam.is_enchant );

    // BEGIN real beam code
    beam.msg_generated = true;

    const bool engulfs = (beam.is_explosion || beam.is_big_cloud);

    // explosions and self-targeted beams cannot be blocked or dodged
    if (!engulfs && !beam.aimed_at_feet)
    {
        // Note: dodging is done before blocking here so that the player
        // can benefit from deflect missiles (no side cost), before choosing
        // to block (which costs a shield_block).

        // Don't touch beam.hit directly... it gets passed back to the caller
        int beamHit = beam.hit;

        if (you.duration[DUR_DEFLECT_MISSILES])
            beamHit = (beam.is_beam) ? (1 + beamHit / 3) : 0;
        else if (player_repel_missiles())
            beamHit = 1 + (beamHit * (beam.is_beam ? 3 : 2)) / 4;

        if (!test_hit( beamHit, player_evasion() ))
        {
            mpr( "The %s misses you.", beam.name );

            if (!you.duration[DUR_DEFLECT_MISSILES])
                exercise_dodging( (coinflip() ? 1 : 2), 3 );

            return (0);           // no extra used by miss!
        }

        // bolt/arrow type stuff can be blocked
        if (beam_is_blockable( beam ))
        {
            const int block = player_shield_class();
            const int hit = 5 + beam.hit / 10 + your_shield_blocks_penalty();

            if (block && player_can_block() && test_block( block, hit ))
            {
                mpr( "You block the %s.", beam.name );

                // Currently leaving this out to improve shields, and for
                // consistancy with the fact that we don't do acid blocks
                // in the melee code. -- bwr
                //
                // if (beam.flavour == BEAM_ACID && you.equip[EQ_SHIELD] != -1)
                //     item_corrode( you.inv[ you.equip[EQ_SHIELD] ] );

                handle_player_shield_blocks( beam.damage.num * beam.damage.size,
                                             coinflip() ? 1 : 2 );

                return (BEAM_STOP);
            }
        }
    }

    // THE BEAM IS NOW GUARANTEED TO BE A NON-ENCHANTMENT WHICH HIT

    mpr( "The %s %s you!", beam.name, (engulfs) ? "engulfs" : "hits" );

    int hurted = 0;
    const int burn_power = (engulfs) ? 7 : (beam.is_beam) ? 4 : 3;

    // Roll the damage
    hurted += roll_dice( beam.damage );

#if DEBUG_DIAGNOSTICS
    int roll = hurted;
#endif

    hurted = apply_player_armour( beam.damage.num * beam.damage.size, hurted,
                                  beam.is_energy );

    // shrapnel
    if (beam.flavour == BEAM_FRAG && !player_light_armour())
    {
        const int ac = player_armour_class();
        hurted -= random2( 1 + ac );
        hurted -= random2( 1 + ac );
    }

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Player damage: rolled=%d; after AC=%d", roll, hurted );
#endif

    exercise_armour( 1, 4 );

    if (hurted < 0)
        hurted = 0;

    hurted = check_your_resists( hurted, static_cast<beam_type>(beam.flavour) );

    if (beam.flavour == BEAM_MIASMA && hurted > 0)
    {
        if (player_res_poison() <= 0)
            poison_player(1);

        if (one_chance_in( 3 + 2 * player_prot_life() ))
            potion_effect( POT_SLOWING, 5 );
    }

    // poisoning
    if (strstr( beam.name, "poison" ) != NULL
        && beam.flavour != BEAM_POISON
        && beam.flavour != BEAM_POISON_ARROW
        && beam.is_launched
        && player_res_poison() <= 0)
    {
        if (strstr( beam.name, "needle" ) != NULL)
        {
            if (random2(100) < 80 - (3 * player_armour_class()))
                poison_player(1);
        }
        else if (hurted)
        {
            poison_player( roll_dice(1,3) );
        }
    }

    // sticky flame
    if (beam.flavour == BEAM_NAPALM
        && ((you.species != SP_MOTTLED_DRACONIAN
                || you.xp_level < 7)
            && !player_equip( EQ_BODY_ARMOUR, ARM_MOTTLED_DRAGON_ARMOUR )))
    {
        you.duration[DUR_LIQUID_FLAMES] += 1 + roll_zdice(2,4);
    }

    // simple cases for scroll burns
    if (beam.flavour == BEAM_LAVA || stricmp( beam.name, "hellfire" ) == 0)
        expose_player_to_element( BEAM_LAVA, burn_power );

    // potions exploding
    if (beam.flavour == BEAM_COLD || stricmp( beam.name, "hellfrost" ) == 0)
        expose_player_to_element( BEAM_COLD, burn_power );

    if (beam.flavour == BEAM_ACID)
        splash_with_acid(5);

    // spore pops
    if (beam.in_explosion_phase && beam.flavour == BEAM_SPORE)
        expose_player_to_element( BEAM_SPORE, 5 );

    if (beam.flavour == BEAM_FRAG && random2( hurted ) > 7)
        cut_player( roll_dice(1,6) );

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Damage: %d", hurted );
#endif

    beam_ouch( hurted, beam );

    return (range_used_on_hit( beam ));
}

// return amount of range used up by affectation of this monster
static int affect_monster( struct bolt &beam, struct monsters *mon )
{
    int tid = mgrd[mon->x][mon->y];
    int hurt;
    int hurt_final;

    // digging -- don't care.
    if (beam.flavour == BEAM_DIGGING)
        return (0);

    // fire storm creates these, so we'll avoid affecting them
    if (strcmp( beam.name, "great blast of fire" ) == 0
        && mon->type == MONS_FIRE_VORTEX)
    {
        return (0);
    }

    // check for tracer
    if (beam.is_tracer)
    {
        // check can see other monster
        if (!beam.can_see_invis && mons_is_invisible( &menv[tid] ))
        {
            // can't see this monster, ignore it
            return 0;
        }
    }

    if (beam.is_enchant)
    {
        if (beam.is_tracer)
        {
            // enchant case -- enchantments always hit, so update target immed.
            if (beam.is_friendly ^ mons_friendly(mon))
            {
                beam.foe_count += 1;
                beam.foe_power += mons_power(tid);
            }
            else
            {
                beam.fr_count += 1;
                beam.fr_power += mons_power(tid);
            }

            return (range_used_on_hit(beam));
        }

        // BEGIN non-tracer enchantment

        // nasty enchantments will annoy the monster, and are considered
        // naughty (even if a monster might resist)
        if (nasty_beam( mon, beam ))
        {
            if (YOU_KILL( beam.thrower ))
            {
                if (mons_friendly( mon ))
                    did_god_conduct( DID_ATTACK_FRIEND, 5 );

                if (mons_holiness( mon ) == MH_HOLY)
                    did_god_conduct( DID_ATTACK_HOLY, mon->hit_dice );
            }

            behaviour_event( mon, ME_SHOT, beam.beam_source );
        }
        else
        {
            behaviour_event( mon, ME_ALERT, beam.beam_source );
        }

        // !@#*( affect_monster_enchantment() has side-effects on
        // the beam structure which screw up range_used_on_hit(),
        // so call it now and store.
        int rangeUsed = range_used_on_hit(beam);

        // XXX: different colours for different effects?

        // Doing this here so that the player gets to see monsters
        // "flicker and vanish" when turning invisible....
        ench_animation( beam.flavour, mon );

        // now do enchantment affect
        int ench_result = affect_monster_enchantment( beam, mon );

        switch (ench_result)
        {
            case MON_RESIST:
                if (mon_msg(mon, " resists."))
                    beam.msg_generated = true;
                break;
            case MON_UNAFFECTED:
                if (mon_msg(mon, " is unaffected."))
                    beam.msg_generated = true;
                break;
            default:
                break;
        }
        return (rangeUsed);

        // END non-tracer enchantment
    }


    // BEGIN non-enchantment (could still be tracer)
    if (mons_is_submerged( mon ) && !beam.aimed_at_feet)
        return (0);                   // missed me!

    if (beam_is_blockable( beam ))
    {
        const int block = mons_shield_class( mon );

        if (!beam.aimed_at_feet && block && mons_can_block( mon, NULL, true ))
        {
            const int hit = 5 + beam.hit / 10 + mons_blocks_penalty( mon );

            if (test_block( block, hit ))
            {
                snprintf( info, INFO_SIZE, " blocks the %s.", beam.name );
                mon_msg( mon, info );

                handle_mon_shield_blocks( mon );
                return (BEAM_STOP);
            }
        }
    }

    // we need to know how much the monster _would_ be hurt by this,  before
    // we decide if it actually hits.

    // Roll the damage:
    hurt = roll_dice( beam.damage );

    hurt_final = hurt;

    if (beam.is_tracer)
        hurt_final -= mon->armour_class / 2;
    else
        hurt_final = apply_mons_armour( hurt_final, mon, beam.is_energy );

    if (beam.flavour == BEAM_FRAG && !mons_light_armour( mon ))
    {
        hurt_final -= random2( mon->armour_class + 1 );
        hurt_final -= random2( mon->armour_class + 1 );
    }

    if (hurt_final < 1)
        hurt_final = 0;

#if DEBUG_DIAGNOSTICS
    const int old_hurt = hurt_final;
#endif

    // check monster resists,  _without_ side effects (since the
    // beam/missile might yet miss!)
    hurt_final = mons_adjust_flavoured( mon, beam, hurt_final, false );

#if DEBUG_DIAGNOSTICS
    if (!beam.is_tracer)
    {
        mpr( MSGCH_DIAGNOSTICS,
              "Monster: %s; Damage: pre-AC: %d; post-AC: %d; post-resist: %d",
              ptr_monam( mon, DESC_PLAIN ), hurt, old_hurt, hurt_final );
    }
#endif

    // now,  we know how much this monster would (probably) be
    // hurt by this beam.
    if (beam.is_tracer)
    {
        if (hurt_final > 0)
        {
            // monster could be hurt somewhat,  but only apply the
            // monster's power based on how badly it is affected.
            // For example,  if a fire giant (power 16) threw a
            // fireball at another fire giant,  and it only took
            // 1/3 damage,  then power of 5 would be applied to
            // foe_power or fr_power.
            if (beam.is_friendly ^ mons_friendly(mon))
            {
                beam.foe_count += 1;
                beam.foe_power += hurt_final * mons_power(tid) / hurt;
            }
            else
            {
                beam.fr_count += 1;
                beam.fr_power += hurt_final * mons_power(tid) / hurt;
            }
        }
        // either way, we could hit this monster, so return range used
        return (range_used_on_hit(beam));
    }
    // END non-enchantment (could still be tracer)

    // BEGIN real non-enchantment beam

    // player beams which hit friendly MIGHT annoy them and be considered
    // naughty if they do much damage (this is so as not to penalize
    // players that fling fireballs into a melee with fire elementals
    // on their side - the elementals won't give a sh*t,  after all)

    if (nasty_beam( mon, beam ))
    {
        // made this block match the nasty_beam block for enchantments above
        if (YOU_KILL(beam.thrower))
        {
            if (mons_friendly(mon))
                did_god_conduct( DID_ATTACK_FRIEND, 5 );

            if (mons_holiness( mon ) == MH_HOLY)
                did_god_conduct( DID_ATTACK_HOLY, mon->hit_dice );
        }

        behaviour_event( mon, ME_SHOT, beam.beam_source );
    }

    // explosions always 'hit'
    const bool engulfs = (beam.is_explosion || beam.is_big_cloud);
    const bool vis = player_monster_visible( &menv[tid] );

    // convential range fire (not magic but physical item launched or thrown)
    const bool conventional = (beam.is_launched || beam.is_thrown);

    const bool launch_target = (conventional
                                && vis
                                && ((beam.target_x == mon->x
                                        && beam.target_y == mon->y)
                                    || beam.target_first));

    // Bit of a kludge... to mark that we've already tried the first we
    // unset target_first (which means we're relying on the target
    // co-ords being out of map_bounds so that no later monster will match).
    // This also means that we can't use this flag to tell what we're
    // doing after the first visible monster.
    if (launch_target)
        beam.target_first = false;

    bool  hit = engulfs;
    bool  blocked_short = false;    // for shots into melee

    if (!engulfs)
    {
        if (!conventional || launch_target)
            hit = test_hit( beam.hit, mon->evasion );
        else
        {
            // Odds of a non-targeted by launcher monster getting hit
            // depends on its size.
            const int profile = 10 + 10 * mons_size( mon->type, PSIZE_BODY );
#if DEBUG_DIAGNOSTICS
            mpr( MSGCH_DIAGNOSTICS, "Avoiding non-target test (reversed):" );
#endif
            hit = !test_hit( beam.hit, profile );

            if (hit)
            {
                // So this can't be abused for cheap hits... blocked
                // shots are much less effective.
                hurt_final /= 3;
                blocked_short = true;
            }
        }
    }

    if (!hit)
    {
        // if the PLAYER cannot see the monster, don't tell them anything!
        if (vis && mons_near( mon ))
        {
            mpr( "The %s misses %s.", beam.name,
                 ptr_monam( mon, DESC_NOCAP_THE ) );
        }

        return (0);
    }

    // the beam hit.
    if (mons_near( mon ))
    {
        if (blocked_short)
        {
            mpr( "%s got in the way of the shot!",
                 (vis) ? ptr_monam( mon, DESC_CAP_THE ) : "Something" );
        }
        else
        {
            mpr( "The %s %s %s.",
                 beam.name,
                 (engulfs) ? "engulfs" : "hits",
                 (vis) ? ptr_monam( mon, DESC_NOCAP_THE ) : "something" );
        }
    }
    else
    {
        // the player might hear something,
        // if _they_ fired a missile (not beam)
        if (!silenced(you.x_pos, you.y_pos)
            && beam.flavour == BEAM_MISSILE
            && YOU_KILL(beam.thrower))
        {
            mpr( MSGCH_SOUND, "The %s hits something.", beam.name );
        }
    }

    // note that hurt_final was calculated above, so we don't need it again.
    // just need to apply flavoured specials (since we called with
    // doFlavouredEffects = false above).  The return value is thrown
    // away rather than applied again.
    mons_adjust_flavoured( mon, beam, hurt_final, true );

    // now hurt monster
    hurt_monster( mon, hurt_final );

    killer_type thrower = (YOU_KILL( beam.thrower ) ? KILL_YOU_MISSILE
                                                    : KILL_MON_MISSILE);

    if (mon->hit_points < 1)
    {
        monster_die( mon, thrower, beam.beam_source );
    }
    else
    {
        if (thrower == KILL_YOU_MISSILE && mons_near(mon))
            print_wounds(mon);

        // sticky flame
        if (beam.flavour == BEAM_NAPALM)
        {
            int levels = 1 + random2( hurt_final ) / 2;
            if (levels > 4)
                levels = 4;

            sticky_flame_monster( tid, YOU_KILL(beam.thrower), levels );
        }

        if (beam.flavour == BEAM_FRAG && random2( hurt_final ) > 7)
        {
            mons_add_ench( mon, ENCH_CUT, beam.beam_source );
        }

        /* looks for missiles which aren't poison but
           are poison*ed* */
        if (strstr(beam.name, "poison") != NULL
            && beam.flavour != BEAM_POISON
            && beam.flavour != BEAM_POISON_ARROW
            && beam.is_launched)
        {
            if (strstr(beam.name, "needle") != NULL)
            {
                if (random2(100) < 80 - (3 * mon->armour_class))
                    poison_monster( mon, YOU_KILL(beam.thrower) );
            }
            else if (random2(hurt_final) > random2(mon->armour_class))
            {
                poison_monster( mon, YOU_KILL(beam.thrower) );
            }
        }

        if (mons_is_mimic( mon->type ))
            mimic_alert(mon);
    }

    return (range_used_on_hit(beam));
}

static int affect_monster_enchantment( struct bolt &beam, struct monsters *mon )
{
    const int holy = mons_holiness( mon );

    switch (beam.flavour)
    {
    case BEAM_TELEPORT:
        if (!beam.aimed_at_feet
            && check_mons_resist_magic( mon, beam.ench_power ))
        {
            return (MON_RESIST);
        }

        if (!mons_has_ench( mon, ENCH_TELEPORT ))
        {
            // need message in this case, teleport function handles the other
            if (mon_msg( mon, " looks slightly unstable." ))
                beam.obvious_effect = true;
        }

        monster_teleport( mon, (beam.aimed_at_feet) ? 1 : (coinflip() ? 3 : 4) );
        return (MON_AFFECTED);

    case BEAM_BLINK:
        if (!beam.aimed_at_feet
            && check_mons_resist_magic( mon, beam.ench_power ))
        {
            return (MON_RESIST);
        }

        if (mons_near( mon ) && player_monster_visible( mon ))
            beam.obvious_effect = true;

        monster_blink( mon );
        return (MON_AFFECTED);

    case BEAM_POLYMORPH:
        if (holy != MH_NATURAL)
            return (MON_UNAFFECTED);

        if (check_mons_resist_magic( mon, beam.ench_power ))
            return (MON_RESIST);

        if (monster_polymorph( mon, RANDOM_MONSTER, 100 ))
            beam.obvious_effect = true;

        return (MON_AFFECTED);

    case BEAM_BANISH:
        if (check_mons_resist_magic( mon, beam.ench_power ))
            return (MON_RESIST);

        if (you.level_type == LEVEL_ABYSS)
            mon_msg( mon, " wobbles for a moment." );
        else
            monster_die( mon, KILL_RESET, beam.beam_source );

        beam.obvious_effect = true;
        return (MON_AFFECTED);


    case BEAM_DEGENERATE:
        if (holy != MH_NATURAL || mon->type == MONS_PULSATING_LUMP)
            return (MON_UNAFFECTED);

        if (check_mons_resist_magic( mon, beam.ench_power ))
            return (MON_RESIST);

        if (monster_polymorph( mon, MONS_PULSATING_LUMP, 100 ))
            beam.obvious_effect = true;

        return (MON_AFFECTED);

    case BEAM_DISPEL_UNDEAD:
        if (holy != MH_UNDEAD)
            return (MON_UNAFFECTED);

        if (mon_msg( mon, " convulses!" ))
            beam.obvious_effect = true;

        hurt_monster( mon, roll_dice( beam.damage ) );
        goto deathCheck;

    case BEAM_ENSLAVE_UNDEAD:
        if (holy != MH_UNDEAD || mon->attitude == ATT_FRIENDLY)
            return (MON_UNAFFECTED);

        if (check_mons_resist_magic( mon, beam.ench_power ))
            return (MON_RESIST);

        mon_msg(mon, " is enslaved.");
        beam.obvious_effect = true;

        // wow, permanent enslaving
        mon->attitude = ATT_FRIENDLY;
        return (MON_AFFECTED);

    case BEAM_ENSLAVE_DEMON:
        if (holy != MH_DEMONIC || mon->attitude == ATT_FRIENDLY)
            return (MON_UNAFFECTED);

        if (check_mons_resist_magic( mon, beam.ench_power ))
            return (MON_RESIST);

        mon_msg(mon, " is enslaved.");
        beam.obvious_effect = true;

        // wow, permanent enslaving
        mon->attitude = ATT_FRIENDLY;
        return (MON_AFFECTED);

    case BEAM_PAIN:      // both pain and agony
        if (mons_res_negative_energy( mon ))
            return (MON_UNAFFECTED);

        if (check_mons_resist_magic( mon, beam.ench_power ))
            return (MON_RESIST);

        if (mon_msg(mon, " convulses in agony!"))
            beam.obvious_effect = true;

        if (strstr( beam.name, "agony" ) != NULL)
        {
            // AGONY
            mon->hit_points = mon->hit_points / 2;

            if (mon->hit_points < 1)
                mon->hit_points = 1;
        }
        else
        {
            // PAIN
            hurt_monster( mon, roll_dice( beam.damage ) );
        }
        goto deathCheck;

    case BEAM_DISINTEGRATION:     // disrupt and disintegrate
        if (mons_has_ench( mon, ENCH_PETRIFY ))
        {
            // Monster goes away like a statue: for 0 XP
            beam.obvious_effect = true;
            mon_msg( mon, "The statue of %s crumbles away!" );
            monster_die( mon, KILL_MISC, MHITNOT );
            return (MON_AFFECTED);
        }

        if (check_mons_resist_magic( mon, beam.ench_power ))
            return (MON_RESIST);

        if (mon_msg(mon, " is blasted."))
            beam.obvious_effect = true;

        hurt_monster( mon, roll_dice( beam.damage ) );
        goto deathCheck;


    case BEAM_BACKLIGHT:
        if (check_mons_resist_magic( mon, beam.ench_power ))
            return (MON_RESIST);

        // has side-effects so must occur after MR check
        if (!backlight_monsters(mon->x, mon->y, beam.hit, 0))
            return (MON_UNAFFECTED);

        beam.obvious_effect = true;
        return (MON_AFFECTED);

    case BEAM_SLEEP:
        if (holy != MH_NATURAL || mon->behaviour == BEH_SLEEP)
            return (MON_UNAFFECTED);

        if (check_mons_resist_magic( mon, beam.ench_power ))
            return (MON_RESIST);

        if (mons_has_ench( mon, ENCH_SLEEP_WARY ))  // slept recently
            return (MON_RESIST);

        if (mon_msg(mon, " looks drowsy..."))
            beam.obvious_effect = true;

        mon->behaviour = BEH_SLEEP;
        mons_add_ench( mon, ENCH_SLEEP_WARY );
        return (MON_AFFECTED);

    case BEAM_SLOW:
    case BEAM_PARALYSIS:
        if (holy == MH_PLANT)
            return (MON_UNAFFECTED);

        if (check_mons_resist_magic( mon, beam.ench_power ))
            return (MON_RESIST);
        break;

    case BEAM_PETRIFY:
        if (holy == MH_NONLIVING)
            return (MON_UNAFFECTED);

        if (check_mons_resist_magic( mon, beam.ench_power ))
            return (MON_RESIST);
        break;

    case BEAM_CONFUSION:
    case BEAM_CHARM:
        if (holy != MH_NATURAL)
            return (MON_UNAFFECTED);

        if (check_mons_resist_magic( mon, beam.ench_power ))
            return (MON_RESIST);
        break;

    case BEAM_HASTE:
        if (holy == MH_PLANT)
            return (MON_UNAFFECTED);
        break;

    case BEAM_HEALING:
        if (holy == MH_UNDEAD || holy == MH_NONLIVING)
            return (MON_UNAFFECTED);
        break;

    case BEAM_INVISIBILITY:
        if (mons_is_invisible( mon ))
            return (MON_UNAFFECTED);
        break;

    case BEAM_DIGGING:
    default:
        break;
    }

    return (mons_ench_f2(mon, beam));

deathCheck:

    killer_type thrower = KILL_YOU_MISSILE;

    if (MON_KILL( beam.thrower ))
        thrower = KILL_MON_MISSILE;

    if (mon->hit_points < 1)
        monster_die( mon, thrower, beam.beam_source );
    else
    {
        print_wounds(mon);

        if (mons_is_mimic( mon->type ))
            mimic_alert(mon);
    }

    return (MON_AFFECTED);
}


// extra range used on hit
static int  range_used_on_hit(struct bolt &beam)
{
    // non-beams can only affect one thing (player/monster)
    if (!beam.is_beam)
        return (BEAM_STOP);

    // CHECK ENCHANTMENTS
    if (beam.is_enchant)
    {
        switch(beam.flavour)
        {
        case BEAM_SLOW:
        case BEAM_HASTE:
        case BEAM_HEALING:
        case BEAM_PARALYSIS:
        case BEAM_CONFUSION:
        case BEAM_INVISIBILITY:
        case BEAM_TELEPORT:
        case BEAM_POLYMORPH:
        case BEAM_CHARM:
        case BEAM_BANISH:
        case BEAM_PAIN:
        case BEAM_DISINTEGRATION:
        case BEAM_DEGENERATE:
        case BEAM_DISPEL_UNDEAD:
        case BEAM_ENSLAVE_UNDEAD:
        case BEAM_ENSLAVE_DEMON:
        case BEAM_SLEEP:
        case BEAM_BACKLIGHT:
            return (BEAM_STOP);
        default:
            break;
        }

        return (0);
    }

    // hellfire stops for nobody!
    if (strcmp( beam.name, "hellfire" ) == 0
        || strcmp( beam.name, "hellfrost" ) == 0)
    {
        return (0);
    }

    // generic explosion
    if (beam.is_explosion || beam.is_big_cloud)
        return (BEAM_STOP);

    // plant spit
    if (beam.flavour == BEAM_ACID)
        return (BEAM_STOP);

    // lava doesn't go far, but it goes through most stuff
    if (beam.flavour == BEAM_LAVA)
        return (1);

    // If it isn't beam-lightning, reduce range by a lot
    if (beam.flavour != BEAM_ELECTRICITY)
        return (2 + random2(4));

    return (0);
}

/*
   Takes a bolt struct and refines it for use in the explosion function. Called
   from missile() and beam() in beam.cc. Explosions which do not follow from
   beams (eg scrolls of immolation) bypass this function.
 */
static void explosion1( struct bolt &pbolt )
{
    int ex_size = 1;
    // convenience
    int x = pbolt.target_x;
    int y = pbolt.target_y;
    const char *seeMsg = NULL;
    const char *hearMsg = NULL;

    // assume that the player can see/hear the explosion, or
    // gets burned by it anyway.  :)
    pbolt.msg_generated = true;

    if (stricmp( pbolt.name, "hellfire" ) == 0)
    {
        seeMsg = "The hellfire explodes!";
        hearMsg = "You hear a strangely unpleasant explosion.";

        pbolt.type = SYM_BURST;
        pbolt.flavour = BEAM_HELLFIRE;
    }

    if (stricmp( pbolt.name, "hellfrost" ) == 0)
    {
        seeMsg = "The hellfrost explodes!";
        hearMsg = "You hear a strangely unpleasant explosion.";

        pbolt.type = SYM_BURST;
        pbolt.flavour = BEAM_HELLFROST;
    }

    if (stricmp( pbolt.name, "golden flame" ) == 0)
    {
        seeMsg = "The flame explodes!";
        hearMsg = "You hear a strange explosion.";

        pbolt.type = SYM_BURST;
        pbolt.flavour = BEAM_HOLY;
        ex_size = 2;
    }

    if (stricmp( pbolt.name, "fireball" ) == 0)
    {
        seeMsg = "The fireball explodes!";
        hearMsg = "You hear an explosion.";

        pbolt.type = SYM_BURST;
        pbolt.flavour = BEAM_FIRE;
    }

    if (stricmp( pbolt.name, "orb of electricity" ) == 0)
    {
        seeMsg = "The orb of electricity explodes!";
        hearMsg = "You hear a clap of thunder!";

        pbolt.type = SYM_BURST;
        pbolt.flavour = BEAM_ELECTRICITY;
        pbolt.colour = LIGHTCYAN;
        ex_size = 2;
    }

    if (stricmp( pbolt.name, "orb of energy" ) == 0)
    {
        seeMsg = "The orb of energy explodes.";
        hearMsg = "You hear an explosion.";
    }

    if (stricmp( pbolt.name, "metal orb" ) == 0)
    {
        seeMsg = "The orb explodes into a blast of deadly shrapnel!";
        hearMsg = "You hear an explosion!";

        strcpy(pbolt.name, "blast of shrapnel");
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_FRAG;     // sets it from pure damage to shrapnel (which is absorbed extra by armour)
    }

    if (stricmp( pbolt.name, "great blast of cold" ) == 0)
    {
        seeMsg = "The blast explodes into a great storm of ice!";
        hearMsg = "You hear a raging storm!";

        strcpy(pbolt.name, "ice storm");
        pbolt.type = SYM_ZAP;
        pbolt.colour = WHITE;
        ex_size = 2 + (random2( pbolt.ench_power ) > 75);
    }

    if (stricmp( pbolt.name, "ball of vapour" ) == 0)
    {
        seeMsg = "The ball expands into a vile cloud!";
        hearMsg = "You hear a gentle \'poof\'.";
        strcpy( pbolt.name, "stinking cloud" );
    }

    if (seeMsg == NULL)
    {
        seeMsg = "The beam explodes into a cloud of software bugs!";
        hearMsg = "You hear the sound of one hand clapping!";
    }

    if (!pbolt.is_tracer)
    {
        // check for see/hear/no msg
        if (see_grid(x,y) || (x == you.x_pos && y == you.y_pos))
            mpr( seeMsg );
        else
        {
            if (player_can_hear( x, y ))
                mpr( MSGCH_SOUND, hearMsg  );
            else
                pbolt.msg_generated = false;
        }
    }

    pbolt.ex_size = ex_size;
    explosion( pbolt );
}                               // end explosion1()


#define MAX_EXPLOSION_RADIUS 9

// explosion is considered to emanate from beam->target_x, target_y
// and has a radius equal to ex_size.  The explosion will respect
// boundaries like walls,  but go through/around statues/idols/etc.

// for each cell affected by the explosion, affect() is called.

void explosion( struct bolt &beam, bool hole_in_the_middle )
{
    int r = beam.ex_size;

    // for now, we don't support explosions greater than 9 radius
    if (r > MAX_EXPLOSION_RADIUS)
        r = MAX_EXPLOSION_RADIUS;

    // beam is now an explosion:
    beam.in_explosion_phase = true;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS,
          "explosion %s (%d, %d) : t=%d c=%d f=%d hit=%d dam=%dd%d",
          (hole_in_the_middle) ? "around" : "at",
          beam.target_x, beam.target_y,
          beam.type, beam.colour, beam.flavour,
          beam.hit, beam.damage.num, beam.damage.size );
#endif

    // make a noise
    if (!beam.is_tracer)
    {
        const int vol = SL_EXPLODE_MIN + SL_EXPLODE_MULT * r;
        noisy( vol, beam.target_x, beam.target_y );
    }

    // set map to false
    for (int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
            explode_map[i][j] = false;
    }

    // discover affected cells - recursion is your friend!
    // this is done to model an explosion's behaviour around
    // corners where a simple 'line of sight' isn't quite
    // enough.   This might be slow for really big explosions,
    // as the recursion runs approximately as R^2
    explosion_map( beam, 0, 0, 0, 0, r );

    // go through affected cells,  drawing effect and
    // calling affect() and affect_items() for each.
    // now, we get a bit fancy,  drawing all radius 0
    // effects, then radius 1, radius 2, etc.  It looks
    // a bit better that way.

    // turn buffering off
#ifdef WIN32CONSOLE
    bool oldValue;
    if (!beam.is_tracer)
        oldValue = setBuffering(false);
#endif

    // --------------------- begin boom ---------------

    bool drawing = true;

    // two passes: the first is draw only, the second affects the cells
    for (int i = 0; i < 2; i++)
    {
        // do center -- but only if its affected
        if (!hole_in_the_middle)
            explosion_cell( beam, 0, 0, drawing );

        // do the rest of it
        for (int rad = 1; rad <= r; rad ++)
        {
            // do sides
            for (int ay = 1 - rad; ay <= rad - 1; ay += 1)
            {
                if (explode_map[-rad + 9][ay + 9])
                    explosion_cell( beam, -rad, ay, drawing );

                if (explode_map[rad + 9][ay + 9])
                    explosion_cell( beam, rad, ay, drawing );
            }

            // do top & bottom
            for (int ax = -rad; ax <= rad; ax += 1)
            {
                if (explode_map[ax + 9][-rad + 9])
                    explosion_cell( beam, ax, -rad, drawing );

                if (explode_map[ax + 9][rad + 9])
                    explosion_cell( beam, ax, rad, drawing );
            }

            // new-- delay after every 'ring' {gdl}
#ifdef LINUX
            // If we don't refresh curses we won't
            // guarantee that the explosion is visible
            if (drawing)
                update_screen();
#endif
            // only delay on real explosion
            if (!beam.is_tracer && drawing)
                delay(50);
        }

        drawing = false;
    }

    // ---------------- end boom --------------------------

#ifdef WIN32CONSOLE
    if (!beam.is_tracer)
        setBuffering(oldValue);
#endif

    // duplicate old behaviour - pause after entire explosion
    // has been drawn.
    if (!beam.is_tracer)
        more();
}

static void explosion_cell( struct bolt &beam, int x, int y, bool drawOnly )
{
    bool random_beam = false;
    int realx = beam.target_x + x;
    int realy = beam.target_y + y;

    if (!drawOnly)
    {
        // random beams: randomize before affect
        if (beam.flavour == BEAM_RANDOM)
        {
            random_beam = true;
            beam.flavour = BEAM_FIRE + random2(7);
        }

        affect( beam, realx, realy );

        if (random_beam)
            beam.flavour = BEAM_RANDOM;
    }

    // early out for tracer
    if (beam.is_tracer)
        return;

    // now affect items
    if (!drawOnly)
        affect_items( beam, realx, realy );

    if (drawOnly)
    {
        int drawx = realx - you.x_pos + 18;
        int drawy = realy - you.y_pos + 9;

        if (see_grid( realx, realy )
            || (realx == you.x_pos && realy == you.y_pos))
        {
            // bounds check
            if (drawx > 8 && drawx < 26 && drawy > 0 && drawy < 18)
            {
                if (beam.colour == BLACK)
                    textcolor( element_colour( EC_RANDOM ) );
                else
                    textcolor( beam.colour );

                gotoxy( drawx, drawy );
                putch( SYM_EXPLOSION );
            }
        }
    }
}

static void explosion_map( struct bolt &beam, int x, int y,
                           int count, int dir, int r )
{
    // 1. check to see out of range
    if (x * x + y * y > r * r + r)
        return;

    // 2. check count
    if (count > 10 * r)
        return;

    // 3. check to see if we're blocked by something
    //    specifically,  we're blocked by WALLS.  Not
    //    statues, idols, etc.
    int dngn_feat = grd[beam.target_x + x][beam.target_y + y];

    // special case: explosion originates from rock/statue
    // (e.g. Lee's rapid deconstruction) - in this case, ignore
    // solid cells at the center of the explosion.
    if (dngn_feat < DNGN_GREEN_CRYSTAL_WALL || dngn_feat == DNGN_WAX_WALL)
    {
        if (x != 0 || y != 0)
            return;
    }

    // hmm, I think we're ok
    explode_map[x + 9][y + 9] = true;

    // now recurse in every direction except the one we
    // came from
    for (int i = 0; i < 4; i++)
    {
        if (i + 1 != dir)
        {
            int cadd = 5;
            if (x * spreadx[i] < 0 || y * spready[i] < 0)
                cadd = 17;

            explosion_map( beam, x + spreadx[i], y + spready[i],
                           count + cadd, opdir[i], r );
        }
    }
}

// returns true if the beam is harmful (ignoring monster
// resists) -- mon is given for 'special' cases where,
// for example,  "Heal" might actually hurt undead, or
// "Holy Word" being ignored by holy monsters,  etc.
//
// only enchantments should need the actual monster type
// to determine this;  non-enchantments are pretty
// straightforward.
bool nasty_beam( struct monsters *mon, struct bolt &beam )
{
    // take care of non-enchantments
    if (!beam.is_enchant)
        return (true);

    // now for some non-hurtful enchantments

    // degeneration / sleep
    if (beam.flavour == BEAM_DEGENERATE || beam.flavour == BEAM_SLEEP)
        return (mons_holiness( mon ) == MH_NATURAL);

    // dispel undead / control undead
    if (beam.flavour == BEAM_DISPEL_UNDEAD || beam.flavour == BEAM_ENSLAVE_UNDEAD)
        return (mons_holiness( mon ) == MH_UNDEAD);

    // pain/agony
    if (beam.flavour == BEAM_PAIN)
        return (!mons_res_negative_energy( mon ));

    // control demon
    if (beam.flavour == BEAM_ENSLAVE_DEMON)
        return (mons_holiness( mon ) == MH_DEMONIC);

    // haste
    if (beam.flavour == BEAM_HASTE)
        return (false);

    // healing
    if (beam.flavour == BEAM_HEALING || beam.flavour == BEAM_INVISIBILITY)
        return (false);

    // everything else is considered nasty by everyone
    return (true);
}

/*
 *  File:       it_use2.cc
 *  Summary:    Functions for using wands, potions, and weapon/armor removal.4\3
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      26jun2000   jmf   added ZAP_MAGMA
 *  <4> 19mar2000   jmf   Added ZAP_BACKLIGHT and ZAP_SLEEP
 *  <3>     10/1/99         BCR             Changed messages for speed and
 *                                          made amulet resist slow up speed
 *  <2>     5/20/99         BWR             Fixed bug with RAP_METABOLISM
 *                                          and RAP_NOISES artefacts/
 *  <1>     -/--/--         LRH             Created
 */

#include "AppHdr.h"
#include "it_use2.h"

#include <string.h>

#include "externs.h"

#include "beam.h"
#include "effects.h"
#include "food.h"
#include "itemname.h"
#include "misc.h"
#include "mutation.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "skills2.h"
#include "spells.h"
#include "spells2.h"
#include "stuff.h"
#include "view.h"

extern bool wield_change;       // defined in output.cc

static void zappy(char z_type, int power, struct bolt &pbolt);

void zapping(char ztype, int power, struct bolt &pbolt)
{
    // GDL: note that rangeMax is set to 0, which means that max range is
    // equal to range.  This is OK,  since rangeMax really only matters for
    // stuff monsters throw/zap.

    // all of the following might be changed by zappy():
    pbolt.range = 8 + random2(5);       // default for "0" beams (I think)
    pbolt.rangeMax = 0;
    pbolt.damage = power;
    pbolt.hit = 0;                      // default for "0" beams (I think)
    pbolt.type = 0;                     // default for "0" beams
    pbolt.flavour = BEAM_MAGIC;         // default for "0" beams
    pbolt.ench_power = power;
    pbolt.obviousEffect = false;
    pbolt.isBeam = false;               // default for all beams.
    pbolt.isTracer = false;             // default for all player beams
    pbolt.thrower = KILL_YOU_MISSILE;   // missile from player

    // fill in the bolt structure
    zappy(ztype, power, pbolt);

    if (ztype == ZAP_LIGHTNING && !silenced(you.x_pos, you.y_pos))
        // needs to check silenced at other location, too {dlb}
    {
        mpr("You hear a mighty clap of thunder!");
        noisy(25, you.x_pos, you.y_pos);
    }

    beam(pbolt);

    return;
}                               // end zapping()

// *do not* call this function directly (duh - it's static), need to
// see zapping() for default values not set within this function {dlb}
static void zappy(char z_type, int power, struct bolt &pbolt)
{
    int temp_rand = 0;          // probability determination {dlb}

    // Applying some power caps so we can have some idea of what the
    // spells are capable of on the high end. -- bwr

    // In general, it's better not to change the caps, but to adjust
    // the spells below.  If you blindly adjust the caps you can
    // easily warp the balance so that first level spells become
    // dragon-slaying weapons of choice... this is because the
    // caps on the low level spells (which are likely to come
    // into effect during the game) are used to prevent them from
    // getting overly powerful (this allows low level spells to
    // develop at a fast enough rate to mean something to low
    // level characters, rather than having them develop slower
    // than high level spells just to keep their damage down so
    // that they don't become the obvious best choice for 400+ power
    // mages (who would be getting as much as 70pts for a single
    // sting... pretty good for one mana, little food, and
    // an easy spell to cast)).
    switch (z_type)
    {
    // level 1
    // This cap can affect mid-level characters (and gives them a good
    // reason to move onto spells that can support more power).
    case ZAP_MAGIC_DARTS:
    case ZAP_PEBBLE:
    case ZAP_STING:
    case ZAP_ELECTRICITY:
    case ZAP_FLAME_TONGUE:
    case ZAP_SMALL_SANDBLAST:
    case ZAP_DISRUPTION:                // ench_power boosted below
    case ZAP_PAIN:                      // ench_power boosted below
        if (power > 50)
            power = 50;
        break;

    // level 2
    // This cap should only affect high level characters (it's actually
    // pretty hard to get this high without intentionally trying to play
    // a strong spellcaster and they should really want to get into the
    // powerful spells below).
    case ZAP_FLAME:
    case ZAP_FROST:
    case ZAP_SANDBLAST:
        if (power > 100)
            power = 100;
        break;

    // level 3/4
    // This cap might actually come into effect (we're talking 20's in
    // the skills involved (inc spellcasting) with INT 20 and a spell
    // magnifier (stave or ring) to start breaking this point).
    case ZAP_STONE_ARROW:
    case ZAP_ORB_OF_ENERGY:
    case ZAP_STICKY_FLAME:
    case ZAP_ICE_BOLT:
        if (power > 150)
            power = 150;
        break;

    // powerful bolts and balls (levels 5-7)
    // This cap will probably seldom get used unless the character is
    // playing a full-mage style.
    case ZAP_FIRE:
    case ZAP_COLD:
    case ZAP_VENOM_BOLT:
    case ZAP_MAGMA:
    case ZAP_AGONY:
    case ZAP_LIGHTNING:
    case ZAP_NEGATIVE_ENERGY:
    case ZAP_IRON_BOLT:
    case ZAP_DISINTEGRATION:
    case ZAP_FIREBALL:
    case ZAP_ORB_OF_ELECTRICITY:
    case ZAP_ORB_OF_FRAGMENTATION:
        if (power > 200)
            power = 200;
        break;

    // extremely powerful spells (levels 8-9)
    // This cap will probably never really come into effect except for
    // very few characters who try for it (you need high skills, some
    // items to magnify your magic, and a high INT to get this type
    // of number).
    case ZAP_CRYSTAL_SPEAR:
    case ZAP_HELLFIRE:
    case ZAP_ICE_STORM:
    case ZAP_CLEANSING_FLAME:
        if (power > 300)
            power = 300;
        break;

    // unlimited power (needs a good reason)
    case ZAP_BONE_SHARDS:    // power limited to 5250 before getting here
    case ZAP_BEAM_OF_ENERGY: // inaccuracy (only on staff, hardly hits)
        break;

    // enchantments and other resistable effects
    case ZAP_SLOWING:
    case ZAP_HASTING:
    case ZAP_PARALYSIS:
    case ZAP_BACKLIGHT:
    case ZAP_SLEEP:
    case ZAP_CONFUSION:
    case ZAP_INVISIBILITY:
    case ZAP_ENSLAVEMENT:
    case ZAP_ENSLAVE_UNDEAD:
    case ZAP_HEALING:
    case ZAP_TELEPORTATION:
    case ZAP_DIGGING:
    case ZAP_POLYMORPH_OTHER:
    case ZAP_DISPEL_UNDEAD:
    case ZAP_DEGENERATION:
    case ZAP_BANISHMENT:
        // This is the only power that matters.  We magnify it apparently
        // to get values that work better with magic resistance checks...
        // those checks will scale down this value and max it out at 120.
        pbolt.ench_power *= 3;
        pbolt.ench_power /= 2;
        break;

    // natural/mutant breath/spit powers and anything else...
    case ZAP_SPIT_POISON:
    case ZAP_BREATHE_FIRE:
    case ZAP_BREATHE_FROST:
    case ZAP_BREATHE_ACID:
    case ZAP_BREATHE_POISON:
    case ZAP_BREATHE_POWER:
    case ZAP_BREATHE_STEAM:
    default:
        if (power > 100)
            power = 100;
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
    case ZAP_FLAME:
        strcpy(pbolt.beam_name, "puff of flame");
        pbolt.colour = RED;
        pbolt.range = 8 + random2(5);
        pbolt.damage = 104 + (power / 20);              // max dam: 24
        pbolt.hit = 8 + (power / 10);                   // max hit: 18
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_FIRE;

        pbolt.obviousEffect = true;
        break;

    case ZAP_FROST:
        strcpy(pbolt.beam_name, "puff of frost");
        pbolt.colour = WHITE;
        pbolt.range = 8 + random2(5);
        pbolt.damage = 104 + (power / 20);              // max dam: 24
        pbolt.hit = 8 + (power / 10);                   // max hit: 18
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_COLD;

        pbolt.obviousEffect = true;
        break;

    case ZAP_SLOWING:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = BLACK;
        pbolt.isBeam = true;
        break;

    case ZAP_HASTING:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = BLUE;
        pbolt.isBeam = true;
        break;

    case ZAP_MAGIC_DARTS:
        strcpy(pbolt.beam_name, "magic dart");
        pbolt.colour = LIGHTMAGENTA;
        pbolt.range = random2(5) + 8;
        pbolt.damage = 102 + (power / 15);              // max dam: 12
        pbolt.hit = 1500;                               // hits always
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable
        pbolt.obviousEffect = true;
        break;

    case ZAP_HEALING:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = GREEN;
        pbolt.isBeam = true;
        break;

    case ZAP_PARALYSIS:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = CYAN;
        pbolt.isBeam = true;
        break;

    case ZAP_FIRE:
        strcpy(pbolt.beam_name, "bolt of fire");
        pbolt.colour = RED;
        pbolt.range = 7 + random2(10);
        pbolt.damage = 107 + (power / 11);              // max dam: 72
        pbolt.hit = 10 + (random2(power) / 25);         // max hit: 17
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_FIRE;

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_COLD:
        strcpy(pbolt.beam_name, "bolt of cold");
        pbolt.colour = WHITE;
        pbolt.range = 7 + random2(10);
        pbolt.damage = 107 + (power / 11);              // max dam: 72
        pbolt.hit = 10 + (random2(power) / 25);         // max hit: 17
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_COLD;

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_MAGMA:
        strcpy(pbolt.beam_name, "bolt of magma");
        pbolt.colour = RED;
        pbolt.range = 5 + random2(4);
        pbolt.damage = 106 + (power / 12);              // max dam: 63
        pbolt.hit = 8 + (random2(power) / 25);          // max hit: 15
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_LAVA;

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_CONFUSION:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = RED;
        pbolt.isBeam = true;
        break;

    case ZAP_INVISIBILITY:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = MAGENTA;
        pbolt.isBeam = true;
        break;

    case ZAP_DIGGING:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = BROWN;
        // not ordinary "0" beam range {dlb}
        pbolt.range = 3 + random2(power) + random2(5);
        pbolt.isBeam = true;
        break;

    case ZAP_FIREBALL:
        strcpy(pbolt.beam_name, "fireball");
        pbolt.colour = RED;
        pbolt.range = 8 + random2(5);
        pbolt.damage = 105 + (power / 7);               // max dam: 96
        pbolt.hit = 40;                                 // hit: 40
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_EXPLOSION;                 // fire

        break;

    case ZAP_TELEPORTATION:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = LIGHTGREY;
        pbolt.range = 9 + random2(5);
        pbolt.isBeam = true;
        break;

    case ZAP_LIGHTNING:
        strcpy(pbolt.beam_name, "bolt of lightning");
        pbolt.colour = LIGHTCYAN;
        pbolt.range = 8 + random2(10);                  // extended in beam
        pbolt.damage = 106 + (power / 12);              // max dam: 63
        pbolt.hit = 7 + (random2(power) / 20);          // max hit: 16
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_ELECTRICITY;               // beams & reflects

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_POLYMORPH_OTHER:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = DARKGREY;
        pbolt.range = 9 + random2(5);
        pbolt.isBeam = true;
        break;

    case ZAP_VENOM_BOLT:
        strcpy(pbolt.beam_name, "bolt of poison");
        pbolt.colour = LIGHTGREEN;
        pbolt.range = 8 + random2(10);
        pbolt.damage = 105 + (power / 14);              // max dam: 54
        pbolt.hit = 8 + (random2(power) / 20);          // max hit: 17
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_POISON;                    // extra damage

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_NEGATIVE_ENERGY:
        strcpy(pbolt.beam_name, "bolt of negative energy");
        pbolt.colour = DARKGREY;
        pbolt.range = 7 + random2(10);
        pbolt.damage = 105 + (power / 14);              // max dam: 54
        pbolt.hit = 8 + (random2(power) / 20);          // max hit: 17
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_NEG;                       // drains levels

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_BEAM_OF_ENERGY:    // bolt of innacuracy
        strcpy(pbolt.beam_name, "narrow beam of energy");
        pbolt.colour = YELLOW;
        pbolt.range = 7 + random2(10);
        pbolt.damage = 110 + (power / 5);               // max dam: unlimited
        pbolt.hit = 2;                                  // hit: 2 (very hard)
        pbolt.type = SYM_ZAP;
        pbolt.flavour = 17;    // whatever

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_ORB_OF_ENERGY:     // Mystic Bolt
        strcpy(pbolt.beam_name, "orb of energy");
        pbolt.colour = LIGHTMAGENTA;
        pbolt.range = 7 + random2(5);
        pbolt.damage = 105 + (power / 20);              // max dam: 33
        pbolt.hit = 10 + (power / 7);                   // max hit: 31
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable

        pbolt.obviousEffect = true;
        break;

    case ZAP_ENSLAVEMENT:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = LIGHTBLUE;
        pbolt.range = 7 + random2(5);
        pbolt.isBeam = true;
        break;

    case ZAP_PAIN:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = LIGHTMAGENTA;
        pbolt.range = 7 + random2(8);

        // is this right? other "0" beams do not define ->hit {dlb}
        // actually, it's used for damage (the spell always hits)
        pbolt.hit = 8 + (power / 5);                   // max dam: 17

        pbolt.ench_power *= 7;
        pbolt.ench_power /= 2;
        pbolt.isBeam = true;
        break;

    case ZAP_STICKY_FLAME:
        strcpy(pbolt.beam_name, "sticky flame");        // extra damage
        pbolt.colour = RED;
        pbolt.range = 8 + random2(5);
        pbolt.damage = 102 + (power / 30);              // max dam: 18
        pbolt.hit = 11 + (power / 10);                  // max hit: 26
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_FIRE;

        pbolt.obviousEffect = true;
        break;

    case ZAP_DISPEL_UNDEAD:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = YELLOW;
        pbolt.range = 7 + random2(8);
        // is this right? other "0" beams do not define ->hit {dlb}
        pbolt.hit = 9 + (power / 5);
        pbolt.isBeam = true;
        break;

    case ZAP_CLEANSING_FLAME:
        strcpy(pbolt.beam_name, "golden flame");
        pbolt.colour = YELLOW;
        pbolt.range = 7 + random2(10);
        pbolt.damage = 110 + (power / 7);               // max dam: 153
        pbolt.hit = 20 + (random2(power) / 80);         // max hit: 23
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_HOLY;

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_BONE_SHARDS:
        strcpy(pbolt.beam_name, "spray of bone shards");
        pbolt.colour = LIGHTGREY;
        pbolt.range = 7 + random2(10);

        // note that f_p[2] has a high value for this spell
        // The power here is quite large because it contains a
        // factor based on the size of the skeleton used... therefore
        // damage is highly dependant on material.
        pbolt.damage = 102 + (power / 250);             // max dam: 66
        pbolt.hit = 5 + (power / 100);                  // max hit: 57 (high)
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_MAGIC;                     // unresisted

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_BANISHMENT:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = LIGHTGREEN;
        pbolt.range = 7 + random2(5);
        pbolt.isBeam = true;
        break;

    case ZAP_DEGENERATION:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = LIGHTCYAN;
        pbolt.range = 7 + random2(5);
        pbolt.isBeam = true;
        break;

    case ZAP_STING:
        strcpy(pbolt.beam_name, "sting");
        pbolt.colour = GREEN;
        pbolt.range = 8 + random2(5);
        pbolt.damage = 4 + (power / 6);                 // max dam: 11
        pbolt.hit = 8 + (power / 20);                   // max hit: 10
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_POISON;                    // extra damage

        pbolt.obviousEffect = true;
        break;

    case ZAP_HELLFIRE:
        strcpy(pbolt.beam_name, "hellfire");
        pbolt.colour = RED;
        pbolt.range = 7 + random2(10);
        pbolt.damage = 110 + (power / 10);              // max dam: 120
        pbolt.hit = 20 + (power / 10);                  // max hit: 50
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_EXPLOSION;

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_PEBBLE:
        strcpy(pbolt.beam_name, "pebble");
        pbolt.colour = BROWN;
        pbolt.range = 8 + random2(5);
        pbolt.damage = 5;                               // dam: 5
        pbolt.hit = 8 + (power / 20);                   // max hit: 10
        pbolt.type = SYM_MISSILE;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable

        pbolt.obviousEffect = true;
        break;

    case ZAP_STONE_ARROW:
        strcpy(pbolt.beam_name, "stone arrow");
        pbolt.colour = LIGHTGREY;
        pbolt.range = 8 + random2(5);
        pbolt.damage = 105 + (power / 20);              // max dam: 33
        pbolt.hit = 7 + (power / 20);                   // max hit: 14
        pbolt.type = SYM_MISSILE;                       // unresistable
        pbolt.flavour = BEAM_MMISSILE;

        pbolt.obviousEffect = true;
        break;

    case ZAP_IRON_BOLT:
        strcpy(pbolt.beam_name, "iron bolt");
        pbolt.colour = LIGHTCYAN;
        pbolt.range = 4 + random2(5);
        pbolt.damage = 108 + (power / 12);              // max dam: 69
        pbolt.hit = 5 + (random2(power) / 25);          // max hit: 13
        pbolt.type = SYM_MISSILE;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable

        pbolt.obviousEffect = true;
        break;

    case ZAP_CRYSTAL_SPEAR:
        strcpy(pbolt.beam_name, "crystal spear");
        pbolt.colour = WHITE;
        pbolt.range = 7 + random2(10);
        pbolt.damage = 112 + (power / 9);               // max dam: 132
        pbolt.hit = 4 + random2(power) / 30;            // max hit: 14
        pbolt.type = SYM_MISSILE;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable

        pbolt.obviousEffect = true;
        break;

    case ZAP_ELECTRICITY:
        strcpy(pbolt.beam_name, "zap");
        pbolt.colour = LIGHTCYAN;
        pbolt.range = 6 + random2(8);                   // extended in beam
        pbolt.damage = 7 + random2(power) / 12;         // max dam: 11
        pbolt.hit = 8 + power / 20;                     // max hit: 10
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_ELECTRICITY;               // beams & reflects

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_ORB_OF_ELECTRICITY:
        strcpy(pbolt.beam_name, "orb of electricity");
        pbolt.colour = LIGHTBLUE;
        pbolt.range = 9 + random2(12);
        pbolt.damage = 105 + (power / 6);               // max dam: 111
        pbolt.hit = 40;                                 // hit: 40
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_ELECTRICITY;

        break;

    case ZAP_SPIT_POISON:
        // power is different here (level based, I believe)
        strcpy(pbolt.beam_name, "splash of poison");
        pbolt.colour = GREEN;
        pbolt.range = 3 + random2(1 + (power / 2));
        pbolt.damage = 4 + (power / 2);
        pbolt.hit = 5 + random2(1 + (power / 3));
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_POISON;

        pbolt.obviousEffect = true;
        break;

    case ZAP_DEBUGGING_RAY:
        strcpy(pbolt.beam_name, "debugging ray");
        pbolt.colour = random_colour();
        pbolt.range = 7 + random2(10);
        pbolt.damage = 150;                             // dam: 150
        pbolt.hit = 60;                                 // hit: 60
        pbolt.type = SYM_DEBUG;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable

        pbolt.obviousEffect = true;
        break;

    case ZAP_BREATHE_FIRE:
        strcpy(pbolt.beam_name, "fiery breath");
        pbolt.colour = RED;
        pbolt.range = 3 + random2(1 + (power / 2));
        pbolt.damage = 104 + (power / 3);  // NB: experience_level + mut * 4
        pbolt.hit = 8 + random2(1 + (power / 3));
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_FIRE;

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_BREATHE_FROST:
        strcpy(pbolt.beam_name, "freezing breath");
        pbolt.colour = WHITE;
        pbolt.range = 3 + random2(1 + (power / 2));
        pbolt.damage = 104 + (power / 3);  // NB: experience_level + mut * 4
        pbolt.hit = 8 + random2(1 + (power / 3));
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_COLD;

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_BREATHE_ACID:
        strcpy(pbolt.beam_name, "acid");
        pbolt.colour = YELLOW;
        pbolt.range = 3 + random2(1 + (power / 2));
        pbolt.damage = 103 + (power / 3);
        pbolt.hit = 5 + random2(1 + (power / 3));
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_ACID;

        pbolt.obviousEffect = true;
        break;

    case ZAP_BREATHE_POISON:    // leaves clouds of gas
        strcpy(pbolt.beam_name, "poison gas");
        pbolt.colour = GREEN;
        pbolt.range = 3 + random2(1 + (power / 2));
        pbolt.damage = 102 + (power / 6);
        pbolt.hit = 5 + random2(1 + (power / 3));
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_POISON;

        pbolt.obviousEffect = true;
        break;

    case ZAP_BREATHE_POWER:
        strcpy(pbolt.beam_name, "bolt of energy");

        pbolt.colour = BLUE;
        if (random2(power) >= 8)
            pbolt.colour = LIGHTBLUE;
        if (random2(power) >= 12)
            pbolt.colour = MAGENTA;
        if (random2(power) >= 17)
            pbolt.colour = LIGHTMAGENTA;

        pbolt.range = 6 + random2(1 + (power / 2));
        pbolt.damage = 103 + (power / 3);
        pbolt.hit = 11 + random2(1 + (power / 3));
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_MMISSILE;                  // unresistable

        pbolt.obviousEffect = true;
        break;

    case ZAP_ENSLAVE_UNDEAD:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = LIGHTRED;
        pbolt.range = 7 + random2(5);
        pbolt.isBeam = true;
        break;

    case ZAP_AGONY:
        strcpy(pbolt.beam_name, "0agony");
        pbolt.colour = LIGHTMAGENTA;
        pbolt.range = 7 + random2(8);
        // is this right? other "0" beams do not define ->hit {dlb}
        pbolt.hit = 6 + (power / 50);
        pbolt.ench_power *= 5;
        pbolt.isBeam = true;
        break;

    case ZAP_DISRUPTION:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = WHITE;
        pbolt.range = 7 + random2(8);
        // is this right? other "0" beams do not define ->hit {dlb}
        // actually, it's used for damage (the spell always hits)
        pbolt.hit = 5 + (power / 5);                   // max dam: 14
        pbolt.ench_power *= 3;
        pbolt.isBeam = true;
        break;

    case ZAP_DISINTEGRATION:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = WHITE;
        pbolt.range = 7 + random2(8);
        // is this right? other "0" beams do not define ->hit {dlb}
        // actually, it's used for damage (the spell always hits)
        pbolt.hit = 15 + (power / 3);                   // max dam: 80
        pbolt.ench_power *= 5;
        pbolt.ench_power /= 2;
        pbolt.isBeam = true;
        break;

    case ZAP_BREATHE_STEAM:
        strcpy(pbolt.beam_name, "ball of steam");
        pbolt.colour = LIGHTGREY;
        pbolt.range = 6 + random2(5);
        pbolt.damage = 104 + (power / 5);
        pbolt.hit = 10 + random2(1 + (power / 5));
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_FIRE;
        pbolt.obviousEffect = true;
        break;

    case ZAP_CONTROL_DEMON:
        strcpy(pbolt.beam_name, "0");
        pbolt.colour = 16;     // this is not a color !!! {dlb}
        pbolt.range = 7 + random2(5);
        pbolt.ench_power *= 17;
        pbolt.ench_power /= 10;
        pbolt.isBeam = true;
        break;

    case ZAP_ORB_OF_FRAGMENTATION:
        strcpy(pbolt.beam_name, "metal orb");
        pbolt.colour = CYAN;
        pbolt.range = 9 + random2(7);
        pbolt.damage = 110 + (power / 7);               // max dam: 111
        pbolt.hit = 20;                                 // hit: 20
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_MMISSILE;
        break;

    case ZAP_ICE_BOLT:
        strcpy(pbolt.beam_name, "bolt of ice");
        pbolt.colour = WHITE;
        pbolt.range = 8 + random2(5);
        pbolt.damage = 105 + (power / 10);              // max dam: 57
        pbolt.hit = 9 + (power / 12);                   // max hit: 21
        pbolt.type = SYM_ZAP;
        pbolt.flavour = BEAM_ICE;
        break;

    case ZAP_ICE_STORM:
        strcpy(pbolt.beam_name, "great blast of cold");
        pbolt.colour = BLUE;
        pbolt.range = 9 + random2(5);
        pbolt.damage = 105 + (power / 7);               // max dam: 138
        pbolt.hit = 10 + (power / 10);                  // max hit: 40
        pbolt.type = SYM_ZAP;
        /* ice */// <- changed from BEAM_COLD b/c of comment 13jan2000 {dlb}
        pbolt.flavour = BEAM_ICE;
        break;

    case ZAP_SLEEP:             //jmf: added
        strcpy(pbolt.beam_name, "0");
        pbolt.flavour = BEAM_SLEEP;
        pbolt.range = 7 + random2(5);
        pbolt.isBeam = true;
        break;

    case ZAP_BACKLIGHT: //jmf: added
        strcpy(pbolt.beam_name, "0");
        pbolt.flavour = BEAM_BACKLIGHT;
        pbolt.range = 7 + random2(5);
        pbolt.isBeam = true;
        break;

    case ZAP_FLAME_TONGUE:      //jmf: ought to be a weak, short-range missile
        strcpy(pbolt.beam_name, "flame");
        pbolt.colour = RED;

        pbolt.range = 1 + random2(2) + random2(power) / 20;
        if (pbolt.range > 4)
            pbolt.range = 4;

        pbolt.damage = 5 + (power / 4);                // max dam: 16
        pbolt.hit = 8 + (power / 10);                  // max hit: 13
        pbolt.type = SYM_BOLT;
        pbolt.flavour = BEAM_FIRE;

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_SANDBLAST: //jmf: ought to be a weak, short-range missile
        strcpy(pbolt.beam_name, coinflip() ? "blast of sand" : "rocky blast");
        pbolt.colour = BROWN;

        pbolt.range = 2 + random2(power) / 20;
        if (pbolt.range > 4)
            pbolt.range = 4;

        // remember -- this one has max power of 100
        pbolt.damage = 6 + (power / 7);                // max dam: 19
        pbolt.hit = 10 + (power / 10);                 // max hit: 20
        pbolt.type = SYM_BOLT;
        pbolt.flavour = BEAM_FRAG;

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;

    case ZAP_SMALL_SANDBLAST:   //jmf: ought to be a weak, short-range missile
        strcpy(pbolt.beam_name, "blast of ");

        temp_rand = random2(4);

        strcat(pbolt.beam_name, (temp_rand == 0) ? "dust" :
                                (temp_rand == 1) ? "dirt" :
                                (temp_rand == 2) ? "grit" : "sand");

        pbolt.colour = BROWN;
        pbolt.range = (random2(power) > random2(50)) ? 2 : 1; // 1 or 2 squares
        pbolt.damage = 4 + (power / 10);               // max dam: 8
        pbolt.hit = 5 + (power / 10);                  // max hit: 10
        pbolt.type = SYM_BOLT;
        pbolt.flavour = BEAM_FRAG;

        pbolt.obviousEffect = true;
        pbolt.isBeam = true;
        break;
    }                           // end of switch
}                               // end zappy()

void potion_effect(char pot_eff, int pow)
{

    int new_value = 0;
    unsigned char i;

    if (pow > 150)
        pow = 150;

    switch (pot_eff)
    {
    case POT_HEALING:
        mpr("You feel better.");
        inc_hp(5 + random2(7), false);

        if (you.hp >= you.hp_max)
        {
            if (you.base_hp < 5000)
                you.base_hp++;

            calc_hp();
            set_hp(you.hp_max, false);
        }

        you.poison = 0;
        you.rotting = 0;
        you.disease = 0;
        you.conf = 0;
        break;

    case POT_HEAL_WOUNDS:
        mpr("You feel much better.");
        inc_hp(10 + random2avg(28, 3), false);

        if (you.hp >= you.hp_max)
        {
            if (you.base_hp < 5000)
                you.base_hp += 2 + random2avg(5, 2);

            if (you.base_hp > 5000)
                you.base_hp = 5000;

            calc_hp();
            set_hp(you.hp_max, false);
        }
        break;

    case POT_SPEED:
        if (you.haste > 90)
            mpr("You already have as much speed as you can handle.");
        else if (wearing_amulet(AMU_RESIST_SLOW))
        {
            mpr("Your amulet glows brightly and you gain speed.");

            if (you.slow)
            {
                you.slow = 0;
                you.haste = 10;
            }
            else
                you.haste += 50 + random2(pow);
        }
        else if (!you.haste)
        {
            mpr("You feel yourself speed up.");

            if (you.slow)
                you.slow = 0;
            else
                you.haste += 40 + random2(pow);
        }
        else
        {
            mpr("You feel as though your speed will last longer.");
            you.haste += 40 + random2(pow);
        }

        if (wearing_amulet(AMU_RESIST_SLOW))
        {
            if (you.haste > 100)
                you.haste = 100;
        }
        else
        {
            if (you.haste > 90)
                you.haste = 90;
        }

        naughty(NAUGHTY_STIMULANTS, 4 + random2(4));
        break;

    case POT_MIGHT:
        {
            bool were_mighty = you.might > 0;

            strcpy(info, "You feel ");
            strcat(info, (you.might) ? "pretty" : "very");
            strcat(info, " mighty");
            strcat(info, (you.might) ? ", still" : " all of a sudden");
            strcat(info, ".");
            mpr(info);

            // conceivable max gain of +184 {dlb}
            you.might += 35 + random2(pow);

            if (!were_mighty)
            {
                modify_stat(STAT_STRENGTH, 5, true);

                if (you.might > 75)
                    you.might = 75;
            }
            else
            {
                // files.cc permits values up to 215, but ... {dlb}
                if (you.might > 150)
                    you.might = 150;
            }

            naughty(NAUGHTY_STIMULANTS, 4 + random2(4));
        }
        break;

    case POT_GAIN_STRENGTH:
        mutate(MUT_STRONG);
        break;

    case POT_GAIN_DEXTERITY:
        mutate(MUT_AGILE);
        break;

    case POT_GAIN_INTELLIGENCE:
        mutate(MUT_CLEVER);
        break;

    case POT_LEVITATION:
        strcpy(info, "You feel");
        strcat(info, (!you.levitation) ? " very" : " more");
        strcat(info, " buoyant.");
        mpr(info);

        if (!you.levitation)
            mpr("You gently float upwards from the floor.");

        you.levitation += 25 + random2(pow);

        if (you.levitation > 100)
            you.levitation = 100;

        burden_change();
        break;

    case POT_POISON:
    case POT_STRONG_POISON:
        if (player_res_poison())
        {
            strcpy(info, "You feel ");
            strcat(info, (pot_eff == POT_POISON) ? "slightly" : "extremely");
            strcat(info, " nauseous.");
        }
        else
        {
            if (you.poison)
                strcpy(info, "You feel even sicker.");
            else
            {
                strcpy(info, "That liquid tasted ");
                strcat(info, (pot_eff == POT_POISON) ? "very" : "extremely");
                strcat(info, " nasty...");
            }

            you.poison += 1 + ((pot_eff == POT_POISON) ?
                               random2avg(5, 2) : 2 + random2avg(13, 2));
        }

        mpr(info);
        break;

    case POT_SLOWING:
        if (wearing_amulet(AMU_RESIST_SLOW))
            mpr("You feel momentarily lethargic.");
        else if (you.slow > 90)
            mpr("You already are as slow as you could be.");
        else
        {
            if (!you.slow)
                mpr("You feel yourself slow down.");
            else
                mpr("You feel as though you will be slow longer.");

            you.slow += 10 + random2(pow);

            if (you.slow > 90)
                you.slow = 90;
        }
        break;

    case POT_PARALYSIS:
        strcpy(info, "You ");
        strcat(info, (you.paralysis) ? "still haven't" : "suddenly lose");
        strcat(info, " the ability to move!");
        mpr(info);

        new_value = 2 + random2(6 + you.paralysis);

        if (new_value > you.paralysis)
            you.paralysis = new_value;

        if (you.paralysis > 13)
            you.paralysis = 13;
        break;

    case POT_CONFUSION:
        strcpy(info, "You feel ");

        if (wearing_amulet(AMU_CLARITY))
            strcat(info, "momentarilty ");
        else if (you.conf)
            strcat(info, "very ");

        strcat(info, "confused.");
        mpr(info);

        // early return for the protected: {dlb}
        if (wearing_amulet(AMU_CLARITY))
            break;

        new_value = 3 + random2(8 + you.conf);

        if (new_value > you.conf)
            you.conf = new_value;

        if (you.conf > 20)
            you.conf = 20;
        break;

    case POT_INVISIBILITY:
        mpr( (!you.invis) ? "You fade into invisibility!"
                          : "You feel safely hidden away." );

        // should really scale this by you.invis {dlb}
        you.invis += 15 + random2(pow);

        if (you.invis > 100)
            you.invis = 100;
        break;

    // carnivore check here? {dlb}
    case POT_PORRIDGE:          // oatmeal - always gluggy white/grey?
        mpr("That potion was really gluggy!");
        lessen_hunger(6000, true);
        break;

    case POT_DEGENERATION:
        mpr("There was something very wrong with that liquid!");
        lose_stat(STAT_RANDOM, 1 + random2avg(4, 2));
        break;

    // Don't generate randomly - should be rare and interesting
    case POT_DECAY:
        strcpy(info, "You feel ");
        strcat(info, (you.is_undead) ? "terrible."
               : "your flesh start to rot away!");
        mpr(info);

        if (!you.is_undead)
            you.rotting += 10 + random2(10);
        break;

    case POT_WATER:
        mpr("This tastes like water.");
        // we should really separate thirst from hunger {dlb}
        // Thirst would just be annoying for the player, the
        // 20 points here doesn't represesent real food anyways -- bwr
        lessen_hunger(20, true);
        break;

    case POT_EXPERIENCE:
        if (you.experience_level < 27)
        {
            mpr("You feel more experienced!");

            you.experience = 1 + exp_needed( 2 + you.experience_level );
            level_change();
        }
        else
            mpr("A flood of memories washes over you.");
        break;                  // I'll let this slip past robe of archmagi

    case POT_MAGIC:
        mpr("You feel magical!");
        inc_mp(5 + random2avg(19, 2), false);
        break;

    case POT_RESTORE_ABILITIES:
        // messaging taken care of within function {dlb}
        // not quite true... if no stat's are restore = no message, and
        // that's just confusing when trying out random potions (this one
        // still auto-identifies so we know what the effect is, but it
        // shouldn't require bringing up the descovery screen to do that -- bwr
        if (restore_stat(STAT_ALL, false) == false)
            mpr( "You feel refreshed." );
        break;

    case POT_BERSERK_RAGE:
        go_berserk(true);
        break;

    case POT_CURE_MUTATION:
        mpr("It has a very clean taste.");
        for (i = 0; i < 7; i++)
        {
            if (random2(10) > i)
                delete_mutation(100);
        }
        break;

    case POT_MUTATION:
        mpr("You feel extremely strange.");
        for (i = 0; i < 3; i++)
        {
            mutate(100, false);
        }

        naughty(NAUGHTY_STIMULANTS, 4 + random2(4));
        break;
    }

    return;
}                               // end potion_effect()

void unwield_item(char unw)
{
    you.special_wield = SPWLD_NONE;
    wield_change = true;

    if (you.inv_class[unw] == OBJ_WEAPONS)
    {
        if (you.inv_dam[unw] >= NWPN_SINGING_SWORD)
        {
            switch (you.inv_dam[unw])
            {
            case NWPN_SINGING_SWORD:
                mpr("The Singing Sword sighs.");
                break;
            case NWPN_WRATH_OF_TROG:
                mpr("You feel less violent.");
                break;
            case NWPN_SCYTHE_OF_CURSES:
            case NWPN_STAFF_OF_OLGREB:
                you.inv_plus[unw] = 50;
                you.inv_plus2[unw] = 50;
                break;
            case NWPN_STAFF_OF_WUCAD_MU:
                you.inv_plus[unw] = 50;
                you.inv_plus2[unw] = 50;
                miscast_effect(SPTYP_DIVINATION, 9, 90, 100);
                break;
            default:
                break;
            }

            you.special_wield = SPWLD_NONE;
            return;
        }

        if (you.inv_dam[unw] != SPWPN_NORMAL)
        {
            if (you.inv_ident[unw] == 1)
                you.inv_ident[unw] = 2;

            in_name(unw, 4, str_pass);
            strcpy(info, str_pass);

            char i_dam = (you.inv_dam[unw] % 30);

            if (you.inv_dam[unw] % 30 >= SPWPN_RANDART_I)
                i_dam = inv_randart_wpn_properties( unw, 0, RAP_BRAND );

            switch (i_dam)
            {
            case SPWPN_FLAMING:
                strcat(info, " stops flaming.");
                mpr(info);
                break;

            case SPWPN_FREEZING:
            case SPWPN_HOLY_WRATH:
                strcat(info, " stops glowing.");
                mpr(info);
                break;

            case SPWPN_ELECTROCUTION:
                strcat(info, " stops crackling.");
                mpr(info);
                break;

            case SPWPN_VENOM:
                strcat(info, " stops dripping with poison.");
                mpr(info);
                break;

            case SPWPN_PROTECTION:
                mpr("You feel less protected.");
                you.redraw_armor_class = 1;
                break;

            case SPWPN_VAMPIRICISM:
                mpr("You feel the strange hunger wane.");
                break;

            /* case 8: draining
               case 9: speed, 10 slicing etc */

            case SPWPN_DISTORTION:
                // Removing the translocations skill reduction of effect,
                // it might seem sensible, but this brand is supposted
                // to be dangerous because it does large bonus damage,
                // as well as free teleport other side effects, and
                // even with the miscast effects you can rely on the
                // occasional spatial bonus to mow down some opponents.
                // It's far too powerful without a real risk, especially
                // if it's to be allowed as a player spell. -- bwr

                // int effect = 9 - random2avg( you.skills[SK_TRANSLOCATIONS] * 2, 2 );
                miscast_effect(SPTYP_TRANSLOCATION, 9, 90, 100);
                break;

                // when more are added here, *must* duplicate unwielding
                // effect in vorpalise weapon scroll effect in read_scoll
            }                   // end switch

            if (you.inv_dam[unw] % 30 >= SPWPN_RANDART_I)
                unuse_randart(unw);

            if (you.duration[DUR_WEAPON_BRAND])
            {
                you.duration[DUR_WEAPON_BRAND] = 0;
                you.inv_dam[unw] -= you.inv_dam[unw] % 30;
                mpr("Your branding evaporates.");
            }
        }                       // end if
    }

    if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES)
    {
        if (you.inv_type[you.equip[EQ_WEAPON]] == STAFF_POWER)
        {
            you.max_magic_points -= 13;
            you.base_magic_points2 -= 13;

            if (you.magic_points > you.max_magic_points)
                you.magic_points = you.max_magic_points;

            calc_mp();
        }
    }

    return;
}                               // end unwield_item()

// This does *not* call ev_mod!
void unwear_armour(char unw)
{
    you.redraw_armor_class = 1;
    you.redraw_evasion = 1;

    switch (you.inv_dam[unw] % 30)
    {
    case SPARM_RUNNING:
        mpr("You feel rather sluggish.");
        break;

    case SPARM_FIRE_RESISTANCE:
        mpr("\"Was it this warm in here before?\"");
        break;

    case SPARM_COLD_RESISTANCE:
        mpr("You catch a bit of a chill.");
        break;

    case SPARM_POISON_RESISTANCE:
        if (!player_res_poison())
            mpr("You feel less healthy.");
        break;

    case SPARM_SEE_INVISIBLE:
        if (!player_see_invis())
            mpr("You feel less perceptive.");
        break;

    case SPARM_DARKNESS:        // I do not understand this {dlb}
        if (you.invis)
            you.invis = 1;
        break;

    case SPARM_STRENGTH:
        modify_stat(STAT_STRENGTH, -3, false);
        break;

    case SPARM_DEXTERITY:
        modify_stat(STAT_DEXTERITY, -3, false);
        break;

    case SPARM_INTELLIGENCE:
        modify_stat(STAT_INTELLIGENCE, -3, false);
        break;

    case SPARM_PONDEROUSNESS:
        mpr("That put a bit of spring back into your step.");
        you.speed -= 2;
        break;

    case SPARM_LEVITATION:
        //you.levitation++;
        if (you.levitation)
            you.levitation = 1;
        break;

    case SPARM_MAGIC_RESISTANCE:
        mpr("You feel less resistant to magic.");
        break;

    case SPARM_PROTECTION:
        mpr("You feel less protected.");
        break;

    case SPARM_STEALTH:
        mpr("You feel less stealthy.");
        break;

    case SPARM_RESISTANCE:
        mpr("You feel hot and cold all over.");
        break;

    case SPARM_POSITIVE_ENERGY:
        mpr("You feel vulnerable.");
        break;

    case SPARM_ARCHMAGI:
        mpr("You feel strangely numb.");
        break;
    }

    if (you.inv_dam[unw] % 30 >= SPARM_RANDART_I)
        unuse_randart(unw);

    return;
}                               // end unwear_armour()

void unuse_randart(unsigned char unw)
{
    if (inv_randart_wpn_properties( unw, 0, RAP_AC ) != 0)
        you.redraw_armor_class = 1;

    if (inv_randart_wpn_properties( unw, 0, RAP_EVASION ) != 0)
        you.redraw_evasion = 1;

    // modify ability scores
    modify_stat(STAT_STRENGTH, -inv_randart_wpn_properties( unw, 0, RAP_STRENGTH ), true);
    modify_stat(STAT_INTELLIGENCE, -inv_randart_wpn_properties( unw, 0, RAP_INTELLIGENCE ), true);
    modify_stat(STAT_DEXTERITY, -inv_randart_wpn_properties( unw, 0, RAP_DEXTERITY ), true);

    if (inv_randart_wpn_properties( unw, 0, RAP_NOISES ) != 0)
        you.special_wield = SPWLD_NONE;
}                               // end unuse_randart()

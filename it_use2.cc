/*
 *  File:       it_use2.cc
 *  Summary:    Functions for using wands, potions, and weapon/armor removal.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
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

extern bool wield_change;    // defined in output.cc


static char zappy(char z_type, int power, struct bolt *pbolt);




void zapping( char ztype, int power, struct bolt *pbolt )
{

    pbolt->range = 9 + random2(5);    // default for "0" beams (I think) - zappy() may overwrite {dlb}
    pbolt->damage = power;            // may be modified in zappy() {dlb}
    pbolt->hit = 0;                   // default for "0" beams (I think) - zappy() may overwrite {dlb}
    pbolt->type = 0;                  // default for "0" beams - zappy() may overwrite {dlb}
    pbolt->flavour = BEAM_MAGIC;      // default for "0" beams - zappy() may overwrite {dlb}
    pbolt->ench_power = power;        // may be modified in zappy() {dlb}
    pbolt->wand_id = 0;               // may be modified in zappy() {dlb}


// see any similarities to the parent function declaration ??? {dlb}
    char luggy = zappy(ztype, power, pbolt);    // whether to call missile() (== 1) or beam() (== 2) {dlb}

    if ( ztype == ZAP_LIGHTNING && !silenced(you.x_pos, you.y_pos) ) // needs to check silenced at other location, too {dlb}
    {
        mpr("You hear a mighty clap of thunder!");
        noisy(25, you.x_pos, you.y_pos);
    }

    pbolt->thing_thrown = KILL_YOU_MISSILE;    // here's a question - zappy sets it, why reset all to this? {dlb}

    switch ( luggy )
    {
      case 1:
        missile(pbolt, 0);
        break;
      case 2:
        beam(pbolt);
        break;
    }

    return;

}          // end zapping()




// *do not* call this function directly (duh - it's static), need to
// see zapping() for default values not set within this function {dlb}
static char zappy( char z_type, int power, struct bolt *pbolt )
{

    int temp_rand = 0;     // probability determination {dlb}


    switch ( z_type )
    {
    case ZAP_FLAME:
        strcpy(pbolt->beam_name, "puff of flame");
        pbolt->colour = RED;
        pbolt->range = 9 + random2(5);
        pbolt->damage = 104 + (power / 35);
        pbolt->hit = 8 + (power / 20);   // = 60;
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_FIRE;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_FROST:
        strcpy(pbolt->beam_name, "puff of frost");
        pbolt->colour = WHITE;
        pbolt->range = 9 + random2(5);
        pbolt->damage = 104 + (power / 35);
        pbolt->hit = 8 + (power / 20);   // = 60;
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_COLD;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_SLOWING:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = BLACK;

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 3;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_HASTING:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = BLUE;

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 3;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_MAGIC_DARTS:
        strcpy(pbolt->beam_name, "magic dart");
        pbolt->colour = LIGHTMAGENTA;
        pbolt->range = random2(5) + 9;
        pbolt->damage = ( coinflip() ? 103 : 102 ) + (power / 45);
        pbolt->hit = 1500;
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_MMISSILE;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_HEALING:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = GREEN;

        pbolt->thing_thrown = KILL_MON;
        return 2;

    case ZAP_PARALYSIS:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = CYAN;

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 3;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_FIRE:
        strcpy(pbolt->beam_name, "bolt of fire");
        pbolt->colour = RED;
        pbolt->range = 8 + random2(10);
        pbolt->damage = 107 + (power / 11);
        pbolt->hit = 10 + (random2(power) / 80);
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_FIRE;

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 2;

    case ZAP_COLD:
        strcpy(pbolt->beam_name, "bolt of cold");
        pbolt->colour = WHITE;
        pbolt->range = 8 + random2(10);
        pbolt->damage = 107 + (power / 11);
        pbolt->hit = 10 + (random2(power) / 80);
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_COLD;

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 2;

    case ZAP_CONFUSION:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = RED;

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 3;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_INVISIBILITY:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = MAGENTA;

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 3;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_DIGGING:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = BROWN;
        pbolt->range = 4 + random2(power) + random2(5);    // not ordinary "0" beam range {dlb}

        pbolt->thing_thrown = KILL_MON;
        return 2;

    case ZAP_FIREBALL:
        strcpy(pbolt->beam_name, "fireball");
        pbolt->colour = RED;
        pbolt->range = 9 + random2(5);
        pbolt->damage = 5 + (power / 7);
        pbolt->hit = 40;
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_EXPLOSION;  // fire

        pbolt->thing_thrown = KILL_MON_MISSILE;
        return 1;

    case ZAP_TELEPORTATION:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = LIGHTGREY;
        pbolt->range = 8 + random2(5);

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 3;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_LIGHTNING:
        strcpy(pbolt->beam_name, "bolt of lightning");
        pbolt->colour = LIGHTCYAN;
        pbolt->range = 9 + random2(10);
        pbolt->damage = 106 + (power / 12);
        pbolt->hit = 10 + (random2(power) / 40);
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_ELECTRICITY;

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 2;

    case ZAP_POLYMORPH_OTHER:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = DARKGREY;
        pbolt->range = 8 + random2(5);

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 3;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_VENOM_BOLT:
        strcpy(pbolt->beam_name, "bolt of poison");
        pbolt->colour = LIGHTGREEN;
        pbolt->range = 9 + random2(10);
        pbolt->damage = 105 + (power / 13);
        pbolt->hit = 10 + (random2(power) / 80);
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_POISON;

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 2;

    case ZAP_NEGATIVE_ENERGY:
        strcpy(pbolt->beam_name, "bolt of negative energy");
        pbolt->colour = DARKGREY;
        pbolt->range = 8 + random2(10);
        pbolt->damage = 105 + (power / 13);
        pbolt->hit = 10 + (random2(power) / 80);
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_NEG;

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 2;

    case ZAP_CRYSTAL_SPEAR:
        strcpy(pbolt->beam_name, "crystal spear");
        pbolt->colour = WHITE;
        pbolt->range = 8 + random2(10);
        pbolt->damage = 112 + (power / 11);
        pbolt->hit = 6 + (random2(power) / 25);
        pbolt->type = SYM_MISSILE;
        pbolt->flavour = BEAM_MMISSILE;

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_BEAM_OF_ENERGY:    // bolt of innacuracy
        strcpy(pbolt->beam_name, "narrow beam of energy");
        pbolt->colour = YELLOW;
        pbolt->range = 8 + random2(10);
        pbolt->damage = 110 + (power / 5);
        pbolt->hit = 2;       //1 + random2(power) / 80;
        pbolt->type = SYM_ZAP;
        pbolt->flavour = 17;      // whatever

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 2;

    case ZAP_ORB_OF_ENERGY:     // Mystic Bolt
        strcpy(pbolt->beam_name, "orb of energy");
        pbolt->colour = LIGHTMAGENTA;
        pbolt->range = 8 + random2(5);
        pbolt->damage = 105 + (power / 20);
        pbolt->hit = 10 + (power / 20);
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_MMISSILE;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_ENSLAVEMENT:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = LIGHTBLUE;
        pbolt->range = 8 + random2(5);

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 3;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_PAIN:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = LIGHTMAGENTA;
        pbolt->range = 8 + random2(8);

        pbolt->hit = 8 + (power / 20);         // is this right? other "0" beams do not define ->hit {dlb}

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 7;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_STICKY_FLAME:
        strcpy(pbolt->beam_name, "sticky flame");
        pbolt->colour = RED;
        pbolt->range = 9 + random2(5);
        pbolt->damage = 102 + (power / 30);
        pbolt->hit = 11 + (power / 10);
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_FIRE;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_DISPEL_UNDEAD:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = YELLOW;
        pbolt->range = 8 + random2(8);

        pbolt->hit = 9 + (power / 5);          // is this right? other "0" beams do not define ->hit {dlb}

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 3;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_CLEANSING_FLAME:
        strcpy(pbolt->beam_name, "golden flame");
        pbolt->colour = YELLOW;
        pbolt->range = 8 + random2(10);
        pbolt->damage = 110 + (power / 5);
        pbolt->hit = 55 + (random2(power) / 80);
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_HOLY;

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 2;

    case ZAP_BONE_SHARDS:
        strcpy(pbolt->beam_name, "spray of bone shards");
        pbolt->colour = LIGHTGREY;
        pbolt->range = 8 + random2(10);
        pbolt->damage = 102 + (power / 150);        // note that f_p[2] has a high value for this spell
        pbolt->hit = 55 + (random2(power) / 850);
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_MAGIC;

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 2;

    case ZAP_BANISHMENT:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = LIGHTGREEN;
        pbolt->range = 8 + random2(5);

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 5;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_DEGENERATION:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = LIGHTCYAN;
        pbolt->range = 8 + random2(5);

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 3;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_STING:
        strcpy(pbolt->beam_name, "sting");
        pbolt->colour = GREEN;
        pbolt->range = 9 + random2(5);
        pbolt->damage = 4 + (power / 15);
        pbolt->hit = 8 + (power / 20);   // = 60;
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_POISON;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_HELLFIRE:
        strcpy(pbolt->beam_name, "hellfire");
        pbolt->colour = RED;
        pbolt->range = 8 + random2(10);
        pbolt->damage = 20;
        pbolt->hit = 20;
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_EXPLOSION;

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 2;

    case ZAP_IRON_BOLT:
        strcpy(pbolt->beam_name, "iron bolt");
        pbolt->colour = LIGHTCYAN;
        pbolt->range = 5 + random2(5);
        pbolt->damage = 108 + (power / 10);
        pbolt->hit = 7 + (power / 25);
        pbolt->type = SYM_MISSILE;
        pbolt->flavour = BEAM_MMISSILE;

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_PEBBLE:
        strcpy(pbolt->beam_name, "pebble");
        pbolt->colour = BROWN;
        pbolt->range = 9 + random2(5);
        pbolt->damage = 5;
        pbolt->hit = 8 + (power / 20);
        pbolt->type = SYM_MISSILE;
        pbolt->flavour = BEAM_MMISSILE;

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_STONE_ARROW:
        strcpy(pbolt->beam_name, "stone arrow");
        pbolt->colour = LIGHTGREY;
        pbolt->range = 9 + random2(5);
        pbolt->damage = 105 + (power / 12);
        pbolt->hit = 7 + (power / 27);
        pbolt->type = SYM_MISSILE;
        pbolt->flavour = BEAM_MMISSILE;

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_ELECTRICITY:
        strcpy(pbolt->beam_name, "zap");
        pbolt->colour = LIGHTCYAN;
        pbolt->range = 9 + random2(5);
        pbolt->damage = 5 + (power / 8);    //jmf: changed for lower level SPELL_SHOCK [was: 10 + (power/6)]
        pbolt->hit = 7 + (power / 27);      // = 1500;
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_ELECTRICITY;

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 2;

    case ZAP_ORB_OF_ELECTRICITY:
        strcpy(pbolt->beam_name, "orb of electricity");
        pbolt->colour = LIGHTBLUE;
        pbolt->range = 12 + random2(10);
        pbolt->damage = 5 + (power / 5);
        pbolt->hit = 40;
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_ELECTRICITY;

        pbolt->thing_thrown = KILL_MON;
        return 1;

    case ZAP_SPIT_POISON:
        strcpy(pbolt->beam_name, "splash of poison");
        pbolt->colour = GREEN;
        pbolt->range = 4 + random2(1 + (power / 2));
        pbolt->damage = 4 + (power / 2);
        pbolt->hit = 5 + random2(1 + (power / 3));
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_POISON;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_DEBUGGING_RAY:
        strcpy(pbolt->beam_name, "debugging ray");
        pbolt->colour = random_colour();
        pbolt->range = 8 + random2(10);
        pbolt->damage = 150;
        pbolt->hit = 60;
        pbolt->type = SYM_DEBUG;
        pbolt->flavour = BEAM_MMISSILE;

        pbolt->thing_thrown = KILL_MON;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_BREATHE_FIRE:
        strcpy(pbolt->beam_name, "fiery breath");
        pbolt->colour = RED;
        pbolt->range = 4 + random2(1 + (power / 2));
        pbolt->damage = 104 + (power / 3);              // NB: experience_level + mut * 4
        pbolt->hit = 8 + random2(1 + (power / 3));
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_FIRE;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 2;

    case ZAP_BREATHE_FROST:
        strcpy(pbolt->beam_name, "freezing breath");
        pbolt->colour = WHITE;
        pbolt->range = 4 + random2(1 + (power / 2));
        pbolt->damage = 104 + (power / 3);              // NB: experience_level + mut * 4
        pbolt->hit = 8 + random2(1 + (power / 3));
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_COLD;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 2;

    case ZAP_BREATHE_ACID:
        strcpy(pbolt->beam_name, "acid");
        pbolt->colour = YELLOW;
        pbolt->range = 4 + random2(1 + (power / 2));
        pbolt->damage = 103 + (power / 3);
        pbolt->hit = 5 + random2(1 + (power / 3));
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_ACID;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_BREATHE_POISON:    // leaves clouds of gas
        strcpy(pbolt->beam_name, "poison gas");
        pbolt->colour = GREEN;
        pbolt->range = 4 + random2(1 + (power / 2));
        pbolt->damage = 102 + (power / 6);
        pbolt->hit = 5 + random2(1 + (power / 3));
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_POISON;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_BREATHE_POWER:
        strcpy(pbolt->beam_name, "bolt of energy");

        pbolt->colour = BLUE;
        if (random2(power) >= 8)
          pbolt->colour = LIGHTBLUE;
        if (random2(power) >= 12)
          pbolt->colour = MAGENTA;
        if (random2(power) >= 17)
          pbolt->colour = LIGHTMAGENTA;

        pbolt->range = 7 + random2(1 + (power / 2));
        pbolt->damage = 103 + (power / 3);
        pbolt->hit = 11 + random2(1 + (power / 3));
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_MMISSILE;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_ENSLAVE_UNDEAD:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = LIGHTRED;
        pbolt->range = 8 + random2(5);

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 3;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_AGONY:
        strcpy(pbolt->beam_name, "0agony");
        pbolt->colour = LIGHTMAGENTA;
        pbolt->range = 8 + random2(8);

        pbolt->hit = 6 + (power / 50);         // is this right? other "0" beams do not define ->hit {dlb}

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 5;
        return 2;

    case ZAP_DISRUPTION:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = WHITE;
        pbolt->range = 8 + random2(8);

        pbolt->hit = 5 + (power / 50);         // is this right? other "0" beams do not define ->hit {dlb}

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 3;
        return 2;

    case ZAP_DISINTEGRATION:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = WHITE;
        pbolt->range = 8 + random2(8);

        pbolt->hit = 15 + (power / 3);         // is this right? other "0" beams do not define ->hit {dlb}

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 5;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_BREATHE_STEAM:
        strcpy(pbolt->beam_name, "ball of steam");
        pbolt->colour = LIGHTGREY;
        pbolt->range = 7 + random2(5);
        pbolt->damage = 104 + (power / 5);
        pbolt->hit = 10 + random2(1 + (power / 5));
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_FIRE;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 1;

    case ZAP_CONTROL_DEMON:
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = 16;              // this is not a color !!! {dlb}
        pbolt->range = 8 + random2(5);

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 17;
        pbolt->ench_power /= 10;
        return 2;

    case ZAP_ORB_OF_FRAGMENTATION:
        strcpy(pbolt->beam_name, "metal orb");
        pbolt->colour = CYAN;
        pbolt->range = 10 + random2(7);
        pbolt->damage = 10 + (power / 7);
        pbolt->hit = 20;
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_MMISSILE;

        pbolt->thing_thrown = KILL_MON;
        return 1;

    case ZAP_ICE_BOLT:
        strcpy(pbolt->beam_name, "bolt of ice");
        pbolt->colour = WHITE;
        pbolt->range = 9 + random2(5);
        pbolt->damage = 105 + (power / 10);
        pbolt->hit = 9 + (power / 12);
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_ICE;

        pbolt->thing_thrown = KILL_MON;
        return 1;

    case ZAP_ICE_STORM:
        strcpy(pbolt->beam_name, "great blast of cold");
        pbolt->colour = BLUE;
        pbolt->range = 10 + random2(5);
        pbolt->damage = 5 + (power / 10);
        pbolt->hit = 9 + (power / 12);
        pbolt->type = SYM_ZAP;
        pbolt->flavour = BEAM_ICE;        /* ice */// <- changed from BEAM_COLD b/c of comment 13jan2000 {dlb}

        pbolt->thing_thrown = KILL_MON;
        return 1;

    case ZAP_SLEEP: //jmf: added
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = BEAM_SLEEP;
        pbolt->range = 8 + random2(5);

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 3;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_BACKLIGHT: //jmf: added
        strcpy(pbolt->beam_name, "0");
        pbolt->colour = BEAM_BACKLIGHT;
        pbolt->range = 8 + random2(5);

        pbolt->thing_thrown = KILL_MON;
        pbolt->ench_power *= 3;
        pbolt->ench_power /= 2;
        return 2;

    case ZAP_FLAME_TONGUE:    //jmf: ought to be a weak, short-range missile
        strcpy(pbolt->beam_name, "flame");
        pbolt->colour = RED;
        pbolt->range = 3 + random2(3);
        pbolt->damage = 104 + power;
        pbolt->hit = power;
        pbolt->type = SYM_BOLT;
        pbolt->flavour = BEAM_FIRE;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 2;

    case ZAP_SANDBLAST:    //jmf: ought to be a weak, short-range missile
        strcpy(pbolt->beam_name, coinflip()  ? "blast of sand" : "rocky blast" );
        pbolt->colour = BROWN;
        pbolt->range = 4 + random2(3);
        pbolt->damage = 104 + power;
        pbolt->hit = power;
        pbolt->type = SYM_BOLT;
        pbolt->flavour = BEAM_FRAG;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 2;

    case ZAP_SMALL_SANDBLAST:    //jmf: ought to be a weak, short-range missile
        strcpy(pbolt->beam_name, "blast of ");

        temp_rand = random2(4);

        strcat(pbolt->beam_name, (temp_rand == 0) ? "dust" :
                                 (temp_rand == 1) ? "dirt" :
                                 (temp_rand == 2) ? "grit"
                                                  : "sand" );

        pbolt->colour = BROWN;
        pbolt->range = ( coinflip() ? 3 : 2 );
        pbolt->damage = 104 + power;
        pbolt->hit = power;
        pbolt->type = SYM_BOLT;
        pbolt->flavour = BEAM_FRAG;

        pbolt->thing_thrown = KILL_MON_MISSILE;
        pbolt->wand_id = 1;
        return 2;

    }                           // end of switch

    return 0;

}          // end zappy()




void potion_effect( char pot_eff, int pow )
{

    int new_value = 0;

    if ( pow > 150 )
      pow = 150;

    switch ( pot_eff )
    {
      case POT_HEALING:
        mpr("You feel better.");
        inc_hp(5 + random2(7), false);

        if ( you.hp >= you.hp_max )
        {
            if ( you.base_hp < 5000 )
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
        inc_hp(10 + random2avg(28,3), false);

        if ( you.hp >= you.hp_max )
        {
            if ( you.base_hp < 5000 )
              you.base_hp += 2 + random2avg(5,2);

            if ( you.base_hp > 5000 )
              you.base_hp = 5000;

            calc_hp();
            set_hp(you.hp_max, false);
        }
        break;

      case POT_SPEED:
        if (you.haste > 90)
          mpr("You already have as much speed as you can handle.");
        else if ( wearing_amulet(AMU_RESIST_SLOW) )
        {
            mpr("Your amulet glows brightly and you gain speed.");

            if ( you.slow )
            {
                you.slow = 0;
                you.haste = 10;
            }
            else
              you.haste += 50 + random2(pow);
        }
        else if ( !you.haste )
        {
            mpr("You feel yourself speed up.");

            if ( you.slow )
              you.slow = 0;
            else
              you.haste += 40 + random2(pow);
        }
        else
        {
            mpr("You feel as though your speed will last longer.");
            you.haste += 40 + random2(pow);
        }

        if ( wearing_amulet(AMU_RESIST_SLOW) )
        {
            if ( you.haste > 100 )
              you.haste = 100;
        }
        else
        {
            if ( you.haste > 90 )
              you.haste = 90;
        }

        naughty(NAUGHTY_STIMULANTS, 4 + random2(4));
        break;

      case POT_MIGHT:
        strcpy(info, "You feel ");
        strcat(info, (you.might) ? "pretty" : "very");
        strcat(info, " mighty");
        strcat(info, (you.might) ? ", still" : " all of a sudden");
        strcat(info, ".");
        mpr(info);

        you.might += 35 + random2(pow);    // conceivable max gain of +184 {dlb}

        if ( !you.might )
        {
            increase_stats(STAT_STRENGTH, 5, true);

            if ( you.might > 75 )
              you.might = 75;
        }
        else
        {
            if ( you.might > 150 )     // files.cc permits values up to 215, but ... {dlb}
              you.might = 150;
        }

        naughty(NAUGHTY_STIMULANTS, 4 + random2(4));
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
        strcpy(info, "You feel ");
        strcat(info, (!you.levitation) ? "very" : "more");
        strcat(info, "buoyant.");
        mpr(info);

        if ( !you.levitation )
          mpr("You gently float upwards from the floor.");

        you.levitation += 25 + random2(pow);

        if ( you.levitation > 100 )
          you.levitation = 100;

        burden_change();
        break;

      case POT_POISON:
      case POT_STRONG_POISON:
        if ( player_res_poison() )
        {
            strcpy(info, "You feel ");
            strcat(info, (pot_eff == POT_POISON) ? "slightly" : "extremely");
            strcat(info, " nauseous.");
        }
        else
        {
            if ( you.poison )
              strcpy(info, "You feel even sicker.");
            else
            {
                strcpy(info, "That liquid tasted ");
                strcat(info, (pot_eff == POT_POISON) ? "very" : "extremely");
                strcat(info, " nasty...");
            }

            you.poison += 1 + ( (pot_eff == POT_POISON) ? random2avg(5,2) : 2 + random2avg(13,2) );
        }

        mpr(info);
        break;

      case POT_SLOWING:
        if ( wearing_amulet(AMU_RESIST_SLOW) )
          mpr("You feel momentarily lethargic.");
        else if ( you.slow > 90 )
          mpr("You already are as slow as you could be.");
        else
        {
            if ( !you.slow )
              mpr("You feel yourself slow down.");
            else
              mpr("You feel as though you will be slow longer.");

            you.slow += 10 + random2(pow);

            if ( you.slow > 90 )
              you.slow = 90;
        }
        break;

      case POT_PARALYSIS:
        strcpy(info, "You ");
        strcat(info, (you.paralysis) ? "still haven't" : "suddenly lose");
        strcat(info, " the ability to move!");
        mpr(info);

        new_value = 2 + random2(6 + you.paralysis);

        if ( new_value > you.paralysis )
          you.paralysis = new_value;

        if ( you.paralysis > 13 )
          you.paralysis = 13;
        break;

      case POT_CONFUSION:
        strcpy(info, "You feel ");

        if ( wearing_amulet(AMU_CLARITY) )
          strcat(info, "momentarilty ");
        else if ( you.conf )
          strcat(info, "very ");

        strcat(info, "confused.");
        mpr(info);

    // early return for the protected: {dlb}
        if ( wearing_amulet(AMU_CLARITY) )
          break;

        new_value = 3 + random2(8 + you.conf);

        if ( new_value > you.conf )
          you.conf = new_value;

        if ( you.conf > 20 )
          you.conf = 20;
        break;

      case POT_INVISIBILITY:
        mpr ( (!you.invis) ? "You fade into invisibility!" : "You feel safely hidden away." );

        you.invis += 15 + random2(pow);    // should really scale this by you.invis {dlb}

        if ( you.invis > 100 )
          you.invis = 100;
        break;

// carnivore check here? {dlb}
      case POT_PORRIDGE:          // oatmeal - always gluggy white/grey?
        mpr("That potion was really gluggy!");
        lessen_hunger(6000, true);
        break;

      case POT_DEGENERATION:
        mpr("There was something very wrong with that liquid!");
        lose_stat(STAT_RANDOM, 1 + random2avg(4,2));
        break;

// Don't generate randomly - should be rare and interesting
      case POT_DECAY:
        strcpy(info, "You feel ");
        strcat(info, (you.is_undead) ? "terrible."
                                     : "your flesh start to rot away!");
        mpr(info);

        if ( !you.is_undead )
          you.rotting += 10 + random2(10);
        break;

      case POT_WATER:
        mpr("This tastes like water.");
        lessen_hunger(20, true);    // we should really separate thirst from hunger {dlb}
        break;

      case POT_EXPERIENCE:
        if ( you.experience_level < 27 )
        {
          mpr("You feel more experienced!");
          you.experience = 1 + exp_needed(2 + you.experience_level, you.species);
          level_change();
        }
        else
          mpr("A flood of memories washes over you.");
        break;                  // I'll let this slip past robe of archmagi

      case POT_MAGIC:
        mpr("You feel magical!");
        inc_mp(5 + random2avg(19,2), false);
        break;

      case POT_RESTORE_ABILITIES:    // messaging taken care of within function {dlb}
        restore_stat(STAT_ALL, false);
        break;

      case POT_BERSERK_RAGE:
        if ( !go_berserk() )
          mpr("You feel angry!");
        break;

      case POT_CURE_MUTATION:
        mpr("It has a very clean taste.");
        for (unsigned char i = 0; i < 7; i++)
          if ( random2(10) > i )     //jmf: added [this bit of - dlb] randomness
            delete_mutation(100);
        break;

      case POT_MUTATION:
        mpr("You feel extremely strange.");
        for (unsigned char i = 0; i < 3; i++)
          mutate(100);
        naughty(NAUGHTY_STIMULANTS, 4 + random2(4));
        break;
    }

    return;

}          // end potion_effect()




void unwield_item( char unw )
{

    you.special_wield = SPWLD_NONE;
    wield_change = true;

    if ( you.inv_class[unw] == OBJ_WEAPONS )
    {
        if ( you.inv_dam[unw] >= NWPN_SINGING_SWORD )
        {
            switch ( you.inv_dam[unw] )
            {
              case NWPN_SINGING_SWORD:
                mpr("The Singing Sword sighs.");
                break;
              case NWPN_WRATH_OF_TROG:
                mpr("You feel less violent.");
                break;
              case NWPN_SCYTHE_OF_CURSES:
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

        if ( you.inv_dam[unw] != SPWPN_NORMAL )
        {
            if ( you.inv_ident[unw] == 1 )
              you.inv_ident[unw] = 2;

            in_name(unw, 4, str_pass);
            strcpy(info, str_pass);

            char i_dam = (you.inv_dam[unw] % 30);

            if ( you.inv_dam[unw] % 30 >= SPWPN_RANDART_I )
              i_dam = randart_wpn_properties(you.inv_class[unw], you.inv_type[unw], you.inv_dam[unw], you.inv_plus[unw], you.inv_plus2[unw], 0, RAP_BRAND);

            switch ( i_dam )
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
                if ( you.skills[SK_TRANSLOCATIONS] < random2(9) )    //jmf: added save
                  miscast_effect(SPTYP_TRANSLOCATION, 9, 90, 100);
                else
                  miscast_effect(SPTYP_TRANSLOCATION, 1, 10, 100);
                break;

    // when more are added here, *must* duplicate unwielding
    // effect in vorpalise weapon scroll effect in read_scoll

            }        // end switch

            if ( you.inv_dam[unw] % 30 >= SPWPN_RANDART_I )
              unuse_randart(unw);

            if ( you.duration[DUR_WEAPON_BRAND] )
              {
                you.duration[DUR_WEAPON_BRAND] = 0;
                you.inv_dam[unw] -= you.inv_dam[unw] % 30;
                mpr("Your branding evaporates.");
              }

        }    // end if

    }


    if ( you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES )
    {
        if ( you.inv_type[you.equip[EQ_WEAPON]] == STAFF_POWER )
        {
            you.max_magic_points -= 13;
            you.base_magic_points2 -= 13;

            if ( you.magic_points > you.max_magic_points )
              you.magic_points = you.max_magic_points;

            calc_mp();
        }
    }

    return;

}          // end unwield_item()




// This does *not* call ev_mod!
void unwear_armour( char unw )
{

    you.redraw_armor_class = 1;
    you.redraw_evasion = 1;

    switch ( you.inv_dam[unw] % 30 )
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
        if ( !player_res_poison() )
          mpr("You feel less healthy.");
        break;

      case SPARM_SEE_INVISIBLE:
        if ( !player_see_invis() )
          mpr("You feel less perceptive.");
        break;

      case SPARM_DARKNESS:    // I do not understand this {dlb}
        if ( you.invis )
          you.invis = 1;
        break;

      case SPARM_STRENGTH:
        decrease_stats(STAT_STRENGTH, 3, false);
        break;

      case SPARM_DEXTERITY:
        decrease_stats(STAT_DEXTERITY, 3, false);
        break;

      case SPARM_INTELLIGENCE:
        decrease_stats(STAT_INTELLIGENCE, 3, false);
        break;

      case SPARM_PONDEROUSNESS:
        mpr("That put a bit of spring back into your step.");
        you.speed -= 2;
        break;

      case SPARM_LEVITATION:
        //you.levitation++;
        if ( you.levitation )
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

    if ( you.inv_dam[unw] % 30 >= SPARM_RANDART_I )
      unuse_randart(unw);

    return;

}          // end unwear_armour()




void unuse_randart( unsigned char unw )
{

    if (randart_wpn_properties(you.inv_class[unw], you.inv_type[unw], you.inv_dam[unw], you.inv_plus[unw], you.inv_plus2[unw], 0, RAP_AC) != 0)
    {
        you.redraw_armor_class = 1;
    }

    if (randart_wpn_properties(you.inv_class[unw], you.inv_type[unw], you.inv_dam[unw], you.inv_plus[unw], you.inv_plus2[unw], 0, RAP_EVASION) != 0)
    {
        you.redraw_evasion = 1;
    }

    if (randart_wpn_properties(you.inv_class[unw], you.inv_type[unw], you.inv_dam[unw], you.inv_plus[unw], you.inv_plus2[unw], 0, RAP_STRENGTH) != 0)
    {
        you.strength -= randart_wpn_properties(you.inv_class[unw], you.inv_type[unw], you.inv_dam[unw], you.inv_plus[unw], you.inv_plus2[unw], 0, RAP_STRENGTH);
        you.max_strength -= randart_wpn_properties(you.inv_class[unw], you.inv_type[unw], you.inv_dam[unw], you.inv_plus[unw], you.inv_plus2[unw], 0, RAP_STRENGTH);
        you.redraw_strength = 1;
    }

    if (randart_wpn_properties(you.inv_class[unw], you.inv_type[unw], you.inv_dam[unw], you.inv_plus[unw], you.inv_plus2[unw], 0, RAP_INTELLIGENCE) != 0)
    {
        you.intel -= randart_wpn_properties(you.inv_class[unw], you.inv_type[unw], you.inv_dam[unw], you.inv_plus[unw], you.inv_plus2[unw], 0, RAP_INTELLIGENCE);
        you.max_intel -= randart_wpn_properties(you.inv_class[unw], you.inv_type[unw], you.inv_dam[unw], you.inv_plus[unw], you.inv_plus2[unw], 0, RAP_INTELLIGENCE);
        you.redraw_intelligence = 1;
    }

    if (randart_wpn_properties(you.inv_class[unw], you.inv_type[unw], you.inv_dam[unw], you.inv_plus[unw], you.inv_plus2[unw], 0, RAP_DEXTERITY) != 0)
    {
        you.dex -= randart_wpn_properties(you.inv_class[unw], you.inv_type[unw], you.inv_dam[unw], you.inv_plus[unw], you.inv_plus2[unw], 0, RAP_DEXTERITY);
        you.max_dex -= randart_wpn_properties(you.inv_class[unw], you.inv_type[unw], you.inv_dam[unw], you.inv_plus[unw], you.inv_plus2[unw], 0, RAP_DEXTERITY);
        you.redraw_dexterity = 1;
    }

    if (randart_wpn_properties(you.inv_class[unw], you.inv_type[unw], you.inv_dam[unw], you.inv_plus[unw], you.inv_plus2[unw], 0, RAP_NOISES) != 0)
    {
        you.special_wield = SPWLD_NONE;
    }

}          // end unuse_randart()

/*
 *  File:       ability.cc
 *  Summary:    Functions related to special abilities.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <3>      5/20/99        BWR             Now use scan_randarts to
 *                                              check for flags, rather than
 *                                              only checking the weapon.
 *      <2>      5/20/99        BWR             Extended screen line support
 *
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "ability.h"

#include <string.h>

#ifdef DOS
#include <conio.h>
#endif

#ifdef LINUX
#include "linuxlib.h"
#endif

#ifdef USE_CURSES
#include <curses.h>
#endif

#include "externs.h"
#include "enum.h"

#include "bang.h"
#include "beam.h"
#include "effects.h"
#include "items.h"
#include "it_use2.h"
#include "misc.h"
#include "monplace.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "spell.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "skills.h"
#include "skills2.h"
#include "spells3.h"
#include "stuff.h"
#include "view.h"

#ifdef MACROS
  #include "macro.h"
#endif

/*
   Activates a menu which gives player access to all of their non-spell
   special abilities - Eg naga's spit poison, or the Invocations you get
   from worshipping. Generated dynamically - the function checks to see which
   abilities you have every time.
 */
void species_ability()          /* Now handles all special abilities */
{
    if (you.berserker > 0)
    {
        mpr("You're too berserk!");
        return;
    }

    if (you.conf > 0)
    {
        mpr("You're too confused!");
        return;
    }

    struct dist abild[1];
    struct bolt beam[1];
    struct dist spd[1];

    int ability[120];
    int ability_fail[120];
    char abil_c = 0;
    unsigned char abil_used;
    unsigned char spc, spc2;
    char keyi;
    int i = 0;

    for (i = 0; i < 120; i++)
    {
        ability[i] = 0;
        ability_fail[i] = 0;
    }

    switch (you.species)
    {
    case SP_NAGA:               // Naga

        ability[abil_c] = ABIL_SPIT_POISON;
        ability_fail[abil_c] = 20 - you.experience_level;
        abil_c++;
        break;
    case SP_GNOME:              // Gnome

        ability[abil_c] = ABIL_MAPPING;
        ability_fail[abil_c] = 20 - you.experience_level;
        abil_c++;
        break;
    }

    if (you.species != SP_NAGA && you.mutation[MUT_SPIT_POISON] > 0)    // spit poison

    {
        ability[abil_c] = ABIL_SPIT_POISON;
        ability_fail[abil_c] = 30 - you.mutation[MUT_SPIT_POISON] * 10 - you.experience_level;
        abil_c++;
    }

    if (you.species != SP_GNOME && (you.mutation[MUT_MAPPING] > 0 || scan_randarts(RAP_MAPPING) > 0))   // sense surr

    {
        ability[abil_c] = ABIL_MAPPING;
        ability_fail[abil_c] = 30 - you.mutation[MUT_MAPPING] * 10 - you.experience_level;
        abil_c++;
    }

    if (you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON || you.mutation[MUT_BREATHE_FLAMES] > 0 || (you.species == SP_RED_DRACONIAN && you.experience_level > 6))     // breathe fire

    {
        ability[abil_c] = ABIL_BREATHE_FIRE;
        ability_fail[abil_c] = 50 - you.mutation[MUT_BREATHE_FLAMES] * 10 - you.experience_level - (you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON) * 20;
        if (you.species == SP_RED_DRACONIAN)
            ability_fail[abil_c] = 30 - you.experience_level;
        abil_c++;
    }

    if (you.species == SP_WHITE_DRACONIAN && you.experience_level > 6)  // breathe frost

    {
        ability[abil_c] = ABIL_BREATHE_FROST;
        ability_fail[abil_c] = 30 - you.experience_level;
        abil_c++;
    }

    if (you.species == SP_GREEN_DRACONIAN && you.experience_level > 6)  // breathe poison

    {
        ability[abil_c] = ABIL_BREATHE_POISON;
        ability_fail[abil_c] = 30 - you.experience_level;
        abil_c++;
    }

    if (you.species == SP_GOLDEN_DRACONIAN && you.experience_level > 6)         // spit acid

    {
        ability[abil_c] = ABIL_SPIT_ACID;
        ability_fail[abil_c] = 30 - you.experience_level;
        abil_c++;
    }

    if (you.species == SP_BLACK_DRACONIAN && you.experience_level > 6)  // breathe lightning

    {
        ability[abil_c] = ABIL_BREATHE_LIGHTNING;
        ability_fail[abil_c] = 30 - you.experience_level;
        abil_c++;
    }

    if (you.species == SP_PURPLE_DRACONIAN && you.experience_level > 6)         // breathe power

    {
        ability[abil_c] = ABIL_BREATHE_POWER;
        ability_fail[abil_c] = 30 - you.experience_level;
        abil_c++;
    }

    if (you.species == SP_MOTTLED_DRACONIAN && you.experience_level > 6)        // spit sticky flame

    {
        ability[abil_c] = ABIL_BREATHE_STICKY_FLAME;
        ability_fail[abil_c] = 30 - you.experience_level;
        abil_c++;
    }

    if (you.species == SP_PALE_DRACONIAN && you.experience_level > 6)   // breathe steam

    {
        ability[abil_c] = ABIL_BREATHE_STEAM;
        ability_fail[abil_c] = 20 - you.experience_level;
        abil_c++;
    }

    if (you.mutation[MUT_BLINK] > 0 || scan_randarts(RAP_BLINK) > 0)    // blink

    {
        ability[abil_c] = ABIL_BLINK;
        ability_fail[abil_c] = 30 - you.mutation[MUT_BLINK] * 10;
        abil_c++;
    }



    if (wearing_amulet(AMU_RAGE) != 0 || scan_randarts(RAP_BERSERK) > 0)
    {
        ability[abil_c] = ABIL_GO_BERSERK_I;
        ability_fail[abil_c] = 50 - you.experience_level * 2;
        if (you.species == SP_HILL_DWARF || you.species == SP_MOUNTAIN_DWARF || you.species == SP_HILL_ORC || you.species == SP_OGRE)
            ability_fail[abil_c] -= 10;
        if (you.species == SP_TROLL)
            ability_fail[abil_c] -= 30;         /* troll */
        abil_c++;
    }

    if (you.species == SP_KENKU && you.experience_level >= 5 && you.duration[DUR_CONTROLLED_FLIGHT] == 0 && you.levitation == 0)        // kenku can fly, but only from the ground (until levitation 15, when it becomes permanent until revoked)

    {
        ability[abil_c] = ABIL_FLY;
        ability_fail[abil_c] = 45 - you.experience_level * 3;
        abil_c++;
    }                           /* Kenku can fly */

/* Assorted demonic powers follow: */

    if (you.mutation[MUT_SUMMON_MINOR_DEMONS] != 0)     // Summon minor demon

    {
        ability[abil_c] = ABIL_SUMMON_MINOR_DEMON;
        ability_fail[abil_c] = 27 - you.experience_level;
        abil_c++;
    }

    if (you.mutation[MUT_SUMMON_DEMONS] != 0)   // Summon demon

    {
        ability[abil_c] = ABIL_SUMMON_DEMON;
        ability_fail[abil_c] = 40 - you.experience_level;
        abil_c++;
    }

    if (you.mutation[MUT_HURL_HELLFIRE] != 0)   // Hellfire

    {
        ability[abil_c] = ABIL_HELLFIRE;
        ability_fail[abil_c] = 50 - you.experience_level;
        abil_c++;
    }

    if (you.mutation[MUT_CALL_TORMENT] != 0)    // Torment

    {
        ability[abil_c] = ABIL_TORMENT;
        ability_fail[abil_c] = 60 - you.experience_level;
        abil_c++;
    }

    if (you.mutation[MUT_RAISE_DEAD] != 0)      // Raise Dead

    {
        ability[abil_c] = ABIL_RAISE_DEAD;
        ability_fail[abil_c] = 50 - you.experience_level;
        abil_c++;
    }

    if (you.mutation[MUT_CONTROL_DEMONS] != 0)  // Control Demon

    {
        ability[abil_c] = ABIL_CONTROL_DEMON;
        ability_fail[abil_c] = 35 - you.experience_level;
        abil_c++;
    }

    if (you.mutation[MUT_PANDEMONIUM] != 0)     // Gate to Pan

    {
        ability[abil_c] = ABIL_TO_PANDEMONIUM;
        ability_fail[abil_c] = 57 - you.experience_level * 2;
        abil_c++;
    }

    if (you.mutation[MUT_CHANNEL_HELL] != 0)    // Channel from Hell

    {
        ability[abil_c] = ABIL_CHANNELING;
        ability_fail[abil_c] = 30 - you.experience_level;
        abil_c++;
    }

    if (you.mutation[MUT_THROW_FLAMES] != 0)    // Throw flame

    {
        ability[abil_c] = ABIL_THROW_FLAME;
        ability_fail[abil_c] = 10 - you.experience_level;
        abil_c++;
    }

    if (you.mutation[MUT_THROW_FROST] != 0)     // Throw frost

    {
        ability[abil_c] = ABIL_THROW_FROST;
        ability_fail[abil_c] = 10 - you.experience_level;
        abil_c++;
    }

    if (you.mutation[MUT_SMITE] != 0)   // Bolt of Draining

    {
        ability[abil_c] = ABIL_BOLT_OF_DRAINING;
        ability_fail[abil_c] = 30 - you.experience_level;
        abil_c++;
    }

/* Most abilities from items are displaced by 50, so that abilities obtained
   naturally or from mutations can be put earlier, to avoid the menu letters
   always changing around */

    if (((you.equip[EQ_LEFT_RING] != -1
                && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_INVISIBILITY)
            || (you.equip[EQ_RIGHT_RING] != -1
                && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_INVISIBILITY)
            || (you.equip[EQ_CLOAK] != -1
                && you.inv_dam[you.equip[EQ_CLOAK]] % 30 == SPARM_DARKNESS)
            || (you.equip[EQ_HELMET] != -1
                && you.inv_dam[you.equip[EQ_HELMET]] % 30 == SPARM_DARKNESS)
            || scan_randarts(RAP_INVISIBLE) > 0)
        && you.invis == 0)
    {
        ability[abil_c] = ABIL_TURN_INVISIBLE;
        ability_fail[abil_c] = 50 - you.experience_level * 2;
        abil_c++;
    }

    if (you.invis != 0)
    {
        ability[abil_c] = ABIL_TURN_VISIBLE;
        ability_fail[abil_c] = 0;
        abil_c++;
    }

    if ((you.equip[EQ_LEFT_RING] != -1
            && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_LEVITATION)
        || (you.equip[EQ_RIGHT_RING] != -1
            && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_LEVITATION)
        || (you.equip[EQ_BOOTS] != -1
            && you.inv_dam[you.equip[EQ_BOOTS]] % 30 == SPARM_LEVITATION)
        || (scan_randarts(RAP_LEVITATE) > 0) && you.levitation == 0)
    {
        ability[abil_c] = ABIL_LEVITATE;
        ability_fail[abil_c] = 50 - you.experience_level * 2;
        abil_c++;
    }

    if (you.levitation != 0)
    {
        ability[abil_c] = ABIL_STOP_LEVITATING;
        ability_fail[abil_c] = 0;
        abil_c++;
    }

    if (you.duration[DUR_TRANSFORMATION] != 0)
    {
        ability[abil_c] = ABIL_END_TRANSFORMATION;
        ability_fail[abil_c] = 0;
        abil_c++;
    }

    if ((you.equip[EQ_LEFT_RING] != -1
            && you.inv_type[you.equip[EQ_LEFT_RING]] == RING_TELEPORTATION)
        || (you.equip[EQ_RIGHT_RING] != -1
            && you.inv_type[you.equip[EQ_RIGHT_RING]] == RING_TELEPORTATION)
        || you.mutation[MUT_TELEPORT_AT_WILL] != 0
        || scan_randarts(RAP_CAN_TELEPORT) > 0)
    {
        ability[abil_c] = ABIL_TELEPORTATION;
        ability_fail[abil_c] = 50;
        if (you.mutation[MUT_TELEPORT_AT_WILL] >= 2)
            ability_fail[abil_c] = 20;
        abil_c++;
    }

    abil_c = 26;

    if (you.piety >= 30)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
            ability[abil_c] = ABIL_REPEL_UNDEAD_I;
            ability_fail[abil_c] = 30 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 6;
            abil_c++;
            break;
        case GOD_SHINING_ONE:
            ability[abil_c] = ABIL_REPEL_UNDEAD_II;
            ability_fail[abil_c] = 30 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 6;
            abil_c++;
            break;
        case GOD_KIKUBAAQUDGHA:
            ability[abil_c] = ABIL_RECALL_UNDEAD_SLAVES;
            ability_fail[abil_c] = 30 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 6;
            abil_c++;
            break;
        case GOD_YREDELEMNUL:
            ability[abil_c] = ABIL_ANIMATE_CORPSE;
            ability_fail[abil_c] = 40 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 3;
            abil_c++;
            break;
        case GOD_OKAWARU:
            ability[abil_c] = ABIL_MIGHT_I;
            ability_fail[abil_c] = 30 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 6;
            abil_c++;
            break;
        case GOD_TROG:
            ability[abil_c] = ABIL_GO_BERSERK_II;
            ability_fail[abil_c] = 30 - you.piety;
            abil_c++;
            break;
        case GOD_ELYVILON:
            ability[abil_c] = ABIL_LESSER_HEALING;
            ability_fail[abil_c] = 30 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 6;
            abil_c++;
            break;
        }
    }

    if (you.piety >= 50)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
            ability[abil_c] = ABIL_MINOR_HEALING;
            ability_fail[abil_c] = 40 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 5;
            abil_c++;
            break;
        case GOD_SHINING_ONE:
            ability[abil_c] = ABIL_SMITING;
            ability_fail[abil_c] = 40 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 5;
            abil_c++;
            break;
        case GOD_YREDELEMNUL:
            ability[abil_c] = ABIL_RECALL_UNDEAD;
            ability_fail[abil_c] = 50 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 4;
            abil_c++;
            break;
        case GOD_OKAWARU:
            ability[abil_c] = ABIL_HEALING_I;
            ability_fail[abil_c] = 40 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 5;
            abil_c++;
            break;
        case GOD_MAKHLEB:
            ability[abil_c] = ABIL_MINOR_DESTRUCTION;
            ability_fail[abil_c] = 40 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 5;
            abil_c++;
            break;
        case GOD_SIF_MUNA:
            ability[abil_c] = ABIL_FORGET_SPELL;
            ability_fail[abil_c] = 40 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 5;
            abil_c++;
            break;
        case GOD_TROG:
            ability[abil_c] = ABIL_MIGHT_II;
            ability_fail[abil_c] = 40 - (you.piety / 3);
            abil_c++;
            break;
        case GOD_ELYVILON:
            ability[abil_c] = ABIL_PURIFICATION;
            ability_fail[abil_c] = 20 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 5;
            abil_c++;
            break;
        }
    }

    if (you.piety >= 75)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
            ability[abil_c] = ABIL_PESTILENCE;
            ability_fail[abil_c] = 60 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 5;
            abil_c++;
            break;
        case GOD_SHINING_ONE:
            ability[abil_c] = ABIL_ANNIHILATE_UNDEAD;
            ability_fail[abil_c] = 60 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 5;
            abil_c++;
            break;
        case GOD_KIKUBAAQUDGHA:
            ability[abil_c] = ABIL_ENSLAVE_UNDEAD;
            ability_fail[abil_c] = 40 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 5;
            abil_c++;
            break;
        case GOD_YREDELEMNUL:
            ability[abil_c] = ABIL_ANIMATE_DEAD;
            ability_fail[abil_c] = 40 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 5;
            abil_c++;
            break;
        case GOD_MAKHLEB:
            ability[abil_c] = ABIL_LESSER_SERVANT_OF_MAKHLEB;
            ability_fail[abil_c] = 40 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 5;
            abil_c++;
            break;
        case GOD_ELYVILON:
            ability[abil_c] = ABIL_HEALING_II;
            ability_fail[abil_c] = 40 - (you.piety / 10) - you.skills[SK_INVOCATIONS] * 5;
            abil_c++;
            break;
        }
    }

    if (you.piety >= 100)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
            ability[abil_c] = ABIL_HOLY_WORD;
            ability_fail[abil_c] = 70 - (you.piety / 15) - you.skills[SK_INVOCATIONS] * 4;
            abil_c++;
            break;
        case GOD_SHINING_ONE:
            ability[abil_c] = ABIL_THUNDERBOLT;
            ability_fail[abil_c] = 70 - (you.piety / 15) - you.skills[SK_INVOCATIONS] * 4;
            abil_c++;
            break;
        case GOD_YREDELEMNUL:
            ability[abil_c] = ABIL_DRAIN_LIFE;
            ability_fail[abil_c] = 60 - (you.piety / 15) - you.skills[SK_INVOCATIONS] * 4;
            abil_c++;
            break;
        case GOD_VEHUMET:
            ability[abil_c] = ABIL_CHANNEL_ENERGY;
            ability_fail[abil_c] = 40 - you.intel - you.skills[SK_INVOCATIONS];
            abil_c++;
            break;
        case GOD_MAKHLEB:
            ability[abil_c] = ABIL_MAJOR_DESTRUCTION;
            ability_fail[abil_c] = 60 - (you.piety / 15) - you.skills[SK_INVOCATIONS] * 4;
            abil_c++;
            break;
        case GOD_TROG:
            ability[abil_c] = ABIL_HASTE_SELF;
            ability_fail[abil_c] = 70 - (you.piety / 15) - you.skills[SK_INVOCATIONS] * 4;
            abil_c++;
            break;
        case GOD_ELYVILON:
            ability[abil_c] = ABIL_RESTORATION;
            ability_fail[abil_c] = 70 - (you.piety / 15) - you.skills[SK_INVOCATIONS] * 4;
            abil_c++;
            break;
        }
    }

    if (you.piety >= 120)
    {
        switch (you.religion)
        {
        case GOD_ZIN:
            ability[abil_c] = ABIL_SUMMON_GUARDIAN;
            ability_fail[abil_c] = 80 - (you.piety / 15) - you.skills[SK_INVOCATIONS] * 4;
            abil_c++;
            break;
        case GOD_SHINING_ONE:
            ability[abil_c] = ABIL_SUMMON_DAEVA;
            ability_fail[abil_c] = 80 - (you.piety / 15) - you.skills[SK_INVOCATIONS] * 4;
            abil_c++;
            break;
        case GOD_KIKUBAAQUDGHA:
            ability[abil_c] = ABIL_INVOKE_DEATH;
            ability_fail[abil_c] = 80 - (you.piety / 15) - you.skills[SK_INVOCATIONS] * 4;
            abil_c++;
            break;
        case GOD_YREDELEMNUL:
            ability[abil_c] = ABIL_CONTROL_UNDEAD;
            ability_fail[abil_c] = 70 - (you.piety / 15) - you.skills[SK_INVOCATIONS] * 4;
            abil_c++;
            break;
        case GOD_OKAWARU:
            ability[abil_c] = ABIL_HASTE;
            ability_fail[abil_c] = 70 - (you.piety / 15) - you.skills[SK_INVOCATIONS] * 4;
            abil_c++;
            break;
        case GOD_MAKHLEB:
            ability[abil_c] = ABIL_GREATER_SERVANT_OF_MAKHLEB;
            ability_fail[abil_c] = 70 - (you.piety / 15) - you.skills[SK_INVOCATIONS] * 4;
            abil_c++;
            break;
        case GOD_ELYVILON:
            ability[abil_c] = ABIL_GREATER_HEALING;
            ability_fail[abil_c] = 80 - (you.piety / 15) - you.skills[SK_INVOCATIONS] * 4;
            abil_c++;
            break;
        }
    }


    if (you.religion != GOD_NO_GOD)
    {
        ability[49] = ABIL_RENOUNCE_RELIGION;
        ability_fail[49] = 0;
    }


    for (i = 0; i < 120; i++)
    {
        if (ability[i] != 0)
            break;
        if (i == 119)
        {
            strcpy(info, "Sorry, you're not good enough to have a special ability.");
            mpr(info);
            return;
        }
    }


/*
   if (you.hunger_state <= 2)
   {
   strcpy(info, "You're too hungry.");
   mpr(info);
   return; // no you.turn_is_over?
   }

   if (you.duration [DUR_BREATH_WEAPON] != 0)
   {
   strcpy(info, "You can't do that yet.");
   mpr(info);
   return; // no you.turn_is_over?
   }
 */

query:
    strcpy(info, "Use which ability? (? or * to list)");
    mpr(info);

    int keyin = get_ch();

    if (keyin == '?' || keyin == '*')
    {
        char unthing = show_abilities(ability, ability_fail);   //you.spells); //invent(0, you.inv_quantity, you.inv_dam, you.inv_class, you.inv_type, you.inv_plus, you.inv_ident, you.equip [0], you.equip [6], you.equip [5], you.equip [2], you.equip [1], you.equip [3], you.equip [4], you.ring);

#ifdef PLAIN_TERM
        redraw_screen();
#endif

        if (unthing == 2)
            return;

        if ((unthing >= 65 && unthing <= 90) || (unthing >= 97 && unthing <= 122))
        {
            keyin = unthing;
        }
        else
        {
            mesclr();
            goto query;
        }
    }

    spc = (int) keyin;

    if ((spc < 97 || spc > 121) && (spc < 65 || spc > 90))
    {
unknown:
        strcpy(info, "You can't do that.");
        mpr(info);
        return;
    }

    spc2 = conv_lett(spc);

    if (ability[spc2] == 0)
    {
        goto unknown;
    }

    abil_used = spc2;

    if (random2(33) + random2(33) + random2(34) <= ability_fail[abil_used])
    {
        mpr("You fail to use your ability.");
        you.turn_is_over = 1;
        return;
    }

    you.turn_is_over = 1;

    you.redraw_magic_points = 1;

    switch (ability[abil_used])
    {
    case ABIL_SPIT_POISON:      // Naga, + spit poison mutation

        if (you.duration[DUR_BREATH_WEAPON] != 0)
        {
            mpr("You can't do that yet.");
            return;
        }
        if (you.hunger_state <= 2)
        {
            strcpy(info, "You're too hungry.");
            mpr(info);
            return;
        }
        if (spell_direction(abild, beam) == -1)
        {
            strcpy(info, "Okay, then.");
            mpr(info);
            return;
        }
        strcpy(info, "You spit poison.");
        mpr(info);
        you.turn_is_over = 1;
        you.duration[DUR_BREATH_WEAPON] += random2(5) + 3;
        zapping(ZAP_SPIT_POISON, you.experience_level + you.mutation[MUT_SPIT_POISON] * 5, beam);
        you.hunger -= 40 + random2(40) + random2(40);
        strcpy(info, "You feel slightly more hungry.");
        mpr(info);
        break;

    case ABIL_MAPPING:          // Gnome + sense surrounds mut

        strcpy(info, "You sense your surroundings.");
        mpr(info);
// you.duration [17] += 30 - you.experience_level + random2(40 - you.experience_level) + random2(5);
        magic_mapping(3 + random2(you.experience_level) + random2(you.experience_level) + you.mutation[MUT_MAPPING] * 10, 40 + random2(you.experience_level) + random2(you.experience_level) + random2(you.experience_level));
        you.hunger -= 30 + random2(30) + random2(30);
        you.turn_is_over = 1;
        break;

    case ABIL_TELEPORTATION:    // ring of teleport, + teleport mut

        if (you.hunger_state <= 2)
        {
            strcpy(info, "You're too hungry.");
            mpr(info);
            return;
        }
        if (you.magic_points < 3)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 3;
        you.redraw_magic_points = 1;
        you.hunger -= 200 + random2(200) + random2(200);
        if (you.mutation[MUT_TELEPORT_AT_WILL] == 3)
            you_teleport2(1);
        else
            you_teleport();
        break;

    case ABIL_BREATHE_FIRE:
    case ABIL_BREATHE_FROST:
    case ABIL_BREATHE_POISON:
    case ABIL_BREATHE_LIGHTNING:
    case ABIL_SPIT_ACID:
    case ABIL_BREATHE_POWER:
    case ABIL_BREATHE_STICKY_FLAME:
    case ABIL_BREATHE_STEAM:    /* Breathe things */
        if (you.duration[DUR_BREATH_WEAPON] != 0 && ability[abil_used] != ABIL_SPIT_ACID)
        {
            mpr("You can't do that yet.");
            return;
        }
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (spell_direction(abild, beam) == -1)
        {
            mpr("Okay, then.");
            return;
        }
        switch (ability[abil_used])
        {
        case ABIL_BREATHE_FIRE:
            mpr("You breathe fire.");
            zapping(ZAP_BREATHE_FIRE, you.experience_level + you.mutation[MUT_SPIT_POISON] * 4 + (you.attribute[ATTR_TRANSFORMATION] == TRAN_DRAGON) * 10, beam);
            break;
        case ABIL_BREATHE_FROST:
            mpr("You breathe a wave of freezing cold.");
            zapping(ZAP_BREATHE_FROST, you.experience_level, beam);
            break;
        case ABIL_BREATHE_POISON:
            mpr("You breathe a blast of poison gas.");
            zapping(ZAP_BREATHE_POISON, you.experience_level, beam);
            break;
        case ABIL_BREATHE_LIGHTNING:
            mpr("You spit a bolt of lightning.");
            zapping(ZAP_LIGHTNING, you.experience_level * 2, beam);
            break;
        case ABIL_SPIT_ACID:
            mpr("You spit acid.");
            zapping(ZAP_BREATHE_ACID, you.experience_level, beam);
            break;
        case ABIL_BREATHE_POWER:
            mpr("You spit a bolt of incandescent energy.");
            zapping(ZAP_BREATHE_POWER, you.experience_level, beam);
            break;
        case ABIL_BREATHE_STICKY_FLAME:
            mpr("You spit a glob of burning liquid.");
            zapping(ZAP_STICKY_FLAME, you.experience_level, beam);
            break;
        case ABIL_BREATHE_STEAM:
            mpr("You breathe a blast of scalding steam.");
            zapping(ZAP_BREATHE_STEAM, you.experience_level, beam);
            break;
        }
        you.turn_is_over = 1;
        if (ability[abil_used] != ABIL_SPIT_ACID)
            you.duration[DUR_BREATH_WEAPON] += random2(5) + 3 + random2(30 - you.experience_level);
        if (ability[abil_used] == ABIL_BREATHE_STEAM)
            you.duration[DUR_BREATH_WEAPON] /= 2;
        you.hunger -= 200 + random2(100) + random2(100) - you.experience_level * random2(5);
        strcpy(info, "You feel slightly more hungry.");
        mpr(info);
        break;

    case ABIL_BLINK:            // blink mut

        if (you.hunger_state <= 2)
        {
            strcpy(info, "You're too hungry.");
            mpr(info);
            return;
        }
        if (you.magic_points < 1)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 1;
        you.redraw_magic_points = 1;
        you.hunger -= 50 + random2(50) + random2(50);
        random_blink();
        break;

    case ABIL_GO_BERSERK_I:     // berserk

        if (you.hunger_state <= 2)
        {
            strcpy(info, "You're too hungry.");
            mpr(info);
            return;
        }
        if (go_berserk() == 0)
            mpr("You fail to go berserk.");
        break;

    case ABIL_FLY:              // Fly (kenku). Eventually becomes permanent (handled in acr.cc)

        if (you.hunger_state <= 2)
        {
            strcpy(info, "You're too hungry.");
            mpr(info);
            return;
        }
        if (you.magic_points < 3)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 3;
        you.redraw_magic_points = 1;
        you.hunger -= 100 + random2(100) + random2(100);
        cast_fly(you.experience_level * 4);
        if (you.experience_level >= 15)
        {
            mpr("You feel very comfortable in the air.");
            you.levitation = 100;
            you.duration[DUR_CONTROLLED_FLIGHT] = 100;
        }
        break;

/* Demonic powers follow: */

    case ABIL_SUMMON_MINOR_DEMON:       /* summon minor demon */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 3)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        if (you.hp <= 3)
        {
            mpr("You don't have enough vitality to use that power.");
            return;
        }
        you.magic_points -= 3;
        you.redraw_magic_points = 1;
        you.hp -= 3;
        you.redraw_hit_points = 1;
        you.hunger -= 50 + random2(100) + random2(100);
        summon_ice_beast_etc(you.experience_level * 4, summon_any_demon(0));
        break;

    case ABIL_SUMMON_DEMON:     /* summon demon */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 5)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        if (you.hp <= 5)
        {
            mpr("You don't have enough vitality to use that power.");
            return;
        }
        you.magic_points -= 5;
        you.redraw_magic_points = 1;
        you.hp -= 5;
        you.redraw_hit_points = 1;
        you.hunger -= 100 + random2(200) + random2(200);
        summon_ice_beast_etc(you.experience_level * 4, summon_any_demon(1));
        break;

    case ABIL_HELLFIRE: /* Hellfire */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 8)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        if (you.hp <= 8)
        {
            mpr("You don't have enough vitality to use that power.");
            return;
        }
        you.magic_points -= 8;
        you.redraw_magic_points = 1;
        you.hp -= 8;
        you.redraw_hit_points = 1;
        you.hunger -= 200 + random2(300) + random2(300);
        your_spells(SPELL_HELLFIRE, 100, 0);    // power (2nd number) is meaningless

        break;

    case ABIL_TORMENT:          /* Torment */
        if (you.is_undead != 0)
        {
            mpr("Undead cannot use this ability.");
            return;
        }
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 9)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 9;
        you.redraw_magic_points = 1;
        you.hunger -= 300 + random2(300) + random2(300);
        torment();
        break;

    case ABIL_RAISE_DEAD:       /* Raise dead */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 5)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        if (you.hp <= 5)
        {
            mpr("You don't have enough vitality to use that power.");
            return;
        }
        you.magic_points -= 5;
        you.redraw_magic_points = 1;
        you.hp -= 5;
        you.redraw_hit_points = 1;
        you.hunger -= 100 + random2(200) + random2(200);
        your_spells(SPELL_ANIMATE_DEAD, you.experience_level * 5, 0);   // power (2nd number) is meaningless

        break;

    case ABIL_CONTROL_DEMON:    /* Control demon */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 4)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        if (you.hp <= 4)
        {
            mpr("You don't have enough vitality to use that power.");
            return;
        }
        if (spell_direction(abild, beam) == -1)
        {
            mpr("Okay, then.");
            return;
        }
        you.magic_points -= 4;
        you.redraw_magic_points = 1;
        you.hp -= 4;
        you.redraw_hit_points = 1;
        you.hunger -= 100 + random2(100) + random2(100);
        zapping(ZAP_CONTROL_DEMON, you.experience_level * 5, beam);
        break;

    case ABIL_TO_PANDEMONIUM:   /* Pandemonium */
        if (you.level_type == 3)
        {
            mpr("You're already here.");
            return;
        }
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 4)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 7;
        you.redraw_magic_points = 1;
        you.hunger -= 200 + random2(200) + random2(200);
        banished(99);
        break;

    case ABIL_CHANNELING:       /* Channeling */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.hp < 1)
        {
            mpr("You don't have enough vitality to use that power.");
            return;
        }
        you.hp -= 1;
        you.redraw_hit_points = 1;
        you.hunger -= 20 + random2(30) + random2(30);
        you.magic_points += 1 + random2(5);
        if (you.magic_points > you.max_magic_points)
            you.magic_points = you.max_magic_points;
        you.redraw_magic_points = 1;
        mpr("You channel some magical energy.");
        break;

    case ABIL_THROW_FLAME:
    case ABIL_THROW_FROST:      /* Throw flame/frost */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 1)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        if (you.hp <= 1)
        {
            mpr("You don't have enough vitality to use that power.");
            return;
        }
        if (spell_direction(abild, beam) == -1)
        {
            mpr("Okay, then.");
            return;
        }
        you.magic_points -= 1;
        you.redraw_magic_points = 1;
        you.hp -= 1;
        you.redraw_hit_points = 1;
        you.hunger -= 50 + random2(50) + random2(50);
        if (ability[abil_used] == ABIL_THROW_FLAME)
            zapping(ZAP_FLAME, you.experience_level * 3, beam);
        else
            zapping(ZAP_FROST, you.experience_level * 3, beam);
        break;

    case ABIL_BOLT_OF_DRAINING: /* bolt of draining */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 4)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        if (you.hp <= 4)
        {
            mpr("You don't have enough vitality to use that power.");
            return;
        }
        if (spell_direction(abild, beam) == -1)
        {
            mpr("Okay, then.");
            return;
        }
        you.magic_points -= 4;
        you.redraw_magic_points = 1;
        you.hp -= 4;
        you.redraw_hit_points = 1;
        you.hunger -= 100 + random2(100) + random2(100);
        zapping(ZAP_NEGATIVE_ENERGY, you.experience_level * 6, beam);
        break;

/* Items etc */
    case ABIL_TURN_INVISIBLE:   /* turn invis */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 2)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 2;
        you.redraw_magic_points = 1;
        you.hunger -= 250 + random2(250) + random2(250);
        potion_effect(POT_INVISIBILITY, you.experience_level + 10);
        break;

    case ABIL_TURN_VISIBLE:     /* uninvisible */
        mpr("You feel less transparent.");
        you.invis = 1;
        break;

    case ABIL_LEVITATE: /* levitate */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 1)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 1;
        you.redraw_magic_points = 1;
        you.hunger -= 100 + random2(100) + random2(100);
        potion_effect(POT_LEVITATION, you.experience_level + 10);
        break;

    case ABIL_STOP_LEVITATING:  /* unlevitate */
        mpr("You feel heavy.");
        you.levitation = 1;
        break;

    case ABIL_END_TRANSFORMATION:       /* untransform */
        mpr("You feel almost normal.");
        you.duration[DUR_TRANSFORMATION] = 2;
        break;


/* Invocations start here: */

    case ABIL_REPEL_UNDEAD_I:
    case ABIL_REPEL_UNDEAD_II:
        if (you.magic_points < 1)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 1;
        you.redraw_magic_points = 1;
        turn_undead(you.piety);
        exercise(SK_INVOCATIONS, 1);
        break;

    case ABIL_MINOR_HEALING:    /* Minor Healing */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 2)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 2;
        you.redraw_magic_points = 1;
        if (cast_lesser_healing() == 0)
            break;
        you.hunger -= 50 + random2(50) + random2(50);
        lose_piety(1);
        exercise(SK_INVOCATIONS, 1 + random2(3));
        break;

    case ABIL_PESTILENCE:       /* Pestilence */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 3)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 3;
        you.redraw_magic_points = 1;
        you.hunger -= 100 + random2(100) + random2(100);
        summon_swarm(you.piety);
        lose_piety(2 + random2(2));
        exercise(SK_INVOCATIONS, 2 + random2(4));
        break;

    case ABIL_HOLY_WORD:        /* Holy Word */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 6)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 6;
        you.redraw_magic_points = 1;
        you.hunger -= 150 + random2(150) + random2(150);
        holy_word(you.piety);
        lose_piety(3 + random2(2));
        exercise(SK_INVOCATIONS, 3 + random2(5));
        break;

    case ABIL_SUMMON_GUARDIAN:  /* Guardian */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 4)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 4;
        you.redraw_magic_points = 1;
        you.hunger -= 150 + random2(150) + random2(150);
        summon_ice_beast_etc(you.skills[SK_INVOCATIONS] * 4, 26);
        lose_piety(3 + random2(4));
        exercise(SK_INVOCATIONS, 8 + random2(10));
        break;




    case ABIL_SMITING:
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 3)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 3;
        you.redraw_magic_points = 1;
        you.hunger -= 50 + random2(50) + random2(50);
        cast_smiting(you.skills[SK_INVOCATIONS] * 8);
        lose_piety(1);
        exercise(SK_INVOCATIONS, 2 + random2(2));
        break;

    case ABIL_ANNIHILATE_UNDEAD:        /* destroy undead */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 3)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        if (spell_direction(spd, beam) == -1)
        {
            mpr("Okay, then.");
            return;
        }
        you.magic_points -= 3;
        you.redraw_magic_points = 1;
        you.hunger -= 50 + random2(50) + random2(50);
        zapping(ZAP_DISPEL_UNDEAD, you.skills[SK_INVOCATIONS] * 6, beam);
        lose_piety(1 + random2(2));
        exercise(SK_INVOCATIONS, 2 + random2(4));
        break;

    case ABIL_THUNDERBOLT:      /* thunderbolt */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 5)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        if (spell_direction(spd, beam) == -1)
        {
            mpr("Okay, then.");
            return;
        }
        you.magic_points -= 5;
        you.redraw_magic_points = 1;
        you.hunger -= 100 + random2(100) + random2(100);
        zapping(ZAP_LIGHTNING, you.skills[SK_INVOCATIONS] * 6, beam);
        lose_piety(1 + random2(2));
        exercise(SK_INVOCATIONS, 3 + random2(6));
        break;

    case ABIL_SUMMON_DAEVA:     /* Daeva */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 5)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 5;
        you.redraw_magic_points = 1;
        you.hunger -= 150 + random2(150) + random2(150);
        summon_ice_beast_etc(you.skills[SK_INVOCATIONS] * 4, 366);
        lose_piety(3 + random2(4));
        exercise(SK_INVOCATIONS, 8 + random2(10));
        break;


    case ABIL_RECALL_UNDEAD_SLAVES:     /* recall undead */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 1)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 1;
        you.redraw_magic_points = 1;
        recall(1);
        exercise(SK_INVOCATIONS, 1);
        break;

    case ABIL_ENSLAVE_UNDEAD:   /* enslave undead */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 4)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        if (spell_direction(spd, beam) == -1)
        {
            mpr("Okay, then.");
            return;
        }
        you.magic_points -= 4;
        you.redraw_magic_points = 1;
        you.hunger -= 150 + random2(150) + random2(150);
        zapping(ZAP_ENSLAVE_UNDEAD, you.skills[SK_INVOCATIONS] * 8, beam);
        lose_piety(2 + random2(2));
        exercise(SK_INVOCATIONS, 5 + random2(5));
        break;

    case ABIL_INVOKE_DEATH:     /* Invoke Death */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 4)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 4;
        you.redraw_magic_points = 1;
        you.hunger -= 350 + random2(350) + random2(350);
        summon_ice_beast_etc(20 + you.skills[SK_INVOCATIONS] * 3, 83);
        lose_piety(3 + random2(4));
        exercise(SK_INVOCATIONS, 10 + random2(14));
        break;


    case ABIL_ANIMATE_CORPSE:   /* Animate Skeleton */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 3)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 3;
        you.redraw_magic_points = 1;
        you.hunger -= 50 + random2(50) + random2(50);
        mpr("You call on the dead to walk for you...");
        animate_a_corpse(you.x_pos, you.y_pos, 7, you.pet_target, 0);
        exercise(SK_INVOCATIONS, 2 + random2(4));
        break;

    case ABIL_RECALL_UNDEAD:    /* Recall Undead */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 4)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 4;
        you.redraw_magic_points = 1;
        you.hunger -= 100 + random2(100) + random2(100);
        recall(1);
        exercise(SK_INVOCATIONS, 2 + random2(4));
        break;

    case ABIL_ANIMATE_DEAD:     /* Animate Dead */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 7)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 7;
        you.redraw_magic_points = 1;
        you.hunger -= 150 + random2(150) + random2(150);
        mpr("You call on the dead to walk for you...");
        animate_dead(you.skills[SK_INVOCATIONS] + 1, 7, you.pet_target, 1);
        exercise(SK_INVOCATIONS, 2 + random2(4));
        lose_piety(1 + random2(2));
        break;

    case ABIL_DRAIN_LIFE:       /* Drain Life */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 6)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 6;
        you.redraw_magic_points = 1;
        you.hunger -= 150 + random2(150) + random2(150);
        drain_life(you.skills[SK_INVOCATIONS]);
        exercise(SK_INVOCATIONS, 2 + random2(4));
        lose_piety(1 + random2(3));
        break;

    case ABIL_CONTROL_UNDEAD:   /* Control Undead */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 5)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 5;
        you.redraw_magic_points = 1;
        you.hunger -= 150 + random2(150) + random2(250);
        mass_enchantment(30, you.skills[SK_INVOCATIONS] * 8);
        exercise(SK_INVOCATIONS, 3 + random2(4));
        lose_piety(2 + random2(3));
        break;

    case ABIL_CHANNEL_ENERGY:   /* channeling */
        if (you.hunger_state <= 2)
        {
            strcpy(info, "You're too hungry.");
            mpr(info);
            return;
        }
        you.hunger -= 50 + random2(50) + random2(50);
        you.magic_points += 1 + random2(you.skills[SK_INVOCATIONS] / 4 + 2);
        if (you.magic_points > you.max_magic_points)
            you.magic_points = you.max_magic_points;
        you.redraw_magic_points = 1;
        strcpy(info, "You channel some magical energy.");
        mpr(info);
        exercise(SK_INVOCATIONS, 1 + random2(3));
        break;

    case ABIL_MIGHT_I:          /* might */
        if (you.hunger_state <= 2)
        {
            strcpy(info, "You're too hungry.");
            mpr(info);
            return;
        }
        if (you.magic_points < 2)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 2;
        potion_effect(POT_MIGHT, you.skills[SK_INVOCATIONS] * 8);
        lose_piety(1);
        you.hunger -= 50 + random2(50) + random2(50);
        exercise(SK_INVOCATIONS, 1 + random2(3));
        break;

    case ABIL_HEALING_I:        /* Minor healing */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 2)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 2;
        if (cast_lesser_healing() == 0)
            break;
        lose_piety(1);
        you.hunger -= 100 + random2(50) + random2(50);
        exercise(SK_INVOCATIONS, 2 + random2(5));
        break;


    case ABIL_HASTE:            /* haste */
        if (you.hunger_state <= 2)
        {
            strcpy(info, "You're too hungry.");
            mpr(info);
            return;
        }
        if (you.magic_points < 5)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 5;
        potion_effect(POT_SPEED, you.skills[SK_INVOCATIONS] * 8);
        lose_piety(2 + random2(3));
        you.hunger -= 100 + random2(100) + random2(100);
        exercise(SK_INVOCATIONS, 3 + random2(7));
        break;




    case ABIL_MINOR_DESTRUCTION:        /* minor destr */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 1)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        if (spell_direction(spd, beam) == -1)
        {
            mpr("Okay, then.");
            return;
        }
        you.magic_points -= 1;
        you.redraw_magic_points = 1;
        you.hunger -= 20 + random2(20) + random2(20);
        switch (random2(5))
        {
        case 0:
            zapping(ZAP_FLAME, you.skills[SK_INVOCATIONS] * 6, beam);
            break;
        case 1:
            zapping(ZAP_AGONY_I, you.skills[SK_INVOCATIONS] * 6, beam);
            break;
        case 2:
            zapping(ZAP_STONE_ARROW, you.skills[SK_INVOCATIONS] * 6, beam);
            break;
        case 3:
            zapping(ZAP_ELECTRICITY, you.skills[SK_INVOCATIONS] * 6, beam);
            break;
        case 4:
            zapping(ZAP_BREATHE_ACID, you.skills[SK_INVOCATIONS] * 6, beam);
            break;
        }
        exercise(SK_INVOCATIONS, 1 + random2(2));
        break;

    case ABIL_LESSER_SERVANT_OF_MAKHLEB:        /* minor summoning */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 2)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 2;
        you.redraw_magic_points = 1;
        you.hunger -= 50 + random2(50) + random2(50);
        summon_ice_beast_etc(20 + you.skills[SK_INVOCATIONS] * 3, 225 + random2(5));
        lose_piety(1 + random2(2));
        exercise(SK_INVOCATIONS, 2 + random2(3));
        break;

    case ABIL_MAJOR_DESTRUCTION:        /* major destr */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 3)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        if (spell_direction(spd, beam) == -1)
        {
            mpr("Okay, then.");
            return;
        }
        you.magic_points -= 3;
        you.redraw_magic_points = 1;
        you.hunger -= 100 + random2(50) + random2(50);
        switch (random2(8))
        {
        case 0:
            zapping(ZAP_FIRE, you.skills[SK_INVOCATIONS] * 5, beam);
            break;
        case 1:
            zapping(ZAP_FIREBALL, you.skills[SK_INVOCATIONS] * 5, beam);
            break;
        case 2:
            zapping(ZAP_LIGHTNING, you.skills[SK_INVOCATIONS] * 5, beam);
            break;
        case 3:
            zapping(ZAP_NEGATIVE_ENERGY, you.skills[SK_INVOCATIONS] * 5, beam);
            break;
        case 4:
            zapping(ZAP_STICKY_FLAME, you.skills[SK_INVOCATIONS] * 5, beam);
            break;
        case 5:
            zapping(ZAP_IRON_BOLT, you.skills[SK_INVOCATIONS] * 5, beam);
            break;
        case 6:
            zapping(ZAP_ORB_OF_ELECTRICITY, you.skills[SK_INVOCATIONS] * 5, beam);
            break;
        case 7:
            you.attribute[ATTR_DIVINE_LIGHTNING_PROTECTION] = 1;
            mpr("Makhleb hurls a blast of lightning!");
            beam[0].beam_source = MNG;
            beam[0].type = 43;
            beam[0].damage = 130;
            beam[0].flavour = 5;
            beam[0].bx = you.x_pos;
            beam[0].by = you.y_pos;
            strcpy(beam[0].beam_name, "blast of lightning");
            beam[0].colour = LIGHTCYAN;
            beam[0].thing_thrown = 1;   /* your expl */
            explosion(1, beam);
            you.attribute[ATTR_DIVINE_LIGHTNING_PROTECTION] = 0;
            break;
        }
        lose_piety(1 + random2(2));
        exercise(SK_INVOCATIONS, 3 + random2(5));
        break;

    case ABIL_GREATER_SERVANT_OF_MAKHLEB:       /* major summoning */
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 4)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 4;
        you.redraw_magic_points = 1;
        you.hunger -= 100 + random2(50) + random2(50);
        summon_ice_beast_etc(20 + you.skills[SK_INVOCATIONS] * 3, 230 + random2(5));
        lose_piety(2 + random2(2));
        exercise(SK_INVOCATIONS, 6 + random2(6));
        break;



    case ABIL_GO_BERSERK_II:    /* berserk */
        if (you.hunger_state <= 2)
        {
            strcpy(info, "You're too hungry.");
            mpr(info);
            return;
        }
        if (go_berserk() == 0)
            mpr("You fail to go berserk.");
        you.hunger -= 100 + random2(100) + random2(100);
        //exercise(SK_INVOCATIONS, 1 + random2(3));
        break;

    case ABIL_MIGHT_II: /* might */
        if (you.hunger_state <= 2)
        {
            strcpy(info, "You're too hungry.");
            mpr(info);
            return;
        }
        potion_effect(POT_MIGHT, you.skills[SK_INVOCATIONS] * 6);
        lose_piety(1 + random2(2));
        you.hunger -= 200 + random2(200) + random2(200);
        //exercise(SK_INVOCATIONS, 2 + random2(5));
        break;

    case ABIL_HASTE_SELF:       /* haste */
        if (you.hunger_state <= 2)
        {
            strcpy(info, "You're too hungry.");
            mpr(info);
            return;
        }
        potion_effect(POT_SPEED, you.skills[SK_INVOCATIONS] * 6);
        lose_piety(2 + random2(3));
        you.hunger -= 250 + random2(250) + random2(250);
        //exercise(SK_INVOCATIONS, 4 + random2(6));
        break;




    case ABIL_FORGET_SPELL:
        cast_selective_amnesia();
        //exercise(SK_INVOCATIONS, 1);
        break;


    case ABIL_LESSER_HEALING:
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 1)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 1;
        you.redraw_magic_points = 1;
        if (cast_lesser_healing() == 0)
            break;
        you.hunger -= 100 + random2(100) + random2(100);
        exercise(SK_INVOCATIONS, 1 + random2(2));
        break;

    case ABIL_PURIFICATION:
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 1)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 1;
        you.redraw_magic_points = 1;
        lose_piety(1);
        you.hunger -= 150 + random2(150) + random2(150);
        purification();
        exercise(SK_INVOCATIONS, 2 + random2(3));
        break;

    case ABIL_HEALING_II:
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 2)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        if (cast_greater_healing() == 0)
            break;
        you.magic_points -= 2;
        you.redraw_magic_points = 1;
        you.hunger -= 250 + random2(250) + random2(250);
        lose_piety(1 + random2(3));
        exercise(SK_INVOCATIONS, 3 + random2(5));
        break;

    case ABIL_RESTORATION:
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 3)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        you.magic_points -= 3;
        you.redraw_magic_points = 1;
        you.hunger -= 400 + random2(450) + random2(450);
        strcpy(info, "You feel refreshed.");    // ...can't think of anything better right now

        mpr(info);
        restore_str();
        restore_int();
        restore_dex();
        if (you.base_hp < 5000)
            you.base_hp = 5000;
        calc_hp();
        lose_piety(2 + random2(3));
        exercise(SK_INVOCATIONS, 4 + random2(6));
        break;

    case ABIL_GREATER_HEALING:
        if (you.hunger_state <= 2)
        {
            mpr("You're too hungry.");
            return;
        }
        if (you.magic_points < 6)
        {
            mpr("You don't have enough magic to use that ability.");
            return;
        }
        if (cast_greatest_healing() == 0)
            break;
        you.magic_points -= 6;
        you.redraw_magic_points = 1;
        you.hunger -= 600 + random2(550) + random2(550);
//cast_purification(you.piety);
        lose_piety(4 + random2(4));
        exercise(SK_INVOCATIONS, 6 + random2(10));
        break;


    case ABIL_RENOUNCE_RELIGION:        /* give up religion */
        mpr("Really give up your religion, and lose all of its fabulous benefits?");
        keyi = get_ch();
        if (keyi != 'y' && keyi != 'Y')
        {
            mpr("Okay, then.");
            break;
        }
        mpr("Are you absolutely sure?");
        keyi = get_ch();
        if (keyi != 'y' && keyi != 'Y')
        {
            mpr("Okay, then.");
            break;
        }
        excommunication();
        break;

    default:
        strcpy(info, "Sorry, you can't do that.");
        mpr(info);
        return;
    }

}
/*
   Lists any abilities the player may have.
 */
char show_abilities(int ability[120], int ability_fail[120])
{
    int j;
    char lines = 0;
    unsigned char anything = 0;
    char strng[5] = "";
    char ft;
    char ki;
    char already = 0;
    char is_invocations = 0;

    for (j = 0; j < 120; j++)
    {
        if (ability[j] >= 100)
            is_invocations = 1;
    }


#ifdef DOS_TERM
    char buffer[4800];

    gettext(1, 1, 80, 25, buffer);
    window(1, 1, 80, 25);
#endif

    clrscr();

    cprintf("  Ability                           Cost                    Success");
    lines++;

    if (lines > NUMBER_OF_LINES - 3)
    {
        gotoxy(1, NUMBER_OF_LINES);
        cprintf("-more-");
        ki = getch();
        if (ki == 27)
        {
#ifdef DOS_TERM
            puttext(1, 1, 80, 25, buffer);
#endif
            return 27;
        }
        if (ki >= 65 && ki < 123)
        {
#ifdef DOS_TERM
            puttext(1, 1, 80, 25, buffer);
#endif
            return ki;
        }
        if (ki == 0)
            ki = getch();
        lines = 0;
        clrscr();
        gotoxy(1, 1);
        anything = 0;

    }
    if (lines > 1)
        cprintf(EOL " ");

    for (j = 0; j < 120; j++)
    {

        if (lines > NUMBER_OF_LINES - 2)
        {
            gotoxy(1, NUMBER_OF_LINES);
            cprintf("-more-");
            ki = getch();
            if (ki == 27)
            {
#ifdef DOS_TERM
                puttext(1, 1, 80, 25, buffer);
#endif
                return 27;
            }
            if (ki >= 65 && ki < 123)
            {
#ifdef DOS_TERM
                puttext(1, 1, 80, 25, buffer);
#endif
                return ki;
            }
            if (ki == 0)
                ki = getch();
            lines = 0;
            clrscr();
            gotoxy(1, 1);
            anything = 0;
        }

        if (j == 25 && is_invocations == 1)
        {
            anything++;
            textcolor(BLUE);
            cprintf(EOL "    Invocations - ");
            textcolor(LIGHTGREY);
            lines++;
            continue;
        }

        if (ability[j] != 0)
        {

            anything++;
            if (lines > 0)
                cprintf(EOL);
            lines++;
            cprintf(" ");
            if (j < 26)
                ft = (char) j + 97;
            else
                ft = (char) j + 65 - 26;
            strng[0] = ft;
            cprintf(strng);
            cprintf(" - ");

            switch (ability[j])
            {
            case ABIL_SPIT_POISON:
                cprintf("Spit Poison                     Food");
                break;
            case ABIL_MAPPING:
                cprintf("Sense Surroundings              Food");
                break;
            case ABIL_TELEPORTATION:
                cprintf("Teleportation                   3 Magic, Food");
                break;
            case ABIL_BREATHE_FIRE:
                cprintf("Breathe Fire                    Food, Delay");
                break;
            case ABIL_BLINK:
                cprintf("Blink                           1 Magic, Food ");
                break;
            case ABIL_BREATHE_FROST:
                cprintf("Breathe Frost                   Food, Delay");
                break;
            case ABIL_BREATHE_POISON:
                cprintf("Breathe Poison Gas              Food, Delay");
                break;
            case ABIL_BREATHE_LIGHTNING:
                cprintf("Breathe Lightning               Food, Delay");
                break;
            case ABIL_SPIT_ACID:
                cprintf("Spit Acid                       Food");
                break;
            case ABIL_BREATHE_POWER:
                cprintf("Breathe Power                   Food, Delay");
                break;
            case ABIL_GO_BERSERK_I:
                cprintf("Go Berserk                      None");
                break;
            case ABIL_BREATHE_STICKY_FLAME:
                cprintf("Breathe Sticky Flame            Food, Delay");
                break;
            case ABIL_BREATHE_STEAM:
                cprintf("Breathe Steam                   Food, Delay");
                break;
            case ABIL_FLY:
                cprintf("Fly                             3 Magic, Food");
                break;
/* Demonic powers: */
            case ABIL_SUMMON_MINOR_DEMON:
                cprintf("Summon Minor Demon              3 Magic, 3 hp, Food");
                break;
            case ABIL_SUMMON_DEMON:
                cprintf("Summon Demon                    5 Magic, 5 hp, Food");
                break;
            case ABIL_HELLFIRE:
                cprintf("Hellfire                        8 Magic, 8 hp, Food");
                break;
            case ABIL_TORMENT:
                cprintf("Torment                         9 Magic, Food");
                break;
            case ABIL_RAISE_DEAD:
                cprintf("Raise Dead                      5 Magic, 5 hp, Food");
                break;
            case ABIL_CONTROL_DEMON:
                cprintf("Control Demon                   4 Magic, 4 hp, Food");
                break;
            case ABIL_TO_PANDEMONIUM:
                cprintf("Open Gate to Pandemonium        7 Magic, Food");
                break;
            case ABIL_CHANNELING:
                cprintf("Channeling                      1 hp, Food");
                break;
            case ABIL_THROW_FLAME:
                cprintf("Throw Flame                     1 Magic, 1 hp, Food");
                break;
            case ABIL_THROW_FROST:
                cprintf("Throw Frost                     1 Magic, 1 hp, Food");
                break;
            case ABIL_BOLT_OF_DRAINING:
                cprintf("Bolt of Draining                4 Magic, 4 hp, Food");
                break;
/* Most of these powers are from items: */
            case ABIL_TURN_INVISIBLE:
                cprintf("Turn Invisible                  2 Magic, Food");
                break;
            case ABIL_TURN_VISIBLE:
                cprintf("Turn Visible                    None");
                break;
            case ABIL_LEVITATE:
                cprintf("Levitate                        1 Magic, Food");
                break;
            case ABIL_STOP_LEVITATING:
                cprintf("Stop Levitating                 None");
                break;
            case ABIL_END_TRANSFORMATION:
                cprintf("End Transformation              None");
                break;
            case 56:
                cprintf(" ");
                break;
/* Invocations: */
            case ABIL_REPEL_UNDEAD_I:
            case ABIL_REPEL_UNDEAD_II:
                cprintf("Repel Undead                    1 Magic, Food");
                break;
            case ABIL_MINOR_HEALING:
                cprintf("Minor Healing                   2 Magic, Food, Piety");
                break;
            case ABIL_PESTILENCE:
                cprintf("Pestilence                      3 Magic, Food, Piety");
                break;
            case ABIL_HOLY_WORD:
                cprintf("Holy Word                       6 Magic, Food, Piety");
                break;
            case ABIL_SUMMON_GUARDIAN:
                cprintf("Summon Guardian                 7 Magic, Food, Piety");
                break;

            case ABIL_SMITING:
                cprintf("Smiting                         3 Magic, Food, Piety");
                break;
            case ABIL_ANNIHILATE_UNDEAD:
                cprintf("Annihilate Undead               3 Magic, Food, Piety");
                break;
            case ABIL_THUNDERBOLT:
                cprintf("Thunderbolt                     5 Magic, Food, Piety");
                break;
            case ABIL_SUMMON_DAEVA:
                cprintf("Summon Daeva                    8 Magic, Food, Piety");
                break;

            case ABIL_RECALL_UNDEAD_SLAVES:
                cprintf("Recall Undead Slaves            1 Magic");
                break;
            case ABIL_ENSLAVE_UNDEAD:
                cprintf("Enslave Undead                  2 Magic, Food, Piety");
                break;
            case ABIL_INVOKE_DEATH:
                cprintf("Invoke Death                    3 Magic, Food, Piety");
                break;

            case ABIL_ANIMATE_CORPSE:
                cprintf("Animate Corpse                  3 Magic, Food");
                break;
            case ABIL_RECALL_UNDEAD:
                cprintf("Recall Undead                   4 Magic, Food");
                break;
            case ABIL_ANIMATE_DEAD:
                cprintf("Animate Dead                    7 Magic, Food, Piety");
                break;
            case ABIL_DRAIN_LIFE:
                cprintf("Drain Life                      6 Magic, Food, Piety");
                break;
            case ABIL_CONTROL_UNDEAD:
                cprintf("Control Undead                  5 Magic, Food, Piety");
                break;

            case ABIL_CHANNEL_ENERGY:
                cprintf("Channel Energy                  Food");
                break;

            case ABIL_MIGHT_I:
                cprintf("Might                           2 Magic, Food, Piety");
                break;
            case ABIL_HEALING_I:
                cprintf("Healing                         2 Magic, Food, Piety");
                break;
            case ABIL_HASTE:
                cprintf("Haste                           5 Magic, Food, Piety");
                break;

            case ABIL_MINOR_DESTRUCTION:
                cprintf("Minor Destruction               1 Magic, Food");
                break;
            case ABIL_LESSER_SERVANT_OF_MAKHLEB:
                cprintf("Lesser Servant of Makhleb       2 Magic, Food, Piety");
                break;
            case ABIL_MAJOR_DESTRUCTION:
                cprintf("Major Destruction               3 Magic, Food, Piety");
                break;
            case ABIL_GREATER_SERVANT_OF_MAKHLEB:
                cprintf("Greater Servant of Makhleb      4 Magic, Food, Piety");
                break;


            case ABIL_GO_BERSERK_II:
                cprintf("Go Berserk                      Food");
                break;
            case ABIL_MIGHT_II:
                cprintf("Might                           Food, Piety");
                break;
            case ABIL_HASTE_SELF:
                cprintf("Haste Self                      Food, Piety");
                break;

            case ABIL_FORGET_SPELL:
                cprintf("Forget Spell                    5 Magic, Piety");
                break;

            case ABIL_LESSER_HEALING:
                cprintf("Lesser Healing                  1 Magic, Food");
                break;
            case ABIL_PURIFICATION:
                cprintf("Purification                    2 Magic, Food, Piety");
                break;
            case ABIL_HEALING_II:
                cprintf("Healing                         2 Magic, Food, Piety");
                break;
            case ABIL_RESTORATION:
                cprintf("Restoration                     3 Magic, Food, Piety");
                break;
            case ABIL_GREATER_HEALING:
                cprintf("Greater Healing                 6 Magic, Food, Piety");
                break;

            case ABIL_RENOUNCE_RELIGION:
                cprintf("Renounce Religion               None");
                break;

            }

            gotoxy(35, wherey());
            already = 0;

            gotoxy(60, wherey());

            int spell_f = ability_fail[j];

            if (spell_f == 100)
            {
                cprintf("Useless");
            }
            else if (spell_f > 90)
            {
                cprintf("Terrible");
            }
            else if (spell_f > 80)
            {
                cprintf("Cruddy");
            }
            else if (spell_f > 70)
            {
                cprintf("Bad");
            }
            else if (spell_f > 60)
            {
                cprintf("Very Poor");
            }
            else if (spell_f > 50)
            {
                cprintf("Poor");
            }
            else if (spell_f > 40)
            {
                cprintf("Fair");
            }
            else if (spell_f > 30)
            {
                cprintf("Good");
            }
            else if (spell_f > 20)
            {
                cprintf("Very Good");
            }
            else if (spell_f > 10)
            {
                cprintf("Great");
            }
            else if (spell_f > 0)
            {
                cprintf("Excellent");
            }
            else
                cprintf("Perfect");

            gotoxy(70, wherey());

        }
    }                           // end of j loop

    if (anything > 0)
    {
        ki = getch();
        if (ki >= 65 && ki < 123)
        {
#ifdef DOS_TERM
            puttext(1, 1, 80, 25, buffer);
#endif
            return ki;
        }
        if (ki == 0)
            ki = getch();
#ifdef DOS_TERM
        puttext(1, 1, 80, 25, buffer);
#endif
        return anything;
    }

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif
    // was 35
    ki = getch();

    return ki;
}

#include "config.h"

#include <string.h>

#ifdef DOS
#include <conio.h>
#endif

#ifdef LINUX
#include "linuxlib.h"
#endif

#include "externs.h"
#include "enum.h"

#include "ability.h"
#include "bang.h"
#include "beam.h"
#include "effects.h"
#include "items.h"
#include "it_use2.h"
#include "macro.h"
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

/*
Activates a menu which gives player access to all of their non-spell
special abilities - Eg naga's spit poison, or the Invocations you get
from worshipping. Generated dynamically - the function checks to see which
abilities you have every time.
*/
void species_ability(void) /* Now handles all special abilities */
{

        if (you[0].berserker > 0)
        {
                mpr("You're too berserk!");
                return;
        }

        if (you[0].conf > 0)
        {
                mpr("You're too confused!");
                return;
        }

        struct dist abild [1];
        struct bolt beam [1];
        struct dist spd [1];

        int ability [120];
        int ability_fail [120];
        char abil_c = 0;
        unsigned char abil_used;
        unsigned char spc, spc2;
        char keyi;
        int i = 0;

        for (i = 0; i < 120; i ++)
        {
                ability [i] = 0;
                ability_fail [i] = 0;
        }

        switch(you[0].species)
        {
                case SP_NAGA: // Naga
                        ability [abil_c] = ABIL_SPIT_POISON;
                        ability_fail [abil_c] = 20 - you[0].xl;
                        abil_c ++;
                        break;
                case SP_GNOME: // Gnome
                        ability [abil_c] = ABIL_MAPPING;
                        ability_fail [abil_c] = 20 - you[0].xl;
                        abil_c ++;
                        break;
        }

        if (you[0].species != SP_NAGA && you[0].mutation [MUT_SPIT_POISON] > 0) // spit poison
        {
                ability [abil_c] = ABIL_SPIT_POISON;
                ability_fail [abil_c] = 30 - you[0].mutation [MUT_SPIT_POISON] * 10 - you[0].xl;
                abil_c ++;
        }

        if (you[0].species != SP_GNOME && (you[0].mutation [MUT_MAPPING] > 0 || scan_randarts(RAP_MAPPING) > 0)) // sense surr
        {
                ability [abil_c] = ABIL_MAPPING;
                ability_fail [abil_c] = 30 - you[0].mutation [MUT_MAPPING] * 10 - you[0].xl;
                abil_c ++;
        }

        if (you[0].attribute [ATTR_TRANSFORMATION] == TRAN_DRAGON || you[0].mutation [MUT_BREATHE_FLAMES] > 0 || (you[0].species == SP_RED_DRACONIAN && you[0].xl > 6)) // breathe fire
        {
                ability [abil_c] = ABIL_BREATHE_FIRE;
                ability_fail [abil_c] = 50 - you[0].mutation [MUT_BREATHE_FLAMES] * 10 - you[0].xl - (you[0].attribute [ATTR_TRANSFORMATION] == TRAN_DRAGON) * 20;
                if (you[0].species == SP_RED_DRACONIAN) ability_fail [abil_c] = 30 - you[0].xl;
                abil_c ++;
        }

        if (you[0].species == SP_WHITE_DRACONIAN && you[0].xl > 6) // breathe frost
        {
                ability [abil_c] = ABIL_BREATHE_FROST;
                ability_fail [abil_c] = 30 - you[0].xl;
                abil_c ++;
        }

        if (you[0].species == SP_GREEN_DRACONIAN && you[0].xl > 6) // breathe poison
        {
                ability [abil_c] = ABIL_BREATHE_POISON;
                ability_fail [abil_c] = 30 - you[0].xl;
                abil_c ++;
        }

        if (you[0].species == SP_GOLDEN_DRACONIAN && you[0].xl > 6) // spit acid
        {
                ability [abil_c] = ABIL_SPIT_ACID;
                ability_fail [abil_c] = 30 - you[0].xl;
                abil_c ++;
        }

        if (you[0].species == SP_BLACK_DRACONIAN && you[0].xl > 6) // breathe lightning
        {
                ability [abil_c] = ABIL_BREATHE_LIGHTNING;
                ability_fail [abil_c] = 30 - you[0].xl;
                abil_c ++;
        }

        if (you[0].species == SP_PURPLE_DRACONIAN && you[0].xl > 6) // breathe power
        {
                ability [abil_c] = ABIL_BREATHE_POWER;
                ability_fail [abil_c] = 30 - you[0].xl;
                abil_c ++;
        }

        if (you[0].species == SP_MOTTLED_DRACONIAN && you[0].xl > 6) // spit sticky flame
        {
         ability [abil_c] = ABIL_BREATHE_STICKY_FLAME;
         ability_fail [abil_c] = 30 - you[0].xl;
         abil_c ++;
        }

        if (you[0].species == SP_PALE_DRACONIAN && you[0].xl > 6) // breathe steam
        {
         ability [abil_c] = ABIL_BREATHE_STEAM;
         ability_fail [abil_c] = 20 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [MUT_BLINK] > 0 || scan_randarts(RAP_BLINK) > 0) // blink
        {
                ability [abil_c] = ABIL_BLINK;
                ability_fail [abil_c] = 30 - you[0].mutation [MUT_BLINK] * 10;
                abil_c ++;
        }



        if (wearing_amulet(AMU_RAGE) != 0 || scan_randarts(RAP_BERSERK) > 0)
        {
                ability [abil_c] = ABIL_GO_BERSERK_I;
                ability_fail [abil_c] = 50 - you[0].xl * 2;
                if (you[0].species == SP_HILL_DWARF || you[0].species == SP_MOUNTAIN_DWARF || you[0].species == SP_HILL_ORC || you[0].species == SP_OGRE) ability_fail [abil_c] -= 10;
                if (you[0].species == SP_TROLL) ability_fail [abil_c] -= 30; /* troll */
                abil_c ++;
        }

        if (you[0].species == SP_KENKU && you[0].xl >= 5 && you[0].duration [DUR_CONTROLLED_FLIGHT] == 0 && you[0].lev == 0) // kenku can fly, but only from the ground (until lev 15, when it becomes permanent until revoked)
        {
                ability [abil_c] = ABIL_FLY;
                ability_fail [abil_c] = 45 - you[0].xl * 3;
                abil_c ++;
        } /* Kenku can fly */

/* Assorted demonic powers follow: */

        if (you[0].mutation [MUT_SUMMON_MINOR_DEMONS] != 0) // Summon minor demon
        {
         ability [abil_c] = ABIL_SUMMON_MINOR_DEMON;
         ability_fail [abil_c] = 27 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [MUT_SUMMON_DEMONS] != 0) // Summon demon
        {
         ability [abil_c] = ABIL_SUMMON_DEMON;
         ability_fail [abil_c] = 40 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [MUT_HURL_HELLFIRE] != 0) // Hellfire
        {
         ability [abil_c] = ABIL_HELLFIRE;
         ability_fail [abil_c] = 50 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [MUT_CALL_TORMENT] != 0) // Torment
        {
         ability [abil_c] = ABIL_TORMENT;
         ability_fail [abil_c] = 60 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [MUT_RAISE_DEAD] != 0) // Raise Dead
        {
         ability [abil_c] = ABIL_RAISE_DEAD;
         ability_fail [abil_c] = 50 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [MUT_CONTROL_DEMONS] != 0) // Control Demon
        {
         ability [abil_c] = ABIL_CONTROL_DEMON;
         ability_fail [abil_c] = 35 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [MUT_PANDEMONIUM] != 0) // Gate to Pan
        {
         ability [abil_c] = ABIL_TO_PANDEMONIUM;
         ability_fail [abil_c] = 57 - you[0].xl * 2;
         abil_c ++;
        }

        if (you[0].mutation [MUT_CHANNEL_HELL] != 0) // Channel from Hell
        {
         ability [abil_c] = ABIL_CHANNELING;
         ability_fail [abil_c] = 30 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [MUT_THROW_FLAMES] != 0) // Throw flame
        {
         ability [abil_c] = ABIL_THROW_FLAME;
         ability_fail [abil_c] = 10 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [MUT_THROW_FROST] != 0) // Throw frost
        {
         ability [abil_c] = ABIL_THROW_FROST;
         ability_fail [abil_c] = 10 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [MUT_SMITE] != 0) // Bolt of Draining
        {
         ability [abil_c] = ABIL_BOLT_OF_DRAINING;
         ability_fail [abil_c] = 30 - you[0].xl;
         abil_c ++;
        }

/* Most abilities from items are displaced by 50, so that abilities obtained
naturally or from mutations can be put earlier, to avoid the menu letters
always changing around */

        if (((you[0].equip [EQ_LEFT_RING] != -1 && you[0].inv_type [you[0].equip [EQ_LEFT_RING]] == RING_INVISIBILITY) || (you[0].equip [EQ_RIGHT_RING] != -1 && you[0].inv_type [you[0].equip [EQ_RIGHT_RING]] == RING_INVISIBILITY) || (you[0].equip [EQ_CLOAK] != -1 && you[0].inv_dam [you[0].equip [EQ_CLOAK]] % 30 == SPARM_DARKNESS) || (you[0].equip [EQ_HELMET] != -1 && you[0].inv_dam [you[0].equip [EQ_HELMET]] % 30 == SPARM_DARKNESS) || scan_randarts(RAP_INVISIBLE) > 0) && you[0].invis == 0)
        {
                ability [abil_c] = ABIL_TURN_INVISIBLE;
                ability_fail [abil_c] = 50 - you[0].xl * 2;
                abil_c ++;
        }

        if (you[0].invis != 0)
        {
                ability [abil_c] = ABIL_TURN_VISIBLE;
                ability_fail [abil_c] = 0;
                abil_c ++;
        }

        if ((you[0].equip [EQ_LEFT_RING] != -1 && you[0].inv_type [you[0].equip [EQ_LEFT_RING]] == RING_LEVITATION) || (you[0].equip [EQ_RIGHT_RING] != -1 && you[0].inv_type [you[0].equip [EQ_RIGHT_RING]] == RING_LEVITATION) || (you[0].equip [EQ_BOOTS] != -1 && you[0].inv_dam [you[0].equip [EQ_BOOTS]] % 30 == SPARM_LEVITATION) || (scan_randarts(RAP_LEVITATE) > 0) && you[0].lev == 0)
        {
                ability [abil_c] = ABIL_LEVITATE;
                ability_fail [abil_c] = 50 - you[0].xl * 2;
                abil_c ++;
        }

        if (you[0].lev != 0)
        {
                ability [abil_c] = ABIL_STOP_LEVITATING;
                ability_fail [abil_c] = 0;
                abil_c ++;
        }

        if (you[0].duration [DUR_TRANSFORMATION] != 0)
        {
                ability [abil_c] = ABIL_END_TRANSFORMATION;
                ability_fail [abil_c] = 0;
                abil_c ++;
        }


        if ((you[0].equip [EQ_LEFT_RING] != -1 && you[0].inv_type [you[0].equip [EQ_LEFT_RING]] == RING_TELEPORTATION) || (you[0].equip [EQ_RIGHT_RING] != -1 && you[0].inv_type [you[0].equip [EQ_RIGHT_RING]] == RING_TELEPORTATION) || you[0].mutation [MUT_TELEPORT_AT_WILL] != 0 || scan_randarts(RAP_CAN_TELEPORT) > 0)
        {
                ability [abil_c] = ABIL_TELEPORTATION;
                ability_fail [abil_c] = 50;
                if (you[0].mutation [MUT_TELEPORT_AT_WILL] >= 2) ability_fail [abil_c] = 20;
                abil_c ++;
        }


        abil_c = 26;

        if (you[0].piety >= 30)
        {
                switch(you[0].religion)
                {
                        case GOD_ZIN:
                                ability [abil_c] = ABIL_REPEL_UNDEAD_I;
                                ability_fail [abil_c] = 30 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 6;
                                abil_c ++;
                                break;
                        case GOD_SHINING_ONE:
                                ability [abil_c] = ABIL_REPEL_UNDEAD_II;
                                ability_fail [abil_c] = 30 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 6;
                                abil_c ++;
                                break;
                        case GOD_KIKUBAAQUDGHA:
                                ability [abil_c] = ABIL_RECALL_UNDEAD_SLAVES;
                                ability_fail [abil_c] = 30 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 6;
                                abil_c ++;
                                break;
                        case GOD_YREDELEMNUL:
                                ability [abil_c] = ABIL_ANIMATE_CORPSE;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 3;
                                abil_c ++;
                                break;
                        case GOD_OKAWARU:
                                ability [abil_c] = ABIL_MIGHT_I;
                                ability_fail [abil_c] = 30 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 6;
                                abil_c ++;
                                break;
                        case GOD_TROG:
                                ability [abil_c] = ABIL_GO_BERSERK_II;
                                ability_fail [abil_c] = 30 - you[0].piety;
                                abil_c ++;
                                break;
                        case GOD_ELYVILON:
                                ability [abil_c] = ABIL_LESSER_HEALING;
                                ability_fail [abil_c] = 30 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 6;
                                abil_c ++;
                                break;
                }
        }

        if (you[0].piety >= 50)
        {
                switch(you[0].religion)
                {
                        case GOD_ZIN:
                                ability [abil_c] = ABIL_MINOR_HEALING;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 5;
                                abil_c ++;
                                break;
                        case GOD_SHINING_ONE:
                                ability [abil_c] = ABIL_SMITING;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 5;
                                abil_c ++;
                                break;
                        case GOD_YREDELEMNUL:
                                ability [abil_c] = ABIL_RECALL_UNDEAD;
                                ability_fail [abil_c] = 50 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 4;
                                abil_c ++;
                                break;
                        case GOD_OKAWARU:
                                ability [abil_c] = ABIL_HEALING_I;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 5;
                                abil_c ++;
                                break;
                        case GOD_MAKHLEB:
                                ability [abil_c] = ABIL_MINOR_DESTRUCTION;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 5;
                                abil_c ++;
                                break;
                        case GOD_SIF_MUNA:
                                ability [abil_c] = ABIL_FORGET_SPELL;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 5;
                                abil_c ++;
                                break;
                        case GOD_TROG:
                                ability [abil_c] = ABIL_MIGHT_II;
                                ability_fail [abil_c] = 40 - (you[0].piety / 3);
                                abil_c ++;
                                break;
                        case GOD_ELYVILON:
                                ability [abil_c] = ABIL_PURIFICATION;
                                ability_fail [abil_c] = 20 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 5;
                                abil_c ++;
                                break;
                }
        }

        if (you[0].piety >= 75)
        {
                switch(you[0].religion)
                {
                        case GOD_ZIN:
                                ability [abil_c] = ABIL_PESTILENCE;
                                ability_fail [abil_c] = 60 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 5;
                                abil_c ++;
                                break;
                        case GOD_SHINING_ONE:
                                ability [abil_c] = ABIL_ANNIHILATE_UNDEAD;
                                ability_fail [abil_c] = 60 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 5;
                                abil_c ++;
                                break;
                        case GOD_KIKUBAAQUDGHA:
                                ability [abil_c] = ABIL_ENSLAVE_UNDEAD;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 5;
                                abil_c ++;
                                break;
                        case GOD_YREDELEMNUL:
                                ability [abil_c] = ABIL_ANIMATE_DEAD;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 5;
                                abil_c ++;
                                break;
                        case GOD_MAKHLEB:
                                ability [abil_c] = ABIL_LESSER_SERVANT_OF_MAKHLEB;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 5;
                                abil_c ++;
                                break;
                        case GOD_ELYVILON:
                                ability [abil_c] = ABIL_HEALING_II;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [SK_INVOCATIONS] * 5;
                                abil_c ++;
                                break;
                }
        }

        if (you[0].piety >= 100)
        {
                switch(you[0].religion)
                {
                        case GOD_ZIN:
                                ability [abil_c] = ABIL_HOLY_WORD;
                                ability_fail [abil_c] = 70 - (you[0].piety / 15) - you[0].skills [SK_INVOCATIONS] * 4;
                                abil_c ++;
                                break;
                        case GOD_SHINING_ONE:
                                ability [abil_c] = ABIL_THUNDERBOLT;
                                ability_fail [abil_c] = 70 - (you[0].piety / 15) - you[0].skills [SK_INVOCATIONS] * 4;
                                abil_c ++;
                                break;
                        case GOD_YREDELEMNUL:
                                ability [abil_c] = ABIL_DRAIN_LIFE;
                                ability_fail [abil_c] = 60 - (you[0].piety / 15) - you[0].skills [SK_INVOCATIONS] * 4;
                                abil_c ++;
                                break;
                        case GOD_VEHUMET:
                                ability [abil_c] = ABIL_CHANNEL_ENERGY;
                                ability_fail [abil_c] = 40 - you[0].intel - you[0].skills [SK_INVOCATIONS];
                                abil_c ++;
                                break;
                        case GOD_MAKHLEB:
                                ability [abil_c] = ABIL_MAJOR_DESTRUCTION;
                                ability_fail [abil_c] = 60 - (you[0].piety / 15) - you[0].skills [SK_INVOCATIONS] * 4;
                                abil_c ++;
                                break;
                        case GOD_TROG:
                                ability [abil_c] = ABIL_HASTE_SELF;
                                ability_fail [abil_c] = 70 - (you[0].piety / 15) - you[0].skills [SK_INVOCATIONS] * 4;
                                abil_c ++;
                                break;
                        case GOD_ELYVILON:
                                ability [abil_c] = ABIL_RESTORATION;
                                ability_fail [abil_c] = 70 - (you[0].piety / 15) - you[0].skills [SK_INVOCATIONS] * 4;
                                abil_c ++;
                                break;
                }
        }

        if (you[0].piety >= 120)
        {
                switch(you[0].religion)
                {
                        case GOD_ZIN:
                                ability [abil_c] = ABIL_SUMMON_GUARDIAN;
                                ability_fail [abil_c] = 80 - (you[0].piety / 15) - you[0].skills [SK_INVOCATIONS] * 4;
                                abil_c ++;
                                break;
                        case GOD_SHINING_ONE:
                                ability [abil_c] = ABIL_SUMMON_DAEVA;
                                ability_fail [abil_c] = 80 - (you[0].piety / 15) - you[0].skills [SK_INVOCATIONS] * 4;
                                abil_c ++;
                                break;
                        case GOD_KIKUBAAQUDGHA:
                                ability [abil_c] = ABIL_INVOKE_DEATH;
                                ability_fail [abil_c] = 80 - (you[0].piety / 15) - you[0].skills [SK_INVOCATIONS] * 4;
                                abil_c ++;
                                break;
                        case GOD_YREDELEMNUL:
                                ability [abil_c] = ABIL_CONTROL_UNDEAD;
                                ability_fail [abil_c] = 70 - (you[0].piety / 15) - you[0].skills [SK_INVOCATIONS] * 4;
                                abil_c ++;
                                break;
                        case GOD_OKAWARU:
                                ability [abil_c] = ABIL_HASTE;
                                ability_fail [abil_c] = 70 - (you[0].piety / 15) - you[0].skills [SK_INVOCATIONS] * 4;
                                abil_c ++;
                                break;
                        case GOD_MAKHLEB:
                                ability [abil_c] = ABIL_GREATER_SERVANT_OF_MAKHLEB;
                                ability_fail [abil_c] = 70 - (you[0].piety / 15) - you[0].skills [SK_INVOCATIONS] * 4;
                                abil_c ++;
                                break;
                        case GOD_ELYVILON:
                                ability [abil_c] = ABIL_GREATER_HEALING;
                                ability_fail [abil_c] = 80 - (you[0].piety / 15) - you[0].skills [SK_INVOCATIONS] * 4;
                                abil_c ++;
                                break;
                }
        }


        if (you[0].religion != GOD_NO_GOD)
        {
                ability [49] = ABIL_RENOUNCE_RELIGION;
                ability_fail [49] = 0;
        }


        for (i = 0; i < 120; i ++)
        {
                if (ability [i] != 0) break;
                if (i == 119)
                {
                        strcpy(info, "Sorry, you're not good enough to have a special ability.");
                        mpr(info);
                        return;
                }
        }


/*
if (you[0].hung_state <= 2)
{
 strcpy(info, "You're too hungry.");
 mpr(info);
 return; // no you[0].turnover?
}

if (you[0].duration [DUR_BREATH_WEAPON] != 0)
{
 strcpy(info, "You can't do that yet.");
 mpr(info);
 return; // no you[0].turnover?
}
*/
        query :
        strcpy(info, "Use which ability? (? or * to list)");
        mpr(info);

        int keyin = get_ch();

        if (keyin == '?' || keyin == '*')
        {
                char unthing = show_abilities(ability, ability_fail); //you[0].spells); //invent(0, you[0].inv_quant, you[0].inv_dam, you[0].inv_class, you[0].inv_type, you[0].inv_plus, you[0].inv_ident, you[0].equip [0], you[0].equip [6], you[0].equip [5], you[0].equip [2], you[0].equip [1], you[0].equip [3], you[0].equip [4], you[0].ring);
#ifdef PLAIN_TERM
                redraw_screen();
#endif

                if (unthing == 2) return;

                if ((unthing >= 65 && unthing <= 90) || (unthing >= 97 && unthing <= 122))
                {
                        keyin = unthing;
                } else
                {
                mesclr();
                        goto query;
                }
        }

        spc = (int) keyin;

        if ((spc < 97 || spc > 121) && (spc < 65 || spc > 90))
        {
                unknown : strcpy(info, "You can't do that.");
                mpr(info);
                return;
        }

        spc2 = conv_lett(spc);

        if (ability [spc2] == 0)
        {
                goto unknown;
        }

        abil_used = spc2;

        if (random2(33) + random2(33) + random2(34) <= ability_fail [abil_used])
        {
                mpr("You fail to use your ability.");
                you[0].turnover = 1;
                return;
        }

    you[0].turnover = 1;

        you[0].ep_ch = 1;

        switch(ability [abil_used])
        {
                case ABIL_SPIT_POISON: // Naga, + spit poison mutation
                        if (you[0].duration [DUR_BREATH_WEAPON] != 0)
                        {
                                mpr("You can't do that yet.");
                                return;
                        }
                        if (you[0].hung_state <= 2)
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
                        you[0].turnover = 1;
                        you[0].duration [DUR_BREATH_WEAPON] += random2(5) + 3;
                        zapping(ZAP_SPIT_POISON, you[0].xl + you[0].mutation [MUT_SPIT_POISON] * 5, beam);
                        you[0].hunger -= 40 + random2(40) + random2(40);
                        strcpy(info, "You feel slightly more hungry.");
                        mpr(info);
                        break;

                case ABIL_MAPPING: // Gnome + sense surrounds mut
                        strcpy(info, "You sense your surroundings.");
                        mpr(info);
// you[0].duration [17] += 30 - you[0].xl + random2(40 - you[0].xl) + random2(5);
                        magic_mapping(3 + random2(you[0].xl) + random2(you[0].xl) + you[0].mutation [MUT_MAPPING] * 10, 40 + random2(you[0].xl) + random2(you[0].xl) + random2(you[0].xl));
                        you[0].hunger -= 30 + random2(30) + random2(30);
                        you[0].turnover = 1;
                        break;

                case ABIL_TELEPORTATION: // ring of teleport, + teleport mut
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 200 + random2(200) + random2(200);
                        if (you[0].mutation [MUT_TELEPORT_AT_WILL] == 3) you_teleport2(1);
                        else you_teleport();
                        break;

                case ABIL_BREATHE_FIRE:
                case ABIL_BREATHE_FROST:
                case ABIL_BREATHE_POISON:
                case ABIL_BREATHE_LIGHTNING:
                case ABIL_SPIT_ACID:
                case ABIL_BREATHE_POWER:
                case ABIL_BREATHE_STICKY_FLAME:
                case ABIL_BREATHE_STEAM: /* Breathe things */
                        if (you[0].duration [DUR_BREATH_WEAPON] != 0 && ability [abil_used] != ABIL_SPIT_ACID)
                        {
                                mpr("You can't do that yet.");
                                return;
                        }
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (spell_direction(abild, beam) == -1)
                        {
                                mpr("Okay, then.");
                                return;
                        }
                        switch(ability [abil_used])
                        {
                                case ABIL_BREATHE_FIRE: mpr("You breathe fire.");
                                        zapping(ZAP_BREATHE_FIRE, you[0].xl + you[0].mutation [MUT_SPIT_POISON] * 4 + (you[0].attribute [ATTR_TRANSFORMATION] == TRAN_DRAGON) * 10, beam); break;
                                case ABIL_BREATHE_FROST: mpr("You breathe a wave of freezing cold.");
                                        zapping(ZAP_BREATHE_FROST, you[0].xl, beam); break;
                                case ABIL_BREATHE_POISON: mpr("You breathe a blast of poison gas.");
                                        zapping(ZAP_BREATHE_POISON, you[0].xl, beam); break;
                                case ABIL_BREATHE_LIGHTNING: mpr("You spit a bolt of lightning.");
                                        zapping(ZAP_LIGHTNING, you[0].xl * 2, beam); break;
                                case ABIL_SPIT_ACID: mpr("You spit acid.");
                                        zapping(ZAP_BREATHE_ACID, you[0].xl, beam); break;
                                case ABIL_BREATHE_POWER: mpr("You spit a bolt of incandescent energy.");
                                        zapping(ZAP_BREATHE_POWER, you[0].xl, beam); break;
                case ABIL_BREATHE_STICKY_FLAME: mpr("You spit a glob of burning liquid.");
                                        zapping(ZAP_STICKY_FLAME, you[0].xl, beam); break;
                            case ABIL_BREATHE_STEAM: mpr("You breathe a blast of scalding steam.");
                                        zapping(ZAP_BREATHE_STEAM, you[0].xl, beam); break;
                }
            you[0].turnover = 1;
                        if (ability [abil_used] != ABIL_SPIT_ACID) you[0].duration [DUR_BREATH_WEAPON] += random2(5) + 3 + random2(30 - you[0].xl);
            if (ability [abil_used] == ABIL_BREATHE_STEAM) you[0].duration [DUR_BREATH_WEAPON] /= 2;
                        you[0].hunger -= 200 + random2(100) + random2(100) - you[0].xl * random2(5);
                        strcpy(info, "You feel slightly more hungry.");
                        mpr(info);
                        break;

                case ABIL_BLINK: // blink mut
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (you[0].ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 50 + random2(50) + random2(50);
                        random_blink();
                        break;

                case ABIL_GO_BERSERK_I: // berserk
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (go_berserk() == 0) mpr("You fail to go berserk.");
                        break;

                case ABIL_FLY: // Fly (kenku). Eventually becomes permanent (handled in acr.cc)
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 100 + random2(100) + random2(100);
            cast_fly(you[0].xl * 4);
            if (you[0].xl >= 15)
            {
              mpr("You feel very comfortable in the air.");
              you[0].lev = 100;
              you[0].duration [DUR_CONTROLLED_FLIGHT] = 100;
            }
                        break;

/* Demonic powers follow: */

                case ABIL_SUMMON_MINOR_DEMON:  /* summon minor demon */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (you[0].hp <= 3)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hp -= 3;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 50 + random2(100) + random2(100);
                        summon_ice_beast_etc(you[0].xl * 4, summon_any_demon(0));
                        break;

                case ABIL_SUMMON_DEMON:  /* summon demon */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 5)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (you[0].hp <= 5)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        you[0].ep -= 5;
                        you[0].ep_ch = 1;
                        you[0].hp -= 5;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 100 + random2(200) + random2(200);
                        summon_ice_beast_etc(you[0].xl * 4, summon_any_demon(1));
                        break;

                case ABIL_HELLFIRE:  /* Hellfire */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 8)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (you[0].hp <= 8)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        you[0].ep -= 8;
                        you[0].ep_ch = 1;
                        you[0].hp -= 8;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 200 + random2(300) + random2(300);
            your_spells(SPELL_HELLFIRE, 100, 0); // power (2nd number) is meaningless
                        break;

                case ABIL_TORMENT:  /* Torment */
                        if (you[0].is_undead != 0)
                        {
                                mpr("Undead cannot use this ability.");
                                return;
                        }
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 9)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 9;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 300 + random2(300) + random2(300);
            torment();
                        break;

                case ABIL_RAISE_DEAD:  /* Raise dead */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 5)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (you[0].hp <= 5)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        you[0].ep -= 5;
                        you[0].ep_ch = 1;
                        you[0].hp -= 5;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 100 + random2(200) + random2(200);
            your_spells(SPELL_ANIMATE_DEAD, you[0].xl * 5, 0); // power (2nd number) is meaningless
                        break;

                case ABIL_CONTROL_DEMON:  /* Control demon */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (you[0].hp <= 4)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        if (spell_direction(abild, beam) == -1)
                        {
                                mpr("Okay, then.");
                                return;
                        }
                        you[0].ep -= 4;
                        you[0].ep_ch = 1;
                        you[0].hp -= 4;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 100 + random2(100) + random2(100);
                        zapping(ZAP_CONTROL_DEMON, you[0].xl * 5, beam);
                        break;

                case ABIL_TO_PANDEMONIUM:  /* Pandemonium */
            if (you[0].level_type == 3)
            {
                                mpr("You're already here.");
                                return;
            }
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 7;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 200 + random2(200) + random2(200);
            banished(99);
                        break;

                case ABIL_CHANNELING:  /* Channeling */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].hp < 1)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        you[0].hp -= 1;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 20 + random2(30) + random2(30);
                    you[0].ep += 1 + random2(5);
                    if (you[0].ep > you[0].ep_max) you[0].ep = you[0].ep_max;
                    you[0].ep_ch = 1;
                    mpr("You channel some magical energy.");
                        break;

        case ABIL_THROW_FLAME:
                case ABIL_THROW_FROST:  /* Throw flame/frost */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (you[0].hp <= 1)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        if (spell_direction(abild, beam) == -1)
                        {
                                mpr("Okay, then.");
                                return;
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        you[0].hp -= 1;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 50 + random2(50) + random2(50);
                        if (ability [abil_used] == ABIL_THROW_FLAME) zapping(ZAP_FLAME, you[0].xl * 3, beam);
             else zapping(ZAP_FROST, you[0].xl * 3, beam);
                        break;

                case ABIL_BOLT_OF_DRAINING:  /* bolt of draining */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (you[0].hp <= 4)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        if (spell_direction(abild, beam) == -1)
                        {
                                mpr("Okay, then.");
                                return;
                        }
                        you[0].ep -= 4;
                        you[0].ep_ch = 1;
                        you[0].hp -= 4;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 100 + random2(100) + random2(100);
                        zapping(ZAP_NEGATIVE_ENERGY, you[0].xl * 6, beam);
                        break;

/* Items etc */
                case ABIL_TURN_INVISIBLE: /* turn invis */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 2)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 2;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 250 + random2(250) + random2(250);
                        potion_effect(POT_INVISIBILITY, you[0].xl + 10);
                        break;

                case ABIL_TURN_VISIBLE: /* uninvisible */
            mpr("You feel less transparent.");
                        you[0].invis = 1;
                        break;

                case ABIL_LEVITATE: /* levitate */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 100 + random2(100) + random2(100);
                        potion_effect(POT_LEVITATION, you[0].xl + 10);
                        break;

                case ABIL_STOP_LEVITATING: /* unlevitate */
            mpr("You feel heavy.");
                        you[0].lev = 1;
                        break;

                case ABIL_END_TRANSFORMATION: /* untransform */
            mpr("You feel almost normal.");
                        you[0].duration [DUR_TRANSFORMATION] = 2;
                        break;


/* Invocations start here: */

                case ABIL_REPEL_UNDEAD_I:
                case ABIL_REPEL_UNDEAD_II:
                        if (you[0].     ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        turn_undead(you[0].piety);
            exercise(SK_INVOCATIONS, 1);
                        break;

                case ABIL_MINOR_HEALING: /* Minor Healing */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].     ep < 2)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 2;
                        you[0].ep_ch = 1;
                        if (cast_lesser_healing() == 0) break;
                        you[0].hunger -= 50 + random2(50) + random2(50);
                        lose_piety(1);
            exercise(SK_INVOCATIONS, 1 + random2(3));
                        break;

                case ABIL_PESTILENCE: /* Pestilence */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 100 + random2(100) + random2(100);
                        summon_swarm(you[0].piety);
                        lose_piety(2 + random2(2));
            exercise(SK_INVOCATIONS, 2 + random2(4));
                        break;

                case ABIL_HOLY_WORD: /* Holy Word */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].     ep < 6)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 6;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 150 + random2(150) + random2(150);
                        holy_word(you[0].piety);
                        lose_piety(3 + random2(2));
            exercise(SK_INVOCATIONS, 3 + random2(5));
                        break;

                case ABIL_SUMMON_GUARDIAN:  /* Guardian */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 4;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 150 + random2(150) + random2(150);
                        summon_ice_beast_etc(you[0].skills [SK_INVOCATIONS] * 4, 26);
                        lose_piety(3 + random2(4));
            exercise(SK_INVOCATIONS, 8 + random2(10));
                        break;




                case ABIL_SMITING:
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 50 + random2(50) + random2(50);
                        cast_smiting(you[0].skills [SK_INVOCATIONS] * 8);
                        lose_piety(1);
            exercise(SK_INVOCATIONS, 2 + random2(2));
                        break;

                case ABIL_ANNIHILATE_UNDEAD:  /* destroy undead */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (spell_direction(spd, beam) == -1)
                        {
                                mpr("Okay, then.");
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 50 + random2(50) + random2(50);
                        zapping(ZAP_DISPEL_UNDEAD, you[0].skills [SK_INVOCATIONS] * 6, beam);
                        lose_piety(1 + random2(2));
            exercise(SK_INVOCATIONS, 2 + random2(4));
                        break;

                case ABIL_THUNDERBOLT:  /* thunderbolt */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 5)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (spell_direction(spd, beam) == -1)
                        {
                                mpr("Okay, then.");
                        }
                        you[0].ep -= 5;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 100 + random2(100) + random2(100);
                        zapping(ZAP_LIGHTNING, you[0].skills [SK_INVOCATIONS] * 6, beam);
                        lose_piety(1 + random2(2));
            exercise(SK_INVOCATIONS, 3 + random2(6));
                        break;

                case ABIL_SUMMON_DAEVA:  /* Daeva */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 5)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 5;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 150 + random2(150) + random2(150);
                        summon_ice_beast_etc(you[0].skills [SK_INVOCATIONS] * 4, 366);
                        lose_piety(3 + random2(4));
            exercise(SK_INVOCATIONS, 8 + random2(10));
                        break;


                case ABIL_RECALL_UNDEAD_SLAVES:  /* recall undead */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        recall(1);
            exercise(SK_INVOCATIONS, 1);
                        break;

                case ABIL_ENSLAVE_UNDEAD:  /* enslave undead */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (spell_direction(spd, beam) == -1)
                        {
                                mpr("Okay, then.");
                        }
                        you[0].ep -= 4;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 150 + random2(150) + random2(150);
                        zapping(ZAP_ENSLAVE_UNDEAD, you[0].skills [SK_INVOCATIONS] * 8, beam);
                        lose_piety(2 + random2(2));
            exercise(SK_INVOCATIONS, 5 + random2(5));
                        break;

                case ABIL_INVOKE_DEATH:  /* Invoke Death */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 4;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 350 + random2(350) + random2(350);
                        summon_ice_beast_etc(20 + you[0].skills [SK_INVOCATIONS] * 3, 83);
                        lose_piety(3 + random2(4));
            exercise(SK_INVOCATIONS, 10 + random2(14));
                        break;


                case ABIL_ANIMATE_CORPSE:  /* Animate Skeleton */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 50 + random2(50) + random2(50);
            mpr("You call on the dead to walk for you...");
            animate_a_corpse(you[0].x_pos, you[0].y_pos, 7, you[0].pet_target, 0);
            exercise(SK_INVOCATIONS, 2 + random2(4));
                        break;

                case ABIL_RECALL_UNDEAD:  /* Recall Undead */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 4;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 100 + random2(100) + random2(100);
            recall(1);
            exercise(SK_INVOCATIONS, 2 + random2(4));
                        break;

                case ABIL_ANIMATE_DEAD:  /* Animate Dead */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 7)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 7;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 150 + random2(150) + random2(150);
            mpr("You call on the dead to walk for you...");
            animate_dead(you[0].skills [SK_INVOCATIONS] + 1, 7, you[0].pet_target, 1);
            exercise(SK_INVOCATIONS, 2 + random2(4));
                        lose_piety(1 + random2(2));
                        break;

                case ABIL_DRAIN_LIFE:  /* Drain Life */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 6)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 6;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 150 + random2(150) + random2(150);
                        drain_life(you[0].skills [SK_INVOCATIONS]);
            exercise(SK_INVOCATIONS, 2 + random2(4));
                        lose_piety(1 + random2(3));
                        break;

                case ABIL_CONTROL_UNDEAD:  /* Control Undead */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 5)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 5;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 150 + random2(150) + random2(250);
            mass_enchantment(30, you[0].skills [SK_INVOCATIONS] * 8);
            exercise(SK_INVOCATIONS, 3 + random2(4));
                        lose_piety(2 + random2(3));
                        break;

                case ABIL_CHANNEL_ENERGY: /* channeling */
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        you[0].hunger -= 50 + random2(50) + random2(50);
                    you[0].ep += 1 + random2(you[0].skills [SK_INVOCATIONS] / 4 + 2);
                    if (you[0].ep > you[0].ep_max) you[0].ep = you[0].ep_max;
                    you[0].ep_ch = 1;
                    strcpy(info, "You channel some magical energy.");
                    mpr(info);
            exercise(SK_INVOCATIONS, 1 + random2(3));
                        break;

                case ABIL_MIGHT_I: /* might */
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (you[0].ep < 2)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 2;
                        potion_effect(POT_MIGHT, you[0].skills [SK_INVOCATIONS] * 8);
                        lose_piety(1);
                        you[0].hunger -= 50 + random2(50) + random2(50);
            exercise(SK_INVOCATIONS, 1 + random2(3));
                        break;

                case ABIL_HEALING_I: /* Minor healing */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 2)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 2;
                        if (cast_lesser_healing() == 0) break;
                        lose_piety(1);
                        you[0].hunger -= 100 + random2(50) + random2(50);
            exercise(SK_INVOCATIONS, 2 + random2(5));
                        break;


                case ABIL_HASTE: /* haste */
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (you[0].ep < 5)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 5;
                        potion_effect(POT_SPEED, you[0].skills [SK_INVOCATIONS] * 8);
                        lose_piety(2 + random2(3));
                        you[0].hunger -= 100 + random2(100) + random2(100);
            exercise(SK_INVOCATIONS, 3 + random2(7));
                        break;




                case ABIL_MINOR_DESTRUCTION:  /* minor destr */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (spell_direction(spd, beam) == -1)
                        {
                                mpr("Okay, then.");
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 20 + random2(20) + random2(20);
                        switch(random2(5))
                        {
                                case 0: zapping(ZAP_FLAME, you[0].skills [SK_INVOCATIONS] * 6, beam); break;
                                case 1: zapping(ZAP_AGONY_I, you[0].skills [SK_INVOCATIONS] * 6, beam); break;
                                case 2: zapping(ZAP_STONE_ARROW, you[0].skills [SK_INVOCATIONS] * 6, beam); break;
                                case 3: zapping(ZAP_ELECTRICITY, you[0].skills [SK_INVOCATIONS] * 6, beam); break;
                                case 4: zapping(ZAP_BREATHE_ACID, you[0].skills [SK_INVOCATIONS] * 6, beam); break;
                        }
            exercise(SK_INVOCATIONS, 1 + random2(2));
                        break;

                case ABIL_LESSER_SERVANT_OF_MAKHLEB:  /* minor summoning */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 2)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 2;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 50 + random2(50) + random2(50);
                        summon_ice_beast_etc(20 + you[0].skills [SK_INVOCATIONS] * 3, 225 + random2(5));
                        lose_piety(1 + random2(2));
            exercise(SK_INVOCATIONS, 2 + random2(3));
                        break;

                case ABIL_MAJOR_DESTRUCTION:  /* major destr */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (spell_direction(spd, beam) == -1)
                        {
                                mpr("Okay, then.");
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 100 + random2(50) + random2(50);
                        switch(random2(8))
                        {
                                case 0: zapping(ZAP_FIRE, you[0].skills [SK_INVOCATIONS] * 5, beam); break;
                                case 1: zapping(ZAP_FIREBALL, you[0].skills [SK_INVOCATIONS] * 5, beam); break;
                                case 2: zapping(ZAP_LIGHTNING, you[0].skills [SK_INVOCATIONS] * 5, beam); break;
                                case 3: zapping(ZAP_NEGATIVE_ENERGY, you[0].skills [SK_INVOCATIONS] * 5, beam); break;
                                case 4: zapping(ZAP_STICKY_FLAME, you[0].skills [SK_INVOCATIONS] * 5, beam); break;
                                case 5: zapping(ZAP_IRON_BOLT, you[0].skills [SK_INVOCATIONS] * 5, beam); break;
                                case 6: zapping(ZAP_ORB_OF_ELECTRICITY, you[0].skills [SK_INVOCATIONS] * 5, beam); break;
                                case 7: you[0].attribute [ATTR_RESIST_LIGHTNING] ++;
                                        mpr("Makhleb hurls a blast of lightning!");
                                        beam[0].beam_source = MNG;
                                        beam[0].type = 43;
                                        beam[0].damage = 130;
                                        beam[0].flavour = 5;
                                        beam[0].bx = you[0].x_pos;
                                        beam[0].by = you[0].y_pos;
                                        strcpy(beam[0].beam_name, "blast of lightning");
                                        beam[0].colour = LIGHTCYAN;
                                        beam[0].thing_thrown = 1; /* your expl */
                                        explosion(1, beam);
                                        you[0].attribute [ATTR_RESIST_LIGHTNING] --;
                                        break;
                        }
                        lose_piety(1 + random2(2));
            exercise(SK_INVOCATIONS, 3 + random2(5));
                        break;

                case ABIL_GREATER_SERVANT_OF_MAKHLEB:  /* major summoning */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 4;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 100 + random2(50) + random2(50);
                        summon_ice_beast_etc(20 + you[0].skills [SK_INVOCATIONS] * 3, 230 + random2(5));
                        lose_piety(2 + random2(2));
            exercise(SK_INVOCATIONS, 6 + random2(6));
                        break;



                case ABIL_GO_BERSERK_II: /* berserk */
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (go_berserk() == 0) mpr("You fail to go berserk.");
                        you[0].hunger -= 100 + random2(100) + random2(100);
            //exercise(SK_INVOCATIONS, 1 + random2(3));
                        break;

                case ABIL_MIGHT_II: /* might */
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        potion_effect(POT_MIGHT, you[0].skills [SK_INVOCATIONS] * 6);
                        lose_piety(1 + random2(2));
                        you[0].hunger -= 200 + random2(200) + random2(200);
            //exercise(SK_INVOCATIONS, 2 + random2(5));
                        break;

                case ABIL_HASTE_SELF: /* haste */
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        potion_effect(POT_SPEED, you[0].skills [SK_INVOCATIONS] * 6);
                        lose_piety(2 + random2(3));
                        you[0].hunger -= 250 + random2(250) + random2(250);
            //exercise(SK_INVOCATIONS, 4 + random2(6));
                        break;




                case ABIL_FORGET_SPELL:
                        cast_selective_amnesia();
            //exercise(SK_INVOCATIONS, 1);
                        break;


                case ABIL_LESSER_HEALING:
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        if (cast_lesser_healing() == 0) break;
                        you[0].hunger -= 100 + random2(100) + random2(100);
            exercise(SK_INVOCATIONS, 1 + random2(2));
                        break;

                case ABIL_PURIFICATION:
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        lose_piety(1);
                        you[0].hunger -= 150 + random2(150) + random2(150);
                        purification();
            exercise(SK_INVOCATIONS, 2 + random2(3));
                        break;

                case ABIL_HEALING_II:
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 2)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (cast_greater_healing() == 0) break;
                        you[0].ep -= 2;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 250 + random2(250) + random2(250);
                        lose_piety(1 + random2(3));
            exercise(SK_INVOCATIONS, 3 + random2(5));
                        break;

                case ABIL_RESTORATION:
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 400 + random2(450) + random2(450);
                    strcpy(info, "You feel refreshed."); // ...can't think of anything better right now
                        mpr(info);
                        restore_str();
                        restore_int();
                        restore_dex();
            if (you[0].base_hp < 5000) you[0].base_hp = 5000;
            calc_hp();
                        lose_piety(2 + random2(3));
            exercise(SK_INVOCATIONS, 4 + random2(6));
                        break;

                case ABIL_GREATER_HEALING:
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 6)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (cast_greatest_healing() == 0) break;
                        you[0].ep -= 6;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 600 + random2(550) + random2(550);
//cast_purification(you[0].piety);
                        lose_piety(4 + random2(4));
            exercise(SK_INVOCATIONS, 6 + random2(10));
                        break;


                case ABIL_RENOUNCE_RELIGION: /* give up religion */
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
char show_abilities(int ability [120], int ability_fail [120])
{
        int j;
        char lines = 0;
        unsigned char anything = 0;
        char strng [5] = "";
        char ft;
        char ki;
        char already = 0;
        char is_invocations = 0;

        for (j = 0; j < 120; j ++)
        {
        if (ability [j] >= 100) is_invocations = 1;
        }


#ifdef DOS_TERM
        char buffer[4800];
        gettext(1, 1, 80, 25, buffer);
        window(1, 1, 80, 25);
#endif

        clrscr();

        cprintf("  Ability                           Cost                    Success");
        lines++;

        if (lines > 22)
        {
                gotoxy(1,25);
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
                if (ki == 0) ki = getch();
                lines = 0;
                clrscr();
                gotoxy(1,1);
                anything = 0;

        }
        if (lines > 1) cprintf(EOL" ");

        for (j = 0; j < 120; j++)
        {

                if (lines > 23)
                {
                        gotoxy(1,25);
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
                        if (ki == 0) ki = getch();
                        lines = 0;
                        clrscr();
                        gotoxy(1,1);
                        anything = 0;
                }

                if (j == 25 && is_invocations == 1)
                {
                anything++;
                        textcolor(BLUE);
                        cprintf(EOL"    Invocations - ");
                        textcolor(LIGHTGREY);
                        lines++;
                        continue;
                }

                if (ability [j] != 0)
                {

                        anything ++;
                        if (lines > 0) cprintf(EOL);
                        lines++;
                        cprintf(" ");
                        if (j < 26) ft = (char) j + 97;
                        else ft = (char) j + 65 - 26;
                        strng [0] = ft;
                        cprintf(strng);
                        cprintf(" - ");

                        switch(ability [j])
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

                        int spell_f = ability_fail [j];

                        if (spell_f == 100)
                        {
                                cprintf("Useless");
                        } else
                        if (spell_f > 90)
                        {
                                cprintf("Terrible");
                        } else
                        if (spell_f > 80)
                        {
                                cprintf("Cruddy");
                        } else
                        if (spell_f > 70)
                        {
                                cprintf("Bad");
                        } else
                        if (spell_f > 60)
                        {
                                cprintf("Very Poor");
                        } else
                        if (spell_f > 50)
                        {
                                cprintf("Poor");
                        } else
                        if (spell_f > 40)
                        {
                                cprintf("Fair");
                        } else
                        if (spell_f > 30)
                        {
                                cprintf("Good");
                        } else
                        if (spell_f > 20)
                        {
                                cprintf("Very Good");
                        } else
                        if (spell_f > 10)
                        {
                                cprintf("Great");
                        } else
                        if (spell_f > 0)
                        {
                                cprintf("Excellent");
                        } else
                                cprintf("Perfect");

                        gotoxy(70, wherey());

                }
        } // end of j loop
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
                if (ki == 0) ki = getch();
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

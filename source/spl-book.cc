/*
 *  File:       spl-book.cc
 *  Summary:    Spellbook/Staff contents array and management functions
 *  Written by: Josh Fishman
 *
 *  Change History (most recent first):
 *
 *      24jun2000  jmf  Changes to many books; addition of Assassinations;
 *                      alteration of type-printout to match new bitfield.
 *  <1> 19mar2000  jmf  Created by taking stuff from dungeon.cc, spells0.cc,
 *                      spells.cc, shopping.cc
 */

#include "AppHdr.h"
#include "spl-book.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "debug.h"
#include "delay.h"
#include "invent.h"
#include "itemname.h"
#include "items.h"
#include "it_use3.h"
#include "player.h"
#include "religion.h"
#include "spl-cast.h"
#include "spl-util.h"
#include "stuff.h"

static bool which_spellbook( int &book, int &spell );
unsigned char spellbook_contents( item_def &book );

int spellbook_template_array[NUMBER_SPELLBOOKS][SPELLBOOK_SIZE] = {

    // 0 - Minor Magic I
    {0,
     SPELL_MAGIC_DART,
     SPELL_SUMMON_SMALL_MAMMAL,
     SPELL_THROW_FLAME,
     SPELL_BLINK,
     SPELL_SLOW,
     SPELL_MEPHITIC_CLOUD,
     SPELL_CONJURE_FLAME,
     SPELL_NO_SPELL,
     },
    // 1 - Minor Magic II
    {0,
     SPELL_MAGIC_DART,
     SPELL_THROW_FROST,
     SPELL_BLINK,
     SPELL_STICKS_TO_SNAKES,
     SPELL_SLOW,
     SPELL_MEPHITIC_CLOUD,
     SPELL_OZOCUBUS_ARMOUR,
     SPELL_NO_SPELL,
     },
    // 2 - Minor Magic III
    {0,
     SPELL_MAGIC_DART,
     SPELL_SUMMON_SMALL_MAMMAL,
     SPELL_BLINK,
     SPELL_SLOW,
     SPELL_SUMMON_LARGE_MAMMAL,
     SPELL_MEPHITIC_CLOUD,
     SPELL_DISCHARGE,
     SPELL_NO_SPELL,
     },
    // 3 - Book of Conjurations I
    {0,
     SPELL_MAGIC_DART,
     SPELL_THROW_FLAME,
     SPELL_STONE_ARROW,
     SPELL_CONJURE_FLAME,
     SPELL_BOLT_OF_MAGMA,
     SPELL_FIREBALL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 4 - Book of Conjurations II
    {0,
     SPELL_MAGIC_DART,
     SPELL_THROW_FROST,
     SPELL_MEPHITIC_CLOUD,
     SPELL_DISCHARGE,
     SPELL_BOLT_OF_COLD,
     SPELL_FREEZING_CLOUD,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 5 - Book of Flames
    {0,
     SPELL_FLAME_TONGUE,
     SPELL_THROW_FLAME,
     SPELL_CONJURE_FLAME,
     SPELL_STICKY_FLAME,
     SPELL_BOLT_OF_MAGMA,
     SPELL_FIREBALL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 6 - Book of Frost
    {0,
     SPELL_FREEZE,
     SPELL_THROW_FROST,
     SPELL_OZOCUBUS_ARMOUR,
     SPELL_ICE_BOLT,
     SPELL_SUMMON_ICE_BEAST,
     SPELL_FREEZING_CLOUD,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 7 - Book of Invocations //jmf: now Summonings
    {0,
     SPELL_ABJURATION_I,
     SPELL_RECALL,
     SPELL_SUMMON_LARGE_MAMMAL,
     SPELL_SHADOW_CREATURES,
     // SPELL_SWARM,
     SPELL_SUMMON_WRAITHS,
     SPELL_SUMMON_HORRIBLE_THINGS,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 8 - Book of Fire
    {0,
     SPELL_EVAPORATE,
     SPELL_FIRE_BRAND,
     SPELL_SUMMON_ELEMENTAL,
     SPELL_DELAYED_FIREBALL,
     SPELL_IGNITE_POISON,
     SPELL_RING_OF_FLAMES,
     SPELL_FIRE_STORM,
     SPELL_NO_SPELL,
     },
    // 9 - Book of Ice
    {0,
     SPELL_FREEZING_AURA,
     SPELL_SLEEP,
     SPELL_CONDENSATION_SHIELD,
     SPELL_BOLT_OF_COLD,
     SPELL_OZOCUBUS_REFRIGERATION,
     SPELL_MASS_SLEEP,
     SPELL_SIMULACRUM,
     SPELL_ICE_STORM,
     },

    // 10 - Book of Surveyances
    {0,
     SPELL_DETECT_SECRET_DOORS,
     SPELL_DETECT_TRAPS,
     SPELL_DETECT_ITEMS,
     SPELL_MAGIC_MAPPING,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL
     },
    // 11 - Book of Spatial Translocations
    {0,
     SPELL_TWIST,               // replacing bend with a LOS attack
     SPELL_APPORTATION,
     SPELL_BLINK,
     // SPELL_SWAP,
     SPELL_CONTROL_TELEPORT,
     SPELL_TELEPORT_SELF,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 12 - Book of Enchantments (fourth one)
    {0,
     SPELL_LEVITATION,
     SPELL_SELECTIVE_AMNESIA,
     SPELL_REMOVE_CURSE,
     SPELL_CAUSE_FEAR,
     SPELL_EXTENSION,
     SPELL_DEFLECT_MISSILES,
     SPELL_HASTE,
     SPELL_NO_SPELL,
     },
    // 13 - Young Poisoner's Handbook
    {0,
     SPELL_STING,
     SPELL_CURE_POISON_II,
     SPELL_MEPHITIC_CLOUD,
     SPELL_POISON_WEAPON,
     SPELL_VENOM_BOLT,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 14 - Book of the Tempests
    {0,
     SPELL_DISCHARGE,
     SPELL_LIGHTNING_BOLT,
     SPELL_FIREBALL,
     SPELL_SHATTER,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 15 - Book of Death
    {0,
     SPELL_CORPSE_ROT,
     SPELL_BONE_SHARDS,
     SPELL_LETHAL_INFUSION,
     SPELL_AGONY,
     SPELL_BOLT_OF_DRAINING,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 16 - Book of Hinderance
    {0,
     SPELL_CONFUSING_TOUCH,
     SPELL_SLOW,
     SPELL_CONFUSE,
     SPELL_PARALYZE,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 17 - Book of Changes
    {0,
     SPELL_DISRUPT,
     SPELL_STICKS_TO_SNAKES,
     SPELL_EVAPORATE,
     SPELL_SPIDER_FORM,
     SPELL_BLADE_HANDS,
     SPELL_DIG,
     SPELL_ICE_FORM,
     SPELL_NO_SPELL,
     },
    // 18 - Book of Transfigurations
    {0,
     SPELL_SANDBLAST,
     SPELL_POLYMORPH_OTHER,
     SPELL_STATUE_FORM,
     SPELL_ALTER_SELF,
     SPELL_DRAGON_FORM,
     SPELL_NECROMUTATION,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 19 - Book of Practical Magic
    {0,
     SPELL_PROJECTED_NOISE,
     SPELL_SELECTIVE_AMNESIA,
     SPELL_DETECT_CURSE,
     SPELL_DIG,
     SPELL_REMOVE_CURSE,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },

    // 20 - Book of War Chants
    {0,
     SPELL_FIRE_BRAND,
     SPELL_FREEZING_AURA,
     SPELL_REPEL_MISSILES,
     SPELL_BERSERKER_RAGE,
     SPELL_REGENERATION,
     SPELL_HASTE,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 21 - Book of Clouds
    {0,
     SPELL_EVAPORATE,
     SPELL_MEPHITIC_CLOUD,
     SPELL_CONJURE_FLAME,
     SPELL_POISONOUS_CLOUD,
     SPELL_FREEZING_CLOUD,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 22 - Book of Healing
    {0,
     SPELL_CURE_POISON_I,
     SPELL_LESSER_HEALING,
     SPELL_GREATER_HEALING,
     SPELL_PURIFICATION,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 23 - Book of Necromancy
    {0,
     SPELL_PAIN,
     SPELL_ANIMATE_SKELETON,
     SPELL_VAMPIRIC_DRAINING,
     SPELL_REGENERATION,
     SPELL_DISPEL_UNDEAD,
     SPELL_ANIMATE_DEAD,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 24 - Necronomicon -- Kikubaaqudgha special
    {0,
     SPELL_SYMBOL_OF_TORMENT,
     SPELL_CONTROL_UNDEAD,
     SPELL_SUMMON_WRAITHS,
     SPELL_DEATHS_DOOR,
     SPELL_NECROMUTATION,
     SPELL_DEATH_CHANNEL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 25 - Book of Callings
    {0,
     SPELL_SUMMON_SMALL_MAMMAL,
     SPELL_STICKS_TO_SNAKES,
     SPELL_CALL_IMP,
     SPELL_SUMMON_SCORPIONS,
     SPELL_SUMMON_ICE_BEAST,
     SPELL_SUMMON_ELEMENTAL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 26 - Book of Charms
    {0,
     SPELL_BACKLIGHT,
     SPELL_REPEL_MISSILES,
     SPELL_SLEEP,
#ifdef USE_SILENCE_CODE
     SPELL_SILENCE,
#endif
     SPELL_CONFUSE,
     SPELL_ENSLAVEMENT,
     SPELL_INVISIBILITY,
#ifndef USE_SILENCE_CODE
     SPELL_NO_SPELL,
#endif
     SPELL_NO_SPELL,
     },
    // 27 - Book of Demonology  -- Vehumet special
    {0,
     SPELL_ABJURATION_I,        //jmf: added
     SPELL_CALL_IMP,            //jmf: added
     SPELL_SUMMON_DEMON,
     SPELL_DEMONIC_HORDE,
     SPELL_SUMMON_GREATER_DEMON,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 28 - Book of Air
    {0,
     SPELL_SHOCK,
     SPELL_SWIFTNESS,
     SPELL_REPEL_MISSILES,
     SPELL_LEVITATION,
     SPELL_MEPHITIC_CLOUD,
     SPELL_DISCHARGE,
     SPELL_SUMMON_ELEMENTAL,
     SPELL_NO_SPELL,
     },

    // Removing Air Walk... it's currently broken in a number or ways:
    // - the AC/EV bonus probably means very little to any character
    //   capable of casting a level nine spell
    // - the penalty of dropping inventory is a bit harsh considering
    //   the above
    // - the fire/ice susceptibility doesn't work... the AC/EV bonus
    //   cancel it out (so you'd hardly be wary of them since they
    //   can't really do any damage).
    // - there is no need for another invulnerability spell (which is
    //   what this spell is trying to be), one is more than enough...
    //   let people use necromancy.
    // - there is no need for another air spell... air spells are
    //   already very common (ie. this level nine spell occured in
    //   two books!)

    // 29 - Book of the Sky
    {0,
#ifdef USE_SILENCE_CODE
     SPELL_SILENCE,
#endif
     SPELL_INSULATION,
     SPELL_AIRSTRIKE,
     SPELL_FLY,
     SPELL_DEFLECT_MISSILES,
     SPELL_LIGHTNING_BOLT,
     SPELL_CONJURE_BALL_LIGHTNING,   // replacing "Orb of Electrocution"
     // SPELL_ORB_OF_ELECTROCUTION,
     // SPELL_AIR_WALK,
     SPELL_NO_SPELL,
#ifndef USE_SILENCE_CODE
     SPELL_NO_SPELL,
#endif
     },

    // 30 - Book of Divinations
    {0,
     SPELL_DETECT_SECRET_DOORS,
     SPELL_DETECT_CREATURES,
     SPELL_DETECT_ITEMS,
     SPELL_DETECT_CURSE,
     SPELL_SEE_INVISIBLE,
     SPELL_FORESCRY,
     SPELL_IDENTIFY,
     SPELL_NO_SPELL,
     },
    // 31 - Book of the Warp
    {0,
     SPELL_RECALL,
     SPELL_CONTROLLED_BLINK,
     SPELL_TELEPORT_OTHER,
     SPELL_BANISHMENT,
     SPELL_DISPERSAL,
     SPELL_PORTAL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 32 - Book of Envenomations
    {0,
     SPELL_SPIDER_FORM,
     SPELL_POISON_AMMUNITION,
     SPELL_SUMMON_SCORPIONS,
     SPELL_RESIST_POISON,
     SPELL_OLGREBS_TOXIC_RADIANCE,
     SPELL_POISONOUS_CLOUD,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 33 - Book of Annihilations -- Vehumet special
    {0,
     SPELL_ISKENDERUNS_MYSTIC_BLAST,
     SPELL_POISONOUS_CLOUD,
     // SPELL_ORB_OF_ELECTROCUTION,
     SPELL_CONJURE_BALL_LIGHTNING,   // replacing "Orb of Electrocution"
     SPELL_LEHUDIBS_CRYSTAL_SPEAR,
     SPELL_ICE_STORM,
     SPELL_FIRE_STORM,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 34 - Book of Unlife
    {0,
     SPELL_SUBLIMATION_OF_BLOOD,
     SPELL_ANIMATE_DEAD,
     SPELL_TWISTED_RESURRECTION,
     SPELL_BORGNJORS_REVIVIFICATION,
     SPELL_SIMULACRUM,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },

    // 35 - Tome of Destruction
    {0,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 36 - Book of Control
    {0,
     SPELL_CONTROL_TELEPORT,    //jmf: added
     SPELL_ENSLAVEMENT,
     SPELL_TAME_BEASTS,         //jmf: added
     SPELL_MASS_CONFUSION,
     SPELL_MASS_SLEEP,          //jmf: added
     // SPELL_CONTROL_UNDEAD,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 37 - Book of Mutations //jmf: now Morphology
    {0,
     SPELL_FRAGMENTATION,
     SPELL_POLYMORPH_OTHER,
     SPELL_ALTER_SELF,
     SPELL_CIGOTUVIS_DEGENERATION,
     // SPELL_IGNITE_POISON,    // moved to Fire which was a bit slim -- bwr
     SPELL_SHATTER,
     // SPELL_AIR_WALK,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 38 - Book of Tukima
    {0,
     SPELL_SURE_BLADE,
     SPELL_TUKIMAS_VORPAL_BLADE,
     SPELL_TUKIMAS_DANCE,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 39 - Book of Geomancy
    {0,
     SPELL_SANDBLAST,
     SPELL_STONESKIN,
     SPELL_PASSWALL,
     SPELL_STONE_ARROW,
     //SPELL_DIG,   //jmf: moved to Book of Earth
     //SPELL_STATUE_FORM, //jmf: moved to Book of Earth
     SPELL_FRAGMENTATION,
     SPELL_BOLT_OF_IRON,
     SPELL_SUMMON_ELEMENTAL,
     SPELL_NO_SPELL,
     },

    // 40 - Book of Earth
    {0,
     SPELL_MAXWELLS_SILVER_HAMMER,
     SPELL_MAGIC_MAPPING,
     SPELL_DIG,
     SPELL_STATUE_FORM,
     SPELL_TOMB_OF_DOROKLOHE,
     //SPELL_ORB_OF_FRAGMENTATION, //jmf: deprecated by Fragmentation
     SPELL_LEHUDIBS_CRYSTAL_SPEAR,
     SPELL_SHATTER,
     SPELL_NO_SPELL,
     },
    // 41 - manuals of all kinds
    {0,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 42 - Book of Wizardry
    {0,
     SPELL_DETECT_CREATURES,
     SPELL_SUMMON_ELEMENTAL,
     SPELL_MAGIC_MAPPING,
     SPELL_TELEPORT_SELF,
     SPELL_FIREBALL,
     SPELL_IDENTIFY,
     SPELL_HASTE,
     SPELL_NO_SPELL,
     },
    // 43 - Book of Power
    {0,
     SPELL_ANIMATE_DEAD,
     SPELL_VENOM_BOLT,
     SPELL_BOLT_OF_IRON,
     SPELL_INVISIBILITY,
     SPELL_MASS_CONFUSION,
     SPELL_POISONOUS_CLOUD,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 44 - Book of Cantrips      //jmf: added 04jan2000
    {0,
     SPELL_CONFUSING_TOUCH,
     SPELL_ANIMATE_SKELETON,
     SPELL_SUMMON_SMALL_MAMMAL,
     SPELL_DISRUPT,
     SPELL_DETECT_SECRET_DOORS,
     SPELL_APPORTATION,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },

    // 45 - Book of Party Tricks  //jmf: added 04jan2000
    {0,
     SPELL_SUMMON_BUTTERFLIES,
     SPELL_APPORTATION,
     SPELL_PROJECTED_NOISE,
     SPELL_BLINK,
     SPELL_LEVITATION,
     SPELL_INTOXICATE,
     SPELL_NO_SPELL,  //jmf: SPELL_ERINGYAS_SURPRISING_BOUQUET, when finished
     SPELL_NO_SPELL,
     },

    // 46 - Book of Beasts //jmf: added 19mar2000
    {0,
     SPELL_SUMMON_SMALL_MAMMAL,
     SPELL_STICKS_TO_SNAKES,
     SPELL_DETECT_CREATURES,
     SPELL_SUMMON_LARGE_MAMMAL,
     SPELL_TAME_BEASTS,
     SPELL_DRAGON_FORM,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 47 - Book of Assassination //jmf: 24jun2000
    {0,
     SPELL_STING,
     // SPELL_PROJECTED_NOISE, // this doesn't get enough play
     SPELL_SURE_BLADE,
     // SPELL_SILENCE,      // not stealthy + reducing occurences --bwr
     SPELL_MEPHITIC_CLOUD,
     SPELL_POISON_WEAPON,
     SPELL_PARALYZE,
     SPELL_INVISIBILITY,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },

    // 48 -- unused
    {0,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },

    // 49 - unused
    {0,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },

    // 50 - Staff of Smiting //jmf: totally obsolete --- no longer looks here.
    {0,
     SPELL_SMITING,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 51 - Staff of Summoning
    {0,
     SPELL_RECALL,
     SPELL_SUMMON_ELEMENTAL,
     SPELL_SWARM,
     // SPELL_SHADOW_CREATURES,
     SPELL_SUMMON_DEMON,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 52 - Staff of Destruction
    {0,
     SPELL_THROW_FLAME,
     SPELL_BOLT_OF_FIRE,
     SPELL_FIREBALL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 53 - Staff of Destruction
    {0,
     SPELL_THROW_FROST,
     SPELL_ICE_BOLT,
     SPELL_FREEZING_CLOUD,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 54 - Staff of Destruction
    {0,
     SPELL_BOLT_OF_IRON,
     SPELL_FIREBALL,
     SPELL_LIGHTNING_BOLT,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 55 - Staff of Destruction
    {0,
     SPELL_BOLT_OF_INACCURACY,
     SPELL_BOLT_OF_MAGMA,
     SPELL_BOLT_OF_COLD,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 56 - Staff of Warding
    {0,
     SPELL_ABJURATION_I,
     SPELL_CONDENSATION_SHIELD,
     SPELL_CAUSE_FEAR,
     SPELL_DEFLECT_MISSILES,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 57 - Staff of Exploration
    {0,
     SPELL_DETECT_SECRET_DOORS,
     SPELL_DETECT_TRAPS,
     SPELL_DETECT_ITEMS,
     SPELL_MAGIC_MAPPING,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
    // 58 - Staff of Demonology
    {0,
     SPELL_RECALL,
     SPELL_SUMMON_DEMON,
     SPELL_DEMONIC_HORDE,
     SPELL_SUMMON_GREATER_DEMON,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     SPELL_NO_SPELL,
     },
};

void spellbook_template( int sbook_type,
                         FixedVector < int, SPELLBOOK_SIZE > &sbtemplate_pass )
{
    ASSERT( sbook_type >= 0 );
    ASSERT( sbook_type < NUMBER_SPELLBOOKS );

    // no point doing anything if tome of destruction or a manual
    if (sbook_type == BOOK_DESTRUCTION || sbook_type == BOOK_MANUAL)
        return;

    for (int i = 0; i < SPELLBOOK_SIZE; i++)    //jmf: was i = 1
    {
        sbtemplate_pass[i] = spellbook_template_array[sbook_type][i];
    }
}                               // end spellbook_template()

int which_spell_in_book(int sbook_type, int spl)
{
    FixedVector < int, SPELLBOOK_SIZE > wsib_pass;      // was 10 {dlb}

    spellbook_template(sbook_type, wsib_pass);

    return (wsib_pass[ spl + 1 ]);
}                               // end which_spell_in_book()

unsigned char spellbook_contents( item_def &book, int action )
{
    unsigned int i;
    int already = 0;
    FixedVector < int, SPELLBOOK_SIZE > spell_types;    // was 10 {dlb}
    int spelcount = 0;
    char str_pass[80];

    const int spell_levels = player_spell_levels();

    // special case for staves
    const int type = (book.base_type == OBJ_STAVES) ? book.sub_type + 40
                                                    : book.sub_type;

    bool spell_skills = false;

    for (int i = SK_SPELLCASTING; i <= SK_POISON_MAGIC; i++)
    {
        if (you.skills[i])
        {
            spell_skills = true;
            break;
        }
    }

    set_ident_flags( book, ISFLAG_KNOW_TYPE );

#ifdef DOS_TERM
    char buffer[4800];
    gettext(1, 1, 80, 25, buffer);
    window(1, 1, 80, 25);
#endif

    spellbook_template( type, spell_types );
    item_name( book, DESC_CAP_THE, str_pass );

    clrscr();
    textcolor(LIGHTGREY);
    cprintf( str_pass );
    cprintf( EOL EOL " Spells                             Type                      Level" EOL );

    for (int j = 1; j < SPELLBOOK_SIZE; j++)
    {
        if (spell_types[j] == SPELL_NO_SPELL)
            continue;

        const int level_diff = spell_difficulty( spell_types[j] );
        const int levels_req = spell_levels_required( spell_types[j] );

        cprintf(" ");
        bool knowsSpell = false;

        for (i = 0; i < 25 && !knowsSpell; i++)
        {
            knowsSpell = (you.spells[i] == spell_types[j]);
        }

        int colour = DARKGREY;
        if (action == RBOOK_USE_STAFF)
        {
            if (you.experience_level >= level_diff
                && you.magic_points >= level_diff)
            {
                colour = LIGHTGREY;
            }
        }
        else
        {
            if (knowsSpell)
                colour = LIGHTGREY;
            else if (you.experience_level >= level_diff
                        && spell_levels >= levels_req
                        && spell_skills)
            {
                colour = LIGHTBLUE;
            }
        }

        textcolor( colour );

        // Old:
        // textcolor(knowsSpell ? DARKGREY : LIGHTGREY);
        //              was: ? LIGHTGREY : LIGHTBLUE

        char strng[2];

        strng[0] = index_to_letter(spelcount);
        strng[1] = '\0';

        cprintf(strng);
        cprintf(" - ");

        cprintf(spell_title(spell_types[j]));
        gotoxy(35, wherey());

        already = 0;

        for (i = 0; i <= SPTYP_LAST_EXPONENT; i++)
        {
            if (spell_typematch( spell_types[j], 1 << i ))
            {
                if (already)
                    cprintf( "/" );
                cprintf( spelltype_name( 1 << i ) );
                already = 1;
            }
        }

        gotoxy( 65, wherey() );

        char sval[3];

        itoa( level_diff, sval, 10 );
        cprintf( sval );
        cprintf( EOL );
        spelcount++;
    }

    textcolor(LIGHTGREY);
    cprintf(EOL);

    switch (action)
    {
    case RBOOK_USE_STAFF:
        cprintf( "Select a spell to cast." EOL );
        break;

    case RBOOK_MEMORIZE:
        cprintf( "Select a spell to memorise." EOL );
        break;

    case RBOOK_READ_SPELL:
        cprintf( "Select a spell to read its description." EOL );
        break;

    default:
        break;
    }

    unsigned char keyn = getch();

    if (keyn == 0)
        getch();

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
    window(1, 18, 80, 25);
#endif

    return (keyn);     // try to figure out that for which this is used {dlb}
}

//jmf: was in shopping.cc
char book_rarity(unsigned char which_book)
{
    switch (which_book)
    {
    case BOOK_MINOR_MAGIC_I:
    case BOOK_MINOR_MAGIC_II:
    case BOOK_MINOR_MAGIC_III:
    case BOOK_SURVEYANCES:
    case BOOK_HINDERANCE:
    case BOOK_CANTRIPS: //jmf: added 04jan2000
        return 1;

    case BOOK_CHANGES:
    case BOOK_CHARMS:
        return 2;

    case BOOK_CONJURATIONS_I:
    case BOOK_CONJURATIONS_II:
    case BOOK_PRACTICAL_MAGIC:
    case BOOK_NECROMANCY:
    case BOOK_CALLINGS:
    case BOOK_WIZARDRY:
        return 3;

    case BOOK_FLAMES:
    case BOOK_FROST:
    case BOOK_AIR:
    case BOOK_GEOMANCY:
        return 4;

    case BOOK_YOUNG_POISONERS:
    case BOOK_STALKING:    //jmf: added 24jun2000
    case BOOK_WAR_CHANTS:
        return 5;

    case BOOK_CLOUDS:
    case BOOK_POWER:
        return 6;

    case BOOK_ENCHANTMENTS:
    case BOOK_PARTY_TRICKS:     //jmf: added 04jan2000
        return 7;

    case BOOK_TRANSFIGURATIONS:
    case BOOK_DIVINATIONS:
        return 8;

    case BOOK_FIRE:
    case BOOK_ICE:
    case BOOK_SKY:
    case BOOK_EARTH:
    case BOOK_UNLIFE:
    case BOOK_CONTROL:
    case BOOK_SPATIAL_TRANSLOCATIONS:
        return 10;

    case BOOK_TEMPESTS:
    case BOOK_DEATH:
        return 11;

    case BOOK_MUTATIONS:
    case BOOK_BEASTS:           //jmf: added 23mar2000
        return 12;

    case BOOK_DEMONOLOGY:
    case BOOK_ENVENOMATIONS:
    case BOOK_WARP:
        return 15;

    case BOOK_TUKIMA:
        return 16;

    case BOOK_ANNIHILATIONS:
        return 17;

    case BOOK_SUMMONINGS:
        return 18;

    case BOOK_NECRONOMICON:
    case BOOK_MANUAL:
        return 20;

    case BOOK_DESTRUCTION:
        return 30;

    case BOOK_HEALING:
        return 100;

    default:
        return 1;
    }
}                               // end book_rarity()

bool is_valid_spell_in_book( unsigned int splbook, int spell )
{
    FixedVector< int, SPELLBOOK_SIZE >  spells;

    spellbook_template( you.inv[ splbook ].sub_type, spells );

    if (spells[ spell ] != SPELL_NO_SPELL)
        return true;

    return false;
}                               // end is_valid_spell_in_book()

static bool which_spellbook( int &book, int &spell )
{
    const int avail_levels = player_spell_levels();

    // Knowing delayed fireball will allow Fireball to be learned for free -bwr
    if (avail_levels < 1 && !player_has_spell(SPELL_DELAYED_FIREBALL))
    {
        mpr("You can't memorise any more spells yet.");
        return (false);
    }
    else if (inv_count() < 1)
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return (false);
    }

    snprintf( info, INFO_SIZE, "You can memorise %d more level%s of spells.",
             avail_levels, (avail_levels > 1) ? "s" : "" );

    mpr( info );

    book = prompt_invent_item( "Memorise from which spellbook?", OBJ_BOOKS );
    if (book == PROMPT_ABORT)
    {
        canned_msg( MSG_OK );
        return (false);
    }

    if (you.inv[book].base_type != OBJ_BOOKS
        || you.inv[book].sub_type == BOOK_MANUAL)
    {
        mpr("That isn't a spellbook!");
        return (false);
    }

    if (you.inv[book].sub_type == BOOK_DESTRUCTION)
    {
        tome_of_power( book );
        return (false);
    }

    spell = read_book( you.inv[book], RBOOK_MEMORIZE );
    clrscr();

    return (true);
}                               // end which_spellbook()

unsigned char read_book( item_def &book, int action )
{
    unsigned char key2 = 0;

    // remember that this function is called from staff spells as well:
    key2 = spellbook_contents( book, action );

    if (book.base_type == OBJ_BOOKS)
    {
        you.had_book[ book.sub_type ] = 1;

        if ( book.sub_type == BOOK_MINOR_MAGIC_I
            || book.sub_type == BOOK_MINOR_MAGIC_II
            || book.sub_type == BOOK_MINOR_MAGIC_III)
        {
            you.had_book[BOOK_MINOR_MAGIC_I] = 1;
            you.had_book[BOOK_MINOR_MAGIC_II] = 1;
            you.had_book[BOOK_MINOR_MAGIC_III] = 1;
        }
        else if (book.sub_type == BOOK_CONJURATIONS_I
             || book.sub_type == BOOK_CONJURATIONS_II)
        {
            you.had_book[BOOK_CONJURATIONS_I] = 1;
            you.had_book[BOOK_CONJURATIONS_II] = 1;
        }
    }

    redraw_screen();

    /* Put special book effects in another function which can be called from
       memorise as well */

    you.turn_is_over = 1;
    set_ident_flags( book, ISFLAG_KNOW_TYPE );

    return (key2);
}                               // end read_book()

// recoded to answer whether an UNDEAD_STATE is
// barred from a particular spell passed to the
// function - note that the function can be expanded
// to prevent memorisation of certain spells by
// the living by setting up an US_ALIVE case returning
// a value of false for a set of spells ... might be
// an idea worth further consideration - 12mar2000 {dlb}
static bool undead_cannot_memorise(unsigned char spell, unsigned char being)
{
    switch (being)
    {
    case US_HUNGRY_DEAD:
        switch (spell)
        {
        //case SPELL_REGENERATION:
        case SPELL_BORGNJORS_REVIVIFICATION:
        case SPELL_CURE_POISON_II:
        case SPELL_DEATHS_DOOR:
        case SPELL_NECROMUTATION:
        case SPELL_RESIST_POISON:
        case SPELL_SYMBOL_OF_TORMENT:
        case SPELL_TAME_BEASTS:
            return true;
        }
        break;

    case US_UNDEAD:
        switch (spell)
        {
        case SPELL_AIR_WALK:
        case SPELL_ALTER_SELF:
        case SPELL_BLADE_HANDS:
        case SPELL_BORGNJORS_REVIVIFICATION:
        case SPELL_CURE_POISON_II:
        case SPELL_DEATHS_DOOR:
        case SPELL_DRAGON_FORM:
        case SPELL_GLAMOUR:
        case SPELL_ICE_FORM:
        case SPELL_INTOXICATE:
        case SPELL_NECROMUTATION:
        case SPELL_PASSWALL:
        case SPELL_REGENERATION:
        case SPELL_RESIST_POISON:
        case SPELL_SPIDER_FORM:
        case SPELL_STATUE_FORM:
        case SPELL_SUMMON_HORRIBLE_THINGS:
        case SPELL_SYMBOL_OF_TORMENT:
        case SPELL_TAME_BEASTS:
            return true;
        }
        break;
    }

    return false;
}                               // end undead_cannot_memorise()

void learn_spell(void)
{
    int chance = 0;
    int levels_needed = 0;
    unsigned char keyin;
    int book, spell;
    int index;

    int i;
    int j = 0;

    for (i = SK_SPELLCASTING; i <= SK_POISON_MAGIC; i++)
    {
        if (you.skills[i])
            j++;
    }

    if (j == 0)
    {
        mpr("You can't use spell magic! I'm afraid it's scrolls only for now.");
        return;
    }

    if (!which_spellbook( book, spell ))
        return;

    if (spell < 'A' || (spell > 'Z' && spell < 'a') || spell > 'z')
    {
      whatt:
        redraw_screen();
        mpr("What?");
        return;
    }

    index = letter_to_index( spell );

    if (index > SPELLBOOK_SIZE)
        goto whatt;

    if (!is_valid_spell_in_book( book, index ))
        goto whatt;

    unsigned int specspell = which_spell_in_book(you.inv[book].sub_type,index);

    if (specspell == SPELL_NO_SPELL)
        goto whatt;

    // You can always memorise selective amnesia:
    if (you.spell_no == 21 && specspell != SPELL_SELECTIVE_AMNESIA)
    {
        mpr("Your head is already too full of spells!");
        return;
    }

    if (you.species == SP_MUMMY && spell_typematch(specspell, SPTYP_HOLY))
    {
        mpr("You cannot use this type of magic!");
        return;
    }

    if (undead_cannot_memorise(specspell, you.is_undead))
    {
        mpr("You cannot use this spell.");
        return;
    }

    for (i = 0; i < 25; i++)
    {
        if (you.spells[i] == specspell)
        {
            redraw_screen();
            mpr("You already know that spell!");
            you.turn_is_over = 1;
            return;
        }
    }

    levels_needed = spell_levels_required( specspell );

    if (player_spell_levels() < levels_needed)
    {
        redraw_screen();
        mpr("You can't memorise that many levels of magic yet!");
        you.turn_is_over = 1;
        return;
    }

    if (you.experience_level < spell_difficulty(specspell))
    {
        redraw_screen();
        mpr("You're too inexperienced to learn that spell!");
        you.turn_is_over = 1;
        return;
    }

    redraw_screen();

    chance = spell_fail(specspell);

    strcpy(info, "This spell is ");

    strcat(info, (chance >= 80) ? "very" :
                 (chance >= 60) ? "quite" :
                 (chance >= 45) ? "rather" :
                 (chance >= 30) ? "somewhat"
                                : "not that");

    strcat(info, " ");

    int temp_rand = random2(3);

    strcat(info, (temp_rand == 0) ? "difficult" :
                 (temp_rand == 1) ? "tricky" :
                 (temp_rand == 2) ? "challenging"
                                  : "");

    strcat(info, " to ");

    temp_rand = random2(4);

    strcat(info, (temp_rand == 0) ? "memorise" :
                 (temp_rand == 1) ? "commit to memory" :
                 (temp_rand == 2) ? "learn" :
                 (temp_rand == 3) ? "absorb"
                                  : "");

    strcat(info, ".");

    mpr(info);

    strcpy(info, "Memorise ");
    strcat(info, spell_title(specspell));
    strcat(info, "?");
    mpr(info);

    for (;;)
    {
        keyin = getch();

        if (keyin == 'n' || keyin == 'N')
        {
            redraw_screen();
            return;
        }

        if (keyin == 'y' || keyin == 'Y')
            break;
    }

    mesclr();

    if (you.mutation[MUT_BLURRY_VISION] > 0
                && random2(4) < you.mutation[MUT_BLURRY_VISION])
    {
        mpr("The writing blurs into unreadable gibberish.");
        you.turn_is_over = 1;
        return;
    }

    if (random2(40) + random2(40) + random2(40) < chance)
    {
        redraw_screen();
        mpr("You fail to memorise the spell.");
        you.turn_is_over = 1;

        if (you.inv[ book ].sub_type == BOOK_NECRONOMICON)
        {
            mpr("The pages of the Necronomicon glow with a dark malevolence...");
            miscast_effect(SPTYP_NECROMANCY, 8, random2avg(88, 3), 100);
        }
        else if (you.inv[ book ].sub_type == BOOK_DEMONOLOGY)
        {
            mpr("This book does not appreciate being disturbed by one of your ineptitude!");
            miscast_effect(SPTYP_SUMMONING, 7, random2avg(88, 3), 100);
        }
        else if (you.inv[ book ].sub_type == BOOK_ANNIHILATIONS)
        {
            mpr("This book does not appreciate being disturbed by one of your ineptitude!");
            miscast_effect(SPTYP_CONJURATION, 8, random2avg(88, 3), 100);
        }

#if WIZARD
        if (you.wizard && !yesno("Memorize anyway?"))
            return;
#else
        return;
#endif
    }

    start_delay( DELAY_MEMORIZE, spell_difficulty( specspell ), specspell );

    you.turn_is_over = 1;
    redraw_screen();

    // is learning as bad as casting, then? {dlb}
    naughty(NAUGHTY_SPELLCASTING, 2 + random2(5));
}                               // end which_spell()

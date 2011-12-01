/*
 *  File:       mon-util.cc
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *   <2>    11/04/99        cdl     added a break to spell selection
 *                                  for kobold Summoners
 *                                  print just "[Ii]t" for invisible undead
 *                                  renamed params to monam()
 *   <1>     -/--/--        LRH     Created
 */

// $pellbinder: (c) D.G.S.E 1998
// some routines snatched from former monsstat.cc

#include "AppHdr.h"
#include "mon-util.h"
#include "monstuff.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "globals.h"
#include "externs.h"

#include "beam.h"
#include "debug.h"
#include "fight.h"
#include "itemname.h"
#include "itemprop.h"
#include "misc.h"
#include "mstuff2.h"
#include "player.h"
#include "randart.h"
#include "skills2.h"
#include "stuff.h"
#include "view.h"

//jmf: moved from inside function
static FixedVector< int, NUM_MONSTERS >  mon_entry;

static struct monsterentry mondata[] = {
#include "mon-data.h"
};

#define MONDATASIZE (sizeof(mondata)/sizeof(struct monsterentry))

static int mspell_list[][7] = {
#include "mon-spll.h"
};

#if DEBUG_DIAGNOSTICS
static const char *monster_spell_name[] = {
    "Magic Missile",
    "Throw Flame",
    "Throw Frost",
    "Paralysis",
    "Slow",
    "Haste",
    "Confuse",
    "Venom Bolt",
    "Fire Bolt",
    "Cold Bolt",
    "Lightning Bolt",
    "Invisibility",
    "Fireball",
    "Heal",
    "Teleport",
    "Teleport Other",
    "Blink",
    "Crystal Spear",
    "Dig",
    "Negative Bolt",
    "Hellfire Burst",
    "Vampire Summon",
    "Orb Energy",
    "Brain Feed",
    "Level Summon",
    "Fake Rakshasa Summon",
    "Steam Ball",
    "Summon Demon",
    "Animate Dead",
    "Pain",
    "Smite",
    "Sticky Flame",
    "Poison Blast",
    "Summon Demon Lesser",
    "Summon Ufetubus",
    "Purple Blast",
    "Summon Beast",
    "Energy Bolt",
    "Sting",
    "Iron Bolt",
    "Stone Arrow",
    "Poison Splash",
    "Summon Undead",
    "Mutation",
    "Cantrip",
    "Disintegrate",
    "Marsh Gas",
    "Quicksilver Bolt",
    "Torment",
    "Hellfire",
    "Metal Splinters",
    "Summon Demon Greater",
    "Banishment",
    "Controlled Blink",
    "Control Undead",
    "Miasma",
    "Summon Lizards",
    "Blink Others",
    "Dispel Undead",
    "Hellfrost",
    "Poison Arrow"
};
#endif

static int mons_exp_mod(int mclass);
static monsterentry *seekmonster( int *p_monsterid );

// macro that saves some typing, nothing more
#define smc seekmonster(&mc)

/* ******************** BEGIN PUBLIC FUNCTIONS ******************** */
void init_monsters( void )
{
    unsigned int x, y;       // must be unsigned to match size_t {dlb}

    // first, fill static array with dummy values {dlb};
    for (x = 0; x < NUM_MONSTERS; x++)
        mon_entry[x] = -1;

    // next, fill static array with location of entry in mondata[] {dlb}:
    for (x = 0; x < MONDATASIZE; x++)
        mon_entry[ mondata[x].mc ] = x;

    // finally, monsters yet with dummy entries point to TTTSNB(tm) {dlb}:
    for (x = 0; x < NUM_MONSTERS; x++)
    {
        if (mon_entry[x] == -1)
            mon_entry[x] = mon_entry[MONS_PROGRAM_BUG];
    }

    // now initiatlize the monster instance array:
    for (x = 0; x < MAX_MONSTERS; x++)
    {
        menv[x].type = -1;
        menv[x].energy = 0;
        menv[x].wait = 0;
        menv[x].flags = 0;
        menv[x].behaviour = BEH_SLEEP;

        menv[x].foe = NON_MONSTER;
        menv[x].attitude = ATT_HOSTILE;

        clear_monster_enchants( x );

        for (y = 0; y < NUM_MONSTER_SLOTS; y++)
            menv[x].inv[y] = NON_ITEM;

        menv[x].colour = BLACK;
        menv[x].number = 0;
    }
}

// NOTE: Using the actual flag enum types in these functions makes
// it more difficult to check for multiple flags at once.  However,
// those cases should be very unlikely, whereas the likelihood of
// someone using the wrong type of flag with one of these functions
// is a certainty under Murphy's Law.

// for MF_ monster instance type flags:
bool mons_flag( struct monsters *mon, mon_instance_flag_type flag )
{
    return ((mon->flags & flag) != 0);
}

void set_mons_flag( struct monsters *mon, mon_instance_flag_type flag )
{
    mon->flags = (mon->flags | flag);
}

void clear_mons_flag( struct monsters *mon, mon_instance_flag_type flag )
{
    mon->flags = (mon->flags & (~flag));
}

// for M_ monster class type flags:
bool mons_class_flag( int mc, mons_class_flags bf )
{
    return ((smc->bitfields & bf) != 0);
}

// for MR_ monster class resistance type flags:
bool mons_class_resist( int mc, mon_resist_flags bf )
{
    return ((smc->resists & bf) != 0);
}

static int scan_mon_inv_randarts( struct monsters *mon, randart_prop_type prop )
{
    int ret = 0;

    if (mons_itemuse( mon->type ) >= MONUSE_STARTING_EQUIPMENT)
    {
        const int weapon = mon->inv[MSLOT_WEAPON];
        const int second = mon->inv[MSLOT_MISSILE]; // ettins/two-head oges
        const int armour = mon->inv[MSLOT_ARMOUR];

        if (weapon != NON_ITEM && mitm[weapon].base_type == OBJ_WEAPONS
            && is_random_artefact( mitm[weapon] ))
        {
            ret += mitm[weapon].ra_props[prop];
        }

        if (second != NON_ITEM && mitm[second].base_type == OBJ_WEAPONS
            && is_random_artefact( mitm[second] ))
        {
            ret += mitm[second].ra_props[prop];
        }

        if (armour != NON_ITEM && mitm[armour].base_type == OBJ_ARMOUR
            && is_random_artefact( mitm[armour] ))
        {
            ret += mitm[armour].ra_props[prop];
        }
    }

    return (ret);
}

brand_type mons_weapon_brand( struct monsters *mon, int hand )
{
    int ret = SPWPN_NORMAL;
    const int wpn = mon->inv[hand];

    if (mon->type == MONS_PLAYER_GHOST || mon->type == MONS_PANDEMONIUM_DEMON)
        ret = env.ghost.values[ GVAL_BRAND ];
    else if (wpn != NON_ITEM)
        ret = get_weapon_brand( mitm[wpn] );

    return (static_cast<brand_type>( ret ));
}

mon_holy_type mons_class_holiness( int mc )
{
    return (smc->holiness);
}                               // end mons_class_holiness()

mon_holy_type mons_holiness( struct monsters *mon )
{
    // Kludgy, cheap way to tell undead abominations from demonic ones
    if (mons_genus( mon->type ) == MONS_ABOMINATION_SMALL
        && (mon->colour == BROWN
            || mon->colour == RED
            || mon->colour == LIGHTRED))
    {
        return (MH_UNDEAD);
    }

    return (mons_class_holiness( mon->type ));
}                               // end mons_holiness()

bool mons_is_shapeshifter( struct monsters *mon )
{
    return (mons_flag( mon, MF_SHIFTER_MASK ));
}

bool mons_is_mimic( int mc )
{
    return (mons_genus(mc) == MONS_GOLD_MIMIC);
}

bool mons_appears_harmless( int mc )
{
    return (mons_is_mimic(mc) || mons_class_holiness(mc) == MH_PLANT);
}

bool mons_is_demon( int mc )
{
    const unsigned char show_char = mons_char( mc );

    // Not every demonic monster is a demon (ie hell hog, hell hound)
    if (mons_class_holiness( mc ) == MH_DEMONIC
        && (isdigit( show_char ) || show_char == '&'))
    {
        return (true);
    }

    return (false);
}

static bool mons_is_demonlord( int mc )
{
    return (mc == MONS_ANTAEUS || mons_char( mc ) == '&');
}

// note that demon ranks count downwards
static int demon_rank( int mc )
{
    ASSERT( mons_class_holiness( mc ) == MH_DEMONIC );

    const unsigned char show_char = mons_char( mc );

    return (mons_is_demonlord( mc ) ? 0 :
            isdigit( show_char )  ? (show_char - '0') : 10);
}

// like demon_rank, the lower ranks are more powerful
static int undead_rank( int mc )
{
    const int order[] =
    {
        MONS_LICH,                      // includes curse skull/toe
        MONS_MUMMY,
        MONS_VAMPIRE,
        MONS_WRAITH,                    // includes wights and spectres
        MONS_SKELETAL_WARRIOR,          // includes skeletal dragons
        MONS_GHOUL,                     // necrophages, hulks
        MONS_PHANTOM,                   // lesser 'p' ghosts
        MONS_ABOMINATION_SMALL,
        MONS_SIMULACRUM_SMALL,
        MONS_ZOMBIE_SMALL,
        MONS_SKELETON_SMALL,
        // others: death ooze, death cob
    };

    const int genus = mons_genus( mc );
    unsigned int ret;

    for (ret = 0; ret < sizeof(order); ret++)
    {
        if (genus == order[ret])
            break;
    }

    return (ret);
}


// Used for elven Glamour ability. -- bwr
bool mons_is_humanoid( int mc )
{
    return ((seekmonster(&mc))->bitfields & M_HUMANOID);
}

bool mons_is_animated_undead( int mc, bool only_physical )
{
    return (mc == MONS_ZOMBIE_SMALL || mc == MONS_ZOMBIE_LARGE
            || mc == MONS_SKELETON_SMALL || mc == MONS_SKELETON_LARGE
            || mc == MONS_SIMULACRUM_SMALL || mc == MONS_SIMULACRUM_LARGE
            || (!only_physical && mc == MONS_SPECTRAL_THING));
}

bool mons_is_summoned( struct monsters *mon )
{
    return (mons_has_ench( mon, ENCH_SUMMONED ));
}

zombie_size_type mons_zombie_size(int mc)
{
    return (smc->zombie_size);
}                               // end mons_zombie_size()

// psize defaults to PSIZE_TORSO like player_size()
size_type mons_size( int mc, int psize )
{
    const int species = mons_species( mc );
    int ret = smc->size;

    // quick hack to keep these in line with the player def.
    switch (species)
    {
    case MONS_NAGA:
        if (psize == PSIZE_TORSO || psize == PSIZE_PROFILE)
            ret = SIZE_MEDIUM;
        break;

    case MONS_CENTAUR:
    case MONS_YAKTAUR:
        if (psize == PSIZE_TORSO)
            ret = SIZE_MEDIUM;
        break;

    case MONS_WORM:
    case MONS_GIANT_LIZARD:
        // long, but skinny and short
        if (psize == PSIZE_TORSO || psize == PSIZE_PROFILE)
            ret -= 2;
        break;

    default:
        break;
    }

    if (ret < SIZE_TINY)
        ret = SIZE_TINY;
    else if (ret > SIZE_HUGE)
        ret = SIZE_HUGE;

    return (static_cast<size_type>( ret ));
}

int mons_weight(int mc)
{
    return (smc->weight);
}                               // end mons_weight()


corpse_effect_type mons_corpse_effect(int mc)
{
    return (smc->corpse_thingy);
}                               // end mons_corpse_effect()


monster_type mons_species( int mc )
{
    return (smc->species);
}                               // end mons_species()

monster_type mons_genus( int mc )
{
    return (smc->genus);
}

monster_type draco_subspecies( struct monsters *const mon )
{
    ASSERT( mons_genus( mon->type ) == MONS_DRACONIAN );

    monster_type  ret = mons_species( mon->type );

    if (ret == MONS_DRACONIAN && mon->type != MONS_DRACONIAN)
        ret = static_cast<monster_type>( mon->number );

    return (ret);
}

shout_type mons_shouts(int mc)
{
    shout_type u = smc->shouts;

    if (u < S_SILENT)
        u = S_SILENT;

    if (u == S_RANDOM)
        u = static_cast<shout_type>( random2( NUM_SHOUTS ) );

    return (u);
}                               // end mons_shouts()

bool mons_is_unique( int mc )
{
    if ((mc >= MONS_MNOLEG && mc <= MONS_MOLLUSC_LORD)  // pandemonium lords
        || (mc >= MONS_TERENCE && mc <= MONS_BORIS)     // adventurers
        || (mc >= MONS_GERYON && mc <= MONS_ERESHKIGAL) // hell lords
        || mc == MONS_PLAYER_GHOST
        || mc == MONS_PANDEMONIUM_DEMON)
    {
        return (true);
    }

    return (false);
}

char mons_see_invis( struct monsters *mon )
{
    if (mon->type == MONS_PLAYER_GHOST || mon->type == MONS_PANDEMONIUM_DEMON)
        return (env.ghost.values[ GVAL_SEE_INVIS ]);
    else if (((seekmonster(&mon->type))->bitfields & M_SEE_INVIS) != 0)
        return (1);
    else if (scan_mon_inv_randarts( mon, RAP_EYESIGHT ) > 0)
        return (1);

    return (0);
}                               // end mons_see_invis()

unsigned char mons_char( int mc )
{
    unsigned char ret = (smc->showchar);

    if (mons_class_flag( mc, M_DCHAR_SYMBOL ))
        ret = Options.char_table[ ret ];

    return (ret);
}                               // end mons_char()

mon_itemuse_type mons_itemuse( int mc )
{
    return (smc->gmon_use);
}                               // end mons_itemuse()

int mons_element_colour( int mc )
{
    return (smc->colour);
}

// Returns the colour of the monster class (or an appropriate random
// one if the class is an elemental shifting one).
int mons_class_colour( int mc )
{
    const int col = mons_element_colour( mc );

    return (is_element_colour(col) ? element_colour(col) : col);
}                               // end mons_class_colour()

// used for check_awaken()
int mons_perception( struct monsters *mon )
{
    const int intel = mons_intel( mon->type );

    // int: 0 plant, 1 insect, 2 animal, 3 normal, 4 high
    int perc = 10 + intel * 4;

    // I assume that creatures who can see invisible are very perceptive
    if (mons_see_invis( mon ))
        perc *= 2;

    perc += 2 * mon->hit_dice;

    // critters that are wandering still have MHITYOU as their foe are
    // still actively on guard for the player,  even if they can't see
    // him.  Give them a large bonus (handle_behaviour() will nuke 'foe'
    // after a while,  removing this bonus.
    //
    // In fact, monster's that are seeking or fleeing the player should
    // also be actively searching... the exception would seem to be sleep.
    if (mon->behaviour != BEH_SLEEP)
    {
        if (mon->foe == MHITYOU)
            perc += 10 + 2 * intel * (mon->behaviour == BEH_SEEK);

        if (mon->behaviour == BEH_INVESTIGATE)
            perc += 10 + 5 * intel;
    }

    if (mons_is_guard( mon ))
        perc += 30;

    if (!mons_player_visible( mon ))
        perc -= 75;

    if (mon->behaviour == BEH_SLEEP)
    {
        const int mon_holy = mons_holiness( mon );

        if (mon_holy == MH_NATURAL)
        {
            // monster is "hibernating"/groggy... reduce chance of waking
            if (mons_has_ench( mon, ENCH_SLEEP_WARY ))
                perc -= 10;
        }
        else // unnatural creature
        {
            // Unnatural monsters don't actually "sleep", they just
            // haven't noticed an intruder yet... we'll assume that
            // they're diligently on guard.
            perc += 10;
        }
    }

    return ((perc > 0) ? perc : 0);
}

int mons_class_evasion( int mc )
{
    return (static_cast<int>( smc->ev ));
}


void get_mons_attack( struct monsters *mon, int rt,
                      struct mon_attack_def &attack )
{
    ASSERT( rt >= 0 );
    ASSERT( rt <= 3 );

    struct monsterentry *m = seekmonster( &mon->type );

    attack.type = m->attack[rt].type;
    attack.flavour = m->attack[rt].flavour;
    attack.damage = m->attack[rt].damage;

    if (attack.type != AT_NONE)
    {
        if (mon->type == MONS_PLAYER_GHOST
            || mon->type == MONS_PANDEMONIUM_DEMON)
        {
            // note that flavour is handled by mons_weapon_brand()
            attack.damage = env.ghost.values[ GVAL_DAMAGE ];
        }
        else if (mons_is_animated_undead( mon->type ))
        {
            int shape_type = static_cast<int>( mon->number );

            struct monsterentry *shape = seekmonster( &shape_type );
            int dam = shape->attack[rt].damage;

            if (mon->type == MONS_SIMULACRUM_SMALL
                || mon->type == MONS_SIMULACRUM_LARGE)
            {
                // Snowmen are many but weaker.
                dam = (dam + 1) / 2;
            }
            else
            {
                // This pretty closely matches the evil curves (yes plural)
                // that used to be done in fight.cc:
                // dam = ((dam >= 20) ? (dam - 5) :
                //        (dam >=  1) ? 1 + ((dam - 1) * 3) / 4
                //                    : 0);

                // Trying this out flat:
                dam = (dam > 0) ? (1 + ((dam - 1) * 3) / 4) : 0;
            }

            // type/modified dam of the shape, but leave flavour of the undead
            attack.type = shape->attack[rt].type;
            attack.damage = dam;
        }

        if (attack.flavour == AF_KLOWN)
        {
            switch (random2(6))
            {
            case 0:
                attack.flavour = AF_POISON_NASTY;
                break;
            case 1:
                attack.flavour = AF_ROT;
                break;
            case 2:
                attack.flavour = AF_DRAIN_XP;
                break;
            case 3:
                attack.flavour = AF_FIRE;
                break;
            case 4:
                attack.flavour = AF_COLD;
                break;
            case 5:
                attack.flavour = AF_BLINK;
                break;
            }
        }
    }
}

const char * get_attack_verb( mon_attack_type type )
{
    switch (type)
    {
    case AT_HIT:
    case AT_SPORE:
        return ("hits");

    case AT_BITE:
        return ("bites");

    case AT_STING:
        return ("stings");

    case AT_TOUCH:
        return ("touches");

    case AT_ENGULF:
        return ("engulfs");

    case AT_CLAW:
        return ("claws");

    case AT_TAIL_SLAP:
        return ("tail-slaps");

    default:
        break;
    }

    return ("BAD ATTACK TYPE");
}

int mons_damage( struct monsters *mon, int rt ) // deprecated
{
    ASSERT( rt >= 0 );
    ASSERT( rt <= 3 );

    struct mon_attack_def  attack;

    get_mons_attack( mon, rt, attack );

    return (attack.damage);
}                               // end mons_damage()

// returns true if monster is in control of it's actions
bool mons_sound_mind_and_body( struct monsters *mon, bool body_only )
{
    if (mons_has_ench( mon, ENCH_PARALYSIS )
        || mons_has_ench( mon, ENCH_PETRIFY ))
    {
        return (false);
    }

    if (!body_only
        && (mons_is_confused( mon ) || mons_has_ench( mon, ENCH_STUN )))
    {
        return (false);
    }

    return (true);
}

static bool mons_natural_long_melee( struct monsters *mon )
{
    const size_type  body = mons_size( mon->type, PSIZE_BODY );

    // Assuming that giants aren't able to leverage their size well for
    // this beacause they need to reach so far down.  They can still
    // get the benefit from weapons, however.
    return (body >= SIZE_GIANT && mons_genus( mon->type ) != MONS_HILL_GIANT);
}

// Returns true if monster is capable of melee at the range of hafted weapons
// or whips (and thus negates the melee EV bonus).
bool mons_long_melee( struct monsters *mon )
{
    const size_type  body = mons_size( mon->type, PSIZE_BODY );
    const int        wpn  = mon->inv[MSLOT_WEAPON];

    if (wpn != NON_ITEM
        && (mitm[wpn].base_type == OBJ_WEAPONS
            || mitm[wpn].base_type == OBJ_STAVES)
        && weapon_ev_bonus( mitm[wpn], 0, body, mon->evasion + 5 ) > 0)
    {
        // Using a weapon that gives an EV bonus at zero skill
        return (true);
    }

    return (mons_natural_long_melee( mon ));
}

// Returns skill level at keeping others at bay (assuming long melee capable)
// This is intentionally allowed to go higher than the player's because
// the player's to_hit values can easily go a lot higher than a monster's.
static int mons_long_skill( struct monsters *mon )
{
    int sk = mon->hit_dice + mons_damage(mon, 0);

    if (mons_class_flag( mon->type, M_FIGHTER ))
        sk += 5;

    return ((sk * mon->evasion) / 10);
}

// Returns the ev bonus the "def"ender gets vs the "att"acker in melee...
// att == NULL means the player.
int mons_weapon_ev_bonus( struct monsters *def, struct monsters *att )
{
    int ret = 0;

    // check cancellation conditions:
    if (!mons_sound_mind_and_body( def ))
        return (0);

    if (att)  // monster attack
    {
        if (mons_long_melee( att )
            || !adjacent( def->x, def->y, att->x, att->y )
            || !mons_monster_visible( def, att ))
        {
            return (0);
        }
    }
    else      // player attack
    {
        if (player_long_melee()
            || !adjacent( def->x, def->y, you.x_pos, you.y_pos )
            || !mons_player_visible( def ))
        {
            return (0);
        }
    }

    // check monster's weapon for EV bonus
    const int        wpn = def->inv[MSLOT_WEAPON];
    const int        skill = mons_long_skill( def );
    const size_type  body  = mons_size( def->type, PSIZE_BODY );

    if (wpn != NON_ITEM)
        ret = weapon_ev_bonus( mitm[wpn], skill, body, def->evasion + 5 );
    else if (mons_natural_long_melee( def ))
    {
        // Monsters with natural reach... duplicating (in essence) the
        // results of weapon_ev_bonus.
        ret = (((def->evasion / 5) + 3) * (skill + 10)) / 20;

        if (ret > 8)
            ret = 8 + (ret - 8) / 2;
    }

    return (ret);
}

int being_weapon_ev_bonus( int def_id, int att_id )
{
    ASSERT( att_id != MHITNOT );
    ASSERT( def_id != MHITNOT );
    ASSERT( def_id != att_id );

    int ret = 0;

    if (def_id == MHITYOU)
        ret = player_weapon_ev_bonus( &menv[att_id] );
    else if (att_id == MHITYOU)
        ret = mons_weapon_ev_bonus( &menv[def_id] );
    else
        ret = mons_weapon_ev_bonus( &menv[def_id], &menv[att_id] );

    return (ret);
}

int being_evasion( int mon_id )
{
    ASSERT( mon_id != MHITNOT );

    return ((mon_id == MHITYOU) ? player_evasion() : menv[mon_id].evasion);
}

// Returns id of monsters shield, or NON_ITEM if monsters isn't using one.
int mons_shield( struct monsters *mon )
{
    int ret = NON_ITEM;
    const int shield = mon->inv[MSLOT_MISSILE];

    if (shield != NON_ITEM
        && mitm[shield].base_type == OBJ_ARMOUR
        && (mitm[shield].sub_type == ARM_BUCKLER
            || mitm[shield].sub_type == ARM_SHIELD
            || mitm[shield].sub_type == ARM_LARGE_SHIELD))
    {
        ret = shield;
    }

    return (ret);
}

// returns monsters shield class
int mons_shield_class( struct monsters *mon )
{
    int ret = 0;

    const int shield = mons_shield( mon );

    if (shield != NON_ITEM)
    {
        switch (mitm[shield].sub_type)
        {
        case ARM_BUCKLER:       ret = 3; break;
        case ARM_SHIELD:        ret = 5; break;
        case ARM_LARGE_SHIELD:  ret = 7; break;
        default:                         break;
        }

        ret += (SIZE_MEDIUM - mons_size( mon->type, PSIZE_BODY ));

        if (ret < 1)
            ret = 2;

        ret = (ret * (13 + mon->hit_dice)) / 10;
        ret += mitm[shield].plus;
    }

    return ((ret < 1) ? 0 : ret);
}

int being_shield_class( int mon_id )
{
    ASSERT( mon_id != MHITNOT );

    if (mon_id == MHITYOU)
        return (player_shield_class());

    return (mons_shield_class( &menv[mon_id] ));
}

bool mons_can_block( struct monsters *mon, struct monsters *att, bool vis )
{
    bool can_see_attack = false;

    if (vis)         // attack is automatically visible (ie spells, range fire)
        can_see_attack = true;
    else if (!att)   // default of NULL means blocking player
        can_see_attack = mons_player_visible( mon );
    else
        can_see_attack = mons_monster_visible( mon, att );

    return (mons_sound_mind_and_body( mon )     // in control of actions
            && can_see_attack                   // sees attack
            && mons_shield_class( mon ));       // effective shield
}

bool being_can_block( int def_id, int att_id, bool vis )
{
    ASSERT( def_id != MHITNOT );
    ASSERT( att_id != MHITNOT );
    ASSERT( def_id != att_id );

    bool ret = false;

    if (def_id == MHITYOU)
        ret = player_can_block( &menv[att_id] );
    else if (att_id == MHITYOU)
        ret = mons_can_block( &menv[def_id], 0, vis );
    else
        ret = mons_can_block( &menv[def_id], &menv[att_id], vis );

    return (ret);
}

int mons_blocks_penalty( struct monsters *mon )
{
    if (!mon->shield_blocks)
        return (0);

    int ret = MULTIBLOCK_FACTOR * 10;

    ret = (ret * 15) / (mon->hit_dice + 5);
    ret *= (mon->shield_blocks * mon->shield_blocks);

    return (ret / 10);
}

int being_blocks_penalty( int mon_id )
{
    ASSERT( mon_id != MHITNOT );

    if (mon_id == MHITYOU)
        return (your_shield_blocks_penalty());

    return (mons_blocks_penalty( &menv[mon_id] ));
}

bool mons_light_armour( struct monsters *mon )
{
    // thick skined always get heavy armour benefits:
    const unsigned long flags = (seekmonster( &mon->type ))->bitfields;

    if (flags & M_THICK_SKIN)
        return (false);

    // check body armour:
    const int armour = mon->inv[MSLOT_ARMOUR];

    if (armour != NON_ITEM)
        return (is_light_armour( mitm[armour] ));

    return (true);
}

int apply_mons_armour( int damage, struct monsters *mon, bool energy )
{

    if (mons_has_ench( mon, ENCH_PETRIFY ))
        return (0);

    if (damage > 0 && mon->armour_class > 0)
    {
        if (mons_light_armour( mon ) || energy)
            damage -= random2( 1 + mon->armour_class );
        else
        {
            const int half = (mon->armour_class + 1) / 2;
            damage -= (half + random2( mon->armour_class - half + 1 ));
        }
    }

    return ((damage > 0) ? damage : 0);
}

int apply_being_armour( int def_id, int damage_size, int damage_taken, bool energy )
{
    ASSERT( def_id != MHITNOT );

    int damage = damage_taken;

    if (def_id == MHITYOU)
        damage = apply_player_armour( damage_size, damage_taken, energy );
    else
        damage = apply_mons_armour( damage_taken, &menv[def_id], energy );

    return ((damage > 0) ? damage : 0);
}

int mons_resist_magic( struct monsters *mon )
{
    int ret = (seekmonster(&mon->type))->resist_magic;

    if (ret == MAG_IMMUNE)
        return (MAG_IMMUNE);

    // value is scaled and multiplied by HD
    ret *= 2 * mon->hit_dice;

    // apply randarts
    ret += scan_mon_inv_randarts( mon, RAP_MAGIC );

    // apply ego armour resistance
    const int armour = mon->inv[MSLOT_ARMOUR];

    if (armour != NON_ITEM
        && get_armour_ego_type( mitm[armour] ) == SPARM_MAGIC_RESISTANCE )
    {
        ret += 30;
    }

    return (ret);
}                               // end mon_resist_magic()


// Returns true if the monster made its save against hostile
// enchantments/some other magics.
bool check_mons_resist_magic( struct monsters *monster, int pow )
{
    int mrs = mons_resist_magic( monster );

    if (mrs == 5000 || mons_has_ench( monster, ENCH_PETRIFY ))
        return (true);

    // pow = stepdown_value( pow, 30, 40, 100, 120 );

    const int target = (100 + mrs) - pow;
    const int roll = random2(101) + random2(101);

#if DEBUG_DIAGNOSTICS
    int num;

    // based on enumerating the two triangles of the roll distribution
    if (target <= 0)
        num = 0;
    else if (target <= 100)
        num = (target * (target + 1)) / 2;
    else if (target >= 201)
        num = 10201;
    else
    {
        const int side = 201 - target;
        num = 10201 - (side * (side + 1)) / 2;
    }

    mpr( MSGCH_DIAGNOSTICS,
          "power: %d, monster's MR: %d, target: %d (%0.2f%%), roll: %d",
          pow, mrs, target, static_cast<float>( num ) / 102.01, roll );
#endif

    return ((roll < target) ? true : false);
}                               // end check_mons_resist_magic()

int being_res_elec( int mid )
{
    ASSERT( mid != MHITNOT );

    int ret = 0;

    if (mid == MHITYOU)
        ret = player_res_electricity();
    else if (mid != MHITNOT)
        ret = mons_res_elec( &menv[mid] );

    return (ret);
}

int mons_res_elec( struct monsters *mon )
{
    int ret = 0;
    int mc = mon->type;

    if (mc == MONS_PLAYER_GHOST || mc == MONS_PANDEMONIUM_DEMON)
        return (env.ghost.values[ GVAL_RES_ELEC ]);

    const unsigned long res = (seekmonster(&mc))->resists;

    // of course it makes no sense setting them both :)
    if (mons_has_ench( mon, ENCH_PETRIFY ))
        ret += 3;
    else if (res & MR_RES_ELEC)
        ret++;
    else if (res & MR_VUL_ELEC)
        ret--;

    // don't bother checking equipment if the monster can't use it
    if (mons_itemuse(mc) >= MONUSE_STARTING_EQUIPMENT)
    {
        ret += scan_mon_inv_randarts( mon, RAP_ELECTRICITY );

        // no ego armour, but storm dragon.
        const int armour = mon->inv[MSLOT_ARMOUR];
        if (armour != NON_ITEM && mitm[armour].base_type == OBJ_ARMOUR
            && mitm[armour].sub_type == ARM_STORM_DRAGON_ARMOUR)
        {
            ret += 1;
        }
    }

    return (ret);
}                               // end mons_res_elec()

int mons_res_acid( struct monsters *mon )
{
    const monster_type   gen  = mons_genus( mon->type );
    const mon_holy_type  holy = mons_holiness( mon );

    return (mon->type == MONS_OKLOB_PLANT                   // has acid attack
            || mon->type == MONS_SPINY_WORM                 // has acid attack
            || gen == MONS_JELLY                            // jellies/oozes
            || (holy == MH_NONLIVING                        // nonlive, except:
                && ((gen != MONS_CLAY_GOLEM                 // - golems, but
                        || mon->type == MONS_CRYSTAL_GOLEM  //     !crystal
                        || mon->type == MONS_ELECTRIC_GOLEM)//     !elec
                    && gen != MONS_GARGOYLE                 // - gargoyles
                    && gen != MONS_GOLD_MIMIC               // - mimics
                    && mon->type != MONS_DANCING_WEAPON))); // - weapons
}

int being_res_acid( int mid )
{
    ASSERT( mid != MHITNOT );

    int ret = 0;

    if (mid == MHITYOU)
        ret = player_res_acid();
    else if (mid != MHITNOT)
        ret = mons_res_acid( &menv[mid] );

    return (ret);
}

int mons_res_poison( struct monsters *mon )
{
    int ret = 0;
    int mc = mon->type;
    const unsigned long res = (seekmonster(&mc))->resists;

    if (mons_has_ench( mon, ENCH_PETRIFY ))
        ret += 3;
    else if (res & MR_RES_POISON)
        ret++;
    else if (res & MR_VUL_POISON)
        ret--;

    if (mons_itemuse(mc) >= MONUSE_STARTING_EQUIPMENT)
    {
        ret += scan_mon_inv_randarts( mon, RAP_POISON );

        const int armour = mon->inv[MSLOT_ARMOUR];
        if (armour != NON_ITEM && mitm[armour].base_type == OBJ_ARMOUR)
        {
            // intrinsic armour abilities
            switch (mitm[armour].sub_type)
            {
            case ARM_SWAMP_DRAGON_ARMOUR: ret += 1; break;
            case ARM_GOLD_DRAGON_ARMOUR:  ret += 1; break;
            default:                                break;
            }

            // ego armour resistance
            if (get_armour_ego_type( mitm[armour] ) == SPARM_POISON_RESISTANCE)
                ret += 1;
        }

        const int shield = mons_shield( mon );
        if (shield != NON_ITEM)
        {
            if (get_armour_ego_type( mitm[shield] ) == SPARM_POISON_RESISTANCE)
                ret += 1;
        }
    }

    return (ret);
}                               // end mons_res_poison()

int being_res_poison( int mid )
{
    ASSERT( mid != MHITNOT );

    int ret = 0;

    if (mid == MHITYOU)
        ret = player_res_poison();
    else if (mid != MHITNOT)
        ret = mons_res_poison( &menv[mid] );

    return (ret);
}

int mons_res_fire( struct monsters *mon )
{
    int ret = 0;
    int ego;
    int mc = mon->type;

    if (mc == MONS_PLAYER_GHOST || mc == MONS_PANDEMONIUM_DEMON)
        return (env.ghost.values[ GVAL_RES_FIRE ]);

    const unsigned long res = (seekmonster(&mc))->resists;

    // no Big Prize (tm) here either if you set all three flags. It's a pity uh?
    //
    // Note that natural monster resistance is two levels, this is duplicate
    // the fact that having this flag used to be a lot better than armour
    // for monsters (it used to make them immune in a lot of cases) -- bwr
    if (mons_has_ench( mon, ENCH_PETRIFY ))
        ret += 3;
    else if (res & MR_RES_HELLFIRE)
        ret += 3;
    else if (res & MR_RES_FIRE)
        ret += 2;
    else if (res & MR_VUL_FIRE)
        ret--;

    if (mons_itemuse(mc) >= MONUSE_STARTING_EQUIPMENT)
    {
        ret += scan_mon_inv_randarts( mon, RAP_FIRE );

        const int armour = mon->inv[MSLOT_ARMOUR];
        if (armour != NON_ITEM && mitm[armour].base_type == OBJ_ARMOUR)
        {
            // intrinsic armour abilities
            switch (mitm[armour].sub_type)
            {
            case ARM_DRAGON_ARMOUR:      ret += 2; break;
            case ARM_GOLD_DRAGON_ARMOUR: ret += 1; break;
            case ARM_ICE_DRAGON_ARMOUR:  ret -= 1; break;
            default:                               break;
            }

            // check ego resistance
            ego = get_armour_ego_type( mitm[armour] );
            if (ego == SPARM_FIRE_RESISTANCE || ego == SPARM_RESISTANCE)
                ret += 1;
        }

        const int shield = mons_shield( mon );
        if (shield != NON_ITEM)
        {
            ego = get_armour_ego_type( mitm[shield] );
            if (ego == SPARM_FIRE_RESISTANCE || ego == SPARM_RESISTANCE)
                ret += 1;
        }
    }

    return (ret);
}                               // end mons_res_fire()

int being_res_fire( int mid )
{
    ASSERT( mid != MHITNOT );

    int ret = 0;

    if (mid == MHITYOU)
        ret = player_res_fire();
    else if (mid != MHITNOT)
        ret = mons_res_fire( &menv[mid] );

    return (ret);
}

int mons_res_cold( struct monsters *mon )
{
    int ret = 0;
    int ego;
    int mc = mon->type;

    if (mc == MONS_PLAYER_GHOST || mc == MONS_PANDEMONIUM_DEMON)
        return (env.ghost.values[ GVAL_RES_COLD ]);

    const unsigned long res = (seekmonster(&mc))->resists;

    // Note that natural monster resistance is two levels, this is duplicate
    // the fact that having this flag used to be a lot better than armour
    // for monsters (it used to make them immune in a lot of cases) -- bwr
    if (mons_has_ench( mon, ENCH_PETRIFY ))
        ret += 3;
    else if (res & MR_RES_HELLFROST)
        ret += 3;
    else if (res & MR_RES_COLD)
        ret += 2;
    else if (res & MR_VUL_COLD)
        ret--;

    if (mons_itemuse(mc) >= MONUSE_STARTING_EQUIPMENT)
    {
        ret += scan_mon_inv_randarts( mon, RAP_COLD );

        const int armour = mon->inv[MSLOT_ARMOUR];
        if (armour != NON_ITEM && mitm[armour].base_type == OBJ_ARMOUR)
        {
            // intrinsic armour abilities
            switch (mitm[armour].sub_type)
            {
            case ARM_ICE_DRAGON_ARMOUR:  ret += 2; break;
            case ARM_GOLD_DRAGON_ARMOUR: ret += 1; break;
            case ARM_DRAGON_ARMOUR:      ret -= 1; break;
            default:                               break;
            }

            // check ego resistance
            ego = get_armour_ego_type( mitm[armour] );
            if (ego == SPARM_COLD_RESISTANCE || ego == SPARM_RESISTANCE)
                ret += 1;
        }

        const int shield = mons_shield( mon );
        if (shield != NON_ITEM)
        {
            ego = get_armour_ego_type( mitm[shield] );
            if (ego == SPARM_COLD_RESISTANCE || ego == SPARM_RESISTANCE)
                ret += 1;
        }
    }

    return (ret);
}                               // end mons_res_cold()

int being_res_cold( int mid )
{
    ASSERT( mid != MHITNOT );

    int ret = 0;

    if (mid == MHITYOU)
        ret = player_res_cold();
    else if (mid != MHITNOT)
        ret = mons_res_cold( &menv[mid] );

    return (ret);
}

int mons_res_negative_energy( struct monsters *mon )
{
    int ret = 0;
    int mc = mon->type;

    // living == plants and natural creatures, nonliving == everything else
    if (mon->type == MONS_SHADOW_DRAGON
        || mon->type == MONS_DEATH_DRAKE
        || !mons_has_lifeforce( mon ))
    {
        return (3);  // to match the value for players
    }

    if (mons_itemuse(mc) >= MONUSE_STARTING_EQUIPMENT)
    {
        ret += scan_mon_inv_randarts( mon, RAP_NEGATIVE_ENERGY );

        const int armour = mon->inv[MSLOT_ARMOUR];
        if (armour != NON_ITEM && mitm[armour].base_type == OBJ_ARMOUR)
        {
            // check for ego resistance
            if (get_armour_ego_type( mitm[armour] ) == SPARM_POSITIVE_ENERGY)
                ret += 1;
        }

        const int shield = mons_shield( mon );
        if (shield != NON_ITEM)
        {
            if (get_armour_ego_type( mitm[shield] ) == SPARM_POSITIVE_ENERGY)
                ret += 1;
        }
    }

    return (ret);
}                               // end mons_res_negative_energy()

int being_res_negative_energy( int mid )
{
    ASSERT( mid != MHITNOT );

    int ret = 0;

    if (mid == MHITYOU)
        ret = player_prot_life();
    else if (mid != MHITNOT)
        ret = mons_res_negative_energy( &menv[mid] );

    return (ret);
}

// Returns:
//     +'ve if the being resists any of the forms of damage in dam_type
//     -'ve if the being is vulnerable to the forms of damage in dam_type
//     zero otherwise
int being_res_damage_type( int mid, int dam_type )
{
    ASSERT( mid != MHITNOT );

    int res = 0, vul = 0;

    if (mid != MHITYOU)
    {
        int flags = (seekmonster( &(menv[mid].type) ))->resists;

        if (testbits( dam_type, DAM_PIERCE ))
        {
            res += testbits( flags, MR_RES_PIERCE );
            vul += testbits( flags, MR_VUL_PIERCE );
        }

        if (testbits( dam_type, DAM_BLUDGEON ))
        {
            res += testbits( flags, MR_RES_BLUDGEON );
            vul += testbits( flags, MR_VUL_BLUDGEON );
        }

        if (testbits( dam_type, DAM_SLICE ))
        {
            res += testbits( flags, MR_RES_SLICE );
            vul += testbits( flags, MR_VUL_SLICE );
        }
    }

    return ((res > 0) ? res : -vul);
}

int being_choose_damage_type( int mid, int dam_types )
{
    int ret = DAM_BASH;
    int count = 0;

    if ((dam_types & DAM_BLUDGEON)
        && being_res_damage_type( mid, DAM_BLUDGEON ) <= 0)
    {
        count += 3;
        ret = DAM_BLUDGEON;
    }

    if ((dam_types & DAM_PIERCE)
        && being_res_damage_type( mid, DAM_PIERCE ) <= 0)
    {
        count++;
        if (one_chance_in(count))
            ret = DAM_PIERCE;
    }

    if ((dam_types & DAM_SLICE) || (dam_types & DAM_WHIP)
        && being_res_damage_type( mid, DAM_SLICE ) <= 0)
    {
        count++;
        if (one_chance_in(count))
            ret = DAM_SLICE;
    }

    return (ret);
}

int mons_damage_type( struct monsters *mon, const struct mon_attack_def &attack,
                      int hand_used )
{
    int ret = DAM_BLUDGEON;

    switch (attack.type)
    {
    case AT_HIT:
        // check for using a weapon
        if (attack.flavour == AF_PLAIN && mon->inv[hand_used] != NON_ITEM)
            ret = get_damage_type( mitm[ mon->inv[hand_used] ] );
        break;

    case AT_BITE:
    case AT_STING:
        ret = DAM_PIERCE;
        break;

    case AT_CLAW:
        ret = DAM_SLICE;
        break;

    default:
        break;
    }

    return (ret);
}

void mons_submerge( struct monsters *mon )
{
    set_mons_flag( mon, MF_SUBMERGED );
}

void mons_unsubmerge( struct monsters *mon )
{
    clear_mons_flag( mon, MF_SUBMERGED );
}

bool mons_is_submerged( struct monsters *mon )
{
    return (mons_flag( mon, MF_SUBMERGED ));
}

// terrain == UNSEEN (default) skips checking the terrain
bool mons_can_submerge( struct monsters *mon, int terrain )
{
    const int habitat = monster_habitat( mon->type );

    return (mon->type == MONS_AIR_ELEMENTAL
            || (habitat != DNGN_FLOOR
                && (terrain == DNGN_UNSEEN || terrain == habitat)));
}


bool mons_is_invisible( struct monsters *mon )
{
    return (mons_class_flag( mon->type, M_INVIS )
            || mons_has_ench( mon, ENCH_INVIS ));
}

bool mons_is_confused( struct monsters *mon )
{
    bool ret = mons_class_flag( mon->type, M_CONFUSED );

    if (mons_has_ench( mon, ENCH_CONFUSION ))
        ret = true;
    else if (mon->type == MONS_AIR_ELEMENTAL && mons_is_submerged( mon ))
        ret = true;
    else if (mons_is_animated_undead( mon->type ))
        ret = mons_class_flag( static_cast<int>( mon->number ), M_CONFUSED );

    return (ret);
}

bool mons_is_batty( struct monsters *mon )
{
    if (mons_is_animated_undead( mon->type ))
        return (mons_class_flag( static_cast<int>( mon->number ), M_BATTY ));

    return (mons_class_flag( mon->type, M_BATTY ));
}

bool mons_is_scared( struct monsters *mon )
{
    return (mon->behaviour == BEH_FLEE || mon->behaviour == BEH_PANIC);
}

// This does NOT do line of sight!  It checks the targ's visibility
// with respect to mon's perception, but doesn't do walls or range.
bool mons_monster_visible( struct monsters *mon, struct monsters *targ )
{
    if (mons_is_submerged( targ )
        || (mons_is_invisible( targ ) && !mons_see_invis( mon )))
    {
        return (false);
    }

    return (true);
}

// This does NOT do line of sight!  It checks the player's visibility
// with respect to mon's perception, but doesn't do walls or range.
bool mons_player_visible( struct monsters *mon )
{
    if (you.invis)
    {
        if (player_in_water() || mons_see_invis( mon ))
            return (true);

        return (false);
    }

    return (true);
}

bool being_visible( int mon_id, int targ_id )
{
    ASSERT( mon_id != MHITNOT );
    ASSERT( targ_id != MHITNOT );
    ASSERT( mon_id != targ_id );

    bool ret = false;

    if (mon_id == MHITYOU)
        ret = player_monster_visible( &menv[targ_id] );
    else if (targ_id == MHITYOU)
        ret = mons_player_visible( &menv[mon_id] );
    else
        ret = mons_monster_visible( &menv[mon_id], &menv[targ_id] );

    return (ret);
}


bool mons_line_of_sight( struct monsters *mon, int x, int y )
{
    return (check_line_of_sight( mon->x, mon->y, x, y ));
}

bool mons_is_evil( struct monsters *mon )
{
    return (mons_class_flag( mon->type, M_EVIL ));
}

bool mons_is_unholy( struct monsters *mon )
{
    const mon_holy_type holy = mons_holiness( mon );

    return (holy == MH_UNDEAD || holy == MH_DEMONIC);
}

bool mons_has_lifeforce( struct monsters *mon )
{
    const int holy = mons_holiness( mon );

    return ((holy == MH_NATURAL || holy == MH_PLANT)
            && !mons_has_ench( mon, ENCH_PETRIFY ));
}

bool mons_skeleton(int mc)
{
    if (mons_zombie_size(mc) == 0
        || mons_weight(mc) == 0
        || ((smc->bitfields & M_NO_SKELETON) != 0))
    {
        return (false);
    }

    return (true);
}                               // end mons_skeleton()

int mons_class_flies(int mc)
{
    if (mc == MONS_PANDEMONIUM_DEMON)
        return (static_cast<int>( env.ghost.values[GVAL_DEMONLORD_FLY] ));

    int f = smc->bitfields;

    // Note: levitation is better for monsters than powered flight so we
    // check it first.
    if (f & M_LEVITATE)
        return (FLY_LEVITATION);
    else if (f & M_FLIES)
        return (FLY_POWERED);

    return (FLY_NOT);
}

int mons_flies( struct monsters *mon )
{
    int ret = FLY_NOT;

    if (mon->type == MONS_SPECTRAL_THING)
        ret = FLY_LEVITATION;
    else if (mons_is_animated_undead( mon->type ))
        ret = mons_class_flies( mon->number );
    else
        ret = mons_class_flies( mon->type );

    if (ret != FLY_LEVITATION && scan_mon_inv_randarts(mon, RAP_LEVITATE) > 0)
        ret = FLY_LEVITATION;

    return (ret);
}                               // end mons_flies()


int hit_points( int hit_dice, int min_hp, int rand_hp )
{
    int hrolled = min_hp * hit_dice;

    if (rand_hp > 0)
    {
        for (int hroll = 0; hroll < hit_dice; hroll++)
            hrolled += random2( 1 + rand_hp );
    }

    return (hrolled);
}                               // end hit_points()

// This function returns the standard number of hit dice for a type
// of monster, not a pacticular monsters current hit dice. -- bwr
int mons_class_hit_dice( int type )
{
    struct monsterentry *mon_class = seekmonster( &type );

    return ((mon_class) ? mon_class->hpdice[0] : 0);
}


// XXX: This function still needs to be seriously looked at... one
// thing that might be nice is to reduce the impact of mHD on experience
// so that draining is less painful (or make the benefit of draining
// hit dice higher).  Perhaps the class hit die could be partially
// applied (so that scary drained monsters are worth more than wimpy
// ones).
int exper_value( struct monsters *monster )
{
    long x_val = 0;

    // these three are the original arguments:
    const int  mclass      = monster->type;
    const int  mHD         = monster->hit_dice;
    const int  maxhp       = monster->max_hit_points;

    // these are some values we care about:
    const int  speed       = mons_class_speed(mclass);
    const int  modifier    = mons_exp_mod(mclass);
    const int  item_usage  = mons_itemuse(mclass);

    const bool spellcaster = (mons_class_flag( mclass, M_SPELLCASTER )
                                && !mons_is_shapeshifter( monster ));

    // early out for no XP monsters
    if (mons_class_flag( mclass, M_NO_EXP_GAIN ))
        return (0);

    // These undead take damage to maxhp, so we use only HD. -- bwr
    if (mons_is_animated_undead( mclass, true ))
        x_val = ((16 + mHD * 4) * mHD * mHD) / 15;
    else
        x_val = ((16 + maxhp) * mHD * mHD) / 15;


    // Let's calculate a simple difficulty modifier -- bwr
    int diff = 0;

    // Let's look for big spells:
    if (spellcaster)
    {
        for (int i = 0; i < NUM_MONSTER_SPELL_SLOTS; i++)
        {
            switch (monster->spells[i])
            {
            case MS_PARALYSIS:
            case MS_SMITE:
            case MS_HELLFIRE_BURST:
            case MS_HELLFIRE:
            case MS_HELLFROST:
            case MS_TORMENT:
            case MS_IRON_BOLT:
                diff += 25;
                break;

            case MS_LIGHTNING_BOLT:
            case MS_NEGATIVE_BOLT:
            case MS_STICKY_FLAME:
            case MS_DISINTEGRATE:
            case MS_SUMMON_DEMON_GREATER:
            case MS_BANISHMENT:
            case MS_CRYSTAL_SPEAR:
            case MS_TELEPORT:
            case MS_TELEPORT_OTHER:
                diff += 10;
                break;

            default:
                break;
            }
        }
    }

    // let's look at regeneration
    if (mons_class_flag( mclass, M_REGEN ))
        diff += 15;

    // Monsters at normal or fast speed with big melee damage
    if (speed >= 10)
    {
        int max_melee = 0;

        for (int i = 0; i < 4; i++)
            max_melee += mons_damage( monster, i );

        if (max_melee > 30)
            diff += (max_melee / ((speed == 10) ? 2 : 1));
    }

    // Monsters who can use equipment (even if only the equipment
    // they are given) can be considerably enhanced because of
    // the way weapons work for monsters. -- bwr
    if (item_usage >= MONUSE_STARTING_EQUIPMENT)
        diff += 10 * (item_usage - MONUSE_STARTING_EQUIPMENT + 1);

    // Set a reasonable range on the difficulty modifier...
    // Currently 70% - 200% -- bwr
    if (diff > 100)
        diff = 100;
    else if (diff < -30)
        diff = -30;

    // Apply difficulty
    x_val *= (100 + diff);
    x_val /= 100;

    // Basic speed modification
    if (speed > 0)
    {
        x_val *= speed;
        x_val /= 10;
    }

    // Slow monsters without spells and items often have big HD which
    // cause the experience value to be overly large... this tries
    // to reduce the inappropriate amount of XP that results. -- bwr
    if ((speed < 10
            || mons_stationary( monster->type )
            || monster->type == MONS_WANDERING_MUSHROOM)
        && !spellcaster
        && item_usage < MONUSE_STARTING_EQUIPMENT)
    {
        x_val /= 2;
    }

    // Apply the modifier in the monster's definition
    if (modifier > 0)
    {
        x_val *= modifier;
        x_val /= 10;
    }

    // Reductions for big values. -- bwr
    if (x_val > 100)
        x_val = 100 + ((x_val - 100) * 3) / 4;

    if (x_val > 1000)
        x_val = 1000 + (x_val - 1000) / 2;

    // guarantee the value is within limits
    if (x_val <= 0)
        x_val = 1;
    else if (x_val > 15000)
        x_val = 15000;

    return (x_val);
}                               // end exper_value()

int mons_spell_template_index( struct monsters *mon )
{
    ASSERT( mon->type != MONS_HYDRA );                     // heads
    ASSERT( mon->type != MONS_MANTICORE );                 // spikes
    ASSERT( !mons_is_animated_undead( mon->type ) );       // monster type
    ASSERT( mons_class_flag( mon->type, M_SPELLCASTER ) ); // expects spells

    return( mon->number );
}

void mons_load_spells( struct monsters *const mon, int book )
{
    int x, y;

    if (book == MST_NO_SPELLS)
    {
        for (y = 0; y < NUM_MONSTER_SPELL_SLOTS; y++)
            mon->spells[y] = MS_NO_SPELL;
        return;
    }

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "%s: loading spellbook #%d",
            ptr_monam( mon, DESC_PLAIN ), book );
#endif

    for (x = 0; x < 6; x++)
        mon->spells[x] = MS_NO_SPELL;

    if (book == MST_GHOST)
    {
        for (y = 0; y < NUM_MONSTER_SPELL_SLOTS; y++)
        {
            mon->spells[y] = env.ghost.values[ GVAL_SPELL_1 + y ];
#if DEBUG_DIAGNOSTICS
            mpr( MSGCH_DIAGNOSTICS, "spell #%d: %d", y, mon->spells[y] );
#endif
        }
    }
    else
    {
        // this needs to be rewritten a la the monsterseek rewrite {dlb}:
        for (x = 0; x < NUM_MSTYPES; x++)
        {
            if (mspell_list[x][0] == book)
                break;
        }

        if (x < NUM_MSTYPES)
        {
            for (y = 0; y < 6; y++)
                mon->spells[y] = mspell_list[x][y + 1];
        }
    }
}

#if DEBUG_DIAGNOSTICS
const char *mons_spell_name( int spell )
{
    if (spell == MS_NO_SPELL || spell >= NUM_MONSTER_SPELLS || spell < 0)
        return ("No spell");

    return (monster_spell_name[ spell ]);
}
#endif

void clear_monster_enchants( int index )
{
    for (int i = 0; i < NUM_MON_ENCHANTS; i++)
    {
        menv[index].ench[i].type     = ENCH_NONE;
        menv[index].ench[i].source   = MHITNOT;
        menv[index].ench[i].duration = 0;
        menv[index].ench[i].work     = 0;
    }
}

// generate a shiny new and unscarred monster
void define_monster( int index, bool polymorphing )
{
    const struct monsterentry *const m = seekmonster( &(menv[index].type) );

    // these are sometimes calculated, set to base first:
    int hd  = m->hpdice[0];
    int ac  = m->AC;
    int ev  = m->ev;
    int spd = m->speed;
    int col = mons_class_colour( menv[index].type );
    int num = menv[index].number;

    int spells = MST_NO_SPELLS;

    if (mons_class_flag( menv[index].type, M_SPELLCASTER ))
        spells = m->sec;

    int temp_rand;

    // ... and now handle those special cases:
    switch (menv[index].type)
    {
    case MONS_ABOMINATION_SMALL:
        hd  = 4 + random2(4);
        ac  = 3 + random2(7);
        ev  = 7 + random2(6);
        spd = 5 + roll_dice(2,5);
        break;

    case MONS_ABOMINATION_LARGE:
        hd  = 8 + random2(4);
        ac  = 3 + roll_dice(2,5);
        ev  = 3 + random2(5);
        spd = 4 + roll_dice(2,4);
        break;

    case MONS_ZOMBIE_SMALL:
        hd  = (coinflip() ? 1 : 2);
        break;

    case MONS_ZOMBIE_LARGE:
        hd  = 3 + random2(5);
        break;

    case MONS_BEAST:
        hd  = 5 + random2(4);
        ac  = 8 + random2(5);
        ev  = 8 + random2(5);
        spd = 9 + random2(5);
        break;

    case MONS_HYDRA:
        num = 4 + random2(5);
        break;

    case MONS_DEEP_ELF_FIGHTER:
    case MONS_DEEP_ELF_KNIGHT:
    case MONS_DEEP_ELF_SOLDIER:
    case MONS_ORC_WIZARD:
        spells = MST_ORC_WIZARD_I + random2(3);
        break;

    case MONS_LICH:
    case MONS_ANCIENT_LICH:
    case MONS_BORIS:     // yep, Boris changes spellbooks between deaths
        spells = MST_LICH_I + random2(4);
        break;

    case MONS_HELL_KNIGHT:
        spells = (coinflip() ? MST_HELL_KNIGHT_I : MST_HELL_KNIGHT_II);
        break;

    case MONS_NECROMANCER:
        spells = (coinflip() ? MST_NECROMANCER_I : MST_NECROMANCER_II);
        break;

    case MONS_WIZARD:
    case MONS_OGRE_MAGE:
    case MONS_EROLCHA:
    case MONS_DEEP_ELF_MAGE:
        spells = MST_WIZARD_I + random2(5);
        break;

    case MONS_DEEP_ELF_CONJURER:
        spells = (coinflip() ? MST_DEEP_ELF_CONJURER_I
                             : MST_DEEP_ELF_CONJURER_II);
        break;

    case MONS_DRACONIAN_CALLER:
    case MONS_DRACONIAN_MONK:
    case MONS_DRACONIAN_ZEALOT:
    case MONS_DRACONIAN_SHIFTER:
    case MONS_DRACONIAN_ANNIHILATOR:
    case MONS_DRACONIAN_SCORCHER:
        num = MONS_BLACK_DRACONIAN + random2(8);
        break;

    case MONS_DRACONIAN_KNIGHT:
        temp_rand = random2(10);

        // hell knight, death knight, chaos knight...
        if (temp_rand < 6)
            spells = (coinflip() ? MST_HELL_KNIGHT_I : MST_HELL_KNIGHT_II);
        else if (temp_rand < 9)
            spells = (coinflip() ? MST_NECROMANCER_I : MST_NECROMANCER_II);
        else
        {
            spells = (coinflip() ? MST_DEEP_ELF_CONJURER_I
                                 : MST_DEEP_ELF_CONJURER_II);
        }

        num = MONS_BLACK_DRACONIAN + random2(8);
        break;

    case MONS_DRACONIAN:
        // these are supposed to only be created by polymorph
        hd += random2(10);
        ac += random2(5);
        ev += random2(5);

        if (hd >= 7)
        {
            menv[index].type = MONS_BLACK_DRACONIAN + random2(8);
            col = mons_class_colour( menv[index].type );
        }
        break;

    case MONS_HUMAN:
    case MONS_ELF:
        // these are supposed to only be created by polymorph
        hd += random2(10);
        ac += random2(5);
        ev += random2(5);
        break;

    case MONS_DANCING_WEAPON:
        if (menv[index].inv[MSLOT_WEAPON] != NON_ITEM)
            col = mitm[ menv[index].inv[MSLOT_WEAPON] ].colour;
        break;

    default:
        if (mons_is_mimic( menv[index].type ))
            col = get_mimic_colour( &menv[index] );
        break;
    }

    // a few final calculations now that class has been figured in:
    const int hp = hit_points( hd, m->hpdice[1], m->hpdice[2] ) + m->hpdice[3];

    if (num == 250)
        num = m->sec;

    // check some things before setting
    ASSERT( col != BLACK );  // make sure all special colour cases are handled

    // so let it be written, so let it be done
    menv[index].hit_dice = hd;
    menv[index].hit_points = hp;
    menv[index].max_hit_points = hp;
    menv[index].armour_class = ac;
    menv[index].evasion = ev;
    menv[index].speed = spd;
    menv[index].number = num;
    menv[index].colour = col;
    menv[index].energy = 70 + random2(10);
    menv[index].shield_blocks = 0;

    if (spells != MST_NO_SPELLS)
        mons_load_spells( &menv[index], spells );

    // keep certain (ie shifter) flags and enchantments across polymorphs
    if (polymorphing)
        menv[index].flags &= MF_POLY_KEEP_MASK;
    else
    {
        menv[index].flags = 0;
        clear_monster_enchants( index );
    }
}                               // end define_monster()

/* ------------------------- monam/moname ------------------------- */
const char *ptr_monam( struct monsters *mon, char desc, bool title )
{
    // We give an item type description for mimics now, note that
    // since gold mimics only have one description (to match the
    // examine code in direct.cc), we won't bother going through
    // this for them. -- bwr
    if (mons_is_mimic( mon->type ) && mon->type != MONS_GOLD_MIMIC)
    {
        static char mimic_name_buff[ ITEMNAME_SIZE ];

        item_def  item;
        get_mimic_item( mon, item );
        item_name( item, desc, mimic_name_buff );

        return (mimic_name_buff);
    }

    return (monam( mon->number, mon->type, player_monster_visible( mon ),
                   desc, mon->inv[MSLOT_WEAPON], title ));
}

const char *ptr_being_name( int mon_id, char desc, bool title )
{
    ASSERT( mon_id != MHITNOT );

    const char *ret = "*BUG*";

    if (mon_id != MHITYOU)
        ret = ptr_monam( &menv[mon_id], desc, title );
    else
    {
        switch (desc)
        {
        case DESC_CAP_THE:
        case DESC_CAP_A:
            ret = "You";
            break;
        case DESC_NOCAP_THE:
        case DESC_NOCAP_A:
        case DESC_PLAIN:
        default:
            ret = "you";
            break;
            //default: DEBUGSTR("bad desc flag");
        }
    }

    return (ret);
}

const char *monam( int mons_num, int mons, bool vis, char desc, int mons_wpn,
                   bool title )
{
    static char gmo_n[ ITEMNAME_SIZE ];
    char gmo_n2[ ITEMNAME_SIZE ] = "";

    gmo_n[0] = '\0';

    // If you can't see the critter, let moname() print [Ii]t.
    if (!vis)
    {
        moname( mons, vis, desc, gmo_n );
        return (gmo_n);
    }

    // These need their description level handled here instead of
    // in monam().
    if (mons == MONS_SPECTRAL_THING || mons_genus( mons ) == MONS_DRACONIAN)
    {
        switch (desc)
        {
        case DESC_CAP_THE:
            strcpy(gmo_n, "The");
            break;
        case DESC_NOCAP_THE:
            strcpy(gmo_n, "the");
            break;
        case DESC_CAP_A:
            strcpy(gmo_n, "A");
            break;
        case DESC_NOCAP_A:
            strcpy(gmo_n, "a");
            break;
        case DESC_PLAIN:         /* do nothing */ ;
            break;
            //default: DEBUGSTR("bad desc flag");
        }
    }

    switch (mons)
    {
    case MONS_ZOMBIE_SMALL:
    case MONS_ZOMBIE_LARGE:
        moname(mons_num, vis, desc, gmo_n);
        strcat(gmo_n, " zombie");
        break;

    case MONS_SKELETON_SMALL:
    case MONS_SKELETON_LARGE:
        moname(mons_num, vis, desc, gmo_n);
        strcat(gmo_n, " skeleton");
        break;

    case MONS_SIMULACRUM_SMALL:
    case MONS_SIMULACRUM_LARGE:
        moname(mons_num, vis, desc, gmo_n);
        strcat(gmo_n, " simulacrum");
        break;

    case MONS_SPECTRAL_THING:
        strcat(gmo_n, " spectral ");
        moname(mons_num, vis, DESC_PLAIN, gmo_n2);
        strcat(gmo_n, gmo_n2);
        break;

    case MONS_DRACONIAN_CALLER:
    case MONS_DRACONIAN_MONK:
    case MONS_DRACONIAN_ZEALOT:
    case MONS_DRACONIAN_SHIFTER:
    case MONS_DRACONIAN_ANNIHILATOR:
    case MONS_DRACONIAN_KNIGHT:
    case MONS_DRACONIAN_SCORCHER:
        if (desc != DESC_PLAIN)
            strcat( gmo_n, " " );

        switch (mons_num)
        {
        default: break;
        case MONS_BLACK_DRACONIAN:   strcat(gmo_n, "black ");   break;
        case MONS_MOTTLED_DRACONIAN: strcat(gmo_n, "mottled "); break;
        case MONS_YELLOW_DRACONIAN:  strcat(gmo_n, "yellow ");  break;
        case MONS_GREEN_DRACONIAN:   strcat(gmo_n, "green ");   break;
        case MONS_PURPLE_DRACONIAN:  strcat(gmo_n, "purple ");  break;
        case MONS_RED_DRACONIAN:     strcat(gmo_n, "red ");     break;
        case MONS_WHITE_DRACONIAN:   strcat(gmo_n, "white ");   break;
        case MONS_PALE_DRACONIAN:    strcat(gmo_n, "pale ");    break;
        }

        moname( mons, vis, DESC_PLAIN, gmo_n2 );
        strcat( gmo_n, gmo_n2 );
        break;

    case MONS_DANCING_WEAPON:
        // safety check -- if we don't have/know the weapon use default name
        if (mons_wpn == NON_ITEM)
            moname( mons, vis, desc, gmo_n );
        else
        {
            item_def item = mitm[mons_wpn];
            unset_ident_flags( item, ISFLAG_KNOW_CURSE | ISFLAG_KNOW_PLUSES );
            item_name( item, desc, gmo_n );
        }
        break;

    case MONS_PLAYER_GHOST:
        if (title)
            snprintf( gmo_n, sizeof(gmo_n), "The ghost of %s", env.ghost.name );
        else
            snprintf( gmo_n, sizeof(gmo_n), "%s's ghost", env.ghost.name );
        break;

    case MONS_PANDEMONIUM_DEMON:
        strcpy(gmo_n, env.ghost.name);
        break;

    default:
        moname(mons, vis, desc, gmo_n);
        break;
    }

    if (title)
    {
        int str, dex;

        // XXX: need to get creative here once we go through and better
        // establish the uniques characters.  The goal here is to eventually
        // give uniques nicknames like the PC.  For now, I'm creating
        // some filler based on a quick scan of monspeak and describe. -- bwr
        switch (mons)
        {
        case MONS_IJYB:
            strncat( gmo_n, " the Twisted Goblin", sizeof(gmo_n) );
            break;

        case MONS_TERENCE:
            strncat( gmo_n, " the Incautious", sizeof(gmo_n) );
            break;

        case MONS_JESSICA:
            strncat( gmo_n, " the Apprentice Sorceress", sizeof(gmo_n) );
            break;

        case MONS_PSYCHE:
            strncat( gmo_n, " the Insane Magess", sizeof(gmo_n) );
            break;

        case MONS_DONALD:
            strncat( gmo_n, " the Adventurer", sizeof(gmo_n) );
            break;

        case MONS_WAYNE:
            strncat( gmo_n, " the Short Priest", sizeof(gmo_n) );
            break;

        case MONS_EDMUND:
        case MONS_LOUISE:
        case MONS_FRANCES:
        case MONS_DUANE:
        case MONS_ADOLF:
        case MONS_JOSEPH:
            strncat( gmo_n, ", a Mercenary", sizeof(gmo_n) );
            break;

        case MONS_SIGMUND:
            strncat( gmo_n, " the Wizened Old Coot", sizeof(gmo_n) );
            break;

        case MONS_MICHAEL:
            strncat( gmo_n, " the Hermit", sizeof(gmo_n) );
            break;

        case MONS_ERICA:
            strncat( gmo_n, " the Manic Pyromancer", sizeof(gmo_n) );
            break;

        case MONS_JOSEPHINE:
            strncat( gmo_n, " the Vexed Old Hag", sizeof(gmo_n) );
            break;

        case MONS_HAROLD:
            strncat( gmo_n, " the Impatient", sizeof(gmo_n) );
            break;

        case MONS_NORBERT:
            strncat( gmo_n, " the Famed Warrior", sizeof(gmo_n) );
            break;

        case MONS_JOZEF:
            strncat( gmo_n, " the Bounty Hunter", sizeof(gmo_n) );
            break;

        case MONS_AGNES:
            strncat( gmo_n, " the Greedy", sizeof(gmo_n) );
            break;

        case MONS_MAUD:
            strncat( gmo_n, " the Quester of 'Entarex'", sizeof(gmo_n) );
            break;

        case MONS_FRANCIS:
            strncat( gmo_n, " the Mad Physician", sizeof(gmo_n) );
            break;

        case MONS_RUPERT:
            strncat( gmo_n, " the Nutty Magician", sizeof(gmo_n) );
            break;

        case MONS_NORRIS:
            strncat( gmo_n, " the Enlightened", sizeof(gmo_n) );
            break;

        case MONS_MARGERY:
            strncat( gmo_n, " the Powerful Sorceress", sizeof(gmo_n) );
            break;

        // Leaving Blork's title in his name because it makes him sound
        // sillier ("Blork the orc does this.  Blork the orc does that.").

        case MONS_URUG:
            strncat( gmo_n, " the orc", sizeof(gmo_n) );
            break;

        case MONS_EROLCHA:
            strncat( gmo_n, " the ogress", sizeof(gmo_n) );
            break;

        case MONS_SNORG:
            strncat( gmo_n, " the troll", sizeof(gmo_n) );
            break;

        case MONS_XTAHUA:
            strncat( gmo_n, " the Ancient", sizeof(gmo_n) );
            break;

        case MONS_BORIS:
            strncat( gmo_n, ", Master of Life and Death", sizeof(gmo_n) );
            break;

        case MONS_MNOLEG:
            strncat( gmo_n, ", Demon Lord of Chaos", sizeof(gmo_n) );
            break;

        case MONS_GLOORX_VLOQ:
            strncat( gmo_n, ", Demon Lord of Darkness", sizeof(gmo_n) );
            break;

        case MONS_CEREBOV:
            strncat( gmo_n, ", Demon Lord of Fire", sizeof(gmo_n) );
            break;

        case MONS_LOM_LOBON:
            strncat( gmo_n, ", Demon Lord of Forbidden Knowledge", sizeof(gmo_n) );
            break;

        case MONS_DISPATER:
            strncat( gmo_n, ", Demon Lord of the Iron City of Dis", sizeof(gmo_n) );
            break;

        case MONS_ASMODEUS:
            strncat( gmo_n, ", Demon Prince of Gehenna", sizeof(gmo_n) );
            break;

        case MONS_ANTAEUS:
            strncat( gmo_n, ", Titan Guardian of Cocytus", sizeof(gmo_n) );
            break;

        case MONS_ERESHKIGAL:
            strncat( gmo_n, ", Demon Queen of Tartarus", sizeof(gmo_n) );
            break;

        case MONS_PLAYER_GHOST:
            fudge_ghost_stats( str, dex );

            strncat( gmo_n, ", the ", sizeof( gmo_n ) );
            strncat( gmo_n, skill_title( env.ghost.values[GVAL_BEST_SKILL],
                                         env.ghost.values[GVAL_SKILL_LEVEL],
                                         env.ghost.values[GVAL_SPECIES],
                                         str, dex, GOD_NO_GOD, 0, 0 ),
                     sizeof(gmo_n) );
            break;

        case MONS_PANDEMONIUM_DEMON:
            // TODO: add code to generate random demon titles -- bwr
            break;

        default:
            break;
        }
    }

    return (gmo_n);
}                               // end monam()

const char * moname( int mons_num, bool vis, char descrip,
                     char buff[ ITEMNAME_SIZE ] )
{
    buff[0] = '\0';

    if (!vis)
    {
        switch (descrip)
        {
        case DESC_CAP_THE:
        case DESC_CAP_A:
            strcpy( buff, "It" );
            break;
        case DESC_NOCAP_THE:
        case DESC_NOCAP_A:
        case DESC_PLAIN:
            strcpy( buff, "it" );
            break;
        }

        return (buff);
    }

    char gmon_name[ ITEMNAME_SIZE ] = "";
    strcpy( gmon_name, seekmonster( &mons_num )->name );

    if (!mons_is_unique( mons_num ))
    {
        switch (descrip)
        {
        case DESC_CAP_THE:
            strcpy( buff, "The " );
            break;
        case DESC_NOCAP_THE:
            strcpy( buff, "the " );
            break;
        case DESC_CAP_A:
            strcpy( buff, "A" );
            break;
        case DESC_NOCAP_A:
            strcpy( buff, "a" );
            break;
        case DESC_PLAIN:
            break;
        // default: DEBUGSTR("bad monster descrip flag");
        }

        if (descrip == DESC_CAP_A || descrip == DESC_NOCAP_A)
        {
            switch (toupper( gmon_name[0] ))
            {
            case 'A':
            case 'E':
            case 'I':
            case 'O':
            case 'U':
                strcat( buff, "n " );
                break;

            default:
                strcat( buff, " " );
                break;
            }
        }
    }

    strcat( buff, gmon_name );

    return (buff);
}                               // end moname()

/* ********************* END PUBLIC FUNCTIONS ********************* */

// see mons_init for initialization of mon_entry array.
static struct monsterentry *seekmonster( int *p_monsterid )
{
    ASSERT( p_monsterid != 0 );

    int me = mon_entry[ (*p_monsterid) ];

    if (me >= 0)                // PARANOIA
        return (&mondata[ mon_entry[(*p_monsterid)] ]);
    else
        return (NULL);
}                               // end seekmonster()

static int mons_exp_mod(int mc)
{
    return (smc->exp_mod);
}                               // end mons_exp_mod()

// Guards have a slightly different behaviour.  They don't leave the
// level they're created on and they don't reveal or use secret doors.
bool mons_is_guard( struct monsters *mon )
{
    return (testbits( mon->flags, MF_GUARD )
            || mon->type == MONS_PLAYER_GHOST
            || mon->type == MONS_PANDEMONIUM_DEMON
            || mon->type == MONS_VAULT_GUARD);
}

bool mons_stationary( int mc )
{
    return (smc->speed < 1
            || mc == MONS_OKLOB_PLANT
            || mc == MONS_CURSE_SKULL
            || (mc >= MONS_CURSE_TOE && mc <= MONS_POTION_MIMIC));
}

static speed_type  mons_net_speed_effect( struct monsters *mon )
{
    int ret = SPEED_NORMAL;

    if (mons_has_ench( mon, ENCH_SLOW ))
        ret -= 1;

    if (mons_has_ench( mon, ENCH_HASTE ))
        ret += 1;

    return (static_cast< speed_type >( ret ));
}

int mons_speed( struct monsters *mon )
{
    // heartbeat for enchantment type effects, stationary now handled above
    if (mon->speed < 1)
        return (10);

    // monster speed is in energy per time.
    int ret = adjust_energy_for_speed( mon->speed, mons_net_speed_effect(mon) );

    return ((ret < 3) ? 3 : ((ret > 40) ? 40 : ret));
}

int mons_class_speed( int mc )
{
    return (smc->speed);
}

// returns the energy cost of mon attacking
int mons_attack_cost( struct monsters *mon )
{
    int ret = 10;

    if (mons_itemuse( mon->type ) >= MONUSE_STARTING_EQUIPMENT)
    {
        const int wpn = mon->inv[MSLOT_WEAPON];
        if (wpn != NON_ITEM
            && (mitm[wpn].base_type == OBJ_WEAPONS
                || mitm[wpn].base_type == OBJ_STAVES)
            && !is_range_weapon( mitm[wpn] ))
        {
            ret = property( mitm[wpn], PWPN_SPEED );

            if (mons_weapon_brand( mon, MSLOT_WEAPON ) == SPWPN_SPEED)
                ret = (ret * 2) / 3;
        }

        const int sec = mon->inv[MSLOT_MISSILE];

        if ((mon->type == MONS_TWO_HEADED_OGRE || mon->type == MONS_ETTIN)
            && sec != NON_ITEM
            && (mitm[sec].base_type == OBJ_WEAPONS
                || mitm[sec].base_type == OBJ_STAVES)
            && !is_range_weapon( mitm[sec] ))
        {
            int tmp = property( mitm[sec], PWPN_SPEED );

            if (mons_weapon_brand( mon, MSLOT_MISSILE ) == SPWPN_SPEED)
                tmp = (tmp * 2) / 3;

            // simple approach - take the slower of the two weapons and add two:
            ret = (tmp > ret) ? tmp + 2 : ret + 2;
        }
    }

    ret = div_rand_round( ret * (seekmonster(&mon->type))->attack_cost, 10 );

    return ((ret < 3) ? 3 : (ret > 20) ? 20 : ret);
}

//jmf: "fixed" to work with new I_ types
monster_intel_type mons_intel(int mc)
{
    switch (smc->intel)
    {
    case I_PLANT:
        return (I_PLANT);
    case I_INSECT:
    case I_REPTILE:
        return (I_INSECT);
    case I_ANIMAL:
    case I_ANIMAL_LIKE:
        return (I_ANIMAL);
    case I_NORMAL:
        return (I_NORMAL);
    case I_HIGH:
        return (I_HIGH);
    default:
        return (I_NORMAL);
    }
}                               // ens mons_intel()


monster_intel_type mons_intel_type(int mc)     //jmf: new, used by my spells
{
    return (smc->intel);
}                               // end mons_intel_type()

int mons_power(int mc)
{
    // for now,  just return monster hit dice.
    return (smc->hpdice[0]);
}

bool beings_aligned( int m1_id, int m2_id )
{
    ASSERT( m1_id != m2_id );

    bool fr1, fr2;

    if (m1_id == MHITNOT || m2_id == MHITNOT)
        return (true);

    if (m1_id == MHITYOU)
        fr1 = true;
    else
    {
        fr1 = (menv[m1_id].attitude == ATT_FRIENDLY
                || mons_has_ench( &menv[m1_id], ENCH_CHARM ));
    }

    if (m2_id == MHITYOU)
        fr2 = true;
    else
    {
        fr2 = (menv[m2_id].attitude == ATT_FRIENDLY
                || mons_has_ench( &menv[m2_id], ENCH_CHARM ));
    }

    return (fr1 == fr2);
}

bool mons_friendly( struct monsters *m )
{
    return (m->attitude == ATT_FRIENDLY || mons_has_ench( m, ENCH_CHARM ));
}

// returns true if dog is a pet of owner
static bool mons_is_pet( struct monsters *owner, struct monsters *dog )
{
    ASSERT( beings_aligned( monster_index(owner), monster_index(dog) ) );

    const int own_genus = mons_genus( owner->type );

    // orcs keep pet wargs:
    if (own_genus == MONS_ORC && dog->type == MONS_WARG)
        return (true);

    // mummies keep pet sphinxes:
    if (own_genus == MONS_MUMMY && dog->type == MONS_SPHINX)
        return (true);

    // necromancers currently treat any undead as pets:
    if (mons_has_spell( owner, MS_ANIMATE_DEAD )
        && mons_holiness( dog ) == MH_UNDEAD)
    {
        return (true);
    }

    // summoners currently treat any summoned monsters as pets:
    if (mons_has_summon_spell(owner) != MS_NO_SPELL && mons_is_summoned( dog ))
        return (true);

    return (false);
}

// Note that this is intentionaly asymetrical.
bool mons_can_swap( struct monsters *mon, struct monsters *targ )
{
    ASSERT( grid_distance( mon->x, mon->y, targ->x, targ->y ) == 1
            && beings_aligned( monster_index(mon), monster_index(targ) )
            && !mons_stationary( mon->type ) );

    // cannot swap with a monster which has recently swapped
    if (testbits( targ->flags, MF_SWAPPED ))
        return (false);

    // swapping with stationary monsters is forbidden.
    if (mons_stationary( targ->type ))
        return (false);

    // uniques cannot be pushed around by non-uniques
    if (mons_is_unique( targ->type ) && !mons_is_unique( mon->type ))
        return (false);

    const bool mon_scared = mons_is_scared( mon );
    const bool targ_scared = mons_is_scared( targ );

    // Distances from the target's target (mon's target isn't interesting
    // because if this move becomes a real consideration it will be
    // because, all things considered, it's the direction we want).
    const int mon_dist = grid_distance( mon->x, mon->y,
                                        targ->target_x, targ->target_y );

    const int targ_dist = grid_distance( targ->x, targ->y,
                                         targ->target_x, targ->target_y );

    // scared mon will force unscared targ
    if (mon_scared && !targ_scared)
        return (true);

    // scared targ is willing to be pushed if it gets them distance
    if (!mon_scared && targ_scared)
        return (mon_dist >= targ_dist);

    // owners can swap positions with pets:
    if (mons_is_pet( mon, targ ))
        return (true);

    const int mon_holy = mons_holiness( mon );
    const int targ_holy = mons_holiness( targ );

    // check teams: demon, undead, same genus
    if (mons_is_demon( mon->type ) && targ_holy == MH_DEMONIC)
    {
        // demonic hierarchy (demonlords, ranked demons, demonic creatures)
        const int mon_rank = demon_rank( mon->type );
        const int targ_rank = demon_rank( targ->type );

        // remember: demon ranks count downwards
        return (mon_rank < targ_rank
                || (mon_rank == targ_rank && mon->hit_dice > targ->hit_dice));
    }
    else if (mon_holy == MH_UNDEAD && targ_holy == MH_UNDEAD)
    {
        // undead hierarchy
        const int mon_rank = undead_rank( mon->type );
        const int targ_rank = undead_rank( targ->type );

        // remember: undead ranks count downwards
        return (mon_rank < targ_rank
                || (mon_rank == targ_rank && mon->hit_dice > targ->hit_dice));
    }
    else if (mons_genus( mon->type ) == mons_genus( targ->type ))
    {
        // "larger" members of the same team may push peons around:
        return (mon->hit_dice > targ->hit_dice);
    }

    return (false);
}

/* ******************************************************************

// In the name of England, I declare this function wasteful! {dlb}

static monsterentry *seekmonster( int mc )
{

    ASSERT(mc >= 0);

    int x = 0;

    while (x < mondatasize)
    {
        if (mondata[x].mc == mc)
          return &mondata[x];

        x++;
    }

    ASSERT(false);

    return seekmonster(MONS_PROGRAM_BUG);    // see the disasters coming if there is no 250?

}          // end seekmonster()
****************************************************************** */


/* ******************************************************************

// only used once, and internal to this file, to boot {dlb}:

// These are easy to implement here. The difficult (dull!) work of converting
// the data structures is finally finished now!
inline char *mons_name( int mc )
{

    return smc->name;

}          // end mons_name()
****************************************************************** */

/*****************************************************************

  Used to determine whether or not a monster should fire a beam (MUST be
  called _after_ fire_tracer() for meaningful result.

*/

bool mons_should_fire(struct bolt &beam)
{
    // use of foe_ratio:
    // the higher this number, the more monsters
    // will _avoid_ collateral damage to their friends.
    // setting this to zero will in fact have all
    // monsters ignore their friends when considering
    // collateral damage.
#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS,
          "fire? foe: %d:%d, friend: %d:%d, ratio: %d, smart: %d",
          beam.foe_count, beam.foe_power,
          beam.fr_count, beam.fr_power,
          beam.foe_ratio, beam.smart_monster );
#endif

    // quick check - did we in fact get any foes?
    if (beam.foe_count == 0)
        return (false);

    // if we either hit no friends, or monster too dumb to care
    if (beam.fr_count == 0 || !beam.smart_monster)
        return (true);

    // only fire if they do acceptably low collateral damage
    // the default for this is 50%;  in other words, don't
    // hit a foe unless you hit 2 or fewer friends.
    if (beam.foe_power >= (beam.foe_ratio * beam.fr_power) / 100)
        return (true);

    return (false);
}

// XXX Ugly temp function to calculate durations for the new enchantment
// system... still must go through all the enchantments and come up with
// real calculations for durations and extensions.
int levels_to_dur( enchant_type ench, int levels )
{
    ASSERT( ench != ENCH_NONE );

    int dur = 0;
    int turns = 0;

    switch (ench)
    {
    case ENCH_HASTE:        // 40
    case ENCH_INVIS:
        turns = 30;
        dur = 10;
        levels = 1;
        break;

    case ENCH_FEAR:         // 30
    case ENCH_CHARM:
        turns = 25;
        dur = 5;
        levels = 1;
        break;

    case ENCH_CONFUSION:    // 25
    case ENCH_QUIET:
    case ENCH_SLOW:
    case ENCH_SLEEP_WARY:
        turns = 20;
        dur = 5;
        levels = 1;
        break;

    case ENCH_ROT:          // 5 per
    case ENCH_STICKY_FLAME:
    case ENCH_BACKLIGHT:
    case ENCH_STUN:
        turns = 3;
        dur = levels * 2;
        break;

    case ENCH_POISON:       // 8 per
    case ENCH_CUT:
        turns = 4;
        dur = levels * 4;
        break;

    case ENCH_SUMMONED:     // 10 per
        turns = 5;
        dur = levels * 5;
        break;

    case ENCH_TELEPORT:     // 1 per level
        dur = levels;
        break;

    case ENCH_PETRIFY:      // 5
    case ENCH_PARALYSIS:
        dur += roll_dice(1,5);
        break;

    default:
        break;
    }

    if (turns)
    {
        // XXX: starting with duplicating old negative binomial,
        // replace this with something better, or get rid of this
        // whole function.
        for (int i = 0; i < levels; i++)
        {
            do
            {
                dur++;
            }
            while (!one_chance_in( turns ));
        }
    }

    return (10 * dur);
}

// Temporary function to calculate levels from durations
int dur_to_levels( enchant_type ench, int dur )
{
    ASSERT( ench != ENCH_NONE );

    int ret = 1;
    int max = 1;

    switch (ench)
    {
    default:
        break;

    case ENCH_ROT:          // 5 per
    case ENCH_STICKY_FLAME:
        ret = (dur / 50);
        max = 4;
        break;

    case ENCH_POISON:       // 8 per
        ret = (dur / 80);
        max = 4;
        break;

    case ENCH_SUMMONED:     // 10 per
        ret = (dur / 100);
        max = 6;
        break;

    case ENCH_TELEPORT:     // 1 per
        ret = (dur / 10);
        max = 4;
        break;
    }

    return ((ret > max) ? max : ret );
}

// Returns true if added or already present.
// NOTE: levels is a transitional factor between the old system and
// the new... don't use it in new code, we'd like to get rid of it
// eventually.
bool mons_add_ench( struct monsters *mon, enchant_type ench,
                    int source, int duration, int levels )
{
    ASSERT( ench != ENCH_NONE );

    int new_spot = -1;
    for (int p = 0; p < NUM_MON_ENCHANTS; p++)
    {
        // don't double-add
        if (mon->ench[p].type == ench)
            return (true);

        // set new_spot to the first empty, but keep looking for duplicate
        if (mon->ench[p].type == ENCH_NONE && new_spot < 0)
            new_spot = p;
    }

    if (new_spot < 0)
        return (false);

    // if duration not passed in, calculate one
    if (duration == -1)
        duration = levels_to_dur( ench, levels );

    mon->ench[ new_spot ].type     = ench;
    mon->ench[ new_spot ].source   = source;
    mon->ench[ new_spot ].duration = duration;

    if (ench == ENCH_FEAR)
        behaviour_event( mon, ME_SCARE, source );

    if (ench == ENCH_PETRIFY)
        mon->colour = (mon->colour == LIGHTGREY) ? WHITE : LIGHTGREY;

    return (true);
}

enchant_retval mons_inc_ench_levels( struct monsters *mon, enchant_type ench,
                                     int source, int levels )
{
    ASSERT( ench != ENCH_NONE );

    enchant_retval ret = ERV_FAIL;

    mon_enchant_def *const ench_ptr = mons_has_ench( mon, ench );

    if (!ench_ptr)
    {
        if (mons_add_ench( mon, ench, source, -1, levels ))
            ret = ERV_NEW;
    }
    else
    {
        // Player inherits control if they enchant at any time.  This
        // matches the old behaviour of poison/sticky flame.
        if (ench_ptr->source != MHITYOU)
            ench_ptr->source = source;

        const int max = levels_to_dur( ench, (ench == ENCH_SUMMONED) ? 6 : 4 );

        if (ench_ptr->duration < max)
        {
            ret = ERV_INCREASED;
            ench_ptr->duration += levels_to_dur( ench, levels );
            if (ench_ptr->duration > max)
                ench_ptr->duration = max;
        }
    }

    return (ret);
}

// returns a pointer to the internal member or 0 if not found
mon_enchant_def * mons_has_ench( struct monsters *mon, enchant_type ench )
{
    ASSERT( ench != ENCH_NONE );

    for (int p = 0; p < NUM_MON_ENCHANTS; p++)
    {
        if (mon->ench[p].type == ench)
            return (&(mon->ench[p]));
    }

    return (0);
}

// does the actual work of removing enchantments for del_ench and dec_ench
bool mons_del_ench_ptr( struct monsters *mon, mon_enchant_def *ench_ptr,
                        bool quiet )
{
    ASSERT( ench_ptr != 0 );
    ASSERT( ench_ptr->type != ENCH_NONE );

    switch (ench_ptr->type)
    {
    default:
        break;

    case ENCH_FEAR:
        if (!quiet)
            mon_msg( mon, " seems to regain its courage." );

        // reevaluate behaviour
        behaviour_event( mon, ME_EVAL );
        break;

    case ENCH_CONFUSION:
        if (!quiet)
            mon_msg( mon, " seems less confused." );

        // reevaluate behaviour
        behaviour_event( mon, ME_EVAL );
        break;

    case ENCH_INVIS:
        // note: invisible monsters stay invisible
        if (mons_near( mon )
            && !player_see_invis()
            && !mons_is_submerged( mon )
            && !mons_class_flag( mon->type, M_INVIS ))
        {
            if (!quiet)
                mpr( "%s appears!", ptr_monam( mon, DESC_CAP_A ) );
        }
        break;

    case ENCH_SUMMONED:
        // MHITYOU gets us a cloud, MHITNOT is no cloud (good for quiet update)
        monster_die( mon, KILL_RESET, (quiet) ? MHITNOT : MHITYOU );
        break;

    case ENCH_CHARM:
        if (!quiet)
            mon_msg( mon, " is no longer charmed." );

        // reevaluate behaviour
        behaviour_event(mon, ME_EVAL);
        break;

    case ENCH_BACKLIGHT:
        if (!quiet)
            mon_msg( mon, " stops glowing." );
        break;

    case ENCH_STICKY_FLAME:
        if (!quiet)
            mon_msg( mon, " stops burning." );
        break;

    case ENCH_POISON:
        if (!quiet)
            mon_msg( mon, " looks more healthy." );
        break;

    case ENCH_ROT:
        if (!quiet)
            mon_msg( mon, " is no longer rotting." );
        break;

    case ENCH_SLOW:
        if (!quiet)
            mon_msg( mon, " speeds up." );
        break;

    case ENCH_HASTE:
        if (!quiet)
            mon_msg( mon, " slows down." );
        break;

    case ENCH_PETRIFY:
        mon->colour = mons_class_colour( mon->type );
        if (!quiet)
            mon_msg( mon, " is no longer stone." );
        break;
    }

    ench_ptr->type      = ENCH_NONE;
    ench_ptr->source    = MHITNOT;
    ench_ptr->duration  = 0;
    ench_ptr->work      = 0;

    return (true);
}

// Returns true if enchantment was deleted (false if not found).
bool mons_del_ench( struct monsters *mon, enchant_type ench, bool quiet )
{
    ASSERT( ench != ENCH_NONE );

    int p;
    for (p = 0; p < NUM_MON_ENCHANTS; p++)
    {
        if (mon->ench[p].type == ench)
            break;
    }

    if (p == NUM_MON_ENCHANTS)
        return (false);

    return (mons_del_ench_ptr( mon, &(mon->ench[p]), quiet ));
}

// Returns true if enchantment is deleted.
bool mons_dec_ench_ptr( struct monsters *mon, mon_enchant_def *ench_ptr,
                        int amount, bool quiet )
{
    ASSERT( ench_ptr != 0 );
    ASSERT( ench_ptr->type != ENCH_NONE );

    if (ench_ptr->duration == PERM_MON_ENCH)
        return (false);

    ench_ptr->duration -= amount;

    if (ench_ptr->duration <= 0)
        return (mons_del_ench_ptr( mon, ench_ptr, quiet ));

    return (false);
}

// note: the caller is reponsible for handling (and attributing) any deaths
void drain_monster( struct monsters* mon, bool quiet )
{
    if (!quiet)
        mon_msg( mon, " is drained." );

    if (one_chance_in(3))
        mon->hit_dice--;

    const int hp_loss = 3 + random2(5);

    mon->max_hit_points -= hp_loss;
    mon->hit_points     -= hp_loss;

    // paranoia:
    if (mon->hit_points > mon->max_hit_points)
        mon->hit_points = mon->max_hit_points;
}


// used to determine whether or not a monster should always
// fire this spell if selected.  If not, we should use a
// tracer.

// note - this function assumes that the monster is "nearby"
// its target!

bool ms_requires_tracer(int monspell)
{
    bool requires = false;

    switch(monspell)
    {
        case MS_BANISHMENT:
        case MS_COLD_BOLT:
        case MS_CONFUSE:
        case MS_CRYSTAL_SPEAR:
        case MS_DISINTEGRATE:
        case MS_ENERGY_BOLT:
        case MS_FIRE_BOLT:
        case MS_FIREBALL:
        case MS_FLAME:
        case MS_FROST:
        case MS_HELLFIRE:
        case MS_HELLFROST:
        case MS_IRON_BOLT:
        case MS_LIGHTNING_BOLT:
        case MS_MARSH_GAS:
        case MS_MIASMA:
        case MS_METAL_SPLINTERS:
        case MS_MMISSILE:
        case MS_NEGATIVE_BOLT:
        case MS_ORB_ENERGY:
        case MS_PAIN:
        case MS_PARALYSIS:
        case MS_POISON_SPLASH:
        case MS_QUICKSILVER_BOLT:
        case MS_SLOW:
        case MS_STEAM_BALL:
        case MS_STICKY_FLAME:
        case MS_STING:
        case MS_STONE_ARROW:
        case MS_TELEPORT_OTHER:
        case MS_VENOM_BOLT:
        case MS_POISON_BLAST:
        case MS_POISON_ARROW:
            requires = true;
            break;

        // self-niceties and direct effects
        case MS_BRAIN_FEED:
        case MS_HELLFIRE_BURST:
        case MS_MUTATION:
        case MS_SMITE:
        case MS_ANIMATE_DEAD:
        case MS_BLINK:
        case MS_CONTROLLED_BLINK:
        case MS_DIG:
        case MS_FAKE_RAKSHASA_SUMMON:
        case MS_HASTE:
        case MS_HEAL:
        case MS_INVIS:
        case MS_LEVEL_SUMMON:
        case MS_SUMMON_BEAST:
        case MS_SUMMON_DEMON_LESSER:
        case MS_SUMMON_DEMON:
        case MS_SUMMON_DEMON_GREATER:
        case MS_SUMMON_UFETUBUS:
        case MS_TELEPORT:
        case MS_TORMENT:
        case MS_DISPEL_UNDEAD:
        case MS_VAMPIRE_SUMMON:
        case MS_CANTRIP:

        // meaningless, but sure, why not?
        case MS_NO_SPELL:
            break;

        default:
            break;

    }

    return (requires);
}

// returns true if the spell is something you wouldn't want done if
// you had a friendly target..  only returns a meaningful value for
// non-beam spells

bool ms_direct_nasty(int monspell)
{
    bool nasty = true;

    switch(monspell)
    {
        // self-niceties/summonings
        case MS_ANIMATE_DEAD:
        case MS_BLINK:
        case MS_CONTROLLED_BLINK:
        case MS_DIG:
        case MS_FAKE_RAKSHASA_SUMMON:
        case MS_HASTE:
        case MS_HEAL:
        case MS_INVIS:
        case MS_LEVEL_SUMMON:
        case MS_SUMMON_BEAST:
        case MS_SUMMON_DEMON_LESSER:
        case MS_SUMMON_DEMON:
        case MS_SUMMON_DEMON_GREATER:
        case MS_SUMMON_UFETUBUS:
        case MS_TELEPORT:
        case MS_VAMPIRE_SUMMON:
            nasty = false;
            break;

        case MS_BRAIN_FEED:
        case MS_HELLFIRE_BURST:
        case MS_MUTATION:
        case MS_SMITE:
        case MS_TORMENT:
        case MS_DISPEL_UNDEAD:

        // meaningless, but sure, why not?
        case MS_NO_SPELL:
            break;

        default:
            break;

    }

    return (nasty);
}

// Spells a monster may want to cast if fleeing from the player, and
// the player is not in sight.
bool ms_useful_fleeing_out_of_sight( struct monsters *mon, int monspell )
{
    UNUSED( mon );

    switch (monspell)
    {
    case MS_HASTE:
    case MS_INVIS:
    case MS_HEAL:
    case MS_ANIMATE_DEAD:
    case MS_TELEPORT:
        return (true);

    case MS_VAMPIRE_SUMMON:
    case MS_SUMMON_UFETUBUS:
    case MS_FAKE_RAKSHASA_SUMMON:
    case MS_LEVEL_SUMMON:
    case MS_SUMMON_DEMON:
    case MS_SUMMON_DEMON_LESSER:
    case MS_SUMMON_BEAST:
    case MS_SUMMON_UNDEAD:
    case MS_SUMMON_DEMON_GREATER:
        if (one_chance_in(10))    // only summon friends some of the time
            return (true);
        break;

    default:
        break;
    }

    return (false);
}

bool ms_low_hitpoint_cast( struct monsters *mon, int monspell )
{
    bool ret = false;
    bool targ_adj = false;

    if (mon->foe == MHITYOU || mon->foe == MHITNOT)
    {
        if (adjacent(you.x_pos, you.y_pos, mon->x, mon->y))
            targ_adj = true;
    }
    else if (adjacent( menv[mon->foe].x, menv[mon->foe].y, mon->x, mon->y ))
    {
        targ_adj = true;
    }

    switch (monspell)
    {
    case MS_VAMPIRE_SUMMON:
    case MS_SUMMON_UFETUBUS:
    case MS_FAKE_RAKSHASA_SUMMON:
    case MS_HEAL:
    case MS_TELEPORT:
        ret = true;
        break;

    case MS_CONTROLLED_BLINK:
    case MS_TELEPORT_OTHER:
        if (mons_near( mon ))
            ret = true;
        break;

    case MS_BLINK:
        if (targ_adj)
            ret = true;
        break;
    }

    return (ret);
}

// Checks to see if a particular spell is worth casting in the first place.
bool ms_waste_of_time( struct monsters *mon, int monspell )
{
    bool  ret = false;
    int intel, est_magic_resist, power, diff;
    struct monsters *targ;

    if (mons_friendly(mon) && (mon->foe == MHITNOT || mon->foe == MHITYOU))
        return (true);

    // Eventually, we'll probably want to be able to have monsters
    // learn which of their elemental bolts were resisted and have those
    // handled here as well. -- bwr
    switch (monspell)
    {
    case MS_HASTE:
        if (mons_has_ench( mon, ENCH_HASTE ))
            ret = true;
        break;

    case MS_INVIS:
        if (mons_is_invisible( mon ))
            ret = true;
        break;

    case MS_HEAL:
        // estimate an average healing, but don't assume too much
        power = 2 * mon->hit_dice;
        if (power > mon->max_hit_points / 2)
            power = mon->max_hit_points / 2;

        if (mon->hit_points > mon->max_hit_points - power)
            ret = true;
        break;

    case MS_TELEPORT:
        // Monsters aren't smart enough to know when to cancel teleport.
        if (mons_has_ench( mon, ENCH_TELEPORT ))
            ret = true;

        if (grid_distance(mon->x, mon->y, mon->target_x, mon->target_y) > 12)
            ret = true;
        break;

    case MS_TELEPORT_OTHER:
        // Monsters aren't smart enough to know when to cancel teleport.
        if (mon->foe == MHITYOU)
        {
            if (you.duration[DUR_TELEPORT])
                return (true);
        }
        else if (mon->foe != MHITNOT)
        {
            if (mons_has_ench( &menv[mon->foe], ENCH_TELEPORT ))
                return (true);
        }
        // intentional fall-through

    case MS_SLOW:
    case MS_CONFUSE:
    case MS_PAIN:
    case MS_BANISHMENT:
    case MS_DISINTEGRATE:
    case MS_PARALYSIS:
        // occasionally we don't estimate... just fire and see:
        if (one_chance_in(5))
            return (false);

        // Only intelligent monsters estimate.
        intel = mons_intel( mon->type );
        if (intel != I_NORMAL && intel != I_HIGH)
            return (false);

        // We'll estimate the target's resistance to magic, by first getting
        // the actual value and then randomizing it.
        est_magic_resist = (mon->foe == MHITNOT) ? 10000 : 0;

        if (mon->foe != MHITNOT)
        {
            if (mon->foe == MHITYOU)
                est_magic_resist = player_res_magic();
            else
            {
                targ = &menv[ mon->foe ];
                est_magic_resist = mons_resist_magic( targ );
            }

            // now randomize (normal intels less accurate than high):
            if (intel == I_HIGH)
                est_magic_resist += random2(30) - 15;
            else
                est_magic_resist += random2(80) - 40;
        }

        power = 12 * mon->hit_dice * (monspell == MS_PAIN ? 2 : 1);
        power = stepdown_value( power, 30, 40, 100, 120 );

        // Determine the amount of chance allowed by the benefit from
        // the spell.  The estimated difficulty is the probability
        // of rolling over 100 + diff on 2d100. -- bwr
        diff = (monspell == MS_PAIN
                || monspell == MS_SLOW
                || monspell == MS_CONFUSE) ? 0 : 50;

        if (est_magic_resist - power > diff)
            ret = true;

        break;

    case MS_DISPEL_UNDEAD:
        // XXX: this should be better... check other monsters than player, etc
        ret = !you.is_undead;
        break;

    case MS_TORMENT:
        // XXX: this should be better... check other monsters than player, etc
        ret = (you.is_undead || you.hp <= you.hp_max / 8);
        break;

    default:
        break;
    }

    return (ret);
}

static bool ms_ranged_spell( int monspell )
{
    switch (monspell)
    {
    case MS_HASTE:
    case MS_HEAL:
    case MS_TELEPORT:
    case MS_INVIS:
    case MS_BLINK:
    case MS_CONTROLLED_BLINK:
        return (false);

    default:
        break;
    }

    return (true);
}

bool mons_has_ranged_spell( struct monsters *mon )
{
    const int  mclass = mon->type;

    if (mons_class_flag( mclass, M_SPELLCASTER ))
    {
        for (int i = 0; i < NUM_MONSTER_SPELL_SLOTS; i++)
        {
            if (ms_ranged_spell( mon->spells[i] ))
                return (true);
        }
    }

    return (false);
}

// returns MS_NO_SPELL or a random teleport spell the monster has
int mons_has_teleport_spell( struct monsters *mon )
{
    const int  mclass = mon->type;
    int  ret = MS_NO_SPELL;

    if (mons_class_flag( mclass, M_SPELLCASTER ))
    {
        int count = 0;
        for (int i = 0; i < NUM_MONSTER_SPELL_SLOTS; i++)
        {
            if (mon->spells[i] == MS_TELEPORT
                || mon->spells[i] == MS_BLINK
                || mon->spells[i] == MS_CONTROLLED_BLINK)
            {
                count++;
                if (one_chance_in(count))
                    ret = mon->spells[i];
            }
        }
    }

    return (ret);
}

bool ms_summon_spell( int monspell )
{
    switch (monspell)
    {
    case MS_VAMPIRE_SUMMON:
    case MS_LEVEL_SUMMON:
    case MS_FAKE_RAKSHASA_SUMMON:
    case MS_SUMMON_DEMON:
    case MS_SUMMON_DEMON_LESSER:
    case MS_SUMMON_UFETUBUS:
    case MS_SUMMON_BEAST:
    case MS_SUMMON_UNDEAD:
    case MS_SUMMON_DEMON_GREATER:
        return (true);

    default:
        break;
    }

    return (false);
}

// returns random summon spell or MS_NO_SPELL if none
int  mons_has_summon_spell( struct monsters *mon )
{
    const int  mclass = mon->type;
    int  ret = MS_NO_SPELL;

    if (mons_class_flag( mclass, M_SPELLCASTER ))
    {
        int count = 0;
        for (int i = 0; i < NUM_MONSTER_SPELL_SLOTS; i++)
        {
            if (ms_summon_spell( mon->spells[i] ))
            {
                count++;
                if (one_chance_in(count))
                    ret = mon->spells[i];
            }
        }
    }

    return (ret);
}

bool mons_has_spell( struct monsters *mon, int mspell )
{
    const int  mclass = mon->type;

    if (mons_class_flag( mclass, M_SPELLCASTER ))
    {
        for (int i = 0; i < NUM_MONSTER_SPELL_SLOTS; i++)
        {
            if (mon->spells[i] == mspell)
                return (true);
        }
    }

    return (false);
}

bool mons_has_ranged_attack( struct monsters *mon )
{
    // special abilities (icky handling)
    switch (mon->type)
    {
    case MONS_MANTICORE:
        if (mon->number > 0)
            return (true);
        break;

    case MONS_DRAGON:
    case MONS_HELL_HOUND:
    case MONS_ICE_DRAGON:
    case MONS_LINDWURM:
    case MONS_FIRE_DRAKE:
    case MONS_XTAHUA:
    case MONS_FIEND:
    case MONS_PIT_FIEND:
    case MONS_ACID_BLOB:
    case MONS_OKLOB_PLANT:
    case MONS_ELECTRICAL_EEL:
    case MONS_LAVA_SNAKE:
        return (true);

    default:
        break;
    }

    if (mons_itemuse( mon->type ) < MONUSE_STARTING_EQUIPMENT)
        return (false);

    const int ammo = mon->inv[MSLOT_MISSILE];

    if (ammo != NON_ITEM)
        return (is_launched( monster_index(mon), mitm[ammo] ) != LRET_FUMBLED);

    return (false);
}


// use of variant:
// 0 : She is tap dancing.
// 1 : It seems she is tap dancing. (lower case pronoun)
// 2 : Her sword explodes!          (upper case possessive)
// 3 : It sticks to her sword!      (lower case possessive)
// ... as needed

const char *mons_pronoun(int mon_type, int variant)
{
    int gender = GENDER_NEUTER;

    if (mons_is_unique( mon_type ))
    {
        switch(mon_type)
        {
            case MONS_JESSICA:
            case MONS_PSYCHE:
            case MONS_JOSEPHINE:
            case MONS_AGNES:
            case MONS_MAUD:
            case MONS_LOUISE:
            case MONS_FRANCES:
            case MONS_MARGERY:
            case MONS_EROLCHA:
            case MONS_ERICA:
            case MONS_ERESHKIGAL:
                gender = GENDER_FEMALE;
                break;
            default:
                gender = GENDER_MALE;
                break;
        }
    }

    switch(variant)
    {
        case PRONOUN_CAP:
            return ((gender == 0) ? "It" :
                    (gender == 1) ? "He" : "She");

        case PRONOUN_NOCAP:
            return ((gender == 0) ? "it" :
                    (gender == 1) ? "he" : "she");

        case PRONOUN_CAP_POSSESSIVE:
            return ((gender == 0) ? "Its" :
                    (gender == 1) ? "His" : "Her");

        case PRONOUN_NOCAP_POSSESSIVE:
            return ((gender == 0) ? "its" :
                    (gender == 1) ? "his" : "her");

        case PRONOUN_REFLEXIVE:  // awkward at start of sentence, always lower
            return ((gender == 0) ? "itself"  :
                    (gender == 1) ? "himself" : "herself");
    }

    return ("");
}

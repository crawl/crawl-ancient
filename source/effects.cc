/*
 *  File:       effects.cc
 *  Summary:    Misc stuff.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "effects.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "globals.h"
#include "externs.h"

#include "abyss.h"
#include "beam.h"
#include "delay.h"
#include "direct.h"
#include "dungeon.h"
#include "itemname.h"
#include "itemprop.h"
#include "items.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mon-util.h"
#include "mutation.h"
#include "newgame.h"
#include "ouch.h"
#include "player.h"
#include "randart.h"
#include "skills2.h"
#include "spells3.h"
#include "spells4.h"
#include "spl-book.h"
#include "spl-util.h"
#include "stuff.h"
#include "view.h"

// torment_monsters is called with power 0 because torment is
// UNRESISTABLE except for being undead or having torment
// resistance!   Even if we used maximum power of 1000,  high
// level monsters and characters would save too often.  (GDL)
static int torment_monsters( int x, int y, int pow, int garbage )
{
    UNUSED( pow );
    UNUSED( garbage );

    // is player?
    if (x == you.x_pos && y == you.y_pos)
    {
        if (you.is_undead || you.mutation[MUT_TORMENT_RESISTANCE] >= 3)
            mpr( "You feel a surge of unholy energy." );
        else
        {
            mpr( "Your body is wracked with pain!" );
            dec_hp( you.hp / (2 + (you.mutation[MUT_TORMENT_RESISTANCE] > 1)) - 1, false );
        }

        return (1);
    }

    // check for monster in cell
    int mon = mgrd[x][y];

    if (mon == NON_MONSTER)
        return (0);

    struct monsters *monster = &menv[mon];

    if (monster->type == -1 || mons_holiness( monster ) != MH_NATURAL)
        return (0);

    monster->hit_points = 1 + (monster->hit_points / 2);
    mon_msg( monster, " convulses!" );

    return (1);
}

void torment( int tx, int ty )
{
    apply_area_within_radius( torment_monsters, tx, ty, 0, 8, 0 );
}                               // end torment()

static int dispel_undead_grid( int x, int y, int pow, int caster_id )
{
    // is player?
    if (x == you.x_pos && y == you.y_pos)
    {
        if (!you.is_undead)
            canned_msg( MSG_YOU_RESIST );
        else
        {
            mpr( "Your body is wracked with pain!" );
            ouch( roll_dice( 3, 7 + pow / 6 ), caster_id, KILLED_BY_WILD_MAGIC,
                    "being dispelled" );
        }

        return (1);
    }

    // check for monster in cell
    int mon = mgrd[x][y];

    if (mon == NON_MONSTER)
        return (0);

    struct monsters *const monster = &menv[mon];

    if (monster->type == -1 || mons_holiness( monster ) != MH_UNDEAD)
        return (0);

    mon_msg( monster, " convulses!" );

    const killer_type killer = (caster_id == MHITYOU) ? KILL_YOU :
                               (caster_id == MHITNOT) ? KILL_MISC
                                                      : KILL_MON;

    hurt_monster_to_kill( monster, roll_dice( 3, 7 + pow / 6 ), killer, caster_id );

    return (1);
}

void dispel_undead_area( int tx, int ty, int pow, int caster_id )
{
    apply_area_within_radius( dispel_undead_grid, tx, ty, pow, 8, caster_id );
}                               // end torment()

void banished( unsigned char gate_type )
{
    if (gate_type == DNGN_ENTER_ABYSS)
    {
        if (you.level_type == LEVEL_ABYSS)
        {
            mpr( "You feel trapped." );
            abyss_teleport( true );
            return;
        }

        mpr( "You are cast into the Abyss!" );
        more();
    }

    down_stairs( gate_type, you.depth, false );
    untag_followers(); // safety
}                               // end banished()

int forget_spells(void)
{
    int ret = 0;

    if (!you.spell_no)
        return (0);

    do
    {
        // find a random spell to forget:
        int slot = -1;
        int num  = 0;

        for (int i = 0; i < 25; i++)
        {
            if (you.spells[i] != SPELL_NO_SPELL)
            {
                num++;
                if (one_chance_in( num ))
                    slot = i;
            }
        }

        if (slot == -1)              // should never happen though
            break;

        del_spell_from_memory_by_slot( slot );
        ret++;
    }
    while (you.spell_no && !one_chance_in(3));

    if (ret)
    {
        mpr( "You have forgotten %s spell%s!",
              (ret > 1) ? "some" : "a", (ret > 1) ? "s" : "" );
    }

    return (ret);
}                               // end forget_spells()

// Returns a random stat based on the weights.  If all are zero, it
// returns STAT_RANDOM, which gets processed specially by lose_stat
// and restore_stat.
stat_type random_stat( unsigned int str_weight, unsigned int int_weight, unsigned int dex_weight )
{
    ASSERT( str_weight + int_weight + dex_weight > 0 );

    stat_type  ret = STAT_RANDOM;

    const unsigned int total = str_weight + int_weight + dex_weight;
    const unsigned int roll = random2( total );

    if (roll < str_weight)
        ret = STAT_STRENGTH;
    else if (roll < str_weight + int_weight)
        ret = STAT_INTELLIGENCE;
    else if (total)             // must have one > 0, else return STAT_RANDOM
        ret = STAT_DEXTERITY;

    return (ret);
}

// use player::decrease_stats() instead iff:
// (a) player_sust_abil() should not factor in; and
// (b) there is no floor to the final stat values {dlb}
bool lose_stat( stat_type which_stat, int stat_loss, bool force )
{
    bool statLowered = false;   // must initialize to false {dlb}
    char *ptr_stat = 0;         // NULL {dlb}
    char newValue = 0;          // holds new value, for comparison to old {dlb}
    unsigned long redraw_flag = 0;  // REDRAW_ flag to set -- bwr

    // begin outputing message: {dlb}
    strcpy(info, "You feel ");

    // set pointers to appropriate variables: {dlb}
    if (which_stat == STAT_RANDOM)
        which_stat = static_cast< stat_type >( random2(NUM_STATS) );

    ASSERT( which_stat < NUM_STATS );

    switch (which_stat)
    {
    case STAT_STRENGTH:
        strcat(info, "weakened");
        ptr_stat = &you.str;
        redraw_flag = REDRAW_STRENGTH;
        break;

    case STAT_DEXTERITY:
        strcat(info, "clumsy");
        ptr_stat = &you.dex;
        redraw_flag = REDRAW_DEXTERITY;
        break;

    case STAT_INTELLIGENCE:
        strcat(info, "dopey");
        ptr_stat = &you.intel;
        redraw_flag = REDRAW_INTELLIGENCE;
        break;

    default:
        // shouldn't happen
        return (false);
    }

    if (!force && player_sust_abil())
        stat_loss = 0;  // Used to divide by 2^(num levels).

    // newValue is current value less modifier: {dlb}
    newValue = *ptr_stat - stat_loss;

    // XXX: Death by stat loss is currently handled in the redraw code. -- bwr
    if (newValue < 0)
        newValue = 0;

    // conceivable that stat was already *at* three
    // or stat_loss zeroed by player_sust_abil(): {dlb}
    //
    // Actually, that code was somewhat flawed.  Several race-class combos
    // can start with a stat lower than three, and this block (which
    // used to say '!=' would actually cause stat gain with the '< 3'
    // check that used to be above.  Crawl has stat-death code and I
    // don't see why we shouldn't be using it here.  -- bwr
    if (newValue < *ptr_stat)
    {
        *ptr_stat = newValue;
        set_redraw_status( redraw_flag );

        // handle burden change, where appropriate: {dlb}
        if (ptr_stat == &you.str)
            burden_change();

        statLowered = true;  // that is, stat was lowered (not just changed)
    }

    // a warning to player that s/he cut it close: {dlb}
    if (!statLowered)
        strcat(info, " for a moment");

    // finish outputting message: {dlb}
    strcat(info, ".");
    mpr(info);

    return (statLowered);
}                               // end lose_stat()

bool restore_stat( stat_type which_stat, bool suppress_msg )
{
    bool statRestored = false;

    // a bit hackish, but cut me some slack, man! --
    // besides, a little recursion never hurt anyone {dlb}:
    if (which_stat == STAT_ALL)
    {
        for (int i = STAT_STRENGTH; i < NUM_STATS; i++)
        {
            statRestored |= restore_stat( static_cast< stat_type >(i), suppress_msg );
        }

        return (statRestored);                 // early return {dlb}
    }

    // the real function begins here {dlb}:
    char *ptr_stat = 0;         // NULL {dlb}
    char *ptr_stat_max = 0;     // NULL {dlb}

    unsigned long redraw_flag = 0;

    if (!suppress_msg)
        strcpy(info, "You feel your ");

    if (which_stat == STAT_RANDOM)
        which_stat = static_cast< stat_type >( random2(NUM_STATS) );

    ASSERT( which_stat < NUM_STATS );

    switch (which_stat)
    {
    case STAT_STRENGTH:
        if (!suppress_msg)
            strcat(info, "strength");

        ptr_stat = &you.str;
        ptr_stat_max = &you.max_str;
        redraw_flag = REDRAW_STRENGTH;
        break;

    case STAT_DEXTERITY:
        if (!suppress_msg)
            strcat(info, "dexterity");

        ptr_stat = &you.dex;
        ptr_stat_max = &you.max_dex;
        redraw_flag = REDRAW_DEXTERITY;
        break;

    case STAT_INTELLIGENCE:
        if (!suppress_msg)
            strcat(info, "intelligence");

        ptr_stat = &you.intel;
        ptr_stat_max = &you.max_intel;
        redraw_flag = REDRAW_INTELLIGENCE;
        break;

    default:
        // shouldn't happen
        return (false);
    }

    if (*ptr_stat < *ptr_stat_max)
    {
        if (!suppress_msg)
        {
            strcat(info, " returning.");
            mpr(info);
        }

        *ptr_stat = *ptr_stat_max;
        set_redraw_status( redraw_flag );
        statRestored = true;

        if (ptr_stat == &you.str)
            burden_change();
    }

    return (statRestored);
}                               // end restore_stat()

void modify_stat( stat_type which_stat, int amount, bool suppress_msg )
{
    char *ptr_stat = NULL;
    char *ptr_stat_max = NULL;
    const char *adj = "buggy";

    unsigned long redraw_flag = 0;

    // sanity - is non-zero amount?
    if (amount == 0)
        return;

    // Stop running/travel if a stat drops.
    if (amount < 0)
        interrupt_activity( AI_STAT_CHANGE );

    if (which_stat == STAT_RANDOM)
        which_stat = static_cast< stat_type >( random2(NUM_STATS) );

    ASSERT( which_stat < NUM_STATS );

    switch (which_stat)
    {
    case STAT_STRENGTH:
        ptr_stat = &you.str;
        ptr_stat_max = &you.max_str;
        redraw_flag = REDRAW_STRENGTH;
        set_redraw_status( REDRAW_ARMOUR_CLASS );
        set_redraw_status( REDRAW_EVASION );
        adj = ((amount > 0) ? "stronger" : "weaker");
        break;

    case STAT_DEXTERITY:
        ptr_stat = &you.dex;
        ptr_stat_max = &you.max_dex;
        redraw_flag = REDRAW_DEXTERITY;
        set_redraw_status( REDRAW_ARMOUR_CLASS );
        set_redraw_status( REDRAW_EVASION );
        adj = ((amount > 0) ? "agile" : "clumsy");
        break;

    case STAT_INTELLIGENCE:
        ptr_stat = &you.intel;
        ptr_stat_max = &you.max_intel;
        redraw_flag = REDRAW_INTELLIGENCE;
        adj = ((amount > 0) ? "clever" : "stupid");
        break;

    default:
        break;
    }

    if (!suppress_msg)
    {
        mpr( ((amount > 0) ? MSGCH_INTRINSIC_GAIN : MSGCH_WARN),
             "You feel %s.", adj );
    }

    *ptr_stat += amount;
    *ptr_stat_max += amount;
    set_redraw_status( redraw_flag );

    if (ptr_stat == &you.dex)
        set_redraw_status( REDRAW_SKILL );
    else if (ptr_stat == &you.str)
    {
        burden_change();
        set_redraw_status( REDRAW_SKILL );
    }

    return;
}                               // end modify_stat()

void direct_effect( struct bolt &pbolt )
{
    int damage = 0;

    switch (pbolt.type)
    {
    case DMNBM_HELLFIRE:
        mpr( "You are engulfed in a burst of hellfire!" );
        strcpy( pbolt.name, "hellfire" );
        pbolt.ex_size = 1;
        pbolt.flavour = BEAM_HELLFIRE;
        pbolt.type = SYM_ZAP;
        pbolt.colour = RED;
        pbolt.thrower = KILL_MON_MISSILE;
        pbolt.aux_source = NULL;
        pbolt.is_explosion = true;
        pbolt.is_beam = false;
        pbolt.is_enchant = false;
        pbolt.is_tracer = false;
        pbolt.damage = dice_def( 3, pbolt.ench_power / 8 );
        pbolt.aux_source = "burst of hellfire";
        explosion( pbolt );
        break;

    case DMNBM_SMITING:
        zap_animation( element_colour( EC_DARK ) );
        mpr( "Something smites you!" );
        strcpy( pbolt.name, "smiting" );    // for ouch
        pbolt.aux_source = "by divine providence";
        damage = 5 + roll_dice( 2, 2 + pbolt.ench_power / 12 );
        break;

    case DMNBM_BRAIN_FEED:
        // lose_stat() must come last {dlb}
        zap_animation( element_colour( EC_MAGIC ) );
        if (one_chance_in(3) && lose_stat(STAT_INTELLIGENCE, 1))
            mpr("Something feeds on your intellect!");
        else
            mpr("Something tries to feed on your intellect!");
        break;

    case DMNBM_MUTATION:
        zap_animation( element_colour( EC_MUTAGENIC ) );
        mpr("Strange energies course through your body.");
        if (one_chance_in(5))
            mutate( PICK_RANDOM_MUTATION );
        else
            give_bad_mutation();
        break;
    }

    // apply damage and handle death, where appropriate {dlb}
    if (damage > 0)
        ouch( damage, pbolt.beam_source, KILLED_BY_BEAM, pbolt.aux_source );

    return;
}                               // end direct_effect()

// monster-to-monster
void mons_direct_effect( struct bolt &pbolt, int i )
{
    // note the translation here - important {dlb}
    int o = menv[i].foe;
    struct monsters *monster = &menv[o];
    int damage = 0;
    const int holy = mons_holiness( monster );
    const int intel = mons_intel( monster->type );

    // annoy the target
    behaviour_event( monster, ME_SHOT, i );

    switch (pbolt.type)
    {
    case DMNBM_HELLFIRE:
        mon_msg(monster, " is engulfed in hellfire.");

        strcpy( pbolt.name, "hellfire" );
        pbolt.ex_size = 1;
        pbolt.flavour = BEAM_HELLFIRE;
        pbolt.type = SYM_ZAP;
        pbolt.colour = RED;
        pbolt.thrower = KILL_MON_MISSILE;
        pbolt.aux_source = NULL;
        pbolt.is_explosion = true;
        pbolt.is_beam = false;
        pbolt.is_enchant = false;
        pbolt.is_tracer = false;
        pbolt.damage = dice_def( 3, pbolt.ench_power / 8 );
        pbolt.aux_source = "burst of hellfire";
        explosion( pbolt );
        break;

    case DMNBM_SMITING:
        zap_animation( element_colour( EC_DARK ), monster );
        mon_msg(monster, " is smitten.");

        strcpy( pbolt.name, "smiting" );
        pbolt.flavour = BEAM_MISSILE;

        damage += 5 + roll_dice( 2, 2 + pbolt.ench_power / 12 );
        break;

    case DMNBM_BRAIN_FEED:
        if (intel != I_PLANT)
        {
            zap_animation( element_colour( EC_MAGIC ), monster );
            mon_msg(monster, " appears befuddled for a moment.");

            if ((intel < I_NORMAL && coinflip()) || one_chance_in(3))
            {
                monster->hit_dice--;
                if (monster->hit_dice < 1)
                {
                    monster->hit_points = 0;
                    monster_die( monster, KILL_MON_MISSILE, i );
                }
            }
        }
        break;

    case DMNBM_MUTATION:
        zap_animation( element_colour( EC_MUTAGENIC ), monster );

        if (mons_is_animated_undead( monster->type, true ))
        {
            mon_msg( monster, " decomposes." );
            damage = 2 + random2(5);
        }
        else if (holy != MH_NATURAL)
            mon_msg(monster, " is unaffected.");
        else if (check_mons_resist_magic( monster, pbolt.ench_power ))
            mon_msg(monster, " resists.");
        else
            monster_polymorph( monster, RANDOM_MONSTER, 100 );
        break;
    }

    // apply damage and handle death, where appropriate {dlb}
    if (hurt_monster( monster, damage ))
    {
        if (monster->hit_points < 1)
            monster_die( monster, KILL_MON_MISSILE, i );
    }

    return;
}                               // end mons_direct_effect()

void random_uselessness( int scroll_id )
{
    char wc[30];
    int temp_rand = 0;          // probability determination {dlb}

    switch (random2(9))
    {
    case 0:
        strcpy(info, "The dust glows a ");
        weird_colours(random2(256), wc);
        strcat(info, wc);
        strcat(info, " colour!");
        mpr(info);
        break;

    case 1:
        if (scroll_id != -1)
        {
            mpr("The scroll reassembles itself in your hand!");
            inc_inv_item_quantity( scroll_id, 1 );
            break;
        }
        // intentional fallthrough for non-scrolls

    case 2:
        if (you.equip[EQ_WEAPON] != -1)
        {
            char str_pass[ ITEMNAME_SIZE ];
            in_name(you.equip[EQ_WEAPON], DESC_CAP_YOUR, str_pass);
            strcpy(info, str_pass);
            strcat(info, " glows ");
            weird_colours(random2(256), wc);
            strcat(info, wc);
            strcat(info, " for a moment.");
            mpr(info);
        }
        else
        {
            canned_msg( MSG_NOTHING_HAPPENS );
        }
        break;

    case 3:
        strcpy(info, "You hear the distant roaring of an enraged ");

        temp_rand = random2(8);

        strcat(info, (temp_rand == 0) ? "frog"          :
                     (temp_rand == 1) ? "pill bug"      :
                     (temp_rand == 2) ? "millipede"     :
                     (temp_rand == 3) ? "eggplant"      :
                     (temp_rand == 4) ? "albino dragon" :
                     (temp_rand == 5) ? "dragon"        :
                     (temp_rand == 6) ? "human"
                                      : "slug");

        strcat(info, "!");
        mpr(info);
        break;

    case 4:
        if (player_can_smell())
        {
            strcpy(info, "You smell ");

            temp_rand = random2(8);

            strcat(info, (temp_rand == 0) ? "coffee."          :
                         (temp_rand == 1) ? "salt."            :
                         (temp_rand == 2) ? "burning hair!"    :
                         (temp_rand == 3) ? "baking bread."    :
                         (temp_rand == 4) ? "something weird." :
                         (temp_rand == 5) ? "wet wool."        :
                         (temp_rand == 6) ? "sulphur."
                                          : "fire and brimstone!");
            mpr(info);
        }
        else
        {
            canned_msg( MSG_NOTHING_HAPPENS );
        }
        break;

    case 5:
        mpr("You experience a momentary feeling of inescapable doom!");
        break;

    case 6:
        strcpy(info, "Your ");

        temp_rand = random2(3);

        strcat(info, (temp_rand == 0) ? "ears itch."   :
                     (temp_rand == 1) ? "brain hurts!"
                                      : "nose twitches suddenly!");
        mpr(info);
        break;

    case 7:
        mpr("You hear the tinkle of a tiny bell.");
        cast_summon_butterflies( 100 );
        break;

    case 8:
        strcpy(info, "You hear ");

        temp_rand = random2(9);

        strcat(info, (temp_rand == 0) ? "snatches of song"                 :
                     (temp_rand == 1) ? "a voice call someone else's name" :
                     (temp_rand == 2) ? "a very strange noise"             :
                     (temp_rand == 3) ? "roaring flame"                    :
                     (temp_rand == 4) ? "a very strange noise indeed"      :
                     (temp_rand == 5) ? "the chiming of a distant gong"    :
                     (temp_rand == 6) ? "the bellowing of a yak"           :
                     (temp_rand == 7) ? "a crunching sound"
                                      : "the tinkle of an enormous bell");
        strcat( info, "." );
        mpr( info );
        break;
    }

    return;
}                               // end random_uselessness()

bool acquirement( unsigned char force_class, bool god_gift )
{
    int thing_created = 0;
    int iteration = 0;

    // Remember lava!
    unsigned char class_wanted = OBJ_RANDOM;
    unsigned char type_wanted = OBJ_RANDOM;

    unsigned char unique = 1;
    unsigned char acqc = 0;
    int i;

    const int max_has_value = 100;
    FixedVector< int, max_has_value > already_has;

    int best_spell = 99;
    int best_any = 99;
    unsigned char keyin;

    for (acqc = 0; acqc < max_has_value; acqc++)
        already_has[acqc] = 0;

    int spell_skills = 0;
    for (i = SK_SPELLCASTING; i <= SK_POISON_MAGIC; i++)
        spell_skills += you.skills[i];

    if (force_class != OBJ_RANDOM)
        class_wanted = force_class;
    else
    {
        mpr( "This is a scroll of acquirement!" );

      query:
        mpr( "[a|A] Weapon  [b|B] Armour  [c|C] Jewellery      [d|D] Book" );
        mpr( "[e|E] Staff   [f|F] Food    [g|G] Miscellaneous  [h|H] Gold" );

        //mpr("[r|R] - Just give me something good.");
        mpr( MSGCH_PROMPT,"What kind of item would you like to acquire? " );

        keyin = tolower( get_ch() );

        if (keyin == 'a')
            class_wanted = OBJ_WEAPONS;
        else if (keyin == 'b')
            class_wanted = OBJ_ARMOUR;
        else if (keyin == 'c')
            class_wanted = OBJ_JEWELLERY;
        else if (keyin == 'd')
            class_wanted = OBJ_BOOKS;
        else if (keyin == 'e')
            class_wanted = OBJ_STAVES;
        else if (keyin == 'f')
            class_wanted = OBJ_FOOD;
        else if (keyin == 'g')
            class_wanted = OBJ_MISCELLANY;
        else if (keyin == 'h')
            class_wanted = OBJ_GOLD;
    }

    for (acqc = 0; acqc < ENDOFPACK; acqc++)
    {
        if (is_valid_item( you.inv[acqc] )
            && you.inv[acqc].base_type == class_wanted)
        {
            ASSERT( you.inv[acqc].sub_type < max_has_value );
            already_has[you.inv[acqc].sub_type] += you.inv[acqc].quantity;
        }
    }

    if (class_wanted == OBJ_FOOD)
    {
        // food is a little less predicatable now -- bwr

        if (you.species == SP_GHOUL)
        {
            type_wanted = one_chance_in(10) ? FOOD_ROYAL_JELLY
                                            : FOOD_CHUNK;
        }
        else
        {
            // Meat is better than bread (except for herbivors), and
            // by choosing it as the default we don't have to worry
            // about special cases for carnivorous races (ie kobold)
            type_wanted = FOOD_MEAT_RATION;

            if (you.mutation[MUT_HERBIVOROUS])
                type_wanted = FOOD_BREAD_RATION;

            // If we have some regular rations, then we're probably be more
            // interested in faster foods (escpecially royal jelly)...
            // otherwise the regular rations should be a good enough offer.
            if (already_has[FOOD_MEAT_RATION]
                    + already_has[FOOD_BREAD_RATION] >= 2 || coinflip())
            {
                type_wanted = one_chance_in(5) ? FOOD_HONEYCOMB
                                               : FOOD_ROYAL_JELLY;
            }
        }

        // quantity is handled by unique for food
        unique = 3 + random2(5);

        // giving more of the lower food value items
        if (type_wanted == FOOD_HONEYCOMB || type_wanted == FOOD_CHUNK)
            unique += random2(5);
    }
    else if (class_wanted == OBJ_WEAPONS)
    {
        int count = 0;
        int skill = SK_FIGHTING;

        for (i = SK_SHORT_BLADES; i <= SK_DARTS; i++)
        {
            if (i == SK_UNUSED_1)
                continue;

            // Adding a small constant allows for the occasional
            // weapon in an untrained skill.  Since the game
            // doesn't allow for much in the way of good launchers,
            // we'll lower their weight.
            int weight = 0;

            if (i < SK_SLINGS)
                weight = you.skills[i] + 3;
            else
                weight = (you.skills[i] + 1) / 2;

            if (weight)
            {
                count += weight;
                if (random2(count) < weight)
                    skill = i;
            }
        }

        count = 0;

        // skipping clubs, knives, blowguns
        for (i = WPN_MACE; i < NUM_WEAPONS; i++)
        {
            // skipping giant clubs
            if (i == WPN_GIANT_CLUB)
                i = WPN_EVENINGSTAR;

            // skipping knife and blowgun
            if (i == WPN_KNIFE)
                i = WPN_FALCHION;

            // blessed blades can only be created by the player, never found
            if (i == WPN_BLESSED_BLADE)
                continue;

            if ((i == WPN_DEMON_TRIDENT
                    || i == WPN_DEMON_WHIP
                    || i == WPN_DEMON_BLADE)
                && (you.religion == GOD_SHINING_ONE
                    || you.religion == GOD_ZIN
                    || you.religion == GOD_ELYVILON))
            {
                continue;
            }

            int wpn_skill;

            if (is_range_weapon_type( static_cast<weapon_type>(i) ))
                wpn_skill = range_skill( OBJ_WEAPONS, i );
            else
                wpn_skill = weapon_skill( OBJ_WEAPONS, i );

            // "rare" weapons are only considered some of the time...
            // still, the chance is higher than actual random creation.
            if (wpn_skill == skill && random2(100) < 25 + 15 * weapon_rarity(i))
            {
                count++;
                if (one_chance_in( count ))
                    type_wanted = i;
            }
        }
    }
    else if (class_wanted == OBJ_MISSILES)
    {
        int count = 0;
        int skill = SK_RANGED_COMBAT;

        for (i = SK_SLINGS; i <= SK_DARTS; i++)
        {
            if (you.skills[i])
            {
                count += you.skills[i];
                if (random2(count) < you.skills[i])
                    skill = i;
            }
        }

        switch (skill)
        {
        case SK_SLINGS:
            type_wanted = MI_STONE;
            break;

        case SK_BOWS:
            type_wanted = MI_ARROW;
            break;

        case SK_CROSSBOWS:
            type_wanted = MI_DART;
            for (i = 0; i < ENDOFPACK; i++)
            {
                // Assuming that crossbow in inventory means that they
                // want bolts for it (not darts for a hand crossbow)...
                // perhaps we should check for both and compare ammo
                // amounts on hand?
                if (is_valid_item( you.inv[i] )
                    && you.inv[i].base_type == OBJ_WEAPONS
                    && you.inv[i].sub_type == WPN_CROSSBOW)
                {
                    type_wanted = MI_BOLT;
                    break;
                }
            }
            break;

        case SK_DARTS:
            type_wanted = MI_DART;
            for (i = 0; i < ENDOFPACK; i++)
            {
                if (is_valid_item( you.inv[i] )
                    && you.inv[i].base_type == OBJ_WEAPONS
                    && you.inv[i].sub_type == WPN_BLOWGUN)
                {
                    // Assuming that blowgun in inventory means that they
                    // may want needles for it (but darts might also be
                    // wanted).  Maybe expand this... see above comment.
                    if (coinflip())
                        type_wanted = MI_NEEDLE;
                    break;
                }
            }
            break;

        default:
            type_wanted = MI_DART;
            break;
        }
    }
    else if (class_wanted == OBJ_ARMOUR)
    {
        // Increasing the representation of the non-body armour
        // slots here to make up for the fact that there's one
        // one type of item for most of them. -- bwr
        //
        // OBJ_RANDOM is body armour and handled below
        type_wanted = (coinflip()) ? OBJ_RANDOM : ARM_SHIELD + random2(5);

        // mutation specific problems (horns allow caps)
        if ((you.mutation[MUT_HOOVES] >= 2 && type_wanted == ARM_BOOTS)
            || (you.mutation[MUT_CLAWS] >= 3 && type_wanted == ARM_GLOVES))
        {
            type_wanted = OBJ_RANDOM;
        }

        // some species specific fitting problems
        switch (you.species)
        {
        case SP_OGRE:
        case SP_OGRE_MAGE:
        case SP_TROLL:
        case SP_SPRIGGAN:
            if (type_wanted == ARM_GLOVES
                || type_wanted == ARM_BOOTS
                || type_wanted == OBJ_RANDOM)
            {
                type_wanted = (coinflip() ? ARM_ROBE : ARM_SHIELD);
            }

            if (type_wanted == ARM_SHIELD)
            {
                if (you.species == SP_SPRIGGAN)
                    type_wanted = ARM_BUCKLER;
                else if (one_chance_in(3)) // giant races
                    type_wanted = ARM_LARGE_SHIELD;
            }
            break;

        case SP_KENKU:
            if (type_wanted == ARM_BOOTS)
                type_wanted = OBJ_RANDOM;
            break;

        default:
            if (player_genus(GENPC_DRACONIAN))
            {
                if (type_wanted == ARM_GLOVES
                    || type_wanted == ARM_BOOTS
                    || type_wanted == OBJ_RANDOM)
                {
                    type_wanted = (coinflip() ? ARM_ROBE : ARM_SHIELD);
                }
            }
            break;
        }

        // Now we'll randomly pick a body armour (light only in the
        // case of ARM_ROBE).  Unlike before, now we're only giving
        // out the finished products here, never the hides.  -- bwr
        if (type_wanted == OBJ_RANDOM || type_wanted == ARM_ROBE)
        {
            // start with normal base armour
            if (type_wanted == ARM_ROBE)
                type_wanted = (!one_chance_in(10) ? ARM_ROBE : ARM_ANIMAL_SKIN);
            else
            {
                type_wanted = ARM_ROBE + random2(8);

                if (one_chance_in(10) && you.skills[SK_ARMOUR] >= 10)
                    type_wanted = ARM_CRYSTAL_PLATE_MAIL;

                if (one_chance_in(10))
                    type_wanted = ARM_ANIMAL_SKIN;
            }

            // everyone can wear things made from hides
            if (one_chance_in(20))
            {
                int rnd = random2(20);

                type_wanted = (rnd <  4) ? ARM_TROLL_LEATHER_ARMOUR  :  // 20%
                              (rnd <  8) ? ARM_STEAM_DRAGON_ARMOUR   :  // 20%
                              (rnd < 11) ? ARM_MOTTLED_DRAGON_ARMOUR :  // 15%
                              (rnd < 14) ? ARM_SWAMP_DRAGON_ARMOUR   :  // 15%
                              (rnd < 16) ? ARM_DRAGON_ARMOUR         :  // 10%
                              (rnd < 18) ? ARM_ICE_DRAGON_ARMOUR     :  // 10%
                              (rnd < 19) ? ARM_STORM_DRAGON_ARMOUR      //  5%
                                         : ARM_GOLD_DRAGON_ARMOUR;      //  5%
            }
        }
    }
    else if (class_wanted != OBJ_GOLD)
    {
        do
        {
            switch (class_wanted)
            {
            case OBJ_JEWELLERY:
                // Try for a base type the player hasn't identified
                for (i = 0; i < 10; i++)
                {
                    type_wanted = random2(24);

                    if (one_chance_in(3))
                        type_wanted = AMU_RAGE + random2(10);

                    if (get_ident_type(OBJ_JEWELLERY, type_wanted) == ID_UNKNOWN_TYPE)
                    {
                        break;
                    }
                }
                break;

            case OBJ_BOOKS:
                // remember, put rarer books higher in the list
                iteration = 1;
                type_wanted = NUM_BOOKS;

                best_spell = best_skill( SK_SPELLCASTING, (NUM_SKILLS - 1),
                                         best_spell );

              which_book:
#if DEBUG_DIAGNOSTICS
                mpr( MSGCH_DIAGNOSTICS,
                      "acquirement: iteration = %d, best_spell = %d",
                      iteration, best_spell );
#endif //jmf: debugging

                switch (best_spell)
                {
                default:
                case SK_SPELLCASTING:
                    if (you.skills[SK_SPELLCASTING] <= 3
                        && !you.had_book[BOOK_CANTRIPS])
                    {
                        // Handful of level one spells, very useful for the
                        // new spellcaster who's asking for a book -- bwr
                        type_wanted = BOOK_CANTRIPS;
                    }
                    else if (you.skills[SK_SPELLCASTING] <= 5
                            && !you.had_book[BOOK_MINOR_MAGIC_I])
                    {
                        type_wanted = BOOK_MINOR_MAGIC_I + random2(3);
                    }
                    else if (!you.had_book[BOOK_WIZARDRY])
                        type_wanted = BOOK_WIZARDRY;
                    else if (!you.had_book[BOOK_ABJURATION])
                        type_wanted = BOOK_ABJURATION;
                    else if (!you.had_book[BOOK_POWER])
                        type_wanted = BOOK_POWER;
                    break;

                case SK_POISON_MAGIC:
                    if (!you.had_book[BOOK_YOUNG_POISONERS])
                        type_wanted = BOOK_YOUNG_POISONERS;
                    else if (!you.had_book[BOOK_ENVENOMATIONS])
                        type_wanted = BOOK_ENVENOMATIONS;
                    break;

                case SK_EARTH_MAGIC:
                    if (!you.had_book[BOOK_GEOMANCY])
                        type_wanted = BOOK_GEOMANCY;
                    else if (!you.had_book[BOOK_EARTH])
                        type_wanted = BOOK_EARTH;
                    break;

                case SK_AIR_MAGIC:
                    // removed the book of clouds... all the other elements
                    // (and most other spell skills) only get two.
                    if (!you.had_book[BOOK_AIR])
                        type_wanted = BOOK_AIR;
                    else if (!you.had_book[BOOK_SKY])
                        type_wanted = BOOK_SKY;
                    break;

                case SK_ICE_MAGIC:
                    if (!you.had_book[BOOK_FROST])
                        type_wanted = BOOK_FROST;
                    else if (!you.had_book[BOOK_ICE])
                        type_wanted = BOOK_ICE;
                    break;

                case SK_FIRE_MAGIC:
                    if (!you.had_book[BOOK_FLAMES])
                        type_wanted = BOOK_FLAMES;
                    else if (!you.had_book[BOOK_FIRE])
                        type_wanted = BOOK_FIRE;
                    break;

                case SK_SUMMONINGS:
                    if (!you.had_book[BOOK_CALLINGS])
                        type_wanted = BOOK_CALLINGS;
                    else if (!you.had_book[BOOK_SUMMONINGS])
                        type_wanted = BOOK_SUMMONINGS;

                    // now a Vehumet special -- bwr
                    // else if (!you.had_book[BOOK_DEMONOLOGY])
                    //     type_wanted = BOOK_DEMONOLOGY;
                    break;

                case SK_ENCHANTMENTS:
                    best_any = best_skill( SK_FIGHTING, (NUM_SKILLS - 1) );

                    // So many enchantment books!  I really can't feel
                    // guilty at all for dividing out the fighting
                    // books and forcing the player to raise a fighting
                    // skill (or enchantments in the case of Crusaders)
                    // to get the remaining books... enchantments are
                    // much too good (most spells, lots of books here,
                    // id wand charges, gives magic resistance),
                    // something will eventually have to be done.  -- bwr
                    if (best_any >= SK_FIGHTING
                                && best_any <= SK_STAVES)
                    {
                        // Fighter mage's get the fighting enchantment books
                        if (!you.had_book[BOOK_WAR_CHANTS])
                            type_wanted = BOOK_WAR_CHANTS;
                        else if (!you.had_book[BOOK_TUKIMA])
                            type_wanted = BOOK_TUKIMA;
                    }
                    else if (!you.had_book[BOOK_CHARMS])
                        type_wanted = BOOK_CHARMS;
                    else if (!you.had_book[BOOK_HINDERANCE])
                        type_wanted = BOOK_HINDERANCE;
                    else if (!you.had_book[BOOK_CONTROL])
                        type_wanted = BOOK_CONTROL;
                    else if (!you.had_book[BOOK_ENCHANTMENTS])
                        type_wanted = BOOK_ENCHANTMENTS;
                    break;

                case SK_CONJURATIONS:
                    if (!you.had_book[BOOK_CONJURATIONS_I])
                        type_wanted = give_first_conjuration_book();
                    else if (!you.had_book[BOOK_TEMPESTS])
                        type_wanted = BOOK_TEMPESTS;

                    // now a Vehumet special -- bwr
                    // else if (!you.had_book[BOOK_ANNIHILATIONS])
                    //     type_wanted = BOOK_ANNIHILATIONS;
                    break;

                case SK_NECROMANCY:
                    if (!you.had_book[BOOK_NECROMANCY])
                        type_wanted = BOOK_NECROMANCY;
                    else if (!you.had_book[BOOK_DEATH])
                        type_wanted = BOOK_DEATH;
                    else if (!you.had_book[BOOK_UNLIFE])
                        type_wanted = BOOK_UNLIFE;

                    // now a Kikubaaqudgha special -- bwr
                    // else if (!you.had_book[BOOK_NECRONOMICON])
                    //    type_wanted = BOOK_NECRONOMICON;
                    break;

                case SK_TRANSLOCATIONS:
                    if (!you.had_book[BOOK_SPATIAL_TRANSLOCATIONS])
                        type_wanted = BOOK_SPATIAL_TRANSLOCATIONS;
                    else if (!you.had_book[BOOK_WARP])
                        type_wanted = BOOK_WARP;
                    break;

                case SK_TRANSMIGRATION:
                    if (!you.had_book[BOOK_CHANGES])
                        type_wanted = BOOK_CHANGES;
                    else if (!you.had_book[BOOK_TRANSFIGURATIONS])
                        type_wanted = BOOK_TRANSFIGURATIONS;
                    else if (!you.had_book[BOOK_MUTATIONS])
                        type_wanted = BOOK_MUTATIONS;
                    break;

                case SK_DIVINATIONS:    //jmf: added 24mar2000
                    if (!you.had_book[BOOK_SURVEYANCES])
                        type_wanted = BOOK_SURVEYANCES;
                    else if (!you.had_book[BOOK_DIVINATIONS])
                        type_wanted = BOOK_DIVINATIONS;
                    break;
                }
/*
                if (type_wanted == 99 && glof == best_skill( SK_SPELLCASTING, (NUM_SKILLS - 1) ))
*/
                if (type_wanted == NUM_BOOKS && iteration == 1)
                {
                    best_spell = best_skill( SK_SPELLCASTING, (NUM_SKILLS - 1),
                                             best_skill(SK_SPELLCASTING,
                                                        (NUM_SKILLS - 1)) );
                    iteration++;
                    goto which_book;
                }

                // if we don't have a book, try and get a new one.
                if (type_wanted == NUM_BOOKS)
                {
                    do
                    {
                        type_wanted = random2(NUM_BOOKS);
                        if (one_chance_in(500))
                            break;
                    }
                    while (you.had_book[type_wanted]);
                }

                // if the book is invalid find any valid one.
                while (book_rarity(type_wanted) == 100
                       || type_wanted == BOOK_DESTRUCTION
                       || type_wanted == BOOK_MANUAL)
                {
                    type_wanted = random2(NUM_BOOKS);
                }
                break;

            case OBJ_STAVES:
                type_wanted = random2(13);

                if (type_wanted >= 10)
                    type_wanted = STAFF_AIR + type_wanted - 10;

                // Elemental preferences -- bwr
                if (type_wanted == STAFF_FIRE || type_wanted == STAFF_COLD)
                {
                    if (you.skills[SK_FIRE_MAGIC] > you.skills[SK_ICE_MAGIC])
                        type_wanted = STAFF_FIRE;
                    else if (you.skills[SK_FIRE_MAGIC] != you.skills[SK_ICE_MAGIC])
                        type_wanted = STAFF_COLD;
                }
                else if (type_wanted == STAFF_AIR || type_wanted == STAFF_EARTH)
                {
                    if (you.skills[SK_AIR_MAGIC] > you.skills[SK_EARTH_MAGIC])
                        type_wanted = STAFF_AIR;
                    else if (you.skills[SK_AIR_MAGIC] != you.skills[SK_EARTH_MAGIC])
                        type_wanted = STAFF_EARTH;
                }

                best_spell = best_skill( SK_SPELLCASTING, (NUM_SKILLS - 1) );

                // If we're going to give out an enhancer stave,
                // we should at least bias things towards the
                // best spell skill. -- bwr
                switch (best_spell)
                {
                case SK_FIRE_MAGIC:
                    if (!already_has[STAFF_FIRE])
                        type_wanted = STAFF_FIRE;
                    break;

                case SK_ICE_MAGIC:
                    if (!already_has[STAFF_COLD])
                        type_wanted = STAFF_COLD;
                    break;

                case SK_AIR_MAGIC:
                    if (!already_has[STAFF_AIR])
                        type_wanted = STAFF_AIR;
                    break;

                case SK_EARTH_MAGIC:
                    if (!already_has[STAFF_EARTH])
                        type_wanted = STAFF_EARTH;
                    break;

                case SK_POISON_MAGIC:
                    if (!already_has[STAFF_POISON])
                        type_wanted = STAFF_POISON;
                    break;

                case SK_NECROMANCY:
                    if (!already_has[STAFF_DEATH])
                        type_wanted = STAFF_DEATH;
                    break;

                case SK_CONJURATIONS:
                    if (!already_has[STAFF_CONJURATION])
                        type_wanted = STAFF_CONJURATION;
                    break;

                case SK_ENCHANTMENTS:
                    if (!already_has[STAFF_ENCHANTMENT])
                        type_wanted = STAFF_ENCHANTMENT;
                    break;

                case SK_SUMMONINGS:
                    if (!already_has[STAFF_SUMMONING])
                        type_wanted = STAFF_SUMMONING;
                    break;

                case SK_EVOCATIONS:
                    if (!one_chance_in(3))
                        type_wanted = STAFF_SMITING + random2(9);
                    break;

                default: // invocations and leftover spell schools
                    switch (random2(5))
                    {
                    case 0:
                        type_wanted = STAFF_WIZARDRY;
                        break;

                    case 1:
                        type_wanted = STAFF_POWER;
                        break;

                    case 2:
                        type_wanted = STAFF_ENERGY;
                        break;

                    case 3:
                        type_wanted = STAFF_CHANNELING;
                        break;

                    case 4:
                        break;
                    }
                    break;
                }

                // Increased chance of getting spell staff for new or
                // non-spellcasters.  -- bwr
                if (one_chance_in(20)
                    || (spell_skills <= 1               // short on spells
                        && (type_wanted < STAFF_SMITING // already making one
                            || type_wanted >= STAFF_AIR)
                        && !one_chance_in(3)))
                {
                    int temp = random2(4);

                    type_wanted = (temp == 0) ? STAFF_SMITING :
                                  (temp == 1) ? STAFF_DISCOVERY :
                                  (temp == 2) ? STAFF_WARDING
                                              : STAFF_SMITING + random2(9);
                }
                break;

            case OBJ_MISCELLANY:
                do
                    type_wanted = random2(NUM_MISCELLANY);
                while (type_wanted == MISC_HORN_OF_GERYON
                    || type_wanted == MISC_RUNE_OF_ZOT
                    || type_wanted == MISC_PORTABLE_ALTAR_OF_NEMELEX
                    || type_wanted == MISC_CRYSTAL_BALL_OF_FIXATION
                    || type_wanted == MISC_EMPTY_EBONY_CASKET);
                break;

            default:
                mesclr();
                goto query;
            }

            ASSERT( type_wanted < max_has_value );
        }
        while (already_has[type_wanted] && !one_chance_in(200));
    }

    if (grid_destroys_items( grd[you.x_pos][you.y_pos] ))
    {
        mpr("You hear a splash.");      // how sad (and stupid)
    }
    else
    {
        // BCR - unique is now used for food quantity.
        thing_created = make_item( unique, class_wanted, type_wanted, true,
                                   MAKE_GOOD_ITEM, 250 );

        if (thing_created == NON_ITEM)
        {
            mpr("The demon of the infinite void smiles at you.");
            return (false);
        }

        if (god_gift)
        {
            do_uncurse_item( mitm[thing_created] );
            mitm[thing_created].flags |= ISFLAG_GOD_GIFT;
        }

        if (mitm[thing_created].base_type == OBJ_BOOKS)
        {
            if (mitm[thing_created].base_type == BOOK_MINOR_MAGIC_I
                || mitm[thing_created].base_type == BOOK_MINOR_MAGIC_II
                || mitm[thing_created].base_type == BOOK_MINOR_MAGIC_III)
            {
                you.had_book[ BOOK_MINOR_MAGIC_I ] = 1;
                you.had_book[ BOOK_MINOR_MAGIC_II ] = 1;
                you.had_book[ BOOK_MINOR_MAGIC_III ] = 1;
            }
            else if (mitm[thing_created].base_type == BOOK_CONJURATIONS_I
                || mitm[thing_created].base_type == BOOK_CONJURATIONS_II)
            {
                you.had_book[ BOOK_CONJURATIONS_I ] = 1;
                you.had_book[ BOOK_CONJURATIONS_II ] = 1;
            }
            else
            {
                you.had_book[ mitm[thing_created].sub_type ] = 1;
            }
        }
        else if (mitm[thing_created].base_type == OBJ_WEAPONS
                    && !is_fixed_artefact( mitm[thing_created] ))
        {
            // HACK: make unwieldable weapons wieldable
            // Note: messing with fixed artefacts is probably very bad.
            switch (you.species)
            {
            case SP_DEMONSPAWN:
            case SP_MUMMY:
            case SP_GHOUL:
                {
                    int brand = get_weapon_brand( mitm[thing_created] );
                    if (brand == SPWPN_HOLY_WRATH || brand == SPWPN_DISRUPTION)
                    {
                        if (is_random_artefact( mitm[thing_created] ))
                        {
                            // keep reseting seed until it's good:
                            for (; brand == SPWPN_HOLY_WRATH
                                      || brand == SPWPN_DISRUPTION;
                                  brand = get_weapon_brand(mitm[thing_created]))
                            {
                                make_item_randart( mitm[thing_created] );
                            }
                        }
                        else if (!is_unrandom_artefact( mitm[thing_created] )
                                && !is_fixed_artefact( mitm[thing_created] ))
                        {
                            set_item_ego_type( mitm[thing_created],
                                               OBJ_WEAPONS, SPWPN_VORPAL );
                        }
                    }
                }
                break;

            case SP_HALFLING:
            case SP_GNOME:
            case SP_KOBOLD:
            case SP_SPRIGGAN:
                switch (mitm[thing_created].sub_type)
                {
                case WPN_GREAT_SWORD:
                case WPN_TRIPLE_SWORD:
                    mitm[thing_created].sub_type =
                            (coinflip() ? WPN_FALCHION : WPN_LONG_SWORD);
                    break;

                case WPN_GREAT_MACE:
                    mitm[thing_created].sub_type = WPN_MACE;
                    break;
                case WPN_DIRE_FLAIL:
                    mitm[thing_created].sub_type = WPN_FLAIL;
                    break;

                case WPN_BATTLEAXE:
                case WPN_EXECUTIONERS_AXE:
                    mitm[thing_created].sub_type =
                            (coinflip() ? WPN_HAND_AXE : WPN_WAR_AXE);
                    break;

                case WPN_HALBERD:
                case WPN_GLAIVE:
                case WPN_SCYTHE:
                    mitm[thing_created].sub_type =
                            (coinflip() ? WPN_SPEAR : WPN_TRIDENT);
                    break;

                case WPN_QUARTERSTAFF:
                    mitm[thing_created].sub_type = WPN_STAFF;
                    break;
                }
                break;

            default:
                break;
            }
        }
        else if (mitm[thing_created].base_type == OBJ_ARMOUR
                    && !is_fixed_artefact( mitm[thing_created] ))
        {
            // HACK: make unwearable hats and boots wearable
            // Note: messing with fixed artefacts is probably very bad.
            switch (mitm[thing_created].sub_type)
            {
            case ARM_HELMET:
                if (you.species == SP_OGRE
                    || you.species == SP_OGRE_MAGE
                    || you.species == SP_TROLL
                    || you.species == SP_MINOTAUR
                    || you.species == SP_KENKU
                    || you.species == SP_SPRIGGAN
                    || you.mutation[MUT_HORNS])
                {
                    // turn it into a cap
                    mitm[thing_created].sub_type = ARM_CAP;
                    set_helmet_type( mitm[thing_created], THELM_CAP );
                    mitm[thing_created].colour = element_colour( EC_RANDOM );
                }
                break;

            case ARM_BOOTS:
                if (you.species == SP_NAGA)
                    mitm[thing_created].sub_type = ARM_NAGA_BARDING;
                else if (you.species == SP_CENTAUR)
                    mitm[thing_created].sub_type = ARM_CENTAUR_BARDING;

                // fix illegal barding ego types caused by above hack
                if (mitm[thing_created].sub_type != ARM_BOOTS
                    && get_armour_ego_type( mitm[thing_created] ) == SPARM_RUNNING)
                {
                    set_item_ego_type( mitm[thing_created], OBJ_ARMOUR, SPARM_NORMAL );
                }
                break;

            default:
                break;
            }
        }

        move_item_to_grid( &thing_created, you.x_pos, you.y_pos );

        // This should never actually be NON_ITEM because of the way
        // move_item_to_grid works (doesn't create a new item ever),
        // but we're checking it anyways. -- bwr
        if (thing_created != NON_ITEM)
            canned_msg(MSG_SOMETHING_APPEARS);
    }

    // Well, the item may have fallen in the drink, but the intent is
    // that acquirement happened. -- bwr
    return (true);
}                               // end acquirement()

bool recharge_wand(void)
{
    const int wand = get_inv_in_hand();

    if (wand == -1)
        return (false);

    if (you.inv[wand].base_type == OBJ_WANDS)
    {
        int charge_gain = 0;

        switch (you.inv[wand].sub_type)
        {
        case WAND_FIREBALL:
        case WAND_INVISIBILITY:
        case WAND_HEALING:
        case WAND_HASTING:
            charge_gain = 3;
            break;

        case WAND_TELEPORTATION:
        case WAND_LIGHTNING:
            charge_gain = 4;
            break;

        case WAND_FIRE:
        case WAND_COLD:
        case WAND_DRAINING:
            charge_gain = 5;
            break;

        default:
            charge_gain = 8;
            break;
        }

        char str_pass[ITEMNAME_SIZE];
        in_name( wand, DESC_CAP_YOUR, str_pass );
        strcpy( info, str_pass );
        strcat( info, " glows for a moment." );
        mpr(info);

        you.inv[wand].plus += 1 + roll_zdice( 3, charge_gain );

        if (you.inv[wand].plus > charge_gain * 3)
            you.inv[wand].plus = charge_gain * 3;

        set_redraw_status( REDRAW_WIELD );
        return (true);
    }
    else if (item_is_rod( you.inv[wand] ))
    {
        bool work = false;

        if (you.inv[wand].plus2 <= MAX_ROD_CHARGE * ROD_CHARGE_MULT)
        {
            you.inv[wand].plus2 += ROD_CHARGE_MULT;

            if (you.inv[wand].plus2 > MAX_ROD_CHARGE * ROD_CHARGE_MULT)
                you.inv[wand].plus2 = MAX_ROD_CHARGE * ROD_CHARGE_MULT;

            work = true;
        }

        if (you.inv[wand].plus < you.inv[wand].plus2)
        {
            you.inv[wand].plus = you.inv[wand].plus2;
            work = true;
        }

        if (work)
        {
            char str_pass[ITEMNAME_SIZE];
            in_name( wand, DESC_CAP_YOUR, str_pass );
            mpr( "%s glows for a moment.", str_pass );

            if (item_ident( you.inv[wand], ISFLAG_KNOW_PLUSES ))
                set_redraw_status( REDRAW_WIELD );

            return (true);
        }
    }

    return (false);
}                               // end recharge_wand()

void yell(void)
{
    bool targ_prev = false;
    int mons_targd = MHITNOT;
    struct dist targ;

    if (silenced(you.x_pos, you.y_pos))
    {
        mpr("You are unable to make a sound!");
        return;
    }

    mpr( MSGCH_PROMPT, "What do you say?" );
    mpr( " ! - Yell" );
    mpr( " a - Order allies to attack a monster" );

    if (!(you.prev_targ == MHITNOT || you.prev_targ == MHITYOU))
    {
        struct monsters *target = &menv[you.prev_targ];

        if (mons_near(target) && player_monster_visible(target))
        {
            mpr(" p - Order allies to attack your previous target");
            targ_prev = true;
        }
    }

    strcpy(info, " Anything else - Stay silent");

    if (one_chance_in(20))
        strcat(info, " (and be thought a fool)");

    mpr(info);

    unsigned char keyn = get_ch();

    switch (keyn)
    {
    case '!':
        mpr(MSGCH_SOUND,"You yell for attention!" );
        you.turn_is_over = true;
        noisy( SL_YELL, you.x_pos, you.y_pos );
        return;

    case 'a':
        mpr(MSGCH_PROMPT,"Gang up on whom?" );
        direction( targ, DIR_TARGET, TARG_ENEMY );

        if (targ.isCancel)
        {
            canned_msg(MSG_OK);
            return;
        }

        if (!targ.isValid || mgrd[targ.tx][targ.ty] == NON_MONSTER
            || !player_monster_visible( &menv[ mgrd[targ.tx][targ.ty] ] ))
        {
            mpr("Yeah, whatever.");
            return;
        }

        mons_targd = mgrd[targ.tx][targ.ty];
        break;

    case 'p':
        if (targ_prev)
        {
            mons_targd = you.prev_targ;
            break;
        }
        /* fall through... */
    default:
        mpr("Okely-dokely.");
        return;
    }

    you.pet_target = mons_targd;
    noisy( SL_YELL, you.x_pos, you.y_pos, "Attack!" );
}                               // end yell()

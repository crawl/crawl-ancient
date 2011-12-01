/*
 *  File:       spl-cast.cc
 *  Summary:    Spell casting and miscast functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <4>      1/02/00        jmf             changed values, marked //jmf:
 *      <3>      6/13/99        BWR             Added Staff auto identify code
 *      <2>      5/20/99        BWR             Added some screen redraws
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "spl-cast.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "globals.h"
#include "externs.h"

#include "beam.h"
#include "cloud.h"
#include "effects.h"
#include "fight.h"
#include "food.h"
#include "it_use2.h"
#include "itemname.h"
#include "itemprop.h"
#include "macro.h"
#include "monplace.h"
#include "monstuff.h"
#include "mutation.h"
#include "ouch.h"
#include "player.h"
#include "religion.h"
#include "skills.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "spells4.h"
#include "spl-book.h"
#include "spl-util.h"
#include "stuff.h"
#include "transfor.h"
#include "view.h"

#ifdef DOS
#include <conio.h>
#endif

#define WILD_MAGIC_NASTINESS 150

static int spell_base_difficulty( int spell );
static int calc_spell_cast_abil( int spell );

static void exercise_spell( int spell, bool success );

const char * failure_description( int fail_rate )
{
    return( (fail_rate ==  0) ? "Perfect"   :
            (fail_rate <= 10) ? "Excellent" :
            (fail_rate <= 20) ? "Great"     :
            (fail_rate <= 30) ? "Very Good" :
            (fail_rate <= 40) ? "Good"      :
            (fail_rate <= 50) ? "Fair"      :
            (fail_rate <= 60) ? "Poor"      :
            (fail_rate <= 70) ? "Very Poor" :
            (fail_rate <= 80) ? "Bad"       :
            (fail_rate <= 90) ? "Cruddy"    :
            (fail_rate <= 99) ? "Terrible"
                              : "Useless" );
}

unsigned char list_spells(void)
{
    int j;
    int lines = 0;
    unsigned int anything = 0;
    unsigned int i;
    int ki;
    bool already = false;

    const int num_lines = get_number_of_lines();

#ifdef DOS_TERM
    char buffer[4800];

    gettext(1, 1, 80, 25, buffer);
#endif

#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif

    clrscr();

    cprintf( " Your Spells                      Type                          Success   Level" );
    lines++;

    for (j = 0; j < 52; j++)
    {
        if (lines > num_lines - 2)
        {
            gotoxy(1, num_lines);
            cprintf( MORE_STRING );

            ki = getch();

            if (ki == ESCAPE)
            {
#ifdef DOS_TERM
                puttext(1, 1, 80, 25, buffer);
#endif
                return (ESCAPE);
            }

            if (isalpha( ki ))
            {
#ifdef DOS_TERM
                puttext(1, 1, 80, 25, buffer);
#endif
                return (ki);
            }

            if (ki == 0)
                ki = getch();

            lines = 0;
            clrscr();
            gotoxy(1, 1);
            anything = 0;
        }

        const char letter = index_to_letter(j);
        const int  spell  = get_spell_by_letter( letter );

        if (spell != SPELL_NO_SPELL)
        {
            anything++;

            if (lines > 0)
                cprintf(EOL);

            lines++;

            cprintf( " %c - %s", letter, spell_title( spell ) );
            gotoxy( 35, wherey() );

            already = false;

            unsigned int bit;
            for (i = 0, bit = 1; i <= SPTYP_LAST_EXPONENT; i++, bit <<= 1)
            {
                if (spell_typematch( spell, bit ))
                {
                    if (already)
                        cprintf( "/" );

                    cprintf( spelltype_name( bit ) );
                    already = true;
                }
            }

            char sval[16];

            //gotoxy(58, wherey());
            gotoxy(65, wherey());

            const int spell_f = spell_fail( spell );

#if DEBUG_DIAGNOSTICS
            cprintf( "(%d)", spell_f );
#else
            cprintf( failure_description(spell_f) );
#endif

            gotoxy(77, wherey());

            itoa( static_cast<int>( spell_level( spell ) ), sval, 10 );
            cprintf( sval );
        }
    }                           // end of j loop

    if (anything > 0)
    {
        ki = getch();

        if (ki >= 'A' && ki <= 'z')
        {
#ifdef DOS_TERM
            puttext(1, 1, 80, 25, buffer);
#endif
            return (ki);
        }

        if (ki == 0)
            ki = getch();

#ifdef DOS_TERM
        puttext(1, 1, 80, 25, buffer);
#endif

        return (ki);
    }

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif
    // was 35
    ki = getch();

    return (ki);
}                               // end list_spells()

static int spell_base_difficulty( int spell )
{
    const int level = spell_level( spell );
    int diff = 40;

    switch (level)
    {
    case  1: diff +=   0; break;
    case  2: diff +=  15; break;
    case  3: diff +=  35; break;
    case  4: diff +=  75; break;
    case  5: diff += 120; break;
    case  6: diff += 170; break;
    case  7: diff += 230; break;
    case  8: diff += 300; break;
    case  9: diff += 370; break;
    case 10: diff += 450; break;
    case 11: diff += 540; break;
    case 12: diff += 640; break;
    default: diff += 750; break;
    }

    return (diff);
}                               // end spell_hunger();

static int calc_spell_cast_abil( int spell )
{
    int power = 12 * you.skills[ SK_SPELLCASTING ];

    const int wizardry = player_mag_abil();     // max currently = 4
    const int level = spell_level( spell );

    unsigned int disciplines = spell_type( spell );
    disciplines &= (~SPTYP_HOLY);   //jmf: evil evil evil -- exclude HOLY bit

    const int skillcount = count_bits( disciplines );

    if (skillcount)
    {
        int total = 0;
        int ndx;
        unsigned int bit;

        ASSERT( (disciplines & SPTYP_HOLY) == 0 );

        for (ndx = 0, bit = 1; ndx <= SPTYP_LAST_EXPONENT; ndx++, bit <<= 1)
        {
            if (disciplines & bit)
                total += 6 * you.skills[ spell_type2skill( bit ) ];
        }

        power += total / skillcount;
    }

    power = stat_mult( you.intel, power );
    power = (power * (10 + wizardry)) / (7 + level) + 10 * wizardry;

    return (power);
}

static bool weapon_pen_spellcasting( const item_def &wpn )
{
    return (wpn.base_type != OBJ_STAVES
            && !(wpn.base_type == OBJ_WEAPONS && wpn.sub_type == WPN_STAFF)
            && weapon_skill( wpn ) != SK_SHORT_BLADES);
}

static int spell_equip_penalty( void )
{
    const int wpn = get_inv_in_hand();
    const int shield = you.equip[EQ_SHIELD];
    const int body = you.equip[EQ_BODY_ARMOUR];
    // const int gloves = you.equip[EQ_GLOVES];
    const size_type size = player_size();

    int chance = 0;

    if (body != -1)
    {

        const int ev_penalty = -property( you.inv[body], PARM_EVASION );

        // The minus 10 is to make the -1 light armours not so bad
        int armour = (15 * ev_penalty) - 10;

        //jmf: armour skill now reduces failure due to armour
        //bwr: this was far too good, an armour skill of 5 was
        //     completely negating plate mail.  Plate mail should
        //     hardly be completely negated, it should still be
        //     an important consideration for even high level characters.
        //     Truth is, even a much worse penalty than the above can
        //     easily be overcome by gaining spell skills... and a lot
        //     faster than any reasonable rate of bonus here.
        int lim_str = (you.str > 30) ? 30 :
                      (you.str < 10) ? 10 : you.str;

        armour -= ((you.skills[SK_ARMOUR] * lim_str) / 15);

        int race_arm = get_equip_race( you.inv[body] );
        int racial_type = 0;

        if (player_genus(GENPC_DWARVEN))
            racial_type = ISFLAG_DWARVEN;
        else if (player_genus(GENPC_ELVEN))
            racial_type = ISFLAG_ELVEN;
        else if (you.species == SP_HILL_ORC)
            racial_type = ISFLAG_ORCISH;

        // Elven armour gives everyone some benefit to spellcasting,
        // Dwarven armour hinders everyone.
        switch (race_arm)
        {
        case ISFLAG_ELVEN:
            armour -= 15;
            break;
        case ISFLAG_DWARVEN:
            armour += 5;
            break;
        default:
            break;
        }

        // Armour of the same racial type reduces penalty.
        if (racial_type && race_arm == racial_type)
            armour -= 10;

        if (armour > 0)
            chance += armour;
    }

    if (shield != -1)
    {
        int pen = 5 * (SIZE_MEDIUM - size);

        // moving 5 points of this, to the wpn + shield check below
        switch (you.inv[shield].sub_type)
        {
        default:
        case ARM_BUCKLER:
            break;

        case ARM_SHIELD:
            pen += 10;
            break;

        case ARM_LARGE_SHIELD:
            pen += 20;
            break;
        }

        if (pen > 0)
            chance += pen;
    }

    if (wpn != -1 && weapon_pen_spellcasting( you.inv[wpn] ))
    {
        const hands_reqd_type hands = hands_reqd( you.inv[wpn], size );

        if (hands == HANDS_TWO)
            chance += 10;
        else if (hands == HANDS_HALF)
            chance += 5;

        // wpn + shield = both hands busy
        if (shield != -1)
            chance += 5;
    }

    return (chance);
}

int spell_fail( int spell, bool raw )
{
    int chance = spell_base_difficulty( spell ) - calc_spell_cast_abil( spell );

    if (chance > 100)
        chance = 100;

    if (chance < 60)
        chance = 60 - (60 - chance) / 2;
    if (chance < 40)
        chance = 40 - (40 - chance) / 2;
    if (chance < 20)
        chance = 20 - (20 - chance) / 2;
    if (chance < 10)
        chance = 10 - (10 - chance) / 2;

    if (!raw)
    {
        chance += spell_equip_penalty();

        if (you.religion == GOD_VEHUMET
            && you.duration[DUR_PRAYER]
            && (!player_under_penance() && you.piety >= 50)
            && (spell_typematch( spell, SPTYP_CONJURATION )
                || spell_typematch( spell, SPTYP_SUMMONING )))
        {
            if (chance >= 20)
                chance -= 10;
            else
                chance = (chance + 1) / 2;
        }

        if (you.burden_state == BS_ENCUMBERED)
            chance += 10;
        else if (you.burden_state == BS_OVERLOADED)
            chance += 25;

        if (you.duration[DUR_TRANSFORMATION] > 0)
            chance += transform_spell_penalty();
    }

    return ((chance > 100) ? 100 : (chance < 0) ? 0 : chance);
}                               // end spell_fail()

int spell_memorise_fail( int spell )
{
    return (spell_fail( spell, true ));
}

static int spell_enhancement( int skill )
{
    int enhanced = 0;

    switch (skill)
    {
    case SK_CONJURATIONS:
        enhanced += player_spec_conj();
        break;

    case SK_ENCHANTMENTS:
        enhanced += player_spec_ench();
        break;

    case SK_SUMMONINGS:
        enhanced += player_spec_summ();
        break;

    case SK_POISON_MAGIC:
        enhanced += player_spec_poison();
        break;

    case SK_NECROMANCY:
        enhanced += player_spec_death();
        break;

    case SK_FIRE_MAGIC:
        enhanced += player_spec_fire();
        break;

    case SK_ICE_MAGIC:
        enhanced += player_spec_cold();
        break;

    case SK_EARTH_MAGIC:
        enhanced += player_spec_earth();
        break;

    case SK_AIR_MAGIC:
        enhanced += player_spec_air();
        break;

    default:
        break;
    }

    if (you.special_wield == SPWLD_SHADOW)
        enhanced -= 2;

    if (enhanced > 3)
        enhanced = 3;
    else if (enhanced < -3)
        enhanced = -3;

    return (enhanced);
}                               // end spell_enhancement()

int calc_spell_power( int spell, bool quiet )
{
    int enhanced = 0;
    unsigned int disciplines = spell_type( spell );

    // Note that power is calculated at x6 in this function (see bottom).
    int power = (you.intel + 3 * player_mag_abil()) * 6
                    + you.skills[SK_SPELLCASTING] * 3;

    //jmf: evil evil evil -- exclude HOLY bit
    disciplines &= (~SPTYP_HOLY);

    const int skillcount = count_bits( disciplines );

    if (skillcount)
    {
        unsigned int ndx, bit;
        int total = 0;

        ASSERT( (disciplines & SPTYP_HOLY) == 0 );

        for (ndx = 0, bit = 1; ndx <= SPTYP_LAST_EXPONENT; ndx++, bit <<= 1)
        {
            if (disciplines & bit)
            {
                const int sk  = spell_type2skill( bit );
                const int enh = spell_enhancement( sk );

                total += (you.skills[sk] + enh) * (12 + 6 * enh);
                enhanced += enh;
            }
        }

        power += total / skillcount;
    }

    if (!quiet && enhanced)               // one way or the other {dlb}
    {
        mpr( "You feel a%s %s",
            (enhanced < -2)  ? "n extraordinarily" :
            (enhanced == -2) ? "n extremely" :
            (enhanced == 2)  ? " strong" :
            (enhanced > 2)   ? " huge"
                             : "",
            (enhanced < 0)   ? "numb sensation."
                             : "surge of power!" );
    }

    return ((power > 0) ? (power + 3) / 6 : 0);
}                               // end calc_spell_power()

// returns false if spell failed, and true otherwise
bool cast_a_spell(void)
{
    unsigned char keyin = 0;

    if (!you.spell_no)
    {
        mpr("You don't know any spells.");
        return (false);
    }

    if (you.berserker)
    {
        canned_msg(MSG_TOO_BERSERK);
        return (false);
    }

    if (silenced( you.x_pos, you.y_pos ))
    {
        mpr("You cannot cast spells when silenced!");
        return (false);
    }

    // first query {dlb}:
    for (;;)
    {
        //jmf: FIXME: change to reflect range of known spells
        mpr( MSGCH_PROMPT, "Cast which spell ([?*] list)? " );

        keyin = get_ch();

        if (keyin == ESCAPE || isalpha(keyin))
            break;
        else if (keyin == '?' || keyin == '*')
        {
            keyin = list_spells();

            redraw_screen();
            if (keyin == ESCAPE || isalpha(keyin))
                break;
            else
                mesclr();
        }
    }

    if (keyin == ESCAPE)
        return (false);

    if (!isalpha(keyin))
    {
        mpr("You don't know that spell.");
        return (false);
    }

    const int spell = get_spell_by_letter( keyin );

    if (spell == SPELL_NO_SPELL)
    {
        mpr("You don't know that spell.");
        return (false);
    }

    if (spell_mana(spell) > you.magic_points)
    {
        mpr("You don't have enough magic to cast that spell.");
        return (false);
    }

    if (you.is_undead != US_UNDEAD
        && (you.hunger_state < HS_HUNGRY || you.hunger <= spell_hunger(spell)))
    {
        mpr("You don't have the energy to cast that spell.");
        return (false);
    }

    int cast = SPRET_FAIL;

    if (you.conf)
        random_uselessness();
    else
    {
        cast = your_spells( spell );

        if (cast)  // ie not aborted, but may have failed
        {
            exercise_spell( spell, (cast == SPRET_SUCCESS) );
            did_god_conduct( DID_SPELL_CASTING, spell_level(spell) );
        }
    }

    if (cast)  // did not abort, spell either succeeded or failed
    {
        you.turn_is_over = true;
        dec_mp( spell_mana(spell) );

        const int energy = player_energy();
        if (energy <= 0 && you.is_undead != US_UNDEAD)
        {
            int food_cost = spell_hunger( spell );

            // I wonder if a better algorithm is called for? {dlb}
            food_cost -= you.intel * you.skills[SK_SPELLCASTING];

            // Spell casting should always carry a little food
            // cost since basic combat does (3 + 2/extra). -- bwr
            const int min_cost = spell_level( spell ) * 5;
            if (food_cost < min_cost)
                food_cost = min_cost;

            make_hungry( food_cost, true );
        }
    }

    return (cast == SPRET_SUCCESS);
}                               // end cast_a_spell()

// returns true if spell is successfully cast for purposes of exercising
// and false otherwise (note: false == less exercise, not none).
int your_spells( int spc2, int powc, bool allow_fail )
{
    int total_skill = 0;
    struct dist spd;
    struct bolt beam;

    if (you.is_undead && spell_typematch( spc2, SPTYP_HOLY ))
    {
        mpr( "You can't use this type of magic!" );
        return (SPRET_ABORT);
    }

    // Normally undead can't memorise these spells, so this check is
    // to catch those in Lich form.
    if (spc2 != SPELL_NECROMUTATION
        && undead_cannot_memorise( spc2, you.is_undead ))
    {
        mpr( "You cannot cast that spell in your current form!" );
        return (SPRET_ABORT);
    }

    if (spc2 == SPELL_SYMBOL_OF_TORMENT
        && (you.is_undead || you.mutation[MUT_TORMENT_RESISTANCE] >= 3))
    {
        mpr( "To torment others, one must first know what torment means." );
        return (SPRET_ABORT);
    }

    if (spc2 == SPELL_ALTER_SELF && !enough_hp( you.hp_max / 2, true ))
    {
        mpr("Your body is in too poor a condition for this spell to function.");
        return (SPRET_ABORT);
    }

    const int flags = get_spell_flags( spc2 );

    // XXX: This handles only some of the cases where spells need targeting...
    // there are others that do their own that will be missed by this
    // (and thus will not properly ESC without cost because of it).
    // Hopefully, those will eventually be fixed. -- bwr
    if (flags & SPFLAG_TARGETING_MASK)
    {
        int targ = (testbits(flags, SPFLAG_HELPFUL) ? TARG_FRIEND : TARG_ENEMY);

        int dir  = (testbits( flags, SPFLAG_TARGET ) ? DIR_TARGET :
                    testbits( flags, SPFLAG_GRID )   ? DIR_GRID   :
                    testbits( flags, SPFLAG_DIR )    ? DIR_DIR    : DIR_NONE);

        if (!spell_direction( spd, beam, dir, targ ))
            return (SPRET_ABORT);

        if (testbits( flags, SPFLAG_NOT_SELF ) && spd.isMe)
        {
            if (spc2 == SPELL_TELEPORT_OTHER || spc2 == SPELL_HEAL_OTHER
                    || spc2 == SPELL_POLYMORPH_OTHER)
            {
                mpr( "Sorry, this spell works on others only." );
            }
            else
            {
                canned_msg(MSG_UNTHINKING_ACT);
            }

            return (SPRET_ABORT);
        }
    }

    // NOTE: ALL SPRET_ABORTs should be above this point... side effects
    // are about to be applied!

    // "Utility" spells for the sake of simplicity are currently ones with
    // enchantments, translocations, or divinations.
    if (!spell_is_utility_spell( spc2 ))
        did_god_conduct( DID_SPELL_NONUTILITY, 10 + spell_level(spc2) );

    if (spell_is_unholy( spc2 ))
        did_god_conduct( DID_UNHOLY, 10 + spell_level(spc2) );

    if (spell_typematch( spc2, SPTYP_NECROMANCY ))
    {
        did_god_conduct( DID_NECROMANCY, 10 + spell_level(spc2) );

        if (spc2 == SPELL_NECROMUTATION
            && (you.religion == GOD_ELYVILON
                || you.religion == GOD_SHINING_ONE
                || you.religion == GOD_ZIN))
        {
            excommunication();
        }
    }

    // Linley says: Condensation Shield needs some disadvantages to keep
    // it from being a no-brainer... this isn't much, but its a start -- bwr
    if (you.duration[DUR_CONDENSATION_SHIELD] > 0
        && spell_typematch( spc2, SPTYP_FIRE ))
    {
        // removes condensation shield:
        expose_player_to_element( BEAM_FIRE );
    }

    // Added this so that the passed in powc can have meaning -- bwr
    // Here now that calc_spell_power handles the surge message. -- bwr
    if (powc == -1)
        powc = calc_spell_power( spc2 );

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Spell #%d, power=%d, cast_abil=%d, fail=%d",
              spc2, powc, calc_spell_cast_abil( spc2 ), spell_fail( spc2 ) );

#endif

    if (allow_fail)
    {
        const int fail = spell_fail(spc2);
        int roll = roll_curved_percentile();

        // Yep... just worshipping Sif Muna once will get you wrath for
        // the rest of the game now.
        if (you.religion != GOD_SIF_MUNA && you.worshipped[GOD_SIF_MUNA]
            && one_chance_in(20))
        {
            god_speaks( GOD_SIF_MUNA, "You feel a surge of divine spite." );

            // This will cause failure and increase the miscast effect.
            roll = -2 * you.penance[GOD_SIF_MUNA];
        }

        if (roll < fail)
        {
            mpr( "You miscast the spell." );
            flush_input_buffer( FLUSH_ON_FAILURE );

            if (you.religion == GOD_SIF_MUNA
                && (!player_under_penance()
                    && you.piety >= 100
                    && you.piety + 50 > random2(300)))
            {
                canned_msg( MSG_NOTHING_HAPPENS );
                return (SPRET_FAIL);
            }

            unsigned int sptype = 0;

            do
            {
                sptype = 1 << random2( SPTYP_LAST_EXPONENT + 1 );
            } while (!spell_typematch( spc2, sptype ));

            // all spell failures give a bit of magical radiation..
            // failure is a function of power squared multiplied
            // by how badly you missed the spell.  High power
            // spells can be quite nasty: 9 * 9 * 90 / 500 = 15
            // points of contamination!
            int nastiness = spell_mana(spc2) * spell_mana(spc2) * (fail - roll) + 250;

            int cont_points = nastiness / 500;
            // handle fraction
            if (random2(500) < (nastiness % 500))
                cont_points++;

            contaminate_player( cont_points );

            miscast_effect( sptype, spell_mana(spc2), (fail - roll), 100 );

            if (you.religion == GOD_XOM && random2(75) < spell_mana(spc2))
                Xom_acts( coinflip(), spell_mana(spc2), false );

            return (SPRET_FAIL);
        }
    }

    int ret = SPRET_SUCCESS;  // used to track late aborts
    int tmp;

    switch (spc2)
    {
    case SPELL_IDENTIFY:
        identify(powc);
        break;

    case SPELL_TELEPORT_SELF:
        you_teleport();
        break;

    case SPELL_CAUSE_FEAR:
        mass_enchantment( ENCH_FEAR, powc, MHITYOU );
        break;

    case SPELL_CREATE_NOISE:  // unused, the player can shout to do this - bwr
        noisy( SL_YELL, you.x_pos, you.y_pos, "You hear a voice call your name!" );
        break;

    case SPELL_REMOVE_CURSE:
        remove_curse(false);
        break;

    case SPELL_MAGIC_DART:
        zapping(ZAP_MAGIC_DARTS, powc, beam);
        break;

    case SPELL_FIREBALL:
        zapping(ZAP_FIREBALL, powc, beam);
        break;

    case SPELL_DELAYED_FIREBALL:
        // This spell has two main advantages over Fireball:
        //
        // (1) The release is instantaneous, so monsters will not
        //     get an action before the player... this allows the
        //     player to hit monsters with a double fireball (this
        //     is why we only allow one delayed fireball at a time,
        //     if you want to allow for more, then the release should
        //     take at least some amount of time).
        //
        //     The casting of this spell still costs a turn.  So
        //     casting Delayed Fireball and immediately releasing
        //     the fireball is only slightly different than casting
        //     a regular Fireball (monsters act in the middle instead
        //     of at the end).  This is why we allow for the spell
        //     level discount so that Fireball is free with this spell
        //     (so that it only costs 7 levels instead of 13 to have
        //     both).
        //
        // (2) When the fireball is released, it is guaranteed to
        //     go off... the spell only fails at this point.  This can
        //     be a large advantage for characters who have difficulty
        //     casting Fireball in their standard equipment.  However,
        //     the power level for the actual fireball is determined at
        //     release, so if you do swap out your enhancers you'll
        //     get a less powerful ball when its released. -- bwr
        //
        if (!you.attribute[ ATTR_DELAYED_FIREBALL ])
        {
            // okay, this message is weak but functional -- bwr
            mpr( "You feel magically charged." );
            you.attribute[ ATTR_DELAYED_FIREBALL ] = 1;
        }
        else
        {
            canned_msg( MSG_NOTHING_HAPPENS );
        }
        break;

    case SPELL_STRIKING:
        zapping( ZAP_STRIKING, powc, beam );
        break;

    case SPELL_CONJURE_FLAME:
        // small clouds can fail if cast at occupied squartes
        ret = cast_small_cloud( powc, beam, CLOUD_FIRE );
        break;

    case SPELL_DIG:
        zapping(ZAP_DIGGING, powc, beam);
        break;

    case SPELL_BOLT_OF_FIRE:
        zapping(ZAP_FIRE, powc, beam);
        break;

    case SPELL_BOLT_OF_COLD:
        zapping(ZAP_COLD, powc, beam);
        break;

    case SPELL_LIGHTNING_BOLT:
        zapping(ZAP_LIGHTNING, powc, beam);
        break;

    case SPELL_BOLT_OF_MAGMA:
        zapping(ZAP_MAGMA, powc, beam);
        break;

    case SPELL_POLYMORPH_OTHER:
        zapping(ZAP_POLYMORPH_OTHER, powc, beam);
        break;

    case SPELL_SLOW:
        zapping(ZAP_SLOWING, powc, beam);
        break;

    case SPELL_HASTE:
        zapping(ZAP_HASTING, powc, beam);
        break;

    case SPELL_PETRIFY:
        zapping( ZAP_PETRIFY, powc, beam );
        break;

    case SPELL_CONFUSE:
        zapping(ZAP_CONFUSION, powc, beam);
        break;

    case SPELL_CONFUSING_TOUCH:
        cast_confusing_touch(powc);
        break;

    case SPELL_SURE_BLADE:
        cast_sure_blade(powc);
        break;

    case SPELL_INVISIBILITY:
        zapping(ZAP_INVISIBILITY, powc, beam);
        break;

    case SPELL_THROW_FLAME:
        zapping(ZAP_FLAME, powc, beam);
        break;

    case SPELL_THROW_FROST:
        zapping(ZAP_FROST, powc, beam);
        break;

    case SPELL_CONTROLLED_BLINK:
        ret = blink();
        break;

    case SPELL_FREEZING_CLOUD:
        cast_big_cloud( powc, beam, CLOUD_COLD );
        break;

    case SPELL_MEPHITIC_CLOUD:
        stinking_cloud( powc, beam );
        break;

    case SPELL_RING_OF_FLAMES:
        cast_ring_of_flames(powc);
        break;

    case SPELL_RESTORE_STRENGTH:
        restore_stat( STAT_STRENGTH );
        break;

    case SPELL_RESTORE_INTELLIGENCE:
        restore_stat( STAT_INTELLIGENCE );
        break;

    case SPELL_RESTORE_DEXTERITY:
        restore_stat( STAT_DEXTERITY );
        break;

    case SPELL_VENOM_BOLT:
        zapping(ZAP_VENOM_BOLT, powc, beam);
        break;

    case SPELL_OLGREBS_TOXIC_RADIANCE:
        cast_toxic_radiance();
        break;

    case SPELL_TELEPORT_OTHER:
        zapping(ZAP_TELEPORTATION, powc, beam);
        break;

    case SPELL_LESSER_HEALING:
        ret = cast_healing(5);
        break;

    case SPELL_GREATER_HEALING:
        ret = cast_healing(25);
        break;

    case SPELL_CURE_POISON_I:   //jmf: `healing' version? group w/ S_C_P_II?
        cast_cure_poison(powc);
        break;

    case SPELL_PURIFICATION:
        purification();
        break;

    case SPELL_DEATHS_DOOR:
        cast_deaths_door(powc);
        break;

    case SPELL_SELECTIVE_AMNESIA:
        cast_selective_amnesia(false);
        break;                  //     Sif Muna power calls with true

    case SPELL_MASS_CONFUSION:
        mass_enchantment( ENCH_CONFUSION, powc, MHITYOU );
        break;

    case SPELL_SMITING:
        ret = cast_smiting(powc);
        break;

    case SPELL_REPEL_UNDEAD:
        turn_undead(50);
        break;

    case SPELL_HOLY_WORD:
        holy_word( 50, false );
        break;

    case SPELL_DETECT_CURSE:
        detect_curse(false);
        break;

    case SPELL_SUMMON_SMALL_MAMMAL:
        summon_small_mammals(powc);
        break;

    case SPELL_ABJURATION_I:    //jmf: why not group with SPELL_ABJURATION_II?
        abjuration(powc);
        break;

    case SPELL_SUMMON_SCORPIONS:
        summon_scorpions(powc);
        break;

    case SPELL_LEVITATION:
        potion_effect( POT_LEVITATION, powc );
        break;

    case SPELL_BOLT_OF_DRAINING:
        zapping(ZAP_NEGATIVE_ENERGY, powc, beam);
        break;

    case SPELL_LEHUDIBS_CRYSTAL_SPEAR:
        zapping(ZAP_CRYSTAL_SPEAR, powc, beam);
        break;

    case SPELL_BOLT_OF_INACCURACY:
        zapping(ZAP_BEAM_OF_ENERGY, powc, beam);
        break;

    case SPELL_POISONOUS_CLOUD:
        cast_big_cloud( powc, beam, CLOUD_POISON );
        break;

    case SPELL_POISON_ARROW:
        zapping( ZAP_POISON_ARROW, powc, beam );
        break;

    case SPELL_FIRE_STORM:
        cast_fire_storm( powc, beam );
        break;

    case SPELL_DETECT_TRAPS:
        strcpy(info, "You detect ");
        strcat(info, (detect_traps(powc) > 0) ? "some traps!" : "nothing.");
        mpr(info);
        break;

    case SPELL_BLINK:
        random_blink( powc );
        break;

    case SPELL_ISKENDERUNS_MYSTIC_BLAST:
        zapping( ZAP_MYSTIC_BLAST, powc, beam );
        break;

    case SPELL_SWARM:
        summon_swarm( powc, false, false );
        break;

    case SPELL_SUMMON_HORRIBLE_THINGS:
        summon_things(powc);
        break;

    case SPELL_ENSLAVEMENT:
        zapping(ZAP_ENSLAVEMENT, powc, beam);
        break;

    case SPELL_MAGIC_MAPPING:
        if (you.level_type == LEVEL_LABYRINTH || you.level_type == LEVEL_ABYSS)
            canned_msg( MSG_DISORIENTED );
        else if (you.level_type == LEVEL_PANDEMONIUM)
            mpr("Your Earth magic cannot map Pandemonium.");
        else
        {
            mpr( "You feel aware of your surroundings." );
            powc = stepdown_value( powc, 10, 10, 40, 45 );
            magic_mapping( 5 + powc, 50 + roll_zdice( 2, powc ) );
        }
        break;

    case SPELL_HEAL_OTHER:
        zapping(ZAP_HEALING, powc, beam);
        break;

    case SPELL_ANIMATE_DEAD:
        mpr("You call on the dead to walk for you.");
        animate_dead( powc, you.x_pos, you.y_pos,
                      BEH_FRIENDLY, you.pet_target, 1 );
        break;

    case SPELL_PAIN:
        dec_hp(1, false);
        zapping(ZAP_PAIN, powc, beam);
        break;

    case SPELL_EXTENSION:
        extension(powc);
        break;

    case SPELL_CONTROL_UNDEAD:
        mass_enchantment( ENCH_CHARM, powc, MHITYOU );
        break;

    case SPELL_ANIMATE_SKELETON:
        mpr("You attempt to give life to the dead...");
        animate_a_corpse( you.x_pos, you.y_pos, BEH_FRIENDLY,
                          you.pet_target, CORPSE_SKELETON );
        break;

    case SPELL_VAMPIRIC_DRAINING:
        ret = vampiric_drain( powc, beam );
        break;

    case SPELL_SUMMON_WRAITHS:
        summon_undead(powc);
        break;

    case SPELL_DETECT_ITEMS:
        strcpy(info, "You detect ");
        strcat(info, (detect_items(powc) > 0) ? "items!" : "nothing.");
        mpr(info);
        break;

    case SPELL_BORGNJORS_REVIVIFICATION:
        cast_revivification(powc);
        break;

    case SPELL_BURN:
        ret = burn_freeze( powc, beam, BEAM_FIRE );
        break;

    case SPELL_FREEZE:
        ret = burn_freeze( powc, beam, BEAM_COLD );
        break;

    case SPELL_SUMMON_ELEMENTAL:
        tmp = summon_elemental( powc, 0, 2, false );
        ret = (tmp == -2) ? SPRET_ABORT :
              (tmp == -1) ? SPRET_FAIL
                          : SPRET_SUCCESS;
        break;

    case SPELL_OZOCUBUS_REFRIGERATION:
        cast_refrigeration(powc);
        break;

    case SPELL_STICKY_FLAME:
        zapping( ZAP_STICKY_FLAME, powc, beam );
        break;

    case SPELL_SUMMON_ICE_BEAST:
        summon_ice_beast_etc( powc, MONS_ICE_BEAST );
        break;

    case SPELL_OZOCUBUS_ARMOUR:
        ice_armour( powc, false );
        break;

    case SPELL_CALL_IMP:
        // if (one_chance_in(3))
        //     summon_ice_beast_etc( powc, MONS_WHITE_IMP );
        // else if (one_chance_in(7))
        //     summon_ice_beast_etc( powc, MONS_SHADOW_IMP );
        // else
        //     summon_ice_beast_etc( powc, MONS_IMP );
        call_imp( powc );
        break;

    case SPELL_REPEL_MISSILES:
        missile_prot( powc );
        break;

    case SPELL_BERSERKER_RAGE:
        cast_berserk();
        break;

    case SPELL_DISPEL_UNDEAD:
        zapping( ZAP_DISPEL_UNDEAD, powc, beam );
        break;

    case SPELL_GUARDIAN:
        summon_ice_beast_etc( powc, MONS_ANGEL );
        break;

    //jmf: FIXME: SPELL_PESTILENCE?

    case SPELL_THUNDERBOLT:
        zapping( ZAP_LIGHTNING, powc, beam );
        break;

    case SPELL_FLAME_OF_CLEANSING:
        zapping( ZAP_CLEANSING_FLAME, powc, beam );
        break;

    //jmf: FIXME: SPELL_SHINING_LIGHT?

    case SPELL_SUMMON_DAEVA:
        summon_ice_beast_etc( powc, MONS_DAEVA );
        break;

    case SPELL_ABJURATION_II:
        abjuration( powc );
        break;

    // Remember that most holy spells above don't yet use powc!

    case SPELL_TWISTED_RESURRECTION:
        cast_twisted( powc, BEH_FRIENDLY, you.pet_target );
        break;

    case SPELL_REGENERATION:
        cast_regen( powc );
        break;

    case SPELL_BONE_SHARDS:
        ret = cast_bone_shards( powc );
        break;

    case SPELL_BANISHMENT:
        zapping( ZAP_BANISHMENT, powc, beam );
        break;

    case SPELL_CIGOTUVIS_DEGENERATION:
        zapping( ZAP_DEGENERATION, powc, beam );
        break;

    case SPELL_STING:
        zapping( ZAP_STING, powc, beam );
        break;

    case SPELL_SUBLIMATION_OF_BLOOD:
        sublimation( powc );
        break;

    case SPELL_TUKIMAS_DANCE:
        dancing_weapon( powc, false );
        break;

    case SPELL_HELLFIRE:
        // should only be available from:
        // staff of Dispater & Sceptre of Asmodeus
        zapping( ZAP_HELLFIRE, powc, beam );
        break;

    case SPELL_SUMMON_DEMON:
        mpr("You open a gate to Pandemonium!");
        summon_ice_beast_etc( powc, rand_demon(DEMON_COMMON) );
        break;

    case SPELL_DEMONIC_HORDE:
        {
            mpr("You open a gate to Pandemonium!");

            const int num = 7 + random2(5);
            for (int i = 0; i < num; i++)
            {
                summon_ice_beast_etc( powc, rand_demon(DEMON_LESSER) );
            }
        }
        break;

    case SPELL_SUMMON_GREATER_DEMON:
        {
            mpr("You open a gate to Pandemonium!");

            const beh_type beh = ((random2(powc) <= 5) ? BEH_HOSTILE
                                                       : BEH_CHARMED);

            if (beh == BEH_CHARMED)
                mpr("You don't feel so good about this...");

            create_monster( rand_demon(DEMON_GREATER), beh, 5 );
        }
        break;

    case SPELL_CORPSE_ROT:
        corpse_rot(0);
        break;

    case SPELL_TUKIMAS_VORPAL_BLADE:
        if (!brand_weapon( SPWPN_VORPAL, powc ))
            canned_msg(MSG_SPELL_FIZZLES);
        break;

    case SPELL_FIRE_BRAND:
        if (!brand_weapon( SPWPN_FLAMING, powc ))
            canned_msg(MSG_SPELL_FIZZLES);
        break;

    case SPELL_FREEZING_AURA:
        if (!brand_weapon( SPWPN_FREEZING, powc ))
            canned_msg(MSG_SPELL_FIZZLES);
        break;

    case SPELL_LETHAL_INFUSION:
        if (!brand_weapon( SPWPN_DRAINING, powc ))
            canned_msg(MSG_SPELL_FIZZLES);
        break;

    case SPELL_MAXWELLS_SILVER_HAMMER:
        if (!brand_weapon( SPWPN_DUMMY_CRUSHING, powc ))
            canned_msg(MSG_SPELL_FIZZLES);
        break;

    case SPELL_POISON_WEAPON:
        if (!brand_weapon( SPWPN_VENOM, powc ))
            canned_msg(MSG_SPELL_FIZZLES);
        break;

    case SPELL_CRUSH:
        ret = burn_freeze( powc, beam, BEAM_MISSILE );
        break;

    case SPELL_BOLT_OF_IRON:
        zapping( ZAP_IRON_BOLT, powc, beam );
        break;

    case SPELL_STONE_ARROW:
        zapping( ZAP_STONE_ARROW, powc, beam );
        break;

    case SPELL_TOMB_OF_DOROKLOHE:
        cast_entomb( powc );
        break;

    case SPELL_STONEMAIL:
        stone_scales( powc );
        break;

    case SPELL_SHOCK:
        zapping( ZAP_ELECTRICITY, powc, beam );
        break;

    case SPELL_SWIFTNESS:
        cast_swiftness(powc);
        break;

    case SPELL_FLY:
        cast_fly(powc);
        break;

    case SPELL_INSULATION:
        cast_insulation(powc);
        break;

    case SPELL_ORB_OF_ELECTROCUTION:
        zapping( ZAP_ORB_OF_ELECTRICITY, powc, beam );
        break;

    case SPELL_DETECT_CREATURES:
        strcpy(info, "You detect ");
        strcat(info, (detect_creatures(powc) > 0) ? "creatures!" : "nothing.");
        mpr(info);
        break;

    case SPELL_CURE_POISON_II:  // poison magic version of cure poison
        cast_cure_poison(powc);
        break;

    case SPELL_CONTROL_TELEPORT:
        cast_teleport_control(powc);
        break;

    case SPELL_POISON_AMMUNITION:
        cast_poison_ammo();
        break;

    case SPELL_RESIST_POISON:
        cast_resist_poison(powc);
        break;

    case SPELL_PROJECTED_NOISE:
        project_noise();
        break;

    case SPELL_ALTER_SELF:
        // Note: may have aborted above if low on HP
        mpr("Your body is suffused with transfigurative energy!");

        set_hp( 1 + random2(you.hp), false );

        if (!mutate( PICK_RANDOM_MUTATION, false, false ))
            mpr("Odd... you don't feel any different.");
        break;

    case SPELL_DEBUGGING_RAY:
        zapping( ZAP_DEBUGGING_RAY, powc, beam );
        break;

    case SPELL_RECALL:
        recall( false );
        break;

    case SPELL_PORTAL:
        portal();
        break;

    case SPELL_AGONY:
        zapping( ZAP_AGONY, powc, beam );
        break;

    case SPELL_SPIDER_FORM:
        transform( powc, TRAN_SPIDER );
        break;

    case SPELL_DISRUPT:
        zapping( ZAP_DISRUPTION, powc, beam );
        break;

    case SPELL_DISINTEGRATE:
        zapping( ZAP_DISINTEGRATION, powc, beam );
        break;

    case SPELL_BLADE_HANDS:
        transform( powc, TRAN_BLADE_HANDS );
        break;

    case SPELL_STATUE_FORM:
        transform( powc, TRAN_STATUE );
        break;

    case SPELL_ICE_FORM:
        transform( powc, TRAN_ICE_BEAST );
        break;

    case SPELL_DRAGON_FORM:
        transform( powc, TRAN_DRAGON );
        break;

    case SPELL_NECROMUTATION:
        transform( powc, TRAN_LICH );
        break;

    case SPELL_DEATH_CHANNEL:
        cast_death_channel( powc );
        break;

    case SPELL_SYMBOL_OF_TORMENT:
        torment( you.x_pos, you.y_pos );
        break;

    case SPELL_DEFLECT_MISSILES:
        deflection( powc );
        break;

    case SPELL_ORB_OF_FRAGMENTATION:
        zapping( ZAP_ORB_OF_FRAGMENTATION, powc, beam );
        break;

    case SPELL_ICE_BOLT:
        zapping( ZAP_ICE_BOLT, powc, beam );
        break;

    case SPELL_ARC:
        ret = burn_freeze( powc, beam, BEAM_ELECTRICITY );
        break;

    case SPELL_AIRSTRIKE:
        ret = airstrike( powc, beam );
        break;

    case SPELL_ICE_STORM:
        zapping( ZAP_ICE_STORM, powc, beam );
        break;

    case SPELL_SHADOW_CREATURES:
        mpr( "Wisps of shadow whirl around you..." );
        create_monster( RANDOM_MONSTER, BEH_FRIENDLY, 1,
                        you.x_pos, you.y_pos, you.pet_target, 250, false,
                        random2( you.depth - 5 ) );
        break;

    //jmf: new spells 19mar2000
    case SPELL_FLAME_TONGUE:
        zapping( ZAP_FLAME_TONGUE, powc, beam );
        break;

    case SPELL_PASSWALL:
        cast_passwall( powc );
        break;

    case SPELL_IGNITE_POISON:
        cast_ignite_poison( powc );
        break;

    case SPELL_STICKS_TO_SNAKES:
        cast_sticks_to_snakes( powc );
        break;

    case SPELL_SUMMON_LARGE_MAMMAL:
        cast_summon_large_mammal( powc );
        break;

    case SPELL_SUMMON_DRAGON:
        cast_summon_dragon(powc);
        break;

    case SPELL_TAME_BEASTS:
        cast_tame_beasts( powc );
        break;

    case SPELL_SLEEP:
        zapping( ZAP_SLEEP, powc, beam );
        break;

    case SPELL_MASS_SLEEP:
        cast_mass_sleep( powc );
        break;

    case SPELL_DETECT_MAGIC:
        mpr( "FIXME: implement!" );
        break;

    case SPELL_DETECT_SECRET_DOORS:
        cast_detect_secret_doors( powc );
        break;

    case SPELL_SEE_INVISIBLE:
        cast_see_invisible( powc );
        break;

    case SPELL_FORESCRY:
        cast_forescry( powc );
        break;

    case SPELL_SUMMON_BUTTERFLIES:
        cast_summon_butterflies( powc );
        break;

    case SPELL_QUIET:
        cast_quiet( powc, beam );
        break;

    case SPELL_SILENCE:
        cast_silence( powc );
        break;

    case SPELL_SHATTER:
        cast_shatter( powc );
        break;

    case SPELL_DISPERSAL:
        cast_dispersal( powc );
        break;

    case SPELL_DISCHARGE:
        cast_discharge( powc );
        break;

    case SPELL_BACKLIGHT:
        zapping( ZAP_BACKLIGHT, powc + 10, beam );
        break;

    case SPELL_INTOXICATE:
        cast_intoxicate( powc );
        break;

    case SPELL_GLAMOUR:
        cast_glamour( powc );
        break;

    case SPELL_EVAPORATE:
        ret = cast_evaporate( powc );
        break;

    case SPELL_FULSOME_DISTILLATION:
        cast_fulsome_distillation( powc );
        break;

    case SPELL_FRAGMENTATION:
        ret = cast_fragmentation( powc );
        break;

    case SPELL_AIR_WALK:
        transform( powc, TRAN_AIR );
        break;

    case SPELL_SANDBLAST:
        cast_sandblast( powc, beam );
        break;

    case SPELL_ROTTING:
        cast_rotting( powc );
        break;

    case SPELL_CONDENSATION_SHIELD:
        cast_condensation_shield( powc );
        break;

    case SPELL_SEMI_CONTROLLED_BLINK:
        cast_semi_controlled_blink( powc );
        break;

    case SPELL_STONESKIN:
        cast_stoneskin( powc );
        break;

    case SPELL_SIMULACRUM:
        simulacrum( powc );
        break;

    case SPELL_CONJURE_BALL_LIGHTNING:
        cast_conjure_ball_lightning( powc );
        break;

    case SPELL_CHAIN_LIGHTNING:
        cast_chain_lightning( powc );
        break;

    case SPELL_SWAP:
        cast_swap( powc );
        break;

    case SPELL_APPORTATION:
        ret = cast_apportation( powc );
        break;

    default:
        mpr("Invalid spell!");
        break;
    }                           // end switch

    // Things to do if the spell is still a success (ie no late abort)
    if (ret == SPRET_SUCCESS)
    {
        alert_nearby_monsters();

        const int staff = get_inv_hand_tool();

        if (staff != -1
            && item_is_staff( you.inv[staff] )
            && !item_ident( you.inv[staff], ISFLAG_KNOW_TYPE ))
        {
            switch (you.inv[staff].sub_type)
            {
            case STAFF_ENERGY:
            case STAFF_WIZARDRY:
                total_skill = you.skills[SK_SPELLCASTING];
                break;
            case STAFF_FIRE:
                if (spell_typematch(spc2, SPTYP_FIRE))
                    total_skill = you.skills[SK_FIRE_MAGIC];
                else if (spell_typematch(spc2, SPTYP_ICE))
                    total_skill = you.skills[SK_ICE_MAGIC];
                break;
            case STAFF_COLD:
                if (spell_typematch(spc2, SPTYP_ICE))
                    total_skill = you.skills[SK_ICE_MAGIC];
                else if (spell_typematch(spc2, SPTYP_FIRE))
                    total_skill = you.skills[SK_FIRE_MAGIC];
                break;
            case STAFF_AIR:
                if (spell_typematch(spc2, SPTYP_AIR))
                    total_skill = you.skills[SK_AIR_MAGIC];
                else if (spell_typematch(spc2, SPTYP_EARTH))
                    total_skill = you.skills[SK_EARTH_MAGIC];
                break;
            case STAFF_EARTH:
                if (spell_typematch(spc2, SPTYP_EARTH))
                    total_skill = you.skills[SK_EARTH_MAGIC];
                else if (spell_typematch(spc2, SPTYP_AIR))
                    total_skill = you.skills[SK_AIR_MAGIC];
                break;
            case STAFF_POISON:
                if (spell_typematch(spc2, SPTYP_POISON))
                    total_skill = you.skills[SK_POISON_MAGIC];
                break;
            case STAFF_DEATH:
                if (spell_typematch(spc2, SPTYP_NECROMANCY))
                    total_skill = you.skills[SK_NECROMANCY];
                break;
            case STAFF_CONJURATION:
                if (spell_typematch(spc2, SPTYP_CONJURATION))
                    total_skill = you.skills[SK_CONJURATIONS];
                break;
            case STAFF_ENCHANTMENT:
                if (spell_typematch(spc2, SPTYP_ENCHANTMENT))
                    total_skill = you.skills[SK_ENCHANTMENTS];
                break;
            case STAFF_SUMMONING:
                if (spell_typematch(spc2, SPTYP_SUMMONING))
                    total_skill = you.skills[SK_SUMMONINGS];
                break;
            }

            if (you.skills[SK_SPELLCASTING] > total_skill)
                total_skill = you.skills[SK_SPELLCASTING];

            if (random2(100) < total_skill)
            {
                char str_pass[ ITEMNAME_SIZE ];

                set_ident_flags( you.inv[staff], ISFLAG_KNOW_TYPE );

                strcpy( info, "You are wielding " );
                in_name( staff, DESC_NOCAP_A, str_pass );
                strcat( info, str_pass );
                strcat( info, "." );
                mpr( info );

                more();
                set_redraw_status( REDRAW_WIELD );
            }
        }
    }

    return (ret);
}                               // end you_spells()

static void exercise_spell( int spell, bool success )
{
    int ndx, bit;
    int exer = 0;
    int workout = 0;

    unsigned int disciplines = spell_type(spell);

    //jmf: evil evil evil -- exclude HOLY bit
    disciplines &= (~SPTYP_HOLY);

    int skillcount = count_bits( disciplines );

    // (!success) reduces skill increase for miscast spells
    if (!success)
        skillcount += 4 + random2(10);

    const int diff = spell_level(spell);

    for (ndx = 0, bit = 1; ndx <= SPTYP_LAST_EXPONENT; ndx++, bit <<= 1)
    {
        if (!spell_typematch( spell, bit ))
            continue;

        workout = div_rand_round( random2( 1 + diff ), skillcount );
        exer += exercise( spell_type2skill( bit ), workout );
    }

    workout = one_chance_in(3) ? 1 : random2( 1 + random2(diff) );
    exer += exercise( SK_SPELLCASTING, workout );

    if (exer)
        did_god_conduct( DID_SPELL_PRACTISE, exer );
}                               // end exercise_spell()

/* This determines how likely it is that more powerful wild magic effects
 * will occur. Set to 100 for the old probabilities (although the individual
 * effects have been made much nastier since then).
 */

bool miscast_effect( unsigned int sp_type, int mag_pow, int mag_fail,
                     int force_effect, const char *cause )
{
/*  sp_type is the type of the spell
 *  mag_pow is overall power of the spell or effect (ie its level)
 *  mag_fail is the degree to which you failed
 *  force_effect forces a certain effect to occur. Currently unused.
 */
    struct bolt beam;
    bool failMsg = true;

    int loopj = 0;
    int spec_effect = 0;
    int hurted = 0;

    if (sp_type == SPTYP_RANDOM)
        sp_type = 1 << (random2(12));

    spec_effect = (mag_pow * mag_fail * (10 + mag_pow) / 7
                                                * WILD_MAGIC_NASTINESS) / 100;

    if (force_effect == 100
        && random2(40) > spec_effect
        && random2(40) > spec_effect)
    {
        canned_msg( MSG_NOTHING_HAPPENS );
        return (false);
    }

    // setup beam
    beam.is_tracer = false;

    spec_effect = spec_effect / 100;

#if DEBUG_DIAGNOSTICS
    const int old_fail = spec_effect;
#endif

    spec_effect = random2( spec_effect );

    if (spec_effect > 3)
        spec_effect = 3;
    else if (spec_effect < 0)
        spec_effect = 0;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Sptype: %d, failure1: %d, failure2: %d",
          sp_type, old_fail, spec_effect );
#endif

    if (force_effect != 100)
        spec_effect = force_effect;

    switch (sp_type)
    {
    case SPTYP_CONJURATION:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
            switch (random2(10))
            {
            case 0:
                mpr( "Sparks fly from your %s!", your_hand(true) );
                break;

            case 1:
                mpr("The air around you crackles with energy!");
                break;

            case 2:
                mpr( "Wisps of smoke drift from your %s.", your_hand(true));
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("You are momentarily dazzled by a flash of light!");
                break;
            case 5:
                mpr("Strange energies run through your body.");
                break;
            case 6:
                mpr("Your skin tingles.");
                break;
            case 7:
                mpr("Your skin glows momentarily.");
                break;
            case 8:
                canned_msg( MSG_NOTHING_HAPPENS );
                break;
            case 9:
                if (player_can_smell())
                    mpr("You smell something strange.");
                else if (you.species == SP_MUMMY)
                    mpr("Your bandages flutter.");
                else
                    canned_msg( MSG_NOTHING_HAPPENS );
                break;
            }
            break;

        case 1:         // a bit less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr( "Smoke pours from your %s!", your_hand(true) );

                big_cloud( CLOUD_GREY_SMOKE, you.x_pos, you.y_pos, 15,
                           7 + random2(7) );
                break;
            case 1:
                mpr("A wave of violent energy washes through your body!");
                ouch( 4 + roll_dice(2,4), 0, KILLED_BY_WILD_MAGIC, cause );
                break;
            }
            break;

        case 2:         // rather less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("Energy rips through your body!");
                ouch( 7 + roll_dice(2,9), 0, KILLED_BY_WILD_MAGIC, cause );
                break;
            case 1:
                mpr("You are caught in a violent explosion!");
                beam.type = SYM_BURST;
                beam.damage = dice_def( 3, 12 );
                beam.flavour = BEAM_MISSILE;
                beam.target_x = you.x_pos;
                beam.target_y = you.y_pos;
                strcpy(beam.name, "explosion");
                beam.colour = element_colour( EC_RANDOM );
                beam.beam_source = ((cause) ? MHITNOT : MHITYOU);
                beam.thrower = ((cause) ? KILL_MISC : KILL_YOU);
                beam.aux_source = cause;
                beam.ex_size = 1;
                beam.is_enchant = false;
                beam.is_explosion = true;

                explosion(beam);
                break;
            }
            break;

        case 3:         // considerably less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("You are blasted with magical energy!");
                ouch( 10 + roll_dice(2,15), 0, KILLED_BY_WILD_MAGIC, cause );
                break;
            case 1:
                mpr("There is a sudden explosion of magical energy!");

                beam.type = SYM_BURST;
                beam.colour = element_colour( EC_RANDOM );
                beam.flavour = BEAM_MISSILE;
                beam.target_x = you.x_pos;
                beam.target_y = you.y_pos;
                beam.damage = dice_def( 3, 20 );
                beam.beam_source = ((cause) ? MHITNOT : MHITYOU);
                beam.thrower = ((cause) ? KILL_MISC : KILL_YOU);
                beam.aux_source = cause;
                beam.is_beam = false;
                beam.is_enchant = false;
                beam.is_explosion = true;
                beam.is_tracer = false;
                strcpy(beam.name, "explosion");
                beam.ex_size = (coinflip() ? 1 : 2);

                explosion(beam);
                break;
            }
            break;
        }
        break;                  // end conjuration

    case SPTYP_ENCHANTMENT:
        switch (spec_effect)
        {
        case 0:         // harmless messages only
            switch (random2(10))
            {
            case 0:
                mpr( "Your %s glow momentarily.", your_hand(true) );
                break;
            case 1:
                mpr("The air around you crackles with energy!");
                break;
            case 2:
                mpr("Multicolored lights dance before your eyes!");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("Waves of light ripple over your body.");
                break;
            case 5:
                mpr("Strange energies run through your body.");
                break;
            case 6:
                mpr("Your skin tingles.");
                break;
            case 7:
                mpr("Your skin glows momentarily.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                if (!silenced( you.x_pos, you.y_pos ))
                    mpr(MSGCH_SOUND,"You hear something strange." );
                else if (you.attribute[ATTR_TRANSFORMATION] != TRAN_AIR)
                    mpr("Your skull vibrates slightly.");
                else
                    canned_msg(MSG_NOTHING_HAPPENS);
                break;
            }
            break;

        case 1:         // slightly annoying
            switch (random2(2))
            {
            case 0:
                potion_effect(POT_LEVITATION, 20);
                break;
            case 1:
                random_uselessness();
                break;
            }
            break;

        case 2:         // much more annoying
            switch (random2(7))
            {
            case 0:
            case 1:
            case 2:
                mpr("You sense a malignant aura.");
                curse_random_item( false );
                break;
            case 3:
            case 4:
            case 5:
                potion_effect(POT_SLOWING, 10);
                break;
            case 6:
                potion_effect(POT_BERSERK_RAGE, 10);
                break;
            }
            break;

        case 3:         // potentially lethal
            switch (random2(4))
            {
            case 0:
                do
                {
                    curse_random_item( false );
                    loopj = random2(3);
                }
                while (loopj != 0);

                mpr("You sense an overwhelmingly malignant aura!");
                break;
            case 1:
                potion_effect(POT_PARALYSIS, 10);
                break;
            case 2:
                potion_effect(POT_CONFUSION, 10);
                break;
            case 3:
                mpr("You feel saturated with unharnessed energies!");
                contaminate_player( 1 + roll_zdice(3,7) );
                break;
            }
            break;
        }
        break;                  // end enchantments

    case SPTYP_TRANSLOCATION:
        switch (spec_effect)
        {
        case 0:         // harmless messages only
            switch (random2(10))
            {
            case 0:
                mpr("Space warps around you.");
                break;
            case 1:
                mpr("The air around you crackles with energy!");
                break;
            case 2:
                mpr("You feel a wrenching sensation.");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("You spin around.");
                break;
            case 5:
                mpr("Strange energies run through your body.");
                break;
            case 6:
                mpr("Your skin tingles.");
                break;
            case 7:
                mpr("The world appears momentarily distorted!");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                mpr("You feel uncomfortable.");
                break;
            }
            break;

        case 1:         // mostly harmless
            switch (random2(6))
            {
            case 0:
            case 1:
            case 2:
                mpr("You are caught in a localised field of spatial distortion.");
                ouch( 2 + roll_dice(2,5), 0, KILLED_BY_WILD_MAGIC, cause );
                break;
            case 3:
            case 4:
                mpr("Space bends around you!");
                random_blink(0);
                ouch( 2 + roll_dice(2,4), 0, KILLED_BY_WILD_MAGIC, cause );
                break;
            case 5:
                mpr("Space twists in upon itself!");
                create_monster( MONS_SPATIAL_VORTEX, BEH_HOSTILE, 3 );
                break;
            }
            break;

        case 2:         // less harmless
            switch (random2(7))
            {
            case 0:
            case 1:
            case 2:
                mpr("You are caught in a strong localised spatial distortion.");
                ouch( 7 + roll_dice(2,12), 0, KILLED_BY_WILD_MAGIC, cause );
                break;
            case 3:
            case 4:
                mpr("Space warps around you!");

                if (one_chance_in(3))
                    you_teleport2( true );
                else
                    random_blink(0);

                ouch( 3 + roll_dice(2,5), 0, KILLED_BY_WILD_MAGIC, cause );
                potion_effect(POT_CONFUSION, 40);
                break;
            case 5:
                mpr("Space twists in upon itself!");

                for (loopj = 0; loopj < 2 + random2(3); loopj++)
                {
                    create_monster( MONS_SPATIAL_VORTEX, BEH_HOSTILE, 3 );
                }
                break;
            case 6:
                banished( DNGN_ENTER_ABYSS );     // sends you to the abyss
                break;
            }
            break;

        case 3:         // much less harmless

            switch (random2(4))
            {
            case 0:
                mpr("You are caught in an extremely strong localised spatial distortion!");
                ouch( 13 + roll_dice(2,15), 0, KILLED_BY_WILD_MAGIC, cause );
                break;
            case 1:
                mpr("Space warps crazily around you!");
                you_teleport2( true );

                ouch( 7 + roll_dice(2,9), 0, KILLED_BY_WILD_MAGIC, cause );
                potion_effect(POT_CONFUSION, 60);
                break;
            case 2:
                banished( DNGN_ENTER_ABYSS );     // sends you to the abyss
                break;
            case 3:
                mpr("You feel saturated with unharnessed energies!");
                contaminate_player( 1 + roll_zdice(3,7) );
                break;
            }
            break;
        }
        break;                  // end translocations

    case SPTYP_SUMMONING:
        switch (spec_effect)
        {
        case 0:         // harmless messages only
            switch (random2(10))
            {
            case 0:
                mpr("Shadowy shapes form in the air around you, then vanish.");
                break;
            case 1:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr(MSGCH_SOUND,"You hear strange voices." );
                else
                    mpr("You feel momentarily dizzy.");
                break;
            case 2:
                mpr("Your head hurts.");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("Your brain hurts!");
                break;
            case 5:
                mpr("Strange energies run through your body.");
                break;
            case 6:
                mpr("The world appears momentarily distorted.");
                break;
            case 7:
                mpr("Space warps around you.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                mpr("Distant voices call out to you!");
                break;
            }
            break;

        case 1:         // a little bad
            switch (random2(6))
            {
            case 0:             // identical to translocation
            case 1:
            case 2:
                mpr("You are caught in a localised spatial distortion.");
                ouch( 3 + roll_dice(2,5), 0, KILLED_BY_WILD_MAGIC, cause );
                break;

            case 3:
                mpr("Space twists in upon itself!");
                create_monster( MONS_SPATIAL_VORTEX, BEH_HOSTILE, 3 );
                break;

            case 4:
            case 5:
                if (create_monster( rand_demon(DEMON_LESSER),
                                    BEH_HOSTILE, 5 ) != -1)
                {
                    mpr("Something appears in a flash of light!");
                }
                break;
            }

        case 2:         // more bad
            switch (random2(6))
            {
            case 0:
                mpr("Space twists in upon itself!");

                for (loopj = 0; loopj < 2 + random2(3); loopj++)
                {
                    create_monster( MONS_SPATIAL_VORTEX, BEH_HOSTILE, 3 );
                }
                break;

            case 1:
            case 2:
                if (create_monster( rand_demon(DEMON_COMMON),
                                    BEH_HOSTILE, 5 ) != -1)
                {
                    mpr("Something forms out of thin air!");
                }
                break;

            case 3:
            case 4:
            case 5:
                mpr("A chorus of chattering voices calls out to you!");
                create_monster(rand_demon(DEMON_LESSER), BEH_HOSTILE, 5);
                create_monster(rand_demon(DEMON_LESSER), BEH_HOSTILE, 5);

                if (coinflip())
                {
                    create_monster( rand_demon(DEMON_LESSER),
                                    BEH_HOSTILE, 5 );
                }

                if (coinflip())
                {
                    create_monster( rand_demon(DEMON_LESSER),
                                    BEH_HOSTILE, 5 );
                }
                break;
            }
            break;

        case 3:         // more bad
            switch (random2(4))
            {
            case 0:
                if (create_monster( MONS_ABOMINATION_SMALL ) != -1)
                    mpr("Something forms out of thin air.");
                break;

            case 1:
                if (create_monster( rand_demon(DEMON_GREATER) ) != -1)
                    mpr("You sense a hostile presence.");
                break;

            case 2:
                mpr("Something turns its malign attention towards you...");

                create_monster( rand_demon(DEMON_COMMON), BEH_HOSTILE, 3 );
                create_monster( rand_demon(DEMON_COMMON), BEH_HOSTILE, 3 );

                if (coinflip())
                {
                    create_monster( rand_demon(DEMON_COMMON), BEH_HOSTILE, 3 );
                }
                break;

            case 3:
                banished( DNGN_ENTER_ABYSS );
                break;
            }
            break;
        }                       // end Summonings
        break;

    case SPTYP_DIVINATION:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
            switch (random2(10))
            {
            case 0:
                mpr("Weird images run through your mind.");
                break;
            case 1:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr(MSGCH_SOUND,"You hear strange voices." );
                else
                    mpr("Your nose twitches.");
                break;
            case 2:
                mpr("Your head hurts.");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("Your brain hurts!");
                break;
            case 5:
                mpr("Strange energies run through your body.");
                break;
            case 6:
                mpr("Everything looks hazy for a moment.");
                break;
            case 7:
                mpr("You seem to have forgotten something, but you can't remember what it was!");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                mpr("You feel uncomfortable.");
                break;
            }
            break;

        case 1:         // more annoying things
            switch (random2(2))
            {
            case 0:
                canned_msg( MSG_DISORIENTED );
                forget_map(10 + random2(10));
                break;
            case 1:
                potion_effect(POT_CONFUSION, 30);
                break;
            }
            break;

        case 2:         // even more annoying things
            switch (random2(2))
            {
            case 0:
                if (lose_stat(STAT_INTELLIGENCE, 1 + random2(3)))
                    mpr("You have damaged your brain!");
                else if (you.is_undead)
                    mpr("You suddenly recall your previous life!");
                else
                    mpr("You have a terrible headache.");
                break;
            case 1:
                mpr("You feel lost.");
                forget_map(40 + random2(40));
                break;
            }

            potion_effect(POT_CONFUSION, 40);  // common to all cases here {dlb}
            break;

        case 3:         // nasty
            switch (random2(3))
            {
            case 0:
                if (!forget_spells())
                    mpr( "You get a splitting headache." );
                break;
            case 1:
                mpr("You feel completely lost.");
                forget_map(100);
                break;
            case 2:
                if (lose_stat(STAT_INTELLIGENCE, 3 + random2(3)))
                    mpr("You have damaged your brain!");
                else if (you.is_undead)
                    mpr("You suddenly recall your previous life.");
                else
                    mpr("You have a terrible headache.");
                break;
            }

            potion_effect(POT_CONFUSION, 60);  // common to all cases here {dlb}
            break;
        }
        break;                  // end divinations

    case SPTYP_NECROMANCY:
        if (you.religion == GOD_KIKUBAAQUDGHA
            && (!player_under_penance() && you.piety >= 50
                && random2(150) <= you.piety))
        {
            canned_msg(MSG_NOTHING_HAPPENS);
            break;
        }

        switch (spec_effect)
        {
        case 0:
            switch (random2(10))
            {
            case 0:
                // mummies cannot smell {dlb}
                if (player_can_smell())
                    mpr("You smell decay.");
                else
                    mpr("You sense a malignant aura.");
                break;
            case 1:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr(MSGCH_SOUND,"You hear strange and distant voices." );
                else
                    mpr("You feel homesick.");
                break;
            case 2:
                mpr("Pain shoots through your body.");
                break;
            case 3:
                mpr("Your bones ache.");
                break;
            case 4:
                mpr("The world around you seems to dim momentarily.");
                break;
            case 5:
                mpr("Strange energies run through your body.");
                break;
            case 6:
                mpr("You shiver with cold.");
                break;
            case 7:
                mpr("You sense a malignant aura.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                mpr("You feel very uncomfortable.");
                break;
            }
            break;

        case 1:         // a bit nasty
            switch (random2(3))
            {
            case 0:
                if (you.is_undead)
                {
                    mpr("You feel weird for a moment.");
                    break;
                }
                mpr("Pain shoots through your body!");
                ouch( 3 + roll_dice(2,8), 0, KILLED_BY_WILD_MAGIC, cause );
                break;
            case 1:
                mpr("You feel horribly lethargic.");
                potion_effect(POT_SLOWING, 15);
                break;
            case 2:
                you.rotting++;
                if (player_can_smell())
                    mpr("You smell decay."); // identical to a harmless message
                else
                    mpr("You sense a malignant aura.");
                break;
            }
            break;

        case 2:         // much nastier
            switch (random2(4))
            {
            case 0:
                mpr("Flickering shadows surround you.");

                create_monster( MONS_SHADOW, BEH_HOSTILE, 2 );

                if (coinflip())
                    create_monster( MONS_SHADOW, BEH_HOSTILE, 2 );

                if (coinflip())
                    create_monster( MONS_SHADOW, BEH_HOSTILE, 2 );
                break;

            case 1:
                if (!player_prot_life() && one_chance_in(3))
                {
                    drain_exp();
                    break;
                }               // otherwise it just flows through...

            case 2:
                if (you.is_undead)
                {
                    mpr("You feel weird for a moment.");
                    break;
                }
                mpr("You convulse helplessly as pain tears through your body!");
                ouch( 13 + roll_dice(2,12), 0, KILLED_BY_WILD_MAGIC, cause );
                break;
            case 3:
                mpr("You sense a malignant aura.");
                curse_random_item( true );
                break;
            }
            break;

        case 3:         // even nastier
            switch (random2(6))
            {
            case 0:
                if (you.is_undead)
                    mpr("Something just walked over your grave.  No, really!");
                else if (you.mutation[MUT_TORMENT_RESISTANCE] >= 3)
                    mpr("You feel a surge of unholy energy.");
                else
                {
                    mpr("Your body is wracked with pain!");
                    dec_hp( you.hp / (2 + (you.mutation[MUT_TORMENT_RESISTANCE] > 1)) - 1, false );
                }
                break;

            case 1:
                mpr("You are engulfed in negative energy!");

                if (!player_prot_life())
                {
                    drain_exp();
                    break;
                }               // otherwise it just flows through...

            case 2:
                lose_stat( STAT_RANDOM, 1 + roll_zdice(2,4) );
                break;

            case 3:
                if (you.is_undead)
                {
                    mpr("You feel terrible.");
                    break;
                }

                rot_player( 1 + roll_zdice(2,4) );
                break;

            case 4:
                if (create_monster( MONS_SOUL_EATER, BEH_HOSTILE, 4 ) != -1)
                    mpr("Something reaches out for you...");
                break;

            case 5:
                if (create_monster( MONS_REAPER, BEH_HOSTILE, 4 ) != -1)
                    mpr("Death has come for you...");
                break;
            case 6:
                mpr("You sense an overwhelmingly malignant aura!");
                for (loopj = 0; loopj < 3; loopj++)
                {
                    curse_random_item( coinflip() );
                }
                break;
            }
            break;
        }
        break;                  // end necromancy

    case SPTYP_TRANSMIGRATION:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
            switch (random2(10))
            {
            case 0:
                mpr( "Your %s glow momentarily.", your_hand(true) );
                break;
            case 1:
                mpr("The air around you crackles with energy!");
                break;
            case 2:
                mpr("Multicolored lights dance before your eyes!");
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("Waves of light ripple over your body.");
                break;
            case 5:
                mpr("Strange energies run through your body.");
                break;
            case 6:
                mpr("Your skin tingles.");
                break;
            case 7:
                mpr("Your skin glows momentarily.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                if (player_can_smell())
                    mpr("You smell something strange.");
                else
                    mpr("Your skin tingles.");
                break;
            }
            break;

        case 1:         // slightly annoying
            switch (random2(2))
            {
            case 0:
                mpr("Your body is twisted painfully.");
                ouch( roll_dice(2,6), 0, KILLED_BY_WILD_MAGIC, cause );
                break;
            case 1:
                random_uselessness();
                break;
            }
            break;

        case 2:         // much more annoying
            switch (random2(4))
            {
            case 0:
                mpr("Your body is twisted very painfully!");
                ouch( 1 + roll_dice(2,12), 0, KILLED_BY_WILD_MAGIC, cause );
                break;
            case 1:
                mpr("You feel saturated with unharnessed energies!");
                contaminate_player( 1 + roll_zdice(3,7) );
                break;
            case 2:
                potion_effect(POT_PARALYSIS, 40);
                break;
            case 3:
                potion_effect(POT_CONFUSION, 40);
                break;
            }
            break;

        case 3:         // even nastier

            switch (random2(3))
            {
            case 0:
                mpr("Your body is flooded with distortional energies!");
                contaminate_player( 1 + roll_zdice(3,12) );
                ouch( 1 + roll_dice(2,10), 0, KILLED_BY_WILD_MAGIC, cause );
                break;

            case 1:
                mpr("You feel very strange.");
                delete_mutation( PICK_RANDOM_MUTATION );
                ouch( 3 + roll_dice(2,12), 0, KILLED_BY_WILD_MAGIC, cause );
                break;

            case 2:
                mpr("Your body is distorted in a weirdly horrible way!");
                failMsg = (give_bad_mutation() == MUT_NONE);
                if (coinflip())
                    give_bad_mutation(false, failMsg);

                ouch( 3 + roll_dice(2,12), 0, KILLED_BY_WILD_MAGIC, cause );
                break;
            }
            break;
        }
        break;                  // end transmigrations

    case SPTYP_FIRE:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
            switch (random2(10))
            {
            case 0:
                mpr( "Sparks fly from your %s!", your_hand(true) );
                break;
            case 1:
                mpr("The air around you burns with energy!");
                break;
            case 2:
                mpr( "Wisps of smoke drift from your %s.", your_hand(true) );
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                // mummies cannot smell
                if (player_can_smell())
                    mpr("You smell smoke.");
                else
                    mpr("You notice a wisp of smoke.");
                break;
            case 5:
                mpr("Heat runs through your body.");
                break;
            case 6:
                mpr("You feel uncomfortably hot.");
                break;
            case 7:
                mpr("Lukewarm flames ripple over your body.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr(MSGCH_SOUND,"You hear a sizzling sound." );
                else
                    mpr("You feel like you have heartburn.");
                break;
            }
            break;

        case 1:         // a bit less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr( "Smoke pours from your %s!", your_hand(true) );

                big_cloud( CLOUD_GREY_SMOKE + random2(3),
                           you.x_pos, you.y_pos, 15, 7 + random2(7) );
                break;

            case 1:
                mpr("Flames sear your flesh.");

                expose_player_to_element( BEAM_FIRE, 4 );

                if (player_res_fire() < 0)
                    ouch( roll_dice(2,7), 0, KILLED_BY_WILD_MAGIC, cause );
                break;
            }
            break;

        case 2:         // rather less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("You are blasted with fire.");

                ouch( check_your_resists( 3 + roll_dice(2,15), BEAM_FIRE ), 0,
                      KILLED_BY_WILD_MAGIC, cause );

                expose_player_to_element( BEAM_FIRE, 7 );
                break;

            case 1:
                mpr("You are caught a fiery explosion!");

                beam.type = SYM_BURST;
                beam.damage = dice_def( 3, 14 );
                beam.flavour = BEAM_FIRE;
                beam.target_x = you.x_pos;
                beam.target_y = you.y_pos;
                strcpy(beam.name, "explosion");
                beam.colour = RED;
                beam.beam_source = ((cause) ? MHITNOT : MHITYOU);
                beam.thrower = ((cause) ? KILL_MISC : KILL_YOU);
                beam.aux_source = cause;
                beam.ex_size = 1;
                beam.is_enchant = false;
                beam.is_explosion = true;

                explosion(beam);
                break;
            }
            break;

        case 3:         // considerably less harmless stuff
            switch (random2(3))
            {
            case 0:
                mpr("You are blasted with searing flames!");

                ouch( check_your_resists( 7 + roll_dice(2,17), BEAM_FIRE ), 0,
                      KILLED_BY_WILD_MAGIC, cause );

                expose_player_to_element( BEAM_FIRE, 14 );
                break;
            case 1:
                mpr("There is a sudden and violent explosion of flames!");

                beam.type = SYM_BURST;
                beam.damage = dice_def( 3, 20 );
                beam.flavour = BEAM_FIRE;
                beam.target_x = you.x_pos;
                beam.target_y = you.y_pos;
                strcpy( beam.name, "fireball" );
                beam.colour = RED;
                beam.beam_source = ((cause) ? MHITNOT : MHITYOU);
                beam.thrower = ((cause) ? KILL_MISC : KILL_YOU);
                beam.aux_source = cause;
                beam.ex_size = coinflip()?1:2;
                beam.is_enchant = false;
                beam.is_explosion = true;

                explosion(beam);
                break;

            case 2:
                mpr("You are covered in liquid fire!");
                you.duration[DUR_LIQUID_FLAMES] += 1 + roll_zdice(2,4);
                break;
            }
            break;
        }
        break;                  // end fire

    case SPTYP_ICE:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
            switch (random2(10))
            {
            case 0:
                mpr("You shiver with cold.");
                break;
            case 1:
                mpr("A chill runs through your body.");
                break;
            case 2:
                mpr("Wisps of condensation drift from your %s.", your_hand(true));
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("Your %s feel numb with cold.", your_hand(true));
                break;
            case 5:
                mpr("A chill runs through your body.");
                break;
            case 6:
                mpr("You feel uncomfortably cold.");
                break;
            case 7:
                mpr("Frost covers your body.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr(MSGCH_SOUND,"You hear a crackling sound." );
                else
                    mpr("A snowflake lands on your nose.");
                break;
            }
            break;

        case 1:         // a bit less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("You feel extremely cold.");
                break;
            case 1:
                mpr("You are covered in a thin layer of ice");

                expose_player_to_element( BEAM_COLD, 4 );

                if (player_res_cold() < 0)
                    ouch( 2 + roll_dice(2,4), 0, KILLED_BY_WILD_MAGIC, cause );
                break;
            }
            break;

        case 2:         // rather less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("Heat is drained from your body.");

                ouch( check_your_resists(5 + random2(6) + random2(7), BEAM_COLD),
                      0, KILLED_BY_WILD_MAGIC, cause );

                expose_player_to_element( BEAM_COLD, 7 );
                break;

            case 1:
                mpr("You are caught in an explosion of ice and frost!");

                beam.type = SYM_BURST;
                beam.damage = dice_def( 3, 11 );
                beam.flavour = BEAM_COLD;
                beam.target_x = you.x_pos;
                beam.target_y = you.y_pos;
                strcpy(beam.name, "explosion");
                beam.colour = WHITE;
                beam.beam_source = ((cause) ? MHITNOT : MHITYOU);
                beam.thrower = ((cause) ? KILL_MISC : KILL_YOU);
                beam.aux_source = cause;
                beam.ex_size = 1;
                beam.is_enchant = false;
                beam.is_explosion = true;

                explosion(beam);
                break;
            }
            break;

        case 3:         // less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("You are blasted with ice!");

                ouch( check_your_resists( 7 + roll_dice(2,12), BEAM_COLD ), 0,
                      KILLED_BY_WILD_MAGIC, cause );

                expose_player_to_element( BEAM_COLD, 14 );
                break;
            case 1:
                mpr("Freezing gasses pour from your %s!", your_hand(true));

                big_cloud(CLOUD_COLD, you.x_pos, you.y_pos, 15, 8 + random2(4));
                break;
            }
            break;
        }
        break;                  // end ice

    case SPTYP_EARTH:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
        case 1:
            switch (random2(10))
            {
            case 0:
                mpr("You feel earthy.");
                break;
            case 1:
                mpr("You are showered with tiny particles of grit.");
                break;
            case 2:
                mpr("Sand pours from your %s.", your_hand(true));
                break;
            case 3:
                mpr("You feel a surge of energy from the ground.");
                break;
            case 4:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr(MSGCH_SOUND,"You hear a distant rumble." );
                else
                    mpr("You sympathise with the stones.");
                break;
            case 5:
                mpr("You feel gritty.");
                break;
            case 6:
                mpr("You feel momentarily lethargic.");
                break;
            case 7:
                mpr("Motes of dust swirl before your eyes.");
                break;
            case 8:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            case 9:
                strcpy(info, "Your ");
                strcat(info, (you.species == SP_NAGA)    ? "underbelly feels" :
                             (you.species == SP_CENTAUR) ? "hooves feel"
                                                         : "feet feel");
                strcat(info, " warm.");
                mpr(info);
                break;
            }
            break;

        case 2:         // slightly less harmless stuff
            switch (random2(1))
            {
            case 0:
                switch (random2(3))
                {
                case 0:
                    mpr("You are hit by flying rocks!");
                    break;
                case 1:
                    mpr("You are blasted with sand!");
                    break;
                case 2:
                    mpr("Rocks fall onto you out of nowhere!");
                    break;
                }

                hurted = 8 + roll_dice(2,7);
                hurted -= random2(1 + player_armour_class());

                ouch(hurted, 0, KILLED_BY_WILD_MAGIC, cause);
                break;
            }
            break;

        case 3:         // less harmless stuff
            switch (random2(1))
            {
            case 0:
                mpr("You are caught in an explosion of flying shrapnel!");

                beam.type = SYM_BURST;
                beam.damage = dice_def( 3, 15 );
                beam.flavour = BEAM_FRAG;
                beam.target_x = you.x_pos;
                beam.target_y = you.y_pos;
                strcpy(beam.name, "explosion");
                beam.colour = CYAN;

                if (one_chance_in(5))
                    beam.colour = BROWN;
                if (one_chance_in(5))
                    beam.colour = LIGHTCYAN;

                beam.beam_source = ((cause) ? MHITNOT : MHITYOU);
                beam.thrower = ((cause) ? KILL_MISC : KILL_YOU);
                beam.aux_source = cause;
                beam.ex_size = 1;
                beam.is_enchant = false;
                beam.is_explosion = true;

                explosion(beam);
                break;
            }
            break;
        }
        break;                  // end Earth

    case SPTYP_AIR:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
            switch (random2(10))
            {
            case 0:
                mpr("Ouch! You gave yourself an electric shock.");
                break;
            case 1:
                mpr("You feel momentarily weightless.");
                break;
            case 2:
                mpr("Wisps of vapour drift from your %s.", your_hand(true));
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("You feel electric!");
                break;
            case 5:
                mpr("Sparks of electricity dance between your %s.", your_hand(true));
                break;
            case 6:
                mpr("You are blasted with air!");
                break;
            case 7:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr(MSGCH_SOUND,"You hear a whooshing sound." );
                else if (player_can_smell())
                    mpr("You smell ozone.");
                else
                    mpr("You feel a sudden rush of air.");
                break;
            case 8:
                canned_msg( MSG_NOTHING_HAPPENS );
                break;
            case 9:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr(MSGCH_SOUND,"You hear a crackling sound." );
                else if (player_can_smell())
                    mpr("You smell something musty.");
                else
                    mpr("You feel electric!");
                break;
            }
            break;

        case 1:         // a bit less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("There is a short, sharp shower of sparks.");
                break;
            case 1:
                mpr( "The wind %s around you!",
                      silenced(you.x_pos, you.y_pos) ? "whips" : "howls" );
                break;
            }
            break;

        case 2:         // rather less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("Electricity courses through your body.");
                ouch( check_your_resists(2 + roll_dice(2,5), BEAM_ELECTRICITY),
                      0, KILLED_BY_WILD_MAGIC, cause );
                break;
            case 1:
                mpr("Noxious gasses pour from your %s!", your_hand(true));

                big_cloud(CLOUD_STINK, you.x_pos, you.y_pos, 15,
                          9 + random2(4));
                break;
            }
            break;

        case 3:         // less harmless stuff
            switch (random2(2))
            {
            case 0:
                mpr("You are caught in an explosion of electrical discharges!");

                beam.type = SYM_BURST;
                beam.damage = dice_def( 3, 8 );
                beam.flavour = BEAM_ELECTRICITY;
                beam.target_x = you.x_pos;
                beam.target_y = you.y_pos;
                strcpy(beam.name, "explosion");
                beam.colour = LIGHTBLUE;
                beam.beam_source = ((cause) ? MHITNOT : MHITYOU);
                beam.thrower = ((cause) ? KILL_MISC : KILL_YOU);
                beam.aux_source = cause;
                beam.ex_size = one_chance_in(4)?1:2;
                beam.is_enchant = false;
                beam.is_explosion = true;

                explosion(beam);
                break;
            case 1:
                mpr("Venomous gasses pour from your %s!", your_hand(true));

                big_cloud(CLOUD_POISON, you.x_pos, you.y_pos, 15, 8 + random2(5));
                break;
            }
            break;
        }
        break;                  // end air

    case SPTYP_POISON:
        switch (spec_effect)
        {
        case 0:         // just a harmless message
            switch (random2(10))
            {
            case 0:
                mpr("You feel mildly nauseous.");
                break;
            case 1:
                mpr("You feel slightly ill.");
                break;
            case 2:
                mpr("Wisps of poison gas drift from your %s.", your_hand(true));
                break;
            case 3:
                mpr("You feel a strange surge of energy!");
                break;
            case 4:
                mpr("You feel faint for a moment.");
                break;
            case 5:
                mpr("You feel sick.");
                break;
            case 6:
                mpr("You feel odd.");
                break;
            case 7:
                mpr("You feel weak for a moment.");
                break;
            case 8:
                canned_msg( MSG_NOTHING_HAPPENS );
                break;
            case 9:
                if (!silenced(you.x_pos, you.y_pos))
                    mpr(MSGCH_SOUND,"You hear a slurping sound." );
                else if (player_can_smell())
                    mpr("You taste almonds.");
                else
                    mpr("You feel sick.");
                break;
            }
            break;

        case 1:         // a bit less harmless stuff
            switch (random2(2))
            {
            case 0:
                if (player_res_poison() >= 3)
                {
                    canned_msg( MSG_NOTHING_HAPPENS );
                    return (false);
                }

                mpr("You feel sick.");
                poison_player( 2 + random2(3) );
                break;

            case 1:
                mpr("Noxious gasses pour from your %s!", your_hand(true) );
                place_cloud(CLOUD_STINK, you.x_pos, you.y_pos, 2 + random2(4));
                break;
            }
            break;

        case 2:         // rather less harmless stuff
            switch (random2(3))
            {
            case 0:
                if (player_res_poison() >= 3)
                {
                    canned_msg( MSG_NOTHING_HAPPENS );
                    return (false);
                }

                mpr("You feel very sick.");
                poison_player( 1 + roll_dice(2,5) );
                break;

            case 1:
                mpr("Noxious gasses pour from your hands!");
                big_cloud(CLOUD_STINK, you.x_pos, you.y_pos, 15, 8 + random2(5));
                break;

            case 2:
                if (player_res_poison() >= 3)
                {
                    canned_msg( MSG_NOTHING_HAPPENS );
                    return (false);
                }
                lose_stat(STAT_RANDOM, 1);
                break;
            }
            break;

        case 3:         // less harmless stuff
            switch (random2(3))
            {
            case 0:
                if (player_res_poison() >= 3)
                {
                    canned_msg( MSG_NOTHING_HAPPENS );
                    return (false);
                }

                mpr("You feel incredibly sick.");
                poison_player( 8 + roll_dice(2,10) );
                break;
            case 1:
                mpr("Venomous gasses pour from your %s!", your_hand(true));

                big_cloud(CLOUD_POISON, you.x_pos, you.y_pos, 15, 7 + random2(7));
                break;
            case 2:
                if (player_res_poison() > random2(3))
                {
                    canned_msg( MSG_NOTHING_HAPPENS );
                    return (false);
                }

                lose_stat( STAT_RANDOM, 1 + roll_zdice(2,4) );
                break;
            }
            break;
        }
        break;                  // end poison
    }

    return (true);
}                               // end miscast_effect()

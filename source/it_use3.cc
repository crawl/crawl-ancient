/*
 *  File:       it_use3.cc
 *  Summary:    Functions for using some of the wackier inventory items.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <4>     6/13/99         BWR             Auto ID Channel staff
 *      <3>     5/22/99         BWR             SPWLD_POWER is now HP/13 - 3
 *      <2>     5/20/99         BWR             Capped SPWLD_POWER to +20
 *      <1>     -/--/--         LRH             Created
 */

#include "AppHdr.h"
#include "it_use3.h"

#include <string.h>
#include <stdio.h>

#include "globals.h"
#include "externs.h"

#include "beam.h"
#include "decks.h"
#include "direct.h"
#include "effects.h"
#include "fight.h"
#include "food.h"
#include "items.h"
#include "it_use2.h"
#include "itemname.h"
#include "itemprop.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mon-util.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "skills.h"
#include "skills2.h"
#include "spells1.h"
#include "spells2.h"
#include "spl-book.h"
#include "spl-cast.h"
#include "spl-util.h"
#include "stuff.h"
#include "view.h"

static bool ball_of_energy(void);
static bool ball_of_fixation(void);
static bool ball_of_seeing(void);
static bool box_of_beasts(void);
static bool disc_of_storms(void);
static bool efreet_flask(void);

void special_wielded(void)
{
    const int wpn = get_inv_hand_tool(); // must be at least a tool for effects

    if (wpn == -1)
        return;

    const bool wielded = player_weapon_wielded(); // some require wielding

    const int old_plus = you.inv[wpn].plus;
    const int old_plus2 = you.inv[wpn].plus2;
    const char old_colour = you.inv[wpn].colour;

    char str_pass[ ITEMNAME_SIZE ];
    int temp_rand = 0;          // for probability determination {dlb}
    bool makes_noise = (one_chance_in(20) && !silenced(you.x_pos, you.y_pos));

    switch (you.special_wield)
    {
    case SPWLD_SING:
        if (makes_noise)
        {
            strcpy(info, "The Singing Sword ");
            temp_rand = random2(32);
            strcat(info,
                     (temp_rand ==  0) ? "hums a little tune." :
                     (temp_rand ==  1) ? "breaks into glorious song!" :
                     (temp_rand ==  2) ? "sings." :
                     (temp_rand ==  3) ? "sings loudly." :
                     (temp_rand ==  4) ? "chimes melodiously." :
                     (temp_rand ==  5) ? "makes a horrible noise." :
                     (temp_rand ==  6) ? "sings off-key." :
                     (temp_rand ==  7) ? "sings 'tra-la-la'." :
                     (temp_rand ==  8) ? "burbles away merrily." :
                     (temp_rand ==  9) ? "gurgles." :
                     (temp_rand == 10) ? "suddenly shrieks!" :
                     (temp_rand == 11) ? "cackles." :
                     (temp_rand == 12) ? "warbles." :
                     (temp_rand == 13) ? "chimes harmoniously." :
                     (temp_rand == 14) ? "makes beautiful music." :
                     (temp_rand == 15) ? "produces a loud orchestral chord." :
                     (temp_rand == 16) ? "whines plaintively." :
                     (temp_rand == 17) ? "tinkles." :
                     (temp_rand == 18) ? "rings like a bell." :
                     (temp_rand == 19) ? "wails mournfully." :
                     (temp_rand == 20) ? "practices its scales." :
                     (temp_rand == 21) ? "lilts tunefully." :
                     (temp_rand == 22) ? "hums tunelessly." :
                     (temp_rand == 23) ? "sighs." :
                     (temp_rand == 24) ? "makes a deep moaning sound." :
                     (temp_rand == 25) ? "makes a popping sound." :
                     (temp_rand == 26) ? "sings a sudden staccato note." :
                     (temp_rand == 27) ? "says 'Hi! I'm the Singing Sword!'." :
                     (temp_rand == 28) ? "whispers something." :
                     (temp_rand == 29) ? "speaks gibberish." :
                     (temp_rand == 30) ? "raves incoherently."
                                       : "yells in some weird language.");
            mpr( MSGCH_SOUND, info );
        }
        break;

    case SPWLD_CURSE:
        makes_noise = false;

        if (one_chance_in(30))
            curse_random_item( false );
        break;

    case SPWLD_VARIABLE:
        makes_noise = false;

        if (wielded)
        {
            do_uncurse_item( you.inv[wpn] );

            if (random2(5) < 2)     // 40% chance {dlb}
                you.inv[wpn].plus  += (coinflip() ? +1 : -1);

            if (random2(5) < 2)     // 40% chance {dlb}
                you.inv[wpn].plus2 += (coinflip() ? +1 : -1);

            if (you.inv[wpn].plus < -4)
                you.inv[wpn].plus = -4;
            else if (you.inv[wpn].plus > 16)
                you.inv[wpn].plus = 16;

            if (you.inv[wpn].plus2 < -4)
                you.inv[wpn].plus2 = -4;
            else if (you.inv[wpn].plus2 > 16)
                you.inv[wpn].plus2 = 16;

            you.inv[wpn].colour = element_colour( EC_RANDOM );
        }
        break;

    case SPWLD_TORMENT:
        makes_noise = false;

        if (one_chance_in(200))
        {
            torment( you.x_pos, you.y_pos );
            did_god_conduct( DID_NECROMANCY, 1 );
        }
        break;

    case SPWLD_ZONGULDROK:
        makes_noise = false;

        if (one_chance_in(5))
        {
            animate_dead( 100, you.x_pos, you.y_pos, BEH_HOSTILE, MHITYOU, 1 );
            did_god_conduct( DID_NECROMANCY, 1 );
        }
        break;

    case SPWLD_POWER:
        makes_noise = false;

        if (wielded)
        {
            you.inv[wpn].plus = stepdown_value(-4 + (you.hp / 5), 4, 4, 4, 20);
            you.inv[wpn].plus2 = you.inv[wpn].plus;
        }
        break;

    case SPWLD_OLGREB:
        makes_noise = false;

        // Giving Olgreb's staff a little lift since staves of poison have
        // been made better. -- bwr
        if (wielded)
        {
            you.inv[wpn].plus = you.skills[SK_POISON_MAGIC] / 3;
            you.inv[wpn].plus2 = you.inv[wpn].plus;
        }
        break;

    case SPWLD_WUCAD_MU:
        makes_noise = false;

        if (wielded)
        {
            you.inv[wpn].plus  = ((you.intel > 25) ? 22 : you.intel - 3);
            you.inv[wpn].plus2 = ((you.intel > 25) ? 13 : you.intel / 2);
        }
        break;

    case SPWLD_SHADOW:
        makes_noise = false;

        if (random2(8) <= player_spec_death())
        {
            did_god_conduct( DID_NECROMANCY, 1 );
            create_monster( MONS_SHADOW, BEH_FRIENDLY, 2 );
        }

        you.flash_colour = DARKGREY;
        break;

    case SPWLD_HUM:
        if (makes_noise)
        {
            in_name(wpn, DESC_CAP_YOUR, str_pass);
            strcpy(info, str_pass);
            strcat(info, " lets out a weird humming sound.");
            mpr(MSGCH_SOUND,info );
        }
        break;

    case SPWLD_CHIME:
        if (makes_noise)
        {
            in_name(wpn, DESC_CAP_YOUR, str_pass);
            strcpy(info, str_pass);
            strcat(info, " chimes like a gong.");
            mpr(MSGCH_SOUND,info );
        }
        break;

    case SPWLD_BECKON:
        if (makes_noise)
            mpr(MSGCH_SOUND,"You hear a voice call your name." );
        break;

    case SPWLD_SHOUT:
        if (makes_noise)
            mpr(MSGCH_SOUND,"You hear a shout." );
        break;

    //case SPWLD_PRUNE:
    default:
        return;
    }

    if (makes_noise)
        noisy( SL_NOISY_WEAPON, you.x_pos, you.y_pos );

    if (old_plus != you.inv[wpn].plus
        || old_plus2 != you.inv[wpn].plus2
        || old_colour != you.inv[wpn].colour)
    {
        set_redraw_status( REDRAW_WIELD );
    }

    return;
}                               // end special_wielded()

bool reaching_weapon_attack( const int x, const int y )
{
    const int wpn = get_inv_wielded();
    ASSERT( wpn != -1 );  // reaching always from wielded weapon

    struct dist beam;
    int targ_mon = NON_MONSTER;

    if (!enough_mp( 1, false ))
        return (false);

    if (x == -1)  // called with default arguments (-1,-1)
    {
        mpr( MSGCH_PROMPT, "Attack whom?" );

        direction( beam, DIR_TARGET, TARG_ENEMY );
        if (!beam.isValid)
            return (false);

        if (beam.isMe)
        {
            canned_msg( MSG_UNTHINKING_ACT );
            return (false);
        }
    }
    else
    {
        beam.tx = x;
        beam.ty = y;
    }

    const int x_dist = abs( beam.tx - you.x_pos );
    const int y_dist = abs( beam.ty - you.y_pos );

    targ_mon = mgrd[beam.tx][beam.ty];

    if (x_dist > 2 || y_dist > 2)
        mpr( "Your weapon cannot reach that far!" );
    else if (x_dist < 2 && y_dist < 2)
        mpr( "That's not a reach!" );
    else
    {
        ASSERT( x_dist >= 0 && x_dist <= 2 );
        ASSERT( y_dist >= 0 && y_dist <= 2 );
        ASSERT( x_dist == 2 || y_dist == 2 );

        mpr( "You reach to attack!" );

        /* BCR - Added a check for monsters in the way.  Only checks cardinal
         *       directions.  Knight moves are ignored.  Assume the weapon
         *       slips between the squares.
         */

        if (x_dist != 1 && y_dist != 1)
        {
            // not a knight move attack (see above)
            ASSERT( x_dist == 0 || x_dist == 2 );
            ASSERT( y_dist == 0 || y_dist == 2 );

            const int mid_x = MAXIMUM( beam.tx, you.x_pos ) - (x_dist / 2);
            const int mid_y = MAXIMUM( beam.ty, you.y_pos ) - (y_dist / 2);

            const int mid_mon = mgrd[mid_x][mid_y];

            if (mid_mon != NON_MONSTER)
            {
                const int sk = weapon_skill( you.inv[wpn] );
                const int vis = player_monster_visible( &menv[mid_mon] );

                int to_miss = ((you.skills[sk] + 3) * you.dex) / (vis ? 5 : 20);

                // Lower evasion monsters are easier to predict and
                // thus easier to avoid.  We add some since attacking
                // around a monster is more difficult.
                if (!test_hit( to_miss, menv[mid_mon].evasion + 10 ))
                {
                    mpr( "%s gets in the way!",
                          (vis ? ptr_monam( &menv[mid_mon], DESC_CAP_THE )
                               : "Something") );

                    targ_mon = mid_mon;
                }
            }
        }

        // XXX: pass to-hit penalty to you_attack() based on mid_mon?
        if (targ_mon == NON_MONSTER)
            mpr( "You attack empty space." );
        else
            you_attack( targ_mon, false );

        dec_mp(1);
        make_hungry( 50 + 3 * (targ_mon == NON_MONSTER), false );

        if (one_chance_in(5))
            exercise( SK_EVOCATIONS, 1 );

        return (true);
    }

    return (false);
}                               // end reaching_weapon_attack()

// returns true if item successfully evoked.
bool evoke_wielded( void )
{
    char opened_gates = 0;
    unsigned char spell_casted = random2(21);
    int count_x, count_y;
    int temp_rand = 0;      // for probability determination {dlb}
    int power = 0;
    int mon;

    int pract = 0;
    bool did_work = false;  // used for default "nothing happens" message

    char str_pass[ ITEMNAME_SIZE ];

    int tool = get_inv_hand_tool();

    if (you.berserker)
    {
        canned_msg( MSG_TOO_BERSERK );
        return (false);
    }
    else if (you.conf)
    {
        canned_msg( MSG_TOO_CONFUSED );
        return (false);
    }
    else if (tool == -1)
    {
        mpr("You aren't wielding anything!");
        return (false);
    }

    switch (you.inv[tool].base_type)
    {
    case OBJ_WEAPONS:
        if (get_weapon_brand( you.inv[tool] ) == SPWPN_REACHING)
        {
            if (player_weapon_wielded())
            {
                reaching_weapon_attack();   // handles costs
                did_work = true;            // messages already handled
                pract = 0;                  // practise already handled
            }
        }
        else if (is_fixed_artefact( you.inv[tool] ))
        {
            switch (you.inv[tool].special)
            {
            case SPWPN_STAFF_OF_DISPATER:
                if (you.deaths_door || !enough_hp(11, true)
                    || !enough_mp(5, true))
                {
                    break;
                }

                mpr("You feel the staff feeding on your energy!");
                power = you.skills[SK_EVOCATIONS] * 8;

                if (your_spells( SPELL_HELLFIRE, power, false ))
                {
                    did_work = true;
                    pract = (coinflip() ? 2 : 1);

                    dec_hp( 3 + roll_dice(2,10), false );
                    dec_mp( 2 + roll_dice(2,4) );
                    make_hungry( 100, false );
                }
                break;

            // let me count the number of ways spell_casted is
            // used here ... one .. two .. three ... >CRUNCH<
            // three licks to get to the center of a ... {dlb}
            case SPWPN_SCEPTRE_OF_ASMODEUS:
                spell_casted = random2(21);  // use one, random number

                if (spell_casted == 0)
                    break;

                make_hungry( 200, false );
                pract = 1;

                if (spell_casted < 2)   // summon devils, maybe a Fiend
                {

                    // use two, type of demon to summon
                    spell_casted = (one_chance_in(4) ? MONS_FIEND
                                                 : MONS_HELLION + random2(10));


                    if (create_monster( spell_casted, BEH_HOSTILE, 6 ) != -1)
                    {
                        if (spell_casted == MONS_FIEND)
                            mpr("\"Your arrogance condemns you, mortal!\"");
                        else
                            mpr("The Sceptre summons one of its servants.");
                    }

                    did_work = true;
                    break;
                }

                temp_rand = random2(240);

                // use three, conjuration cast
                if (temp_rand > 125)
                    spell_casted = SPELL_BOLT_OF_FIRE;      // 114 in 240
                else if (temp_rand > 68)
                    spell_casted = SPELL_LIGHTNING_BOLT;    //  57 in 240
                else if (temp_rand > 11)
                    spell_casted = SPELL_BOLT_OF_DRAINING;  //  57 in 240
                else
                    spell_casted = SPELL_HELLFIRE;          //  12 in 240

                power = you.skills[SK_EVOCATIONS] * 8;

                if (your_spells( spell_casted, power, false ))
                    did_work = true;
                break;

            case SPWPN_STAFF_OF_OLGREB:
                if (!enough_mp( 4, true )
                    || you.skills[SK_EVOCATIONS] < random2(6))
                {
                    break;
                }

                dec_mp(4);
                make_hungry( 50, false );
                pract = 1;
                did_work = true;

                power = 10 + you.skills[SK_EVOCATIONS] * 8;

                // this cannot be aborted, and will not fail:
                your_spells( SPELL_OLGREBS_TOXIC_RADIANCE, power, false );

                if (you.skills[SK_EVOCATIONS] >= random2(10))
                    your_spells( SPELL_VENOM_BOLT, power, false );
                break;

            case SPWPN_STAFF_OF_WUCAD_MU:
                if (you.magic_points == you.max_magic_points
                    || you.skills[SK_EVOCATIONS] < random2(25))
                {
                    break;
                }

                mpr("Magical energy flows into your mind!");

                inc_mp( 3 + random2(5) + you.skills[SK_EVOCATIONS] / 3, false );
                make_hungry( 50, false );
                pract = 1;
                did_work = true;

                if (one_chance_in(3))
                {
                    miscast_effect( SPTYP_DIVINATION, random2(9),
                                    random2(70), 100, "the Staff of Wucad Mu" );
                }
                break;

            default:
                break;
            }
        }
        break;

    case OBJ_STAVES:
        if (item_is_rod( you.inv[tool] ))
        {
            pract = staff_spell( tool );
            did_work = true;  // staff_spell() will handle messages
        }
        else if (you.inv[tool].sub_type == STAFF_CHANNELING)
        {
            /* if (item_cursed( you.inv[tool] ))
            {
                mpr("You feel drained.");
                dec_mp( 3 + random2(6) );
            }
            else */
            if (you.magic_points < you.max_magic_points
                && you.skills[SK_EVOCATIONS] >= random2(30))
            {
                mpr("You channel some magical energy.");
                inc_mp( 1 + random2(3), false );
                make_hungry( 50, false );
                pract = (one_chance_in(5) ? 1 : 0);
                did_work = true;

                if (!item_ident( you.inv[tool], ISFLAG_KNOW_TYPE ))
                {
                    set_ident_flags( you.inv[tool], ISFLAG_KNOW_TYPE );

                    strcpy( info, "You are evoking " );
                    in_name( tool, DESC_NOCAP_A, str_pass );
                    strcat( info, str_pass );
                    strcat( info, "." );

                    mpr( info );
                    more();

                    set_redraw_status( REDRAW_WIELD );
                }
            }
        }
        break;

    case OBJ_MISCELLANY:
        did_work = true; // easier to do it this way for misc items
        switch (you.inv[tool].sub_type)
        {
        case MISC_BOTTLED_EFREET:
            if (efreet_flask())
                pract = 2;
            break;

        case MISC_CRYSTAL_BALL_OF_SEEING:
            if (ball_of_seeing())
                pract = 1;
            break;

        case MISC_AIR_ELEMENTAL_FAN:
            if (you.skills[SK_EVOCATIONS] < random2(30))
                canned_msg( MSG_NOTHING_HAPPENS );
            else if (enough_mp( 8, false ))
            {
                mon = summon_elemental( 100, MONS_AIR_ELEMENTAL, 4, true );

                if (mon > -2)  // not abort
                {
                    dec_mp(8);
                    pract = 1;

                    if (mon >= 0 && menv[mon].attitude == ATT_HOSTILE)
                        mons_add_ench( &menv[mon], ENCH_CHARM );
                }
            }
            break;

        case MISC_LAMP_OF_FIRE:
            if (you.skills[SK_EVOCATIONS] < random2(30))
                canned_msg( MSG_NOTHING_HAPPENS );
            else if (enough_mp( 8, false ))
            {
                mon = summon_elemental( 100, MONS_FIRE_ELEMENTAL, 4, true );

                if (mon > -2)  // not abort
                {
                    dec_mp(8);
                    pract = 1;

                    if (mon >= 0 && menv[mon].attitude == ATT_HOSTILE)
                        mons_add_ench( &menv[mon], ENCH_CHARM );
                }
            }
            break;

        case MISC_STONE_OF_EARTH_ELEMENTALS:
            if (you.skills[SK_EVOCATIONS] < random2(30))
                canned_msg(MSG_NOTHING_HAPPENS);
            else if (enough_mp( 8, false ))
            {
                mon = summon_elemental( 100, MONS_EARTH_ELEMENTAL, 4, true );

                if (mon > -2)  // not abort
                {
                    dec_mp(8);
                    pract = 1;

                    if (mon >= 0 && menv[mon].attitude == ATT_HOSTILE)
                        mons_add_ench( &menv[mon], ENCH_CHARM );
                }
            }
            break;

        case MISC_HORN_OF_GERYON:
            if (player_in_branch( BRANCH_VESTIBULE_OF_HELL ))
            {
                mpr("You produce a weird and mournful sound.");

                for (count_x = you.x_pos - SL_HORN;
                     count_x < you.x_pos + SL_HORN;
                     count_x++)
                {
                    for (count_y = you.y_pos - SL_HORN;
                         count_y < you.y_pos + SL_HORN;
                         count_y++)
                    {
                        if (!in_bounds( count_x, count_y ))
                            continue;

                        if (grd[count_x][count_y] == DNGN_STONE_ARCH)
                        {
                            opened_gates++;

                            // Note: This assumes that the Vestibule has
                            // not been changed.
                            switch (grd[count_x + 2][count_y])
                            {
                            case DNGN_FLOOR:
                                grd[count_x][count_y] = DNGN_ENTER_DIS;
                                break;
                            case DNGN_LAVA:
                                grd[count_x][count_y] = DNGN_ENTER_GEHENNA;
                                break;
                            case DNGN_STONE_WALL:
                                grd[count_x][count_y] = DNGN_ENTER_TARTARUS;
                                break;
                            case DNGN_DEEP_WATER:
                                grd[count_x][count_y] = DNGN_ENTER_COCYTUS;
                                break;
                            default:
                                grd[count_x][count_y] = DNGN_ENTER_ABYSS;
                                break;
                            }

                            set_terrain_changed( count_x, count_y );
                        }
                    }
                }

                if (opened_gates)
                {
                    mpr("Your way has been unbarred.");
                    pract = 1;
                }
            }
            else
            {
                mpr("You produce a hideous howling noise!");
                pract = (one_chance_in(3) ? 1 : 0);
                create_monster( MONS_BEAST, BEH_HOSTILE, 4 );
            }
            break;

        case MISC_DECK_OF_WONDERS:
            deck_of_cards(DECK_OF_WONDERS);
            pract = 1;
            break;

        case MISC_DECK_OF_SUMMONINGS:
            deck_of_cards(DECK_OF_SUMMONING);
            pract = 1;
            break;

        case MISC_DECK_OF_TRICKS:
            deck_of_cards(DECK_OF_TRICKS);
            pract = 1;
            break;

        case MISC_DECK_OF_POWER:
            deck_of_cards(DECK_OF_POWER);
            pract = 1;
            break;

        case MISC_BOX_OF_BEASTS:
            if (box_of_beasts())
                pract = 1;
            break;

        case MISC_CRYSTAL_BALL_OF_ENERGY:
            if (ball_of_energy())
                pract = 1;
            break;

        case MISC_CRYSTAL_BALL_OF_FIXATION:
            if (ball_of_fixation())
                pract = 1;
            break;

        case MISC_DISC_OF_STORMS:
            if (disc_of_storms())
                pract = (coinflip() ? 2 : 1);
            break;

        case MISC_PORTABLE_ALTAR_OF_NEMELEX:
            if (player_in_branch( BRANCH_ECUMENICAL_TEMPLE ))
            {
                mpr( "Don't you think this level already has more than enough altars?" );
            }
            else if (grd[you.x_pos][you.y_pos] != DNGN_FLOOR)
                mpr("You need a clear area to place this item.");
            else
            {
                mpr("You unfold the altar and place it on the floor.");
                grd[you.x_pos][you.y_pos] = DNGN_ALTAR_NEMELEX_XOBEH;
                you.nemelex_altar_index = you.equip[EQ_WEAPON];
                dec_inv_item_quantity( you.equip[EQ_WEAPON], 1 );
            }
            break;

        case MISC_ROD_OF_STRIKING:
            if (enough_mp( 1, false ))
            {
                dec_mp(1);
                make_hungry( 10, false );

                power = 3 * skill_bump( SK_EVOCATIONS );
                if (your_spells( SPELL_STRIKING, power, false ))
                    pract = 1;
            }
            break;

        default:
            did_work = false;
            break;
        }
        break;

    default:
        break;
    }

    if (!did_work)
        canned_msg( MSG_NOTHING_HAPPENS );
    else if (pract > 0)
        exercise( SK_EVOCATIONS, pract );

    you.turn_is_over = true;

    return (did_work);
}                               // end evoke_wielded()

static bool efreet_flask(void)
{
    const int flask = get_inv_hand_tool();
    ASSERT( flask != -1 );

    const beh_type behaviour = ((you.skills[SK_EVOCATIONS] > random2(20))
                                ? BEH_FRIENDLY : BEH_HOSTILE);

    mpr("You open the flask...");
    dec_inv_item_quantity( flask, 1 );

    if (create_monster( MONS_EFREET, behaviour, 5 ) != -1)
    {
        mpr( "...and a huge efreet comes out." );

        mpr( (behaviour == BEH_FRIENDLY) ? "\"Thank you for releasing me!\""
                                         : "It howls insanely!" );
    }
    else
        canned_msg(MSG_NOTHING_HAPPENS);

    return (true);
}                               // end efreet_flask()

static bool ball_of_seeing(void)
{
    int use = 0;
    bool ret = false;

    mpr("You gaze into the crystal ball.");

    use = ((!you.conf) ? random2(you.skills[SK_EVOCATIONS] * 6) : random2(5));

    if (use < 2)
    {
        lose_stat( STAT_INTELLIGENCE, 1 );
    }
    else if (use < 5 && enough_mp(1, true))
    {
        mpr("You feel power drain from you!");
        set_mp(0, false);
    }
    else if (use < 10)
    {
        confuse_player( 10 + random2(10), false );
    }
    else if (use < 15
             || you.level_type == LEVEL_LABYRINTH
             || you.level_type == LEVEL_ABYSS)
    {
        mpr("You see nothing.");
    }
    else
    {
        mpr("You see a map of your surroundings!");
        magic_mapping( 15 + you.skills[SK_EVOCATIONS],
                       45 + 2 * you.skills[SK_EVOCATIONS] );
        ret = true;
    }

    return (ret);
}                               // end ball_of_seeing()

static bool disc_of_storms(void)
{
    int temp_rand = 0;          // probability determination {dlb}
    struct bolt beam;
    int disc_count = 0;
    unsigned char which_zap = 0;

    const int fail_rate = (30 - you.skills[SK_EVOCATIONS]);
    bool ret = false;

    if (player_res_electricity() > 0 || (random2(100) < fail_rate))
        canned_msg(MSG_NOTHING_HAPPENS);
    else if (random2(100) < fail_rate)
        mpr("The disc glows for a moment, then fades.");
    else if (random2(100) < fail_rate)
        mpr("Little bolts of electricity crackle over the disc.");
    else
    {
        mpr("The disc erupts in an explosion of electricity!");

        disc_count = roll_dice( 2, 1 + you.skills[SK_EVOCATIONS] / 7 );

        while (disc_count)
        {
            temp_rand = random2(3);

            which_zap = ((temp_rand > 1) ? ZAP_LIGHTNING :
                         (temp_rand > 0) ? ZAP_ELECTRICITY
                                         : ZAP_ORB_OF_ELECTRICITY);

            beam.source_x = you.x_pos;
            beam.source_y = you.y_pos;
            beam.target_x = you.x_pos + random2(13) - 6;
            beam.target_y = you.y_pos + random2(13) - 6;

            zapping( which_zap, 30 + you.skills[SK_EVOCATIONS] * 2, beam );

            disc_count--;
        }

        ret = true;
    }

    return (ret);
}                               // end disc_of_storms()

void tome_of_power( int tome )
{
    ASSERT( tome != -1 );

    int temp_rand = 0;          // probability determination {dlb}

    int powc = 5 + you.skills[SK_EVOCATIONS]
                 + roll_dice( 5, you.skills[SK_EVOCATIONS] );

    int spell_casted = 0;
    struct bolt beam;

    strcpy(info, "The book opens to a page covered in ");

    char wc[30];
    weird_writing( wc );
    strcat( info, wc );
    strcat( info, "." );
    mpr( info );

    you.turn_is_over = true;

    if (!yesno("Read it?"))
        return;

    set_ident_flags( you.inv[tome], ISFLAG_IDENT_MASK );

    if (you.mutation[MUT_BLURRY_VISION] > 0
        && random2(4) < you.mutation[MUT_BLURRY_VISION])
    {
        mpr("The page is too blurry for you to read.");
        return;
    }

    mpr("You find yourself reciting the magical words!");
    exercise( SK_EVOCATIONS, 1 );

    temp_rand = random2(50) + random2( you.skills[SK_EVOCATIONS] / 3 );

    switch (temp_rand)
    {
    case 0:
    case 3:
    case 4:
    case 6:
    case 7:
    case 8:
    case 9:
        mpr("A cloud of weird smoke pours from the book's pages!");
        big_cloud( CLOUD_GREY_SMOKE + random2(3), you.x_pos, you.y_pos, 15,
                                                          10 + random2(8) );
        return;
    case 1:
    case 14:
        mpr("A cloud of choking fumes pours from the book's pages!");
        big_cloud(CLOUD_POISON, you.x_pos, you.y_pos, 15, 7 + random2(5));
        return;

    case 2:
    case 13:
        mpr("A cloud of freezing gas pours from the book's pages!");
        big_cloud(CLOUD_COLD, you.x_pos, you.y_pos, 15, 8 + random2(5));
        return;

    case 5:
    case 11:
    case 12:
        if (one_chance_in(5))
        {
            mpr("The book disappears in a mighty explosion!");
            dec_inv_item_quantity( tome, 1 );
        }

        beam.type = SYM_BURST;
        beam.damage = dice_def( 3, 15 );
        beam.flavour = BEAM_FIRE;
        beam.is_explosion = true;
        beam.target_x = you.x_pos;
        beam.target_y = you.y_pos;
        strcpy( beam.name, "fiery explosion" );
        beam.colour = RED;
        // your explosion, (not someone else's explosion)
        beam.beam_source = MHITYOU;
        beam.thrower = KILL_YOU;
        beam.aux_source = "an exploding Tome of Power";
        beam.ex_size = 2;
        beam.is_tracer = false;

        explosion(beam);
        return;


    case 10:
        if (create_monster( MONS_ABOMINATION_SMALL, BEH_HOSTILE, 6 ) != -1)
        {
            mpr("A horrible Thing appears!");
            mpr("It doesn't look too friendly.");
        }
        return;
    }

    viewwindow(true, false);

    temp_rand = random2(23) + random2( you.skills[SK_EVOCATIONS] / 3 );

    if (temp_rand > 25)
        temp_rand = 25;

    spell_casted = ((temp_rand > 19) ? SPELL_FIREBALL :
                    (temp_rand > 16) ? SPELL_BOLT_OF_FIRE :
                    (temp_rand > 13) ? SPELL_BOLT_OF_COLD :
                    (temp_rand > 11) ? SPELL_LIGHTNING_BOLT :
                    (temp_rand > 10) ? SPELL_LEHUDIBS_CRYSTAL_SPEAR :
                    (temp_rand >  9) ? SPELL_VENOM_BOLT :
                    (temp_rand >  8) ? SPELL_BOLT_OF_DRAINING :
                    (temp_rand >  7) ? SPELL_BOLT_OF_INACCURACY :
                    (temp_rand >  6) ? SPELL_STICKY_FLAME :
                    (temp_rand >  5) ? SPELL_TELEPORT_SELF :
                    (temp_rand >  4) ? SPELL_CIGOTUVIS_DEGENERATION :
                    (temp_rand >  3) ? SPELL_POLYMORPH_OTHER :
                    (temp_rand >  2) ? SPELL_MEPHITIC_CLOUD :
                    (temp_rand >  1) ? SPELL_THROW_FLAME :
                    (temp_rand >  0) ? SPELL_THROW_FROST
                                     : SPELL_MAGIC_DART);

    if (your_spells( spell_casted, powc, false ))
        exercise( SK_EVOCATIONS, 2 );
}                               // end tome_of_power()

void skill_manual( int manual )
{
    ASSERT( manual != -1 );

    char skname[80];

    set_ident_flags( you.inv[manual], ISFLAG_IDENT_MASK );

    strncpy( skname, skill_name( you.inv[manual].plus ), sizeof(skname) );

    mpr( "This is a manual of %s!", skname );

    you.turn_is_over = true;

    if (!yesno("Read it?"))
        return;

    mpr( "You read about %s.", strlwr(skname) );

    exercise( you.inv[manual].plus, 500 );

    if (one_chance_in(10))
    {
        mpr("The book crumbles into dust.");
        dec_inv_item_quantity( manual, 1 );
    }
    else
    {
        mpr("The book looks somewhat more worn.");
    }
}                               // end skill_manual()

static bool box_of_beasts(void)
{
    const int box = get_inv_hand_tool();
    ASSERT( box != -1 );

    int beasty = MONS_PROGRAM_BUG;      // error trapping {dlb}
    int temp_rand = 0;          // probability determination {dlb}

    int ret = false;

    mpr("You open the lid...");

    if (random2(100) < 60 + you.skills[SK_EVOCATIONS])
    {
        temp_rand = random2(11);

        beasty = ((temp_rand == 0) ? MONS_GIANT_BAT :
                  (temp_rand == 1) ? MONS_HOUND :
                  (temp_rand == 2) ? MONS_JACKAL :
                  (temp_rand == 3) ? MONS_RAT :
                  (temp_rand == 4) ? MONS_ICE_BEAST :
                  (temp_rand == 5) ? MONS_SNAKE :
                  (temp_rand == 6) ? MONS_YAK :
                  (temp_rand == 7) ? MONS_BUTTERFLY :
                  (temp_rand == 8) ? MONS_HELL_HOUND :
                  (temp_rand == 9) ? MONS_BROWN_SNAKE
                                   : MONS_GIANT_LIZARD);

        beh_type beh = (one_chance_in(you.skills[SK_EVOCATIONS] + 5) ? BEH_HOSTILE
                                                                     : BEH_FRIENDLY);

        if (create_monster( beasty, beh, 2 + random2(4) ) != -1)
        {
            mpr("...and something leaps out!");
            ret = true;
        }
    }
    else
    {
        if (!one_chance_in(6))
            mpr("...but nothing happens.");
        else
        {
            mpr("...but the box appears empty.");
            you.inv[box].sub_type = MISC_EMPTY_EBONY_CASKET;
        }
    }

    return (ret);
}                               // end box_of_beasts()

static bool ball_of_energy(void)
{
    int use = 0;
    int proportional = 0;

    bool ret = false;

    mpr("You gaze into the crystal ball.");

    use = ((!you.conf) ? random2(you.skills[SK_EVOCATIONS] * 6) : random2(6));

    if (use < 2 || you.max_magic_points == 0)
    {
        lose_stat(STAT_INTELLIGENCE, 1);
    }
    else if ((use < 4 && enough_mp(1, true))
             || you.magic_points == you.max_magic_points)
    {
        mpr( "You feel your power drain away!" );
        set_mp( 0, false );
    }
    else if (use < 6)
    {
        confuse_player( 10 + random2(10), false );
    }
    else
    {
        proportional = you.magic_points * 100;
        proportional /= you.max_magic_points;

        if (roll_zdice( 2, 39 - skill_bump(SK_EVOCATIONS) ) > proportional
            || one_chance_in(25))
        {
            mpr( "You feel your power drain away!" );
            set_mp( 0, false );
        }
        else
        {
            mpr( "You are suffused with power!" );
            inc_mp( 6 + roll_dice( 2, you.skills[SK_EVOCATIONS] ), false );

            ret = true;
        }
    }

    return (ret);
}                               // end ball_of_energy()

static bool ball_of_fixation(void)
{
    mpr("You gaze into the crystal ball.");
    mpr("You are mesmerised by a rainbow of scintillating colours!");

    you.paralysis = 100;
    you.slow = 100;

    return (true);
}                               // end ball_of_fixation()

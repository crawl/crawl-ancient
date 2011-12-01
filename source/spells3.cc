/*
 *  File:       spells3.cc
 *  Summary:    Implementations of some additional spells.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <2>     9/11/99        LRH    Teleportation takes longer in the Abyss
 *      <2>     8/05/99        BWR    Added allow_control_teleport
 *      <1>     -/--/--        LRH    Created
 */

#include "AppHdr.h"
#include "spells3.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "externs.h"

#include "abyss.h"
#include "beam.h"
#include "cloud.h"
#include "direct.h"
#include "debug.h"
#include "delay.h"
#include "itemname.h"
#include "itemprop.h"
#include "items.h"
#include "it_use2.h"
#include "misc.h"
#include "monplace.h"
#include "mon-pick.h"
#include "monstuff.h"
#include "mon-util.h"
#include "ouch.h"
#include "player.h"
#include "randart.h"
#include "spells1.h"
#include "spl-cast.h"
#include "spl-util.h"
#include "stuff.h"
#include "view.h"

static bool monster_on_level(int monster);

void cast_selective_amnesia(bool force)
{
    int ep_gain = 0;
    unsigned char keyin = 0;

    if (you.spell_no == 0)
        mpr("You don't know any spells.");      // re: sif muna {dlb}
    else
    {
        // query - conditional ordering is important {dlb}:
        for (;;)
        {
            mpr( MSGCH_PROMPT, "Forget which spell ([?*] list [ESC] exit)? " );

            keyin = static_cast<unsigned char>( get_ch() );

            if (keyin == ESCAPE)
                return;         // early return {dlb}

            if (keyin == '?' || keyin == '*')
            {
                // this reassignment is "key" {dlb}
                keyin = static_cast<unsigned char>( list_spells() );

                redraw_screen();
            }

            if (!isalpha( keyin ))
                mesclr( true );
            else
                break;
        }

        // actual handling begins here {dlb}:
        const int spell = get_spell_by_letter( keyin );
        const int slot  = get_spell_slot_by_letter( keyin );

        if (spell == SPELL_NO_SPELL)
            mpr( "You don't know that spell." );
        else
        {
            if (!force
                 && (you.religion != GOD_SIF_MUNA
                     && random2(you.skills[SK_SPELLCASTING])
                         < random2(spell_level( spell ))))
            {
                mpr("Oops! This spell sure is a blunt instrument.");
                forget_map(20 + random2(50));
            }
            else
            {
                ep_gain = spell_mana( spell );
                del_spell_from_memory_by_slot( slot );

                if (ep_gain > 0)
                {
                    inc_mp(ep_gain, false);
                    mpr( "The spell releases its latent energy back to you as it unravels." );
                }
            }
        }
    }

    return;
}                               // end cast_selective_amnesia()

bool remove_curse( bool suppress_msg )
{
    int loopy = 0;              // general purpose loop variable {dlb}
    bool success = false;       // whether or not curse(s) removed {dlb}

    const int tool = get_inv_hand_tool();
    if (tool != -1 && item_cursed( you.inv[tool] ))
    {
        do_uncurse_item( you.inv[tool] );
        success = true;
        set_redraw_status( REDRAW_WIELD );
    }

    // everything else uses the same paradigm - are we certain?
    // what of artefact rings and amulets? {dlb}:
    for (loopy = EQ_CLOAK; loopy < NUM_EQUIP; loopy++)
    {
        if (you.equip[loopy] != -1 && item_cursed(you.inv[you.equip[loopy]]))
        {
            do_uncurse_item( you.inv[you.equip[loopy]] );
            success = true;
        }
    }

    // messaging output {dlb}:
    if (!suppress_msg)
    {
        if (success)
            mpr("You feel as if something is helping you.");
        else
            canned_msg( MSG_NOTHING_HAPPENS );
    }

    return (success);
}                               // end remove_curse()

bool detect_curse(bool suppress_msg)
{
    int loopy = 0;              // general purpose loop variable {dlb}
    bool success = false;       // whether or not any curses found {dlb}

    for (loopy = 0; loopy < ENDOFPACK; loopy++)
    {
        if (you.inv[loopy].quantity
            && (you.inv[loopy].base_type == OBJ_WEAPONS
                || you.inv[loopy].base_type == OBJ_ARMOUR
                || you.inv[loopy].base_type == OBJ_JEWELLERY))
        {
            if (!item_ident( you.inv[loopy], ISFLAG_KNOW_CURSE ))
                success = true;

            set_ident_flags( you.inv[loopy], ISFLAG_KNOW_CURSE );
        }
    }

    // messaging output {dlb}:
    if (!suppress_msg)
    {
        if (success)
            mpr("You sense the presence of curses on your possessions.");
        else
            canned_msg( MSG_NOTHING_HAPPENS );
    }

    return (success);
}                               // end detect_curse()

int cast_smiting( int power )
{
    struct dist beam;

    mpr( MSGCH_PROMPT, "Smite whom?" );

    direction( beam, DIR_TARGET, TARG_ENEMY );

    if (!beam.isValid || beam.isMe)
    {
        canned_msg( MSG_SPELL_FIZZLES );
        return (SPRET_ABORT);
    }

    const int mon = mgrd[beam.tx][beam.ty];

    if (mon == NON_MONSTER)
    {
        canned_msg( MSG_NOTHING_HAPPENS );
        return (SPRET_FAIL);
    }

    struct monsters *const monster = &menv[mon];
    const int dam = roll_dice( 3, 5 + power / 5 );

    zap_animation( element_colour( EC_HOLY ), monster );
    mon_msg( monster, "You smite %s!", false );

    you_hurt_monster( monster, dam );

    return (SPRET_SUCCESS);
}                               // end cast_smiting()

int airstrike( int power, const struct bolt &beam )
{
    const int mon = mgrd[beam.target_x][beam.target_y];

    if (mon == NON_MONSTER)
    {
        canned_msg( MSG_NOTHING_HAPPENS );
        return (SPRET_FAIL);
    }

    struct monsters *const monster = &menv[mon];

    zap_animation( element_colour( EC_AIR ), monster );

    mon_msg( monster, "The air twists around and strikes %s!", false );

    int hurted = roll_dice( 3, 5 + power / 10 );

    hurted = apply_mons_armour( hurted, monster, true );

    you_hurt_monster( monster, hurted );

    return (SPRET_SUCCESS);
}                               // end airstrike()

int cast_bone_shards( int power )
{
    int ret = SPRET_ABORT;
    struct bolt beam;
    struct dist spelld;

    const int skel = get_inv_in_hand();

    if (skel == -1 || you.inv[skel].base_type != OBJ_CORPSES)
        canned_msg( MSG_SPELL_FIZZLES );
    else if (you.inv[skel].sub_type != CORPSE_SKELETON)
        mpr("The corpse collapses into a mass of pulpy flesh.");
    else if (spell_direction( spelld, beam ))
    {
        power += mons_weight( you.inv[skel].plus ) / 10;
        power = stepdown_value( power, 50, 50, 150, 150 );

        mpr("The skeleton explodes into sharp fragments of bone!");

        dec_inv_item_quantity( skel, 1 );
        zapping( ZAP_BONE_SHARDS, power, beam );

        ret = SPRET_SUCCESS;
    }

    return (ret);
}                               // end cast_bone_shards()

void sublimation(int power)
{
    unsigned char loopy = 0;    // general purpose loop variable {dlb}

    const int chunk = get_inv_in_hand();

    if (chunk != -1
        && you.inv[chunk].base_type == OBJ_FOOD
        && you.inv[chunk].sub_type == FOOD_CHUNK)
    {
        mpr("The chunk of flesh you are holding crumbles to dust.");
        mpr("A flood of magical energy pours into your mind!");

        const int amount = 7 * (you.inv[chunk].special >= 100) + random2(7);
        inc_mp( amount, false );

        dec_inv_item_quantity( chunk, 1 );
    }
    else
    {
        if (you.deaths_door)
        {
            mpr( "A conflicting enchantment prevents the spell from coming into effect." );
        }
        else if (!enough_hp( 2, true ))
        {
             mpr("Your attempt to draw power from your own body fails.");
        }
        else
        {
            mpr("You draw magical energy from your own body!");

            while (you.magic_points < you.max_magic_points && you.hp > 1)
            {
                inc_mp( 1, false );
                dec_hp( 2, false );

                for (loopy = 0; loopy < (you.hp > 1 ? 3 : 0); loopy++)
                {
                    if (random2(power) < 6)
                        dec_hp( coinflip() ? 1 : 2, false );
                }

                if (random2(power) < 6)
                    break;
            }
        }
    }

    return;
}                               // end sublimation()

// Simulacrum
//
// This spell extends creating undead to Ice mages, as such it's high
// level, requires wielding of the material component, and the undead
// aren't overly powerful (they're also vulnerable to fire).  I've put
// back the abjuration level in order to keep down the army sizes again.
//
// As for what it offers necromancers considering all the downsides
// above... it allows the turning of a single corpse into an army of
// monsters (one per food chunk)... which is also a good reason for
// why it's high level.
//
// Hides and other "animal part" items are intentionally left out, it's
// unrequired complexity, and fresh flesh makes more "sense" for a spell
// reforming the original monster out of ice anyways.
void simulacrum( int power )
{
    int max_num = 2 + random2(power) / 20;
    if (max_num > 8)
        max_num = 8;

    const int chunk = get_inv_in_hand();

    if (chunk != -1
        && is_valid_item( you.inv[chunk] )
        && (you.inv[chunk].base_type == OBJ_CORPSES
            || (you.inv[chunk].base_type == OBJ_FOOD
                && you.inv[chunk].sub_type == FOOD_CHUNK)))
    {
        const int mons_type = you.inv[chunk].plus;

        // Can't create more than the available chunks
        if (you.inv[chunk].quantity < max_num)
            max_num = you.inv[chunk].quantity;

        dec_inv_item_quantity( chunk, max_num );

        int summoned = 0;

        for (int i = 0; i < max_num; i++)
        {
            int mon = create_monster( MONS_SIMULACRUM_SMALL, BEH_FRIENDLY, 6,
                                      you.x_pos, you.y_pos, you.pet_target,
                                      mons_type );

            if (mon != -1)
                summoned++;
        }

        if (!summoned)
            mpr( "You feel cold for a second." );
        else
        {
            strcpy( info, (summoned == 1) ? "An icy figure forms "
                                          : "Some icy figures form " );
            strcat( info, "before you!" );
            mpr( info );
        }
    }
    else
    {
        mpr( "You need to wield a piece of raw flesh for this spell to be effective!" );
    }
}                               // end sublimation()

void dancing_weapon(int pow, bool force_hostile)
{
    int numsc = 2 + (random2(pow) / 5);
    char str_pass[ ITEMNAME_SIZE ];

    if (numsc > 6)
        numsc = 6;

    int i;
    int summs = 0;
    beh_type behavi = BEH_FRIENDLY;

    const int wpn = get_inv_wielded();

    // See if wielded item is appropriate:
    if (wpn == -1
        || you.inv[wpn].base_type != OBJ_WEAPONS
        || is_range_weapon( you.inv[wpn] )
        || is_fixed_artefact( you.inv[wpn] ))
    {
        goto failed_spell;
    }

    // See if we can get an mitm for the dancing weapon:
    i = get_item_slot();
    if (i == NON_ITEM)
        goto failed_spell;

    // cursed weapons become hostile
    if (item_cursed( you.inv[wpn] ) || force_hostile)
        behavi = BEH_HOSTILE;

    summs = create_monster( MONS_DANCING_WEAPON, behavi, numsc,
                            you.x_pos, you.y_pos, you.pet_target, 1 );

    if (summs < 0)
    {
        // must delete the item before failing!
        mitm[i].base_type = OBJ_UNASSIGNED;
        mitm[i].quantity = 0;
        goto failed_spell;
    }

    // We are successful:
    unwield_item();                        // remove wield effects

    // copy item (done here after any wield effects are removed)
    mitm[i] = you.inv[wpn];
    mitm[i].quantity = 1;
    mitm[i].x = 0;
    mitm[i].y = 0;
    mitm[i].link = NON_ITEM;

    in_name( wpn, DESC_CAP_YOUR, str_pass );
    strcpy( info, str_pass );
    strcat( info, " dances into the air!" );
    mpr( info );

    you.inv[ wpn ].quantity = 0;

    menv[summs].inv[MSLOT_WEAPON] = i;
    menv[summs].colour = mitm[i].colour;
    return;

failed_spell:
    mpr("Your weapon vibrates crazily for a second.");
}                               // end dancing_weapon()

static bool monster_on_level(int monster)
{
    for (int i = 0; i < MAX_MONSTERS; i++)
    {
        if (menv[i].type == monster)
            return true;
    }

    return false;
}                               // end monster_on_level()

//
// This function returns true if the player can use controlled
// teleport here.
//
bool allow_control_teleport( bool silent )
{
    bool ret = true;

    if (you.level_type == LEVEL_ABYSS || you.level_type == LEVEL_LABYRINTH)
        ret = false;
    else if (you.level_type == LEVEL_DUNGEON)
    {
        switch (you.where_are_you)
        {
        case BRANCH_TOMB:
            // The tomb is a laid out 3D maze, it'd be a shame if the
            // player just teleports through any of it... so we only
            // allow teleport once they have the rune.
            ret = false;
            for (int i = 0; i < ENDOFPACK; i++)
            {
                if (is_valid_item( you.inv[i] )
                    && you.inv[i].base_type == OBJ_MISCELLANY
                    && you.inv[i].sub_type == MISC_RUNE_OF_ZOT
                    && you.inv[i].plus == BRANCH_TOMB)
                {
                    ret = true;
                    break;
                }
            }
            break;

        case BRANCH_SLIME_PITS:
            // Cannot teleport into the slime pit vaults until
            // royal jelly is gone.
            if (monster_on_level(MONS_ROYAL_JELLY))
                ret = false;
            break;

        case BRANCH_ELVEN_HALLS:
            // Cannot raid the elven halls while magic fountains aren't drained
            if (you.branch_stairs[STAIRS_ELVEN_HALLS] +
                    branch_depth(STAIRS_ELVEN_HALLS) == you.depth)
            {
                for (int x = X_BOUND_1 + 1; x < X_BOUND_2; x++)
                {
                    for (int y = Y_BOUND_1 + 1; y < Y_BOUND_2; y++)
                    {
                        if (grd[x][y] == DNGN_SPARKLING_FOUNTAIN)
                            ret = false;
                    }
                }
            }
            break;

        case BRANCH_HALL_OF_ZOT:
            // Cannot control teleport until the Orb is picked up
            if (you.branch_stairs[STAIRS_HALL_OF_ZOT] +
                    branch_depth(STAIRS_HALL_OF_ZOT) == you.depth
                && you.char_direction != DIR_ASCENDING)
            {
                ret = false;
            }
            break;
        }
    }

    // Tell the player why if they have teleport control.
    if (!ret && !silent && player_teleport_control())
        mpr( "A powerful magic prevents control of your teleportation." );

    return (ret);
}                               // end allow_control_teleport()

void you_teleport(void)
{
    if (scan_randarts( RAP_PREVENT_TELEPORTATION ))
        mpr("You feel a weird sense of stasis.");
    else if (you.duration[DUR_TELEPORT])
    {
        mpr("You feel strangely stable.");
        you.duration[DUR_TELEPORT] = 0;
    }
    else
    {
        mpr("You feel strangely unstable.");

        you.duration[DUR_TELEPORT] = 3 + random2(3);

        if (you.level_type == LEVEL_ABYSS && !one_chance_in(5))
        {
            mpr("You have a feeling this translocation may take a while to kick in...");
            you.duration[DUR_TELEPORT] += 5 + random2(10);
        }
    }

    return;
}                               // end you_teleport()

static void scatter_point( int &x, int &y )
{
    // was 100% chance
    if (random2(100) > 2 * you.skills[SK_TRANSLOCATIONS])
    {
        x += random2(3) - 1;
        y += random2(3) - 1;
    }

    // was 25% chance
    if (random2(100) < 40 - you.skills[SK_TRANSLOCATIONS])
    {
        x += random2(3) - 1;
        y += random2(3) - 1;
    }
}

void you_teleport2( bool allow_control, bool new_abyss_area )
{
    const int control = player_teleport_control();
    bool is_controlled = (allow_control
                            && control
                            && player_sound_mind_and_body()
                            && allow_control_teleport());

    if (scan_randarts( RAP_PREVENT_TELEPORTATION ))
    {
        mpr("You feel a strange sense of stasis.");
        return;
    }

    // after this point, we're guaranteed to teleport. Turn off auto-butcher.
    // corpses still get butchered,  but at least we don't get a silly message.
    if (current_delay_action() == DELAY_BUTCHER)
        stop_delay();

    interrupt_activity( AI_TELEPORT );

    expose_player_to_element( BEAM_TELEPORT );

    if (you.level_type == LEVEL_ABYSS)
    {
        abyss_teleport( new_abyss_area );
        you.pet_target = MHITNOT;
        return;
    }

    FixedVector< int, 2 >  plox;

    if (is_controlled)
    {
        mpr("You may choose your destination (press '.', return, or space to select).");

        mpr( "Expect %s deviation.", (control >= 2) ? "minor" : "some" );
        more();

        is_controlled = show_map( plox );
        redraw_screen();

        if (is_controlled)
        {
            const int targ_x = plox[0];
            const int targ_y = plox[1];

            // Scatter target if player tried to target (ie did not ESCAPE).
            // Attempted control teleport contaminates the player... note
            // that hitting escape when asked will avoid this, so uncontrolled
            // teleport is always an option. -- bwr
            contaminate_player(1);

            if (control == 1)
            {
                plox[0] += random2(9) - 4;
                plox[1] += random2(9) - 4;
            }

            do
            {
                scatter_point( plox[0], plox[1] );
            }
            while (control == 1 && coinflip());

            // control fails and teleport is redirected if:
            // - grid is out of bounds (thus in solid permanent rock)
            // - grid is hostile (deep water and lava)
            // - something is there (solid rock, unsubmerged monster, cloud)
            // - scattering blocked
            if (!in_bounds( plox[0], plox[1] )
                || grid_destroys_items( grd[ plox[0] ][ plox[1] ] )
                || something_there( plox[0], plox[1] )
                || !check_line_of_sight( targ_x, targ_y, plox[0], plox[1] ))
            {
                mpr( "Your rematerialisation was disrupted by something!" );
                is_controlled = false;
            }
        }

        // if it's still controlled, move the player... else fall through
        if (is_controlled)
            move_player_to_grid( plox[0], plox[1], false, true, true );
    }                           // end "if is_controlled"

    if (!is_controlled)
    {
        int count = 0;

        do
        {
            random_in_bounds( plox[0], plox[1] );
            count++;
        }
        while (count < 1000
                && (something_there( plox[0], plox[1] )
                    || grid_distance( you.x_pos, you.y_pos, plox[0], plox[1] ) <= 1));

        if (count == 1000)
            canned_msg( MSG_SPELL_FIZZLES );
        else
        {
            mpr("Your surroundings suddenly seem different.");
            move_player_to_grid( plox[0], plox[1], false, true, true );
        }
    }
}                               // end you_teleport()

int blink( void )
{
    struct dist beam;

    // yes, there is a logic to this ordering {dlb}:
    if (scan_randarts( RAP_PREVENT_TELEPORTATION ))
        mpr("You feel a weird sense of stasis.");
    else if (you.level_type == LEVEL_ABYSS && one_chance_in(3))
        mpr("The power of the Abyss keeps you in your place!");
    else if (you.conf)
        random_blink(0);
    else if (!allow_control_teleport(true))
    {
        mpr("A powerful magic interferes with your control of the blink.");
        random_blink(0);
    }
    else
    {
        // query for location {dlb}:
        for (;;)
        {
            mpr(MSGCH_PROMPT,"Blink to where?" );

            direction( beam, DIR_TARGET );

            if (!beam.isValid)
            {
                canned_msg(MSG_SPELL_FIZZLES);
                return (SPRET_ABORT);         // early return {dlb}
            }

            if (see_grid( beam.tx, beam.ty ))
                break;
            else
            {
                mesclr();
                mpr("You can't blink there!");
            }
        }

        scatter_point( beam.tx, beam.ty );

        if (!see_grid( beam.tx, beam.ty ) || something_there( beam.tx, beam.ty ))
        {
            mpr( "Your rematerialisation was disrupted by something!" );
            random_blink(0);
        }
        else if (you.level_type == LEVEL_ABYSS)
        {
            if (beam.tx >= X_ABYSS_1 && beam.tx <= X_ABYSS_2
                && beam.ty >= Y_ABYSS_1 && beam.ty <= Y_ABYSS_2)
            {
                if (!move_player_to_grid( beam.tx, beam.ty, false, true, true ))
                    random_blink(0);
                else
                    contaminate_player( 1 );
            }
            else
            {
                abyss_teleport( false );
                you.pet_target = MHITNOT;
            }
        }
        else
        {
            // controlling teleport contaminates the player -- bwr
            if (!move_player_to_grid( beam.tx, beam.ty, false, true, true ))
                random_blink(0);
            else
                contaminate_player( 1 );
        }

        // condensation shield always goes down when teleporting:
        expose_player_to_element( BEAM_TELEPORT );
    }

    return (SPRET_SUCCESS);
}                               // end blink()

void random_blink( int power )
{
    // low power would give us such a scatter that it wouldn't matter anyways
    const bool allow_partial_control = (power >= 20);
    const bool with_min = (you.mutation[MUT_BLINK] > 1);
    const bool los_only = (you.mutation[MUT_BLINK] < 3 || coinflip());

    int tx, ty;
    bool succ = false;

    if (scan_randarts( RAP_PREVENT_TELEPORTATION ))
        mpr("You feel a weird sense of stasis.");
    else if (you.level_type == LEVEL_ABYSS && one_chance_in(3))
    {
        mpr("The power of the Abyss keeps you in your place!");
    }
    else if (!random_near_space(you.x_pos, you.y_pos, tx, ty, with_min, los_only))
    {
        mpr("You feel jittery for a moment.");
    }
#ifdef USE_SEMI_CONTROLLED_BLINK
    //jmf: add back control, but effect is cast_semi_controlled_blink(pow)
    else if (player_teleport_control() >= 3
                && player_sound_mind_and_body()
                && allow_partial_control
                && allow_control_teleport())
    {
        mpr( MSGCH_PROMPT, "You may select the general direction of your translocation." );

        if (cast_semi_controlled_blink( power ))
            contaminate_player(1);

        succ = true;
    }
#endif
    else
    {
        mpr("You blink.");

        succ = true;

        if (you.level_type != LEVEL_ABYSS
            || (tx >= X_ABYSS_1 && tx <= X_ABYSS_2
                && ty >= Y_ABYSS_1 && ty <= Y_ABYSS_2))
        {
            move_player_to_grid( tx, ty, false, true, true );
        }
        else
        {
            abyss_teleport( false );
            you.pet_target = MHITNOT;
        }
    }

    // condensation shield always goes down when teleporting:
    expose_player_to_element( BEAM_TELEPORT );

    return;
}                               // end random_blink()

// Remember that this is lucky to exist at all... teleport control + blink
// was removed as a far too powerful combo, and so this function is more
// about "quaint feature" than it is about supplying a serious option.
static int quadrant_blink( int x, int y, int pow, int garbage )
{
    UNUSED( garbage );

    if (x == you.x_pos && y == you.y_pos)
        return (0);

    if (pow > 100)
        pow = 100;

    int ret = 1;

    const int l = x - you.x_pos;
    const int m = y - you.y_pos;
    const int r = 3 + random2(6);

    int bx = you.x_pos + r * l;
    int by = you.y_pos + r * m;

    if (r > 5)
    {
        bx += random2(3) - 1;
        by += random2(3) - 1;
    }

    do
    {
        scatter_point( bx, by );
    }
    while (pow < random2( 100 + 5 * r * r ));

    if (!in_bounds( bx, by )
        || grid_distance( you.x_pos, you.y_pos, bx, by ) <= 1
        || grid_destroys_items( grd[bx][by] )
        || something_there( bx, by )
        || !check_line_of_sight( you.x_pos, you.y_pos, bx, by ))
    {
        const int ox = you.x_pos;
        const int oy = you.y_pos;

        random_blink(0);
        ret = 0;

        const int dx = (you.x_pos - ox);
        const int dy = (you.y_pos - oy);

        if ((l > 0 && dx < 0)
            || (l < 0 && dx > 0)
            || (m > 0 && dy < 0)
            || (m < 0 && dy > 0))
        {
            // only give message if we when in an unexpected direction
            mpr( "Your rematerialisation was disrupted and redirected!" );
        }
        else
        {
            // Pretend we really generated the result, even though it
            // only happened by chance here.
            contaminate_player( one_chance_in(3) );
        }
    }
    else if (you.level_type != LEVEL_ABYSS
            || (bx >= X_ABYSS_1 && bx <= X_ABYSS_2
                && by >= Y_ABYSS_1 && by <= Y_ABYSS_2))
    {
        move_player_to_grid( bx, by, false, true, true );
        contaminate_player( one_chance_in(3) );
    }
    else
    {
        abyss_teleport( false );
        you.pet_target = MHITNOT;
    }

    // condensation shield always goes down when teleporting:
    if (ret)
        expose_player_to_element( BEAM_TELEPORT );

    return (ret);
}

int cast_semi_controlled_blink( int pow )
{
    return (apply_one_neighbouring_square( quadrant_blink, pow ));
}

bool cast_entomb( int power )
{
    int loopy = 0;              // general purpose loop variable {dlb}
    bool proceed = false;       // loop management varaiable {dlb}
    int which_trap = 0;         // used in clearing out certain traps {dlb}
    char srx = 0, sry = 0;
    char number_built = 0;

    FixedVector < int , 7 > safe_to_overwrite;

    // hack - passing chars through '...' promotes them to ints, which
    // barfs under gcc in fixvec.h.  So don't.
    safe_to_overwrite[0] = DNGN_FLOOR;
    safe_to_overwrite[1] = DNGN_SHALLOW_WATER;
    safe_to_overwrite[2] = DNGN_OPEN_DOOR;
    safe_to_overwrite[3] = DNGN_TRAP_MECHANICAL;
    safe_to_overwrite[4] = DNGN_TRAP_MAGICAL;
    safe_to_overwrite[5] = DNGN_TRAP_III;
    safe_to_overwrite[6] = DNGN_UNDISCOVERED_TRAP;


    for (srx = you.x_pos - 1; srx < you.x_pos + 2; srx++)
    {
        for (sry = you.y_pos - 1; sry < you.y_pos + 2; sry++)
        {
            proceed = false;

            // tile already occupied by monster or yourself {dlb}:
            if (mgrd[srx][sry] != NON_MONSTER
                    || (srx == you.x_pos && sry == you.y_pos))
            {
                continue;
            }

            if (random2(power) < 10)
                continue;

            // the break here affects innermost containing loop {dlb}:
            for (loopy = 0; loopy < 7; loopy++)
            {
                if (grd[srx][sry] == safe_to_overwrite[loopy])
                {
                    proceed = true;
                    break;
                }
            }

            // checkpoint one - do we have a legitimate tile? {dlb}
            if (!proceed)
                continue;

            int objl = igrd[srx][sry];
            int hrg = 0;

            while (objl != NON_ITEM)
            {
                // hate to see the orb get  detroyed by accident {dlb}:
                if (mitm[objl].base_type == OBJ_ORBS)
                {
                    proceed = false;
                    break;
                }

                hrg = mitm[objl].link;
                objl = hrg;
            }

            // checkpoint two - is the orb resting in the tile? {dlb}:
            if (!proceed)
                continue;

            objl = igrd[srx][sry];
            hrg = 0;

            while (objl != NON_ITEM)
            {
                hrg = mitm[objl].link;
                destroy_item(objl);
                objl = hrg;
            }

            // deal with clouds {dlb}:
            if (is_cloud( srx, sry ))
                delete_cloud( env.cgrid[srx][sry] ); // delete req's id

            // mechanical traps are destroyed {dlb}:
            which_trap = trap_at_xy( srx, sry );
            if (which_trap != -1)
            {
                const int cat = trap_category( env.trap[which_trap].type );

                if (cat == DNGN_TRAP_MECHANICAL)
                    remove_trap( which_trap );
            }

            // finally, place the wall {dlb}:
            grd[srx][sry] = DNGN_ROCK_WALL;
            number_built++;
        }                       // end "for srx,sry"
    }

    if (number_built > 0)
        mpr("Walls emerge from the floor!");
    else
        canned_msg( MSG_NOTHING_HAPPENS );

    return (number_built > 0);
}                               // end entomb()

void cast_poison_ammo(void)
{
    const int ammo = get_inv_in_hand();
    char str_pass[ ITEMNAME_SIZE ];

    if (ammo == -1
        || you.inv[ammo].base_type != OBJ_MISSILES
        || get_ammo_brand( you.inv[ammo] ) != SPMSL_NORMAL
        || you.inv[ammo].sub_type == MI_STONE
        || you.inv[ammo].sub_type == MI_LARGE_ROCK)
    {
        canned_msg( MSG_NOTHING_HAPPENS );
        return;
    }

    if (set_item_ego_type( you.inv[ammo], OBJ_MISSILES, SPMSL_POISONED ))
    {
        in_name(ammo, DESC_CAP_YOUR, str_pass);
        strcpy(info, str_pass);
        strcat(info, (you.inv[ammo].quantity == 1) ? " is" : " are");
        strcat(info, " covered in a thin film of poison.");
        mpr(info);

        set_redraw_status( REDRAW_WIELD );
    }
    else
    {
        canned_msg( MSG_NOTHING_HAPPENS );
    }
}                               // end cast_poison_ammo()

int project_noise(void)
{
    int success = SPRET_FAIL;
    FixedVector< int, 2 >  plox;

    mpr( "Choose the noise's source (press '.', return, or space to select)." );
    more();

    const bool got_source = show_map( plox );
    redraw_screen();

    if (!got_source)
    {
        canned_msg( MSG_SPELL_FIZZLES );
        return (SPRET_ABORT);
    }

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Target square (%d,%d)", plox[0], plox[1] );
#endif

    if (!silenced( plox[0], plox[1] ))
    {
        // player can use this spell to "sound out" the dungeon -- bwr
        if (in_bounds( plox[0], plox[1] )
            && !grid_is_solid( grd[ plox[0] ][ plox[1] ] ))
        {
            noisy( SL_PROJECTED_NOISE, plox[0], plox[1] );
            success = SPRET_SUCCESS;
        }

        // XXX: we're doing the distance check from noisy() here so that
        // we can hide the edge of the dungeon from the player.
        if (distance( you.x_pos, you.y_pos, plox[0], plox[1] ) <= 900
            && !silenced( you.x_pos, you.y_pos ))
        {
            if (success != SPRET_SUCCESS)
                mpr(MSGCH_SOUND,"You hear a dull thud." );
            else
            {
                mpr( MSGCH_SOUND, "You hear a %svoice call your name.",
                      (!see_grid( plox[0], plox[1] ) ? "distant " : "") );
            }
        }
    }

    return (success);
}                               // end project_noise()

bool recall( bool undead_only )
{
    int loopy = 0;              // general purpose looping variable {dlb}
    bool success = false;       // more accurately: "apparent success" {dlb}
    int start_count = 0;
    int step_value = 1;
    int end_count = (MAX_MONSTERS - 1);
    FixedVector < char, 2 > empty;
    struct monsters *monster = 0;       // NULL {dlb}

    empty[0] = empty[1] = 0;

// someone really had to make life difficult {dlb}:
// sometimes goes through monster list backwards
    if (coinflip())
    {
        start_count = (MAX_MONSTERS - 1);
        end_count = 0;
        step_value = -1;
    }

    for (loopy = start_count; loopy != end_count; loopy += step_value)
    {
        monster = &menv[loopy];

        if (monster->type == -1)
            continue;

        if (!mons_friendly( monster ))
            continue;

        if (monster_habitat( monster->type ) != DNGN_FLOOR)
            continue;

        if (undead_only && mons_holiness( monster ) != MH_UNDEAD)
            continue;

        if (empty_surrounds( you.x_pos, you.y_pos, DNGN_FLOOR, 2,
                             false, true, empty ))
        {
            move_monster_to_grid( monster_index(monster), empty[0], empty[1] );

            // only informed if monsters recalled are visible {dlb}:
            if (mon_msg( monster, " is recalled." ))
                success = true;
        }
        else
        {
            break;              // no more room to place monsters {dlb}
        }
    }

    if (!success)
        mpr("Nothing appears to have answered your call.");

    return (success);
}                               // end recall()

void portal(void)
{
    char dir_sign = 0;
    unsigned char keyi;
    int target_level = 0;
    int old_level = you.depth;

    if (!player_in_branch( BRANCH_MAIN_DUNGEON ))
    {
        mpr("This spell doesn't work here.");
    }
    else if (grd[you.x_pos][you.y_pos] != DNGN_FLOOR)
    {
        mpr("You must find a clear area in which to cast this spell.");
    }
    else
    {
        // the first query {dlb}:
        mpr(MSGCH_PROMPT,"Which direction ('<' for up, '>' for down, 'x' to quit)?" );

        for (;;)
        {
            keyi = static_cast<unsigned char>( get_ch() );

            if (keyi == '<')
            {
                if (you.depth == 0)
                    mpr("You can't go any further upwards with this spell.");
                else
                {
                    dir_sign = -1;
                    break;
                }
            }

            if (keyi == '>')
            {
                if (you.depth == 35)
                    mpr("You can't go any further downwards with this spell.");
                else
                {
                    dir_sign = 1;
                    break;
                }
            }

            if (keyi == 'x')
            {
                canned_msg(MSG_OK);
                return;         // an early return {dlb}
            }
        }

        // the second query {dlb}:
        mpr(MSGCH_PROMPT,"How many levels (1 - 9, 'x' to quit)?" );

        for (;;)
        {
            keyi = static_cast<unsigned char>( get_ch() );

            if (keyi == 'x')
            {
                canned_msg(MSG_OK);
                return;         // another early return {dlb}
            }

            if (!(keyi < '1' || keyi > '9'))
            {
                target_level = you.depth + ((keyi - '0') * dir_sign);
                break;
            }
        }

        // actual handling begins here {dlb}:
        if (player_in_branch( BRANCH_MAIN_DUNGEON ))
        {
            if (target_level < 0)
                target_level = 0;
            else if (target_level > 26)
                target_level = 26;
        }

        mpr( "You fall through a mystic portal, and materialise at the foot of a staircase." );
        more();

        you.depth = target_level - 1;
        down_stairs( DNGN_STONE_STAIRS_DOWN_I, old_level, false );
        untag_followers();
    }

    return;
}                               // end portal()

bool cast_death_channel(int power)
{
    bool success = false;

    if (you.duration[DUR_DEATH_CHANNEL] < 30)
    {
        mpr("Malign forces permeate your being, awaiting release.");

        you.duration[DUR_DEATH_CHANNEL] += 15 + random2(1 + (power / 3));

        if (you.duration[DUR_DEATH_CHANNEL] > 100)
            you.duration[DUR_DEATH_CHANNEL] = 100;

        success = true;
    }
    else
    {
        canned_msg( MSG_NOTHING_HAPPENS );
    }

    return (success);
}                               // end cast_death_channel()

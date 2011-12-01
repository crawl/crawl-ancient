/*
 *  File:     delay.cc
 *  Summary:  Functions for handling multi-turn actions.
 *
 *  Change History (most recent first):
 *
 * <1> Sept 09, 2001     BWR             Created
 */

#include "AppHdr.h"
#include "globals.h"
#include "externs.h"

#include <stdio.h>
#include <string.h>

#include "delay.h"
#include "effects.h"
#include "enum.h"
#include "fight.h"
#include "food.h"
#include "items.h"
#include "itemname.h"
#include "itemprop.h"
#include "item_use.h"
#include "it_use2.h"
#include "message.h"
#include "misc.h"
#include "monstuff.h"
#include "mstuff2.h"
#include "ouch.h"
#include "output.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "spl-cast.h"
#include "spl-util.h"
#include "stuff.h"
#include "transfor.h"
#include "view.h"

void start_delay( int type, int turns, int parm1, int parm2 )
{
    delay_queue_item  delay;

    delay.type = type;
    delay.duration = turns;
    delay.parm1 = parm1;
    delay.parm2 = parm2;

    you.delay_queue.push( delay );
}

// returns true if current action is stopped (queued always stop)
bool stop_delay( void )
{
    delay_queue_item  delay = you.delay_queue.front();

    // At the very least we can remove any queued delays, as
    // right now there is no problem with doing this... note that
    // any queuing here can only happen from a single command,
    // as the effect of a delay doesn't normally allow interaction
    // until it is done... it merely chains up individual actions
    // into a single action.  -- bwr
    if (you.delay_queue.size() > 1)
    {
        while (you.delay_queue.size())
            you.delay_queue.pop();

        you.delay_queue.push( delay );
    }

    bool stopped = false;

    switch (delay.type)
    {
    case DELAY_BUTCHER:
        // Corpse keeps track of work in plus2 field, see handle_delay() -- bwr
        mpr( "You stop butchering the corpse." );
        stopped = true;
        break;

    case DELAY_MEMORISE:
        // Losing work here is okay... having to start from
        // scratch is a reasonable behaviour. -- bwr
        mpr( "Your memorisation is interrupted." );
        stopped = true;
        break;

    case DELAY_PASSWALL:
        // The lost work here is okay since this spell requires
        // the player to "attune to the rock".  If changed, the
        // the delay should be increased to reduce the power of
        // this spell. -- bwr
        mpr( "Your meditation is interrupted." );
        stopped = true;
        break;

    case DELAY_INTERUPTABLE:
        // always stopable by definition...
        // using/creating a more specific type is prefered -- bwr
        stopped = true;
        break;

    case DELAY_EAT:
        // Always assume that the player wants to do at least one
        // turn of eating (so they can fend off starvation while
        // they're being attacked).  This is also required so that
        // ghouls can eat chunks and others can eat royal jellies
        // in combat while getting the effects appropriately (ie
        // once... not zero or infinity).  Yes this does mean that
        // the character gets attacked twice if the first attack is
        // immediately after the 'E' command... this is currently
        // considered a feature.  -- bwr
        if (!you.delay_work)
            break;

        mpr( "You stop eating." );
        stopped = true;

        // need to separate partially eaten food from stacks:
        if (delay.parm1)    // from inventory
        {
            if (you.inv[delay.parm2].quantity > 1)
            {
                // This is a bit sloppy (creating an outside object),
                // but currently this is the easiest and safest way.
                int new_item = get_item_slot(0);

                if (new_item == NON_ITEM)
                    mpr( "The demon of the infinite void grins at you." );
                else
                {
                    // copy food item (but only one of them)
                    mitm[new_item] = you.inv[delay.parm2];
                    mitm[new_item].quantity = 1;
                    mitm[new_item].x = 0;
                    mitm[new_item].y = 0;
                    mitm[new_item].link = NON_ITEM;

                    // move item to player's feet:
                    move_item_to_grid( &new_item, you.x_pos, you.y_pos );

                    // try to move to player
                    int num = move_item_to_player( new_item, 1, true );

                    if (num <= 0)
                        mpr( "You drop the leftovers." );
                }

                you.inv[delay.parm2].plus2 = 0;
                dec_inv_item_quantity( delay.parm2, 1 );
            }
        }
        else                // from ground
        {
            if (mitm[delay.parm2].quantity > 1)
            {
                copy_item_to_grid( mitm[delay.parm2], you.x_pos, you.y_pos, 1 );
                mitm[delay.parm2].plus2 = 0;
                dec_mitm_item_quantity( delay.parm2, 1 );
            }
        }
        break;

    case DELAY_ARMOUR_ON:
    case DELAY_ARMOUR_OFF:
        // These two have the default action of not being interuptable,
        // although they will often be chained (remove cloak, remove
        // armour, wear new armour, replace cloak), all of which can
        // be stopped when complete.  This is a fairly reasonable
        // behaviour, although perhaps the character should have
        // option of reversing the current action if it would take
        // less time to get out of the plate mail that's half on
        // than it would take to continue.  Probably too much trouble,
        // and would have to have a prompt... this works just fine. -- bwr
        break;

    case DELAY_AUTOPICKUP:        // one turn... too much trouble
    case DELAY_WEAPON_SWAP:       // one turn... too much trouble
    case DELAY_DROP_ITEM:         // one turn... only used for easy armour drops
    case DELAY_ASCENDING_STAIRS:  // short... and probably what people want
    case DELAY_DESCENDING_STAIRS: // short... and probably what people want
    case DELAY_UNINTERUPTABLE:    // never stopable
    default:
        break;
    }

    if (stopped)
    {
        you.delay_queue.pop();
        you.delay_work = 0;
    }

    return (stopped);
}

bool you_are_delayed( void )
{
    return (!you.delay_queue.empty());
}

int current_delay_action( void )
{
    return (you_are_delayed() ? you.delay_queue.front().type
                              : DELAY_NOT_DELAYED);
}

void handle_delay( void )
{
    int   tmp;
    int   ego;
    int   nx = 0;
    int   ny = 0;
    int   mon;
    char  str_pass[ ITEMNAME_SIZE ];

    if (!you_are_delayed())
        return;

    delay_queue_item &delay = you.delay_queue.front();

    // First check cases where delay may no longer be valid:
    switch (delay.type)
    {
    default:
        break;

    case DELAY_PASSWALL:
        if (you.delay_work)
        {
            // scan out from the player to current scan depth
            nx = you.x_pos;
            ny = you.y_pos;
            for (tmp = 0; tmp < you.delay_work; tmp++)
            {
                nx += delay.parm1;
                ny += delay.parm2;

                if (in_bounds( nx, ny ) && grd[nx][ny] != DNGN_ROCK_WALL)
                    break;
            }

            // if empty at current scan depth and at least 2 away, done
            if (tmp == you.delay_work - 1 && tmp > 0)
                delay.duration = 0;
            else if (tmp < you.delay_work)
            {
                // something changed in the path, stop the spell
                stop_delay();
                return;
            }
        }
        break;

    case DELAY_EAT:
        {
            item_def &item = (delay.parm1) ? you.inv[ delay.parm2 ]
                                           : mitm[ delay.parm2 ];

            // Food could disappear while we're eating it (ie giant spore)!...
            // see butchery for more details.
            if (is_valid_item( item )
                && item.base_type == OBJ_FOOD
                && (delay.parm1 || (item.x == you.x_pos && item.y == you.y_pos)))
            {
                ASSERT( item.plus2 < food_turns( item )); // still some left

                lessen_hunger( food_value( item ), true );
                item.plus2++;
            }
            else
            {
                stop_delay();
                return;
            }
        }
        break;

    case DELAY_BUTCHER:
        // A monster may have raised the corpse you're chopping up! -- bwr
        // Note that a monster could have raised the corpse and another
        // monster could die and create a corpse with the same ID number...
        // However, it would not be at the player's square like the
        // original and that's why we do it this way (providing teleport
        // stops this action).  Note that we ignore the conversion to
        // skeleton possiblity just to be nice. -- bwr
        if (player_can_cut_meat()
            && is_valid_item( mitm[ delay.parm1 ] )
            && mitm[ delay.parm1 ].base_type == OBJ_CORPSES
            && mitm[ delay.parm1 ].x == you.x_pos
            && mitm[ delay.parm1 ].y == you.y_pos)
        {
            // mark work done on the corpse in case we stop -- bwr
            mitm[ delay.parm1 ].plus2++;
        }
        else
        {
            // corpse is no longer valid!
            stop_delay();
            return;
        }
        break;

    case DELAY_MEMORISE:
        if ((you.mutation[MUT_BLURRY_VISION] > 0
                && random2(4) < you.mutation[MUT_BLURRY_VISION])
            || you.conf)
        {
            mpr("The writing blurs into unreadable gibberish.");
            stop_delay();
            return;
        }
        break;
    }

    // Handle delay:
    if (delay.duration > 0)
    {
        switch (delay.type)
        {
        case DELAY_EAT:
            switch (random2(20))
            {
            default:
                mpr( MSGCH_MULTITURN_ACTION, "You continue eating.");
                break;

            case 1:
                mpr( MSGCH_MULTITURN_ACTION, "Munch, munch." );
                break;

            case 2:
                mpr( MSGCH_MULTITURN_ACTION, "Yum!" );
                break;
            }
            break;

        case DELAY_ARMOUR_ON:
        case DELAY_ARMOUR_OFF:
            in_name( delay.parm1, DESC_NOCAP_YOUR, str_pass );

            mpr( MSGCH_MULTITURN_ACTION, "You continue %s %s.",
                  (delay.type == DELAY_ARMOUR_ON) ? "putting on" : "removing",
                   str_pass );
            break;

        case DELAY_MEMORISE:
            mpr( MSGCH_MULTITURN_ACTION, "You continue memorising %s.",
                  spell_title( delay.parm1 ) );

            break;

        case DELAY_BUTCHER:
            mpr( MSGCH_MULTITURN_ACTION, "You continue dissecting the corpse." );
            break;

        case DELAY_PASSWALL:
            mpr( MSGCH_MULTITURN_ACTION, "You continue mediating on the rock." );
            break;

        default:
#if DEBUG_DIAGNOSTICS
            mpr( MSGCH_DIAGNOSTICS, "Delay type: %d   duration: %d",
                      delay.type, delay.duration );
#endif
            break;
        }

        delay.duration--;
        you.delay_work++;
    }
    else
    {
        int slot;

        you.turn_is_over = true;

        you.delay_queue.pop();
        you.delay_work = 0;

        // Delay done, finish job.
        switch (delay.type)
        {
        case DELAY_AUTOPICKUP:
            break;

        case DELAY_WEAPON_SWAP:
            weapon_switch( delay.parm1 );
            break;

        case DELAY_ARMOUR_ON:
            set_ident_flags( you.inv[delay.parm1], ISFLAG_EQ_ARMOUR_MASK );

            in_name( delay.parm1, DESC_NOCAP_YOUR, str_pass );
            mpr( "You finish putting on %s.", str_pass );

            slot = get_armour_slot( you.inv[delay.parm1] );
            you.equip[slot] = delay.parm1;

            if (slot == EQ_BODY_ARMOUR || slot == EQ_SHIELD)
            {
                if (you.duration[DUR_ICY_ARMOUR] != 0)
                {
                    mpr( "Your icy armour melts away.", MSGCH_DURATION );
                    set_redraw_status( REDRAW_ARMOUR_CLASS );
                    you.duration[DUR_ICY_ARMOUR] = 0;
                }

                if (slot == EQ_BODY_ARMOUR)
                    you.naked_okay = false; // reset check for nakedness

                const int pen = (slot == EQ_SHIELD) ? heavy_shield_penalty()
                                                    : heavy_armour_penalty();

                if (pen > 250)
                {
                    mpr( MSGCH_WARN,
                         "This %s is %s burdensome for you!",
                         (slot == EQ_SHIELD) ? "shield" : "armour",
                         (pen <=  500) ? "somewhat" :
                         (pen <=  750) ? "quite" :
                         (pen <= 1000) ? "very"
                                       : "extremely" );
                }
            }

            ego = get_armour_ego_type( you.inv[delay.parm1] );
            if (ego != SPARM_NORMAL)
            {
                switch (ego)
                {
                case SPARM_RUNNING:
                    strcpy(info, "You feel quick");
                    strcat(info, (you.species == SP_NAGA
                            || you.species == SP_CENTAUR) ? "." : " on your feet.");
                    mpr(info);
                    break;

                case SPARM_FIRE_RESISTANCE:
                    mpr("You feel resistant to fire.");
                    break;

                case SPARM_COLD_RESISTANCE:
                    mpr("You feel resistant to cold.");
                    break;

                case SPARM_POISON_RESISTANCE:
                    mpr("You feel healthy.");
                    break;

                case SPARM_SEE_INVISIBLE:
                    mpr("You feel perceptive.");
                    break;

                case SPARM_DARKNESS:
                    if (!you.invis)
                        mpr("You become transparent for a moment.");
                    break;

                case SPARM_STRENGTH:
                    modify_stat(STAT_STRENGTH, 3, false);
                    break;

                case SPARM_DEXTERITY:
                    modify_stat(STAT_DEXTERITY, 3, false);
                    break;

                case SPARM_INTELLIGENCE:
                    modify_stat(STAT_INTELLIGENCE, 3, false);
                    break;

                case SPARM_PONDEROUSNESS:
                    mpr("You feel rather ponderous.");
                    // you.speed += 2;
                    set_redraw_status( REDRAW_EVASION );
                    break;

                case SPARM_LEVITATION:
                    mpr("You feel rather light.");
                    break;

                case SPARM_MAGIC_RESISTANCE:
                    mpr("You feel resistant to magic.");
                    break;

                case SPARM_PROTECTION:
                    mpr("You feel protected.");
                    break;

                case SPARM_STEALTH:
                    mpr("You feel more stealthy.");
                    break;

                case SPARM_RESISTANCE:
                    mpr("You feel resistant to extremes of temperature.");
                    break;

                case SPARM_POSITIVE_ENERGY:
                    mpr("Your life-force is being protected.");
                    break;

                case SPARM_ARCHMAGI:
                    if (!you.skills[SK_SPELLCASTING])
                        mpr("You feel strangely numb.");
                    else
                        mpr("You feel extremely powerful.");
                    break;
                }
            }

            if (is_random_artefact( you.inv[delay.parm1] ))
                use_randart( delay.parm1 );

            if (item_cursed( you.inv[delay.parm1] ))
                mpr( "Oops, that feels deathly cold." );

            set_redraw_status( REDRAW_ARMOUR_CLASS );
            set_redraw_status( REDRAW_EVASION );
            break;

        case DELAY_ARMOUR_OFF:
            in_name( delay.parm1, DESC_NOCAP_YOUR, str_pass );
            mpr( "You finish taking off %s.", str_pass );

            slot = get_armour_slot( you.inv[delay.parm1] );
            you.equip[slot] = -1;

            unwear_armour( delay.parm1 );

            set_redraw_status( REDRAW_ARMOUR_CLASS );
            set_redraw_status( REDRAW_EVASION );
            break;

        case DELAY_EAT:
            mpr( "You finish eating." );

            if (delay.parm1)
            {
                // clear the partial work field in case we were stacked
                you.inv[delay.parm2].plus2 = 0;
                dec_inv_item_quantity( delay.parm2, 1 );
            }
            else
            {
                // clear the partial work field in case we were stacked
                mitm[delay.parm2].plus2 = 0;
                dec_mitm_item_quantity( delay.parm2, 1 );
            }
            break;

        case DELAY_MEMORISE:
            tmp = spell_memorise_fail( delay.parm1 );

            if (roll_zdice( 3, 40 ) < tmp)
            {
                mpr("You fail to memorise the spell.");

                if (delay.parm2 == BOOK_NECRONOMICON)
                {
                    mpr("The pages of the Necronomicon glow with a dark malevolence...");
                    miscast_effect( SPTYP_NECROMANCY, 8, roll_dice(3,30), 100,
                                    "reading the Necronomicon" );
                }
                else if (delay.parm2 == BOOK_DEMONOLOGY)
                {
                    mpr("This book does not appreciate being disturbed by one of your ineptitude!");
                    miscast_effect( SPTYP_SUMMONING, 7, roll_dice(3,30), 100,
                                    "reading the book of Demonology" );
                }
                else if (delay.parm2 == BOOK_ANNIHILATIONS)
                {
                    mpr("This book does not appreciate being disturbed by one of your ineptitude!");
                    miscast_effect( SPTYP_CONJURATION, 8, roll_dice(3,30), 100,
                                    "reading the book of Annihilations" );
                }

                viewwindow(true, false);

#if WIZARD
                if (!you.wizard)
                    break;
                else if (!yesno("Memorise anyway?"))
                    break;
#else
                break;
#endif
            }

            tmp = add_spell_to_memory( delay.parm1 );

            mpr( "You finish memorising %s (%c).",
                  spell_title( delay.parm1 ), index_to_letter( tmp ) );

            did_god_conduct(DID_SPELL_MEMORISE, spell_level(delay.parm1));
            break;

        case DELAY_PASSWALL:
            ASSERT( !adjacent( nx, ny, you.x_pos, you.y_pos ) );

            mpr( "You finish merging with the rock." );

            if (grd[nx][ny] == DNGN_SECRET_DOOR
                || grd[nx][ny] == DNGN_CLOSED_DOOR)
            {
                // open the door
                grd[nx][ny] = DNGN_OPEN_DOOR;
            }
            else if (grid_is_solid( grd[nx][ny] ))
            {
                // solid
                ouch( INSTANT_DEATH, 0, KILLED_BY_PETRIFICATION );
            }

            // move any monsters out of the way:
            mon = mgrd[nx][ny];
            if (mon != NON_MONSTER)
            {
                // one square, a few squares, anywhere...
                if (!shift_monster(&menv[mon])
                    && !monster_blink( &menv[mon], false, true ))
                {
                    monster_teleport( &menv[mon], 0, true );
                }
            }

            move_player_to_grid( nx, ny, false, true, true );
            more();  // or the messages won't be seen
            redraw_screen();
            start_delay( DELAY_UNINTERUPTABLE, 1 ); // recovery time
            break;

        case DELAY_BUTCHER:
            mpr( "You finish %s the corpse into pieces.",
                      (player_size() >= SIZE_GIANT) ? "rending" :
                      (player_barehand_butcher())   ? "ripping"
                                                    : "chopping" );


            turn_corpse_into_chunks( mitm[delay.parm1] );

            if (you.berserker && you.berserk_penalty != NO_BERSERK_PENALTY)
            {
                mpr("You enjoyed that.");
                you.berserk_penalty = 0;
            }
            break;

        case DELAY_DROP_ITEM:
            // Note:  checking if item is dropable is assumed to
            // be done before setting up this delay... this includes
            // quantity (delay.parm2). -- bwr

            // Make sure item still exists.
            if (!is_valid_item( you.inv[ delay.parm1 ] ))
                break;

            // Must handle unwield_item before we attempt to copy
            // so that temporary brands and such are cleared. -- bwr
            if (delay.parm1 == you.equip[EQ_WEAPON])
                unwield_item();

            if (!copy_item_to_grid( you.inv[ delay.parm1 ],
                                    you.x_pos, you.y_pos, delay.parm2 ))
            {
                mpr("Too many items on this level, not dropping the item.");
            }
            else
            {
                quant_name( you.inv[ delay.parm1 ], delay.parm2,
                            DESC_NOCAP_A, str_pass );

                mpr( "You drop %s.", str_pass );

                dec_inv_item_quantity( delay.parm1, delay.parm2 );
            }
            break;

        case DELAY_ASCENDING_STAIRS:
            up_stairs( grd[you.x_pos][you.y_pos], true );
            untag_followers();
            break;

        case DELAY_DESCENDING_STAIRS:
            down_stairs( grd[you.x_pos][you.y_pos], delay.parm1, true );
            untag_followers();
            break;

        case DELAY_INTERUPTABLE:
        case DELAY_UNINTERUPTABLE:
            // these are simple delays that have no effect when complete
            break;

        default:
            mpr( "You finish doing something." );
            break;
        }

        set_redraw_status( REDRAW_WIELD );
        set_redraw_status( REDRAW_STATUS );
        print_stats();
    }
}

// TODO: merge the multiturn actions (activities and delays) into one handler...
// there really isn't much difference between eating a ration over multiple
// turns and dropping a list of items over several turns.
void perform_activity( void )
{
    switch (you.activity)
    {
    case ACT_MULTIDROP:
        do_drop();
        break;
    default:
        break;
    }
}

void interrupt_activity( activity_interrupt_type ai )
{
    if (you.running && !you.activity)
        you.activity = (you.running > 0) ? ACT_RUNNING : ACT_TRAVELING;

    if (!you.activity)
        return;

    if (!ai || (Options.activity_interrupts[ you.activity ] & ai))
    {
        if (you.activity == ACT_RUNNING || you.activity == ACT_TRAVELING)
            you.running = 0;

        you.activity = ACT_NONE;
    }

    if (you.activity == ACT_RUNNING || you.activity == ACT_TRAVELING)
        you.activity = ACT_NONE;
}

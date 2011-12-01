/*
 *  File:       food.cc
 *  Summary:    Functions for eating and butchering.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <2>      5/20/99        BWR           Added CRAWL_PIZZA.
 *      <1>      -/--/--        LRH           Created
 */

#include "AppHdr.h"
#include "food.h"

#include <string.h>
// required for abs() {dlb}:
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifdef DOS
#include <conio.h>
#endif

#include "globals.h"
#include "externs.h"

#include "debug.h"
#include "delay.h"
#include "effects.h"
#include "invent.h"
#include "items.h"
#include "itemname.h"
#include "itemprop.h"
#include "item_use.h"
#include "it_use2.h"
#include "macro.h"
#include "misc.h"
#include "mon-util.h"
#include "mutation.h"
#include "player.h"
#include "religion.h"
#include "skills2.h"
// #include "spells2.h"
#include "stuff.h"

static void  describe_food_change( int hunger_increment );
static bool  food_change( bool suppress_message );

static bool  can_ingest( const item_def &item, bool suppress_msg );
static bool  eat_from_floor(void);

static int   determine_chunk_effect( int which_chunk_type, bool rotten_chunk );
static void  ghoul_eat_flesh( int chunk_effect );
static void  eat_chunk( bool from_inv, int id );
static void  eating( bool from_inv, int id );

/*
 **************************************************
 *                                                *
 *             BEGIN PUBLIC FUNCTIONS             *
 *                                                *
 **************************************************
*/

void make_hungry( int hunger_amount, bool suppress_msg )
{
    if (you.is_undead == US_UNDEAD)
        return;

    you.hunger -= hunger_amount;

    if (you.hunger < 0)
        you.hunger = 0;

    // so we don't get two messages, ever.
    bool state_message = food_change(false);

    // ... except that there are no messages above hungry.
    if (!suppress_msg && (!state_message || you.hunger_state > HS_HUNGRY))
        describe_food_change( -hunger_amount );
}                               // end make_hungry()

void lessen_hunger( int satiated_amount, bool suppress_msg )
{
    if (you.is_undead == US_UNDEAD)
        return;

    you.hunger += satiated_amount;

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "feeding: %d; stomach: %d",
          satiated_amount, you.hunger );
#endif

    if (you.hunger > HUNGER_MAX_FOOD)
        you.hunger = HUNGER_MAX_FOOD;

    // so we don't get two messages, ever
    bool state_message = food_change(false);

    // ... except that there are no messages above hungry.
    if (!suppress_msg && (!state_message || you.hunger_state > HS_HUNGRY))
        describe_food_change(satiated_amount);
}                               // end lessen_hunger()

void set_hunger( int new_hunger_level, bool suppress_msg )
{
    if (you.is_undead == US_UNDEAD)
        return;

    int hunger_difference = (new_hunger_level - you.hunger);

    if (hunger_difference < 0)
        make_hungry( abs( hunger_difference ), suppress_msg );
    else if (hunger_difference > 0)
        lessen_hunger( hunger_difference, suppress_msg );
}                               // end set_hunger()

// more of a "weapon_switch back from butchering" function, switching
// to a weapon is done using the wield_weapon code.
// special cases like staves of power or other special weps are taken
// care of by calling wield_effects()    {gdl}
void weapon_switch( int targ )
{
    if (targ == -1)
    {
        mpr( "You switch back to your bare hands." );
    }
    else
    {
        char buff[80];
        in_name( targ, DESC_NOCAP_A, buff );

        char let = index_to_letter( targ );

        mpr( "Switching back to %c - %s.", let, buff );
    }

    // unwield the old weapon and wield the new.
    // XXX This is a pretty dangerous hack;  I don't like it.--GDL
    //
    // Well yeah, but that's because interacting with the wielding
    // code is a mess... this whole function's purpose was to
    // isolate this hack until there's a proper way to do things. -- bwr

    unwield_item( true );               // remove old first if required
    you.equip[EQ_WEAPON] = targ;

    // special checks: staves of power, etc
    if (targ != -1)
        wield_effects( false );
}


int find_butcher_knife( void )
{
    // We'll now proceed to look through the entire inventory for
    // choppers/slicers.  We'll skip special weapons because
    // wielding/unwielding a foo of distortion would be disastrous.
    //
    // XXX: Protecting against a weapon of distortion is a bit spoilerish...
    // it's not something that players can be expected to know and thus
    // this code might expose more information than it should.  A better
    // solution would probably be a option that takes a range to search,
    // so the player can move items they never want to use [1] for this
    // to safe slots.
    //
    // [1] Note that this might include large weapons, demonic weapons,
    // or weapons only usable as tools (when weapons are available),
    // which if caught in the middle of a disection by surprise might
    // also be disastrous.  Naturally, the player can already order the
    // inventory to stop these (and they can also do that for distortion)...
    // but if an auto safty is required it should be non-spoiler.
    int i;
    for (i = 0; i < ENDOFPACK; ++i)
    {
        if (is_valid_item( you.inv[i] )
            && can_cut_meat( you.inv[i] )
            && you.inv[i].base_type == OBJ_WEAPONS
            && item_known_uncursed(you.inv[i])
            && item_ident( you.inv[i], ISFLAG_KNOW_TYPE )
            // && get_weapon_brand(you.inv[i]) != SPWPN_DISTORTION
            && can_wield( you.inv[i] ))
        {
            break;
        }
    }

    return (i);
}

bool butchery(void)
{
    char str_pass[ITEMNAME_SIZE];
    int obj;
    int work_req;
    int wpn;
    unsigned char keyin;

    bool can_butcher = false;
    bool wpn_switch = false;
    bool new_cursed = false;
    bool found_body = false;
    int old_weapon = you.equip[EQ_WEAPON];

    const bool barehand_butcher = player_barehand_butcher();

    if (igrd[you.x_pos][you.y_pos] == NON_ITEM)
    {
        mpr("There isn't anything here!");
        return (false);
    }

    if (!player_access_floor())
    {
        canned_msg( MSG_CANT_REACH );
        return (false);
    }

    can_butcher = player_can_cut_meat();

    if (!can_butcher && Options.easy_butcher)
    {
        // mv: check for berserk first
        if (you.berserker)
        {
            mpr ("You are too berserk to search for a butchering knife!");
            return (false);
        }

        const int knife = find_butcher_knife();
        if (knife != ENDOFPACK)
        {
            mpr( "Switching to a butchering implement." );
            wpn_switch = true;
            wield_weapon( true, knife );
        }

        // if we didn't swap above, then we still can't cut... let's
        // call wield_weapon() in the "prompt the user" way...
        if (!wpn_switch)
        {
            // prompt for new weapon
            mpr( MSGCH_PROMPT, "What would you like to use?" );
            wield_weapon( false );

            // let's see if the user did something...
            if (you.equip[EQ_WEAPON] != old_weapon)
                wpn_switch = true;
        }

        if (wpn_switch)
            can_butcher = player_can_cut_meat();
    }

    // Account for the weapon switch above if it happened... we're
    // doing this here since the above switch may reveal information
    // about the weapon (curse status, ego type).  So even if the
    // character fails to or decides not to butcher past this point,
    // they have achieved something and there should be a cost.
    if (wpn_switch)
        start_delay( DELAY_UNINTERUPTABLE, 1 );

    // check to see if the new implement is cursed - if so,  set a
    // flag indicating this.  If a player actually butchers anything,
    // this flag can be checked before switching back.
    wpn = you.equip[EQ_WEAPON];

    if (wpn != -1
        && you.inv[wpn].base_type == OBJ_WEAPONS
        && item_cursed( you.inv[wpn] ))
    {
        new_cursed = true;
    }

    // Final checks and clue-giving...
    if (!can_butcher)
    {
        if (!barehand_butcher && you.equip[EQ_WEAPON] == -1)
            mpr("What, with your bare hands?");
        else if (barehand_butcher && you.equip[ EQ_GLOVES ] != -1)
            mpr("You can't use your claws with your gloves on!");
        else
            mpr("Maybe you should try using a sharper implement.");

        goto abort_butcher;
    }

    // No turning back at this point, we better be qualified.
    ASSERT( can_butcher );

    for (obj = igrd[you.x_pos][you.y_pos]; obj != NON_ITEM; obj = mitm[obj].link)
    {
        if (mitm[obj].base_type != OBJ_CORPSES
            || mitm[obj].sub_type != CORPSE_BODY)
        {
            continue;
        }

        found_body = true;

        strcpy( info, "Butcher " );
        it_name( obj, DESC_NOCAP_A, str_pass );
        strcat( info, str_pass );
        strcat( info, "\?" );
        mpr( MSGCH_PROMPT, info );

        keyin = getch();
        if (keyin == 0)
        {
            getch();
            keyin = 0;
        }

        if (keyin == 'q' || keyin == ESCAPE)
        {
            canned_msg( MSG_OK );
            goto abort_butcher;
        }

        if (keyin == 'y' || keyin == 'Y')
        {
            if (player_size() >= SIZE_GIANT)
                mpr("You start tearing the corpse apart.");
            else if (barehand_butcher && you.equip[EQ_GLOVES] == -1)
                mpr("You start ripping the corpse apart.");
            else
                mpr("You start hacking away.");

            if (!offer_corpse(obj))
            {
                work_req = 3 - mitm[obj].plus2;
                if (work_req < 0)
                    work_req = 0;

                start_delay( DELAY_BUTCHER, work_req, obj );
            }

            // need to count the swap delay in this case
            if (wpn_switch && !new_cursed)
                start_delay( DELAY_WEAPON_SWAP, 1, old_weapon );

            you.turn_is_over = true;
            return (true);
        }
    }           // for o

    mpr( "There isn't anything%s to dissect here.", (found_body) ? " else" : "" );

abort_butcher:
    if (wpn_switch && !new_cursed)
        weapon_switch( old_weapon );

    return (false);
}                               // end butchery()

void eat_food(void)
{
    int which_inventory_slot;

    if (you.is_undead == US_UNDEAD)
    {
        mpr("You can't eat.");
        return;
    }

    if (you.hunger >= HUNGER_ENGORGED)
    {
        mpr("You're too full to eat anything.");
        return;
    }

    if (igrd[you.x_pos][you.y_pos] != NON_ITEM)
    {
        if (eat_from_floor())
            return;
    }

    if (inv_count() < 1)
    {
        canned_msg(MSG_NOTHING_CARRIED);
        return;
    }

    which_inventory_slot = prompt_invent_item( "Eat which item?", OBJ_FOOD );
    if (which_inventory_slot == PROMPT_ABORT)
    {
        canned_msg( MSG_OK );
        return;
    }

    // this conditional can later be merged into food::can_ingest() when
    // expanded to handle more than just OBJ_FOOD 16mar200 {dlb}
    if (you.inv[which_inventory_slot].base_type != OBJ_FOOD)
    {
        mpr("You can't eat that!");
        return;
    }

    if (!can_ingest( you.inv[which_inventory_slot], false ))
        return;

    eating( true, which_inventory_slot );
}                               // end eat_food()

/*
 **************************************************
 *                                                *
 *              END PUBLIC FUNCTIONS              *
 *                                                *
 **************************************************
*/

static bool food_change( bool suppress_message )
{
    bool state_changed = false;

    // If non-hungry undead should not be here, let's check if they are here.
    ASSERT( you.is_undead != US_UNDEAD );

    // Hungry dead (ie ghouls) cannot be full.
    if (you.is_undead == US_HUNGRY_DEAD && you.hunger >= HUNGER_FULL)
        you.hunger = HUNGER_FULL - 1;

    // get new hunger state (note: HUNGER marks points away from the middle)
    const char newstate = ((you.hunger >= HUNGER_ENGORGED) ? HS_ENGORGED :
                           (you.hunger >= HUNGER_FULL)     ? HS_FULL :
                           (you.hunger <= HUNGER_STARVING) ? HS_STARVING :
                           (you.hunger <= HUNGER_HUNGRY)   ? HS_HUNGRY
                                                           : HS_SATIATED);

    if (newstate != you.hunger_state)
    {
        state_changed = true;
        you.hunger_state = newstate;

        // Stop the travel command, if it's in progress and we just got hungry
        if (newstate < HS_SATIATED)
            interrupt_activity( AI_HUNGRY );

        if (suppress_message == false)
        {
            switch (you.hunger_state)
            {
            case HS_STARVING:
                mpr( MSGCH_FOOD, "You are starving!" );
                break;
            case HS_HUNGRY:
                mpr( MSGCH_FOOD, "You are feeling hungry." );
                break;
            default:
                break;
            }
        }
    }

    return (state_changed);
}                               // end food_change()


// food_increment is positive for eating,  negative for hungering
static void describe_food_change( int food_increment )
{
    if (food_increment == 0)
        return;

    const int magnitude = abs( food_increment );

    mpr( MSGCH_FOOD, "You feel %s%s %s.",
                (magnitude <  100) ? "slightly " :
                (magnitude <  200) ? "a bit "    :
                (magnitude <  400) ? ""          :
                (magnitude <  800) ? "much "
                                   : "a lot ",

                ((you.hunger_state > HS_SATIATED) ^ (food_increment < 0))
                    ? "more" : "less",

                (you.hunger_state > HS_SATIATED) ? "full" : "hungry" );
}                               // end describe_food_change()

// Returns true if no further attempts to eat should be made and false
// otherwise (typically true means we ate already, but see the auto_list
// case below where we fake that).
static bool eat_from_floor(void)
{
    char str_pass[ ITEMNAME_SIZE ];

    if (!player_access_floor())
        return (false);

    for (int o = igrd[you.x_pos][you.y_pos]; o != NON_ITEM; o = mitm[o].link)
    {
        if (mitm[o].base_type != OBJ_FOOD)
            continue;

        it_name( o, DESC_NOCAP_A, str_pass );
        mpr( MSGCH_PROMPT, "Eat %s%s?",
                (mitm[o].quantity > 1) ? "one of " : "", str_pass );

        unsigned char keyin = tolower( getch() );

        if (keyin == 0)
        {
            getch();
            keyin = 0;
        }

        if (keyin == 'q')
            return (false);

        if (keyin == 'y')
        {
            // XXX: Do we really want to fake eating for auto_list just
            // so the error message comes up? -- bwr
            if (!can_ingest( mitm[o], false ))
                return (Options.auto_list);

            eating( false, o );
            // dec_mitm_item_quantity( o, 1 );
            return (true);
        }
    }

    return (false);
}

static void ghoul_eat_flesh( int chunk_effect )
{
    bool healed = false;

    if (chunk_effect != CE_ROTTEN && chunk_effect != CE_CONTAMINATED)
    {
        mpr("This raw flesh tastes good.");

        if (!one_chance_in(5))
            healed = true;

        if (player_rotted() && !one_chance_in(3))
        {
            mpr("You feel more resilient.");
            unrot_hp(1);
        }
    }
    else
    {
        if (chunk_effect == CE_ROTTEN)
            mpr( "This rotting flesh tastes delicious!" );
        else // CE_CONTAMINATED
            mpr( "This flesh tastes delicious!" );

        healed = true;

        if (player_rotted() && !one_chance_in(4))
        {
            mpr("You feel more resilient.");
            unrot_hp(1);
        }
    }

    if (you.str < you.max_str && one_chance_in(5))
    {
        mpr("You feel your strength returning.");
        you.str++;
        set_redraw_status( REDRAW_STRENGTH );
        burden_change();
    }

    if (healed && you.hp < you.hp_max)
        inc_hp( 1 + random2(5) + random2(1 + you.xp_level), false );

    calc_hp_max();

    return;
}                               // end ghoul_eat_flesh()

// never called directly - chunk_effect values must pass
// through food::determine_chunk_effect() first {dlb}:
static void eat_chunk( bool from_inv, int id )
{
    const item_def &item = (from_inv) ? you.inv[id] : mitm[id];

    // Used for the cases where the food isn't "eaten", but just
    // has an effect and disappears.
    bool (*dec_quant_func)( int, int ) = (from_inv) ? dec_inv_item_quantity
                                                    : dec_mitm_item_quantity;

    const int mons_type = item.plus;
    const int chunk_type = mons_corpse_effect( mons_type );
    const bool rotten = (item.special < 100);

    const int chunk_effect = determine_chunk_effect( chunk_type, rotten );
    // const int value = food_value( item );

    const bool likes_chunks = (you.species == SP_KOBOLD
                                 || you.species == SP_OGRE
                                 || you.species == SP_TROLL
                                 || you.species == SP_GHOUL
                                 || you.species == SP_GIANT
                                 || you.mutation[MUT_CARNIVOROUS]);

    const int delay_needed = food_turns( item ) - item.plus2 - 1;
    ASSERT (delay_needed >= 0);

    if (you.species == SP_GHOUL)
    {
        // XXX: this and other side-effects (ie royal jelly) should probably
        // be moved over to delay.cc to match the other delays (so that
        // the benefit is only gained when the chunk is fully eaten, instead
        // of gained on the first nibble).

        // only apply ghoul recovery once per chunk:
        if (item.plus2 == 0)
            ghoul_eat_flesh( chunk_effect );
        else
            mpr("This raw flesh tastes good.");

        start_delay( DELAY_EAT, delay_needed, from_inv, id );
    }
    else
    {
        switch (chunk_effect)
        {
        case CE_MUTAGEN_RANDOM:
            mpr("This meat tastes really weird.");
            mutate( PICK_RANDOM_MUTATION );
            dec_quant_func( id, 1 );
            break;

        case CE_MUTAGEN_BAD:
            mpr("This meat tastes *really* weird.");
            give_bad_mutation();
            dec_quant_func( id, 1 );
            break;

        case CE_HCL:
            rot_player( 10 + random2(10) );
            disease_player( 50 + random2(100) );
            dec_quant_func( id, 1 );
            break;

        case CE_POISONOUS:
            mpr("Yeeuch - this meat is poisonous!");

            // Note that res_poison is applied in chunk conversion too,
            // so the res_poison case here is really just res_poison == 1.
            if (player_res_poison())
                poison_player( 1 + random2(3) );
            else
                poison_player( 3 + random2(4) );

            dec_quant_func( id, 1 );
            break;

        case CE_ROTTEN:
        case CE_CONTAMINATED:
            mpr("There is something wrong with this meat.");
            disease_player( 50 + random2(100) );
            dec_quant_func( id, 1 );
            break;

        // note that this is the only case that takes time and forces redraw
        case CE_CLEAN:
            strcpy(info, "This raw flesh ");

            strcat(info, (likes_chunks) ? "tastes good."
                                        : "is not very appetising.");
            mpr(info);

            start_delay( DELAY_EAT, delay_needed, from_inv, id );
            break;
        }
    }

    return;
}                               // end eat_chunk()

static void eating( bool from_inv, int id )
{
    const item_def &item = (from_inv) ? you.inv[id] : mitm[id];

    int temp_rand;              // probability determination {dlb}

    const int how_herbivorous = you.mutation[MUT_HERBIVOROUS];
    const int how_carnivorous = you.mutation[MUT_CARNIVOROUS];

    const int value = food_value( item );
    const int delay_needed = food_turns( item ) - item.plus2 - 1;

    ASSERT (delay_needed >= 0);

    if (item.base_type != OBJ_FOOD)
        return;

    if (value <= 0)
    {
        mpr( "That has no food value for you." );
        return;
    }

    // committed to eating now:
    you.turn_is_over = true;

    if (item.sub_type == FOOD_CHUNK)
    {
        eat_chunk( from_inv, id );
        return;
    }

    // Take care of messaging {dlb}:
    if (how_carnivorous && food_is_veg( item ))
        mpr("Blech - you need meat!");
    else if (how_herbivorous && food_is_meat( item ))
        mpr("Blech - you need greens!");

    // NOTE: Any food with special random messages or side effects
    // currently only takes one turn to eat... if this changes then
    // those items will have to have special code (like ghoul chunks)
    // to guarantee that the special thing is only done once.

    if (!how_herbivorous)
    {
        switch (item.sub_type)
        {
        case FOOD_MEAT_RATION:
            mpr("This meat ration really hits the spot!");
            break;
        case FOOD_BEEF_JERKY:
            strcpy(info, "This beef jerky is ");
            strcat(info, (one_chance_in(4)) ? "jerk-a-riffic"
                                            : "delicious");
            strcat(info, "!");
            mpr(info);
            break;
        default:
            break;
        }
    }

    if (!how_carnivorous)
    {
        switch (item.sub_type)
        {
        case FOOD_BREAD_RATION:
            mpr("This bread ration really hits the spot!");
            break;
        case FOOD_PEAR:
        case FOOD_APPLE:
        case FOOD_APRICOT:
            strcpy(info, "Mmmm... Yummy ");
            strcat(info, (item.sub_type == FOOD_APPLE)   ? "apple." :
                         (item.sub_type == FOOD_PEAR)    ? "pear." :
                         (item.sub_type == FOOD_APRICOT) ? "apricot."
                                                         : "fruit.");
            mpr(info);
            break;
        case FOOD_CHOKO:
            mpr("This choko is very bland.");
            break;
        case FOOD_SNOZZCUMBER:
            mpr("This snozzcumber tastes truly putrid!");
            break;
        case FOOD_ORANGE:
            strcpy(info, "This orange is delicious!");
            if (one_chance_in(8))
                strcat(info, " Even the peel tastes good!");
            mpr(info);
            break;
        case FOOD_BANANA:
            strcpy(info, "This banana is delicious!");
            if (one_chance_in(8))
                strcat(info, " Even the peel tastes good!");
            mpr(info);
            break;
        case FOOD_STRAWBERRY:
            mpr("This strawberry is delicious!");
            break;
        case FOOD_RAMBUTAN:
            mpr("This rambutan is delicious!");
            break;
        case FOOD_LEMON:
            mpr("This lemon is rather sour... but delicious nonetheless!");
            break;
        case FOOD_GRAPE:
            mpr("This grape is delicious!");
            break;
        case FOOD_SULTANA:
            mpr("This sultana is delicious! (but very small)");
            break;
        case FOOD_LYCHEE:
            mpr("This lychee is delicious!");
            break;
        default:
            break;
        }
    }

    switch (item.sub_type)
    {
    case FOOD_HONEYCOMB:
        mpr("This honeycomb is delicious.");
        break;

    case FOOD_ROYAL_JELLY:
        mpr("This royal jelly is delicious!");
        restore_stat( STAT_ALL );
        break;

    case FOOD_PIZZA:
        if (SysEnv.crawl_pizza && one_chance_in(3))
        {
            snprintf( info, INFO_SIZE, "Mmm... %s", SysEnv.crawl_pizza );
        }
        else
        {
            temp_rand = random2(9);

            snprintf( info, INFO_SIZE, "Mmm... %s",
                         (temp_rand == 0) ? "Ham and pineapple." :
                         (temp_rand == 1) ? "Extra thick crust." :
                         (temp_rand == 2) ? "Vegetable." :
                         (temp_rand == 3) ? "Pepperoni." :
                         (temp_rand == 4) ? "Yeuchh - Anchovies!" :
                         (temp_rand == 5) ? "Cheesy." :
                         (temp_rand == 6) ? "Supreme." :
                         (temp_rand == 7) ? "Super Supreme!"
                                          : "Chicken." );
        }

        mpr(info);
        break;

    case FOOD_CHEESE:
        strcpy(info, "Mmm... ");
        temp_rand = random2(9);

        strcat(info, (temp_rand == 0) ? "Cheddar" :
                     (temp_rand == 1) ? "Edam" :
                     (temp_rand == 2) ? "Wensleydale" :
                     (temp_rand == 3) ? "Camembert" :
                     (temp_rand == 4) ? "Goat cheese" :
                     (temp_rand == 5) ? "Fruit cheese" :
                     (temp_rand == 6) ? "Mozzarella" :
                     (temp_rand == 7) ? "Sheep cheese"
                                      : "Yak cheese");

        strcat(info, ".");
        mpr(info);
        break;
    case FOOD_SAUSAGE:
        mpr("This sausage is delicious!");
        break;
    default:
        break;
    }

    start_delay( DELAY_EAT, delay_needed, from_inv, id );
}                               // end eating()

static bool can_ingest( const item_def &item, bool suppress_msg )
{
    bool survey_says = false;

    bool ur_carnivorous = (you.species == SP_GHOUL
                           || you.species == SP_KOBOLD
                           || you.mutation[MUT_CARNIVOROUS] == 3);

    bool ur_herbivorous = (you.mutation[MUT_HERBIVOROUS] > 1);

    // ur_chunkslover not defined in terms of ur_carnivorous because
    // a player could be one and not the other IMHO - 13mar2000 {dlb}
    bool ur_chunkslover = (you.hunger_state <= HS_HUNGRY
                           || player_equip( EQ_AMULET, AMU_THE_GOURMAND )
                           || you.species == SP_KOBOLD
                           || you.species == SP_OGRE
                           || you.species == SP_TROLL
                           || you.species == SP_GHOUL
                           || you.mutation[MUT_CARNIVOROUS]);

    // other object types are set to return false for now until
    // someone wants to recode the eating code to permit consumption
    // of things other than just food -- corpses first, then more
    // exotic stuff later would be good to explore - 13mar2000 {dlb}
    if (item.base_type == OBJ_FOOD)
    {
        survey_says = true;

        if (food_is_veg( item ))
        {
            if (ur_carnivorous)
            {
                survey_says = false;
                if (!suppress_msg)
                    mpr("Sorry, you're a carnivore.");
            }
        }
        else if (item.sub_type == FOOD_CHUNK)
        {
            if (ur_herbivorous)
            {
                survey_says = false;
                if (!suppress_msg)
                    mpr("You can't eat raw meat!");
            }
            else if (!ur_chunkslover)
            {
                survey_says = false;
                if (!suppress_msg)
                    mpr("You aren't quite hungry enough to eat that!");
            }
        }
    }

    return (survey_says);
}                               // end can_ingest()

// see if you can follow along here -- except for the Amulet of the Gourmand
// addition (long missing and requested), what follows is an expansion of how
// chunks were handled in the codebase up to this date -- I have never really
// understood why liches are hungry and not true undead beings ... {dlb}:
static int determine_chunk_effect( int which_chunk_type, bool rotten_chunk )
{
    int this_chunk_effect = which_chunk_type;

    // cover resistances to bad effects (makes them merely contaminated) -- bwr
    switch (this_chunk_effect)
    {
    case CE_HCL:
    case CE_MUTAGEN_RANDOM:
        if (you.is_undead)
            this_chunk_effect = CE_CONTAMINATED;
        break;

    case CE_POISONOUS:
        if (player_res_poison() > 1)
            this_chunk_effect = CE_CONTAMINATED;
        break;

    default:
        break;
    }

    // now apply resistance to contaminated meat -- bwr
    if (this_chunk_effect == CE_CONTAMINATED)
    {
        switch (you.species)
        {
        case SP_GHOUL:
            // Ghouls love contaminated meat, pass it through without save.
            break;

        case SP_TROLL:
            if (!one_chance_in(15))
                this_chunk_effect = CE_CLEAN;
            break;

        case SP_HILL_ORC:
        case SP_KOBOLD:
        case SP_OGRE:
            if (!one_chance_in(5))
                this_chunk_effect = CE_CLEAN;
            break;

        default:
            if (coinflip())
                this_chunk_effect = CE_CLEAN;
            break;
        }
    }

    // now apply the decay state without clobbering unnecessarily -- bwr
    if (rotten_chunk && !player_equip( EQ_AMULET, AMU_THE_GOURMAND ))
    {
        switch (this_chunk_effect)
        {
        case CE_CLEAN:
        case CE_CONTAMINATED:
            if (you.attribute[ATTR_TRANSFORMATION] != TRAN_LICH)
            {
                switch (you.species)
                {
                case SP_KOBOLD:
                case SP_TROLL:
                    if (one_chance_in(3))
                        this_chunk_effect = CE_ROTTEN;
                    break;
                case SP_HILL_ORC:
                case SP_OGRE:
                    if (!one_chance_in(3))
                        this_chunk_effect = CE_ROTTEN;
                    break;
                default:
                    this_chunk_effect = CE_ROTTEN;
                    break;
                }
            }
            break;

        case CE_MUTAGEN_RANDOM:
            this_chunk_effect = CE_MUTAGEN_BAD;
            break;

        default:
            // XXX: all others are already bad enough or worse than rotten?
            // -- we probably eventually want these as flags so we can
            // have poisonous, rotten meat (for lots of nasty).
            break;
        }
    }

    // Ghouls with amulets of the gormand have chunks made tastier. -- bwr
    if (you.species == SP_GHOUL && player_equip( EQ_AMULET, AMU_THE_GOURMAND ))
    {
        // have to handle rotten_chunk here to undo the above "AotG protection
        if (rotten_chunk
            || (this_chunk_effect == CE_CONTAMINATED && one_chance_in(3)))
        {
            this_chunk_effect = CE_ROTTEN;
        }
        else if (this_chunk_effect == CE_CLEAN && one_chance_in(3))
            this_chunk_effect = CE_CONTAMINATED;
    }

    return (this_chunk_effect);
}                               // end determine_chunk_effect()

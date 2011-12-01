/*
 *  File:       shopping.cc
 *  Summary:    Shop keeper functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <3>     Jul 30 00      JDJ      in_a_shop uses shoppy instead of i when calling shop_set_id.
 *      <2>     Oct 31 99      CDL      right justify prices
 *      <1>     -/--/--        LRH      Created
 */

#include "AppHdr.h"
#include "chardump.h"
#include "shopping.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef DOS
#include <conio.h>
#endif

#include "globals.h"
#include "externs.h"

#include "describe.h"
#include "invent.h"
#include "items.h"
#include "itemname.h"
#include "itemprop.h"
#include "macro.h"
#include "player.h"
#include "randart.h"
#include "spl-book.h"
#include "spl-util.h"
#include "stash.h"
#include "stuff.h"

// stat multiples
#define PB_STAT         20
#define PB_AC_EV        18
#define PB_HIT_DAM      15

#define PB_ELEC         40
#define PB_FIRE         30
#define PB_COLD         30
#define PB_NEG          25
#define PB_POIS         20
#define PB_MAG          20

#define PB_MULTIPLE     25

#define PB_STEALTH      20
#define PB_SPELL        10


static void in_a_shop( char shoppy, char id[NUM_IDTYPE][MAX_SUBTYPES] );
static char more3(void);
static void purchase( int shop, int item_got, int cost );
static void shop_init_id( int i, FixedArray < int, 4, 50 > &shop_id );
static void shop_print( const char *shoppy, char sh_line );
static void shop_set_ident_type( int i, FixedArray < int, 4, 50 > &shop_id,
                                 unsigned char base_type, unsigned char sub_type );
static void shop_uninit_id( FixedArray < int, 4, 50 > &shop_id );

static int shop_value( int greed, int ival )
{
    // greed used to be in 10% increments, now we've upped the granularity
    // while leaving greed = 10 at 100%.
    int bonus = (ival * (greed - 10)) / 20;

    if (bonus > greed * 10)
        bonus = greed * 10;

    // int gp_value = (ival * (greed + 10)) / 20;
    int gp_value = ival + bonus;
    if (gp_value < 1)
        gp_value = 1;

    return (gp_value);
}

void in_a_shop( char shoppy, char id[NUM_IDTYPE][MAX_SUBTYPES] )
{
    // easier to work with {dlb}
    unsigned int greedy = env.shop[shoppy].greed;

    FixedArray< int, 4, 50 >            shop_id;
    FixedVector< int, MAX_SHOP_ITEMS >  shop_items;

    char st_pass[ ITEMNAME_SIZE ] = "";
    unsigned int gp_value = 0;
    int i;
    int ft;
    int num_items;

#ifdef DOS_TERM
    char buffer[4800];
    gettext(1, 1, 80, 25, buffer);
#endif

#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif

    clrscr();
    int itty = 0;

    shop_init_id(shoppy, shop_id);
    save_id(id);

    for (;;)
    {
        clrscr();
        gotoxy(1, 1);
        textcolor( 1 + (env.shop[shoppy].x + env.shop[shoppy].y) % 15 );
#if DEBUG_DIAGNOSTICS
        cprintf( "   Welcome to %s! (greed %d%%)",
                  shop_name( env.shop[shoppy].x, env.shop[shoppy].y ),
                  5 * (env.shop[shoppy].greed + 10) );
#else
        cprintf( "   Welcome to %s!",
                  shop_name( env.shop[shoppy].x, env.shop[shoppy].y ) );
#endif

#ifdef STASH_TRACKING
        ShopInfo &si = stashes.get_shop( env.shop[shoppy].x, env.shop[shoppy].y );
        si.set_name( shop_name( env.shop[shoppy].x, env.shop[shoppy].y ) );
#endif

        textcolor(LIGHTGREY);

        // collect items into shop_items[] for referencing:
        itty = igrd[0][5 + shoppy];

#ifdef STASH_TRACKING
        si.reset();
#endif

        if (itty == NON_ITEM)
        {
            shop_print("I'm sorry, my shop is empty now.", 20);
            more3();
            break;
        }

        num_items = 0;
        for (i = 0; i < MAX_SHOP_ITEMS; i++)
        {
            if (itty == NON_ITEM)
            {
                // Setting all remaining elements of the array to
                // NON_ITEM so that we can catch bugs in this mess. -- bwr
                shop_items[i] = NON_ITEM;
                continue;
            }

            num_items++;
            shop_items[i] = itty;
            itty = mitm[itty].link;
        }

        // display items:
        for (i = 0; i < num_items; i++)
        {
            gotoxy( 1, i + 2 );

            // XXX: Allowing item colours to be displayed introduces a
            // player ID system for things like potions and wands...
            // we're considering that OK for now, but this might go away
            // later (I'm going with "quaint trick" right now, and waiting
            // to see if it's grossly abusable).  Also, note the hack
            // so that scroll shops (all items WHITE) are more readable. -- bwr
            if (Options.invent_colours && env.shop[shoppy].type != SHOP_SCROLL)
                textcolor( mitm[ shop_items[i] ].colour );
            else
                textcolor( (i % 2) ? WHITE : LIGHTGREY );

            putch( index_to_letter(i) );
            cprintf( " - " );
            it_name( shop_items[i], DESC_NOCAP_A, st_pass, false, true );
            cprintf( st_pass );

            gp_value = shop_value(greedy, item_value(mitm[shop_items[i]], id));

            gotoxy( 69, i + 2 );
            cprintf( "%5d gold", gp_value );

#ifdef STASH_TRACKING
            string desc;

            if (is_dumpable_artefact(mitm[shop_items[i]], Options.verbose_dump))
            {
                desc = munge_description(
                        get_item_description( mitm[shop_items[i]],
                                              Options.verbose_dump,
                                              true ));
            }

            si.add_item( Stash::stash_item_name(mitm[shop_items[i]]),
                         desc, gp_value);
#endif
        }

        textcolor(LIGHTGREY);
        shop_print("Type letter to buy item, x/Esc to leave, ?/* for inventory, v to examine.", 23);

      prompt:
        snprintf( info, INFO_SIZE, "You have %d gold piece%s.", you.gold,
                  (you.gold == 1) ? "" : "s" );

        textcolor(YELLOW);
        shop_print( info, 19 );

        textcolor(CYAN);
        shop_print( "What would you like to purchase?", 20 );
        textcolor(LIGHTGREY);

        ft = get_ch();

        if (ft == 'x' || ft == ESCAPE)
            break;
        else if (ft == 'v')
        {
            textcolor(CYAN);
            shop_print("Examine which item?", 20);
            textcolor(LIGHTGREY);
            ft = get_ch();

            if (!isalpha(ft))
                goto huh;

            ft = letter_to_index(ft);

            if (ft >= num_items)
                goto huh;

            describe_item( mitm[ shop_items[ft] ] );
        }
        else if (ft == '?' || ft == '*')
        {
            shop_uninit_id( shop_id );
            show_invent( -1 );
            shop_init_id( shoppy, shop_id );
#ifdef DOS_TERM
            window(1, 1, 80, 25);
#endif
        }
        else if (isalpha(ft))
        {
            ft = letter_to_index(ft);

            if (ft >= num_items)
                goto huh;

            gp_value = shop_value(greedy, item_value(mitm[shop_items[ft]], id));

            if (gp_value > you.gold)
            {
                shop_print("I'm sorry, you don't seem to have enough money.", 20);
                more3();
                goto prompt;
            }

            shop_set_ident_type( shoppy, shop_id, mitm[shop_items[ft]].base_type,
                                 mitm[shop_items[ft]].sub_type );

            purchase( shoppy, shop_items[ft], gp_value );
        }
#ifdef DEBUG_DIAGNOSTICS
        else if (ft == '&' && num_items > 0)
        {
            bool on = true;

            if (item_ident( mitm[shop_items[0]], ISFLAG_KNOW_TYPE ))
                on = false;

            // Toggle id status of inventory.
            for (i = 0; i < num_items; i++)
            {
                if (on)
                    set_ident_flags( mitm[shop_items[i]], ISFLAG_IDENT_MASK );
                else
                    unset_ident_flags( mitm[shop_items[i]], ISFLAG_IDENT_MASK );
            }

            // toggling types (this might get screwy)
            const int tid = (on) ? ID_KNOWN_TYPE : ID_UNKNOWN_TYPE;

            for (i = 0; i < MAX_SUBTYPES; i++)
            {
                id[ IDTYPE_WANDS ][i] = tid;
                set_ident_type( OBJ_WANDS, i, tid );

                id[ IDTYPE_SCROLLS ][i] = tid;
                set_ident_type( OBJ_SCROLLS, i, tid );

                id[ IDTYPE_JEWELLERY ][i] = tid;
                set_ident_type( OBJ_JEWELLERY, i, tid );

                id[ IDTYPE_POTIONS ][i] = tid;
                set_ident_type( OBJ_POTIONS, i, tid );
            }
        }
#endif
        else
        {
          huh:
            shop_print("Huh?", 20);
            more3();
            goto prompt;
        }
    }

    //clear_line();
    shop_print("Goodbye!", 20);
    more3();

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
    gotoxy(1, 1);
    cprintf(" ");
#endif

    shop_uninit_id( shop_id );
}

void shop_init_id( int i, FixedArray<int, NUM_IDTYPE, MAX_SUBTYPES> &shop_id )
{
    for (int j = 0; j < MAX_SUBTYPES; j++)
    {
        shop_id[ IDTYPE_WANDS ][j] = get_ident_type(OBJ_WANDS, j);
        shop_id[ IDTYPE_SCROLLS ][j] = get_ident_type(OBJ_SCROLLS, j);
        shop_id[ IDTYPE_JEWELLERY ][j] = get_ident_type(OBJ_JEWELLERY, j);
        shop_id[ IDTYPE_POTIONS ][j] = get_ident_type(OBJ_POTIONS, j);

        if (env.shop[i].type != SHOP_WEAPON_ANTIQUE
            && env.shop[i].type != SHOP_ARMOUR_ANTIQUE
            && env.shop[i].type != SHOP_GENERAL_ANTIQUE)
        {
            set_ident_type(OBJ_WANDS, j, ID_KNOWN_TYPE);
            set_ident_type(OBJ_SCROLLS, j, ID_KNOWN_TYPE);
            set_ident_type(OBJ_JEWELLERY, j, ID_KNOWN_TYPE);
            set_ident_type(OBJ_POTIONS, j, ID_KNOWN_TYPE);
        }
    }
}

void shop_uninit_id( FixedArray<int, NUM_IDTYPE, MAX_SUBTYPES> &shop_id )
{
    for (int j = 0; j < MAX_SUBTYPES; j++)
    {
        set_ident_type( OBJ_WANDS, j, shop_id[ IDTYPE_WANDS ][j], true );
        set_ident_type( OBJ_SCROLLS, j, shop_id[ IDTYPE_SCROLLS ][j], true );
        set_ident_type( OBJ_JEWELLERY, j, shop_id[ IDTYPE_JEWELLERY ][j], true );
        set_ident_type( OBJ_POTIONS, j, shop_id[ IDTYPE_POTIONS ][j], true );
    }
}

void shop_set_ident_type( int i,
                          FixedArray<int, NUM_IDTYPE, MAX_SUBTYPES> &shop_id,
                          unsigned char base_type, unsigned char sub_type )
{
    if (env.shop[i].type != SHOP_WEAPON_ANTIQUE
        && env.shop[i].type != SHOP_ARMOUR_ANTIQUE
        && env.shop[i].type != SHOP_GENERAL_ANTIQUE)
    {
        switch (base_type)
        {
        case OBJ_WANDS:
            shop_id[ IDTYPE_WANDS ][sub_type] = ID_KNOWN_TYPE;
            break;
        case OBJ_SCROLLS:
            shop_id[ IDTYPE_SCROLLS ][sub_type] = ID_KNOWN_TYPE;
            break;
        case OBJ_JEWELLERY:
            shop_id[ IDTYPE_JEWELLERY ][sub_type] = ID_KNOWN_TYPE;
            break;
        case OBJ_POTIONS:
            shop_id[ IDTYPE_POTIONS ][sub_type] = ID_KNOWN_TYPE;
            break;
        }
    }
}

void shop_print( const char *shoppy, char sh_lines )
{
    gotoxy(1, sh_lines);

    cprintf(shoppy);

    for (int i = strlen(shoppy); i < 80; i++)
        cprintf(" ");
}

char more3(void)
{
    char keyin = 0;

    gotoxy(70, 20);
    cprintf( MORE_STRING );
    keyin = getch();
    if (keyin == 0)
        getch();
    //clear_line();
    return keyin;
}

static void purchase( int shop, int item_got, int cost )
{
    you.gold -= cost;

    int num = move_item_to_player( item_got, mitm[item_got].quantity, true );

    // Shopkeepers will now place goods you can't carry outside the shop.
    if (num < mitm[item_got].quantity)
    {
        snprintf( info, INFO_SIZE, "I'll put %s outside for you.",
                 (mitm[item_got].quantity == 1) ? "it" :
                 (num > 0)                      ? "the rest"
                                                : "these" );

        shop_print( info, 20 );
        more3();

        move_item_to_grid( &item_got, env.shop[shop].x, env.shop[shop].y );
    }
}                               // end purchase()

static inline int plus_bonus_value( int mult, int plus )
{
    if (plus > 0)
        return (2 * mult + mult * plus * plus);
    else
        return (mult * plus * 2);
}

// This is also used in make_item_randart() to limit some of the insanely
// powerful aretfacts that regularly get produced now that we seriously
// try to avoid null artefacts. -- bwr
int randart_value( const item_def &item )
{
    ASSERT( is_random_artefact( item ) );

    int cost = 0;

    // Note: Brands are already accounted for via existing ego checks

    cost += plus_bonus_value( PB_AC_EV, item.ra_props[RAP_AC] );
    cost += plus_bonus_value( PB_AC_EV, item.ra_props[RAP_EVASION] );

    cost += plus_bonus_value( PB_HIT_DAM, item.ra_props[RAP_ACCURACY] );
    cost += plus_bonus_value( PB_HIT_DAM, item.ra_props[RAP_DAMAGE] );

    cost += plus_bonus_value( PB_STAT, item.ra_props[RAP_STRENGTH] );
    cost += plus_bonus_value( PB_STAT, item.ra_props[RAP_INTELLIGENCE] );
    cost += plus_bonus_value( PB_STAT, item.ra_props[RAP_DEXTERITY] );

    // simple base value for pluses... +'ve opposites get additional bonuses
    // both AC and EV bonus
    if ((item.ra_props[RAP_AC] > 0
            || (item.base_type == OBJ_WEAPONS
                && item.special == SPWPN_PROTECTION)
            || (item.base_type == OBJ_ARMOUR
                && item.special == SPARM_PROTECTION)
            || (item.base_type == OBJ_JEWELLERY
                && item.sub_type == RING_PROTECTION
                && item.plus > 0))
        && (item.ra_props[RAP_EVASION] > 0
            || (item.base_type == OBJ_JEWELLERY
                && item.sub_type == RING_EVASION
                && item.plus > 0)))
    {
        cost += plus_bonus_value( PB_MULTIPLE, 1 );
    }

    // bonus for substantial (more than 1 point) positive stats (even just 1):
    const int num_stats = (item.ra_props[RAP_STRENGTH] > 1)
                            + (item.ra_props[RAP_INTELLIGENCE] > 1)
                            + (item.ra_props[RAP_DEXTERITY] > 1)
                            + (item.base_type == OBJ_JEWELLERY
                                && (item.sub_type == RING_STRENGTH
                                    || item.sub_type == RING_DEXTERITY
                                    || item.sub_type == RING_INTELLIGENCE)
                                && item.plus > 1)
                            + (item.base_type == OBJ_ARMOUR
                                && (item.special == SPARM_STRENGTH
                                    || item.special == SPARM_DEXTERITY
                                    || item.special == SPARM_INTELLIGENCE));

    if (num_stats > 0)
        cost += plus_bonus_value( PB_MULTIPLE, num_stats );

    // now do resistances (with bonus for multiple)
    int res = 0;

    // Fire and cold resistances have meaningful levels and negatives
    cost += plus_bonus_value( PB_FIRE, 2 * item.ra_props[RAP_FIRE] );
    res += (item.ra_props[RAP_FIRE] > 0);

    cost += plus_bonus_value( PB_COLD, 2 * item.ra_props[RAP_COLD] );
    res += (item.ra_props[RAP_COLD] > 0);

    // multiple levels but no negative
    cost += plus_bonus_value( PB_NEG, 2 * item.ra_props[RAP_NEGATIVE_ENERGY] );
    res += (item.ra_props[RAP_NEGATIVE_ENERGY] > 0);

    // multiple levels now:
    cost += plus_bonus_value( PB_POIS, 2 * item.ra_props[RAP_POISON] );
    res += (item.ra_props[RAP_POISON] > 0);

    // only one meaningful level (but harder to get):
    if (item.ra_props[RAP_ELECTRICITY] > 0)
    {
        cost += plus_bonus_value( PB_ELEC, 2 );
        res++;
    }

    // magic resistance is from 20-60
    if (item.ra_props[RAP_MAGIC] > 0)
    {
        cost += plus_bonus_value( PB_MAG, 2 ); // level not in description
        res++;
    }

    // Multiple types of resistance bonus
    if (res > 1)
        cost += plus_bonus_value( PB_MULTIPLE, res );

    // These normally come alone or in resist/susceptible pairs...
    // we're making items a bit more expensive if they have both positive.
    if (item.ra_props[RAP_FIRE] > 0 && item.ra_props[RAP_COLD] > 0)
        cost += 150;

    if (item.ra_props[RAP_EYESIGHT])
        cost += 150;

    // ranges from -100 to 80
    if (item.ra_props[RAP_STEALTH])
    {
        const int as = abs( item.ra_props[RAP_STEALTH] );
        const int lvl = (item.ra_props[RAP_STEALTH] / as) * ((as > 20) ? 2 : 1);

        cost += plus_bonus_value( PB_STEALTH, lvl );
    }

    // abilities:
    if (item.ra_props[RAP_LEVITATE])
        cost += plus_bonus_value( PB_SPELL, spell_level( SPELL_LEVITATION ) );

    if (item.ra_props[RAP_BLINK])
        cost += plus_bonus_value( PB_SPELL, spell_level( SPELL_BLINK ) );

    if (item.ra_props[RAP_BERSERK])
        cost += plus_bonus_value( PB_SPELL, spell_level( SPELL_BERSERKER_RAGE ) );

    if (item.ra_props[RAP_CAN_TELEPORT])
        cost += plus_bonus_value( PB_SPELL, spell_level( SPELL_TELEPORT_SELF ) );

    if (item.ra_props[RAP_MAPPING])
        cost += plus_bonus_value( PB_SPELL, spell_level( SPELL_MAGIC_MAPPING ) );

    if (item.ra_props[RAP_INVISIBLE])
        cost += plus_bonus_value( PB_SPELL, spell_level( SPELL_INVISIBILITY ) );

    // curses:
    if (item.ra_props[RAP_NOISES])
        cost -= 75;

    if (item.ra_props[RAP_ANGRY])
        cost -= 75;

    if (item.ra_props[RAP_CAUSE_TELEPORTATION])
        cost -= 100;

    if (item.ra_props[RAP_PREVENT_TELEPORTATION])
        cost -= 150;

    if (item.ra_props[RAP_PREVENT_SPELLCASTING])
        cost -= 300;

    // ranges from 2-5
    if (item.ra_props[RAP_MUTAGENIC])
        cost -= ((item.ra_props[RAP_MUTAGENIC] <= 3) ? 150 : 250);

    // ranges from 1-3
    if (item.ra_props[RAP_METABOLISM])
        cost -= ((item.ra_props[RAP_METABOLISM] < 3) ? 75 : 100);

    return ((cost > 0) ? cost : 0);
}

// cover value for quickly getting an item's value
unsigned int item_value( item_def item )
{
    char  identy[NUM_IDTYPE][MAX_SUBTYPES];

    save_id( identy );  // make local copy of id table

    return (item_value( item, identy, false ));
}

unsigned int item_value( item_def item, char id[NUM_IDESC][MAX_SUBTYPES], bool ident )
{
    // Note that we pass item in by value, since we want a local
    // copy to mangle as neccessary... maybe that should be fixed,
    // but this function isn't called too often.
    item.flags = (ident) ? (item.flags | ISFLAG_IDENT_MASK) : (item.flags);

    const bool known = item_ident( item, ISFLAG_KNOW_TYPE );

    int valued = 0;             // value in gold
    int div = 1;                // amount needed to divide by to get back
    int min = 5;                // minimum allowed value for an item
    int eq_slot;                // equipment slot of armour piece
    int race;                   // equipment racial type
    appearance_type app;        // appearance of item
    int pluses;

    switch (item.base_type)
    {
    case OBJ_WEAPONS:
        // handle fixed artefacts separately and return early:
        if (is_fixed_artefact( item ))
        {
            if (item_ident( item, ISFLAG_KNOW_PROPERTIES ))
            {
                switch (item.special)
                {
                case SPWPN_SCEPTRE_OF_ASMODEUS:
                    valued += 3000;
                    break;

                case SPWPN_STAFF_OF_DISPATER:
                    valued += 2500;
                    break;

                case SPWPN_SWORD_OF_CEREBOV:
                    valued += 2000;
                    break;

                case SPWPN_SWORD_OF_POWER:
                case SPWPN_STAFF_OF_OLGREB:
                    valued += 1500;
                    break;

                case SPWPN_SCYTHE_OF_CURSES:
                    valued += 1313;
                    break;

                case SPWPN_GLAIVE_OF_PRUNE:
                case SPWPN_MACE_OF_VARIABILITY:
                case SPWPN_SCEPTRE_OF_TORMENT:
                case SPWPN_SWORD_OF_ZONGULDROK:
                case SPWPN_WRATH_OF_TROG:
                    valued += 1200;
                    break;

                case SPWPN_KNIFE_OF_ACCURACY:
                case SPWPN_VAMPIRES_TOOTH:
                    valued += 1000;
                    break;

                case SPWPN_SINGING_SWORD:
                case SPWPN_STAFF_OF_WUCAD_MU:
                default:
                    valued += 800;
                    break;
                }
                break;          // break out of OBJ_WEAPONS
            }
        }                       // end uniques

        // base:
        switch (item.sub_type)
        {
        case WPN_CLUB:
        case WPN_SLING:
            valued += 5;
            break;

        case WPN_KNIFE:
            valued += 7;
            break;

        default:
        case WPN_DAGGER:
        case WPN_BLOWGUN:
        case WPN_WHIP:
            valued += 10;
            break;

        case WPN_GIANT_CLUB:
        case WPN_HAMMER:
        case WPN_SPEAR:
            valued += 13;
            break;

        case WPN_GIANT_SPIKED_CLUB:
        case WPN_HAND_AXE:
        case WPN_MACE:
        case WPN_SCYTHE:
        case WPN_SHORT_SWORD:
            valued += 15;
            break;

        case WPN_BOW:
        case WPN_FLAIL:
        case WPN_SABRE:
        case WPN_STAFF:
        case WPN_TRIDENT:
            valued += 20;
            break;

        case WPN_ANCUS:
        case WPN_CROSSBOW:
        case WPN_FALCHION:
        case WPN_HALBERD:
        case WPN_HAND_CROSSBOW:
        case WPN_MORNINGSTAR:
        case WPN_QUARTERSTAFF:
            valued += 25;
            break;

        case WPN_GLAIVE:
        case WPN_LONG_SWORD:
        case WPN_LONGBOW:
            valued += 30;
            break;

        case WPN_EVENINGSTAR:
        case WPN_SCIMITAR:
            valued += 33;
            break;

        case WPN_SPIKED_FLAIL:
        case WPN_WAR_AXE:
            valued += 35;
            break;

        case WPN_BROAD_AXE:
        case WPN_GREAT_SWORD:
            valued += 40;
            break;

        case WPN_BATTLEAXE:
        case WPN_GREAT_MACE:
            valued += 50;
            break;

        case WPN_DIRE_FLAIL:
        case WPN_LOCHABER_AXE:
            valued += 55;
            break;

        case WPN_DEMON_TRIDENT:
        case WPN_DEMON_WHIP:
        case WPN_DEMON_BLADE:
        case WPN_DOUBLE_SWORD:
            valued += 100;
            break;

        case WPN_QUICK_BLADE:
            valued += 150;
            break;

        case WPN_BLESSED_BLADE:
        case WPN_EXECUTIONERS_AXE:
        case WPN_KATANA:
        case WPN_LAJATANG:
        case WPN_TRIPLE_SWORD:
            valued += 200;
            break;
        }

        // modification for racial types:
        race = get_equip_race( item );

        if (race == ISFLAG_ELVEN)
            valued = (valued * 12) / 10;
        else if (race == ISFLAG_DWARVEN)
            valued = ((valued + 10) * 12) / 10;
        else if (race == ISFLAG_ORCISH)
            valued = (valued *  9) / 10;

        // ego modification:
        if (known)
        {
            int mult = 0;

            switch (get_weapon_brand( item ))
            {
            case SPWPN_NORMAL:
            default:            // randart
                break;

            case SPWPN_VAMPIRICISM:
            case SPWPN_SPEED:
                mult = 30;
                break;

            case SPWPN_DISRUPTION:
                mult = 25;
                break;

            case SPWPN_PAIN:
            case SPWPN_DISTORTION:
            case SPWPN_ELECTROCUTION:
                mult = 20;
                break;

            case SPWPN_PROTECTION:
            case SPWPN_HOLY_WRATH:
            case SPWPN_VORPAL:
            case SPWPN_DRAINING:
                mult = 15;
                break;

            case SPWPN_FLAMING:
            case SPWPN_FREEZING:
            case SPWPN_VENOM:
            case SPWPN_REACHING:
                mult = 10;
                break;

            case SPWPN_ORC_SLAYING:
                mult = 5;
                break;
            }

            // Note: valued == valued, if mult == 0
            valued = (valued * (mult + 10)) / 10 + 15 * mult;
        }
        else if (is_demonic( item ))
        {
            // demonic weapons are often ego, assume mult = 15
            valued = (valued * 25) / 10 + 225;
        }

        // pluses modification:
        if (item_ident( item, ISFLAG_KNOW_PLUSES ))
        {
            int mult = 0;

            if (item.plus > 0)
            {
                valued += 2 + item.plus;

                mult += item.plus;
                if (item.plus > MAX_WPN_ENCHANT)
                    mult += 2 + (item.plus - MAX_WPN_ENCHANT);
            }
            else if (item.plus < 0)
            {
                valued -= 2;
                valued += (item.plus * item.plus * item.plus);

                if (valued < 1)
                    valued = 1;
            }

            if (item.plus2 > 0)
            {
                valued += 3 + item.plus2 * 2;

                mult += item.plus2;
                if (item.plus2 > MAX_WPN_ENCHANT)
                    mult += 2 + (item.plus2 - MAX_WPN_ENCHANT);
            }
            else if (item.plus2 < 0)
            {
                valued -= 3;
                valued += (item.plus2 * item.plus2 * item.plus2);

                if (valued < 1)
                    valued = 1;
            }

            if (mult)
            {
                // limit the value gain from pluses
                int pval = 5 + valued / 10;
                int max = 20;

                if (is_random_artefact( item ))
                {
                    pval += 5;
                    max = 50;
                }

                if (pval > max)
                    pval = max;

                valued += (mult * pval + 1) / 2;
            }
        }

        // appearance factor: special artefact or just fancy item mod:
        app = item_appearance( item );

        if (app == APP_FANCY)
        {
            if (!known)
                valued += 75 + valued / 2;
            else
            {
                const int runes = 5 + valued / 25;
                valued += (runes > 50) ? 50 : runes;
            }
        }
        else if (app == APP_SPECIAL)
        {
            valued += valued / 10;
            if (known && is_random_artefact( item ))
                valued += randart_value(item);
            else
                valued += 200;
        }

        // cursed modification:
        if (item_known_cursed( item ))
        {
            if (valued > 75)
                valued -= 30;
            else
                valued = (valued * 6) / 10;
        }
        break;

    case OBJ_MISSILES:          // ammunition
        min = 1;
        div = 10;

        switch (item.sub_type)
        {
        case MI_LARGE_ROCK:
        case MI_STONE:
        default:
            valued += 1;
            break;

        case MI_DART:
        case MI_NEEDLE:
            valued += 2;
            break;

        case MI_ARROW:
        case MI_BOLT:
            valued += 3;
            break;
        }

        // Note: poisoned are more valuable because they're not consumed
        switch (get_ammo_brand( item ))
        {
        case SPMSL_FLAME:
        case SPMSL_ICE:
            if (known)
                valued += 1;
            break;

        case SPMSL_POISONED:
        case SPMSL_POISONED_UNUSED:
            valued += 2;        // always known
            break;

        default:
        case SPMSL_NORMAL:
            break;
        }

        if (item_ident( item, ISFLAG_KNOW_PLUSES ))
            valued += (3 * item.plus) / 5;

        // modification for racial types:
        race = get_equip_race( item );

        if (race == ISFLAG_ELVEN || race == ISFLAG_DWARVEN)
            valued += 1;
        else if (race == ISFLAG_ORCISH)
            valued -= 1;

        if (item_appearance( item ) == APP_FANCY)
            valued += 1 + 2 * (!known);

        // at least 1 every 10
        if (valued < 1)
            valued = 1;
        break;

    case OBJ_ARMOUR:
        eq_slot = get_armour_slot( item );

        // base value:
        switch (item.sub_type)
        {
        case ARM_GOLD_DRAGON_ARMOUR:
            valued += 1000;
            break;

        case ARM_GOLD_DRAGON_HIDE:
            valued += 950;
            break;

        case ARM_STORM_DRAGON_ARMOUR:
            valued += 850;
            break;

        case ARM_STORM_DRAGON_HIDE:
            valued += 800;
            break;

        case ARM_DRAGON_ARMOUR:
        case ARM_ICE_DRAGON_ARMOUR:
            valued += 750;
            break;

        case ARM_DRAGON_HIDE:
        case ARM_ICE_DRAGON_HIDE:
            valued += 700;
            break;

        case ARM_CRYSTAL_PLATE_MAIL:
            valued += 600;
            break;

        case ARM_SWAMP_DRAGON_ARMOUR:
            valued += 500;
            break;

        case ARM_MOTTLED_DRAGON_ARMOUR:
            valued += 480;
            break;

        case ARM_SWAMP_DRAGON_HIDE:
        case ARM_STEAM_DRAGON_ARMOUR:
            valued += 450;
            break;

        case ARM_MOTTLED_DRAGON_HIDE:
            valued += 430;
            break;

        case ARM_STEAM_DRAGON_HIDE:
            valued += 400;
            break;

        case ARM_CENTAUR_BARDING:
        case ARM_NAGA_BARDING:
        case ARM_TROLL_LEATHER_ARMOUR:
            valued += 200;
            break;

        case ARM_TROLL_HIDE:
            valued += 150;
            break;

        case ARM_PLATE_MAIL:
            valued += 60;
            break;

        case ARM_BANDED_MAIL:
            valued += 55;
            break;

        case ARM_SPLINT_MAIL:
            valued += 45;
            break;

        case ARM_CHAIN_MAIL:
        case ARM_LARGE_SHIELD:
            valued += 40;
            break;

        case ARM_SHIELD:
            valued += 30;
            break;

        case ARM_SCALE_MAIL:
            valued += 25;
            break;

        case ARM_HELMET:
        case ARM_CLOAK:
        case ARM_BUCKLER:
        case ARM_RING_MAIL:
            valued += 20;
            break;

        case ARM_CAP:
        case ARM_BOOTS:
        case ARM_GLOVES:
        case ARM_STUDDED_LEATHER_ARMOUR:
            valued += 15;
            break;

        case ARM_LEATHER_ARMOUR:
            valued += 10;
            break;

        case ARM_ROBE:
            valued += 5;
            break;

        case ARM_ANIMAL_SKIN:
            valued += 3;
            break;
        }

        // adjustmenst for all the helms and cap sub-types:
        if (eq_slot == EQ_HELMET)
        {
            const int desc = get_helmet_desc( item );
            const int type = get_helmet_type( item );

            // Note that we're adding a bonus to these two if they're
            // not known because they're often ego or better... if
            // they are known, we'll add the appropriate value later.
            if (type == THELM_WIZARD_HAT)    // often ego
                valued += 25 + (!known) * 100;
            else if (type == THELM_SPECIAL)  // these are unique artefacts
                valued = (valued + 25) * 3 + (!known) * 100;

            if (desc == THELM_DESC_HORNED || desc == THELM_DESC_SPIKED)
                valued += 10 + valued / 2;    // extra headbutt damage
            else if (desc == THELM_DESC_JEWELLED)
                valued += 50;
            else if (desc)
                valued += 10;                // winged, plumed, etc
        }

        // racial modification:
        race = get_equip_race( item );

        if (race == ISFLAG_ELVEN)
        {
            if (eq_slot == EQ_CLOAK
                || eq_slot == EQ_BOOTS
                || (eq_slot == EQ_BODY_ARMOUR && !base_armour_is_light( item )))
            {
                valued += 50;
            }

            valued = (valued * 12) / 10;
        }
        else if (race == ISFLAG_DWARVEN)
            valued = ((valued + 10) * 12) / 10;
        else if (race == ISFLAG_ORCISH)
            valued = (valued * 9) / 10;

        // ego modification:
        if (known)
        {
            int bonus = 0;

            // Most of these now simulate the randart bonuses:
            switch (get_armour_ego_type( item ))
            {
            case SPARM_NORMAL:
            default:
                break;

            case SPARM_PONDEROUSNESS:
                bonus -= 75;
                break;

            case SPARM_ARCHMAGI:
                bonus += 1200;
                break;

            case SPARM_RUNNING:
                bonus += 600;
                break;

            case SPARM_PRESERVATION:
                bonus += 350;
                break;

            case SPARM_DARKNESS:
            case SPARM_SEE_INVISIBLE:
            case SPARM_LEVITATION:
                bonus += 200;
                break;

            case SPARM_RESISTANCE:
                bonus += 300 + 2 * plus_bonus_value( PB_FIRE, 2 );
                break;

            case SPARM_COLD_RESISTANCE:
            case SPARM_FIRE_RESISTANCE:
                bonus += plus_bonus_value( PB_FIRE, 2 );
                break;

            case SPARM_POSITIVE_ENERGY:
                bonus += plus_bonus_value( PB_NEG, 2 );
                break;

            case SPARM_DEXTERITY:
            case SPARM_INTELLIGENCE:
            case SPARM_STRENGTH:
                bonus += plus_bonus_value( PB_STAT, 3 );
                break;

            case SPARM_MAGIC_RESISTANCE:
                bonus += plus_bonus_value( PB_MAG, 2 );
                break;

            case SPARM_STEALTH:
                bonus += plus_bonus_value( PB_STEALTH, 2 );
                break;

            case SPARM_POISON_RESISTANCE:
                bonus += plus_bonus_value( PB_POIS, 2 );
                break;

            case SPARM_PROTECTION:
                bonus += plus_bonus_value( PB_AC_EV, 3 );
                break;
            }

            // Boost for non-body armours (who's sole purpose is to carry
            // intrinsics, thus they're more valuable there).
            valued += (bonus * (4 + (eq_slot != EQ_BODY_ARMOUR)) / 4);
        }

        // modification for pluses:
        if (item_ident( item, ISFLAG_KNOW_PLUSES ))
        {
            if (item.plus >= 0)
            {
                int base = 5 + valued / 10;
                int max = 20;

                if (is_random_artefact( item ))
                {
                    base += 5;
                    max = 50;
                }

                if (base > max)
                    base = max;

                int max_plus = armour_max_enchant( item );

                int mult = item.plus;
                if (item.plus > max_plus)
                    mult += 2 + (item.plus - max_plus);

                valued += mult * base;
            }
            else // if (item.plus < 0)
            {
                valued -= 5;
                valued += item.plus * item.plus * item.plus;

                if (valued < 1)
                    valued = 1;
            }
        }

        // appearance factor: special artefact or just fancy item mod:
        app = item_appearance( item );

        if (app == APP_FANCY)
        {
            // Since hides (even manufactured) and crystal plate mail
            // are not made ego, we assume that shop keepers will use
            // the known evaluation here.
            if (!known
                && !armour_is_hide( item, true )
                && item.sub_type != ARM_CRYSTAL_PLATE_MAIL)
            {
                valued += 50 + valued / 2 + 50 * (eq_slot != EQ_BODY_ARMOUR);
            }
            else
            {
                const int runes = 5 + valued / 25;
                valued += (runes > 50) ? 50 : runes;
            }
        }
        else if (app == APP_SPECIAL)
        {
            valued += valued / 10;

            if (eq_slot != EQ_BODY_ARMOUR)
                valued += 50;

            if (known && is_random_artefact( item ))
                valued += randart_value(item);
            else
                valued += 200;
        }

        // cursed modification:
        if (item_known_cursed( item ))
        {
            if (valued > 75)
                valued -= 30;
            else
                valued = (valued * 6) / 10;
        }
        break;

    case OBJ_WANDS:
        min = 50;

        if (!id[ IDTYPE_WANDS ][item.sub_type])
            valued += 200;
        else
        {
            switch (item.sub_type)
            {
            case WAND_FIREBALL:
            case WAND_HASTING:
                valued += 600;
                break;

            case WAND_HEALING:
            case WAND_TELEPORTATION:
                valued += 500;
                break;

            case WAND_INVISIBILITY:
            case WAND_DISINTEGRATION:
            case WAND_LIGHTNING:
                valued += 400;
                break;

            case WAND_COLD:
            case WAND_FIRE:
                valued += 350;
                break;

            case WAND_DIGGING:
                valued += 200;
                break;

            case WAND_FLAME:
            case WAND_FROST:
            case WAND_DRAINING:
            case WAND_PARALYSIS:
                valued += 150;
                break;

            case WAND_ENSLAVEMENT:
            case WAND_POLYMORPH_OTHER:
            case WAND_SLOWING:
                valued += 100;
                break;

            case WAND_CONFUSION:
            case WAND_MAGIC_DARTS:
            case WAND_RANDOM_EFFECTS:
            default:
                valued += 75;
                break;
            }

            if (item_ident( item, ISFLAG_KNOW_PLUSES ))
            {
                if (item.plus == 0)
                    valued -= 50;
                else
                    valued = (valued * (item.plus + 45)) / 50;
            }
        }
        break;

    case OBJ_POTIONS:
        if (!id[ IDTYPE_POTIONS ][item.sub_type])
            valued += 35;
        else
        {
            switch (item.sub_type)
            {
            case POT_EXPERIENCE:
                valued += 750;
                break;

            case POT_MAGIC:
                valued += 250;
                break;

            case POT_GAIN_DEXTERITY:
            case POT_GAIN_INTELLIGENCE:
            case POT_GAIN_STRENGTH:
                valued += 200;
                break;

            case POT_CURE_MUTATION:
                valued += 100;
                break;

            case POT_INVISIBILITY:
            case POT_SPEED:
                valued += 75;
                break;

            case POT_BERSERK_RAGE:
                valued += 70;
                break;

            case POT_RESTORE_ABILITIES:
                valued += 60;
                break;

            case POT_MIGHT:
            case POT_MUTATION:
                valued += 50;
                break;

            case POT_HEAL_WOUNDS:
                valued += 40;
                break;

            case POT_HEALING:
                valued += 35;
                break;

            case POT_PORRIDGE:
                valued += 30;
                break;

            case POT_LEVITATION:
                valued += 25;
                break;

            case POT_CONFUSION:
            case POT_DECAY:
            case POT_DEGENERATION:
            case POT_PARALYSIS:
            case POT_POISON:
            case POT_SLOWING:
            case POT_STRONG_POISON:
            case POT_WATER:
            default:
                valued++;
                break;
            }
        }
        break;

    case OBJ_FOOD:
        min = 1;

        switch (item.sub_type)
        {
        case FOOD_ROYAL_JELLY:
            valued = 100;
            break;

        case FOOD_MEAT_RATION:
            valued = 28;
            break;

        case FOOD_BREAD_RATION:
            valued = 25;
            break;

        case FOOD_HONEYCOMB:
            valued = 15;
            break;

        case FOOD_BEEF_JERKY:
        case FOOD_PIZZA:
            valued = 10;
            break;

        case FOOD_CHEESE:
        case FOOD_SAUSAGE:
            valued = 8;
            break;

        case FOOD_LEMON:
        case FOOD_ORANGE:
        case FOOD_BANANA:
            valued = 6;
            break;

        case FOOD_APPLE:
        case FOOD_APRICOT:
        case FOOD_PEAR:
        case FOOD_SNOZZCUMBER:
            valued = 4;
            break;

        case FOOD_CHOKO:
        case FOOD_LYCHEE:
        case FOOD_RAMBUTAN:
        case FOOD_CHUNK:
            valued = 3;
            break;

        case FOOD_STRAWBERRY:
            valued = 8;
            div = 10;
            break;

        case FOOD_GRAPE:
            valued = 4;
            div = 10;
            break;

        case FOOD_SULTANA:
            valued = 3;
            div = 10;
            break;
        }
        break;

    case OBJ_SCROLLS:
        if (!id[ IDTYPE_SCROLLS ][item.sub_type])
            valued += 40;
        else
        {
            switch (item.sub_type)
            {
            case SCR_ACQUIREMENT:
                valued += 500;
                break;
            case SCR_ENCHANT_WEAPON_III:
            case SCR_VORPALISE_WEAPON:
                valued += 300;
                break;
            case SCR_RECHARGING:
                valued += 150;
                break;
            case SCR_SUMMONING:
                valued += 100;
                break;
            case SCR_ENCHANT_WEAPON_II:
                valued += 80;
                break;
            case SCR_ENCHANT_ARMOUR:
            case SCR_ENCHANT_WEAPON_I:
            case SCR_BLINKING:
                valued += 75;
                break;
            case SCR_TORMENT:
                valued += 66;
                break;
            case SCR_DETECT_CURSE:
                valued += 60;
                break;
            case SCR_TELEPORTATION:
                valued += 50;
                break;
            case SCR_REMOVE_CURSE:
            case SCR_FEAR:
                valued += 40;
                break;
            case SCR_MAGIC_MAPPING:
                valued += 30;
                break;
            case SCR_IDENTIFY:
                valued += 25;
                break;
            case SCR_IMMOLATION:
                valued += 13;
                break;
            case SCR_NOISE:
            case SCR_RANDOM_USELESSNESS:
                valued += 5;
                break;
            case SCR_CURSE_ARMOUR:
            case SCR_CURSE_WEAPON:
            case SCR_FORGETFULNESS:
            case SCR_PAPER:
                valued++;
                break;
            }
        }
        break;

    case OBJ_JEWELLERY:
        min = 25;

        // Note that artefacts are treated as independant from their
        // base type, thus we need !known.  !id gives us the regular
        // rings of the same type (because they might not have
        // the id flag set (since actual ID wasn't necessarily used)).
        if (!known || !id[ IDTYPE_JEWELLERY ][item.sub_type])
        {
            valued += 250 - 50 * item_known_cursed( item );

            // unknown artefacts that have "special" descriptors:
            if (item_appearance( item ) == APP_SPECIAL)
                valued += 250;
        }
        else
        {
            switch (item.sub_type)
            {
            case RING_FIRE:
            case RING_ICE:
                valued += 900;
                break;

            case RING_TELEPORT_CONTROL:
                valued += 600;
                break;

            case RING_INVISIBILITY:
            case AMU_THE_GOURMAND:
                valued += 500;
                break;

            case RING_SUSTENANCE:
            case RING_REGENERATION:
            case RING_SUSTAIN_ABILITIES:
            case AMU_RESIST_MUTATION:
                valued += 400;
                break;

            case RING_PROTECTION_FROM_COLD:
            case RING_PROTECTION_FROM_FIRE:
            case RING_LIFE_PROTECTION:
            case AMU_CONSERVATION:
            case AMU_CLARITY:
                valued += 350;
                break;

            case RING_WIZARDRY:
            case RING_MAGICAL_POWER:
            case RING_PROTECTION_FROM_MAGIC:
            case AMU_RESIST_SLOW:
            case AMU_RESIST_CORROSION:
                valued += 300;
                break;

            case AMU_RAGE:
            case AMU_CONTROLLED_FLIGHT:
                valued += 280;
                break;

            case RING_POISON_RESISTANCE:
            case RING_SEE_INVISIBLE:
                valued += 250;
                break;

            case AMU_WARDING:
                valued += 200;
                break;

            case RING_TELEPORTATION: // normally cursed, which makes it cheaper
                valued += 150;
                break;

            case RING_STRENGTH:
            case RING_DEXTERITY:
            case RING_INTELLIGENCE:
                valued += PB_STAT * 5;
                break;

            case RING_SLAYING:
                valued += PB_HIT_DAM * 10;
                break;

            case RING_EVASION:
            case RING_PROTECTION:
                valued += PB_AC_EV * 5;
                break;

            case RING_LEVITATION:
                valued += 100;
                break;

            case RING_HUNGER:
            case AMU_INACCURACY:
                valued += 25;
                break;
                // XXX: got to do delusion!
            }

            pluses = ring_has_pluses( item );

            if (pluses && item_ident( item, ISFLAG_KNOW_PLUSES ))
            {
                int bonus = plus_bonus_value( valued / 5, item.plus );

                if (pluses == 2)
                {
                    bonus += plus_bonus_value( valued / 5, item.plus2 );
                    bonus /= 2;

                    // match above bonus for +to-him with +to-dam
                    if (item.plus > 0 && item.plus2 > 0)
                        bonus += plus_bonus_value( PB_MULTIPLE, 1 );
                }

                valued += bonus;
            }

            // appearance factor: no "good" markers just artefact or not
            if (item_appearance( item ) == APP_SPECIAL)
            {
                // Checking for artefact in case later we have "special"
                // looking things that aren't... note that unknown artefacts
                // are already handled at the top of this case.
                valued += valued / 10;
                if (known && is_random_artefact( item ))
                    valued += randart_value(item);
                else
                    valued += 200;
            }

            // cursed modification:
            if (item_known_cursed( item ))
            {
                if (valued > 125)
                    valued -= 50;
                else
                    valued = (valued * 6) / 10;
            }
        }
        break;

    case OBJ_MISCELLANY:
        switch (item.sub_type)
        {
        case MISC_RUNE_OF_ZOT:  // upped from 1200 to encourage collecting
            valued += (known) ? 2000 : 1500;
            break;

        case MISC_HORN_OF_GERYON:
            valued += (known) ? 1000 : 750;
            break;

        case MISC_DECK_OF_WONDERS:
        case MISC_DECK_OF_POWER:
            valued += (known) ? 700 : 250;
            break;

        case MISC_DISC_OF_STORMS:
            valued += (known) ? 500 : 250;
            break;

        case MISC_DECK_OF_SUMMONINGS:
        case MISC_DECK_OF_TRICKS:
        case MISC_LANTERN_OF_SHADOWS:
        case MISC_STONE_OF_EARTH_ELEMENTALS:
        case MISC_CRYSTAL_BALL_OF_SEEING:
        case MISC_CRYSTAL_BALL_OF_ENERGY:
        default:
            valued += (known) ? 350 : 250;
            break;

        case MISC_BOTTLED_EFREET:
        case MISC_BOX_OF_BEASTS:
        case MISC_AIR_ELEMENTAL_FAN:
        case MISC_PORTABLE_ALTAR_OF_NEMELEX:
        case MISC_LAMP_OF_FIRE:
        case MISC_ROD_OF_STRIKING:
            valued += 250;
            break;

        case MISC_CRYSTAL_BALL_OF_FIXATION:
        case MISC_EMPTY_EBONY_CASKET:
            valued += (known) ? 50 : 250;
            break;
        }
        break;

    case OBJ_BOOKS:
        // rarity = spell books: 1-20, manuals 20, tome of destruction: 30
        valued = 200 + 40 * ((known) ? book_rarity(item.sub_type) : 4);
        break;

    case OBJ_STAVES:
        if (!known)
            valued = 750;
        else
        {
            switch (item.sub_type)
            {
            case STAFF_FIRE:
            case STAFF_COLD:
            case STAFF_AIR:
            case STAFF_EARTH:
            case STAFF_DEATH:
            case STAFF_CONJURATION:
            case STAFF_POISON:
                valued += 1000;
                break;

            case STAFF_POWER:
            case STAFF_ENERGY:
            case STAFF_CHANNELING:
            case STAFF_WIZARDRY:
            case STAFF_ENCHANTMENT:
            case STAFF_SUMMONING:
            case STAFF_DESTRUCTION_I:
            case STAFF_DESTRUCTION_II:
            case STAFF_DESTRUCTION_III:
            case STAFF_DESTRUCTION_IV:
                valued += 750;
                break;

            case STAFF_SMITING:
            case STAFF_WARDING:
            case STAFF_DISCOVERY:
            case STAFF_SPELL_SUMMONING:
            case STAFF_DEMONOLOGY:
            default:
                valued += 500;
                break;
            }

            if (item_is_rod( item ) && item_ident( item, ISFLAG_KNOW_PLUSES ))
                valued += 50 * (item.plus2 / ROD_CHARGE_MULT);
        }
        break;

    case OBJ_ORBS:
        valued = 100000;
        break;
    }                           // end switch

    // apply quantity and return back to regular gold scale (if needed)
    valued = (valued * item.quantity) / div;

    if (valued < min)
        valued = min;

    return (valued);
}                               // end item_value()

void shop(void)
{
    unsigned char i = 0;

    for (i = 0; i < MAX_SHOPS; i++)
    {
        if (env.shop[i].x == you.x_pos && env.shop[i].y == you.y_pos)
            break;
    }

    if (i == MAX_SHOPS)
    {
        mpr("Help! Non-existent shop.");
        return;
    }

    char identy[NUM_IDTYPE][MAX_SUBTYPES];

    save_id(identy);

    in_a_shop( i, identy );
    set_redraw_status( REDRAW_GOLD );
    burden_change();

    redraw_screen();
}                               // end shop()

const char *shop_name( int sx, int sy )
{
    static char sh_name[80];
    int shoppy;

    // paranoia
    if (grd[sx][sy] != DNGN_ENTER_SHOP)
        return ("");

    // find shop
    for (shoppy = 0; shoppy < MAX_SHOPS; shoppy ++)
    {
        // find shop index plus a little bit of paranoia
        if (env.shop[shoppy].x == sx && env.shop[shoppy].y == sy &&
            env.shop[shoppy].type != SHOP_UNASSIGNED)
        {
            break;
        }
    }

    if (shoppy == MAX_SHOPS)
    {
        mpr("Help! Non-existent shop.");
        return ("Buggy Shop");
    }

    const shop_struct &shop = env.shop[shoppy];
    const int name_len = strlen( shop.keeper_name );

    snprintf( sh_name, sizeof( sh_name ), "%s'%s ",
              shop.keeper_name,
              shop.keeper_name[ name_len - 1 ] == 's' ? "" : "s" );

    if (shop.type == SHOP_WEAPON_ANTIQUE || shop.type == SHOP_ARMOUR_ANTIQUE)
        strcat( sh_name, "Antique " );

    strcat(sh_name, (shop.type == SHOP_WEAPON
                     || shop.type == SHOP_WEAPON_ANTIQUE) ? "Weapon" :
                    (shop.type == SHOP_ARMOUR
                     || shop.type == SHOP_ARMOUR_ANTIQUE) ? "Armour" :

                    (shop.type == SHOP_JEWELLERY)         ? "Jewellery" :
                    (shop.type == SHOP_WAND)              ? "Magical Wand" :
                    (shop.type == SHOP_BOOK)              ? "Book" :
                    (shop.type == SHOP_FOOD)              ? "Food" :
                    (shop.type == SHOP_SCROLL)            ? "Magic Scroll" :
                    (shop.type == SHOP_GENERAL_ANTIQUE) ? "Assorted Antiques" :
                    (shop.type == SHOP_DISTILLERY)        ? "Distillery" :
                    (shop.type == SHOP_GENERAL)           ? "General Store"
                                                          : "Bug");


    if (shop.type != SHOP_GENERAL
        && shop.type != SHOP_GENERAL_ANTIQUE && shop.type != SHOP_DISTILLERY)
    {
        int temp = sx + sy % 4;
        strcat( sh_name, (temp == 0) ? " Shoppe" :
                         (temp == 1) ? " Boutique" :
                         (temp == 2) ? " Emporium"
                                     : " Shop" );
    }

    return (sh_name);
}

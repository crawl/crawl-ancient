/*
 *  File:       monstuff.cc
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <8>      7 Aug  2001   MV      Inteligent monsters now pick up gold
 *      <7>     26 Mar  2001   GDL     Fixed monster reaching
 *      <6>     13 Mar  2001   GDL     Rewrite of monster AI
 *      <5>     31 July 2000   GDL     More Manticore fixes.
 *      <4>     29 July 2000   JDJ     Fixed a bunch of places in handle_pickup where MSLOT_WEAPON
 *                                     was being erroneously used.
 *      <3>     25 July 2000   GDL     Fixed Manticores
 *      <2>     11/23/99       LRH     Upgraded monster AI
 *      <1>     -/--/--        LRH     Created
 */

#include "AppHdr.h"
#include "monstuff.h"

#include <queue>
#include <vector>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef DOS
#include <conio.h>
#endif

#include "globals.h"
#include "externs.h"

#include "beam.h"
#include "cloud.h"
#include "debug.h"
#include "dungeon.h"
#include "fight.h"
#include "itemname.h"
#include "itemprop.h"
#include "items.h"
#include "misc.h"
#include "monplace.h"
#include "monspeak.h"
#include "mon-util.h"
#include "mstuff2.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "spl-cast.h"
#include "spells1.h"
#include "spells2.h"
#include "spells4.h"
#include "stuff.h"
#include "view.h"

static bool mons_likes_weapon( struct monsters *mon, item_def &item );
static bool mons_likes_armour( struct monsters *mon, item_def &item );
static bool mons_likes_ammo( struct monsters *mon, item_def &item );
static bool mons_likes_scroll( struct monsters *mon, item_def &item );
static bool mons_likes_potion( struct monsters *mon, item_def &item );
static bool mons_likes_wand( struct monsters *mon, item_def &item );
static bool mons_likes_carrion( struct monsters *mon );
static bool mons_likes_gold( struct monsters *mon );

static bool handle_monster_move( struct monsters *monster );
static bool handle_special_ability( struct monsters *monster, bolt & beem );
static bool handle_pickup( struct monsters *monster );
static void handle_behaviour( struct monsters *monster );
static bool plant_spit( struct monsters *monster, struct bolt &pbolt );
static int map_wand_to_mspell( int wand_type );

char mmov_x, mmov_y;

static int compass_x[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
static int compass_y[8] = { -1, -1, -1, 0, 1, 1, 1, 0 };

#define FAR_AWAY    1000000         // used in monster_move()

// This function creates an arteficial item to represent a mimic's appearance.
// Eventually, mimics could be redone to be more like Dancing wepaons...
// there'd only be one type and it would look like the item it carries. -- bwr
void get_mimic_item( const struct monsters *mimic, item_def &item )
{
    ASSERT( mimic != NULL && mons_is_mimic( mimic->type ) );

    init_item( item );

    item.quantity = 1;
    item.x = mimic->x;
    item.y = mimic->y;

    int prop = 127 * mimic->x + 269 * mimic->y;

    switch (mimic->type)
    {
    case MONS_WEAPON_MIMIC:
        item.base_type = OBJ_WEAPONS;
        item.sub_type = (59 * mimic->x + 79 * mimic->y) % NUM_WEAPONS;

        prop %= 100;

        if (prop < 20)
        {
            item.flags |= ISFLAG_RANDART;
            item.special = ((mimic->x << 8 + mimic->y) & RANDART_SEED_MASK);
        }
        else if (prop < 50)
            set_equip_desc( item, ISFLAG_GLOWING );
        else if (prop < 80)
            set_equip_desc( item, ISFLAG_RUNED );
        else if (prop < 85)
            set_equip_race( item, ISFLAG_ORCISH );
        else if (prop < 90)
            set_equip_race( item, ISFLAG_DWARVEN );
        else if (prop < 95)
            set_equip_race( item, ISFLAG_ELVEN );
        break;

    case MONS_ARMOUR_MIMIC:
        item.base_type = OBJ_ARMOUR;
        item.sub_type = (59 * mimic->x + 79 * mimic->y) % NUM_ARMOURS;

        prop %= 100;

        if (prop < 20)
        {
            item.flags |= ISFLAG_RANDART;
            item.special = ((mimic->x << 8 + mimic->y) & RANDART_SEED_MASK);
        }
        else if (prop < 40)
            set_equip_desc( item, ISFLAG_GLOWING );
        else if (prop < 60)
            set_equip_desc( item, ISFLAG_RUNED );
        else if (prop < 80)
            set_equip_desc( item, ISFLAG_SHINY );
        else if (prop < 85)
            set_equip_race( item, ISFLAG_ORCISH );
        else if (prop < 90)
            set_equip_race( item, ISFLAG_DWARVEN );
        else if (prop < 95)
            set_equip_race( item, ISFLAG_ELVEN );
        break;

    case MONS_SCROLL_MIMIC:
        item.base_type = OBJ_SCROLLS;
        item.sub_type = prop % NUM_SCROLLS;
        break;

    case MONS_POTION_MIMIC:
        item.base_type = OBJ_POTIONS;
        item.sub_type = prop % NUM_POTIONS;
        break;

    case MONS_GOLD_MIMIC:
    default:
        item.base_type = OBJ_GOLD;
        item.quantity = 5 + prop % 30;
        break;
    }

    item_colour( item ); // also sets special vals for scrolls/poitions
}

// Sets the colour of a mimic to match its description... should be called
// whenever a mimic is created or teleported. -- bwr
int get_mimic_colour( struct monsters *mimic )
{
    ASSERT( mimic != NULL && mons_is_mimic( mimic->type ) );

    // first check for predefined colours
    const int col = mons_class_colour( mimic->type );
    if (col != BLACK)
        return (col);

    item_def  item;
    get_mimic_item( mimic, item );

    return (item.colour);
}

// Monster curses a random player inventory item:
bool curse_random_item( bool special_curses )
{
    int count = 0;
    int item  = ENDOFPACK;

    for (int i = 0; i < ENDOFPACK; i++)
    {
        if (!is_valid_item( you.inv[i] ))
            continue;

        if (you.inv[i].base_type == OBJ_WEAPONS
            || you.inv[i].base_type == OBJ_STAVES
            || you.inv[i].base_type == OBJ_ARMOUR
            || you.inv[i].base_type == OBJ_JEWELLERY
            || you.inv[i].base_type == OBJ_POTIONS
            || you.inv[i].base_type == OBJ_SCROLLS
            || you.inv[i].base_type == OBJ_WANDS)
        {
            if (item_cursed( you.inv[i] ))
                continue;

            if (special_curses)
            {
                if (you.inv[i].base_type == OBJ_POTIONS
                    && (you.inv[i].sub_type == POT_DECAY
                        || you.inv[i].sub_type == POT_WATER))
                {
                    continue;
                }

                if (you.inv[i].base_type == OBJ_SCROLLS
                    && (you.inv[i].sub_type == SCR_PAPER
                        || you.inv[i].sub_type == SCR_RANDOM_USELESSNESS
                        || you.inv[i].sub_type == SCR_REMOVE_CURSE))
                {
                    continue;
                }

                if (you.inv[i].base_type == OBJ_WANDS
                    && you.inv[i].plus <= random2(6))
                {
                    continue;
                }
            }

            // item is valid for cursing, so we'll give it a chance
            count++;
            if (one_chance_in( count ))
                item = i;
        }
    }

    // any item to curse?
    if (item == ENDOFPACK)
        return (false);

    // curse item:

    /* problem: changes large piles of potions */
    /* don't change you.inv_special (just for fun) */
    switch (you.inv[item].base_type)
    {
    case OBJ_POTIONS:
        you.inv[item].sub_type = coinflip() ? POT_WATER
                                            : POT_DECAY;

        unset_ident_flags( you.inv[item], ISFLAG_IDENT_MASK ); // all different
        break;

    case OBJ_SCROLLS:
        you.inv[item].sub_type = coinflip() ? SCR_PAPER
                                            : SCR_RANDOM_USELESSNESS;

        unset_ident_flags( you.inv[item], ISFLAG_IDENT_MASK ); // all different
        break;

    case OBJ_WANDS:
        // Note: this can cause wand destruction if it was just used
        you.inv[item].plus = 0;
        break;

    default:
        do_curse_item( you.inv[item] );
        break;
    }

    if (item == you.equip[EQ_WEAPON])
        set_redraw_status( REDRAW_WIELD );

    return (true);
}

#if SCATTER_RADIUS
static bool nearby_empty_item_space( int &x, int &y )
{
    int nx = 0;
    int ny = 0;
    int count = 0;
    int small_dist = 3;
    int small_size = 3;

    for (int i = -SCATTER_RADIUS; i <= SCATTER_RADIUS; i++)
    {
        for (int j = -SCATTER_RADIUS; j <= SCATTER_RADIUS; j++)
        {
            const int tx = x + i;
            const int ty = y + j;

            if (!in_bounds( tx, ty )
                || grid_is_solid( grd[tx][ty] )
                || grid_destroys_items( grd[tx][ty] )
                || !check_line_of_sight( x, y, tx, ty ))
            {
                continue;
            }

            // Stack size is currently simple: zero, one, many... this should
            // be good enough since things don't go lopsided until >50 items
            // have been dropped (more if not all from the extact same spot),
            // and that's a lot of items.
            const int size = ((igrd[tx][ty] == NON_ITEM)            ? 0 :
                              (mitm[igrd[tx][ty]].link == NON_ITEM) ? 1
                                                                    : 2);
            const int dist = grid_distance( x, y, tx, ty );

            if (size < small_size
                || (size == small_size && dist < small_dist))
            {
                // if we have a smaller stack or a closer stack of the
                // current size, we reset our priorities and count
                count = 1;
                small_size = size;
                small_dist = dist;
                nx = tx;
                ny = ty;
            }
            else if (size == small_size && dist == small_dist)
            {
                // pick a random one of of the best size and distance
                count++;
                if (one_chance_in(count))
                {
                    nx = tx;
                    ny = ty;
                }
            }
        }
    }

    if (count)
    {
        x = nx;
        y = ny;
    }

    return (count > 0);
}
#endif

static int monster_drop_ething( struct monsters *monster )
{
    /* drop weapons & missiles last (ie on top) so others pick up */
    bool destroyed = false;
    bool hostile_grid = grid_destroys_items( grd[monster->x][monster->y] );
    int  ret = 0;

    for (int i = MSLOT_GOLD; i >= MSLOT_WEAPON; i--)
    {
        int item = monster->inv[i];

        if (item != NON_ITEM)
        {
            if (hostile_grid)
            {
                destroyed = true;
                destroy_item( item );
            }
            else
            {
#if SCATTER_RADIUS
                // try to scatter the monster's items
                int x = monster->x;
                int y = monster->y;

                nearby_empty_item_space( x, y );
                move_item_to_grid( &item, x, y );
#else
                // drop everything on the monster's square
                move_item_to_grid( &item, monster->x, monster->y );
#endif
                ret++;
            }

            monster->inv[i] = NON_ITEM;
        }
    }

    if (destroyed)
    {
        if (grd[monster->x][monster->y] == DNGN_LAVA)
            mpr("You hear a hissing sound.");
        else
            mpr("You hear a splashing sound.");
    }

    return (ret);
}                               // end monster_drop_ething()

static void place_monster_corpse( struct monsters *monster )
{
    monster_type corpse_class = mons_species( monster->type );

    if (corpse_class == MONS_DRACONIAN)
    {
        corpse_class = draco_subspecies( monster );
        monster->colour = mons_class_colour( corpse_class );
    }

    // Catch shapeshifters, but don't drop corpses if they're
    // a non-natural shapeshifter.
    if (mons_holiness( monster ) == MH_NATURAL)
    {
        if (mons_flag( monster, MF_SHAPESHIFT ))
        {
            corpse_class = MONS_SHAPESHIFTER;
            monster->colour = mons_class_colour( MONS_SHAPESHIFTER );
        }
        else if (mons_flag( monster, MF_GLOW_SHAPESHIFT ))
        {
            corpse_class = MONS_GLOWING_SHAPESHIFTER;
            monster->colour = mons_class_colour( MONS_GLOWING_SHAPESHIFTER );
        }
    }

    if (mons_weight( corpse_class ) == 0
        || grid_destroys_items( grd[monster->x][monster->y] )
        || coinflip())
    {
        return;
    }

    int o = get_item_slot();
    if (o == NON_ITEM)
        return;

    mitm[o].flags = 0;
    mitm[o].base_type = OBJ_CORPSES;
    mitm[o].plus = corpse_class;
    mitm[o].plus2 = 0;                          // butcher work done
    mitm[o].sub_type = CORPSE_BODY;
    mitm[o].special = 210;                      // rot time
    mitm[o].quantity = 1;

    if (Options.colourful_corpses)
        mitm[o].colour = monster->colour;           // colour at time of death
    else
        mitm[o].colour = mons_class_colour( corpse_class );

    // Don't care if 'o' is changed, and it shouldn't be (corpses don't stack)
    move_item_to_grid( &o, monster->x, monster->y );
}                               // end place_monster_corpse()

void monster_die( struct monsters *monster, killer_type killer, int src )
{
    int dmi;                    // dead monster's inventory
    int monster_killed = monster_index(monster);
    bool death_message = (mons_near(monster) && player_monster_visible(monster));

    // From time to time Trog gives you a little bonus
    if (killer == KILL_YOU && you.berserker)
    {
        if (you.religion == GOD_TROG
            && (!player_under_penance() && you.piety > random2(1000)))
        {
            inc_berserk( 2 + roll_dice(2,5) );

            mpr( MSGCH_GOD, GOD_TROG, "You feel the power of Trog in you as your rage grows." );
        }
        else if (player_equip( EQ_AMULET, AMU_RAGE ) && one_chance_in(30))
        {
            inc_berserk( 2 + random2(4) );

            mpr( "Your amulet glows a violent red." );
        }
    }

    if (you.prev_targ == monster_killed)
        you.prev_targ = MHITNOT;

    const bool pet_kill = (MON_KILL(killer) && ((src >= 0 && src < 200)
                            && mons_friendly(&menv[src])));

    if (monster->type == MONS_GIANT_SPORE
        || monster->type == MONS_BALL_LIGHTNING)
    {
        if (monster->hit_points < 1 && monster->hit_points > -15)
            return;
    }
    else if (monster->type == MONS_FIRE_VORTEX
             || monster->type == MONS_SPATIAL_VORTEX)
    {
        mon_msg( monster, " dissipates!", true, MSGCH_MONSTER_DAMAGE, MDAM_DEAD );

        if (!testbits(monster->flags, MF_CREATED_FRIENDLY))
        {
            if (YOU_KILL(killer))
                gain_exp( exper_value( monster ) );
            else if (pet_kill)
                gain_exp( exper_value( monster ) / 2 + 1 );
        }

        if (monster->type == MONS_FIRE_VORTEX)
            place_cloud(CLOUD_FIRE_MON, monster->x, monster->y, 2 + random2(4));
    }
    else if (monster->type == MONS_SIMULACRUM_SMALL
             || monster->type == MONS_SIMULACRUM_LARGE)
    {
        if (killer != KILL_RESET || src != MHITNOT)
        {
            mon_msg( monster, " vaporizes!", true, MSGCH_MONSTER_DAMAGE, MDAM_DEAD );

            if (!testbits( monster->flags, MF_CREATED_FRIENDLY ))
            {
                if (YOU_KILL(killer))
                    gain_exp( exper_value( monster ) );
                else if (pet_kill)
                    gain_exp( exper_value( monster ) / 2 + 1 );
            }

            place_cloud(CLOUD_COLD_MON, monster->x, monster->y, 2 + random2(4));
        }
    }
    else if (monster->type == MONS_DANCING_WEAPON)
    {
        mon_msg( monster, " falls from the air.", true, MSGCH_MONSTER_DAMAGE, MDAM_DEAD );

        if (!testbits(monster->flags, MF_CREATED_FRIENDLY))
        {
            if (YOU_KILL(killer))
                gain_exp( exper_value( monster ) );
            else if (pet_kill)
                gain_exp( exper_value( monster ) / 2 + 1 );
        }
    }
    else
    {
        const int targ_holy = mons_holiness( monster );

        switch (killer)
        {
        case KILL_YOU:          /* You kill in combat. */
        case KILL_YOU_MISSILE:  /* You kill by missile or beam. */
            strcpy(info, "You ");
            strcat(info, (wounded_damaged(monster->type)) ? "destroy" : "kill");
            strcat(info, " ");
            strcat(info, ptr_monam(monster, DESC_NOCAP_THE));
            strcat(info, "!");

            if (death_message)
                mpr( MSGCH_MONSTER_DAMAGE, MDAM_DEAD, info );

            if (!testbits( monster->flags, MF_CREATED_FRIENDLY ))
            {
                gain_exp( exper_value( monster ) );
            }
            else
            {
                if (death_message)
                    mpr("That felt strangely unrewarding.");
            }

            // Xom doesn't care who you killed:
            if (you.religion == GOD_XOM
                    && random2(70) <= 10 + monster->hit_dice)
            {
                Xom_acts( true, 1 + random2(monster->hit_dice), false );
            }

            // Trying to prevent summoning abuse here, so we're trying to
            // prevent summoned creatures from being being done_good kills,
            // Only affects monsters friendly when created.
            if (!testbits( monster->flags, MF_CREATED_FRIENDLY ))
            {
                bool notice = false;

                if (you.duration[DUR_PRAYER])
                {
                    if (targ_holy == MH_NATURAL)
                    {
                        notice |= did_god_conduct( DID_DEDICATED_KILL_LIVING,
                                                   monster->hit_dice );

                        if (mons_class_flag( monster->type, M_EVIL ))
                        {
                            notice |= did_god_conduct( DID_DEDICATED_KILL_NATURAL_EVIL,
                                                       monster->hit_dice );
                        }
                    }
                    else if (targ_holy == MH_UNDEAD)
                    {
                        notice |= did_god_conduct( DID_DEDICATED_KILL_UNDEAD,
                                                   monster->hit_dice );
                    }
                    else if (targ_holy == MH_DEMONIC)
                    {
                        notice |= did_god_conduct( DID_DEDICATED_KILL_DEMON,
                                                   monster->hit_dice );
                    }

                    // Note that the following two might be in addition
                    // to the above (or in exception to, thus "|=" ).

                    //jmf: Trog hates wizards
                    if (mons_class_flag( monster->type, M_ACTUAL_SPELLS ))
                    {
                        notice |= did_god_conduct( DID_DEDICATED_KILL_WIZARD,
                                                   monster->hit_dice );
                    }

                    //jmf: maybe someone hates priests?
                    if (mons_class_flag( monster->type, M_PRIEST ))
                    {
                        notice |= did_god_conduct( DID_DEDICATED_KILL_PRIEST,
                                                   monster->hit_dice );
                    }
                }

                // Angel deaths are always noticed
                if (targ_holy == MH_HOLY)
                {
                    notice |= did_god_conduct( DID_KILL_ANGEL, monster->hit_dice );
                }

                // Note: assumption that Makhleb noticing is always good.
                if ((you.mutation[MUT_DEATH_STRENGTH]
                        && targ_holy == MH_NATURAL
                        && one_chance_in(9 - 2 * you.mutation[MUT_DEATH_STRENGTH]))
                    || (you.religion == GOD_MAKHLEB
                        && you.duration[DUR_PRAYER]
                        && notice
                        && (!player_under_penance() && random2(you.piety) >= 30)))
                {
                    if (you.hp < you.hp_max)
                    {
                        mpr( "You feel a little better." );
                        inc_hp( 1 + roll_zdice(2, monster->hit_dice / 2), false );
                    }
                }

                // Note: assumption that Vehumet/Makhleb noticing is always good
                if ((you.religion == GOD_MAKHLEB || you.religion == GOD_VEHUMET)
                    && you.duration[DUR_PRAYER]
                    && notice
                    && (!player_under_penance() && random2(you.piety) >= 30))
                {
                    if (you.magic_points < you.max_magic_points)
                    {
                        mpr("You feel your power returning.");
                        inc_mp( 1 + random2(monster->hit_dice / 2), false );
                    }
                }
            }
            break;

        case KILL_MON:          /* Monster kills in combat */
        case KILL_MON_MISSILE:  /* Monster kills by missile or beam */
            mon_msg( monster, " dies!", true, MSGCH_MONSTER_DAMAGE, MDAM_DEAD );

            // no piety loss if god gifts killed by other monsters
            if (mons_friendly(monster) && !testbits(monster->flags,MF_GOD_GIFT))
                did_god_conduct( DID_FRIEND_DIES, 1 + (monster->hit_dice / 2) );

            // Trying to prevent summoning abuse here, so we're trying to
            // prevent summoned creatures from being being done_good kills.
            // Only affects creatures which were friendly when summoned.
            if (!testbits( monster->flags, MF_CREATED_FRIENDLY ) && pet_kill)
            {
                bool notice = false;

                gain_exp( exper_value(monster) / 2 + 1 );

                if (mons_holiness( &menv[src] ) == MH_UNDEAD)
                {
                    // Only Kiku and Yred watch undead slaves, by not bothering
                    // to check for the god gift flag we save on the trouble
                    // of getting the enslave/control and any animate spells
                    // cast under Kiku to work correctly.  Note that reapers
                    // are marked god gifts and are handled below. -- bwr
                    if (targ_holy == MH_NATURAL)
                    {
                        notice |= did_god_conduct( DID_LIVING_KILLED_BY_UNDEAD_SLAVE,
                                                   monster->hit_dice );
                    }
                }
                else if (you.religion == GOD_VEHUMET
                        || testbits( menv[src].flags, MF_GOD_GIFT ))
                {
                    // Yes, we are splitting undead pets from the others
                    // as a way to focus Necomancy vs Summoning (ignoring
                    // Summon Wraith here)... at least we're being nice and
                    // putting the natural creature Summons together with
                    // the Demon ones.  Note that Vehumet gets a free
                    // pass here since those followers are assumed to
                    // come from Summoning spells...  the others are
                    // from invocations (Zin, TSO, Makh, Kiku). -- bwr

                    if (targ_holy == MH_NATURAL)
                    {
                        notice |= did_god_conduct( DID_LIVING_KILLED_BY_SERVANT,
                                                       monster->hit_dice );

                        if (mons_class_flag( monster->type, M_EVIL ))
                        {
                            notice |= did_god_conduct( DID_NATURAL_EVIL_KILLED_BY_SERVANT,
                                                           monster->hit_dice );
                        }
                    }
                    else if (targ_holy == MH_DEMONIC)
                    {
                        notice |= did_god_conduct( DID_DEMON_KILLED_BY_SERVANT,
                                                   monster->hit_dice );
                    }
                    else if (targ_holy == MH_UNDEAD)
                    {
                        notice |= did_god_conduct( DID_UNDEAD_KILLED_BY_SERVANT,
                                                   monster->hit_dice );
                    }
                }

                // Angel kills are always noticed.
                if (targ_holy == MH_HOLY)
                {
                    notice |= did_god_conduct( DID_ANGEL_KILLED_BY_SERVANT,
                                               monster->hit_dice );
                }

                // Note: assumption that Vehumet noticing is always good.
                if (you.religion == GOD_VEHUMET
                    && notice
                    && (!player_under_penance() && random2(you.piety) >= 30))
                {
                    /* Vehumet - only for non-undead servants (coding
                       convenience, no real reason except that Vehumet
                       prefers demons) */
                    if (you.magic_points < you.max_magic_points)
                    {
                        mpr("You feel your power returning.");
                        inc_mp( 1 + random2(monster->hit_dice / 2), false );
                    }
                }
            }
            break;

        /* Monster killed by trap/inanimate thing/itself/poison not from you */
        case KILL_MISC:
            mon_msg( monster, " dies!", true, MSGCH_MONSTER_DAMAGE, MDAM_DEAD );
            break;

        case KILL_RESET:
        /* Monster doesn't die, just goes back to wherever it came from
           This must only be called by monsters running out of time (or
           abjuration), because it uses the beam variables! Or does it??? */
            if (src != MHITNOT)
            {
                mon_msg( monster, " disappears in a puff of smoke!" );

                place_cloud( CLOUD_GREY_SMOKE_MON + random2(3), monster->x,
                             monster->y, 1 + random2(3) );
            }

            for (dmi = MSLOT_GOLD; dmi >= MSLOT_WEAPON; dmi--)
            {                   /* takes whatever it's carrying back home */
                if (monster->inv[dmi] != NON_ITEM)
                    destroy_item( monster->inv[dmi] );

                monster->inv[dmi] = NON_ITEM;
            }
            break;
        }

        // experimental: moved this down here, added no reset,
        // created friendly, and mons_near checks.  -- bwr
        if (you.duration[DUR_DEATH_CHANNEL]
            && killer != KILL_RESET                        // not unsummoned
            && targ_holy == MH_NATURAL                     // natural creature
            && !testbits( monster->flags, MF_CREATED_FRIENDLY ) // not friend
            && !mons_has_ench( monster, ENCH_SUMMONED )         // not summoned
            && mons_near( monster )                        // nearby
            && mons_weight( mons_species(monster->type) )) // corporeal
        {
            if (create_monster( MONS_SPECTRAL_THING, BEH_FRIENDLY, 0,
                                monster->x, monster->y, you.pet_target,
                                mons_species(monster->type)) != -1)
            {
                if (death_message)
                    mpr( "A glowing mist starts to gather..." );
            }
        }
    }

    if (monster->type == MONS_MUMMY)
    {
        if (YOU_KILL(killer) || (pet_kill && coinflip()))
        {
            if (you.religion == GOD_ZIN && random2(100) < 20 + you.piety / 2)
                simple_god_message( " protects you from the mummy's curse!" );
            else if (curse_random_item( true ))
                mpr(MSGCH_MONSTER_SPELL,"You feel nervous for a moment..." );
        }
    }
    else if (monster->type == MONS_GUARDIAN_MUMMY
             || monster->type == MONS_GREATER_MUMMY
             || monster->type == MONS_MUMMY_PRIEST)
    {
        if (YOU_KILL(killer) || (pet_kill && coinflip()))
        {
            if (you.religion == GOD_ZIN && random2(100) < 10 + you.piety / 3)
                simple_god_message( " protects you from the mummy's curse!" );
            else
            {
                mpr(MSGCH_MONSTER_SPELL,"You feel extremely nervous for a moment..." );

                char buff[ INFO_SIZE ];

                snprintf( buff, sizeof(buff), "%s's death curse",
                          monam( monster->number, monster->type, true,
                                 DESC_NOCAP_A ));

                miscast_effect( SPTYP_NECROMANCY,
                                3 + (monster->type == MONS_GREATER_MUMMY) * 8
                                  + (monster->type == MONS_MUMMY_PRIEST) * 5,
                                roll_dice(3,30), 100, buff );
            }
        }
    }
    else if (monster->type == MONS_BORIS)
    {
        // XXX: actual blood curse effect for Boris? -- bwr

        // Provide the player with an ingame clue to Boris' return.  -- bwr
        const int tmp = random2(7);

        switch (tmp)
        {
        case 0:
            // might as well do the entire Inspector Gadget line...
            snprintf( info, INFO_SIZE,
                      " says, \"I'll get you next time, %s... next time!\"",
                      you.your_name );

            mon_msg( monster, info, true, MSGCH_TALK );
            break;

        default:
            mon_msg( monster,
                (tmp == 1) ? " says, \"You haven't seen the last of me!\"" :
                (tmp == 2) ? " says, \"This isn't over yet!\"" :
                (tmp == 3) ? " says, \"I'll be back!\"" :
                (tmp == 4) ? " says, \"This isn't the end, it's only just beginning!\"" :
                (tmp == 5) ? " says, \"Kill me?  I think not!\""
                           : " says, \"You cannot defeat me so easily!\"",
                               true,
                               MSGCH_TALK );
            break;
        }

        // Now that Boris is dead, he's a valid target for monster
        // creation again. -- bwr
        you.unique_creatures[ monster->type - 280 ] = 0;
    }

    if (killer != KILL_RESET)
    {
        you.kills.record_kill(monster);

        if (mons_has_ench( monster, ENCH_SUMMONED ))
        {
            if (mons_weight( mons_species(monster->type) ))
            {
                if (monster->type == MONS_SIMULACRUM_SMALL
                    || monster->type == MONS_SIMULACRUM_LARGE)
                {
                    mon_msg( monster, " vaporizes!" );

                    place_cloud( CLOUD_COLD_MON, monster->x, monster->y,
                                 1 + random2(3) );
                }
                else
                {
                    mon_msg(monster, "'s corpse disappears in a puff of smoke!");

                    place_cloud( CLOUD_GREY_SMOKE_MON + random2(3),
                                 monster->x, monster->y, 1 + random2(3) );
                }
            }
        }
        else if (!mons_has_ench( monster, ENCH_PETRIFY )) // stone isn't tasty
        {
            place_monster_corpse(monster);
        }
    }

    monster_drop_ething(monster);
    monster_cleanup(monster);
}                                                   // end monster_die

void monster_cleanup( struct monsters *monster )
{
    int monster_killed = monster_index( monster );
    int dmi = 0;

    clear_monster_enchants( monster_killed );

    monster->flags = 0;
    monster->type = -1;
    monster->hit_points = 0;
    monster->max_hit_points = 0;
    monster->hit_dice = 0;
    monster->armour_class = 0;
    monster->evasion = 0;
    monster->energy = 0;
    monster->attitude = ATT_HOSTILE;
    monster->behaviour = BEH_SLEEP;
    monster->foe = MHITNOT;

    mgrd[monster->x][monster->y] = NON_MONSTER;

    for (dmi = 0; dmi < NUM_MONSTER_SLOTS; dmi++)
        monster->inv[dmi] = NON_ITEM;

    for (dmi = 0; dmi < NUM_MONSTER_SPELL_SLOTS; dmi++)
        monster->spells[dmi] = MS_NO_SPELL;

    for (dmi = 0; dmi < MAX_MONSTERS; dmi++)
    {
        if (menv[dmi].foe == monster_killed)
            menv[dmi].foe = MHITNOT;
    }

    if (you.pet_target == monster_killed)
        you.pet_target = MHITNOT;

}                               // end monster_cleanup()

static bool jelly_divide( struct monsters *parent )
{
    int jex = 0, jey = 0;       // loop variables {dlb}
    bool foundSpot = false;     // to rid code of hideous goto {dlb}
    struct monsters *child = 0; // NULL - value determined with loop {dlb}

    if (!mons_class_flag( parent->type, M_SPLITS ) || parent->hit_points == 1)
        return (false);

    // first, find a suitable spot for the child {dlb}:
    for (jex = -1; jex < 3; jex++)
    {
        // loop moves beyond those tiles contiguous to parent {dlb}:
        if (jex > 1)
            return (false);

        for (jey = -1; jey < 2; jey++)
        {
            // 10-50 for now - must take clouds into account:
            if (mgrd[parent->x + jex][parent->y + jey] == NON_MONSTER
                && !grid_is_solid( grd[parent->x + jex][parent->y + jey] )
                && (parent->x + jex != you.x_pos
                    || parent->y + jey != you.y_pos))
            {
                foundSpot = true;
                break;
            }
        }

        if (foundSpot)
            break;
    }                           /* end of for jex */

    int k = 0;                  // must remain outside loop that follows {dlb}

    // now that we have a spot, find a monster slot {dlb}:
    for (k = 0; k < MAX_MONSTERS; k++)
    {
        child = &menv[k];

        if (child->type == -1)
            break;
        else if (k == MAX_MONSTERS - 1)
            return (false);
    }

    // handle impact of split on parent {dlb}:
    parent->max_hit_points /= 2;

    if (parent->hit_points > parent->max_hit_points)
        parent->hit_points = parent->max_hit_points;

    // create child {dlb}:
    // this is terribly partial and really requires
    // more thought as to generation ... {dlb}
    child->type = parent->type;
    child->hit_dice = parent->hit_dice;
    child->hit_points = parent->hit_points;
    child->max_hit_points = child->hit_points;
    child->armour_class = parent->armour_class;
    child->evasion = parent->evasion;
    child->speed = parent->speed;
    child->energy = 70 + random2(10);
    child->behaviour = parent->behaviour; /* Look at this! */
    child->foe = parent->foe;
    child->attitude = parent->attitude;

    child->x = parent->x + jex;
    child->y = parent->y + jey;

    mgrd[child->x][child->y] = k;

    if (!mon_msg( parent, " splits in two!" )
        && player_can_hear( parent->x, parent->y ))
    {
        mpr( MSGCH_SOUND,"You hear a squelching noise."  );
    }

    return (true);
}                               // end jelly_divde()

// if you're invis and throw/zap whatever, alerts menv to your position
void alert_nearby_monsters(void)
{
    struct monsters *monster = 0;       // NULL {dlb}

    for (int it = 0; it < MAX_MONSTERS; it++)
    {
        monster = &menv[it];

        // Judging from the above comment, this function isn't
        // intended to wake up monsters, so we're only going to
        // alert monsters that aren't sleeping.  For cases where an
        // event should wake up monsters and alert them, I'd suggest
        // calling noisy() before calling this function. -- bwr
        if (monster->type != -1
            && monster->behaviour != BEH_SLEEP
            && mons_near(monster))
        {
            behaviour_event( monster, ME_ALERT, MHITYOU );
        }
    }
}                               // end alert_nearby_monsters()

static bool valid_morph( struct monsters *monster, int new_mclass )
{
    unsigned char current_tile = grd[monster->x][monster->y];

    // morph targets are _always_ "base" classes,  not derived ones.
    new_mclass = mons_species(new_mclass);

    /* various inappropriate polymorph targets */
    if (mons_class_holiness( new_mclass ) != mons_class_holiness( monster->type )
        || mons_class_flag( new_mclass, M_NO_EXP_GAIN )        // not helpless
        || new_mclass == mons_species( monster->type ) // must be different
        || new_mclass == MONS_PROGRAM_BUG
        || mons_is_unique( new_mclass ))
    {
        return (false);
    }

    /* Not fair to instakill a monster like this --
     order of evaluation of inner conditional important */
    if (grid_destroys_items( current_tile ))
    {
        if (mons_class_flies(new_mclass) != FLY_NOT
            || monster_habitat(new_mclass) != current_tile)
        {
            return (false);
        }
    }

    // not fair to strand a water monster on dry land, either.  :)
    if (monster_habitat(new_mclass) == DNGN_DEEP_WATER
        && current_tile != DNGN_DEEP_WATER
        && current_tile != DNGN_SHALLOW_WATER)
    {
        return (false);
    }

    // and putting lava monsters on non-lava sqaures is a no-no, too
    if (monster_habitat(new_mclass) == DNGN_LAVA && current_tile != DNGN_LAVA)
        return (false);

    return (true);
}        // end valid_morph()

// note that power is (as of yet) unused within this function -
// may be worthy of consideration of later implementation, though,
// so I'll still let the parameter exist for the time being {dlb}
bool monster_polymorph( struct monsters *monster, int targetc, int power )
{
    char str_polymon[INFO_SIZE] = "";      // cannot use info[] here {dlb}
    bool player_messaged = false;
    int source_power, target_power, relax;

    UNUSED( power );

    // Used to be mons_power, but that just returns hit_dice
    // for the monster class.  By using the current hit dice
    // the player gets the opportunity to use draining more
    // effectively against shapeshifters. -- bwr
    source_power = monster->hit_dice;
    relax = 2;

    if (targetc == RANDOM_MONSTER)
    {
        do
        {
            targetc = random2( NUM_MONSTERS );

            // valid targets are always base classes
            targetc = mons_species( targetc );

            target_power = mons_power( targetc );

            if (one_chance_in(100))
                relax++;

            if (relax > 10)
                return (mon_msg( monster, " shudders." ));
        }
        while (!valid_morph( monster, targetc )
                || target_power < source_power - relax
                || target_power > source_power + relax);
    }

    const bool shifter = mons_is_shapeshifter( monster );
    const bool invis   = mons_is_invisible( monster );

    // messaging: {dlb}
    if (shifter)
        strcat( str_polymon, " changes into " );
    else if (targetc == MONS_PULSATING_LUMP)
        strcat( str_polymon, " degenerates into " );
    else
        strcat( str_polymon, " evaporates and reforms as " );

    if (mons_class_flag( targetc, M_INVIS ) && !player_see_invis())
        strcat( str_polymon, "something you cannot see!" );
    else
    {
        strcat( str_polymon, monam( 250, targetc, !invis, DESC_NOCAP_A ) );

        if (targetc == MONS_PULSATING_LUMP)
            strcat( str_polymon, " of flesh" );

        strcat( str_polymon, "!" );
    }

    player_messaged = mon_msg( monster, str_polymon );

    // the actual polymorphing:

    // first grab some old stats before define_monster() clobbers them
    int old_hp = monster->hit_points;
    int old_hp_max = monster->max_hit_points;

    /* deal with mons_sec */
    monster->type = targetc;
    monster->number = 250;
    monster->colour = mons_class_colour( targetc );

    define_monster( monster_index( monster ), true );

    // Doing it this way allows for monsters to degenerate into
    // shapeshifters (but not both types at the same time).
    if (monster->type == MONS_SHAPESHIFTER)
        set_mons_flag( monster, MF_SHAPESHIFT );
    else if (monster->type == MONS_GLOWING_SHAPESHIFTER)
        set_mons_flag( monster, MF_GLOW_SHAPESHIFT );

    monster->hit_points = 1 + monster->max_hit_points
                                * ((old_hp * 100) / old_hp_max) / 100
                                + random2( monster->max_hit_points );

    if (monster->hit_points > monster->max_hit_points)
        monster->hit_points = monster->max_hit_points;

    monster->energy = 67 + random2(6);

    if (mons_itemuse( monster->type ) < MONUSE_WEAPONS_ARMOUR)
    {
        int num_dropped = monster_drop_ething( monster );
        if (player_messaged && num_dropped)
        {
            mpr( "%s items fall%s to the ground.",
                 (num_dropped > 1) ? "Some" : "An",
                 (num_dropped > 1) ? "s" : "" );
        }
    }
    else
    {
        const int wpn = monster->inv[MSLOT_WEAPON];
        const int body = monster->inv[MSLOT_ARMOUR];
        const int shield = monster->inv[MSLOT_MISSILE];

        if (wpn != NON_ITEM && mitm[wpn].base_type == OBJ_WEAPONS)
        {
            if (get_weapon_brand( mitm[wpn] ))
                monster->armour_class += 5;
        }

        if (body != NON_ITEM && mitm[body].base_type == OBJ_ARMOUR)
        {
            monster->armour_class += property( mitm[body], PARM_AC );
            monster->armour_class += mitm[body].plus;
            monster->evasion += property( mitm[body], PARM_EVASION ) / 2;
        }

        if (shield != NON_ITEM && mitm[shield].base_type == OBJ_ARMOUR)
        {
            if (get_armour_ego_type( mitm[shield] ) == SPARM_PROTECTION)
                monster->armour_class += 3;
        }
    }

    return (player_messaged);
}                                        // end monster_polymorph()

// TODO: controlled blink currently assumes that the monster always wants
// to get out of LoS with a blink (most templates use controlled blink in
// the fleeing slot because of this)... however, we might want to add a case
// so that some monsters (healthy, strong melee types) can use controlled
// blink to quickly get adjacent to their target.
bool monster_blink( struct monsters *monster, bool control, bool quiet )
{
    int nx, ny;
    int count = 0;

    // store if monster is visible to player to start.
    const bool mon_seen = (mons_near( monster )
                            && player_monster_visible( monster ));

    do
    {
        if (!random_near_space( monster->x, monster->y, nx, ny, false, false ))
            return (false);

        count++;
    }
    while (control && see_grid( nx, ny ) && count < 10);

    move_monster_to_grid( monster_index(monster), nx, ny );

    if (!quiet)
    {
        if (mon_seen)
            mpr( "%s blinks.", ptr_monam( monster, DESC_CAP_THE ) );
        else if (mons_near( monster )
                && player_monster_visible( monster ))
        {
            // Use of the indefinite article here matches the
            // message for monster invis going down.
            mpr( "%s appears!", ptr_monam( monster, DESC_CAP_A ) );
        }
    }

    return (true);
}                               // end monster_blink()

// allow_adjacent:  allow target to be adjacent to origin
// restrict_LOS:    restict target to be within PLAYER line of sight
bool random_near_space( int ox, int oy, int &tx, int &ty,
                        bool allow_adjacent, bool restrict_LOS )
{
    int tries = 0;

    do
    {
        tx = ox - 7 + random2(15);
        ty = oy - 7 + random2(15);

        // origin is not 'near'
        if (tx == ox && ty == oy)
            continue;

        if (!in_bounds( tx, ty ))
            continue;

        tries++;

        if (tries > 149)
            break;
    }
    while ((restrict_LOS && !check_line_of_sight( ox, oy, tx, ty ))
           || grd[tx][ty] < DNGN_SHALLOW_WATER
           || something_there( tx, ty )
           || (!allow_adjacent && grid_distance( ox, oy, tx, ty ) < 2));

    return (tries < 150);
}                               // end random_near_space()

static bool habitat_okay( struct monsters *monster, int targ )
{
    bool ret = false;
    const int habitat = monster_habitat( monster->type );

    if (mons_flies( monster ))
    {
        // flying monsters don't care
        ret = true;
    }

    if (mons_class_flag( monster->type, M_BURROWS ) && targ == DNGN_ROCK_WALL)
        ret = true;

    if (monster->type == MONS_WATER_ELEMENTAL
        || mons_class_flag( monster->type, M_AMPHIBIOUS ))
    {
        // water elementals and amphibious creatures can crawl out over the land
        if (targ >= DNGN_DEEP_WATER)
            ret = true;
    }

    if (habitat == DNGN_DEEP_WATER)
    {
        // Water monster to water
        if (targ == DNGN_DEEP_WATER || targ == DNGN_SHALLOW_WATER)
            ret = true;
    }

    if (habitat == DNGN_LAVA
        || monster->type == MONS_FIRE_ELEMENTAL
        || mons_class_resist( monster->type, MR_RES_HELLFIRE ))
    {
        // Lava monster to lava
        if (targ == DNGN_LAVA)
            ret = true;
    }

    if (habitat == DNGN_FLOOR)
    {
        // FLOOR habitat monster going to a non-bad place
        if (targ >= DNGN_FLOOR || targ == DNGN_SHALLOW_WATER)
            ret = true;
    }

    return (ret);
}

// This doesn't really swap places, it just sets the monster's
// position equal to the player (the player has to be moved afterwards).
// It also has a slight problem with the fact the if the player is
// levitating over an inhospitable habitat for the monster the monster
// will be put in a place it normally couldn't go (this could be a
// feature because it prevents insta-killing).  In order to prevent
// that little problem, we go looking for a square for the monster
// to "scatter" to instead... and if we can't find one the monster
// just refuses to be swapped (not a bug, this is intentionally
// avoiding the insta-kill).  Another option is to look a bit
// wider for a vaild square (either by a last attempt blink, or
// by looking at a wider radius)...  insta-killing should be a
// last resort in this function (especially since Tome, Dig, and
// Summoning can be used to set up death traps).  If worse comes
// to worse, at least consider making the Swap spell not work
// when the player is over lava or water (if the player want's to
// swap pets to their death, we can let that go). -- bwr
bool swap_places( struct monsters *monster )
{
    bool swap;

    // Check if target grid is good for player to swap into... note that
    // non-swimmers are not being allowed to swap into shallow water.
    if (grd[monster->x][monster->y] < DNGN_FLOOR
        && !player_is_levitating()
        && !(you.species == SP_MERFOLK
            && (grd[monster->x][monster->y] == DNGN_SHALLOW_WATER
                || grd[monster->x][monster->y] == DNGN_DEEP_WATER)))
    {
        mon_msg( monster, " resists being moved." );
        return (false);
    }

    int loc_x = you.x_pos;
    int loc_y = you.y_pos;

    swap = habitat_okay( monster, grd[loc_x][loc_y] );

    // chose an appropiate habitat square at random around the target.
    if (!swap)
    {
        int num_found = 0;
        int temp_x, temp_y;

        for (int x = -1; x <= 1; x++)
        {
            temp_x = you.x_pos + x;

            for (int y = -1; y <= 1; y++)
            {
                if (x == 0 && y == 0)
                    continue;

                temp_y = you.y_pos + y;

                if (!in_bounds( temp_x, temp_y ))
                    continue;

                if (mgrd[temp_x][temp_y] == NON_MONSTER
                    && habitat_okay( monster, grd[temp_x][temp_y] ))
                {
                    // Found an appropiate space... check if we
                    // switch the current choice to this one.
                    num_found++;
                    if (one_chance_in( num_found ))
                    {
                        loc_x = temp_x;
                        loc_y = temp_y;
                    }
                }
            }
        }

        if (num_found)
            swap = true;
    }

    if (swap)
    {
        mon_msg( monster, "You swap places with %s." );
        move_monster_to_grid( monster_index(monster), loc_x, loc_y );
    }
    else
    {
        // Might not be ideal, but it's better that insta-killing
        // the monster... maybe try for a short blinki instead? -- bwr
        mon_msg( monster, " resists being moved." );
    }

    return (swap);
}                               // end swap_places()

void print_wounds(struct monsters *monster)
{
    // prevents segfault -- cannot use info[] here {dlb}
    char str_wound[INFO_SIZE];
    int  dam_level;

    if (monster->type == -1)
        return;

    if (monster->hit_points == monster->max_hit_points
        || monster->hit_points < 1)
    {
        return;
    }

    if (mons_class_flag( monster->type, M_NO_WOUNDS ))
        return;

    strcpy(str_wound, " is ");

    if (monster->hit_points <= monster->max_hit_points / 6)
    {
        strcat(str_wound, "almost ");
        strcat(str_wound, wounded_damaged(monster->type) ? "destroyed"
                                                         : "dead");
        dam_level = MDAM_ALMOST_DEAD;
    }
    else
    {
        if (monster->hit_points <= monster->max_hit_points / 6)
        {
            strcat(str_wound, "horribly ");
            dam_level = MDAM_HORRIBLY_DAMAGED;
        }
        else if (monster->hit_points <= monster->max_hit_points / 3)
        {
            strcat(str_wound, "heavily " );
            dam_level = MDAM_HEAVILY_DAMAGED;
        }
        else if (monster->hit_points <= 3 * (monster-> max_hit_points / 4))
        {
            strcat(str_wound, "moderately ");
            dam_level = MDAM_MODERATELY_DAMAGED;
        }
        else
        {
            strcat(str_wound, "lightly ");
            dam_level = MDAM_LIGHTLY_DAMAGED;
        }

        strcat(str_wound, wounded_damaged(monster->type) ? "damaged"
                                                         : "wounded");
    }

    strcat(str_wound, ".");
    mon_msg(monster, str_wound, true, MSGCH_MONSTER_DAMAGE, dam_level);
}                               // end print_wounds()

// (true == 'damaged') [constructs, undead, etc.]
// and (false == 'wounded') [living creatures, etc.] {dlb}
bool wounded_damaged( int wound_class )
{
    const int holy = mons_class_holiness( wound_class );

    if (holy == MH_UNDEAD || holy == MH_NONLIVING || holy == MH_PLANT)
        return (true);

    return (false);
}                               // end wounded_damaged()


//---------------------------------------------------------------
//
// behaviour_event
//
// 1. Change any of: monster state, foe, and attitude
// 2. Call handle_behaviour to re-evaluate AI state and target x,y
//
//---------------------------------------------------------------
void behaviour_event( struct monsters *mon, int event, int src,
                      int src_x, int src_y )
{
    const bool isSmart = (mons_intel(mon->type) > I_ANIMAL);
    const bool is_friendly = mons_friendly(mon);
    const bool isScared = mons_has_ench( mon, ENCH_FEAR );

    bool sourceFriendly = false;
    bool setTarget = false;
    bool breakCharm = false;

    if (src == MHITYOU)
        sourceFriendly = true;
    else if (src != MHITNOT)    // ie src is a monster id
    {
        // ignore events we cause ourselves
        if (monster_index(mon) == src)
            return;

        sourceFriendly = mons_friendly( &menv[src] );
    }

    switch (event)
    {
    case ME_DISTURB:
        // assumes disturbed by noise...
        if (mon->behaviour == BEH_SLEEP)
            mon->behaviour = BEH_INVESTIGATE;

        // A bit of code to make Project Noise actually so
        // something again.  Basically, stupid monsters and
        // monsters who aren't otherwise occupied will at
        // least consider the (apparent) source of the noise
        // interesting for a moment. -- bwr
        if (!isSmart || mon->foe == MHITNOT || mon->behaviour == BEH_WANDER)
        {
            mon->behaviour = BEH_INVESTIGATE;

            if (src_x && src_y)
            {
                mon->target_x = src_x;
                mon->target_y = src_y;
            }
        }
        break;

    case ME_WHACK:
    case ME_SHOT:
        if (!isScared)
        {
            if (mon->behaviour == BEH_FLEE)
            {
                int dist = FAR_AWAY;

                if (src == MHITYOU)
                    dist = grid_distance(mon->x, mon->y, you.x_pos, you.y_pos);
                else if (src != MHITNOT)
                    dist = grid_distance(mon->x, mon->y, menv[src].x, menv[src].y);

                // Note: Cannot give message here since it would come
                // between the shot message and the hit message.
                if (event == ME_WHACK
                    || (dist <= 3 && one_chance_in(3))
                    || one_chance_in(10))
                {
                    mon->behaviour = BEH_CORNERED;
                }
            }
            else if (mon->behaviour != BEH_CORNERED
                    && mon->behaviour != BEH_PANIC
                    && mon->behaviour != BEH_FLEE)
            {
                mon->behaviour = BEH_SEEK;
            }
        }

        mon->foe = src;
        setTarget = true;

        if (sourceFriendly && is_friendly)
        {
            mon->attitude = ATT_HOSTILE;
            breakCharm = true;
        }
        break;

    case ME_ALERT:
        // will alert monster to <src> and turn them
        // against them,  unless they have a current foe.
        // it won't turn friends hostile either.
        if (mon->behaviour != BEH_CORNERED && !isScared)
            mon->behaviour = BEH_SEEK;

        if (mon->foe == MHITYOU)
        {
            if (src == MHITYOU)
            {
                mon->target_x = you.x_pos;
                mon->target_y = you.y_pos;
            }
        }
        else if (mon->foe == MHITNOT)
        {
            mon->foe = src;
            if (src == MHITYOU)
            {
                mon->target_x = you.x_pos;
                mon->target_y = you.y_pos;
            }
            else if (src != MHITNOT)
            {
                mon->target_x = menv[src].x;
                mon->target_y = menv[src].y;
            }
        }
        break;

    case ME_SCARE:  // only from scroll/spell of fear (so no BEH_PANIC case)
        mon->foe = src;
        mon->behaviour = BEH_FLEE;
        // assume monsters know where to run from, even
        // if player is invisible.
        setTarget = true;
        breakCharm = true;
        break;

    case ME_CORNERED:
        // just set behaviour.. foe doesn't change.
        if (mon->behaviour != BEH_CORNERED && !isScared)
        {
            mon_msg(mon, " turns to fight!");
            mon->behaviour = BEH_CORNERED;
        }
        break;

    case ME_EVAL:
    default:
        break;
    }

    if (setTarget)
    {
        if (src == MHITYOU)
        {
            mon->target_x = you.x_pos;
            mon->target_y = you.y_pos;
            mon->attitude = ATT_HOSTILE;
        }
        else if (src != MHITNOT)
        {
            mon->target_x = menv[src].x;
            mon->target_y = menv[src].y;
        }
    }

    // now, break charms if appropriate
    if (breakCharm)
        mons_del_ench( mon, ENCH_CHARM );

    // do any resultant foe or state changes
    handle_behaviour( mon );
}

void set_wander_target( struct monsters *mon )
{
    int i;

    const int use = mons_itemuse( mon->type );
    const int want_equip = (use >= MONUSE_WEAPONS_ARMOUR);
    const int want_items = (use == MONUSE_EATS_ITEMS);
    const int want_corpse = mons_likes_carrion( mon );

    if (want_corpse || want_equip || want_items)
    {
        int count = 0;
        int targ_item = NON_ITEM;
        int best_dist = 100;

        for (i = 0; i < MAX_ITEMS; i++)
        {
            if (!is_valid_item( mitm[i] ))
                continue;

            if (mon->x == mitm[i].x && mon->y == mitm[i].y)
                continue;

            const int dist = grid_distance( mon->x, mon->y, mitm[i].x, mitm[i].y );
            if (dist > best_dist)
                continue;

            if (!mons_line_of_sight( mon, mitm[i].x, mitm[i].y ))
                continue;

            bool desirable = false;

            if (want_items)
                desirable = true;
            else if (want_equip || want_corpse)
            {
                switch (mitm[i].base_type)
                {
                case OBJ_WEAPONS:
                    if (mons_likes_weapon( mon, mitm[i] ))
                        desirable = true;
                    break;

                case OBJ_ARMOUR:
                    if (mons_likes_armour( mon, mitm[i] ))
                        desirable = true;
                    break;

                case OBJ_MISSILES:
                    // other things are prefered to missiles
                    if ((targ_item == NON_ITEM
                            || mitm[targ_item].base_type == OBJ_MISSILES)
                        && mons_likes_ammo( mon, mitm[i] ))
                    {
                        desirable = true;
                    }
                    break;

                case OBJ_SCROLLS:
                    if (mons_likes_scroll( mon, mitm[i] ))
                        desirable = true;
                    break;

                case OBJ_POTIONS:
                    if (mons_likes_potion( mon, mitm[i] ))
                        desirable = true;
                    break;

                case OBJ_WANDS:
                    if (mons_likes_wand( mon, mitm[i] ))
                        desirable = true;
                    break;

                case OBJ_FOOD:
                    if (mitm[i].sub_type != FOOD_CHUNK)
                        break;
                    // fall through
                case OBJ_CORPSES:
                    if (want_corpse)
                        desirable = true;
                    break;

                case OBJ_GOLD:
                    // other things are prefered to gold
                    desirable = (targ_item == NON_ITEM
                                || mitm[targ_item].base_type == OBJ_GOLD);
                    break;

                default:
                    break;
                }
            }

            if (desirable)
            {
                if (dist < best_dist)   // item is closer
                {
                    best_dist = dist;
                    count = 1;
                    targ_item = i;
                }
                else // randomly pick one at closest distance:
                {
                    count++;

                    if (one_chance_in(count))
                        targ_item = i;
                }
            }
        }  // for each item

        if (targ_item != NON_ITEM)
        {
            mon->target_x = mitm[targ_item].x;
            mon->target_y = mitm[targ_item].y;
            return;
        }
        // fall through to random if no good items
    } // if wants stuff

    // Try a few times to find a wandering point we can at least
    // take one step towards.
    const int hab = monster_habitat( mon->type );

    for (i = 5; i > 0; i--)
    {
        int count = 0;
        int tx, ty;

        for (int x = -i; x <= i; x++)
        {
            for (int y = -i; y <= i; y++)
            {
                if (abs(x) != i && abs(y) != i)
                    continue;

                if (!in_bounds( mon->x + x, mon->y + y ))
                    continue;

                const int dx = sgn(x);
                const int dy = sgn(y);
                const int grid = grd[mon->x + dx][mon->y + dy];

                if (grid == hab
                    || (hab == DNGN_DEEP_WATER && grid == DNGN_SHALLOW_WATER)
                    || (hab == DNGN_FLOOR && grid >= DNGN_SHALLOW_WATER)
                    || (mon->type == MONS_BOULDER_BEETLE && grid == DNGN_ROCK_WALL))
                {
                    continue;
                }

                if (mons_line_of_sight( mon, mon->x + x, mon->y + y ))
                {
                    count++;
                    if (one_chance_in( count ))
                    {
                        tx = x;
                        ty = y;
                    }
                }
            }
        }

        if (count)
        {
            mon->target_x = tx;
            mon->target_y = ty;
            break;
        }
    }

    if (i == 0)
        random_in_bounds( mon->target_x, mon->target_y );
}

//---------------------------------------------------------------
//
// handle_behaviour
//
// 1. Evalutates current AI state
// 2. Sets monster targetx,y based on current foe
//
//---------------------------------------------------------------
static void handle_behaviour( struct monsters *mon )
{
    bool changed = true;
    bool is_friendly = mons_friendly(mon);
    bool proxPlayer = false;
    bool proxFoe = false;

    // Note: the -1 keeps weak creatures from fleeing too often.
    bool isHurt = (mon->hit_points <= mon->max_hit_points / 4 - 1);

    // !isHurt is for safety, since isHurt && isHealthy cause an infinite loop
    bool isHealthy = (!isHurt
                    && (mon->hit_points == mon->max_hit_points
                        || (mon->hit_points >= 10
                            && mon->hit_points > mon->max_hit_points / 2)));

    bool isSmart = (mons_intel(mon->type) > I_ANIMAL);
    bool canThink = (mons_intel(mon->type) != I_PLANT);
    bool isSlow = (mons_speed(mon) < 10);
    bool isScared = mons_has_ench( mon, ENCH_FEAR );

    const bool isMobile = !mons_stationary( mon->type );

    // check for confusion -- early out.
    if (mons_is_confused( mon ))
    {
        mon->target_x = mon->x + random2(3) - 1;
        mon->target_y = mon->y + random2(3) - 1;
        return;
    }

    // validate current target exists
    if (mon->foe != MHITNOT && mon->foe != MHITYOU)
    {
        if (menv[mon->foe].type == -1)
            mon->foe = MHITNOT;
    }

    // First we check LoS to our target to set proxPlayer, note that proxFoe
    // is evaluated each time at the top of the loop below so we don't bother
    // with it here.
    proxPlayer = mons_near(mon);

    // Burrowers have some sort of LoS-range clairvoyance so that burrowing
    // actually works.  XXX: Maybe there should be other limitations here
    // to better judge how well they sense through stone.
    if (!proxPlayer && mons_class_flag( mon->type, M_BURROWS ))
    {
        proxPlayer = (grid_distance( mon->x, mon->y, you.x_pos, you.y_pos )
                            <= MONSTER_LOS_RANGE);
    }

    // Now change proxPlayer depending on invisibility and standing
    // in shallow water
    if (proxPlayer && you.invis)
    {
        // check monster see invis, player stand in water, etc
        if (!mons_player_visible( mon ))
            proxPlayer = false;

        // now, the corollary to that is that sometimes, if a
        // player is right next to a monster, they will 'see'
        if (adjacent( you.x_pos, you.y_pos, mon->x, mon->y )
            && one_chance_in(3))
        {
            proxPlayer = true;
        }
    }

    // set friendly target, if they don't already have one
    if (is_friendly
        && you.pet_target != MHITNOT
        && (mon->foe == MHITNOT || mon->foe == MHITYOU))
    {
        mon->foe = you.pet_target;
    }

    // monsters do not attack themselves {dlb}
    if (mon->foe == monster_index( mon ))
        mon->foe = MHITNOT;

    // friendly monsters do not attack other friendly monsters
    if (mon->foe != MHITNOT && mon->foe != MHITYOU)
    {
        if (is_friendly && mons_friendly(&menv[mon->foe]))
            mon->foe = MHITNOT;
    }

    // unfriendly monsters fighting other monsters will usually
    // target the player,  if they're healthy
    if (!is_friendly && mon->foe != MHITYOU && mon->foe != MHITNOT
        && proxPlayer && !one_chance_in(3) && isHealthy)
    {
        mon->foe = MHITYOU;
    }

    // validate target again
    if (mon->foe != MHITNOT && mon->foe != MHITYOU)
    {
        if (menv[mon->foe].type == -1)
            mon->foe = MHITNOT;
    }

    // changed to a for loop
    int i;
    bool eval_proxFoe = true;

    for (i = 0; changed && i < 1000; i++)
    {
        int foe_x = you.x_pos;
        int foe_y = you.y_pos;

        // foe can change in this loop, so we may need to re-evaluate.
        if (eval_proxFoe)
        {
            if (mon->foe == MHITNOT)
                proxFoe = false;
            else
            {
                if (mon->foe == MHITYOU)
                {
                    foe_x = you.x_pos;
                    foe_y = you.y_pos;
                    proxFoe = proxPlayer;
                }
                else
                {
                    foe_x = menv[mon->foe].x;
                    foe_y = menv[mon->foe].y;

                    // first the LoS/Burrowing check:
                    if (mons_class_flag( mon->type, M_BURROWS ))
                    {
                        proxFoe = (grid_distance( mon->x, mon->y, foe_x, foe_y )
                                                    <= MONSTER_LOS_RANGE);
                    }
                    else
                    {
                        proxFoe = (check_line_of_sight( mon->x, mon->y,
                                                        foe_x, foe_y ));
                    }

                    // ... and duplicating the proxPlayer visibility check:
                    if (proxFoe && mons_is_invisible( &menv[mon->foe] ))
                    {
                        if (!mons_monster_visible( mon, &menv[mon->foe] ))
                            proxFoe = false;

                        // now, the corollary to that is that sometimes, if a
                        // player is right next to a monster, they will 'see'
                        if (adjacent( mon->x, mon->y,
                                      menv[mon->foe].x, menv[mon->foe].y )
                                && one_chance_in(3))

                        {
                            proxFoe = true;
                        }
                    }
                }
            }

            eval_proxFoe = false;
        }

        // track changes to state; attitude never changes here.
        beh_type    new_beh = mon->behaviour;
        int         new_foe = mon->foe;

        // take care of monster state changes
        switch (mon->behaviour)
        {
        case BEH_SLEEP:
            // default sleep state
            mon->target_x = mon->x;
            mon->target_y = mon->y;
            new_foe = MHITNOT;
            break;

        case BEH_SEEK:
            // no foe? then wander or seek the player
            if (mon->foe == MHITNOT)
            {
                if (!proxPlayer)
                    new_beh = BEH_WANDER;
                else
                {
                    new_foe = MHITYOU;
                    mon->target_x = you.x_pos;
                    mon->target_y = you.y_pos;
                }

                break;
            }

            // foe gone out of LOS?
            if (!proxFoe)
            {
                if (is_friendly)
                {
                    new_foe = MHITYOU;
                    mon->target_x = you.x_pos;
                    mon->target_y = you.y_pos;
                    break;
                }

                if (mon->foe_memory > 0 && mon->foe != MHITNOT)
                {
                    // if we've arrived at our target x,y
                    // do a stealth check.  If the foe
                    // fails,  monster will then start
                    // tracking foe's CURRENT position,
                    // but only for a few moves (smell and
                    // intuition only go so far)
                    if (mon->x == mon->target_x && mon->y == mon->target_y)
                    {
                        if (mon->foe == MHITYOU)
                        {
                            if (check_awaken( monster_index(mon) ))
                            {
                                mon->target_x = you.x_pos;
                                mon->target_y = you.y_pos;
                            }
                            else
                                mon->foe_memory = 1;
                        }
                        else if (mon->foe != MHITNOT)
                        {
                            if (coinflip())     // XXX: cheesy!
                            {
                                mon->target_x = foe_x;
                                mon->target_y = foe_y;
                            }
                            else
                                mon->foe_memory = 1;
                        }
                    }

                    // either keep chasing, or start
                    // wandering.
                    if (mon->foe_memory < 2)
                    {
                        mon->foe_memory = 0;
                        new_beh = BEH_WANDER;
                    }
                    break;
                }

                // hack: smarter monsters will
                // tend to persue the player longer.
                int memory;
                switch (mons_intel( monster_index(mon) ))
                {
                    case I_HIGH:
                        memory = 100 + random2(200);
                        break;
                    case I_NORMAL:
                        memory = 50 + random2(100);
                        break;
                    case I_ANIMAL:
                    default:
                        memory = 25 + random2(75);
                        break;
                    case I_INSECT:
                        memory = 10 + random2(50);
                        break;
                }

                mon->foe_memory = memory;
                break;      // from case
            } // !proxFoe

            // monster can see foe: continue 'tracking'
            // by updating target x,y
            if (mon->foe == MHITYOU)
            {
                // sometimes,  your friends will wander a bit.
                if (is_friendly && one_chance_in(8))
                {
                    mon->foe = MHITNOT;
                    new_beh = BEH_WANDER;
                }
                else
                {
                    mon->target_x = you.x_pos;
                    mon->target_y = you.y_pos;
                }
            }
            else if (mon->foe != MHITNOT)
            {
                mon->target_x = foe_x;
                mon->target_y = foe_y;
            }

            if (isHurt && canThink && isMobile)
            {
                new_beh = ((isSmart) ? BEH_PANIC :
                           (isSlow)  ? BEH_CORNERED
                                     : BEH_FLEE);
            }
            break;

        case BEH_INVESTIGATE:
            if (grid_distance(mon->x, mon->y, mon->target_x, mon->target_y) <= 2)
            {
                if (mgrd[mon->target_x][mon->target_y] != NON_MONSTER)
                {
                    new_foe = mgrd[mon->target_x][mon->target_y];
                    new_beh = BEH_SEEK;
                }
                else if (you.x_pos == mon->target_x
                        && you.y_pos == mon->target_y)
                {
                    new_foe = MHITYOU;
                    new_beh = BEH_SEEK;
                }
                else
                {
                    new_beh = BEH_WANDER;
                }
            }
            break;

        case BEH_WANDER:
            // is our foe in LOS?
            // Batty monsters don't automatically reseek so that
            // they'll flitter away, we'll reset them just before
            // they get movement in handle_monsters() instead. -- bwr
            if (proxFoe && !mons_is_batty( mon ))
            {
                new_beh = BEH_SEEK;
                break;
            }

            // default wander behaviour
            //
            // XXX: This is really dumb wander behaviour... instead of
            // changing the goal square every turn, better would be to
            // have the monster store a direction and have the monster
            // head in that direction for a while, then shift the
            // direction to the left or right.  We're changing this so
            // wandering monsters at least appear to have some sort of
            // attention span.  -- bwr
            if (mons_is_batty( mon ))
            {
                random_in_bounds( mon->target_x, mon->target_y );
            }
            else if ((mon->x == mon->target_x && mon->y == mon->target_y)
                    || one_chance_in(20))
            {
                set_wander_target( mon );
            }

            // during their wanderings,  monsters will
            // eventually relax their guard (stupid
            // ones will do so faster,  smart monsters
            // have longer memories
            if (!proxFoe && mon->foe != MHITNOT)
            {
                if (one_chance_in( isSmart ? 60 : 20 ))
                    new_foe = MHITNOT;
            }
            break;

        case BEH_FLEE:
        case BEH_PANIC:
            // check for healed
            if (isHealthy && !isScared)
                new_beh = BEH_SEEK;

            // smart monsters flee until they can
            // flee no more...  possible to get a
            // 'CORNERED' event,  at which point
            // we can jump back to WANDER if the foe
            // isn't present.

            if (proxFoe)
            {
                // try to flee _from_ the correct position
                mon->target_x = foe_x;
                mon->target_y = foe_y;
            }
            break;

        case BEH_CORNERED:
            if (isHealthy && !isScared)
                new_beh = BEH_SEEK;

            // foe gone out of LOS?
            if (!proxFoe)
            {
                if (is_friendly || proxPlayer)
                    new_foe = MHITYOU;
                else
                    new_beh = ((isSmart) ? BEH_PANIC : BEH_FLEE);
            }
            else
            {
                mon->target_x = foe_x;
                mon->target_y = foe_y;
            }
            break;

        default:
            return;     // uh oh
        }

        changed = (new_beh != mon->behaviour || new_foe != mon->foe);

        mon->behaviour = new_beh;

        if (mon->foe != new_foe)
        {
            mon->foe_memory = 0;
            mon->foe = new_foe;
            eval_proxFoe = true;
        }
    }

#ifdef DEBUG_DIAGNOSTICS
    // This shouldn't happen... but it has been a site of game hanging bugs
    // in the past and we want to know about it without hanging the game.
    if (i >= 1000)
    {
        mpr( MSGCH_WARN, "WARNING: behaviour loop used max iterations and aborted!" );
    }
#endif
}                               // end handle_behaviour()

// Intentionally avoids info and str_pass now. -- bwr
bool mon_msg( struct monsters *monster, const char *event,
              bool check_vis, msg_channel_type channel, int param )
{
    char buff[ INFO_SIZE ];

    if (!check_vis
        || (mons_near( monster )
            && (channel == MSGCH_MONSTER_SPELL
                || player_monster_visible(monster))))
    {
        if (strstr( event, "%s" ) != NULL)
        {
            snprintf( buff, sizeof(buff), event,
                      ptr_monam( monster, DESC_NOCAP_THE ) );
        }
        else
        {
            snprintf( buff, sizeof(buff), "%s%s",
                      ptr_monam( monster, DESC_CAP_THE ), event );
        }

        mpr( channel, param, buff );
        return (true);
    }

    return (false);
}                               // end mon_msg()

// Used to adjust enchantment durations for amount of time passed between
// player actions (as this function should only be called once for each
// monster per player action)
static inline int mod_time( int val )
{
    return (div_rand_round( val * you.time_taken, 10 ));
}

// returns true if monster died
static bool handle_enchantment( struct monsters *monster, int time_passed )
{
    ASSERT( time_passed > 0 );

    int  levels;
    int  dam;
    int  tmp;

    bool died = false;

    for (int p = 0; p < NUM_MON_ENCHANTS && !died; p++)
    {
        mon_enchant_def *const ench = &(monster->ench[p]);

        const killer_type  killer = (ench->source == MHITYOU) ? KILL_YOU
                                                              : KILL_MISC;

        switch (ench->type)
        {
        case ENCH_NONE:
            break;

        case ENCH_SLOW:
        case ENCH_HASTE:
        case ENCH_FEAR:
        case ENCH_CONFUSION:
        case ENCH_INVIS:
        case ENCH_BACKLIGHT:
        case ENCH_CHARM:
        case ENCH_SLEEP_WARY:
        case ENCH_PETRIFY:
        case ENCH_PARALYSIS:
        case ENCH_QUIET:
        case ENCH_STUN:
        default:
            mons_dec_ench_ptr( monster, ench, time_passed );
            break;

        case ENCH_CUT:
            // XXX: is this damage reasonable?  too small? too infrequent?
            // XXX: scale this damage to duration?
            if (random2(1000) < mod_time(333))
            {
                mon_msg( monster, " is bleeding!" );
                tmp = roll_dice(1,3);

                if (hurt_monster_to_kill( monster, tmp, killer, ench->source ))
                    died = true;
            }

            if (!died)
                mons_dec_ench_ptr( monster, ench, time_passed );
            break;

        case ENCH_POISON:
            levels = dur_to_levels( ENCH_POISON, ench->duration );

            dam = (levels > 2) ? 1 : 0;

            if (coinflip())
                dam += roll_dice( 1, levels + 2 );

            if (mons_res_poison( monster ) < 0)
                dam += roll_dice( 2, levels ) - 1;

            // Adjust damage for time since last call
            dam = div_rand_round( dam * time_passed, 10 );

            if (dam)
            {
#if DEBUG_DIAGNOSTICS
                mpr( MSGCH_DIAGNOSTICS, "%s poison: %d",
                     ptr_monam( monster, DESC_PLAIN ), dam );
#endif
                if (hurt_monster_to_kill( monster, dam, killer, ench->source ))
                    died = true;
            }

            if (!died)
                mons_dec_ench_ptr( monster, ench, time_passed );
            break;

        // assumption: mons_res_fire has already been checked
        case ENCH_STICKY_FLAME:
            dam = roll_dice(2,4) - 1;

            if (mons_res_fire( monster ) < 0)
                dam += roll_dice(2,5) - 1;

            // Adjust damage for time since last call
            dam = div_rand_round( dam * time_passed, 10 );

            if (dam)
            {
                mon_msg( monster, " burns!" );
#if DEBUG_DIAGNOSTICS
                mpr( MSGCH_DIAGNOSTICS, "sticky flame damage: %d", dam );
#endif
                if (hurt_monster_to_kill( monster, dam, killer, ench->source ))
                    died = true;
            }

            if (!died)
                mons_dec_ench_ptr( monster, ench, time_passed );
            break;

        case ENCH_ROT:
            //jmf: FIXME: if (undead) make_small_rot_cloud();
            // XXX: is this damage reasonable?  too small? too infrequent?
            levels = dur_to_levels( ENCH_ROT, ench->duration );
            if (random2(3000) < mod_time( 150 * levels ))
            {
                monster->max_hit_points--;

                tmp = roll_dice( 1, levels );

                if (hurt_monster_to_kill( monster, tmp, killer, ench->source ))
                    died = true;
            }

            if (!died)
                mons_dec_ench_ptr( monster, ench, time_passed );
            break;

        case ENCH_SUMMONED:
            died = mons_dec_ench_ptr( monster, ench, time_passed );
            break;

        case ENCH_TELEPORT:
            if (mons_dec_ench_ptr( monster, ench, time_passed ))
                monster_teleport( monster );
            break;
        }
    }

    const int  habitat = monster_habitat( monster->type );
    const int  grid    = grd[monster->x][monster->y];

    if (mons_is_submerged( monster )
        && !is_cloud( monster->x, monster->y ))
    {
        if (monster->type == MONS_AIR_ELEMENTAL)
        {
            // Air elementals are a special case, as their submerging in
            // air isn't up to choice. -- bwr
            if (one_chance_in(5))
                mons_unsubmerge( monster );
        }
        else if (habitat == DNGN_FLOOR || habitat != grid)
        {
            mons_unsubmerge( monster ); // forced to surface
        }
        else if (monster->hit_points > monster->max_hit_points / 2)
        {
            // Now we handle the others:
            // Badly injured monsters prefer to stay submerged...
            // electrical eels and lava snakes have ranged attacks
            // and are more likely to surface.  -- bwr
            if (((monster->type == MONS_ELECTRICAL_EEL
                    || monster->type == MONS_LAVA_SNAKE)
                && (random2(1000) < mod_time(20)
                    || (mons_near(monster)
                        && monster->hit_points == monster->max_hit_points)))
                || random2(5000) < mod_time(10))
            {
                mons_unsubmerge( monster );
            }
        }
    }

    if (mons_is_shapeshifter( monster )
        && mons_sound_mind_and_body( monster, true ))
    {
        // This way glowing shapeshift takes precidence when if both are set.
        if (mons_flag( monster, MF_GLOW_SHAPESHIFT ))
            tmp = 250;
        else // regular shapeshifter
            tmp = 1000 / ((15 * monster->hit_dice) / 5);

        if (monster->type == MONS_GLOWING_SHAPESHIFTER
            || monster->type == MONS_SHAPESHIFTER
            || random2(1000) < mod_time(tmp))
        {
            monster_polymorph( monster, RANDOM_MONSTER, 0 );
        }
    }

    return (died);
}                               // end handle_enchantment()

static int friends_near_target( struct monsters *monster )
{
    int friends = 0;

    if (monster->foe != MHITNOT)
    {
        for (int cx = -2; cx <= 2; cx++)
        {
            for (int cy = -2; cy <= 2; cy++)
            {
                const int tx = monster->target_x + cx;
                const int ty = monster->target_y + cy;

                // valid target?
                if (!in_bounds( tx, ty )
                    || mgrd[tx][ty] == NON_MONSTER
                    || mgrd[tx][ty] == monster_index( monster ))
                {
                    continue;
                }

                // ignore any "friends" who won't make good distractions:
                if (mons_stationary( monster->type )
                    || mons_class_flag( monster->type, M_NO_EXP_GAIN ))
                {
                    continue;
                }

                if (beings_aligned( monster_index(monster), mgrd[tx][ty] ))
                    friends++;
            }
        }
    }

    return (friends);
}

//---------------------------------------------------------------
//
// calc_movement
//
// Calculate mmov_x and mmov_y for the monster
//
//---------------------------------------------------------------
static void calc_movement( struct monsters *monster, bool allow_hold )
{
    int tx = monster->target_x;
    int ty = monster->target_y;

    bool see_target = false;

    // locate target of our movement
    if (mons_class_flag( monster->type, M_BURROWS ))
    {
        // Burrowing monsters always head right towards their targets...
        if (monster->foe == MHITYOU)
        {
            tx = you.x_pos;
            ty = you.y_pos;
        }
        else if (monster->foe != MHITNOT)
        {
            tx = menv[ monster->foe ].x;
            ty = menv[ monster->foe ].y;
        }
    }

    // extract LoS and delta to our target
    see_target = check_line_of_sight( monster->x, monster->y, tx, ty );

    int dx = tx - monster->x;
    int dy = ty - monster->y;

    // calculate the monster's move:
    mmov_x = sgn(dx);
    mmov_y = sgn(dy);

    // Note that unseen refers to visibility, and has nothing to do with
    // LoS to the target...  see_target might be refering to an empty
    // square that the monster wishes to go to.
    const bool unseen = ((monster->foe == MHITYOU
                            && !player_monster_visible( monster ))
                        || ((monster->foe != MHITNOT && monster->foe != MHITYOU)
                            && !mons_monster_visible( monster, &menv[monster->foe] )));

    bool targ_hurt = false;
    bool targ_friend = false;

    if (monster->foe == MHITYOU)
    {
        targ_hurt = (you.hp < you.hp_max / 4);
        targ_friend = mons_friendly( monster );
    }
    else if (monster->foe != MHITNOT)
    {
        const struct monsters *tmon = &menv[monster->foe];
        targ_hurt = (tmon->hit_points < tmon->max_hit_points / 4);
        targ_friend = beings_aligned( monster_index(monster), monster->foe );
    }

    bool back_off = false;
    bool hold_pos = false;

    if (!targ_friend)
    {
        const bool healthy = (monster->hit_points > monster->max_hit_points / 2);
        const bool range_ability = (mons_has_ranged_spell( monster )
                                    || mons_has_ranged_attack( monster ));

        const int friends = friends_near_target( monster );

        const bool adj  = (abs(dx) < 2 && abs(dy) < 2);
        const bool pnear = (abs(dx) <= 2 && abs(dy) <= 2);

        const bool teleport = mons_has_ench( monster, ENCH_TELEPORT );

        // "Determine" if we should back-off or hold pos on our enemy target.
        if (monster->behaviour == BEH_FLEE)
        {
            if (!targ_hurt || !adj)
                back_off = true;
        }
        else if (monster->behaviour == BEH_PANIC)
        {
            if (!see_target)
                back_off = true;
            else if (unseen)
            {
                if (friends || !one_chance_in(3))
                    back_off = true;
            }
            else if (teleport && !targ_friend)
            {
                if (adj)
                {
                    if (!targ_hurt)
                        back_off = true;
                }
                else if (pnear)
                {
                    if (range_ability && allow_hold)
                        hold_pos = true;
                    else
                        back_off = true;
                }
            }
            else if (range_ability)
            {
                if (adj && (friends || (!targ_hurt && one_chance_in(5))))
                    back_off = true;
                else if (!adj)
                {
                    if (friends > 1 || !allow_hold || one_chance_in(5))
                        back_off = true;
                    else
                        hold_pos = true;
                }
            }
            else if (friends > 1) // vis melee monster, LoS of target, 2 friends
            {
                back_off = true;
            }
        }
        else if (see_target && range_ability)
        {
            if (!pnear)
            {
                if (allow_hold && coinflip())
                    hold_pos = true;
                else if (friends && coinflip())
                    back_off = true;
            }
            else if (!adj && allow_hold)
                hold_pos = true;
            else if (friends > 1 && !healthy && (!targ_hurt || coinflip()))
                back_off = true;
        }

        // finally apply back_off or hold_pos
        if (back_off || (hold_pos && !allow_hold))
        {
            mmov_x *= -1;
            mmov_y *= -1;
        }
        else if (hold_pos && (!mons_friendly( monster ) || one_chance_in(5)))
        {
            ASSERT( allow_hold );

            // Friendly monsters don't use hold_pos as often since
            // it might make them too powerful (as this causes the
            // handle functions to assume that the monster is cornered
            // and try more interesting things).
            mmov_x = 0;
            mmov_y = 0;
        }
    }

    // bounds check: don't let fleeing monsters try to run
    // off the map
    if (monster->target_x + mmov_x <= X_BOUND_1
        || monster->target_x + mmov_x >= X_BOUND_2)
    {
        mmov_x = 0;
    }

    if (monster->target_y + mmov_y <= Y_BOUND_1
        || monster->target_y + mmov_y >= Y_BOUND_2)
    {
        mmov_y = 0;
    }

    // now quit if we're can't move
    if (mmov_x == 0 && mmov_y == 0)
        return;

    // reproduced here is some semi-legacy code that makes monsters
    // move somewhat randomly along oblique paths.  It is an exceedingly
    // good idea,  given crawl's unique line of sight properties. -- gdl?
    //
    // Added a check so that oblique movement paths aren't used when
    // close to the target square.  Monsters that aren't seen by their
    // targets, or who are fleeing may still move obliquely, since
    // it makes their movement slightly more unpredictable.  -- bwr
    if (grid_distance( dx, dy, 0, 0 ) > 3 || unseen || (see_target && back_off))
    {
        const int ax = abs(dx);
        const int ay = abs(dy);

        if (ax > ay)
        {
            // sometimes we'll just move parallel the x axis
            if (one_chance_in( (ax >= ay * 2) ? 2 : 3 ))
                mmov_y = 0;
        }
        else if (ay > ax)
        {
            // sometimes we'll just move parallel the y axis
            if (one_chance_in( (ay >= ax * 2) ? 2 : 3 ))
                mmov_x = 0;
        }
    }
}                               // end calc_movement()

//---------------------------------------------------------------
//
// handle_nearby_ability
//
// Gives monsters a chance to use a special ability when they're
// next to the player.
//
//---------------------------------------------------------------
static bool handle_nearby_ability( struct monsters *monster )
{
    if (!mons_near( monster ) || monster->behaviour == BEH_SLEEP)
        return (false);

    const bool attentive = (monster->behaviour != BEH_WANDER
                            && monster->behaviour != BEH_INVESTIGATE
                            && !mons_is_submerged( monster ));

    // XXX: perhaps monsters should get some wandering dialog? -- bwr
    if (mons_class_flag( monster->type, M_SPEAKS ) && attentive && one_chance_in(21))
        mons_speaks(monster);

    switch (monster->type)
    {
    case MONS_GIANT_EYEBALL:
        if (((!you.paralysis || one_chance_in(3))
                && coinflip())
            && !mons_friendly(monster)
            && attentive)
        {
            mon_msg(monster, " stares at you.");
            paralyse_player( 2 + random2(3) );
        }
        break;

    case MONS_EYE_OF_DRAINING:
        if (you.magic_points
            && !one_chance_in(3)
            && !mons_friendly(monster)
            && attentive)
        {
            mon_msg( monster, " stares at you." );

            dec_mp( 2 + roll_dice(3,5) );
            heal_monster( monster, 10, true ); // heh heh {dlb}
        }
        break;

    case MONS_LAVA_WORM:
    case MONS_LAVA_FISH:
    case MONS_LAVA_SNAKE:
    case MONS_SALAMANDER:
    case MONS_BIG_FISH:
    case MONS_GIANT_GOLDFISH:
    case MONS_ELECTRICAL_EEL:
    case MONS_JELLYFISH:
    case MONS_WATER_ELEMENTAL:
    case MONS_SWAMP_WORM:
        // XXX: We're being a bit player-centric here right now...
        // really we should replace the grid_distance() check
        // with one that checks for unaligned monsters as well. -- bwr
        if (mons_is_submerged( monster ))
        {
            if (grd[monster->x][monster->y] == DNGN_SHALLOW_WATER
                || grd[monster->x][monster->y] == DNGN_BLUE_FOUNTAIN
                || (!mons_friendly(monster)
                    && grid_distance( monster->x, monster->y,
                                      you.x_pos, you.y_pos ) == 1
                    && (monster->hit_points == monster->max_hit_points
                        || (monster->hit_points > monster->max_hit_points / 2
                            && coinflip()))))
            {
                mons_unsubmerge( monster );
            }
        }
        else if (monster_habitat(monster->type) == grd[monster->x][monster->y]
                 && (one_chance_in(5)
                     || (grid_distance( monster->x, monster->y,
                                        you.x_pos, you.y_pos ) > 1
                            && monster->type != MONS_ELECTRICAL_EEL
                            && monster->type != MONS_LAVA_SNAKE
                            && !one_chance_in(20))
                     || monster->hit_points <= monster->max_hit_points / 2)
                     || is_cloud( monster->x, monster->y ))
        {
            mons_submerge( monster );
        }
        break;

    case MONS_AIR_ELEMENTAL:
        if (one_chance_in(5))
            mons_submerge( monster );
        break;

    case MONS_PANDEMONIUM_DEMON:
        if (env.ghost.values[GVAL_DEMONLORD_CYCLE_COLOUR])
            monster->colour = element_colour( EC_RANDOM );
        break;
    }

    // currently, none of these cost the monster an action
    return (false);
}                               // end handle_nearby_ability()

//---------------------------------------------------------------
//
// handle_special_ability
//
// $$$ not sure what to say here...
//
//---------------------------------------------------------------
static bool handle_special_ability( struct monsters *monster, bolt & beem )
{
    bool used = false;

    const monster_type mclass = (mons_genus( monster->type ) == MONS_DRACONIAN)
                                    ? draco_subspecies( monster )
                                    : static_cast<monster_type>( monster->type );

    // FixedArray < unsigned int, 19, 19 > show;

    if (!mons_class_flag( mclass, M_SPECIAL_ABILITY )
        || !mons_near( monster )
        || monster->behaviour == BEH_SLEEP
        || mons_is_submerged( monster ))
    {
        return (false);
    }

    // losight(show, grd, you.x_pos, you.y_pos);

    switch (mclass)
    {
    default:
        break;

    case MONS_BALL_LIGHTNING:
        if (monster->attitude == ATT_HOSTILE
            && distance( you.x_pos, you.y_pos, monster->x, monster->y ) <= 5)
        {
            monster->hit_points = -1;
            used = true;
            break;
        }

        for (int i = 0; i < MAX_MONSTERS; i++)
        {
            struct monsters *targ = &menv[i];

            if (targ->type == -1 || targ->type == NON_MONSTER)
                continue;

            if (distance( monster->x, monster->y, targ->x, targ->y ) >= 5)
                continue;

            if (monster->attitude == targ->attitude)
                continue;

            // faking LOS by checking the neighbouring square
            int dx = targ->x - monster->x;
            if (dx)
                dx /= dx;

            int dy = targ->y - monster->y;
            if (dy)
                dy /= dy;

            const int tx = monster->x + dx;
            const int ty = monster->y + dy;

            if (!in_bounds( tx, ty ))
                continue;

            if (!grid_is_solid( grd[tx][ty] ))
            {
                monster->hit_points = -1;
                used = true;
                break;
            }
        }
        break;

    case MONS_LAVA_SNAKE:
        if (!mons_sound_mind_and_body( monster ))
            break;

        if (!mons_player_visible( monster ))
            break;

        if ((mmov_x || mmov_y) && coinflip())
            break;

        // setup tracer
        strcpy(beem.name, "glob of lava");
        beem.range = 8 + random2(5);
        beem.rangeMax = 8;
        beem.damage = dice_def( 3, 10 );
        beem.colour = RED;
        beem.type = SYM_ZAP;
        beem.flavour = BEAM_LAVA;
        beem.hit = 60;
        beem.beam_source = monster_index( monster );
        beem.thrower = KILL_MON;
        beem.aux_source = "glob of lava";

        // fire tracer
        fire_tracer(monster, beem);

        // good idea?
        if (mons_should_fire(beem))
        {
            mon_msg(monster, " spits lava!");
            fire_beam(beem);
            used = true;
        }
        break;

    case MONS_ELECTRICAL_EEL:
        if (!mons_sound_mind_and_body( monster ))
            break;

        if (!mons_player_visible( monster ))
            break;

        if ((mmov_x || mmov_y) && coinflip())
            break;

        // setup tracer
        strcpy(beem.name, "bolt of electricity");
        beem.damage = dice_def( 3, 6 );
        beem.colour = LIGHTCYAN;
        beem.type = SYM_ZAP;
        beem.flavour = BEAM_ELECTRICITY;
        beem.hit = 80;
        beem.beam_source = monster_index( monster );
        beem.thrower = KILL_MON;
        beem.aux_source = "bolt of electricity";
        beem.range = 8 + random2(5);
        beem.rangeMax = 8;
        beem.is_beam = true;
        beem.is_enchant = false;

        // fire tracer
        fire_tracer(monster, beem);

        // good idea?
        if (mons_should_fire(beem))
        {
            mon_msg(monster, " shoots out a bolt of electricity!");
            fire_beam(beem);
            used = true;
        }
        break;

    case MONS_ACID_BLOB:
    case MONS_OKLOB_PLANT:
    case MONS_YELLOW_DRACONIAN:
        if (!mons_sound_mind_and_body( monster ))
            break;

        if (!mons_player_visible( monster ))
            break;

        if (one_chance_in(3))
            used = plant_spit(monster, beem);

        break;

    case MONS_PIT_FIEND:
        if ((mmov_x || mmov_y) && one_chance_in(3))
            break;
        // deliberate fall through
    case MONS_FIEND:
        if (!mons_sound_mind_and_body( monster ))
            break;

        // friendly fiends won't use torment,  preferring hellfire
        // (right now there is no way a monster can predict how
        // badly they'll damage the player with torment) -- GDL
        if ((mmov_x || mmov_y) && one_chance_in(4))
        {
            int spell_cast;

            switch (random2(4))
            {
            case 0:
                if (!mons_friendly(monster))
                {
                    spell_cast = (you.is_undead) ? MS_DISPEL_UNDEAD : MS_TORMENT;
                    mons_cast(monster, beem, spell_cast);
                    used = true;
                    break;
                }
                // deliberate fallthrough -- see above
            case 1:
            case 2:
            case 3:
                spell_cast = MS_HELLFIRE;
                setup_mons_cast(monster, beem, spell_cast);

                // fire tracer
                fire_tracer(monster, beem);

                // good idea?
                if (mons_should_fire(beem))
                {
                    mon_msg( monster, " makes a gesture!", true, MSGCH_MONSTER_SPELL );

                    mons_cast(monster, beem, spell_cast);
                    used = true;
                }
                break;
            }

            mmov_x = 0;
            mmov_y = 0;
        }
        break;

    case MONS_IMP:
    case MONS_BLINK_IMP:
    case MONS_PHANTOM:
    case MONS_INSUBSTANTIAL_WISP:
    case MONS_BLINK_FROG:
    case MONS_KILLER_KLOWN:
        if ((mmov_x || mmov_y) && one_chance_in(7))
            monster_blink( monster );
        break;


    case MONS_SWARM_IMP:
        if (one_chance_in( 20 * (mons_friendly( monster ) ? 10 : 1) ))
        {
            mon_noisy( SL_SHRIEK, monster, " screams!" );
            create_monster( MONS_SWARM_IMP, SAME_ATTITUDE(monster), 2,
                            monster->x, monster->y, monster->foe, 200 );
        }
        break;


    case MONS_MANTICORE:
        if (!mons_player_visible( monster ))
            break;

        if (!mons_sound_mind_and_body( monster ))
            break;

        if (!mons_near(monster))
            break;

        // the fewer spikes the manticore has left,  the less
        // likely it will use them.
        if ((mmov_x || mmov_y)
            && random2(16) >= static_cast<int>( monster->number ))
        {
            break;
        }

        // do the throwing right here,  since the beam is so
        // easy to set up and doesn't involve inventory.

        // set up the beam
        strcpy(beem.name, "volley of spikes");
        beem.range = 8;
        beem.rangeMax = 8;
        beem.hit = 60;
        beem.damage = dice_def( 2, 10 );
        beem.beam_source = monster_index( monster );
        beem.type = SYM_MISSILE;
        beem.colour = LIGHTGREY;
        beem.flavour = BEAM_MISSILE;
        beem.thrower = KILL_MON;
        beem.aux_source = "volley of spikes";
        beem.is_beam = false;
        beem.is_enchant = false;

        // fire tracer
        fire_tracer(monster, beem);

        // good idea?
        if (mons_should_fire(beem))
        {
            mon_msg(monster, " flicks its tail!");
            fire_beam(beem);
            used = true;
            // decrement # of volleys left
            monster->number -= 1;
        }
        break;

    // dragon breath weapon:
    case MONS_HELL_HOUND:
    case MONS_FIRE_DRAKE:
    case MONS_LINDWURM:
    case MONS_DRAGON:
    case MONS_XTAHUA:
    case MONS_FROST_DRAKE:
    case MONS_ICE_DRAGON:
    case MONS_WHITE_DRACONIAN:
    case MONS_RED_DRACONIAN:
        if (!mons_player_visible( monster ))
            break;

        if (!mons_sound_mind_and_body( monster ))
            break;

        if ((!mmov_x && !mmov_y)
            || (mclass != MONS_HELL_HOUND && random2(13) < 3)
            || one_chance_in(10))
        {
            setup_dragon(monster, beem);

            // fire tracer
            fire_tracer(monster, beem);

            // good idea?
            if (mons_should_fire(beem))
            {
                mon_msg(monster, " breathes.");
                fire_beam(beem);
                mmov_x = 0;
                mmov_y = 0;
                used = true;
            }
        }
        break;
    }

    return (used);
}                               // end handle_special_ability()

//---------------------------------------------------------------
//
// handle_potion
//
// Give the monster a chance to quaff a potion. Returns true if
// the monster imbibed.
//
//---------------------------------------------------------------
static bool handle_potion(struct monsters *monster, bolt & beem)
{

    // yes, there is a logic to this ordering {dlb}:
    if (monster->behaviour == BEH_SLEEP)
        return (false);
    else if (monster->inv[MSLOT_POTION] == NON_ITEM)
        return (false);
    else if ((mmov_x || mmov_y) && !one_chance_in(3))
        return (false);
    else
    {
        const mon_holy_type holy = mons_holiness( monster );
        bool imbibed = false;

        switch (mitm[monster->inv[MSLOT_POTION]].sub_type)
        {
        case POT_HEALING:
        case POT_HEAL_WOUNDS:
            if (monster->hit_points <= monster->max_hit_points / 2
                && holy != MH_UNDEAD
                && holy != MH_NONLIVING
                && holy != MH_PLANT)
            {
                mon_msg(monster, " drinks a potion.");

                if (heal_monster( monster, 6 + roll_dice(2,6), false ))
                    mon_msg(monster, " is healed!");

                if (mitm[monster->inv[MSLOT_POTION]].sub_type
                                                    == POT_HEAL_WOUNDS)
                {
                    heal_monster( monster, 13 + roll_dice(2,13), false );
                }

                imbibed = true;
            }
            break;

        case POT_SPEED:
        case POT_INVISIBILITY:
            if (mitm[monster->inv[MSLOT_POTION]].sub_type == POT_INVISIBILITY)
            {
                // Don't waste the potion unless the player is nearby...
                // but we don't need to see 'em to drink this one
                if (grid_distance(monster->x, monster->y, you.x_pos, you.y_pos) > 6
                    || mons_has_ench( monster, ENCH_INVIS ))
                {
                    break;
                }

                beem.flavour = BEAM_INVISIBILITY;
            }
            else
            {
                // don't haste unless we're in sight of the player
                if (!mons_near( monster )
                    || mons_has_ench( monster, ENCH_HASTE ))
                {
                    break;
                }

                beem.flavour = BEAM_HASTE;
            }

            mon_msg( monster, " drinks a potion." );
            mons_ench_f2( monster, beem );
            imbibed = true;
            break;
        }

        if (imbibed)
        {
            if (dec_mitm_item_quantity( monster->inv[MSLOT_POTION], 1 ))
                monster->inv[MSLOT_POTION] = NON_ITEM;
        }

        return (imbibed);
    }
}                               // end handle_potion()

static bool handle_reaching(struct monsters *monster)
{
    bool       ret = false;
    const int  wpn = monster->inv[MSLOT_WEAPON];

    if (!mons_sound_mind_and_body( monster ))
        return (false);

    if (beings_aligned( monster_index(monster), monster->foe ))
        return (false);

    if (mons_is_submerged( monster ))
        return (false);

    if (wpn != NON_ITEM && get_weapon_brand( mitm[wpn] ) == SPWPN_REACHING )
    {
        if (monster->foe == MHITYOU)
        {
            // this check isn't redundant -- player may be invisible.
            if (monster->target_x == you.x_pos && monster->target_y == you.y_pos)
            {
                int dx = abs(monster->x - you.x_pos);
                int dy = abs(monster->y - you.y_pos);

                if ((dx == 2 && dy <= 2) || (dy == 2 && dx <= 2))
                {
                    ret = true;
                    monster_attack( monster_index(monster), MHITYOU );
                }
            }
        }
        else if (monster->foe != MHITNOT)
        {
            // same comments as to invisibility as above.
            if (monster->target_x == menv[monster->foe].x
                && monster->target_y == menv[monster->foe].y)
            {
                int dx = abs(monster->x - menv[monster->foe].x);
                int dy = abs(monster->y - menv[monster->foe].y);

                if ((dx == 2 && dy <= 2) || (dy == 2 && dx <= 2))
                {
                    ret = true;
                    monster_attack( monster_index(monster), monster->foe );
                }
            }
        }
    }

    return (ret);
}                               // end handle_reaching()

//---------------------------------------------------------------
//
// handle_scroll
//
// Give the monster a chance to read a scroll. Returns true if
// the monster read something.
//
//---------------------------------------------------------------
static bool handle_scroll(struct monsters *monster)
{
    const int scroll = monster->inv[MSLOT_SCROLL];
    bool read = false;

    // yes, there is a logic to this ordering {dlb}:
    if (!mons_sound_mind_and_body( monster )
        || monster->behaviour == BEH_SLEEP
        || mons_is_submerged( monster ))
    {
        return (false);
    }
    else if (scroll == NON_ITEM)
        return (false);
    else if (!mons_near(monster))
        return (false);
    else if ((mmov_x || mmov_y) && !one_chance_in(3))
        return (false);
    else
    {
        // notice how few cases are actually accounted for here {dlb}:
        switch (mitm[scroll].sub_type)
        {
        case SCR_TELEPORTATION:
        case SCR_BLINKING:
            if (mons_is_scared( monster )
                && !mons_has_ench( monster, ENCH_TELEPORT ))
            {
                mon_msg(monster, " reads a scroll.");
                read = true;

                if (mitm[scroll].sub_type == SCR_TELEPORTATION)
                    monster_teleport( monster, (coinflip() ? 1 : 2) );
                else
                {
                    if (!monster_blink( monster, true ))
                        read = false;
                }
            }
            break;

        case SCR_SUMMONING:
            mon_msg(monster, " reads a scroll.");
            create_monster( MONS_ABOMINATION_SMALL, SAME_ATTITUDE(monster), 2,
                            monster->x, monster->y, monster->foe, 250 );
            read = true;
            break;
        }

        if (read)
        {
            if (dec_mitm_item_quantity( monster->inv[MSLOT_SCROLL], 1 ))
                monster->inv[MSLOT_SCROLL] = NON_ITEM;
        }
    }

    return (read);
}                               // end handle_scroll()

//---------------------------------------------------------------
//
// handle_wand
//
// Give the monster a chance to zap a wand. Returns true if the
// monster zapped.
//
//---------------------------------------------------------------
static bool handle_wand( struct monsters *monster, bolt &beem )
{
    const int wand = monster->inv[MSLOT_WAND];

    // yes, there is a logic to this ordering {dlb}:
    if (monster->behaviour == BEH_SLEEP
        || mons_is_submerged( monster )
        || wand == NON_ITEM
        || mitm[wand].plus <= 0
        || (!mons_near(monster) && mitm[wand].sub_type != WAND_DIGGING)
        || ((mmov_x || mmov_y) && coinflip()))
    {
        return (false);
    }

    if (mitm[wand].sub_type == WAND_RANDOM_EFFECTS
        && mons_intel(monster->type) > I_NORMAL
        && (mons_has_ranged_spell( monster )
            || mons_has_ranged_attack( monster )))
    {
        return (false);
    }

    bool niceWand = false;
    bool zap = false;

    // map wand type to monster spell type
    int mzap = map_wand_to_mspell( mitm[wand].sub_type );
    if (mzap == 0)
        return (false);

    // set up the beam
    int power = 30 + 2 * monster->hit_dice;
    mons_spells( mzap, power, beem );

    // XXX: ugly hack this:
    static char wand_buff[ ITEMNAME_SIZE ];

    beem.beam_source = monster_index( monster );
    beem.source_x = monster->x;
    beem.source_y = monster->y;

    item_def item = mitm[wand]; // make copy so we can record name

#if HISCORE_WEAPON_DETAIL
    set_ident_flags( item, ISFLAG_IDENT_MASK );
#else
    unset_ident_flags( item, ISFLAG_IDENT_MASK );
    set_ident_flags( item, ISFLAG_KNOW_TYPE );
#endif

    item_name( item, DESC_PLAIN, wand_buff );

    beem.aux_source = wand_buff;

    const bool low_hp = (monster->hit_points <= monster->max_hit_points / 2
                        || (monster->hit_points < 10
                            && monster->max_hit_points >= 10));

    switch (mitm[wand].sub_type)
    {
    case WAND_DIGGING:
        // make sure we have a valid target we can't see:
        if (monster->foe != MHITNOT
            && in_bounds( monster->target_x, monster->target_y )
            && !check_line_of_sight( monster->x, monster->y,
                                     monster->target_x, monster->target_y ))
        {
            beem.target_x = monster->target_x;
            beem.target_y = monster->target_y;

            niceWand = true;
            break;
        }
        return (false);

    // these are wands that monsters will aim at themselves {dlb}:
    case WAND_HASTING:
        if (!mons_has_ench(monster, ENCH_HASTE))
        {
            beem.target_x = monster->x;
            beem.target_y = monster->y;

            niceWand = true;
            break;
        }
        return (false);

    case WAND_HEALING:
        if (low_hp)
        {
            beem.target_x = monster->x;
            beem.target_y = monster->y;

            niceWand = true;
            break;
        }
        return (false);

    case WAND_INVISIBILITY:
        if (!mons_is_invisible( monster ))
        {
            beem.target_x = monster->x;
            beem.target_y = monster->y;

            niceWand = true;
            break;
        }
        return (false);

    case WAND_TELEPORTATION:
        if (low_hp)
        {
            if (!mons_has_ench( monster, ENCH_TELEPORT )
                && !one_chance_in(20))
            {
                beem.target_x = monster->x;
                beem.target_y = monster->y;

                niceWand = true;
                break;
            }
            // this break causes the wand to be tried on the player:
            break;
        }
        return (false);
    }

    // fire tracer, if necessary
    if (!niceWand)
    {
        fire_tracer( monster, beem );
        zap = mons_should_fire( beem );
    }

    if (niceWand || zap)
    {
        if (!mon_msg(monster, " zaps a wand.")
            && player_can_hear( monster->x, monster->y ))
        {
            mpr(MSGCH_SOUND,"You hear a zap." );
        }

        // charge expenditure {dlb}
        mitm[monster->inv[MSLOT_WAND]].plus--;
        beem.is_tracer = false;
        fire_beam( beem );

        return (true);
    }

    return (false);
}                               // end handle_wand()

static int get_draconian_breath_spell( struct monsters *monster )
{
    int draco_breath = MS_NO_SPELL;

    if (mons_genus( monster->type ) == MONS_DRACONIAN)
    {
        switch (draco_subspecies( monster ))
        {
        case MONS_BLACK_DRACONIAN:
            draco_breath = MS_LIGHTNING_BOLT;
            break;

        case MONS_PALE_DRACONIAN:
            draco_breath = MS_STEAM_BALL;
            break;

        case MONS_GREEN_DRACONIAN:
            draco_breath = MS_POISON_BLAST;
            break;

        case MONS_PURPLE_DRACONIAN:
            draco_breath = MS_ORB_ENERGY;
            break;

        case MONS_MOTTLED_DRACONIAN:
            draco_breath = MS_STICKY_FLAME;
            break;

        case MONS_DRACONIAN:
        case MONS_YELLOW_DRACONIAN:     // already handled as ability
        case MONS_RED_DRACONIAN:        // already handled as ability
        case MONS_WHITE_DRACONIAN:      // already handled as ability
        default:
            break;
        }
    }

    return (draco_breath);
}

//---------------------------------------------------------------
//
// handle_spell
//
// Give the monster a chance to cast a spell. Returns true if
// a spell was cast.
//
//---------------------------------------------------------------
static bool handle_spell( struct monsters *monster, bolt & beem )
{
    const bool monsterNearby = mons_near(monster);
    const bool is_friendly = mons_friendly( monster );
    const bool isTemp = (mons_has_ench( monster, ENCH_CHARM )
                        || mons_is_summoned( monster ));

    const int draco_breath = get_draconian_breath_spell( monster );

    bool finalAnswer = false;   // as in: "Is that your...?" {dlb}

    // yes, there is a logic to this ordering {dlb}:
    if (monster->behaviour == BEH_SLEEP
        || (!mons_class_flag( monster->type, M_SPELLCASTER )
            && draco_breath == MS_NO_SPELL)
        || mons_is_submerged( monster ))
    {
        return (false);
    }

    if ((mons_class_flag( monster->type, M_ACTUAL_SPELLS )
            || mons_class_flag( monster->type, M_PRIEST ))
        && mons_is_shapeshifter( monster ))
    {
        //jmf: shapeshiftes don't get spells, just physical powers.
        return (false);
    }
    else if (!mons_sound_mind_and_body( monster ))
    {
        return (false);
    }
    else if (monster->type == MONS_PANDEMONIUM_DEMON
            && !env.ghost.values[ GVAL_DEMONLORD_SPELLCASTER ])
    {
        return (false);
    }
    else if ((mmov_x || mmov_y)
            && random2(200) > monster->hit_dice + 50 * (!is_friendly))
    {
        return (false);
    }

    int spell_cast = draco_breath;
    int spells[ NUM_MONSTER_SPELL_SLOTS ];  // local temp copy

    // remove healing/invis/haste if we don't need them
    int num_no_spell = 0;

    for (int i = 0; i < NUM_MONSTER_SPELL_SLOTS; i++)
    {
        // copy from monster to temporary and clobber if needed:
        spells[i] = monster->spells[i];

        // any conversions:
        // XXX: Eventually these should be pulled into a function, perhaps
        // with some additional ones that do things like turn sting into
        // poison arrow for high level monsters... which would allow for
        // the same template to cover a wider spread.
        if (!monsterNearby && spells[i] == MS_CONTROLLED_BLINK)
            spells[i] = MS_TELEPORT;

        if (you.is_undead && spells[i] == MS_TORMENT)
        {
            if (mons_holiness( monster ) != MH_UNDEAD)
                spells[i] = MS_DISPEL_UNDEAD;
            else // skull/mummy
                spells[i] = MS_SUMMON_UNDEAD;
        }

        // check if it's worth anything:
        if (ms_waste_of_time( monster, spells[i] ))
            spells[i] = MS_NO_SPELL;

        // Temporary friends don't summon additional help... this is a
        // quick solution to solve the problem that we don't track who
        // owns each servant.
        //
        // XXX: Eventually, it might be nice to do that... it would also
        // help with deciding when to cast summoning spells since we could
        // limit each monster to a certain number of pets.  A similar
        // limitation might be useful for the PC as well, in order to keep
        // the size of hordes in check.
        if (isTemp
            && (spells[i] == MS_ANIMATE_DEAD || ms_summon_spell( spells[i] )))
        {
            spells[i] = MS_NO_SPELL;
        }

        // count the "no spells":
        if (spells[i] == MS_NO_SPELL)
            num_no_spell++;
    }

    // If no useful spells... cast no spell.
    if (num_no_spell == NUM_MONSTER_SPELL_SLOTS)
    {
        if (draco_breath == MS_NO_SPELL)
            return (false);
        else
        {
            spell_cast = draco_breath;
            finalAnswer = true;
        }
    }

    if (!finalAnswer && !monsterNearby)
    {
        // forces the casting of dig when player not visible - this is EVIL!
        if (spells[4] == MS_DIG && monster->behaviour == BEH_SEEK)
        {
            spell_cast = MS_DIG;
            setup_mons_cast( monster, beem, spell_cast );
            finalAnswer = true;
        }
        else if ((spells[2] == MS_HEAL || spells[5] == MS_HEAL)
                    && monster->hit_points < monster->max_hit_points)
        {
            // The player's out of sight!
            // Quick, let's take a turn to heal ourselves. -- bwr
            spell_cast = MS_HEAL;
            finalAnswer = true;
        }
        else if (mons_is_scared( monster ))
        {
            // Since the player isn't around, we'll extend the monster's
            // normal fleeing choices to include the self-enchant slot.
            if (ms_useful_fleeing_out_of_sight( monster, spells[5] ))
            {
                spell_cast = spells[5];
                finalAnswer = true;
            }
            else if (ms_useful_fleeing_out_of_sight( monster, spells[2] ))
            {
                spell_cast = spells[2];
                finalAnswer = true;
            }
#if DEBUG_DIAGNOSTICS
            if (finalAnswer)
            {
                mpr( MSGCH_DIAGNOSTICS, "%s casts %s fleeing out of sight.",
                      monam(monster->number, monster->type, true, DESC_CAP_THE),
                      mons_spell_name(spell_cast) );
            }
#endif
        }
        else if (monster->foe == MHITYOU)
        {
            return (false);
        }
    }

    // Promote the casting of useful spells for low-HP monsters.
    if (!finalAnswer
        && monster->hit_points < monster->max_hit_points / 3
        && !one_chance_in(5))
    {
        // Note: There should always be at least some chance we don't
        // get here... even if the monster is on it's last HP.  That
        // way we don't have to worry about monsters infinitely casting
        // Healing on themselves (ie orc priests).
        if (mons_is_scared( monster )
            && ms_low_hitpoint_cast( monster, spells[5] ))
        {
            spell_cast = spells[5];
            finalAnswer = true;
        }
        else if (ms_low_hitpoint_cast( monster, spells[2] ))
        {
            spell_cast = spells[2];
            finalAnswer = true;
        }
#if DEBUG_DIAGNOSTICS
        if (finalAnswer)
        {
            mpr( MSGCH_DIAGNOSTICS, "%s casts %s at low hit points.",
                  monam(monster->number, monster->type, true, DESC_CAP_THE),
                  mons_spell_name(spell_cast) );
        }
#endif
    }

    if (!finalAnswer)
    {
        // should monster not have selected dig by now, it never will:
        if (spells[4] == MS_DIG)
            spells[4] = MS_NO_SPELL;

        // up to four tries to pick a spell.
        for (int loopy = 0; loopy < 4; loopy ++)
        {
            bool spellOK = false;

            // setup spell - fleeing monsters will always try to
            // choose their emergency spells.  Others choose randomly.
            if (mons_is_scared( monster ))
                spell_cast = (one_chance_in(5) ? spells[2] : spells[5]);
            else
                spell_cast = spells[ random2(5) ];

            if (spell_cast == MS_NO_SPELL)
                continue;

            // setup the spell
            setup_mons_cast( monster, beem, spell_cast );

            // beam-type spells requiring tracers
            if (ms_requires_tracer( spell_cast ))
            {
                // Friendly monsters with targets or nearby hostiles
                // can use range spells.
                if ((is_friendly
                        && monster->foe != MHITYOU
                        && monster->foe != MHITNOT)
                    || (!is_friendly
                        && mons_near(monster)))
                {
                    fire_tracer(monster, beem);

                    if (mons_should_fire(beem))
                        spellOK = true;

#if DEBUG_DIAGNOSTICS
                    mpr( MSGCH_DIAGNOSTICS, "%s: %sOKAY",
                          mons_spell_name( spell_cast ),
                          spellOK ? "": "NOT " );
#endif
                }
            }
            else
            {
                // all direct-effect/summoning/self-enchantments/etc
                spellOK = true;

                if (ms_direct_nasty( spell_cast )
                    && beings_aligned( monster_index(monster), monster->foe ))
                {
                    spellOK = false;
                }
                else if (monster->foe == MHITYOU || monster->foe == MHITNOT)
                {
                    // XXX: Note the crude hack so that monsters can
                    // use ME_ALERT to target (we should really have
                    // a measure of time instead of peeking to see
                    // if the player is still there). -- bwr
                    if (!mons_player_visible( monster )
                        && (monster->target_x != you.x_pos
                            || monster->target_y != you.y_pos
                            || coinflip()))
                    {
                        spellOK = false;
                    }
                }
                else if (!mons_monster_visible( monster,&menv[monster->foe] ))
                {
                    spellOK = false;
                }
                else if (!mons_line_of_sight( monster, menv[monster->foe].x,
                                                    menv[monster->foe].y ))
                {
                    spellOK = false;
                }
                else
                {
                    const int holy = mons_holiness( &menv[monster->foe] );
                    const int intel = mons_intel( menv[monster->foe].type );
                    const int anim = mons_is_animated_undead(menv[monster->foe].type, true);

                    if ((spell_cast == MS_BRAIN_FEED && intel == I_PLANT)
                        || (spell_cast == MS_MUTATION && holy != MH_NATURAL && !anim))
                    {
                        spellOK = false;
                    }
                }
            }

            // if not okay, then maybe we'll cast a defensive spell
            if (!spellOK)
                spell_cast = (coinflip() ? spells[2] : MS_NO_SPELL);

            if (spell_cast != MS_NO_SPELL)
                break;
        }
    }

    // should the monster *still* not have a spell, well, too bad {dlb}:
    if (spell_cast == MS_NO_SPELL)
        return (false);

    // Try to animate dead: if nothing rises, pretend we didn't cast it
    if (spell_cast == MS_ANIMATE_DEAD
        && !animate_dead( 100, monster->x, monster->y, SAME_ATTITUDE(monster),
                          monster->foe, 0 ))
    {
        return (false);
    }

    if (monsterNearby)      // handle monsters within range of player
    {
        if (monster->type == MONS_GERYON)
        {
            if (silenced( monster->x, monster->y ))
                return (false);

            mon_noisy( SL_HORN, monster, " winds a great silver horn." );
        }
        else if (mons_is_demon( monster->type ))
        {
            mon_msg( monster, " gestures.", true, MSGCH_MONSTER_SPELL );
        }
        else if (spell_cast == draco_breath)
        {
            mon_noisy( SL_ROAR, monster );

            if (!mon_msg( monster, " breathes.", true, MSGCH_MONSTER_SPELL )
                && player_can_hear( monster->x, monster->y ))
            {
                mpr( MSGCH_SOUND,"You hear a roar." );
            }
        }
        else
        {
            switch (monster->type)
            {
            default:
                if (silenced( monster->x, monster->y ))
                    return (false);

                if (mons_class_flag(monster->type, M_PRIEST))
                {
                    switch (random2(3))
                    {
                    case 0:
                        mon_msg( monster, " prays.", true, MSGCH_MONSTER_SPELL );
                        break;
                    case 1:
                        mon_msg( monster, " mumbles some strange prayers.", true,
                                            MSGCH_MONSTER_SPELL );
                        break;
                    case 2:
                    default:
                        mon_msg( monster, " utters an invocation.", true,
                                            MSGCH_MONSTER_SPELL );
                        break;
                    }
                }
                else // spellcaster
                {
                    switch (random2(3))
                    {
                    case 0:
                        // XXX: could be better, chosen to match the
                        // ones in monspeak.cc... has the problem
                        // that it doesn't suggest a vocal component. -- bwr
                        if (player_monster_visible( monster ))
                        {
                            mon_msg( monster, " gestures wildly.", true,
                                                    MSGCH_MONSTER_SPELL );
                            break;
                        }
                        // intentional fall-through
                    case 1:
                        mon_msg( monster, " mumbles some strange words.", true,
                                                MSGCH_MONSTER_SPELL );
                        break;
                    case 2:
                    default:
                        mon_msg( monster, " casts a spell.", true,
                                                MSGCH_MONSTER_SPELL );
                        break;
                    }
                }
                break;

            case MONS_NORRIS:
                // Psionic type messages... no sound, only sight
                if (player_monster_visible( monster ))
                {
                    switch (random2(3))
                    {
                    case 0:
                        mon_msg( monster, " concentrates for a moment.", true,
                                                MSGCH_MONSTER_SPELL );
                        break;
                    case 1:
                        mon_msg( monster, " gestures wildly.", true,
                                                MSGCH_MONSTER_SPELL );
                        break;
                    case 2:
                    default:
                        mon_msg( monster, " focuses his mind.", true,
                                                MSGCH_MONSTER_SPELL );
                        break;
                    }
                }
                break;

            case MONS_BALL_LIGHTNING:
                monster->hit_points = -1;
                break;

            case MONS_STEAM_DRAGON:
            case MONS_MOTTLED_DRAGON:
            case MONS_SPARK_DRAKE:
            case MONS_STORM_DRAGON:
            case MONS_GOLDEN_DRAGON:
            case MONS_SHADOW_DRAGON:
            case MONS_SWAMP_DRAGON:
            case MONS_SWAMP_DRAKE:
            case MONS_DEATH_DRAKE:
            case MONS_HELL_HOG:
            case MONS_SERPENT_OF_HELL:
            case MONS_QUICKSILVER_DRAGON:
            case MONS_IRON_DRAGON:
                mon_noisy( SL_ROAR, monster );

                if (!mon_msg( monster, " breathes.", true, MSGCH_MONSTER_SPELL )
                    && player_can_hear( monster->x, monster->y ))
                {
                    mpr( MSGCH_SOUND,"You hear a roar." );
                }
                break;

            case MONS_BRAIN_WORM:
            case MONS_ELECTRIC_GOLEM:
                // These don't show any signs that they're casting a spell.
                break;

            case MONS_KILLER_KLOWN:
                mons_speaks( monster );
                break;

            case MONS_GREAT_ORB_OF_EYES:
            case MONS_SHINING_EYE:
            case MONS_EYE_OF_DEVASTATION:
                mon_msg(monster, " gazes.", true, MSGCH_MONSTER_SPELL);
                break;

            case MONS_GIANT_ORANGE_BRAIN:
                mon_msg(monster, " pulsates.", true, MSGCH_MONSTER_SPELL);
                break;

            case MONS_NAGA:
            case MONS_NAGA_WARRIOR:
                mon_msg(monster, " spits poison.", true, MSGCH_MONSTER_SPELL);
                break;
            }
        }
    }
    else                    // handle far-away monsters
    {
        if (monster->type == MONS_GERYON
            && player_can_hear( monster->x, monster->y ))
        {
            mpr(MSGCH_SOUND,"You hear a weird and mournful sound." );
        }
    }

    // FINALLY! determine primary spell effects {dlb}:
    if (spell_cast == MS_BLINK || spell_cast == MS_CONTROLLED_BLINK)
    {
        // why only cast blink if nearby? {dlb}
        // -- probably from when monster blink used the player's LOS...
        //    left in because it's only interesting to do when the player
        //    is around anyways (so let the attempt to cast a spell fail
        //    and have the monster continue looking for something better
        //    to do) {bwr}
        if (!monsterNearby)
            return (false);

        bool succ = monster_blink(monster, (spell_cast == MS_CONTROLLED_BLINK));

        if (!succ && spell_cast == MS_CONTROLLED_BLINK)
        {
            // panic controlled blink to teleport fall back
            if (mons_is_scared( monster ))
            {
#if DEBUG_DIAGNOSTICS
                mpr( MSGCH_DIAGNOSTICS, "controlled blink -> teleport" );
#endif
                monster_teleport( monster, 1 );
                return (true);
            }

            // fall back
            succ = monster_blink( monster );
        }

        return (succ);
    }
    else
    {
        mons_cast(monster, beem, spell_cast);
        mmov_x = 0;
        mmov_y = 0;
    }

    return (true);
}                               // end handle_spell()

//---------------------------------------------------------------
//
// handle_throw
//
// Give the monster a chance to throw something. Returns true if
// the monster hurled.
//
//---------------------------------------------------------------
static bool handle_throw( struct monsters *mon, bolt & beem )
{
    // yes, there is a logic to this ordering {dlb}:
    if (!mons_sound_mind_and_body( mon )
        || mon->behaviour == BEH_SLEEP
        || mons_is_submerged( mon ))
    {
        return (false);
    }

    if (mons_itemuse(mon->type) < MONUSE_OPEN_DOORS)
        return (false);

    const int ammo = mon->inv[MSLOT_MISSILE];
    if (ammo == NON_ITEM || !is_valid_item( mitm[ammo] ))
        return (false);

    // don't allow offscreen throwing.. for now.
    if (mon->foe == MHITYOU && !mons_near(mon))
        return (false);

    // poor 2-headed ogres {dlb}
    if (mon->type == MONS_TWO_HEADED_OGRE || mon->type == MONS_ETTIN)
        return (false);

    // recent addition {GDL} - monsters won't throw if they can do melee.
    // wastes valuable ammo,  and most monsters are better at melee anyway.
    if (adjacent( beem.target_x, beem.target_y, mon->x, mon->y ))
        return (false);

    if ((mmov_x || mmov_y) && one_chance_in(5))
        return (false);

    const launch_retval launched = is_launched( monster_index(mon), mitm[ammo] );

    if (launched == LRET_FUMBLED)
        return (false);

    beem.is_launched = (launched == LRET_LAUNCHED);

    // ok, we'll try it.
    setup_generic_throw( mon, beem );

    // fire tracer
    fire_tracer( mon, beem );

    // good idea?
    if (mons_should_fire( beem ))
    {
        beem.name[0] = '\0';
        return (mons_throw( mon, beem, ammo ));
    }

    return (false);
}                               // end handle_throw()

// functor for the priority queue in handle_monsters
struct mon_energy_cmp
{
    bool operator() ( struct monsters *const m1, struct monsters *const m2 )
    {
        return (m1->energy < m2->energy
                || (m1->energy == m2->energy && m1->wait > m2->wait));
    }
};

//---------------------------------------------------------------
//
// handle_monsters
//
// This is the routine that controls monster AI.
//
//---------------------------------------------------------------
void handle_monsters(void)
{
    int i;

    // FixedArray < unsigned int, 19, 19 > show;
    // losight(show, grd, you.x_pos, you.y_pos);

    std::priority_queue< struct monsters *, std::vector< struct monsters * >,
                         mon_energy_cmp >  action_queue;

    // Go through all the monsters updating their status, handing out energy,
    // and queueing them up for actions.  Do things here that apply for each
    // turn (not each of it's possible many actions (or lack thereof)).
    for (i = 0; i < MAX_MONSTERS; i++)
    {
        struct monsters *const  monster = &menv[i];

        if (monster->type == -1)
            continue;

        if (monster->hit_points > monster->max_hit_points)
            monster->hit_points = monster->max_hit_points;

        const int  speed = mons_speed( monster );
        const bool can_move = (!mons_has_ench( monster, ENCH_PETRIFY )
                                && !mons_has_ench( monster, ENCH_PARALYSIS ));

        monster->wait = 0;

        if (can_move)
            monster->energy += div_rand_round( speed * you.time_taken, 10 );

        // memory is decremented here for a reason -- we only want it
        // decrementing once per monster "move"
        if (monster->foe_memory > 0)
            monster->foe_memory--;

        // otherwise there are potential problems with summonings
        if (monster->type == MONS_SHAPESHIFTER)
            set_mons_flag( monster, MF_SHAPESHIFT );
        else if (monster->type == MONS_GLOWING_SHAPESHIFTER)
            set_mons_flag( monster, MF_GLOW_SHAPESHIFT );

        // We reset batty monsters from wander to seek here, instead
        // of in handle_behaviour() since that will be called with
        // every single movement, and we want these monsters to
        // hit and run. -- bwr
        if (monster->foe != MHITNOT
            && monster->behaviour == BEH_WANDER
            && mons_is_batty( monster ))
        {
            monster->behaviour = BEH_SEEK;
        }

        // shift colour of elemental shifters
        if (mons_class_flag( monster->type, M_COLOUR_SHIFT ))
            monster->colour = mons_class_colour( monster->type );

        if (handle_enchantment( monster, you.time_taken ))
            continue;

        if (monster->type != -1 && monster->hit_points < 1)
        {
            if (monster->type == MONS_GIANT_SPORE
                || monster->type == MONS_BALL_LIGHTNING)
            {
                // detach monster from the grid first, so it
                // doesn't get hit by its own explosion (GDL)
                mgrd[monster->x][monster->y] = NON_MONSTER;

                spore_goes_pop( monster );
                monster_cleanup( monster );
                continue;
            }
            else
            {
                monster_die( monster, KILL_MISC, 0 );
                continue;
            }
        }

        // HP regeneration:
        const int cloud = get_cloud_type( monster->x, monster->y );

        int amount = 0;
        if (mons_class_flag( monster->type, M_REGEN )

            || (monster->type == MONS_FIRE_ELEMENTAL
                && (grd[monster->x][monster->y] == DNGN_LAVA
                    || cloud == CLOUD_FIRE
                    || cloud == CLOUD_FIRE_MON))

            || (monster->type == MONS_WATER_ELEMENTAL
                && (grd[monster->x][monster->y] == DNGN_SHALLOW_WATER
                    || grd[monster->x][monster->y] == DNGN_DEEP_WATER)))
        {
            // regeneration... 1 per 10 ticks.
            amount = div_rand_round( you.time_taken, 10 );
        }
        else if (random2(1000) < mod_time(100))
        {
            // regular healing, 1 in 10 chance of 1 HP over 10 ticks.
            amount = 1;
        }

        if (amount)
            heal_monster( monster, amount, false );

        // Those marked as taking the stairs pay for that now... any
        // excess can still be used to do other things.  -- bwr
        if (testbits( monster->flags, MF_TAKING_STAIRS ))
            monster->energy -= 10;

        if (monster->energy >= ENERGY_THRESHOLD && can_move)
            action_queue.push( monster );
    }

    // Actual action loop... do things here that apply everytime a monster gets
    // a real move.
    //
    // In this loop we goto next_action if the monster is still viable and did
    // something, in order to decide if the monster gets a second action in the
    // time alloted.
    //
    // If the monster becomes ineligable for queuing (ie it dies), then we
    // simply continue this loop and get the next monster.

    while (!action_queue.empty())
    {
        struct monsters *const  monster = action_queue.top();
        const int index = monster_index( monster );
        action_queue.pop();

#if (DEBUG_DIAGNOSTICS > 1)
        mpr( MSGCH_DIAGNOSTICS, "Moving %s (#%d), energy = %d",
              ptr_monam( monster, DESC_PLAIN ), index, monster->energy );
#endif

        // in case the monster died while waiting:
        if (monster->type == -1 || monster->hit_points < 1)
            continue;

        if (mons_flag( monster, MF_SHORT_LIVED ) && one_chance_in(3))
            monster->hit_points = -1;

        // monster regains opportunity to be swapped every action
        monster->flags &= ~MF_SWAPPED;

        // clear movement globals
        mmov_x = 0;
        mmov_y = 0;

        // Like the player, we're carrying over the blocking time
        // penalty to the cost of the next action.
        // monster->energy -= (10 + 2 * monster->shield_blocks);
        const unsigned int start_energy = monster->energy;
        monster->energy -= 10;
        monster->shield_blocks = 0;

        // Since clouds might be moved in and out of during the turn,
        // we handle them on an action by action basis.
        if (is_cloud( monster->x, monster->y ) && !mons_is_submerged( monster ))
        {
            // submerging monsters will hide from clouds
            if (mons_can_submerge( monster, grd[monster->x][monster->y] ))
                mons_submerge( monster );
            else
                mons_in_cloud( monster );
        }

        if (monster->type == -1)
            continue;

        handle_behaviour( monster );

        if (mons_is_animated_undead( monster->type, true ))
            monster->max_hit_points = monster->hit_points;

        if (igrd[monster->x][monster->y] != NON_ITEM)
        {
            const int monuse = mons_itemuse( monster->type );

            if (monuse >= MONUSE_WEAPONS_ARMOUR
                || monuse == MONUSE_EATS_ITEMS
                || mons_likes_carrion( monster ))
            {
                if (handle_pickup(monster))
                    goto next_action;
            }
        }

        if (mons_is_confused( monster ))
        {
            mmov_x = random2(3) - 1;
            mmov_y = random2(3) - 1;

            // bounds check: don't let confused monsters try to run
            // off the map
            if (monster->target_x + mmov_x <= X_BOUND_1
                || monster->target_x + mmov_x >= X_BOUND_2)
            {
                mmov_x = 0;
            }

            if (monster->target_y + mmov_y <= Y_BOUND_1
                || monster->target_y + mmov_y >= Y_BOUND_2)
            {
                mmov_y = 0;
            }

            const int targ = mgrd[monster->x + mmov_x][monster->y + mmov_y];

            if (targ != NON_MONSTER && (mmov_x != 0 || mmov_y != 0))
            {
                mmov_x = 0;
                mmov_y = 0;

                if (monster_attack( index, targ ))
                    goto next_action;
            }
        }
        else
        {
            // calculates mmov_x, mmov_y based on monster target.
            calc_movement( monster, true );
        }

        // These include things like submerging, and thus are handled outside
        // of the lower block... they also are all currently free actions, but
        // we check anyways.
        if (handle_nearby_ability( monster ))
            goto next_action;

        if (monster->behaviour != BEH_SLEEP
            && monster->behaviour != BEH_WANDER
            && monster->behaviour != BEH_INVESTIGATE
            && !mons_is_submerged( monster ))
        {
            struct bolt beem;

            beem.target_x = monster->target_x;
            beem.target_y = monster->target_y;

            if (handle_potion( monster, beem ))
                goto next_action;

            if (handle_scroll( monster ))
                goto next_action;

            if (handle_special_ability( monster, beem ))
                goto next_action;

            if (handle_spell( monster, beem ))
                goto next_action;

            if (handle_wand( monster, beem ))
                goto next_action;

            if (handle_reaching( monster ))
                goto next_action;

            if (handle_throw( monster, beem ))
                goto next_action;
        }

        // moving attacking stuff (so we better have a direction in mind)
        if (mmov_x || mmov_y)
        {
            // see if we move into (and fight) an unfriendly monster
            int targmon = mgrd[ monster->x + mmov_x ][ monster->y + mmov_y ];

            // do they fight?
            if (targmon != NON_MONSTER && monster_attack( index, targmon ))
            {
                if (mons_is_batty( monster ))
                {
                    monster->behaviour = BEH_WANDER;
                    set_wander_target( monster );
                }

                mmov_x = 0;
                mmov_y = 0;

                goto next_action;
            }

            // If player can't see monster, we pass on the attack
            // for now and let the monster move function handle that.
            if ((monster->x + mmov_x == you.x_pos
                    && monster->y + mmov_y == you.y_pos)
                && player_monster_visible( monster )
                && !mons_friendly( monster ))
            {
                monster_attack( index, MHITYOU );
                mmov_x = 0;
                mmov_y = 0;

                if (mons_is_batty( monster ))
                {
                    monster->behaviour = BEH_WANDER;
                    set_wander_target( monster );
                }

                if ((monster->type == MONS_GIANT_SPORE
                        || monster->type == MONS_BALL_LIGHTNING)
                    && monster->hit_points < 1)
                {
                    // detach monster from the grid first, so it
                    // doesn't get hit by its own explosion (GDL)
                    mgrd[monster->x][monster->y] = NON_MONSTER;

                    spore_goes_pop(monster);
                    monster_cleanup(monster);
                    continue;
                }

                goto next_action;
            }

            if (monster->type == -1)
                continue;

            if (handle_monster_move( monster ))
                goto next_action;

            // END OF MONSTER ATTEMPTS TO DO SOMETHING

            // Monster hasn't done anything yet... so in the case of the
            // monster being (potentially) temporarily blocked by another,
            // we allow it to hold until that monster gets its move (thus
            // a chain of same speed monsters in a hallway should adjust
            // their priorities to make things smoother in the future).
            targmon = mgrd[ monster->x + mmov_x ][ monster->y + mmov_y ];

            if (!mons_is_scared( monster )                  // never scared
                && targmon != NON_MONSTER                   // monster blocking
                && menv[targmon].energy >= ENERGY_THRESHOLD // has action
                && monster->energy <= menv[targmon].energy  // spent to lower
                && start_energy > menv[targmon].energy      // started higher
                && monster->wait < 10)                      // paranoia
            {
#if (DEBUG_DIAGNOSTICS > 1)
                mpr( MSGCH_DIAGNOSTICS,
                      "monster holding: energy %d, wait for %d",
                      monster->energy, menv[targmon].energy );
#endif

                monster->energy = menv[targmon].energy;
                monster->wait = menv[targmon].wait + 1;
            }
        }

next_action:

        if (monster->type == -1)
            continue;

        if (monster->hit_points < 1)
        {
            if (monster->type == MONS_GIANT_SPORE
                || monster->type == MONS_BALL_LIGHTNING)
            {
                // detach monster from the grid first, so it
                // doesn't get hit by its own explosion (GDL)
                mgrd[monster->x][monster->y] = NON_MONSTER;

                spore_goes_pop( monster );
                monster_cleanup( monster );
                continue;
            }
            else
            {
                monster_die( monster, KILL_MISC, 0 );
                continue;
            }
        }

        // reevaluate behaviour, since the monster's
        // surroundings have changed (it may have moved,
        // or died for that matter.  Don't bother for
        // dead monsters.  :)
        handle_behaviour( monster );

        // if monster still has enough energy, requeue
        if (monster->energy >= ENERGY_THRESHOLD)
            action_queue.push( monster );
    }
}                               // end handle_monster()

// Note: Assumes everything is okay with this as far as eligibility for
// the swap and terrain issues.
void swap_monsters_positions( int m1_id, int m2_id )
{
    ASSERT( m1_id != m2_id );

    struct monsters *const mon1 = &menv[m1_id];
    struct monsters *const mon2 = &menv[m2_id];

    const int tmp_x = mon2->x;
    const int tmp_y = mon2->y;

    mon2->x = mon1->x;
    mon2->y = mon1->y;

    mon1->x = tmp_x;
    mon1->y = tmp_y;

    mgrd[mon1->x][mon1->y] = m1_id;
    mgrd[mon2->x][mon2->y] = m2_id;

    mon1->flags |= MF_SWAPPED;
    mon2->flags |= MF_SWAPPED;
}

// Note: Doesn't do terrain checking like the player version, just handles
// the updating of the structures and some ASSERT checks.
void move_monster_to_grid( int mon_id, int x, int y )
{
    struct monsters *const mon = &menv[mon_id];

    ASSERT( mgrd[x][y] == NON_MONSTER );
    ASSERT( in_bounds( x, y ) );

    // Assuming that if mon->x is 0, we're moving the monster onto
    // the playing grid... otherwise it's a point to point move and
    // so we have to clear the old grid.
    if (mon->x)
        mgrd[ mon->x ][ mon->y ] = NON_MONSTER;

    mgrd[ x ][ y ] = mon_id;

    mon->x = x;
    mon->y = y;
}

static bool mons_likes_weapon( struct monsters *mon, item_def &item )
{
    ASSERT( item.base_type == OBJ_WEAPONS );

    if (mons_itemuse( mon->type ) < MONUSE_WEAPONS_ARMOUR)
        return (false);

    if (mon->inv[MSLOT_WEAPON] != NON_ITEM)
        return (false);

    if (is_fixed_artefact( item ))
        return (false);

    if (is_random_artefact( item ))
        return (false);

    // No monster picks up knives:
    if (item.sub_type == WPN_KNIFE)
        return (false);

    const int brand = get_weapon_brand( item );

    if (mons_is_unholy( mon ))
    {
        if (brand == SPWPN_HOLY_WRATH || brand == SPWPN_DISRUPTION)
            return (false);
    }
    else if (mons_holiness( mon ) == MH_HOLY)
    {
        if (brand == SPWPN_PAIN
            || brand == SPWPN_VAMPIRICISM
            || brand == SPWPN_VENOM)
        {
            return (false);
        }
    }

    const size_type        size  = mons_size( mon->type );
    const hands_reqd_type  hands = hands_reqd( item, size );

    // avoid two-handed weapons with shields
    if (mon->inv[MSLOT_MISSILE] != NON_ITEM
        && mitm[mon->inv[MSLOT_MISSILE]].base_type == OBJ_ARMOUR
        && get_armour_slot( mitm[mon->inv[MSLOT_MISSILE]] ) == EQ_SHIELD
        && hands == HANDS_TWO)
    {
        return (false);
    }

    // wimpy monsters (Kob, gob) shouldn't pick up halberds etc
    // large monsters look for big weapons
    // giant monsters only use two handed weapons
    if (size < SIZE_LITTLE || size > SIZE_GIANT)
        return (false);
    else if (size <= SIZE_SMALL && property( item, PWPN_HIT ) <= 0)
        return (false);
    else if (size == SIZE_LARGE && hands == HANDS_ONE)
        return (false);
    else if (size > SIZE_LARGE && hands != HANDS_TWO)
        return (false);

    return (true);
}

static bool mons_likes_armour( struct monsters *mon, item_def &item )
{
    ASSERT( item.base_type == OBJ_ARMOUR );

    if (mons_itemuse( mon->type ) < MONUSE_WEAPONS_ARMOUR)
        return (false);

    if (!mons_is_humanoid( mon->type ))
        return (false);

    const int size = mons_size( mon->type );
    const int slot = get_armour_slot( item );

    if (slot == EQ_BODY_ARMOUR)
    {
        if (mon->inv[MSLOT_ARMOUR] != NON_ITEM)
            return (false);

        if (size < SIZE_LITTLE || size > SIZE_LARGE)
            return (false);

        if (size == SIZE_SMALL && !is_light_armour( item ))
            return (false);

        if ((size == SIZE_LARGE || size == SIZE_LITTLE)
            && (item.sub_type != ARM_ROBE && item.sub_type != ARM_ANIMAL_SKIN))
        {
            return (false);
        }

        // check if we're ahappy with the AC/EV of wearing it:
        if (property( item, PARM_AC ) + item.plus < 1
            || mon->evasion + property( item, PARM_EVASION ) / 2 < 5)
        {
            return (false);
        }
    }
    else if (slot == EQ_SHIELD)
    {
        if (mon->inv[MSLOT_MISSILE] != NON_ITEM)
            return (false);

        if (size == SIZE_SMALL && item.sub_type != ARM_BUCKLER)
            return (false);

        if (size == SIZE_LARGE && item.sub_type != ARM_LARGE_SHIELD)
            return (false);

        if (item.plus < -2)
            return (false);
    }
    else // all other armours are unusable currently
    {
        return (false);
    }

    return (true);
}

static bool mons_likes_ammo( struct monsters *mon, item_def &item )
{
    ASSERT( item.base_type == OBJ_MISSILES );

    // must be capable of using them
    if (mons_itemuse( mon->type ) < MONUSE_WEAPONS_ARMOUR)
        return (false);

    // don't pick up if we're in combat
    if (mon->behaviour != BEH_WANDER)
        return (false);

    // Only large rocks are picked up in the player's presence... it's
    // too abusable right now to allow otherwise.  At least until we
    // can tell when an item has been plated by the player (which
    // should probably be used with magic items as well).
    if (item.sub_type != MI_LARGE_ROCK
        && grid_distance( mon->x, mon->y, you.x_pos, you.y_pos ) < 9)
    {
        return (false);
    }

    // already have some ammo, then only looking for stackable types:
    if (mon->inv[MSLOT_MISSILE] != NON_ITEM)
    {
        return (mitm[mon->inv[MSLOT_MISSILE]].sub_type == item.sub_type
                && mitm[mon->inv[MSLOT_MISSILE]].plus == item.plus
                && mitm[mon->inv[MSLOT_MISSILE]].special == item.special);
    }

    // now looking for fresh ammo, important that we never clobber any old
    ASSERT( mon->inv[MSLOT_MISSILE] == NON_ITEM );

    const int bow = mon->inv[MSLOT_WEAPON];

    // monsters with a launcher weapon look only at ammo for it
    if (bow != NON_ITEM
        && mitm[bow].base_type == OBJ_WEAPONS
        && is_range_weapon( mitm[bow] )
        && item.sub_type == fires_ammo_type( mitm[bow] ))
    {
        return (true);
    }
    else  // monster barehanded or using a melee weapon
    {
        const int  size = mons_size( mon->type );
        const bool weak = (mons_damage( mon, 0 ) <= 5);

        if (size < SIZE_LITTLE || size > SIZE_GIANT)
            return (false);
        else if (size == SIZE_GIANT)
            return (item.sub_type == MI_LARGE_ROCK);
        else if (weak)
            return (item.sub_type == MI_DART || item.sub_type == MI_STONE);
    }

    return (false);
}

static bool mons_likes_scroll( struct monsters *mon, item_def &item )
{
    ASSERT( item.base_type == OBJ_SCROLLS );

    if (mons_itemuse( mon->type ) < MONUSE_MAGIC_ITEMS)
        return (false);

    if (mon->inv[MSLOT_SCROLL] != NON_ITEM)
        return (false);

    // these are the only supported monster scrolls:
    if (item.sub_type == SCR_TELEPORTATION
        || item.sub_type == SCR_BLINKING
        || item.sub_type == SCR_SUMMONING)
    {
        return (true);
    }

    // XXX: this needs to be better.
    // trying to avoid giving too much information here:
    return (random2(8) >= mons_intel(mon->type));
}

static bool mons_likes_potion( struct monsters *mon, item_def &item )
{
    ASSERT( item.base_type == OBJ_POTIONS );

    if (mons_itemuse( mon->type ) < MONUSE_MAGIC_ITEMS)
        return (false);

    if (mon->inv[MSLOT_POTION] != NON_ITEM)
        return (false);

    // these are the only supported monster potions:
    if (item.sub_type == POT_HEALING
        || item.sub_type == POT_HEAL_WOUNDS
        || item.sub_type == POT_SPEED
        || item.sub_type == POT_INVISIBILITY)
    {
        return (true);
    }

    // XXX: this needs to be better.
    // trying to avoid giving too much information here:
    return (random2(8) >= mons_intel(mon->type));
}

static bool mons_likes_wand( struct monsters *mon, item_def &item )
{
    ASSERT( item.base_type == OBJ_WANDS );

    if (mons_itemuse( mon->type ) < MONUSE_MAGIC_ITEMS)
        return (false);

    if (mon->inv[MSLOT_WAND] != NON_ITEM)
        return (false);

    // most wands are monster effective, but only if charged:
    return (item.plus > 2);
}

static bool mons_likes_carrion( struct monsters *mon )
{
    return (mon->type == MONS_GHOUL
            || mon->type == MONS_NECROPHAGE
            || mon->type == MONS_DEATH_OOZE
            || mon->type == MONS_DEMONIC_CRAWLER
            || mon->type == MONS_DEATH_DRAKE);
}

static bool mons_likes_gold( struct monsters *mon )
{
    if (mons_itemuse( mon->type ) < MONUSE_WEAPONS_ARMOUR)
        return (false);

    // only humanoid sentients care for it
    if (!mons_is_humanoid( mon->type ))
        return (false);

    // stupid monsters and very intelligent monsters don't waste time on gold
    if (mons_intel( mon->type ) != I_NORMAL)
        return (false);

    return (true);
}

//---------------------------------------------------------------
//
// handle_pickup
//
// Returns false if monster doesn't spend any time pickup up
//
//---------------------------------------------------------------
static bool handle_pickup( struct monsters *monster )
{
    // single calculation permissible {dlb}
    bool monsterNearby = mons_near(monster);
    const int monuse = mons_itemuse( monster->type );
    char str_pass[ ITEMNAME_SIZE ];
    int  item = NON_ITEM;
    int  next = NON_ITEM;

    if (monster->energy < 60)
        return (false);

    if (mons_is_submerged( monster ))
        return (false);

    if (monuse == MONUSE_EATS_ITEMS)
    {
        int hps_gained = 0;
        int max_eat = roll_dice( 1, 10 );
        int eaten = 0;

        for (item = igrd[monster->x][monster->y];
             item != NON_ITEM && eaten < max_eat && hps_gained < 50;
             item = next)
        {
            int quant = mitm[item].quantity;
            next = mitm[item].link;          // in case we can't get it later

            // don't eat fixed artefacts
            if (is_fixed_artefact( mitm[item] ))
                continue;

            // shouldn't eat stone things
            //    - but what about wands and rings?
            if (mitm[item].base_type == OBJ_MISSILES
                && (mitm[item].sub_type == MI_STONE
                    || mitm[item].sub_type == MI_LARGE_ROCK))
            {
                continue;
            }

            // don't eat special game items
            if (mitm[item].base_type == OBJ_ORBS
                || (mitm[item].base_type == OBJ_MISCELLANY
                    && mitm[item].sub_type == MISC_RUNE_OF_ZOT))
            {
                continue;
            }

            if (mitm[igrd[monster->x][monster->y]].base_type != OBJ_GOLD)
            {
                if (quant > max_eat - eaten)
                    quant = max_eat - eaten;

                hps_gained += (quant * item_mass( mitm[item] )) / 20 + quant;
                eaten += quant;
            }
            else
            {
                // shouldn't be much trouble to digest a huge pile of gold!
                if (quant > 500)
                    quant = 500 + roll_dice( 2, (quant - 500) / 2 );

                hps_gained += quant / 10 + 1;
                eaten++;
            }

            dec_mitm_item_quantity( item, quant );
        }

        if (eaten)
        {
            if (hps_gained < 1)
                hps_gained = 1;
            else if (hps_gained > 50)
                hps_gained = 50;

            // This is done manually instead of using heal_monster(),
            // because that function doesn't work quite this way.  -- bwr
            monster->hit_points += hps_gained;

            if (monster->max_hit_points < monster->hit_points)
                monster->max_hit_points = monster->hit_points;

            if (player_can_hear( monster->x, monster->y ))
            {
                mpr( MSGCH_SOUND, "You hear a%s slurping noise.",
                      monsterNearby ? "" : " distant" );
            }

            if (mons_class_flag( monster->type, M_SPLITS ))
            {
                const int reqd = (monster->hit_dice <= 6)
                                            ? 50 : monster->hit_dice * 8;

                if (monster->hit_points >= reqd)
                    jelly_divide(monster);
            }
        }

        return (false);
    }                           // end "if jellies"

    bool picked_up = false;     // picked anything up?

    for (item = igrd[monster->x][monster->y]; item != NON_ITEM; item = next)
    {
        next = mitm[item].link;  // in case we pick up the current

        // XXX: give monsters the ability to exchange items for "better" ones
        switch (mitm[item].base_type)
        {
        case OBJ_WEAPONS:
            if (!mons_likes_weapon( monster, mitm[item] ))
                break;

            if (monsterNearby)
            {
                strcpy( info, ptr_monam(monster, DESC_CAP_THE) );
                strcat( info, " picks up " );
                it_name( item, DESC_NOCAP_A, str_pass );
                strcat( info, str_pass );
                strcat( info, "." );

                mpr(info);
            }

            unlink_item( item );
            monster->inv[MSLOT_WEAPON] = item;

            if (get_weapon_brand( mitm[item] ) == SPWPN_PROTECTION)
                monster->armour_class += 3;

            picked_up = true;
            break;

        case OBJ_MISSILES:
            if (!mons_likes_ammo( monster, mitm[item] ))
                break;

            if (monsterNearby)
            {
                strcpy( info, ptr_monam(monster, DESC_CAP_THE) );
                strcat( info, " picks up " );
                it_name( item, DESC_NOCAP_A, str_pass );
                strcat( info, str_pass );
                strcat( info, "." );

                mpr(info);
            }

            if (monster->inv[MSLOT_MISSILE] != NON_ITEM)
            {
                // better have chosen stackable ammo:
                const int ammo = monster->inv[MSLOT_MISSILE];
                ASSERT(mitm[ammo].sub_type == mitm[item].sub_type
                        && mitm[ammo].plus == mitm[item].plus
                        && mitm[ammo].special == mitm[item].special);

                inc_mitm_item_quantity( ammo, mitm[item].quantity );
                dec_mitm_item_quantity( item, mitm[item].quantity );
            }
            else
            {
                // fill empty slot with ammo:
                unlink_item( item );
                monster->inv[MSLOT_MISSILE] = item;
            }

            picked_up = true;
            break;

        case OBJ_ARMOUR:
            if (!mons_likes_armour( monster, mitm[item] ))
                break;

            if (monsterNearby)
            {
                strcpy( info, ptr_monam(monster, DESC_CAP_THE) );
                strcat( info, " picks up " );
                it_name( item, DESC_NOCAP_A, str_pass );
                strcat( info, str_pass );
                strcat( info, "." );

                mpr(info);
            }

            unlink_item( item );

            if (get_armour_slot( mitm[item] ) == EQ_BODY_ARMOUR)
            {
                monster->inv[MSLOT_ARMOUR] = item;

                monster->armour_class += property( mitm[item], PARM_AC );
                monster->armour_class += mitm[item].plus;
                monster->evasion += property( mitm[item], PARM_EVASION ) / 2;
                monster->energy -= (100 / mons_speed( monster ));
            }
            else
            {
                ASSERT( get_armour_slot( mitm[item] ) == EQ_SHIELD );

                monster->inv[MSLOT_MISSILE] = item;
                if (get_armour_ego_type( mitm[item] ) == SPARM_PROTECTION)
                    monster->armour_class += 3;
            }

            picked_up = true;
            break;

        case OBJ_WANDS:
            if (!mons_likes_wand( monster, mitm[item] ))
                break;

            if (monsterNearby)
            {
                strcpy( info, ptr_monam(monster, DESC_CAP_THE) );
                strcat( info, " picks up " );
                it_name( item, DESC_NOCAP_A, str_pass );
                strcat( info, str_pass );
                strcat( info, "." );

                mpr(info);
            }

            unlink_item( item );
            monster->inv[MSLOT_WAND] = item;
            picked_up = true;
            break;

        case OBJ_SCROLLS:
            if (!mons_likes_scroll( monster, mitm[item] ))
                break;

            if (monsterNearby)
            {
                strcpy( info, ptr_monam(monster, DESC_CAP_THE) );
                strcat( info, " picks up " );
                it_name( item, DESC_NOCAP_A, str_pass );
                strcat( info, str_pass );
                strcat( info, "." );

                mpr(info);
            }

            unlink_item( item );
            monster->inv[MSLOT_SCROLL] = item;
            picked_up = true;
            break;

        case OBJ_POTIONS:
            if (!mons_likes_potion( monster, mitm[item] ))
                break;

            if (monsterNearby)
            {
                strcpy( info, ptr_monam(monster, DESC_CAP_THE) );
                strcat( info, " picks up " );
                it_name( item, DESC_NOCAP_A, str_pass );
                strcat( info, str_pass );
                strcat( info, "." );

                mpr(info);
            }

            unlink_item( item );
            monster->inv[MSLOT_POTION] = item;
            picked_up = true;
            break;

        case OBJ_FOOD:
            if (mitm[item].sub_type != FOOD_CHUNK)
                break;
            // fall-through
        case OBJ_CORPSES:
            if (!mons_likes_carrion( monster ))
                break;

            if (monsterNearby)
            {
                strcpy( info, ptr_monam(monster, DESC_CAP_THE) );
                strcat( info, " eats " );
                it_name( item, DESC_NOCAP_THE, str_pass );
                strcat( info, str_pass );
                strcat( info, "." );

                mpr(info);
            }

            if (mitm[item].base_type == OBJ_FOOD)
            {
                const bool rot = (mitm[item].special < 100);
                monster->hit_points += (rot ? 5 : 3) * mitm[item].quantity;
            }
            else
            {
                const int type = mitm[item].plus;
                monster->hit_points += 1 + random2( mons_weight(type) ) / 50;
            }

            // limited growth factor here
            if (monster->hit_points > 12 * monster->hit_dice)
                monster->hit_points = 12 * monster->hit_dice;

            if (monster->hit_points > monster->max_hit_points)
                monster->max_hit_points = monster->hit_points;

            destroy_item( item );
            picked_up = true;

            if (mons_class_flag( monster->type, M_SPLITS ))
            {
                const int reqd = (monster->hit_dice <= 6)
                                            ? 50 : 8 * monster->hit_dice;

                if (monster->hit_points >= reqd)
                    jelly_divide(monster);
            }
            break;

        case OBJ_GOLD: //mv - monsters now pick up gold (19 May 2001)
            if (!mons_likes_gold( monster ) || monsterNearby)
                break;

            if (monster->inv[MSLOT_GOLD] != NON_ITEM)
            {
                // transfer gold to monster's object, destroy ground object
                inc_mitm_item_quantity( monster->inv[MSLOT_GOLD],
                                        mitm[item].quantity );

                destroy_item( item );
                picked_up = true;
            }
            else
            {
                // grab gold object
                unlink_item( item );
                monster->inv[MSLOT_GOLD] = item;
                picked_up = true;
            }
            break;

        default:
            break;
        }
    }

    return (picked_up);
}                               // end handle_pickup()

static bool handle_monster_move( struct monsters *monster )
{
    FixedArray < bool, 3, 3 > good_move;
    int count_x, count_y, count;
    int okmove = MINMOVE;

    // first some safety checks: stationary monsters don't move...
    if (mons_stationary( monster->type ))
    {
        mmov_x = 0;
        mmov_y = 0;
        return (false);
    }

    // ... and make sure we have some intended direction calculated.
    if (!mmov_x && !mmov_y)
    {
        // but take a second look first -- it might well come out different
        calc_movement( monster, false );

        if (!mmov_x && !mmov_y)
            return (false);
    }

    const int habitat = monster_habitat( monster->type );
    bool deep_water_available = false;
    bool out_of_sight_available = false;

    int teleport_trap_id = -1;   // set to trap_id if one is available
    int zot_trap_id = -1;        // set to trap_id if one is available

    const bool isGuard = mons_is_guard( monster );
    const bool isSmart = (mons_intel( monster->type ) > I_ANIMAL);
    const bool canThink = (mons_intel( monster->type ) != I_PLANT);

    // Non-flying land monsters will be slowed some of the time by lava
    // and water (providing they meet the conditions to pass through those).
    if (!mons_flies(monster)
        && habitat == DNGN_FLOOR
        && !mons_class_flag( monster->type, M_AMPHIBIOUS )
        && one_chance_in(5))
    {
        okmove = DNGN_WATER_STUCK;
    }

    const bool curr_seen = see_grid( monster->x, monster->y );
    const int  curr_cloud = get_cloud_type( monster->x, monster->y );

    for (count_x = 0; count_x < 3; count_x++)
    {
        for (count_y = 0; count_y < 3; count_y++)
        {
            good_move[count_x][count_y] = true;

            const int targ_x = monster->x + count_x - 1;
            const int targ_y = monster->y + count_y - 1;

            // bounds check - don't consider moving out of grid!
            // ... or not moving.
            if (!in_bounds( targ_x, targ_y )
                || (count_x == 1 && count_y == 1))
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            const int  targ_grid  = grd[targ_x][targ_y];
            const bool targ_seen  = see_grid( targ_x, targ_y );
            const int  targ_cloud = get_cloud_type( targ_x, targ_y );

            if (targ_grid == DNGN_DEEP_WATER)
                deep_water_available = true;

            if (grd[targ_x][targ_y] < okmove)
            {
                if (mons_class_flag( monster->type, M_BURROWS )
                    && targ_grid == DNGN_ROCK_WALL)
                {
                    // don't burrow at an angle (legacy behaviour)
                    if (count_x != 1 && count_y != 1)
                    {
                        good_move[count_x][count_y] = false;
                        continue;
                    }
                }
                else
                {
                    good_move[count_x][count_y] = false;
                    continue;
                }
            }

            if (!habitat_okay( monster, targ_grid ))
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            if (monster->type == MONS_WANDERING_MUSHROOM
                && (curr_seen || targ_seen))
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            // avoid moving into line of sight when scared
            if (!curr_seen && targ_seen
                && (mons_is_scared( monster )
                    || monster->behaviour == BEH_CORNERED))
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            // Water/ice elementals avoid fire and heat
            if ((monster->type == MONS_WATER_ELEMENTAL
                    || monster->type == MONS_ICE_BEAST
                    || monster->type == MONS_SIMULACRUM_SMALL
                    || monster->type == MONS_SIMULACRUM_LARGE)
                && (targ_grid == DNGN_LAVA
                    || targ_cloud == CLOUD_FIRE
                    || targ_cloud == CLOUD_FIRE_MON
                    || targ_cloud == CLOUD_STEAM
                    || targ_cloud == CLOUD_STEAM_MON))
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            // Fire elementals avoid water and cold
            if ((monster->type == MONS_FIRE_ELEMENTAL
                    || monster->type == MONS_FIRE_VORTEX)
                && (targ_grid == DNGN_DEEP_WATER
                    || targ_grid == DNGN_SHALLOW_WATER
                    || targ_grid == DNGN_BLUE_FOUNTAIN
                    || targ_cloud == CLOUD_COLD
                    || targ_cloud == CLOUD_COLD_MON))
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            // Submerged water creatures avoid the shallows where
            // they would be forced to surface. -- bwr
            if (habitat == DNGN_DEEP_WATER
                && (targ_x != you.x_pos || targ_y != you.y_pos)
                && targ_grid != DNGN_DEEP_WATER
                && grd[monster->x][monster->y] == DNGN_DEEP_WATER
                && (mons_is_submerged( monster )
                    || monster->hit_points < (monster->max_hit_points * 3) / 4))
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            // smacking the player is always a good move if
            // we're hostile (even if we're heading somewhere
            // else)
            if (targ_x == you.x_pos && targ_y == you.y_pos
                && mons_friendly( monster ))
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            // smacking another monster is good,  if the monsters
            // are aligned differently
            if (mgrd[targ_x][targ_y] != NON_MONSTER
                && beings_aligned( monster_index(monster), mgrd[targ_x][targ_y] )
                && !mons_can_swap( monster, &menv[ mgrd[targ_x][targ_y] ] ))
            {
                good_move[count_x][count_y] = false;
                continue;
            }

            if (targ_cloud != CLOUD_NONE)
            {
                if (curr_cloud != CLOUD_NONE && targ_cloud == curr_cloud)
                    continue;

                switch (targ_cloud)
                {
                case CLOUD_FIRE:
                case CLOUD_FIRE_MON:
                    if (mons_res_fire(monster) > 0)
                        continue;

                    if (monster->hit_points >= 10 + roll_dice(5,10))
                        continue;
                    break;

                case CLOUD_STINK:
                case CLOUD_STINK_MON:
                    if (mons_res_poison(monster) > 0)
                        continue;
                    if (1 + random2(5) < monster->hit_dice)
                        continue;
                    if (monster->hit_points >= 4 + roll_dice(2,10))
                        continue;
                    break;

                case CLOUD_COLD:
                case CLOUD_COLD_MON:
                    if (mons_res_cold(monster) > 0)
                        continue;

                    if (monster->hit_points >= 10 + roll_dice(5,10))
                        continue;
                    break;

                case CLOUD_POISON:
                case CLOUD_POISON_MON:
                    if (mons_res_poison(monster) > 0)
                        continue;

                    if (monster->hit_points >= 8 + roll_dice(4,10))
                        continue;
                    break;

                // this isn't harmful,  but dumb critters might think so.
                case CLOUD_GREY_SMOKE:
                case CLOUD_GREY_SMOKE_MON:
                    if (isSmart || !canThink || coinflip())
                        continue;

                    if (mons_res_fire(monster) > 0)
                        continue;

                    if (monster->hit_points >= roll_zdice(2,10))
                        continue;
                    break;

                case CLOUD_MIASMA:
                case CLOUD_MIASMA_MON:
                    if (mons_res_negative_energy(monster) >= 3)
                        continue;

                    if (mons_res_poison(monster) > 0
                        && mons_res_negative_energy(monster) > 0)
                    {
                        continue;
                    }

                    // Dumb monsters aren't so stupid as to step in
                    // this stuff.
                    good_move[count_x][count_y] = false;
                    continue;

                default:
                    continue;   // harmless clouds
                }

                // if we get here, the cloud is potentially harmful.
                // exceedingly dumb creatures will still wander in.
                if (canThink)
                {
                    good_move[count_x][count_y] = false;
                    continue;
                }
            }

            // wandering through a trap is OK if we're pretty healthy, or
            // really stupid.
            const int tid = trap_at_xy( targ_x, targ_y );

            if (tid != -1)
            {
                if (trap_category( env.trap[tid].type ) == DNGN_TRAP_MECHANICAL)
                {
                    if (canThink
                        && monster->hit_points < monster->max_hit_points / 2)
                    {
                        good_move[count_x][count_y] = false;
                        continue;
                    }
                }
                else if (env.trap[tid].type == TRAP_TELEPORT)
                {
                    if (teleport_trap_id == -1 || coinflip())
                        teleport_trap_id = tid;
                }
                else if (env.trap[tid].type == TRAP_ZOT)
                {
                    if (zot_trap_id == -1 || coinflip())
                        zot_trap_id = tid;
                }
            }

            if (!targ_seen)
                out_of_sight_available = true;
        }
    } // now we know where we _can_ move.

    // normal/smart monsters know about secret doors (they _live_ in the
    // dungeon!)
    if (grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_CLOSED_DOOR
        || (grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_SECRET_DOOR
            && isSmart && !isGuard && one_chance_in(10)))
    {
        if (mons_is_animated_undead( monster->type ))
        {
            // for zombies, monster type is kept in mon->number
            if (mons_itemuse(monster->number) >= MONUSE_OPEN_DOORS)
            {
                grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_OPEN_DOOR;
                mmov_x = 0;
                mmov_y = 0;
                return (true);
            }
        }
        else if (mons_itemuse(monster->type) >= MONUSE_OPEN_DOORS)
        {
            grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_OPEN_DOOR;
            mmov_x = 0;
            mmov_y = 0;
            return (true);
        }
    } // endif - secret/closed doors

    // jellies eat doors.  Yum!
    if ((grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_CLOSED_DOOR
            || grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_OPEN_DOOR)
        && mons_itemuse(monster->type) == MONUSE_EATS_ITEMS)
    {
        grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_FLOOR;

        if (player_can_hear( monster->x, monster->y ))
        {
            strcpy(info, "You hear a");

            if (!mons_near(monster))
                strcat(info, " distant");

            strcat(info, " slurping noise.");
            mpr(MSGCH_SOUND,info );
        }

        monster->hit_points += 5;

        // note here, that this makes jellies "grow" {dlb}:
        if (monster->hit_points > monster->max_hit_points)
            monster->max_hit_points = monster->hit_points;

        if (mons_class_flag( monster->type, M_SPLITS ))
        {
            // and here is where the jelly might divide {dlb}
            const int reqd = (monster->hit_dice < 6) ? 50
                                                     : monster->hit_dice * 8;

            if (monster->hit_points >= reqd)
                jelly_divide(monster);
        }
    } // done door-eating jellies

    // water creatures have a preferance for water they can hide in -- bwr
    if (habitat == DNGN_DEEP_WATER
        && deep_water_available
        && grd[monster->x][monster->y] != DNGN_DEEP_WATER
        && grd[monster->x + mmov_x][monster->y + mmov_y] != DNGN_DEEP_WATER
        && (monster->x + mmov_x != you.x_pos
            || monster->y + mmov_y != you.y_pos)
        && (coinflip()
            || monster->hit_points <= (monster->max_hit_points * 3) / 4))
    {
        count = 0;

        for (count_x = 0; count_x < 3; count_x++)
        {
            for (count_y = 0; count_y < 3; count_y++)
            {
                if (good_move[count_x][count_y]
                    && grd[monster->x + count_x - 1][monster->y + count_y - 1]
                            == DNGN_DEEP_WATER)
                {
                    count++;

                    if (one_chance_in( count ))
                    {
                        mmov_x = count_x - 1;
                        mmov_y = count_y - 1;
                    }
                }
            }
        }
    }
    else if (mons_is_scared( monster ) || monster->behaviour == BEH_CORNERED)
    {
        if (teleport_trap_id != -1)
        {
            // make our prefered move a teleport trap if available
            mmov_x = env.trap[teleport_trap_id].x - monster->x;
            mmov_y = env.trap[teleport_trap_id].y - monster->y;
        }
        else if (out_of_sight_available)
        {
            // remove all in sight options from the good list
            for (count_x = 0; count_x < 3; count_x++)
            {
                for (count_y = 0; count_y < 3; count_y++)
                {
                    if (see_grid( monster->x + count_x - 1,
                                  monster->y + count_y - 1 ))
                    {
                        good_move[count_x][count_y] = false;
                    }
                }
            }
        }
    }

    // now,  if a monster can't move in its intended direction,  try
    // either side.  If they're both good,  move in whichever dir
    // gets it closer(farther for fleeing monsters) to its target.
    // If neither does,  do nothing.
    //
    // Unseen monsters will sometimes also choose a less desirable
    // direction to help keep themselves hidden.  This might seem like
    // cheating, but the player can often tell from experience or
    // monster behaviour if a monster can't see invisible, too.
    if (good_move[mmov_x + 1][mmov_y + 1] == false
        || (!player_monster_visible( monster ) && !one_chance_in(3)))
    {
        const int curr_dist = grid_distance( monster->x, monster->y,
                                             monster->target_x,
                                             monster->target_y );

        const int mmov_dist = grid_distance( monster->x + mmov_x,
                                             monster->y + mmov_y,
                                             monster->target_x,
                                             monster->target_y );

        const bool forced_flee = mons_has_ench( monster, ENCH_FEAR);

        const bool flee = (monster->behaviour == BEH_FLEE
                            || mmov_dist > curr_dist); // to detect back-off

        int dir = -1;
        int i, mod, newdir;

        for (i = 0; i < 8; i++)
        {
            if (compass_x[i] == mmov_x && compass_y[i] == mmov_y)
            {
                dir = i;
                break;
            }
        }

        if (dir < 0)
            goto forget_it;

        int dist[2];
        int best_prev_dist = (flee ? (-FAR_AWAY) : FAR_AWAY);

        // first 1 away, then 2
        for (int j = 1; j <= ((flee) ? 3 : 2); j++)
        {
            int sdir, inc;

            if (coinflip())
            {
                sdir = -j;
                inc = 2 * j;
            }
            else
            {
                sdir = j;
                inc = -2 * j;
            }

            // try both directions
            for (mod = sdir, i = 0; i < 2; mod += inc, i++)
            {
                newdir = (dir + 8 + mod) % 8;

                if (good_move[compass_x[newdir] + 1][compass_y[newdir] + 1])
                {
                    dist[i] = grid_distance( monster->x + compass_x[newdir],
                                             monster->y + compass_y[newdir],
                                             monster->target_x,
                                             monster->target_y );
                }
                else
                {
                    dist[i] = ((flee) ? (-FAR_AWAY) : FAR_AWAY);
                }
            }

            // now choose:

            // neither is any good
            if (dist[0] == dist[1] && abs(dist[0]) == FAR_AWAY)
                continue;

            // which one was better? -- depends on FLEEING or not
            if (flee)
            {
                // Note: when fleeing we don't allow moves to squares
                // of the same distance away at this point so that
                // monsters who run into walls of rooms realize that
                // they're cornered.  We catch cases where the monster
                // can dive out of LoS below.  In the case of magically
                // enchanted "forced" fleeing, cornering isn't going to
                // happen so we might as well allow them to fumble along
                // the wall.
                if (dist[0] >= dist[1]
                    && (dist[0] > curr_dist
                        || (forced_flee && dist[0] == curr_dist))
                    && (dist[0] > best_prev_dist
                        || (dist[0] == best_prev_dist && one_chance_in(3))))
                {
                    mmov_x = compass_x[ ((dir+8) + sdir) % 8 ];
                    mmov_y = compass_y[ ((dir+8) + sdir) % 8 ];
                    best_prev_dist = dist[0];
                    continue;
                }

                if (dist[1] >= dist[0]
                    && (dist[1] > curr_dist
                        || (forced_flee && dist[0] == curr_dist))
                    && (dist[1] > best_prev_dist
                        || (dist[1] == best_prev_dist && one_chance_in(3))))
                {
                    mmov_x = compass_x[ ((dir+8) - sdir) % 8 ];
                    mmov_y = compass_y[ ((dir+8) - sdir) % 8 ];
                    best_prev_dist = dist[1];
                    continue;
                }
            }
            else
            {
                if (dist[0] <= dist[1]
                    && dist[0] <= curr_dist
                    && (dist[0] < best_prev_dist
                        || (dist[0] == best_prev_dist && one_chance_in(3))))
                {
                    mmov_x = compass_x[((dir+8)+sdir)%8];
                    mmov_y = compass_y[((dir+8)+sdir)%8];
                    best_prev_dist = dist[0];
                    continue;
                }

                if (dist[1] <= dist[0]
                    && dist[1] <= curr_dist
                    && (dist[1] < best_prev_dist
                        || (dist[1] == best_prev_dist && one_chance_in(3))))
                {
                    mmov_x = compass_x[((dir+8)-sdir)%8];
                    mmov_y = compass_y[((dir+8)-sdir)%8];
                    best_prev_dist = dist[1];
                    continue;
                }
            }
        }
    } // end - try to find good alternate move

forget_it:

    // still no good?  if we can hide and might want to... pick randomly
    if (good_move[mmov_x + 1][mmov_y + 1] == false)
    {
        if (monster->behaviour == BEH_WANDER)
        {
            // Choose a new wandering target if we're having some trouble.
            set_wander_target( monster );
        }
        else if (mons_is_scared(monster) || monster->behaviour == BEH_CORNERED)
        {
            if (out_of_sight_available)
            {
                count = 0;

                for (count_x = 0; count_x < 3; count_x++)
                {
                    for (count_y = 0; count_y < 3; count_y++)
                    {
                        if (good_move[count_x][count_y])
                        {
                            count++;

                            if (one_chance_in(count))
                            {
                                mmov_x = count_x - 1;
                                mmov_y = count_y - 1;
                            }
                        }
                    }
                }
            }
            else if (zot_trap_id != -1)
            {
                mmov_x = env.trap[zot_trap_id].x - monster->x;
                mmov_y = env.trap[zot_trap_id].y - monster->y;
            }
        }
    }

    // ------------------------------------------------------------------
    // if we haven't found a good move by this point, we're not going to.
    // ------------------------------------------------------------------

    bool ret = true;

    // take care of beetle burrowing
    if (mons_class_flag( monster->type, M_BURROWS ))
    {
        if (grd[monster->x + mmov_x][monster->y + mmov_y] == DNGN_ROCK_WALL
            && good_move[mmov_x + 1][mmov_y + 1] == true)
        {
            grd[monster->x + mmov_x][monster->y + mmov_y] = DNGN_FLOOR;

            if (!see_grid( monster->x + mmov_x, monster->y + mmov_y ))
                set_terrain_changed( monster->x + mmov_x, monster->y + mmov_y );

            noisy( SL_DIG, monster->x + mmov_x, monster->y + mmov_y,
                   "You hear a grinding noise." );
        }
    }

    if (good_move[mmov_x + 1][mmov_y + 1] && (mmov_x || mmov_y))
    {
        const int index = monster_index(monster);

        // check for attacking player
        if (monster->x + mmov_x == you.x_pos
            && monster->y + mmov_y == you.y_pos)
        {
            monster_attack( index, MHITYOU );
            mmov_x = 0;
            mmov_y = 0;
        }

        // If we're following the player through stairs, the only valid
        // movement is towards the player. -- bwr
        if (testbits( monster->flags, MF_TAKING_STAIRS ))
        {
#if DEBUG_DIAGNOSTICS
            mpr( MSGCH_DIAGNOSTICS,
                    "%s is skipping movement in order to follow.",
                      ptr_monam( monster, DESC_CAP_THE ) );
#endif
            mmov_x = 0;
            mmov_y = 0;
        }

        if (monster->type == MONS_EFREET
            || monster->type == MONS_FIRE_ELEMENTAL)
        {
            place_cloud( CLOUD_FIRE_MON, monster->x, monster->y,
                         2 + random2(4) );
        }

        // do movement, fighting, and swapping (but only if we have a move):
        if (mmov_x || mmov_y)
        {
            const int new_x = monster->x + mmov_x;
            const int new_y = monster->y + mmov_y;
            const int targmon = mgrd[new_x][new_y];

            if (targmon != NON_MONSTER)
            {
                // attacking/swapping another monster (depends on alignment)
                if (!beings_aligned( index, targmon ))
                {
                    // fight
                    monster_attack( index, targmon );
                    mmov_x = 0;
                    mmov_y = 0;
                }
                else
                {
                    swap_monsters_positions( index, targmon );

#if DEBUG_DIAGNOSTICS
                    snprintf( info, INFO_SIZE, " swaps positions with %s.",
                              ptr_monam( &menv[targmon], DESC_NOCAP_THE ) );

                    mon_msg( monster, info, true, MSGCH_DIAGNOSTICS );
#endif
                    // wake up monster we just swapped with:
                    if (menv[targmon].behaviour == BEH_SLEEP)
                        behaviour_event( &menv[targmon], ME_DISTURB );
                }
            }
            else // empty -- move on in
            {
                // this appears to be the real one, ie where movement occurs:
                move_monster_to_grid( index, new_x, new_y );

                // monsters stepping on traps:
                if (grd[monster->x][monster->y] >= DNGN_TRAP_MECHANICAL
                    && grd[monster->x][monster->y] <= DNGN_UNDISCOVERED_TRAP)
                {
                    mons_trap( monster );
                }
            }
        }
    }
    else
    {
        // failed to find good move... try adjusting behaviour for next time
        ret = false;

        // fleeing monsters that can't move will panic and possibly
        // turn to face their attacker
        if (curr_seen && monster->behaviour == BEH_FLEE
            && one_chance_in( friends_near_target(monster) ))
        {
            behaviour_event( monster, ME_CORNERED );
        }
        else // if (monster->behaviour == BEH_WANDER)
        {
            // reset the target hoping to get something better
            set_wander_target( monster );
        }
    }

    return (ret);
}                               // end monster_move()


static bool plant_spit(struct monsters *monster, struct bolt &pbolt)
{
    bool didSpit = false;

    char spit_string[INFO_SIZE];

    // setup plant spit
    strcpy( pbolt.name, "acid" );
    pbolt.type = SYM_ZAP;
    pbolt.range = 8;
    pbolt.rangeMax = 8;
    pbolt.colour = YELLOW;
    pbolt.flavour = BEAM_ACID;
    pbolt.beam_source = monster_index(monster);
    pbolt.damage = dice_def( 3, 7 );
    pbolt.hit = 30 + (3 * monster->hit_dice);
    pbolt.thrower = KILL_MON_MISSILE;
    pbolt.aux_source = NULL;
    pbolt.is_beam = false;
    pbolt.is_enchant = false;

    // fire tracer
    fire_tracer( monster, pbolt );

    if (mons_should_fire( pbolt ))
    {
        strcpy( spit_string, " spits" );
        if (pbolt.target_x == you.x_pos && pbolt.target_y == you.y_pos)
            strcat( spit_string, " at you" );

        strcat( spit_string, "." );
        mon_msg( monster, spit_string );

        fire_beam( pbolt );
        didSpit = true;
    }

    return (didSpit);
}                               // end plant_spit()

void mons_in_cloud( struct monsters *monster )
{
    int type = get_cloud_type( monster->x, monster->y );
    int hurted = 0;
    struct bolt beam;

    // Note: this function needs to be called at the rate of monster
    // actions... therefore we need to adjust things for the time spent
    // by the monster doing an action.  For simplicity, we're assuming
    // that all actions are of standard duration and adjusting for the
    // number of them the monster can do in 10 ticks (ie mons_speed / 10).
    const int speed = mons_speed( monster );

    if (mons_is_mimic( monster->type ))
    {
        mimic_alert(monster);
        return;
    }

    // cprintf("Fatal error: monster steps on nonexistent cloud!");
    ASSERT( type != CLOUD_DEBUGGING );

    switch (type)
    {
    case CLOUD_FIRE:
    case CLOUD_FIRE_MON:
        if (monster->type == MONS_FIRE_VORTEX
            || monster->type == MONS_EFREET
            || monster->type == MONS_FIRE_ELEMENTAL)
        {
            return;
        }

        mon_msg( monster, " is engulfed in flame!" );

        if (mons_res_fire(monster) > 0)
            return;

        hurted += div_rand_round( (3 + roll_dice(3,6)) * 10, speed );

        if (mons_res_fire(monster) < 0)
            hurted += div_rand_round( random2(15) * 10, speed );

        // remember that the above is in addition to the other you.damage.
        hurted = apply_mons_armour( hurted, monster, true );
        break;                  // to damage routine at end {dlb}

    case CLOUD_COLD:
    case CLOUD_COLD_MON:
        mon_msg(monster, " is engulfed in freezing vapours!");

        if (mons_res_cold(monster) > 0)
            return;

        hurted += div_rand_round( (3 + roll_dice(3,6)) * 10, speed );

        if (mons_res_cold(monster) < 0)
            hurted += div_rand_round( random2(15) * 10, speed );

        // remember that the above is in addition to the other damage.
        hurted = apply_mons_armour( hurted, monster, true );
        break;                  // to damage routine at end {dlb}

    case CLOUD_STINK:
    case CLOUD_STINK_MON:
        mon_msg(monster, " is engulfed in noxious gasses!");

        if (mons_res_poison(monster) > 0)
            return;

        beam.flavour = BEAM_CONFUSION;

        if (random2(30) > 2 * monster->hit_dice)
            mons_ench_f2( monster, beam );

        hurted += div_rand_round( random2(3) * 10, speed );
        break;                  // to damage routine at end {dlb}

    // what of armour of poison resistance here? {dlb}
    case CLOUD_POISON:
    case CLOUD_POISON_MON:
        mon_msg(monster, " is engulfed in a cloud of poison!");

        if (mons_res_poison(monster) > 0)
            return;

        poison_monster( monster, (type == CLOUD_POISON) );

        hurted += div_rand_round( random2(8) * 10, speed );

        if (mons_res_poison(monster) < 0)
            hurted += div_rand_round( random2(4) * 10, speed );
        break;                  // to damage routine at end {dlb}

    case CLOUD_STEAM:
    case CLOUD_STEAM_MON:
        // couldn't be bothered coding for armour of res fire

        // what of whether it is wearing steam dragon armour? {dlb}
        if (monster->type == MONS_STEAM_DRAGON)
            return;

        mon_msg(monster, " is engulfed in steam!");

        if (mons_res_fire(monster) > 0)
            return;

        hurted += div_rand_round( random2(6) * 10, speed );

        if (mons_res_fire(monster) < 0)
            hurted += div_rand_round( random2(6) * 10, speed );

        hurted = apply_mons_armour( hurted, monster, true );
        break;                  // to damage routine at end {dlb}

    case CLOUD_MIASMA:
    case CLOUD_MIASMA_MON:
        mon_msg(monster, " is engulfed in a dark miasma!");

        if (mons_res_negative_energy( monster ) >= random2(3))
            return;

        if (mons_res_poison( monster ) <= 0)
            poison_monster( monster, (type == CLOUD_MIASMA) );

        if (mons_res_negative_energy( monster ) <= 0)
        {
            if (monster->max_hit_points > 4 && coinflip())
                monster->max_hit_points--;
        }

        if (one_chance_in( 3 + 2 * mons_res_negative_energy(monster) ))
        {
            beam.flavour = BEAM_SLOW;
            mons_ench_f2( monster, beam );
        }

        hurted += div_rand_round( roll_dice(3,4) * 10, speed );
        break;              // to damage routine at end {dlb}

    default:                // 'harmless' clouds -- colored smoke, etc {dlb}.
        return;
    }

    if (hurted)
    {
        const killer_type killer = (type == CLOUD_FIRE_MON
                                    || type == CLOUD_STINK_MON
                                    || type == CLOUD_COLD_MON
                                    || type == CLOUD_POISON_MON
                                    || type == CLOUD_STEAM_MON
                                    || type == CLOUD_MIASMA_MON) ? KILL_MISC
                                                                 : KILL_YOU;

        hurt_monster_to_kill( monster, hurted, killer, 0 );
    }
}                               // end mons_in_cloud()

unsigned char monster_habitat(int which_class)
{
    switch (which_class)
    {
    case MONS_BIG_FISH:
    case MONS_GIANT_GOLDFISH:
    case MONS_ELECTRICAL_EEL:
    case MONS_JELLYFISH:
    case MONS_SWAMP_WORM:
    case MONS_WATER_ELEMENTAL:
        return (DNGN_DEEP_WATER); // no shallow water (only) monsters? {dlb}
        // must remain DEEP_WATER for now, else breaks code {dlb}

    case MONS_LAVA_WORM:
    case MONS_LAVA_FISH:
    case MONS_LAVA_SNAKE:
    case MONS_SALAMANDER:
        return (DNGN_LAVA);

    default:
        return (DNGN_FLOOR);      // closest match to terra firma {dlb}
    }
}                               // end monster_habitat()

bool message_current_target(void)
{
    if (you.prev_targ != MHITNOT && you.prev_targ != MHITYOU)
    {
        struct monsters *montarget = &menv[you.prev_targ];

        if (mons_near(montarget) && player_monster_visible( montarget ))
        {
            mpr( "You are currently targeting %s (use p/t to fire).",
                  ptr_monam(montarget, DESC_NOCAP_THE) );

            return (true);        // early return {dlb}
        }

        mpr("You have no current target.");
    }

    return (false);
}                               // end message_current_target()

// aaah, the simple joys of pointer arithmetic! {dlb}:
int monster_index( struct monsters *monster )
{
    return (monster - menv.buffer());
}                               // end monster_index()

// returns true if monster damaged (does not call monster_die)
bool hurt_monster( struct monsters *victim, int damage )
{
    bool damaged = false;

    if (damage > 0)
    {
        damaged = true;
        victim->hit_points -= damage;
    }

    return (damaged);
}                               // end hurt_monster()

// returns true if the monster died and monster_die was called
bool hurt_monster_to_kill( struct monsters *victim, int damage,
                           killer_type killer, int src )
{
    bool died = false;
    if (hurt_monster( victim, damage ))
    {
        if (victim->hit_points < 1)
        {
            monster_die( victim, killer, src );
            died = true;
        }
    }

    return (died);
}                               // end hurt_monster_to_kill()


// just to avoid typing this over and over
// now returns true if monster died -- bwr
bool you_hurt_monster( struct monsters *victim, int damage )
{
    if (damage <= 0)
        return (false);

    const bool ret = hurt_monster_to_kill( victim, damage, KILL_YOU, MHITYOU );

    if (!ret)
        print_wounds( victim );

    return (ret);
}                               // end you_hurt_monster()


bool heal_monster( struct monsters *patient, int health_boost,
                   bool permit_growth )
{
    bool succ = false;

    succ = mons_del_ench( patient, ENCH_CUT );

    if (health_boost > 0
        && (patient->hit_points < patient->max_hit_points || permit_growth))
    {
        succ = true;
        patient->hit_points += health_boost;

        if (patient->hit_points > patient->max_hit_points)
        {
            if (permit_growth)
                patient->max_hit_points++;

            patient->hit_points = patient->max_hit_points;
        }
    }

    return (succ);
}                               // end heal_monster()

static int map_wand_to_mspell(int wand_type)
{
    int mzap = 0;

    if (wand_type == WAND_RANDOM_EFFECTS)
    {
        wand_type = random2(15);
        if (one_chance_in(16))
            wand_type = WAND_DRAINING;
    }

    switch (wand_type)
    {
        case WAND_FLAME:           mzap = MS_FLAME; break;
        case WAND_FROST:           mzap = MS_FROST; break;
        case WAND_SLOWING:         mzap = MS_SLOW; break;
        case WAND_HASTING:         mzap = MS_HASTE; break;
        case WAND_MAGIC_DARTS:     mzap = MS_MMISSILE; break;
        case WAND_HEALING:         mzap = MS_HEAL; break;
        case WAND_PARALYSIS:       mzap = MS_PARALYSIS; break;
        case WAND_FIRE:            mzap = MS_FIRE_BOLT; break;
        case WAND_COLD:            mzap = MS_COLD_BOLT; break;
        case WAND_CONFUSION:       mzap = MS_CONFUSE; break;
        case WAND_INVISIBILITY:    mzap = MS_INVIS; break;
        case WAND_FIREBALL:        mzap = MS_FIREBALL; break;
        case WAND_TELEPORTATION:   mzap = MS_TELEPORT_OTHER; break;
        case WAND_LIGHTNING:       mzap = MS_LIGHTNING_BOLT; break;
        case WAND_DRAINING:        mzap = MS_NEGATIVE_BOLT; break;
        case WAND_DIGGING:         mzap = MS_DIG; break;
        case WAND_ENSLAVEMENT:     mzap = MS_CONFUSE; break;
        case WAND_DISINTEGRATION:  mzap = MS_DISINTEGRATE; break;

        case WAND_POLYMORPH_OTHER:
        case WAND_RANDOM_EFFECTS:
        default:
            mzap = 0;
            break;
    }

    return (mzap);
}

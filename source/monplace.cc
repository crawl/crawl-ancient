/*
 *  File:       monplace.cc
 *  Summary:    Functions used when placing monsters in the dungeon.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "monplace.h"

#include <stdio.h>

#include "beam.h"
#include "globals.h"
#include "externs.h"
#include "dungeon.h"
#include "monstuff.h"
#include "mon-pick.h"
#include "mon-util.h"
#include "misc.h"
#include "player.h"
#include "stuff.h"
#include "spells4.h"
#include "view.h"

// NEW place_monster -- note that power should be set to:
// 51 for abyss
// 52 for pandemonium
// x otherwise

// proximity is the same as for mons_place:
// 0 is no restrictions
// 1 attempts to place near player
// 2 attempts to avoid player LOS

#define MAX_BAND        20

static int band_member( int band, int power );
static int choose_band( int mclass, int power, int &band_size );

static int place_monster_aux( int mclass, beh_type behaviour, int target,
                              int px, int py, int power, int extra,
                              bool first_band_member );


bool place_monster( int &id, int mclass, int power, beh_type behaviour,
                    int target, bool summoned, int px, int py, bool allow_bands,
                    int proximity, int extra )
{
    int band_size = 0;
    int band_monsters[MAX_BAND];        // band monster types
    int lev_mons = power;               // final 'power'
    int count;
    int i;

    // set initial id to -1  (unsuccessful create)
    id = -1;

    // (1) early out (summoned to occupied grid)
    if (summoned && mgrd[px][py] != NON_MONSTER)
        return (false);

    // (2) take care of random monsters
    if (mclass == RANDOM_MONSTER
        && player_in_branch( BRANCH_HALL_OF_BLADES ))
    {
        mclass = MONS_DANCING_WEAPON;
    }

    if (mclass == RANDOM_MONSTER)
    {
        if (player_in_branch( BRANCH_MAIN_DUNGEON )
            && lev_mons != PANDEMONIUM_DEPTH
            && lev_mons != ABYSS_DEPTH
            && one_chance_in(4))
        {
            lev_mons = random2(power);
        }

        if (player_in_branch( BRANCH_MAIN_DUNGEON ) && lev_mons < 28)
        {
            // potentially nasty surprise, but very rare
            if (one_chance_in(5000))
                lev_mons = random2(27);

            // slightly out of depth monsters are more common:
            if (one_chance_in(50))
                lev_mons += random2(6);

            if (lev_mons > 27)
                lev_mons = 27;
        }

        /* Abyss or Pandemonium. Almost never called from Pan;
           probably only if a rand demon gets summon anything spell */
        if (lev_mons == PANDEMONIUM_DEPTH
            || lev_mons == ABYSS_DEPTH
            || you.level_type == LEVEL_PANDEMONIUM
            || you.level_type == LEVEL_ABYSS)
        {
            do
            {
                count = 0;

                do
                {
                    // was: random2(400) {dlb}
                    mclass = random2(NUM_MONSTERS);
                    count++;
                }
                while (mons_abyss(mclass) == 0 && count < 2000);

                if (count == 2000)
                    return (false);
            }
            while (roll_curved_percentile() > mons_rare_abyss(mclass)
                    && !one_chance_in(100));
        }
        else
        {
            int level, diff, chance;

            if (lev_mons > 30)
                lev_mons = 30;

            for (i = 0; i < 10000; i++)
            {
                count = 0;

                do
                {
                    mclass = random2(NUM_MONSTERS);
                    count++;
                }
                while (mons_rarity( mclass ) == 0 && count < 2000);

                if (count == 2000)
                    return (false);

                level  = mons_level( mclass );
                diff   = level - lev_mons;
                chance = mons_rarity( mclass ) - (diff * diff);

                if (lev_mons >= level - 5
                    && lev_mons <= level + 5
                    && roll_curved_percentile() < chance)
                {
                    break;
                }
            }

            if (i == 10000)
                return (false);
        }
    }

    // (3) decide on banding (good lord!)
    band_size = 1;
    band_monsters[0] = mclass;

    if (allow_bands)
    {
        int band = choose_band( mclass, power, band_size );
        band_size ++;

        for (i = 1; i < band_size; i++)
            band_monsters[i] = band_member( band, power );
    }

    // Monsters that can't move shouldn't be taking the stairs -- bwr
    if (proximity == PROX_NEAR_STAIRS && mons_stationary( mclass ))
        proximity = PROX_AWAY_FROM_PLAYER;

    // (4) for first monster, choose location.  This is pretty intensive.
    bool proxOK;
    bool close_to_player;

    // player shoved out of the way?
    bool shoved = false;
    unsigned char stair_gfx = 0;
    int pval = 0;

    if (!summoned)
    {
        int tries = 0;
        // try to pick px, py that is
        // a) not occupied
        // b) compatible
        // c) in the 'correct' proximity to the player
        const int grid_wanted = monster_habitat( mclass );

        while (true)
        {
            tries++;

            // give up on stair placement?
            if (proximity == PROX_NEAR_STAIRS)
            {
                if (tries > 320)
                {
                    proximity = PROX_AWAY_FROM_PLAYER;
                    tries = 0;
                }
            }
            else if (tries > 300)
                return (false);

            random_in_bounds( px, py, grid_wanted, true );

            // occupied?
            if (mgrd[px][py] != NON_MONSTER
                || (px == you.x_pos && py == you.y_pos))
            {
                continue;
            }

            // compatible - floor?
            if (grid_wanted == DNGN_FLOOR && grd[px][py] < DNGN_FLOOR)
                continue;

            // compatible - others (must match, except for deep water monsters
            // generated in shallow water)
            if ((grid_wanted != DNGN_FLOOR && grd[px][py] != grid_wanted)
                && (grid_wanted != DNGN_DEEP_WATER || grd[px][py] != DNGN_SHALLOW_WATER))
            {
                continue;
            }

            // don't generate monsters on top of teleport traps
            // (how did they get there?)
            int trap = trap_at_xy(px, py);
            if (trap != -1)
            {
                if (env.trap[trap].type == TRAP_TELEPORT)
                    continue;
            }

            // check proximity to player
            proxOK = true;

            switch (proximity)
            {
                case PROX_ANYWHERE:
                    if (grid_distance( you.x_pos, you.y_pos, px, py ) < 2 + random2(3))
                    {
                        proxOK = false;
                    }
                    break;

                case PROX_CLOSE_TO_PLAYER:
                case PROX_AWAY_FROM_PLAYER:
                    close_to_player = (distance(you.x_pos, you.y_pos, px, py) < 64);

                    if ((proximity == PROX_CLOSE_TO_PLAYER && !close_to_player)
                        || (proximity == PROX_AWAY_FROM_PLAYER && close_to_player))
                    {
                        proxOK = false;
                    }
                    break;

                case PROX_NEAR_STAIRS:
                    pval = near_stairs(px, py, 1, stair_gfx);

                    if (pval == 2)
                    {
                        // 0 speed monsters can't shove player out of their way.
                        if (mons_stationary( mclass ))
                        {
                            proxOK = false;
                            break;
                        }

                        // swap the monster and the player spots,  unless the
                        // monster was generated in lava or deep water.
                        if (grid_destroys_items( grd[px][py] ))
                        {
                            proxOK = false;
                            break;
                        }

                        shoved = true;
                        int tpx = px;
                        int tpy = py;
                        px = you.x_pos;
                        py = you.y_pos;
                        you.x_pos = tpx;
                        you.y_pos = tpy;
                    }

                    proxOK = (pval > 0);
                    break;
                default:
                    break;
            }

            if (!proxOK)
                continue;

            // cool.. passes all tests
            break;
        } // end while.. place first monster
    }

    id = place_monster_aux( mclass, behaviour, target, px, py, lev_mons,
                            extra, true );

    // now, forget about banding if the first placement failed,  or there's too
    // many monsters already,  or we successfully placed by stairs
    if (id < 0 || id+30 > MAX_MONSTERS)
        return false;

    // message to player from stairwell/gate appearance?
    if (see_grid(px, py) && proximity == PROX_NEAR_STAIRS)
    {
        info[0] = '\0';

        if (player_monster_visible( &menv[id] ))
            strcpy(info, ptr_monam( &menv[id], DESC_CAP_A ));
        else if (shoved)
            strcpy(info, "Something");

        if (shoved)
        {
            strcat(info, " shoves you out of the ");
            strcat(info, (stair_gfx == '>' || stair_gfx == '<') ? "stairwell!"
                                                                : "gateway!");
            mpr(info);
        }
        else if (info[0] != '\0')
        {
            strcat(info, (stair_gfx == '>') ? " comes up the stairs." :
                         (stair_gfx == '<') ? " comes down the stairs."
                                            : " comes through the gate.");
            mpr(info);
        }

        // special case: must update the view for monsters created in player LOS
        viewwindow(true, false);
    }

    // monsters placed by stairs don't bring the whole band up/down/through
    // with them
    if (proximity == PROX_NEAR_STAIRS)
        return (true);

    // (5) for each band monster, loop call to place_monster_aux().
    for(i = 1; i < band_size; i++)
    {
        place_monster_aux( band_monsters[i], behaviour, target, px, py,
                           lev_mons, extra, false );
    }

    // placement of first monster, at least, was a success.
    return (true);
}

// moved here from dungeon.cc
int place_unique_monsters( bool just_one, int prox )
{
    int total_placed = 0;
    int not_used = 0;           // used for ignored mid reference

    int depth = you.depth + 1;

    if (depth > 1
        && you.level_type == LEVEL_DUNGEON  // avoid temp levels
        && !player_in_hell()
        && !player_in_branch( BRANCH_ECUMENICAL_TEMPLE )
        && !player_in_branch( BRANCH_HIVE )
        && !player_in_branch( BRANCH_SLIME_PITS )
        && !player_in_branch( BRANCH_TOMB )
        && !player_in_branch( BRANCH_HALL_OF_BLADES )
        && (!player_in_branch( BRANCH_LAIR ) || one_chance_in(3)))
    {
        while (just_one
                || (player_in_branch( BRANCH_ORCISH_MINES ) && coinflip())
                || one_chance_in(3))
        {
            int unique = -1;

            // special code for areas:
            if (player_in_branch( BRANCH_ORCISH_MINES ))
            {
                if (!you.unique_creatures[ MONS_BLORK_THE_ORC - MONS_TERENCE ])
                    unique = MONS_BLORK_THE_ORC;

                if (!you.unique_creatures[ MONS_URUG - MONS_TERENCE ]
                    && coinflip())
                {
                    unique = MONS_URUG;
                }
            }

            while (unique < 0 || you.unique_creatures[ unique - MONS_TERENCE ])
            {
                // potential quit/reduce depth if unique was already placed.
                if (unique >= 0)
                {
                    if (depth >= 6 && !one_chance_in(3))
                        depth -= 3;
                    else if (one_chance_in(3))
                    {
                        unique = -1;  // causes double break
                        break;
                    }
                }

                unique = (depth >= 20) ? MONS_LOUISE  + random2(11) :
                         (depth >= 16) ? MONS_ERICA   + random2(13) :
                         (depth >= 12) ? MONS_MICHAEL + random2(11) :
                         (depth >=  9) ? MONS_PSYCHE  + random2(7)  :
                         (depth >=  6) ? MONS_SIGMUND + random2(7)  :
                         (depth >=  3) ? MONS_TERENCE + random2(6)  :
                         (coinflip()   ? MONS_TERENCE : MONS_IJYB);
            }

            // usually, we'll have quit after a few tries. Make sure we don't
            // create unique[-1] by accident.
            if (unique < 0)
                break;

            // note: unique_creatures 40 + used by unique demons
            if (place_monster( not_used, unique, you.depth, BEH_SLEEP,
                               MHITNOT, false, 1, 1, true, prox ))
            {
                total_placed++;
                if (just_one)
                    break;
            }
        }
    }

    return (total_placed);
}


static int place_monster_aux( int mclass, beh_type behaviour, int target,
                              int px, int py, int power, int extra,
                              bool first_band_member )
{
    int id, i;
    char grid_wanted;
    int fx=0, fy=0;     // final x,y

    // gotta be able to pick an ID
    for (id = 0; id < MAX_MONSTERS; id++)
    {
        if (menv[id].type == -1)
            break;
    }

    if (id == MAX_MONSTERS)
        return -1;

    // scrap monster inventory
    for (i = 0; i < NUM_MONSTER_SLOTS; i++)
        menv[id].inv[i] = NON_ITEM;

    // scrap monster enchantments
    clear_monster_enchants( id );

    // setup habitat and placement
    if (first_band_member)
    {
        fx = px;
        fy = py;
    }
    else
    {
        grid_wanted = monster_habitat( mclass );

        // we'll try 1000 times for a good spot
        for (i = 0; i < 1000; i++)
        {
            fx = px + random2(7) - 3;
            fy = py + random2(7) - 3;

            // occupied?
            if (mgrd[fx][fy] != NON_MONSTER)
                continue;

            // compatible - floor?
            if (grid_wanted == DNGN_FLOOR && grd[fx][fy] < DNGN_FLOOR)
                continue;

            // compatible - others (must match, except for deep water monsters
            // generated in shallow water)
            if ((grid_wanted != DNGN_FLOOR && grd[fx][fy] != grid_wanted)
                && (grid_wanted != DNGN_DEEP_WATER || grd[fx][fy] != DNGN_SHALLOW_WATER))
                continue;

            // don't generate monsters on top of teleport traps
            // (how do they get there?)
            int trap = trap_at_xy(fx, fy);
            if (trap != -1)
            {
                if (env.trap[trap].type == TRAP_TELEPORT)
                    continue;
            }

            // cool.. passes all tests
            break;
        }

        // did we really try 1000 times?
        if (i == 1000)
            return (-1);
    }

    // now, actually create the monster (wheeee!)
    menv[id].type = mclass;
    menv[id].number = MST_NO_SPELLS;
    menv[id].colour = mons_class_colour( mclass );

    menv[id].x = fx;
    menv[id].y = fy;

    // link monster into monster grid
    mgrd[fx][fy] = id;

    // generate a brand shiny new monster, or zombie
    if (mons_is_animated_undead( mclass ))
        define_zombie( id, extra, mclass, power );
    else
        define_monster( id );

    // The return of Boris is now handled in monster_die()...
    // not setting this for Boris here allows for multiple Borises
    // in the dungeon at the same time.  -- bwr
    if (mclass >= MONS_TERENCE && mclass <= MONS_BORIS)
    {
        you.unique_creatures[ mclass - MONS_TERENCE ] = 1;

#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "Created unique: %s",
              ptr_monam( &(menv[id]), DESC_PLAIN ) );
#endif
    }

    if (extra != 250)
        menv[id].number = extra;

    if (mclass == MONS_SHAPESHIFTER)
        set_mons_flag( &menv[id], MF_SHAPESHIFT );

    if (mclass == MONS_GLOWING_SHAPESHIFTER)
        set_mons_flag( &menv[id], MF_GLOW_SHAPESHIFT );

    if (behaviour == BEH_GUARD)
        menv[id].flags |= MF_GUARD;

    if ((mclass == MONS_BIG_FISH
            || mclass == MONS_GIANT_GOLDFISH
            || mclass == MONS_ELECTRICAL_EEL
            || mclass == MONS_JELLYFISH
            || mclass == MONS_WATER_ELEMENTAL
            || mclass == MONS_SWAMP_WORM)
        && grd[fx][fy] == DNGN_DEEP_WATER
        && !one_chance_in(5))
    {
        mons_submerge( &menv[id] );
    }

    if ((mclass == MONS_LAVA_WORM
            || mclass == MONS_LAVA_FISH
            || mclass == MONS_LAVA_SNAKE
            || mclass == MONS_SALAMANDER)
        && grd[fx][fy] == DNGN_LAVA
        && !one_chance_in(5))
    {
        mons_submerge( &menv[id] );
    }

    if (mclass == MONS_DANCING_WEAPON && extra != 1) // ie not from spell
    {
        give_item( id, power );

        if (menv[id].inv[MSLOT_WEAPON] != NON_ITEM)
            menv[id].colour = mitm[ menv[id].inv[MSLOT_WEAPON] ].colour;
    }
    else if (mons_itemuse( mclass ) >= MONUSE_STARTING_EQUIPMENT)
    {
        give_item( id, power );

        // Give these monsters a second weapon -- bwr
        if (mclass == MONS_TWO_HEADED_OGRE || mclass == MONS_ETTIN)
            give_item( id, power );
    }


    // give manticores 8 to 16 spike volleys.
    // they're not spellcasters so this doesn't screw anything up.
    if (mclass == MONS_MANTICORE)
        menv[id].number = 8 + roll_zdice(2,5);

    // set attitude, behaviour and target
    menv[id].attitude = ATT_HOSTILE;
    menv[id].behaviour = behaviour;
    menv[id].foe_memory = 0;

    // setting attitude will always make the
    // monster wander.. if you want sleeping
    // hostiles,  use BEH_SLEEP since the default
    // attitude is hostile.
    if (behaviour > NUM_BEHAVIOURS)
    {
        if (behaviour == BEH_FRIENDLY || behaviour == BEH_GOD_GIFT)
            menv[id].attitude = ATT_FRIENDLY;

        menv[id].behaviour = BEH_WANDER;
    }

    menv[id].foe = target;

    return (id);
}                               // end place_monster_aux()


static int choose_band( int mclass, int power, int &band_size )
{
    // init
    band_size = 0;
    int band = BAND_NO_BAND;

    switch (mclass)
    {
    case MONS_ORC:
        if (coinflip())
            break;
        // intentional fall-through {dlb}
    case MONS_URUG:
    case MONS_BLORK_THE_ORC:
    case MONS_ORC_WARRIOR:
        band = BAND_ORCS;             // orcs with some priests and wizards
        band_size = 2 + random2(3);
        break;

    case MONS_BIG_KOBOLD:
        if (power > 3)
        {
            band = BAND_KOBOLDS;
            band_size = 2 + random2(6);
        }
        break;

    case MONS_ORC_WARLORD:
        band_size = 5 + random2(5);   // warlords have large bands
        // intentional fall through
    case MONS_ORC_KNIGHT:
        band = BAND_ORC_KNIGHT;       // orcs + knight
        band_size += 3 + random2(4);
        break;

    case MONS_KILLER_BEE:
        band = BAND_KILLER_BEES;       // killer bees
        band_size = 2 + random2(4);
        break;

    case MONS_FLYING_SKULL:
        band = BAND_FLYING_SKULLS;       // flying skulls
        band_size = 2 + random2(4);
        break;
    case MONS_SLIME_CREATURE:
        band = BAND_SLIME_CREATURES;       // slime creatures
        band_size = 2 + random2(4);
        break;
    case MONS_YAK:
        band = BAND_YAKS;       // yaks
        band_size = 2 + random2(4);
        break;
    case MONS_UGLY_THING:
        band = BAND_UGLY_THINGS;       // ugly things
        band_size = 2 + random2(4);
        break;
    case MONS_HELL_HOUND:
        band = BAND_HELL_HOUNDS;       // hell hound
        band_size = 2 + random2(3);
        break;
    case MONS_JACKAL:
        band = BAND_JACKALS;      // jackal
        band_size = 1 + random2(3);
        break;
    case MONS_HELL_KNIGHT:
    case MONS_MARGERY:
        band = BAND_HELL_KNIGHTS;      // hell knight
        band_size = 4 + random2(4);
        break;
    case MONS_JOSEPHINE:
    case MONS_NECROMANCER:
    case MONS_VAMPIRE_MAGE:
        band = BAND_NECROMANCER;      // necromancer
        band_size = 4 + random2(4);
        break;
    case MONS_ORC_HIGH_PRIEST:
        band = BAND_ORC_HIGH_PRIEST;      // orc high priest
        band_size = 4 + random2(4);
        break;
    case MONS_GNOLL:
        band = BAND_GNOLLS;      // gnoll
        band_size = ((coinflip())? 3 : 2);
        break;
    case MONS_BUMBLEBEE:
        band = BAND_BUMBLEBEES;      // bumble bees
        band_size = 2 + random2(4);
        break;
    case MONS_CENTAUR:
    case MONS_CENTAUR_WARRIOR:
        if (power > 9 && one_chance_in(3))
        {
            band = BAND_CENTAURS;  // centaurs
            band_size = 2 + random2(4);
        }
        break;

    case MONS_YAKTAUR:
    case MONS_YAKTAUR_CAPTAIN:
        if (coinflip())
        {
            band = BAND_YAKTAURS;  // yaktaurs
            band_size = 2 + random2(3);
        }
        break;

    case MONS_DEATH_YAK:
        band = BAND_DEATH_YAKS;      // death yaks
        band_size = 2 + random2(4);
        break;
    case MONS_INSUBSTANTIAL_WISP:
        band = BAND_INSUBSTANTIAL_WISPS;      // wisps
        band_size = 4 + random2(5);
        break;
    case MONS_OGRE_MAGE:
        band = BAND_OGRE_MAGE;      // ogre mage
        band_size = 4 + random2(4);
        break;
    case MONS_BALRUG:
        band = BAND_BALRUG;      // RED gr demon
        band_size = 2 + random2(3);
        break;
    case MONS_CACODEMON:
        band = BAND_CACODEMON;      // BROWN gr demon
        band_size = 1 + random2(3);
        break;

    case MONS_EXECUTIONER:
        if (coinflip())
        {
            band = BAND_EXECUTIONER;  // DARKGREY gr demon
            band_size = 1 + random2(3);
        }
        break;

    case MONS_HELLWING:
        if (coinflip())
        {
            band = BAND_HELLWING;  // LIGHTGREY gr demon
            band_size = 1 + random2(4);
        }
        break;

    case MONS_DEEP_ELF_FIGHTER:
        if (coinflip())
        {
            band = BAND_DEEP_ELF_FIGHTER;  // deep elf warrior
            band_size = 3 + random2(4);
        }
        break;

    case MONS_DEEP_ELF_KNIGHT:
        if (coinflip())
        {
            band = BAND_DEEP_ELF_KNIGHT;  // deep elf knight
            band_size = 3 + random2(4);
        }
        break;

    case MONS_DEEP_ELF_HIGH_PRIEST:
        if (coinflip())
        {
            band = BAND_DEEP_ELF_HIGH_PRIEST;  // deep elf high priest
            band_size = 3 + random2(4);
        }
        break;

    case MONS_KOBOLD_DEMONOLOGIST:
        if (coinflip())
        {
            band = BAND_KOBOLD_DEMONOLOGIST;  // kobold demonologist
            band_size = 3 + random2(6);
        }
        break;

    case MONS_NAGA_MAGE:
    case MONS_NAGA_WARRIOR:
        band = BAND_NAGAS;      // Nagas
        band_size = 3 + random2(4);
        break;
    case MONS_WAR_DOG:
        band = BAND_WAR_DOGS;      // war dogs
        band_size = 2 + random2(4);
        break;
    case MONS_GREY_RAT:
        band = BAND_GREY_RATS;      // grey rats
        band_size = 4 + random2(6);
        break;
    case MONS_GREEN_RAT:
        band = BAND_GREEN_RATS;      // green rats
        band_size = 4 + random2(6);
        break;
    case MONS_ORANGE_RAT:
        band = BAND_ORANGE_RATS;      // orange rats
        band_size = 3 + random2(4);
        break;
    case MONS_SHEEP:
        band = BAND_SHEEP;      // sheep
        band_size = 3 + random2(5);
        break;
    case MONS_GHOUL:
        band = BAND_GHOULS;      // ghoul
        band_size = 2 + random2(3);
        break;
    case MONS_HOG:
        band = BAND_HOGS;      // hog
        band_size = 1 + random2(3);
        break;
    case MONS_GIANT_MOSQUITO:
        band = BAND_GIANT_MOSQUITOES;      // mosquito
        band_size = 1 + random2(3);
        break;
    case MONS_DEEP_TROLL:
        band = BAND_DEEP_TROLLS;      // deep troll
        band_size = 3 + random2(3);
        break;
    case MONS_HELL_HOG:
        band = BAND_HELL_HOGS;      // hell-hog
        band_size = 1 + random2(3);
        break;
    case MONS_BOGGART:
        band = BAND_BOGGARTS;      // boggart
        band_size = 2 + random2(3);
        break;
    case MONS_BLINK_FROG:
        band = BAND_BLINK_FROGS;      // blink frog
        band_size = 2 + random2(3);
        break;
    case MONS_SKELETAL_WARRIOR:
        band = BAND_SKELETAL_WARRIORS;      // skeletal warrior
        band_size = 2 + random2(3);
        break;
    // Journey -- Added Draconian Packs
    case MONS_WHITE_DRACONIAN:
    case MONS_RED_DRACONIAN:
    case MONS_PURPLE_DRACONIAN:
    case MONS_MOTTLED_DRACONIAN:
    case MONS_YELLOW_DRACONIAN:
    case MONS_BLACK_DRACONIAN:
    case MONS_GREEN_DRACONIAN:
    case MONS_PALE_DRACONIAN:
        if (power > 18 && one_chance_in(3))
        {
           band = BAND_DRACONIAN;
           band_size = 2 + random2(3);
        }
        break;
    case MONS_DRACONIAN_CALLER:
    case MONS_DRACONIAN_MONK:
    case MONS_DRACONIAN_SCORCHER:
    case MONS_DRACONIAN_KNIGHT:
    case MONS_DRACONIAN_ANNIHILATOR:
    case MONS_DRACONIAN_ZEALOT:
    case MONS_DRACONIAN_SHIFTER:
        if (power > 20)
        {
           band = BAND_DRACONIAN;
           band_size = 2 + random2(4);
        }
        break;
    } // end switch

    if (band != BAND_NO_BAND && band_size == 0)
        band = BAND_NO_BAND;

    if (band_size >= MAX_BAND)
        band_size = MAX_BAND - 1;

    return (band);
}

static int band_member( int band, int power )
{
    int mclass = -1;
    int temp_rand;

    if (band == BAND_NO_BAND)
        return -1;

    switch (band)
    {
    case BAND_KOBOLDS:
        mclass = MONS_KOBOLD;
        break;

    case BAND_ORCS:
        mclass = MONS_ORC;
        if (one_chance_in(5))
            mclass = MONS_ORC_WIZARD;
        if (one_chance_in(7))
            mclass = MONS_ORC_PRIEST;
        break;

    case BAND_ORC_KNIGHT:
    case BAND_ORC_HIGH_PRIEST:
        temp_rand = random2(30);
        mclass = ((temp_rand > 19) ? MONS_ORC :          // 10 in 30
                    (temp_rand > 12) ? MONS_ORC_WARRIOR :  //  7 in 30
                    (temp_rand > 10) ? MONS_WARG :         //  2 in 30
                    (temp_rand >  6) ? MONS_ORC_WIZARD :   //  4 in 30
                    (temp_rand >  2) ? MONS_ORC_PRIEST :   //  4 in 30
                    (temp_rand >  1) ? MONS_OGRE :         //  1 in 30
                    (temp_rand >  0) ? MONS_TROLL          //  1 in 30
                                     : MONS_ORC_SORCERER); //  1 in 30
        break;

    case BAND_KILLER_BEES:
        mclass = MONS_KILLER_BEE;
        break;

    case BAND_FLYING_SKULLS:
        mclass = MONS_FLYING_SKULL;
        break;

    case BAND_SLIME_CREATURES:
        mclass = MONS_SLIME_CREATURE;
        break;

    case BAND_YAKS:
        mclass = MONS_YAK;
        break;

    case BAND_UGLY_THINGS:
        mclass = ((power > 21 && one_chance_in(4)) ?
                                MONS_VERY_UGLY_THING : MONS_UGLY_THING);
        break;

    case BAND_HELL_HOUNDS:
        mclass = MONS_HELL_HOUND;
        break;

    case BAND_JACKALS:
        mclass = MONS_JACKAL;
        break;

    case BAND_GNOLLS:
        mclass = MONS_GNOLL;
        break;

    case BAND_BUMBLEBEES:
        mclass = MONS_BUMBLEBEE;
        break;

    case BAND_CENTAURS:
        mclass = MONS_CENTAUR;
        break;

    case BAND_YAKTAURS:
        mclass = MONS_YAKTAUR;
        break;

    case BAND_INSUBSTANTIAL_WISPS:
        mclass = MONS_INSUBSTANTIAL_WISP;
        break;

    case BAND_DEATH_YAKS:
        mclass = MONS_DEATH_YAK;
        break;

    case BAND_NECROMANCER:                // necromancer
        temp_rand = random2(13);
        mclass = ((temp_rand > 9) ? MONS_ZOMBIE_SMALL :   // 3 in 13
                    (temp_rand > 6) ? MONS_ZOMBIE_LARGE :   // 3 in 13
                    (temp_rand > 3) ? MONS_SKELETON_SMALL : // 3 in 13
                    (temp_rand > 0) ? MONS_SKELETON_LARGE   // 3 in 13
                                    : MONS_NECROPHAGE);     // 1 in 13
        break;

    case BAND_BALRUG:
        mclass = (coinflip()? MONS_NEQOXEC : MONS_ORANGE_DEMON);
        break;

    case BAND_CACODEMON:
        mclass = MONS_LEMURE;
        break;

    case BAND_EXECUTIONER:
        mclass = (coinflip() ? MONS_ABOMINATION_SMALL : MONS_ABOMINATION_LARGE);
        break;

    case BAND_HELLWING:
        mclass = (coinflip() ? MONS_HELLWING : MONS_SMOKE_DEMON);
        break;

    case BAND_DEEP_ELF_FIGHTER:    // deep elf fighter
        temp_rand = random2(11);
        mclass = ((temp_rand >  4) ? MONS_DEEP_ELF_SOLDIER : // 6 in 11
                    (temp_rand == 4) ? MONS_DEEP_ELF_FIGHTER : // 1 in 11
                    (temp_rand == 3) ? MONS_DEEP_ELF_KNIGHT :  // 1 in 11
                    (temp_rand == 2) ? MONS_DEEP_ELF_CONJURER :// 1 in 11
                    (temp_rand == 1) ? MONS_DEEP_ELF_MAGE      // 1 in 11
                                     : MONS_DEEP_ELF_PRIEST);  // 1 in 11
        break;

    case BAND_HELL_KNIGHTS:
        mclass = MONS_HELL_KNIGHT;
        if (one_chance_in(4))
            mclass = MONS_NECROMANCER;
        break;

    //case 12 is orc high priest

    case BAND_OGRE_MAGE:
        mclass = MONS_OGRE;
        if (one_chance_in(3))
            mclass = MONS_TWO_HEADED_OGRE;
        break;                  // ogre mage

        // comment does not match value (30, TWO_HEADED_OGRE) prior to
        // enum replacement [!!!] 14jan2000 {dlb}

    case BAND_DEEP_ELF_KNIGHT:                    // deep elf knight
        temp_rand = random2(208);
        mclass =
                ((temp_rand > 159) ? MONS_DEEP_ELF_SOLDIER :    // 23.08%
                 (temp_rand > 111) ? MONS_DEEP_ELF_FIGHTER :    // 23.08%
                 (temp_rand >  79) ? MONS_DEEP_ELF_KNIGHT :     // 15.38%
                 (temp_rand >  51) ? MONS_DEEP_ELF_MAGE :       // 13.46%
                 (temp_rand >  35) ? MONS_DEEP_ELF_PRIEST :     //  7.69%
                 (temp_rand >  19) ? MONS_DEEP_ELF_CONJURER :   //  7.69%
                 (temp_rand >   3) ? MONS_DEEP_ELF_SUMMONER :    // 7.69%
                 (temp_rand ==  3) ? MONS_DEEP_ELF_DEMONOLOGIST :// 0.48%
                 (temp_rand ==  2) ? MONS_DEEP_ELF_ANNIHILATOR : // 0.48%
                 (temp_rand ==  1) ? MONS_DEEP_ELF_SORCERER      // 0.48%
                                   : MONS_DEEP_ELF_DEATH_MAGE);  // 0.48%
        break;

    case BAND_DEEP_ELF_HIGH_PRIEST:                // deep elf high priest
        temp_rand = random2(16);
        mclass =
                ((temp_rand > 12) ? MONS_DEEP_ELF_SOLDIER :     // 3 in 16
                 (temp_rand >  9) ? MONS_DEEP_ELF_FIGHTER :     // 3 in 16
                 (temp_rand >  6) ? MONS_DEEP_ELF_PRIEST :      // 3 in 16
                 (temp_rand == 6) ? MONS_DEEP_ELF_MAGE :        // 1 in 16
                 (temp_rand == 5) ? MONS_DEEP_ELF_SUMMONER :    // 1 in 16
                 (temp_rand == 4) ? MONS_DEEP_ELF_CONJURER :    // 1 in 16
                 (temp_rand == 3) ? MONS_DEEP_ELF_DEMONOLOGIST :// 1 in 16
                 (temp_rand == 2) ? MONS_DEEP_ELF_ANNIHILATOR : // 1 in 16
                 (temp_rand == 1) ? MONS_DEEP_ELF_SORCERER      // 1 in 16
                                  : MONS_DEEP_ELF_DEATH_MAGE);  // 1 in 16
        break;

    case BAND_KOBOLD_DEMONOLOGIST:
        temp_rand = random2(13);
        mclass = ((temp_rand > 4) ? MONS_KOBOLD :             // 8 in 13
                    (temp_rand > 0) ? MONS_BIG_KOBOLD           // 4 in 13
                                    : MONS_KOBOLD_DEMONOLOGIST);// 1 in 13
        break;

    case BAND_NAGAS:
        mclass = MONS_NAGA;
        break;
    case BAND_WAR_DOGS:
        mclass = MONS_WAR_DOG;
        break;
    case BAND_GREY_RATS:
        mclass = MONS_GREY_RAT;
        break;
    case BAND_GREEN_RATS:
        mclass = MONS_GREEN_RAT;
        break;
    case BAND_ORANGE_RATS:
        mclass = MONS_ORANGE_RAT;
        break;
    case BAND_SHEEP:
        mclass = MONS_SHEEP;
        break;
    case BAND_GHOULS:
        mclass = (coinflip() ? MONS_GHOUL : MONS_NECROPHAGE);
        break;
    case BAND_DEEP_TROLLS:
        mclass = MONS_DEEP_TROLL;
        break;
    case BAND_HOGS:
        mclass = MONS_HOG;
        break;
    case BAND_HELL_HOGS:
        mclass = MONS_HELL_HOG;
        break;
    case BAND_GIANT_MOSQUITOES:
        mclass = MONS_GIANT_MOSQUITO;
        break;
    case BAND_BOGGARTS:
        mclass = MONS_BOGGART;
        break;
    case BAND_BLINK_FROGS:
        mclass = MONS_BLINK_FROG;
        break;
    case BAND_SKELETAL_WARRIORS:
        mclass = MONS_SKELETAL_WARRIOR;
        break;
    case BAND_DRACONIAN:
        temp_rand = random2( (power < 24) ? 24 : 37 );
        mclass =
                ((temp_rand > 35) ? MONS_DRACONIAN_CALLER :     // 1 in 34
                 (temp_rand > 33) ? MONS_DRACONIAN_KNIGHT :     // 2 in 34
                 (temp_rand > 31) ? MONS_DRACONIAN_MONK :       // 2 in 34
                 (temp_rand > 29) ? MONS_DRACONIAN_SHIFTER :    // 2 in 34
                 (temp_rand > 27) ? MONS_DRACONIAN_ANNIHILATOR :// 2 in 34
                 (temp_rand > 25) ? MONS_DRACONIAN_SCORCHER :   // 2 in 34
                 (temp_rand > 23) ? MONS_DRACONIAN_ZEALOT :     // 2 in 34
                 (temp_rand > 20) ? MONS_YELLOW_DRACONIAN :     // 3 in 34
                 (temp_rand > 17) ? MONS_GREEN_DRACONIAN :      // 3 in 34
                 (temp_rand > 14) ? MONS_BLACK_DRACONIAN :      // 3 in 34
                 (temp_rand > 11) ? MONS_WHITE_DRACONIAN :      // 3 in 34
                 (temp_rand >  8) ? MONS_PALE_DRACONIAN :       // 3 in 34
                 (temp_rand >  5) ? MONS_PURPLE_DRACONIAN :     // 3 in 34
                 (temp_rand >  2) ? MONS_MOTTLED_DRACONIAN :    // 3 in 34
                                    MONS_RED_DRACONIAN );       // 3 in 34
        break;
    }

    return (mclass);
}

// PUBLIC FUNCTION -- mons_place().

int mons_place( int mclass, beh_type behaviour, int target, bool summoned,
                int px, int py, int level_type, int proximity, int power,
                int extra )
{
    int mon_count = 0;
    int temp_rand;          // probabilty determination {dlb}
    bool permit_bands = false;

    for (int il = 0; il < MAX_MONSTERS; il++)
    {
        if (menv[il].type != -1)
            mon_count++;
    }

    if (mclass == WANDERING_MONSTER)
    {
        if (mon_count > 150)
            return (-1);

        mclass = RANDOM_MONSTER;
    }

    // all monsters have been assigned? {dlb}
    if (mon_count > MAX_MONSTERS - 2)
        return (-1);

    // this gives a slight challenge to the player as they ascend the
    // dungeon with the Orb
    if (you.char_direction == DIR_ASCENDING && mclass == RANDOM_MONSTER
        && you.level_type == LEVEL_DUNGEON)
    {
        temp_rand = random2(100);

        mclass = (temp_rand < 82) ? rand_demon( DEMON_RANDOM ) :
                 (temp_rand < 90) ? MONS_HELL_HOUND :
                 (temp_rand < 95) ? MONS_ABOMINATION_LARGE
                                  : MONS_ABOMINATION_SMALL;
    }

    if (!summoned
         && (mclass == RANDOM_MONSTER || level_type == LEVEL_PANDEMONIUM))
    {
        permit_bands = true;
    }

    int mid = -1;

    if (power < 0)
    {
        power = (level_type == LEVEL_PANDEMONIUM) ? PANDEMONIUM_DEPTH :
                (level_type == LEVEL_ABYSS)       ? ABYSS_DEPTH
                                                  : you.depth;
    }

    if (place_monster( mid, mclass, power, behaviour, target, summoned,
                       px, py, permit_bands, proximity, extra ) == false)
    {
        return (-1);
    }

    if (mid != -1)
    {
        struct monsters *const creation = &menv[mid];

        // look at special cases: CHARMED, FRIENDLY, HOSTILE, GOD_GIFT
        // alert summoned being to player's presence
        if (behaviour > NUM_BEHAVIOURS)
        {
            switch (behaviour)
            {
            case BEH_CHARMED:
                creation->attitude = ATT_HOSTILE;
                creation->flags |= MF_CREATED_FRIENDLY;
                mons_add_ench( creation, ENCH_CHARM );
                break;

            case BEH_GOD_GIFT:
                creation->flags |= (MF_CREATED_FRIENDLY | MF_GOD_GIFT);
                break;

            case BEH_FRIENDLY:
                creation->flags |= MF_CREATED_FRIENDLY;
                break;

            case BEH_GUARD:
                creation->flags |= MF_GUARD;
                break;

            default:
                break;
            }

            // make summoned being aware of player's presence
            behaviour_event( creation, ME_ALERT, MHITYOU );
        }
    }

    return (mid);
}                               // end mons_place()


int create_monster( int cls, beh_type beha, int dur,
                    int cr_x, int cr_y, int hitting, int zsec,
                    bool force_pos, int power )
{
    int summd = -1;
    FixedVector < char, 2 > empty;

    const unsigned char spcw = ((cls == RANDOM_MONSTER)
                                    ? static_cast<unsigned char>( DNGN_FLOOR )
                                    : monster_habitat( cls ));

    if (cr_x == -1)
    {
        cr_x = you.x_pos;
        cr_y = you.y_pos;
    }

    // never try to force the target to an occupied grid
    if ((you.x_pos == cr_x && you.y_pos == cr_y)
        || mgrd[cr_x][cr_y] != NON_MONSTER
        || !force_pos)
    {
        empty[0] = 0;
        empty[1] = 0;

        // Might be better if we chose a space and tried to match the monster
        // to it in the case of RANDOM_MONSTER, that way if the target square
        // is surrounded by lava this function would work smoothly. -- bwr
        if (!empty_surrounds( cr_x, cr_y, spcw, 2, true, true, empty ))
            return (-1);

        cr_x = empty[0];
        cr_y = empty[1];
    }

    summd = mons_place( cls, beha, hitting, true, cr_x, cr_y,
                        you.level_type, 0, power, zsec );

    // determine whether creating a monster is successful (summd != -1) {dlb}:
    // then handle the outcome {dlb}:
    if (summd == -1)
    {
        if (see_grid( cr_x, cr_y ))
            mpr("You see a puff of smoke.");
    }
    else
    {
        struct monsters *const creation = &menv[summd];

        // dur should always be ENCH_SUMMONED_xx
        if (dur)
            mons_add_ench( creation, ENCH_SUMMONED, MHITNOT, -1, dur );

        // look at special cases: CHARMED, FRIENDLY, HOSTILE, GOD_GIFT
        // alert summoned being to player's presence
        if (beha > NUM_BEHAVIOURS)
        {
            if (beha == BEH_FRIENDLY || beha == BEH_GOD_GIFT)
                creation->flags |= MF_CREATED_FRIENDLY;

            if (beha == BEH_GOD_GIFT)
                creation->flags |= MF_GOD_GIFT;

            if (beha == BEH_GUARD)
                creation->flags |= MF_GUARD;

            if (beha == BEH_CHARMED)
            {
                creation->attitude = ATT_HOSTILE;
                mons_add_ench( creation, ENCH_CHARM );
            }

            // make summoned being aware of player's presence
            behaviour_event( creation, ME_ALERT, MHITYOU );
        }

        if (creation->type == MONS_RAKSHASA_FAKE && !one_chance_in(3))
            mons_add_ench( creation, ENCH_INVIS );

#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "mon #%d (%s), cls = %d, (%d,%d)", summd,
                ptr_monam( &menv[summd], DESC_PLAIN ), menv[summd].type,
                menv[summd].x, menv[summd].y );
#endif
    }

    // the return value is either -1 (failure of some sort)
    // or the index of the monster placed (if I read things right) {dlb}
    return (summd);
}                               // end create_monster()

bool empty_surrounds( int emx, int emy, unsigned char spc_wanted,
                      int radius, bool allow_centre, bool check_los,
                      FixedVector < char, 2 > &empty )
{
    int good_count = 0;
    int count_x, count_y;

    for (count_x = -radius; count_x <= radius; count_x++)
    {
        for (count_y = -radius; count_y <= radius; count_y++)
        {
            if (!allow_centre && count_x == 0 && count_y == 0)
                continue;

            const int tx = emx + count_x;
            const int ty = emy + count_y;

            if (tx == you.x_pos && ty == you.y_pos)
                continue;

            if (mgrd[tx][ty] != NON_MONSTER)
                continue;

            // players won't summon out of LOS
            if (check_los && !check_line_of_sight( emx, emy, tx, ty ))
                continue;

            if (grd[tx][ty] == spc_wanted
                || (spc_wanted == DNGN_FLOOR
                    && grd[tx][ty] >= DNGN_SHALLOW_WATER)
                || (spc_wanted == DNGN_DEEP_WATER
                    && grd[tx][ty] == DNGN_SHALLOW_WATER))
            {
                good_count++;

                if (one_chance_in( good_count ))
                {
                    empty[0] = tx;
                    empty[1] = ty;
                }
            }
        }                       // end "for count_y"
    }                           // end "for count_x"

    return (good_count > 0);
}                               // end empty_surrounds()

monster_type rand_imp( void )
{
    monster_type mons_pick = MONS_IMP;

    const int tmp_rand = random2(20);

    switch (tmp_rand)
    {
    default:
    case 0:
    case 13: mons_pick = MONS_IMP;          break;
    case 1:
    case 14: mons_pick = MONS_BLINK_IMP;    break;
    case 2:
    case 3:  mons_pick = MONS_BRAIN_IMP;    break;
    case 4:
    case 5:  mons_pick = MONS_POISON_IMP;   break;
    case 6:  mons_pick = MONS_SWIFT_IMP;    break;
    case 7:  mons_pick = MONS_STONE_IMP;    break;
    case 8:
    case 9:  mons_pick = MONS_FLAME_IMP;    break;
    case 10: mons_pick = MONS_INVIS_IMP;    break;
    case 11: mons_pick = MONS_DART_IMP;     break;
    case 12:
    case 15: mons_pick = MONS_MORPH_IMP;    break;
    case 16:
    case 17: mons_pick = MONS_WHITE_IMP;    break;
    case 18: mons_pick = MONS_SHADOW_IMP;   break;
    case 19: mons_pick = MONS_SWARM_IMP;    break;
    }

    return (mons_pick);
}

monster_type rand_demon( demon_class_type type )
{
    monster_type  summoned = MONS_PROGRAM_BUG;
    int           temp_rand;

    if (type == DEMON_RANDOM)
        type = static_cast<demon_class_type>( random2(3) );

    switch (type)
    {
    case DEMON_LESSER:
        temp_rand = random2(60);
        summoned = ((temp_rand > 40) ? rand_imp() :      // 19 in 60
                    (temp_rand > 31) ? MONS_LEMURE :     //  9 in 60
                    (temp_rand > 22) ? MONS_UFETUBUS :   //  9 in 60
                    (temp_rand > 13) ? MONS_MANES :      //  9 in 60
                    (temp_rand > 4)  ? MONS_MIDGE        //  9 in 60
                                     : MONS_SHADOW_IMP); //  5 in 60
        break;

    case DEMON_COMMON:
        temp_rand = random2(3948);
        summoned = ((temp_rand > 3367) ? MONS_NEQOXEC :         // 14.69%
                    (temp_rand > 2787) ? MONS_ORANGE_DEMON :    // 14.69%
                    (temp_rand > 2207) ? MONS_HELLWING :        // 14.69%
                    (temp_rand > 1627) ? MONS_SMOKE_DEMON :     // 14.69%
                    (temp_rand > 1047) ? MONS_YNOXINUL :        // 14.69%
                    (temp_rand > 889)  ? MONS_RED_DEVIL :       //  4.00%
                    (temp_rand > 810)  ? MONS_HELLION :         //  2.00%
                    (temp_rand > 731)  ? MONS_ROTTING_DEVIL :   //  2.00%
                    (temp_rand > 652)  ? MONS_TORMENTOR :       //  2.00%
                    (temp_rand > 573)  ? MONS_REAPER :          //  2.00%
                    (temp_rand > 494)  ? MONS_SOUL_EATER :      //  2.00%
                    (temp_rand > 415)  ? MONS_HAIRY_DEVIL :     //  2.00%
                    (temp_rand > 336)  ? MONS_ICE_DEVIL :       //  2.00%
                    (temp_rand > 257)  ? MONS_BLUE_DEVIL :      //  2.00%
                    (temp_rand > 178)  ? MONS_BEAST :           //  2.00%
                    (temp_rand > 99)   ? MONS_IRON_DEVIL :      //  2.00%
                    (temp_rand > 49)   ? MONS_SUN_DEMON         //  1.26%
                                       : MONS_SHADOW_IMP);      //  1.26%
        break;

    case DEMON_GREATER:
        temp_rand = random2(1000);
        summoned = ((temp_rand > 868) ? MONS_CACODEMON :        // 13.1%
                    (temp_rand > 737) ? MONS_BALRUG :           // 13.1%
                    (temp_rand > 606) ? MONS_BLUE_DEATH :       // 13.1%
                    (temp_rand > 475) ? MONS_GREEN_DEATH :      // 13.1%
                    (temp_rand > 344) ? MONS_EXECUTIONER :      // 13.1%
                    (temp_rand > 244) ? MONS_FIEND :            // 10.0%
                    (temp_rand > 154) ? MONS_ICE_FIEND :        //  9.0%
                    (temp_rand > 73)  ? MONS_SHADOW_FIEND       //  8.1%
                                      : MONS_PIT_FIEND);        //  7.4%
        break;

    default:
        summoned = MONS_GIANT_ANT;      // this was the original behaviour {dlb}
        break;
    }

    return (summoned);
}                               // end rand_demon()

monster_type rand_dragon( dragon_class_type type )
{
    monster_type  summoned = MONS_PROGRAM_BUG;
    int temp_rand;

    switch (type)
    {
    case DRAGON_LIZARD:
        temp_rand = random2(100);
        summoned = ((temp_rand > 85) ? MONS_GIANT_GECKO :
                    (temp_rand > 60) ? MONS_GIANT_LIZARD :
                    (temp_rand > 35) ? MONS_GIANT_IGUANA :
                    (temp_rand > 25) ? MONS_GILA_MONSTER :
                    (temp_rand > 15) ? MONS_KOMODO_DRAGON :
                    (temp_rand > 12) ? MONS_FIRE_DRAKE :
                    (temp_rand >  9) ? MONS_SWAMP_DRAKE :
                    (temp_rand >  6) ? MONS_FROST_DRAKE :
                    (temp_rand >  3) ? MONS_SPARK_DRAKE
                                     : MONS_DEATH_DRAKE );
        break;

    case DRAGON_DRACONIAN:
        temp_rand = random2(70);
        summoned = ((temp_rand > 60) ? MONS_YELLOW_DRACONIAN :
                    (temp_rand > 50) ? MONS_BLACK_DRACONIAN :
                    (temp_rand > 40) ? MONS_PALE_DRACONIAN :
                    (temp_rand > 30) ? MONS_GREEN_DRACONIAN :
                    (temp_rand > 20) ? MONS_PURPLE_DRACONIAN :
                    (temp_rand > 10) ? MONS_RED_DRACONIAN
                                     : MONS_WHITE_DRACONIAN);
        break;

    case DRAGON_DRAGON:
        temp_rand = random2(90);
        summoned = ((temp_rand > 80) ? MONS_EARTHWURM :
                    (temp_rand > 70) ? MONS_LINDWURM :
                    (temp_rand > 60) ? MONS_STORM_DRAGON :
                    (temp_rand > 50) ? MONS_MOTTLED_DRAGON :
                    (temp_rand > 40) ? MONS_STEAM_DRAGON :
                    (temp_rand > 30) ? MONS_DRAGON :
                    (temp_rand > 20) ? MONS_ICE_DRAGON :
                    (temp_rand > 10) ? MONS_SWAMP_DRAGON
                                     : MONS_SHADOW_DRAGON);
        break;

    default:
        break;
    }

    return (summoned);
}

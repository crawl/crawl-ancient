/*
 *  File:       files.cc
 *  Summary:    Functions used to save and load levels/games.
 *  Written by: Linley Henzell and Alexey Guzeev
 *
 *  Change History (most recent first):
 *
 *   <7>   19 June 2000  GDL    Change handle to FILE *
 *   <6>   11/14/99      cdl    Don't let player ghosts follow you up/down
 *   <5>    7/13/99      BWR    Monsters now regenerate hps off level &
 ghosts teleport
 *   <4>    6/13/99      BWR    Added tmp file pairs to save file.
 *   <3>    6/11/99      DML    Replaced temp file deletion code.
 *
 *   <2>    5/12/99      BWR    Multiuser system support,
 *                                        including appending UID to
 *                                        name, and compressed saves
 *                                        in the SAVE_DIR_PATH directory
 *
 *   <1>   --/--/--      LRH    Created
 */

#include "AppHdr.h"
#include "files.h"

#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifdef DOS
#include <conio.h>
#include <file.h>
#endif

#ifdef LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#ifdef USE_EMX
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#ifdef OS9
#include <stat.h>
#else
#include <sys/stat.h>
#endif

#ifdef __MINGW32__
#include <io.h>
#endif

#include "globals.h"
#include "externs.h"

#include "cloud.h"
#include "debug.h"
#include "dungeon.h"
#include "itemname.h"
#include "itemprop.h"
#include "items.h"
#include "message.h"
#include "misc.h"
#include "monstuff.h"
#include "mon-util.h"
#include "mstuff2.h"
#include "player.h"
#include "randart.h"
#include "skills2.h"
#include "stash.h"
#include "stuff.h"
#include "tags.h"
#include "travel.h"
#include "view.h"

void save_level(int level_saved, bool was_a_labyrinth, char where_were_you);

/*
   Order for looking for conjurations for the 1st & 2nd spell slots,
   when finding spells to be remembered by a player's ghost:

 */
int search_order_conj[] =
{
/* 0 */
    SPELL_LEHUDIBS_CRYSTAL_SPEAR,
    SPELL_BOLT_OF_DRAINING,
    SPELL_BOLT_OF_IRON,
    SPELL_AGONY,
    SPELL_DISINTEGRATE,
    SPELL_LIGHTNING_BOLT,
    SPELL_STICKY_FLAME,
    SPELL_ISKENDERUNS_MYSTIC_BLAST,
    SPELL_BOLT_OF_FIRE,
    SPELL_BOLT_OF_COLD,
    SPELL_FIREBALL,
    SPELL_DELAYED_FIREBALL,
/* 10 */
    SPELL_POISON_ARROW,
    SPELL_VENOM_BOLT,
    SPELL_STONE_ARROW,
    SPELL_THROW_FLAME,
    SPELL_THROW_FROST,
    SPELL_PAIN,
    SPELL_STING,
    SPELL_MAGIC_DART,
    SPELL_NO_SPELL,                        // MUST BE LAST
};

const int num_search_order_conj = sizeof( search_order_conj ) / sizeof( int );

/*
   Order for looking for summonings and self-enchants for the 3rd spell slot:
 */
int search_order_third[] =
{
/* 0 */
    SPELL_SYMBOL_OF_TORMENT,
    SPELL_SUMMON_GREATER_DEMON,
    SPELL_SUMMON_WRAITHS,
    SPELL_SUMMON_HORRIBLE_THINGS,
    SPELL_SUMMON_DEMON,
    SPELL_DEMONIC_HORDE,
    SPELL_HASTE,
    SPELL_ANIMATE_DEAD,
    SPELL_INVISIBILITY,
    SPELL_BLINK,
    SPELL_CONTROLLED_BLINK,
    SPELL_CALL_IMP,
    SPELL_SUMMON_SMALL_MAMMAL,
    SPELL_DISPEL_UNDEAD,
/* 10 */
    SPELL_NO_SPELL,                        // MUST BE LAST
};

const int num_search_order_third = sizeof( search_order_third ) / sizeof( int );

/*
   Order for looking for enchants for the 4th + 5th spell slot. If fails, will
   go through conjs.
   Note: Dig must be in misc2 (5th) position to work.
 */
int search_order_misc[] =
{
/* 0 */
    SPELL_AGONY,
    SPELL_BANISHMENT,
    SPELL_PETRIFY,
    SPELL_CONFUSE,
    SPELL_SLOW,
    SPELL_POLYMORPH_OTHER,
    SPELL_TELEPORT_OTHER,
    SPELL_NO_SPELL,                        // MUST BE LAST
};

const int num_search_order_misc = sizeof( search_order_misc ) / sizeof( int );

/* Last slot (emergency) can only be teleport self or blink. */

int translate_spell( int spel );
int search_spell_list( int *const list, int ignore_spell = SPELL_NO_SPELL );

void add_spells( struct ghost_struct &gs );

static bool determine_version( FILE *restoreFile,
                               char &majorVersion, char &minorVersion );

static void restore_version( FILE *restoreFile,
                             char majorVersion, char minorVersion );

static bool determine_level_version( FILE *levelFile,
                                     char &majorVersion, char &minorVersion );

static void restore_level_version( FILE *levelFile,
                                   char majorVersion, char minorVersion );

static bool determine_ghost_version( FILE *ghostFile,
                                     char &majorVersion, char &minorVersion );

static void restore_ghost_version( FILE *ghostFile,
                                   char majorVersion, char minorVersion );

static void restore_tagged_file( FILE *restoreFile, int fileType,
                                 char minorVersion );

static void load_ghost();

void make_filename( char *buf, const char *prefix, int level, int where,
                    bool isLabyrinth, bool isGhost )
{
    UNUSED( isGhost );

    char suffix[4], lvl[5];
    char finalprefix[ kFileNameLen ];

    strcpy( suffix, (level < 10) ? "0" : "" );
    itoa( level, lvl, 10 );
    strcat( suffix, lvl );
    suffix[2] = where + 'a';
    suffix[3] = '\0';

    // init buf
    buf[0] = '\0';

#ifdef SAVE_DIR_PATH
    strcpy( buf, SAVE_DIR_PATH );
#endif

    strncpy( finalprefix, prefix, kFileNameLen );
    finalprefix[ kFileNameLen ] = '\0';

    strcat( buf, finalprefix );

#ifdef SAVE_DIR_PATH
    // everyone sees everyone else's ghosts. :)
    if (!isGhost)
    {
        char uid[10];

        itoa( static_cast<int>( getuid() ), uid, 10 );
        strcat( buf, "-" );
        strcat( buf, uid );
    }
#endif

    strcat( buf, "." );
    if (isLabyrinth)
        strcat( buf, "lab" );     // temporary level
    else
        strcat( buf, suffix );
}

static void write_tagged_file( FILE *dataFile, char majorVersion,
                               char minorVersion, int fileType )
{
    char tags[NUM_TAGS];

    struct tagHeader th;
    struct tagHeader versionTag;

    // find all relevant tags
    tag_set_expected( tags, fileType );

    // write version
    versionTag.offset = 0;
    versionTag.tagID = TAG_VERSION;

    marshallByte( versionTag, majorVersion );
    marshallByte( versionTag, minorVersion );
    tag_write( versionTag, dataFile );

    // all other tags
    for (int i = 1; i < NUM_TAGS; i++)
    {
        if (tags[i] == 1)
        {
            tag_construct(th, i);
            tag_write(th, dataFile);
        }
    }
}

static bool is_down_stair( unsigned char grid )
{
    return ((grid >= DNGN_STONE_STAIRS_DOWN_I
                && grid <= DNGN_ROCK_STAIRS_DOWN)
            || (grid >= DNGN_ENTER_ORCISH_MINES
                && grid <= DNGN_ENTER_SWAMP));
}

static bool is_up_stair( unsigned char grid )
{
    return ((grid >= DNGN_STONE_STAIRS_UP_I
                && grid <= DNGN_ROCK_STAIRS_UP)
            || (grid >= DNGN_RETURN_FROM_ORCISH_MINES
                && grid <= DNGN_RETURN_FROM_SWAMP));
}

static bool is_clear( unsigned char grid )
{
    return (grid >= DNGN_FLOOR
            && (grid < DNGN_TRAP_MECHANICAL
                || grid > DNGN_UNDISCOVERED_TRAP));
}

static int find_grid_class(bool (*func)(unsigned char), int &x_pos, int &y_pos)
{
    int found = 0;

    for (int count_x = 0; count_x < GXM; count_x++)
    {
        for (int count_y = 0; count_y < GYM; count_y++)
        {
            if (func( grd[count_x][count_y] ))
            {
                found++;
                if (one_chance_in( found ))
                {
                    x_pos = count_x;
                    y_pos = count_y;
                }
            }
        }
    }

    return (found);
}

void load( unsigned char stair_taken, int load_mode, bool was_a_labyrinth,
           char old_level, char where_were_you2 )
{
    int j = 0;
    int i = 0, count_x = 0, count_y = 0;
    char cha_fil[kFileNameSize];

    // XXX: clean this ugly mess up!
    int             foll_class[8];
    int             foll_hp[8];
    int             foll_hp_max[8];
    unsigned char   foll_HD[8];
    int             foll_AC[8];
    char            foll_ev[8];
    unsigned char   foll_speed[8];
    unsigned char   foll_speed_inc[8];
    int             foll_shield_blocks[8];

    unsigned char   foll_targ_1_x[8];
    unsigned char   foll_targ_1_y[8];
    beh_type        foll_beh[8];
    unsigned char   foll_att[8];
    int             foll_sec[8];
    int             foll_col[8];
    unsigned char   foll_hit[8];

    mon_enchant_def foll_ench[8][NUM_MON_ENCHANTS];
    unsigned char   foll_flags[8];

    item_def        foll_item[8][NUM_MONSTER_SLOTS];
    int             foll_spells[8][NUM_MONSTER_SPELL_SLOTS];

    int itmf = 0;
    int ic = 0;
    int imn = 0;
    int val;

    bool just_created_level = false;

#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif

    make_filename( cha_fil, you.your_name, you.depth, you.where_are_you,
                   (you.level_type != LEVEL_DUNGEON), false );

    if (you.level_type == LEVEL_DUNGEON)
    {
        if (env.level_files[you.depth][you.where_are_you] == false)
        {
            // make sure old file is gone
            unlink(cha_fil);

            // save the information for later deletion -- DML 6/11/99
            env.level_files[you.depth][you.where_are_you] = true;
        }
    }

    you.prev_targ = MHITNOT;

    int following = -1;
    int minvc = 0;

    // Don't delete clouds just because the player saved and restarted.
    if (load_mode != LOAD_RESTART_GAME)
    {
        for (int clouty = 0; clouty < MAX_CLOUDS; ++clouty)
            delete_cloud( clouty );

        ASSERT( env.cloud_no == 0 );
    }

    // This block is to grab followers and save the old level to disk.
    if (load_mode == LOAD_ENTER_LEVEL)
    {
        // grab followers
        for (count_x = you.x_pos - 1; count_x < you.x_pos + 2; count_x++)
        {
            for (count_y = you.y_pos - 1; count_y < you.y_pos + 2; count_y++)
            {
                if (count_x == you.x_pos && count_y == you.y_pos)
                    continue;

                following++;
                foll_class[following] = -1;

                if (mgrd[count_x][count_y] == NON_MONSTER)
                    continue;

                struct monsters *const fmenv = &menv[ mgrd[count_x][count_y] ];

                if (fmenv->type == MONS_PLAYER_GHOST
                    && fmenv->hit_points < fmenv->max_hit_points / 2)
                {
                    mpr("The ghost fades into the shadows.");
                    monster_teleport( fmenv, 0, true, true );
                    continue;
                }

                // monster has to be already tagged in order to follow:
                if (!testbits( fmenv->flags, MF_TAKING_STAIRS ))
                    continue;

#if DEBUG_DIAGNOSTICS
                mpr( MSGCH_DIAGNOSTICS, "%s is following.",
                      ptr_monam( fmenv, DESC_CAP_THE ) );
#endif

                foll_class[following] = fmenv->type;
                foll_hp[following] = fmenv->hit_points;
                foll_hp_max[following] = fmenv->max_hit_points;
                foll_HD[following] = fmenv->hit_dice;
                foll_AC[following] = fmenv->armour_class;
                foll_ev[following] = fmenv->evasion;
                foll_speed[following] = fmenv->speed;
                foll_speed_inc[following] = fmenv->energy;
                foll_shield_blocks[following] = fmenv->shield_blocks;
                foll_targ_1_x[following] = fmenv->target_x;
                foll_targ_1_y[following] = fmenv->target_y;

                for (minvc = 0; minvc < NUM_MONSTER_SLOTS; ++minvc)
                {
                    const int item = fmenv->inv[minvc];
                    if (item == NON_ITEM)
                    {
                        foll_item[following][minvc].quantity = 0;
                        continue;
                    }

                    foll_item[following][minvc] = mitm[item];
                    destroy_item( item );
                }

                for (j = 0; j < NUM_MONSTER_SPELL_SLOTS; ++j)
                    foll_spells[following][j] = fmenv->spells[j];

                foll_beh[following] = fmenv->behaviour;
                foll_att[following] = fmenv->attitude;
                foll_sec[following] = fmenv->number;
                foll_col[following] = fmenv->colour;
                foll_hit[following] = fmenv->foe;

                for (j = 0; j < NUM_MON_ENCHANTS; j++)
                {
                    foll_ench[following][j].type = fmenv->ench[j].type;
                    foll_ench[following][j].source = fmenv->ench[j].source;
                    foll_ench[following][j].duration = fmenv->ench[j].duration;
                    foll_ench[following][j].work = fmenv->ench[j].work;

                    fmenv->ench[j].type = ENCH_NONE;
                    fmenv->ench[j].source = MHITNOT;
                    fmenv->ench[j].duration = 0;
                    fmenv->ench[j].work = 0;
                }

                foll_flags[following] = fmenv->flags;

                fmenv->flags = 0;
                fmenv->type = -1;
                fmenv->hit_points = 0;
                fmenv->max_hit_points = 0;
                fmenv->hit_dice = 0;
                fmenv->armour_class = 0;
                fmenv->evasion = 0;

                mgrd[count_x][count_y] = NON_MONSTER;
            }
        }                        // end of grabbing followers

        if (!was_a_labyrinth)
            save_level( old_level, false, where_were_you2 );

        was_a_labyrinth = false;
    }

    // clear out ghost/demon lord information:
    strcpy( env.ghost.name, "" );
    for (ic = 0; ic < NUM_GHOST_VALUES; ++ic)
        env.ghost.values[ic] = 0;

#ifdef DOS
    strupr(cha_fil);
#endif

    // Try to open level savefile.
    FILE *levelFile = fopen(cha_fil, "rb");

    // GENERATE new level when the file can't be opened:
    if (levelFile == NULL)
    {
        strcpy( env.ghost.name, "" );

        for (imn = 0; imn < GVAL_SPELL_1; ++imn)
            env.ghost.values[imn] = 0;

        for (imn = GVAL_SPELL_1; imn < NUM_GHOST_VALUES; ++imn)
            env.ghost.values[imn] = MS_NO_SPELL;

        builder( you.depth, you.level_type );
        just_created_level = true;

        if (you.level_type == LEVEL_PANDEMONIUM)
            generate_random_demon();

        if (you.depth > 1 && one_chance_in(3))
            load_ghost();
    }
    else
    {
        // BEGIN -- must load the old level : pre-load tasks

        // LOAD various tags
        char majorVersion;
        char minorVersion;

        if (!determine_level_version( levelFile, majorVersion, minorVersion ))
        {
            perror("\nLevel file appears to be invalid.\n");
            end(-1);
        }

        restore_level_version( levelFile, majorVersion, minorVersion );

        // sanity check - EOF
        if (!feof( levelFile ))
        {
            snprintf( info, INFO_SIZE, "\nIncomplete read of \"%s\" - aborting.\n", cha_fil);
            perror(info);
            end(-1);
        }

        fclose( levelFile );

        // POST-LOAD tasks :
        link_items();
        set_redraw_status( REDRAW_ALL );
    }

    if (just_created_level)
        clear_envmap();

    // closes all the gates if you're on the way out
    for (i = 0; i < GXM; i++)
    {
        for (j = 0; j < GYM; j++)
        {
            if (you.char_direction == DIR_ASCENDING
                && you.level_type != LEVEL_PANDEMONIUM)
            {
                if (grd[i][j] == DNGN_ENTER_HELL
                    || grd[i][j] == DNGN_ENTER_ABYSS
                    || grd[i][j] == DNGN_ENTER_PANDEMONIUM)
                {
                    grd[i][j] = DNGN_STONE_ARCH;
                }
            }

            if (load_mode != LOAD_RESTART_GAME)
                env.cgrid[i][j] = EMPTY_CLOUD;
        }
    }

    // This next block is for cases where we want to look for a stairs
    // to place the player (ie not reloading the game).
    if (load_mode != LOAD_RESTART_GAME)
    {
        if (you.level_type == LEVEL_ABYSS)
        {
            you.x_pos = 45;
            you.y_pos = 35;
            grd[you.x_pos][you.y_pos] = DNGN_FLOOR;
        }
        else
        {
            bool find_first = true;

            // Order is important here:
            if (you.level_type == LEVEL_LABYRINTH)
            {
                stair_taken = DNGN_STONE_STAIRS_UP_I;
            }
            else if (you.level_type == LEVEL_DUNGEON
                && where_were_you2 == BRANCH_VESTIBULE_OF_HELL
                && stair_taken == DNGN_STONE_STAIRS_UP_I)
            {
                // leaving hell - look for entry potal first
                stair_taken = DNGN_ENTER_HELL;
                find_first = false;
            }
            else if (stair_taken == DNGN_EXIT_PANDEMONIUM)
            {
                stair_taken = DNGN_ENTER_PANDEMONIUM;
                find_first = false;
            }
            else if (stair_taken == DNGN_EXIT_ABYSS)
            {
                stair_taken = DNGN_ENTER_ABYSS;
                find_first = false;
            }
            else if (stair_taken == DNGN_ENTER_HELL)
            {
                // the vestibule starts from this stair
                stair_taken = DNGN_STONE_STAIRS_UP_I;
            }
            else if (stair_taken == DNGN_EXIT_HELL)
            {
                stair_taken = DNGN_STONE_STAIRS_DOWN_I + random2(3);
            }
            else if (stair_taken >= DNGN_STONE_STAIRS_DOWN_I
                && stair_taken <= DNGN_ROCK_STAIRS_DOWN)
            {
                // look for coresponding up stair
                stair_taken += (DNGN_STONE_STAIRS_UP_I - DNGN_STONE_STAIRS_DOWN_I);
            }
            else if (stair_taken >= DNGN_STONE_STAIRS_UP_I
                && stair_taken <= DNGN_ROCK_STAIRS_UP)
            {
                // look for coresponding down stair
                stair_taken += (DNGN_STONE_STAIRS_DOWN_I - DNGN_STONE_STAIRS_UP_I);
            }
            else if (stair_taken >= DNGN_RETURN_FROM_ORCISH_MINES
                && stair_taken < 150) // 20 slots reserved
            {
                // find entry point to subdungeon when leaving
                stair_taken += (DNGN_ENTER_ORCISH_MINES - DNGN_RETURN_FROM_ORCISH_MINES);
            }
            else if (stair_taken >= DNGN_ENTER_ORCISH_MINES
                 && stair_taken < DNGN_RETURN_FROM_ORCISH_MINES)
            {
                // find exit staircase from subdungeon when entering
                stair_taken += (DNGN_RETURN_FROM_ORCISH_MINES - DNGN_ENTER_ORCISH_MINES);
            }
            else if (stair_taken >= DNGN_ENTER_DIS
                && stair_taken <= DNGN_TRANSIT_PANDEMONIUM)
            {
                // when entering a hell or pandemonium
                stair_taken = DNGN_STONE_STAIRS_UP_I;
            }
            else // Note: stair_taken can equal things like DNGN_FLOOR
            {
                // just find a nice empty square
                stair_taken = DNGN_FLOOR;
                find_first = false;
            }

            int found = 0;
            int x_pos = 0, y_pos = 0;

            // Start by looking for the expected entry point:
            for (count_x = 0; count_x < GXM; count_x++)
            {
                for (count_y = 0; count_y < GYM; count_y++)
                {
                    if (grd[count_x][count_y] == stair_taken)
                    {
                        found++;
                        if (one_chance_in( found ))
                        {
                            x_pos = count_x;
                            y_pos = count_y;
                        }

                        if (find_first)
                            goto found_stair;  // double break
                    }
                }
            }

    found_stair:
            // second pass - look for stair in right direction
            if (!found)
            {
                if (stair_taken <= DNGN_ROCK_STAIRS_DOWN)
                    found = find_grid_class( is_down_stair, x_pos, y_pos );
                else
                    found = find_grid_class( is_up_stair, x_pos, y_pos );
            }

            // third pass - anything clear?
            if (!found)
                found = find_grid_class( is_clear, x_pos, y_pos );

            // If still not found, the level is very buggy.
            ASSERT( found );

            you.x_pos = x_pos;
            you.y_pos = y_pos;

            if (load_mode == LOAD_START_GAME)
            {
                // new game: move monsters away from player
                for (i = 0; i < MAX_MONSTERS; i++)
                {
                    if (menv[i].type == -1)
                        continue;

                    if (monster_habitat( menv[i].type ) != DNGN_FLOOR)
                        continue;

                    if (grid_distance( you.x_pos, you.y_pos,
                                       menv[i].x, menv[i].y ) <= 8)
                    {
                        monster_teleport( &menv[i], 0, true, true );
                    }
                }
            }

            if (you.level_type == LEVEL_LABYRINTH)
                grd[you.x_pos][you.y_pos] = DNGN_FLOOR;
        }
    }

    // This should fix the "monster occuring under the player" bug?
    if (mgrd[you.x_pos][you.y_pos] != NON_MONSTER)
        monster_teleport( &menv[mgrd[you.x_pos][you.y_pos]], 0, true, true );

    following = 0;

    // actually "move" the followers if applicable
    if ((you.level_type == LEVEL_DUNGEON
            || you.level_type == LEVEL_PANDEMONIUM)
        && load_mode == LOAD_ENTER_LEVEL)
    {
        for (int fcount = 0; fcount < 8; fcount++)
        {
            if (foll_class[fcount] == -1)
                continue;

            while (menv[following].type != -1)
            {
                following++;

                if (following >= MAX_MONSTERS)
                    goto out_of_foll;
            }

            menv[following].type = foll_class[fcount];
            menv[following].hit_points = foll_hp[fcount];
            menv[following].max_hit_points = foll_hp_max[fcount];
            menv[following].hit_dice = foll_HD[fcount];
            menv[following].armour_class = foll_AC[fcount];
            menv[following].evasion = foll_ev[fcount];
            menv[following].speed = foll_speed[fcount];
            menv[following].x = 0;  // monster moved later
            menv[following].y = 0;  // monster moved later
            menv[following].target_x = you.x_pos;
            menv[following].target_y = you.y_pos;
            menv[following].energy = foll_speed_inc[fcount];
            menv[following].shield_blocks = foll_shield_blocks[fcount];

            for (minvc = 0; minvc < NUM_MONSTER_SLOTS; minvc++)
            {

                if (!is_valid_item( foll_item[fcount][minvc] ))
                {
                    menv[following].inv[minvc] = NON_ITEM;
                    continue;
                }

                itmf = get_item_slot(0);
                if (itmf == NON_ITEM)
                {
                    menv[following].inv[minvc] = NON_ITEM;
                    continue;
                }

                mitm[itmf] = foll_item[fcount][minvc];
                mitm[itmf].x = 0;
                mitm[itmf].y = 0;
                mitm[itmf].link = NON_ITEM;
                mitm[itmf].slot = -1;

                menv[following].inv[minvc] = itmf;
            }

            for (j = 0; j < NUM_MONSTER_SPELL_SLOTS; ++j)
                menv[following].spells[j] = foll_spells[fcount][j];

            menv[following].behaviour = foll_beh[fcount];
            menv[following].attitude = foll_att[fcount];
            menv[following].number = foll_sec[fcount];
            menv[following].colour = foll_col[fcount];
            menv[following].foe = foll_hit[fcount];

            for (j = 0; j < NUM_MON_ENCHANTS; j++)
                menv[following].ench[j].type = foll_ench[fcount][j].type;

            menv[following].flags = foll_flags[fcount];

            if (!shift_monster( &menv[following], you.x_pos, you.y_pos ))
            {
                int nx = 0, ny = 0;

                if (!random_near_space( you.x_pos, you.y_pos, nx, ny, false, false ))
                {
                    const int hab = monster_habitat( menv[following].type );
                    random_in_bounds( nx, ny, hab, true );
                }

                move_monster_to_grid( following, nx, ny );
            }
        }
    }                       // end of moving followers

  out_of_foll:
    set_redraw_status( REDRAW_ALL );

    // Sanity forcing of monster inventory items (required?)
    for (i = 0; i < MAX_MONSTERS; i++)
    {
        if (menv[i].type == -1)
            continue;

        for (j = 0; j < NUM_MONSTER_SLOTS; j++)
        {
            if (menv[i].inv[j] == NON_ITEM)
                continue;

            /* items carried by monsters shouldn't be linked */
            if (mitm[ menv[i].inv[j] ].link != NON_ITEM)
                mitm[ menv[i].inv[j] ].link = NON_ITEM;
        }
    }

    if (just_created_level)
    {
        // Translate stairs for pandemonium/hell levels:
        if (you.level_type == LEVEL_PANDEMONIUM)
        {
            grd[you.x_pos][you.y_pos] = DNGN_FLOOR;

            for (count_x = 0; count_x < GXM; count_x++)
            {
                for (count_y = 0; count_y < GYM; count_y++)
                {
                    if (grd[count_x][count_y] >= DNGN_STONE_STAIRS_UP_I
                        && grd[count_x][count_y] <= DNGN_ROCK_STAIRS_UP)
                    {
                        if (grid_distance( you.x_pos, you.y_pos, count_x, count_y ) > 8
                            && one_chance_in( (you.mutation[MUT_PANDEMONIUM] > 1) ? 5 : 50 ))
                        {
                            grd[count_x][count_y] = DNGN_EXIT_PANDEMONIUM;
                        }
                        else
                        {
                            grd[count_x][count_y] = DNGN_FLOOR;
                        }
                    }

                    if (grd[count_x][count_y] >= DNGN_ENTER_LABYRINTH
                        && grd[count_x][count_y] <= DNGN_ROCK_STAIRS_DOWN)
                    {
                        grd[count_x][count_y] = DNGN_TRANSIT_PANDEMONIUM;
                    }
                }
            }
        }
        else if (player_in_hell() && you.depth == HELL_DEPTH)
        {
            for (count_x = 0; count_x < MAX_MONSTERS; count_x++)
            {
                struct monsters *const mon = &menv[count_x];

                if (mon->type >= MONS_DISPATER && mon->type <= MONS_ERESHKIGAL)
                {
                    grd[mon->x][mon->y] = DNGN_EXIT_HELL;
                    break;
                }
            }
        }
    }

    // Things to update for player entering level
    if (load_mode == LOAD_ENTER_LEVEL)
    {
        // update corpses and fountains
        if (env.elapsed_time < 1.0 && you.elapsed_time > env.elapsed_time)
            update_level( you.elapsed_time - env.elapsed_time );

        val = 10 * player_movement_speed();

        // -2 bonus goes away for quadraped on stairs:
        if (you.species == SP_CENTAUR && !player_is_shapechanged())
            val += 20;

        // stairs are difficult for the big:
        if (player_size( PSIZE_BODY ) > SIZE_LARGE)
            val = (val * 3) / 2;

        // previously visited levels are more wary:
        if (!just_created_level)
            val = (val * 3) / 2;

        val -= stepdown_value( player_stealth(), 50, 25, 150, 150 );

#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "arrival time: %0.1f",
                static_cast<float>( val ) / 10.0 );
#endif
        if (!you.running)
            more();                 // so we don't miss messages

        redraw_screen();

        if (val > 0)
        {
            you.time_taken = div_rand_round( val, 10 );

            if (you.time_taken > 15)
                you.time_taken = 15;

            handle_monsters();
        }
    }

    // Save the created/updated level out to disk:
    save_level( you.depth, (you.level_type != LEVEL_DUNGEON),
                you.where_are_you );
}                               // end load()

void save_level( int level_saved, bool was_a_labyrinth, char where_were_you )
{
    char cha_fil[kFileNameSize];

    make_filename( cha_fil, you.your_name, level_saved, where_were_you,
                   was_a_labyrinth, false );

    you.prev_targ = MHITNOT;

#ifdef DOS
    strupr( cha_fil );
#endif

    FILE *saveFile = fopen( cha_fil, "wb" );

    if (saveFile == NULL)
    {
        strcpy( info, "Unable to open \"" );
        strcat( info, cha_fil );
        strcat( info, "\" for writing!" );
        perror( info );
        end(-1);
    }

    // nail all items to the ground
    fix_item_coordinates();

    // 4.0 initial genesis of saved format
    // 4.1 added attitude tag
    // 4.2 replaced old 'enchantment1' and with 'flags' (bitfield)
    // 4.3 changes to make the item structure more sane
    // 4.4 changes to the ghost save section
    // 4.5 spell and ability letter arrays
    // 4.6 saved out shield blocks and extended env.map

    write_tagged_file( saveFile, 4, 6, TAGTYPE_LEVEL );

    fclose( saveFile );

#ifdef SHARED_FILES_CHMOD_PRIVATE
    chmod( cha_fil, SHARED_FILES_CHMOD_PRIVATE );
#endif
}                               // end save_level()

void save_game( bool leave_game )
{
    char charFile[kFileNameSize];

    // XXX: could be a problem with adding 4 here for OSes with short filenames
    char stashFile[kFileNameSize + 4];
    char killFile[kFileNameSize + 4];
    char travelCacheFile[kFileNameSize + 4];

#ifdef SAVE_PACKAGE_CMD
    char cmd_buff[1024];
    char name_buff[kFileNameSize];

    snprintf( name_buff, sizeof(name_buff),
              SAVE_DIR_PATH "%s-%d", you.your_name, static_cast<int>(getuid()) );

    snprintf( cmd_buff, sizeof(cmd_buff),
              SAVE_PACKAGE_CMD, name_buff, name_buff );

    strcpy( stashFile, name_buff );
    strcpy( killFile, name_buff );
    strcpy( travelCacheFile, name_buff );

    snprintf( charFile, sizeof(charFile), "%s.sav", name_buff );

#else
    strncpy( charFile, you.your_name, kFileNameLen );
    charFile[kFileNameLen] = 0;

    strcpy( stashFile, charFile );
    strcpy( killFile, charFile );
    strcpy( travelCacheFile, charFile );

    strcat( charFile, ".sav" );

#ifdef DOS
    strupr( charFile );
    strupr( stashFile );
    strupr( killFile );
    strupr( travelCacheFile );
#endif
#endif

    strcat(stashFile, ".st");
    strcat(killFile, ".kil");
    strcat(travelCacheFile, ".tc");

#ifdef STASH_TRACKING
    FILE *stashf = fopen(stashFile, "wb");
    if (stashf)
    {
        stashes.save(stashf);
        fclose(stashf);

#ifdef SHARED_FILES_CHMOD_PRIVATE
        // change mode (unices)
        chmod(stashFile, SHARED_FILES_CHMOD_PRIVATE);
#endif
    }
#endif // STASH_TRACKING

    FILE *travelf = fopen(travelCacheFile, "wb");
    if (travelf)
    {
        travel_cache.save(travelf);
        fclose(travelf);
#ifdef SHARED_FILES_CHMOD_PRIVATE
        // change mode (unices)
        chmod(travelCacheFile, SHARED_FILES_CHMOD_PRIVATE);
#endif
    }

    FILE *killf = fopen(killFile, "wb");
    if (killf)
    {
        you.kills.save(killf);
        fclose(killf);

#ifdef SHARED_FILES_CHMOD_PRIVATE
        // change mode (unices)
        chmod(killFile, SHARED_FILES_CHMOD_PRIVATE);
#endif
    }

    FILE *saveFile = fopen( charFile, "wb" );

    if (saveFile == NULL)
    {
        strcpy( info, "Unable to open \"" );
        strcat( info, charFile );
        strcat( info, "\" for writing!" );
        perror( info );
        end(-1);
    }

    // 4.0 initial genesis of saved format
    // 4.1 changes to make the item structure more sane
    // 4.2 spell and ability tables
    // 4.3 magic contamination and shield blocks saved out

    write_tagged_file( saveFile, 4, 3, TAGTYPE_PLAYER );

    fclose( saveFile );

#ifdef SHARED_FILES_CHMOD_PRIVATE
    // change mode (unices)
    chmod( charFile, SHARED_FILES_CHMOD_PRIVATE );
#endif

    // if just save, early out
    if (!leave_game)
        return;

    // must be exiting -- save level & goodbye!
    save_level(you.depth, (you.level_type != LEVEL_DUNGEON), you.where_are_you);

#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif

    clrscr();

#ifdef SAVE_PACKAGE_CMD
    // XXX: this should be replaced by a proper fork/exec
    if (system( cmd_buff ) != 0)
    {
        cprintf( EOL "Warning: Zip command (SAVE_PACKAGE_CMD) returned non-zero value!" EOL );
    }

#ifdef SHARED_FILES_CHMOD_PRIVATE
    strcat( name_buff, PACKAGE_SUFFIX );
    // change mode (unices)
    chmod( name_buff, SHARED_FILES_CHMOD_PRIVATE );
#endif

#endif

    cprintf( "See you soon, %s!" EOL, you.your_name );
    end(0);
}                               // end save_game()

void load_ghost( void )
{
    char majorVersion;
    char minorVersion;
    char cha_fil[kFileNameSize];
    int imn;
    int i;

    make_filename( cha_fil, "bones", you.depth, you.where_are_you,
                   (you.level_type != LEVEL_DUNGEON), true );

    FILE *gfile = fopen( cha_fil, "rb" );

    if (gfile == NULL)
        return;                 // no such env.ghost.

    if (!determine_ghost_version( gfile, majorVersion, minorVersion ))
    {
        fclose( gfile );

#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "Ghost file \"%s\" seems to be invalid.", cha_fil );
        more();
#endif
        return;
    }

    restore_ghost_version( gfile, majorVersion, minorVersion );

    // sanity check - EOF
    if (!feof( gfile ))
    {
        fclose( gfile );

#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "Incomplete read of \"%s\".", cha_fil );
        more();
#endif
        return;
    }

    fclose( gfile );

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Loaded ghost.");
#endif

    // remove bones file - ghosts are hardly permanent.
    unlink( cha_fil );

    // translate ghost to monster and place.
    for (imn = 0; imn < MAX_MONSTERS - 10; imn++)
    {
        if (menv[imn].type != -1)
            continue;

        menv[imn].type = MONS_PLAYER_GHOST;
        menv[imn].hit_dice = env.ghost.values[ GVAL_EXP_LEVEL ];
        menv[imn].hit_points = env.ghost.values[ GVAL_MAX_HP ];
        menv[imn].max_hit_points = env.ghost.values[ GVAL_MAX_HP ];
        menv[imn].armour_class = env.ghost.values[ GVAL_AC];
        menv[imn].evasion = env.ghost.values[ GVAL_EV ];
        menv[imn].speed = 10;
        menv[imn].energy = 70 + random2(10);
        menv[imn].shield_blocks = 0;
        menv[imn].attitude = ATT_HOSTILE;
        menv[imn].behaviour = BEH_WANDER;
        menv[imn].flags = 0;
        menv[imn].foe = MHITNOT;
        menv[imn].foe_memory = 0;
        menv[imn].colour = mons_class_colour( MONS_PLAYER_GHOST );

        menv[imn].number = 250;

        for (i = GVAL_SPELL_1; i <= GVAL_SPELL_6; i++)
        {
            if (env.ghost.values[i] != MS_NO_SPELL)
            {
                menv[imn].number = MST_GHOST;
                break;
            }
        }

        mons_load_spells( &menv[imn], MST_GHOST );

        for (i = 0; i < NUM_MONSTER_SLOTS; i++)
            menv[imn].inv[i] = NON_ITEM;

        clear_monster_enchants( imn );

        do
        {
            random_in_bounds( menv[imn].x, menv[imn].y );
        }
        while (mgrd[ menv[imn].x ][ menv[imn].y ] != NON_MONSTER);

        mgrd[ menv[imn].x ][ menv[imn].y ] = imn;
        break;
    }
}


void restore_game( void )
{
    char char_f[kFileNameSize];
    char stash_f[kFileNameSize];
    char kill_f[kFileNameSize];
    char travel_f[kFileNameSize];

#ifdef SAVE_DIR_PATH
    snprintf( char_f, sizeof(char_f),
              SAVE_DIR_PATH "%s-%d", you.your_name, static_cast<int>(getuid()) );
#else
    strncpy( char_f, you.your_name, kFileNameLen );
    char_f[kFileNameLen] = 0;
#endif


    strcpy( stash_f, char_f );
    strcpy( kill_f, stash_f );
    strcpy( travel_f, char_f );
    strcat( stash_f, ".st" );
    strcat( kill_f, ".kil" );
    strcat( travel_f, ".tc" );
    strcat( char_f, ".sav" );

#ifdef DOS
    strupr( char_f );
    strupr( stash_f );
    strupr( kill_f );
    strupr( travel_f );
#endif

    FILE *restoreFile = fopen( char_f, "rb" );

    if (restoreFile == NULL)
    {
        strcpy( info, "Unable to open \"" );
        strcat( info, char_f );
        strcat( info, "\" for reading!" );
        perror( info );
        end(-1);
    }

    char majorVersion;
    char minorVersion;

    if (!determine_version( restoreFile, majorVersion, minorVersion ))
    {
        perror( "\nSavefile appears to be invalid.\n" );
        end(-1);
    }

    restore_version( restoreFile, majorVersion, minorVersion );

    // sanity check - EOF
    if (!feof( restoreFile ))
    {
        snprintf( info, INFO_SIZE,
                  "\nIncomplete read of \"%s\" - aborting.\n", char_f);

        perror( info );
        end( -1 );
    }

    fclose( restoreFile );

#ifdef STASH_TRACKING
    FILE *stashFile = fopen(stash_f, "rb");
    if (stashFile)
    {
        stashes.load(stashFile);
        fclose(stashFile);
    }
#endif

    FILE *travelFile = fopen(travel_f, "rb");
    if (travelFile)
    {
        travel_cache.load(travelFile);
        fclose(travelFile);
    }

    FILE *killFile = fopen(kill_f, "rb");
    if (killFile)
    {
        you.kills.load(killFile);
        fclose(killFile);
    }
}

static bool determine_version( FILE *restoreFile,
                               char &majorVersion, char &minorVersion )
{
    // read first two bytes.
    char buf[2];

    if (read2( restoreFile, buf, 2 ) != 2)
        return (false);               // empty file?

    // check for 3.30
    if (buf[0] == you.your_name[0] && buf[1] == you.your_name[1])
    {
        majorVersion = 0;
        minorVersion = 0;

        rewind( restoreFile );
        return (true);
    }

    // otherwise, read version and validate.
    majorVersion = buf[0];
    minorVersion = buf[1];

    if (majorVersion == 1 || majorVersion == 4)
        return (true);

    return (false);   // if its not 1 or 4, no idea!
}

static void restore_version( FILE *restoreFile,
                             char majorVersion, char minorVersion )
{
    // assuming the following check can be removed once we can read all
    // savefile versions.
    if (majorVersion < 4)
    {
        snprintf( info, INFO_SIZE,
                  "\nSorry, this release cannot read a v%d.%d savefile.\n",
                   majorVersion, minorVersion );

        perror( info );
        end( -1 );
    }

    switch (majorVersion)
    {
        case 4:
            restore_tagged_file( restoreFile, TAGTYPE_PLAYER, minorVersion );
            break;
        default:
            break;
    }
}

// generic v4 restore function
static void restore_tagged_file( FILE *restoreFile, int fileType,
                                 char minorVersion )
{
    int  i;
    char tags[NUM_TAGS];

    tag_set_expected( tags, fileType );

    while (1)
    {
        i = tag_read( restoreFile, minorVersion );
        if (i == 0)                 // no tag!
            break;

        tags[i] = 0;                // tag read
    }

    // go through and init missing tags
    for (i = 0; i < NUM_TAGS; i++)
    {
        if (tags[i] == 1)           // expected but never read
            tag_missing( i, minorVersion );
    }
}

static bool determine_level_version( FILE *levelFile,
                                     char &majorVersion, char &minorVersion )
{
    // read first two bytes.
    char buf[2];

    if (read2( levelFile, buf, 2 ) != 2)
        return (false);               // empty file?

    // check for 3.30 -- simply started right in with player name.
    if (isprint( buf[0] ) && buf[0] > 4)      // who knows?
    {
        majorVersion = 0;
        minorVersion = 0;
        rewind( levelFile );
        return (true);
    }

    // otherwise, read version and validate.
    majorVersion = buf[0];
    minorVersion = buf[1];

    if (majorVersion == 1 || majorVersion == 4)
        return (true);

    return (false);   // if its not 1 or 4, no idea!
}

static void restore_level_version( FILE *levelFile,
                                   char majorVersion, char minorVersion )
{
    // assuming the following check can be removed once we can read all
    // savefile versions.
    if (majorVersion < 4)
    {
        snprintf( info, INFO_SIZE,
                  "\nSorry, this release cannot read a v%d.%d level file.\n",
                  majorVersion, minorVersion);

        perror( info );
        end( -1 );
    }

    switch (majorVersion)
    {
        case 4:
            restore_tagged_file( levelFile, TAGTYPE_LEVEL, minorVersion );
            break;
        default:
            break;
    }
}

static bool determine_ghost_version( FILE *ghostFile,
                                     char &majorVersion, char &minorVersion )
{
    // read first two bytes.
    char buf[2];

    if (read2( ghostFile, buf, 2 ) != 2)
        return (false);               // empty file?

    // check for pre-v4 -- simply started right in with ghost name.
    if (isprint( buf[0] ) && buf[0] > 4)
    {
        majorVersion = 0;
        minorVersion = 0;
        rewind( ghostFile );
        return (true);
    }

    // otherwise, read version and validate.
    majorVersion = buf[0];
    minorVersion = buf[1];

    if (majorVersion == 4)
        return (true);

    return (false);   // if its not 4, no idea!
}

static void restore_old_ghost( FILE *ghostFile )
{
    char buf[41];

    read2(ghostFile, buf, 41);  // 41 causes EOF. 40 will not.

    // translate
    memcpy( env.ghost.name, buf, 20 );

    for (int i = 0; i < 20; i++)
        env.ghost.values[i] = static_cast< unsigned short >( buf[i+20] );

    if (env.ghost.values[ GVAL_RES_FIRE ] >= 97)
        env.ghost.values[ GVAL_RES_FIRE ] -= 100;

    if (env.ghost.values[ GVAL_RES_COLD ] >= 97)
        env.ghost.values[ GVAL_RES_COLD ] -= 100;
}

static void restore_ghost_version( FILE *ghostFile,
                                   char majorVersion, char minorVersion )
{
    // currently, we can read all known ghost versions.
    switch (majorVersion)
    {
        case 4:
            restore_tagged_file( ghostFile, TAGTYPE_GHOST, minorVersion );
            break;
        case 0:
            restore_old_ghost( ghostFile );
            break;
        default:
            break;
    }
}

void save_ghost( bool force )
{
    char cha_fil[kFileNameSize];
    const int wpn = get_inv_wielded();

    if (!force && (you.depth < 2 || you.is_undead))
        return;

    make_filename( cha_fil, "bones", you.depth, you.where_are_you,
                   (you.level_type != LEVEL_DUNGEON), true );

    FILE *gfile = fopen(cha_fil, "rb");

    // don't overwrite existing bones!
    if (gfile != NULL)
    {
        fclose(gfile);
        return;
    }

    memcpy( env.ghost.name, you.your_name, 20 );

    env.ghost.values[ GVAL_MAX_HP ]    = (you.hp_max >= 250) ? 250 : you.hp_max;
    env.ghost.values[ GVAL_EV ]        = player_evasion();
    env.ghost.values[ GVAL_AC ]        = player_armour_class();
    env.ghost.values[ GVAL_SEE_INVIS ] = player_see_invis();
    env.ghost.values[ GVAL_RES_FIRE ]  = player_res_fire();
    env.ghost.values[ GVAL_RES_COLD ]  = player_res_cold();
    env.ghost.values[ GVAL_RES_ELEC ]  = player_res_electricity();

    /* note - as ghosts, automatically get res poison + prot_life */

    int d = 4;
    int e = 0;

    if (wpn != -1)
    {
        if (you.inv[wpn].base_type == OBJ_WEAPONS
            || you.inv[wpn].base_type == OBJ_STAVES)
        {
            d = property( you.inv[wpn], PWPN_DAMAGE );

            d *= 25 + you.skills[ weapon_skill(you.inv[wpn]) ];
            d /= 25;

            if (you.inv[wpn].base_type == OBJ_WEAPONS)
                e = get_weapon_brand( you.inv[wpn] );
        }
    }
    else
    {
        /* Unarmed combat */
        if (you.species == SP_TROLL)
            d += you.xp_level;

        d += you.skills[SK_UNARMED_COMBAT];
    }

    d *= 30 + you.skills[SK_FIGHTING];
    d /= 30;

    d += you.str / 4;

    if (d > 50)
        d = 50;

    const unsigned char best = static_cast<unsigned char>( best_skill( SK_FIGHTING, (NUM_SKILLS - 1) ) );

    env.ghost.values[ GVAL_DAMAGE ] = d;
    env.ghost.values[ GVAL_BRAND ]  = e;
    env.ghost.values[ GVAL_SPECIES ] = you.species;
    env.ghost.values[ GVAL_BEST_SKILL ] = best;
    env.ghost.values[ GVAL_SKILL_LEVEL ] = you.skills[best];
    env.ghost.values[ GVAL_EXP_LEVEL ] = you.xp_level;
    env.ghost.values[ GVAL_CLASS ] = you.char_class;

    add_spells( env.ghost );

    gfile = fopen( cha_fil, "wb" );

    if (gfile == NULL)
    {
        strcpy(info, "Error creating ghost file: ");
        strcat(info, cha_fil);
        mpr(info);
        more();
        return;
    }

    // 4.0-4.3  old tagged savefile (values as unsigned char)
    // 4.4      new tagged savefile (values as signed short)
    write_tagged_file( gfile, 4, 5, TAGTYPE_GHOST );

    fclose( gfile );

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Saved ghost.");
#endif

#ifdef SHARED_FILES_CHMOD_PUBLIC
    chmod( cha_fil, SHARED_FILES_CHMOD_PUBLIC );
#endif
}                               // end save_ghost()

/*
   When passed the number for a player spell, returns the equivalent monster
   spell. Returns SPELL_NO_SPELL on failure (no equiv).
 */
int translate_spell( int spel )
{
    switch (spel)
    {
    case SPELL_NO_SPELL:
        return (MS_NO_SPELL);
    case SPELL_TELEPORT_SELF:
        return (MS_TELEPORT);
    case SPELL_MAGIC_DART:
        return (MS_MMISSILE);
    case SPELL_FIREBALL:
    case SPELL_DELAYED_FIREBALL:
        return (MS_FIREBALL);
    case SPELL_DIG:
        return (MS_DIG);
    case SPELL_BOLT_OF_FIRE:
        return (MS_FIRE_BOLT);
    case SPELL_BOLT_OF_COLD:
        return (MS_COLD_BOLT);
    case SPELL_LIGHTNING_BOLT:
        return (MS_LIGHTNING_BOLT);
    case SPELL_POLYMORPH_OTHER:
        return (MS_MUTATION);
    case SPELL_SLOW:
        return (MS_SLOW);
    case SPELL_HASTE:
        return (MS_HASTE);
    case SPELL_PETRIFY:
        return (MS_PARALYSIS);
    case SPELL_CONFUSE:
        return (MS_CONFUSE);
    case SPELL_INVISIBILITY:
        return (MS_INVIS);
    case SPELL_THROW_FLAME:
        return (MS_FLAME);
    case SPELL_THROW_FROST:
        return (MS_FROST);
    case SPELL_CONTROLLED_BLINK:
        return (MS_CONTROLLED_BLINK);
/*  case FREEZING_CLOUD: return ; no freezing/mephitic cloud yet
   case MEPHITIC_CLOUD: return ; */
    case SPELL_VENOM_BOLT:
        return (MS_VENOM_BOLT);
    case SPELL_POISON_ARROW:
        return (MS_POISON_ARROW);
    case SPELL_TELEPORT_OTHER:
        return (MS_TELEPORT_OTHER);
    case SPELL_SUMMON_SMALL_MAMMAL:
        return (MS_VAMPIRE_SUMMON);       /* approximate */
    case SPELL_BOLT_OF_DRAINING:
        return (MS_NEGATIVE_BOLT);
    case SPELL_LEHUDIBS_CRYSTAL_SPEAR:
        return (MS_CRYSTAL_SPEAR);
    case SPELL_BLINK:
        return (MS_BLINK);
    case SPELL_ISKENDERUNS_MYSTIC_BLAST:
        return (MS_ORB_ENERGY);
    case SPELL_SUMMON_HORRIBLE_THINGS:
        return (MS_LEVEL_SUMMON); /* approximate */
    case SPELL_ANIMATE_DEAD:
        return (MS_ANIMATE_DEAD);
    case SPELL_PAIN:
        return (MS_PAIN);
    case SPELL_SUMMON_WRAITHS:
        return (MS_SUMMON_UNDEAD);        /* approximate */
    case SPELL_STICKY_FLAME:
        return (MS_STICKY_FLAME);
    case SPELL_CALL_IMP:
        return (MS_SUMMON_DEMON_LESSER);
    case SPELL_BANISHMENT:
        return (MS_BANISHMENT);
    case SPELL_STING:
        return (MS_STING);
    case SPELL_SUMMON_DEMON:
        return (MS_SUMMON_DEMON);
    case SPELL_DEMONIC_HORDE:
        return (MS_SUMMON_DEMON_LESSER);
    case SPELL_SUMMON_GREATER_DEMON:
        return (MS_SUMMON_DEMON_GREATER);
    case SPELL_BOLT_OF_IRON:
        return (MS_IRON_BOLT);
    case SPELL_STONE_ARROW:
        return (MS_STONE_ARROW);
    case SPELL_DISINTEGRATE:
        return (MS_DISINTEGRATE);
    case SPELL_AGONY:
    case SPELL_SYMBOL_OF_TORMENT:
        return (MS_TORMENT);
    case SPELL_DISPEL_UNDEAD:
        return (MS_DISPEL_UNDEAD);
    default:
        break;
    }

    return (MS_NO_SPELL);
}

int search_spell_list( int *const list, int skip_to )
{
    bool ignoring = (skip_to == SPELL_NO_SPELL) ? false : true;

    for (int *list_ptr = list; *list_ptr != SPELL_NO_SPELL; list_ptr++)
    {
        if (ignoring)
        {
            if (*list_ptr == skip_to)
                ignoring = false;

            continue;
        }

        if (player_has_spell( *list_ptr ))
            return (*list_ptr);
    }

    return (SPELL_NO_SPELL);
}

/*
   Used when creating ghosts: goes through and finds spells for the ghost to
   cast. Death is a traumatic experience, so ghosts only remember a few spells.
 */
void add_spells( struct ghost_struct &gs )
{
    int i = 0;

    for (i = GVAL_SPELL_1; i <= GVAL_SPELL_6; i++)
        gs.values[i] = SPELL_NO_SPELL;

    gs.values[ GVAL_SPELL_1 ] = search_spell_list( search_order_conj );

    gs.values[ GVAL_SPELL_2 ] = search_spell_list( search_order_conj,
                                                   gs.values[GVAL_SPELL_1] );

    gs.values[ GVAL_SPELL_3 ] = search_spell_list( search_order_third );
    gs.values[ GVAL_SPELL_4 ] = search_spell_list( search_order_misc );

    gs.values[ GVAL_SPELL_5 ] = search_spell_list( search_order_third,
                                                   gs.values[GVAL_SPELL_3] );

    if (gs.values[ GVAL_SPELL_5 ] == SPELL_NO_SPELL)
    {
        if (player_has_spell( SPELL_DIG ))
            gs.values[ GVAL_SPELL_5 ] = SPELL_DIG;
        else
        {
            gs.values[ GVAL_SPELL_5 ] = search_spell_list( search_order_misc,
                                                           gs.values[GVAL_SPELL_4] );
        }
    }

    if (gs.values[ GVAL_SPELL_4 ] == SPELL_NO_SPELL)
        gs.values[ GVAL_SPELL_4 ] = gs.values[ GVAL_SPELL_1 ];

    if (gs.values[ GVAL_SPELL_5 ] == SPELL_NO_SPELL)
        gs.values[ GVAL_SPELL_5 ] = gs.values[ GVAL_SPELL_2 ];

    /* Looks for blink/tport for emergency slot */
    if (player_has_spell( SPELL_CONTROLLED_BLINK )
        || player_has_spell( SPELL_BLINK )
        || player_has_spell( SPELL_TELEPORT_SELF ))
    {
        gs.values[ GVAL_SPELL_6 ] = SPELL_CONTROLLED_BLINK;
    }
    else
    {
        gs.values[ GVAL_SPELL_6 ] = gs.values[ GVAL_SPELL_3 ];
    }

    for (i = GVAL_SPELL_1; i <= GVAL_SPELL_6; i++)
        gs.values[i] = translate_spell( gs.values[i] );
}                               // end add_spells()

void generate_random_demon( void )
{
    int rdem = 0;
    int i = 0;
    int tmp;

    for (rdem = 0; rdem < MAX_MONSTERS + 1; rdem++)
    {
        if (rdem == MAX_MONSTERS)
            return;

        if (menv[rdem].type == MONS_PANDEMONIUM_DEMON)
            break;
    }

    char st_p[ITEMNAME_SIZE];

    make_name( random_int(), false, st_p );

    strncpy( env.ghost.name, st_p, kNameLen );
    env.ghost.name[ kNameLen - 1 ] = '\0';

    // hp - could be defined below (as could ev, AC etc). Oh well, too late:
    env.ghost.values[GVAL_MAX_HP] = 100 + roll_dice( 3, 50 );

    env.ghost.values[GVAL_EV] = 5 + random2(30);
    env.ghost.values[GVAL_AC] = 5 + random2(30);

    env.ghost.values[GVAL_SEE_INVIS] = (one_chance_in(10) ? 0 : 1);

    if (!one_chance_in(3))
        env.ghost.values[GVAL_RES_FIRE] = (coinflip() ? 2 : 3);
    else
        env.ghost.values[GVAL_RES_FIRE] = (one_chance_in(10) ? -1 : 0);

    if (!one_chance_in(3))
        env.ghost.values[GVAL_RES_COLD] = (coinflip() ? 2 : 3);
    else
        env.ghost.values[GVAL_RES_COLD] = (one_chance_in(10) ? -1 : 0);

    if (env.ghost.values[GVAL_RES_COLD] == -1 && env.ghost.values[GVAL_RES_FIRE] < 3)
        env.ghost.values[GVAL_RES_FIRE]++;

    if (env.ghost.values[GVAL_RES_FIRE] == -1 && env.ghost.values[GVAL_RES_COLD] < 3)
        env.ghost.values[GVAL_RES_COLD]++;

    // demons, like ghosts, automatically get poison res. and life prot.

    // resist electricity:
    env.ghost.values[GVAL_RES_ELEC] = (!one_chance_in(3) ? 1 : 0);

    // HTH damage:
    env.ghost.values[GVAL_DAMAGE] = 20 + roll_dice( 2, 20 );

    // special attack type (uses weapon brand code):
    env.ghost.values[GVAL_BRAND] = SPWPN_NORMAL;

    if (!one_chance_in(3))
    {
        env.ghost.values[GVAL_BRAND] = random2(17);

        /* some brands inappropriate (eg holy wrath) */
        if (env.ghost.values[ GVAL_BRAND ] == SPWPN_HOLY_WRATH
            || env.ghost.values[ GVAL_BRAND ] == SPWPN_DISRUPTION
            || env.ghost.values[ GVAL_BRAND ] == SPWPN_ORC_SLAYING
            || env.ghost.values[ GVAL_BRAND ] == SPWPN_PROTECTION)
        {
            env.ghost.values[ GVAL_BRAND ] = SPWPN_SPEED;
        }
    }

    // is demon a spellcaster?
    // upped from one_chance_in(3)... spellcasters are more interesting
    // and I expect named demons to typically have a trick or two -- bwr
    env.ghost.values[GVAL_DEMONLORD_SPELLCASTER] = (one_chance_in(10) ? 0 : 1);

    // does demon fly? (0 = no, 1 = fly, 2 = levitate)
    env.ghost.values[GVAL_DEMONLORD_FLY] = (one_chance_in(3) ? 0 :
                                        one_chance_in(5) ? 2 : 1);

    // vacant <ghost best skill level>:
    env.ghost.values[GVAL_DEMONLORD_UNUSED] = 0;

    // hit dice:
    env.ghost.values[GVAL_DEMONLORD_HIT_DICE] = 10 + roll_dice(2, 10);

    // does demon cycle colours?
    env.ghost.values[GVAL_DEMONLORD_CYCLE_COLOUR] = (one_chance_in(10) ? 1 : 0);

    menv[rdem].hit_dice = env.ghost.values[GVAL_DEMONLORD_HIT_DICE];
    menv[rdem].hit_points = env.ghost.values[GVAL_MAX_HP];
    menv[rdem].max_hit_points = env.ghost.values[GVAL_MAX_HP];
    menv[rdem].armour_class = env.ghost.values[GVAL_AC];
    menv[rdem].evasion = env.ghost.values[GVAL_EV];
    menv[rdem].speed = 8 + roll_dice(2, 6);
    menv[rdem].energy = 70 + random2(10);
    menv[rdem].shield_blocks = 0;
    menv[rdem].colour = element_colour( EC_RANDOM );
    menv[rdem].number = 0;

    /* This bit uses the list of player spells to find appropriate spells
       for the demon, then converts those spells to the monster spell indices.
       Some special monster-only spells are at the end. */
    if (!env.ghost.values[GVAL_DEMONLORD_SPELLCASTER])
    {
        for (i = GVAL_SPELL_1; i <= GVAL_SPELL_6; i++)
            env.ghost.values[i] = MS_NO_SPELL;
    }
    else
    {
        for (i = GVAL_SPELL_1; i <= GVAL_SPELL_6; i++)
            env.ghost.values[i] = SPELL_NO_SPELL;

        if (coinflip())
        {
            env.ghost.values[GVAL_SPELL_1] =
                    search_order_conj[ random2(num_search_order_conj) ];
        }

        if (coinflip())
        {
            env.ghost.values[GVAL_SPELL_2] =
                    search_order_conj[ random2(num_search_order_conj) ];
        }

        if (!one_chance_in(4))
        {
            env.ghost.values[GVAL_SPELL_3] =
                    search_order_third[ random2(num_search_order_third) ];
        }

        if (coinflip())
        {
            env.ghost.values[GVAL_SPELL_4] =
                    search_order_misc[ random2(num_search_order_misc) ] ;
        }

        if (coinflip())
        {
            env.ghost.values[GVAL_SPELL_5] =
                    search_order_misc[ random2(num_search_order_misc) ] ;
        }

        /* Converts the player spell indices to monster spell ones */
        for (i = GVAL_SPELL_1; i <= GVAL_SPELL_6; i++)
        {
            env.ghost.values[i] = translate_spell( env.ghost.values[i] );

            if (i != GVAL_SPELL_5 && env.ghost.values[i] == MS_DIG)
            {
                tmp = env.ghost.values[GVAL_SPELL_5];
                env.ghost.values[GVAL_SPELL_5] = MS_DIG;
                env.ghost.values[i] = tmp;
            }

            if (env.ghost.values[i] < 0 || env.ghost.values[i] > NUM_MONSTER_SPELLS)
                env.ghost.values[i] = MS_NO_SPELL;
        }

        /* give demon a chance for some monster-only spells: */
        /* and demon-summoning should be fairly common: */
        if (one_chance_in(25))
            env.ghost.values[GVAL_SPELL_1] = MS_HELLFIRE_BURST;
        if (one_chance_in(25))
            env.ghost.values[GVAL_SPELL_1] = MS_METAL_SPLINTERS;
        if (one_chance_in(25))
            env.ghost.values[GVAL_SPELL_1] = MS_ENERGY_BOLT;  /* eye of devas */

        if (one_chance_in(25))
            env.ghost.values[GVAL_SPELL_2] = MS_STEAM_BALL;
        if (one_chance_in(25))
            env.ghost.values[GVAL_SPELL_2] = MS_PURPLE_BLAST;

        if (one_chance_in(25))
        {
            env.ghost.values[GVAL_SPELL_2] = coinflip() ? MS_HELLFIRE
                                                        : MS_HELLFROST;
        }

        if (one_chance_in(25))
            env.ghost.values[GVAL_SPELL_3] = MS_SMITE;
        if (one_chance_in(25))
            env.ghost.values[GVAL_SPELL_3] = MS_HELLFIRE_BURST;
        if (one_chance_in(12))
            env.ghost.values[GVAL_SPELL_3] = MS_SUMMON_DEMON_GREATER;
        if (one_chance_in(12))
            env.ghost.values[GVAL_SPELL_3] = MS_SUMMON_DEMON;

        /* at least they can summon demons */
        if (env.ghost.values[GVAL_SPELL_4] == MS_NO_SPELL)
        {
            env.ghost.values[GVAL_SPELL_4] = (coinflip() ? MS_SUMMON_DEMON
                                                     : MS_SUMMON_DEMON_GREATER);
        }

        if (one_chance_in(15))
            env.ghost.values[GVAL_SPELL_5] = MS_DIG;

        tmp = random2(20);
        env.ghost.values[GVAL_SPELL_6] = ((tmp < 10) ? MS_CONTROLLED_BLINK :
                                      (tmp < 13) ? MS_SUMMON_DEMON :
                                      (tmp < 15) ? MS_SUMMON_DEMON_GREATER :
                                      (tmp < 17) ? MS_HEAL :
                                      (tmp < 18) ? MS_TORMENT :
                                      (tmp < 19) ? MS_HELLFIRE_BURST
                                                 : MS_TELEPORT_OTHER);
    }

    mons_load_spells( &menv[rdem], MST_GHOST );
}                               // end generate_random_demon()

// Largest string we'll save
#define STR_CAP 1000

// XXX: these, of course, are essentially duplications of the mashalling
// functions over in tags.cc -- icky!
void writeShort( FILE *file, short s )
{
    char data[2];

    // High byte first - network order
    data[0] = static_cast<char>( (s >> 8) & 0xFF );
    data[1] = static_cast<char>( s & 0xFF );

    write2( file, data, sizeof(data) );
}

short readShort( FILE *file )
{
    unsigned char data[2];

    read2( file, reinterpret_cast<char *>( data ), 2 );

    // High byte first
    return (((static_cast<short>( data[0] )) << 8)
            | static_cast<short>( data[1] ));
}

void writeByte( FILE *file, unsigned char byte )
{
    write2( file, reinterpret_cast<char *>( &byte ), sizeof( byte ) );
}

unsigned char readByte( FILE *file )
{
    unsigned char byte;

    read2( file, reinterpret_cast<char *>( &byte ), sizeof( byte ) );

    return (byte);
}

void writeString( FILE* file, const std::string &s )
{
    int length = s.length();

    if (length > STR_CAP)
        length = STR_CAP;

    writeShort( file, length );
    write2( file, s.c_str(), length );
}

std::string readString( FILE *file )
{
    char buf[STR_CAP + 1];
    short length = readShort(file);

    if (length)
        read2( file, buf, length );

    buf[length] = '\0';
    return (std::string(buf));
}

void writeLong( FILE* file, long num )
{
    // High word first, network order
    writeShort( file, static_cast<short>( (num >> 16) & 0xFFFFL ) );
    writeShort( file, static_cast<short>( num & 0xFFFFL ) );
}

long readLong( FILE *file )
{
    // We need the unsigned short cast even for the high word because we
    // might be on a system where long is more than 4 bytes, and we don't want
    // to sign extend the high short.
    return (static_cast<long>( static_cast<unsigned short>(readShort(file)) << 16 )
            | static_cast<long>( static_cast<unsigned short>(readShort(file)) ));
}

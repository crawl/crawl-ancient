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

#ifdef MAC
#include <stat.h>
#else
#include <sys/stat.h>
#endif

#include "externs.h"

#include "cloud.h"
#include "debug.h"
#include "dungeon.h"
#include "itemname.h"
#include "items.h"
#include "message.h"
#include "misc.h"
#include "monstuff.h"
#include "mon-util.h"
#include "mstuff2.h"
#include "player.h"
#include "randart.h"
#include "skills2.h"
#include "stuff.h"
#include "tags.h"
#include "wpn-misc.h"

void save_level(int level_saved, bool was_a_labyrinth, char where_were_you);

// temp file pairs used for file level cleanup
extern FixedArray < bool, MAX_LEVELS, MAX_BRANCHES > tmp_file_pairs;

/*
   Order for looking for conjurations for the 1st & 2nd spell slots,
   when finding spells to be remembered by a player's ghost:
 */
unsigned char search_order_conj[] = {
/* 0 */
    SPELL_LEHUDIBS_CRYSTAL_SPEAR,
    SPELL_BOLT_OF_DRAINING,
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
    SPELL_VENOM_BOLT,
    SPELL_BOLT_OF_IRON,
    SPELL_STONE_ARROW,
    SPELL_THROW_FLAME,
    SPELL_THROW_FROST,
    SPELL_PAIN,
    SPELL_STING,
    SPELL_MAGIC_DART,
    SPELL_NO_SPELL,                        // end search
};

/*
   Order for looking for summonings and self-enchants for the 3rd spell slot:
 */
unsigned char search_order_third[] = {
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
    SPELL_CALL_IMP,
    SPELL_SUMMON_SMALL_MAMMAL,
/* 10 */
    SPELL_CONTROLLED_BLINK,
    SPELL_BLINK,
    SPELL_NO_SPELL,                        // end search
};

/*
   Order for looking for enchants for the 4th + 5th spell slot. If fails, will
   go through conjs.
   Note: Dig must be in misc2 (5th) position to work.
 */
unsigned char search_order_misc[] = {
/* 0 */
    SPELL_AGONY,
    SPELL_BANISHMENT,
    SPELL_PARALYZE,
    SPELL_CONFUSE,
    SPELL_SLOW,
    SPELL_POLYMORPH_OTHER,
    SPELL_TELEPORT_OTHER,
    SPELL_DIG,
    SPELL_NO_SPELL,                        // end search
};

/* Last slot (emergency) can only be teleport self or blink. */

static void redraw_all(void)
{
    you.redraw_hit_points = 1;
    you.redraw_magic_points = 1;
    you.redraw_strength = 1;
    you.redraw_intelligence = 1;
    you.redraw_dexterity = 1;
    you.redraw_armour_class = 1;
    you.redraw_evasion = 1;
    you.redraw_experience = 1;
    you.redraw_gold = 1;
    you.redraw_hunger = 1;
}

struct ghost_struct ghost;

bool find_spell(unsigned char which_sp);

unsigned char translate_spell(unsigned char spel);
unsigned char search_third_list(unsigned char ignore_spell);
unsigned char search_second_list(unsigned char ignore_spell);
unsigned char search_first_list(unsigned char ignore_spell);

void add_spells(struct ghost_struct &gh);
void generate_random_demon();

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

void make_filename( char *buf, char *prefix, int level, int where,
                    bool isLabyrinth, bool isGhost )
{
    char suffix[4], lvl[5];
    char finalprefix[kFileNameLen];

    strcpy(suffix, (level < 10) ? "0" : "");
    itoa(level, lvl, 10);
    strcat(suffix, lvl);
    suffix[2] = where + 97;
    suffix[3] = '\0';

    // init buf
    buf[0] = '\0';

#ifdef SAVE_DIR_PATH
    strcpy(buf, SAVE_DIR_PATH);
#endif

    strncpy(finalprefix, prefix, kFileNameLen);
    finalprefix[kFileNameLen] = '\0';

    strcat(buf, finalprefix);

#ifdef SAVE_DIR_PATH
    // everyone sees everyone else's ghosts. :)
    char uid[10];
    if (!isGhost)
    {
        itoa( (int) getuid(), uid, 10 );
        strcat(buf, uid);
    }
#endif

    strcat(buf, ".");
    if (isLabyrinth)
        strcat(buf, "lab");     // temporary level
    else
        strcat(buf, suffix);
}

static void write_tagged_file( FILE *dataFile, char majorVersion,
                               char minorVersion, int fileType )
{
    struct tagHeader th;

    // find all relevant tags
    char tags[NUM_TAGS];
    tag_set_expected(tags, fileType);

    // write version
    struct tagHeader versionTag;
    versionTag.offset = 0;
    versionTag.tagID = TAG_VERSION;
    marshallByte(versionTag, majorVersion);
    marshallByte(versionTag, minorVersion);
    tag_write(versionTag, dataFile);

    // all other tags
    for(int i=1; i<NUM_TAGS; i++)
    {
        if (tags[i] == 1)
        {
            tag_construct(th, i);
            tag_write(th, dataFile);
        }
    }
}

void load( unsigned char stair_taken, int load_mode, bool was_a_labyrinth,
           char old_level, char where_were_you2 )
{
    int j = 0;
    int i = 0, count_x = 0, count_y = 0;
    char cha_fil[kFileNameSize];

    int foll_class[8];
    int foll_hp[8];
    int foll_hp_max[8];
    unsigned char foll_HD[8];
    int foll_AC[8];
    char foll_ev[8];
    unsigned char foll_speed[8];
    unsigned char foll_speed_inc[8];

    unsigned char foll_targ_1_x[8];
    unsigned char foll_targ_1_y[8];
    int foll_inv[8][8];
    unsigned char foll_beh[8];
    unsigned char foll_att[8];
    int foll_sec[8];
    unsigned char foll_hit[8];

    unsigned char foll_ench[8][NUM_MON_ENCHANTS];
    unsigned char foll_flags[8];

    item_def foll_item[8][8];

    int itmf = 0;
    int ic = 0;
    int imn = 0;
    int val;

    bool just_created_level = false;

#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif

    make_filename( cha_fil, you.your_name, you.your_level, you.where_are_you,
                   you.level_type != LEVEL_DUNGEON, false );

    if (you.level_type == LEVEL_DUNGEON)
    {
        if (tmp_file_pairs[you.your_level][you.where_are_you] == false)
        {
            // make sure old file is gone
            unlink(cha_fil);

            // save the information for later deletion -- DML 6/11/99
            tmp_file_pairs[you.your_level][you.where_are_you] = true;
        }
    }

    you.prev_targ = MHITNOT;

    int following = -1;
    int fmenv = 0;     // not used again until after found_stair label {dlb}
    int minvc = 0;

    // Don't delete clouds just because the player saved and restarted.
    if (load_mode != LOAD_RESTART_GAME)
    {
        for (int clouty = 0; clouty < MAX_CLOUDS; ++clouty)
        {
            delete_cloud( clouty );
        }

        ASSERT( env.cloud_no == 0 );
    }

    // This block is to grab followers and save the old level to disk.
    if (load_mode == LOAD_ENTER_LEVEL)
    {
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

                struct monsters *fmenv = &menv[mgrd[count_x][count_y]];

                if (fmenv->type == MONS_PLAYER_GHOST
                    && fmenv->hit_points < fmenv->max_hit_points / 2)
                {
                    mpr("The ghost fades into the shadows.");
                    monster_teleport(fmenv, true);
                    continue;
                }

                // monster has to be already tagged in order to follow:
                if (!testbits( fmenv->flags, MF_TAKING_STAIRS ))
                    continue;

#if DEBUG_DIAGNOSTICS
                snprintf( info, INFO_SIZE, "%s is following.",
                          ptr_monam( fmenv, DESC_CAP_THE ) );
                mpr( info, MSGCH_DIAGNOSTIC );
#endif

                foll_class[following] = fmenv->type;
                //itoa(foll_class[following], st_prn, 10);

                foll_hp[following] = fmenv->hit_points;
                foll_hp_max[following] = fmenv->max_hit_points;
                foll_HD[following] = fmenv->hit_dice;
                foll_AC[following] = fmenv->armour_class;
                foll_ev[following] = fmenv->evasion;
                foll_speed[following] = fmenv->speed;
                foll_speed_inc[following] = fmenv->speed_increment;
                foll_targ_1_x[following] = fmenv->target_x;
                foll_targ_1_y[following] = fmenv->target_y;

                for (minvc = 0; minvc < NUM_MONSTER_SLOTS; ++minvc)
                {
                    foll_inv[following][minvc] = NON_ITEM;
                }

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

                foll_beh[following] = fmenv->behaviour;
                foll_att[following] = fmenv->attitude;
                foll_sec[following] = fmenv->number;
                foll_hit[following] = fmenv->foe;

                for (j = 0; j < NUM_MON_ENCHANTS; j++)
                {
                    foll_ench[following][j] = fmenv->enchantment[j];
                    fmenv->enchantment[j] = ENCH_NONE;
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
            }                   // end "for count_x, count_y"
        }

        if (!was_a_labyrinth)
            save_level( old_level, false, where_were_you2 );

        was_a_labyrinth = false;
    }

    strcpy(ghost.name, "");

    for (ic = 0; ic < 20; ++ic)
    {
        ghost.values[ic] = 0;
    }

#ifdef DOS
    strupr(cha_fil);
#endif

    FILE *levelFile = fopen(cha_fil, "rb");

    // GENERATE new level when the file can't be opened:
    if (levelFile == NULL)
    {
        strcpy(ghost.name, "");

        for (imn = 0; imn < 20; ++imn)
        {
            ghost.values[imn] = 0;
        }

        builder(you.your_level, you.level_type);
        just_created_level = true;

        if (you.level_type == LEVEL_PANDEMONIUM)
            generate_random_demon();

        if (you.your_level > 1 && one_chance_in(3))
            load_ghost();

        // closes all the gates if you're on the way out
        for (i = 0; i < GXM; i++)
        {
            for (j = 0; j < GYM; j++)
            {
                env.map[i][j] = 0;

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
                env.cgrid[i][j] = EMPTY_CLOUD;
            }
        }

        // This next block is for cases where we want to look
        // for a stairs to place the player.
        if (load_mode != LOAD_RESTART_GAME)
        {
            if (stair_taken == DNGN_ENTER_HELL
                || stair_taken == DNGN_ENTER_LABYRINTH)
            {
                stair_taken = DNGN_STONE_STAIRS_UP_I;
            }
            else if (stair_taken <= DNGN_ROCK_STAIRS_DOWN)
                stair_taken += 4;
            else if (stair_taken >= DNGN_RETURN_DUNGEON_I && stair_taken < 150)
                stair_taken -= 20;
            else if (stair_taken >= DNGN_ENTER_ORCISH_MINES
                     && stair_taken < DNGN_RETURN_DUNGEON_I)
            {
                stair_taken += 20;
            }
            else if (stair_taken > 90)
                stair_taken = 86;
            else if (stair_taken == DNGN_FLOOR)
                stair_taken = DNGN_FLOOR;
            else
                stair_taken -= 4;

            for (count_x = 0; count_x < GXM; count_x++)
            {
                for (count_y = 0; count_y < GYM; count_y++)
                {
                    if (grd[count_x][count_y] == stair_taken)
                        goto found_stair;
                }
            }

            if (stair_taken <= DNGN_ROCK_STAIRS_DOWN)
                stair_taken = DNGN_STONE_STAIRS_DOWN_I;
            else
                stair_taken = DNGN_STONE_STAIRS_UP_I;


            for (count_x = 0; count_x < GXM; count_x++)
            {
                for (count_y = 0; count_y < GYM; count_y++)
                {
                    if (grd[count_x][count_y] == stair_taken)
                        goto found_stair;
                }
            }

            for (count_x = 0; count_x < GXM; count_x++)
            {
                for (count_y = 0; count_y < GYM; count_y++)
                {
                    if (grd[count_x][count_y] == DNGN_FLOOR)
                        goto found_stair;
                }
            }
        }

      found_stair:
        if (load_mode != LOAD_RESTART_GAME)
        {
            you.x_pos = count_x;
            you.y_pos = count_y;
        }

        // This should fix the "monster occuring under the player" bug?
        if (mgrd[you.x_pos][you.y_pos] != NON_MONSTER)
            monster_teleport(&menv[mgrd[you.x_pos][you.y_pos]], true);

        if (you.level_type == LEVEL_LABYRINTH
            || you.level_type == LEVEL_ABYSS)
        {
            grd[you.x_pos][you.y_pos] = DNGN_FLOOR;
        }

        following = 0;
        fmenv = -1;

        // actually "move" the followers if applicable
        if ((you.level_type == LEVEL_DUNGEON
                || you.level_type == LEVEL_PANDEMONIUM)
            && load_mode == LOAD_ENTER_LEVEL)
        {
            for (ic = 0; ic < 2; ic++)
            {
                for (count_x = you.x_pos - 6; count_x < you.x_pos + 7;
                     count_x++)
                {
                    for (count_y = you.y_pos - 6; count_y < you.y_pos + 7;
                         count_y++)
                    {
                        if (ic == 0
                            && ((count_x < you.x_pos - 1)
                                || (count_x > you.x_pos + 1)
                                || (count_y < you.y_pos - 1)
                                || (count_y > you.y_pos + 1)))
                        {
                            continue;
                        }

                        if (count_x == you.x_pos && count_y == you.y_pos)
                            continue;

                        if (mgrd[count_x][count_y] != NON_MONSTER
                            || grd[count_x][count_y] < DNGN_FLOOR)
                        {
                            continue;
                        }

                        while (menv[following].type != -1)
                        {
                            following++;

                            if (following >= MAX_MONSTERS)
                                goto out_of_foll;
                        }

                        while (fmenv < 7)
                        {
                            fmenv++;

                            if (foll_class[fmenv] == -1)
                                continue;

                            menv[following].type = foll_class[fmenv];
                            menv[following].hit_points = foll_hp[fmenv];
                            menv[following].max_hit_points = foll_hp_max[fmenv];
                            menv[following].hit_dice = foll_HD[fmenv];
                            menv[following].armour_class = foll_AC[fmenv];
                            menv[following].evasion = foll_ev[fmenv];
                            menv[following].speed = foll_speed[fmenv];
                            menv[following].x = count_x;
                            menv[following].y = count_y;
                            menv[following].target_x = 0;
                            menv[following].target_y = 0;
                            menv[following].speed_increment
                                                    = foll_speed_inc[fmenv];

                            for (minvc = 0; minvc < NUM_MONSTER_SLOTS; minvc++)
                            {

                                if (!is_valid_item(foll_item[fmenv][minvc]))
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

                                mitm[itmf] = foll_item[fmenv][minvc];
                                mitm[itmf].x = 0;
                                mitm[itmf].y = 0;
                                mitm[itmf].link = NON_ITEM;

                                menv[following].inv[minvc] = itmf;
                            }

                            menv[following].behaviour = foll_beh[fmenv];
                            menv[following].attitude = foll_att[fmenv];
                            menv[following].number = foll_sec[fmenv];
                            menv[following].foe = foll_hit[fmenv];

                            for (j = 0; j < NUM_MON_ENCHANTS; j++)
                                menv[following].enchantment[j]=foll_ench[fmenv][j];
                            menv[following].flags = foll_flags[fmenv];
                            menv[following].flags |= MF_JUST_SUMMONED;

                            mgrd[count_x][count_y] = following;
                            break;
                        }
                    }
                }
            }
        }                       // end if level_type == LEVEL_DUNGEON

      out_of_foll:
        redraw_all();
        // moving_level = false;

        for (i = 0; i < MAX_MONSTERS; i++)
        {
            if (menv[i].type == -1)
                continue;

            for (j = 0; j < NUM_MONSTER_SLOTS; j++)
            {
                if (menv[i].inv[j] == NON_ITEM)
                    continue;

                if (mitm[menv[i].inv[j]].link != NON_ITEM)
                {
                    /* items carried by monsters shouldn't be linked */
                    mitm[menv[i].inv[j]].link = NON_ITEM;
                }
            }
        }

        if (you.level_type == LEVEL_PANDEMONIUM)
        {
            for (count_x = 0; count_x < GXM; count_x++)
            {
                for (count_y = 0; count_y < GYM; count_y++)
                {
                    if (grd[count_x][count_y] >= DNGN_STONE_STAIRS_UP_I
                        && grd[count_x][count_y] <= DNGN_ROCK_STAIRS_UP)
                    {
                        if (one_chance_in( you.mutation[MUT_PANDEMONIUM] ? 15
                                                                         : 50 ))
                        {
                            grd[count_x][count_y] = DNGN_EXIT_PANDEMONIUM;
                        }
                        else
                            grd[count_x][count_y] = DNGN_FLOOR;
                    }

                    if (grd[count_x][count_y] >= DNGN_ENTER_LABYRINTH
                        && grd[count_x][count_y] <= DNGN_ROCK_STAIRS_DOWN)
                    {
                        grd[count_x][count_y] = DNGN_TRANSIT_PANDEMONIUM;
                    }
                }
            }
        }

        if (load_mode == LOAD_ENTER_LEVEL)
        {
            // update corpses and fountains
            if (env.elapsed_time != 0.0)
                update_level( you.elapsed_time - env.elapsed_time );

            // Centaurs have difficulty with stairs
            val = ((you.species != SP_CENTAUR) ? player_movement_speed() : 15);

            // new levels have less wary monsters:
            if (just_created_level)
                val /= 2;

            val -= (stepdown_value( check_stealth(), 50, 50, 150, 150 ) / 10);

#if DEBUG_DIAGNOSTICS
                snprintf( info, INFO_SIZE, "arrival time: %d", val );
                mpr( info, MSGCH_DIAGNOSTIC );
#endif

            if (val > 0)
            {
                you.time_taken = val;
                handle_monsters();
            }
        }

        save_level( you.your_level, (you.level_type != LEVEL_DUNGEON),
                    you.where_are_you );
        return;
    }

    // BEGIN -- must load the old level : pre-load tasks

    // moving_level = false;

    // LOAD various tags
    char majorVersion;
    char minorVersion;

    if (!determine_level_version(levelFile, majorVersion, minorVersion))
    {
        perror("\nLevel file appears to be invalid.\n");
        end(-1);
    }

    restore_level_version(levelFile, majorVersion, minorVersion);

    // sanity check - EOF
    if (!feof(levelFile))
    {
        snprintf( info, INFO_SIZE, "\nIncomplete read of \"%s\" - aborting.\n", cha_fil);
        perror(info);
        end(-1);
    }

    fclose(levelFile);

    // POST-LOAD tasks :

    link_items();

    redraw_all();

    // closes certain gates if you're on the way out
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
        }
    }

    if (stair_taken == DNGN_FLOOR)
    {
        for (count_x = 0; count_x < GXM; count_x++)
        {
            for (count_y = 0; count_y < GYM; count_y++)
            {
                if (grd[count_x][count_y] == stair_taken)
                    goto found_stair;
            }
        }
    }

    if (stair_taken >= DNGN_RETURN_DUNGEON_I && stair_taken < 150)
        stair_taken -= 20;
    else if (stair_taken >= DNGN_ENTER_ORCISH_MINES
             && stair_taken <= DNGN_ENTER_SWAMP)
    {
        stair_taken += 20;
    }
    else if (stair_taken < DNGN_STONE_STAIRS_UP_I)
        stair_taken += 4;
    else
        stair_taken -= 4;

    for (count_x = 0; count_x < GXM; count_x++)
    {
        for (count_y = 0; count_y < GYM; count_y++)
        {
            if (grd[count_x][count_y] == stair_taken)
                goto found_stair;
        }
    }

    if (stair_taken < DNGN_STONE_STAIRS_UP_I)
        stair_taken = DNGN_STONE_STAIRS_DOWN_I;
    else
        stair_taken = DNGN_STONE_STAIRS_UP_I;

    for (count_x = 0; count_x < GXM; count_x++)
    {
        for (count_y = 0; count_y < GYM; count_y++)
        {
            if (grd[count_x][count_y] == stair_taken)
                goto found_stair;
        }
    }

    if (env.elapsed_time != 0.0)
        update_level( you.elapsed_time - env.elapsed_time );
}                               // end load()

void save_level(int level_saved, bool was_a_labyrinth, char where_were_you)
{
    char cha_fil[kFileNameSize];

    make_filename(cha_fil, you.your_name, level_saved, where_were_you,
        was_a_labyrinth, false);

    you.prev_targ = MHITNOT;

#ifdef DOS
    strupr(cha_fil);
#endif

    FILE *saveFile = fopen(cha_fil, "wb");

    if (saveFile == NULL)
    {
        strcpy(info, "Unable to open \"");
        strcat(info, cha_fil );
        strcat(info, "\" for writing!");
        perror(info);
        end(-1);
    }

    // nail all items to the ground
    fix_item_coordinates();

    // 4.0 initial genesis of saved format
    // 4.1 added attitude tag
    // 4.2 replaced old 'enchantment1' and with 'flags' (bitfield)
    // 4.3 changes to make the item structure more sane

    write_tagged_file( saveFile, 4, 3, TAGTYPE_LEVEL );

    fclose(saveFile);

#ifdef SHARED_FILES_CHMOD_PRIVATE
    chmod(cha_fil, SHARED_FILES_CHMOD_PRIVATE);
#endif
}                               // end save_level()

void save_game(bool leave_game)
{
    char charFile[kFileNameSize];
    char cmd_buff[1024];

#ifdef SAVE_PACKAGE_CMD
    char name_buff[kFileNameSize];

    snprintf( name_buff, sizeof(name_buff),
              SAVE_DIR_PATH "%s%d", you.your_name, (int) getuid() );

    snprintf( cmd_buff, sizeof(cmd_buff),
              SAVE_PACKAGE_CMD, name_buff, name_buff );

    snprintf( charFile, sizeof(charFile),
              "%s.sav", name_buff );

#else
    strncpy(charFile, you.your_name, kFileNameLen);
    charFile[kFileNameLen] = 0;
    strcat(charFile, ".sav");

#ifdef DOS
    strupr(charFile);
#endif
#endif

    FILE *saveFile = fopen(charFile, "wb");

    if (saveFile == NULL)
    {
        strcpy(info, "Unable to open \"");
        strcat(info, charFile );
        strcat(info, "\" for writing!");
        perror(info);
        end(-1);
    }

    // 4.0 initial genesis of saved format
    // 4.1 changes to make the item structure more sane

    write_tagged_file( saveFile, 4, 1, TAGTYPE_PLAYER );

    fclose(saveFile);

#ifdef SHARED_FILES_CHMOD_PRIVATE
    // change mode (unices)
    chmod(charFile, SHARED_FILES_CHMOD_PRIVATE);
#endif

    // if just save, early out
    if (!leave_game)
        return;

    // must be exiting -- save level & goodbye!
    save_level(you.your_level, (you.level_type != LEVEL_DUNGEON),
               you.where_are_you);

#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif

    clrscr();

#ifdef SAVE_PACKAGE_CMD
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

    cprintf( "See you soon, %s!", you.your_name );

    end(0);
}                               // end save_game()

void load_ghost(void)
{
    char cha_fil[kFileNameSize];

    make_filename(cha_fil, "bones", you.your_level, you.where_are_you,
        you.level_type != LEVEL_DUNGEON, true);

    FILE *gfile = fopen(cha_fil, "rb");

    if (gfile == NULL)
        return;                 // no such ghost.

    char majorVersion;
    char minorVersion;

    if (!determine_ghost_version(gfile, majorVersion, minorVersion))
    {
        fclose(gfile);
        snprintf( info, INFO_SIZE, "Ghost file \"%s\" seems to be invalid.",
            cha_fil);
        mpr(info, MSGCH_WARN);
        more();
        return;
    }

    restore_ghost_version(gfile, majorVersion, minorVersion);

    // sanity check - EOF
    if (!feof(gfile))
    {
        fclose(gfile);
        snprintf( info, INFO_SIZE, "Incomplete read of \"%s\".", cha_fil);
        mpr(info, MSGCH_WARN);
        more();
        return;
    }

    fclose(gfile);

    // remove bones file - ghosts are hardly permanent.
    unlink(cha_fil);

    // translate ghost to monster and place.
    for (int imn = 0; imn < MAX_MONSTERS - 10; imn++)
    {
        if (menv[imn].type != -1)
            continue;

        menv[imn].type = MONS_PLAYER_GHOST;
        menv[imn].hit_dice = ghost.values[12];
        menv[imn].hit_points = ghost.values[0];
        menv[imn].max_hit_points = ghost.values[0];
        menv[imn].armour_class = ghost.values[2];
        menv[imn].evasion = ghost.values[1];
        menv[imn].speed = 10;
        menv[imn].speed_increment = 70;
        menv[imn].attitude = ATT_HOSTILE;
        menv[imn].behaviour = BEH_WANDER;
        menv[imn].flags = 0;
        menv[imn].foe = MHITNOT;
        menv[imn].foe_memory = 0;

        menv[imn].number = 250;
        for(int i=14; i<20; i++)
        {
            if (ghost.values[i] != MS_NO_SPELL)
            {
                menv[imn].number = MST_GHOST;
                break;
            }
        }

        do
        {
            menv[imn].x = random2(GXM - 20) + 10;
            menv[imn].y = random2(GYM - 20) + 10;
        }
        while ((grd[menv[imn].x][menv[imn].y] != DNGN_FLOOR)
               || (mgrd[menv[imn].x][menv[imn].y] != NON_MONSTER));

        mgrd[menv[imn].x][menv[imn].y] = imn;
        break;
    }
}


void restore_game(void)
{
    char char_f[kFileNameSize];

#ifdef SAVE_DIR_PATH
    snprintf( char_f, sizeof(char_f),
              SAVE_DIR_PATH "%s%d", you.your_name, (int) getuid() );
#else
    strncpy(char_f, you.your_name, kFileNameLen);
    char_f[kFileNameLen] = 0;
#endif

    strcat(char_f, ".sav");

#ifdef DOS
    strupr(char_f);
#endif

    FILE *restoreFile = fopen(char_f, "rb");

    if (restoreFile == NULL)
    {
        strcpy(info, "Unable to open \"");
        strcat(info, char_f );
        strcat(info, "\" for reading!");
        perror(info);
        end(-1);
    }

    char majorVersion;
    char minorVersion;

    if (!determine_version(restoreFile, majorVersion, minorVersion))
    {
        perror("\nSavefile appears to be invalid.\n");
        end(-1);
    }

    restore_version(restoreFile, majorVersion, minorVersion);

    // sanity check - EOF
    if (!feof(restoreFile))
    {
        snprintf( info, INFO_SIZE, "\nIncomplete read of \"%s\" - aborting.\n", char_f);
        perror(info);
        end(-1);
    }

    fclose(restoreFile);
}

static bool determine_version( FILE *restoreFile,
                               char &majorVersion, char &minorVersion )
{
    // read first two bytes.
    char buf[2];
    if (read2(restoreFile, buf, 2) != 2)
        return false;               // empty file?

    // check for 3.30
    if (buf[0] == you.your_name[0] && buf[1] == you.your_name[1])
    {
        majorVersion = 0;
        minorVersion = 0;
        rewind(restoreFile);
        return true;
    }

    // otherwise, read version and validate.
    majorVersion = buf[0];
    minorVersion = buf[1];

    if (majorVersion == 1 || majorVersion == 4)
        return true;

    return false;   // if its not 1 or 4, no idea!
}

static void restore_version( FILE *restoreFile,
                             char majorVersion, char minorVersion )
{
    // assuming the following check can be removed once we can read all
    // savefile versions.
    if (majorVersion < 4)
    {
        snprintf( info, INFO_SIZE, "\nSorry, this release cannot read a v%d.%d savefile.\n",
            majorVersion, minorVersion);
        perror(info);
        end(-1);
    }

    switch(majorVersion)
    {
        case 4:
            restore_tagged_file(restoreFile, TAGTYPE_PLAYER, minorVersion);
            break;
        default:
            break;
    }
}

// generic v4 restore function
static void restore_tagged_file( FILE *restoreFile, int fileType,
                                 char minorVersion )
{
    int i;

    char tags[NUM_TAGS];
    tag_set_expected(tags, fileType);

    while(1)
    {
        i = tag_read(restoreFile, minorVersion);
        if (i == 0)                 // no tag!
            break;
        tags[i] = 0;                // tag read
    }

    // go through and init missing tags
    for(i=0; i<NUM_TAGS; i++)
    {
        if (tags[i] == 1)           // expected but never read
            tag_missing(i, minorVersion);
    }
}

static bool determine_level_version( FILE *levelFile,
                                     char &majorVersion, char &minorVersion )
{
    // read first two bytes.
    char buf[2];
    if (read2(levelFile, buf, 2) != 2)
        return false;               // empty file?

    // check for 3.30 -- simply started right in with player name.
    if (isprint(buf[0]) && buf[0] > 4)      // who knows?
    {
        majorVersion = 0;
        minorVersion = 0;
        rewind(levelFile);
        return true;
    }

    // otherwise, read version and validate.
    majorVersion = buf[0];
    minorVersion = buf[1];

    if (majorVersion == 1 || majorVersion == 4)
        return true;

    return false;   // if its not 1 or 4, no idea!
}

static void restore_level_version( FILE *levelFile,
                                   char majorVersion, char minorVersion )
{
    // assuming the following check can be removed once we can read all
    // savefile versions.
    if (majorVersion < 4)
    {
        snprintf( info, INFO_SIZE, "\nSorry, this release cannot read a v%d.%d level file.\n",
            majorVersion, minorVersion);
        perror(info);
        end(-1);
    }

    switch(majorVersion)
    {
        case 4:
            restore_tagged_file(levelFile, TAGTYPE_LEVEL, minorVersion);
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
    if (read2(ghostFile, buf, 2) != 2)
        return false;               // empty file?

    // check for pre-v4 -- simply started right in with ghost name.
    if (isprint(buf[0]) && buf[0] > 4)
    {
        majorVersion = 0;
        minorVersion = 0;
        rewind(ghostFile);
        return true;
    }

    // otherwise, read version and validate.
    majorVersion = buf[0];
    minorVersion = buf[1];

    if (majorVersion == 4)
        return true;

    return false;   // if its not 4, no idea!
}

static void restore_old_ghost( FILE *ghostFile )
{
    char buf[41];
    read2(ghostFile, buf, 41);  // 41 causes EOF. 40 will not.

    // translate
    memcpy(ghost.name, buf, 20);

    for(int i=0; i<20; i++)
        ghost.values[i] = buf[i+20];
}

static void restore_ghost_version( FILE *ghostFile,
                                   char majorVersion, char minorVersion )
{
    // currently, we can read all known ghost versions.
    switch(majorVersion)
    {
        case 4:
            restore_tagged_file(ghostFile, TAGTYPE_GHOST, minorVersion);
            break;
        case 0:
            restore_old_ghost(ghostFile);
            break;
        default:
            break;
    }
}

void save_ghost(void)
{
    char cha_fil[kFileNameSize];
    const int wpn = you.equip[EQ_WEAPON];

    if (you.your_level < 2 || you.is_undead)
        return;

    make_filename(cha_fil, "bones", you.your_level, you.where_are_you,
        you.level_type != LEVEL_DUNGEON, true);

    FILE *gfile = fopen(cha_fil, "rb");

    // don't overwrite existing bones!
    if (gfile != NULL)
    {
        fclose(gfile);
        return;
    }

    memcpy(ghost.name, you.your_name, 20);

    ghost.values[0] = (you.hp_max >= 150) ? (150) : (you.hp_max);
    ghost.values[1] = player_evasion();
    ghost.values[2] = player_AC();
    ghost.values[3] = player_see_invis();
    ghost.values[4] = player_res_fire();
    /* note - as ghosts, automatically get res poison + prot_life */
    ghost.values[5] = player_res_cold();
    ghost.values[6] = player_res_electricity();

    int d = 4;
    int e = 0;

    if (wpn != -1)
    {
        if (you.inv[wpn].base_type == OBJ_WEAPONS
            || you.inv[wpn].base_type == OBJ_STAVES)
        {
            d = property( you.inv[wpn], PWPN_DAMAGE );

            d *= 25 + you.skills[weapon_skill( you.inv[wpn].base_type,
                                               you.inv[wpn].sub_type )];
            d /= 25;

            if (you.inv[wpn].base_type == OBJ_WEAPONS)
            {
                if (is_random_artefact( you.inv[wpn] ))
                    e = randart_wpn_property( you.inv[wpn], RAP_BRAND );
                else
                    e = you.inv[wpn].special;
            }
        }
    }
    else
    {
        /* Unarmed combat */
        if (you.species == SP_TROLL)
            d += you.experience_level;

        d += you.skills[SK_UNARMED_COMBAT];
    }

    d *= 30 + you.skills[SK_FIGHTING];
    d /= 30;

    d += you.strength / 4;

    if (d > 50)
        d = 50;

    ghost.values[7] = d;
    ghost.values[8] = e;
    ghost.values[9] = you.species;
    ghost.values[10] = best_skill(SK_FIGHTING, (NUM_SKILLS - 1), 99);
    ghost.values[11] = you.skills[best_skill(SK_FIGHTING, (NUM_SKILLS - 1), 99)];
    ghost.values[12] = you.experience_level;
    ghost.values[13] = you.char_class;
    add_spells(ghost);

    gfile = fopen(cha_fil, "wb");

    if (gfile == NULL)
    {
        strcpy(info, "Error creating ghost file: ");
        strcat(info, cha_fil);
        mpr(info);
        more();
        return;
    }

    write_tagged_file( gfile, 4, 0, TAGTYPE_GHOST );

    fclose(gfile);

#ifdef SHARED_FILES_CHMOD_PUBLIC
    chmod(cha_fil, SHARED_FILES_CHMOD_PUBLIC);
#endif
}                               // end save_ghost()

/*
   Used when creating ghosts: goes through and finds spells for the ghost to
   cast. Death is a traumatic experience, so ghosts only remember a few spells.
 */
void add_spells(struct ghost_struct &gs)
{
    int i = 0;

    gs.values[14] = SPELL_NO_SPELL;
    gs.values[15] = SPELL_NO_SPELL;
    gs.values[16] = SPELL_NO_SPELL;
    gs.values[17] = SPELL_NO_SPELL;
    gs.values[18] = SPELL_NO_SPELL;
    gs.values[19] = SPELL_NO_SPELL;

    gs.values[14] = search_first_list(SPELL_NO_SPELL);
    gs.values[15] = search_first_list(gs.values[14]);
    gs.values[16] = search_second_list(SPELL_NO_SPELL);
    gs.values[17] = search_third_list(SPELL_NO_SPELL);

    if (gs.values[17] == SPELL_NO_SPELL)
        gs.values[17] = search_first_list(SPELL_NO_SPELL);

    gs.values[18] = search_first_list(gs.values[17]);

    if (gs.values[18] == SPELL_NO_SPELL)
        gs.values[18] = search_first_list(gs.values[17]);

    if (find_spell(SPELL_DIG))
        gs.values[18] = SPELL_DIG;

    /* Looks for blink/tport for emergency slot */
    if (find_spell(SPELL_CONTROLLED_BLINK) || find_spell(SPELL_BLINK))
        gs.values[19] = SPELL_CONTROLLED_BLINK;

    if (find_spell(SPELL_TELEPORT_SELF))
        gs.values[19] = SPELL_TELEPORT_SELF;

    for (i = 14; i < 20; i++)
        gs.values[i] = translate_spell(gs.values[i]);
}                               // end add_spells()

unsigned char search_first_list(unsigned char ignore_spell)
{
    for (int i = 0; i < 20; i++)
    {
        if (search_order_conj[i] == SPELL_NO_SPELL)
            return SPELL_NO_SPELL;

        if (search_order_conj[i] == ignore_spell)
            continue;

        if (find_spell(search_order_conj[i]))
            return search_order_conj[i];
    }

    return SPELL_NO_SPELL;
}                               // end search_first_list()

unsigned char search_second_list(unsigned char ignore_spell)
{
    for (int i = 0; i < 20; i++)
    {
        if (search_order_third[i] == SPELL_NO_SPELL)
            return SPELL_NO_SPELL;

        if (search_order_third[i] == ignore_spell)
            continue;

        if (find_spell(search_order_third[i]))
            return search_order_third[i];
    }

    return SPELL_NO_SPELL;
}                               // end search_second_list()

unsigned char search_third_list(unsigned char ignore_spell)
{
    for (int i = 0; i < 20; i++)
    {
        if (search_order_misc[i] == SPELL_NO_SPELL)
            return SPELL_NO_SPELL;

        if (search_order_misc[i] == ignore_spell)
            continue;

        if (find_spell(search_order_misc[i]))
            return search_order_misc[i];
    }

    return SPELL_NO_SPELL;
}                               // end search_third_list()

/*
   Searches for a specific spell, according to search order in the global arrays
   defined start of file (top to bottom).
 */
bool find_spell(unsigned char which_sp)
{
    for (int i = 0; i < 21; i++)
    {
        if (you.spells[i] == which_sp)
            return true;
    }

    return false;
}                               // end find_spell()

/*
   When passed the number for a player spell, returns the equivalent monster
   spell. Returns SPELL_NO_SPELL on failure (no equiv).
 */
unsigned char translate_spell(unsigned char spel)
{
    switch (spel)
    {
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
    case SPELL_PARALYZE:
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
        return (MS_BLINK);        /* approximate */
/*  case FREEZING_CLOUD: return ; no freezing/mephitic cloud yet
   case MEPHITIC_CLOUD: return ; */
    case SPELL_VENOM_BOLT:
        return (MS_VENOM_BOLT);
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
        /* Too powerful to give ghosts Torment for Agony? Nah. */
        return (MS_TORMENT);
    case SPELL_SYMBOL_OF_TORMENT:
        return (MS_TORMENT);
    default:
        break;
    }

    return (MS_NO_SPELL);
}

void generate_random_demon(void)
{
    int rdem = 0;
    int i = 0;

    for (rdem = 0; rdem < MAX_MONSTERS + 1; rdem++)
    {
        if (rdem == MAX_MONSTERS)
            return;

        if (menv[rdem].type == MONS_PANDEMONIUM_DEMON)
            break;
    }

    make_name(random2(250), random2(250), random2(250), 3, st_prn);
    strcpy(ghost.name, st_prn);

    // hp - could be defined below (as could ev, AC etc). Oh well, too late:
    ghost.values[0] = 50 + (one_chance_in(3) ? random2avg(295, 6)
                                             : random2avg(197, 4));

    // evasion:
    ghost.values[1] = 5 + random2(10);

    // AC:
    ghost.values[2] = 0;        // was random2(0), which simply returns 0 {dlb}

    // see invisible:
    ghost.values[3] = (one_chance_in(3) ? 1 : 0);

    if (!one_chance_in(3))
    {
        ghost.values[4] = 0;    /* res_fire */

        if (one_chance_in(4))
            ghost.values[4] = 99;

        if (one_chance_in(4))
            ghost.values[4] = 102;
    }
    else
        ghost.values[4] = 101;

    if (!one_chance_in(3))
    {
        ghost.values[5] = 0;    /* res_cold */
        if (one_chance_in(4))
            ghost.values[5] = 99;
    }
    else
        ghost.values[5] = 101;

    // demons, like ghosts, automatically get poison res. and life prot.

    // resist electricity:
    ghost.values[6] = (one_chance_in(3) ? 1 : 0);

    // HTH damage:
    ghost.values[7] = 10 + random2avg(58, 3);

    // special attack type (uses weapon brand code):
    ghost.values[8] = 0;

    if (coinflip())
    {
        ghost.values[8] = random2(17);

        if (ghost.values[8] == 3 || ghost.values[8] == 5
            || ghost.values[8] == 7 || ghost.values[8] == 11
            || ghost.values[8] == 12 || ghost.values[8] == 14)
        {
            /* some brands inappropriate (eg holy wrath) */
            ghost.values[8] = 0;
        }
    }

    // is demon a spellcaster?
    // upped from one_chance_in(3)... spellcasters are more interesting
    // and I expect named demons to typically have a trick or two -- bwr
    ghost.values[9] = (one_chance_in(10) ? 0 : 1);

    // does demon fly?
    ghost.values[10] = (one_chance_in(3) ? 0 : random2(3));

    // vacant <ghost best skill level>:
    ghost.values[11] = 0;

    // hit dice:
    ghost.values[12] = 10 + random2(10);

    // does demon cycle colours?
    ghost.values[13] = (one_chance_in(10) ? 1 : 0);

    menv[rdem].hit_dice = ghost.values[12];
    menv[rdem].hit_points = ghost.values[0];
    menv[rdem].max_hit_points = ghost.values[0];
    menv[rdem].armour_class = ghost.values[2];
    menv[rdem].evasion = ghost.values[1];
    menv[rdem].speed = (one_chance_in(3) ? 10 : 8 + random2(10));
    menv[rdem].speed_increment = 70;
    menv[rdem].number = random_colour();        // demon's colour

    ghost.values[14] = SPELL_NO_SPELL;
    ghost.values[15] = SPELL_NO_SPELL;
    ghost.values[16] = SPELL_NO_SPELL;
    ghost.values[17] = SPELL_NO_SPELL;
    ghost.values[18] = SPELL_NO_SPELL;
    ghost.values[19] = SPELL_NO_SPELL;

    /* This bit uses the list of player spells to find appropriate spells
       for the demon, then converts those spells to the monster spell indices.
       Some special monster-only spells are at the end. */
    if (ghost.values[9] == 1)
    {
        if (coinflip())
        {
            for (;;)
            {
                if (one_chance_in(3))
                    break;

                ghost.values[14] = search_order_conj[i];
                i++;

                if (search_order_conj[i] == SPELL_NO_SPELL)
                    break;
            }
        }

        if (coinflip())
        {
            for (;;)
            {
                if (one_chance_in(3))
                    break;

                ghost.values[15] = search_order_conj[i];
                i++;

                if (search_order_conj[i] == SPELL_NO_SPELL)
                    break;
            }
        }

        if (!one_chance_in(4))
        {
            for (;;)
            {
                if (one_chance_in(3))
                    break;

                ghost.values[16] = search_order_third[i];
                i++;

                if (search_order_third[i] == SPELL_NO_SPELL)
                    break;
            }
        }

        if (coinflip())
        {
            for (;;)
            {
                if (one_chance_in(3))
                    break;

                ghost.values[17] = search_order_misc[i];
                i++;

                if (search_order_misc[i] == SPELL_NO_SPELL)
                    break;
            }
        }

        if (coinflip())
        {
            for(;;)
            {
                if (one_chance_in(3))
                    break;

                ghost.values[18] = search_order_misc[i];
                i++;

                if (search_order_misc[i] == SPELL_NO_SPELL)
                    break;
            }
        }

        if (coinflip())
            ghost.values[19] = SPELL_BLINK;
        if (coinflip())
            ghost.values[19] = SPELL_TELEPORT_SELF;

        /* Converts the player spell indices to monster spell ones */
        for (i = 14; i < 20; i++)
            ghost.values[i] = translate_spell(ghost.values[i]);

        /* give demon a chance for some monster-only spells: */
        if (one_chance_in(25))
            ghost.values[14] = MS_HELLFIRE_BURST;
        if (one_chance_in(25))
            ghost.values[14] = MS_METAL_SPLINTERS;
        if (one_chance_in(25))
            ghost.values[14] = MS_ENERGY_BOLT;  /* eye of devas */
        if (one_chance_in(25))
            ghost.values[15] = MS_STEAM_BALL;
        if (one_chance_in(25))
            ghost.values[15] = MS_PURPLE_BLAST;
        if (one_chance_in(25))
            ghost.values[15] = MS_HELLFIRE;
        if (one_chance_in(25))
            ghost.values[16] = MS_SMITE;
        if (one_chance_in(25))
            ghost.values[16] = MS_HELLFIRE_BURST;
        if (one_chance_in(15))
            ghost.values[18] = MS_DIG;

        /* and demon-summoning should be fairly common: */
        if (one_chance_in(12))
            ghost.values[16] = MS_SUMMON_DEMON_GREATER;
        if (one_chance_in(12))
            ghost.values[16] = MS_SUMMON_DEMON;
        if (one_chance_in(20))
            ghost.values[17] = MS_SUMMON_DEMON_GREATER;
        if (one_chance_in(20))
            ghost.values[17] = MS_SUMMON_DEMON;

        /* at least they can summon demons */
        if (ghost.values[17] == SPELL_NO_SPELL)
            ghost.values[17] = MS_SUMMON_DEMON;
    }
}                               // end generate_random_demon()

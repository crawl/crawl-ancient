/*
 *  File: highscore.cc
 *  Summary: deal with reading and writing of highscore file
 *  Written by: Gordon Lipford
 *
 *  Change History (most recent first):
 *
 *   <1>   16feb2001      gdl    Created
 */

/*
 * ----------- MODIFYING THE PRINTED SCORE FORMAT ---------------------
 *   Do this at your leisure.  Change hiscores_format_single() as much
 * as you like.
 *
 *
 * ----------- IF YOU MODIFY THE INTERNAL SCOREFILE FORMAT ------------
 *              .. as defined by the struct 'scorefile_entry' ..
 *   You MUST change hs_copy(),  hs_parse_numeric(),  hs_parse_ascii(),
 *       and hs_write().  It's also a really good idea to change the
 *       version numbers assigned in ouch() so that Crawl can tell the
 *       difference between your new entry and previous versions.
 *
 *
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include "AppHdr.h"
#include "externs.h"
#include "hiscores.h"
#include "mon-util.h"
#include "player.h"

//jmf: brent sez:
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason ...
#ifdef USE_CURSES
#include <curses.h>
#endif


// enough memory allocated to snarf in the scorefile entries
static struct scorefile_entry hs_list[SCORE_FILE_ENTRIES];

// hackish: scorefile position of newest entry.  Will be highlit during
// highscore printing (always -1 when run from command line).
static int newest_entry = -1;

static FILE *hs_open(char *mode);
static void hs_close(FILE *handle, char *mode);
static bool hs_read(FILE *scores, struct scorefile_entry &dest);
static void hs_parse_numeric(char *inbuf, struct scorefile_entry &dest);
static void hs_parse_string(char *inbuf, struct scorefile_entry &dest);
static void hs_copy(struct scorefile_entry &dest, struct scorefile_entry &src);
static void hs_write(FILE *scores, struct scorefile_entry &entry);
static void hs_nextstring(char *&inbuf, char *dest);
static int hs_nextint(char *&inbuf);
static long hs_nextlong(char *&inbuf);

// functions dealing with old scorefile entries
static void hs_parse_generic_1(char *&inbuf, char *outbuf, const char *stopvalues);
static void hs_parse_generic_2(char *&inbuf, char *outbuf, const char *continuevalues);
static void hs_stripblanks(char *buf);
static void hs_search_death(char *inbuf, struct scorefile_entry &se);
static void hs_search_where(char *inbuf, struct scorefile_entry &se);

// file locking stuff
static bool lock_file_handle( FILE *handle, int type );
static bool unlock_file_handle( FILE *handle );

void hiscores_new_entry(struct scorefile_entry &ne)
{
    FILE *scores;
    int i, total_entries;
    bool inserted = false;

    // open highscore file (reading) -- note that NULL is not fatal!
    scores = hs_open("r");

    // read highscore file, inserting new entry at appropriate point,
    for(i=0;  i < SCORE_FILE_ENTRIES; i++)
    {
        if (hs_read(scores, hs_list[i]) == false)
            break;

        // compare points..
        if (ne.points >= hs_list[i].points && inserted == false)
        {
            newest_entry = i;           // for later printing
            inserted = true;
            // copy read entry to i+1th position
            if (i+1 < SCORE_FILE_ENTRIES)
                hs_copy(hs_list[i+1], hs_list[i]);

            // copy new entry to current position
            hs_copy(hs_list[i], ne);
            i ++;
        }
    }

    // special case: lowest score,  with room
    if (!inserted && i < SCORE_FILE_ENTRIES)
    {
        newest_entry = i;
        inserted = true;
        // copy new entry
        hs_copy(hs_list[i], ne);
        i++;
    }

    total_entries = i;

    // close so we can re-open for writing
    hs_close(scores,"r");

    // open highscore file (writing) -- NULL *is* fatal here.
    scores = hs_open("w");
    if (scores == NULL)
    {
        perror("Entry not added - failure opening score file for writing.");
        return;
    }

    // write scorefile entries.
    for(i=0; i<total_entries; i++)
    {
        hs_write(scores, hs_list[i]);
    }

    // close scorefile.
    hs_close(scores, "w");
}

void hiscores_print_list(void)
{
    FILE *scores;
    int i, total_entries;

    // open highscore file (reading)
    scores = hs_open("r");
    if (scores == NULL)
    {
        // will only happen from command line
        puts("No high scores.");
        return;
    }

    // read highscore file
    for(i=0;  i < SCORE_FILE_ENTRIES; i++)
    {
        if (hs_read(scores, hs_list[i]) == false)
            break;
    }
    total_entries = i;

    // close off
    hs_close(scores, "r");

    // print N entries
    int display_count = NUMBER_OF_LINES - 7;
    // if highscore print,  print as many as user wanted
    if (Options.sc_entries > 0)
        display_count = Options.sc_entries;

    textcolor(LIGHTGREY);
    for(i=0; i<display_count && i<total_entries; i++)
    {
        // check for recently added entry
        if (i == newest_entry)
            textcolor(YELLOW);

        // print position (not tracked in score file)
        sprintf(info, "%2d.", i+1);
        cprintf(info);

        // format the entry
        hiscores_format_single(info, hs_list[i]);

        // truncate
        info[76] = '\0';

        // print entry
        strcat(info, EOL);
        cprintf(info);

        if (i == newest_entry)
            textcolor(LIGHTGREY);
    }
}

void hiscores_format_single(char *buf, struct scorefile_entry &se)
{
    char scratch[100];

    // race_class_name overrides race & class
    if (strlen(se.race_class_name) == 0)
        sprintf(scratch, "%s%s", species_abbrev(se.race), class_abbrev(se.cls));
    else
        strcpy(scratch, se.race_class_name);

    sprintf(buf, "%8d %-10s - %s%d%s,", se.points, se.name,
        scratch, se.lvl, (se.wiz_mode==1)?" Wiz":"");

    // get monster type & number, if applicable
    int mon_type = se.death_source;
    int mon_number = se.mon_num;

    switch (se.death_type)
    {
    case KILLED_BY_MONSTER:

/* BCR
 * Note: There was a bug where deep elves weren't getting the 'a' before
 *       their names.  It turns out that the code originally assumed that
 *       Monsters with type between 250 and 310 would be uniques.  However,
 *       Some new monsters were added between 260 and 280 that are not unique.
 *       For now, I've updated the check to be accurate, but there may be other
 *       issues with this.
 */
        // GDL: here's an example of using final_hp.  Verbiage could be better.
        strcat(buf, (se.final_hp > -6)?" slain by ":
                    (se.final_hp > -14)?" mangled by ":
                    (se.final_hp > -22)?" blasted by ":
                    " annihilated by ");

        // if death_source_name is non-null,  override lookup (names might have
        // changed!)
        if (strlen(se.death_source_name) > 0)
        {
            strcat(buf, se.death_source_name);
        }
        else
        {
            if (mon_type < MONS_PROGRAM_BUG
                || (mon_type < MONS_TERENCE && mon_type >= MONS_NAGA_MAGE)
                || mon_type > MONS_BORIS && mon_type != MONS_PLAYER_GHOST)
            {
                strcat(buf, "a");
            }

            strcat(buf, monam(mon_number, mon_type, 0, 99));
        }

        break;

    case KILLED_BY_POISON:
        //if (dam == -9999) strcat(buf, "an overload of ");
        strcat(buf, " killed by a lethal dose of poison");
        break;

    case KILLED_BY_CLOUD:
        strcat(buf, " killed by a cloud");
        break;

    // beam - beam[0].name is a local variable, so can't access it
    // without horrible hacks
    case KILLED_BY_BEAM:
        strcat(buf, " killed from afar by ");

        // if death_source_name is non-null,  override this
        if (strlen(se.death_source_name) > 0)
        {
            strcat(buf, se.death_source_name);
        }
        else
        {
            if (mon_type < MONS_PROGRAM_BUG
                || (mon_type < MONS_TERENCE && mon_type >= MONS_NAGA_MAGE)
                || mon_type > MONS_BORIS && mon_type != MONS_PLAYER_GHOST)
            {
                strcat(buf, "a");
            }

            strcat(buf, monam(mon_number, mon_type, 0, 99));
        }
        break;

/*
    case KILLED_BY_DEATHS_DOOR:
        // death's door running out - NOTE: This is no longer fatal
        strcat(buf, " ran out of time");
        break;
*/

    case KILLED_BY_LAVA:
        strcat(buf, " took a swim in molten lava");
        break;

    case KILLED_BY_WATER:
        if (se.race == SP_MUMMY)
            strcat(buf, " soaked and fell apart");
        else
            strcat(buf, " drowned");
        break;

    // these three are probably only possible if you wear a ring
    // of >= +3 ability, get drained to 3, then take it off, or have a very
    // low abil and wear a -ve ring. or, as of 2.7x, mutations can cause this
    // Don't forget decks of cards (they have some nasty code for this) -- bwr
    case KILLED_BY_STUPIDITY:
        strcat(buf, " died of stupidity");
        break;

    case KILLED_BY_WEAKNESS:
        strcat(buf, " too weak to continue adventuring");
        break;

    case KILLED_BY_CLUMSINESS:
        strcat(buf, " slipped on a banana peel");
        break;

    case KILLED_BY_TRAP:
        strcat(buf, " killed by a trap");
        break;

    case KILLED_BY_LEAVING:
        strcat(buf, " got out of the dungeon alive.");
        break;

    case KILLED_BY_WINNING:
        strcat(buf, " escaped with the Orb!");
        break;

    case KILLED_BY_QUITTING:
        strcat(buf, " quit");
        break;

    case KILLED_BY_DRAINING:
        strcat(buf, " was drained of all life");
        break;

    case KILLED_BY_STARVATION:
        strcat(buf, " starved to death");
        break;

    case KILLED_BY_FREEZING:
        strcat(buf, " froze to death");
        break;

    case KILLED_BY_BURNING:
        strcat(buf, " burnt to a crisp");
        break;

    case KILLED_BY_WILD_MAGIC:
        strcat(buf, " killed by wild magic");
        break;

    case KILLED_BY_XOM:
        strcat(buf, " killed by Xom");
        break;

    case KILLED_BY_STATUE:
        strcat(buf, " killed by a statue");
        break;

    case KILLED_BY_ROTTING:
        strcat(buf, " rotted away");
        break;

    case KILLED_BY_TARGETTING:
        strcat(buf, " killed by bad targeting");
        break;

    case KILLED_BY_SPORE:
        strcat(buf, " killed by an exploding spore");
        break;

    case KILLED_BY_TSO_SMITING:
        strcat(buf, " smote by The Shining One");
        break;

    case KILLED_BY_PETRIFICATION:
        strcat(buf, " turned to stone");
        break;

    case KILLED_BY_SHUGGOTH:
        strcat(buf, " eviscerated by a hatching shuggoth");
        break;

    case KILLED_BY_SOMETHING:
        strcat(buf, " died");
        break;

    default:
        strcat(buf, " nibbled to death by software bugs");
        break;
    }                           // end switch

    if (se.death_type != KILLED_BY_LEAVING && se.death_type != KILLED_BY_WINNING)
    {
        if (se.level_type == LEVEL_ABYSS)
        {
            strcat(buf, " in the Abyss.");
            return;
        }
        else if (se.level_type == LEVEL_PANDEMONIUM)
        {
            strcat(buf, " in Pandemonium.");
            return;
        }
        else if (se.level_type == LEVEL_LABYRINTH)
        {
            strcat(buf, " in a labyrinth.");
            return;
        }

        itoa((se.dlvl + 1), scratch, 10);

        if (se.branch != BRANCH_VESTIBULE_OF_HELL)
        {
            strcat(buf, " on L");
            strcat(buf, scratch);
        }

        switch (se.branch)
        {
        case BRANCH_DIS:
            strcat(buf, " of Dis");
            break;
        case BRANCH_GEHENNA:
            strcat(buf, " of Gehenna");
            break;
        case BRANCH_VESTIBULE_OF_HELL:
            strcat(buf, " in the Vestibule");
            break;
        case BRANCH_COCYTUS:
            strcat(buf, " of Cocytus");
            break;
        case BRANCH_TARTARUS:
            strcat(buf, " of Tartarus");
            break;
        case BRANCH_ORCISH_MINES:
            strcat(buf, " of the Mines");
            break;
        case BRANCH_HIVE:
            strcat(buf, " of the Hive");
            break;
        case BRANCH_LAIR:
            strcat(buf, " of the Lair");
            break;
        case BRANCH_SLIME_PITS:
            strcat(buf, " of the Slime Pits");
            break;
        case BRANCH_VAULTS:
            strcat(buf, " of the Vaults");
            break;
        case BRANCH_CRYPT:
            strcat(buf, " of the Crypt");
            break;
        case BRANCH_HALL_OF_BLADES:
            strcat(buf, " of the Hall");
            break;
        case BRANCH_HALL_OF_ZOT:
            strcat(buf, " of Zot's Hall");
            break;
        case BRANCH_ECUMENICAL_TEMPLE:
            strcat(buf, " of the Temple");
            break;
        case BRANCH_SNAKE_PIT:
            strcat(buf, " of the Snake Pit");
            break;
        case BRANCH_ELVEN_HALLS:
            strcat(buf, " of the Elf Hall");
            break;
        case BRANCH_TOMB:
            strcat(buf, " of the Tomb");
            break;
        case BRANCH_SWAMP:
            strcat(buf, " of the Swamp");
            break;
        }

        strcat(buf, ".");
    } // endif - killed by winning

    return;
}

// --------------------------------------------------------------------------
// BEGIN private functions
// --------------------------------------------------------------------------

// first, some file locking stuff for multiuser crawl
#ifdef USE_FILE_LOCKING

static bool lock_file_handle( FILE *handle, int type )
{
    struct flock  lock;
    int           status;

    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = type;

#ifdef USE_BLOCKING_LOCK

    status = fcntl( fileno( handle ), F_SETLKW, &lock );

#else

    for (int i = 0; i < 30; i++)
    {
        status = fcntl( fileno( handle ), F_SETLK, &lock );

        // success
        if (status == 0)
            break;

        // known failure
        if (status == -1 && (errno != EACCES && errno != EAGAIN))
            break;

        perror( "Problems locking file... retrying..." );
        delay( 1000 );
    }

#endif

    return (status == 0);
}

static bool unlock_file_handle( FILE *handle )
{
    struct flock  lock;
    int           status;

    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = F_UNLCK;

#ifdef USE_BLOCKING_LOCK

    status = fcntl( fileno( handle ), F_SETLKW, &lock );

#else

    for (int i = 0; i < 30; i++)
    {
        status = fcntl( fileno( handle ), F_SETLK, &lock );

        // success
        if (status == 0)
            break;

        // known failure
        if (status == -1 && (errno != EACCES && errno != EAGAIN))
            break;

        perror( "Problems unlocking file... retrying..." );
        delay( 1000 );
    }

#endif

    return (status == 0);
}

#endif



FILE *hs_open(char *mode)
{
#ifdef SAVE_DIR_PATH
    FILE *handle = fopen(SAVE_DIR_PATH "scores", mode);
#else
    FILE *handle = fopen("scores", mode);
#endif

#ifdef USE_FILE_LOCKING
    int locktype = F_RDLCK;
    if (stricmp(mode, "w") == 0)
        locktype = F_WRLCK;

    if (handle && !lock_file_handle( handle, locktype ))
    {
        perror( "Could not lock scorefile... " );
        fclose( handle );
        handle = NULL;
    }
#endif
    return handle;
}

void hs_close(FILE *handle, char *mode)
{
    if (handle == NULL)
        return;

#ifdef USE_FILE_LOCKING
    unlock_file_handle( handle );
#endif

    // actually close
    fclose(handle);

#ifdef SHARED_FILES_CHMOD_VAL
    if (stricmp(mode, "w") == 0)
    {
  #ifdef SAVE_DIR_PATH
        chmod(SAVE_DIR_PATH "scores", SHARED_FILES_CHMOD_VAL);
  #else
        chmod("scores", SHARED_FILES_CHMOD_VAL);
  #endif
    }
#endif
}

void hs_copy(struct scorefile_entry &dest, struct scorefile_entry &src)
{
    // simple field copy -- assume src is well constructed.

    dest.version = src.version;
    dest.points = src.points;
    strcpy(dest.name, src.name);
    dest.uid = src.uid;
    dest.race = src.race;
    dest.cls = src.cls;
    dest.lvl = src.lvl;
    strcpy(dest.race_class_name, src.race_class_name);
    dest.best_skill = src.best_skill;
    dest.best_skill_lvl = src.best_skill_lvl;
    dest.death_type = src.death_type;
    dest.death_source = src.death_source;
    dest.mon_num = src.mon_num;
    strcpy(dest.death_source_name, src.death_source_name);
    dest.dlvl = src.dlvl;
    dest.level_type = src.level_type;
    dest.branch = src.branch;
    dest.final_hp = src.final_hp;
    dest.wiz_mode = src.wiz_mode;
}

bool hs_read(FILE *scores, struct scorefile_entry &dest)
{
    char inbuf[200];
    char *result;
    int c;

    // get a character..
    if (scores != NULL)
        c = fgetc(scores);

    // check for NULL scores file or EOF
    if (scores == NULL || c == EOF)
    {
        dest.points = 0;
        return false;
    }

    // get a line - this is tricky.  "Lines" come in three flavors:
    // 1) old-style lines which were 80 character blocks
    // 2) 4.0 pr1 through pr7 versions which were newline terminated
    // 3) 4.0 pr8 and onwards which are 'current' ASCII format, and
    //    may exceed 80 characters!

    // put 'c' in first spot
    inbuf[0] = c;

    if (fgets(&inbuf[1], (c==':')?198:81, scores) == NULL)
    {
        dest.points = 0;
        return false;
    }

    // check type;  lines starting with a colon are new-style scores.
    if (c == ':')
        hs_parse_numeric(inbuf, dest);
    else
        hs_parse_string(inbuf, dest);

    return true;
}

static void hs_nextstring(char *&inbuf, char *dest)
{
    char *p = dest;
    // assume we're on a ':'

    inbuf ++;
    while(*inbuf != ':' && *inbuf != '\0')
        *p++ = *inbuf++;

    *p = '\0';
}

static int hs_nextint(char *&inbuf)
{
    char num[20];
    hs_nextstring(inbuf, num);
    return atoi(num);
}

static long hs_nextlong(char *&inbuf)
{
    char num[20];
    hs_nextstring(inbuf, num);
    return atol(num);
}

static void hs_parse_numeric(char *inbuf, struct scorefile_entry &se)
{
    se.version = hs_nextint(inbuf);
    se.release = hs_nextint(inbuf);

    // this would be a good point to check for version numbers and branch
    // appropriately

    // acceptable versions are 0 (converted from old hiscore format) and 4
    if (!(se.version == 0 || se.version == 4))
        return;

    se.points = hs_nextlong(inbuf);
    hs_nextstring(inbuf, se.name);
    se.uid = hs_nextlong(inbuf);
    se.race = hs_nextint(inbuf);
    se.cls = hs_nextint(inbuf);
    hs_nextstring(inbuf, se.race_class_name);
    se.lvl = hs_nextint(inbuf);
    se.best_skill = hs_nextint(inbuf);
    se.best_skill_lvl = hs_nextint(inbuf);
    se.death_type = hs_nextint(inbuf);
    se.death_source = hs_nextint(inbuf);
    se.mon_num = hs_nextint(inbuf);
    hs_nextstring(inbuf, se.death_source_name);
    se.dlvl = hs_nextint(inbuf);
    se.level_type = hs_nextint(inbuf);
    se.branch = hs_nextint(inbuf);
    se.final_hp = hs_nextint(inbuf);
    se.wiz_mode = hs_nextint(inbuf);
}

static void hs_write(FILE *scores, struct scorefile_entry &se)
{
    fprintf(scores, ":%d:%d:%ld:%s:%ld:%d:%d:%s:%d:%d:%d",
        se.version, se.release, se.points, se.name,
        se.uid, se.race, se.cls, se.race_class_name, se.lvl,
        se.best_skill, se.best_skill_lvl);
    fprintf(scores, ":%d:%d:%d:%s:%d:%d:%d:%d:%d:\n",
        se.death_type, se.death_source, se.mon_num,
        se.death_source_name, se.dlvl, se.level_type,
        se.branch, se.final_hp, se.wiz_mode);
    return;
}

// -------------------------------------------------------------------------
// functions dealing with old-style scorefile entries.
// -------------------------------------------------------------------------

static void hs_parse_string(char *inbuf, struct scorefile_entry &se)
{
    /* old entries are of the following format (Brent introduced some
       spacing at one point,  we have to take this into account):

6263    BoBo       - DSD10 Wiz, killed by an acid blob on L1 of the Slime Pits.
5877    Aldus-DGM10, killed by a lethal dose of poison on L10.
5419    Yarf       - Koa10, killed by a warg on L1 of the Mines.

    1. All numerics up to the first non-numeric are the score
    2. All non '-' characters are the name.  Strip spaces.
    3. All alphabetics up to the first numeric are race/class
    4. All numerics up to the comma are the clevel
    5. From the comma, search for known fixed substrings and
       translate to death_type.  Leave death source = 0 for old
       scores,  and just copy in the monster name.
    6. Look for the branch type (again, substring search for
       fixed strings) and level.

    Very ugly and time consuming.

    */

    char scratch[80];

    // 1. get score
    hs_parse_generic_2(inbuf, scratch, "0123456789");

    se.version = 0;         // version # of converted score
    se.release = 0;
    se.points = atoi(scratch);

    // 2. get name
    hs_parse_generic_1(inbuf, scratch, "-");
    hs_stripblanks(scratch);
    strcpy(se.name, scratch);

    // 3. get race, class
    inbuf++;    // skip '-'
    hs_parse_generic_1(inbuf, scratch, "0123456789");
    hs_stripblanks(scratch);
    strcpy(se.race_class_name, scratch);
    se.race = 0;
    se.cls = 0;

    // 4. get clevel
    hs_parse_generic_2(inbuf, scratch, "0123456789");
    se.lvl = atoi(scratch);

    // 4a. get wizard mode
    hs_parse_generic_1(inbuf, scratch, ",");
    if (strstr(scratch, "Wiz") != NULL)
        se.wiz_mode = 1;
    else
        se.wiz_mode = 0;

    // 5. get death type
    inbuf++;    // skip comma
    hs_search_death(inbuf, se);

    // 6. get branch, level
    hs_search_where(inbuf, se);

    // set things that can't be picked out of old scorefile entries
    se.uid = 0;
    se.best_skill = 0;
    se.best_skill_lvl = 0;
    se.final_hp = 0;
}

static void hs_parse_generic_1(char *&inbuf, char *outbuf, const char *stopvalues)
{
    char *p = outbuf;

    while(strchr(stopvalues, *inbuf) == NULL && *inbuf != '\0')
        *p++ = *inbuf++;

    *p = '\0';
}

static void hs_parse_generic_2(char *&inbuf, char *outbuf, const char *continuevalues)
{
    char *p = outbuf;

    while(strchr(continuevalues, *inbuf) != NULL && *inbuf != '\0')
        *p++ = *inbuf++;

    *p = '\0';
}

static void hs_stripblanks(char *buf)
{
    char *p = buf;
    char *q = buf;

    // strip leading
    while(*p == ' ')
        p++;
    while(*p != '\0')
        *q++ = *p++;

    *q-- = '\0';
    // strip trailing
    while(*q == ' ')
    {
        *q = '\0';
        q--;
    }
}

static void hs_search_death(char *inbuf, struct scorefile_entry &se)
{
    // assume killed by monster
    se.death_type = KILLED_BY_MONSTER;

    // sigh..
    if (strstr(inbuf, "killed by a lethal dose of poison") != NULL)
        se.death_type = KILLED_BY_POISON;
    else if (strstr(inbuf, "killed by a cloud") != NULL)
        se.death_type = KILLED_BY_CLOUD;
    else if (strstr(inbuf, "killed from afar by") != NULL)
        se.death_type = KILLED_BY_BEAM;
    else if (strstr(inbuf, "took a swim in molten lava") != NULL)
        se.death_type = KILLED_BY_LAVA;
    else if (strstr(inbuf, "soaked and fell apart") != NULL)
    {
        se.death_type = KILLED_BY_WATER;
        se.race = SP_MUMMY;
    }
    else if (strstr(inbuf, "drowned") != NULL)
        se.death_type = KILLED_BY_WATER;
    else if (strstr(inbuf, "died of stupidity") != NULL)
        se.death_type = KILLED_BY_STUPIDITY;
    else if (strstr(inbuf, "too weak to continue adventuring") != NULL)
        se.death_type = KILLED_BY_WEAKNESS;
    else if (strstr(inbuf, "slipped on a banana peel") != NULL)
        se.death_type = KILLED_BY_CLUMSINESS;
    else if (strstr(inbuf, "killed by a trap") != NULL)
        se.death_type = KILLED_BY_TRAP;
    else if (strstr(inbuf, "got out of the dungeon alive") != NULL)
        se.death_type = KILLED_BY_LEAVING;
    else if (strstr(inbuf, "escaped with the Orb") != NULL)
        se.death_type = KILLED_BY_WINNING;
    else if (strstr(inbuf, "quit") != NULL)
        se.death_type = KILLED_BY_QUITTING;
    else if (strstr(inbuf, "was drained of all life") != NULL)
        se.death_type = KILLED_BY_DRAINING;
    else if (strstr(inbuf, "starved to death") != NULL)
        se.death_type = KILLED_BY_STARVATION;
    else if (strstr(inbuf, "froze to death") != NULL)
        se.death_type = KILLED_BY_FREEZING;
    else if (strstr(inbuf, "burnt to a crisp") != NULL)
        se.death_type = KILLED_BY_BURNING;
    else if (strstr(inbuf, "killed by wild magic") != NULL)
        se.death_type = KILLED_BY_WILD_MAGIC;
    else if (strstr(inbuf, "killed by Xom") != NULL)
        se.death_type = KILLED_BY_XOM;
    else if (strstr(inbuf, "killed by a statue") != NULL)
        se.death_type = KILLED_BY_STATUE;
    else if (strstr(inbuf, "rotted away") != NULL)
        se.death_type = KILLED_BY_ROTTING;
    else if (strstr(inbuf, "killed by bad targetting") != NULL)
        se.death_type = KILLED_BY_TARGETTING;
    else if (strstr(inbuf, "killed by an exploding spore") != NULL)
        se.death_type = KILLED_BY_SPORE;
    else if (strstr(inbuf, "smote by The Shining One") != NULL)
        se.death_type = KILLED_BY_TSO_SMITING;
    else if (strstr(inbuf, "turned to stone") != NULL)
        se.death_type = KILLED_BY_PETRIFICATION;
    else if (strstr(inbuf, "eviscerated by a hatching shuggoth") != NULL)
        se.death_type = KILLED_BY_SHUGGOTH;

    // whew!

    // now, if we're still KILLED_BY_MONSTER,  make sure that there is
    // a "killed by" somewhere,  or else we're setting it to UNKNOWN.
    if (se.death_type == KILLED_BY_MONSTER)
    {
        if (strstr(inbuf, "killed by") == NULL)
            se.death_type = KILLED_BY_SOMETHING;
    }

    // set some fields
    se.death_source = 0;
    se.mon_num = 0;
    strcpy(se.death_source_name, "");

    // now try to pull the monster out.
    if (se.death_type == KILLED_BY_MONSTER || se.death_type == KILLED_BY_BEAM)
    {
        char *p = strstr(inbuf, " by ");
        p += 4;
        char *q = strstr(inbuf, " on ");
        if (q == NULL)
            q = strstr(inbuf, " in ");
        char *d = se.death_source_name;
        while(p != q)
            *d++ = *p++;

        *d = '\0';
    }
}

static void hs_search_where(char *inbuf, struct scorefile_entry &se)
{
    char scratch[6];

    se.level_type = LEVEL_DUNGEON;
    se.branch = BRANCH_MAIN_DUNGEON;
    se.dlvl = 0;

    // early out
    if (se.death_type == KILLED_BY_LEAVING || se.death_type == KILLED_BY_WINNING)
        return;

    // here we go again.
    if (strstr(inbuf, "in the Abyss") != NULL)
        se.level_type = LEVEL_ABYSS;
    else if (strstr(inbuf, "in Pandemonium") != NULL)
        se.level_type = LEVEL_PANDEMONIUM;
    else if (strstr(inbuf, "in a labyrinth") != NULL)
        se.level_type = LEVEL_LABYRINTH;

    // early out for special level types
    if (se.level_type != LEVEL_DUNGEON)
        return;

    // check for vestible
    if (strstr(inbuf, "in the Vestibule") != NULL)
    {
        se.branch = BRANCH_VESTIBULE_OF_HELL;
        return;
    }

    // from here,  we have branch and level.
    char *p = strstr(inbuf, "on L");
    if (p != NULL)
    {
        p += 4;
        hs_parse_generic_2(p, scratch, "0123456789");
        se.dlvl = atoi(scratch) - 1;
    }

    // get branch.
    if (strstr(inbuf, "of Dis") != NULL)
        se.branch = BRANCH_DIS;
    else if (strstr(inbuf, "of Gehenna") != NULL)
        se.branch = BRANCH_GEHENNA;
    else if (strstr(inbuf, "of Cocytus") != NULL)
        se.branch = BRANCH_COCYTUS;
    else if (strstr(inbuf, "of Tartarus") != NULL)
        se.branch = BRANCH_TARTARUS;
    else if (strstr(inbuf, "of the Mines") != NULL)
        se.branch = BRANCH_ORCISH_MINES;
    else if (strstr(inbuf, "of the Hive") != NULL)
        se.branch = BRANCH_HIVE;
    else if (strstr(inbuf, "of the Lair") != NULL)
        se.branch = BRANCH_LAIR;
    else if (strstr(inbuf, "of the Slime Pits") != NULL)
        se.branch = BRANCH_SLIME_PITS;
    else if (strstr(inbuf, "of the Vaults") != NULL)
        se.branch = BRANCH_VAULTS;
    else if (strstr(inbuf, "of the Crypt") != NULL)
        se.branch = BRANCH_CRYPT;
    else if (strstr(inbuf, "of the Hall") != NULL)
        se.branch = BRANCH_HALL_OF_BLADES;
    else if (strstr(inbuf, "of Zot's Hall") != NULL)
        se.branch = BRANCH_HALL_OF_ZOT;
    else if (strstr(inbuf, "of the Temple") != NULL)
        se.branch = BRANCH_ECUMENICAL_TEMPLE;
    else if (strstr(inbuf, "of the Snake Pit") != NULL)
        se.branch = BRANCH_SNAKE_PIT;
    else if (strstr(inbuf, "of the Elf Hall") != NULL)
        se.branch = BRANCH_ELVEN_HALLS;
    else if (strstr(inbuf, "of the Tomb") != NULL)
        se.branch = BRANCH_TOMB;
    else if (strstr(inbuf, "of the Swamp") != NULL)
        se.branch = BRANCH_SWAMP;
}

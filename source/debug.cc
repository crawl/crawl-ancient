/*
 *  File:       debug.cc
 *  Summary:    Debug and wizard related functions.
 *  Written by: Linley Henzell and Jesse Jones
 *
 *  Change History (most recent first):
 *
 *               <4>     14/12/99       LRH             Added cast_spec_spell_name()
 *               <3>     5/06/99        JDJ     Added TRACE.
 *               <2>     -/--/--        JDJ     Added a bunch od debugging macros.
 *                                                   Old code is now #if WIZARD.
 *               <1>     -/--/--        LRH     Created
 */

#include "AppHdr.h"
#include "debug.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "direct.h"
#include "dungeon.h"
#include "itemname.h"
#include "monplace.h"
#include "mon-util.h"
#include "player.h"
#include "skills.h"
#include "skills2.h"
#include "spell.h"
#include "spl-util.h"
#include "stuff.h"

#ifndef WIZARD
#define WIZARD
#endif

#if DEBUG && WIN
#define MyDebugBreak() _asm {int 3}
#endif

//-----------------------------------
//      Internal Variables
//
#if WIN
static HANDLE sConsole = NULL;
#endif

// ========================================================================
//      Internal Functions
// ========================================================================

//---------------------------------------------------------------
//
// BreakStrToDebugger
//
//---------------------------------------------------------------
#if DEBUG
static void BreakStrToDebugger(const char *mesg)
{
#if MAC
    unsigned char s[256];

    int len = strlen(mesg);

    if (len > 255)
        len = 255;

    s[0] = (Byte) len;
    BlockMoveData(mesg, s + 1, len);

    DebugStr(s);

#elif WIN
    MSG msg;    // remove pending quit messages so the message box displays

    bool quitting = (bool)::PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);

    char text[2500];

    int flags = MB_YESNO +   // want abort and ignore buttons
                             // (too bad we can't ditch the retry button...)
        MB_ICONERROR +       // display the icon for errors
        MB_TASKMODAL +       // don't let the user do anything else in the app
        MB_SETFOREGROUND;    // bring the app to the front

    strcpy(text, mesg);
    strcat(text, "\nDo you want to drop into the debugger?");

    int result = MessageBoxA(NULL, text, "Debug Break", flags);

    if (result == IDYES)
        MyDebugBreak();

    if (quitting)
        PostQuitMessage(msg.wParam);

#else
    fprintf(stderr, "%s\n", mesg);
    abort();
#endif
}
#endif


//---------------------------------------------------------------
//
// IsDebuggerPresent95
//
// From March 1999 Windows Developer's Journal. This should only
// be called if we're running on Win 95 (normally I'd add an
// ASSERT, but that's a bit dicy since this is called by ASSERT...)
//
//---------------------------------------------------------------
#if WIN
static bool IsDebuggerPresent95()
{
    bool present = false;

    const DWORD kDebuggerPresentFlag = 0x000000001;
    const DWORD kProcessDatabaseBytes = 190;
    const DWORD kOffsetFlags = 8;

    DWORD threadID = GetCurrentThreadId();
    DWORD processID = GetCurrentProcessId();
    DWORD obfuscator = 0;

#if __MWERKS__
    asm
    {
        mov ax, fs
            mov es, ax
            mov eax, 0x18
            mov eax, es:[eax]
            sub eax, 0x10 xor eax,[threadID] mov[obfuscator], eax
    }

#else
    _asm
    {
        mov ax, fs
            mov es, ax
            mov eax, 18 h
            mov eax, es:[eax]
            sub eax, 10 h xor eax,[threadID] mov[obfuscator], eax
    }
#endif

    const DWORD *processDatabase =
                    reinterpret_cast< const DWORD * >(processID ^ obfuscator);

    if (!IsBadReadPtr(processDatabase, kProcessDatabaseBytes))
    {
        DWORD flags = processDatabase[kOffsetFlags];

        present = (flags & kDebuggerPresentFlag) != 0;
    }

    return present;
}
#endif


//---------------------------------------------------------------
//
// IsDebuggerPresent
//
//---------------------------------------------------------------
#if WIN
bool IsDebuggerPresent()
{
    bool present = false;

    typedef BOOL(WINAPI * IsDebuggerPresentProc) ();

    HINSTANCE kernelH = LoadLibrary("KERNEL32.DLL");

    if (kernelH != NULL)
    {                           // should never fail

        IsDebuggerPresentProc proc =
            (IsDebuggerPresentProc)::GetProcAddress( kernelH,
                                                     "IsDebuggerPresent" );

        if (proc != NULL)       // only present in NT and Win 98
            present = proc() != 0;
        else
            present = IsDebuggerPresent95();
    }

    return present;
}
#endif


//---------------------------------------------------------------
//
// CreateConsoleWindow
//
//---------------------------------------------------------------
#if WIN
static void CreateConsoleWindow()
{
    ASSERT(sConsole == NULL);

    // Create the console window
    if (::AllocConsole())
    {
        // Get the console window's handle
        sConsole =::GetStdHandle(STD_ERROR_HANDLE);
        if (sConsole == INVALID_HANDLE_VALUE)
            sConsole = NULL;

        // Set some options
        if (sConsole != NULL)
        {
            VERIFY(::SetConsoleTextAttribute(sConsole, FOREGROUND_GREEN));
            // green text on a black background (there doesn't appear to
            // be a way to get black text)

            VERIFY(::SetConsoleTitle("Debug Log"));

            COORD size = { 80, 120 };

            VERIFY(::SetConsoleScreenBufferSize(sConsole, size));
        }
        else
            DEBUGSTR(L "Couldn't get the console window's handle!");
    }
    else
        DEBUGSTR(L "Couldn't allocate the console window!");
}
#endif


//---------------------------------------------------------------
//
// TraceString
//
//---------------------------------------------------------------
static void TraceString(const char *mesg)
{
    // Write the string to the debug window
#if WIN
    if (IsDebuggerPresent())
    {
        OutputDebugStringA(mesg);       // if you're using CodeWarrior you'll need to enable the "Log System Messages" checkbox to get this working
    }
    else
    {
        if (sConsole == NULL)   // otherwise we'll use a console window
            CreateConsoleWindow();

        if (sConsole != NULL)
        {
            unsigned long written;

            VERIFY(WriteConsoleA(sConsole, mesg, strlen(mesg), &written, NULL));
        }
    }
#else
    fprintf(stderr, "%s", mesg);
#endif

    // Write the string to the debug log
    static bool inited = false;
    static FILE *file = NULL;

    if (!inited)
    {
        ASSERT(file == NULL);

        const char *fileName = "DebugLog.txt";

        file = fopen(fileName, "w");
        ASSERT(file != NULL);

        inited = true;
    }

    if (file != NULL)
    {
        fputs(mesg, file);
        fflush(file);           // make sure all the output makes it to the file

    }
}

#if MAC
#pragma mark -
#endif

// ========================================================================
//      Global Functions
// ========================================================================

//---------------------------------------------------------------
//
// AssertFailed
//
//---------------------------------------------------------------
#if DEBUG
void AssertFailed(const char *expr, const char *file, int line)
{
    char mesg[512];

#if MAC
    sprintf(mesg, "ASSERT(%s) in %s at line %d failed.", expr, file, line);

#else
    const char *fileName = file + strlen(file); // strip off path

    while (fileName > file && fileName[-1] != '\\')
        --fileName;

    sprintf(mesg, "ASSERT(%s) in '%s' at line %d failed.", expr, fileName,
            line);
#endif

    BreakStrToDebugger(mesg);
}
#endif


//---------------------------------------------------------------
//
// DEBUGSTR
//
//---------------------------------------------------------------
#if DEBUG
void DEBUGSTR(const char *format, ...)
{
    char mesg[2048];

    va_list args;

    va_start(args, format);
    vsprintf(mesg, format, args);
    va_end(args);

    BreakStrToDebugger(mesg);
}
#endif


//---------------------------------------------------------------
//
// TRACE
//
//---------------------------------------------------------------
#if DEBUG
void TRACE(const char *format, ...)
{
    char mesg[2048];

    va_list args;

    va_start(args, format);
    vsprintf(mesg, format, args);
    va_end(args);

    TraceString(mesg);
}
#endif // DEBUG

/*
   Some debugging functions, accessable through keys like %, $, &, ) etc when
   a section of code in input() in acr.cc is uncommented.
 */

//---------------------------------------------------------------
//
// cast_spec_spell
//
//---------------------------------------------------------------
#ifdef WIZARD
void cast_spec_spell(void)
{
    char specs[4];

    mpr("Cast which spell by number? ");

    specs[0] = getche();
    specs[1] = getche();
    specs[2] = getche();
    specs[3] = '\0';

    your_spells(atoi(specs), 0, false);
}
#endif


//---------------------------------------------------------------
//
// cast_spec_spell_name
//
//---------------------------------------------------------------
#ifdef WIZARD
void cast_spec_spell_name(void)
{
    int i = 0;
    char specs[50];
    char spname[60];

    mpr("Cast which spell by name? ");

#if defined(LINUX)
    echo();
    getstr(specs);
    noecho();
#elif defined(MAC)
    getstr(specs, sizeof(specs));
#else
    gets(specs);
#endif

    for (i = 0; i < NUM_SPELLS; i++)
    {
        strcpy(spname, spell_title(i));

        if (strstr(strlwr(spname), strlwr(specs)) != NULL)
        {
            your_spells(i, 0, false);
            return;
        }
    }

    mpr((one_chance_in(20)) ? "Maybe you should go back to WIZARD school."
                            : "I couldn't find that spell.");
}
#endif


//---------------------------------------------------------------
//
// create_spec_monster
//
//---------------------------------------------------------------
#ifdef WIZARD
void create_spec_monster(void)
{
    char specs[3];

    mpr("Create which monster by number? ");

    specs[0] = getche();
    specs[1] = getche();
    specs[2] = getche();

    create_monster( atoi(specs), 0, BEH_SLEEP, you.x_pos, you.y_pos,
                    MHITNOT, 250 );
}                               // end create_spec_monster()


//---------------------------------------------------------------
//
// create_spec_monster_name
//
//---------------------------------------------------------------
void create_spec_monster_name(void)
{
    char specs[50];
    char spname[60];

    mpr("(Hint: 'generated' names, eg 'orc zombie', won't work)");
    mpr("Create which monster by name? ");

#if defined(LINUX)
    echo();
    getstr(specs);
    noecho();
#elif defined(MAC)
    getstr(specs, sizeof(specs));
#else
    gets(specs);
#endif

    int mon = -1;
    for (int i = 0; i < NUM_MONSTERS; i++)
    {
        moname(i, 0, 1, 100, spname);

        char *ptr = strstr( strlwr(spname), strlwr(specs) );
        if (ptr != NULL)
        {
            mpr( spname );
            if (ptr == spname && strlen(specs) > 0)
            {
                // we prefer prefixes over partial matches
                mon = i;
                break;
            }
            else
                mon = i;
        }
    }

    if (mon == -1)
    {
        mpr("I couldn't find that monster.");

        if (one_chance_in(20))
            mpr("Maybe it's hiding.");
    }
    else
        create_monster(mon, 0, BEH_SLEEP, you.x_pos, you.y_pos, MHITNOT, 250);

}                               // end create_spec_monster_name()


//---------------------------------------------------------------
//
// level_travel
//
//---------------------------------------------------------------
void level_travel(void)
{
    char specs[3];

    mpr("Travel to which level? ");

    specs[0] = getche();
    specs[1] = getche();
    specs[2] = 0;

    you.your_level = atoi(specs);

    mpr("Your level has been reset.");
    mpr("Enter a staircase for more obvious effects.");
}                               // end level_travel()


//---------------------------------------------------------------
//
// create_spec_object
//
//---------------------------------------------------------------
void create_spec_object(void)
{
    unsigned char class_wanted = 0;
    unsigned char type_wanted = 0;

    mpr("Create which item (class then type)? ");

    class_wanted = ((unsigned char) getche() - 48) * 10;
    class_wanted += (unsigned char) getche() - 48;

    type_wanted = ((unsigned char) getche() - 48) * 10;
    type_wanted += (unsigned char) getche() - 48;

    itoa(property(class_wanted, type_wanted, 2), st_prn, 10);
    strcpy(info, st_prn);
    mpr(info);

    int thing_created = items( 1, class_wanted, type_wanted, 1,
                               you.your_level, 250 );

    canned_msg(MSG_SOMETHING_APPEARS);

    int what_was_there = igrd[you.x_pos][you.y_pos];

    mitm.link[thing_created] = what_was_there;
    igrd[you.x_pos][you.y_pos] = thing_created;
    //mitm.special [thing_created] = 13;
}


//---------------------------------------------------------------
//
// create_spec_object2
//
//---------------------------------------------------------------
void create_spec_object2(void)
{
    unsigned char class_wanted = 0;
    unsigned char type_wanted = 0;
    int dam_wanted = 0;

    mpr("Create which item (class, type, then dam)? ");

    class_wanted = ((unsigned char) getche() - 48) * 10;
    class_wanted += (unsigned char) getche() - 48;

    type_wanted = ((unsigned char) getche() - 48) * 10;
    type_wanted += (unsigned char) getche() - 48;

    dam_wanted = (getche() - 48) * 10;
    dam_wanted += getche() - 48;

    //itoa(property[class_wanted][type_wanted][2], st_prn, 10);
    //mpr(st_prn);

    int thing_created = items( 1, class_wanted, type_wanted, 1,
                               you.your_level, 250 );

    mitm.special[thing_created] = dam_wanted;

    canned_msg(MSG_SOMETHING_APPEARS);

    int what_was_there = igrd[you.x_pos][you.y_pos];

    mitm.link[thing_created] = what_was_there;
    igrd[you.x_pos][you.y_pos] = thing_created;
    //mitm.special [thing_created] = 13;
}
#endif


//---------------------------------------------------------------
//
// stethoscope
//
//---------------------------------------------------------------
void stethoscope(int mwh)
{
    struct dist stth;
    int steth_x, steth_y;
    int i;

    if (mwh == RANDOM_MONSTER)
    {
        mpr("Which monster?");

        direction(stth);

        if (!stth.isValid)
            return;

        if (stth.isTarget)
        {
            steth_x = stth.tx;
            steth_y = stth.ty;
        }
        else
        {
            steth_x = you.x_pos + stth.dx;
            steth_y = you.x_pos + stth.dy;
        }

        if (env.cgrid[steth_x][steth_y] != EMPTY_CLOUD)
        {
            itoa(env.cloud_type[env.cgrid[steth_x][steth_y]], st_prn, 10);
            strcpy(info, st_prn);
            strcat(info, "/");
            itoa(env.cloud_decay[env.cgrid[steth_x][steth_y]], st_prn, 10);
            strcat(info, st_prn);
            strcat(info, "   ");
            mpr(info);
        }

        if (mgrd[steth_x][steth_y] == NON_MONSTER)
        {
            itoa((int) igrd[steth_x][steth_y], st_prn, 10);
            strcpy(info, st_prn);
            mpr(info);

            // these two lines can't be important -- bwr
            // stth.move_x = 0;
            // stth.move_y = 0;
            return;
        }

        i = mgrd[steth_x][steth_y];
    }
    else
        i = mwh;

    strcpy(info, ptr_monam( &(menv[i]), 0));

    strcat(info, ". ID#");
    itoa(i, st_prn, 10);
    strcat(info, st_prn);

    strcat(info, ", type ");
    itoa(menv[i].type, st_prn, 10);
    strcat(info, st_prn);

    strcat(info, "   ");
    itoa(menv[i].hit_points, st_prn, 10);
    strcat(info, st_prn);
    strcat(info, "/");
    itoa(menv[i].max_hit_points, st_prn, 10);
    strcat(info, st_prn);

    strcat(info, " hp.  b/foe= ");
    itoa(menv[i].behavior, st_prn, 10);
    strcat(info, st_prn);
    strcat(info, "/");
    itoa(menv[i].foe, st_prn, 10);
    strcat(info, st_prn);
    mpr(info);

    strcpy(info, "speed ");
    itoa(menv[i].speed, st_prn, 10);
    strcat(info, st_prn);
    strcat(info, ", inc ");
    itoa(menv[i].speed_increment, st_prn, 10);
    strcat(info, st_prn);
    mpr(info);

    sprintf(info, "number: %d   flags: %02x", menv[i].number,
        menv[i].flags);
    mpr(info);

    strcpy(info, "target: ");
    itoa(menv[i].target_x, st_prn, 10);
    strcat(info, st_prn);
    strcat(info, ", ");
    itoa(menv[i].target_y, st_prn, 10);
    strcat(info, st_prn);
    mpr(info);

    sprintf(info, "enchts: [ %d %d %d %d %d %d ]",
        menv[i].enchantment[0], menv[i].enchantment[1],
        menv[i].enchantment[2], menv[i].enchantment[3],
        menv[i].enchantment[4], menv[i].enchantment[5]);
    mpr(info);

    if (menv[i].type == MONS_PLAYER_GHOST)
    {
        strcpy(info, "Ghost damage: ");
        itoa(ghost.values[7], st_prn, 10);
        strcat(info, st_prn);
        mpr(info);
    }
}                               // end stethoscope()


//---------------------------------------------------------------
//
// debug_add_skills
//
//---------------------------------------------------------------
void debug_add_skills(void)
{
    char specs[50];

    mpr("Which skill (by name, not number)? ");

#if defined(LINUX)
    echo();
    getstr(specs);
    noecho();
#elif defined(MAC) || defined(WIN32CONSOLE)
    getstr(specs, sizeof(specs));
#else
    gets(specs);
#endif

    int skill = -1;

    for (int i = 0; i < NUM_SKILLS; i++)
    {
        // avoid the bad values:
        if (i == SK_UNUSED_1 || (i > SK_UNARMED_COMBAT && i < SK_SPELLCASTING))
            continue;

        char sk_name[50];
        strcpy( sk_name, skill_name(i) );

        char *ptr = strstr( strlwr(sk_name), strlwr(specs) );
        if (ptr != NULL)
        {
            if (ptr == sk_name && strlen(specs) > 0)
            {
                // we prefer prefixes over partial matches
                skill = i;
                break;
            }
            else
                skill = i;
        }
    }

    mpr("Exercising...");

    if (skill == -1)
        mpr("That skill doesn't seem to exist.");
    else
        exercise(skill, 100);
}                               // end debug_add_skills()


void error_message_to_player(void)
{
    mpr("Oh dear. There appears to be a bug in the program.");
    mpr("I suggest you leave this level then save as soon as possible.");

}                               // end error_message_to_player()

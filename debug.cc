/*
 *  File:       debug.cc
 *  Summary:    Debug and wizard related functions.
 *  Written by: Linley Henzell and Jesse Jones
 *
 *  Change History (most recent first):
 *
 *               <3>     5/06/99        JDJ             Added TRACE.
 *               <2>     -/--/--        JDJ             Added a bunch od debugging macros.
 *                                                                      Old code is now #if WIZARD.
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "debug.h"

#ifdef DOS
#include <conio.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "direct.h"
#include "externs.h"
#include "enum.h"
#include "itemname.h"
#include "player.h"
#include "shopping.h"
#include "dungeon.h"
#include "stuff.h"
#include "monplace.h"
#include "mstruct.h"
#include "skills.h"
#include "spell.h"

#define WIZARD


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
    MSG msg;                    // remove pending quit messages so the message box displays

    bool quitting = (bool)::PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);

    char text[2500];

    int flags = MB_YESNO +      // want abort and ignore buttons (too bad we can't ditch the retry button...)
     MB_ICONERROR +             // display the icon for errors
     MB_TASKMODAL +             // don't let the user do anything else in the app
     MB_SETFOREGROUND;          // bring the app to the front

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
        sub eax, 0x10
          xor eax,[threadID]
          mov[obfuscator], eax
    }
#else
    _asm
    {
        mov ax, fs
          mov es, ax
          mov eax, 18 h
          mov eax, es:[eax]
        sub eax, 10 h
          xor eax,[threadID]
          mov[obfuscator], eax
    }
#endif

    const DWORD *processDatabase = reinterpret_cast < const DWORD * >(processID ^ obfuscator);

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

        IsDebuggerPresentProc proc = (IsDebuggerPresentProc)::GetProcAddress(kernelH, "IsDebuggerPresent");

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
      sConsole = ::GetStdHandle(STD_ERROR_HANDLE);
        if (sConsole == INVALID_HANDLE_VALUE)
            sConsole = NULL;

        // Set some options
        if (sConsole != NULL)
        {
          VERIFY(::SetConsoleTextAttribute(sConsole, FOREGROUND_GREEN));
                                                                        // green text on a black background (there doesn't appear to be a way to get black text)

          VERIFY(::SetConsoleTitle("Debug Log"));

            COORD size =
            {80, 120};

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
    const char *fileName = file + strlen(file);         // strip off path

    while (fileName > file && fileName[-1] != '\\')
        --fileName;

    sprintf(mesg, "ASSERT(%s) in '%s' at line %d failed.", expr, fileName, line);
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
void DEBUGSTR(const char *format,...)
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
void TRACE(const char *format,...)
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
void cast_spec_spell()
{
    char specs[3];

    strcpy(info, "Cast which spell? ");
    mpr(info);

    specs[0] = getche();
    specs[1] = getche();
    specs[2] = getche();

    your_spells(atoi(specs), magic_ability(player_mag_abil(), you.intel), 0);
}
#endif


//---------------------------------------------------------------
//
// create_spec_monster
//
//---------------------------------------------------------------
#ifdef WIZARD
void create_spec_monster()
{
    char specs[3];

    strcpy(info, "Create which monster? ");
    mpr(info);

    specs[0] = getche();
    specs[1] = getche();
    specs[2] = getche();

    create_monster(atoi(specs), 0, 0, you.x_pos, you.y_pos, MHITNOT, 250);
}


//---------------------------------------------------------------
//
// level_travel
//
//---------------------------------------------------------------
void level_travel()
{
    char specs[3];

    strcpy(info, "Travel to which level? ");
    mpr(info);

    specs[0] = getche();
    specs[1] = getche();
    specs[2] = 0;

    you.your_level = atoi(specs);
}


//---------------------------------------------------------------
//
// create_spec_object
//
//---------------------------------------------------------------
void create_spec_object()
{
    int class_wanted = 0;
    int type_wanted = 0;

    strcpy(info, "Create which item (class then type)? ");
    mpr(info);

    class_wanted = (getche() - 48) * 10;
    class_wanted += getche() - 48;

    type_wanted = (getche() - 48) * 10;
    type_wanted += getche() - 48;

    itoa(property(class_wanted, type_wanted, 2), st_prn, 10);
    strcpy(info, st_prn);
    mpr(info);

    int thing_created = items(1, class_wanted, type_wanted, 1, you.your_level, 250);

    if (you.species != SP_NAGA)
        strcpy(info, "Something appears at your feet!");
    else
        strcpy(info, "Something appears before you!");
    mpr(info);

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
void create_spec_object2()
{
    int class_wanted = 0;
    int type_wanted = 0;
    int dam_wanted = 0;

    strcpy(info, "Create which item (class, type, then dam)? ");
    mpr(info);

    class_wanted = (getche() - 48) * 10;
    class_wanted += getche() - 48;

    type_wanted = (getche() - 48) * 10;
    type_wanted += getche() - 48;

    dam_wanted = (getche() - 48) * 10;
    dam_wanted += getche() - 48;

//itoa(property [class_wanted] [type_wanted] [2], st_prn, 10);
    //strcpy(info, st_prn);
    //mpr(info);

//int thing_created = items(you.unique_items, 1, you.item_description,
    //       grd, class_wanted, type_wanted, 1, 100, 250);

    int thing_created = items(1, class_wanted, type_wanted, 1, you.your_level, 250);


    mitm.special[thing_created] = dam_wanted;

    if (you.species != SP_NAGA)
        strcpy(info, "Something appears at your feet!");
    else
        strcpy(info, "Something appears before you!");
    mpr(info);

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
// STETH can't examine spaces in cardinal directions more than 1 space from you

    struct dist stth[1];
    int i;

    if (mwh == 250)
    {
        strcpy(info, "Which monster?");
        mpr(info);

        direction(1, stth);

        if (stth[0].nothing == -1)
            return;

        if (env.cgrid[you.x_pos + stth[0].move_x][you.y_pos + stth[0].move_y] != CNG)
        {
            itoa(env.cloud_type[env.cgrid[you.x_pos + stth[0].move_x][you.y_pos + stth[0].move_y]], st_prn, 10);
            strcpy(info, st_prn);
            strcat(info, "/");
            itoa(env.cloud_decay[env.cgrid[you.x_pos + stth[0].move_x][you.y_pos + stth[0].move_y]], st_prn, 10);
            strcat(info, st_prn);
            strcat(info, "   ");
            mpr(info);
        }

        if (mgrd[you.x_pos + stth[0].move_x][you.y_pos + stth[0].move_y] == MNG)
        {
            //      strcpy(info, "There isn't anything there.!");

            itoa((int) igrd[you.x_pos + stth[0].move_x][you.y_pos + stth[0].move_y], st_prn, 10);
            strcpy(info, st_prn);
//strcat(info, "/");
            //itoa(menv [i].hit_points, st_prn, 10);
            //strcpy(info, st_prn);
            mpr(info);

            //mpr(info);
            stth[0].move_x = 0;
            stth[0].move_y = 0;
            return;
        }

        i = mgrd[you.x_pos + stth[0].move_x][you.y_pos + stth[0].move_y];

    }
    else
        i = mwh;

    strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));       //gmon_name [mons_class [i]]);

    strcat(info, ": ");

    {

        itoa(i, st_prn, 10);
        strcat(info, st_prn);
        strcat(info, ": cl ");

        itoa(menv[i].type, st_prn, 10);
        strcat(info, st_prn);
        strcat(info, " - ");

        itoa(menv[i].hit_points, st_prn, 10);
        strcat(info, st_prn);
        strcat(info, "/");
        itoa(menv[i].max_hit_points, st_prn, 10);
        strcat(info, st_prn);
        strcat(info, "   ");

        itoa(menv[i].behavior, st_prn, 10);
        strcat(info, st_prn);
        strcat(info, "/");
        itoa(menv[i].monster_foe, st_prn, 10);
        strcat(info, st_prn);
        mpr(info);

        strcpy(info, "speed:");
        itoa(menv[i].speed, st_prn, 10);
        strcat(info, st_prn);
        strcat(info, "/");
        itoa(menv[i].speed_increment, st_prn, 10);
        strcat(info, st_prn);
        mpr(info);


        strcpy(info, "sec:");
        itoa(menv[i].number, st_prn, 10);
        strcat(info, st_prn);
        mpr(info);

        strcpy(info, "target: ");
        itoa(menv[i].target_x, st_prn, 10);
        strcat(info, st_prn);
        strcat(info, ", ");
        itoa(menv[i].target_y, st_prn, 10);
        strcat(info, st_prn);
        mpr(info);

        strcpy(info, "ench0: ");
        itoa(menv[i].enchantment[0], st_prn, 10);
        strcat(info, st_prn);
        strcat(info, " ench1: ");
        itoa(menv[i].enchantment[1], st_prn, 10);
        strcat(info, st_prn);
        strcat(info, " ench2: ");
        itoa(menv[i].enchantment[2], st_prn, 10);
        strcat(info, st_prn);
        mpr(info);

        strcpy(info, "Ghost damage: ");
        itoa(ghost.values[7], st_prn, 10);
        strcat(info, st_prn);
        mpr(info);

    }
    stth[0].move_x = 0;
    stth[0].move_y = 0;

    return;
}


//---------------------------------------------------------------
//
// debug_add_skills
//
//---------------------------------------------------------------
void debug_add_skills()
{
    char specs[2];

    strcpy(info, "Practice which skill? ");
    mpr(info);

    specs[0] = getche();
    specs[1] = getche();

    exercise(atoi(specs), 100);

}

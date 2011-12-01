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
#include <ctype.h>

#ifdef DOS
#include <conio.h>
#endif

#include "globals.h"
#include "externs.h"

#include "chardump.h"
#include "direct.h"
#include "dungeon.h"
#include "files.h"
#include "invent.h"
#include "itemname.h"
#include "itemprop.h"
#include "items.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mon-util.h"
#include "mutation.h"
#include "newgame.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "skills.h"
#include "skills2.h"
#include "spl-cast.h"
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

#if OSX
    fprintf(stderr, mesg);
// raise(SIGINT);               // this is what DebugStr() does on OS X according to Tech Note 2030
    int* p = NULL;              // but this gives us a stack crawl...
    *p = 0;
#elif MAC
     unsigned char s[50];

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

    int flags = (MB_YESNO    // want abort and ignore buttons
                             // (too bad we can't ditch the retry button...)
        | MB_ICONERROR       // display the icon for errors
        | MB_TASKMODAL       // don't let the user do anything else in the app
        | MB_SETFOREGROUND); // bring the app to the front

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


#if DEBUG
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
#endif

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

//---------------------------------------------------------------
//
// debug_prompt_for_monster
//
//---------------------------------------------------------------
#ifdef WIZARD
static int debug_prompt_for_monster( void )
{
    char  specs[80];
    char  obj_name[ ITEMNAME_SIZE ];
    char *ptr;

    mpr( MSGCH_PROMPT, "(Hint: 'generated' names, eg 'orc zombie', won't work)" );
    mpr( MSGCH_PROMPT, "Which monster by name? " );
    get_input_line( specs, sizeof( specs ) );

    if (specs[0] == '\0')
        return (-1);

    int mon = -1;

    for (int i = 0; i < NUM_MONSTERS; i++)
    {
        moname( i, true, DESC_PLAIN, obj_name );

        ptr = strstr( strlwr(obj_name), strlwr(specs) );
        if (ptr != NULL)
        {
            mpr( obj_name );
            if (ptr == obj_name)
            {
                // we prefer prefixes over partial matches
                mon = i;
                break;
            }
            else
                mon = i;
        }
    }

    return (mon);
}
#endif

//---------------------------------------------------------------
//
// debug_prompt_for_skill
//
//---------------------------------------------------------------
#ifdef WIZARD
static int debug_prompt_for_skill( const char *prompt )
{
    char specs[80];

    mpr( MSGCH_PROMPT, prompt );
    get_input_line( specs, sizeof( specs ) );

    if (specs[0] == '\0')
        return (-1);

    int skill = -1;

    for (int i = 0; i < NUM_SKILLS; i++)
    {
        // avoid the bad values:
        if (i == SK_UNUSED_1 || (i > SK_UNARMED_COMBAT && i < SK_SPELLCASTING))
            continue;

        char sk_name[80];
        strncpy( sk_name, skill_name(i), sizeof( sk_name ) );

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

    return (skill);
}
#endif

//---------------------------------------------------------------
//
// debug_change_species
//
//---------------------------------------------------------------
#ifdef WIZARD
void debug_change_species( void )
{
    char specs[80];
    int i;

    mpr( MSGCH_PROMPT, "What species would you like to be now? "  );
    get_input_line( specs, sizeof( specs ) );

    if (specs[0] == '\0')
        return;

    int sp = -1;

    for (i = SP_HUMAN; i < NUM_SPECIES; i++)
    {
        char sp_name[80];

        strncpy( sp_name, species_name( i, 27 ), sizeof( sp_name ) );

        char *ptr = strstr( strlwr(sp_name), strlwr(specs) );
        if (ptr != NULL)
        {
            if (ptr == sp_name && strlen(specs) > 0)
            {
                // we prefer prefixes over partial matches
                sp = i;
                break;
            }
            else
                sp = i;
        }
    }

    if (sp == -1)
        mpr( "That species isn't available." );
    else
    {
        for (i = 0; i < NUM_SKILLS; i++)
        {
            you.skill_points[i] *= species_skills( i, sp );
            you.skill_points[i] /= species_skills( i, you.species );
        }

        if (sp == SP_MUMMY)
            you.is_undead = US_UNDEAD;
        else if (sp == SP_GHOUL)
            you.is_undead = US_HUNGRY_DEAD;
        else
            you.is_undead = US_ALIVE;

        you.species = sp;
        redraw_screen();
    }
}
#endif
//---------------------------------------------------------------
//
// debug_prompt_for_int
//
// If nonneg, then it returns a non-negative number or -1 on fail
// If !nonneg, then it returns an integer, and 0 on fail
//
//---------------------------------------------------------------
#ifdef WIZARD
static int debug_prompt_for_int( const char *prompt, bool nonneg )
{
    char specs[80];

    mpr( MSGCH_PROMPT, prompt );
    get_input_line( specs, sizeof( specs ) );

    if (specs[0] == '\0')
        return ((nonneg) ? -1 : 0);

    char *end;
    int   ret = strtol( specs, &end, 10 );

    if ((ret < 0 && nonneg) || (ret == 0 && end == specs))
        ret = ((nonneg) ? -1 : 0);

    return (ret);
}
#endif

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
    int spell = debug_prompt_for_int( "Cast which spell by number? ", true );

    if (spell == -1)
        canned_msg( MSG_OK );
    else
        your_spells( spell, -1, false );
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
    char specs[80];
    char spname[80];

    mpr( MSGCH_PROMPT, "Cast which spell by name? " );
    get_input_line( specs, sizeof( specs ) );

    for (i = 0; i < NUM_SPELLS; i++)
    {
        strncpy( spname, spell_title(i), sizeof( spname ) );

        if (strstr( strlwr(spname), strlwr(specs) ) != NULL)
        {
            your_spells( i, -1, false );
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
    int mon = debug_prompt_for_int( "Create which monster by number? ", true );

    if (mon == -1)
        canned_msg( MSG_OK );
    else
    {
        const int mid = create_monster( mon, BEH_SLEEP );

        if (mid != -1)
        {
            if (mon == MONS_PANDEMONIUM_DEMON)
                generate_random_demon();

#if DEBUG_DIAGNOSITICS
            mpr( MSGCH_DIAGNOSTICS, "%s (id #%d) has been created.",
                 ptr_monam( &menv[mid], DESC_CAP_A ), mid );
#endif
        }

        if (mon == MONS_PANDEMONIUM_DEMON)
            generate_random_demon();
    }
}                               // end create_spec_monster()
#endif


//---------------------------------------------------------------
//
// create_spec_monster_name
//
//---------------------------------------------------------------
#ifdef WIZARD
void create_spec_monster_name(void)
{
    int mon = debug_prompt_for_monster();

    if (mon == -1)
    {
        mpr("I couldn't find that monster.");

        if (one_chance_in(20))
            mpr("Maybe it's hiding.");
    }
    else
    {
        const int mid = create_monster( mon, BEH_SLEEP );

        if (mid != -1)
        {
            if (mon == MONS_PANDEMONIUM_DEMON)
                generate_random_demon();

#if DEBUG_DIAGNOSITICS
            mpr( MSGCH_DIAGNOSTICS, "%s (id #%d) has been created.",
                 ptr_monam( &menv[mid], DESC_CAP_A ), mid );
#endif
        }
    }
}                               // end create_spec_monster_name()
#endif


//---------------------------------------------------------------
//
// level_travel
//
//---------------------------------------------------------------
#ifdef WIZARD
void level_travel( int delta )
{
    int   old_level = you.depth;
    int   new_level = you.depth + delta;

    if (delta == 0)
    {
        new_level = debug_prompt_for_int( "Travel to which level? ", true ) - 1;
    }

    if (new_level < 0 || new_level >= 50)
    {
        mpr( "That level is out of bounds." );
        return;
    }

    you.depth = new_level - 1;
    down_stairs( DNGN_STONE_STAIRS_DOWN_I, old_level, false );
    untag_followers();
}                               // end level_travel()
#endif


//---------------------------------------------------------------
//
// create_spec_object
//
//---------------------------------------------------------------
#ifdef WIZARD
void create_spec_object(void)
{
    static int max_subtype[] =
    {
        NUM_WEAPONS,
        NUM_MISSILES,
        NUM_ARMOURS,
        NUM_WANDS,
        NUM_FOODS,
        0,              // unknown I
        NUM_SCROLLS,
        NUM_JEWELLERY,
        NUM_POTIONS,
        0,              // unknown II
        NUM_BOOKS,
        NUM_STAVES,
        0,              // Orbs         -- only one, handled specially
        NUM_MISCELLANY,
        0,              // corpses      -- handled specially
        0,              // gold         -- handled specially
        0,              // "gemstones"  -- no items of type
    };
    char           specs[80];
    char           obj_name[ ITEMNAME_SIZE ];
    char           keyin;

    char *         ptr;
    int            best_index;
    int            mon;
    int            i;
    int            pluses;

    int            class_wanted   = OBJ_UNASSIGNED;
    int            type_wanted    = -1;
    int            special_wanted = 0;

    int            thing_created;

    for (;;)
    {
        mpr( "Creating a specific object of type:" );
        mpr( MSGCH_PROMPT, ") - weapons     ( - missiles  [ - armour  / - wands    ?  - scrolls" );
        mpr( MSGCH_PROMPT, "= - jewellery   ! - potions   : - books   | - staves   0  - The Orb" );
        // XXX: get mpr() to handle escaping % correctly
        mpr( MSGCH_PROMPT, "} - miscellany  & - corpses   %% - food    $ - gold    ESC - exit" );

        mpr(MSGCH_PROMPT,"What class of item? " );

        keyin = get_ch();

        if (keyin == ESCAPE || keyin == ' ' || keyin == '\r' || keyin == '\n')
        {
            canned_msg( MSG_OK );
            return;
        }

        class_wanted = conv_char_to_obj_class( keyin );

        if (class_wanted != OBJ_UNASSIGNED)
            break;
    }

    // allocate an item to play with:
    thing_created = get_item_slot();
    if (thing_created == NON_ITEM)
    {
        mpr( "Could not allocate item." );
        return;
    }

    // turn item into appropriate kind:
    if (class_wanted == OBJ_ORBS)
    {
        mitm[thing_created].base_type = OBJ_ORBS;
        mitm[thing_created].sub_type  = ORB_ZOT;
        mitm[thing_created].quantity  = 1;
    }
    else if (class_wanted == OBJ_GOLD)
    {
        int amount = debug_prompt_for_int( "How much gold? ", true );
        if (amount <= 0)
        {
            canned_msg( MSG_OK );
            return;
        }

        mitm[thing_created].base_type = OBJ_GOLD;
        mitm[thing_created].sub_type = 0;
        mitm[thing_created].quantity = amount;
    }
    else if (class_wanted == OBJ_CORPSES)
    {
        mon = debug_prompt_for_monster();

        if (mon == -1)
        {
            mpr( "No such monster." );
            return;
        }

        mitm[thing_created].base_type = OBJ_CORPSES;
        mitm[thing_created].sub_type  = CORPSE_BODY;
        mitm[thing_created].plus      = mon;
        mitm[thing_created].plus2     = 0;
        mitm[thing_created].special   = 210;
        mitm[thing_created].colour    = mons_class_colour(mon);
        mitm[thing_created].quantity  = 1;
        mitm[thing_created].flags     = 0;
    }
    else
    {
        mpr( MSGCH_PROMPT, "What type of item? " );
        get_input_line( specs, sizeof( specs ) );

        if (specs[0] == '\0')
        {
            canned_msg( MSG_OK );
            return;
        }

        // In order to get the sub-type, we'll fill out the base type...
        // then we're going to iterate over all possible subtype values
        // and see if we get a winner. -- bwr
        mitm[thing_created].base_type = class_wanted;
        mitm[thing_created].sub_type  = 0;
        mitm[thing_created].plus      = 0;
        mitm[thing_created].plus2     = 0;
        mitm[thing_created].special   = 0;
        mitm[thing_created].flags     = 0;
        mitm[thing_created].quantity  = 1;
        set_ident_flags( mitm[thing_created], ISFLAG_IDENT_MASK );

        if (class_wanted == OBJ_ARMOUR)
        {
            if (strstr( "cap", specs ))
            {
                mitm[thing_created].sub_type = ARM_CAP;
                mitm[thing_created].plus2 = THELM_CAP;
            }
            if (strstr( "wizard's hat", specs ))
            {
                mitm[thing_created].sub_type = ARM_CAP;
                mitm[thing_created].plus2 = THELM_WIZARD_HAT;
            }
            else if (strstr( "helm", specs ))
            {
                mitm[thing_created].sub_type = ARM_HELMET;
                mitm[thing_created].plus2 = THELM_HELM;
            }
        }

        if (!mitm[thing_created].sub_type)
        {
            type_wanted = -1;
            best_index  = 10000;

            for (i = 0; i < max_subtype[ mitm[thing_created].base_type ]; i++)
            {
                mitm[thing_created].sub_type = i;
                item_name( mitm[thing_created], DESC_PLAIN, obj_name );

                ptr = strstr( strlwr(obj_name), strlwr(specs) );
                if (ptr != NULL)
                {
                    // earliest match is the winner
                    if (ptr - obj_name < best_index)
                    {
                        mpr( obj_name );
                        type_wanted = i;
                        best_index = ptr - obj_name;
                    }
                }
            }

            if (type_wanted == -1)
            {
                // ds -- if specs is a valid int, try using that.
                //       Since zero is atoi's copout, the wizard
                //       must enter (subtype + 1).
                type_wanted = atoi(specs);
                if (!type_wanted)
                {
                    mpr( "No such item." );
                    return;
                }
                type_wanted--;
            }

            mitm[thing_created].sub_type = type_wanted;
        }

        switch (mitm[thing_created].base_type)
        {
        case OBJ_MISSILES:
            mitm[thing_created].quantity = 30;
            // intentional fall-through
        case OBJ_WEAPONS:
        case OBJ_ARMOUR:
            mpr( MSGCH_PROMPT, "What ego type? " );
            get_input_line( specs, sizeof( specs ) );

            if (specs[0] != '\0')
            {
                special_wanted = 0;
                best_index = 10000;

                for (i = 1; i < 25; i++)
                {
                    mitm[thing_created].special = i;
                    item_name( mitm[thing_created], DESC_PLAIN, obj_name );

                    ptr = strstr( strlwr(obj_name), strlwr(specs) );
                    if (ptr != NULL)
                    {
                        // earliest match is the winner
                        if (ptr - obj_name < best_index)
                        {
                            mpr( obj_name );
                            special_wanted = i;
                            best_index = ptr - obj_name;
                        }
                    }
                }

                mitm[thing_created].special = special_wanted;
            }
            break;

        case OBJ_JEWELLERY:
            pluses = ring_has_pluses( mitm[thing_created] );

            if (pluses)
            {
                mpr( MSGCH_PROMPT, "Plus value? " );
                get_input_line( specs, sizeof( specs ) );

                if (specs[0] == '\0')
                    return;

                char *end;
                int   value = strtol( specs, &end, 10 );

                if (end == specs)
                    value = MAX_RING_PLUS;

                mitm[thing_created].plus = value;

                if (pluses == 2)
                {
                    mpr( MSGCH_PROMPT, "Plus2 value? " );
                    get_input_line( specs, sizeof( specs ) );

                    if (specs[0] == '\0')
                        return;

                    value = strtol( specs, &end, 10 );

                    if (end == specs)
                        value = MAX_RING_PLUS;

                    mitm[thing_created].plus2 = value;
                }
            }
            break;

        case OBJ_BOOKS:
            if (mitm[thing_created].sub_type == BOOK_MANUAL)
            {
                special_wanted = debug_prompt_for_skill( "A manual for which skill? " );
                if (special_wanted != -1)
                    mitm[thing_created].plus = special_wanted;
                else
                    mpr( "Sorry, no books on that skill today." );
            }
            break;

        case OBJ_WANDS:
            mitm[thing_created].plus = 24;
            break;

        case OBJ_STAVES:
            if (item_is_rod( mitm[thing_created] ))
            {
                mitm[thing_created].plus = MAX_ROD_CHARGE * ROD_CHARGE_MULT;
                mitm[thing_created].plus2 = MAX_ROD_CHARGE * ROD_CHARGE_MULT;
            }
            break;

        case OBJ_MISCELLANY:
            // Runes to "demonic", decks have 50 cards, ignored elsewhere?
            mitm[thing_created].plus = 50;
            break;

        case OBJ_FOOD:
        case OBJ_SCROLLS:
        case OBJ_POTIONS:
            mitm[thing_created].quantity = 12;
            break;

        default:
            break;
        }
    }

    item_colour( mitm[thing_created] );

    move_item_to_grid( &thing_created, you.x_pos, you.y_pos );

    if (thing_created != NON_ITEM)
        canned_msg( MSG_SOMETHING_APPEARS );

}

void create_object( bool make_good )
{
    static int max_subtype[] =
    {
        NUM_WEAPONS,
        NUM_MISSILES,
        NUM_ARMOURS,
        NUM_WANDS,
        NUM_FOODS,
        0,              // unknown I
        NUM_SCROLLS,
        NUM_JEWELLERY,
        NUM_POTIONS,
        0,              // unknown II
        NUM_BOOKS,
        NUM_STAVES,
        0,              // Orbs         -- only one, handled specially
        NUM_MISCELLANY,
        0,              // corpses      -- handled specially
        0,              // gold         -- handled specially
        0,              // "gemstones"  -- no items of type
    };

    char           specs[80];
    char           obj_name[ ITEMNAME_SIZE ];
    char           keyin;

    char *         ptr;
    int            best_index;
    int            i;

    int            class_wanted   = OBJ_UNASSIGNED;
    int            type_wanted    = -1;

    int            thing_created;

    for (;;)
    {
        if (make_good)
            mpr("Creating random good objects of type:");
        else
            mpr("Creating random objects of type:");

        mpr( MSGCH_PROMPT, ") weapons     ( missiles  [ armour  / wands    ? scrolls" );
        mpr( MSGCH_PROMPT, "= jewellery   ! potions   : books   | staves   0 The Orb" );
        mpr( MSGCH_PROMPT, "} miscellany  X corpses   %% food    $ gold     * random" );

        mpr(MSGCH_PROMPT,"What class of item? " );

        keyin = toupper( get_ch() );

        if (keyin == ')')
            class_wanted = OBJ_WEAPONS;
        else if (keyin == '(')
            class_wanted = OBJ_MISSILES;
        else if (keyin == '[' || keyin == ']')
            class_wanted = OBJ_ARMOUR;
        else if (keyin == '/' || keyin == '\\')
            class_wanted = OBJ_WANDS;
        else if (keyin == '?')
            class_wanted = OBJ_SCROLLS;
        else if (keyin == '=' || keyin == '"')
            class_wanted = OBJ_JEWELLERY;
        else if (keyin == '!')
            class_wanted = OBJ_POTIONS;
        else if (keyin == ':')
            class_wanted = OBJ_BOOKS;
        else if (keyin == '|')
            class_wanted = OBJ_STAVES;
        else if (keyin == '0' || keyin == 'O')
            class_wanted = OBJ_ORBS;
        else if (keyin == '}' || keyin == '{')
            class_wanted = OBJ_MISCELLANY;
        else if (keyin == 'X')
            class_wanted = OBJ_CORPSES;
        else if (keyin == '%')
            class_wanted = OBJ_FOOD;
        else if (keyin == '$')
            class_wanted = OBJ_GOLD;
        else if (keyin == '*')
            class_wanted = OBJ_RANDOM;
        else if (keyin == ESCAPE || keyin == ' '
                || keyin == '\r' || keyin == '\n')
        {
            canned_msg( MSG_OK );
            return;
        }

        if (class_wanted != OBJ_UNASSIGNED)
            break;
    }

    if (class_wanted != OBJ_RANDOM)
    {
        type_wanted = OBJ_RANDOM;

        mpr( MSGCH_PROMPT, "What type of item? " );
        get_input_line( specs, sizeof( specs ) );

        if (specs[0] != '\0' && specs[0] != '*')
        {
            item_def       item;                // for generating names

            item.base_type = class_wanted;
            item.plus = 0;
            item.plus2 = 0;
            item.special = 0;
            item.flags = 0;
            item.quantity = 0;
            set_ident_flags( item, ISFLAG_IDENT_MASK );

            best_index  = 10000;

            for (i = 0; i < max_subtype[ class_wanted ]; i++)
            {
                item.sub_type  = i;
                item_name( item, DESC_PLAIN, obj_name );

                ptr = strstr( strlwr(obj_name), strlwr(specs) );
                if (ptr != NULL)
                {
                    // earliest match is the winner
                    if (ptr - obj_name < best_index)
                    {
                        mpr( obj_name );
                        type_wanted = i;
                        best_index = ptr - obj_name;
                    }
                }
            }
        }
    }

    mpr( MSGCH_PROMPT, "Number to create? " );
    get_input_line( specs, sizeof( specs ) );

    if (specs[0] == '\0')
        return;

    char *end;
    int   num = strtol( specs, &end, 10 );

    if (num == 0 || end == specs)
        num = 1;

    bool succ = false;
    for (i = 0; i < num; i++)
    {
        thing_created = make_item( 1, class_wanted, type_wanted, true,
                                   (make_good) ? MAKE_GOOD_ITEM : you.depth,
                                   MAKE_ITEM_RANDOM_RACE );

        if (thing_created != NON_ITEM)
        {
            succ = true;
            set_ident_flags( mitm[thing_created], ISFLAG_IDENT_MASK );
            move_item_to_grid( &thing_created, you.x_pos, you.y_pos );
        }
    }

    if (succ)
        canned_msg( MSG_SOMETHING_APPEARS );
}
#endif


//---------------------------------------------------------------
//
// create_spec_object
//
//---------------------------------------------------------------
#ifdef WIZARD
void tweak_object(void)
{
    char specs[50];
    char keyin;

    int item = prompt_invent_item( "Tweak which item? ", -1 );
    if (item == PROMPT_ABORT)
    {
        canned_msg( MSG_OK );
        return;
    }

    if (item == you.equip[EQ_WEAPON])
        set_redraw_status( REDRAW_WIELD );

    for (;;)
    {
        void *field_ptr = NULL;

        for (;;)
        {
            item_name( you.inv[item], DESC_INVENTORY_EQUIP, info );
            mpr( info );

            mpr( MSGCH_PROMPT, "a - plus  b - plus2  c - special  d - quantity  ESC - exit" );
            mpr( MSGCH_PROMPT, "Which field? " );

            keyin = tolower( get_ch() );

            if (keyin == 'a')
                field_ptr = &(you.inv[item].plus);
            else if (keyin == 'b')
                field_ptr = &(you.inv[item].plus2);
            else if (keyin == 'c')
                field_ptr = &(you.inv[item].special);
            else if (keyin == 'd')
                field_ptr = &(you.inv[item].quantity);
            else if (keyin == ESCAPE || keyin == ' '
                    || keyin == '\r' || keyin == '\n')
            {
                canned_msg( MSG_OK );
                return;
            }

            if (keyin >= 'a' && keyin <= 'd')
                break;
        }

        if (keyin != 'c')
        {
            const short *const ptr = static_cast< short * >( field_ptr );
            snprintf( info, INFO_SIZE, "Old value: %d (0x%04x)", *ptr, *ptr );
        }
        else
        {
            const long *const ptr = static_cast< long * >( field_ptr );
            snprintf( info, INFO_SIZE, "Old value: %ld (0x%08lx)", *ptr, *ptr );
        }

        mpr( info );

        mpr( MSGCH_PROMPT, "New value? " );
        get_input_line( specs, sizeof( specs ) );

        if (specs[0] == '\0')
            return;

        char *end;
        int   new_value = strtol( specs, &end, 10 );

        if (new_value == 0 && end == specs)
            return;

        if (keyin != 'c')
        {
            short *ptr = static_cast< short * >( field_ptr );
            *ptr = new_value;
        }
        else
        {
            long *ptr = static_cast< long * >( field_ptr );
            *ptr = new_value;
        }
    }
}
#endif


//---------------------------------------------------------------
//
// stethoscope
//
//---------------------------------------------------------------
#if DEBUG_DIAGNOSTICS

static const char *enchant_names[] =
{
    "None",
    "Slow", "Haste", "BUG:3", "Fear", "Conf", "Invis",
    "Pois", "BUG:YPois-2", "BUG:YPois-3", "BUG:YPois-4",
    "BUG:11", "BUG:12", "BUG:13", "BUG:14",
    "Rot", "BUG:YRot-2", "BUG:YRot-3", "BUG:YRot-4",
    "BUG:Summon", "Summoned", "BUG:Abj-2", "BUG:Abj-3", "BUG:Abj-4",
    "BUG:Abj-5", "BUG:Abj-6",
    "Corona", "BUG:Corona-2", "BUG:Corona-3", "BUG:Corona-4",
    "Charm", "BUG:YSticky-1", "BUG:YSticky-2", "BUG:YSticky-3", "BUG:YSticky-4",
    "BUG:35", "BUG:36", "BUG:37",
    "GlowShapeshifter", "Shapeshifter",
    "Tele", "BUG:Tele-2", "BUG:Tele-3", "BUG:Tele-4",
    "BUG:44", "BUG:45", "BUG:46", "BUG:47", "BUG:48", "BUG:49",
    "BUG:50", "BUG:51", "BUG:52", "BUG:53", "BUG:54", "BUG:55",
    "BUG:56",
    "BUG:Pois-1", "BUG:Pois-2", "BUG:Pois-3", "BUG:Pois-4",
    "Sticky", "BUG:Sticky-2", "BUG:Sticky-3", "BUG:Sticky-4",
    "BUG:65", "BUG:66", "BUG:67", "BUG:68", "BUG:69", "BUG:70",
    "BUG:71", "SleepWary", "Submerged", "Short Lived", "Petrify", "Paralysis",
    "Quiet", "Stun", "Cut",
    "BUG:TooBig1", "BUG:TooBig2", "BUG:TooBig3", "BUG:TooBig4", "BUG:TooBig5",
};

void stethoscope(int mwh)
{
    struct dist stth;
    int steth_x, steth_y;
    int i, j;

    if (mwh != RANDOM_MONSTER)
        i = mwh;
    else
    {
        mpr( MSGCH_PROMPT, "Which monster?" );

        direction( stth );

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
            mpr( MSGCH_DIAGNOSTICS, "cloud type: %d delay: %d",
                     env.cloud[ env.cgrid[steth_x][steth_y] ].type,
                     env.cloud[ env.cgrid[steth_x][steth_y] ].decay );
        }

        if (mgrd[steth_x][steth_y] == NON_MONSTER)
        {
            mpr( MSGCH_DIAGNOSTICS, "item grid = %d", igrd[steth_x][steth_y] );
            return;
        }

        i = mgrd[steth_x][steth_y];
    }

    // print type of monster
    mpr( MSGCH_DIAGNOSTICS,
        "%s (id #%d; type=%d loc=(%d,%d) align=%s flags=%04x)",
              monam( menv[i].number, menv[i].type, true, DESC_CAP_THE ),
              i, menv[i].type,
              menv[i].x, menv[i].y,
              ((menv[i].attitude == ATT_FRIENDLY) ? "friendly" :
               (menv[i].attitude == ATT_HOSTILE)  ? "hostile" :
               (menv[i].attitude == ATT_NEUTRAL)  ? "neutral"
                                                  : "unknown alignment"),
              menv[i].flags );

    const int fly = mons_flies( &menv[i] );

    // print stats and other info
    mpr( MSGCH_DIAGNOSTICS,
        "HD=%d HP=%d/%d AC=%d EV=%d(%d) SH=%d(%d) MR=%d SPD=%d%s PER=%d energy=%d",
             menv[i].hit_dice,
             menv[i].hit_points, menv[i].max_hit_points,
             menv[i].armour_class,
             menv[i].evasion, mons_weapon_ev_bonus( &menv[i] ),
             mons_shield_class( &menv[i] ),
             menv[i].shield_blocks,
             mons_resist_magic( &menv[i] ),

             mons_speed( &menv[i] ),
             ((fly == FLY_POWERED)              ? "f" :
              (fly == FLY_LEVITATION)           ? "l" :
              mons_stationary( menv[i].type )   ? "s" : ""),

             mons_perception( &menv[i] ),
             menv[i].energy );

    // print behaviour information

    const int hab = monster_habitat( menv[i].type );

    mpr( MSGCH_DIAGNOSTICS,
            "num=%d hab=%s beh=%s(%d) foe=%s(%d) mem=%d target=(%d,%d)",
             menv[i].number,
             ((hab == DNGN_DEEP_WATER)            ? "water" :
              (hab == DNGN_LAVA)                  ? "lava"
                                                  : "floor"),

             ((menv[i].behaviour == BEH_SLEEP)       ? "sleep" :
              (menv[i].behaviour == BEH_WANDER)      ? "wander" :
              (menv[i].behaviour == BEH_INVESTIGATE) ? "investigate" :
              (menv[i].behaviour == BEH_SEEK)        ? "seek" :
              (menv[i].behaviour == BEH_FLEE)        ? "flee" :
              (menv[i].behaviour == BEH_PANIC)       ? "panic" :
              (menv[i].behaviour == BEH_CORNERED)    ? "cornered"
                                                     : "unknown"),
             menv[i].behaviour,

             ((menv[i].foe == MHITYOU)            ? "you" :
              (menv[i].foe == MHITNOT)            ? "none" :
              (menv[menv[i].foe].type == -1)      ? "unassigned"
                 : monam( menv[menv[i].foe].number, menv[menv[i].foe].type,
                          true, DESC_PLAIN )),
             menv[i].foe,
             menv[i].foe_memory,

             menv[i].target_x, menv[i].target_y );

    // print resistances
    mpr( MSGCH_DIAGNOSTICS, "res: fire=%d%s cold=%d%s elec=%d pois=%d neg=%d",
              mons_res_fire( &menv[i] ),
              (mons_class_resist( menv[i].type, MR_RES_HELLFIRE ) ? "*" : ""),
              mons_res_cold( &menv[i] ),
              (mons_class_resist( menv[i].type, MR_RES_HELLFROST ) ? "*" : ""),
              mons_res_elec( &menv[i] ),
              mons_res_poison( &menv[i] ),
              mons_res_negative_energy( &menv[i] ) );

    // print enchantments
    strncpy( info, "ench: ", INFO_SIZE );
    for (j = 0; j < NUM_MON_ENCHANTS; j++)
    {
        char buff[80];

        if (menv[i].ench[j].type >= 0 && menv[i].ench[j].type < NUM_ENCHANTMENTS)
            strncat( info, enchant_names[ menv[i].ench[j].type ], INFO_SIZE );
        else
        {
            snprintf( buff, sizeof(buff), "BUG:%d", menv[i].ench[j].type );
            strncat( info, buff, INFO_SIZE );
        }

        snprintf( buff, sizeof(buff), "(%d)", menv[i].ench[j].duration );
        strncat( info, buff, INFO_SIZE );

        if (strlen( info ) <= 70)
            strncat( info, " ", INFO_SIZE );
        else if (j < 5)
        {
            mpr( MSGCH_DIAGNOSTICS, info );
            strncpy( info, "ench: ", INFO_SIZE );
        }
    }

    mpr( MSGCH_DIAGNOSTICS, info );

    if (menv[i].type == MONS_PLAYER_GHOST
        || menv[i].type == MONS_PANDEMONIUM_DEMON)
    {
        snprintf( info, INFO_SIZE, "Ghost damage: %d; brand: %d",
                  env.ghost.values[ GVAL_DAMAGE ], env.ghost.values[ GVAL_BRAND ] );
        mpr( MSGCH_DIAGNOSTICS, info );
    }
    else
    {
        mpr( MSGCH_DIAGNOSTICS, "attacks: %d %d %d %d",
                  mons_damage( &menv[i], 0 ),
                  mons_damage( &menv[i], 1 ),
                  mons_damage( &menv[i], 2 ),
                  mons_damage( &menv[i], 3 ) );
    }
}                               // end stethoscope()
#endif

#if DEBUG_ITEM_SCAN
//---------------------------------------------------------------
//
// dump_item
//
//---------------------------------------------------------------
static void dump_item( const char *name, int num, const item_def &item )
{
    mpr( MSGCH_WARN, name );

    mpr( "    item #%d:  base: %d; sub: %d; plus: %d; plus2: %d; special: %ld",
             num, item.base_type, item.sub_type,
             item.plus, item.plus2, item.special );

    mpr( "    quant: %d; colour: %d; ident: 0x%08lx; ident_type: %d",
             item.quantity, item.colour, item.flags,
             get_ident_type( item.base_type, item.sub_type ) );

    mpr( "    x: %d; y: %d; link: %d; slot: %c",
             item.x, item.y, item.link,
             (item.slot == -1) ? '-' : index_to_letter( item.slot ) );
}

//---------------------------------------------------------------
//
// debug_item_scan
//
//---------------------------------------------------------------
void debug_item_scan( void )
{
    int   i;
    char  name[256];

    // unset marks
    for (i = 0; i < MAX_ITEMS; i++)
        mitm[i].flags &= (~ISFLAG_DEBUG_MARK);

    // First we're going to check all the stacks on the level:
    for (int x = 0; x < GXM; x++)
    {
        for (int y = 0; y < GYM; y++)
        {
            // These are unlinked monster inventory items -- skip them:
            if (x == 0 && y == 0)
                continue;

            // Looking for infinite stacks (ie more links than tems allowed)
            // and for items which have bad coordinates (can't find their stack)
            for (int obj = igrd[x][y]; obj != NON_ITEM; obj = mitm[obj].link)
            {
                // Check for invalid (zero quantity) items that are linked in
                if (!is_valid_item( mitm[obj] ))
                {
                    mpr( MSGCH_WARN, "Linked invalid item at (%d,%d)!", x, y);
                    item_name( mitm[obj], DESC_PLAIN, name );
                    dump_item( name, obj, mitm[obj] );
                }

                // Check that item knows what stack it's in
                if (mitm[obj].x != x || mitm[obj].y != y)
                {
                    mpr( MSGCH_WARN, "Item position incorrect at (%d,%d)!", x, y);
                    item_name( mitm[obj], DESC_PLAIN, name );
                    dump_item( name, obj, mitm[obj] );
                }

                // If we run into a premarked item we're in real trouble,
                // this will also keep this from being an infinite loop.
                if (mitm[obj].flags & ISFLAG_DEBUG_MARK)
                {
                    mpr( MSGCH_WARN, "Potential INFINITE STACK at (%d, %d)", x, y );
                    break;
                }

                mitm[obj].flags |= ISFLAG_DEBUG_MARK;
            }
        }
    }

    // Now scan all the items on the level:
    for (i = 0; i < MAX_ITEMS; i++)
    {
        if (!is_valid_item( mitm[i] ))
            continue;

        item_name( mitm[i], DESC_PLAIN, name );

        // Don't check (-1,-1) player items or (0,0) monster items
        if ((mitm[i].x > 0 || mitm[i].y > 0)
            && !(mitm[i].flags & ISFLAG_DEBUG_MARK))
        {
            mpr( MSGCH_WARN, "Unlinked item:" );
            dump_item( name, i, mitm[i] );

            mpr( "igrd(%d,%d) = %d",
                  mitm[i].x, mitm[i].y, igrd[ mitm[i].x ][ mitm[i].y ] );

            // Let's check to see if it's an errant monster object:
            for (int j = 0; j < MAX_MONSTERS; j++)
            {
                for (int k = 0; k < NUM_MONSTER_SLOTS; k++)
                {
                    if (menv[j].inv[k] == i)
                    {
                        mpr( "Held by monster #%d: %s at (%d,%d)",
                             j, ptr_monam( &menv[j], DESC_CAP_A ),
                             menv[j].x, menv[j].y );
                    }
                }
            }
        }

        // Current bad items of interest:
        //   -- armour and weapons with large enchantments/illegal special vals
        //
        //   -- items described as questionable (the class 100 bug)
        //
        //   -- eggplant is an illegal throwing weapon
        //
        //   -- bola is an illegal fixed artefact
        //
        //   -- items described as buggy (typically adjectives out of range)
        //      (note: covers buggy, bugginess, buggily, whatever else)
        //
        if (strstr( name, "questionable" ) != NULL
            || strstr( name, "eggplant" ) != NULL
            || strstr( name, "bola" ) != NULL
            || strstr( name, "bugg" ) != NULL)
        {
            mpr( MSGCH_WARN, "Bad item:" );
            dump_item( name, i, mitm[i] );
        }
        else if ((mitm[i].base_type == OBJ_WEAPONS
                && (abs(mitm[i].plus) > 30
                    || abs(mitm[i].plus2) > 30
                    || (!is_random_artefact( mitm[i] )
                        && (mitm[i].special >= 30
                            && mitm[i].special < 181))))

            || (mitm[i].base_type == OBJ_MISSILES
                && (abs(mitm[i].plus) > 25
                    || (!is_random_artefact( mitm[i] )
                        && mitm[i].special >= 30)))

            || (mitm[i].base_type == OBJ_ARMOUR
                && (abs(mitm[i].plus) > 25
                    || (!is_random_artefact( mitm[i] )
                        && mitm[i].sub_type != ARM_HELMET
                        && mitm[i].special >= 30))))
        {
            mpr( MSGCH_WARN, "Bad plus or special value:" );
            dump_item( name, i, mitm[i] );
        }
    }

    // Don't want debugging marks interfering with anything else.
    for (i = 0; i < MAX_ITEMS; i++)
        mitm[i].flags &= (~ISFLAG_DEBUG_MARK);

    // Quickly scan monsters for "program bug"s.
    for (i = 0; i < MAX_MONSTERS; i++)
    {
        const struct monsters *const monster = &menv[i];

        if (monster->type == -1)
            continue;

        moname( monster->type, true, DESC_PLAIN, name );

        if (strcmp( name, "program bug" ) == 0)
        {
            mpr( MSGCH_WARN,
                  "Buggy monster detected: monster #%d; position (%d,%d)",
                  i, monster->x, monster->y );

        }

        if (mgrd[monster->x][monster->y] != i)
        {
            mpr( MSGCH_WARN,
                  "monster grid problem: monster #%d; grid (%d,%d) says %d",
                  i, monster->x, monster->y, mgrd[monster->x][monster->y] );
        }
    }
}
#endif

//---------------------------------------------------------------
//
// debug_add_skills
//
//---------------------------------------------------------------
#ifdef WIZARD
void debug_add_skills(void)
{
    int skill = debug_prompt_for_skill( "Which skill (by name)? " );

    if (skill == -1)
        mpr("That skill doesn't seem to exist.");
    else
    {
        mpr("Exercising...");
        exercise(skill, 100);
    }
}                               // end debug_add_skills()
#endif

//---------------------------------------------------------------
//
// debug_set_skills
//
//---------------------------------------------------------------
#ifdef WIZARD
void debug_set_skills(void)
{
    int skill = debug_prompt_for_skill( "Which skill (by name)? " );

    if (skill == -1)
        mpr("That skill doesn't seem to exist.");
    else
    {
        mpr( skill_name(skill) );
        int amount = debug_prompt_for_int( "To what level? ", true );

        if (amount == -1)
            canned_msg( MSG_OK );
        else
        {
            const int points = (skill_exp_needed( amount + 1 )
                                * species_skills( skill, you.species )) / 100;

            you.skill_points[skill] = points + 1;
            you.skills[skill] = amount;

            calc_total_skill_points();

            set_redraw_status( REDRAW_SKILL );

            switch (skill)
            {
            case SK_FIGHTING:
                calc_hp_max();
                break;

            case SK_SPELLCASTING:
            case SK_INVOCATIONS:
            case SK_EVOCATIONS:
                calc_mp_max();
                break;

            case SK_DODGING:
                set_redraw_status( REDRAW_EVASION );
                break;

            case SK_ARMOUR:
                set_redraw_status( REDRAW_ARMOUR_CLASS );
                set_redraw_status( REDRAW_EVASION );
                break;

            default:
                break;
            }
        }
    }
}                               // end debug_add_skills()
#endif


//---------------------------------------------------------------
//
// debug_set_all_skills
//
//---------------------------------------------------------------
#ifdef WIZARD
void debug_set_all_skills( int amount )
{
    int i;

    if (amount == -1)
        amount = debug_prompt_for_int( "Set all skills to what level? ", true );

    if (amount < 0)             // cancel returns -1 -- bwr
        canned_msg( MSG_OK );
    else
    {
        if (amount > 27)
            amount = 27;

        for (i = SK_FIGHTING; i < NUM_SKILLS; i++)
        {
            if (i == SK_UNUSED_1
                || (i > SK_UNARMED_COMBAT && i < SK_SPELLCASTING))
            {
                continue;
            }

            const int points = (skill_exp_needed( amount + 1 )
                                * species_skills( i, you.species )) / 100;

            you.skill_points[i] = points + 1;
            you.skills[i] = amount;
        }

        set_redraw_status( REDRAW_SKILL );

        calc_total_skill_points();

        calc_hp_max();
        calc_mp_max();

        set_redraw_status( REDRAW_ARMOUR_CLASS );
        set_redraw_status( REDRAW_EVASION );
    }
}                               // end debug_add_skills()
#endif


//---------------------------------------------------------------
//
// debug_add_mutation
//
//---------------------------------------------------------------
#ifdef WIZARD
bool debug_add_mutation(void)
{
    bool success = false;
    char specs[80];

    // Yeah, the gaining message isn't too good for this... but
    // there isn't an array of simple mutation names. -- bwr
    mpr( MSGCH_PROMPT, "Which mutation (by message when getting mutation)? " );
    get_input_line( specs, sizeof( specs ) );

    if (specs[0] == '\0')
        return (false);

    int mutation = -1;

    for (int i = 0; i < NUM_MUTATIONS; i++)
    {
        char mut_name[80];
        strncpy( mut_name, mutation_desc( i, 1 ), sizeof( mut_name ) );

        char *ptr = strstr( strlwr(mut_name), strlwr(specs) );
        if (ptr != NULL)
        {
            // we take the first mutation that matches
            mutation = i;
            break;
        }
    }

    if (mutation == -1)
        mpr("I can't warp you that way!");
    else
    {
        mpr( "Found: %s", mutation_desc( mutation, 1 ) );

        int levels = debug_prompt_for_int( "How many levels? ", false );

        if (levels == 0)
        {
            canned_msg( MSG_OK );
            success = false;
        }
        else if (levels > 0)
        {
            for (int i = 0; i < levels; i++)
            {
                if (mutate( mutation, true ))
                    success = true;
            }
        }
        else
        {
            for (int i = 0; i < -levels; i++)
            {
                if (delete_mutation( mutation, true ))
                    success = true;
            }
        }
    }

    return (success);
}                               // end debug_add_mutation()
#endif


//---------------------------------------------------------------
//
// debug_get_religion
//
//---------------------------------------------------------------
#ifdef WIZARD
void debug_get_religion(void)
{
    char specs[80];

    mpr( MSGCH_PROMPT, "Which god (by name)? " );
    get_input_line( specs, sizeof( specs ) );

    if (specs[0] == '\0')
        return;

    int god = -1;

    for (int i = 1; i < NUM_GODS; i++)
    {
        char name[80];
        strncpy( name, god_name(i), sizeof( name ) );

        char *ptr = strstr( strlwr(name), strlwr(specs) );
        if (ptr != NULL)
        {
            god = i;
            break;
        }
    }

    if (god == -1)
        mpr( "That god doesn't seem to be taking followers today." );
    else
    {
        grd[you.x_pos][you.y_pos] = 179 + god;
        god_pitch(god);
    }
}                               // end debug_add_skills()
#endif

#ifdef WIZARD
//---------------------------------------------------------------
//
// debug_destroy_item
//
//---------------------------------------------------------------
// Issues to watch for here:
// - equipment cannot be destroyed... not only is this the more accurate
//   than testing for curse status (to prevent easy removal of cursed items),
//   but the original code would leave all the equiped items properties
//   (including weight) which would cause a bit of a mess to state.
void debug_destroy_item( void )
{
    int i;
    char str_pass[ ITEMNAME_SIZE ];

    // ask the item to destroy
    int item = prompt_invent_item( "Destroy which item? ", -1,
                                   true, false, true, '.' );

    if (item == PROMPT_ABORT)
        return;

    if (item == PROMPT_GOT_SPECIAL)
    {
        destroy_item_stack( you.x_pos, you.y_pos );
        mpr( "Done." );
        return;
    }

    // Used to check for cursed... but that's not the real problem -- bwr
    for (i = 0; i < NUM_EQUIP; i++)
    {
        if (you.equip[i] == item)
        {
            mesclr( true );
            mpr( "You cannot destroy equipped items!" );
            return;
        }
    }

    // ask confirmation
    // quant_name(you.inv[item], you.inv[item].quantity, DESC_NOCAP_A, str_pass );
    item_name( you.inv[item], DESC_NOCAP_THE, str_pass );
    snprintf( info, INFO_SIZE, "Destroy %s? ", str_pass );

    if (yesno( info, true ))
    {
       //destroy it!!
        mpr( "You destroy %s.", str_pass );
        dec_inv_item_quantity( item, you.inv[item].quantity );
        burden_change();
    }
}
#endif

#ifdef DEBUG_DUMP_SKILL_TOTALS

bool class_allowed( unsigned char speci, int char_class, bool check_valid = false );
void init_player( void );

void dump_skill_totals(void)
{
    Options.random_pick = false;
    Options.chaos_knight = GOD_RANDOM;
    Options.death_knight = DK_RANDOM;
    Options.priest = GOD_RANDOM;
    Options.weapon = WPN_RANDOM;

    for (int job = JOB_FIGHTER; job < NUM_JOBS; job++)
    {
        for (int sp = SP_HUMAN; sp < NUM_SPECIES; sp++)
        {
            if (sp == SP_WHITE_DRACONIAN)
                sp = SP_CENTAUR;

            if (class_allowed( sp, job ))
            {
                init_player();

                you.species = sp;
                you.char_class = job;
                new_game();

                strcpy( you.your_name, get_class_name( you.char_class ) );

                snprintf( info, INFO_SIZE, "%s%s",
                          get_species_abbrev( you.species ),
                          get_class_abbrev( you.char_class ) );

                dump_char( info, true );

                int total_levels = 0;

                for (int i = 0; i < MAX_SKILLS; i++)
                    total_levels += you.skills[i];

                calc_total_skill_points();

                printf( "%d\t%d\t%20s\t%20s\n", you.total_skill_points,
                                                total_levels,
                                                get_class_name( job ),
                                                species_name( sp, 1 ) );
            }
        }
    }

    exit(0);
}
#endif

void error_message_to_player(void)
{
    mpr("Oh dear. There appears to be a bug in the program.");
    mpr("I suggest you leave this level then save as soon as possible.");

}                               // end error_message_to_player()

/*
 *  File:       AppHdr.h
 *  Summary:    Precompiled header used by Crawl.
 *  Written by: Jesse Jones
 *
 * Abstract: CodeWarrior and MSVC both support precompiled headers which can
 *      significantly speed up compiles. Unlike CodeWarrior MSVC imposes
 *      some annoying restrictions on precompiled headers: the precompiled
 *      header *must* be the first include in all cc files. Any includes or
 *      other statements that occur before the pch include are ignored. This
 *      is really stupid and can lead to bizarre errors, but it does mean
 *      that we shouldn't run into any problems on systems without precompiled
 *      headers.
 *
 *  Copyright © 1999 Jesse Jones.
 *
 *  Change History (most recent first):
 *
 *       <7>    9 May 2000  GDL     Added Windows 32 bit console support
 *       <6>    24mar2000   jmf     Added a whole slew of new options, which
 *                                  ought to be mandatory :-)
 *       <5>     10/12/99   BCR     Added USE_NEW_RANDOM #define
 *       <4>     9/25/99    CDL     linuxlib -> liblinux
 *       <3>     6/18/99    BCR     Moved the CHARACTER_SET #define here from
 *                                  linuxlib.cc.  Also wrapped the #define
 *                                  MACROS to prevent it from being used by
 *                                  Linux.
 *       <2>     6/17/99    BCR     Removed 'linux' check, replaced it with
 *                                  'LINUX' check.  Now need to be -DLINUX
 *                                  during compile.  Also moved
 *                                  CHARACTER_SET #define here from
 *                                  linuxlib.cc
 *       <1>     5/30/99    JDJ     Created (from config.h)
 */


#ifndef APPHDR_H
#define APPHDR_H

#if _MSC_VER >= 1100            // note that we can't just check for _MSC_VER: most compilers will wind up defining this in order to work with the SDK headers...
#pragma message("Compiling AppHeader.h (this message should only appear once)")
#endif


// =========================================================================
//  System Defines
// =========================================================================
#ifdef SOLARIS
    // Most of the linux stuff applies, and so we want it
    #define LINUX
    #define PLAIN_TERM
    #define MULTIUSER
    #include "liblinux.h"

    // The ALTCHARSET may come across as DEC characters/JIS on non-ibm platforms
    #define CHARACTER_SET           0

    #define USE_CURSES
    #define EOL "\n"

    // This is used for Posix termios.
    #define USE_POSIX_TERMIOS

    // This is used for BSD tchars type ioctl, use this if you can't
    // use the Posix support above.
    // #define USE_TCHARS_IOCTL
    //
    // This uses Unix signal control to block some things, may be
    // useful in conjunction with USE_TCHARS_IOCTL.
    //
    #define USE_UNIX_SIGNALS

    // This is for systems with no usleep... comment if you have it.
    #define USE_SELECT_FOR_DELAY

    // Default to non-ibm character set
    #define USE_ASCII_CHARACTERS

// Define plain_term for linux and similar, and dos_term for DOS and EMX.
#elif defined(LINUX)

    #define PLAIN_TERM
    #define CHARACTER_SET           A_ALTCHARSET
    //#define CHARACTER_SET           0
    //#define USE_ASCII_CHARACTERS

    #define USE_CURSES
    #define EOL "\n"

    #include <string>
    #include "liblinux.h"

// To compile with EMX for OS/2 define USE_EMX macro with compiler command line
// (already defined in supplied makefile.emx)
#elif defined(USE_EMX)
    #define DOS_TERM
    #define EOL "\n"
    #define CHARACTER_SET           A_ALTCHARSET

    #include <string>
    #include "libemx.h"

#elif _MSC_VER >= 1100
    #include <string>
    #include "WinHdr.h"
    #error MSVC is not supported yet
    #define CHARACTER_SET           A_ALTCHARSET

// macintosh is predefined on all the common Mac compilers
#elif defined(macintosh)
    #define MAC 1
    #define PLAIN_TERM
    #define HAS_NAMESPACES  1
    #define EOL "\r"
    #define CHARACTER_SET           A_ALTCHARSET

    #include <string>
    #include "MacHdr.h"
    #include "libmac.h"

#elif defined(DOS)
    #define DOS_TERM
    #define SHORT_FILE_NAMES
    #define EOL "\n\r"
    #define CHARACTER_SET           A_ALTCHARSET

    #include <string>

#elif defined(WIN32CONSOLE) && (defined(__IBMCPP__) || defined(__BCPLUSPLUS__))
    #include "libw32c.h"
    #define PLAIN_TERM
    #define SHORT_FILE_NAMES
    #define EOL "\n"
    #define CHARACTER_SET           A_ALTCHARSET
    #define getstr(X,Y)         getConsoleString(X,Y)
#else
    #error unsupported compiler
#endif


// =========================================================================
//  Debugging Defines
// =========================================================================
#ifdef _DEBUG       // this is how MSVC signals a debug build
    #define DEBUG       1
#else
//  #define DEBUG   0 // leave this undefined for those lamers who use #ifdef
#endif

#if DEBUG
    #if __MWERKS__
        #define MSIPL_DEBUG_MODE
    #endif
#else
    #if !defined(NDEBUG)
        #define NDEBUG                  // used by <assert.h>
    #endif
#endif


// =========================================================================
//  Game Play Defines
// =========================================================================
#ifdef USE_CURSES
    #define NUMBER_OF_LINES   LINES
#elif MAC
    #define NUMBER_OF_LINES   30
#else
    #define NUMBER_OF_LINES   25
#endif

// if this works out okay, eventually we can change this to USE_OLD_RANDOM
#define USE_NEW_RANDOM

// Uncomment this if you find the labyrinth to be buggy and what to
// remove it from the game.
// #define SHUT_LABYRINTH

// Define MACRO if you want to use the macro patch in macro.cc.
// #define MACROS

// Set this to the number of runes that will be required to enter Zot's
// domain.  You shouldn't set this really high unless you want to
// make players spend far too much time in Pandemonium/The Abyss.
//
// Traditional setting of this is one rune, three is pretty standard now.
#define NUMBER_OF_RUNES_NEEDED    3

// Number of top scores to keep.
#define SCORE_FILE_ENTRIES      100

// ================================================= --------------------------
//jmf: New defines for a bunch of optional features.
// ================================================= --------------------------

// Give Grey elves a "glamour" ability (random confuse, charm, sleep humanoids)
#define USE_ELVISH_GLAMOUR_ABILITY

// Give Wizards better starting spellbooks
#define USE_BETTER_MINOR_MAGIC_BOOKS

// New cloud code
#define USE_NEW_CLOUD_CODE

// New silence code -- seems to actually work! Use it!
#define USE_SILENCE_CODE

// Transformation && (Species + Mutation) AC Bonuses NOT Cumulative
#define USE_HARDER_AC_RULES

// Branch-tailored altar selection
#define USE_NEW_ALTAR_CODE

// A few new mini-vaults, featuring altars
#define USE_NEW_MINIVAULTS

// Use special colours for all god-related messages
#define USE_GOD_COLOURS

// Wizard death option (needed to test new death messages)
#define USE_OPTIONAL_WIZARD_DEATH

// Lighter scrolls, rings, potions and spellbooks
#define USE_LIGHTER_MAGIC_ITEMS

// Better torment code
#define USE_NEW_TORMENT_CODE

// Semi-Controlled Blink
#define USE_SEMI_CONTROLLED_BLINK

// Use new system for weighting str and dex based on weapon type -- bwr
#define USE_NEW_COMBAT_STATS

// Use this is you want the occasional spellcaster or ranger type wanderer
// to show up... comment it if you find these types silly or too powerful,
// or just want fighter type wanderers.
#define USE_SPELLCASTER_AND_RANGER_WANDERER_TEMPLATES

// LRH's skill-pool drainer for high skill pools
// #define USE_SKILL_POOL_DRAIN

// ====================== -----------------------------------------------------
//jmf: end of new defines
// ====================== -----------------------------------------------------


#ifdef MULTIUSER
    // Define SAVE_DIR to the directory where saves, bones, and score file
    // will go... end it with a '\'.  Since all player files will be in the
    // same directory, the players UID will be appended when this option
    // is set.
    //
    // Setting it to nothing or not setting it will cause all game files to
    // be dumped in the current directory.
    //
    #define SAVE_DIR_PATH       "/opt/local/newcrawl/lib/"

    // will make this little thing go away.  Define SAVE_PACKAGE_CMD
    // to a command to compress and bundle the save game files into a
    // single unit... the two %s will be replaced with the players
    // save file name.  Define LOAD_UNPACKAGE_CMD to undo this process
    // the %s is the same as above.
    //
    // PACKAGE_SUFFIX is used when the package file name is needed
    //
    // Comment these lines out if you want to leave the save files uncompressed.
    //
    #define SAVE_PACKAGE_CMD    "/opt/bin/zip -m -q -j -1 %s.zip %s.*"
    #define LOAD_UNPACKAGE_CMD  "/opt/bin/unzip -q -o %s.zip -d" SAVE_DIR_PATH
    #define PACKAGE_SUFFIX      ".zip"

    // This provides some rudimentary protection against people using
    // save file cheats on multi-user systems.
    #define DO_ANTICHEAT_CHECKS

    // This defines the chmod permissions for score and bones files.
    #define SHARED_FILES_CHMOD_VAL  0664

    // If we're on a multiuser system, file locking of shared files is
    // very important (else things will just keep getting corrupted)
    #define USE_FILE_LOCKING

    // Define this if you'd rather have the game block on locked files,
    // commenting it will poll the file lock once a second for thirty
    // seconds before giving up.
    #define USE_BLOCKING_LOCK

#endif

// ===========================================================================
//  Misc
// ===========================================================================
#if HAS_NAMESPACES
    using namespace std;
#endif

template < class T >
inline void UNUSED(const volatile T &)
{
}                               // Note that this generates no code with CodeWarrior or MSVC (if inlining is on).

#endif

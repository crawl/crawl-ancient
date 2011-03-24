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
 *       <9>    9 Aug 2001   MV     Added USE_RIVERS,USE_NEW_UNRANDS
 *                                  and MISSILE_TRAILS_OFF #define
 *       <8>   10 May 2001  GDL     Added FreeBSD support
 *                                  courtesy Andrew E. Filonov
 *       <7>    9 May 2000  GDL     Added Windows 32 bit console support
 *       <6>    24mar2000   jmf     Added a whole slew of new options, which
 *                                  ought to be mandatory :-)
 *       <5>     10/12/99   BCR     Added USE_NEW_RANDOM #define
 *       <4>     9/25/99    CDL     linuxlib -> liblinux
 *       <3>     6/18/99    BCR     Moved the CHARACTER_SET #define here from
 *                                  linuxlib.cc.  Also wrapped the #define
 *                                  USE_MACROS to prevent it from being used by
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
// Define plain_term for linux and similar, and dos_term for DOS and EMX.
#ifdef LINUX

    #define PLAIN_TERM
    #define USE_ASCII_CHARACTERS
    #define MULTIUSER

    // Set curses include file if you don't want the default curses.h
    #define USE_CURSES
    // #define CURSES_INCLUDE_FILE     <ncurses.h>
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

    #include <string>
    #include "liblinux.h"

#elif defined(SOLARIS)
    // Most of the linux stuff applies, and so we want it
    #define LINUX
    #define PLAIN_TERM
    #define MULTIUSER
    #include "liblinux.h"

    // Set curses include file if you don't want the default curses.h
    #define USE_CURSES
    // #define CURSES_INCLUDE_FILE     <ncurses.h>
    #define EOL "\n"

    // This is used for Posix termios.
    #define USE_POSIX_TERMIOS

    // This is used for BSD tchars type ioctl, use this if you can't
    // use the Posix support above.
    // #define USE_TCHARS_IOCTL

    // This uses Unix signal control to block SIGQUIT and SIGINT,
    // which can be annoying (especially since control-Y sends
    // SIGQUIT).
    #define USE_UNIX_SIGNALS

    // This is for older versions of Solaris... comment if you have it.
    // #define NEED_USLEEP

    // Default to non-ibm character set
    #define USE_ASCII_CHARACTERS

#elif defined (HPUX)
    // Most of the linux stuff applies, and so we want it
    #define LINUX
    #define PLAIN_TERM
    #define MULTIUSER
    #include "liblinux.h"

    // Set curses include file if you don't want the default curses.h
    // Under HP-UX its typically easier to use ncurses than try and
    // get the colour curses library to work. -- bwr
    #define USE_CURSES
    #define CURSES_INCLUDE_FILE         <ncurses.h>
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
    // #define NEED_USLEEP

    // Default to non-ibm character set
    #define USE_ASCII_CHARACTERS

// Define plain_term for linux and similar, and dos_term for DOS and EMX.
#elif defined ( BSD )
    // Most of the linux stuff applies, and so we want it
    #define LINUX
    #define PLAIN_TERM
//#define MULTIUSER
    #include "liblinux.h"

    // Set curses include file if you don't want the default curses.h
    #define USE_CURSES
    // #define CURSES_INCLUDE_FILE     <ncurses.h>
    #define EOL "\n"

    // This is used for Posix termios.
    #define USE_POSIX_TERMIOS

    // This is used for BSD tchars type ioctl, use this if you can't
    // use the Posix support above.
    #define USE_TCHARS_IOCTL

    // This uses Unix signal control to block some things, may be
    // useful in conjunction with USE_TCHARS_IOCTL.
    //
    // #define USE_UNIX_SIGNALS

    // Default to non-ibm character set
    #define USE_ASCII_CHARACTERS


// To compile with EMX for OS/2 define USE_EMX macro with compiler command line
// (already defined in supplied makefile.emx)
#elif defined(USE_EMX)
    #define DOS_TERM
    #define EOL "\n"

    #include <string>
    #include "libemx.h"

#elif _MSC_VER >= 1100
    #include <string>
    #include "WinHdr.h"
    #error MSVC is not supported yet

// macintosh is predefined on all the common Mac compilers
#elif defined(macintosh)
    #define PLAIN_TERM
    #define HAS_NAMESPACES  1
    #define EOL "\r"
    #include <string>
    #include "libmac.h"

#if OSX
    #define USE_8_COLOUR_TERM_MAP

    // Darkgrey is a particular problem in the 8 colour mode.  Popular
    // values for replacing it around here are: WHITE, BLUE, and MAGENTA.
    #define COL_TO_REPLACE_DARKGREY     MAGENTA
#endif

#elif defined(DOS)
    #define DOS_TERM
    #define SHORT_FILE_NAMES
    #define EOL "\r\n"

    #include <string>

    #ifdef __DJGPP__
        #define NEED_SNPRINTF
    #endif

#elif defined(WIN32CONSOLE) && (defined(__IBMCPP__) || defined(__BCPLUSPLUS__) || defined(__MINGW32__))
    #include "libw32c.h"
    #define PLAIN_TERM
    #define SHORT_FILE_NAMES
    #define EOL "\n"
    #define getstr(X,Y)         getConsoleString(X,Y)
#else
    #error unsupported compiler
#endif


// =========================================================================
//  Debugging Defines
// =========================================================================
#ifdef FULLDEBUG
    // Bounds checking and asserts
    #define DEBUG       1

    // Outputs many "hidden" details, defaults to wizard on.
    //
    // Note that level 2 produces even more noise
    #define DEBUG_DIAGNOSTICS   1

    // Scan for bad items before every input (may be slow)
    //
    // This function might slow things down quite a bit
    // on slow machines because it's going to go through
    // every item on the level and do string comparisons
    // against the name.  Still, it is nice to know the
    // turn in which "bad" items appear.
    #define DEBUG_ITEM_SCAN     1
#endif

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
//  Curses features:
// =========================================================================
#ifdef USE_CURSES
    // For cases when the game will be played on terms that don't support the
    // curses "bold == lighter" 16 colour mode. -- bwr
    //
    // Darkgrey is a particular problem in the 8 colour mode.  Popular values
    // for replacing it around here are: WHITE, BLUE, and MAGENTA.  THis
    // option has no affect in 16 colour mode. -- bwr
    //
    // #define USE_8_COLOUR_TERM_MAP
    // #define COL_TO_REPLACE_DARKGREY     MAGENTA
#endif

// =========================================================================
//  Game Play Defines
// =========================================================================
// number of back messages saved during play (currently none saved into files)
#define NUM_STORED_MESSAGES   1000

// Define USE_MACRO if you want to use the macro patch in macro.cc.
#define USE_MACROS

// Set this to the number of runes that will be required to enter Zot's
// domain.  You shouldn't set this really high unless you want to
// make players spend far too much time in Pandemonium/The Abyss.
//
// Traditional setting of this is one rune, but three is pretty standard now.
#define NUMBER_OF_RUNES_NEEDED    3

// Number of top scores to keep.
#define SCORE_FILE_ENTRIES      1000

// Darshan's item stack (stash) tracking code:
#define STASH_TRACKING

// Option to allow scoring of wizard characters.  Note that even if
// you define this option, wizard characters are still tagged as such
// in the score file.
// #define SCORE_WIZARD_CHARACTERS

// For those that prefer the field of view == move distance
//#define USE_SQUARE_FOV

// Use special colours for various channels of messages
#define USE_COLOUR_MESSAGES

// Wizard death option (needed to test new death messages)
#define USE_OPTIONAL_WIZARD_DEATH

// Semi-Controlled Blink
#define USE_SEMI_CONTROLLED_BLINK

// Use new system for weighting str and dex based on weapon type, -- bwr
// #define USE_NEW_COMBAT_STATS

// Use this is you want the occasional spellcaster or ranger type wanderer
// to show up... comment it if you find these types silly or too powerful,
// or just want fighter type wanderers.
// #define USE_SPELLCASTER_AND_RANGER_WANDERER_TEMPLATES

//mv: (new 9 Aug 01) switches on new rivers & lakes code
#define USE_RIVERS

//mv: (new 9 Aug 01) switches on new unrands
#define USE_NEW_UNRANDS

// bwr: use the new "twisty" passages (all alike) labyrinth
#define USE_NEW_LABYRINTH           1

// bwr: Make monsters "explode" and scatter their items on death.  This
//      is the maximum grid_distance an item can scatter (0 or undefine
//      for none).  This is a compile time option because it does affect
//      the game more than options normally allow.
#define SCATTER_RADIUS              2

// Define this to use GNU scientific library PRNGs.  This is not
// necessary, but may be useful in some rare situations (the MT19937
// generator we're packaging with the code now should be well beyond
// good enough for almost everybody).   Note that you will want to
// check to see that -lgsl and -lgslcblas are added to the LIB variable
// in the makefile you are using (also check that the the include
// path (probably -I/usr/local/include) is set in INCLUDES).
//
// Note that if you use this there may very well be license problems
// with distributing binaries.
//
// #define USE_GSL_RAND                1

#if USE_GSL_RAND

#include <gsl/gsl_rng.h>

// MT-19937 is both high quality and very fast.
#define GSL_RAND_GENERATOR          gsl_rng_mt19937

#endif

// This option will allow the use of the clib rand().  It's seriously
// not recommended as the typical implications of rand() are not nearly
// powerful enough for Crawl.  In fact, some (ie Borland with it's very
// small range) are downright catastropic and fail miserably with the
// current code.  Use this only as a last resort.
//
// #define USE_SYSTEM_RAND         1

// Use this to seed the PRNG with a bit more than just time()... turning
// this off is perfectly okay, the game just becomes more exploitable
// with a bit of hacking (ie only by people who know how).
//
// For now, we'll make it default to on for Linux (who should have
// no problems with compiling this).
#ifdef LINUX
#define USE_MORE_SECURE_SEED    1
#else
#define USE_MORE_SECURE_SEED    0
#endif

// mv: (new 9 Aug 01) turns off missile trails, might be slow on some computers
// #define MISSILE_TRAILS_OFF

// bwr: set this to non-zero if you want to know the pluses, "runed" status
// of the monster's weapons in the hiscore file.
#define HISCORE_WEAPON_DETAIL   1

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
    #define SAVE_DIR_PATH       "/crawl-anc/saves/"

    // will make this little thing go away.  Define SAVE_PACKAGE_CMD
    // to a command to compress and bundle the save game files into a
    // single unit... the two %s will be replaced with the players
    // save file name.  Define LOAD_UNPACKAGE_CMD to undo this process
    // the %s is the same as above.
    //
    // PACKAGE_SUFFIX is used when the package file name is needed
    //
    // Comment these lines out if you want to leave the save files uncompressed.
    // However, says Rax, if you do, the game won't be able to load save files.
    // I just left them defined rather than fixing the bug because I am a
    // laziness elemental.
    // #define SAVE_PACKAGE_CMD    "/usr/bin/zip -m -q -j -1 %s.zip %s.*"
    // #define LOAD_UNPACKAGE_CMD  "/usr/bin/unzip -q -o %s.zip -d" SAVE_DIR_PATH
    #define PACKAGE_SUFFIX      ".zip"

    // This provides some rudimentary protection against people using
    // save file cheats on multi-user systems.
    //#define DO_ANTICHEAT_CHECKS

    // This defines the chmod permissions for score and bones files.
    #define SHARED_FILES_CHMOD_PRIVATE  0644
    #define SHARED_FILES_CHMOD_PUBLIC   0644

    // If we're on a multiuser system, file locking of shared files is
    // very important (else things will just keep getting corrupted)
    #define USE_FILE_LOCKING

    // Define this if you'd rather have the game block on locked files,
    // commenting it will poll the file lock once a second for thirty
    // seconds before giving up.
    #define USE_BLOCKING_LOCK

// some files needed for file locking
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#endif

// ===========================================================================
//  Misc
// ===========================================================================
#if HAS_NAMESPACES
    using namespace std;
#endif

// Uncomment these if you can't find these functions on your system
// #define NEED_USLEEP
// #define NEED_SNPRINTF

// Must include libutil.h here if one of the above is defined.
#include "libutil.h"

template < class T >
inline void UNUSED(const volatile T &)
{
}                               // Note that this generates no code with CodeWarrior or MSVC (if inlining is on).

#endif

/*
 *  File:       debug.h
 *  Summary:    Debug and wizard related functions.
 *  Written by: Linley Henzell and Jesse Jones
 *
 *  Change History (most recent first):
 *
 *              <4>             5/30/99         JDJ             Added synch checks.
 *              <3>             5/06/99         JDJ             Added TRACE.
 *              <2>             -/--/--         JDJ             Added a bunch of debugging macros. Old code is now #if WIZARD.
 *              <1>             -/--/--         LRH             Created
 */
#ifndef DEBUG_H
#define DEBUG_H

// Synch with ANSI definitions.
#if DEBUG && defined(NDEBUG)
        #error DEBUG and NDEBUG are out of sync!
#endif

#if !DEBUG && !defined(NDEBUG)
        #error DEBUG and NDEBUG are out of sync!
#endif

// Synch with MSL definitions.
#if __MSL__ && DEBUG != defined(MSIPL_DEBUG_MODE)
        #error DEBUG and MSIPL_DEBUG_MODE are out of sync!
#endif

// Synch with MSVC.
#if _MSC_VER >= 1100 && DEBUG != defined(_DEBUG)
        #error DEBUG and _DEBUG are out of sync!
#endif


#ifndef _lint
#define COMPILE_CHECK(p)                        {struct _CC {char a[(p) ? 1 : -1];};} 0
#else
#define COMPILE_CHECK(p)
#endif

#ifdef DEBUG
void AssertFailed(const char *expr, const char *file, int line);

#define ASSERT(p)       do {if (!(p)) AssertFailed(#p, __FILE__, __LINE__);} while (false)
#define VERIFY(p)       ASSERT(p)

void DEBUGSTR(const char *format,...);
void TRACE(const char *format,...);

#else
#define ASSERT(p)       ((void) 0)
#define VERIFY(p)       do {if (p) ;} while (false)

inline void __DUMMY_TRACE__(...)
{
}
#define DEBUGSTR                                                1 ? ((void) 0) : __DUMMY_TRACE__
#define TRACE                                                   1 ? ((void) 0) : __DUMMY_TRACE__
#endif

void cast_spec_spell();
void cast_spec_spell_name();
void create_spec_monster();
void create_spec_monster_name();
void level_travel();
void create_spec_object();
void create_spec_object2();
void stethoscope(int mwh);
void debug_add_skills();
void error_message_to_player();

#endif // DEBUG_H

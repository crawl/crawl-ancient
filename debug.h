#ifndef DEBUG_H
#define DEBUG_H

#ifndef _lint
        #define COMPILE_CHECK(p)                        {struct _CC {char a[(p) ? 1 : -1];};} 0
#else
        #define COMPILE_CHECK(p)
#endif

#if DEBUG_BUILD
        void AssertFailed(const char* expr, const char* file, int line);

        #define ASSERT(p)       do {if (!(p)) AssertFailed(#p, __FILE__, __LINE__);} while (false)

        #define VERIFY(p)       ASSERT(p)
#else
        #define ASSERT(p)       ((void) 0)
        #define VERIFY(p)       do {if (p) ;} while (false)
#endif

void cast_spec_spell(void);
void create_spec_monster(void);
void level_travel(void);
void create_spec_object(void);
void create_spec_object2(void);
void stethoscope(int mwh);
void debug_add_skills(void);

#endif  // DEBUG_H

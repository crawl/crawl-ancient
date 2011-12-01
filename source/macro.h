/*
 *  File:       macro.cc
 *  Summary:    Crude macro-capability
 *  Written by: Juho Snellman <jsnell@lyseo.edu.ouka.fi>
 *
 *  Change History (most recent first):
 *
 *               <2>     6/25/02        JS              Removed old cruft
 *               <1>     -/--/--        JS              Created
 */

#ifdef USE_MACROS

#ifndef MACRO_H
#define MACRO_H

#ifndef MACRO_CC

#undef getch
#define getch() getchm()

#endif

enum KeymapContext {
    KC_DEFAULT,         // For no-arg getchm().
    KC_LEVELMAP,        // When in the 'X' level map
    KC_TARGETING,       // Only during 'x' and other targeting modes

    KC_CONTEXT_COUNT    // Must always be the last
};

int getchm(void);       // keymaps applied (ie for prompts)
int getchm(KeymapContext context);

int getch_with_command_macros(void);  // keymaps and macros (ie for commands)

void flush_input_buffer( int reason );

void macro_add_query(void);
int macro_init(void);
void macro_save(void);

#endif

#else

#define getch_with_command_macros()     getch()
#define getchm(x)                       getch()
#define flush_input_buffer(XXX)         ;

#endif

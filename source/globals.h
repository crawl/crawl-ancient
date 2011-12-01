/*
 *  File:       globals.h
 *  Summary:    Moving all the global declarations into a single file.
 *  Written by: Brent Ross
 *
 *  History:
 *
 *  <1>         Created
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include "defines.h"
#include "externs.h"
#include "FixAry.h"

extern struct player_def                you;
extern struct crawl_environment         env;
extern struct game_options              Options;
extern struct system_environment        SysEnv;
extern char                             info[INFO_SIZE];
extern bool                             Game_Has_Started;

#if USE_GSL_RAND
extern gsl_rng                         *GSL_RNG_State;
#endif

#endif // GLOBALS_H

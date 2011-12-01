/*
 *  File:       globals.cc
 *  Summary:    Actual allocations for all global variables.
 *  Written by: Brent Ross
 *
 *  History:
 *
 *  <1>         Created
 */

#include "AppHdr.h"

#include "defines.h"
#include "globals.h"
#include "externs.h"
#include "FixAry.h"

struct player_def               you;             // player character stuff
struct crawl_environment        env;             // dungeon stuff
struct game_options             Options;         // game options
struct system_environment       SysEnv;          // environment variables

char                            info[INFO_SIZE]; // general use text buffer

#if USE_GSL_RAND
gsl_rng                        *GSL_RNG_State;
#endif

// set to true once a new game starts or an old game loads
bool                            Game_Has_Started = false;

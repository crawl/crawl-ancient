#ifndef ABYSS_H
#define ABYSS_H
/*
 *  File:       abyss.h
 *  Summary:    Misc functions (most of which don't appear to be related to priests).
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

char *god_name(char which_god);

void priest_spells(int func_pass[10], char religious);

char area_shift(void);

void abyss_teleport(void);
#endif

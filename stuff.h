/*
 *  File:       stuff.cc
 *  Summary:    Misc stuff.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *   <3>    11/14/99     cdl    added random40
 *   <2>    11/06/99     cdl    added random22
 *   <1>     -/--/--     LRH    Created
 */

#ifndef STUFF_H
#define STUFF_H

int random2(int randmax);
int random2avg(int max, int rolls);
int random2limit(int max, int limit);
unsigned char get_ch(void);
bool see_grid(unsigned char grx, unsigned char gry);
int magic_ability(int mag_abil, int intel);
void end(int end_arg);
void output_value(char string[100], int value);
void redraw_screen(void);
int stepdown_value(int base_value, int stepping, int first_step, int last_step, int ceiling_value);
bool one_chance_in(int a_million);
void cf_setseed(void);
bool coinflip(void);
int table_lookup(int die_roll,...);
void modify_stats(int STmod, int IQmod, int DXmod);

#endif

/*
 *  File:       stuff.cc
 *  Summary:    Misc stuff.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#ifndef STUFF_H
#define STUFF_H

int random2(unsigned int randmax);
unsigned char get_ch(void);
char see_grid(unsigned char grx, unsigned char gry);
int magic_ability(int mag_abil, int intel);
void end(int end_arg);
void output_value(char string[100], int value);
void redraw_screen(void);
#endif

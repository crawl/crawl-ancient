/*
 *  File:       ouch.cc
 *  Summary:    Functions used when Bad Things happen to the player.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef OUCH_H
#define OUCH_H

int check_your_resists(int hurted, int flavour);

void splash_with_acid(char acid_strength);
void weapon_acid(char acid_strength);
void item_corrode(char itco);
void scrolls_burn(char burn_strength, char target_class);
void ouch(int dam, char death_source, char death_type);
void lose_level(void);
void drain_exp(void);
#endif

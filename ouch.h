/*
 *  File:       ouch.cc
 *  Summary:    Functions used when Bad Things happen to the player.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *     <2>     9/29/99     BCR     Added the DEATH_NAME_LENGTH define
 *                                 It determines how many characters of
 *                                 a long player name are printed in
 *                                 the score list.
 *     <1>     -/--/--     LRH     Created
 */


#ifndef OUCH_H
#define OUCH_H

#define DEATH_NAME_LENGTH 10

int check_your_resists(int hurted, int flavour);

void splash_with_acid(char acid_strength);
void weapon_acid(char acid_strength);
void item_corrode(char itco);
void scrolls_burn(char burn_strength, char target_class);
void ouch(int dam, char death_source, char death_type);
void lose_level(void);
void drain_exp(void);
#endif

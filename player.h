#ifndef PLAYER_H
#define PLAYER_H
/*
 *  File:       player.cc
 *  Summary:    Player related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */
#include "externs.h"

int player_teleport(void);
int player_fast_run(void);
int player_speed(void);
int player_AC(void);
int player_evasion(void);
int player_shield_class(void);
int player_see_invis(void);
int player_mag_abil(void);
int carrying_capacity(void);
int burden_change(void);
int player_regen(void);
int player_sust_abil(void);
int player_res_fire(void);
int player_res_cold(void);
int player_res_poison(void);
int player_prot_life(void);
char you_resist_magic(int power);
void forget_map(char chance_forgotten);
void how_hungered(int hunge);
void gain_exp(unsigned int exp_gained);
void display_char_status(void);
void redraw_skill(char your_name[kNameLen], char class_name[40]);
char *species_name(char speci);
char wearing_amulet(char which_am);
unsigned long exp_needed(int lev, char species);
int check_stealth(void);
void level_change(void);
int player_spec_summ(void);
int player_spec_ench(void);
int player_spec_conj(void);
int player_spec_air(void);
int player_spec_earth(void);
int player_spec_cold(void);
int player_spec_fire(void);
int player_spec_holy(void);
int player_spec_death(void);
int player_spec_poison(void);
int player_energy(void);
int slaying_bonus(char which_affected);         /* returns bonuses from rings of slaying etc */
int scan_randarts(char which_property);
#endif

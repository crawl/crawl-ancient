/*
 *  File:       skills.cc
 *  Summary:    Skill exercising functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */
#ifndef SKILLS_H
#define SKILLS_H

void exercise(char exsk, int deg);

void show_skills(void);

char *skill_title(char best_skill, char skill_lev, char char_class, char char_lev);

char best_skill(char min_skill, char max_skill, char excl_skill);

int species_skills(char skill, char species);

int calc_hp();
int calc_ep();

unsigned int skill_exp_needed(int lev);

char *skill_name(char which_skill);

int add_skill(int min_skill, int max_skill, int sk_tot);
void class_sk(int clsk[8][3], int chcls);

#endif

/*
 *  File:       mutation.cc
 *  Summary:    Functions for handling player mutations.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */
#ifndef MUTATION_H
#define MUTATION_H

char mutate(int which_mutation);
void display_mutations(void);
char delete_mutation(char which_mutation);
int how_mutated(void);
char body_covered(void);
char *mutation_name(char which_mutat);
char give_good_mutation(void);
char give_bad_mutation(void);
void demonspawn(void);
#endif

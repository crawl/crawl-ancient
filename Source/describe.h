
/*
 *  File:       describe.h
 *  Summary:    Functions used to print information about various game objects.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <2>      5/21/99        BWR             Changed from is_artifact to is_dumpable_artifact
 *      <1>      4/20/99        JDJ             Added get_item_description and is_artifact.
 */


#ifndef DESCRIBE_H
#define DESCRIBE_H


#include <string>


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: chardump - spells4
 * *********************************************************************** */
bool is_dumpable_artifact(int item_class, int item_type, int item_plus,
                          int item_plus2, int item_dam,
                          unsigned char item_id, char verbose);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: chardump - describe
 * *********************************************************************** */
string get_item_description(int item_class, int item_type, int item_plus,
                            int item_plus2, int item_dam,
                            unsigned char item_id, char verbose);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: acr - religion
 * *********************************************************************** */
void describe_god(int which_god);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: item_use - shopping
 * *********************************************************************** */
void describe_item(int item_class, int item_type, int item_plus,
                   int item_plus2, int item_dam, unsigned char item_id);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: direct
 * *********************************************************************** */
void describe_monsters(int class_described, unsigned char which_mons);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: item_use
 * *********************************************************************** */
void describe_spell(int spelled);


#endif

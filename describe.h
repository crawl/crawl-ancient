#ifndef DESCRIBE_H
#define DESCRIBE_H

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

#include <string>

string get_item_description(int item_class, int item_type, int item_plus, int item_plus2, int item_dam, unsigned char item_id);
bool is_dumpable_artifact(int item_class, int item_type, int item_plus, int item_plus2, int item_dam, unsigned char item_id);

void describe_monsters(int class_described, unsigned char which_mons);
void describe_item(int item_class, int item_type, int item_plus, int item_plus2, int item_dam, unsigned char item_id);
void describe_spell(int spelled);
void describe_god(int which_god);

#endif // DESCRIBE_H

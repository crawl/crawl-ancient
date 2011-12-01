
/*
 *  File:       describe.h
 *  Summary:    Functions used to print information about various game objects.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <2>      5/21/99        BWR             Changed from is_artefact to is_dumpable_artefact
 *      <1>      4/20/99        JDJ             Added get_item_description and is_artefact.
 */

#ifndef DESCRIBE_H
#define DESCRIBE_H

#include <string>
#include "externs.h"

const char *const size_description( size_type size );

bool is_dumpable_artefact( const item_def &item, char verbose );

std::string get_item_description( const item_def &item, char verbose,
                                  bool dump = false );

void describe_god( int which_god, bool give_title );

void describe_item( const item_def &item );

void describe_monsters(int class_described, unsigned char which_mons);

char describe_spell(int spelled);

// last updated 13oct2003 {darshan}
/* ***********************************************************************
 * called from: describe_monsters - describe, kill_ghost - Kills
 * *********************************************************************** */
std::string ghost_description(bool concise = false);

#endif

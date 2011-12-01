/*
 *  File:       files.cc
 *  Summary:    Functions used to save and load levels/games.
 *  Written by: Linley Henzell and Alexey Guzeev
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef FILES_H
#define FILES_H

#include "FixAry.h"
#include <stdio.h>
#include <string>

void load( unsigned char stair_taken, int load_mode, bool was_a_labyrinth,
           char old_level, char where_were_you2 );

void save_game(bool leave_game);
void restore_game(void);

void save_ghost( bool force = false );

void generate_random_demon( void );

void make_filename( char *buf, const char *prefix, int level, int where,
                    bool isLabyrinth, bool isGhost );


void writeShort(FILE *file, short s);

short readShort(FILE *file);

void writeByte(FILE *file, unsigned char byte);

unsigned char readByte(FILE *file);

void writeString(FILE* file, const std::string &s);

std::string readString(FILE *file);

void writeLong(FILE* file, long num);

long readLong(FILE *file);

#endif

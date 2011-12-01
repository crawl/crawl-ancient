/*
 *  File:       tags.h
 *  Summary:    Auxilary functions to make savefile versioning simpler.
 *  Written by: Gordon Lipford
 *
 *  Change History (most recent first):
 *
 *   <1>   27 Jan 2001      GDL    Created
 */

#ifndef TAGS_H
#define TAGS_H

#include <stdio.h>
#include "externs.h"

int write2( FILE * file, const char *buffer, unsigned int count );
int read2( FILE * file, char *buffer, unsigned int count );

void marshallByte(struct tagHeader &th, char data);
void marshallShort(struct tagHeader &th, short data);
void marshallLong(struct tagHeader &th, long data);
void marshallFloat(struct tagHeader &th, float data);
void marshallBoolean(struct tagHeader &th, bool data);
void marshallString(struct tagHeader &th, char *data, int maxSize = 0);

char  unmarshallByte(struct tagHeader &th);
short unmarshallShort(struct tagHeader &th);
long  unmarshallLong(struct tagHeader &th);
float unmarshallFloat(struct tagHeader &th);
bool  unmarshallBoolean(struct tagHeader &th);
void  unmarshallString(struct tagHeader &th, char *data, int maxSize);

void make_date_string( time_t in_date, char buff[20] );
time_t parse_date_string( char[20] );

void tag_init(long largest_tag = 50000);
void tag_construct(struct tagHeader &th, int i);
void tag_write(struct tagHeader &th, FILE *saveFile);
void tag_set_expected(char tags[], int fileType);
void tag_missing(int tag, char minorVersion);
int tag_read(FILE *fp, char minorVersion);

#endif // TAGS_H

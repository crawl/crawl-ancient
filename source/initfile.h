/*
 *  File:       initfile.h
 *  Summary:    Simple reading of init file.
 *  Written by: David Loewenstern
 *
 *  Change History (most recent first):
 *
 *               <1>     6/9/99        DML             Created
 */


#ifndef INITFILE_H
#define INITFILE_H

#include <string>

std::string & trim_string( std::string &str );
std::string & tolower_string( std::string &str );

short str_to_colour( const std::string &str );

void read_init_file(void);

void get_system_environment(void);

bool parse_args(int argc, char **argv, bool rc_only);

#endif

#ifndef INITFILE_H
#define INITFILE_H
/*
 *  File:       initfile.h
 *  Summary:    Simple reading of init file.
 *  Written by: David Loewenstern
 *
 *  Change History (most recent first):
 *
 *               <1>     6/9/99        DML             Created
 */

extern char* obj_syms;
extern int obj_syms_len;

void read_init_file(void);
void get_system_environment(void);


#endif

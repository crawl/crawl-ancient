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


extern char *obj_syms;
extern int obj_syms_len;


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: acr
 * *********************************************************************** */
void read_init_file(void);


// last updated 12may2000 {dlb}
/* ***********************************************************************
 * called from: acr
 * *********************************************************************** */
void get_system_environment(void);


#endif

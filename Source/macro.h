/*
 *  File:       macro.cc
 *  Summary:    Crude macro-capability
 *  Written by: Juho Snellman <jsnell@lyseo.edu.ouka.fi>
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        JS              Created
 */


#ifndef MACRO_H
#define MACRO_H

#ifndef MACRO_C
#undef getch
#define getch() getchm()
#endif


#include "llist.h"


typedef struct
{
    node_s *node;
    int *key;
    int *action;
    int final;
}
macro_s;


int *i_strcpy(int *, int *);
int getchm(void);
int i_strcmp(int *, int *);
int i_strlen(int *);
int kbhit2();
int macro_add(int *, int *);
int macro_add_query(void);
int macro_init(void);
int macro_save(void);


#endif

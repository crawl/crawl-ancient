#ifndef MACRO_C
# undef getch
# define getch() getchm()
#endif

#ifndef MACRO_H
# define MACRO_H

# include "llist.h"

typedef struct
{
        node_s* node;
        int* key;
        int* action;
        int final;
}
macro_s;

int macro_init(void);
int macro_save(void);
int macro_add(int*, int*);
int macro_add_query(void);
int getchm(void);
int i_strlen(int*);
int i_strcmp(int*, int*);
int* i_strcpy(int*, int*);

int kbhit2();

#endif

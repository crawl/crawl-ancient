/*
 *  File:       macro.cc
 *  Summary:    Crude macro-capability
 *  Written by: Juho Snellman <jsnell@lyseo.edu.ouka.fi>
 *
 *  Change History (most recent first):
 *
 *      <2>     6/13/99         BWR             sys_env.crawl_dir support
 *      <1>     -/--/--         JS              Created
 */

#define _LIBLINUX_IMPLEMENTATION
#include "AppHdr.h"
#undef _LIBLINUX_IMPLEMENTATION

#define MACRO_C
#include "macro.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>

#ifdef LINUX
  #include <curses.h>
#elif MAC
#else
  #include <conio.h>
#endif

#include "externs.h"
#include "libmac.h"
#include "llist.h"
#include "message.h"

int *i_strrev(int *str, int len);

/* A linked list is not really appropriate for this purpose, should
 * use a hash-table (FIXME)*/
node_s *macro_list;
node_s *macro_buf;


int macro_init(void)
{
    FILE *f;
    char *s;
    unsigned int ssize;


    macro_list = new_list();
    macro_buf = new_list();
    macro_buf->data = macro_buf->next;

    s = (char *) malloc(255);
    ssize = 255;

    char  file_name [kPathLen] = "\0";
    if (sys_env.crawl_dir)
        strncpy( file_name, sys_env.crawl_dir, kPathLen );

    strncat( file_name, "macro.txt", kPathLen );

    f = fopen( file_name, "r" );
    if (f != NULL)
    {
        char *key;
        int *rkey;
        char *act;
        int *ract;
        unsigned int a, len, rlen, tlen;

        /* Fixes a crash with empty macro-file. */
        a = len = rlen = tlen = 0;

        key = (char *) malloc(ssize);
        act = (char *) malloc(ssize);
        rkey = (int *) malloc(ssize * sizeof(int));
        ract = (int *) malloc(ssize * sizeof(int));

        key[0] = 0;
        act[0] = 0;

        do
        {
            a = 0;
            do
            {
                if (a == ssize - 1)
                {
                    s = (char *) realloc(s, ssize + 255);
                    key = (char *) realloc(key, ssize + 255);
                    act = (char *) realloc(act, ssize + 255);
                    rkey = (int *) realloc(rkey, (ssize + 255) * sizeof(int));
                    ract = (int *) realloc(rkey, (ssize + 255) * sizeof(int));
                }
                fgets(s + a, 255, f);
                a += strlen(s + a);
            }
            while (a == ssize - 1);

            if (sscanf(s, "K:%[^\n]%n", key, &len))
            {
                key[len - 2] = 0;
//                              printf("[%d][%s]\n", len, key);
            }
            else if (sscanf(s, "A:%[^\n]%n", act, &len))
                act[len - 2] = 0;
            if (key[0] != 0 && act[0] != 0)
            {
/*                              rkey[0] = -1; */
                len = 0;
                rlen = 0;
                tlen = 1;
                while (len < strlen(key) && tlen > 0)
                {
                    if (sscanf(&key[len], "\\%i%n", &rkey[rlen], &tlen) == 0)
                    {
                        rkey[rlen] = key[len];
                        tlen = 1;
                    }
                    rlen++;
                    len += tlen;
                }
                rkey[rlen] = -1;

/*                              ract[0] = 0; */
                len = 0;
                rlen = 0;
                tlen = 1;
                while (len < strlen(act) && tlen > 0)
                {
                    if (sscanf(&act[len], "\\%i%n", &ract[rlen], &tlen) == 0)
                    {
                        ract[rlen] = act[len];
                        tlen = 1;
                    }
                    rlen++;
                    len += tlen;
                }
                ract[rlen] = -1;

                macro_add(rkey, ract);
                key[0] = 0;
                act[0] = 0;
            }
        }
        while (!feof(f));
    }
    return 0;
}

int macro_add(int *key, int *action)
{
    node_s *node, *node2;
    macro_s *nmacro;
    int *str1, *str2;

    node = new_node();
    nmacro = (macro_s *) malloc(sizeof(macro_s));
    if (node == NULL || nmacro == NULL)
        return -1;
    node->data = nmacro;
    nmacro->node = node;
    nmacro->final = 0;

    str1 = (int *) malloc((i_strlen(key) + 1) * sizeof(int));
    str2 = (int *) malloc((i_strlen(action) + 1) * sizeof(int));

    if (str1 == NULL || str2 == NULL)
        return -1;
    i_strcpy(str1, key);
    i_strcpy(str2, action);

    nmacro->key = str1;
    nmacro->action = str2;

    node2 = macro_list->next;
    while (node2 != node2->next &&
           i_strcmp(((macro_s *) node2->data)->key, nmacro->key) < 1)
    {
        node2 = node2->next;
    }

    insert_node(node, node2->prev);

    return 0;
}

static int macro_buf_add(int *actions)
{
    int i, l = i_strlen(actions);
    node_s *node;

    for (i = 0; i < l; i++)
    {
        node = new_node();
        node->data = (void *) actions[i];       /* We don't need no stinking
                                                   structs for one value... */
        insert_node(node, ((node_s *) macro_buf->data)->prev);
    }
    return 0;
}

static int macro_buf_get(void)
{
    int a;

    if (macro_buf->next == macro_buf->next->next)
        return -1;

    a = int (macro_buf->next->data);

    delete_node(macro_buf->next);

    return a;
}

static int macro_retrieve(int *key)
{
    node_s *node = macro_list->next;

    while (node != node->next &&
           i_strcmp(((macro_s *) node->data)->key, key) != 0)
        node = node->next;
    if (node == node->next)
        return 0;

    macro_buf_add(((macro_s *) node->data)->action);

    return 1;
}

int macro_save(void)
{
    FILE *f = fopen("macro.txt", "w");
    node_s *node = macro_list->next;
    int i, c;

    if (f == NULL)
        return -1;

    while (node != node->next)
    {
        fprintf(f, "K:");
        for (i = 0; i < i_strlen(((macro_s *) node->data)->key); i++)
        {
            c = ((macro_s *) node->data)->key[i];
            /* It might be a good idea to allow some escaping of '\'
             * too :-) */
            if (c >= 32 && c < 128 && c != '\\')
                fprintf(f, "%c", c);
            else
            {
                if (c < 0)
                    fprintf(f, "\\-0%o", -c);
                else
                    fprintf(f, "\\0%o", c);
            }
        }
        fprintf(f, "\n");
        fprintf(f, "A:");
        for (i = 0; i < i_strlen(((macro_s *) node->data)->action); i++)
        {
            c = ((macro_s *) node->data)->action[i];
            if (c >= 32 && c < 128)
                fprintf(f, "%c", c);
            else
            {
                if (c < 0)
                    fprintf(f, "\\-%o", -i);
                else
                    fprintf(f, "\\%o", i);
            }
        }
        fprintf(f, "\n\n");
        node = node->next;
    }

    return 0;
}

static int getch_mul(int size, int *buf)
{
    int a;

    int i = 0;

    buf[i++] = a = getch();

    buf[i] = -1;

    while ((kbhit() || a == 0) && i < size)
    {
        a = getch();

        buf[i++] = a;
        buf[i] = -1;
    }

    return i;
}


int getchm(void)
{
    int buf[255];
    int a;

    if ((a = macro_buf_get()) != -1)
        return a;

    getch_mul(sizeof(buf), buf);

    if (!macro_retrieve(buf))
        macro_buf_add(buf);

    return macro_buf_get();
}


int macro_add_query(void)
{
    int key[255], act[255];

    mpr("Key: ");
    getch_mul(sizeof(key), key);

    if (macro_retrieve(key) != 0)
    {
        while (macro_buf->next != macro_buf->next->next)
            macro_buf_get();
        mpr("That key already has an action assigned to it.");
        return -1;
    }

    mpr("Action: ");
    getch_mul(sizeof(act), act);

    macro_add(key, act);
    return 0;
}

/* I think these should work... */

int i_strlen(int *str)
{
    int len = 0;

    while (*(str++) != -1)
        len++;
    return len;
}

int *i_strcpy(int *str1, int *str2)
{
    int i = 0;

    while ((str1[i] = str2[i]) != -1)
        i++;
    return str1;
}

int i_strcmp(int *str1, int *str2)
{
    int i = 0;

    while (str1[i] == str2[i] && str1[i] != -1 && str2[i] != -1)
    {
        i++;
    }
    return str1[i] - str2[i];
}

/*
 *  File:       llist.cc
 *  Summary:    Linked list implementation
 *  Written by: Juho Snellman <jsnell@lyseo.edu.ouka.fi>
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        JS             Created
 */

#include "AppHdr.h"
#include "llist.h"

#include <stdlib.h>

node_s *new_node(void)
{
    node_s *nnode;

    nnode = (node_s *) malloc(sizeof(node_s));

    nnode->next = NULL;
    nnode->prev = NULL;
    nnode->data = NULL;

    return nnode;
}

node_s *new_list(void)
{
    node_s *begin;
    node_s *end;

    begin = new_node();
    end = new_node();

    begin->prev = begin;
    begin->next = end;
    end->prev = begin;
    end->next = end;

    return begin;
}

int insert_node(node_s * ins, node_s * bef)
{
    node_s *nxt = bef->next;

    // Can't insert after the last node
    if (bef->next == bef)
        return -1;

    // Can't insert a node that is already in a list
    if ((ins->next != NULL) || (ins->prev != NULL))
        return -2;

    ins->next = bef->next;
    ins->prev = bef;
    nxt->prev = ins;
    bef->next = ins;

    return 0;
}

int delete_node(node_s * del)
{
    node_s *bef = del->prev;
    node_s *nxt = del->next;

    if (del->next == del)       // Can't delete last node
        return -1;

    if (del->prev == del)       // Can't delete first node
        return -2;

    bef->next = nxt;
    nxt->prev = bef;

    free(del);

    return 0;
}

int delete_list(node_s * begin)
{
    node_s *nxt = NULL;

    if (begin->prev != begin)   // node must be the first node in a list
        return -1;

    while (begin->next != begin)
    {
        nxt = begin->next;
        free(begin);
        begin = nxt;
    }

    free(nxt);
    return 0;
}

int move_node(node_s * move, node_s * bef2)
{
    node_s *nxt1 = move->next;
    node_s *bef1 = move->prev;
    node_s *nxt2 = bef2->next;

    if (bef2->next == bef2)     // Can't move after the last node
        return -1;

    if (move->next == move)     // Can't move last node
        return -2;

    if (move->prev == move)     // Can't move first node
        return -3;

    bef1->next = nxt1;
    nxt1->prev = bef1;

    nxt2->prev = move;
    bef2->next = move;
    move->next = nxt2;
    move->prev = bef2;

    return 0;
}

/*
int sort_list (node_s* begin)
{
   node_s* cur, *comp, *small;

   if ( begin == NULL )
     return -1;

   cur = begin->next;

   while ( cur != cur->next )
   {
       comp = cur->next;
       small = cur;
       while ( comp != comp->next )
       {
           if ( comp->snum < small->snum )
             small = comp;

           comp = comp->next;
       }

       if ( small != cur )
         {
           //printf("s: %d -> %d\n", cur->snum, small->snum);
           move_node(small, cur->prev);
         }

       cur = small->next;
   }

   return 0;
}
*/

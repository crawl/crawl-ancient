#ifndef LLIST_H
# define LLIST_H
typedef struct node_s
{
        struct node_s* next;
        struct node_s* prev;
        void* data;
}
node_s;

node_s* new_node(void);
node_s* new_list (void);
int insert_node (node_s*, node_s*);
int delete_node (node_s*);
int delete_list (node_s*);
int move_node (node_s*, node_s*);
int sort_list (node_s*);

#endif /* LLIST_H */

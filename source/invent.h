/*
 *  File:       invent.cc
 *  Summary:    Functions for inventory related commands.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef INVENT_H
#define INVENT_H

#include <stddef.h>
#include <vector>
#include "menu.h"

#define PROMPT_ABORT        -1
#define PROMPT_GOT_SPECIAL  -2

struct SelItem {
    int slot;
    int quantity;
    const item_def *item;

    SelItem() : slot(0), quantity(0), item(NULL) { }

    SelItem( int s, int q, const item_def *it = NULL )
        : slot(s), quantity(q), item(it)
    {
    }
};


int prompt_invent_item( const char *prompt, int type_expect,
                        bool must_exist = true,
                        bool allow_auto_list = true,
                        bool allow_easy_quit = true,
                        const char other_valid_char = '\0',
                        int *const count = NULL );

unsigned char show_invent( int item_class_inv, bool show_price = false );

std::vector<SelItem> select_items( std::vector<item_def*> &items,
                                   const char *title );

std::vector<SelItem> prompt_invent_items(
                        const char *prompt,
                        int type_expect,
                        std::string (*titlefn)( int menuflags,
                                                const std::string &oldt )
                            = NULL,
                        bool allow_auto_list = true,
                        bool allow_easy_quit = true,
                        const char other_valid_char = '\0',
                        std::vector<std::string> *filter = NULL );

unsigned char invent_select( int item_class_inv,
                   int select_flags = MF_NOSELECT,
                   std::string (*titlefn)( int menuflags,
                                           const std::string &oldt )
                        = NULL,
                   std::vector<SelItem> *sels = NULL,
                   std::vector<std::string> *filter = NULL );

bool in_inventory(const item_def &i);

void list_commands(bool wizard);

#endif

#ifndef __MENU_H__
#define __MENU_H__

#include <string>
#include <vector>
#include <algorithm>
#include "AppHdr.h"
#include "defines.h"
#include "libutil.h"

enum MenuEntryLevel {
    MEL_NONE = -1,
    MEL_TITLE,
    MEL_SUBTITLE,
    MEL_ITEM
};

struct item_def;

struct MenuEntry
{
    std::string      text;
    int              quantity, selected_qty;
    int              colour;
    std::vector<int> hotkeys;
    MenuEntryLevel   level;

    MenuEntry( const std::string &txt = std::string(""),
               MenuEntryLevel lev = MEL_ITEM,
               int qty = 0,
               int hotk = 0 ) :
        text(txt), quantity(qty), selected_qty(0), hotkeys(),
        level(lev)
    {
        colour = lev == MEL_ITEM?       LIGHTGREY :
                                        WHITE;
        if (hotk)
            hotkeys.push_back( hotk );
    }

    virtual ~MenuEntry() { }

    void add_hotkey( int key )
    {
        if (key && !is_hotkey(key))
            hotkeys.push_back( key );
    }

    bool is_hotkey( int key ) const
    {
        return (find( hotkeys.begin(), hotkeys.end(), key ) != hotkeys.end());
    }

    bool is_primary_hotkey( int key ) const
    {
        return (hotkeys.size() ? (hotkeys[0] == key) : false);
    }

    virtual std::string get_text() const
    {
        return (std::string( (level == MEL_SUBTITLE) ? " " :
                             (level == MEL_ITEM)     ? ""
                                                     : "  ") + text);
    }

    virtual bool selected() const
    {
        return (selected_qty > 0 && quantity);
    }

    void select( int qty = -1 )
    {
        if (selected())
            selected_qty = 0;
        else if (quantity)
            selected_qty = ((qty == -1) ? quantity : qty);
    }
};

// XXX: we now have two types prefixed with MF
enum MenuFlag
{
    MF_NOSELECT         = 0,           // No selection is permitted
    MF_SINGLESELECT     = (1 << 0),    // Select just one item
    MF_MULTISELECT      = (1 << 1),    // Select multiple items
    MF_NO_SELECT_QTY    = (1 << 2),    // Disallow partial selections
    MF_ANYPRINTABLE     = (1 << 3),    // Any printable character is valid, and
                                       // closes the menu.
    MF_SELECT_ANY_PAGE  = (1 << 4),    // Allow selections to occur on any page.

    MF_EASY_EXIT        = (1 << 6)
};

#define NUMBUFSIZ 10
class Menu
{
public:
    Menu( int flags = MF_MULTISELECT );
    virtual ~Menu();

    void set_flags( int in_flags )   { this->flags = in_flags; }
    int  get_flags() const           { return (flags); }
    bool is_set( int flag ) const;

    void set_title( MenuEntry *e );
    void add_entry( MenuEntry *entry );
    void get_selected( std::vector<MenuEntry*> *sel ) const;

    void set_select_filter( std::vector<std::string> filter )
    {
        select_filter = filter;
    }

    unsigned char getkey() const { return lastch; }

    std::vector< MenuEntry * > show();
protected:
    MenuEntry *title;
    int flags;

    int first_entry, y_offset;
    int pagesize;

    std::vector<MenuEntry*>  items;
    std::vector<MenuEntry*>  *sel;
    std::vector<std::string> select_filter;

    int num;

    unsigned char lastch;

    bool alive;

    int last_entry() const;
    void do_menu( std::vector<MenuEntry*> *selected );
    void draw_select_count( int count );
    void draw_item( int index ) const;
    void draw_title();
    void draw_menu( std::vector<MenuEntry*> *selected );
    bool page_down();
    bool line_down();
    bool page_up();
    bool line_up();

    void deselect_all();
    void select_items( int key, int qty = -1 );
    void select_index( int index, int qty = -1 );

    bool is_hotkey(int index, int key );
    bool is_selectable(int index) const;

    virtual bool process_key( int keyin );
};

#endif

/*
 *  File:       menu.cc
 *  Summary:    Menu stuff
 *  Written by: Darshan Shaligram
 *
 *  Known issues:
 */

// Important: AppHdr needed here... otherwise DEBUG/NDEBUG get out of sync!
#include <string.h>

#include "AppHdr.h"
#include "globals.h"
#include "menu.h"
#include "view.h"

Menu::Menu( int in_flags )
{
    sel         = NULL;
    title       = NULL;
    this->flags = in_flags;
    y_offset    = 0;
    num         = -1;
    lastch      = 0;
}

Menu::~Menu()
{
    for (int i = 0, count = items.size(); i < count; ++i)
        delete items[i];

    delete title;
}

void Menu::set_title( MenuEntry *e )
{
    if (title != e)
        delete title;

    title = e;
    title->level = MEL_TITLE;
}

void Menu::add_entry( MenuEntry *entry )
{
    items.push_back( entry );
}

std::vector< MenuEntry * > Menu::show()
{
    std::vector< MenuEntry * > selected;

    first_entry = 0;

    // Lose lines for the title + room for more.
    pagesize = get_number_of_lines() - 2;

#ifdef DOS_TERM
    char buffer[4600];

    gettext(1, 1, 80, 25, buffer);
    window(1, 1, 80, 25);
#endif

    do_menu( &selected );

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif

    return (selected);
}

void Menu::do_menu( std::vector<MenuEntry*> *selected )
{
    sel = selected;
    draw_menu( selected );

    alive = true;
    while (alive)
    {
        int keyin = c_getch();

        if (!process_key( keyin ))
            return;
    }
}

bool Menu::is_set( int flag ) const
{
    if (flag == MF_EASY_EXIT && Options.easy_exit_menu)
        return (true);

    return ((flags & flag) == flag);
}

bool Menu::process_key( int keyin )
{
    if (items.size() == 0)
        return (false);

    bool nav = false, repaint = false;
    switch (keyin)
    {
    case CK_ENTER:
        return (false);

    case CK_ESCAPE:
        sel->clear();
        return (false);

    case ' ':
    case CK_PGDN:
    case '>':
    case '\'':
        nav = true;
        repaint = page_down();
        if (!repaint && flags && !is_set(MF_EASY_EXIT))
        {
            repaint = (first_entry != 0);
            first_entry = 0;
        }
        break;

    case CK_PGUP:
    case '<':
    case ';':
        nav = true;
        repaint = page_up();
        break;

    case CK_UP:
        nav = true;
        repaint = line_up();
        break;

    case CK_DOWN:
        nav = true;
        repaint = line_down();
        break;

    default:
        lastch = keyin;

        // If no selection at all is allowed, exit now.
        if (!(flags & (MF_SINGLESELECT | MF_MULTISELECT)))
            return (false);

        if (!is_set(MF_NO_SELECT_QTY) && isdigit( keyin ))
        {
            if (num > 999)
                num = -1;

            num = (num == -1) ? keyin - '0'
                              : num * 10 + keyin - '0';
        }

        select_items( keyin, num );
        get_selected( sel );

        if (sel->size() == 1 && (flags & MF_SINGLESELECT))
            return (false);

        draw_select_count( sel->size() );

        if (flags & MF_ANYPRINTABLE && !isdigit( keyin ))
            return (false);

        break;
    }

    if (!isdigit( keyin ))
        num = -1;

    if (nav)
    {
        if (repaint)
            draw_menu( sel );
        else if (!flags || is_set(MF_EASY_EXIT))
        {
            sel->clear();
            return (false);
        }
    }

    return (true);
}

void Menu::draw_select_count( int count )
{
    int oldx = wherex(), oldy = wherey();
    draw_title();

    int x = wherex();

    char buf[100];
    if (count)
    {
        snprintf( buf, sizeof buf, "  (%d item%s)  ", count,
                  (count > 1) ? "s" : "" );
    }
    else
    {
        strcpy(buf, "              ");
    }

    // Take care to avoid wraparound down to next line
    if (x + strlen(buf) - 1 > GXM)
    {
        if (x > GXM || x < 1)
            return;

        buf[ GXM - x + 1 ] = 0;
    }

    cprintf(buf);
    gotoxy( oldx, oldy );
}

void Menu::get_selected( std::vector<MenuEntry*> *selected ) const
{
    selected->clear();

    for (int i = 0, count = items.size(); i < count; ++i)
    {
        if (items[i]->selected())
            selected->push_back( items[i] );
    }
}

void Menu::deselect_all()
{
    for (int i = 0, count = items.size(); i < count; ++i)
    {
        if (items[i]->level == MEL_ITEM)
        {
            items[i]->select(0);
            draw_item(i);
        }
    }
}

bool Menu::is_hotkey( int i, int key )
{
    int  end = last_entry();
    bool ishotkey = is_set( MF_SINGLESELECT ) ? items[i]->is_primary_hotkey(key)
                                              : items[i]->is_hotkey(key);

    return (is_set( MF_SELECT_ANY_PAGE ) ? ishotkey
                                : (ishotkey && i >= first_entry && i < end));
}

void Menu::select_items( int key, int qty )
{
    int x = wherex(), y = wherey();

    if (key == ',' || key == '*')
        select_index( (key == '*') ? -2 : -1, qty );
    else if (key == '-' || key == CONTROL('C'))
        select_index( (key == CONTROL('C')) ? -2 : -1, 0 );
    else
    {
        int final = items.size();
        bool selected = false;

        // Process all items, in case user hits hotkey for an
        // item not on the current page.

        // We have to use some hackery to handle items that share
        // the same hotkey (as for pickup when there's a stack of
        // >52 items).  If there are duplicate hotkeys, the items
        // are usually separated by at least a page, so we should
        // only select the item on the current page. This is why we
        // use two loops, and check to see if we've matched an item
        // by its primary hotkey (which is assumed to always be
        // hotkeys[0]), in which case, we stop selecting further
        // items.
        for (int i = first_entry; i < final; ++i)
        {
            if (is_hotkey( i, key ))
            {
                select_index( i, qty );
                if (items[i]->hotkeys[0] == key)
                {
                    selected = true;
                    break;
                }
            }
        }

        if (!selected)
        {
            for (int i = 0; i < first_entry; ++i)
            {
                if (is_hotkey( i, key ))
                {
                    select_index( i, qty );
                    if (items[i]->hotkeys[0] == key)
                    {
                        selected = true;
                        break;
                    }
                }
            }
        }
    }

    gotoxy( x, y );
}

bool Menu::is_selectable( int item ) const
{
    if (select_filter.empty())
        return (true);

    std::string text = items[item]->get_text();
    for (int i = 0, count = select_filter.size(); i < count; ++i)
    {
        if (pattern_match( select_filter[i].c_str(), text.c_str() ))
            return (true);
    }

    return (false);
}

void Menu::select_index( int index, int qty )
{
    const int si = ((index < 0) ? first_entry : index);

    if (index < 0 && (flags & MF_MULTISELECT))
    {
        for (int i = 0, count = items.size(); i < count; ++i)
        {
            if (items[i]->level != MEL_ITEM)
                continue;

            if (is_hotkey( i, items[i]->hotkeys[0] )
                && (index == -2 || is_selectable(i)))
            {
                items[i]->select( qty );
                draw_item( i );
            }
        }
    }
    else if (items[si]->level == MEL_SUBTITLE && (flags & MF_MULTISELECT))
    {
        for (int i = si + 1, count = items.size(); i < count; ++i)
        {
            if (items[i]->level != MEL_ITEM)
                break;

            if (is_hotkey(i, items[i]->hotkeys[0]))
            {
                items[i]->select( qty );
                draw_item( i );
            }
        }
    }
    else if (items[si]->level == MEL_ITEM &&
            (flags & (MF_SINGLESELECT | MF_MULTISELECT)))
    {
        items[si]->select( qty );
        draw_item( si );
    }
}

int Menu::last_entry() const
{
    int end = first_entry + pagesize;

    // If we can squeeze in the last item by using the line taken up by
    // -more-, do so.
    const int num_items = static_cast<int>( items.size() );

    if (end == num_items - 1)
        end = items.size();
    else if (end > num_items)
        end = items.size();

    return (end);
}

void Menu::draw_menu( std::vector< MenuEntry* > *selected )
{
    clrscr();

    draw_title();
    draw_select_count( selected->size() );
    y_offset = 2;

    int end = last_entry();

    for (int i = first_entry; i < end; ++i)
    {
        draw_item( i );
    }

    if (end < static_cast<int>( items.size() ))
    {
        gotoxy( 1, y_offset + pagesize );
        textcolor( LIGHTGREY );
        cprintf("-more-");
    }
}

void Menu::draw_title()
{
    if (title)
    {
        gotoxy(1, 1);
        textcolor( title->colour );
        cprintf( "%s", title->get_text().c_str() );
    }
}

void Menu::draw_item( int index ) const
{
    if (index < first_entry || index >= last_entry())
        return;

    gotoxy( 1, y_offset + index - first_entry );
    textcolor( items[index]->colour );
    cprintf( "%s", items[index]->get_text().c_str() );
}

bool Menu::page_down()
{
    int old_first = first_entry;

    if (static_cast<int>( items.size() ) > last_entry())
    {
        first_entry += pagesize;
        if (old_first != first_entry)
            return (true);
    }

    return (false);
}

bool Menu::page_up()
{
    int old_first = first_entry;

    if (first_entry > 0)
    {
        if ((first_entry -= pagesize) < 0)
            first_entry = 0;

        if (old_first != first_entry)
            return (true);
    }

    return (false);
}

bool Menu::line_down()
{
    if (last_entry() < static_cast<int>( items.size() ))
    {
        ++first_entry;
        return (true);
    }

    return (false);
}

bool Menu::line_up()
{
    if (first_entry > 0)
    {
        --first_entry;
        return (true);
    }

    return (false);
}

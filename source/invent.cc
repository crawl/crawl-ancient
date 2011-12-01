/*
 *  File:       invent.cc
 *  Summary:    Functions for inventory related commands.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <5>     10/9/99     BCR     Added wizard help screen
 *      <4>     10/1/99     BCR     Clarified help screen
 *      <3>     6/9/99      DML     Autopickup
 *      <2>     5/20/99     BWR     Extended screen lines support
 *      <1>     -/--/--     LRH     Created
 */

#include "AppHdr.h"
#include "invent.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef DOS
#include <conio.h>
#endif

#include "globals.h"
#include "externs.h"

#include "itemname.h"
#include "items.h"
#include "itemprop.h"
#include "macro.h"
#include "player.h"
#include "shopping.h"
#include "stuff.h"
#include "view.h"
#include "menu.h"

const char *command_string( int i );
const char *wizard_string( int i );

// Macros to help clean up some of the mess caused by the special
// handling of display screens for DOS.
#ifdef DOS_TERM

#define SCREEN_INIT()           char dos_screen_buffer[4600]; \
                                gettext(1, 1, 80, 25, dos_screen_buffer); \
                                window(1, 1, 80, 25); \
                                clrscr();

#define RETURN_CLEANUP( xxx )   puttext(1, 1, 80, 25, dos_screen_buffer); \
                                return (xxx);

#define RETURN_CLEANUP_VOID()   puttext(1, 1, 80, 25, dos_screen_buffer); \
                                return;
#else

#define SCREEN_INIT()           clrscr();
#define RETURN_CLEANUP( xxx )   return (xxx);
#define RETURN_CLEANUP_VOID()   return;

#endif


struct InvTitle : public MenuEntry
{
    Menu *m;
    std::string (*titlefn)( int menuflags, const std::string &oldt );

    InvTitle( Menu *mn, const char *title,
              std::string (*tfn)( int menuflags, const std::string &oldt ) )
        : MenuEntry( title )
    {
        m       = mn;
        titlefn = tfn;
    }

    std::string get_text() const
    {
        return ((titlefn) ? titlefn( m->get_flags(), MenuEntry::get_text() )
                          : MenuEntry::get_text());
    }
};

// Must be less than ITEMNAME_SIZE
#define INVENT_WEIGHT_COLUMN 67
struct InvEntry : public MenuEntry
{
    const item_def *item;

    InvEntry( const item_def &i ) : MenuEntry( "", MEL_ITEM ), item( &i )
    {
        char buf[ITEMNAME_SIZE];
        if (i.base_type == OBJ_GOLD)
        {
            snprintf( buf, sizeof( buf ), "%d gold piece%s", i.quantity,
                      ((i.quantity > 1) ? "s" : "") );
        }
        else
        {
            item_name( i, (in_inventory(i)) ? DESC_INVENTORY_EQUIP
                                            : DESC_NOCAP_A, buf, false );
        }

        text = buf;

        if (i.base_type != OBJ_GOLD)
        {
            if (in_inventory(i))
                text = text.substr( 4 );        // Skip the inventory letter.

            add_hotkey( index_to_letter( i.slot ) );
        }
        else
        {
            add_hotkey('$');
        }

        // add_hotkey( get_class_hotkey(i) );
        add_class_hotkeys(i);

        quantity = i.quantity;

        if (Options.invent_colours)
            colour = i.colour;
    }

    std::string get_text() const
    {
        char buf[ITEMNAME_SIZE];
        snprintf( buf, sizeof( buf ), "%c %c %s",
                  hotkeys[0],
                  (!selected_qty) ? '-' : (selected_qty < quantity) ? '#' : '+',
                  text.c_str() );

        if (Options.invent_weights)
        {
            const int mass = item_mass( *item ) * item->quantity;
            char buf2[ITEMNAME_SIZE];

            // Truncate name if necessary (with a one space margin)
            buf[INVENT_WEIGHT_COLUMN - 2] = 0;
            if (mass)
            {
                snprintf( buf2, sizeof( buf2 ), "%-*s%6.1f aum",
                          INVENT_WEIGHT_COLUMN, buf,
                          static_cast<float>( mass ) / 10.0);

                return (std::string(buf2));
            }
        }

        return (std::string( buf ));
    }

private:
    void add_class_hotkeys( const item_def &i )
    {
        // XXX: this function uses characters based off IBM/DEC appearance...
        // the standard ASCII appearances are difference (see below)
        switch (i.base_type)
        {
        case OBJ_WEAPONS:
            add_hotkey(')');
            break;
        case OBJ_MISSILES:
            add_hotkey('(');
            break;
        case OBJ_ARMOUR:
            add_hotkey('[');
            break;
        case OBJ_WANDS:
            add_hotkey('/');
            break;
        case OBJ_FOOD:
            add_hotkey('%');
            break;
        case OBJ_BOOKS:
            add_hotkey('+');
            add_hotkey(':');
            break;
        case OBJ_SCROLLS:
            add_hotkey('?');
            break;
        case OBJ_JEWELLERY:
            add_hotkey( (i.sub_type >= AMU_RAGE) ? '"' : '=' );
            break;
        case OBJ_POTIONS:
            add_hotkey('!');
            break;
        case OBJ_STAVES:
            add_hotkey('\\');
            add_hotkey('|');
            break;
        case OBJ_MISCELLANY:
            add_hotkey('}');
            break;
        case OBJ_CORPSES:
            add_hotkey('&');
            break;
        default:
            break;
        }
    }
};

class InvMenu : public Menu
{
public:
    InvMenu( int mf_flags = MF_MULTISELECT ) : Menu( mf_flags ) { }

protected:
    bool process_key( int key );
};

bool InvMenu::process_key( int key )
{
    if (items.size() && (key == CONTROL('D') || key == '@'))
    {
        const int newflag = (is_set(MF_MULTISELECT))
                          ? MF_SINGLESELECT | MF_EASY_EXIT | MF_ANYPRINTABLE
                          : MF_MULTISELECT;

        flags &= ~(MF_SINGLESELECT | MF_MULTISELECT |
                   MF_EASY_EXIT | MF_ANYPRINTABLE);

        flags |= newflag;

        deselect_all();
        sel->clear();
        draw_select_count(0);
        return (true);
    }

    return (Menu::process_key( key ));
}

bool in_inventory( const item_def &i )
{
    return (i.x == -1 && i.y == -1);
}

static std::string item_class_name( int type )
{
    std::string s;
    switch (type)
    {
        case OBJ_GOLD:       s = "Gold";            break;
        case OBJ_WEAPONS:    s = "Hand Weapons";    break;
        case OBJ_MISSILES:   s = "Missiles";        break;
        case OBJ_ARMOUR:     s = "Armour";          break;
        case OBJ_WANDS:      s = "Magical Devices"; break;
        case OBJ_FOOD:       s = "Comestibles";     break;
        case OBJ_UNKNOWN_I:  s = "Books";           break;
        case OBJ_SCROLLS:    s = "Scrolls";         break;
        case OBJ_JEWELLERY:  s = "Jewellery";       break;
        case OBJ_POTIONS:    s = "Potions";         break;
        case OBJ_UNKNOWN_II: s = "Gems";            break;
        case OBJ_BOOKS:      s = "Books";           break;
        case OBJ_STAVES:     s = "Magical Staves";  break;
        case OBJ_ORBS:       s = "Orbs of Power";   break;
        case OBJ_MISCELLANY: s = "Miscellaneous";   break;
        case OBJ_CORPSES:    s = "Carrion";         break;
        //case OBJ_GEMSTONES: cprintf("Miscellaneous"); break;
    }

    return (s);
}

std::vector<SelItem> select_items( std::vector<item_def*> &items,
                                   const char *title )
{
    std::vector<SelItem> selected;

    if (items.empty())
        return (selected);

    FixedVector< int, NUM_OBJECT_CLASSES >  inv_class;
    for (int i = 0; i < NUM_OBJECT_CLASSES; ++i)
        inv_class[i] = 0;

    for (int i = 0, count = items.size(); i < count; ++i)
        inv_class[ items[i]->base_type ]++;

    Menu menu;
    menu.set_title( new MenuEntry(title) );

    char ckey = 'a';
    for (int i = 0; i < NUM_OBJECT_CLASSES; ++i)
    {
        if (!inv_class[i])
            continue;

        menu.add_entry( new MenuEntry( item_class_name(i), MEL_SUBTITLE ) );

        for (int j = 0, count = items.size(); j < count; ++j)
        {
            if (items[j]->base_type != i)
                continue;

            InvEntry *ie = new InvEntry( *items[j] );
            ie->hotkeys[0] = ckey;

            menu.add_entry( ie );

            ckey = (ckey == 'z') ? 'A' :
                   (ckey == 'Z') ? 'a'
                                 : ckey + 1;
        }
    }

    menu.set_flags( MF_MULTISELECT | MF_SELECT_ANY_PAGE );
    std::vector< MenuEntry * > sel = menu.show();

    for (int i = 0, count = sel.size(); i < count; ++i)
    {
        InvEntry *inv = static_cast<InvEntry *>( sel[i] );

        selected.push_back( SelItem( inv->hotkeys[0],
                                     inv->selected_qty,
                                     inv->item ) );
    }

    return (selected);
}

unsigned char invent_select( int item_class_inv,
                             int flags,
                             std::string (*titlefn)( int menuflags,
                                                     const std::string &oldt ),
                             std::vector< SelItem > *items,
                             std::vector<std::string> *filter )
{
    char temp_id[ NUM_IDTYPE ][ MAX_SUBTYPES ];

    init_id_array( temp_id, ID_KNOWN_TYPE );

    InvMenu menu;

    if (filter)
        menu.set_select_filter( *filter );

    FixedVector< int, NUM_OBJECT_CLASSES >  inv_class2;
    for (int i = 0; i < NUM_OBJECT_CLASSES; i++)
        inv_class2[i] = 0;

    int inv_count = 0;

    for (int i = 0; i < ENDOFPACK; i++)
    {
        if (you.inv[i].quantity)
        {
            inv_class2[ you.inv[i].base_type ]++;
            inv_count++;
        }
    }

    if (!inv_count)
    {
        menu.set_title( new MenuEntry( "You aren't carrying anything." ) );
        menu.show();
        return (0);
    }

    if (item_class_inv != -1)
    {
        for (int i = 0; i < NUM_OBJECT_CLASSES; i++)
        {
            if (item_class_inv == OBJ_MISSILES && i == OBJ_WEAPONS)
                i++;

            if (item_class_inv == OBJ_WEAPONS
                && (i == OBJ_STAVES || i == OBJ_MISCELLANY))
            {
                i++;
            }

            if (item_class_inv == OBJ_SCROLLS && i == OBJ_BOOKS)
                i++;

            if (i < NUM_OBJECT_CLASSES && item_class_inv != i)
                inv_class2[i] = 0;
        }
    }

    if ((item_class_inv == -1 && inv_count > 0)
        || (item_class_inv != -1 && inv_class2[item_class_inv] > 0)
        || (item_class_inv == OBJ_MISSILES && inv_class2[OBJ_WEAPONS] > 0)
        || (item_class_inv == OBJ_WEAPONS
            && (inv_class2[OBJ_STAVES] > 0 || inv_class2[OBJ_MISCELLANY] > 0))
        || (item_class_inv == OBJ_SCROLLS && inv_class2[OBJ_BOOKS] > 0))
    {
        const int cap = carrying_capacity();

        char title_buf[200];
        snprintf( title_buf, sizeof( title_buf ),
                  "Inventory: %d.%d aum (%d%% of %d.%d aum maximum)",
                  you.burden / 10, you.burden % 10,
                  (you.burden * 100) / cap, cap / 10, cap % 10 );

        menu.set_title( new InvTitle( &menu, title_buf, titlefn ) );

        for (int i = 0; i < 15; i++)
        {
            if (inv_class2[i] != 0)
            {
                std::string s = item_class_name(i);
                menu.add_entry( new MenuEntry( s, MEL_SUBTITLE ) );
                for (int j = 0; j < ENDOFPACK; j++)
                {
                    if (is_valid_item(you.inv[j]) && you.inv[j].base_type == i)
                        menu.add_entry( new InvEntry( you.inv[j] ) );
                }               // end of j loop
            }                   // end of if inv_class2
        }                       // end of i loop.
    }
    else
    {
        std::string s;

        if (item_class_inv == -1)
            s = "You aren't carrying anything.";
        else
        {
            if (item_class_inv == OBJ_WEAPONS)
                s = "You aren't carrying any weapons.";
            else if (item_class_inv == OBJ_MISSILES)
                s = "You aren't carrying any ammunition.";
            else
                s = "You aren't carrying any such object.";
        }

        menu.set_title( new MenuEntry( s ) );
    }

    menu.set_flags( flags );
    std::vector< MenuEntry * > sel = menu.show();

    if (items)
    {
        for (int i = 0, count = sel.size(); i < count; ++i)
        {
            items->push_back( SelItem( sel[i]->hotkeys[0],
                                       sel[i]->selected_qty ) );
        }
    }

    unsigned char mkey = menu.getkey();
    if (!isalnum(mkey) && mkey != '$')
        mkey = ' ';

    return (mkey);
}

unsigned char show_invent( int item_class_inv, bool show_price )
{
    // [ds] Okay, evil hackiness. Bypass old menu systems if
    // we're not showing prices. When the devil *do* we show prices?
    //
    // Prices are shown when the PC leaves the dungeon... ie when
    // the items are being scored.  -- bwr
    if (!show_price)
        return (invent_select( item_class_inv ));

    char st_pass[ ITEMNAME_SIZE ] = "";

    int   i, j;
    char  lines = 0;
    bool  anything = false;
    char  temp_id[NUM_IDTYPE][MAX_SUBTYPES];     // used for price display

    const int num_lines = get_number_of_lines();

    FixedVector< int, NUM_OBJECT_CLASSES >  num_class;
    int inv_count = 0;
    unsigned char ki = 0;

    SCREEN_INIT();

    // init id array to give full id prices:
    init_id_array( temp_id, ID_KNOWN_TYPE );

    // init number of items for each class:
    for (i = 0; i < NUM_OBJECT_CLASSES; i++)
        num_class[i] = 0;

    for (i = 0; i < ENDOFPACK; i++)
    {
        if (you.inv[i].quantity)
        {
            num_class[ you.inv[i].base_type ]++;
            inv_count++;
        }
    }

    if (!inv_count)
    {
        cprintf("You aren't carrying anything.");

        if (getch() == 0)
            getch();

        RETURN_CLEANUP( ESCAPE );
    }

    // init for handling the special class '?' lists:
    if (item_class_inv != -1)
    {
        for (i = 0; i < NUM_OBJECT_CLASSES; i++)
        {
            if (item_class_inv == OBJ_MISSILES && i == OBJ_WEAPONS)
                i++;

            if (item_class_inv == OBJ_WEAPONS
                && (i == OBJ_STAVES || i == OBJ_MISCELLANY))
            {
                i++;
            }

            if (item_class_inv == OBJ_SCROLLS && i == OBJ_BOOKS)
                i++;

            if (i < NUM_OBJECT_CLASSES && item_class_inv != i)
                num_class[i] = 0;
        }
    }

    // display list
    if ((item_class_inv == -1 && inv_count > 0)
        || (item_class_inv != -1 && num_class[item_class_inv] > 0)
        || (item_class_inv == OBJ_MISSILES && num_class[OBJ_WEAPONS] > 0)
        || (item_class_inv == OBJ_WEAPONS
            && (num_class[OBJ_STAVES] > 0 || num_class[OBJ_MISCELLANY] > 0))
        || (item_class_inv == OBJ_SCROLLS && num_class[OBJ_BOOKS] > 0))
    {
        const int cap = carrying_capacity();

        cprintf( "  Inventory: %d.%d aum (%d%% of %d.%d aum maximum)",
                 you.burden / 10, you.burden % 10,
                 (you.burden * 100) / cap, cap / 10, cap % 10 );
        lines++;

        for (i = 0; i < NUM_OBJECT_CLASSES; i++)
        {
            const int c = Options.list_order[i];

            if (num_class[c] != 0)
            {
                if (lines > num_lines - 3)
                {
                    gotoxy(1, num_lines);
                    cprintf( MORE_STRING );

                    ki = getch();

                    if (ki == ESCAPE || isalnum(ki) || ki == '?' || ki == '*')
                    {
                        RETURN_CLEANUP( ki );
                    }

                    if (ki == 0)
                        ki = getch();

                    lines = 0;
                    clrscr();
                    gotoxy(1, 1);
                    anything = false;
                }

                if (lines > 0)
                    cprintf(EOL " ");

                // NOTE: Testing to see if textbackground() really works:
                textcolor( WHITE );
                textbackground( BLUE );

                switch (c)
                {
                case OBJ_WEAPONS:    cprintf("Hand Weapons");    break;
                case OBJ_MISSILES:   cprintf("Missiles");        break;
                case OBJ_ARMOUR:     cprintf("Armour");          break;
                case OBJ_WANDS:      cprintf("Magical Devices"); break;
                case OBJ_FOOD:       cprintf("Comestibles");     break;
                case OBJ_SCROLLS:    cprintf("Scrolls");         break;
                case OBJ_JEWELLERY:  cprintf("Jewellery");       break;
                case OBJ_POTIONS:    cprintf("Potions");         break;
                case OBJ_BOOKS:      cprintf("Books");           break;
                case OBJ_STAVES:     cprintf("Magical Staves");  break;
                case OBJ_ORBS:       cprintf("Orbs of Power");   break;
                case OBJ_MISCELLANY: cprintf("Miscellaneous");   break;
                case OBJ_CORPSES:    cprintf("Carrion");         break;

                //case OBJ_UNKNOWN_I:  cprintf("Books");           break;
                //case OBJ_UNKNOWN_II: cprintf("Gems");            break;
                //case OBJ_GEMSTONES:  cprintf("Miscellaneous");   break;
                default:             cprintf("Buggy Items");     break;
                }

                textcolor( LIGHTGREY );
                textbackground( BLACK );
                lines++;

                for (j = 0; j < ENDOFPACK; j++)
                {
                    if (lines > num_lines - 2 && inv_count > 0)
                    {
                        gotoxy(1, num_lines);
                        cprintf( MORE_STRING );
                        ki = getch();

                        if (ki == ESCAPE || isalnum(ki) || ki == '?' || ki == '*')
                        {
                            RETURN_CLEANUP( ki );
                        }

                        if (ki == 0)
                            ki = getch();

                        lines = 0;
                        clrscr();
                        gotoxy(1, 1);
                        anything = false;
                    }

                    if (is_valid_item(you.inv[j]) && you.inv[j].base_type == c)
                    {
                        anything = true;

                        if (lines > 0)
                            cprintf(EOL);

                        lines++;
                        inv_count--;

                        in_name( j, DESC_INVENTORY_EQUIP, st_pass, false, true );
                        strncpy( info, st_pass, INFO_SIZE );

                        info[78] = '\0';  // truncate to fit line

                        if (Options.invent_colours)
                            textcolor( you.inv[j].colour );

                        cprintf( info );

                        if (show_price)
                        {
                            gotoxy( 65 - 12 * Options.invent_weights, wherey() );
                            cprintf( " %7d gold",
                                     item_value( you.inv[j], temp_id, true ) );
                        }

                        if (Options.invent_weights)
                        {
                            const int mass = item_mass( you.inv[j] ) * you.inv[j].quantity;
                            gotoxy( 67, wherey() );
                            cprintf( " %6.1f aum",
                                     static_cast<float>( mass ) / 10.0 );
                        }
                    }
                }               // end of j loop
            }                   // end of if num_class
        }                       // end of i loop.
    }
    else
    {
        // display message if nothing to display:
        if (item_class_inv == -1)
            cprintf("You aren't carrying anything.");
        else
        {
            if (item_class_inv == OBJ_WEAPONS)
                cprintf("You aren't carrying any weapons.");
            else if (item_class_inv == OBJ_MISSILES)
                cprintf("You aren't carrying any ammunition.");
            else
                cprintf("You aren't carrying any such object.");

            anything = true;
        }
    }

    if (anything)
    {
        ki = getch();

        if (ki == 0)
            ki = getch();
    }

    RETURN_CLEANUP( ki );
}                               // end show_invent()


// Reads in digits for a count and apprends then to val, the
// return value is the character that stopped the reading.
static unsigned char get_invent_quant( unsigned char keyin, int &quant )
{
    quant = keyin - '0';

    for(;;)
    {
        keyin = get_ch();

        if (!isdigit( keyin ))
            break;

        quant *= 10;
        quant += (keyin - '0');

        if (quant > 9999999)
        {
            quant = 9999999;
            keyin = '\0';
            break;
        }
    }

    return (keyin);
}

// This function prompts the user for an item, handles the '?' and '*'
// listings, and returns the inventory slot to the caller (which if
// must_exist is true (the default) will be an assigned item, with
// a positive quantity.
//
// It returns PROMPT_ABORT       if the player hits escape.
// It returns PROMPT_GOT_SPECIAL if the player hits the "other_valid_char".
//
// Note: This function never checks if the item is appropriate.
std::vector<SelItem> prompt_invent_items(
                        const char *prompt,
                        int type_expect,
                        std::string (*titlefn)( int menuflags,
                                                const std::string &oldt ),
                        bool allow_auto_list,
                        bool allow_easy_quit,
                        const char other_valid_char,
                        std::vector<std::string> *select_filter )
{
    unsigned char  keyin = 0;
    int            ret = -1;

    bool           need_redraw = false;
    bool           need_prompt = true;
    bool           need_getch  = true;

    if (Options.auto_list && allow_auto_list)
    {
        need_getch  = false;
        need_redraw = false;
        need_prompt = false;
        keyin       = '?';
    }

    std::vector< SelItem > items;
    int count = -1;
    for (;;)
    {
        if (need_redraw)
        {
            redraw_screen();
            mesclr( true );
        }

        if (need_prompt)
            mpr( prompt, MSGCH_PROMPT );

        if (need_getch)
            keyin = get_ch();

        need_redraw = false;
        need_prompt = true;
        need_getch  = true;

        // Note:  We handle any "special" character first, so that
        //        it can be used to override the others.
        if (other_valid_char != '\0' && keyin == other_valid_char)
        {
            ret = PROMPT_GOT_SPECIAL;
            break;
        }
        else if (keyin == '?' || keyin == '*' || keyin == ',')
        {
            const int selmode = (Options.drop_mode == DM_SINGLE)
                            ? MF_SINGLESELECT | MF_EASY_EXIT | MF_ANYPRINTABLE
                            : MF_MULTISELECT;

            // The "view inventory listing" mode.
            int ch = invent_select( (keyin == '*') ? -1 : type_expect,
                                    (selmode | MF_SELECT_ANY_PAGE),
                                    titlefn, &items, select_filter );

            if (selmode & MF_SINGLESELECT)
            {
                keyin       = ch;
                need_getch  = false;
            }
            else
            {
                keyin       = 0;
                need_getch  = true;
            }

            if (items.size())
            {
                redraw_screen();
                mesclr( true );

                const int num_items = items.size();
                for (int i = 0; i < num_items; ++i)
                    items[i].slot = letter_to_index( items[i].slot );

                return (items);
            }

            need_redraw = (keyin != '?' && keyin != '*'
                            && keyin != ',' && keyin != '+');

            need_prompt = need_redraw;
        }
        else if (isdigit( keyin ))
        {
            // The "read in quantity" mode
            keyin = get_invent_quant( keyin, count );

            need_prompt = false;
            need_getch  = false;
        }
        else if (keyin == ESCAPE
                || (Options.easy_quit_item_prompts
                    && allow_easy_quit
                    && keyin == ' '))
        {
            ret = PROMPT_ABORT;
            break;
        }
        else if (isalpha( keyin ))
        {
            ret = letter_to_index( keyin );

            if (!is_valid_item( you.inv[ret] ))
                mpr( "You do not have any such object." );
            else
                break;
        }
        else if (!isspace( keyin ))
        {
            // we've got a character we don't understand...
            canned_msg( MSG_HUH );
        }
    }

    if (ret != PROMPT_ABORT)
        items.push_back( SelItem( ret, count ) );

    return (items);
}

// This function prompts the user for an item, handles the '?' and '*'
// listings, and returns the inventory slot to the caller (which if
// must_exist is true (the default) will be an assigned item, with
// a positive quantity.
//
// It returns PROMPT_ABORT       if the player hits escape.
// It returns PROMPT_GOT_SPECIAL if the player hits the "other_valid_char".
//
// Note: This function never checks if the item is appropriate.
int prompt_invent_item( const char *prompt, int type_expect,
                        bool must_exist, bool allow_auto_list,
                        bool allow_easy_quit,
                        const char other_valid_char,
                        int *const count )
{
    unsigned char  keyin = 0;
    int            ret = -1;

    bool           need_redraw = false;
    bool           need_prompt = true;
    bool           need_getch  = true;

    if (Options.auto_list && allow_auto_list)
    {
        std::vector<SelItem> items;

        // pretend the player has hit '?' and setup state.
        keyin = invent_select( type_expect,
                               MF_SINGLESELECT | MF_ANYPRINTABLE | MF_EASY_EXIT,
                               NULL, &items );

        if (items.size())
        {
            if (count)
                *count = items[0].quantity;

            redraw_screen();
            mesclr( true );
            return (letter_to_index( keyin ));
        }

        need_getch = false;

        // Don't redraw if we're just going to display another listing
        need_redraw = (keyin != '?' && keyin != '*');

        // A prompt is nice for when we're moving to "count" mode.
        need_prompt = (count != NULL && isdigit( keyin ));
    }

    // start the state machine:
    for (;;)
    {
        // handle things we need
        if (need_redraw)
        {
            redraw_screen();
            mesclr( true );
        }

        if (need_prompt)
            mpr( MSGCH_PROMPT, prompt );

        if (need_getch)
            keyin = get_ch();

        // set needs to default state
        need_redraw = false;
        need_prompt = true;
        need_getch  = true;

        // Note:  We handle any "special" character first, so that
        //        it can be used to override the others.
        if (other_valid_char != '\0' && keyin == other_valid_char)
        {
            ret = PROMPT_GOT_SPECIAL;
            break;
        }
        else if (keyin == '?' || keyin == '*')
        {
            // The "view inventory listing" mode.
            std::vector< SelItem > items;

            keyin = invent_select( (keyin == '*') ? -1 : type_expect,
                               MF_SINGLESELECT | MF_ANYPRINTABLE | MF_EASY_EXIT,
                               NULL,
                               &items );

            if (items.size())
            {
                if (count)
                    *count = items[0].quantity;

                redraw_screen();
                mesclr( true );
                return (letter_to_index( keyin ));
            }

            need_getch  = false;

            // Don't redraw if we're just going to display another listing
            need_redraw = (keyin != '?' && keyin != '*');

            // A prompt is nice for when we're moving to "count" mode.
            need_prompt = (count != NULL && isdigit( keyin ));
        }
        else if (count != NULL && isdigit( keyin ))
        {
            // The "read in quantity" mode
            keyin = get_invent_quant( keyin, *count );

            need_prompt = false;
            need_getch  = false;
        }
        else if (keyin == ESCAPE
                || (Options.easy_quit_item_prompts
                    && allow_easy_quit
                    && keyin == ' '))
        {
            ret = PROMPT_ABORT;
            break;
        }
        else if (isalpha( keyin ))
        {
            ret = letter_to_index( keyin );

            if (must_exist && !is_valid_item( you.inv[ret] ))
                mpr( "You do not have any such object." );
            else
                break;
        }
        else if (!isspace( keyin ))
        {
            // we've got a character we don't understand...
            canned_msg( MSG_HUH );
        }
    }

    return (ret);
}

void list_commands( bool wizard )
{
    const char *line;
    int j = 0;

    SCREEN_INIT();

    clrscr();

    // BCR - Set to screen length - 1 to display the "more" string
    int moreLength = (get_number_of_lines() - 1) * 2;

    for (int i = 0; i < 500; i++)
    {
        if (wizard)
            line = wizard_string( i );
        else
            line = command_string( i );

        if (strlen( line ) != 0)
        {
            // BCR - If we've reached the end of the screen, clear
            if (j == moreLength)
            {
                gotoxy(2, j / 2 + 1);
                cprintf("More...");
                getch();
                clrscr();
                j = 0;
            }

            gotoxy( ((j % 2) ? 40 : 2), ((j / 2) + 1) );
            cprintf( line );

            j++;
        }
    }

    getch();

    RETURN_CLEANUP_VOID();
}                               // end list_commands()

const char *wizard_string( int i )
{
    UNUSED( i );

#ifdef WIZARD
    return((i ==  10) ? "a    : acquirement"                  :
           (i ==  13) ? "A    : set all skills to level"      :
           (i ==  15) ? "b    : controlled blink"             :
           (i ==  20) ? "B    : banish yourself to the Abyss" :
           (i ==  25) ? "e    : permanently enslave a monster":
           (i ==  30) ? "g    : add a skill"                  :
           (i ==  35) ? "G    : remove all monsters"          :
           (i ==  40) ? "h/H  : heal yourself (super-Heal)"   :
           (i ==  50) ? "i/I  : identify/unidentify inventory":
           (i ==  70) ? "l    : make entrance to labyrinth"   :
           (i ==  80) ? "m/M  : create monster by number/name":
           (i ==  90) ? "o/%%  : create an object"            :
           (i == 100) ? "p    : make entrance to pandemonium" :
           (i == 110) ? "x    : gain an experience level"     :
           (i == 115) ? "r    : change character's species"   :
           (i == 120) ? "s    : gain 20000 skill points"      :
           (i == 130) ? "S    : set skill to level"           :
           (i == 140) ? "t    : tweak object properties"      :
           (i == 150) ? "X    : Receive a gift from Xom"      :
           (i == 160) ? "z/Z  : cast any spell by number/name":
           (i == 200) ? "$    : get 1000 gold"                :
           (i == 210) ? "</>  : create up/down staircase"     :
           (i == 220) ? "u/d  : shift up/down one level"      :
           (i == 230) ? "~/\"  : goto a level"                :
           (i == 240) ? "(    : create a feature"             :
           (i == 250) ? "]    : get a mutation"               :
           (i == 260) ? "[    : get a demonspawn mutation"    :
           (i == 270) ? ":    : find branch"                  :
           (i == 280) ? "{    : magic mapping"                :
           (i == 290) ? "^    : gain piety"                   :
           (i == 300) ? "_    : gain religion"                :
           (i == 310) ? "\'    : list items"                  :
           (i == 320) ? "?    : list wizard commands"         :
           (i == 330) ? "|    : acquire all unrand artefacts" :
           (i == 340) ? "+    : turn item into random artefact" :
           (i == 350) ? "=    : sum skill points"
                      : "");

#else
    return ("");
#endif
}                               // end wizard_string()

const char *command_string( int i )
{
    /*
     * BCR - Command printing, case statement
     * Note: The numbers in this case indicate the order in which the
     *       commands will be printed out.  Make sure none of these
     *       numbers is greater than 500, because that is the limit.
     *
     * Arranged alpha lower, alpha upper, punctuation, ctrl.
     *
     */

    return((i ==  10) ? "a    : use special ability"              :
           (i ==  20) ? "d(#) : drop (exact quantity of) items"   :
           (i ==  30) ? "e    : eat food"                         :
           (i ==  40) ? "f    : fire first available missile"     :
           (i ==  50) ? "i    : inventory listing"                :
           (i ==  55) ? "m    : check skills"                     :
           (i ==  60) ? "o c  : open/close a door"                :
           (i ==  65) ? "p    : pray"                             :
           (i ==  70) ? "q    : quaff a potion"                   :
           (i ==  80) ? "r    : read a scroll or book"            :
           (i ==  90) ? "s    : search adjacent tiles"            :
           (i == 100) ? "t    : throw/shoot an item"              :
           (i == 110) ? "v    : view item description"            :
           (i == 120) ? "w    : wield an item"                    :
           (i == 130) ? "x    : examine visible surroundings"     :
           (i == 135) ? "z    : zap a wand"                       :
           (i == 140) ? "A    : list abilities/mutations"         :
           (i == 141) ? "C    : check experience"                 :
           (i == 142) ? "D    : dissect a corpse"                 :
           (i == 145) ? "E    : evoke power of wielded item"      :
           (i == 150) ? "M    : memorise a spell"                 :
           (i == 155) ? "O    : overview of the dungeon"          :
           (i == 160) ? "P R  : put on/remove jewellery"          :
           (i == 165) ? "Q    : quit without saving"              :
           (i == 168) ? "S    : save game and exit"               :
           (i == 200) ? "W T  : wear/take off armour"             :
           (i == 210) ? "X    : examine level map"                :
           (i == 220) ? "Z    : cast a spell"                     :
           (i == 240) ? ", g  : pick up items"                    :
           (i == 242) ? ".    : rest one turn"                    :
           (i == 250) ? "< >  : ascend/descend a staircase"       :
           (i == 270) ? ";    : examine occupied tile"            :
           (i == 280) ? "\\    : check item knowledge"            :
#ifdef WIZARD
           (i == 290) ? "&    : use WIZARD command"               :
#endif
           (i == 300) ? "+ -  : scroll level map up/down"         :
           (i == 310) ? "!    : shout or command allies"          :
           (i == 325) ? "^    : describe religion"                :
           (i == 337) ? "@    : status"                           :
           (i == 340) ? "#    : dump character to file"           :
           (i == 350) ? "=    : set abil/invent/spell letter"     :
           (i == 360) ? "\'    : wield item a, or switch to b"    :
#ifdef USE_MACROS
           (i == 380) ? "`    : add macro"                        :
           (i == 390) ? "~    : save macros"                      :
#endif
           (i == 400) ? ")    : display current weapons"          :
           (i == 401) ? "]    : display worn armour"              :
           (i == 402) ? "\"    : display worn jewellery"          :
           (i == 405) ? "Ctrl-A : toggle autopickup"              :
           (i == 410) ? "Ctrl-C : clean map"                      :
           (i == 415) ? "Ctrl-P : see old messages"               :
#ifdef PLAIN_TERM
           (i == 420) ? "Ctrl-R : redraw screen"                  :
#endif
           (i == 425) ? "Ctrl-X : save game without query"        :

#ifdef ALLOW_DESTROY_ITEM_COMMAND
           (i == 430) ? "Ctrl-D : destroy inventory item"         :
#endif

#ifdef STASH_TRACKING
           (i == 435) ? "Ctrl-S : mark stash"                     :
           (i == 440) ? "Ctrl-E : forget stash"                   :
#endif
           (i == 442) ? "Ctrl-F : fix waypoint"                   :
           (i == 445) ? "Ctrl-G : interlevel travel"              :
           (i == 447) ? "Ctrl-O : explore"                        :
           (i == 450) ? "Ctrl-V : version information"            :

           (i == 460) ? ": DIR  : step without pickup"            :
           (i == 465) ? "Shift-DIR : long walk"                   :
           (i == 470) ? "/ DIR  : long walk"                      :
           (i == 475) ? "Ctrl-DIR  : door; untrap; attack"        :
           (i == 480) ? "* DIR  : door; untrap; attack"           :
           (i == 485) ? "Shift-5 on keypad : rest 100 turns"
                      : "");
}                               // end command_string()

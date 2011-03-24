/*
 *  File:       stash.cc
 *  Summary:    Classes tracking player stashes
 *  Written by: Darshan Shaligram
 */

#include "AppHdr.h"
#include "chardump.h"
#include "describe.h"
#include "itemname.h"
#include "files.h"
#include "Kills.h"
#include "stash.h"
#include "tags.h"
#include "travel.h"
#include <fstream>
#include <stdio.h>
#include <algorithm>

#ifdef STASH_TRACKING

using std::ofstream;
using std::endl;

#define ST_MAJOR_VER (static_cast<unsigned char>(4))
#define ST_MINOR_VER (static_cast<unsigned char>(2))

bool Stash::aggressive_verify = true;
std::vector<item_def> Stash::filters;

Stash::Stash( int xp, int yp ) : enabled(true), items()
{
    // First, fix what square we're interested in
    if (xp == -1)
    {
        xp = you.x_pos;
        yp = you.y_pos;
    }

    x = static_cast<unsigned char>( xp );
    y = static_cast<unsigned char>( yp );
    abspos = GXM * static_cast<int>( y ) + static_cast<int>( x );

    update();
}

bool Stash::are_items_same( const item_def &a, const item_def &b )
{
    return (a.base_type == b.base_type
            && a.sub_type == b.sub_type
            && a.plus == b.plus
            && a.plus2 == b.plus2
            && a.special == b.special
            && a.colour == b.colour
            && a.flags == b.flags
            && a.quantity == b.quantity);
}

void Stash::filter( const std::string &str )
{
    std::string base = str;

    base.erase( base.find_last_not_of(" \n\t") + 1 );
    base.erase( 0, base.find_first_not_of(" \n\t") );

    unsigned char subc = 255;
    std::string::size_type cpos = base.find(":", 0);
    if (cpos != std::string::npos)
    {
        std::string subs = base.substr(cpos + 1);
        subc = atoi(subs.c_str());

        base = base.substr(0, cpos);
    }

    unsigned char basec = atoi(base.c_str());
    filter(basec, subc);
}

void Stash::filter( unsigned char base, unsigned char sub )
{
    item_def item;
    item.base_type = base;
    item.sub_type  = sub;

    filters.push_back(item);
}

bool Stash::is_filtered( const item_def &item )
{
    for (int i = 0, count = filters.size(); i < count; ++i)
    {
        const item_def &filter = filters[i];
        if (item.base_type == filter.base_type
            && (filter.sub_type == 255
                || item.sub_type == filter.sub_type))
        {
            return (true);
        }
    }

    return (false);
}

void Stash::update()
{
    int objl = igrd[x][y];

    // If this is your position, you know what's on this square
    if (x == you.x_pos && y == you.y_pos)
    {
        // Zap existing items
        items.clear();

        // Now, grab all items on that square and fill our vector
        while (objl != NON_ITEM)
        {
            if (!is_filtered( mitm[objl] ))
                items.push_back( mitm[objl] );

            objl = mitm[objl].link;
        }

        verified = true;
    }
    else
    {
        // If this is not your position, the only thing we can do is verify that
        // what the player sees on the square is the first item in this vector.
        if (objl == NON_ITEM)
        {
            items.clear();
            verified = true;
            return ;
        }

        // There's something on this square. Take a squint at it.
        item_def &item = mitm[objl];

        if (item.link == NON_ITEM)
            items.clear();

        // We knew of nothing on this square, so we'll assume this is the
        // only item here, but mark it as unverified unless we can see nothing
        // under the item.
        if (items.size() == 0)
        {
            if (!is_filtered(item))
                items.push_back(item);

            verified = (item.link == NON_ITEM);
            return;
        }

        // There's more than one item in this pile. As long as the top item is
        // not filtered, we can check to see if it matches what we think the
        // top item is.

        if (is_filtered(item))
            return;

        item_def &first = items[0];
        // Compare these items
        if (!are_items_same(first, item))
        {
            if (aggressive_verify)
            {
                // See if 'item' matches any of the items we have. If it does,
                // we'll just make that the first item and leave 'verified'
                // unchanged.

                // Start from 1 because we've already checked items[0]
                for (int i = 1, count = items.size(); i < count; ++i)
                {
                    if (are_items_same(items[i], item))
                    {
                        // Found it. Swap it to the front of the vector.
                        item_def temp   = items[i];
                        items[i]        = items[0];
                        items[0]        = temp;

                        // We don't set verified to true. If this stash was
                        // already unverified, it remains so.
                        return;
                    }
                }
            }

            // If this is unverified, forget last item on stack. This isn't
            // terribly clever, but it prevents the vector swelling forever.
            if (!verified)
                items.pop_back();

            // Items are different. We'll put this item in the front of our
            // vector, and mark this as unverified
            items.insert( items.begin(), item );
            verified = false;
        }
    }
}

// Returns the item name for a given item, with any appropriate
// stash-tracking pre/suffixes.
string Stash::stash_item_name( const item_def &item )
{
    char buf[ITEMNAME_SIZE];

    item_name(item, DESC_NOCAP_A, buf, false);
    string sname = buf;

    return (sname);
}

void Stash::display(ostream &os, int refx, int refy, std::string place) const
{
    if (!enabled || (items.size() == 0 && verified))
        return;

    os << "(" << (static_cast<int>(x) - refx) << ", "
              << (static_cast<int>(y) - refy) << (place.length()? ", " + place : "")
       << ")"
       << endl;

    char buf[ITEMNAME_SIZE];
    for (unsigned int i = 0; i < items.size(); ++i)
    {
        const item_def &item = items[i];
        if (item.base_type == OBJ_GOLD)
        {
            sprintf(buf, "%d gold piece%s", item.quantity,
                    (item.quantity > 1? "s" : ""));
        }
        else
        {
            string s = stash_item_name(item);
            strncpy(buf, s.c_str(), sizeof buf);
        }

        os << "  " << buf
           << (!verified && (items.size() > 1 || i) ? " (still there?)" : "")
           << endl;

        if (is_dumpable_artefact(item, Options.verbose_dump))
        {
            string desc = munge_description(get_item_description(item,
                                                          Options.verbose_dump,
                                                          true ));

            // Kill leading and trailing whitespace
            desc.erase(desc.find_last_not_of(" \n\t") + 1);
            desc.erase(0, desc.find_first_not_of(" \n\t"));

            // If string is not-empty, pad out to a neat indent
            if (desc.length())
            {
                // Walk backwards and prepend indenting spaces to \n characters
                for (int j = desc.length() - 1; j >= 0; --j)
                {
                    if (desc[j] == '\n')
                        desc.insert( j + 1, " " );
                }

                os << "    " << desc << endl;
            }
        }
    }

    if (items.size() <= 1 && !verified)
        os << "  (unseen)" << endl;
}

void Stash::save(FILE *file) const
{
    // How many items on this square?
    writeShort( file, static_cast<short>( items.size() ) );

    writeByte(file, x);
    writeByte(file, y);

    // Note: Enabled save value is inverted logic, so that it defaults to true
    writeByte( file, static_cast<unsigned char>((verified ? 1 : 0) | (!enabled ? 2 : 0)) );

    // And dump the items individually. We don't bother saving fields we're
    // not interested in (and don't anticipate being interested in).
    for (unsigned int i = 0; i < items.size(); ++i)
    {
        const item_def &item = items[i];

        writeByte(file, item.base_type);
        writeByte(file, item.sub_type);
        writeShort(file, item.plus);
        writeShort(file, item.plus2);
        writeLong(file, item.special);
        writeShort(file, item.quantity);
        writeByte(file, item.colour);
        writeLong(file, item.flags);
    }
}

void Stash::load(FILE *file)
{
    // How many items?
    int count = readShort(file);

    x = readByte(file);
    y = readByte(file);

    unsigned char flags = readByte(file);
    verified = (flags & 1) != 0;

    // Note: Enabled save value is inverted so it defaults to true.
    enabled = (flags & 2) == 0;

    abspos = GXM * static_cast<int>( y ) + static_cast<int>( x );

    // Zap out item vector, in case it's in use (however unlikely)
    items.clear();

    // Read in the items
    for (int i = 0; i < count; ++i)
    {
        item_def item;
        item.base_type  = readByte(file);
        item.sub_type   = readByte(file);
        item.plus       = readShort(file);
        item.plus2      = readShort(file);
        item.special    = readLong(file);
        item.quantity   = readShort(file);
        item.colour     = readByte(file);
        item.flags      = readLong(file);

        items.push_back(item);
    }
}

ostream &operator << (ostream &os, const Stash &s)
{
    s.display(os);
    return os;
}

ShopInfo::ShopInfo(int xp, int yp)
{
    x = xp;
    y = yp;

    // Most of our initialization will be done externally; this class is really
    // a mildly glorified struct.
}

void ShopInfo::add_item( const string &in_name, const string &desc,
                         unsigned int price )
{
    shop_item it;
    it.name = in_name;
    it.desc = desc;
    it.price = price;

    // trim leading whitespace
    string::size_type notwhite = it.desc.find_first_not_of(" \t\n");
    it.desc.erase(0, notwhite);

    // trim trailing whitespace
    notwhite = it.desc.find_last_not_of(" \t\n");
    it.desc.erase(notwhite + 1);

    // Walk backwards and prepend indenting spaces to \n characters
    for (int i = it.desc.length() - 1; i >= 0; --i)
    {
        if (it.desc[i] == '\n')
            it.desc.insert(i + 1, " ");
    }

    items.push_back(it);
}

void ShopInfo::display(ostream &os) const
{
    os << "[Shop] " << name << endl;

    if (items.size() > 0)
    {
        for (unsigned int i = 0; i < items.size(); ++i)
        {
            os << "  " << items[i].name << " (" << items[i].price
               << " gold)" << endl;

            if (items[i].desc.length() > 0)
                os << "    " << items[i].desc << endl;
        }
    }
    else
        os << "  (Shop is empty)" << endl;
}

void ShopInfo::save(FILE *file) const
{
    writeShort( file, static_cast<short>(x) );
    writeShort( file, static_cast<short>(y) );

    writeShort( file, static_cast<short>( items.size() ) );

    writeString(file, name);

    for (unsigned int i = 0; i < items.size(); ++i)
    {
        writeString(file, items[i].name);
        writeString(file, items[i].desc);
        writeShort(file, static_cast<short>( items[i].price ) );
    }
}

void ShopInfo::load(FILE *file)
{
    x = readShort(file);
    y = readShort(file);

    int itemcount = readShort(file);

    name = readString(file);
    for (int i = 0; i < itemcount; ++i)
    {
        shop_item item;
        item.name = readString(file);
        item.desc = readString(file);
        item.price = static_cast<unsigned int>( readShort(file) );
        items.push_back(item);
    }
}

ostream &operator << (ostream &os, const ShopInfo &s)
{
    s.display(os);
    return os;
}

LevelStashes::LevelStashes()
{
    branch = you.where_are_you;
    depth  = you.depth;
}

bool LevelStashes::operator < (const LevelStashes &lev) const
{
    return branch < lev.branch || (branch == lev.branch && depth < lev.depth);
}

bool LevelStashes::isBelowPlayer() const
{
    return branch > you.where_are_you
        || (branch == you.where_are_you && depth > you.depth);
}

Stash *LevelStashes::find_stash(int x, int y)
{
    if (x == -1 || y == -1)
    {
        x = you.x_pos;
        y = you.y_pos;
    }

    int abspos = (GXM * y) + x;
    map<int, Stash, ltint>::iterator st = stashes.find(abspos);
    return (st == stashes.end()? NULL : &st->second);
}

ShopInfo &LevelStashes::get_shop(int x, int y)
{
    for (unsigned int i = 0; i < shops.size(); ++i)
    {
        if (shops[i].isAt(x, y))
            return (shops[i]);
    }

    ShopInfo si(x, y);
    shops.push_back(si);
    return (get_shop(x, y));
}

// Updates the stash at (x,y). Returns true if there was a stash at (x,y), false
// otherwise.
bool LevelStashes::update_stash(int x, int y)
{
    Stash *s = find_stash(x, y);

    if (s)
    {
        s->update();
        if (s->empty())
            kill_stash(*s);

        return (true);
    }

    return (false);
}

// Removes a Stash from the level.
void LevelStashes::kill_stash(const Stash &s)
{
    stashes.erase(s.abs_pos());
}

void LevelStashes::no_stash( int x, int y )
{
    Stash *s = find_stash(x, y);
    bool en = false;

    if (s)
    {
        en = s->enabled = !s->enabled;
        s->update();
        if (s->empty())
            kill_stash(*s);
    }
    else
    {
        Stash newStash(x, y);
        newStash.enabled = false;

        stashes[newStash.abs_pos()] = newStash;
    }

    mpr( (en) ? "I'll no longer ignore what I see on this square."
              : "Ok, I'll ignore what I see on this square." );
}

void LevelStashes::add_stash( int x, int y )
{
    Stash *s = find_stash(x, y);
    if (s)
    {
        s->update();
        if (s->empty())
            kill_stash(*s);
    }
    else
    {
        Stash newStash(x, y);
        if (!newStash.empty())
            stashes[newStash.abs_pos()] = newStash;
    }
}

bool LevelStashes::isCurrent() const
{
    return (branch == you.where_are_you && depth == you.depth);
}

bool LevelStashes::in_hell() const
{
    return (branch >= BRANCH_DIS
            && branch <= BRANCH_THE_PIT
            && branch != BRANCH_VESTIBULE_OF_HELL);
}

bool LevelStashes::in_branch( int branchid ) const
{
    return (branch == branchid);
}

string branch_level_name( unsigned char branch, int sub_depth )
{
    if (you.level_type == LEVEL_PANDEMONIUM)
        return string("Pandemonium");
    else if (you.level_type == LEVEL_ABYSS)
        return string("The Abyss");
    else if (you.level_type == LEVEL_LABYRINTH)
        return string("A Labyrinth");
    else
    {
        char buf[200];
        const char *s = NULL;
        *buf = 0;
        // level_type == LEVEL_DUNGEON
        if (branch != BRANCH_VESTIBULE_OF_HELL
                && branch != BRANCH_ECUMENICAL_TEMPLE
                && branch != BRANCH_HALL_OF_BLADES)
        {
            sprintf(buf, "Level %d", sub_depth);
        }

        switch (branch)
        {
        case BRANCH_MAIN_DUNGEON:
            s = " of the Dungeon";
            break;
        case BRANCH_DIS:
            s = " of Dis";
            break;
        case BRANCH_GEHENNA:
            s = " of Gehenna";
            break;
        case BRANCH_VESTIBULE_OF_HELL:
            s = "The Vestibule of Hell";
            break;
        case BRANCH_COCYTUS:
            s = " of Cocytus";
            break;
        case BRANCH_TARTARUS:
            s = " of Tartarus";
            break;
        case BRANCH_INFERNO:
            s = " of the Inferno";
            break;
        case BRANCH_THE_PIT:
            s = " of the Pit";
            break;
        case BRANCH_ORCISH_MINES:
            s = " of the Orcish Mines";
            break;
        case BRANCH_HIVE:
            s = " of the Hive";
            break;
        case BRANCH_LAIR:
            s = " of the Lair";
            break;
        case BRANCH_SLIME_PITS:
            s = " of the Slime Pits";
            break;
        case BRANCH_VAULTS:
            s = " of the Vaults";
            break;
        case BRANCH_CRYPT:
            s = " of the Crypt";
            break;
        case BRANCH_HALL_OF_BLADES:
            s = "The Hall of Blades";
            break;
        case BRANCH_HALL_OF_ZOT:
            s = " of the Realm of Zot";
            break;
        case BRANCH_ECUMENICAL_TEMPLE:
            s = "The Ecumenical Temple";
            break;
        case BRANCH_SNAKE_PIT:
            s = " of the Snake Pit";
            break;
        case BRANCH_ELVEN_HALLS:
            s = " of the Elven Halls";
            break;
        case BRANCH_TOMB:
            s = " of the Tomb";
            break;
        case BRANCH_SWAMP:
            s = " of the Swamp";
            break;
        }

        if (s)
            strcat(buf, s);

        return (string(buf));
    }
}

string LevelStashes::level_name() const
{
    int curr_subdungeon_level = subdungeon_depth( branch, depth );
    return (branch_level_name(branch, curr_subdungeon_level));
}

string LevelStashes::short_level_name() const
{
    return (short_place_name( get_packed_place( branch,
                                    subdungeon_depth( branch, depth ),
                                    LEVEL_DUNGEON ) ));
}

int LevelStashes::count_stashes() const
{
    int rawcount = stashes.size();
    if (!rawcount)
        return (0);

    for (map<int, Stash, ltint>::const_iterator iter = stashes.begin();
            iter != stashes.end(); iter++)
    {
        if (!iter->second.enabled)
            --rawcount;
    }

    return (rawcount);
}

void LevelStashes::display(ostream &os) const
{
    if (visible_stash_count() == 0)
        return;

    os << level_name() << endl;

    for (unsigned int i = 0; i < shops.size(); ++i)
    {
        os << shops[i] << endl;
    }

    if (stashes.size())
    {
        const Stash &s = stashes.begin()->second;
        int refx = s.getX(), refy = s.getY();

        for (map<int, Stash, ltint>::const_iterator iter = stashes.begin();
                iter != stashes.end(); iter++)
        {
            iter->second.display(os, refx, refy, short_level_name());
        }
    }

    os << endl;
}

void LevelStashes::save(FILE *file) const
{
    // How many stashes on this level?
    writeShort(file, static_cast<short>( stashes.size() ));

    writeByte(file, branch);
    writeShort(file, static_cast<short>( depth ));

    // And write the individual stashes
    for (map<int, Stash, ltint>::const_iterator iter = stashes.begin();
            iter != stashes.end(); iter++)
    {
        iter->second.save(file);
    }

    writeShort(file, static_cast<short>( shops.size() ));
    for (unsigned int i = 0; i < shops.size(); ++i)
        shops[i].save(file);
}

void LevelStashes::load(FILE *file)
{
    int size = readShort(file);

    branch = readByte(file);
    depth  = readShort(file);

    stashes.clear();
    for (int i = 0; i < size; ++i)
    {
        Stash s;
        s.load(file);
        if (!s.empty())
            stashes[s.abs_pos()] = s;
    }

    shops.clear();
    int shopc = readShort(file);
    for (int i = 0; i < shopc; ++i)
    {
        ShopInfo si(0, 0);
        si.load(file);
        shops.push_back(si);
    }
}

ostream &operator << (ostream &os, const LevelStashes &ls)
{
    ls.display(os);
    return (os);
}

LevelStashes &StashTracker::get_current_level()
{
    vector<LevelStashes>::iterator iter = levels.begin();

    for ( ; iter != levels.end() && !iter->isBelowPlayer(); iter++)
    {
        if (iter->isCurrent())
            return (*iter);
    }

    if (iter == levels.end())
        levels.push_back(LevelStashes());
    else
        levels.insert(iter, LevelStashes());

    return (get_current_level());
}

LevelStashes *StashTracker::find_current_level()
{
    vector<LevelStashes>::iterator iter = levels.begin();

    for ( ; iter != levels.end() && !iter->isBelowPlayer(); iter++)
    {
        if (iter->isCurrent())
            return (&*iter);
    }

    return (NULL);
}


bool StashTracker::update_stash(int x, int y)
{
    LevelStashes *lev = find_current_level();

    if (lev)
    {
        bool res = lev->update_stash(x, y);

        if (!lev->stash_count())
            remove_level(*lev);

        return (res);
    }
    return (false);
}

void StashTracker::remove_level(const LevelStashes &ls)
{
    vector<LevelStashes>::iterator iter = levels.begin();

    for ( ; iter != levels.end(); ++iter)
    {
        if (&ls == &*iter)
        {
            levels.erase(iter);
            break ;
        }
    }
}

void StashTracker::no_stash(int x, int y)
{
    if (is_level_untrackable())
        return ;

    LevelStashes &current = get_current_level();
    current.no_stash(x, y);

    if (!current.stash_count())
        remove_level(current);
}

void StashTracker::add_stash(int x, int y)
{
    if (is_level_untrackable())
        return ;

    LevelStashes &current = get_current_level();
    current.add_stash(x, y);

    if (!current.stash_count())
        remove_level(current);
}

void StashTracker::dump(const char *filename) const
{
    ofstream outf(filename);
    if (outf)
    {
        display(outf);
        outf.close();
    }
}

void StashTracker::display(ostream &os) const
{
    os << you.your_name << endl << endl;

    if (!levels.size())
        os << "  You have no stashes." << endl;
    else
    {
        vector<LevelStashes>::const_iterator iter = levels.begin();
        for ( ; iter != levels.end(); iter++)
        {
            os << *iter << endl;
        }
    }
}

void StashTracker::save(FILE *file) const
{
    // Write version info first - major + minor
    writeByte(file, ST_MAJOR_VER);
    writeByte(file, ST_MINOR_VER);

    // How many levels have we?
    writeShort(file, static_cast<short>( levels.size() ));

    // And ask each level to write itself to the tag
    vector<LevelStashes>::const_iterator iter = levels.begin();
    for ( ; iter != levels.end(); iter++)
    {
        iter->save(file);
    }
}

void StashTracker::load(FILE *file)
{
    // Check version. Compatibility isn't important, since stash-tracking
    // is non-critical.
    unsigned char major = readByte(file),
                  minor = readByte(file);

    if (major != ST_MAJOR_VER || minor != ST_MINOR_VER)
        return;

    int count = readShort(file);

    levels.clear();
    for (int i = 0; i < count; ++i)
    {
        LevelStashes st;
        st.load(file);

        if (st.stash_count())
            levels.push_back(st);
    }
}

void StashTracker::update_visible_stashes( StashTracker::stash_update_mode mode)
{
    if (is_level_untrackable())
        return ;

    LevelStashes *lev = find_current_level();
    for (int cy = 1; cy <= 17; ++cy)
    {
        for (int cx = 9; cx <= 25; ++cx)
        {
            const int x = you.x_pos + cx - 17;
            const int y = you.y_pos + cy - 9;

            if (x < 0 || x >= GXM || y < 0 || y >= GYM)
                continue;

            if (!env.show[cx - 8][cy] && !(cx == 17 && cy == 9))
                continue;

            if ((!lev || !lev->update_stash(x, y)) && mode == ST_AGGRESSIVE
                && igrd[x][y] != NON_ITEM)
            {
                if (!lev)
                    lev = &get_current_level();
                lev->add_stash(x, y);
            }
        }
    }
    if (lev && !lev->stash_count())
        remove_level(*lev);
}

// XXX: move this to global.cc, or find a home in one of the other structs?
// Global
StashTracker stashes;

#endif

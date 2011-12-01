/*
 *  File:       stash.h
 *  Summary:    Classes tracking player stashes
 *  Written by: Darshan Shaligram
 */
#ifndef STASH_H
#define STASH_H

#include "AppHdr.h"
#include "globals.h"
#include <string>
using std::string;

#ifdef STASH_TRACKING

#include <iostream>
using std::ostream;
using std::istream;

#include <list>
using std::list;

#include <vector>
using std::vector;

#include "externs.h"

// Stash definitions

enum STASH_TRACK_MODES
{
    STM_NONE,             // Stashes are not tracked
    STM_EXPLICIT,         // Only explicitly marked stashes are tracked
    STM_DROPPED,          // Dropped items and explicitly marked stashes are
                          // tracked
    STM_ALL               // All seen items are tracked
};

class Stash {
public:
    Stash(int xp = -1, int yp = -1);

    static void filter( unsigned char base_type, unsigned char sub_type );
    static void filter( const std::string &filt );

    static string stash_item_name( const item_def &item );
    void update();
    void save( FILE* ) const;
    void load( FILE* );

    void display( ostream &os, int refx = 0, int refy = 0,
                  std::string place = "" ) const;

    bool empty() const {
        return (items.empty() && enabled);
    }

    bool isAt(int xp, int yp) const { return (x == xp && y == yp); }
    int  abs_pos() const { return (abspos); }
    int  getX() const { return (static_cast<int>(x)); }
    int  getY() const { return (static_cast<int>(y)); }

    bool is_verified() const {  return (verified); }

    bool enabled;       // If false, this Stash will neither track
                        // items nor dump itself. Disabled stashes are
                        // also never removed from the level's map of
                        // stashes.
private:
    bool verified;      // Is this correct to the best of our knowledge?
    unsigned char x, y;
    int  abspos;
    vector<item_def>   items;

    /*
     * If true (the default), the stash-tracker is a lot more likely to consider
     * squares that the player is not standing on as correctly-verified. This
     * will lead to cleaner dumps, but the chance of stashes not accurately
     * reflecting what's in the dungeon also increases.
     */
    static bool aggressive_verify;
    static std::vector<item_def> filters;

    static bool are_items_same(const item_def &, const item_def &);
    static bool is_filtered(const item_def &item);
};

class ShopInfo {
public:
    ShopInfo(int xp, int yp);

    // Note that we aren't bothering to use virtual functions here.
    void save(FILE*) const;
    void load(FILE*);

    void display(ostream &os) const;

    void reset() { items.clear(); }
    void set_name(const char *s) { name = s; }
    void add_item(const string &in_name, const string &desc, unsigned int price);

    bool isAt(int xp, int yp) const { return (x == xp && y == yp); }
private:
    int x, y;
    string name;

    // Messy!
    struct shop_item {
        string          name;
        string          desc;
        unsigned int    price;
    };
    vector<shop_item> items;
};

extern ostream &operator << (ostream &, const Stash &);

class LevelStashes {
public:
    LevelStashes();

    Stash *find_stash(int x = -1, int y = -1);

    ShopInfo &get_shop(int x, int y);

    // Update stash at (x,y) on current level, defaulting to player's current
    // location if no parameters are supplied.
    bool  update_stash(int x = -1, int y = -1);

    // Add stash at (x,y), or player's current location if no parameters are
    // supplied
    void  add_stash(int x = -1, int y = -1);

    // Mark square (x,y) as not stashworthy. The player's current location is
    // used if no parameters are supplied.
    void  no_stash(int x = -1, int y = -1);

    void  kill_stash(const Stash &s);

    void  save(FILE *) const;
    void  load(FILE *);

    void  display(ostream &os) const;
    string level_name() const;
    string short_level_name() const;
    bool  in_hell() const;
    bool  in_branch(int) const;

    int   stash_count() const { return (stashes.size() + shops.size()); }
    int   visible_stash_count() const { return (count_stashes() + shops.size()); }

    bool  isCurrent() const;
    bool  isBelowPlayer() const;
    bool  operator < (const LevelStashes &lev) const;
private:
    // which level
    unsigned char branch;
    int depth;

    // and a list of the stashes on this level
    list<Stash> stashes;

    // a vector of shops on this level
    vector<ShopInfo> shops;

    int count_stashes() const;
};

extern ostream &operator << (ostream &, const LevelStashes &);

class StashTracker {
public:
    static bool is_level_untrackable() {
        return (you.level_type == LEVEL_LABYRINTH
                || you.level_type == LEVEL_ABYSS);
    }

    StashTracker() : levels() {
    }

    LevelStashes &get_current_level();
    LevelStashes *find_current_level();

    ShopInfo &get_shop(int x, int y) {
        return (get_current_level().get_shop(x, y));
    }

    void remove_level(const LevelStashes &ls);

    enum stash_update_mode {
        ST_PASSIVE,             // Maintain existing stashes only.
        ST_AGGRESSIVE           // Create stashes for each square containing
                                // objects, even if those squares were not
                                // previously marked as stashes.
    };

    void update_visible_stashes(StashTracker::stash_update_mode = ST_PASSIVE);

    // Update stash at (x,y) on current level, defaulting to player's current
    // location if no parameters are supplied, return true if a stash was
    // updated.
    bool update_stash(int x = -1, int y = -1);

    // Add stash at (x,y), or player's current location if no parameters are
    // supplied
    void add_stash(int x = -1, int y = -1);

    // Mark square (x,y) as not stashworthy. The player's current location is
    // used if no parameters are supplied.
    void no_stash(int x = -1, int y = -1);

    void save(FILE*) const;
    void load(FILE*);

    void display(ostream &os) const;

    void dump(const char *filename) const;
private:
    vector<LevelStashes> levels;
};

string branch_level_name(unsigned char branch, int sub_depth);

extern StashTracker stashes;

#endif // STASH_TRACKING

#endif

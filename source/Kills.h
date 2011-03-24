/*
 *  File:       kills.h
 *  Summary:    Tracks monsters the player has killed.
 *  Written by: Darshan Shaligram
 */
#ifndef KILLS_H
#define KILLS_H

#include <vector>
#include <string>
#include <map>
#include <stdio.h>

struct monsters;

// Not intended for external use!
struct ___monster_desc {
    ___monster_desc(const struct monsters *);
    ___monster_desc() { }

    void save(FILE*) const;
    void load(FILE*);

    enum name_modifier {
        M_NORMAL, M_ZOMBIE, M_SKELETON, M_SIMULACRUM, M_SPECTRE,
        M_SHAPESHIFTER  // A shapeshifter pretending to be 'monnum'
    };

    int monnum;                 // Number of the beast
    name_modifier modifier;     // Nature of the beast

    struct less_than
    {
        bool operator () ( const ___monster_desc &m1,
                           const ___monster_desc &m2)
        {
            return (m1.monnum < m2.monnum
                    || (m1.monnum == m2.monnum && m1.modifier < m2.modifier));
        }
    };
};

#define PLACE_LIMIT 5   // How many unique kill places we're prepared to track
class kill_def {
public:
    kill_def(const struct monsters *mon);
    kill_def() : kills(0), exp(0) {
        // This object just says to the world that it's uninitialized
    }

    void save(FILE*) const;
    void load(FILE*);

    void add_kill(const struct monsters *mon, unsigned short place);

    std::string info( const ___monster_desc &md ) const;
    std::string base_name( const ___monster_desc &md ) const;

    unsigned short kills;           // How many kills does the player have?
    int            exp;             // Experience gained for slaying the beast.
                                    // Only set *once*, even for shapeshifters.

    std::vector<unsigned short> places; // Places where we've killed the beast.
private:
    std::string append_places( const ___monster_desc &md,
                               const std::string &name ) const;
};

class kill_ghost {
public:
    kill_ghost(const struct monsters *mon);
    kill_ghost() { }

    void save(FILE*) const;
    void load(FILE*);

    std::string info() const;

    std::string ghost_name;
    int exp;
    unsigned short place;
};

class Kills {
public:
    void record_kill(const struct monsters *mon);

    void save(FILE*) const;
    void load(FILE*);

    std::string kill_info() const;
private:
    std::map<___monster_desc, kill_def, ___monster_desc::less_than> kills;
    std::vector<kill_ghost> ghosts;

    void record_ghost_kill(const struct monsters *mon);
};

unsigned short get_packed_place( unsigned char branch, int subdepth,
                                 char level_type );

std::string short_place_name( unsigned short place );

enum KILL_DUMP_OPTIONS {
    KDO_NO_PLACES,          // Don't dump places at all
    KDO_ONE_PLACE,          // Show places only for single kills and uniques.
    KDO_ALL_PLACES          // Show all available place information
};

#endif

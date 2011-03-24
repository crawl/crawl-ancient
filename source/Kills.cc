/*
 *  File:       kills.cc
 *  Summary:    Player kill tracking
 *  Written by: Darshan Shaligram
 */
#include "AppHdr.h"
#include "chardump.h"
#include "describe.h"
#include "mon-util.h"
#include "files.h"
#include "globals.h"
#include "itemname.h"
#include "travel.h"
#include "tags.h"
#include "Kills.h"
#include <algorithm>
#include <string.h>

#define KILLS_MAJOR_VERSION 4
#define KILLS_MINOR_VERSION 0

unsigned short get_packed_place( unsigned char branch, int subdepth,
                                 char level_type )
{
    unsigned short place = static_cast<unsigned short>( (branch << 8) | subdepth );

    if (level_type == LEVEL_ABYSS
        || level_type == LEVEL_PANDEMONIUM
        || level_type == LEVEL_LABYRINTH)
    {
        place = static_cast<unsigned short>( (level_type << 8) | 0xFF );
    }

    return (place);
}

static unsigned short get_packed_place()
{
    return (get_packed_place( you.where_are_you,
                              subdungeon_depth( you.where_are_you, you.depth ),
                              you.level_type ));
}

void Kills::record_kill( const struct monsters *mon )
{
    // Handle player ghosts separately.
    if (mon->type == MONS_PLAYER_GHOST || mon->type == MONS_PANDEMONIUM_DEMON)
    {
        record_ghost_kill(mon);
        return;
    }

    // Normal monsters
    // Create a descriptor
    ___monster_desc descriptor = mon;

    kill_def::kill_def &k = kills[descriptor];

    if (k.kills)
        k.add_kill(mon, get_packed_place());
    else
        k = kill_def::kill_def(mon);
}

struct kill_exp {
    int exp;
    std::string base_name;
    std::string desc;

    kill_exp( int ex, const std::string &base, const std::string &s )
    {
        exp = ex;
        desc = s;
        base_name = base;
    }

    bool operator < ( const kill_exp &b) const
    {
        return exp == b.exp? (base_name < b.base_name) : (exp > b.exp);
    }
};

std::string Kills::kill_info() const
{
    if (!kills.size() && !ghosts.size())
        return std::string("");

    std::string killtext = "  Vanquished Creatures" EOL;

    // We put all the kills into a vector, then sort the vector based
    // on experience (descending) and monster name (alphabetic).
    std::vector<kill_exp> all_kills;

    long count = 0;

    std::map<___monster_desc, kill_def::kill_def, ___monster_desc::less_than>::
        const_iterator iter = kills.begin();

    for (; iter != kills.end(); ++iter)
    {
        const ___monster_desc &md = iter->first;
        const kill_def::kill_def &k = iter->second;

        all_kills.push_back( kill_exp( k.exp, k.base_name(md), k.info(md) ) );
        count += k.kills;
    }

    std::vector<kill_ghost>::const_iterator gi = ghosts.begin();
    for (; gi != ghosts.end(); ++gi)
    {
        all_kills.push_back( kill_exp( gi->exp, std::string(""), gi->info() ) );
    }

    count += ghosts.size();

    std::sort(all_kills.begin(), all_kills.end());
    for (int i = 0, sz = all_kills.size(); i < sz; ++i)
    {
        killtext += all_kills[i].desc;
        killtext += EOL;
    }

    killtext += EOL;

    {
        char numbuf[100];

        snprintf( numbuf, sizeof( numbuf ),
                  " %ld creature%s vanquished." EOL, count,
                  (count > 1) ? "s" : "" );

        killtext += numbuf;
    }

    return (killtext);
}

// Takes a packed 'place' and returns a compact stringified place name.
// XXX: This is done in several other places; a unified function to
//      describe places would be nice.
std::string short_place_name(unsigned short place)
{
    unsigned char branch = static_cast<unsigned char>( (place >> 8) & 0xFF );
    int lev = place & 0xFF;

    const char *s;
    bool level_num = false;

    if (lev == 0xFF)
    {
        switch (branch)
        {
        case LEVEL_ABYSS:
            s = "Abyss";
            break;
        case LEVEL_PANDEMONIUM:
            s = "Pan";
            break;
        case LEVEL_LABYRINTH:
            s = "Lab";
            break;
        default:
            s = "Buggy Badlands";
            break;
        }
    }
    else
    {
        switch (branch)
        {
        case BRANCH_VESTIBULE_OF_HELL:
            s = "Hell";
            break;
        case BRANCH_HALL_OF_BLADES:
            s = "Blade";
            break;
        case BRANCH_ECUMENICAL_TEMPLE:
            s = "Temple";
            break;
        default:
            level_num = true;
            s = (branch == BRANCH_DIS)          ? "Dis:" :
                (branch == BRANCH_GEHENNA)      ? "Geh:" :
                (branch == BRANCH_COCYTUS)      ? "Coc:" :
                (branch == BRANCH_TARTARUS)     ? "Tar:" :
                (branch == BRANCH_ORCISH_MINES) ? "Orc:" :
                (branch == BRANCH_HIVE)         ? "Hive:" :
                (branch == BRANCH_LAIR)         ? "Lair:" :
                (branch == BRANCH_SLIME_PITS)   ? "Slime:" :
                (branch == BRANCH_VAULTS)       ? "Vault:" :
                (branch == BRANCH_CRYPT)        ? "Crypt:" :
                (branch == BRANCH_HALL_OF_ZOT)  ? "Zot:" :
                (branch == BRANCH_SNAKE_PIT)    ? "Snake:" :
                (branch == BRANCH_ELVEN_HALLS)  ? "Elf:" :
                (branch == BRANCH_TOMB)         ? "Tomb:" :
                (branch == BRANCH_SWAMP)        ? "Swamp:" : "D:";
            break;
        }
    }

    std::string pl = s;

    if (level_num)
    {
        char buf[20];
        snprintf( buf, sizeof( buf ), "%d", lev );
        pl += buf;
    }

    return (pl);
}

void Kills::save( FILE *file ) const
{
    // Write the version of the kills file
    writeByte(file, KILLS_MAJOR_VERSION);
    writeByte(file, KILLS_MINOR_VERSION);

    // How many kill records do we have?
    writeLong(file, kills.size());

    std::map<___monster_desc, kill_def::kill_def, ___monster_desc::less_than>::
        const_iterator kill_iter = kills.begin();

    for ( ; kill_iter != kills.end(); ++kill_iter)
    {
        kill_iter->first.save(file);
        kill_iter->second.save(file);
    }

    // How many ghosts do we have?
    writeShort(file, ghosts.size());
    for (std::vector<kill_ghost>::const_iterator ghost_iter = ghosts.begin();
            ghost_iter != ghosts.end(); ++ghost_iter)
    {
        ghost_iter->save(file);
    }
}

void Kills::load( FILE *file )
{
    unsigned char major = readByte(file),
                  minor = readByte(file);

    if (major != KILLS_MAJOR_VERSION || minor != KILLS_MINOR_VERSION)
        return;

    // How many kill records?
    long kill_count = readLong(file);
    kills.clear();
    for (long i = 0; i < kill_count; ++i)
    {
        ___monster_desc md;

        md.load(file);
        kills[md].load(file);
    }

    short ghost_count = readShort(file);
    ghosts.clear();

    for (short i = 0; i < ghost_count; ++i)
    {
        kill_ghost kg;

        kg.load(file);
        ghosts.push_back(kg);
    }
}

void Kills::record_ghost_kill(const struct monsters *mon)
{
    kill_ghost ghost(mon);
    ghosts.push_back(ghost);
}

kill_def::kill_def( const struct monsters *mon ) : kills(0), exp(0)
{
    exp = exper_value( const_cast<struct monsters *>(mon) );
    add_kill(mon, get_packed_place());
}

static bool ends_with( const std::string &s, const char *suffix )
{
    std::string other = suffix;

    if (s.length() < other.length())
        return (false);

    return (s.substr( s.length() - other.length() ) == other);
}

static bool ends_with(const std::string &s, const char *suffixes[])
{
    if (!suffixes)
        return (false);

    for ( ; *suffixes; suffixes++)
    {
        if (ends_with(s, *suffixes))
            return (true);
    }

    return (false);
}

// For monster names ending with these suffixes, we pluralize directly without
// attempting to use the "of" rule. For instance:
//
//      moth of wrath           => moths of wrath but
//      moth of wrath zombie    => moth of wrath zombies.
//
// This is not necessary right now, since there are currently no monsters that
// require this special treatment (no monster with 'of' in its name is eligible
// for zombies or skeletons).
static const char *modifier_suffixes[] =
{
    "zombie", "skeleton", "simulacrum", NULL,
};

// Pluralizes a monster name. This'll need to be updated for correctness
// whenever new monsters are added.
static std::string pluralize(const std::string &name,
                             const char *no_of[] = NULL)
{
    std::string::size_type pos;

    // Pluralize first word of names like 'eye of draining', but only if the
    // whole name is not suffixed by a modifier, such as 'zombie' or 'skeleton'
    if ( (pos = name.find(" of ")) != std::string::npos
            && !ends_with(name, no_of) )
    {
        return (pluralize(name.substr(0, pos)) + name.substr(pos));
    }
    else if (ends_with(name, "us"))
        // Fungus, ufetubus, for instance.
        return (name.substr(0, name.length() - 2) + "i");
    else if (ends_with(name, "larva") || ends_with(name, "amoeba"))
        // Giant amoebae sounds a little weird, to tell the truth.
        return (name + "e");
    else if (ends_with(name, "ex"))
        // Vortex; vortexes is legal, but the classic plural is cooler.
        return (name.substr(0, name.length() - 2) + "ices");
    else if (ends_with(name, "cyclops"))
        return (name.substr(0, name.length() - 1) + "es");
    else if (ends_with(name, "y"))
        return (name.substr(0, name.length() - 1) + "ies");
    else if (ends_with(name, "lf"))
        // Elf, wolf. Dwarfs can stay dwarfs, if there were dwarfs.
        return (name.substr(0, name.length() - 1) + "ves");
    else if ( ends_with(name, "sheep") || ends_with(name, "manes")
            || ends_with(name, "fish") )
        // Maybe we should generalise 'manes' to ends_with("es")?
        return (name);
    else if (ends_with(name, "ch") || ends_with(name, "sh")
            || ends_with(name, "x"))
        // To handle cockroaches, fish and sphinxes. Fish will be netted by
        // the previous check anyway.
        return (name + "es");
    else if (ends_with(name, "um"))
        // simulacrum -> simulacra
        return (name.substr(0, name.length() - 2) + "a");
    else if (ends_with(name, "efreet"))
        // efreet -> efreeti. Not sure this is correct.
        return (name + "i");
    else if (ends_with(name, "mage"))
        // mage -> magi.
        return (name.substr(0, name.length() - 1) + "i");

    return (name + "s");
}

// Naively prefix A/an to a monster name. At the moment, we don't have monster
// names that demand more sophistication (maybe ynoxinul - don't know how
// that's pronounced).
static std::string article_a(const std::string &name)
{
    if (!name.length())
        return (name);

    switch (name[0])
    {
    case 'a': case 'e': case 'i': case 'o': case 'u':
    case 'A': case 'E': case 'I': case 'O': case 'U':
        return ("An " + name);
    default:
        break;
    }

    return ("A " + name);
}

// For a non-unique monster, prefixes a suitable article if we have only one
// kill, else prefixes a kill count and pluralizes the monster name.
static std::string n_names(const std::string &name, int n)
{
    if (n > 1)
    {
        char buf[20];

        snprintf(buf, sizeof buf, "%d ", n);
        return (buf + pluralize(name, modifier_suffixes));
    }

    return (article_a(name));
}

// Returns a string describing the number of times a unique has been killed.
// Currently required only for Boris.
//
static std::string kill_times(int kills)
{
    char buf[50];

    switch (kills)
    {
    case 1:
        strcpy( buf, " (once)" );
        break;
    case 2:
        strcpy( buf, " (twice)" );
        break;
    case 3:
        strcpy( buf, " (thrice)" );
        break;
    default:
        snprintf( buf, sizeof( buf ), " (%d times)", kills );
        break;
    }

    return (std::string(buf));
}

void kill_def::add_kill(const struct monsters *mon, unsigned short place)
{
    kills++;

    for (unsigned int i = 0; i < places.size(); ++i)
    {
        if (places[i] == place)
            return;
    }

    if (places.size() < PLACE_LIMIT || mons_is_unique(mon->type))
        places.push_back(place);
}

std::string kill_def::base_name(const ___monster_desc &md) const
{
    char monnamebuf[ITEMNAME_SIZE];     // Le sigh.
    moname(md.monnum, true, DESC_PLAIN, monnamebuf);

    std::string name = monnamebuf;
    switch (md.modifier)
    {
    case ___monster_desc::M_ZOMBIE:
        name += " zombie";
        break;
    case ___monster_desc::M_SKELETON:
        name += " skeleton";
        break;
    case ___monster_desc::M_SIMULACRUM:
        name += " simulacrum";
        break;
    case ___monster_desc::M_SPECTRE:
        name = "spectral " + name;
        break;
    default:
        // Silence compiler warning about not handling M_NORMAL and
        // M_SHAPESHIFTER
        break;
    }

    // XXX: I'm not particularly happy with the flavour of these prefixes.
    //      Maybe 'fake rakshasas' should be 'illusory rakshasas', and we
    //      could just drop the 'small' prefix for small abominations?
    switch (md.monnum)
    {
    case MONS_ABOMINATION_LARGE:
        name = "large " + name;
        break;
    case MONS_ABOMINATION_SMALL:
        name = "small " + name;
        break;
    case MONS_RAKSHASA_FAKE:
        name = "fake " + name;
        break;
    }

    return (name);
}

std::string kill_def::info(const ___monster_desc &md) const
{
    std::string name = base_name(md);

    if (!mons_is_unique(md.monnum))
    {
        // Pluralize as needed
        name = n_names(name, kills);

        // We brand shapeshifters with the (shapeshifter) qualifier. This
        // has to be done after doing pluralize(), else we get very odd plurals
        // :)
        if (md.modifier == ___monster_desc::M_SHAPESHIFTER
            && md.monnum != MONS_SHAPESHIFTER
            && md.monnum != MONS_GLOWING_SHAPESHIFTER)
        {
            name += " (shapeshifter)";
        }
    }
    else if (kills > 1)
    {
        // Aha! A resurrected unique
        name += kill_times(kills);
    }

    // What places we killed this type of monster
    return (append_places(md, name));
}

std::string kill_def::append_places(const ___monster_desc &md,
                                const std::string &name) const
{
    if (Options.dump_kill_places == KDO_NO_PLACES)
        return (name);

    int nplaces = places.size();

    if (nplaces == 1 || mons_is_unique(md.monnum)
        || Options.dump_kill_places == KDO_ALL_PLACES)
    {
        std::string augmented = name;

        augmented += " (";
        for (std::vector<unsigned short>::const_iterator iter = places.begin();
                iter != places.end(); ++iter)
        {
            if (iter != places.begin())
                augmented += " ";

            augmented += short_place_name(*iter);
        }

        augmented += ")";
        return (augmented);
    }

    return (name);
}

void kill_def::save(FILE *file) const
{
    writeShort(file, kills);
    writeShort(file, exp);

    writeShort(file, places.size());
    for (std::vector<unsigned short>::const_iterator iter = places.begin();
            iter != places.end(); ++iter)
    {
        writeShort(file, *iter);
    }
}

void kill_def::load(FILE *file)
{
    kills = static_cast<unsigned short>( readShort(file) );
    exp   = readShort(file);

    places.clear();
    short place_count = readShort(file);
    for (short i = 0; i < place_count; ++i)
        places.push_back( static_cast<unsigned short>( readShort(file) ) );
}

kill_ghost::kill_ghost(const struct monsters *mon)
{
    exp = exper_value( const_cast<struct monsters *>( mon ) );
    place = get_packed_place();
    ghost_name = env.ghost.name;

    // Check whether this is really a ghost, since we also have to handle
    // the Pandemonic demons.
    if (mon->type == MONS_PLAYER_GHOST)
        ghost_name = "The ghost of " + ghost_description(true);
}

std::string kill_ghost::info() const
{
    return (ghost_name +
                ((Options.dump_kill_places != KDO_NO_PLACES)
                     ? " (" + short_place_name(place) + ")" : std::string("")));
}

void kill_ghost::save(FILE *file) const
{
    writeString(file, ghost_name);
    writeShort(file, static_cast<unsigned short>( exp ));
    writeShort(file, place);
}

void kill_ghost::load(FILE *file)
{
    ghost_name = readString(file);
    exp = readShort(file);
    place = static_cast<unsigned short>( readShort(file) );
}

___monster_desc::___monster_desc(const monsters *mon)
{
    // TODO: We need to understand how shapeshifters are handled.
    monnum = mon->type;
    modifier = M_NORMAL;
    switch (mon->type)
    {
    case MONS_ZOMBIE_LARGE: case MONS_ZOMBIE_SMALL:
        modifier = M_ZOMBIE;
        break;
    case MONS_SKELETON_LARGE: case MONS_SKELETON_SMALL:
        modifier = M_SKELETON;
        break;
    case MONS_SIMULACRUM_LARGE: case MONS_SIMULACRUM_SMALL:
        modifier = M_SIMULACRUM;
        break;
    case MONS_SPECTRAL_THING:
        modifier = M_SPECTRE;
        break;
    }

    if (modifier != M_NORMAL)
        monnum = mon->number;

    if (mons_is_shapeshifter( const_cast<monsters*>(mon) ))
        modifier = M_SHAPESHIFTER;

    // XXX: Ugly hack - merge all mimics into once mimic record, since they'll
    //      be named simply as "mimic" in the dump, and we don't want multiple
    //      lines saying "mimic". Gold mimics get their own name, so they're
    //      okay.
    if (monnum > MONS_WEAPON_MIMIC && monnum <= MONS_POTION_MIMIC)
        monnum = MONS_WEAPON_MIMIC;
}

void ___monster_desc::save(FILE *file) const
{
    writeShort( file, static_cast<short>( monnum ) );
    writeShort( file, static_cast<short>( modifier ) );
}

void ___monster_desc::load(FILE *file)
{
    monnum = static_cast<int>( readShort(file) );
    modifier = static_cast<name_modifier>( readShort(file) );
}

/*
 *  File:       rumours.cc
 *  Summary:    List of rumours.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"

#include "defines.h"

#include <stdlib.h>

#define NO_RUMOURS 30

char *rumour[] =
{
    "They say Dispater's truename is Kenneth.",
    "They say Ereshkigal's truename is Mabel.",
    "Lose weight fast - drink a potion of decay!",
    "A necrophage corpse is the ultimate slimming food.",
    "A Jelly is by no means a basic opponent.",
    "Too much meat is bad for your blood pressure.",
    "Are you getting all your vitamins?",
    "Giant ants are just protecting their Queen.",
    "Killer bees are just protecting their Queen.",
    "Trog's temper is legendary.",
// 10
    "The Scythe of Curses is best used in small doses.",
    "An orb of fixation contains the meaning of life, if you just look hard enough.",
    "An orb of fixation is just a poorly-tuned orb of seeing.",
  "The Singing Sword provides one with a constant source of entertainment.",
    "Giant slugs are easy to kill, if you can just find a large enough salt shaker.",
    "They say that Jellies reproduce by a process of binary fission.",
    "A Necromancer with a wand of polymorph can make powerful friends.",
    "Summoners are the laziest people around.",
    "A deck of wonders is a truly wonderful find.",
    "What goes down rarely comes back up again.",
// 20
    "Life is like a kobold - nasty, brutish and short.",
    "Isn't it great, living in the State of Nature?",
    "Beware of the dungeon's notorious nudist.",
    "Zapping a wand of lightning in an enclosed space may rebound on you.",
    "Not all plants are harmless scenery.",
    "They say nobody has ever seen a wandering mushroom move.",
    "They say giant snails live in giant snailshells.",
    "Passing through Death's Door is usually a one-way trip.",
    "Mummies make great kindling.",
    "Paralysis can be almost instantly lethal."
// 30

};


char *rumours(void);
char *rumours(void)
{
    return rumour[random() % NO_RUMOURS];
}

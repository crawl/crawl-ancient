#ifndef FILES_H
#define FILES_H
/*
 *  File:       files.cc
 *  Summary:    Functions used to save and load levels/games.
 *  Written by: Linley Henzell and Alexey Guzeev
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#define MAX_LEVELS      50
#define MAX_DUNGEONS    30

extern bool tmp_file_pairs[MAX_LEVELS][MAX_DUNGEONS];

void load(unsigned char stair_taken, char moving_level, char was_a_labyrinth, char old_level, char want_followers, char just_made_new_lev, char where_were_you2);
void save_level(int level_saved, char was_a_labyrinth, char where_were_you);
void save_game(char leave_game);
void restore_game();
void save_ghost();

#endif

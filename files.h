#ifndef __DCRAWL__FILES_H__
#define __DCRAWL__FILES_H__


void load (unsigned char stair_taken, char moving_level, char was_a_labyrinth, char old_level, char want_followers, char just_made_new_lev, char where_were_you2);
void save_level (int level_saved, char was_a_labyrinth, char where_were_you);
void save_game (char leave_game);
void restore_game ();
void save_ghost ();


#endif //!__DCRAWL__FILES_H__

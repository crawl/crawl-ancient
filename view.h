/*
 *  File:       view.cc
 *  Summary:    Misc function used to render the dungeon.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */
#ifndef VIEW_H
#define VIEW_H

#include "externs.h"

void moname(int mcl, char mench, char see_inv, char descrip, char glog[40]);

void item();
void monster_grid();
void noisy(char loudness, char nois_x, char nois_y);

int check_awaken(int mons_aw);

void viewwindow2(char draw_it, bool do_updates);
void viewwindow3(char draw_it, bool do_updates); /* non-IBM graphics */
void magic_mapping(int map_radius, int proportion);
void show_map(int spec_place[2]);
bool mons_near(unsigned char monst);
void losight(unsigned int sh[19][19], unsigned char gr[80][70], int x_p, int y_p);
void draw_border(int bord_col, char your_name[kNameLen], char class_name[40], char tspecies);
#endif

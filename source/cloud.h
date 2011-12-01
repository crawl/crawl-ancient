/*
 *  File:       cloud.h
 *  Summary:    Functions related to clouds.
 *  Written by: Brent Ross
 *
 *  Change History (most recent first):
 *
 *     <1>    Oct 1/2001      BWR             Created
 */


#ifndef CLOUD_H
#define CLOUD_H

#include "externs.h"

bool is_cloud( int x, int y );
int get_cloud_type( int x, int y );

void delete_cloud( int cloud );
void move_cloud( int cloud, int new_x, int new_y );

void place_cloud( int cl_type, int ctarget_x, int ctarget_y, int cl_range,
                  bool affect_grid = true );

#endif

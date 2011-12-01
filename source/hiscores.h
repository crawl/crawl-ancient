/*
 *  File:       hiscores.h
 *  Summary:    Scorefile manipulation functions
 *  Written by: Gordon Lipford
 *
 *  Change History (most recent first):
 *
 *     <1>     16feb2001     GDL     Created
 */


#ifndef HISCORES_H
#define HISCORES_H

void hiscores_new_entry( struct scorefile_entry &se );
void hiscores_print_list( int display_count = -1, int format = SCORE_TERSE );

void hiscores_format_single( char *buffer, struct scorefile_entry &se );
int  hiscores_format_single_long( char *buffer, struct scorefile_entry &se,
                                  bool verbose = false );

long calc_skill_bonus( long skill_points );

#endif  // HISCORES_H

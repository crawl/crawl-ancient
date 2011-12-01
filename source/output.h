/*
 *  File:       output.cc
 *  Summary:    Functions used to print player related info.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#ifndef OUTPUT_H
#define OUTPUT_H

void print_stats(void);
void draw_border(void);

// Arg! Even the prototype was outside the sentrys... this really is the
// ugliest piece of code in Crawl in a while.
void get_full_detail(char* buffer, bool calc_unid);

#endif

/*
 *  File:       transfor.cc
 *  Summary:    Misc function related to player transformations.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */
#ifndef TRANSFOR_H
#define TRANSFOR_H

void untransform(void);
char can_equip(char use_which);
char transform(int pow, char which_trans);
int remove_equipment(char remove_stuff[8]);
#endif

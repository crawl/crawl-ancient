#include "config.h"

#ifdef DOS
#include <conio.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "direct.h"
#include "externs.h"
#include "itemname.h"
#include "player.h"
#include "shopping.h"
#include "dungeon.h"
#include "stuff.h"
#include "monplace.h"
#include "mstruct.h"
#include "spell.h"

#define DEBUG

/*
Some debugging functions, accessable through keys like %, $, &, ) etc when
a section of code in input() in acr.cc is uncommented.
*/


#ifdef DEBUG
// Debug:
void cast_spec_spell(void)
{

        char specs [3];

        strcpy(info, "Cast which spell? ");
        mpr(info);

        specs [0] = getche();
        specs [1] = getche();
        specs [2] = getche();

        your_spells(atoi(specs), magic_ability(player_mag_abil(), you[0].intel), 0);

}
#endif


#ifdef DEBUG
void create_spec_monster(void)
{

        char specs [3];

        strcpy(info, "Create which monster? ");
        mpr(info);

        specs [0] = getche();
        specs [1] = getche();
        specs [2] = getche();


        create_monster(atoi(specs), 0, 0, you[0].x_pos, you[0].y_pos, MHITNOT, 250);

}

void level_travel(void)
{

        char specs [3];

        strcpy(info, "Travel to which level? ");
        mpr(info);

        specs [0] = getche();
        specs [1] = getche();
        specs [2] = 0;

        you[0].your_level = atoi(specs);

}

// Debug:
void create_spec_object(void)
{

        int class_wanted = 0;
        int type_wanted = 0;

        strcpy(info, "Create which item (class then type)? ");
        mpr(info);

        class_wanted = (getche() - 48) * 10;
        class_wanted += getche() - 48;

        type_wanted = (getche() - 48) * 10;
        type_wanted += getche() - 48;

        itoa(property(class_wanted, type_wanted, 2), st_prn, 10);
        strcpy(info, st_prn);
        mpr(info);

        int thing_created = items(1, class_wanted, type_wanted, 1, you[0].your_level, 250);

        if (you[0].species != 13) strcpy(info, "Something appears at your feet!");
        else strcpy(info, "Something appears before you!");
        mpr(info);

        int what_was_there = igrd [you[0].x_pos] [you[0].y_pos];
        mitm.ilink [thing_created] = what_was_there;
        igrd [you[0].x_pos] [you[0].y_pos] = thing_created;
        //mitm.idam [thing_created] = 13;

}

void create_spec_object2(void)
{

        int class_wanted = 0;
        int type_wanted = 0;
        int dam_wanted = 0;

        strcpy(info, "Create which item (class, type, then dam)? ");
        mpr(info);

        class_wanted = (getche() - 48) * 10;
        class_wanted += getche() - 48;

        type_wanted = (getche() - 48) * 10;
        type_wanted += getche() - 48;

        dam_wanted = (getche() - 48) * 10;
        dam_wanted += getche() - 48;

//itoa(property [class_wanted] [type_wanted] [2], st_prn, 10);
//strcpy(info, st_prn);
//mpr(info);

//int thing_created = items(you[0].unique_items, 1, you[0].item_description,
//       grd, class_wanted, type_wanted, 1, 100, 250);

        int thing_created = items(1, class_wanted, type_wanted, 1, you[0].your_level, 250);


        mitm.idam [thing_created] = dam_wanted;

        if (you[0].species != 13) strcpy(info, "Something appears at your feet!");
        else strcpy(info, "Something appears before you!");
        mpr(info);

        int what_was_there = igrd [you[0].x_pos] [you[0].y_pos];
        mitm.ilink [thing_created] = what_was_there;
        igrd [you[0].x_pos] [you[0].y_pos] = thing_created;
        //mitm.idam [thing_created] = 13;

}


#endif



void stethoscope(int mwh)
{
// STETH can't examine spaces in cardinal directions more than 1 space from you

        struct dist stth [1];
        int i;

        if (mwh == 250)
        {

                strcpy(info, "Which monster?");
                mpr(info);

                direction(1, stth);

                if (stth[0].nothing == -1) return;

                if (env[0].cgrid [you[0].x_pos + stth[0].move_x] [you[0].y_pos + stth[0].move_y] != CNG)
                {
                        itoa(env[0].cloud_type [env[0].cgrid [you[0].x_pos + stth[0].move_x] [you[0].y_pos + stth[0].move_y]], st_prn, 10);
                        strcpy(info, st_prn);
                        strcat(info, "/");
                        itoa(env[0].cloud_decay [env[0].cgrid [you[0].x_pos + stth[0].move_x] [you[0].y_pos + stth[0].move_y]], st_prn, 10);
                        strcat(info, st_prn);
                        strcat(info, "   ");
                        mpr(info);
                }

                if (mgrd [you[0].x_pos + stth[0].move_x] [you[0].y_pos + stth[0].move_y] == MNG)
                {
        //      strcpy(info, "There isn't anything there.!");

                        itoa((int) igrd [you[0].x_pos + stth[0].move_x] [you[0].y_pos + stth[0].move_y], st_prn, 10);
                        strcpy(info, st_prn);
//strcat(info, "/");
//itoa(menv [i].m_hp, st_prn, 10);
//strcpy(info, st_prn);
                        mpr(info);

        //mpr(info);
                        stth[0].move_x = 0;
                        stth[0].move_y = 0;
                        return;
                }

                i = mgrd [you[0].x_pos + stth[0].move_x] [you[0].y_pos + stth[0].move_y];

        } else i = mwh;

        strcpy(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 0)); //gmon_name [mons_class [i]]);
        strcat(info, ": ");

        {

                itoa(i, st_prn, 10);
                strcat(info, st_prn);
                strcat(info, ": cl ");

                itoa(menv [i].m_class, st_prn, 10);
                strcat(info, st_prn);
                strcat(info, " - ");

                itoa(menv [i].m_hp, st_prn, 10);
                strcat(info, st_prn);
                strcat(info, "/");
                itoa(menv [i].m_hp_max, st_prn, 10);
                strcat(info, st_prn);
                strcat(info, "   ");

                itoa(menv [i].m_beh, st_prn, 10);
                strcat(info, st_prn);
                strcat(info, "/");
                itoa(menv [i].m_hit, st_prn, 10);
                strcat(info, st_prn);
                mpr(info);

                strcpy(info, "speed:");
                itoa(menv [i].m_speed, st_prn, 10);
                strcat(info, st_prn);
                strcat(info, "/");
                itoa(menv [i].m_speed_inc, st_prn, 10);
                strcat(info, st_prn);
                mpr(info);


                strcpy(info, "sec:");
                itoa(menv [i].m_sec, st_prn, 10);
                strcat(info, st_prn);
                mpr(info);

                strcpy(info, "target: ");
                itoa(menv [i].m_targ_1_x, st_prn, 10);
                strcat(info, st_prn);
                strcat(info, ", ");
                itoa(menv [i].m_targ_1_y, st_prn, 10);
                strcat(info, st_prn);
                mpr(info);

                strcpy(info, "ench0: ");
                itoa(menv [i].m_ench [0], st_prn, 10);
                strcat(info, st_prn);
                strcat(info, " ench1: ");
                itoa(menv [i].m_ench [1], st_prn, 10);
                strcat(info, st_prn);
                strcat(info, " ench2: ");
                itoa(menv [i].m_ench [2], st_prn, 10);
                strcat(info, st_prn);
                mpr(info);

                strcpy(info, "Ghost damage: ");
                itoa(ghost.ghs [7], st_prn, 10);
                strcat(info, st_prn);
                mpr(info);

        }
        stth[0].move_x = 0;
        stth[0].move_y = 0;

        return;

} // end of stethoscope()

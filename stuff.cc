/*
 *  File:       stuff.cc
 *  Summary:    Misc stuff.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <2>     9/25/99        CDL             linuxlib -> liblinux
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "stuff.h"

#ifdef DOS
#include <conio.h>
#endif

#ifdef LINUX
#include "liblinux.h"
#endif

#include "externs.h"
#include "enum.h"
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "misc.h"
#include "output.h"
#include "view.h"
#include "skills2.h"

#ifdef MACROS
  #include "macro.h"
#endif

extern char wield_change;

int random2(unsigned int randmax)
{
    if (randmax <= 0)
        return 0;

    return random() % randmax;
}

unsigned char get_ch()
{
    unsigned char gotched = getch();

    if (gotched == 0)
        gotched = getch();
    return gotched;
}



char see_grid(unsigned char grx, unsigned char gry)
{

    if (grx > you.x_pos - 9 && grx < you.x_pos + 9 && gry > you.y_pos - 9 && gry < you.y_pos + 9)
    {
        if (env.show[grx - you.x_pos + 9][gry - you.y_pos + 9] != 0)
            return 1;
    }

    return 0;

}                               // end of char mons_near(char)


int magic_ability(int mag_abil, int intel)
{

    int retv = mag_abil * intel;

    retv /= 10;


    return retv;

}

void end(int end_arg)
{
#ifdef LINUX
    lincurses_shutdown();
#endif

#ifdef MAC
    deinit_mac();
#endif

    exit(end_arg);
}

void output_value(char string[100], int value)
{
    strcpy(info, string);
    itoa(value, st_prn, 10);
    strcat(info, st_prn);
    mpr(info);
}

#ifdef PLAIN_TERM
// this function is used for systems without gettext/puttext to redraw the
// playing screen after a call to for example inventory.
void redraw_screen(void)
{
    char title[40];

    strcpy(title, skill_title(best_skill(0, 50, 99), you.skills[best_skill(0, 50, 99)], you.char_class, you.experience_level));
    draw_border(you.your_name, title, you.species);

    you.redraw_hit_points = 1;
    you.redraw_magic_points = 1;
    you.redraw_strength = 1;
    you.redraw_intelligence = 1;
    you.redraw_dexterity = 1;
    you.redraw_armor_class = 1;
    you.redraw_evasion = 1;
    you.redraw_gold = 1;
    you.redraw_experience = 1;
    you.redraw_hunger = 1;
    you.redraw_burden = 1;
    wield_change = 1;

    print_stats();

    new_level();

    viewwindow(1, false);


}

#endif


#include <string.h>

#include "externs.h"
#include "ouch.h"
#include "message.h"
#include "stuff.h"
#include "items.h"
#include "itemname.h"
#include "invent.h"
#include "spells0.h"

void adjust_item(void);
void adjust_spells(void);


#ifdef DEBUG

void stethoscope(int mwh)
{
// STETH can't examine spaces in cardinal directions more than 1 space from you

int i, nothing;

if (mwh == 250)
{

strcpy(info, "Which monster?");
mpr(info);

struct dist stmove [1];

direction(1, stmove);

if (nothing == -1) return;

if (env[0].cgrid [you[0].x_pos + stmove[0].move_x] [you[0].y_pos + stmove[0].move_y] != CNG)
{
 itoa(env[0].cloud_type [env[0].cgrid [you[0].x_pos + stmove[0].move_x] [you[0].y_pos + stmove[0].move_y]], st_prn, 10);
 strcpy(info, st_prn);
 strcat(info, "/");
 itoa(env[0].cloud_decay [env[0].cgrid [you[0].x_pos + stmove[0].move_x] [you[0].y_pos + stmove[0].move_y]], st_prn, 10);
 strcat(info, st_prn);
 strcat(info, "   ");
 mpr(info);
}

if (mgrd [you[0].x_pos + stmove[0].move_x] [you[0].y_pos + stmove[0].move_y] == MNG)
{

itoa((int) igrd [you[0].x_pos + stmove[0].move_x] [you[0].y_pos + stmove[0].move_y], st_prn, 10);
strcpy(info, st_prn);
mpr(info);


/*      move_x = 0;
        move_y = 0;*/
        return;
}

i = mgrd [you[0].x_pos + stmove[0].move_x] [you[0].y_pos + stmove[0].move_y];

} else i = mwh;

strcpy(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 0)); //gmon_name [mons_class [i]]);
strcat(info, ": ");

{
/*
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
*/
//if (menv [i].m_class == 400)
//{
 strcpy(info, "Ghost damage: ");
 itoa(ghost.ghs [7], st_prn, 10);
 strcat(info, st_prn);
 mpr(info);
//}

}
stmove[0].move_x = 0;
stmove[0].move_y = 0;

return;

} // end of stethoscope()


#endif

void quit_game(void)
{

strcpy(info, "Really quit?");
mpr(info);

char keyin = get_ch();

if (keyin == 'y' | keyin == 'Y')
{
ouch(-9999, 0, 13);

}

} // end of void quit_game




void version(void)
{
 strcpy(info, "This is Dungeon Crawl v2.82. (Last build 24/10/98)");
 mpr(info);
}


void adjust(void)
{

strcpy(info, "Adjust (i)tems or (s)pells?");
mpr(info);

unsigned char keyin = get_ch();

if (keyin == 'i' | keyin == 'I')
{
 adjust_item();
 return;
}
if (keyin == 's' | keyin == 'S')
{
 adjust_spells();
 return;
}

mpr("Huh?");
return;

}



void adjust_item(void)
{

unsigned char throw_2, throw_3, nthing;

if (you[0].inv_no == 0)
        {
        strcpy(info, "You aren't carrying anything.");
        mpr(info);
        return;
        }

query : strcpy(info, "Adjust which item?");
mpr(info);

unsigned char keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '*' | keyin == '?') nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
    mesclr();
    goto query;
        }
}

int throw_1 = (int) keyin;

if (throw_1 < 65 | (throw_1 > 90 && throw_1 < 97) | throw_1 > 122)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

throw_2 = conv_lett(throw_1);

if (you[0].inv_quant [throw_2] == 0)
{
        strcpy(info, "You don't have any such object.");
        mpr(info);
        return;
}

strcpy(info, " ");
   if (throw_2 <= 25) info [0] = throw_2 + 97;
        else info [0] = throw_2 + 39;

info [1] = 0; /* This null-terminates it, right? */
strcat(info, " - ");

in_name(throw_2, 3, str_pass);
strcat(info, str_pass);
mpr(info);

strcpy(info, "Adjust to which letter?");
mpr(info);

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '*' | keyin == '?') nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
    mesclr();
    goto query;
        }
}

throw_1 = (int) keyin;

if (throw_1 < 65 | (throw_1 > 90 && throw_1 < 97) | throw_1 > 122)
{
        strcpy(info, "What?");
        mpr(info);
        return;
}

throw_3 = conv_lett(throw_1);

if (you[0].inv_quant [throw_3] != 0)
{
        strcpy(info, "Sorry, that inventory letter is already in use.");
        mpr(info);
        return;
}

you[0].inv_ident [throw_3] = you[0].inv_ident [throw_2];
you[0].inv_class [throw_3] = you[0].inv_class [throw_2];
you[0].inv_type [throw_3] = you[0].inv_type [throw_2];
you[0].inv_plus [throw_3] = you[0].inv_plus [throw_2];
you[0].inv_plus2 [throw_3] = you[0].inv_plus2 [throw_2];
you[0].inv_dam [throw_3] = you[0].inv_dam [throw_2];
you[0].inv_col [throw_3] = you[0].inv_col [throw_2];
you[0].inv_quant [throw_3] = you[0].inv_quant [throw_2];

int i = 0;

for (i = 0; i < 10; i ++)
{
 if (you[0].equip [i] == throw_2) you[0].equip [i] = throw_3;
}

you[0].inv_quant [throw_2] = 0;

strcpy(info, " ");
   if (throw_3 <= 25) info [0] = throw_3 + 97;
        else info [0] = throw_3 + 39;

info [1] = 0; /* This null-terminates it, right? */
strcat(info, " - ");

in_name(throw_3, 3, str_pass);
strcat(info, str_pass);
mpr(info);


}





void adjust_spells(void)
{

unsigned char throw_2, throw_3, nthing;

if (you[0].spell_no == 0)
        {
        strcpy(info, "You don't know any spells.");
        mpr(info);
        return;
        }

query : strcpy(info, "Adjust which spell?");
mpr(info);

unsigned char keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '*' | keyin == '?') nthing = spell_list();
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
    mesclr();
    goto query;
        }
}

int throw_1 = (int) keyin;

if (throw_1 < 97 | throw_1 > 119)
{
        strcpy(info, "You don't know that spell.");
        mpr(info);
        return;
}

throw_2 = conv_lett(throw_1);

if (you[0].spells [throw_2] == 210)
{
        strcpy(info, "You don't know that spell.");
        mpr(info);
        return;
}

strcpy(info, " ");
info [0] = throw_2 + 97;
info [1] = 0; /* This null-terminates it, right? */
strcat(info, " - ");
spell_name(you[0].spells [throw_2], str_pass);
strcat(info, str_pass);
mpr(info);

strcpy(info, "Adjust to which letter?");
mpr(info);

keyin = get_ch();

if (keyin == '?' | keyin == '*')
{
        if (keyin == '*' | keyin == '?') nthing = get_invent(-1);
        if ((nthing >= 65 && nthing <= 90) | (nthing >= 97 && nthing <= 122))
        {
                keyin = nthing;
        } else
        {
    mesclr();
    goto query;
        }
}

throw_1 = (int) keyin;

//if (throw_1 < 97 | throw_1 > 122)
if (throw_1 < 97 | throw_1 > 118)
{
        strcpy(info, "What?");
        mpr(info);
        return;
}

throw_3 = conv_lett(throw_1);

int backup = you[0].spells [throw_3];
you[0].spells [throw_3] = you[0].spells [throw_2];
you[0].spells [throw_2] = backup;

strcpy(info, " ");
info [0] = throw_3 + 97;
info [1] = 0; /* This null-terminates it, right? */
strcat(info, " - ");
spell_name(you[0].spells [throw_3], str_pass);
strcat(info, str_pass);
mpr(info);

if (you[0].spells [throw_2] != 210)
{
 strcpy(info, " ");
 info [0] = throw_2 + 97;
 info [1] = 0; /* This null-terminates it, right? */
 strcat(info, " - ");
 spell_name(you[0].spells [throw_2], str_pass);
 strcat(info, str_pass);
 mpr(info);
}

}

#include "config.h"

#ifdef DOS
#include <conio.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "externs.h"

#include "view.h"
#include "itemname.h"
#include "mstruct.h"
#include "describe.h"
#include "player.h"
#include "monstuff.h"
#include "stuff.h"

/* direction module */
int dir_cursor(char rng);
int look_around(struct dist moves [1]);


void direction(char rnge, struct dist moves [1])
{

char ink = 0;
char looked = 0;
moves[0].nothing = dir_cursor(rnge);
char info [200];

if (moves[0].nothing == -9999)
{
 moves[0].nothing = look_around(moves);
 looked = 1;
}

if (moves[0].nothing > 50000)
{
        moves[0].nothing -= 50000;
}

if (moves[0].nothing > 10000)
{
        moves[0].nothing -= 10000;
        ink = 1;
}

if (moves[0].nothing == -1)
{
        strcpy(info, "What an unusual direction.");
        mpr(info);
        return;
}

if (moves[0].nothing == 253)
{
        if (you[0].prev_targ == 500)
        {
                strcpy(info, "You haven't got a target.");
                mpr(info);
                moves[0].nothing = -1;
                return;
        }

        if (mons_near(you[0].prev_targ) != 1 | (menv [you[0].prev_targ].m_ench [2] == 6 && player_see_invis() == 0))
        {
                strcpy(info, "You can't see that creature any more.");
                mpr(info);
                moves[0].nothing = -1;
                return;
        }

        moves[0].move_x = menv [you[0].prev_targ].m_x - you[0].x_pos;
        moves[0].move_y = menv [you[0].prev_targ].m_y - you[0].y_pos;
        moves[0].target_x = moves[0].move_x + you[0].x_pos;
        moves[0].target_y = moves[0].move_y + you[0].y_pos;
        return;
}


if (moves[0].nothing == 254)
{
        return;
}

if (looked == 0)
{
 moves[0].move_x = ((int) (moves[0].nothing / 100)) - 7;
 moves[0].move_y = moves[0].nothing - (((int) moves[0].nothing / 100) * 100) - 7;
}

/*target_x = move_x + you[0].x_pos;
target_y = move_y + you[0].y_pos;*/

if (ink == 1)
{
        moves[0].target_x = 1;
        moves[0].target_y = 1;
}


if (moves[0].move_x == 0)
{
        if (moves[0].move_y > 0) moves[0].move_y = 1;
        if (moves[0].move_y < 0) moves[0].move_y = -1;
}

if (moves[0].move_y == 0)
{
        if (moves[0].move_x > 0) moves[0].move_x = 1;
        if (moves[0].move_x < 0) moves[0].move_x = -1;
}

if (moves[0].move_x == moves[0].move_y | moves[0].move_x * -1 == moves[0].move_y)
{
        if (moves[0].move_y > 0) moves[0].move_y = 1;
        if (moves[0].move_y < 0) moves[0].move_y = -1;
        if (moves[0].move_x > 0) moves[0].move_x = 1;
        if (moves[0].move_x < 0) moves[0].move_x = -1;
}

        if (env[0].mgrid [moves[0].target_x] [moves[0].target_y] != MNG)
        {

                        if (rnge != 100)
                        {
                                you[0].prev_targ = env[0].mgrid [moves[0].target_x] [moves[0].target_y];
                        }
        }

} // end of direction()



int dir_cursor(char rng)
{
        char mve_x, mve_y;
        char bk = 0;
        char keyy;

if (rng == 100) return -9999;

        keyy = getch();

        if (keyy != 0 && keyy != '*' && keyy != '.')
        {
  switch(keyy)
  {
                case 'b': mve_x = -1; mve_y = 1; break;
                case 'j': mve_y = 1; mve_x = 0; break;
                case 'u': mve_x = 1; mve_y = -1; break;
                case 'k': mve_y = -1; mve_x = 0; break;
                case 'y': mve_y = -1; mve_x = -1; break;
                case 'h': mve_x = -1; mve_y = 0; break;
                case 'n': mve_y = 1; mve_x = 1; break;
                case 'l': mve_x = 1; mve_y = 0; break;
                case 'p': return 253;
                default: return -1;
  }
               return mve_x * 100 + mve_y + 707 + 10000;

        }

        if (keyy != '*' && keyy != '.') keyy = getch();

        switch(keyy)
        {
                case 'O': mve_x = -1; mve_y = 1; bk = 1; break;
                case 'P': mve_y = 1; mve_x = 0;bk = 1; break;
                case 'I': mve_x = 1; mve_y = -1;bk = 1; break;
                case 'H': mve_y = -1; mve_x = 0;bk = 1; break;
                case 'G': mve_y = -1; mve_x = -1;bk = 1; break;
                case 'K': mve_x = -1; mve_y = 0;bk = 1; break;
                case 'Q': mve_y = 1; mve_x = 1; bk = 1; break;
                case 'M': mve_x = 1; mve_y = 0;bk = 1; break;
  case '.':
                case 'S': bk = 1; mve_x = 0; mve_y = 0; break;
                case '*': bk = 0; break; // was 'L'

                default: return -1;
        }

        if (bk == 1)
        {
               return mve_x * 100 + mve_y + 707 + 10000;
        }

        if (rng == 0) return 254;

        return -9999;

}




int look_around(struct dist moves [1])
{

        int xps = 17;
 int yps = 9;
        char gotch;
        char mve_x, mve_y;

 strcpy(info, "Move the cursor around to observe a square.");
 mpr(info);

 strcpy(info, "Press '?' for a monster description.");
 mpr(info);

        gotoxy(xps,yps);
        gotoxy(xps + 1,yps);

//unsigned int show [19] [19];

losight(env[0].show, grd, you[0].x_pos, you[0].y_pos);


do
{

        gotch = getch();

        if (gotch != 0 && gotch != 13)
        {
  switch(gotch)
  {
                case '.': mve_x = 0; mve_y = 0; gotch = 'S'; goto thingy;
                case 'b': mve_x = -1; mve_y = 1; break;
                case 'j': mve_y = 1; mve_x = 0; break;
                case 'u': mve_x = 1; mve_y = -1; break;
                case 'k': mve_y = -1; mve_x = 0; break;
                case 'y': mve_y = -1; mve_x = -1; break;
                case 'h': mve_x = -1; mve_y = 0; break;
                case 'n': mve_y = 1; mve_x = 1; break;
                case 'l': mve_x = 1; mve_y = 0; break;
                case '?': mve_x = 0; mve_y = 0;
                if (mgrd [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 9] == MNG) continue;
                if (menv[mgrd [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 9]].m_ench [2] == 6 && player_see_invis() == 0) continue;
                if (menv[mgrd [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 9]].m_class >= MLAVA0 && menv[mgrd [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 9]].m_sec == 1) continue;
                describe_monsters(menv[mgrd [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 9]].m_class);
#ifdef LINUX
redraw_screen();
#endif
                break;
                case 'p':
                goto finished_looking;
                case '>':
                goto finished_looking;
    default:    return - 1;
  }
 goto gotchy;
 }

        if (gotch != 13) gotch = getch();

        mve_x = 0; mve_y = 0;

        thingy : switch(gotch)
        {
                case 13: gotch = 'S'; break;
                case 'O': mve_x = -1; mve_y = 1; break;
                case 'P': mve_y = 1; mve_x = 0; break;
                case 'I': mve_x = 1; mve_y = -1; break;
                case 'H': mve_y = -1; mve_x = 0; break;
                case 'G': mve_y = -1; mve_x = -1; break;
                case 'K': mve_x = -1; mve_y = 0; break;
                case 'Q': mve_y = 1; mve_x = 1; break;
                case 'M': mve_x = 1; mve_y = 0; break;
                case 'S': break;
                // need <, > etc

                default: return - 1;
        }

gotchy : gotoxy(xps,yps);

if (xps + mve_x >= 9 && xps + mve_x < 26) xps += mve_x;
if (yps + mve_y >= 1 && yps + mve_y < 18) yps += mve_y;

mesclr();

if (env[0].show [xps - 8] [yps] == 0 && (xps != 17 | yps != 9))
{
 strcpy(info, "You can't see that place.");
 mpr(info);
/* itoa(see_grid(you[0].x_pos + xps - 17, you[0].y_pos + yps - 9), st_prn, 10);
 strcat(info, st_prn);
 mpr(info);*/
 goto glogokh;
}


if (mgrd [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 9] != MNG)
{

int i = mgrd [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 9];

if (menv [i].m_ench [2] == 6 && player_see_invis() == 0) goto look_clouds;

        int mmov_x = menv [i].m_inv [0];

        if (menv [i].m_class == 144)
        {
         item_name(mitm.iplus2 [mmov_x], mitm.iclass [mmov_x], mitm.itype [mmov_x], mitm.idam [mmov_x], mitm.iplus [mmov_x], mitm.iquant [mmov_x], mitm.iid [mmov_x], 2, str_pass);
         strcpy(info, str_pass);
        } else strcpy(info, monam (menv [i].m_sec, menv [i].m_class, menv [i].m_ench [2], 2));
strcat(info, ".");
mpr(info);

 print_wounds(i);


if (menv [i].m_class == 106)
{
 strcpy(info, "It has ");
 itoa(menv [i].m_sec, st_prn, 10);
 strcat(info, st_prn);
 strcat(info, " heads.");
 mpr(info);
}

if (menv [i].m_beh == 7)
{
 strcpy(info, "It is friendly.");
 mpr(info);
}
if (menv [i].m_beh == 0)
{
 strcpy(info, "It doesn't appear to have noticed you.");
 mpr(info);
}

#ifdef DEBUG
stethoscope(i);
#endif

}

look_clouds:
if (env[0].cgrid [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 9] != CNG)
{
 switch(env[0].cloud_type [env[0].cgrid [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 9]] % 100)
 {
  case 1: strcpy(info, "There is a cloud of flame here."); break;
  case 2: strcpy(info, "There is a cloud of noxious fumes here."); break;
  case 3: strcpy(info, "There is a cloud of freezing vapour here."); break;
  case 4: strcpy(info, "There is a cloud of poison gas here."); break;
  case 5: strcpy(info, "There is a cloud of smoke here."); break;
  case 6: strcpy(info, "There is a cloud of blue smoke here."); break;
  case 7: strcpy(info, "There is a cloud of purple smoke here."); break;
  case 8: strcpy(info, "There is a cloud of steam here."); break;
  case 9: strcpy(info, "There is an evil black miasma here."); break;
 }
mpr(info);
} // end of look_clouds:

if (igrd [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 9] != 501)
{
 if (mitm.ilink [igrd [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 9]] != 501 | mitm.iquant [igrd [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 7]] > 1)
 {
  strcpy(info, "You see a pile of items here.");
 } else strcpy(info, "You see an item here.");

 mpr(info);
}

switch(grd [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 9])
{
  case 2: strcpy(info, "A stone wall."); break;
  case 1:
  case 5: // secret door
  if (you[0].level_type == 3)
  {
   strcpy(info, "A wall of the weird stuff which makes up Pandemonium.");
  } else strcpy(info, "A rock wall.");
  break;
  case 3: strcpy(info, "A closed door.");
  break;
  case 4: strcpy(info, "A metal wall.");
  break;
  case 6: strcpy(info, "A wall of green crystal.");
  break;
  case 7: strcpy(info, "An orcish idol.");
  break;
  case 8: strcpy(info, "A wall of solid wax.");
  break;
  case 61: strcpy(info, "Some lava.");
  break;
  case 62: strcpy(info, "Some water.");
  break;
  case 78: // undiscovered trap
  case 67: strcpy(info, "Floor.");
  break;
  case 70: strcpy(info, "An open door.");
  break;

  case 85:
  strcpy(info, "A ladder leading down.");
  break;
  case 82:
  case 83:
  case 84:
  strcpy(info, "A staircase leading down.");
  break;
  case 89:
  strcpy(info, "A ladder leading upwards.");
  break;
  case 86:
  case 87:
  case 88:
  strcpy(info, "A staircase leading up.");
  break;
  case 69: strcpy(info, "A gateway to hell.");
  break;
  case 71: strcpy(info, "A staircase to a branch level.");
  break;
  case 75: strcpy(info, "A trap.");
  break;
  case 76: strcpy(info, "A magical trap.");
  break;
  case 77: strcpy(info, "A trap.");
  break;
  case 80: strcpy(info, "A shop.");
  break;
  case 81: strcpy(info, "A labyrinth entrance.");
  break;
  case 92: strcpy(info, "A gateway to the Iron City of Dis.");
  break;
  case 93: strcpy(info, "A gateway to Gehenna.");
  break;
  case 94: strcpy(info, "A gateway to the freezing wastes of Cocytus.");
  break;
  case 95: strcpy(info, "A gateway to the decaying netherworld of Tartarus.");
  break;
  case 96: strcpy(info, "A gateway to the infinite Abyss.");
  break;
  case 97: strcpy(info, "A gateway leading out of the Abyss.");
  break;
  case 98: strcpy(info, "An empty arch of ancient stone.");
  break;
  case 99: strcpy(info, "A gate leading to the halls of Pandemonium.");
  break;
  case 100: strcpy(info, "A gate leading out of Pandemonium.");
  break;
  case 101: strcpy(info, "A gate leading to another region of Pandemonium.");
  break;

  case 110: strcpy(info, "A staircase to the Orcish Mines.");
  break;
  case 111: strcpy(info, "A staircase to the Hive.");
  break;
  case 112: strcpy(info, "A staircase to the Lair.");
  break;
  case 113: strcpy(info, "A staircase to the Slime Pits.");
  break;
  case 114: strcpy(info, "A staircase to the Vaults.");
  break;
  case 115: strcpy(info, "A staircase to the Crypt.");
  break;
  case 116: strcpy(info, "A staircase to the Hall of Blades.");
  break;
  case 117: strcpy(info, "A staircase to the Hall of Zot.");
  break;

  case 130:
  case 131:
  case 132:
  case 134:
  case 137:
  strcpy(info, "A staircase back to the Dungeon.");
  break;

  case 133:
  strcpy(info, "A staircase back to the Lair.");
  break;

  case 135:
  case 136:
  strcpy(info, "A staircase back to the Vaults.");
  break;

}
mpr(info);

glogokh:        // test relay_message();

itoa((int) grd [you[0].x_pos + xps - 17] [you[0].y_pos + yps - 9], st_prn, 10);
strcpy(info, st_prn);

gotoxy(xps + 1,yps);

} while (gotch != 'S');

// end of while.

finished_looking :
moves[0].move_x = xps - 17;
moves[0].move_y = yps - 9;
moves[0].target_x = you[0].x_pos + xps - 17;
moves[0].target_y = you[0].y_pos + yps - 9;

if (gotch == 'p') return 253;
if (gotch == '>') return 50001;

return 0;//mve_x * 100 + mve_y + 707 + 10000;

} // end of look_around

/*
 *  File:       view.cc
 *  Summary:    Misc function used to render the dungeon.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <6>      9/29/99        BCR             Removed first argument from draw_border
 *      <5>      6/22/99        BWR             Fixed and improved the stealth
 *      <4>      5/20/99        BWR             show_map colours all portals,
 *                                              exits from subdungeons now
 *                                              look like up stairs.
 *      <3>      5/09/99        JDJ             show_map draws shops in yellow.
 *      <2>      5/09/99        JDJ             show_map accepts '\r' along with '.'.
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "view.h"

#ifdef DOS
#include <conio.h>
#endif

#ifdef USE_CURSES
#include <curses.h>
#endif

#include <string.h>

#include "debug.h"
#include "externs.h"
#include "enum.h"
#include "mstruct.h"
#include "player.h"
#include "stuff.h"

#ifdef MACROS
  #include "macro.h"
#endif

/*#define menv env.mons
   #define mit env.it[0] */

/* for player_see_invis */

void moname(int mcl, char mench, char see_inv, char descrip, char glog[40]);

void item(void);
void monster_grid( bool do_updates );
void noisy(char loudness, char nois_x, char nois_y);
void cloud_grid(void);
int check_awaken(int mons_aw);
void losight(unsigned int sh[19][19], unsigned char gr[80][70], int x_p, int y_p);
unsigned char mapchar(unsigned char ldfk);
unsigned char mapchar2(unsigned char ldfk);
unsigned char mapchar3(unsigned char ldfk);
unsigned char mapchar4(unsigned char ldfk);

unsigned char (*mapch) (unsigned char);
unsigned char (*mapch2) (unsigned char);

unsigned char your_sign;        /* these two are accessed as externs in transform and acr */
unsigned char your_colour;

unsigned char show_green;
extern int stealth;             /* defined in acr.cc */
extern char visible[10];        /* also acr.cc */


//---------------------------------------------------------------
//
// get_ibm_symbol
//
// Returns the DOS character code and color for everything drawn
// with the IBM graphics option.
//
//---------------------------------------------------------------
static void get_ibm_symbol(unsigned int object, unsigned char *ch, unsigned char *color)
{
    ASSERT(color != NULL);
    ASSERT(ch != NULL);

    switch (object)
    {
    case 0:
        *ch = 0;
        break;
    case 1:
        *color = env.rock_colour;
        *ch = 177;
        break;                  // rock wall - remember earth elementals

    case 2:
        if (you.where_are_you == 17) {
            // stone in the realm of Zot is coloured the same as rock
            *color = env.rock_colour;
        }
        else
        {
            *color = LIGHTGREY;
        }
        *ch = 177;
        break;                  // stone wall

    case 3:
        *ch = 254;
        break;
    case 4:
        *ch = 177;
        *color = CYAN;
        break;
    case 5:
        *ch = 177;              // hidden secret door

        *color = env.rock_colour;
        break;
    case 6:
        *ch = 177;
        *color = GREEN;
        break;                  // green crystal wall

    case 7:
        *ch = '8';
        *color = DARKGREY;      // orcish idol

        break;
    case 8:
        *ch = 177;
        *color = YELLOW;
        break;                  // wax wall
        /* Anything added here must also be added to the PLAIN_TERMINAL viewwindow2 below */
    case 21:
        *ch = '8';
        *color = WHITE;         // silver statue

        if (visible[1] == 0)
            visible[1] = 3;
        else
            visible[1] = 2;
        visible[0] = 2;
        break;
    case 22:
        *ch = '8';
        *color = LIGHTGREY;     // granite statue

        break;
    case 23:
        *ch = '8';
        *color = LIGHTRED;      // orange crystal statue

        if (visible[2] == 0)
            visible[2] = 3;
        else
            visible[2] = 2;
        visible[0] = 2;
        break;
    case 35:
        *ch = '#';
        break;
    case 61:
        *ch = 247;
        *color = RED;           // lava!

        break;
    case 62:
        *ch = 247;              // this wavy thing also used for water elemental

        *color = BLUE;          // water

        break;
    case 65:
        *ch = 247;              // this wavy thing also used for water elemental

        *color = CYAN;          // shallow water

        break;
    case 67:
        *color = env.floor_colour;      //LIGHTGREY;

        *ch = 249;
        break;
    case 69:
        *ch = 239;
        *color = RED;
        break;                  // staircase to hell

    case 70:
        *ch = 39;
        break;                  // open door

    case 71:
        *ch = 240;
        *color = BROWN;
        break;                  // branch staircase

    case 75:
        *color = 11;
        *ch = 94;
        break;                  // ^ dart trap

    case 76:
        *color = MAGENTA;
        *ch = 94;
        break;
    case 77:
        *color = LIGHTGREY;
        *ch = 94;
        break;
    case 78:
        *ch = 249;
        *color = env.floor_colour;
        break;                  // undiscovered trap

    case 80:
        *ch = 239;
        *color = YELLOW;        // shop?

        break;
        // if I change anything above here, must also change magic mapping!
    case 81:
        *ch = 239;
        *color = LIGHTGREY;
        break;                  // labyrinth

    case 85:
        *color = BROWN;         // ladder

    case 82:
    case 83:
    case 84:
        *ch = '>';
        break;
    case 89:
        *color = BROWN;         // ladder

    case 86:
    case 87:
    case 88:
        *ch = '<';
        break;
    case 92:
        *color = CYAN;
        *ch = 239;
        break;                  // Stairway to Dis

    case 93:
        *color = RED;
        *ch = 239;
        break;                  // Gehenna

    case 94:
        *color = LIGHTCYAN;
        *ch = 239;
        break;                  // Cocytus

    case 95:
        *color = DARKGREY;
        *ch = 239;
        break;                  // Tartarus

    case 96:
        *color = random2(16);
        *ch = 239;
        break;                  // To Abyss

    case 97:
        *color = random2(16);
        *ch = 239;
        break;                  // From Abyss

    case 98:
        *color = LIGHTGREY;
        *ch = 239;
        break;                  // Closed gate to hell

    case 99:
        *color = LIGHTBLUE;
        *ch = 239;
        break;                  // gate to pandemonium

    case 100:
        *color = LIGHTBLUE;
        *ch = 239;
        break;                  // gate out of pandemonium

    case 101:
        *color = LIGHTGREEN;
        *ch = 239;
        break;                  // gate to other part of pandemonium

    case 110:
    case 111:
    case 112:
    case 113:
    case 114:
    case 115:
    case 116:
    case 118:
    case 119:
    case 120:
    case 121:
    case 122:
    case 123:
    case 124:
    case 125:
    case 126:
        *color = YELLOW;
        *ch = '>';
        break;

    case 117:
        *color = MAGENTA;
        *ch = 239;
        break;

    case 130:
    case 131:
    case 132:
    case 133:
    case 134:
    case 135:
    case 136:
    case 138:
    case 139:
    case 140:
    case 141:
    case 142:
    case 143:
    case 144:
    case 145:
    case 146:
        *color = YELLOW;
        *ch = '<';
        break;                  // stair from the orcish mines to main dungeon

    case 137:
        *color = MAGENTA;
        *ch = 239;
        break;

    case 180:
        *color = WHITE;
        *ch = 220;
        break;                  /* Altar to Zin */
    case 181:
        *color = YELLOW;
        *ch = 220;
        break;                  /* Altar to TSO */
    case 182:
        *color = DARKGREY;
        *ch = 220;
        break;                  /* Altar to Kiku */
    case 183:
        *color = DARKGREY;
        if (random2(3) == 0)
            *color = RED;
        *ch = 220;
        break;                  /* Altar to Yredelemnul */
    case 184:
        *color = random2(15) + 1;
        *ch = 220;
        break;                  /* Altar to Xom */
    case 185:
        *color = LIGHTBLUE;
        if (random2(3) == 0)
            *color = LIGHTMAGENTA;
        if (random2(3) == 0)
            *color = LIGHTRED;
        *ch = 220;
        break;                  /* Altar to Vehumet */
    case 186:
        *color = CYAN;
        *ch = 220;
        break;                  /* Altar to Okawaru */
    case 187:
        *color = RED;
        if (random2(3) == 0)
            *color = LIGHTRED;
        if (random2(3) == 0)
            *color = YELLOW;
        *ch = 220;
        break;                  /* Altar to Makhleb */
    case 188:
        *color = BLUE;
        *ch = 220;
        break;                  /* Altar to Sif Muna */
    case 189:
        *color = RED;
        *ch = 220;
        break;                  /* Altar to Trog */
    case 190:
        *color = LIGHTMAGENTA;
        *ch = 220;
        break;                  /* Altar to Nemelex */
    case 191:
        *color = LIGHTGREY;
        *ch = 220;
        break;                  /* Altar to Elyvilon */

    case 200:
        *color = BLUE;
        *ch = 159;
        break;                  /* water fountain - looks like a weird f */
    case 201:
        *color = LIGHTGREY;
        *ch = 159;
        break;                  /* dry fountain */
    case 202:
        *color = LIGHTBLUE;
        *ch = 159;
        break;                  /* Magic fountain */
    case 203:
        *color = LIGHTGREY;
        *ch = 159;
        break;                  /* dry fountain */
    case 210:
        *color = LIGHTGREY;
        *ch = 159;
        break;                  /* permenantly dry fountain */
    case 256:
        *ch = '0';
        break;
    case 257:
        *color = CYAN;
        *ch = '~';
        break;                  /* Invis creature walking through water */
    case 258:
        *ch = 41;
        break;                  // weapon )

    case 259:
        *ch = 91;
        break;                  // armour [

    case 260:
        *ch = 47;
        break;                  // food %

    case 261:
        *ch = 37;
        break;                  // wands / &c

    case 262:
        *ch = 43;
        break;                  // books +

    case 263:
        *ch = 63;
        break;                  // scroll ?

    case 264:
        *ch = 61;
        break;                  // ring = etc

    case 265:
        *ch = 33;
        break;                  // potions !

    case 266:
        *ch = 40;
        break;                  // stones

    case 267:
        *ch = '+';
        break;                  // book +

    case 268:
        *ch = 37;
        break;                  // corpses part 1

    case 269:
        *ch = '\\';
        break;                  // magical staves

    case 270:
        *ch = '}';
        break;                  // gems

    case 271:
        *ch = '%';
        break;                  // don't know ?

    case 272:
        *ch = 36;
        break;                  // $ gold

    case 273:
        *ch = '"';
        break;                  // amulet

    default:
        if (object >= 297)
            *ch = mons_char(object - 297);      // yeah

        else
            *ch = object;
    }
}


//---------------------------------------------------------------
//
// viewwindow2
//
// Draws the main window using the extended IBM character set.
//
// This function should not interfer with the game condition,
// unless do_updates is set (ie.  stealth checks for visible
// monsters).
//
//---------------------------------------------------------------
void viewwindow2(char draw_it, bool do_updates)
{
    const long BUFFER_SIZE = 1550;
    unsigned char buffy[BUFFER_SIZE];   //[800]; //392];

    unsigned char ch, color;

    _setcursortype(_NOCURSOR);

#ifdef WIZARD
    memset(buffy, 255, sizeof(buffy));
#endif

    losight(env.show, env.grid, you.x_pos, you.y_pos);

    int count_x, count_y;

    for (count_x = 0; count_x < 18; count_x++)
    {
        for (count_y = 0; count_y < 18; count_y++)
        {
            env.show_col[count_x][count_y] = 7;
        }
    }

    item();
    cloud_grid();
    monster_grid( do_updates );
    int bufcount = 0;

    if (draw_it == 1)
    {
        for (count_y = you.y_pos - 8; count_y < you.y_pos + 9; count_y++)
        {
            bufcount += 16;
            for (count_x = you.x_pos - 8; count_x < you.x_pos + 9; count_x++)
            {
                color = env.show_col[count_x - you.x_pos + 9][count_y - you.y_pos + 9];         // may be overriden by the code below

                if (count_x != you.x_pos || count_y != you.y_pos)
                {
                    unsigned int object = env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9];

                    get_ibm_symbol(object, &ch, &color);

                }
                else
                {
                    ch = your_sign;
                    color = your_colour;
                }

                ASSERT(bufcount + 1 < BUFFER_SIZE);
                buffy[bufcount] = ch;
                buffy[bufcount + 1] = color;

                bufcount += 2;
            }

            bufcount += 16;
        }

        if (you.level_type != 1 && you.level_type != 2)
        {
            bufcount = 0;
            for (count_y = 0; count_y < 17; count_y++)
            {
                bufcount += 16;
                for (count_x = 0; count_x < 17; count_x++)
                {
                    ASSERT(bufcount < BUFFER_SIZE);

                    if (buffy[bufcount] != 0)
                        env.map[count_x + you.x_pos - 9][count_y + you.y_pos - 9] = buffy[bufcount];
                    bufcount += 2;
                }
                bufcount += 16;
            }
        }

        bufcount = 0;
        for (count_y = 0; count_y < 17; count_y++)
        {
            for (count_x = 0; count_x < 33; count_x++)
            {
                if (count_x + you.x_pos - 17 < 3 || count_y + you.y_pos - 9 < 3 || count_x + you.x_pos - 14 > 77 || count_y + you.y_pos - 9 > 67)
                {
                    ASSERT(bufcount < BUFFER_SIZE);
                    buffy[bufcount] = 0;
                    bufcount++;
                    buffy[bufcount] = 0;
                    bufcount++;
                    continue;
                }
                if (count_x >= 8 && count_x <= 24 && count_y >= 0 && count_y <= 16 && buffy[bufcount] != 0)
                {
                    bufcount += 2;
                    continue;
                }
                ASSERT(bufcount + 1 < BUFFER_SIZE);
                buffy[bufcount] = env.map[count_x + you.x_pos - 17][count_y + you.y_pos - 9];
                buffy[bufcount + 1] = DARKGREY;
                bufcount += 2;
            }
        }

        if (you.berserker != 0)
        {
            for (count_x = 1; count_x < 1400; count_x += 2)
            {
                if (buffy[count_x] != DARKGREY)
                    buffy[count_x] = RED;
            }
        }

        if (show_green != 0)
        {
            for (count_x = 1; count_x < 1400; count_x += 2)
            {
                if (buffy[count_x] != DARKGREY)
                    buffy[count_x] = show_green;
            }
            show_green = 0;
            if (you.special_wield == 50)
                show_green = DARKGREY;
        }

#ifdef DOS_TERM
        puttext(2, 1, 34, 17, buffy);
#endif

#ifdef PLAIN_TERM
        gotoxy(2, 1);
        bufcount = 0;

        // if (you.running == 0) // this line is purely optional
        for (count_x = 0; count_x < 1120; count_x += 2)
        {                       // 1056

            ch = buffy[count_x];
            color = buffy[count_x + 1];
            ASSERT(color < 16);
            ASSERT(ch < 255);

            textcolor(color);
            putch(ch);
            if (count_x % 66 == 64 && count_x > 0)
                gotoxy(2, wherey() + 1);
        }
#endif
    }
}


void monster_grid( bool do_updates )
{

    int s;                      // a loop thing

    int mnc = 0;

#ifdef DEBUG
    if (do_updates) {
        mpr( "Stealth checks..." );
    }
#endif

    for (s = 0; s < MNST; s++)
    {

//if (type->menv [s].type != -1)
        if (menv[s].type != -1)
        {
            mnc++;

            if (menv[s].x > you.x_pos - 9 && menv[s].x < you.x_pos + 9 && menv[s].y > you.y_pos - 9 && menv[s].y < you.y_pos + 9 && env.show[menv[s].x - you.x_pos + 9][menv[s].y - you.y_pos + 9] != 0)
                // Put the bit commented out on the previous line back to restore shadow checking for monsters
            {
                if (do_updates && (menv[s].behavior == BEH_SLEEP
                                        || menv[s].behavior == BEH_WANDER)
                               && check_awaken(s))
                {
                    menv[s].behavior = 1;       // put stealth/you.invis here.

                    menv[s].target_x = you.x_pos;
                    menv[s].target_y = you.y_pos;

                    if (you.turn_is_over == 1 && mons_shouts(menv[s].type) > 0
                                    && random2(30) >= you.skills[SK_STEALTH])
                    {
                        switch (mons_shouts(menv[s].type))
                        {
                        case 1:
                            mpr("You hear a shout!");
                            break;
                        case 2:
                            mpr("You hear a bark!");
                            break;
                        case 3:
                            mpr("You hear two shouts!");
                            break;
                        case 4:
                            mpr("You hear a roar!");
                            break;
                        case 5:
                            mpr("You hear a hideous shriek!");
                            break;
                        case 6:
                            mpr("You hear a bellow!");
                            break;
                        case 7:
                            mpr("You hear a screech!");
                            break;
                        case 8:
                            mpr("You hear an angry buzzing noise.");
                            break;
                        case 9:
                            mpr("You hear a chilling moan.");
                            break;
                        case 10:
                            mpr("You hear an irritating high-pitched whine.");
                            break;
                        case 11:
                            mpr("You hear a croak.");
                            break;
                        }
                        noisy(8, menv[s].x, menv[s].y);
                    }
                }

/*      if (mons [s].enchantment [2] == 6 && (you.see_invis == 0 || (mons [s].type >= MLAVA0 && mons [s].number == 1))) */
                if (menv[s].enchantment[2] == 6 && (player_see_invis() == 0 || (menv[s].type >= MLAVA0 && menv[s].number == 1)))
                {
                    if (grd[menv[s].x][menv[s].y] == 65 && mons_flies(menv[s].type) == 0)
                    {
                        env.show[menv[s].x - you.x_pos + 9][menv[s].y - you.y_pos + 9] = 257;
                    }
                    continue;
                }
                else if (menv[s].behavior != 7 && (menv[s].type < 389 || menv[s].type > 393))
                    you.running = 0;    /* Friendly monsters or mimics don't disturb */
                env.show[menv[s].x - you.x_pos + 9][menv[s].y - you.y_pos + 9] = menv[s].type + 297;
                env.show_col[menv[s].x - you.x_pos + 9][menv[s].y - you.y_pos + 9] = mcolour[menv[s].type];

                if (mcolour[menv[s].type] == BLACK)
                    env.show_col[menv[s].x - you.x_pos + 9][menv[s].y - you.y_pos + 9] = menv[s].number;

            }
        }
    }                           // end of for i above

}                               // really the end of monster_grid


int check_awaken(int mons_aw)
{
    int mons_perc = 0;

    // Berserkers aren't really conserned about being stealthy.
    if (you.berserker > 0)
        return 1;

    mons_perc = (mons_intel(menv[mons_aw].type) * 4)
                    + menv[mons_aw].hit_dice
                    + mons_see_invis(menv[mons_aw].type) * 5;

    // I assume that creatures who can see invisible are very perceptive
    mons_perc += 10;

    if (you.invis != 0 && mons_see_invis(menv[mons_aw].type) == 0)
        mons_perc -= 75;

    if (mons_perc <= 0)
        mons_perc = 0;

    if (random2(stealth) <= mons_perc)
        return 1;

    return 0;
}


void item()
{
    char count_x, count_y;

    for (count_y = (you.y_pos - 8); (count_y < you.y_pos + 9); count_y++)
    {
        for (count_x = (you.x_pos - 8); (count_x < you.x_pos + 9); count_x++)
        {
            if (env.igrid[count_x][count_y] != 501)
            {
                if (env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] != 0)
                {
                    env.show_col[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = mitm.colour[env.igrid[count_x][count_y]];
                    if (grd[count_x][count_y] == 65)
                        env.show_col[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = CYAN;
                    switch (mitm.base_type[env.igrid[count_x][count_y]])
                    {
                    case 0:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 258;
                        break;
                        // need + 6 because show is 0 - 12, not -6 - +6
                    case 1:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 258;
                        break;
                    case 2:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 259;
                        break;
                    case 3:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 260;
                        break;
                    case 4:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 261;
                        break;
                    case 5:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 262;
                        break;
                    case 6:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 263;
                        break;
                    case 7:
                        if (mitm.sub_type[env.igrid[count_x][count_y]] >= 35)
                            env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 273;
                        else
                            env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 264;
                        break;
                    case 8:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 265;
                        break;
                    case 9:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 266;
                        break;
                    case 10:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 267;
                        break;
                    case 11:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 269;
                        break;
                    case 12:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 256;
                        break;
                    case 13:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 270;
                        break;
                    case 14:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 271;
                        break;
                    case 15:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = 272;
                        env.show_col[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = YELLOW;
                        break;
                    default:
                        env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9] = '8';
                        break;
                    }
                }
            }
        }                       // end of for count_x

    }                           // end of for count_y


}                               // end of item()




void cloud_grid(void)
{
    int s;                      // a loop thing

    int mnc = 0;

    for (s = 0; s < CLOUDS; s++)
    {

        if (mnc > env.cloud_no)
            break;

        if (env.cloud_type[s] != 0)
        {
            mnc++;

            if (env.cloud_x[s] > you.x_pos - 9 && env.cloud_x[s] < you.x_pos + 9 && env.cloud_y[s] > you.y_pos - 9 && env.cloud_y[s] < you.y_pos + 9 && env.show[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] != 0)
                // Put the bit commented out on the previous line back to restore shadow checking for clouds
            {
                env.show[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = 35;

                switch (env.cloud_type[s] % 100)
                {
                case 1:
                    if (env.cloud_decay[s] <= 20 || random2(4) == 0)
                    {
                        env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = RED;
                        break;
                    }
                    if (env.cloud_decay[s] <= 40 || random2(4) == 0)
                    {
                        env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = LIGHTRED;
                        break;
                    }
                    env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = YELLOW;
                    break;
                case 2:
                    env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = GREEN;
                    break;
                case 3: //show_col [cloud_x [s] - you.x_pos + 9] [cloud_y [s] - you.y_pos + 9] = WHITE; break;

                    if (env.cloud_decay[s] <= 20 || random2(4) == 0)
                    {
                        env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = BLUE;
                        break;
                    }
                    if (env.cloud_decay[s] <= 40 || random2(4) == 0)
                    {
                        env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = LIGHTBLUE;
                        break;
                    }
                    env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = WHITE;
                    break;
                case 4:
                    if (random2(3) != 0)
                        env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = GREEN;
                    else
                        env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = LIGHTGREEN;
                    break;
                case 5:
                    env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = LIGHTGREY;
                    break;
                case 6:
                    env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = LIGHTBLUE;
                    break;
                case 7:
                    env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = MAGENTA;
                    break;
                case 8:
                    env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = LIGHTGREY;
                    break;
                case 9:
                    env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = DARKGREY;
                    break;      // miasma

                case 10:
                    env.show_col[env.cloud_x[s] - you.x_pos + 9][env.cloud_y[s] - you.y_pos + 9] = DARKGREY;
                    break;      // black smoke

                }
            }
        }
    }                           // end of for i above

}                               // end of cloudter_grid()



// All items must have show values >= 38, all grid squares must be < 38
// because of monster invisibility.

void noisy(char loudness, char nois_x, char nois_y)
{
    int p = 0;

    for (p = 0; p < MNST; p++)
    {
        if (menv[p].x >= nois_x - loudness && menv[p].x <= nois_x + loudness && menv[p].y >= nois_y - loudness && menv[p].y <= nois_y + loudness)
        {
            if (menv[p].behavior == 0)
                menv[p].behavior = 1;
            menv[p].target_x = nois_x;
            menv[p].target_y = nois_y;
        }
    }
}



/*
   The losight function is so complex and tangled that I daren't even look at it.
   Good luck trying to work out what each bit does.
 */
void losight(unsigned int sh[19][19], unsigned char gr[80][70], int x_p, int y_p)
{
    char shad;
    char see_section;
    char startPoint_x = 0;      // = 8;

    char startPoint_y = 0;      // = 7;

    char behind = 0;
    char xs = 0;                // the multiplier of the x addition thing

    char ys = 0;
    char cx = 0;
    char cy = 0;

    short int see;              // see = 1 means 'visible'

// first comes the horizontal east:
    see = 1;
    for (cx = (x_p + 1); (cx < x_p + 9); cx++)
    {
        if (gr[cx - 1][y_p] < MINSEE)
            see = 0;
        if (see == 1)
        {
            sh[cx - x_p + 9][9] = gr[cx][y_p];
        }
        else
            sh[cx - x_p + 9][9] = 0;
    }

// now the horizontal West:
    see = 1;
    for (cx = (x_p - 1); (cx > x_p - 9); cx--)
    {
        if (gr[cx + 1][y_p] < MINSEE)
            see = 0;
        if (see == 1)
        {
            sh[cx - x_p + 9][9] = gr[cx][y_p];
        }
        else
            sh[cx - x_p + 9][9] = 0;
    }

// now for the North:
    see = 1;
    for (cy = (y_p - 1); (cy > y_p - 9); cy--)
    {
        if (gr[x_p][cy + 1] < MINSEE)
            see = 0;
        if (see == 1)
        {
            sh[9][cy - y_p + 9] = gr[x_p][cy];
        }
        else
            sh[9][cy - y_p + 9] = 0;
    }

// and the South...

    see = 1;
    for (cy = (y_p + 1); (cy < y_p + 9); cy++)
    {
        if (gr[x_p][cy - 1] < MINSEE)
            see = 0;
        if (see == 1)
        {
            sh[9][cy - y_p + 9] = gr[x_p][cy];
        }
        else
            sh[9][cy - y_p + 9] = 0;
    }

// Try the Southeast:

    see = 1;
    cy = y_p + 1;
    for (cx = x_p + 1; cx < x_p + 7; cx++)
    {
        if (gr[cx - 1][cy - 1] < MINSEE)
            see = 0;
        if (see == 1)
        {
            sh[cx - x_p + 9][cy - y_p + 9] = gr[cx][cy];
        }
        else
            sh[cx - x_p + 9][cy - y_p + 9] = 0;
        cy++;
    }

// Now for the Northeast:

    see = 1;
    cy = y_p - 1;
    for (cx = x_p + 1; cx < x_p + 7; cx++)
    {
        if (gr[cx - 1][cy + 1] < MINSEE)
            see = 0;
        if (see == 1)
        {
            sh[cx - x_p + 9][cy - y_p + 9] = gr[cx][cy];
        }
        else
            sh[cx - x_p + 9][cy - y_p + 9] = 0;
        cy--;
    }

// The Northwest:

    see = 1;
    cy = y_p - 1;
    for (cx = x_p - 1; cx > x_p - 7; cx--)
    {
        if (gr[cx + 1][cy + 1] < MINSEE)
            see = 0;
        if (see == 1)
        {
            sh[cx - x_p + 9][cy - y_p + 9] = gr[cx][cy];
        }
        else
            sh[cx - x_p + 9][cy - y_p + 9] = 0;
        cy--;
    }

// And the Southwest

    see = 1;
    cy = y_p + 1;
    for (cx = x_p - 1; cx > x_p - 7; cx--)
    {
        if (gr[cx + 1][cy - 1] < MINSEE)
            see = 0;
        if (see == 1)
        {
            sh[cx - x_p + 9][cy - y_p + 9] = gr[cx][cy];
        }
        else
            sh[cx - x_p + 9][cy - y_p + 9] = 0;
        cy++;
    }




// Anyway, now for the Fun part!

    see = 1;

// int three; // etc
    // int four; // whatever


    for (shad = 1; shad < 5; shad++)
    {

        if (shad == 1)
        {
            xs = 1;
            ys = 1;
            startPoint_x = 11;
            startPoint_y = 10;
        }

        if (shad == 2)
        {
            xs = 1;
            ys = -1;
            startPoint_x = 11;
            startPoint_y = 8;
        }

        if (shad == 3)
        {
            xs = -1;
            ys = 1;
            startPoint_x = 7;
            startPoint_y = 10;
        }

        if (shad == 4)
        {
            xs = -1;
            ys = -1;
            startPoint_x = 7;
            startPoint_y = 8;

        }

        behind = 0;
        see = 1;
        see_section = 1;

        if (gr[x_p + xs][y_p + ys] < MINSEE && gr[x_p + xs][y_p] < MINSEE)
            see_section = 0;

        see = see_section;

        if (see == 1)
            sh[startPoint_x][startPoint_y] = gr[x_p + startPoint_x - 9][y_p + startPoint_y - 9];
        else
            sh[startPoint_x][startPoint_y] = 0;

        if (gr[x_p + startPoint_x - 9][y_p + startPoint_y - 9] < MINSEE)
            see = 0;

        if (see == 1)
            sh[startPoint_x + xs][startPoint_y + ys] = gr[x_p + startPoint_x + xs - 9][y_p + startPoint_y + ys - 9];
        else
            sh[startPoint_x + xs][startPoint_y + ys] = 0;

        if (gr[x_p + startPoint_x + xs - 9][y_p + startPoint_y + ys - 9] < MINSEE)
            see = 0;

        if (see == 1)
            sh[startPoint_x + 2 * xs][startPoint_y + 2 * ys] = gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y + 2 * ys - 9];
        else
            sh[startPoint_x + 2 * xs][startPoint_y + 2 * ys] = 0;

        if (gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y + 2 * ys - 9] < MINSEE)
            see = 0;

        if (see == 1)
            sh[startPoint_x + 3 * xs][startPoint_y + 3 * ys] = gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + 3 * ys - 9];
        else
            sh[startPoint_x + 3 * xs][startPoint_y + 3 * ys] = 0;

        if (see == 1)
            sh[startPoint_x + 3 * xs][startPoint_y + 2 * ys] = gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + 2 * ys - 9];
        else
            sh[startPoint_x + 3 * xs][startPoint_y + 2 * ys] = 0;

// Wider:

        // This is done in a different way: see the >= MINSEE instead of < MINSEE

        if (gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + 3 * ys - 9] >= MINSEE)     //see = 0;

        {
            if (see == 1)
                sh[startPoint_x + 4 * xs][startPoint_y + 4 * ys] = gr[x_p + startPoint_x + 4 * xs - 9][y_p + startPoint_y + 4 * ys - 9];
            else
            {
                sh[startPoint_x + 4 * xs][startPoint_y + 4 * ys] = 0;
                see = 0;
            }
        }
        else
        {
            sh[startPoint_x + 4 * xs][startPoint_y + 4 * ys] = 0;
            see = 0;
        }


/*  if (sh [startPoint_x - 3*xs] [startPoint_y + 6*ys] != 0) //see = 0;
   if (!(gr [x_p + startPoint_x - 3*xs - 9] [y_p + startPoint_y + 6*ys - 9] < MINSEE)) //see = 0;
   {
   if (!(gr [x_p + startPoint_x - 3*xs - 9] [y_p + startPoint_y + 5*ys - 9] < MINSEE)) //see = 0;
   {
   if (see == 1) sh [startPoint_x - 2*xs] [startPoint_y + 7*ys] =  gr [x_p + startPoint_x - 2*xs - 9] [y_p + startPoint_y + 7*ys - 9]; else sh [startPoint_x - 2*xs] [startPoint_y + 7*ys] = 0;
   } else sh [startPoint_x - 2*xs] [startPoint_y + 7*ys] = 0;
   } else sh [startPoint_x - 2*xs] [startPoint_y + 7*ys] = 0;
   else sh [startPoint_x - 2*xs] [startPoint_y + 7*ys] = 0;
 */



// That's one line done...

        see_section = 1;

        if (gr[x_p + xs][y_p + ys] < MINSEE && gr[x_p + xs][y_p] < MINSEE)
            see_section = 0;

        if (gr[x_p + 2 * xs][y_p + ys] < MINSEE && gr[x_p + 2 * xs][y_p] < MINSEE)
            see_section = 0;

        see = see_section;

        if (see == 1)
            sh[startPoint_x + xs][startPoint_y] = gr[x_p + startPoint_x + xs - 9][y_p + startPoint_y - 9];
        else
            sh[startPoint_x + xs][startPoint_y] = 0;

        if (gr[x_p + startPoint_x + xs - 9][y_p + startPoint_y - 9] < MINSEE)
            see = 0;

        if (see == 1 && gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y - 9] >= MINSEE)
        {
            sh[startPoint_x + 3 * xs][startPoint_y + ys] = gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + ys - 9];
            // Wider:
            if (gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y - 9] >= MINSEE)
            {
                sh[startPoint_x + 4 * xs][startPoint_y + ys] = gr[x_p + startPoint_x + 4 * xs - 9][y_p + startPoint_y + ys - 9];        //else sh [startPoint_x + 4*xs] [startPoint_y + ys] = 0;

            }
            else
                sh[startPoint_x + 4 * xs][startPoint_y + ys] = 0;
            // Okay.

        }
        else
        {
            sh[startPoint_x + 3 * xs][startPoint_y + ys] = 0;
            sh[startPoint_x + 4 * xs][startPoint_y + ys] = 0;
            sh[startPoint_x + 5 * xs][startPoint_y + ys] = 0;
            sh[startPoint_x + 6 * xs][startPoint_y + ys] = 0;
        }

        if (see == 1)
            sh[startPoint_x + 2 * xs][startPoint_y + ys] = gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y + ys - 9];
        else
            sh[startPoint_x + 2 * xs][startPoint_y + ys] = 0;

        if (gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y + ys - 9] < MINSEE)
            see = 0;

        if (see == 1)
            sh[startPoint_x + 3 * xs][startPoint_y + 2 * ys] = gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + 2 * ys - 9];
        else
            sh[startPoint_x + 3 * xs][startPoint_y + 2 * ys] = 0;

// Wider:

        if (gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + 2 * ys - 9] >= MINSEE)     //see = 0;

        {
            if (see == 1)
                sh[startPoint_x + 4 * xs][startPoint_y + 3 * ys] = gr[x_p + startPoint_x + 4 * xs - 9][y_p + startPoint_y + 3 * ys - 9];
            else
                sh[startPoint_x + 4 * xs][startPoint_y + 3 * ys] = 0;
        }
        else
            sh[startPoint_x + 4 * xs][startPoint_y + 3 * ys] = 0;

        // This should work better:
        if (gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + ys - 9] >= MINSEE)         //see = 0;

        {
            if (see == 1)
                sh[startPoint_x + 4 * xs][startPoint_y + 2 * ys] = gr[x_p + startPoint_x + 4 * xs - 9][y_p + startPoint_y + 2 * ys - 9];
            else
            {
                sh[startPoint_x + 4 * xs][startPoint_y + 2 * ys] = 0;
                see = 0;
            }
        }
        else
        {
            see = 1;
            sh[startPoint_x + 4 * xs][startPoint_y + 2 * ys] = 0;
        }

//              if (see == 1) sh [startPoint_x + 5*xs] [startPoint_y + 2*ys] = gr [x_p + startPoint_x + 5*xs - 9] [y_p + startPoint_y + 2*ys - 9]; else sh [startPoint_x + 5*xs] [startPoint_y + 2*ys] = 0;


        see = see_section;


// And one more for this section:

        if (gr[x_p + 3 * xs][y_p] < MINSEE && gr[x_p + 3 * xs][y_p + ys] < MINSEE)
            see_section = 0;

        see = see_section;



        if (gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y - 9] < MINSEE)
            see = 0;



        if (gr[x_p + 3 * xs][y_p + ys] < MINSEE && gr[x_p + 3 * xs][y_p + 2 * ys] < MINSEE)
            see_section = 0;


        see = see_section;

        if (see == 1)
            sh[startPoint_x + 2 * xs][startPoint_y] = gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y - 9];
        else
            sh[startPoint_x + 2 * xs][startPoint_y] = 0;


        if (gr[x_p + 4 * xs][y_p + 2 * ys] < MINSEE && gr[x_p + 4 * xs][y_p + ys] < MINSEE)
            see = 0;

        if ((gr[x_p + xs][y_p] < MINSEE && gr[x_p + xs][y_p + ys] < MINSEE) || (gr[x_p + 2 * xs][y_p] < MINSEE && gr[x_p + 2 * xs][y_p + ys] < MINSEE) || (gr[x_p + 3 * xs][y_p] < MINSEE && gr[x_p + 3 * xs][y_p + ys] < MINSEE))
        {
            sh[startPoint_x + 2 * xs][startPoint_y] = 0;
        }
        else
            sh[startPoint_x + 2 * xs][startPoint_y] = gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y - 9];


        if ((gr[x_p + xs][y_p] < MINSEE && gr[x_p + xs][y_p + ys] < MINSEE) || (gr[x_p + 2 * xs][y_p] < MINSEE && gr[x_p + 2 * xs][y_p + ys] < MINSEE) || (gr[x_p + 3 * xs][y_p] < MINSEE && gr[x_p + 3 * xs][y_p + ys] < MINSEE) || (gr[x_p + 4 * xs][y_p] < MINSEE && gr[x_p + 4 * xs][y_p + ys] < MINSEE))
        {
            sh[startPoint_x + 3 * xs][startPoint_y] = 0;
            sh[startPoint_x + 4 * xs][startPoint_y] = 0;
        }
        else
        {
            sh[startPoint_x + 3 * xs][startPoint_y] = gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y - 9];


            if (gr[x_p + 5 * xs][y_p] < MINSEE && gr[x_p + 5 * xs][y_p + ys] < MINSEE)
            {
                sh[startPoint_x + 4 * xs][startPoint_y] = 0;
            }
            else
                sh[startPoint_x + 4 * xs][startPoint_y] = gr[x_p + startPoint_x + 4 * xs - 9][y_p + startPoint_y - 9];



        }                       // end of else

        // These do the far two layers.
        see = 1;

        if (sh[startPoint_x + 4 * xs][startPoint_y + 3 * ys] != 0)      //see = 0;

            if (!(gr[x_p + startPoint_x + 4 * xs - 9][y_p + startPoint_y + 3 * ys - 9] < MINSEE))       //see = 0;

            {
                if (!(gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + 3 * ys - 9] < MINSEE))   //see = 0;

                {
                    if (see == 1)
                        sh[startPoint_x + 5 * xs][startPoint_y + 4 * ys] = gr[x_p + startPoint_x + 5 * xs - 9][y_p + startPoint_y + 4 * ys - 9];
                    else
                        sh[startPoint_x + 5 * xs][startPoint_y + 4 * ys] = 0;
                }
                else
                    sh[startPoint_x + 5 * xs][startPoint_y + 4 * ys] = 0;
            }
            else
                sh[startPoint_x + 5 * xs][startPoint_y + 4 * ys] = 0;
        else
            sh[startPoint_x + 5 * xs][startPoint_y + 4 * ys] = 0;

//  if (see == 0) sh [startPoint_x + 3*xs] [startPoint_y + 6*ys] = 0;


//  if (sh [startPoint_x] [startPoint_y] == 0) see = 0;
        if (sh[startPoint_x + 4 * xs][startPoint_y + 2 * ys] != 0)      //see = 0;

            if (!(gr[x_p + startPoint_x + 4 * xs - 9][y_p + startPoint_y + 2 * ys - 9] < MINSEE))       //see = 0;

            {
                if (!(gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + 2 * ys - 9] < MINSEE))   //see = 0;

                {
                    if (see == 1)
                        sh[startPoint_x + 5 * xs][startPoint_y + 3 * ys] = gr[x_p + startPoint_x + 5 * xs - 9][y_p + startPoint_y + 3 * ys - 9];
                    else
                        sh[startPoint_x + 5 * xs][startPoint_y + 3 * ys] = 0;
                }
                else
                    sh[startPoint_x + 5 * xs][startPoint_y + 3 * ys] = 0;
            }
            else
                sh[startPoint_x + 5 * xs][startPoint_y + 3 * ys] = 0;
        else
            sh[startPoint_x + 5 * xs][startPoint_y + 3 * ys] = 0;

//  if (see == 0) sh [startPoint_x + 2*xs] [startPoint_y + 6*ys] = 0;
        //  if (sh [startPoint_x] [startPoint_y] == 0) see = 0;


        if (sh[startPoint_x + 4 * xs][startPoint_y + ys] != 0)       //see = 0;

            if (!(gr[x_p + startPoint_x + 4 * xs - 9][y_p + startPoint_y + ys - 9] < MINSEE))        //see = 0;

            {
                if (!(gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + ys - 9] < MINSEE))    //see = 0;

                {
                    if (see == 1)
                        sh[startPoint_x + 5 * xs][startPoint_y + 2 * ys] = gr[x_p + startPoint_x + 5 * xs - 9][y_p + startPoint_y + 2 * ys - 9];
                    else
                        sh[startPoint_x + 5 * xs][startPoint_y + 2 * ys] = 0;
                }
                else
                    sh[startPoint_x + 5 * xs][startPoint_y + 2 * ys] = 0;
            }
            else
                sh[startPoint_x + 5 * xs][startPoint_y + 2 * ys] = 0;
        else
            sh[startPoint_x + 5 * xs][startPoint_y + 2 * ys] = 0;

//  if (see == 0) sh [startPoint_x + xs] [startPoint_y + 6*ys] = 0;


        if (sh[startPoint_x + 4 * xs][startPoint_y] != 0)  //see = 0;

            if (!(gr[x_p + startPoint_x + 4 * xs - 9][y_p + startPoint_y - 9] < MINSEE))   //see = 0;

            {
                if (!(gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y - 9] < MINSEE))       //see = 0;

                {
                    if (see == 1)
                        sh[startPoint_x + 5 * xs][startPoint_y + ys] = gr[x_p + startPoint_x + 5 * xs - 9][y_p + startPoint_y + ys - 9];
                    else
                        sh[startPoint_x + 5 * xs][startPoint_y + ys] = 0;
                }
                else
                    sh[startPoint_x + 5 * xs][startPoint_y + ys] = 0;
            }
            else
                sh[startPoint_x + 5 * xs][startPoint_y + ys] = 0;
        else
            sh[startPoint_x + 5 * xs][startPoint_y + ys] = 0;



        if (sh[startPoint_x + 4 * xs][startPoint_y - 1 * ys] != 0)      //see = 0;

            if (!(gr[x_p + startPoint_x + 4 * xs - 9][y_p + startPoint_y - 1 * ys - 9] < MINSEE))       //see = 0;

            {
                if (!(gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y - 1 * ys - 9] < MINSEE))   //see = 0;

                {
                    if (see == 1)
                        sh[startPoint_x + 5 * xs][startPoint_y] = gr[x_p + startPoint_x + 5 * xs - 9][y_p + startPoint_y - 9];
                    else
                        sh[startPoint_x + 5 * xs][startPoint_y] = 0;
                }
                else
                    sh[startPoint_x + 5 * xs][startPoint_y] = 0;
            }
            else
                sh[startPoint_x + 5 * xs][startPoint_y] = 0;
        else
            sh[startPoint_x + 5 * xs][startPoint_y] = 0;



        if (sh[startPoint_x + 5 * xs][startPoint_y] != 0)  //see = 0;

            if (!(gr[x_p + startPoint_x + 5 * xs - 9][y_p + startPoint_y - 9] < MINSEE))   //see = 0;

            {
                if (!(gr[x_p + startPoint_x + 4 * xs - 9][y_p + startPoint_y - 9] < MINSEE))       //see = 0;

                {
                    if (see == 1)
                        sh[startPoint_x + 6 * xs][startPoint_y + ys] = gr[x_p + startPoint_x  + 6 * xs - 9][y_p + startPoint_y + ys - 9];
                    else
                        sh[startPoint_x + 6 * xs][startPoint_y + ys] = 0;
                }
                else
                    sh[startPoint_x + 6 * xs][startPoint_y + ys] = 0;
            }
            else
                sh[startPoint_x + 6 * xs][startPoint_y + ys] = 0;
        else
            sh[startPoint_x + 6 * xs][startPoint_y + ys] = 0;



        if (sh[startPoint_x + 5 * xs][startPoint_y - ys] != 0)      //see = 0;
            if (!(gr[x_p + startPoint_x + 5 * xs - 9][y_p + startPoint_y - ys - 9] < MINSEE))       //see = 0;

            {
                if (!(gr[x_p + startPoint_x + 4 * xs - 9][y_p + startPoint_y - ys - 9] < MINSEE))   //see = 0;

                {
                    if (see == 1)
                        sh[startPoint_x + 6 * xs][startPoint_y] = gr[x_p + startPoint_x + 6 * xs - 9][y_p + startPoint_y - 9];
                    else
                        sh[startPoint_x + 6 * xs][startPoint_y] = 0;
                }
                else
                    sh[startPoint_x + 6 * xs][startPoint_y] = 0;
            }
            else
                sh[startPoint_x + 6 * xs][startPoint_y] = 0;
        else
            sh[startPoint_x + 6 * xs][startPoint_y] = 0;

    }                           // end of the for (i) above.



// The second lot:

    for (shad = 1; shad < 5; shad++)
    {

        if (shad == 1)
        {
            xs = 1;
            ys = 1;
//      three = 3;
            startPoint_x = 10;
            startPoint_y = 11;
        }

        if (shad == 2)
        {
            xs = 1;
            ys = -1;
            startPoint_x = 10;
            startPoint_y = 7;
        }

        if (shad == 3)
        {
            xs = -1;
            ys = 1;
            startPoint_x = 8;
            startPoint_y = 11;
        }

        if (shad == 4)
        {
            xs = -1;
            ys = -1;
            startPoint_x = 8;
            startPoint_y = 7;

        }


        behind = 0;
        see = 1;
        see_section = 1;

        if (gr[x_p + xs][y_p + ys] < MINSEE && gr[x_p][y_p + ys] < MINSEE)
            see_section = 0;

        see = see_section;

        if (see == 1)
            sh[startPoint_x][startPoint_y] = gr[x_p + startPoint_x - 9][y_p + startPoint_y - 9];
        else
            sh[startPoint_x][startPoint_y] = 0;

        if (gr[x_p + startPoint_x - 9][y_p + startPoint_y - 9] < MINSEE)
            see = 0;

        if (see == 1)
            sh[startPoint_x + xs][startPoint_y + ys] = gr[x_p + startPoint_x + xs - 9][y_p + startPoint_y + ys - 9];
        else
            sh[startPoint_x + xs][startPoint_y + ys] = 0;

        if (gr[x_p + startPoint_x + xs - 9][y_p + startPoint_y + ys - 9] < MINSEE)
            see = 0;

        if (see == 1)
            sh[startPoint_x + 2 * xs][startPoint_y + 2 * ys] = gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y + 2 * ys - 9];
        else
            sh[startPoint_x + 2 * xs][startPoint_y + 2 * ys] = 0;

        if (gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y + 2 * ys - 9] < MINSEE)
            see = 0;

        if (see == 1)
            sh[startPoint_x + 3 * xs][startPoint_y + 3 * ys] = gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + 3 * ys - 9];
        else
            sh[startPoint_x + 3 * xs][startPoint_y + 3 * ys] = 0;

        if (see == 1)
            sh[startPoint_x + 2 * xs][startPoint_y + 3 * ys] = gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y + 3 * ys - 9];
        else
            sh[startPoint_x + 2 * xs][startPoint_y + 3 * ys] = 0;

// Wider:


        if (gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + 3 * ys - 9] >= MINSEE)     //see = 0;

        {
            if (see == 1)
                sh[startPoint_x + 4 * xs][startPoint_y + 4 * ys] = gr[x_p + startPoint_x + 4 * xs - 9][y_p + startPoint_y + 4 * ys - 9];
            else
                sh[startPoint_x + 4 * xs][startPoint_y + 4 * ys] = 0;
        }
        else
            sh[startPoint_x + 4 * xs][startPoint_y + 4 * ys] = 0;

// Okay.




/*  if (sh [startPoint_x + 6*xs] [startPoint_y - 3*ys] != 0)
   if (!(gr [x_p + startPoint_x + 6*xs - 9] [y_p + startPoint_y - 3*ys - 9] < MINSEE))
   {
   if (!(gr [x_p + startPoint_x + 5*xs - 9] [y_p + startPoint_y - 3*ys - 9] < MINSEE))
   {
   sh [startPoint_x + 7*xs] [startPoint_y - 2*ys] =  gr [x_p + startPoint_x + 7*xs - 9] [y_p + startPoint_y - 2*ys - 9]; //else sh [startPoint_x + 7*xs] [startPoint_y - 2*ys] = 0;
   } else sh [startPoint_x + 7*xs] [startPoint_y - 2*ys] = 0;
   } else sh [startPoint_x + 7*xs] [startPoint_y - 2*ys] = 0;
   else sh [startPoint_x + 7*xs] [startPoint_y - 2*ys] = 0;

   / *  if (sh [startPoint_x + 6*xs] [startPoint_y - 3*ys] != 0)
   if (!(gr [x_p + startPoint_x + 6*xs - 9] [y_p + startPoint_y - 3*ys - 9] < MINSEE))
   {
   if (!(gr [x_p + startPoint_x + 5*xs - 9] [y_p + startPoint_y - 3*ys - 9] < MINSEE))
   {
   sh [startPoint_x + 7*xs] [startPoint_y - 3*ys] =  gr [x_p + startPoint_x + 7*xs - 9] [y_p + startPoint_y - 3*ys - 9]; //else sh [startPoint_x + 7*xs] [startPoint_y - 3*ys] = 0;
   } else sh [startPoint_x + 7*xs] [startPoint_y - 3*ys] = 0;
   } else sh [startPoint_x + 7*xs] [startPoint_y - 3*ys] = 0;
   else sh [startPoint_x + 7*xs] [startPoint_y - 3*ys] = 0;
 */



// That's one line done...

        if (gr[x_p + xs][y_p + ys] < MINSEE && gr[x_p][y_p + ys] < MINSEE)
            see_section = 0;

        if (gr[x_p + xs][y_p + 2 * ys] < MINSEE && gr[x_p][y_p + 2 * ys] < MINSEE)
            see_section = 0;

        see = see_section;

        if (see == 1)
            sh[startPoint_x][startPoint_y + ys] = gr[x_p + startPoint_x - 9][y_p + startPoint_y + ys - 9];
        else
            sh[startPoint_x][startPoint_y + ys] = 0;

        if (gr[x_p + startPoint_x - 9][y_p + startPoint_y + ys - 9] < MINSEE)
            see = 0;


        if (see == 1 && gr[x_p + startPoint_x - 9][y_p + startPoint_y + 2 * ys - 9] >= MINSEE)
        {
            sh[startPoint_x + xs][startPoint_y + 3 * ys] = gr[x_p + startPoint_x + xs - 9][y_p + startPoint_y + 3 * ys - 9];
            // Wider:
            if (gr[x_p + startPoint_x - 9][y_p + startPoint_y + 3 * ys - 9] >= MINSEE)
            {
                sh[startPoint_x + xs][startPoint_y + 4 * ys] = gr[x_p + startPoint_x + xs - 9][y_p + startPoint_y + 4 * ys - 9];        //else sh [startPoint_x + 4*xs] [startPoint_y + ys] = 0;

            }
            else
                sh[startPoint_x + xs][startPoint_y + 4 * ys] = 0;
            // Okay.
        }
        else
        {
            sh[startPoint_x + xs][startPoint_y + 3 * ys] = 0;
            sh[startPoint_x + xs][startPoint_y + 4 * ys] = 0;
        }


        if (see == 1)
            sh[startPoint_x + xs][startPoint_y + 2 * ys] = gr[x_p + startPoint_x + xs - 9][y_p + startPoint_y + 2 * ys - 9];
        else
            sh[startPoint_x + xs][startPoint_y + 2 * ys] = 0;

        if (gr[x_p + startPoint_x + xs - 9][y_p + startPoint_y + 2 * ys - 9] < MINSEE)
            see = 0;

        if (see == 1 && behind == 0)
            sh[startPoint_x + 2 * xs][startPoint_y + 3 * ys] = gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y + 3 * ys - 9];
        else
            sh[startPoint_x + 2 * xs][startPoint_y + 3 * ys] = 0;


// Wider:

        if (gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y + 3 * ys - 9] >= MINSEE)     //see = 0;

        {
            if (see == 1)
                sh[startPoint_x + 3 * xs][startPoint_y + 4 * ys] = gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + 4 * ys - 9];
            else
                sh[startPoint_x + 3 * xs][startPoint_y + 4 * ys] = 0;
        }
        else
            sh[startPoint_x + 3 * xs][startPoint_y + 4 * ys] = 0;


        if (gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y + 3 * ys - 9] >= MINSEE || gr[x_p + startPoint_x + xs - 9][y_p + startPoint_y + 3 * ys - 9] >= MINSEE)       //see = 0;

        {
            if (see == 1)
                sh[startPoint_x + 2 * xs][startPoint_y + 4 * ys] = gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y + 4 * ys - 9];
            else
                sh[startPoint_x + 2 * xs][startPoint_y + 4 * ys] = 0;
        }
        else
            sh[startPoint_x + 2 * xs][startPoint_y + 4 * ys] = 0;

// Okay.


        see = see_section;

// And one more for this section:

        if (gr[x_p][y_p + 3 * ys] < MINSEE && gr[x_p + xs][y_p + 3 * ys] < MINSEE)
            see_section = 0;

        see = see_section;

        if (see == 1)
            sh[startPoint_x][startPoint_y + 2 * ys] = gr[x_p + startPoint_x - 9][y_p + startPoint_y + 2 * ys - 9];
        else
            sh[startPoint_x][startPoint_y + 2 * ys] = 0;


        if (gr[x_p + startPoint_x - 9][y_p + startPoint_y + 2 * ys - 9] < MINSEE)
            see = 0;


        if (gr[x_p + xs][y_p + 3 * ys] < MINSEE && gr[x_p + 2 * xs][y_p + 3 * ys] < MINSEE)
            see_section = 0;

        see = see_section;

        see = see_section;

        if (gr[x_p + 2 * xs][y_p + 4 * ys] < MINSEE && gr[x_p + xs][y_p + 4 * ys] < MINSEE)
            see = 0;




        if ((gr[x_p][y_p + ys] < MINSEE && gr[x_p + xs][y_p + ys] < MINSEE) || (gr[x_p][y_p + 2 * ys] < MINSEE && gr[x_p + xs][y_p + 2 * ys] < MINSEE) || (gr[x_p][y_p + 3 * ys] < MINSEE && gr[x_p + xs][y_p + 3 * ys] < MINSEE))
        {
            sh[startPoint_x][startPoint_y + 2 * ys] = 0;
        }
        else
            sh[startPoint_x][startPoint_y + 2 * ys] = gr[x_p + startPoint_x - 9][y_p + startPoint_y + 2 * ys - 9];



        if ((gr[x_p][y_p + ys] < MINSEE && gr[x_p + xs][y_p + ys] < MINSEE) || (gr[x_p][y_p + 2 * ys] < MINSEE && gr[x_p + xs][y_p + 2 * ys] < MINSEE) || (gr[x_p][y_p + 3 * ys] < MINSEE && gr[x_p + xs][y_p + 3 * ys] < MINSEE) || (gr[x_p][y_p + 4 * ys] < MINSEE && gr[x_p + xs][y_p + 4 * ys] < MINSEE))
        {
            sh[startPoint_x][startPoint_y + 3 * ys] = 0;
            sh[startPoint_x][startPoint_y + 4 * ys] = 0;
        }
        else
        {
            sh[startPoint_x][startPoint_y + 3 * ys] = gr[x_p + startPoint_x - 9][y_p + startPoint_y + 3 * ys - 9];


            if (gr[x_p][y_p + 5 * ys] < MINSEE && gr[x_p][y_p + 5 * ys] < MINSEE)
            {
                sh[startPoint_x][startPoint_y + 4 * ys] = 0;
            }
            else
                sh[startPoint_x][startPoint_y + 4 * ys] = gr[x_p + startPoint_x - 9][y_p + startPoint_y + 4 * ys - 9];

        }                       // end of else

//sh [5] [5] = 'X';


        // These fo the far two layers

        see = 1;
        if (sh[startPoint_x + 3 * xs][startPoint_y + 4 * ys] != 0)      //see = 0;

            if (!(gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + 4 * ys - 9] < MINSEE))       //see = 0;

            {
                if (!(gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + 3 * ys - 9] < MINSEE))   //see = 0;

                {
                    if (see == 1)
                        sh[startPoint_x + 4 * xs][startPoint_y + 5 * ys] = gr[x_p + startPoint_x + 4 * xs - 9][y_p + startPoint_y + 5 * ys - 9];
                    else
                        sh[startPoint_x + 4 * xs][startPoint_y + 5 * ys] = 0;
                }
                else
                    sh[startPoint_x + 4 * xs][startPoint_y + 5 * ys] = 0;
            }
            else
                sh[startPoint_x + 4 * xs][startPoint_y + 5 * ys] = 0;
        else
            sh[startPoint_x + 4 * xs][startPoint_y + 5 * ys] = 0;



        if (sh[startPoint_x + 2 * xs][startPoint_y + 4 * ys] != 0)      //see = 0;

            if (!(gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y + 4 * ys - 9] < MINSEE))       //see = 0;

            {
                if (!(gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y + 3 * ys - 9] < MINSEE))   //see = 0;

                {
                    if (see == 1)
                        sh[startPoint_x + 3 * xs][startPoint_y + 5 * ys] = gr[x_p + startPoint_x + 3 * xs - 9][y_p + startPoint_y + 5 * ys - 9];
                    else
                        sh[startPoint_x + 3 * xs][startPoint_y + 5 * ys] = 0;
                }
                else
                    sh[startPoint_x + 3 * xs][startPoint_y + 5 * ys] = 0;
            }
            else
                sh[startPoint_x + 3 * xs][startPoint_y + 5 * ys] = 0;
        else
            sh[startPoint_x + 3 * xs][startPoint_y + 5 * ys] = 0;

//  if (see == 0) sh [startPoint_x + 2*xs] [startPoint_y + 6*ys] = 0;
        //  if (sh [startPoint_x] [startPoint_y] == 0) see = 0;


        if (sh[startPoint_x + xs][startPoint_y + 4 * ys] != 0)      //see = 0;

            if (!(gr[x_p + startPoint_x + xs - 9][y_p + startPoint_y + 4 * ys - 9] < MINSEE))       //see = 0;

            {
                if (!(gr[x_p + startPoint_x + xs - 9][y_p + startPoint_y + 3 * ys - 9] < MINSEE))   //see = 0;

                {
                    if (see == 1)
                        sh[startPoint_x + 2 * xs][startPoint_y + 5 * ys] = gr[x_p + startPoint_x + 2 * xs - 9][y_p + startPoint_y + 5 * ys - 9];
                    else
                        sh[startPoint_x + 2 * xs][startPoint_y + 5 * ys] = 0;
                }
                else
                    sh[startPoint_x + 2 * xs][startPoint_y + 5 * ys] = 0;
            }
            else
                sh[startPoint_x + 2 * xs][startPoint_y + 5 * ys] = 0;
        else
            sh[startPoint_x + 2 * xs][startPoint_y + 5 * ys] = 0;

//  if (see == 0) sh [startPoint_x + xs] [startPoint_y + 6*ys] = 0;

        if (sh[startPoint_x][startPoint_y + 4 * ys] != 0)      //see = 0;

            if (!(gr[x_p + startPoint_x - 9][y_p + startPoint_y + 4 * ys - 9] < MINSEE))       //see = 0;

            {
                if (!(gr[x_p + startPoint_x - 9][y_p + startPoint_y + 3 * ys - 9] < MINSEE))   //see = 0;

                {
                    if (see == 1)
                        sh[startPoint_x + 1 * xs][startPoint_y + 5 * ys] = gr[x_p + startPoint_x + 1 * xs - 9][y_p + startPoint_y + 5 * ys - 9];
                    else
                        sh[startPoint_x + 1 * xs][startPoint_y + 5 * ys] = 0;
                }
                else
                    sh[startPoint_x + 1 * xs][startPoint_y + 5 * ys] = 0;
            }
            else
                sh[startPoint_x + 1 * xs][startPoint_y + 5 * ys] = 0;
        else
            sh[startPoint_x + 1 * xs][startPoint_y + 5 * ys] = 0;


        if (sh[startPoint_x - 1 * xs][startPoint_y + 4 * ys] != 0)      //see = 0;

            if (!(gr[x_p + startPoint_x - 1 * xs - 9][y_p + startPoint_y + 4 * ys - 9] < MINSEE))       //see = 0;

            {
                if (!(gr[x_p + startPoint_x - 1 * xs - 9][y_p + startPoint_y + 3 * ys - 9] < MINSEE))   //see = 0;

                {
                    if (see == 1)
                        sh[startPoint_x][startPoint_y + 5 * ys] = gr[x_p + startPoint_x - 9][y_p + startPoint_y + 5 * ys - 9];
                    else
                        sh[startPoint_x][startPoint_y + 5 * ys] = 0;
                }
                else
                    sh[startPoint_x][startPoint_y + 5 * ys] = 0;
            }
            else
                sh[startPoint_x][startPoint_y + 5 * ys] = 0;
        else
            sh[startPoint_x][startPoint_y + 5 * ys] = 0;


        if (sh[startPoint_x][startPoint_y + 5 * ys] != 0)      //see = 0;

            if (!(gr[x_p + startPoint_x - 9][y_p + startPoint_y + 5 * ys - 9] < MINSEE))       //see = 0;

            {
                if (!(gr[x_p + startPoint_x - 9][y_p + startPoint_y + 4 * ys - 9] < MINSEE))   //see = 0;

                {
                    sh[startPoint_x + 1 * xs][startPoint_y + 6 * ys] = gr[x_p + startPoint_x + 1 * xs - 9][y_p + startPoint_y + 6 * ys - 9];    //else sh [startPoint_x + 7*xs] [startPoint_y + 0*ys] = 0;

                }
                else
                    sh[startPoint_x + 1 * xs][startPoint_y + 6 * ys] = 0;
            }
            else
                sh[startPoint_x + 1 * xs][startPoint_y + 6 * ys] = 0;
        else
            sh[startPoint_x + 1 * xs][startPoint_y + 6 * ys] = 0;


        if (sh[startPoint_x - 1 * xs][startPoint_y + 5 * ys] != 0)      //see = 0;

            if (!(gr[x_p + startPoint_x - 1 * xs - 9][y_p + startPoint_y + 5 * ys - 9] < MINSEE))       //see = 0;

            {
                if (!(gr[x_p + startPoint_x - 1 * xs - 9][y_p + startPoint_y + 4 * ys - 9] < MINSEE))   //see = 0;

                {
                    sh[startPoint_x + 0 * xs][startPoint_y + 6 * ys] = gr[x_p + startPoint_x + 0 * xs - 9][y_p + startPoint_y + 6 * ys - 9];    //else sh [startPoint_x + 7*xs] [startPoint_y - 1*ys] = 0;

                }
                else
                    sh[startPoint_x + 0 * xs][startPoint_y + 6 * ys] = 0;
            }
            else
                sh[startPoint_x + 0 * xs][startPoint_y + 6 * ys] = 0;
        else
            sh[startPoint_x + 0 * xs][startPoint_y + 6 * ys] = 0;

    }                           // end of the for (shad) above.
}

void draw_border(char your_name[kNameLen], char class_name[40], char tspecies)
{

    textcolor(BORDER_COLOR);
// this bit draws the borders:
#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif

    clrscr();
    gotoxy(40, 1);
    textcolor(7);
    char print_it[80];
    char print_it2[42];

    int i = 0;
    char spaces = 0;

    strcpy(print_it, your_name);
    strcat(print_it, " the ");
    strcat(print_it, class_name);

    for (i = 0; i < 39; i++)
    {
        print_it2[i] = print_it[i];
        if (print_it[i] == 0)
            break;
    }

    for (i = 0; i < 40; i++)
    {
        if (print_it2[i] == 0)
            spaces = 1;
        if (spaces == 1)
            print_it2[i] = ' ';
    }

    print_it2[39] = 0;

    textcolor(LIGHTGREY);

#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif
    gotoxy(40, 1);
    textcolor(7);
    cprintf(print_it2);
    gotoxy(40, 2);
    cprintf(species_name(tspecies));
    gotoxy(40, 3);
    cprintf("HP:");
    gotoxy(40, 4);
    cprintf("Magic:");
    gotoxy(40, 5);
    cprintf("AC:");
    gotoxy(40, 6);
    cprintf("EV:");

    gotoxy(40, 7);
    cprintf("Str:");
    gotoxy(40, 8);
    cprintf("Int:");
    gotoxy(40, 9);
    cprintf("Dex:");
    gotoxy(40, 10);
    cprintf("Gold:");
    gotoxy(40, 11);
    cprintf("Experience:");
    gotoxy(40, 12);
    cprintf("Level");

}                               // end of void draw_border(int bord_col)



void show_map(int spec_place[2])
{
    int curs_x = you.x_pos;
    int curs_y = 12;
    int screen_y = you.y_pos;

    int i;
    int bufcount2 = 0;
    int j;

    char move_x = 0;
    char move_y = 0;
    char getty = 0;

#ifdef DOS_TERM
    char buffer[4800];

#endif
    char buffer2[70 * 80 * 2];

    char min_y = 0;
    char max_y = 0;
    char found = 0;
    unsigned char square;

    for (j = 0; j < 70; j++)
    {
        for (i = 0; i < 80; i++)
        {
            if (env.map[i][j] != 0 && found == 0)
            {
                found = 1;
                min_y = j;
            }

            if (env.map[i][j] != 0)
                max_y = j;
        }
    }

#ifdef DOS_TERM
    gettext(1, 1, 80, 25, buffer);
    window(1, 1, 80, 25);
#endif

    clrscr();
    textcolor(DARKGREY);

put_screen:
    bufcount2 = 0;

#ifdef PLAIN_TERM
    gotoxy(1, 1);
#endif

    for (j = 0; j < NUMBER_OF_LINES; j++)
    {
        for (i = 0; i < 80; i++)
        {
            if (screen_y + j - 12 >= 65
                                || screen_y + j - 12 <= 4)
            {
                buffer2[bufcount2 + 1] = DARKGREY;
                buffer2[bufcount2] = 0;
                bufcount2 += 2;
#ifdef PLAIN_TERM
                goto print_it;
#endif

#ifdef DOS_TERM
                continue;
#endif
            }

            square = grd[i + 1][j + screen_y - 11];
            switch (square)
            {
            case 75:            // dart trap
                buffer2[bufcount2 + 1] = LIGHTCYAN;
                break;

            case 76:            // trap?
                buffer2[bufcount2 + 1] = MAGENTA;
                break;

            case 77:            // trap?
                buffer2[bufcount2 + 1] = MAGENTA;
                break;

            case 80:            // shop?
                buffer2[bufcount2 + 1] = YELLOW;
                break;

            case 92:            // dis
                buffer2[bufcount2 + 1] = CYAN;
                break;

            case 69:            // hell
            case 93:            // gehenne
                buffer2[bufcount2 + 1] = RED;
                break;

            case 94:            // cocytus
                buffer2[bufcount2 + 1] = LIGHTCYAN;
                break;

            case 96:            // abyss
                buffer2[bufcount2 + 1] = random2(16);
                break;

            case 81:            // labyrinth
            case 98:            // closed hell gates
                buffer2[bufcount2 + 1] = LIGHTGREY;
                break;


            case 99:            // to pandemonium
                buffer2[bufcount2 + 1] = LIGHTBLUE;
                break;

            case 100:           // out of
            case 101:           // to another part
                // These are Pandemonium gates, I'm using light
                // green for all types of gates as to maintain
                // the fact that the player should have to
                // visit them to verify what they are...
                // not just use a crystal ball. -- bwross
                buffer2[bufcount2 + 1] = LIGHTGREEN;
                break;

            case 117:           // realm of zot?
            case 137:           // realm of zot?
                buffer2[bufcount2 + 1] = MAGENTA;
                break;

            case 82:            // down stairs
            case 83:
            case 84:
            case 85:
                buffer2[bufcount2 + 1] = RED;
                break;

            case 86:            // up stairs
            case 87:
            case 88:
            case 89:
                buffer2[bufcount2 + 1] = BLUE;
                break;

            case 110:           // down stairs?
            case 111:
            case 112:
            case 113:
            case 114:
            case 115:
            case 116:
            case 118:
            case 119:
            case 120:
            case 121:
            case 122:
            case 123:
            case 124:
            case 125:
            case 126:
                buffer2[bufcount2 + 1] = LIGHTRED;
                break;

            case 130:           // orcish mine stairs?
            case 131:
            case 132:
            case 133:
            case 134:
            case 135:
            case 136:
            case 138:
            case 139:
            case 140:
            case 141:
            case 142:
            case 143:
            case 144:
            case 145:
            case 146:
                buffer2[bufcount2 + 1] = LIGHTBLUE;
                break;

            default:
                buffer2[bufcount2 + 1] = DARKGREY;
            }

            if (i == you.x_pos - 1
                            && j + screen_y - 11 == you.y_pos)
            {
                buffer2[bufcount2 + 1] = WHITE;
            }

            buffer2[bufcount2] = env.map[i][j + screen_y - 12];
            bufcount2 += 2;

#ifdef PLAIN_TERM
          print_it:
            if (j == NUMBER_OF_LINES - 1 && i == 79)
                continue;

            if (i == 79)
            {
                cprintf(EOL);
                continue;
            }                   /* needed for screens >80 width */

            textcolor(buffer2[bufcount2 - 1]);
            putch(buffer2[bufcount2 - 2]);
#endif
        }
    }

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer2);
#endif

    gotoxy(curs_x, curs_y);

gettything:
    getty = getch();

#ifdef LINUX
    getty = translate_keypad( getty );
#endif

    if (spec_place[0] == 0 && getty != 0 && getty != '+' && getty != '-'
            && getty != 'h' && getty != 'j' && getty != 'k' && getty != 'l'
            && getty != 'y' && getty != 'u' && getty != 'b' && getty != 'n'
            && (getty < '0' || getty > '9'))
        goto putty;

    if (spec_place[0] == 1 && getty != 0 && getty != '+' && getty != '-'
            && getty != 'h' && getty != 'j' && getty != 'k' && getty != 'l'
            && getty != 'y' && getty != 'u' && getty != 'b' && getty != 'n'
            && getty != '.' && getty != 'S' && (getty < '0' || getty > '9'))
        goto gettything;

    if (getty == 0)
        getty = getch();

    switch (getty)
    {
    case 'b':
    case '1':
        move_x = -1;
        move_y = 1;
        break;

    case 'j':
    case '2':
        move_y = 1;
        move_x = 0;
        break;

    case 'u':
    case '9':
        move_x = 1;
        move_y = -1;
        break;

    case 'k':
    case '8':
        move_y = -1;
        move_x = 0;
        break;

    case 'y':
    case '7':
        move_y = -1;
        move_x = -1;
        break;

    case 'h':
    case '4':
        move_x = -1;
        move_y = 0;
        break;

    case 'n':
    case '3':
        move_y = 1;
        move_x = 1;
        break;

    case 'l':
    case '6':
        move_x = 1;
        move_y = 0;
        break;

#ifndef LINUX
    // This is old DOS keypad support
    case 'H':
        move_y = -1;
        move_x = 0;
        break;
    case 'P':
        move_y = 1;
        move_x = 0;
        break;
    case 'K':
        move_x = -1;
        move_y = 0;
        break;
    case 'M':
        move_x = 1;
        move_y = 0;
        break;

    case 'O':
        move_x = -1;
        move_y = 1;
        break;
    case 'I':
        move_x = 1;
        move_y = -1;
        break;
    case 'G':
        move_y = -1;
        move_x = -1;
        break;
    case 'Q':
        move_y = 1;
        move_x = 1;
        break;
#endif

    case '+':
        move_y = 20;
        move_x = 0;
        break;
    case '-':
        move_y = -20;
        move_x = 0;
        break;
    case '.':
    case '\r':
    case 'S':
        spec_place[0] = curs_x;
        spec_place[1] = screen_y + curs_y - 12;
        goto putty;
        break;
    default:
        move_x = 0;
        move_y = 0;
        break;
    }

    if (curs_x + move_x < 1 || curs_x + move_x > 75)
        move_x = 0;

    curs_x += move_x;

    if (getty == '-' || getty == '+')
    {
        if (getty == '-')
            screen_y -= 20;

        if (screen_y <= 11 + min_y)
            screen_y = 11 + min_y;

        if (getty == '+')
            screen_y += 20;

        if (screen_y >= max_y - 11)
            screen_y = max_y - 11;

        goto put_screen;
    }

    if (curs_y + move_y < 1)
    {
        if (screen_y > 11 + min_y)
        {
            screen_y--;
            goto put_screen;
        }
        else
            move_y = 0;
    }

    if (curs_y + move_y > NUMBER_OF_LINES - 1)
    {
        if (screen_y < max_y - 11)
        {
            screen_y++;
            goto put_screen;
        }
        else
            move_y = 0;
    }

    curs_y += move_y;
    goto put_screen;

putty:
#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif

    return;
}


void magic_mapping(int map_radius, int proportion)
{
    int i, j, k, l, empty_count;

    if (map_radius >= 50)
        map_radius = 50;

    for (i = you.x_pos - map_radius; i < you.x_pos + map_radius; i++)
    {
        for (j = you.y_pos - map_radius; j < you.y_pos + map_radius; j++)
        {
            if (random2(100) > proportion)
                continue;       // note that proportion can be over 100

            if (i < 5 || j < 5 || i > 75 || j > 65)
                continue;
            if (env.map[i][j] == mapch2(grd[i + 1][j + 1]))
                continue;
            empty_count = 8;
            if (grd[i][j] <= 60 && grd[i][j] != 3)
                for (k = 0; k < 3; k++)
                {
                    for (l = 0; l < 3; l++)
                    {
                        if (k == 1 && l == 1)
                            continue;
                        if (grd[i + k][j + l] <= 60 && grd[i + k][j + l] != 3)
                            empty_count--;
                    }
                }
            if (empty_count > 0)
                env.map[i][j] = mapch(grd[i + 1][j + 1]);
        }
    }


}

/* mapchars 3 & 4 are for non-ibm char sets */

unsigned char mapchar(unsigned char ldfk)
{
    unsigned char showed = 0;

    switch (ldfk)
    {
    case 0:
        showed = 0;
        break;
    case 5:                     // secret door

    case 1:
        showed = 176;
        break;                  // rock wall

    case 2:
        showed = 176;
        break;                  // stone wall

    case 3:
        showed = 206;
        break;                  // door

    case 4:
    case 6:
    case 8:
        showed = 176;
        break;
    case 11:
        showed = 247;
        break;
    case 12:
        showed = 247;
        break;
    case 20:
        showed = '8';
        break;                  // orcish idol

    case 21:
        showed = '8';
        break;                  // silver statue

    case 22:
        showed = '8';
        break;                  // granite statue

    case 23:
        showed = '8';
        break;                  //

    case 24:
        showed = '8';
        break;                  //

    case 25:
        showed = '8';
        break;                  //

    case 61:
    case 62:
    case 65:
        showed = 247;
        break;
    case 67:
        showed = 250;
        break;
        //case 68: showed = '>'; break; // < (60)
    case 69:
        showed = 239;
        break;                  // >

    case 70:
        showed = 39;
        break;                  // open door
        //case 72: showed = '<'; break;

    case 75:
        showed = 94;
        break;                  // ^ dart trap

    case 76:
        showed = 94;
        break;                  // ^ magic trap

    case 77:
        showed = 94;
        break;
    case 78:
        showed = 250;
        break;                  // undiscovered trap

    case 81:
    case 80:
        showed = 239;
        break;                  // shop

    case 82:
    case 83:
    case 84:
    case 85:
    case 110:
    case 111:
    case 112:
    case 113:
    case 114:
    case 115:
    case 116:
    case 118:
    case 119:
    case 120:
    case 121:
    case 122:
    case 123:
    case 124:
    case 125:
    case 126:
        showed = '>';
        break;

    case 86:
    case 87:
    case 88:
    case 89:
    case 130:
    case 131:
    case 132:
    case 133:
    case 134:
    case 135:
    case 136:
    case 138:
    case 139:
    case 140:
    case 141:
    case 142:
    case 143:
    case 144:
    case 145:
    case 146:
        showed = '<';
        break;

    case 92:
    case 93:
    case 94:
    case 95:
    case 96:
    case 97:
    case 98:
    case 99:
    case 100:
    case 101:
    case 117:
    case 137:
        showed = 239;
        break;

    case 180:
    case 181:
    case 182:
    case 183:
    case 184:
    case 185:
    case 186:
    case 187:
    case 188:
    case 189:
    case 190:
    case 191:
        showed = 220;
        break;                  /* Altars */

    case 200:
    case 201:
    case 202:
    case 203:
    case 204:
    case 205:
    case 206:
    case 207:
    case 208:
    case 209:
    case 210:
        showed = 159;
        break;                  /* fountain */

    default:
        showed = 0;
        break;
    }

    return showed;

}

unsigned char mapchar2(unsigned char ldfk)
{
    unsigned char showed = 0;

    switch (ldfk)
    {
    case 0:
        showed = 0;
        break;
    case 5:                     // secret door

    case 1:
        showed = 177;
        break;                  // rock wall

    case 2:
        showed = 177;
        break;                  // stone wall

    case 3:
        showed = 254;
        break;                  // door

    case 4:
    case 6:
    case 8:
        showed = 177;
        break;
/*  case 11: showed = 247; break;
   case 12: showed = 247; break; */
    case 20:
        showed = '8';
        break;                  // orcish idol

    case 21:
        showed = '8';
        break;                  // silver statue

    case 22:
        showed = '8';
        break;                  // granite statue

    case 23:
        showed = '8';
        break;                  //

    case 24:
        showed = '8';
        break;                  //

    case 25:
        showed = '8';
        break;                  //

    case 61:
    case 62:
    case 65:
        showed = 247;
        break;
    case 67:
        showed = 249;
        break;
    case 68:
        showed = '>';
        break;                  // <

    case 69:
        showed = 239;
        break;                  // >

    case 70:
        showed = 39;
        break;                  // open door

    case 72:
        showed = '<';
        break;                  // <

    case 75:
        showed = 94;
        break;                  // ^ dart trap

    case 76:
        showed = 94;
        break;                  // ^ magic trap

    case 77:
        showed = 94;
        break;
    case 78:
        showed = 249;
        break;                  // undiscovered trap

    case 81:
    case 80:
        showed = 239;
        break;                  // shop

    case 82:
    case 83:
    case 84:
    case 85:
    case 110:
    case 111:
    case 112:
    case 113:
    case 114:
    case 115:
    case 116:
    case 118:
    case 119:
    case 120:
    case 121:
    case 122:
    case 123:
    case 124:
    case 125:
    case 126:
        showed = '>';
        break;

    case 86:
    case 87:
    case 88:
    case 89:
    case 130:
    case 131:
    case 132:
    case 133:
    case 134:
    case 135:
    case 136:
    case 138:
    case 139:
    case 140:
    case 141:
    case 142:
    case 143:
    case 144:
    case 145:
    case 146:
        showed = '<';
        break;

    case 92:
    case 93:
    case 94:
    case 95:
    case 96:
    case 97:
    case 98:
    case 99:
    case 100:
    case 101:
    case 117:
    case 137:
        showed = 239;
        break;

    case 180:
    case 181:
    case 182:
    case 183:
    case 184:
    case 185:
    case 186:
    case 187:
    case 188:
    case 189:
    case 190:
    case 191:
        showed = 220;
        break;                  /* Altars */

    case 200:
    case 201:
    case 202:
    case 203:
    case 204:
    case 205:
    case 206:
    case 207:
    case 208:
    case 209:
    case 210:
        showed = 159;
        break;                  /* fountain */
    default:
        showed = 0;
        break;
    }

    return showed;

}


bool mons_near(unsigned char monst)
{

    if (menv[monst].x > you.x_pos - 9 && menv[monst].x < you.x_pos + 9 && menv[monst].y > you.y_pos - 9 && menv[monst].y < you.y_pos + 9)
    {
        if (env.show[menv[monst].x - you.x_pos + 9][menv[monst].y - you.y_pos + 9] != 0)
            return true;
    }

    return false;

}                               // end of char mons_near(char)



/*
   This is the viewwindow function for computers without IBM graphic displays.
   It is activated by a command line argument, which sets a function pointer.
 */
void viewwindow3(char draw_it, bool do_updates)
{

    int bufcount = 0;
    unsigned char buffy[1500];  //[800]; //392];

    unsigned char showed = 0;

    int count_x, count_y;

    _setcursortype(_NOCURSOR);

    losight(env.show, env.grid, you.x_pos, you.y_pos);


    for (count_x = 0; count_x < 18; count_x++)
    {
        for (count_y = 0; count_y < 18; count_y++)
        {
            env.show_col[count_x][count_y] = 7;
        }
    }

    item();
    cloud_grid();
    monster_grid( do_updates );
    bufcount = 0;


    if (draw_it == 1)
    {

        for (count_y = (you.y_pos - 8); (count_y < you.y_pos + 9); count_y++)
        {
            bufcount += 16;
            for (count_x = (you.x_pos - 8); (count_x < you.x_pos + 9); count_x++)
            {
                buffy[bufcount + 1] = env.show_col[count_x - you.x_pos + 9][count_y - you.y_pos + 9];


                if (count_x != you.x_pos || count_y != you.y_pos)
                {



                    switch (env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9])
                    {

                    case 0:
                        showed = 0;
                        break;
                    case 1:
                        buffy[bufcount + 1] = env.rock_colour;
                        showed = '#';
                        break;  // rock wall - remember earth elementals

                    case 2:
                        if (you.where_are_you == 17)
                            buffy[bufcount + 1] = env.rock_colour;
                        else
                            buffy[bufcount + 1] = LIGHTGREY;
                        showed = '#';
                        break;  // stone wall

                    case 3:
                        showed = '+';
                        break;
                    case 4:
                        showed = '#';
                        buffy[bufcount + 1] = CYAN;
                        break;
                    case 5:
                        showed = '#';   // hidden secret door

                        buffy[bufcount + 1] = env.rock_colour;
                        break;
                    case 6:
                        showed = '#';
                        buffy[bufcount + 1] = GREEN;
                        break;  // green crystal wall

                    case 7:
                        showed = '8';
                        buffy[bufcount + 1] = DARKGREY;         // orcish idol

                        break;
                    case 8:
                        showed = '#';
                        buffy[bufcount + 1] = YELLOW;
                        break;  // wax wall

                    case 21:
                        showed = '8';
                        buffy[bufcount + 1] = WHITE;    // silver statue

                        if (visible[1] == 0)
                            visible[1] = 3;
                        else
                            visible[1] = 2;
                        visible[0] = 2;
                        break;
                    case 22:
                        showed = '8';
                        buffy[bufcount + 1] = LIGHTGREY;        // granite statue

                        break;
                    case 23:
                        showed = '8';
                        buffy[bufcount + 1] = LIGHTRED;         // orange crystal statue

                        if (visible[2] == 0)
                            visible[2] = 3;
                        else
                            visible[2] = 2;
                        visible[0] = 2;
                        break;
                    case 35:
                        showed = '#';
                        break;
                    case 61:
                        showed = '{';
                        buffy[bufcount + 1] = RED;      // lava!

                        break;
                    case 62:
                        showed = '{';   // this wavy thing also used for water elemental
                        // note that some monsters which use IBM graphics aren't set for this function - too tricky for now.

                        buffy[bufcount + 1] = BLUE;     // water

                        break;
                    case 65:
                        showed = '{';
                        buffy[bufcount + 1] = CYAN;     // shallow water

                        break;

                    case 67:
                        buffy[bufcount + 1] = env.floor_colour;         //LIGHTGREY;

                        showed = '.';
                        break;
                    case 69:
                        showed = '\\';
                        buffy[bufcount + 1] = RED;
                        break;  // staircase to hell

                    case 70:
                        showed = 39;
                        break;  // open door

                    case 71:
                        showed = '>';
                        buffy[bufcount + 1] = BROWN;
                        break;  // branch staircase

                    case 75:
                        buffy[bufcount + 1] = 11;
                        showed = 94;
                        break;  // ^ dart trap

                    case 76:
                        buffy[bufcount + 1] = MAGENTA;
                        showed = 94;
                        break;
                    case 77:
                        buffy[bufcount + 1] = LIGHTGREY;
                        showed = 94;
                        break;
                    case 78:
                        showed = '.';
                        buffy[bufcount + 1] = env.floor_colour;
                        break;  // undiscovered trap

                    case 80:
                        showed = '\\';
                        buffy[bufcount + 1] = YELLOW;
                        break;
// if I change anything above here, must also change magic mapping!
                    case 81:
                        showed = '\\';
                        buffy[bufcount + 1] = LIGHTGREY;
                        break;  // labyrinth

                    case 85:
                        buffy[bufcount + 1] = BROWN;    // ladder

                    case 82:
                    case 83:
                    case 84:
                        showed = '>';
                        break;
                    case 89:
                        buffy[bufcount + 1] = BROWN;    // ladder

                    case 86:
                    case 87:
                    case 88:
                        showed = '<';
                        break;

                    case 92:
                        buffy[bufcount + 1] = CYAN;
                        showed = '\\';
                        break;  // Stairway to Dis

                    case 93:
                        buffy[bufcount + 1] = RED;
                        showed = '\\';
                        break;  // Gehenna

                    case 94:
                        buffy[bufcount + 1] = LIGHTCYAN;
                        showed = '\\';
                        break;  // Cocytus

                    case 95:
                        buffy[bufcount + 1] = DARKGREY;
                        showed = '\\';
                        break;  // Tartarus

                    case 96:
                        buffy[bufcount + 1] = random2(16);
                        showed = '\\';
                        break;  // To Abyss

                    case 97:
                        buffy[bufcount + 1] = random2(16);
                        showed = '\\';
                        break;  // From Abyss

                    case 98:
                        buffy[bufcount + 1] = LIGHTGREY;
                        showed = '\\';
                        break;  // Closed gate to hell

                    case 99:
                        buffy[bufcount + 1] = LIGHTBLUE;
                        showed = '\\';
                        break;  // gate to pandemonium

                    case 100:
                        buffy[bufcount + 1] = LIGHTBLUE;
                        showed = '\\';
                        break;  // gate out of pandemonium

                    case 101:
                        buffy[bufcount + 1] = LIGHTGREEN;
                        showed = '\\';
                        break;  // gate to other part of pandemonium

                    case 110:
                    case 111:
                    case 112:
                    case 113:
                    case 114:
                    case 115:
                    case 118:
                    case 119:
                    case 120:
                    case 121:
                    case 122:
                    case 123:
                    case 124:
                    case 125:
                    case 126:
                    case 116:
                        buffy[bufcount + 1] = YELLOW;
                        showed = '>';
                        break;  // stair to orc mine

                    case 117:
                        buffy[bufcount + 1] = MAGENTA;
                        showed = '\\';
                        break;

                    case 130:
                    case 131:
                    case 132:
                    case 133:
                    case 134:
                    case 135:
                    case 136:
                    case 138:
                    case 139:
                    case 140:
                    case 141:
                    case 142:
                    case 143:
                    case 144:
                    case 145:
                    case 146:
                        buffy[bufcount + 1] = YELLOW;
                        showed = '<';
                        break;  // stairs out of sub-dungeons

                    case 137:
                        buffy[bufcount + 1] = MAGENTA;
                        showed = '\\';
                        break;



                    case 180:
                        buffy[bufcount + 1] = WHITE;
                        showed = '_';
                        break;  /* Altar to Zin */
                    case 181:
                        buffy[bufcount + 1] = YELLOW;
                        showed = '_';
                        break;  /* Altar to TSO */
                    case 182:
                        buffy[bufcount + 1] = DARKGREY;
                        showed = '_';
                        break;  /* Altar to Kiku */
                    case 183:
                        buffy[bufcount + 1] = DARKGREY;
                        if (random2(3) == 0)
                            buffy[bufcount + 1] = RED;
                        showed = '_';
                        break;  /* Altar to Yredelemnul */
                    case 184:
                        buffy[bufcount + 1] = random2(15) + 1;
                        showed = '_';
                        break;  /* Altar to Xom */
                    case 185:
                        buffy[bufcount + 1] = LIGHTBLUE;
                        if (random2(3) == 0)
                            buffy[bufcount + 1] = LIGHTMAGENTA;
                        if (random2(3) == 0)
                            buffy[bufcount + 1] = LIGHTRED;
                        showed = '_';
                        break;  /* Altar to Vehumet */
                    case 186:
                        buffy[bufcount + 1] = CYAN;
                        showed = '_';
                        break;  /* Altar to Okawaru */
                    case 187:
                        buffy[bufcount + 1] = RED;
                        if (random2(3) == 0)
                            buffy[bufcount + 1] = LIGHTRED;
                        if (random2(3) == 0)
                            buffy[bufcount + 1] = YELLOW;
                        showed = '_';
                        break;  /* Altar to Makhleb */
                    case 188:
                        buffy[bufcount + 1] = BLUE;
                        showed = '_';
                        break;  /* Altar to Sif Muna */
                    case 189:
                        buffy[bufcount + 1] = RED;
                        showed = '_';
                        break;  /* Altar to Trog */
                    case 190:
                        buffy[bufcount + 1] = LIGHTMAGENTA;
                        showed = '_';
                        break;  /* Altar to Nemelex */
                    case 191:
                        buffy[bufcount + 1] = LIGHTGREY;
                        showed = '_';
                        break;  /* Altar to Elyvilon */

                    case 200:
                        buffy[bufcount + 1] = BLUE;
                        showed = '}';
                        break;  /* Fountain */
                    case 201:
                        buffy[bufcount + 1] = LIGHTGREY;
                        showed = '}';
                        break;  /* dry fountain */
                    case 202:
                        buffy[bufcount + 1] = LIGHTBLUE;
                        showed = '}';
                        break;  /* Magic fountain */
                    case 203:
                        buffy[bufcount + 1] = LIGHTGREY;
                        showed = '}';
                        break;  /* dry fountain */

                    case 210:
                        buffy[bufcount + 1] = LIGHTGREY;
                        showed = '}';
                        break;  /* permenantly dry fountain */

                    case 256:
                        showed = '0';
                        break;

                    case 257:
                        buffy[bufcount + 1] = CYAN;
                        showed = '~';
                        break;  /* Invis creature walking through water */

                    case 258:
                        showed = 41;
                        break;  // weapon )

                    case 259:
                        showed = 91;
                        break;  // armour [

                    case 260:
                        showed = 47;
                        break;  // food %

                    case 261:
                        showed = 37;
                        break;  // wands / &c

                    case 262:
                        showed = 43;
                        break;  // books +

                    case 263:
                        showed = 63;
                        break;  // scroll ?

                    case 264:
                        showed = 61;
                        break;  // ring = etc

                    case 265:
                        showed = 33;
                        break;  // potions !

                    case 266:
                        showed = 40;
                        break;  // stones

                    case 267:
                        showed = ':';
                        break;  // book +

                    case 268:
                        showed = 37;
                        break;  // corpses part 1

                    case 269:
                        showed = '|';
                        break;  // magical staves

                    case 270:
                        showed = '}';
                        break;  // gems

                    case 271:
                        showed = '%';
                        break;  // don't know ?

                    case 272:
                        showed = 36;
                        break;  // $ gold

                    case 273:
                        showed = '"';
                        break;  // amulet

                    default:
                        int mnr = env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9];

                        if (mnr >= 297)
                        {
                            showed = mons_char(mnr - 297);      // yeah

                        }
                        else
                            showed = env.show[count_x - you.x_pos + 9][count_y - you.y_pos + 9];
                        break;

                    }

                }
                else
                {
                    showed = your_sign;
                    buffy[bufcount + 1] = your_colour;
                }


                buffy[bufcount] = showed;
// have the colour defined before the switch, because some cases change it.

                bufcount += 2;
            }
            bufcount += 16;
        }
        bufcount = 0;

        if (you.level_type != 1 && you.level_type != 2)
        {
            for (count_y = 0; count_y < 17; count_y++)
            {
                bufcount += 16;
                for (count_x = 0; count_x < 17; count_x++)
                {
                    if (buffy[bufcount] != 0)
                        env.map[count_x + you.x_pos - 9][count_y + you.y_pos - 9] = buffy[bufcount];
                    bufcount += 2;
                }
                bufcount += 16;
            }
        }


        bufcount = 0;
        for (count_y = 0; count_y < 17; count_y++)
        {
            for (count_x = 0; count_x < 33; count_x++)
            {
                if (count_x + you.x_pos - 17 < 3 || count_y + you.y_pos - 9 < 3 || count_x + you.x_pos - 14 > 77 || count_y + you.y_pos - 9 > 67)
                {
                    buffy[bufcount] = 0;
                    bufcount++;
                    buffy[bufcount] = 0;
                    bufcount++;
                    continue;
                }
                if (count_x >= 8 && count_x <= 24 && count_y >= 0 && count_y <= 16 && buffy[bufcount] != 0)
                {
                    bufcount += 2;
                    continue;
                }
                buffy[bufcount] = env.map[count_x + you.x_pos - 17][count_y + you.y_pos - 9];
                buffy[bufcount + 1] = DARKGREY;
                bufcount += 2;
            }
        }

        if (you.berserker != 0)
        {
            for (count_x = 1; count_x < 1400; count_x += 2)
            {
                if (buffy[count_x] != DARKGREY)
                    buffy[count_x] = RED;
            }
        }

        if (show_green != 0)
        {
            for (count_x = 1; count_x < 1400; count_x += 2)
            {
                if (buffy[count_x] != DARKGREY)
                    buffy[count_x] = show_green;
            }
            show_green = 0;
            if (you.special_wield == 50)
                show_green = DARKGREY;
        }



#ifdef DOS_TERM
        puttext(2, 1, 34, 17, buffy);
#endif

#ifdef PLAIN_TERM
        gotoxy(2, 1);
        bufcount = 0;

        if (you.running == 0)   // this line is purely optional

        {
            for (count_x = 0; count_x < 1120; count_x += 2)     // 1056

            {
                textcolor(buffy[count_x + 1]);
                putch(buffy[count_x]);
                if (count_x % 66 == 64 && count_x > 0)
#ifdef DOS_TERM
                    cprintf(EOL " ");
#endif
#ifdef PLAIN_TERM
                gotoxy(2, wherey() + 1);
#endif

            }
        }
#endif

    }                           // end of (if brek...)

}



unsigned char mapchar3(unsigned char ldfk)
{
    unsigned char showed = 0;

    switch (ldfk)
    {
    case 0:
        showed = 0;
        break;
    case 5:                     // secret door

    case 1:
        showed = '*';
        break;                  // rock wall

    case 2:
        showed = '*';
        break;                  // stone wall

    case 3:
        showed = '+';
        break;                  // door

    case 4:
    case 6:
    case 8:
        showed = '*';
        break;
    case 11:
        showed = '{';
        break;
    case 12:
        showed = '{';
        break;
    case 20:
        showed = '8';
        break;                  // orcish idol

    case 21:
        showed = '8';
        break;                  // silver statue

    case 22:
        showed = '8';
        break;                  // granite statue

    case 23:
        showed = '8';
        break;                  //

    case 24:
        showed = '8';
        break;                  //

    case 25:
        showed = '8';
        break;                  //

    case 61:
    case 62:
    case 65:
        showed = '{';
        break;
    case 67:
        showed = ',';
        break;
        //case 68: showed = '>'; break; // < (60)
    case 69:
        showed = '\\';
        break;                  // >

    case 70:
        showed = 39;
        break;                  // open door
        //case 72: showed = '<'; break;

    case 75:
        showed = 94;
        break;                  // ^ dart trap

    case 76:
        showed = 94;
        break;                  // ^ magic trap

    case 77:
        showed = 94;
        break;
    case 78:
        showed = ',';
        break;                  // undiscovered trap

    case 81:
    case 80:
        showed = '\\';
        break;                  // shop

    case 82:
    case 83:
    case 84:
    case 85:
    case 110:
    case 111:
    case 112:
    case 113:
    case 114:
    case 115:
    case 116:
    case 118:
    case 119:
    case 120:
    case 121:
    case 122:
    case 123:
    case 124:
    case 125:
    case 126:
        showed = '>';
        break;

    case 86:
    case 87:
    case 88:
    case 89:
    case 130:
    case 131:
    case 132:
    case 133:
    case 134:
    case 135:
    case 136:
    case 138:
    case 139:
    case 140:
    case 141:
    case 142:
    case 143:
    case 144:
    case 145:
    case 146:
        showed = '<';
        break;

    case 92:
    case 93:
    case 94:
    case 95:
    case 96:
    case 97:
    case 98:
    case 99:
    case 100:
    case 101:
    case 117:
    case 137:
        showed = '\\';
        break;

    case 180:
    case 181:
    case 182:
    case 183:
    case 184:
    case 185:
    case 186:
    case 187:
    case 188:
    case 189:
    case 190:
    case 191:
        showed = '_';
        break;                  /* Altars */

    case 200:
    case 201:
    case 202:
    case 203:
    case 204:
    case 205:
    case 206:
    case 207:
    case 208:
    case 209:
    case 210:
        showed = '}';
        break;                  /* fountain */

    default:
        showed = 0;
        break;
    }

    return showed;

}

unsigned char mapchar4(unsigned char ldfk)
{
    unsigned char showed = 0;

    switch (ldfk)
    {
    case 0:
        showed = 0;
        break;
    case 5:                     // secret door

    case 1:
        showed = '#';
        break;                  // rock wall

    case 2:
        showed = '#';
        break;                  // stone wall

    case 3:
        showed = '+';
        break;                  // door

    case 4:
    case 6:
    case 8:
        showed = '#';
        break;
    case 11:
        showed = '{';
        break;
    case 12:
        showed = '{';
        break;
    case 20:
        showed = '8';
        break;                  // orcish idol

    case 21:
        showed = '8';
        break;                  // silver statue

    case 22:
        showed = '8';
        break;                  // granite statue

    case 23:
        showed = '8';
        break;                  //

    case 24:
        showed = '8';
        break;                  //

    case 25:
        showed = '8';
        break;                  //

    case 61:
    case 62:
    case 65:
        showed = '{';
        break;
    case 67:
        showed = '.';
        break;
    case 68:
        showed = '>';
        break;                  // <

    case 69:
        showed = '\\';
        break;                  // >

    case 70:
        showed = 39;
        break;                  // open door

    case 72:
        showed = '<';
        break;                  // <

    case 75:
        showed = 94;
        break;                  // ^ dart trap

    case 76:
        showed = 94;
        break;                  // ^ magic trap

    case 77:
        showed = 94;
        break;
    case 78:
        showed = '.';
        break;                  // undiscovered trap

    case 81:
    case 80:
        showed = '\\';
        break;                  // shop

    case 82:
    case 83:
    case 84:
    case 85:
    case 110:
    case 111:
    case 112:
    case 113:
    case 114:
    case 115:
    case 116:
    case 118:
    case 119:
    case 120:
    case 121:
    case 122:
    case 123:
    case 124:
    case 125:
    case 126:
        showed = '>';
        break;

    case 86:
    case 87:
    case 88:
    case 89:
    case 130:
    case 131:
    case 132:
    case 133:
    case 134:
    case 135:
    case 136:
    case 138:
    case 139:
    case 140:
    case 141:
    case 142:
    case 143:
    case 144:
    case 145:
    case 146:
        showed = '<';
        break;

    case 92:
    case 93:
    case 94:
    case 95:
    case 96:
    case 97:
    case 98:
    case 99:
    case 100:
    case 101:
    case 117:
    case 137:
        showed = '\\';
        break;

    case 180:
    case 181:
    case 182:
    case 183:
    case 184:
    case 185:
    case 186:
    case 187:
    case 188:
    case 189:
    case 190:
    case 191:
        showed = '_';
        break;                  /* Altars */

    case 200:
    case 201:
    case 202:
    case 203:
    case 204:
    case 205:
    case 206:
    case 207:
    case 208:
    case 209:
    case 210:
        showed = '}';
        break;                  /* fountain */
    default:
        showed = 0;
        break;
    }

    return showed;

}

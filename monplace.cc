#include "config.h"

#include "externs.h"

#include "dungeon.h"
#include "stuff.h"

char empty_surrounds(int emx, int emy, unsigned char spc_wanted, char allow_centre, char empty [2]);


int mons_place(int typed, int type_place, int px, int py, char behaviour, int hitting, unsigned char plus_seventy, int lev_mons)
{
int passed [2];
passed [0] = 0;
passed [1] = 0;

char bands = 1;
int nomons = 0;
int il;

for (il = 0; il < MNST; il ++)
{
 if (menv [il].m_class != -1) nomons ++;
}

if (typed == 2500) /* random placement */
{
 if (nomons > 150) return -1;
 typed = 250;
}

if (nomons > MNST - 2) return -1;

if (you[0].char_direction == 1 && typed == 250 && you[0].level_type == 0)
{
 typed = 80 + random2(10);
 if (random2(5) == 0) typed = 3;
 if (random2(5) == 0) typed = 73;
 if (random2(7) == 0) typed = 23;
 if (random2(7) == 0) typed = 49;
 if (random2(13) == 0) typed = 245;
 if (random2(3) == 0) typed = 220 + random2(15);
}

if (typed != 250 && lev_mons != 52) bands = 0;

/*if (no_mons == MNST - 1) return 0;*/

int glokp = place_monster(
plus_seventy,
typed, type_place, px, py, behaviour, hitting,
bands,
lev_mons, passed
);


/*
place_monster(
it_no,
you[0].unique_items,
you[0].item_description,
property,
plus_seventy, /* used for mons_sec for zombies * /
typed,
type_place,
px,
py,
behaviour,
hitting,
bands,
gmon_use,
grd,
passed,
you[0].x_pos,
you[0].y_pos,
mgrd,
menv,
unique_creatures,
it,
lev_mons
);
*/
//no_mons += glokp;
//mn_no += glokp;

if (glokp > 0) return passed [0];
                else return -1;


} /* end of mons_place() */


















int create_monster(int cls, int dur, int beha, int cr_x, int cr_y, int hitting, int zsec)
{
int pets = 0;
int summd = 0;

int dem_beha = beha;

if (beha == 100) beha = 7;

unsigned char spcw = 67;

if (cls >= MLAVA0) spcw = 61;
if (cls >= MWATER0) spcw = 62;

char empty [2];

if (empty_surrounds(cr_x, cr_y, spcw, 1, empty) == 0)
{
 puffy :
 if (see_grid(cr_x, cr_y) == 1)
 {
  strcpy(info, "You see a puff of smoke.");
  mpr(info);
 }
 return -1;
} else
       {
         summd = mons_place(cls, 1, empty [0], empty [1], beha, hitting, zsec, you[0].your_level);
         if (summd == -1) goto puffy;
         goto bkout;
       } /* end else */


bkout : pets = 0;

if (dem_beha == 100) menv [summd].m_ench [0] = 30;

if (dur != 0)
{
 menv [summd].m_ench [1] = dur; /* some monsters, eg butterflies, use [0] for confusion */
 menv [summd].m_ench_1 = 1;
}

if (menv [summd].m_class == 78 && random2(3) != 0)
{
 menv [summd].m_ench [2] = 6; /* fake R - designed to annoy */
 menv [summd].m_ench_1 = 1;
}

return summd;

}



char empty_surrounds(int emx, int emy, unsigned char spc_wanted, char allow_centre, char empty [2])
{

int count_x, count_y;

char minx = -1;
char maxx = 3;
char miny = -1;
char maxy = 2;
char xinc = 1;
char yinc = 1;

if (random2(2) == 0)
{
        minx = 1; maxx = -3; xinc = -1;
}

if (random2(2) == 0)
{
        miny = 1; maxy = -2; yinc = -1;
}


for (count_x = minx; count_x != maxx; count_x += xinc)
{

   if (count_x == 2 | count_x == -2)
   {
     return 0;
   }

   for (count_y = miny; count_y != maxy; count_y += yinc)
   {

      if (count_x == 0 && count_y == 0 && allow_centre == 0) continue;
      if (emx + count_x == you[0].x_pos && emy + count_y == you[0].y_pos) continue;

      if (spc_wanted != 67)
      {
        if (grd [emx + count_x] [emy + count_y] == spc_wanted && mgrd [emx + count_x] [emy + count_y] == MNG)
         goto bkout;
      } else
             if (grd [emx + count_x] [emy + count_y] >= 67 && mgrd [emx + count_x] [emy + count_y] == MNG)
             {
                goto bkout;
             }

   } /* end of count_y */
} /* end of count_x */

bkout:
empty [0] = emx + count_x;
empty [1] = emy + count_y;
return 1;

} /* end empty surrounds */

#include "config.h"

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#ifdef DOS
#include <file.h>
#endif
#ifdef LINUX
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

//#include "bang.h"
//#include "beam.h"
//#include "chardump.h"
//#include "command.h"
//#include "debug.h"
//#include "decks.h"
//#include "describe.h"
//#include "direct.h"
#include "dungeon.h"
//#include "effects.h"
//#include "fight.h"
#include "files.h"
//#include "food.h"
//#include "invent.h"
//#include "it_use2.h"
//#include "it_use3.h"
//#include "item_use.h"
#include "itemname.h"
//#include "items.h"
//#include "levels.h"
//#include "maps.h"
#include "message.h"
#include "misc.h"
//#include "monplace.h"
//#include "monstuff.h"
#include "mstruct.h"
#include "mstuff2.h"
//#include "newgame.h"
//#include "ouch.h"
#include "output.h"
#include "player.h"
//#include "priest.h"
//#include "shopping.h"
//#include "skills.h"
#include "skills2.h"
//#include "spell.h"
//#include "spells.h"
//#include "spells0.h"
//#include "spells1.h"
//#include "spells2.h"
//#include "spells3.h"
#include "stuff.h"
#include "view.h"

struct ghost_struct ghost;

void load(unsigned char stair_taken, char moving_level, char level_saved, char was_a_labyrinth, char old_level, char want_followers, char just_made_new_lev, char where_were_you2)
{
int j = 0;
int i, count_x, count_y;
int handle = 0;
char rm [250];
char cha_fil [80];
int temp_int = 0;

char ting_quant [12];

char corr_level [4];

char already_saved = 0;

int foll_class [8];
int foll_hp [8];
int foll_hp_max [8];
unsigned char foll_HD [8];
int foll_AC [8];
char foll_ev [8];
unsigned char foll_speed [8];
unsigned char foll_speed_inc [8];

unsigned char foll_targ_1_x [8];
unsigned char foll_targ_1_y [8];
int foll_inv [8] [8];
unsigned char foll_beh [8];
        int foll_sec [8];
 unsigned char foll_hit [8];

 unsigned char foll_ench [8] [3];
 unsigned char foll_ench_1 [8];


         unsigned char fit_iclass [8] [8];
         unsigned char fit_itype [8] [8];
         unsigned char fit_iplus [8] [8];
         unsigned char fit_iplus2 [8] [8];
         unsigned char fit_idam [8] [8];
         unsigned int fit_iquant [8] [8];
         unsigned char fit_icol [8] [8];
         char fit_iid [8] [8];

int itmf = 0;
int ic = 0;

strcpy(ghost.gname, "");
for (ic = 0; ic < 20; ic ++)
{
 ghost.ghs [ic] = 0;
}



#ifdef DOS
window(1,1,80,25);
#endif


char hbjh [5];

strcpy(corr_level, "");

if (you[0].your_level < 10) strcpy(corr_level, "0");
itoa(you[0].your_level, hbjh, 10);
strcat(corr_level, hbjh);
corr_level [2] = you[0].where_are_you + 97;
corr_level [3] = 0; /* null-terminate it */
strcpy(cha_fil, "");
strncat(cha_fil, you[0].your_name, 6);
strcat(cha_fil, ".");
if (you[0].level_type != 0) strcat(cha_fil, "lab"); /* temporary level */
 else strcat(cha_fil, corr_level);


/*//itoa(level_saved, corr_level, 10);
if (you[0].your_level < 10) strcpy(corr_level, "0");
itoa(you[0].your_level, corr_level, 10);
//if (you[0].where_are_you > 0)
strcpy(cha_fil, "");
strncat(cha_fil, you[0].your_name, 6);
strcat(cha_fil, ".");
if (you[0].level_type != 0) strcat(cha_fil, "lab");
 else strcat(cha_fil, corr_level);*/





        int sysg = 0;

 int following = -1;
 int fmenv = 0;

 int minvc = 0;

 int clouty = 0;

if (moving_level == 1)
{
 for (clouty = 0; clouty < CLOUDS; clouty ++)
 {
   env[0].cloud_type [clouty] = 0;
   env[0].cgrid [env[0].cloud_x [clouty]] [env[0].cloud_y [clouty]] = CNG;
   env[0].cloud_decay [clouty] = 0;
   env[0].cloud_no--;
 }
 env[0].cloud_no = 0;
}


if (want_followers == 1 && just_made_new_lev == 0)
{
for (count_x = you[0].x_pos - 1; count_x < you[0].x_pos + 2; count_x ++)
{
 for (count_y = you[0].y_pos - 1; count_y < you[0].y_pos + 2; count_y ++)
 {
   if (count_x == you[0].x_pos && count_y == you[0].y_pos) continue;
   following ++;
   foll_class [following] = -1;
   if (mgrd [count_x] [count_y] == MNG) continue;
   fmenv = mgrd [count_x] [count_y];
   if (menv [fmenv].m_class == 400 | menv [fmenv].m_class == 19 | menv [fmenv].m_class == 56 | menv [fmenv].m_class == -1) continue;
   if (menv [fmenv].m_class >= MLAVA0) continue;
   if (menv [fmenv].m_speed_inc < 50) continue;
   foll_class [following] = menv [fmenv].m_class;
   itoa(foll_class [following], st_prn, 10);

   foll_hp [following] = menv [fmenv].m_hp;
   foll_hp_max [following] = menv [fmenv].m_hp_max;
   foll_HD [following] = menv [fmenv].m_HD;
   foll_AC [following] = menv [fmenv].m_AC;
   foll_ev [following] = menv [fmenv].m_ev;
   foll_speed [following] = menv [fmenv].m_speed;
   foll_speed_inc [following] = menv [fmenv].m_speed_inc;
   foll_targ_1_x [following] = menv [fmenv].m_targ_1_x;
   foll_targ_1_y [following] = menv [fmenv].m_targ_1_y;
   for (minvc = 0; minvc < 8; minvc ++)
   {
    foll_inv [following] [minvc] = 501;
   }



for (minvc = 0; minvc < 8; minvc ++)
{
  if (menv [fmenv].m_inv [minvc] == 501)
  {
   fit_iquant [following] [minvc] = 0;
   continue;
  }

  fit_iclass [following] [minvc] = mitm.iclass [menv [fmenv].m_inv [minvc]];
         fit_itype [following] [minvc] = mitm.itype [menv [fmenv].m_inv [minvc]];
         fit_iplus [following] [minvc] = mitm.iplus [menv [fmenv].m_inv [minvc]];
         fit_iplus2 [following] [minvc] = mitm.iplus2 [menv [fmenv].m_inv [minvc]];
         fit_idam [following] [minvc] = mitm.idam [menv [fmenv].m_inv [minvc]];
        fit_iquant [following] [minvc] = mitm.iquant [menv [fmenv].m_inv [minvc]];
         fit_icol [following] [minvc] = mitm.icol [menv [fmenv].m_inv [minvc]];
         fit_iid [following] [minvc] = mitm.iid [menv [fmenv].m_inv [minvc]];
destroy_item(menv [fmenv].m_inv [minvc]);
}

   foll_beh [following] = menv [fmenv].m_beh;
   foll_sec [following] = menv [fmenv].m_sec;
   foll_hit [following] = menv [fmenv].m_hit;
   foll_ench_1 [following] = menv [fmenv].m_ench_1;
   foll_ench [following] [0] = menv [fmenv].m_ench [0];
   foll_ench [following] [1] = menv [fmenv].m_ench [1];
   foll_ench [following] [2] = menv [fmenv].m_ench [2];

   /* now for the icky part:*/
     for (j = 0; j < 3; j++)
        {
                menv [fmenv].m_ench [j] = 0;
        }

        menv [fmenv].m_ench_1 = 0;

menv [fmenv].m_class = -1;
menv [fmenv].m_hp = 0;
menv [fmenv].m_hp_max = 0;
menv [fmenv].m_HD = 0;
menv [fmenv].m_AC = 0;
menv [fmenv].m_ev = 0;
mgrd [count_x] [count_y] = MNG;

 }
}
if (was_a_labyrinth == 0) save_level(old_level, 0, where_were_you2);
already_saved = 1;
was_a_labyrinth = 0;
} /* end if level_type == 0*/


#ifdef DOS
strupr(cha_fil);
#endif
handle = open(cha_fil, O_BINARY, 0660);

//cprintf(cha_fil);
//if (getch() == 0) getch();

if (handle == -1)
{

//cprintf(cha_fil);
//if (getch() == 0) getch();


//   itoa(you[0].your_level, eggo, 10);

int imn = 0;
strcpy(ghost.gname, "");
for (imn = 0; imn < 20; imn ++)
{
 ghost.ghs [imn] = 0;
}

builder(you[0].your_level, you[0].level_type);

if (random2(3) == 0 && you[0].your_level > 3)
{

strcpy(corr_level, "");

if (you[0].your_level < 10) strcpy(corr_level, "0");
itoa(you[0].your_level, hbjh, 10);
strcat(corr_level, hbjh);
corr_level [2] = you[0].where_are_you + 97;
corr_level [3] = 0; /* null-terminate it */
strcpy(cha_fil, "");
strcat(cha_fil, "bones.");
if (you[0].level_type != 0) strcat(cha_fil, "lab"); /* temporary level */
 else strcat(cha_fil, corr_level);

int gfile = open(cha_fil, S_IWRITE, S_IREAD);

if (gfile != -1)
{
        close(gfile);
        char hjbh [5];
//      gfile = open(cha_fil, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0660);
        gfile = open(cha_fil, O_RDONLY, O_CREAT | O_TRUNC | O_BINARY, 0660);
        if (gfile == -1)
        {
         strcpy(info, "Error opening ghost file: ");
         strcat(info, cha_fil);
         mpr(info);
         more();
         close(gfile);
        } else
        {
         read(gfile, ghost.gname, 20);
         read(gfile, hjbh, 5);
         ghost.ghs [0] = hjbh [0] - 30;
         ghost.ghs [1] = hjbh [1] - 30;
         ghost.ghs [2] = hjbh [2] - 30;
         ghost.ghs [3] = hjbh [3] - 30;
         ghost.ghs [4] = hjbh [4] - 30;

         read(gfile, hbjh, 4);
         ghost.ghs [5] = hbjh [0] - 30;
         ghost.ghs [6] = hbjh [1] - 30;
         ghost.ghs [7] = hbjh [2] - 30;
         ghost.ghs [8] = hbjh [3] - 30;

         /* note - as ghosts, automatically get res poison + prot_life */

         read(gfile, hbjh, 5);
         ghost.ghs [9] = hbjh [0] - 30;
         ghost.ghs [10] = hbjh [1] - 30;
         ghost.ghs [11] = hbjh [2] - 30;
         ghost.ghs [12] = hbjh [3] - 30;
         ghost.ghs [13] = hbjh [4] - 30;

         close(gfile);
         unlink(cha_fil);

         for (imn = 0; imn < MNST - 10; imn ++)
         {
          if (menv [imn].m_class != -1) continue;
          menv [imn].m_class = 400;
          menv [imn].m_HD = ghost.ghs [12];
          menv [imn].m_hp = ghost.ghs [0];
          menv [imn].m_hp_max = ghost.ghs [0];
          menv [imn].m_AC = ghost.ghs [2];
          menv [imn].m_ev = ghost.ghs [1];
          menv [imn].m_speed = 10;
          menv [imn].m_speed_inc = 70;
          menv [imn].m_sec = 250;
          do
          {
           menv [imn].m_x = random2(60) + 10;
           menv [imn].m_y = random2(50) + 10;
          } while (grd [menv [imn].m_x] [menv [imn].m_y] != 67 | mgrd [menv [imn].m_x] [menv [imn].m_y] != MNG);
          mgrd [menv [imn].m_x] [menv [imn].m_y] = imn;
          break;
         }
        }

}
close(gfile);

}

/*gloj = builder(
igrd,
grid,
0, / *it_no* /
you[0].unique_items,
you[0].item_description,
property,
trap_type,
trap_x,
trap_y,
gmon_use,
stair_x,
stair_y,
you[0].your_level,
/ *you[0].x_pos,
/you[0].y_pos,* /
you[0].level_type,
mgrd,
menv,
unique_creatures,
it,
shops);*/
#ifdef UNBUG

        clrscr();
                for (i = 0; i < ITEMS; i ++)
                {
                 if (mitm.ilink [i] == 501) continue;
                 itoa(i, st_prn, 10);
                 strcpy(info, st_prn);
                 strcat(info, " is linked to ");
                 itoa(mitm.ilink [i], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " c:");
                 itoa(mitm.iclass [i], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " t:");
                 itoa(mitm.itype [i], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " p:");
                 itoa(mitm.iplus [i], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " p2:");
                 itoa(mitm.iplus2 [i], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " d:");
                 itoa(mitm.idam [i], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " q: ");
                 itoa(mitm.iquant [i], st_prn, 10);
                 strcat(info, st_prn);
                 cprintf(info);
                 cprintf("\n\r");
                getch();
                }
                strcpy(info, "igrd:");
                cprintf("igrd: \n\r");
                for (i = 0; i < GXM; i ++)
                {
                 for (j = 0; j < GYM; j ++)
                 {
                  if (igrd [i] [j] != 501)
                  {
                   itoa(igrd [i] [j], st_prn, 10);
                   strcpy(info, st_prn);
                   strcat(info, " at ");
                   itoa(i, st_prn, 10);
                   strcat(info, st_prn);
                   strcat(info, ", ");
                   itoa(j, st_prn, 10);
                   strcat(info, st_prn);
                   cprintf(info);
                   cprintf("\n\r");
                getch();
                  }
                 }
                }
                getch();
#endif


for (i = 0; i < GXM; i ++)
{
 for (j = 0; j < GYM; j ++)
 {
  env[0].map [i] [j] = 0;
  if (you[0].char_direction == 1 && you[0].level_type != 3)
  {
   if (grd [i] [j] == 69 | grd [i] [j] == 96 | grd [i] [j] == 99) grd [i] [j] = 98;
   /* closes all the gates if you're on the way out */
  }
  env[0].cgrid [i] [j] = CNG;
 }
}

for (i = 0; i < MNST; i ++)
{
 if (menv [i].m_class == 255) menv [i].m_class = -1;
   else
       {
/*         mn_no ++;
         no_menv ++;*/
       }
}


if (just_made_new_lev == 0)
{

if (stair_taken == 69 | stair_taken == 81) stair_taken = 86;
 else
  if (stair_taken < 86) stair_taken += 4;
   else
    if (stair_taken >= 130 && stair_taken < 150) stair_taken -= 20;
     else
      if (stair_taken >= 110 && stair_taken < 130) stair_taken += 20;
       else
        if (stair_taken == -1) stair_taken = 69;
         else
          if (stair_taken > 90)
          {
           stair_taken = 86;
          } else stair_taken -= 4;

for (count_x = 0; count_x < GXM; count_x ++)
{
 for (count_y = 0; count_y < GYM; count_y ++)
 {
  if (grd [count_x] [count_y] == stair_taken) goto found_stair;
 }
}
if (stair_taken < 86) stair_taken = 82; else stair_taken = 86;

for (count_x = 0; count_x < GXM; count_x ++)
{
 for (count_y = 0; count_y < GYM; count_y ++)
 {
  if (grd [count_x] [count_y] == stair_taken) goto found_stair;
 }
}

/*      do
        {
                count_x = random2(70) + 10;
                count_y = random2(60) + 10;
        } while (grd [count_x] [count_y] != 67);*/

count_x = 30; count_y = 30;

}


found_stair :
if (just_made_new_lev == 0)
{
 you[0].x_pos = count_x;
 you[0].y_pos = count_y;
}

if (you[0].level_type == 1 | you[0].level_type == 2) grd [you[0].x_pos] [you[0].y_pos] = 67;

following = 0;
fmenv = -1;

if ((you[0].level_type == 0 | you[0].level_type == 3) && want_followers == 1 && just_made_new_lev == 0)
{
for (ic = 0; ic < 2; ic ++)
{
for (count_x = you[0].x_pos - 3; count_x < you[0].x_pos + 4; count_x ++)
{
 for (count_y = you[0].y_pos - 3; count_y < you[0].y_pos + 4; count_y ++)
 {
   if (ic == 0 && (count_x < you[0].x_pos - 1 | count_x > you[0].x_pos + 1 | count_y < you[0].y_pos - 1 | count_y > you[0].y_pos + 1))
     continue;
   if (count_x == you[0].x_pos && count_y == you[0].y_pos) continue;
   if (mgrd [count_x] [count_y] != MNG | grd [count_x] [count_y] < 67) continue;

   while(menv [following].m_class != -1)
   {
    following ++;
    if (following >= MNST) goto out_of_foll;
   }

   if (fmenv < 7)
   do
   {
   fmenv ++;
   if (foll_class [fmenv] == -1) continue;
   menv [following].m_class = foll_class [fmenv];

   itoa(menv [following].m_class, st_prn, 10);


   menv [following].m_hp = foll_hp [fmenv];
   menv [following].m_hp_max = foll_hp_max [fmenv];
   menv [following].m_HD = foll_HD [fmenv];
   menv [following].m_AC = foll_AC [fmenv];
   menv [following].m_ev = foll_ev [fmenv];
   menv [following].m_speed = foll_speed [fmenv];
   menv [following].m_speed_inc = foll_speed_inc [fmenv];
   menv [following].m_x = count_x;
   menv [following].m_y = count_y;
   menv [following].m_targ_1_x = 0;
   menv [following].m_targ_1_y = 0;
/*   no_menv ++;*/
for (minvc = 0; minvc < 8; minvc ++)
   {
    if (fit_iquant [fmenv] [minvc] == 0)
    {
     menv [following].m_inv [minvc] = 501;
     continue;
    }

    itmf = 0;
    while(mitm.iquant [itmf] > 0)
    {
     itmf ++;
    }

    menv [following].m_inv [minvc] = itmf;

  mitm.iclass [itmf] = fit_iclass [fmenv] [minvc];
         mitm.itype [itmf] = fit_itype [fmenv] [minvc];
         mitm.iplus [itmf] = fit_iplus [fmenv] [minvc];
         mitm.iplus2 [itmf] = fit_iplus2 [fmenv] [minvc];
         mitm.idam [itmf] = fit_idam [fmenv] [minvc];
        mitm.iquant [itmf] = fit_iquant [fmenv] [minvc];
         mitm.icol [itmf] = fit_icol [fmenv] [minvc];
         mitm.iid [itmf] = fit_iid [fmenv] [minvc];
  mitm.ilink [itmf] = 501;
}



   menv [following].m_beh = foll_beh [fmenv];
   menv [following].m_sec = foll_sec [fmenv];
   menv [following].m_hit = foll_hit [fmenv];
   menv [following].m_ench_1 = foll_ench_1 [fmenv];
   menv [following].m_ench [0] = foll_ench [fmenv] [0];
   menv [following].m_ench [1] = foll_ench [fmenv] [1];
   menv [following].m_ench [2] = foll_ench [fmenv] [2];

   mgrd [count_x] [count_y] = following;
   break;

   } while (fmenv < 7);

   } /* end for ic (at least, one of these brackets is) */
   }
}
} /* end if level_type == 0 */




        out_of_foll :

reset_ch();

//char title [40];

//strcpy(title, skill_title(best_skill(0, 50, 99), you[0].skills [best_skill(0, 50, 99)], you[0].clas, you[0].xl));
//draw_border(BROWN, you[0].your_name, title, you[0].species);

 moving_level = 0;

for (i = 0; i < MNST; i++)
{
 if (menv [i].m_class == -1) continue;
        for (j = 0; j < 8; j ++)
        {
                if (menv [i].m_inv [j] == 501) continue;
                if (mitm.ilink [menv [i].m_inv [j]] != 501)
                {
                        mitm.ilink [menv [i].m_inv [j]] = 501;
                        /* items carried by menvters shouldn't be linked */
                }

        }
}

if (you[0].level_type == 3)
{
 for (count_x = 0; count_x < GXM; count_x ++)
 {
  for (count_y = 0; count_y < GYM; count_y ++)
  {
   if (grd [count_x] [count_y] >= 86 && grd [count_x] [count_y] <= 89)
    {
     grd [count_x] [count_y] = 67;
     if (random2(30) == 0) grd [count_x] [count_y] = 100;
    }
   if (grd [count_x] [count_y] >= 81 && grd [count_x] [count_y] <= 85)
    grd [count_x] [count_y] = 101;

  }
 }

}

 //if (was_a_labyrinth == 0)
 save_level(you[0].your_level, (you[0].level_type == 1), you[0].where_are_you);

 return;

}

moving_level = 0;

close(handle);



        sysg = 0;


#ifdef DOS
strupr(cha_fil);
#endif
handle = open(cha_fil, O_RDONLY, O_CREAT | O_TRUNC | O_BINARY, 0660);

if (handle == -1)
{
        perror("Errghor");
        exit(-1);
}


if (handle > -1)
{

for (count_x = 0; count_x < ITEMS; count_x ++)
{
        mitm.ilink [count_x] = 501;
}


for (i = 0; i < GXM; i ++)
{
        for (j = 0; j < GYM; j ++)
        {
                igrd [i] [j] = 501;
        }
}


         read(handle, ghost.gname, 20);
         read(handle, ghost.ghs, 14);

         for (j = 0; j < 20; j ++)
         {
          ghost.ghs [j] -= 30;
         }

for (count_x = 0; count_x < GXM; count_x ++)
{
        for (count_y = 0; count_y < GYM; count_y ++)
        {
        read(handle, rm, 4);

        grd [count_x] [count_y] = rm [0] - 30;
 env[0].map [count_x] [count_y] = rm [1] - 30;
 mgrd [count_x] [count_y] = MNG; //rm [2] - 30;
  if (mgrd [count_x] [count_y] != MNG && (menv [mgrd [count_x] [count_y]].m_class == -1 | menv [mgrd [count_x] [count_y]].m_x != count_x | menv [mgrd [count_x] [count_y]].m_y != count_y))
  {
/*   cprintf("First: ");
   itoa(count_x, st_prn, 10);
   cprintf(st_prn);
   cprintf("/");
   itoa(count_y, st_prn, 10);
   cprintf(st_prn);
   cprintf("\n\r");
   getch();*/
   mgrd [count_x] [count_y] = MNG; /* This is one of the worst things I've ever done */
  }
 env[0].cgrid [count_x] [count_y] = rm [3] - 30;
        }
}

for (i = 0; i < NTRAPS; i ++)
{
        read(handle, rm, 3);
        env[0].trap_type [i] = rm [0] - 40;
        env[0].trap_x [i] = rm [1] - 40;
        env[0].trap_y [i] = rm [2] - 40;

}

/*no_menv = 0;*/







for (count_x = 0; count_x < GXM; count_x ++)
{
        for (count_y = 0; count_y < GYM; count_y ++)
        {
         if (igrd [count_x] [count_y] < 0 | igrd [count_x] [count_y] > 501)
         {
#ifdef UNBUG
          itoa(igrd [count_x] [count_y], st_prn, 10);
          cprintf(st_prn);
          cprintf("x5! ");
          getch();
#endif

          igrd [count_x] [count_y] = 501;
         }
        }
}




for (i = 0; i < ITEMS; i ++)
{


        read(handle, rm, 25);

        mitm.iclass [i] = rm [0] - 100;
        mitm.itype [i] = rm [1] - 100;
        mitm.iplus [i] = rm [2] - 40;
        mitm.iplus2 [i] = rm [24] - 40;
        mitm.idam [i] = rm [3] - 30;
        mitm.iquant [i] = rm [4] - 100;
        mitm.iquant [i] = (rm [9] - 48) + (rm [8] - 48) * 10 + (rm [7] - 48) * 100 + (rm [6] - 48) * 1000 + (rm [5] - 48) * 10000;

        /* - 48 because ASCII for zero (0) is 48. */
        mitm.icol [i] = rm [10] - 100;

        mitm.ix [i] = rm [11] - 100;
        mitm.iy [i] = rm [12] - 100;

        mitm.iid [i] = rm [13] - 100;


        mitm.ilink [i] = (rm [18] - 48) + (rm [17] - 48) * 10 + (rm [16] - 48) * 100 + (rm [15] - 48) * 1000;
        igrd [mitm.ix [i]] [mitm.iy [i]] = (rm [23] - 48) + (rm [22] - 48) * 10 + (rm [21] - 48) * 100 + (rm [20] - 48) * 1000;

/* rm [24] is plus2 */


        if (mitm.iclass [i] == 100)
        {
         mitm.iquant [i] = 0;
         mitm.ilink [i] = 501;
        }

/*      if (mitm.iquant [i] != 0) it_no ++;*/

}
#ifdef UNBUG
for (count_x = 0; count_x < GXM; count_x ++)
{
        for (count_y = 0; count_y < GYM; count_y ++)
        {
         if (igrd [count_x] [count_y] < 0 | igrd [count_x] [count_y] > 501)
         {
          itoa(igrd [count_x] [count_y], st_prn, 10);
          cprintf(st_prn);
          cprintf("x6! ");
          getch();
         }
        }
}
#endif

#ifdef UNBUG
for (count_x = 0; count_x < GXM; count_x ++)
{
        for (count_y = 0; count_y < GYM; count_y ++)
        {
         if (igrd [count_x] [count_y] < 0 | igrd [count_x] [count_y] > 501)
         {
          itoa(igrd [count_x] [count_y], st_prn, 10);
          cprintf(st_prn);
          cprintf("x8! ");
          getch();
         }
        }
}
#endif



read(handle, rm, 1);
env[0].cloud_no = rm [0] - 40;

for (i = 0; i < CLOUDS; i ++)
{

 read(handle,rm, 9);
 env[0].cloud_x [i] = rm [0] - 40;
 env[0].cloud_y [i] = rm [1] - 40;
 env[0].cloud_type [i] = rm [2] - 40;
        env[0].cloud_decay [i] = ((rm [8] - 48) + (rm [7] - 48) * 10 + (rm [6] - 48) * 100 + (rm [5] - 48) * 1000 + (rm [4] - 48) * 10000) / 10;
}

/*cprintf("2nd:\n\r");
for (count_x = 0; count_x < 80; count_x ++)
{
 for (count_y = 0; count_y < 70; count_y ++)
 {
  if (mgrd [count_x] [count_y] == MNG) continue;
  if (menv [mgrd [count_x] [count_y]].m_class == -1)
  {
   itoa(count_x, st_prn, 10);
   cprintf(st_prn);
   cprintf("/");
   itoa(count_y, st_prn, 10);
   cprintf(st_prn);
   cprintf("\n\r");
   getch();
  }
 }
}
*/
for (i = 0; i < 5; i ++)
{

 read(handle,rm, 8);
 env[0].keeper_name [i] [0] = rm [0] - 40;
 env[0].keeper_name [i] [1] = rm [1] - 40;
 env[0].keeper_name [i] [2] = rm [2] - 40;
 env[0].sh_x [i] = rm [3] - 40;
 env[0].sh_y [i] = rm [4] - 40;
 env[0].sh_greed [i] = rm [5] - 40;
 env[0].sh_type [i] = rm [6] - 40;
 env[0].sh_level [i] = rm [7] - 40;
 if (mgrd [env[0].sh_x [i] - 1] [env[0].sh_y [i] - 1] == 31)
 {
  cprintf("x");
  getch();
 }

}

for (i = 0; i < 20; i ++)
{
 read(handle,rm, 5);
 env[0].mons_alloc [i] = ((rm [4] - 48) + (rm [3] - 48) * 10 + (rm [2] - 48) * 100 + (rm [1] - 48) * 1000);
}


for (count_x = 0; count_x < MNST; count_x ++)
{
 read(handle, rm, 18);

 menv [count_x].m_AC = rm [3] - 40;

 menv [count_x].m_ev = rm [4] - 40;

 menv [count_x].m_HD = rm [5] - 40;
 menv [count_x].m_speed = rm [6] - 40;
 menv [count_x].m_speed_inc = rm [7] - 40;
 menv [count_x].m_beh = rm [8] - 40;
 menv [count_x].m_x = rm [9] - 40;
 menv [count_x].m_y = rm [10] - 40;
 menv [count_x].m_targ_1_x = rm [11] - 40;
 menv [count_x].m_targ_1_y = rm [12] - 40;

 menv [count_x].m_ench_1 = rm [14] - 40;

 for (j = 0; j < 3; j++)
 {
  menv [count_x].m_ench [j] = rm [15 + j] - 40;
 }

int p = 0;

        strcpy(ting_quant, "");
        read(handle, rm, 5);
        for (p = 0; p < 5; p ++)
        {
                        ting_quant [p] = rm [p];
        }
        ting_quant [5] = '\0';
        temp_int = atoi(ting_quant);
        menv [count_x].m_class = temp_int - 50080;

        read(handle, rm, 5);
        for (p = 0; p < 5; p ++)
        {
                        ting_quant [p] = rm [p];
        }
        temp_int = atoi(ting_quant);
        menv [count_x].m_hp = temp_int - 50000;

        read(handle, rm, 5);
        for (p = 0; p < 5; p ++)
        {
                        ting_quant [p] = rm [p];
        }
        temp_int = atoi(ting_quant);
        menv [count_x].m_hp_max = temp_int - 50000;

        read(handle, rm, 5);
        for (p = 0; p < 5; p ++)
        {
                        ting_quant [p] = rm [p];
        }
        temp_int = atoi(ting_quant);
        menv [count_x].m_sec = temp_int - 50000;


for (j = 0; j < 8; j ++)
        {

read(handle, rm, 5);
        menv[count_x].m_inv [j] = (rm [4] - 48) + (rm [3] - 48) * 10 + (rm [2] - 48) * 100 + (rm [1] - 48) * 1000;
        }

/*
         read(handle, ghost.gname, 20);
         read(handle, ghost.ghs, 14);

         for (j = 0; j < 20; j ++)
         {
          ghost.ghs [j] -= 30;
         }*/

//         read(handle, rm, 20);

//       strcpy(ghost.gname, "Hello");

/*       read(handle, rm, 14);
         for (j = 0; j < 14; j ++)
         {
          ghost.ghs [j] = rm [j] - 30;
         }*/

for (j = 0; j < MNST; j ++)
{
 if (menv [j].m_class != -1)
  mgrd [menv [j].m_x] [menv [j].m_y] = j;
}

}


}
else
{
  cprintf("Error opening file\n");
  exit(-1);
}

reset_ch();


close(handle);


//char title [40];

//strcpy(title, skill_title(best_skill(0, 50, 99), you[0].skills [best_skill(0, 50, 99)], you[0].clas, you[0].xl));
//draw_border(BROWN, you[0].your_name, title, you[0].species);



/*it_no = 0;*/

for (i = 0; i < ITEMS; i ++)
{

/*if (mitm.iquant [i] > 0)
        it_no ++;*/
}


i = 0;
j = 0;



for (i = 0; i < GXM; i ++)
{
 for (j = 0; j < GYM; j ++)
 {
  if (igrd [i] [j] < 0 | igrd [i] [j] > 501) igrd [i] [j] = 501;
 }
} /* WAS HERE */

for (i = 0; i < MNST; i ++)
{
 for (j = 0; j < 8; j ++)
 {
  if (menv [i].m_inv [j] < 0 | menv [i].m_inv [j] > 501) menv [i].m_inv [j] = 501;
  if (menv [i].m_inv [j] != 501) mitm.ilink [menv [i].m_inv [j]] = 501;
 }
}

for (i = 0; i < ITEMS; i ++)
{
 if (mitm.ilink [i] > 501) mitm.ilink [i] = 501;
}

#ifdef UNBUG2
                for (i = 0; i < ITEMS; i ++)
                {
                 itoa(i, st_prn, 10);
                 strcpy(info, st_prn);
                 strcat(info, " is linked to ");
                 itoa(mitm.ilink [i], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " c:");
                 itoa(mitm.iclass [i], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " t:");
                 itoa(mitm.itype [i], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " p:");
                 itoa(mitm.iplus [i], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " p2:");
                 itoa(mitm.iplus2 [i], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " d:");
                 itoa(mitm.idam [i], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " q: ");
                 itoa(mitm.iquant [i], st_prn, 10);
                 strcat(info, st_prn);
                 cprintf(info);
                 cprintf("\n\r");
                }
                strcpy(info, "igrd:");
                cprintf(info);
                cprintf("\n\r");
                for (i = 0; i < GXM; i ++)
                {
                 for (j = 0; j < GYM; j ++)
                 {
                  if (igrd [i] [j] != 501)
                  {
                   itoa(igrd [i] [j], st_prn, 10);
                   strcpy(info, st_prn);
                   strcat(info, " at ");
                   itoa(i, st_prn, 10);
                   strcat(info, st_prn);
                   strcat(info, ", ");
                   itoa(j, st_prn, 10);
                   strcat(info, st_prn);
                 strcat(info, " c:");
                 itoa(mitm.iclass [igrd [i] [j]], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " t:");
                 itoa(mitm.itype [igrd [i] [j]], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " p:");
                 itoa(mitm.iplus [igrd [i] [j]], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " p2:");
                 itoa(mitm.iplus2 [igrd [i] [j]], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " d:");
                 itoa(mitm.idam [igrd [i] [j]], st_prn, 10);
                 strcat(info, st_prn);
                 strcat(info, " q: ");
                 itoa(mitm.iquant [igrd [i] [j]], st_prn, 10);
                 strcat(info, st_prn);
                   cprintf(info);
                   cprintf("\n\r");
                  }
                 }
                }


#endif














for (i = 0; i < MNST; i++)
{
 if (menv [i].m_class == -1) continue;
        for (j = 0; j < 8; j ++)
        {
                if (menv [i].m_inv [j] == 501) continue;
                if (mitm.ilink [menv [i].m_inv [j]] != 501)
                {
#ifdef UNBUG
   cprintf("UNLINKING\n\r");
   getch();
#endif
                        mitm.ilink [menv [i].m_inv [j]] = 501;
                }

        }
}


/*for (i = 0; i < GXM; i ++)
{
 for (j = 0; j < GYM; j ++)
 {
  if (you[0].char_direction == 1)
  {
   if (grd [i] [j] == 69 | grd [i] [j] == 96 | grd [i] [j] == 99) grd [i] [j] = 98;
   / * closes all the gates if you're on the way out * /
  }
 }
}*/

if (you[0].your_level == 35 && stair_taken >= 86)
{
  do
  {
   you[0].x_pos = 10 + random2(GXM - 10);
   you[0].y_pos = 10 + random2(GYM - 10);
  } while(grd [you[0].x_pos] [you[0].y_pos] != 67 | mgrd [you[0].x_pos] [you[0].y_pos] != MNG);
  count_x = you[0].x_pos;
  count_y = you[0].y_pos;
  goto found_stair;
} else
{
/*if (stair_taken == 130) stair_taken = 110;
 else
  if (stair_taken == 110) stair_taken = 130;
   else*/
if (stair_taken >= 130 && stair_taken < 150) stair_taken -= 20;
 else
  if (stair_taken >= 110 && stair_taken < 130) stair_taken += 20;
   else
     if (stair_taken < 86) stair_taken += 4; else stair_taken -= 4;

for (count_x = 0; count_x < GXM; count_x ++)
{
 for (count_y = 0; count_y < GYM; count_y ++)
 {
  if (grd [count_x] [count_y] == stair_taken) goto found_stair;
 }
}
if (stair_taken < 86) stair_taken = 82; else stair_taken = 86;

for (count_x = 0; count_x < GXM; count_x ++)
{
 for (count_y = 0; count_y < GYM; count_y ++)
 {
  if (grd [count_x] [count_y] == stair_taken) goto found_stair;
 }
}
} /* end else */

for (count_x = 0; count_x < GXM; count_x ++)
{
        for (count_y = 0; count_y < GYM; count_y ++)
        {
          if (mgrd [count_x] [count_y] != MNG && (menv [mgrd [count_x] [count_y]].m_class == -1 | menv [mgrd [count_x] [count_y]].m_x != count_x | menv [mgrd [count_x] [count_y]].m_y != count_y))
          {
           mgrd [count_x] [count_y] = MNG; /* This is one of the worst things I've ever done */
          }
        }
}



} /* end of void load_level(); */





void save_level(int level_saved, char was_a_labyrinth, char where_were_you)
{


char rm [491];
char cha_fil [20];
char extens [5];
char thing_quant [8];
int count_x, count_y;
int i, temp_int, j, handle, p;

char hbjh [5];



strcpy(extens, "");
if (level_saved < 10) strcpy(extens, "0");
itoa(level_saved, hbjh, 10);
strcat(extens, hbjh);
extens [2] = where_were_you + 97;
extens [3] = 0; /* null-terminate it */
strcpy(cha_fil, "");
strncat(cha_fil, you[0].your_name, 6);
strcat(cha_fil, ".");
if (was_a_labyrinth == 1) strcat(cha_fil, "lab"); /* temporary level */
 else strcat(cha_fil, extens);



/*strcpy(cha_fil, "");
strncat(cha_fil, you[0].your_name, 6);
strcat(cha_fil, ".");
itoa(level_saved, extens, 10);
if (was_a_labyrinth == 1) strcat(cha_fil, "lab");
 else strcat(cha_fil, extens);*/



 #ifdef DOS
strupr(cha_fil);
#endif
handle = open(cha_fil, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0660);


        if (handle == -1)
        {
                perror("Oh dear... ");
                exit(-1);
        }


int fry;
int frx;

/*cprintf("pre-Saving 1:\n\r");
for (count_x = 0; count_x < 80; count_x ++)
{
 for (count_y = 0; count_y < 70; count_y ++)
 {
  if (mgrd [count_x] [count_y] == MNG) continue;
  if (menv [mgrd [count_x] [count_y]].m_class == -1)
  {
   itoa(count_x, st_prn, 10);
   cprintf(st_prn);
   cprintf("/");
   itoa(count_y, st_prn, 10);
   cprintf(st_prn);
   cprintf("\n\r");
   getch();
  }
 }
}*/


/* Setting up ix & iy, which aren't normally used: */

for (frx = 0; frx < MNST; frx ++)
{
 for (fry = 0; fry < 8; fry ++)
 {
  if (menv [frx].m_inv [fry] != 501)
  {
   mitm.ix [menv [frx].m_inv [fry]] = 2;
   mitm.iy [menv [frx].m_inv [fry]] = 2;
   mitm.ilink [menv [frx].m_inv [fry]] = 501;
  }
 }
}



#ifdef UNBUG
for (count_x = 0; count_x < 80; count_x ++)
{
        for (count_y = 0; count_y < 70; count_y ++)
        {
         if (igrd [count_x] [count_y] < 0 | igrd [count_x] [count_y] > 501)
         {
          itoa(igrd [count_x] [count_y], st_prn, 10);
          cprintf(st_prn);
          cprintf("x1! ");
          getch();
         }
        }
}
#endif
/*
cprintf("pre-Saving 2:\n\r");
for (count_x = 0; count_x < 80; count_x ++)
{
 for (count_y = 0; count_y < 70; count_y ++)
 {
  if (mgrd [count_x] [count_y] == MNG) continue;
  if (menv [mgrd [count_x] [count_y]].m_class == -1)
  {
   itoa(count_x, st_prn, 10);
   cprintf(st_prn);
   cprintf("/");
   itoa(count_y, st_prn, 10);
   cprintf(st_prn);
   cprintf("\n\r");
   getch();
  }
 }
}


cprintf("Items: pre-saving:\n\r");
for (count_x = 0; count_x < 80; count_x ++)
{
 for (count_y = 0; count_y < 70; count_y ++)
 {
  if (igrd [count_x] [count_y] == MNG) continue;
  if (igrd [count_x] [count_y] < 0 | igrd [count_x] [count_y] > 501)
  {
   itoa(igrd [count_x] [count_y], st_prn, 10);
   cprintf(st_prn);
   cprintf(" : ");
   itoa(count_x, st_prn, 10);
   cprintf(st_prn);
   cprintf("/");
   itoa(count_y, st_prn, 10);
   cprintf(st_prn);
   cprintf("\n\r");
   getch();
  }
 }
}

cprintf("Item links: pre-saving:\n\r");
 for (count_y = 0; count_y < ITEMS; count_y ++)
 {
  if (mitm.ilink [count_y] == 501) continue;
  if (mitm.ilink [count_y] < 0 | mitm.ilink [count_y] > 501)
  {
   itoa(count_y, st_prn, 10);
   cprintf(st_prn);
   cprintf(" : ");
   itoa(mitm.ilink [count_y], st_prn, 10);
   cprintf(st_prn);
   cprintf("\n\r");
   getch();
  }
 }
*/

int count_out = 0;

for (count_x = 0; count_x < 80; count_x ++)
{
        for (count_y = 0; count_y < 70; count_y ++)
        {

        count_out = 0;

        if (igrd [count_x] [count_y] < 0 | igrd [count_x] [count_y] > 501) igrd [count_x] [count_y] = 501;
        if (igrd [count_x] [count_y] > 501) igrd [count_x] [count_y] = 501;

        if (igrd [count_x] [count_y] == 501) continue;

        frx = igrd [count_x] [count_y];

        while(frx != 501)
        {
                mitm.ix [frx] = count_x;
                mitm.iy [frx] = count_y;

                if (frx > 501 | frx < 0)
                {
                 cprintf("Error! Item out of bounds: ");
                 itoa(frx, st_prn, 10);
                 cprintf(st_prn);
                 if (getch() == 0) getch();
                 cprintf("\n\r");
                 break;
                }

                fry = mitm.ilink [frx];
                frx = fry;
                count_out ++;
                if (count_out > 1000)
                {
                 count_out = 0;
                 mitm.ilink [frx] = 501;
                 mpr("Item link error.");
                 break;
                }

                if (frx == 501) break;
        }


        }
}


/*cprintf("Saving before pre-3:\n\r");
for (count_x = 0; count_x < 80; count_x ++)
{
 for (count_y = 0; count_y < 70; count_y ++)
 {
  if (mgrd [count_x] [count_y] == MNG) continue;
  if (menv [mgrd [count_x] [count_y]].m_class == -1)
  {
   itoa(count_x, st_prn, 10);
   cprintf(st_prn);
   cprintf("/");
   itoa(count_y, st_prn, 10);
   cprintf(st_prn);
   cprintf("\n\r");
   getch();
  }
 }
}

cprintf("pre-Saving 3:\n\r");
for (count_x = 0; count_x < 80; count_x ++)
{
 for (count_y = 0; count_y < 70; count_y ++)
 {
  if (mgrd [count_x] [count_y] == MNG) continue;
  if (menv [mgrd [count_x] [count_y]].m_class == -1)
  {
   itoa(count_x, st_prn, 10);
   cprintf(st_prn);
   cprintf("/");
   itoa(count_y, st_prn, 10);
   cprintf(st_prn);
   cprintf("\n\r");
   getch();
  }
 }
}

cprintf("Saving:\n\r");
for (count_x = 0; count_x < 80; count_x ++)
{
 for (count_y = 0; count_y < 70; count_y ++)
 {
  if (mgrd [count_x] [count_y] == MNG) continue;
  if (menv [mgrd [count_x] [count_y]].m_class == -1)
  {
   itoa(count_x, st_prn, 10);
   cprintf(st_prn);
   cprintf("/");
   itoa(count_y, st_prn, 10);
   cprintf(st_prn);
   cprintf("\n\r");
   getch();
  }
 }
}
*/

unsigned char ghost_bak [20];

for (j = 0; j < 20; j ++)
{
  ghost_bak [j] = ghost.ghs [j] + 30;
}

write(handle, ghost.gname, 20);

write(handle, ghost_bak, 14);


int glorpogop = 0;

for (count_x = 0; count_x < 80; count_x ++)
{
                for (count_y = 0; count_y < 70; count_y++)
                {
                        rm [glorpogop] = (grd [count_x] [count_y]) + 30;
   glorpogop ++;
   rm [glorpogop] = env[0].map [count_x] [count_y] + 30;
   glorpogop ++;
   rm [glorpogop] = mgrd [count_x] [count_y] + 30;
   glorpogop ++;
   rm [glorpogop] = env[0].cgrid [count_x] [count_y] + 30;
   glorpogop ++;
         }
                write(handle, rm, 280);
  glorpogop = 0;
}


for (i = 0; i < NTRAPS; i ++)
{
        rm [0] = env[0].trap_type [i] + 40;
        rm [1] = env[0].trap_x [i] + 40;
        rm [2] = env[0].trap_y [i] + 40;
        write(handle, rm, 3);
}

#ifdef UNBUG
for (count_x = 0; count_x < 80; count_x ++)
{
        for (count_y = 0; count_y < 70; count_y ++)
        {
         if (igrd [count_x] [count_y] < 0 | igrd [count_x] [count_y] > 501)
         {
          itoa(igrd [count_x] [count_y], st_prn, 10);
          cprintf(st_prn);
          cprintf("x3! ");
          getch();
         }
        }
}
#endif

for (i = 0; i < ITEMS; i ++)
{
          rm [0] = mitm.iclass [i] + 100;
          rm [1] = mitm.itype [i] + 100;
          rm [2] = mitm.iplus [i] + 40;
          rm [24] = mitm.iplus2 [i] + 40;
          rm [3] = mitm.idam [i] + 30;
        temp_int = mitm.iquant [i] + 100000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 6; p ++)
        {
                rm [4 + p] = thing_quant [p];
        }

          rm [10] = mitm.icol [i] + 100;
          rm [11] = mitm.ix [i] + 100;
          rm [12] = mitm.iy [i] + 100;
          rm [13] = mitm.iid [i] + 100;

        if (mitm.iquant [i] == 0) mitm.ilink [i] = 501;


        temp_int = mitm.ilink [i] + 50000;
        itoa(temp_int, thing_quant, 10);

        for (p = 0; p < 5; p ++)
        {
                rm [p + 14] = thing_quant [p];
        }

        temp_int = igrd [mitm.ix [i]] [mitm.iy [i]] + 50000;
        itoa(temp_int, thing_quant, 10);

        for (p = 0; p < 5; p ++)
        {
                rm [p + 19] = thing_quant [p];
        }

        write(handle, rm, 25); /* 24 is plus2 */


}


#ifdef UNBUG
for (count_x = 0; count_x < 80; count_x ++)
{
        for (count_y = 0; count_y < 70; count_y ++)
        {
         if (igrd [count_x] [count_y] < 0 | igrd [count_x] [count_y] > 501)
         {
          itoa(igrd [count_x] [count_y], st_prn, 10);
          cprintf(st_prn);
          cprintf("x4! ");
          getch();
         }
        }
}
#endif

rm [0] = env[0].cloud_no + 40;
write(handle, rm, 1);

for (i = 0; i < CLOUDS; i ++)
{
 rm [0] = env[0].cloud_x [i] + 40;
 rm [1] = env[0].cloud_y [i] + 40;
 rm [2] = env[0].cloud_type [i] + 40;

        temp_int = env[0].cloud_decay [i] + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [3 + p] = thing_quant [p];
        }
 write(handle,rm, 9);
}



for (i = 0; i < 5; i ++)
{

 rm [0] = env[0].keeper_name [i] [0] + 40;
 rm [1] = env[0].keeper_name [i] [1] + 40;
 rm [2] = env[0].keeper_name [i] [2] + 40;
 rm [3] = env[0].sh_x [i] + 40;
 rm [4] = env[0].sh_y [i] + 40;
 rm [5] = env[0].sh_greed [i] + 40;
 rm [6] = env[0].sh_type [i] + 40;
 rm [7] = env[0].sh_level [i] + 40;
 if (mgrd [env[0].sh_x [i] - 1] [env[0].sh_y [i] - 1] == 31)
 {
  cprintf("y");
  getch();
 }
 write(handle, rm, 8);

}




for (i = 0; i < 20; i ++)
{
        temp_int = env[0].mons_alloc [i] + 20000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [p] = thing_quant [p];
        }
 write(handle,rm, 5);
}



for (i = 0; i < MNST; i++)
{
 rm [0] = 5;
 rm [1] = 5;
 rm [2] = 5;
 rm [3] = menv [i].m_AC + 40;
        rm [4] = menv [i].m_ev + 40;
        rm [5] = menv [i].m_HD + 40;
        rm [6] = (menv [i].m_speed) + 40;
        rm [7] = menv [i].m_speed_inc + 40;
        rm [8] = menv [i].m_beh + 40;
        rm [9] = menv [i].m_x + 40;
        rm [10] = menv [i].m_y + 40;
        rm [11] = menv [i].m_targ_1_x + 40;
        rm [12] = menv [i].m_targ_1_y + 40;

 rm [13] = 5;

        rm [13] = 0;

 rm [14] = menv [i].m_ench_1 + 40;

 for (j = 0; j < 3; j++)
 {
  rm [15 + j] = menv [i].m_ench [j] + 40;
 }

 write(handle, rm, 18);

 strcpy(thing_quant, "");
        int temp_int = menv [i].m_class + 50080;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [p] = thing_quant [p];
        }
 write(handle, rm, 5);

        temp_int = menv [i].m_hp + 50000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [p] = thing_quant [p];
        }
 write(handle, rm, 5);

        temp_int = menv [i].m_hp_max + 50000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [p] = thing_quant [p];
        }
 write(handle, rm, 5);

        temp_int = menv [i].m_sec + 50000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [p] = thing_quant [p];
        }
 write(handle, rm, 5);

        for (j = 0; j < 8; j ++)
        {
         temp_int = menv [i].m_inv [j] + 50000;
         itoa(temp_int, thing_quant, 10);
         for (p = 0; p < 5; p ++)
         {
                rm [p] = thing_quant [p];
         }
         write(handle, rm, 5);
        }

}

/*
         write(handle, ghost.gname, 20);
         for (j = 0; j < 20; j ++)
         {
          ghost.ghs [j] += 30;
         }

         write(handle, ghost.ghs, 14);
*/
//       strcpy(rm, ghost.gname);
//strcpy(ghost.gname, "Hi!");
//         write(handle, rm, 20);

/*       for (j = 0; j < 14; j ++)
         {
          rm [j] = ghost.ghs [j] + 30;
         }
         write(handle, rm, 14);*/


close(handle);

/*cprintf("End of save function:\n\r");
for (count_x = 0; count_x < 80; count_x ++)
{
 for (count_y = 0; count_y < 70; count_y ++)
 {
  if (mgrd [count_x] [count_y] == MNG) continue;
  if (menv [mgrd [count_x] [count_y]].m_class == -1)
  {
   itoa(count_x, st_prn, 10);
   cprintf(st_prn);
   cprintf("/");
   itoa(count_y, st_prn, 10);
   cprintf(st_prn);
   cprintf("\n\r");
   getch();
  }
 }
}*/



}



void save_game(char leave_game)
{
char rm [69];
int temp_int = 0;
unsigned long temp_long;
char char_f [15];
int handle, p, i, j;
char thing_quant [8];


strcpy(char_f, "");
strncat(char_f, you[0].your_name, 6);
strcat(char_f, ".sav");


/* Use the first one to amend already created files, and the second to create
   new - the 2nd will be used eventually, as save games will be deleted after
   loading. */
#ifdef DOS
strupr(char_f);
#endif
//handle = open(char_f, O_CREAT | O_TRUNC | O_BINARY, 0660);
handle = open(char_f, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0660);


for (p = 0; p < 69; p ++)
{
 rm [p] = 40;
}



if (handle == -1)
{
        perror("Unable to open file for writing");
        exit(-1);
}

for (p = 0; p < 30; p ++)
{
 rm [p] = you[0].your_name [p];
 if (you[0].your_name [p] == 26 | you[0].your_name [p] == 27) rm [p] = 0;
}
write(handle, rm, 30);

rm [0] = you[0].religion + 40;
rm [1] = you[0].piety + 40;
/*rm [2] = you[0].res_fire + 40;
rm [3] = you[0].res_poison + 40;
rm [4] = you[0].res_cold + 40;
rm [5] = you[0].fast_run + 40;*/
rm [6] = you[0].shock_shield + 40;
/*rm [7] = you[0].sust_abil + 40;
rm [8] = you[0].magic_battery + 40;*/
rm [9] = you[0].deaths_door + 40;
/*rm [10] = you[0].spec_fire + 40;
rm [11] = you[0].spec_cold + 40;
rm [12] = you[0].spec_poison + 40;*/
rm [13] = you[0].pet_target + 40;
/*rm [14] = you[0].prot_life + 40;
temp_int = you[0].res_magic + 10000;*/
itoa(temp_int, thing_quant, 10);
for (p = 0; p < 5; p ++)
{
                rm [p + 15] = thing_quant [p];
}
rm [20] = you[0].spell_levels + 80;
rm [21] = you[0].max_level + 40;
rm [22] = you[0].where_are_you + 40;
rm [23] = you[0].char_direction + 40;
rm [24] = you[0].your_level + 40;
rm [25] = you[0].is_undead + 40;
rm [26] = you[0].special_wield + 40;
rm [27] = you[0].berserker + 40;
/*rm [28] = you[0].energy + 40;*/
rm [29] = you[0].level_type + 40;
rm [30] = you[0].corpse_count + 40;
rm [31] = you[0].disease + 40;
/*rm [32] = you[0].spec_death + 40;
rm [33] = you[0].spec_holy + 40;*/
rm [34] = you[0].species + 40;
write(handle, rm, 35);

/*rm [0] = you[0].spec_conj + 40;
rm [1] = you[0].spec_ench + 40;
rm [2] = you[0].spec_summ + 40;
write(handle, rm, 3);*/

temp_int = you[0].hp + 10000;
itoa(temp_int, thing_quant, 10);
for (p = 0; p < 5; p ++)
{
                rm [p] = thing_quant [p];
}

temp_int = you[0].hp_max + 10000;
itoa(temp_int, thing_quant, 10);
for (p = 0; p < 5; p ++)
{
                rm [5 + p] = thing_quant [p];
}

write(handle, rm, 10);


if (you[0].haste > 215) you[0].haste = 215;
rm [0] = you[0].haste + 40;

if (you[0].might > 215) you[0].might = 215;
rm [1] = you[0].might + 40;

if (you[0].lev > 215) you[0].lev = 215;
rm [2] = you[0].lev + 40;

if (you[0].poison > 215) you[0].poison = 215;
rm [3] = you[0].poison + 40;

temp_int = you[0].hunger + 100000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 6; p ++)
        {
                rm [5 + p] = thing_quant [p];
        } /* DON'T save when hunger is -ve */


/*rm [11] = you[0].equip [0] + 40;*/


for (i = 0; i < NO_EQUIP; i++)
{
        rm [12 + i] = you[0].equip [i] + 40;
}

/*rm [18] = you[0].ring [0] + 40;
rm [19] = you[0].ring [1] + 40;
rm [20] = you[0].ring [2] + 40;*/

//rm [21] = 40;

rm [22] = you[0].ep + 40;

rm [23] = you[0].ep_max + 40;

rm [24] = you[0].strength + 40;

rm [25] = you[0].intel + 40;

rm [26] = you[0].dex + 40;

/*rm [27] = you[0].AC + 80; / * Remember that AC shown is 10 - AC, but AC still can be -ve. */

/*rm [28] = you[0].evasion + 40;

rm [29] = you[0].damage + 40;*/

/*temp_int = (int) (you[0].rate_regen * 100) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [31 + p] = thing_quant [p];
        }*/

/*rm [31] = you[0].rate_regen + 40;*/


temp_int = (int) (you[0].incr_regen * 100) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [36 + p] = thing_quant [p];
        }

rm [32] = you[0].incr_regen + 40;

        temp_long = you[0].xp + 100000;
        itoa(temp_long, thing_quant, 10);
        for (p = 0; p < 6; p ++)
        {
                rm [41 + p] = thing_quant [p];
        }



rm [47] = you[0].xl + 40;

temp_int = you[0].gp + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [48 + p] = thing_quant [p];
        }

rm [53] = you[0].clas + 40;

/*temp_int = (int) (you[0].f_abil) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [54 + p] = thing_quant [p];
        }*/

/*temp_int = (int) (you[0].speed * 100) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [59 + p] = thing_quant [p];
        }*/

/*temp_int = (int) (you[0].speed_inc * 100) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [64 + p] = thing_quant [p];
        }*/

        write(handle, rm, 69);

        temp_int = you[0].exp_available + 100000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 6; p ++)
        {
                rm [p] = thing_quant [p];
        }
        write(handle, rm, 6);

/* max values */
rm [0] = you[0].max_strength + 40;
rm [1] = you[0].max_intel + 40;
rm [2] = you[0].max_dex + 40;
rm [3] = you[0].hunger_inc + 40;
rm [4] = you[0].ep_incr_regen + 30;
write(handle, rm, 5);


/*temp_int = (int) (you[0].mag_abil) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [0 + p] = thing_quant [p];
        }*/


temp_int = (int) (you[0].base_hp) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [5 + p] = thing_quant [p];
        }

temp_int = (int) (you[0].base_hp2) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [10 + p] = thing_quant [p];
        }

temp_int = (int) (you[0].base_ep) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [15 + p] = thing_quant [p];
        }

temp_int = (int) (you[0].base_ep2) + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [20 + p] = thing_quant [p];
        }

 write(handle, rm, 25);




        rm [0] = (unsigned char) you[0].x_pos + 40;
        rm [1] = (unsigned char) you[0].y_pos + 40;

        write(handle, rm, 2);

        strcpy(rm, you[0].clasnam);
        write(handle, rm, 30);

temp_int = you[0].burden + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [p] = thing_quant [p];
        }
        write(handle, rm, 5);


 for (i = 0; i < 25; i ++)
 {
  rm [i] = you[0].spells [i] + 30;
 }
 write(handle, rm, 25);

        for (i = 0; i < 52; i++)
        {
        rm [0] = you[0].inv_class [i] + 40;
        rm [1] = you[0].inv_type [i] + 40;
        rm [2] = you[0].inv_plus [i] + 40;
        rm [11] = you[0].inv_plus2 [i] + 40;
        rm [3] = you[0].inv_dam [i] + 30;
        rm [4] = you[0].inv_col [i] + 40;
        rm [5] = you[0].inv_ident [i] + 40;

        temp_int = you[0].inv_quant [i] + 10000;
        itoa(temp_int, thing_quant, 10);
        for (p = 0; p < 5; p ++)
        {
                rm [6 + p] = thing_quant [p];
        }
        write(handle, rm, 12);
        } /* end of for i */

/*              for (i = 0; i < 30; i ++)
        {
                rm [i] = you[0].levels_exist [i];
        }
        write(handle, rm, 30);*/



for (i = 0; i < 5; i ++)
{
   for (j = 0; j < 50; j ++)
   {
    rm [j] = you[0].item_description [i] [j] + 40;
   }

   write(handle, rm, 50);
}

char identy [4] [30];
save_id(identy);


for (i = 0; i < 4; i ++)
{
   for (j = 0; j < 30; j ++)
   {
      rm [j] = identy [i] [j] + 40;
   }
   write(handle, rm, 30);
}


   for (j = 0; j < 50; j ++)
   {
      rm [j] = you[0].skills [j] + 40; /* skills! */
   }
   write(handle, rm, 50);

   for (j = 0; j < 50; j ++)
   {
      rm [j] = you[0].practise_skill [j] + 40; /* skills! */
   }
   write(handle, rm, 50);

   for (j = 0; j < 50; j ++)
   {
    temp_int = you[0].skill_points [j] + 10000;
    itoa(temp_int, thing_quant, 10);
    for (p = 0; p < 6; p ++)
    {
        rm [p] = thing_quant [p];
    }
    write(handle, rm, 6);
   }

   for (j = 0; j < 50; j ++)
   {
      rm [j] = you[0].unique_items [j] + 40; /* unique items */
   }
   write(handle, rm, 50);

   for (j = 0; j < 50; j ++)
   {
      rm [j] = you[0].unique_creatures [j] + 40; /* unique beasties */
   }
   write(handle, rm, 50);

   for (j = 0; j < 30; j ++)
   {
      rm [j] = you[0].duration [j] + 40;
   }
   write(handle, rm, 30);

   for (j = 0; j < 10; j ++)
   {
      rm [j] = you[0].attribute [j] + 40;
   }
   write(handle, rm, 10);

   for (j = 0; j < 20; j ++)
   {
      rm [j] = you[0].branch_stairs [j] + 30;
   }
   write(handle, rm, 20);

   for (j = 0; j < 50; j ++)
   {
      rm [j] = you[0].mutation [j] + 30;
   }
   write(handle, rm, 50);


/*rm [0] = you[0].piety + 40;
rm [1] = you[0].religion + 40;
write(handle, rm, 2);*/

        close(handle);

if (leave_game == 0) return;

/*if (you[0].level_type != 0) was_a_labyrinth = 1;*/
if (you[0].level_type == 0) save_level(you[0].your_level, 0, you[0].where_are_you);
 else save_level(you[0].your_level, 1, you[0].where_are_you);
/* save_level(you[0].your_level);*/

/* was_a_labyrinth = 0;*/



#ifdef DOS
window(1, 1, 80, 25);
#endif

clrscr();

cprintf("See you soon!");

exit(0);

} /* end of save_game() */



void reset_ch(void)
{

        you[0].hp_ch = 1;
/*      you[0].hp_max_ch = 1;*/
        you[0].ep_ch = 1;
/*      you[0].ep_max_ch = 1;*/
        you[0].strength_ch = 1;
        you[0].intel_ch = 1;
        you[0].dex_ch = 1;
        you[0].AC_ch =  1; /* remember that the AC shown = 10 - AC */
        you[0].evasion_ch = 1;
/*      you[0].damage_ch = 1;*/
        you[0].xp_ch = 1;
/*      you[0].xl_ch = 1;*/
        you[0].gp_ch = 1;
        you[0].hung_ch = 1;

}







void restore_game(void)
{
char rm [69];
int temp_int = 0;
long temp_long;
char char_f [15];
int handle, i, j, p;
char thing_quant [8];

strcpy(char_f, "");
strncat(char_f, you[0].your_name, 6);
strcat(char_f, ".sav");

#ifdef DOS
strupr(char_f);
#endif
handle = open(char_f, O_RDONLY, S_IWRITE, S_IREAD);


if (handle == -1)
{
        perror("Unable to open file for reading");
        exit(-1);
}

for (p = 0; p < 69; p ++)
{
 rm [p] = 40;
}

read(handle, rm, 30);
for (p = 0; p < 30; p ++)
{
 you[0].your_name [p] = rm [p];
}

read(handle, rm, 35);
you[0].religion = rm [0] - 40;
you[0].piety = rm [1] - 40;
/*you[0].res_fire = rm [2] - 40;
you[0].res_poison = rm [3] - 40;
you[0].res_cold = rm [4] - 40;
you[0].fast_run = rm [5] - 40;*/
you[0].shock_shield = rm [6] - 40;
/*you[0].sust_abil = rm [7] - 40;
you[0].magic_battery = rm [8] - 40;*/
you[0].deaths_door = rm [9] - 40;
/*you[0].spec_fire = rm [10] - 40;
you[0].spec_cold = rm [11] - 40;
you[0].spec_poison = rm [12] - 40;*/
you[0].pet_target = rm [13] - 40;
/*you[0].prot_life = rm [14] - 40;
        for (p = 0; p < 5; p ++)
        {
                thing_quant [p] = rm [p + 15];
        }
temp_int = atoi(thing_quant);
you[0].res_magic = temp_int - 10000;*/
you[0].spell_levels = rm [20] - 80;
you[0].max_level = rm [21] - 40;
you[0].where_are_you = rm [22] - 40;
you[0].char_direction = rm [23] - 40;
you[0].your_level = rm [24] - 40;
you[0].is_undead = rm [25] - 40;
you[0].special_wield = rm [26] - 40;
you[0].berserker = rm [27] - 40;
/*you[0].energy = rm [28] - 40;*/
you[0].level_type = rm [29] - 40;
you[0].corpse_count = rm [30] - 40;
you[0].disease = rm [31] - 40;
/*you[0].spec_death = rm [32] - 40;
you[0].spec_holy = rm [33] - 40;*/
you[0].species = rm [34] - 40;

/*read(handle, rm, 3);
you[0].spec_conj = rm [0] - 40;
you[0].spec_ench = rm [1] - 40;
you[0].spec_summ = rm [2] - 40;*/


read(handle, rm, 10);

        for (p = 0; p < 5; p ++)
        {
                thing_quant [p] = rm [p];
        }
temp_int = atoi(thing_quant);
you[0].hp = temp_int - 10000;

        for (p = 0; p < 5; p ++)
        {
                thing_quant [p] = rm [5 + p];
        }
temp_int = atoi(thing_quant);
you[0].hp_max = temp_int - 10000;

read(handle, rm, 69);
you[0].haste = rm [0] - 40;
you[0].might = rm [1] - 40;
you[0].lev = rm [2] - 40;
you[0].poison = rm [3] - 40;

        for (p = 0; p < 6; p ++)
        {
                thing_quant [p] = rm [5 + p];
        }
temp_int = atoi(thing_quant);
you[0].hunger = temp_int - 100000;

/*you[0].item_wielded = rm [11] - 40;*/



for (i = 0; i < NO_EQUIP; i ++)
{
 you[0].equip [i] = rm [12 + i] - 40;
}

/*you[0].ring [0] = rm [18] - 40;
you[0].ring [1] = rm [19] - 40;
you[0].ring [2] = rm [20] - 40;*/
you[0].ep = rm [22] - 40;
you[0].ep_max = rm [23] - 40;
you[0].strength = rm [24] - 40;

you[0].intel = rm [25] - 40;
you[0].dex = rm [26] - 40;

/*you[0].AC = rm [27] - 80;
you[0].evasion = rm [28] - 40;*/

/*you[0].damage = rm [29] - 40;

you[0].rate_regen = ((int) ((rm [35] - 48) + (rm [34] - 48) * 10 + (rm [33] - 48) * 100 + (rm [32] - 48) * 1000));
you[0].rate_regen = rm [31] - 40;*/


you[0].incr_regen = ((int) ((rm [40] - 48) + (rm [39] - 48) * 10 + (rm [38] - 48) * 100 + (rm [37] - 48) * 1000));
you[0].incr_regen = rm [32] - 40;


for (p = 0; p < 6; p ++)
{
        thing_quant [p] = rm [41 + p];
}
temp_long = atol(thing_quant);
you[0].xp = temp_long - 100000;

you[0].xl = rm [47] - 40;

        you[0].gp = (rm [52] - 48) + (rm [51] - 48) * 10 + (rm [50] - 48) * 100 + (rm [49] - 48) * 1000;

you[0].clas = rm [53] - 40;


/*      you[0].f_abil = (int) ((rm [58] - 48) + (rm [57] - 48) * 10 + (rm [56] - 48) * 100 + (rm [55] - 48) * 1000);

        you[0].speed = (int) ((rm [63] - 48) + (rm [62] - 48) * 10 + (rm [61] - 48) * 100 + (rm [60] - 48) * 1000);

        you[0].speed_inc = (int) ((rm [68] - 48) + (rm [67] - 48) * 10 + (rm [66] - 48) * 100 + (rm [65] - 48) * 1000);
*/

read(handle, rm, 6);
for (p = 0; p < 6; p ++)
{
        thing_quant [p] = rm [p];
}
temp_long = atol(thing_quant);
you[0].exp_available = temp_long - 100000;

read(handle, rm, 5);
you[0].max_strength = rm [0] - 40;
you[0].max_intel = rm [1] - 40;
you[0].max_dex = rm [2] - 40;
you[0].hunger_inc = rm [3] - 40;
you[0].ep_incr_regen = rm [4] - 30;

 read(handle, rm, 25);

/*you[0].mag_abil = (int) ((rm [4] - 48) + (rm [3] - 48) * 10 + (rm [2] - 48) * 100 + (rm [1] - 48) * 1000);*/
you[0].base_hp = (int) ((rm [9] - 48) + (rm [8] - 48) * 10 + (rm [7] - 48) * 100 + (rm [6] - 48) * 1000);
you[0].base_hp2 = (int) ((rm [14] - 48) + (rm [13] - 48) * 10 + (rm [12] - 48) * 100 + (rm [11] - 48) * 1000);
you[0].base_ep = (int) ((rm [19] - 48) + (rm [18] - 48) * 10 + (rm [17] - 48) * 100 + (rm [16] - 48) * 1000);
you[0].base_ep2 = (int) ((rm [24] - 48) + (rm [23] - 48) * 10 + (rm [22] - 48) * 100 + (rm [21] - 48) * 1000);




        read(handle, rm, 2);

        you[0].x_pos = (int) rm [0] - 40;
        you[0].y_pos = (int) rm [1] - 40;

        read(handle, rm, 30);

        strcpy(you[0].clasnam, rm);

read(handle, rm, 5);

        you[0].burden = (rm [4] - 48) + (rm [3] - 48) * 10 + (rm [2] - 48) * 100 + (rm [1] - 48) * 1000;
        /*  burden reloads wrongly if it is negative. */

 you[0].spell_no = 0;
 read(handle, rm, 25);
 for (i = 0; i < 25; i ++)
 {
  you[0].spells [i] = rm [i] - 30;
  if (you[0].spells [i] != 210) you[0].spell_no ++;
 }

you[0].inv_no = 0;


for (i = 0; i < 52; i++)
{
        read(handle, rm, 12);

        you[0].inv_class [i] = rm [0] - 40;
        you[0].inv_type [i] = rm [1] - 40;
you[0].inv_plus [i] = rm [2] - 40;
you[0].inv_plus2 [i] = rm [11] - 40;
you[0].inv_dam [i] = rm [3] - 30;
you[0].inv_col [i] = rm [4] - 40;


        you[0].inv_ident [i] = rm [5] - 40;

        you[0].inv_quant [i] = (rm [10] - 48) + (rm [9] - 48) * 10 + (rm [8] - 48) * 100 + (rm [7] - 48) * 1000;

        if (you[0].inv_quant [i] > 0) you[0].inv_no++;

} /* end of for i */

/*
        read(handle, rm, 30);
        for (i = 0; i < 30; i ++)
        {
                you[0].levels_exist [i] = rm [i];
        }*/


for (i = 0; i < 5; i ++)
{
read(handle, rm, 50);

   for (j = 0; j < 50; j ++)
   {
    you[0].item_description [i] [j] = rm [j] - 40;
   }
}

for (i = 0; i < 4; i ++)
{
read(handle, rm, 30);

   for (j = 0; j < 30; j ++)
   {
 switch(i)
 {
        case 0: set_id(3, j, rm [j] - 40); break;
        case 1: set_id(6, j, rm [j] - 40); break;
        case 2: set_id(7, j, rm [j] - 40); break;
        case 3: set_id(8, j, rm [j] - 40); break;
 }
   }
}
   read(handle, rm, 50);

   for (j = 0; j < 50; j ++)
   {
      you[0].skills [j] = rm [j] - 40; /* skills! */
   }

   read(handle, rm, 50);

   for (j = 0; j < 50; j ++)
   {
      you[0].practise_skill [j] = rm [j] - 40; /* skills! */
   }


for (j = 0; j < 50; j ++)
{
  read(handle, rm, 6);
  for (p = 0; p < 6; p ++)
  {
        thing_quant [p] = rm [p];
  }
  temp_int = atoi(thing_quant);
  you[0].skill_points [j] = temp_int - 10000;
}

   read(handle, rm, 50);

   for (j = 0; j < 50; j ++)
   {
      you[0].unique_items [j] = rm [j] - 40; /* unique items */
   }

   read(handle, rm, 50);

   for (j = 0; j < 50; j ++)
   {
      you[0].unique_creatures [j] = rm [j] - 40; /* unique creatures */
   }

   read(handle, rm, 30);
   for (j = 0; j < 30; j ++)
   {
      you[0].duration [j] = rm [j] - 40;
   }
   read(handle, rm, 10);
   for (j = 0; j < 10; j ++)
   {
      you[0].attribute [j] = rm [j] - 40;
   }

   read(handle, rm, 20);
   for (j = 0; j < 20; j ++)
   {
      you[0].branch_stairs [j] = rm [j] - 30;
   }

   read(handle, rm, 50);
   for (j = 0; j < 50; j ++)
   {
      you[0].mutation [j] = rm [j] - 30;
   }

/*
read(handle, rm, 2);
you[0].piety = rm [0] - 40;
you[0].religion = rm [1] - 40;*/


        close(handle);


/*

 char del_file [25];

        strcpy(del_file, char_f);
        sysg = unlink(del_file);

*/

/*
   you[0].speed = 10;*/



} /* end of restore_game() */

/* This file was optimized & heavily rewritten by Alexey Guzeev */

#include "config.h"

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"
#include "enum.h"

#ifdef DOS
#include <file.h>
#endif

#ifdef LINUX
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#ifdef USE_EMX
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#ifdef MAC
#include <stat.h>
#else
#include <sys/stat.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "dungeon.h"
#include "files.h"
#include "itemname.h"
#include "message.h"
#include "misc.h"
#include "mstruct.h"
#include "mstuff2.h"
#include "output.h"
#include "player.h"
#include "randart.h"
#include "skills2.h"
#include "stuff.h"
#include "view.h"

/*
Order for looking for conjurations for the 1st & 2nd spell slots,
when finding spells to be remembered by a player's ghost:
*/
unsigned char search_order_conj [] = {
/* 0 */
54, // crystal spear
53, // -ve energy
148, // agony
151, // disintegrate
17, // lightning
79, // sticky flame
60, // isk's blast
15, // firebolt
16, // cold bolt
6, // fireball
/* 10 */
35, // venom bolt
128, // iron
129, // stone arrow
26, // thr fl
27, // thr fr
67, // pain
115, // sting
5, // m dart
250, // terminate search
};

/*
Order for looking for summonings and self-enchants for the 3rd spell slot:
*/
unsigned char search_order_third [] = {
/* 0 */
158, // symbol of torment
121, // summon gr dmn
72, // summon wraiths
62, // summon hor th
119, // summon demon
120, // demonic horde
22, // haste
28, // animate dead
25, // invis
82, // call imp
49, // summon small mammal
/* 10 */
28, // controlled blink
59, // blink
250, // end search
};


/*
Order for looking for enchants for the 4th + 5th spell slot. If fails, will
go through conjs.
Note: Dig must be in misc2 (5th) position to work.
*/
unsigned char search_order_misc [] = {
/* 0 */
148, // agony - evil spell
113, // banishment
23, // paralyse
24, // confuse
21, // slow
20, // polymorph other
37, // teleport other
14, // dig
250, // end search
};

/* Last slot (emergency) can only be teleport self or blink. */


int write2(FILE *file, char *buffer, unsigned int count);
int read2(FILE *file, char *buffer, unsigned int count);

static void reset_ch () {
  you[0].hp_ch = 1;
  you[0].ep_ch = 1;
  you[0].strength_ch = 1;
  you[0].intel_ch = 1;
  you[0].dex_ch = 1;
  you[0].AC_ch =  1;
  you[0].evasion_ch = 1;
  you[0].xp_ch = 1;
  you[0].gp_ch = 1;
  you[0].hung_ch = 1;
  you[0].hung_state = 3;
}


struct ghost_struct ghost;


extern unsigned char your_sign; /* these two are defined in view.cc */
extern unsigned char your_colour;

unsigned char translate_spell(unsigned char spel);
unsigned char search_third_list(unsigned char ignore_spell);
unsigned char search_second_list(unsigned char ignore_spell);
unsigned char search_first_list(unsigned char ignore_spell);
char find_spell(unsigned char which_sp);
void add_spells(unsigned char buffer [40]);
void generate_random_demon(void);

static void save_int (char **pp, int val, int digits) {
  char *p=*pp;
  char thing_quant[8];
  int add=1;
  for (int i=1; i<digits; ++i) add*=10;
  itoa(val+add, thing_quant, 10);
  for (int i=0; i<digits; ++i) *p++=thing_quant[i];
  *pp=p;
}


static int load_int (char **pp, int digits) {
  char *p=*pp;
  char thing_quant[8];
  int add=1;
  thing_quant[0]=*p++;
  for (int i=1; i<digits; ++i) {
    thing_quant[i]=*p++;
    add*=10;
  }
  *pp=p;
  thing_quant[digits]=0;
  return atoi(thing_quant)-add;
}


void load (unsigned char stair_taken, char moving_level, char was_a_labyrinth, char old_level, char want_followers, char just_made_new_lev, char where_were_you2) {
  int j = 0;
  int i, count_x, count_y;
  char cha_fil [kFileNameSize];

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

#ifdef DOS_TERM
  window(1,1,80,25);
#endif

  char hbjh [5];

  strcpy(corr_level, "");
  if (you[0].your_level<10) strcpy(corr_level, "0");
  itoa(you[0].your_level, hbjh, 10);
  strcat(corr_level, hbjh);
  corr_level [2] = you[0].where_are_you + 97;
  corr_level [3] = 0; /* null-terminate it */
  strncpy(cha_fil, you[0].your_name, kFileNameLen);
  cha_fil [kFileNameLen] = 0;
  strcat(cha_fil, ".");
  if (you[0].level_type!=0) strcat(cha_fil, "lab"); /* temporary level */
  else strcat(cha_fil, corr_level);


  you[0].prev_targ=MHITNOT;

  int following = -1;
  int fmenv = 0;

  int minvc = 0;

  if (moving_level==1) {
    for (int clouty=0; clouty<CLOUDS; ++clouty) {
      env[0].cloud_type[clouty]=0;
      env[0].cgrid[env[0].cloud_x [clouty]] [env[0].cloud_y [clouty]]=CNG;
      env[0].cloud_decay[clouty]=0;
      --env[0].cloud_no;
    }
    env[0].cloud_no=0;
  }

  if (want_followers == 1 && just_made_new_lev == 0) {
    for (count_x = you[0].x_pos - 1; count_x < you[0].x_pos + 2; count_x ++) {
      for (count_y = you[0].y_pos - 1; count_y < you[0].y_pos + 2; count_y ++) {
        if (count_x == you[0].x_pos && count_y == you[0].y_pos) continue;
        following ++;
        foll_class [following] = -1;
        if (mgrd [count_x] [count_y] == MNG) continue;
        fmenv = mgrd [count_x] [count_y];
        if ((menv[fmenv].m_class==400) || (menv[fmenv].m_class==19) || (menv[fmenv].m_class == 401) || (menv[fmenv].m_class==56) || (menv[fmenv].m_class==-1)) continue;
        if (menv [fmenv].m_class >= MLAVA0) continue;
        if (menv [fmenv].m_speed_inc < 50) continue;
        foll_class [following] = menv [fmenv].m_class;
//        itoa(foll_class [following], st_prn, 10);

        foll_hp [following] = menv [fmenv].m_hp;
        foll_hp_max [following] = menv [fmenv].m_hp_max;
        foll_HD [following] = menv [fmenv].m_HD;
        foll_AC [following] = menv [fmenv].m_AC;
        foll_ev [following] = menv [fmenv].m_ev;
        foll_speed [following] = menv [fmenv].m_speed;
        foll_speed_inc [following] = menv [fmenv].m_speed_inc;
        foll_targ_1_x [following] = menv [fmenv].m_targ_1_x;
        foll_targ_1_y [following] = menv [fmenv].m_targ_1_y;
        for (minvc=0; minvc<8; ++minvc) foll_inv[following][minvc]=501;
        for (minvc=0; minvc<8; ++minvc) {
          if (menv[fmenv].m_inv[minvc]==501) {
            fit_iquant[following][minvc]=0;
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
        for (j=0; j<3; ++j) menv [fmenv].m_ench[j]=0;
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

  strcpy(ghost.gname, "");
  for (ic=0; ic<20; ++ic) ghost.ghs [ic] = 0;

#ifdef DOS
  strupr(cha_fil);
#endif

  FILE *handle;
  int handle2 = open(cha_fil, O_RDONLY, O_CREAT | O_TRUNC | O_BINARY, 0660);
  if (handle2 != -1)
  {
         close(handle2);
     handle = fopen(cha_fil, "rb");
  } else
//  if (handle2==-1)
    { /* generate new level */
    strcpy(ghost.gname, "");
    for (int imn=0; imn<20; ++imn) ghost.ghs[imn]=0;

    builder(you[0].your_level, you[0].level_type);

    if (you[0].level_type == 3) generate_random_demon();

    if (random2(3) == 0 && you[0].your_level > 1)
    {
      strcpy(corr_level, "");
      if (you[0].your_level<10) strcpy(corr_level, "0");
      itoa(you[0].your_level, hbjh, 10);
      strcat(corr_level, hbjh);
      corr_level[2]=you[0].where_are_you+97;
      corr_level [3] = 0; /* null-terminate it */
      strcpy(cha_fil, "bones.");
      if (you[0].level_type!=0) strcat(cha_fil, "lab"); /* temporary level */
      else strcat(cha_fil, corr_level);

      int gfile2 = open(cha_fil, S_IWRITE, S_IREAD);

      if (gfile2!=-1) {
        close(gfile2);
//        gfile = open(cha_fil, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0660);
//        gfile = open(cha_fil, O_RDONLY, O_CREAT | O_TRUNC | O_BINARY, 0660);
       FILE *gfile = fopen(cha_fil, "rb");
        if (gfile==NULL) {
          strcpy(info, "Error opening ghost file: ");
          strcat(info, cha_fil);
          mpr(info);
          more();
        } else {
          char buf1[40];
          read2(gfile, buf1, 40);
          fclose(gfile);
          for (int iiii=0; iiii<20; ++iiii) ghost.gname[iiii]=buf1[iiii];
          ghost.ghs[0]=buf1[20];
          ghost.ghs[1]=buf1[21];
          ghost.ghs[2]=buf1[22];
          ghost.ghs[3]=buf1[23];
          ghost.ghs[4]=buf1[24];
          ghost.ghs[5]=buf1[25];
          ghost.ghs[6]=buf1[26];
          ghost.ghs[7]=buf1[27];
          ghost.ghs[8]=buf1[28];
          /* note - as ghosts, automatically get res poison + prot_life */
          ghost.ghs[9]=buf1[29];
          ghost.ghs[10]=buf1[30];
          ghost.ghs[11]=buf1[31];
          ghost.ghs[12]=buf1[32];
          ghost.ghs[13]=buf1[33];

          ghost.ghs[14]=buf1[34];
          ghost.ghs[15]=buf1[35];
          ghost.ghs[16]=buf1[36];
          ghost.ghs[17]=buf1[37];
          ghost.ghs[18]=buf1[38];
          ghost.ghs[19]=buf1[39];
          unlink(cha_fil);
          for (int imn = 0; imn < MNST - 10; imn ++) {
            if (menv [imn].m_class!=-1) continue;
            menv [imn].m_class = 400;
            menv [imn].m_HD = ghost.ghs [12];
            menv [imn].m_hp = ghost.ghs [0];
            menv [imn].m_hp_max = ghost.ghs [0];
            menv [imn].m_AC = ghost.ghs [2];
            menv [imn].m_ev = ghost.ghs [1];
            menv [imn].m_speed = 10;
            menv [imn].m_speed_inc = 70;
            if (ghost.ghs [14] != 250 || ghost.ghs [15] != 250 || ghost.ghs [16] != 250 || ghost.ghs [17] != 250 || ghost.ghs [18] != 250 || ghost.ghs [19] != 250)
              menv [imn].m_sec = 119; else menv [imn].m_sec = 250;
            do {
              menv [imn].m_x = random2(60) + 10;
              menv [imn].m_y = random2(50) + 10;
            } while ((grd[menv[imn].m_x][menv[imn].m_y]!=67) || (mgrd[menv[imn].m_x][menv[imn].m_y]!=MNG));
            mgrd [menv [imn].m_x] [menv [imn].m_y] = imn;
            break;
          }
        }
      }
    }

    for (i = 0; i < GXM; i ++) {
      for (j = 0; j < GYM; j ++) {
        env[0].map [i] [j] = 0;
        if ((you[0].char_direction==1) && (you[0].level_type!=3)) {
          /* closes all the gates if you're on the way out */
          if ((grd[i][j]==69) || (grd[i][j]==96) || (grd[i][j]==99)) grd[i][j]=98;
        }
        env[0].cgrid [i] [j] = CNG;
      }
    }

    for (i = 0; i < MNST; i ++) {
      if (menv[i].m_class==255) menv[i].m_class=-1;
    }

    if (just_made_new_lev == 0) {
      if (stair_taken == 69 | stair_taken == 81) stair_taken = 86;
      else if (stair_taken < 86) stair_taken += 4;
      else if (stair_taken >= 130 && stair_taken < 150) stair_taken -= 20;
      else if (stair_taken >= 110 && stair_taken < 130) stair_taken += 20;
      else if (stair_taken > 90) stair_taken = 86;
      else if (stair_taken == 67) stair_taken = 67;
      else stair_taken -= 4;

      for (count_x = 0; count_x < GXM; count_x ++) {
        for (count_y = 0; count_y < GYM; count_y ++) {
          if (grd [count_x] [count_y] == stair_taken) goto found_stair;
        }
      }

      if (stair_taken < 86) stair_taken=82;
      else stair_taken=86;

      for (count_x = 0; count_x < GXM; count_x ++) {
        for (count_y = 0; count_y < GYM; count_y ++) {
          if (grd [count_x] [count_y] == stair_taken) goto found_stair;
        }
      }
      for (count_x = 0; count_x < GXM; count_x ++)
      {
        for (count_y = 0; count_y < GYM; count_y ++)
        {
          if (grd [count_x] [count_y] == 67) goto found_stair;
        }
      }
    }

found_stair :

    if (just_made_new_lev == 0) {
      you[0].x_pos = count_x;
      you[0].y_pos = count_y;
    }

    if ((you[0].level_type==1) || (you[0].level_type==2)) grd[you[0].x_pos][you[0].y_pos]=67;

    following = 0;
    fmenv = -1;

    if (((you[0].level_type==0) || (you[0].level_type==3)) && (want_followers==1) && (just_made_new_lev==0)) {
      for (ic = 0; ic < 2; ic ++) {
        for (count_x = you[0].x_pos - 6; count_x < you[0].x_pos + 7; count_x ++) {
          for (count_y = you[0].y_pos - 6; count_y < you[0].y_pos + 7; count_y ++) {
            if ((ic==0) &&
                (
                 (count_x<you[0].x_pos-1) ||
                 (count_x>you[0].x_pos+1) ||
                 (count_y<you[0].y_pos-1) ||
                 (count_y>you[0].y_pos+1)
                )
               ) continue;
            if ((count_x==you[0].x_pos) && (count_y==you[0].y_pos)) continue;
            if ((mgrd[count_x][count_y]!=MNG) || (grd[count_x][count_y]<67)) continue;
            while (menv [following].m_class != -1) {
             following ++;
             if (following >= MNST) goto out_of_foll;
            }

            while (fmenv < 7) {
              fmenv ++;
              if (foll_class [fmenv] == -1) continue;
              menv [following].m_class = foll_class [fmenv];
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
              for (minvc = 0; minvc < 8; minvc ++) {
                if (fit_iquant[fmenv][minvc]==0) {
                  menv [following].m_inv [minvc] = 501;
                  continue;
                }
                itmf = 0;
                while (mitm.iquant[itmf]>0) ++itmf;
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
            }
          }
        }
      }
    } /* end if level_type == 0 */
out_of_foll :

    reset_ch();

    moving_level = 0;

    for (i = 0; i < MNST; i++) {
      if (menv [i].m_class == -1) continue;
      for (j = 0; j < 8; j ++) {
        if (menv [i].m_inv [j] == 501) continue;
        if (mitm.ilink [menv [i].m_inv [j]] != 501) {
          /* items carried by monsters shouldn't be linked */
          mitm.ilink [menv [i].m_inv [j]] = 501;
        }
      }
    }

    if (you[0].level_type == 3) {
      for (count_x = 0; count_x < GXM; count_x ++) {
        for (count_y = 0; count_y < GYM; count_y ++) {
          if ((grd[count_x][count_y]>=86) && (grd[count_x][count_y]<=89)) {
            grd [count_x] [count_y] = 67;
            if (random2(30) == 0) grd [count_x] [count_y] = 100;
          }
          if ((grd[count_x][count_y]>=81) && (grd[count_x][count_y]<=85)) {
            grd [count_x] [count_y] = 101;
          }
        }
      }
    }

    save_level(you[0].your_level, (you[0].level_type != 0), you[0].where_are_you);
    return;
  }

  moving_level = 0;

  for (count_x = 0; count_x < ITEMS; count_x ++) mitm.ilink[count_x]=501;

  for (i = 0; i < GXM; i ++) {
    for (j = 0; j < GYM; j ++) igrd [i] [j] = 501;
  }

  const int datalen=20+20+4*80*70+3*NTRAPS+25*ITEMS+1+9*CLOUDS+5*8+5*20+(18+5+5+5+5+8*5)*MNST;
  char *buf=(char*)malloc(datalen);
  char *p=buf;
  int retval=read2(handle, buf, datalen);
  if (datalen!=retval) {
    perror("opa (7)...");
    cprintf(EOL"Wanted to read ");
    itoa(datalen, st_prn, 10);
    cprintf(st_prn);
    cprintf(" bytes; could only read ");
    itoa(retval, st_prn, 10);
    cprintf(st_prn);
    cprintf(".");
    end(-1);
  }
  fclose(handle);

  for (i=0; i<20; ++i) ghost.gname[i]=*p++;
  for (i=0; i<20; ++i) ghost.ghs[i]=*p++;
//  for (j=0; j<20; ++j) ghost.ghs[j]-=30;

  for (count_x = 0; count_x < GXM; count_x ++) {
    for (count_y = 0; count_y < GYM; count_y ++) {
      grd[count_x][count_y]=*p++;
      env[0].map[count_x][count_y]=*p++;
      if (env[0].map [count_x] [count_y] == 201) env[0].map [count_x] [count_y] = 239;
      mgrd [count_x] [count_y] = MNG; ++p;
      if ((mgrd[count_x][count_y]!=MNG) &&
          (
           (menv[mgrd[count_x][count_y]].m_class==-1) ||
           (menv[mgrd[count_x][count_y]].m_x!=count_x) ||
           (menv[mgrd[count_x][count_y]].m_y!=count_y)
          )) {
        mgrd [count_x] [count_y] = MNG; /* This is one of the worst things I've ever done */
      }
      env[0].cgrid[count_x][count_y]=*p++;
    }
  }

  for (i = 0; i < NTRAPS; i ++) {
    env[0].trap_type[i]=*p++;
    env[0].trap_x[i]=*p++;
    env[0].trap_y[i]=*p++;
  }

  for (count_x = 0; count_x < GXM; count_x ++) {
    for (count_y = 0; count_y < GYM; count_y ++) {
      if ((igrd[count_x][count_y]<0) || (igrd[count_x][count_y]>501)) {
        igrd [count_x] [count_y] = 501;
      }
    }
  }

  for (i=0; i<ITEMS; ++i) {
    mitm.iclass[i]=*p++;
    mitm.itype[i]=*p++;
    mitm.iplus[i]=*p++;
    mitm.idam[i]=*p++;
    mitm.iquant[i]=load_int(&p, 6);
    mitm.icol[i]=*p++;
    mitm.ix[i]=*p++;
    mitm.iy[i]=*p++;
    mitm.iid[i]=*p++;
    mitm.ilink[i]=load_int(&p, 5)-40000;
    igrd[mitm.ix[i]][mitm.iy[i]]=load_int(&p, 5)-40000;
    mitm.iplus2[i]=*p++;
    if (mitm.iclass[i]==100) {
      mitm.iquant[i]=0;
      mitm.ilink[i]=501;
    }
  }

  env[0].cloud_no=*p++;

  for (i=0; i<CLOUDS; ++i) {
    env[0].cloud_x[i]=*p++;
    env[0].cloud_y[i]=*p++;
    env[0].cloud_type[i]=*p++;
    env[0].cloud_decay[i]=load_int(&p, 5);
    ++p;
  }

  for (i=0; i<5; ++i) {
    env[0].keeper_name[i][0]=*p++;
    env[0].keeper_name[i][1]=*p++;
    env[0].keeper_name[i][2]=*p++;
    env[0].sh_x[i]=*p++;
    env[0].sh_y[i]=*p++;
    env[0].sh_greed[i]=*p++;
    env[0].sh_type[i]=*p++;
    env[0].sh_level[i]=*p++;
    if (mgrd[env[0].sh_x[i]-1][env[0].sh_y[i]-1]==31) {
      cprintf("x");
      getch();
    }
  }

  for (i = 0; i < 20; i ++) env[0].mons_alloc[i]=load_int(&p, 5)-10000;

  for (count_x=0; count_x<MNST; ++count_x) {
    p+=3;
    menv[count_x].m_AC=*p++;
    menv[count_x].m_ev=*p++;
    menv[count_x].m_HD=*p++;
    menv[count_x].m_speed=*p++;
    menv[count_x].m_speed_inc=*p++;
    menv[count_x].m_beh=*p++;
    menv[count_x].m_x=*p++;
    menv[count_x].m_y=*p++;
    menv[count_x].m_targ_1_x=*p++;
    menv[count_x].m_targ_1_y=*p++;
    ++p;
    menv[count_x].m_ench_1=*p++;
    for (j=0; j<3; ++j) menv[count_x].m_ench[j]=*p++;
    menv[count_x].m_class=load_int(&p, 5)-40080;
    menv[count_x].m_hp=load_int(&p, 5)-40000;
    menv[count_x].m_hp_max=load_int(&p, 5)-40000;
    menv[count_x].m_sec=load_int(&p, 5)-40000;
    for (j=0; j<8; ++j) menv[count_x].m_inv[j]=load_int(&p, 5)-40000;
    for (j=0; j<MNST; ++j) {
      if (menv[j].m_class!=-1) mgrd[menv[j].m_x][menv [j].m_y]=j;
    }
  }

  reset_ch();

  free(buf);
  if (p!=buf+datalen) {
    perror("opa (6)...");
    end(-1);
  }

  for (i = 0; i < GXM; i ++) {
    for (j = 0; j < GYM; j ++) {
      if (igrd [i] [j] < 0 | igrd [i] [j] > 501) igrd [i] [j] = 501;
    }
  }
  for (i = 0; i < MNST; i ++) {
    for (j = 0; j < 8; j ++) {
      if ((menv[i].m_inv[j]<0) || (menv[i].m_inv[j]>501)) menv[i].m_inv[j]=501;
      if (menv [i].m_inv [j] != 501) mitm.ilink [menv [i].m_inv [j]] = 501;
    }
  }
  for (i = 0; i < ITEMS; i ++) {
    if (mitm.ilink [i] > 501) mitm.ilink [i] = 501;
  }
  for (i = 0; i < MNST; i++) {
    if (menv [i].m_class == -1) continue;
    for (j = 0; j < 8; j ++) {
      if (menv [i].m_inv [j] == 501) continue;
      if (mitm.ilink [menv [i].m_inv [j]] != 501) {
        mitm.ilink [menv [i].m_inv [j]] = 501;
      }
    }
  }
  if (you[0].your_level == 35 && stair_taken >= 86) {
    do {
     you[0].x_pos = 10 + random2(GXM - 10);
     you[0].y_pos = 10 + random2(GYM - 10);
    } while ((grd[you[0].x_pos][you[0].y_pos]!=67) || (mgrd[you[0].x_pos][you[0].y_pos]!=MNG));
    count_x = you[0].x_pos;
    count_y = you[0].y_pos;
    goto found_stair;
  } else {
        if (stair_taken == 67)
     for (count_x = 0; count_x < GXM; count_x ++)
     {
       for (count_y = 0; count_y < GYM; count_y ++)
       {
         if (grd [count_x] [count_y] == stair_taken) goto found_stair;
       }
     }
    if (stair_taken >= 130 && stair_taken < 150) stair_taken -= 20;
    else if (stair_taken >= 110 && stair_taken < 130) stair_taken += 20;
    else if (stair_taken < 86) stair_taken += 4;
    else stair_taken -= 4;
    for (count_x = 0; count_x < GXM; count_x ++) {
      for (count_y = 0; count_y < GYM; count_y ++) {
        if (grd [count_x] [count_y] == stair_taken) goto found_stair;
      }
    }
    if (stair_taken < 86) stair_taken = 82;
    else stair_taken = 86;
    for (count_x = 0; count_x < GXM; count_x ++) {
      for (count_y = 0; count_y < GYM; count_y ++) {
        if (grd [count_x] [count_y] == stair_taken) goto found_stair;
      }
    }
  }
  for (count_x = 0; count_x < GXM; count_x ++) {
    for (count_y = 0; count_y < GYM; count_y ++) {
      if ((mgrd[count_x][count_y]!=MNG) &&
          (
           (menv[mgrd[count_x][count_y]].m_class==-1)  ||
           (menv[mgrd[count_x][count_y]].m_x!=count_x) ||
           (menv [mgrd [count_x] [count_y]].m_y != count_y)
          )
         ) {
        mgrd [count_x] [count_y] = MNG; /* This is one of the worst things I've ever done */
      }
    }
  }
} /* end of void load_level(); */


void save_level (int level_saved, char was_a_labyrinth, char where_were_you) {
  char cha_fil[kFileNameSize];
  char extens[5];
  int count_x, count_y;
  int i, j;
  char hbjh[5];

  strcpy(extens, "");
  if (level_saved < 10) strcpy(extens, "0");
  itoa(level_saved, hbjh, 10);
  strcat(extens, hbjh);
  extens [2] = where_were_you + 97;
  extens [3] = 0; /* null-terminate it */
  strncpy(cha_fil, you[0].your_name, kFileNameLen);
  cha_fil [kFileNameLen] = 0;
  strcat(cha_fil, ".");
  if (was_a_labyrinth == 1) strcat(cha_fil, "lab"); /* temporary level */
  else strcat(cha_fil, extens);

  you[0].prev_targ=MHITNOT;

#ifdef DOS
  strupr(cha_fil);
#endif

  int fry;
  int frx;

/* Setting up ix & iy, which aren't normally used: */

  for (frx = 0; frx < MNST; frx ++) {
    for (fry = 0; fry < 8; fry ++) {
      if (menv [frx].m_inv [fry] != 501) {
        mitm.ix [menv [frx].m_inv [fry]] = 2;
        mitm.iy [menv [frx].m_inv [fry]] = 2;
        mitm.ilink [menv [frx].m_inv [fry]] = 501;
      }
    }
  }

  for (count_x = 0; count_x < 80; count_x ++) {
    for (count_y = 0; count_y < 70; count_y ++) {

      int count_out = 0;

      if (igrd [count_x] [count_y] < 0 | igrd [count_x] [count_y] > 501) igrd [count_x] [count_y] = 501;
      if (igrd [count_x] [count_y] > 501) igrd [count_x] [count_y] = 501;
      if (igrd [count_x] [count_y] == 501) continue;

      frx = igrd [count_x] [count_y];

      while(frx != 501) {
        mitm.ix [frx] = count_x;
        mitm.iy [frx] = count_y;

        if (frx > 501 | frx < 0) {
          cprintf("Error! Item out of bounds: ");
          itoa(frx, st_prn, 10);
          cprintf(st_prn);
          if (getch() == 0) getch();
          cprintf(EOL);
          break;
        }

        fry = mitm.ilink [frx];
        frx = fry;
        ++count_out;
        if (count_out > 1000) {
          count_out = 0;
          mitm.ilink [frx] = 501;
          mpr("Item link error.");
          break;
        }

        if (frx == 501) break;
      }
    }
  }

  const int datalen=20+20+4*80*70+3*NTRAPS+25*ITEMS+1+9*CLOUDS+5*8+5*20+(18+5+5+5+5+8*5)*MNST;
  char *buf=(char*)malloc(datalen);
  char *p=buf;

  unsigned char ghost_bak[20];
  for (j = 0; j < 20; j ++) ghost_bak [j] = ghost.ghs [j];
  for (i=0; i<20; ++i) *p++=ghost.gname[i];
  for (i=0; i<20; ++i) *p++=ghost_bak[i];

  for (count_x = 0; count_x < 80; count_x ++) {
    for (count_y = 0; count_y < 70; count_y++) {
      *p++=(grd [count_x] [count_y]);
      *p++=env[0].map [count_x] [count_y];
      *p++=mgrd [count_x] [count_y];
      *p++=env[0].cgrid [count_x] [count_y];
    }
  }

  for (i=0; i<NTRAPS; ++i) {
    *p++=env[0].trap_type [i];
    *p++=env[0].trap_x [i];
    *p++=env[0].trap_y [i];
  }

  for (i=0; i<ITEMS; ++i) {
    *p++=mitm.iclass[i];
    *p++=mitm.itype[i];
    *p++=mitm.iplus[i];
    *p++=mitm.idam[i];
    save_int(&p, mitm.iquant[i], 6);
    *p++=mitm.icol[i];
    *p++=mitm.ix[i];
    *p++=mitm.iy[i];
    *p++=mitm.iid[i];
    if (mitm.iquant[i]==0) mitm.ilink[i]=501;
    save_int(&p, mitm.ilink[i]+40000, 5);
    save_int(&p, igrd[mitm.ix[i]][mitm.iy[i]]+40000, 5);
    *p++=mitm.iplus2[i];
  }

  *p++=env[0].cloud_no;

  for (i = 0; i < CLOUDS; i ++) {
    *p++=env[0].cloud_x[i];
    *p++=env[0].cloud_y[i];
    *p++=env[0].cloud_type[i];
    save_int(&p, env[0].cloud_decay[i], 5);
    *p++=0;
  }

  for (i = 0; i < 5; i ++) {
    *p++=env[0].keeper_name [i] [0];
    *p++=env[0].keeper_name [i] [1];
    *p++=env[0].keeper_name [i] [2];
    *p++=env[0].sh_x [i];
    *p++=env[0].sh_y [i];
    *p++=env[0].sh_greed [i];
    *p++=env[0].sh_type [i];
    *p++=env[0].sh_level [i];
    if (mgrd [env[0].sh_x [i] - 1] [env[0].sh_y [i] - 1] == 31) {
      cprintf("y");
      getch();
    }
  }

  for (i=0; i<20; ++i) save_int(&p, env[0].mons_alloc[i]+10000, 5);

  for (i = 0; i < MNST; i++) {
    *p++=5;
    *p++=5;
    *p++=5;
    *p++=menv [i].m_AC;
    *p++=menv [i].m_ev;
    *p++=menv [i].m_HD;
    *p++=(menv [i].m_speed);
    *p++=menv [i].m_speed_inc;
    *p++=menv [i].m_beh;
    *p++=menv [i].m_x;
    *p++=menv [i].m_y;
    *p++=menv [i].m_targ_1_x;
    *p++=menv [i].m_targ_1_y;
//    *p++=5;
    *p++=0;
    *p++=menv [i].m_ench_1;
    for (j = 0; j < 3; j++) *p++=menv [i].m_ench [j];
    save_int(&p, menv[i].m_class+40080, 5);
    save_int(&p, menv[i].m_hp+40000, 5);
    save_int(&p, menv[i].m_hp_max+40000, 5);
    save_int(&p, menv[i].m_sec+40000, 5);
    for (j = 0; j < 8; j ++) save_int(&p, menv[i].m_inv[j]+40000, 5);
  }

  if (p!=buf+datalen) {
    perror("opa (1)...");
    end(-1);
  }
  FILE *handle=fopen(cha_fil, "wb");
//open(cha_fil, O_RDWR|O_CREAT|O_TRUNC|O_BINARY, 0660);
  if (handle==NULL) {
    perror("Oh dear... ");
    end(-1);
  }
  int retval=write2(handle, buf, datalen);
  free(buf);
  if (datalen!=retval) {
    perror("opa (2)...");
    end(-1);
  }
  fclose(handle);
}


void save_game (char leave_game) {
  char char_f [kFileNameSize];
  int i, j;

  strncpy(char_f, you[0].your_name, kFileNameLen);
  char_f [kFileNameLen] = 0;
  strcat(char_f, ".sav");
#ifdef DOS
  strupr(char_f);
#endif

  int datalen=30+35+10+69+6+5+25+2+30+5+25+12*52+50*5+50*4+50+50+6*50+50+50+30+30+30+100+50+100+NO_UNRANDARTS;
  char *buf=(char*)malloc(datalen);
  char *p=buf;

  for (j=0; j<30; ++j) {
    unsigned char ch=you[0].your_name[j];
    if ((ch==26) || (ch==27)) ch=0;
    *p++=ch;
  }

  *p++=you[0].religion;
  *p++=you[0].piety;
  *p++=you[0].invis;
  *p++=you[0].conf;
  *p++=you[0].paralysis;
  *p++=you[0].slow;
  *p++=you[0].shock_shield;
  *p++=you[0].rotting;
  *p++=0;
  *p++=you[0].deaths_door;
  *p++=your_sign;
  *p++=your_colour;
  *p++=0;
  *p++=you[0].pet_target;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=you[0].spell_levels;
  *p++=you[0].max_level;
  *p++=you[0].where_are_you;
  *p++=you[0].char_direction;
  *p++=you[0].your_level;
  *p++=you[0].is_undead;
  *p++=you[0].special_wield;
  *p++=you[0].berserker;
  *p++=0;
  *p++=you[0].level_type;
  *p++=you[0].corpse_count;
  *p++=you[0].disease;
  *p++=0;
  *p++=0;
  *p++=you[0].species;

  save_int(&p, you[0].hp, 5);
  save_int(&p, you[0].hp, 5);

  if (you[0].haste>215) you[0].haste=215;
  *p++=you[0].haste;
  if (you[0].might>215) you[0].might=215;
  *p++=you[0].might;
  if (you[0].lev>215) you[0].lev=215;
  *p++=you[0].lev;
  if (you[0].poison>215) you[0].poison=215;
  *p++=you[0].poison;
  *p++=0;

  save_int(&p, you[0].hunger, 6);

  *p++=0;
  for (i=0; i<NO_EQUIP; ++i) *p++=you[0].equip[i];
  *p++=you[0].ep;
  *p++=you[0].ep_max;
  *p++=you[0].strength;
  *p++=you[0].intel;
  *p++=you[0].dex;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=you[0].incr_regen;
  *p++=0;
  *p++=0;
  *p++=0;

  save_int(&p, (int)(you[0].incr_regen*100), 5);
  save_int(&p, you[0].xp, 7);
  save_int(&p, you[0].gp, 5);

  *p++=you[0].clas;
  *p++=you[0].xl;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;

  save_int(&p, you[0].exp_available, 6);

  /* max values */
  *p++=you[0].max_strength;
  *p++=you[0].max_intel;
  *p++=you[0].max_dex;
  *p++=you[0].hunger_inc;
  *p++=you[0].ep_incr_regen;

  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;
  *p++=0;

  save_int(&p, you[0].base_hp, 5);
  save_int(&p, you[0].base_hp2, 5);
  save_int(&p, you[0].base_ep, 5);
  save_int(&p, you[0].base_ep2, 5);

  *p++=(unsigned char)you[0].x_pos;
  *p++=(unsigned char)you[0].y_pos;

  for (j=0; j<30; ++j) *p++=you[0].clasnam[j];

  save_int(&p, you[0].burden, 5);

  for (i=0; i<25; ++i) *p++=you[0].spells[i];

  for (i=0; i<52; ++i) {
    *p++=you[0].inv_class[i];
    *p++=you[0].inv_type[i];
    *p++=you[0].inv_plus[i];
    *p++=you[0].inv_dam[i];
    *p++=you[0].inv_col[i];
    *p++=you[0].inv_ident[i];
    save_int(&p, you[0].inv_quant[i], 5);
    *p++=you[0].inv_plus2[i];
  }

  for (i=0; i<5; ++i) {
    for (j=0; j<50; ++j) *p++=you[0].item_description[i][j];
  }

  char identy[4][50];
  save_id(identy);

  for (i=0; i<4; ++i) {
    for (j=0; j<50; ++j) *p++=identy[i][j];
  }

  for (j=0; j<50; ++j) *p++=you[0].skills[j]; /* skills! */

  for (j=0; j<50; ++j) *p++=you[0].practise_skill[j]; /* skills! */

  for (j=0; j<50; ++j) save_int(&p, you[0].skill_points[j], 6);

  for (j=0; j<50; ++j) *p++=you[0].unique_items[j]; /* unique items */

  for (j=0; j<50; ++j) *p++=you[0].unique_creatures[j]; /* unique beasties */

  for (j=0; j<30; ++j) *p++=you[0].duration[j];

  for (j=0; j<30; ++j) *p++=you[0].attribute[j];

  for (j=0; j<30; ++j) *p++=you[0].branch_stairs[j];

  for (j=0; j<100; ++j) *p++=you[0].mutation[j];

  for (j=0; j<50; ++j) *p++=you[0].had_item[j];

  for (j=0; j<100; ++j) *p++=you[0].demon_pow[j];

  for (j = 0; j < NO_UNRANDARTS; ++j) *p++= does_unrandart_exist(j);

  if (p!=buf+datalen) {
    perror("opa (3)...");
    end(-1);
  }

/* Use the first one to amend already created files, and the second to create
   new - the 2nd will be used eventually, as save games will be deleted after
   loading. */

//  int handle=open(char_f, O_CREAT|O_TRUNC|O_BINARY, 0660);
  FILE *handle=//open(char_f, O_RDWR|O_CREAT|O_TRUNC|O_BINARY, 0660);
   fopen(char_f, "wb");
  if (handle==NULL)
  {
    perror("Unable to open file for writing");
    end(-1);
  }
  int retval=write2(handle, buf, datalen);
  free(buf);
  if (datalen!=retval)
  {
    perror("opa (4)...");
    end(-1);
  }
  fclose(handle);

  if (!leave_game) return;

/*if (you[0].level_type != 0) was_a_labyrinth = 1;*/
  if (you[0].level_type == 0) save_level(you[0].your_level, 0, you[0].where_are_you);
  else save_level(you[0].your_level, 1, you[0].where_are_you);
/* save_level(you[0].your_level);*/

/* was_a_labyrinth = 0;*/

#ifdef DOS_TERM
  window(1, 1, 80, 25);
#endif

  clrscr();
  cprintf("See you soon!");

  end(0);

} /* end of save_game() */


void restore_game () {
  char char_f [kFileNameSize];
  int i, j;

  strncpy(char_f, you[0].your_name, kFileNameLen);
  char_f [kFileNameLen] = 0;
  strcat(char_f, ".sav");
#ifdef DOS
  strupr(char_f);
#endif
//  int handle = open(char_f, O_RDONLY, S_IWRITE, S_IREAD);
  FILE *handle=//open(char_f, O_RDWR|O_CREAT|O_TRUNC|O_BINARY, 0660);
   fopen(char_f, "rb");
  if (handle==NULL) {
    perror("Unable to open file for reading");
    end(-1);
  }

  int datalen=30+35+10+69+6+5+25+2+30+5+25+12*52+50*5+50*4+50+50+6*50+50+50+30+30+30+100+50+100+NO_UNRANDARTS;
  char *buf=(char*)malloc(datalen);
  char *p=buf;
  if (datalen!=read2(handle, buf, datalen)) {
    free(buf);
    perror("Unable to read file");
    end(-1);
  }
  fclose(handle);

  for (j=0; j<30; ++j) you[0].your_name [j]=*p++;

  you[0].religion=*p++;
  you[0].piety=*p++;
  you[0].invis=*p++;
  you[0].conf=*p++;
  you[0].paralysis=*p++;
  you[0].slow=*p++;
  you[0].shock_shield=*p++;
  you[0].rotting=*p++;
/*you[0].magic_battery=*p++-40;*/ ++p;
  you[0].deaths_door=*p++;
  your_sign=*p++;
  your_colour=*p++;
/*you[0].spec_poison=*p++-40;*/ ++p;
  you[0].pet_target=*p++;
/*you[0].prot_life=*p++-40;*/ ++p;
/*  you[0].res_magic=load_int(&p, 5);*/ p+=5;
  you[0].spell_levels=*p++;
  you[0].max_level=*p++;
  you[0].where_are_you=*p++;
  you[0].char_direction=*p++;
  you[0].your_level=*p++;
  you[0].is_undead=*p++;
  you[0].special_wield=*p++;
  you[0].berserker=*p++;
/*you[0].energy=*p++;*/ ++p;
  you[0].level_type=*p++;
  you[0].corpse_count=*p++;
  you[0].disease=*p++;
/*you[0].spec_death=*p++;*/ ++p;
/*you[0].spec_holy=*p++;*/ ++p;
  you[0].species=*p++;
  you[0].hp=load_int(&p, 5);
  you[0].hp_max=load_int(&p, 5);
  you[0].haste=*p++;
  you[0].might=*p++;
  you[0].lev=*p++;
  you[0].poison=*p++;
  ++p;
  you[0].hunger=load_int(&p, 6);

/*you[0].item_wielded=*p++;*/ ++p;

  for (i=0; i<NO_EQUIP; ++i) you[0].equip[i]=*p++;

  you[0].ep=*p++;
  you[0].ep_max=*p++;
  you[0].strength=*p++;
  you[0].intel=*p++;
  you[0].dex=*p++;

/*you[0].AC=*p++-80;*/ ++p;
/*you[0].evasion=*p++;*/ ++p;
/*you[0].damage=*p++;*/ ++p;
  p+=2;
  you[0].incr_regen=*p++;
  p+=3;
  p+=5;

  you[0].xp=load_int(&p, 7);
  you[0].gp=load_int(&p, 5);

  you[0].clas=*p++;
  you[0].xl=*p++;
  p+=14;

  you[0].exp_available=load_int(&p, 6);

  you[0].max_strength=*p++;
  you[0].max_intel=*p++;
  you[0].max_dex=*p++;
  you[0].hunger_inc=*p++;
  you[0].ep_incr_regen=*p++;

  p+=5;

  you[0].base_hp=load_int(&p, 5);
  you[0].base_hp2=load_int(&p, 5);
  you[0].base_ep=load_int(&p, 5);
  you[0].base_ep2=load_int(&p, 5);

  you[0].x_pos=(int)*p++;
  you[0].y_pos=(int)*p++;

  for (i=0; i<30; ++i) you[0].clasnam[i]=*p++;

  you[0].burden=load_int(&p, 5);

  you[0].spell_no = 0;
  for (i=0; i<25; ++i) {
    you[0].spells[i]=*p++;
    if (you[0].spells[i]!=210) ++you[0].spell_no;
  }

  you[0].inv_no = 0;
  for (i=0; i<52; ++i) {
    you[0].inv_class[i]=*p++;
    you[0].inv_type[i]=*p++;
    you[0].inv_plus[i]=*p++;
    you[0].inv_dam[i]=*p++;
    you[0].inv_col[i]=*p++;
    you[0].inv_ident[i]=*p++;

    you[0].inv_quant[i]=load_int(&p, 5);

    you[0].inv_plus2[i]=*p++;
    if (you[0].inv_quant[i]>0) ++you[0].inv_no;
  }

  for (i=0; i<5; ++i) {
    for (j=0; j<50; ++j) you[0].item_description[i][j]=*p++;
  }

  for (i=0; i<4; ++i) {
    for (j=0; j<50; ++j) {
      unsigned char ch=*p++;
      switch (i) {
        case 0: set_id(OBJ_WANDS, j, ch); break;
        case 1: set_id(OBJ_SCROLLS, j, ch); break;
        case 2: set_id(OBJ_JEWELLERY, j, ch); break;
        case 3: set_id(OBJ_POTIONS, j, ch); break;
      }
    }
  }

  for (j=0; j<50; ++j) you[0].skills[j]=*p++; /* skills! */

  for (j=0; j<50; ++j) you[0].practise_skill[j]=*p++; /* skills! */

  for (j=0; j<50; ++j) you[0].skill_points[j]=load_int(&p, 6);

  for (j=0; j<50; ++j) you[0].unique_items[j]=*p++; /* unique items */

  for (j=0; j<50; ++j) you[0].unique_creatures[j]=*p++; /* unique creatures */

  for (j=0; j<30; ++j) you[0].duration[j]=*p++;

  for (j=0; j<30; ++j) you[0].attribute[j]=*p++;

  for (j=0; j<30; ++j) you[0].branch_stairs[j]=*p++;

  for (j=0; j<100; ++j) you[0].mutation[j]=*p++;

  for (j=0; j<50; ++j) you[0].had_item[j]=*p++;

  for (j=0; j<100; ++j) you[0].demon_pow[j]=*p++;

  for (j = 0; j < NO_UNRANDARTS; ++j) set_unrandart_exist(j, *p++);

  if (p!=buf+datalen) {
    free(buf);
    perror("opa (5)...");
    end(-1);
  }
  free(buf);

} /* end of restore_game() */


void save_ghost () {
  if ((you[0].your_level<=1) || (you[0].is_undead!=0)) return;
  char corr_level[10];
  char hbjh [5];
  char cha_fil [kFileNameSize];
  corr_level[0]=0;
  if (you[0].your_level<10) strcpy(corr_level, "0");
  itoa(you[0].your_level, hbjh, 10);
  strcat(corr_level, hbjh);
  corr_level[2]=you[0].where_are_you+'a';
  corr_level[3]=0; /* null-terminate it */
  strcpy(cha_fil, "bones.");
  if (you[0].level_type!=0) strcat(cha_fil, "lab"); /* temporary level */
  else strcat(cha_fil, corr_level);

//  int gfile=open(cha_fil, S_IWRITE, S_IREAD);
  FILE *gfile=fopen(cha_fil, "rb");
  if (gfile!=NULL) {
    fclose(gfile);
    return;
  }

  unsigned char buf1[40];
  for (int i=0; i<20; ++i) buf1[i]=you[0].your_name[i];
  buf1[20]=(you[0].hp_max>=120)?(150):(you[0].hp_max);
  buf1[21]=player_evasion();
  buf1[22]=player_AC();
  buf1[23]=player_see_invis();
  buf1[24]=player_res_fire();
  /* note - as ghosts, automatically get res poison + prot_life */
  buf1[25]=player_res_cold();
  buf1[26]=you[0].attribute[ATTR_RESIST_LIGHTNING];

  int d=4;
  int e=0;
  if (you[0].species==SP_TROLL) d+=you[0].xl;
  d += you[0].skills [SK_UNARMED_COMBAT]; /* Unarmed combat */
  if (you[0].equip[EQ_WEAPON]!= -1)
  {
    if (you[0].inv_class[you[0].equip[EQ_WEAPON]]==0)
    {
      d=property(you[0].inv_class[you[0].equip[EQ_WEAPON]], you[0].inv_type[you[0].equip[EQ_WEAPON]], 0);
      if (you[0].inv_dam[you[0].equip[EQ_WEAPON]]<180) e=you[0].inv_dam[you[0].equip[EQ_WEAPON]]%30;
      if (you[0].inv_dam[you[0].equip[EQ_WEAPON]]%30>=25) e=randart_wpn_properties(you[0].inv_class[you[0].equip[EQ_WEAPON]], you[0].inv_type[you[0].equip[EQ_WEAPON]], you[0].inv_dam[you[0].equip[EQ_WEAPON]], you[0].inv_plus[you[0].equip[EQ_WEAPON]], you[0].inv_plus2[you[0].equip[EQ_WEAPON]], 0, RAP_BRAND);
    }
    if (you[0].inv_class[you[0].equip[EQ_WEAPON]]==11) d=5;
  }

  if (((you[0].equip[EQ_WEAPON]!=-1) && (you[0].inv_class[you[0].equip[EQ_WEAPON]]==0)) ||
      (you[0].inv_class[you[0].equip[EQ_WEAPON]]==11)) {
    d*=26+you[0].skills[weapon_skill(you[0].inv_class[you[0].equip[EQ_WEAPON]], you[0].inv_type[you[0].equip[EQ_WEAPON]])];
    d/=25;
  }

  d*=31+you[0].skills[SK_FIGHTING];
  d/=30;

  d+=you[0].strength/4;
  if (d>50) d=50;

  buf1[27]=d;
  buf1[28]=e;
  buf1[29]=you[0].species;
  buf1[30]=best_skill(0, 50, 99);
  buf1[31]=you[0].skills[best_skill(0, 50, 99)];
  buf1[32]=you[0].xl;
  buf1[33]=you[0].clas;
  add_spells(buf1);

  gfile=fopen(cha_fil, "wb"); //open(cha_fil, O_RDWR|O_CREAT|O_TRUNC|O_BINARY, 0660);
  if (gfile==NULL) {
   strcpy(info, "Error creating ghost file: ");
   strcat(info, cha_fil);
   mpr(info);
   more();
   return;
  }
  write2(gfile, (char*)buf1, 40);
  fclose(gfile);
}


//int write2(int file, const void *buffer, unsigned count)
int write2(FILE *file, char *buffer, unsigned int count)
{

/*unsigned int i = 0;

for (i = 0; i < count; i ++)
{
 if (buffer [i] == EOF | buffer [i] == 26) buffer [i] = 0;
}*/

return fwrite(buffer, 1, count, file);

//return write(file, buffer, count);

}

int read2(FILE *file, char *buffer, unsigned int count)
{

return fread(buffer, 1, count, file);

}

/*
Used when creating ghosts: goes through and finds spells for the ghost to
cast. Death is a traumatic experience, so ghosts only remember a few spells.
*/
void add_spells(unsigned char buffer [40])
{

int i = 0;

buffer [34] = 250;
buffer [35] = 250;
buffer [36] = 250;
buffer [37] = 250;
buffer [38] = 250;
buffer [39] = 250;

buffer [34] = search_first_list(250);
buffer [35] = search_first_list(buffer [34]);
buffer [36] = search_second_list(250);
buffer [37] = search_third_list(250);
if (buffer [37] == 250) buffer [37] = search_first_list(250);
buffer [38] = search_first_list(buffer [37]);
if (buffer [38] == 250) buffer [38] = search_first_list(buffer [37]);
if (find_spell(14) == 1) buffer [38] = 14;

/* Looks for blink/tport for emergency slot */
if (find_spell(28) == 1 || find_spell(59) == 1) buffer [39] = 28;
if (find_spell(1) == 1) buffer [39] = 1;

for (i = 34; i < 40; i ++)
{
 buffer [i] = translate_spell(buffer [i]);
}

}

unsigned char search_first_list(unsigned char ignore_spell)
{

int i;

for (i = 0; i < 20; i ++)
{
 if (search_order_conj [i] == 250) return 250;
 if (search_order_conj [i] == ignore_spell) continue;
 if (find_spell(search_order_conj [i]) == 1) return search_order_conj [i];
}
return 250;
}

unsigned char search_second_list(unsigned char ignore_spell)
{

int i;

for (i = 0; i < 20; i ++)
{
 if (search_order_third [i] == 250) return 250;
 if (search_order_third [i] == ignore_spell) continue;
 if (find_spell(search_order_third [i]) == 1) return search_order_third [i];
}
return 250;
}

unsigned char search_third_list(unsigned char ignore_spell)
{

int i;

for (i = 0; i < 20; i ++)
{
 if (search_order_misc [i] == 250) return 250;
 if (search_order_misc [i] == ignore_spell) continue;
 if (find_spell(search_order_misc [i]) == 1) return search_order_misc [i];
}
return 250;
}

/*
Searches for a specific spell, according to search order in the global arrays
defined start of file (top to bottom).
*/
char find_spell(unsigned char which_sp)
{

int i = 0;

for (i = 0; i < 21; i ++)
{
 if (you[0].spells [i] == which_sp) return 1;
}

return 0;

}


/*
When passed the number for a player spell, returns the equivalent monster
spell. Returns 250 on failure (no equiv).
*/
unsigned char translate_spell(unsigned char spel)
{
 switch(spel)
 {
  case 1: return 14;
  case 5: return 0;
  case 6: return 12;
  case 14: return 18;
  case 15: return 8;
  case 16: return 9;
  case 17: return 10;
  case 20: return 43;
  case 21: return 4;
  case 22: return 5;
  case 23: return 3;
  case 24: return 6;
  case 25: return 11;
  case 26: return 1;
  case 27: return 2;
  case 28: return 16; /* approximate */
/*  case 29: return ; no freezing/mephitic cloud yet
  case 30: return ; */
  case 35: return 7;
  case 37: return 15;
  case 49: return 21; /* approximate */
  case 53: return 19;
  case 54: return 17;
  case 59: return 16;
  case 60: return 22;
  case 62: return 24; /* approximate */
  case 66: return 28;
  case 67: return 29;
  case 72: return 42; /* approximate */
  case 79: return 31;
  case 82: return 33;
  case 113: return 52;
  case 115: return 38;
  case 119: return 27;
  case 120: return 33;
  case 121: return 51;
  case 128: return 39;
  case 129: return 40;
  case 148: return 48; /* Too powerful to give ghosts Torment for Agony? Nah. */
  case 151: return 45;
  case 158: return 48;
  default: return 100;
 }

return 100;

}


void generate_random_demon(void)
{

int rdem = 0;
int i = 0;

for (rdem = 0; rdem < MNST + 1; rdem ++)
{
 if (rdem == MNST) return; /* obviously no random demon */
 if (menv [rdem].m_class == 401) break; /* found one! */
}

make_name(random2(250), random2(250), random2(250), 3, st_prn);
strcpy(ghost.gname, st_prn);

ghost.ghs [0] = 50 + random2(50) + random2(50) + random2(50) + random2(50);
if (random2(3) == 0) ghost.ghs [0] += random2(50) + random2(50);
/* hp - could be defined below (as could ev, AC etc). Oh well, too late */

ghost.ghs [1] = 5 + random2(10); /* evasion */
ghost.ghs [2] = random2(0); /* AC */
if (random2(3) == 0) ghost.ghs [3] = 1; else ghost.ghs [3] = 0; /* see inv */
if (random2(3) != 0)
{
 ghost.ghs [4] = 0; /* res_fire */
 if (random2(4) == 0) ghost.ghs [4] = 99;
 if (random2(4) == 0) ghost.ghs [4] = 102;
} else ghost.ghs [4] = 101;
if (random2(3) != 0)
{
 ghost.ghs [5] = 0; /* res_cold */
 if (random2(4) == 0) ghost.ghs [5] = 99;
} else ghost.ghs [5] = 101;
/* demons, as ghosts, automatically get res poison + prot_life */
if (random2(3) != 0)
{
 ghost.ghs [6] = 0; /* res_elec */
} else ghost.ghs [6] = 1;

ghost.ghs [7] = 10 + random2(20) + random2(20) + random2(20); /* damage in combat */
ghost.ghs [8] = 0; /* special attack type (uses weapon brand code) */
if (random2(2) == 0)
{
 ghost.ghs [8] = random2(17);
 if (ghost.ghs [8] == 3 || ghost.ghs [8] == 5 || ghost.ghs [8] == 7 || ghost.ghs [8] == 11 || ghost.ghs [8] == 12 || ghost.ghs [8] == 14)
  ghost.ghs [8] = 0; /* some brands inappropriate (eg holy wrath) */
}
ghost.ghs [9] = 0; /* ghost species - used for: is demon a spellcaster? */
if (random2(3) != 0) ghost.ghs [9] = 1;
ghost.ghs [10] = random2(3); /* ghost best skill - used for: does demon fly? */
if (random2(3) == 0) ghost.ghs [10] = 0;
ghost.ghs [11] = 0; /* vacant - ghost best skill level */
ghost.ghs [12] = 10 + random2(10); /* Hit Dice */
ghost.ghs [13] = 0; /* ghost class - used for: does demon cycle colours? */
if (random2(10) == 0) ghost.ghs [13] = 1;

menv [rdem].m_HD = ghost.ghs [12];
menv [rdem].m_hp = ghost.ghs [0];
menv [rdem].m_hp_max = ghost.ghs [0];
menv [rdem].m_AC = ghost.ghs [2];
menv [rdem].m_ev = ghost.ghs [1];
menv [rdem].m_speed = 10;
if (random2(3) == 0) menv [rdem].m_speed = 6 + random2(13);
menv [rdem].m_speed_inc = 70;
menv [rdem].m_sec = 1 + random2(15); /* demon's colour */

ghost.ghs [14] = 250;
ghost.ghs [15] = 250;
ghost.ghs [16] = 250;
ghost.ghs [17] = 250;
ghost.ghs [18] = 250;
ghost.ghs [19] = 250;

/* This bit uses the list of player spells to find appropriate spells
for the demon, then converts those spells to the monster spell indices.
Some special monster-only spells are at the end. */
if (ghost.ghs [9] == 1)
{
 if (random2(2) == 0)
  do
  {
   if (random2(3) == 0) break;
   ghost.ghs [14] = search_order_conj [i];
   i ++;
   if (search_order_conj [i] == 250) break;
  } while(1);

 if (random2(2) == 0)
  do
  {
   if (random2(3) == 0) break;
   ghost.ghs [15] = search_order_conj [i];
   i ++;
   if (search_order_conj [i] == 250) break;
  } while(1);

 if (random2(4) != 0)
  do
  {
   if (random2(3) == 0) break;
   ghost.ghs [16] = search_order_third [i];
   i ++;
   if (search_order_third [i] == 250) break;
  } while(1);

 if (random2(2) == 0)
  do
  {
   if (random2(3) == 0) break;
   ghost.ghs [17] = search_order_misc [i];
   i ++;
   if (search_order_misc [i] == 250) break;
  } while(1);

 if (random2(2) == 0)
  do
  {
   if (random2(3) == 0) break;
   ghost.ghs [18] = search_order_misc [i];
   i ++;
   if (search_order_misc [i] == 250) break;
  } while(1);

 if (random2(2) == 0) ghost.ghs [19] = 59; /* blink */
 if (random2(2) == 0) ghost.ghs [19] = 1; /* teleport */

 for (i = 14; i < 20; i ++)
 {
  ghost.ghs [i] = translate_spell(ghost.ghs [i]);
 } /* Converts the player spell indices to monster spell ones */

/* give demon a chance for some monster-only spells: */
if (random2(25) == 0) ghost.ghs [14] = 50; /* metal splinters */
if (random2(25) == 0) ghost.ghs [14] = 37; /* eye of devas */
if (random2(25) == 0) ghost.ghs [15] = 26; /* steam */
if (random2(25) == 0) ghost.ghs [15] = 35; /* blast */
if (random2(25) == 0) ghost.ghs [15] = 49; /* fiend's hellfire */
if (random2(25) == 0) ghost.ghs [16] = 30; /* smiting */
if (random2(25) == 0) ghost.ghs [16] = 20; /* burst of hellfire */
if (random2(25) == 0) ghost.ghs [16] = 20; /* burst of hellfire */
if (random2(15) == 0) ghost.ghs [18] = 18; /* dig */

/* and demon-summoning should be fairly common: */
if (random2(12) == 0) ghost.ghs [16] = 51; /* summon class 1 demons */
if (random2(12) == 0) ghost.ghs [16] = 27; /* summon demons */
if (random2(20) == 0) ghost.ghs [17] = 51; /* summon class 1 demons */
if (random2(20) == 0) ghost.ghs [17] = 27; /* summon demons */


if (ghost.ghs [17] == 250) ghost.ghs [17] = 27; /* at least they can summon demons */

}


}

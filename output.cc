#include "config.h"

#ifdef DOS
#include <conio.h>
#endif

#include <stdlib.h>

#include "externs.h"
#include "enum.h"
#include "itemname.h"
#include "player.h"
#include "ouch.h"

char wield_change;

void print_stats(void)
{

        textcolor(7);

char temp_quant [15];

        if (you[0].hp_ch == 1)
        {
         if (you[0].hp <= you[0].hp_max / 4) textcolor(YELLOW);
         if (you[0].hp <= you[0].hp_max / 10) textcolor(RED);
         itoa(you[0].hp, st_prn, 10);
         gotoxy (44,3);
         cprintf(st_prn);
         textcolor(LIGHTGREY);
         itoa(you[0].hp_max, st_prn, 10);
         cprintf("("); cprintf(st_prn); cprintf(")    ");
         you[0].hp_ch = 0;
        }

        if (you[0].ep_ch == 1)
        {
         itoa(you[0].ep, st_prn, 10);
         gotoxy (47,4);
         cprintf(st_prn);
         itoa(you[0].ep_max, st_prn, 10);
         cprintf("("); cprintf(st_prn); cprintf(")   ");
         you[0].ep_ch = 0;
        }

        if (you[0].strength_ch == 1)
        {
         if (you[0].strength <= 0) you[0].strength = 0;
         if (you[0].strength >= 72) you[0].strength = 72;
         if (you[0].max_strength >= 72) you[0].max_strength = 72;
         itoa(you[0].strength, st_prn, 10);
         gotoxy (45,7);
         if (you[0].strength < you[0].max_strength) textcolor(YELLOW);
         cprintf(st_prn);
         if (you[0].strength != you[0].max_strength)
         {
          textcolor(LIGHTGREY);
          itoa(you[0].max_strength, st_prn, 10);
          cprintf("("); cprintf(st_prn); cprintf(")   ");
         } else cprintf("      ");
         you[0].strength_ch = 0;
         if (you[0].strength <= 0) ouch(-9999, 0, 8);
         burden_change();
        }

        if (you[0].intel_ch == 1)
        {
         if (you[0].intel <= 0) you[0].intel = 0;
         if (you[0].intel >= 72) you[0].intel = 72;
         if (you[0].max_intel >= 72) you[0].max_intel = 72;
         itoa(you[0].intel, st_prn, 10);
         gotoxy (45,8);
         if (you[0].intel < you[0].max_intel) textcolor(YELLOW);
         cprintf(st_prn);
         if (you[0].intel != you[0].max_intel)
         {
          textcolor(LIGHTGREY);
          itoa(you[0].max_intel, st_prn, 10);
          cprintf("("); cprintf(st_prn); cprintf(")   ");
         } else cprintf("      ");
         you[0].intel_ch = 0;
         if (you[0].intel <= 0) ouch(-9999, 0, 7);
        }

        if (you[0].dex_ch == 1)
        {
         if (you[0].dex <= 0) you[0].dex = 0;
         if (you[0].dex >= 72) you[0].dex = 72;
         if (you[0].max_dex >= 72) you[0].max_dex = 72;
         itoa(you[0].dex, st_prn, 10);
         gotoxy (45,9);
         if (you[0].dex < you[0].max_dex) textcolor(YELLOW);
         cprintf(st_prn);
         if (you[0].dex != you[0].max_dex)
         {
          textcolor(LIGHTGREY);
          itoa(you[0].max_dex, st_prn, 10);
          cprintf("("); cprintf(st_prn); cprintf(")   ");
         } else cprintf("      ");
         you[0].dex_ch = 0;
         if (you[0].dex <= 0) ouch(-9999, 0, 9);
         textcolor(LIGHTGREY);
        }

        if (you[0].AC_ch == 1)
        {
         itoa(player_AC(), st_prn, 10);
         gotoxy (44,5);
         cprintf(st_prn);
         if (strlen(st_prn) <= 1) cprintf(" ");
         if (strlen(st_prn) <= 2) cprintf(" ");
         if (strlen(st_prn) <= 3) cprintf(" ");
         cprintf("(");
         itoa(player_shield_class(), st_prn, 10);
         cprintf(st_prn);
         cprintf(")   ");
         you[0].AC_ch = 0;
        }

        if (you[0].evasion_ch == 1)
        {
         itoa(player_evasion(), st_prn, 10);
         gotoxy (44,6);
         cprintf(st_prn);
         cprintf("  ");
         you[0].evasion_ch = 0;
        }

        if (you[0].gp_ch == 1)
        {
         itoa(you[0].gp, st_prn, 10);
         gotoxy (46,10);
         cprintf(st_prn);
         cprintf("    ");
         you[0].gp_ch = 0;
        }

        if (you[0].xp_ch == 1)
        {
        itoa(you[0].xl, temp_quant, 10);
        itoa(you[0].xp, st_prn, 10);
        gotoxy (52,11);
        cprintf(temp_quant); cprintf("/"); cprintf(st_prn);
        cprintf("      ");
        you[0].xp_ch = 0;
        }

        if (you[0].hung_ch == 1)
        {
         gotoxy(40,14);
         switch(you[0].hung_state)
         {
          case 5: textcolor(BLUE); cprintf("Engorged"); textcolor(LIGHTGREY); break;
          case 4: textcolor(GREEN); cprintf("Full    "); textcolor(LIGHTGREY); break;
          case 3: cprintf("        "); break;
          case 2: textcolor(YELLOW); cprintf("Hungry  "); textcolor(LIGHTGREY); break;
          case 1: textcolor(RED); cprintf("Starving"); textcolor(LIGHTGREY); break;
         }
         you[0].hung_ch = 0;
         }

        if (you[0].burden_ch == 1)
        {
         gotoxy(40,15);
         switch(you[0].burden_state)
         {
          case 5: textcolor(YELLOW); cprintf("Overloaded"); textcolor(LIGHTGREY); break;
          case 2: textcolor(LIGHTRED); cprintf("Encumbered"); textcolor(LIGHTGREY); break;
          case 0: cprintf("          "); break;
         }
         you[0].burden_ch = 0;
        }
        if (wield_change == 1)
        {
         gotoxy(40,13);
         cprintf("                                       ");
         if (you[0].equip [EQ_WEAPON] != -1)
          {
            gotoxy(40,13);
            textcolor(you[0].inv_col [you[0].equip [EQ_WEAPON]]);
            if (you[0].equip [EQ_WEAPON] <= 25) putch(you[0].equip [EQ_WEAPON] + 97);
                else putch(you[0].equip [EQ_WEAPON] + 39);
                cprintf(" - ");
                in_name(you[0].equip [EQ_WEAPON], 3, str_pass);
                str_pass [35] = 0;
                cprintf(str_pass);
            textcolor(LIGHTGREY);
      } else
      {
       gotoxy(40,13);
       if (you[0].attribute [ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
       {
            textcolor(RED);
                cprintf("Blade Hands");
            textcolor(LIGHTGREY);
       } else
       {
            textcolor(LIGHTGREY);
                cprintf("Empty hands");
       }
      }
         wield_change = 0;
        }

}

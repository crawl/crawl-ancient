#include "config.h"

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "invent.h"
#include "itemname.h"
#include "macro.h"
#include "player.h"

#include <stdlib.h>
#include <string.h>

/*#include "crawlfnc.h"
#include "fn2.h"
#include "invent.h"
#include "struct.h"*/

//#define ITEMS 500
/*
struct item_struct
{
         unsigned char iclass [ITEMS];
         unsigned char itype [ITEMS];
         unsigned char iplus [ITEMS]; // +, charges, remaining food value
         unsigned char iplus2 [ITEMS];
         unsigned char idam [ITEMS]; // damage
         unsigned int iquant [ITEMS]; // multiple items
         unsigned char ix [ITEMS]; //  x-location
         unsigned char iy [ITEMS]; //  y-location
         unsigned char icol [ITEMS];
         unsigned char iid [ITEMS];

         unsigned int ilink [ITEMS];

};

//struct item_struct it [1];

struct shop_struct
{

unsigned char keeper_name [5] [3];
unsigned char sh_x [5];
unsigned char sh_y [5];
unsigned char sh_greed [5];
unsigned char sh_type [5];
unsigned char sh_level [5];

};

*/

void purchase(int item_got);

//void purchase(struct player you [1]);

unsigned int item_value(unsigned char item_clas, unsigned char item_typ, unsigned char item_da, unsigned char it_plus, unsigned char it_plus2, unsigned int item_quant, char ident_lev, char id [4] [50]);


char in_a_shop(char shoppy, char id [4] [50]);

void shop_print(char *shoppy, char sh_line);


char more3(void);


void clear_line(void);


char shop_getch(void);

void shop_init_id(int i, int shop_id [4] [50]);
void shop_uninit_id(int i, int shop_id [4] [50]);

void shop_set_id(int i, int shop_id [4] [50], unsigned char iclass, unsigned char itype);

char book_rarity(char which_book);

char in_a_shop(char shoppy, char id [4] [50])
{
   unsigned int greedy = env[0].sh_greed [shoppy];
   int shop_id [4] [50];
   int shop_items [20];

   char st_pass [60];

   char gold_p [10];

   int gp_value = 0;
   char i;
   char ft;

#ifdef DOS_TERM
   char buffer[4800];
   gettext(1, 1, 80, 25, buffer);
#endif
   strcpy(st_pass, "");

#ifdef DOS_TERM
   window(1, 1, 80, 25);
#endif

   clrscr();
//   char st_pass [50];
int itty = 0;
char sh_name [40];


//char *make_name(unsigned char var1, unsigned char var2, unsigned char var3, char ncase)

      strcpy(sh_name, "Welcome to ");
      char str_pass [50];
      make_name(env[0].keeper_name [shoppy] [0], env[0].keeper_name [shoppy] [1], env[0].keeper_name [shoppy] [2], 3, str_pass);
      strcat(sh_name, str_pass);
      strcat(sh_name, "'s ");
      switch(env[0].sh_type [shoppy])
      {
       case 0: strcat(sh_name, "Weapon Shop!"); break;
       case 1: strcat(sh_name, "Armour Shop!"); break;
       case 2: strcat(sh_name, "Antique Weapon Shop!"); break;
       case 3: strcat(sh_name, "Antique Armour Shop!"); break;
       case 4: strcat(sh_name, "Assorted Antiques!"); break;
       case 5: strcat(sh_name, "Jewellery Shop!"); break;
       case 6: strcat(sh_name, "Magical Wand Shop!"); break;
       case 7: strcat(sh_name, "Book Shop!"); break;
       case 8: strcat(sh_name, "Food Shop!"); break;
       case 9: strcat(sh_name, "Distillery!"); break;
       case 10: strcat(sh_name, "Magic scroll Shop!"); break;
       case 11: strcat(sh_name, "General Store!"); break;
      }

//      strcat(sh_name, " shop.");

      shop_print(sh_name, 20); //"Hello, and welcome to ");

      more3();

//      for (i = 0; i < 4; i ++) // when invent becomes available, must replace!
//      {
//      }
      shop_init_id(shoppy, shop_id);
  /* THINGS TO DO:
   Allow inventory
   Remove id change for antique shops
   selling?
  */
       save_id(id);

print_stock :
clrscr();
itty = igrd [0] [5 + shoppy];


    if (igrd [0] [5 + shoppy] == 501)
     {
      empty_shop : shop_print("I'm sorry, my shop is empty now.", 20);
      more3();
      goto goodbye;
     }

     if (you[0].inv_no >= 52)
     {
         shop_print("You seem to be carrying too much.", 20);
         more3();
         goto goodbye;
     }


      if (itty == 501) goto empty_shop;

      for (i = 1; i < 20; i ++) // remember i is a char
      {

       shop_items [i - 1] = itty;

       if (itty == 501) //mitm.ilink [itty] == 501)
       {
         shop_items [i - 1] = 501;
         continue;
       }

       itty = mitm.ilink [itty];

      }

      itty = igrd [0] [5 + shoppy];

      for (i = 1; i < 18; i ++) // remember i is a char
      {

       gotoxy(1, i);

       if (i % 2 == 0) textcolor(LIGHTGREY); else textcolor(WHITE);

        item_name(mitm.iplus2 [itty], mitm.iclass [itty], mitm.itype [itty], mitm.idam [itty], mitm.iplus [itty], mitm.iquant [itty], mitm.iid [itty], 3, st_pass);
       putch(i + 96);
       cprintf(" - ");
       cprintf(st_pass);

       gp_value = greedy * item_value(mitm.iclass [itty], mitm.itype [itty], mitm.idam [itty], mitm.iplus [itty], mitm.iplus2 [itty], mitm.iquant [itty], mitm.iid [itty], id);
       gp_value /= 10;
       if (gp_value <= 1) gp_value = 1;
       gotoxy(60, i);
       itoa(gp_value, st_pass, 10);
       cprintf(st_pass);
       cprintf("gp");
       if (mitm.ilink [itty] == 501) break;

       itty = mitm.ilink [itty];

      }

      textcolor(LIGHTGREY);

      shop_print("Type letter of item to buy it, x or Esc to leave, or ? or * for an inventory.", 23);

      purchase :
      strcpy(sh_name, "You have ");
      itoa(you[0].gp, gold_p, 10);
      strcat(sh_name, gold_p);
      strcat(sh_name, " gold piece");
      if (you[0].gp == 1) strcat(sh_name, "."); else strcat(sh_name, "s.");
      shop_print(sh_name, 19);

      //shop_print("What would you like to purchase?", 20);

      shop_print("What would you like to purchase?", 20);

      ft = shop_getch();

      if (ft == 'x' || ft == 27) goto goodbye;

      if (ft == '?' || ft == '*')
      {
       shop_uninit_id(shoppy, shop_id);
       invent(-1, 0);
       shop_init_id(shoppy, shop_id);
#ifdef DOS_TERM
       window(1, 1, 80, 25);
#endif
       goto print_stock;
      }

      if (ft < 'a' || ft > 'z')
        {
         huh : shop_print("Huh?", 20);
         more3();
         goto purchase;
        }

      ft -= 97;

      if (ft > 18) goto huh;

      if (shop_items [ft] == 501)
        {
         shop_print("I'm sorry, you seem to be confused.", 20);
         more3();
         goto purchase;
        }

      if (greedy * (item_value(mitm.iclass [shop_items [ft]], mitm.itype [shop_items [ft]], mitm.idam [shop_items [ft]], mitm.iplus [shop_items [ft]], mitm.iplus2 [shop_items [ft]], mitm.iquant [shop_items [ft]], mitm.iid [shop_items [ft]], id)) / 10 > you[0].gp)
      {
         shop_print("I'm sorry, you don't seem to have enough money.", 20);
         more3();
         goto purchase;
      }

      purchase(shop_items [ft]);
      shop_set_id(i, shop_id, mitm.iclass [shop_items [ft]], mitm.itype [shop_items [ft]]);

      you[0].gp -= (greedy * item_value(mitm.iclass [shop_items [ft]], mitm.itype [shop_items [ft]], mitm.idam [shop_items [ft]], mitm.iplus [shop_items [ft]], mitm.iplus2 [shop_items [ft]], mitm.iquant [shop_items [ft]], mitm.iid [shop_items [ft]], id)) / 10;

      mitm.iquant [shop_items [ft]] = 0;
      if (ft == 0)
      {
       igrd [0] [5 + shoppy] = mitm.ilink [shop_items [ft]];
      } else mitm.ilink [shop_items [ft - 1]] = mitm.ilink [shop_items [ft]];

      goto print_stock;

      goodbye :
      clear_line();
      shop_print("Goodbye!", 20);
      more3();
#ifdef DOS_TERM
      puttext(1, 1, 80, 25, buffer);
      gotoxy(1,1);
      cprintf(" ");
#endif
//      for (i = 0; i < 4; i ++)
//      {
//      }
//
      shop_uninit_id(shoppy, shop_id);
      return 0;
}


void shop_init_id(int i, int shop_id [4] [50])
{
int j = 0;
    if (env[0].sh_type [i] != 2 && env[0].sh_type [i] != 3 && env[0].sh_type [i] != 4)
       for (j = 0; j < 50; j ++)
       {
        shop_id [0] [j] = get_id(3, j);
        set_id(3, j, 1);
        shop_id [1] [j] = get_id(6, j);
        set_id(6, j, 1);
        shop_id [2] [j] = get_id(7, j);
        set_id(7, j, 1);
        shop_id [3] [j] = get_id(8, j);
        set_id(8, j, 1);
       }


}

void shop_uninit_id(int i, int shop_id [4] [50])
{
int j = 0;

    if (env[0].sh_type [i] != 2 && env[0].sh_type [i] != 3 && env[0].sh_type [i] != 4)
       for (j = 0; j < 50; j ++)
       {
//        shop_id [i] [j] = get_id(i, j);
        set_id(3, j, shop_id [0] [j]);
        set_id(6, j, shop_id [1] [j]);
        set_id(7, j, shop_id [2] [j]);
        set_id(8, j, shop_id [3] [j]);
       }
}

void shop_set_id(int i, int shop_id [4] [50], unsigned char iclass, unsigned char itype)
{

  if (env[0].sh_type [i] != 2 && env[0].sh_type [i] != 3 && env[0].sh_type [i] != 4)
  {
      switch(iclass)
        {
          case 3: shop_id [0] [itype] = 1; break;
          case 6: shop_id [1] [itype] = 1; break;
          case 7: shop_id [2] [itype] = 1; break;
          case 8: shop_id [3] [itype] = 1; break;
        }
      set_id(iclass, itype, 1);
  }

}


void shop_print(char *shoppy, char sh_lines)
{
 int i;
 gotoxy(1, sh_lines);
 cprintf(shoppy);
 for (i = strlen(shoppy); i < 80; i ++)
 {
  cprintf(" ");
 }

}

char more3(void)
{
 char keyin = 0;
 gotoxy(70,20);
 cprintf("-more-");
 keyin = getch();
 if (keyin == 0) getch();
 clear_line();
 return keyin;
}

void clear_line(void)
{
// int i;
//window(1, 20, 80, 21);
//clrscr();
//window(1, 1, 80, 25);
// gotoxy(1,20);
// for (i = 0; i < 80; i ++)
//    {
//     cprintf("XXXXXXXXXXXXXXXXXX                                                       ");
//    }
return;
}

char shop_getch(void)
{
 char keyin = 0;
 keyin = getch();
 if (keyin == 0) getch();
 return keyin;
}

void purchase(int item_got)
{

 int i = 0;
 for (i = 0; i < 53; i ++)
 {
  if (i == 52)
    {
     shop_print("The demon of the infinite void grins at you.", 20);
     more3();
     return;
    }
  if (you[0].inv_quant [i] <= 0) break;
 }

                you[0].inv_ident [i] = mitm.iid [item_got];
                you[0].inv_class [i] = mitm.iclass [item_got];
                you[0].inv_type [i] = mitm.itype [item_got];
                you[0].inv_plus [i] = mitm.iplus [item_got];
                you[0].inv_plus2 [i] = mitm.iplus2 [item_got];
                you[0].inv_dam [i] = mitm.idam [item_got];
                you[0].inv_col [i] = mitm.icol [item_got];
                you[0].inv_quant [i] = mitm.iquant [item_got];//mitm.iquant [item_got];

                you[0].inv_no++;
}




unsigned int item_value(unsigned char item_clas, unsigned char item_typ, unsigned char item_da, unsigned char it_plus, unsigned char it_plus2, unsigned int item_quant, char ident_lev, char id [4] [50])
{

int valued = 0;
int charge_value = 0;

switch(item_clas)
  {
        case 0:

 if (item_da > 180)
 {
  if (ident_lev > 0)
  {
   switch(item_da - 180)
   {
        case 1: valued += 1200; break; //strcat(glog , "Singing Sword"); break;
        case 2: valued += 1000; break; //strcat(glog , "Wrath of Trog"); break;
        case 3: valued += 800; break; //strcat(glog , "Scythe of Curses"); break;
        case 4: valued += 700; break; //strcat(glog , "Mace of Variability"); break;
        case 5: valued += 1000; break; //strcat(glog , "Glaive of Prune"); break;
        case 6: valued += 1200; break; //strcat(glog , "Sceptre of Torment"); break;
        case 7: valued += 1250; break; //strcat(glog , "Sword of Zonguldrok"); break;
        case 8: valued += 2000; break; //strcat(glog , "Sword of Okawaru"); break;
        case 9: valued += 1200; break; //strcat(glog , ""); break;
        case 10: valued += 1500; break; //strcat(glog , ""); break;


        default: valued += 1000; break;
   }
  break;
  } // end if ident_lev
 } // end uniques

//        valued = 30;
        switch(item_typ)
        {
        case 0: valued += 10; break; //strcat(glog , "club"); break;
        case 1: valued += 30; break; //strcat(glog , "mace"); break;
        case 2: valued += 35; break; //strcat(glog , "flail"); break;
        case 3: valued += 20; break; //strcat(glog , "dagger"); break;
        case 4: valued += 35; break; //strcat(glog , "spiked mace"); break;
        case 5: valued += 32; break; //strcat(glog , "short sword"); break;
        case 6: valued += 45; break; //strcat(glog , "long sword"); break;
        case 7: valued += 60; break; //strcat(glog , "great sword"); break;
        case 8: valued += 45; break; //strcat(glog , "scimitar"); break;
        case 9: valued += 28; break; //strcat(glog , "hand axe"); break;
        case 10: valued += 65; break; //strcat(glog , "battleaxe"); break;
        case 11: valued += 32; break; //strcat(glog , "spear"); break;
        case 12: valued += 52; break; //strcat(glog , "halberd"); break;
        case 13: valued += 15; break; //strcat(glog , "sling"); break;
        case 14: valued += 31; break; //strcat(glog , "bow"); break;
        case 15: valued += 41; break; //strcat(glog , "crossbow"); break;
        case 16: valued += 51; break; //strcat(glog , "hand crossbow"); break;
        case 17: valued += 55; break; //strcat(glog , "glaive"); break;
        case 18: valued += 25; break; //strcat(glog , "quarterstaff"); break;
        case 19: valued += 30; break; //strcat(glog , "scythe"); break;
        case 20: valued += 17; break; //strcat(glog , "giant club"); break;
        case 21: valued += 19; break; //strcat(glog , "giant spiked club"); break;
        case 22: valued += 65; break; // eveningstar
        case 23: valued += 250; break; // quick blade
        case 24: valued += 300; break; // katana
        case 25: valued += 100; break; // exec axe
        case 26: valued += 200; break; // 2x sw
        case 27: valued += 300; break; // 3x sw
        case 28: valued += 30; break; // hammer
        case 29: valued += 40; break; // ancus
        case 30: valued += 25; break; // whip
        case 31: valued += 40; break; // sabre
        case 32: valued += 300; break; // demon blade
        case 33: valued += 230; break; // demon whip

        }


if (ident_lev > 1)
{
 switch(item_da % 30)
 {
  case 0: break;
  case 1: valued *= 25; break; //strcat(glog, " of flaming"); break;
  case 2: valued *= 25; break; //strcat(glog, " of freezing"); break;
  case 3: valued *= 50; break; //strcat(glog, " of holy wrath"); break;
  case 4: valued *= 30; break; //strcat(glog, " of electrocution"); break;
  case 5: valued *= 21; break; //strcat(glog, " of orc slaying"); break;
  case 6: valued *= 23; break; //strcat(glog, " of venom"); break;
  case 7: valued *= 20; break; //strcat(glog, " of protection"); break;
  case 8: valued *= 64; break; //strcat(glog, " of draining"); break;
  case 9: valued *= 40; break; //strcat(glog, " of speed"); break;
  case 10: valued *= 30; break; // slicing etc
  case 11: valued *= 50; break; // flaming
  case 12: valued *= 50; break; // frost
  case 13: valued *= 60; break; // vampiric
  case 14: valued *= 50; break; // mace of disruption
  case 15: valued *= 30; break; // pain
  case 16: valued *= 30; break; // distortion
  default: valued *= 70; break; /* randart */
 }
 if (item_da % 30 != 0) valued /= 10;
}

if (item_da / 30 == 4 || item_da / 30 == 5) // elf/dwarf
{
  valued *= 12;
  valued /= 10;
}

if (item_da / 30 == 6) // orc
{
  valued *= 8;
  valued /= 10;
}


        if (ident_lev > 2)
        {

         if (it_plus >= 50 && (it_plus <= 130 || it_plus >= 150))
         {
           valued += (it_plus % 50) * 2;
           valued *= 10 + (it_plus % 50) + 2 * (it_plus % 50);
           valued /= 10;
         }

         if (it_plus2 >= 50)
         {
           valued += (it_plus2 % 50) * 2;
           valued *= 10 + (it_plus2 % 50) + 2 * (it_plus2 % 50);
           valued /= 10;
         }

//        if (valued == 30) valued = 0;
/*
        switch(item_typ)
        {
        case 0: valued += 2; break; //strcat(glog , "club"); break;
        case 1: valued += 20; break; //strcat(glog , "mace"); break;
        case 2: valued += 25; break; //strcat(glog , "flail"); break;
        case 3: valued += 10; break; //strcat(glog , "dagger"); break;
        case 4: valued += 25; break; //strcat(glog , "spiked mace"); break;
        case 5: valued += 22; break; //strcat(glog , "short sword"); break;
        case 6: valued += 30; break; //strcat(glog , "long sword"); break;
        case 7: valued += 50; break; //strcat(glog , "great sword"); break;
        case 8: valued += 27; break; //strcat(glog , "scimitar"); break;
        case 9: valued += 20; break; //strcat(glog , "hand axe"); break;
        case 10: valued += 55; break; //strcat(glog , "battleaxe"); break;
        case 11: valued += 12; break; //strcat(glog , "spear"); break;
        case 12: valued += 45; break; //strcat(glog , "halberd"); break;
        case 13: valued += 5; break; //strcat(glog , "sling"); break;
        case 14: valued += 21; break; //strcat(glog , "bow"); break;
        case 15: valued += 31; break; //strcat(glog , "crossbow"); break;
 case 16: valued += 41; break; //strcat(glog , "hand crossbow"); break;
 case 17: valued += 41; break; //strcat(glog , "glaive"); break;
        case 18: valued += 11; break; //strcat(glog , "quarterstaff"); break;
 case 19: valued += 10; break; //strcat(glog , "scythe"); break;
        case 20: valued += 7; break; //strcat(glog , "giant club"); break;
        case 21: valued += 9; break; //strcat(glog , "giant spiked club"); break;

        }
*/

         if (it_plus % 50 > 30)
         {
          valued -= 5;
          valued += ((it_plus % 50) - 50) * ((it_plus % 50) - 50) * ((it_plus % 50) - 50);
          if (valued < 1) valued = 1;
//          break;
         }

         if (it_plus2 % 50 > 30)
         {
          valued -= 5;
          valued += ((it_plus2 % 50) - 50) * ((it_plus2 % 50) - 50) * ((it_plus2 % 50) - 50);
          if (valued < 1) valued = 1;
         }


        }

        if (ident_lev > 0)
        {

        if (it_plus >= 130)
        {
           valued *= 6;
           valued /= 10;
        }


        }

 break;


        case 1: // ammunition
        if (ident_lev > 0)
        {
         // assume not cursed (can they be anyway?)
         if (it_plus < 50) valued -= 11150;
         if (it_plus >= 50) valued += (it_plus % 50) * 2;
        }

        switch(item_typ)
        {
        case 0: valued += 1; break; //strcat(glog , "stone"); break;
        case 1: valued += 2; break; //strcat(glog , "arrow"); break;
        case 2: valued += 2; break; //strcat(glog , "bolt"); break;
        case 3: valued += 1; break; //strcat(glog , "dart"); break;
        case 4: valued += 1; break; //strcat(glog , "eggplant"); break;
        case 5: valued += 1; break; //strcat(glog , "large rock"); break;
        case 6: valued += 5; break; //strcat(glog , ""); break;
        case 7: valued += 5; break; //strcat(glog , ""); break;
        case 8: valued += 5; break; //strcat(glog , ""); break;
        case 9: valued += 5; break; //strcat(glog , ""); break;
        case 10: valued += 5; break; //strcat(glog , ""); break;
        case 12: valued += 5; break; //strcat(glog , ""); break;
        case 13: valued += 5; break; //strcat(glog , ""); break;
        case 14: valued += 5; break; //strcat(glog , ""); break;
        case 15: valued += 5; break; //strcat(glog , ""); break;
        case 16: valued += 5; break; //strcat(glog , ""); break;
        }

        break;



        case 2: // armour


  /*
        if (ident_lev > 2)
        {
                if (it_plus >= 50 && (it_plus <= 130 || it_plus >= 150)) valued += it_plus % 50 * 15;
                if (it_plus < 50) valued -= 20 + (50 - it_plus) * 15;

//             valued += it_plus % 50;
        }*/

        switch(item_typ )
        {
        case 0: valued += 7; break; //strcat(glog , "robe"); break;
        case 1: valued += 20; break; //strcat(glog , "leather armour");
        case 2: valued += 40; break; //strcat(glog , "ring mail"); break;
        case 3: valued += 83; break; //strcat(glog , "scale mail"); break;
        case 4: valued += 110; break; //strcat(glog , "chain mail"); break;
        case 5: valued += 140; break; //strcat(glog , "splint mail"); break;
        case 6: valued += 150; break; //strcat(glog , "banded mail"); break;
        case 7: valued += 230; break; //strcat(glog , "plate mail"); break;
        case 8: valued += 45; break; //strcat(glog , "shield"); break;
        case 9: valued += 10; break; //strcat(glog , "cloak");
        case 10: valued += 25; break; //strcat(glog , "helmet"); break;
        case 11: valued += 12; break; //strcat(glog , "gloves");
        case 12: valued += 15; break; //strcat(glog , "boots");
        case 13: valued += 25; break; //strcat(glog , "buckler"); break;
        case 14: valued += 75; break; //strcat(glog , "large shield"); break;
        case 15: valued += 500; break; //strcat(glog , "dragon hide"); break;
        case 16: valued += 130; break; // strcat(glog , "troll hide"); break;
        case 17: valued += 500; break; // strcat(glog , "crystal plate mail"); break;
        case 18: valued += 750; break; // strcat(glog , "dragon scale mail"); break;
        case 19: valued += 500; break; // strcat(glog , "troll leather armour"); break;
        case 20: valued += 500; break; // strcat(glog , "ice dragon hide"); break;
        case 21: valued += 750; break; // strcat(glog , "ice dragon scale mail"); break;
        case 22: valued += 200; break; //strcat(glog , "steam dragon hide"); break;
        case 23: valued += 300; break; //  , "steam dragon scale mail"); break;
        case 24: valued += 300; break; //  , "mottled dragon hide"); break;
        case 25: valued += 400; break; //  , "mottled dragon scale mail"); break;
        case 26: valued += 900; break; //  , "storm dragon hide"); break;
        case 27: valued += 1050; break; //  , "storm dragon scale mail"); break;
        case 28: valued += 1400; break; //  , "gold dragon hide"); break;
        case 29: valued += 1600; break; //  , "gold dragon scale mail"); break;
                case 30: valued += 3; break; // animal skins
        case 31: valued += 400; break; // strcat(glog , "swamp dragon hide"); break;
        case 32: valued += 650; break; // strcat(glog , "swamp dragon scale mail"); break;


        }


 if (ident_lev > 1)
 {
  switch(item_da % 30)
  {
   case 0: break;
   case 1: valued *= 40; break; //strcat(glog, " of running"); break;
   case 2: valued *= 30; break; //strcat(glog, " of fire resistance"); break;
   case 3: valued *= 30; break; //strcat(glog, " of cold resistance"); break;
   case 4: valued *= 20; break; //strcat(glog, " of poison resistance"); break;
   case 5: valued *= 30; break; //strcat(glog, " of see invisible"); break;
   case 6: valued *= 60; break; //strcat(glog, " of invisibility"); break;
   case 7: valued *= 30; break; //strcat(glog, " of strength"); break;
   case 8: valued *= 30; break; //strcat(glog, " of dexterity"); break;
   case 9: valued *= 30; break; //strcat(glog, " of intelligence"); break;
  case 10: valued *= 5; break; //strcat(glog, " of ponderousness"); break;
  case 11: valued *= 30; break; //strcat(glog, " of levitation"); break;
  case 12: valued *= 40; break; //strcat(glog, " of magic resistance"); break;
  case 13: valued *= 40; break; //strcat(glog, " of protection"); break;
  case 14: valued *= 30; break; // of stealth
  case 15: valued *= 60; break; // of resistance
  case 16: valued *= 50; break; // of life prot
  case 17: valued *= 100; break; // of the Archmagi
  case 18: valued *= 30; break; // of preservation
  default: /* assume a randart */
       valued *= 70; break;
  }
 if (item_da % 30 != 0) valued /= 10;
 }

if (item_da / 30 == 4 || item_da / 30 == 5) // elf/dwarf
{
  valued *= 12;
  valued /= 10;
}

if (item_da / 30 == 6) // orc
{
  valued *= 8;
  valued /= 10;
}

/*      if (ident_lev > 0)
        {
                if (it_plus >= 130)
                {
                        //strcat(glog, "cursed ");
                        valued *= 5; // oh well.
                        valued /= 10;
                        break;
                }
        }

*/

        if (ident_lev >= 2)
        {
         valued += 5;
         if (it_plus >= 50 && (it_plus <= 130 || it_plus >= 150))
         {
           valued += (it_plus % 50) * 30;
           valued *= 10 + (it_plus % 50) * 3 * (it_plus % 50);
           valued /= 10;
         }

         if (it_plus % 50 > 30)
         {
          valued += ((it_plus % 50) - 50) * ((it_plus % 50) - 50) * ((it_plus % 50) - 50);
          if (valued < 1) valued = 1;
//          break;
         }
        }

        if (ident_lev > 0)
        {

        if (it_plus >= 130)
        {
           valued *= 6;
           valued /= 10;
        }


        }




 break;


        case 3: // devices
        charge_value = 0;
        if (id [0] [item_typ] >= 1)
        {
                switch(item_typ)
                {
                case 0: valued += 5; charge_value += 2; break; //strcat(glog, "wand of flame"); break;
                case 1: valued += 5; charge_value += 2; break; //strcat(glog, "wand of frost"); break;
                case 2: valued += 10; charge_value += 3; break; //strcat(glog, "wand of slowing"); break;
                case 3: valued += 15; charge_value += 3; break; //strcat(glog, "wand of hasting"); break;
                case 4: valued += 3; charge_value += 1; break; //strcat(glog, "wand of magic darts"); break;
                case 5: valued += 7; charge_value += 3; break; //strcat(glog, "wand of healing"); break;
                case 6: valued += 12; charge_value += 3; break; //strcat(glog, "wand of paralysis"); break;
                case 7: valued += 15; charge_value += 3; break; //strcat(glog, "wand of fire"); break;
                case 8: valued += 15; charge_value += 3; break; //strcat(glog, "wand of cold"); break;
                case 9: valued += 10; charge_value += 2; break; //strcat(glog, "wand of confusion"); break;
                case 10: valued += 15; charge_value += 2; break; //strcat(glog, "wand of invisibility"); break;
                case 11: valued += 10; charge_value += 2; break; //strcat(glog, "wand of digging"); break;
      case 12: valued += 20; charge_value += 5; break; //strcat(glog, "wand of fireball"); break;
      case 13: valued += 10; charge_value += 2; break; //strcat(glog, "wand of teleportation"); break;
                case 14: valued += 20; charge_value += 5; break; //strcat(glog, "wand of lightning"); break;
      case 15: valued += 15; charge_value += 4; break; //strcat(glog, "wand of polymorph"); break;
      case 16: valued += 15; charge_value += 3; break; //strcat(glog, "wand of enslavement"); break;
      case 17: valued += 20; charge_value += 4; break; //strcat(glog, "wand of draining"); break;
      case 18: valued += 13; charge_value += 3; break; //strcat(glog, "wand of random effects"); break;
      case 19: valued += 17; charge_value += 4; break; //strcat(glog, "wand of disintegration"); break;
      default: valued += 10; break;
      } //else valued = 10;

        if (ident_lev > 1)
        {
         valued += it_plus * charge_value;
        }
       valued *= 3;
       if (it_plus == 0) valued = 3; // change if wands are rechargeable!
       } else valued = 35;
       break;




        case 8: // potions

        if (id [3] [item_typ] == 0)
        {
         valued += 9;
         break;
        }
                switch(item_typ)
                {
                case 0: valued += 20; break; //strcat(glog , "healing"); break;
                case 1: valued += 30; break; //strcat(glog , "heal wounds"); break;
                case 2: valued += 25; break; //strcat(glog , "speed"); break;
                case 3: valued += 25; break; //strcat(glog , "might"); break;
                case 4: valued += 350; break; //strcat(glog , "gain strength"); break;
                case 5: valued += 350; break; //strcat(glog , "gain dexterity"); break;
                case 6: valued += 350; break; //strcat(glog , "gain intelligence"); break;
                case 7: valued += 20; break; //strcat(glog , "levitation"); break;
                case 8: valued += 1; break; //strcat(glog , "poison"); break;
                case 9: valued += 1; break; //strcat(glog , "slowing"); break;
                case 10: valued += 1; break; //strcat(glog , "paralysis"); break;
                case 11: valued += 1; break; //strcat(glog , "confusion"); break;
                case 12: valued += 55; break; //strcat(glog , "invisibility"); break;
                case 13: valued += 10; break; //strcat(glog , "porridge"); break;
                case 14: valued += 1; break; //strcat(glog , "degeneration"); break;
                case 15: valued += 1; break; //strcat(glog , "decay"); break; // Rare!
                case 16: valued += 1; break; //strcat(glog , "water"); break;
                case 17: valued += 500; break; //strcat(glog , "experience"); break;
                case 18: valued += 120; break; //strcat(glog , "magic"); break;
                case 19: valued += 50; break; //strcat(glog , "restore abilities"); break;
                case 20: valued += 1; break; //strcat(glog , "strong poison"); break;
                case 21: valued += 30; break; // berserk rage
                case 22: valued += 150; break; // cure mutation
                case 23: valued += 50; break; // mutation
                }
        break;

        case 4: // food
//      icolour  = 6;
        switch(item_typ )
        {
        case 0: valued = 10; break; //strcat(glog , "meat ration"); break;
        case 1: valued = 10; break; //strcat(glog , "bread ration"); break;
        case 2: valued = 3; break; //strcat(glog , "pear"); break;
        case 3: valued = 3; break; //strcat(glog , "apple"); break; break; // make this less common.
        case 4: valued = 5; break; //strcat(glog , "choko"); break;
        case 5: valued = 14; break; //strcat(glog , "honeycomb"); break;
        case 6: valued = 35; break; //strcat(glog , "royal jell"); break; break; // maybe a joke monster of the same name? - mix something with jelly crystals?
        case 7: valued = 1; break; //strcat(glog , "snozzcumber"); break;
 case 8: valued = 5; break; //strcat(glog , "slice of pizza"); break;
 case 9: valued = 3; break; //strcat(glog , "apricot"); break;
 case 10: valued = 4; break; //strcat(glog , "orange"); break;
 case 11: valued = 3; break; //strcat(glog , "banana"); break;
 case 12: valued = 1; break; //strcat(glog , "strawberr"); break;
 case 13: valued = 1; break; //strcat(glog , "rambutan"); break;
 case 14: valued = 4; break; //strcat(glog , "lemon"); break;
 case 15: valued = 1; break; //strcat(glog , "grape"); break;
 case 16: valued = 1; break; //strcat(glog , "sultana"); break;
 case 17: valued = 2; break; //strcat(glog , "lychee"); break;
 case 18: valued = 7; break; //strcat(glog , "beef jerk"); break;
 case 19: valued = 6; break; //strcat(glog , "cheese"); break;
 case 20: valued = 4; break; //strcat(glog , "sausage"); break;
 case 21: valued = 1; break;
      // break;
 }
 valued *= 8;
 break;

        case 6: // scrolls

        if (id [1] [item_typ] == 0)
        {
         valued += 10;
        }

        switch(item_typ)
        {
        case 0: valued += 20; break; //strcat(glog , "of identify"); break;
        case 1: valued += 30; break; //strcat(glog , "of teleportation"); break;
        case 2: valued += 45; break; //strcat(glog , "of fear"); break;
        case 3: valued += 2; break; //strcat(glog , "of noise"); break;
        case 4: valued += 30; break; //strcat(glog , "of remove curse"); break;
        case 5: valued += 20; break; //strcat(glog , "of detect curse"); break;
        case 6: valued += 95; break; //strcat(glog , "of summoning"); break;
        case 7: valued += 48; break; //strcat(glog , "of enchant weapon"); break;
        case 8: valued += 48; break; //strcat(glog , "of enchant armour"); break;
        case 9: valued += 75; break; //strcat(glog , "of torment"); break;
        case 10: valued += 2; break; //strcat(glog , "of random uselessness"); break;
        case 11: valued += 1; break; //strcat(glog , "of curse weapon"); break;
        case 12: valued += 1; break; //strcat(glog , "of curse armour"); break;
        case 13: valued += 1; break; //strcat(glog , "of immolation"); break;
        case 14: valued += 30; break; //strcat(glog , "of blinking"); break;
        case 15: valued += 1; break; //strcat(glog , "of paper"); break;
        case 16: valued += 35; break; //strcat(glog , "of magic mapping"); break;
        case 17: valued += 1; break; //strcat(glog , "of forgetfulness"); break;
        case 18: valued += 520; break; //strcat(glog , "of acquirement"); break;
        case 19: valued += 55; break; //strcat(glog , "of enchant weapon II"); break;
        case 20: valued += 200; break; //strcat(glog , "of vorpalise weapon"); break;
        case 21: valued += 50; break; // recharging
        case 22: valued += 200; break; // ench wpn iii
        }
        break;

        case 7: // jewellery

        if (id [2] [item_typ] == 0)
        {
         valued += 50;
        }


   if (ident_lev > 0)
   {
      if (it_plus >= 80)
      {
         valued -= 10;
      }
   }


   if (id [2] [item_typ] > 0)
        {

        if (ident_lev > 1 && item_typ == 1 || item_typ == 5 || item_typ == 11 || item_typ == 14 || item_typ == 15)
        {
                if (it_plus >= 50 && (it_plus <= 130 || it_plus >= 150)) valued += 10 * (it_plus % 50);
        }

        switch(item_typ)
        {
        case 0: valued += 75; break; //strcat(glog , "ring of regeneration"); break;
        case 1: valued += 10; break; //strcat(glog , "ring of protection"); break;
        case 2: valued += 30; break; //strcat(glog , "ring of protection from fire"); break;
        case 3: valued += 30; break; //strcat(glog , "ring of poison resistance"); break;
        case 4: valued += 30; break; //strcat(glog , "ring of protection from cold"); break;
        case 5: valued += 10; break; //strcat(glog , "ring of strength"); break;
        case 6: valued += 30; break; //strcat(glog , "ring of slaying"); break;
        case 7: valued += 20; break; //strcat(glog , "ring of see invisible"); break;
        case 8: valued += 100; break; //strcat(glog , "ring of invisibility"); break;
        case 9: valued -= 50; break; //strcat(glog , "ring of hunger"); break;
        case 10: valued -= 10; break; //strcat(glog , "ring of teleportation"); break;
        case 11: valued += 10; break; //strcat(glog , "ring of evasion"); break;
 case 12: valued += 25; break; //strcat(glog , "ring of sustain abilities"); break;
 case 13: valued += 25; break; //strcat(glog , "ring of sustenance"); break;
 case 14: valued += 10; break; //strcat(glog , "ring of dexterity"); break;
 case 15: valued += 10; break; //strcat(glog , "ring of intelligence"); break;
 case 16: valued += 35; break; //strcat(glog , "ring of wizardry"); break;
 case 17: valued += 40; break; //strcat(glog , "ring of magical power"); break;
 case 18: valued += 30; break; //strcat(glog , "ring of levitation"); break;
 case 19: valued += 60; break; //strcat(glog , "ring of life protection"); break;
 case 20: valued += 40; break; //strcat(glog , "ring of protection from magic"); break;
 case 21: valued += 62; break; //strcat(glog , "ring of fire"); break;
 case 22: valued += 62; break; //strcat(glog , "ring of ice"); break;
 case 23: valued += 42; break; // ring of teleport control

 case 35: valued += 20; break; //amulet of rage
 case 36: valued += 30; break; //amulet of maintain speed - not foolproof
 case 37: valued += 30; break; //amulet of clarity - not foolproof
 case 38: valued += 30; break; //amulet of warding
 case 39: valued += 30; break; //amulet of resist corrosion
 case 40: valued += 15; break; //amulet of the gourmand
 case 41: valued += 25; break; //amulet of conservation
 case 42: valued += 25; break; //amulet of controlled flight
 case 43: valued -= 50; break; //amulet of inaccuracy
 case 44: valued += 30; break; //amulet of resist mutation

 // got to do delusion!
 }

 if (item_da == 200) valued += 50;

 valued *= 7;

 if (valued <= 0) valued = 1;

 } // end of if ident_lev
break;

case 13: // Miscellaneous
switch(ident_lev)
{
case 0:
 switch(item_typ)
 {
 case 0: valued += 350; break; //strcat(glog, "sealed bronze flask"); break;
 case 1: valued += 450; break; //strcat(glog, "crystal ball"); break;
// case 2: valued += 40; break; //strcat(glog, ""); break;
 case 6: valued += 1000; break; // silver horn
 case 14: valued += 1000; break; // rune
 default: valued += 400;
 }
break;

default:
 switch(item_typ)
 {
 case 0: valued += 400; break; //strcat(glog, "bottled efreet"); break;
 case 1: valued += 500; break; //strcat(glog, "crystal ball of seeing"); break;
// case 2: valued += 40; break; //strcat(glog, ""); break;
 case 6: valued += 5000; break; // horn of Geryon
      case 11:
      case 12:
      valued += 20; break;
 case 13: // disc of storms
      valued += 2000;
      break;
 case 14: valued += 1200; break; // rune of Zot
 default: valued += 500;
 }
break;

} // end switch ident_lev
break;

//case 10: break;

case 10:
//strcpy(glog, "");
if (ident_lev == 0)
{
 valued = 150;
 break;
}

valued = 150 + book_rarity(item_typ) * 50;

/*
valued = 210;

if (item_typ < 7 || item_typ == 23 || item_typ == 25)
  {
   valued = 150;
   break;
  }

if (item_typ == 14 || item_typ == 24) // gr conj/necronomicon
{
 valued = 550;
 break;
}

if (item_typ == 15)
{
 valued = 550;
 break;
}

if (item_typ == 17 || item_typ == 35)
{
 valued = 470;
 break;
}
*/
break;


case 11: // magical staves
if (ident_lev == 0)
{
 valued = 120;
 break;
} // end if

valued = 250;
if (item_typ == 10 || item_typ == 16 || item_typ == 17)
  {
   valued = 150;
  }
break;


case 12:
valued = 10000;
     //strcpy(glog , "Orb of Zot");
break;


} // end switch

if (valued <= 1) valued = 1;
valued *= item_quant;
return valued;

//}
}




char book_rarity(char which_book)
{

 switch(which_book)
 {
  case 0:
  case 1:
  case 2:
//  case 3:
//  case 4:
  return 1;

  case 3:
  case 4:
  return 3;

  case 5:
  case 6:
  return 4;

  case 7:
  return 18;

  case 8:
  case 9:
  return 10;

  case 10:
  return 1;

  case 11:
  return 5;

  case 12:
  return 7;

  case 13:
  return 1;

  case 14:
  case 15:
  return 11;

  case 16:
  return 1;

  case 17:
  return 2;

  case 18:
  return 8;

  case 19:
  return 3;

  case 21:
  return 6;

  case 22:
  return 100;

  case 23:
  return 3;

  case 24:
  return 20;

  case 25:
  return 3;

  case 26:
  return 2;

  case 27:
  return 15;

  case 28:
  return 4;

  case 29:
  return 10;

  case 30:
  return 8;

  case 31:
  return 6;

  case 32:
  return 12;

  case 33:
  return 17;

  case 34:
  return 10;

  case 35: // tome of destruction
  return 30;

  case 36:
  return 10;

  case 37:
  return 12;

  case 38:
  return 16;

  case 39:
  return 4;

  case 40:
  return 10;

  case 41: // manuals
  return 20;

  case 42: // wizardry
  return 3;

  case 43: // power
  return 6;

 }

return 1;

}

void shop(void)
{

int i;

for (i = 0; i < 6; i ++)
{
 if (i == 5)
 {
  strcpy(info, "Help! Non-existent shop.");
  mpr(info);
  return;
 }
 if (env[0].sh_x [i] == you[0].x_pos && env[0].sh_y [i] == you[0].y_pos)
    break;
}

char identy [4] [50];
save_id(identy);

in_a_shop(i, identy);
you[0].gp_ch = 1;
burden_change();
#ifdef PLAIN_TERM
redraw_screen();
#endif
}

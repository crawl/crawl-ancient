/*
 *  File:       shopping.cc
 *  Summary:    Shop keeper functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "shopping.h"

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"
#include "enum.h"

#include "describe.h"
#include "invent.h"
#include "itemname.h"
#include "player.h"
#include "stuff.h"

#include <stdlib.h>
#include <string.h>

#ifdef MACROS
  #include "macro.h"
#endif

/*#include "crawlfnc.h"
   #include "fn2.h"
   #include "invent.h"
   #include "struct.h" */

//#define ITEMS 500
/*
   struct item_struct
   {
   unsigned char base_type [ITEMS];
   unsigned char sub_type [ITEMS];
   unsigned char pluses [ITEMS]; // +, charges, remaining food value
   unsigned char pluses2 [ITEMS];
   unsigned char special [ITEMS]; // damage
   unsigned int quantity [ITEMS]; // multiple items
   unsigned char x [ITEMS]; //  x-location
   unsigned char y [ITEMS]; //  y-location
   unsigned char colour [ITEMS];
   unsigned char id [ITEMS];

   unsigned int link [ITEMS];

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

unsigned int item_value(unsigned char item_clas, unsigned char item_typ, unsigned char item_da, unsigned char it_plus, unsigned char it_plus2, unsigned int item_quant, char ident_lev, char id[4][50]);

char in_a_shop(char shoppy, char id[4][50]);

void shop_print(char *shoppy, char sh_line);

char more3();

void clear_line();

char shop_getch();

void shop_init_id(int i, int shop_id[4][50]);
void shop_uninit_id(int i, int shop_id[4][50]);

void shop_set_id(int i, int shop_id[4][50], unsigned char base_type, unsigned char sub_type);

char book_rarity(char which_book);

char in_a_shop(char shoppy, char id[4][50])
{
    unsigned int greedy = env.sh_greed[shoppy];
    int shop_id[4][50];
    int shop_items[20];

    char st_pass[60];

    char gold_p[10];

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
    char sh_name[40];


//char *make_name(unsigned char var1, unsigned char var2, unsigned char var3, char ncase)

    strcpy(sh_name, "Welcome to ");
    char str_pass[50];

    make_name(env.keeper_name[shoppy][0], env.keeper_name[shoppy][1], env.keeper_name[shoppy][2], 3, str_pass);
    strcat(sh_name, str_pass);
    strcat(sh_name, "'s ");
    switch (env.sh_type[shoppy])
    {
    case 0:
        strcat(sh_name, "Weapon Shop!");
        break;
    case 1:
        strcat(sh_name, "Armour Shop!");
        break;
    case 2:
        strcat(sh_name, "Antique Weapon Shop!");
        break;
    case 3:
        strcat(sh_name, "Antique Armour Shop!");
        break;
    case 4:
        strcat(sh_name, "Assorted Antiques!");
        break;
    case 5:
        strcat(sh_name, "Jewellery Shop!");
        break;
    case 6:
        strcat(sh_name, "Magical Wand Shop!");
        break;
    case 7:
        strcat(sh_name, "Book Shop!");
        break;
    case 8:
        strcat(sh_name, "Food Shop!");
        break;
    case 9:
        strcat(sh_name, "Distillery!");
        break;
    case 10:
        strcat(sh_name, "Magic scroll Shop!");
        break;
    case 11:
        strcat(sh_name, "General Store!");
        break;
    }

//      strcat(sh_name, " shop.");

    shop_print(sh_name, 20);    //"Hello, and welcome to ");

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

print_stock:
    clrscr();
    itty = igrd[0][5 + shoppy];


    if (igrd[0][5 + shoppy] == 501)
    {
      empty_shop:shop_print("I'm sorry, my shop is empty now.", 20);
        more3();
        goto goodbye;
    }

    if (you.num_inv_items >= 52)
    {
        shop_print("You seem to be carrying too much.", 20);
        more3();
        goto goodbye;
    }


    if (itty == 501)
        goto empty_shop;

    for (i = 1; i < 20; i++)    // remember i is a char

    {

        shop_items[i - 1] = itty;

        if (itty == 501)        //mitm.link [itty] == 501)

        {
            shop_items[i - 1] = 501;
            continue;
        }

        itty = mitm.link[itty];

    }

    itty = igrd[0][5 + shoppy];

    for (i = 1; i < 18; i++)    // remember i is a char

    {

        gotoxy(1, i);

        if (i % 2 == 0)
            textcolor(LIGHTGREY);
        else
            textcolor(WHITE);

        item_name(mitm.pluses2[itty], mitm.base_type[itty], mitm.sub_type[itty], mitm.special[itty], mitm.pluses[itty], mitm.quantity[itty], mitm.id[itty], 3, st_pass);
        putch(i + 96);
        cprintf(" - ");
        cprintf(st_pass);

        gp_value = greedy * item_value(mitm.base_type[itty], mitm.sub_type[itty], mitm.special[itty], mitm.pluses[itty], mitm.pluses2[itty], mitm.quantity[itty], mitm.id[itty], id);
        gp_value /= 10;
        if (gp_value <= 1)
            gp_value = 1;
        gotoxy(60, i);
        itoa(gp_value, st_pass, 10);
        cprintf(st_pass);
        cprintf("gold");
        if (mitm.link[itty] == 501)
            break;

        itty = mitm.link[itty];

    }

    textcolor(LIGHTGREY);

    shop_print("Type letter to buy item, x/Esc to leave, ?/* for inventory, V to examine.", 23);

purchase:
    strcpy(sh_name, "You have ");
    itoa(you.gold, gold_p, 10);
    strcat(sh_name, gold_p);
    strcat(sh_name, " gold piece");
    if (you.gold == 1)
        strcat(sh_name, ".");
    else
        strcat(sh_name, "s.");
    shop_print(sh_name, 19);

    //shop_print("What would you like to purchase?", 20);

    shop_print("What would you like to purchase?", 20);

    ft = shop_getch();

    if (ft == 'x' || ft == 27)
        goto goodbye;

    if (ft == 'V')
    {
        shop_print("Examine which item?", 20);
        ft = shop_getch();

        if (ft < 'a' || ft > 'z')
        {
            goto huh;
        }

        ft -= 97;

        if (ft > 18)
            goto huh;

        if (shop_items[ft] == 501)
        {
            shop_print("I'm sorry, you seem to be confused.", 20);
            more3();
            goto purchase;
        }

        describe_item(mitm.base_type[shop_items[ft]], mitm.sub_type[shop_items[ft]], mitm.pluses[shop_items[ft]], mitm.pluses2[shop_items[ft]], mitm.special[shop_items[ft]], mitm.id[shop_items[ft]]);

        goto print_stock;

    }

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
      huh:shop_print("Huh?", 20);
        more3();
        goto purchase;
    }

    ft -= 97;

    if (ft > 18)
        goto huh;

    if (shop_items[ft] == 501)
    {
        shop_print("I'm sorry, you seem to be confused.", 20);
        more3();
        goto purchase;
    }

    if (greedy * (item_value(mitm.base_type[shop_items[ft]], mitm.sub_type[shop_items[ft]], mitm.special[shop_items[ft]], mitm.pluses[shop_items[ft]], mitm.pluses2[shop_items[ft]], mitm.quantity[shop_items[ft]], mitm.id[shop_items[ft]], id)) / 10 > you.gold)
    {
        shop_print("I'm sorry, you don't seem to have enough money.", 20);
        more3();
        goto purchase;
    }

    purchase(shop_items[ft]);
    shop_set_id(i, shop_id, mitm.base_type[shop_items[ft]], mitm.sub_type[shop_items[ft]]);

    you.gold -= (greedy * item_value(mitm.base_type[shop_items[ft]], mitm.sub_type[shop_items[ft]], mitm.special[shop_items[ft]], mitm.pluses[shop_items[ft]], mitm.pluses2[shop_items[ft]], mitm.quantity[shop_items[ft]], mitm.id[shop_items[ft]], id)) / 10;

    mitm.quantity[shop_items[ft]] = 0;
    if (ft == 0)
    {
        igrd[0][5 + shoppy] = mitm.link[shop_items[ft]];
    }
    else
        mitm.link[shop_items[ft - 1]] = mitm.link[shop_items[ft]];

    goto print_stock;

goodbye:
    clear_line();
    shop_print("Goodbye!", 20);
    more3();
#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
    gotoxy(1, 1);
    cprintf(" ");
#endif
//      for (i = 0; i < 4; i ++)
    //      {
    //      }
    //
    shop_uninit_id(shoppy, shop_id);
    return 0;
}


void shop_init_id(int i, int shop_id[4][50])
{
    int j = 0;

    if (env.sh_type[i] != 2 && env.sh_type[i] != 3 && env.sh_type[i] != 4)
        for (j = 0; j < 50; j++)
        {
            shop_id[0][j] = get_id(OBJ_WANDS, j);
            set_id(OBJ_WANDS, j, 1);
            shop_id[1][j] = get_id(OBJ_SCROLLS, j);
            set_id(OBJ_SCROLLS, j, 1);
            shop_id[2][j] = get_id(OBJ_JEWELLERY, j);
            set_id(OBJ_JEWELLERY, j, 1);
            shop_id[3][j] = get_id(OBJ_POTIONS, j);
            set_id(OBJ_POTIONS, j, 1);
        }


}

void shop_uninit_id(int i, int shop_id[4][50])
{
    int j = 0;

    if (env.sh_type[i] != 2 && env.sh_type[i] != 3 && env.sh_type[i] != 4)
        for (j = 0; j < 50; j++)
        {
//        shop_id [i] [j] = get_id(i, j);
            set_id(OBJ_WANDS, j, shop_id[0][j]);
            set_id(OBJ_SCROLLS, j, shop_id[1][j]);
            set_id(OBJ_JEWELLERY, j, shop_id[2][j]);
            set_id(OBJ_POTIONS, j, shop_id[3][j]);
        }
}

void shop_set_id(int i, int shop_id[4][50], unsigned char base_type, unsigned char sub_type)
{

    if (env.sh_type[i] != 2 && env.sh_type[i] != 3 && env.sh_type[i] != 4)
    {
        switch (base_type)
        {
        case OBJ_WANDS:
            shop_id[0][sub_type] = 1;
            break;
        case OBJ_SCROLLS:
            shop_id[1][sub_type] = 1;
            break;
        case OBJ_JEWELLERY:
            shop_id[2][sub_type] = 1;
            break;
        case OBJ_POTIONS:
            shop_id[3][sub_type] = 1;
            break;
        }
        set_id(base_type, sub_type, 1);
    }

}


void shop_print(char *shoppy, char sh_lines)
{
    int i;

    gotoxy(1, sh_lines);
    cprintf(shoppy);
    for (i = strlen(shoppy); i < 80; i++)
    {
        cprintf(" ");
    }

}

char more3(void)
{
    char keyin = 0;

    gotoxy(70, 20);
    cprintf("-more-");
    keyin = getch();
    if (keyin == 0)
        getch();
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
    if (keyin == 0)
        getch();
    return keyin;
}

void purchase(int item_got)
{

    int i = 0;

    for (i = 0; i < 53; i++)
    {
        if (i == 52)
        {
            shop_print("The demon of the infinite void grins at you.", 20);
            more3();
            return;
        }
        if (you.inv_quantity[i] <= 0)
            break;
    }

    you.inv_ident[i] = mitm.id[item_got];
    you.inv_class[i] = mitm.base_type[item_got];
    you.inv_type[i] = mitm.sub_type[item_got];
    you.inv_plus[i] = mitm.pluses[item_got];
    you.inv_plus2[i] = mitm.pluses2[item_got];
    you.inv_dam[i] = mitm.special[item_got];
    you.inv_colour[i] = mitm.colour[item_got];
    you.inv_quantity[i] = mitm.quantity[item_got];      //mitm.quantity [item_got];

    you.num_inv_items++;
}




unsigned int item_value(unsigned char item_clas, unsigned char item_typ, unsigned char item_da, unsigned char it_plus, unsigned char it_plus2, unsigned int item_quant, char ident_lev, char id[4][50])
{

    int valued = 0;
    int charge_value = 0;

    switch (item_clas)
    {
    case OBJ_WEAPONS:

        if (item_da > 180)
        {
            if (ident_lev > 0)
            {
                switch (item_da - 180)
                {
                case 1:
                    valued += 1200;
                    break;      //strcat(glog , "Singing Sword"); break;

                case 2:
                    valued += 1000;
                    break;      //strcat(glog , "Wrath of Trog"); break;

                case 3:
                    valued += 800;
                    break;      //strcat(glog , "Scythe of Curses"); break;

                case 4:
                    valued += 700;
                    break;      //strcat(glog , "Mace of Variability"); break;

                case 5:
                    valued += 1000;
                    break;      //strcat(glog , "Glaive of Prune"); break;

                case 6:
                    valued += 1200;
                    break;      //strcat(glog , "Sceptre of Torment"); break;

                case 7:
                    valued += 1250;
                    break;      //strcat(glog , "Sword of Zonguldrok"); break;

                case 8:
                    valued += 2000;
                    break;      //strcat(glog , "Sword of Okawaru"); break;

                case 9:
                    valued += 1200;
                    break;      //strcat(glog , ""); break;

                case 10:
                    valued += 1500;
                    break;      //strcat(glog , ""); break;


                default:
                    valued += 1000;
                    break;
                }
                break;
            }                   // end if ident_lev

        }                       // end uniques

//        valued = 30;
        switch (item_typ)
        {
        case WPN_CLUB:
            valued += 10;
            break;              //strcat(glog , "club"); break;

        case WPN_MACE:
            valued += 30;
            break;              //strcat(glog , "mace"); break;

        case WPN_FLAIL:
            valued += 35;
            break;              //strcat(glog , "flail"); break;

        case WPN_DAGGER:
            valued += 20;
            break;              //strcat(glog , "dagger"); break;

        case WPN_MORNINGSTAR:
            valued += 35;
            break;              //strcat(glog , "spiked mace"); break;

        case WPN_SHORT_SWORD:
            valued += 32;
            break;              //strcat(glog , "short sword"); break;

        case WPN_LONG_SWORD:
            valued += 45;
            break;              //strcat(glog , "long sword"); break;

        case WPN_GREAT_SWORD:
            valued += 60;
            break;              //strcat(glog , "great sword"); break;

        case WPN_SCIMITAR:
            valued += 45;
            break;              //strcat(glog , "scimitar"); break;

        case WPN_HAND_AXE:
            valued += 28;
            break;              //strcat(glog , "hand axe"); break;

        case WPN_BATTLEAXE:
            valued += 65;
            break;              //strcat(glog , "battleaxe"); break;

        case WPN_SPEAR:
            valued += 32;
            break;              //strcat(glog , "spear"); break;

        case WPN_HALBERD:
            valued += 52;
            break;              //strcat(glog , "halberd"); break;

        case WPN_SLING:
            valued += 15;
            break;              //strcat(glog , "sling"); break;

        case WPN_BOW:
            valued += 31;
            break;              //strcat(glog , "bow"); break;

        case WPN_CROSSBOW:
            valued += 41;
            break;              //strcat(glog , "crossbow"); break;

        case WPN_HAND_CROSSBOW:
            valued += 51;
            break;              //strcat(glog , "hand crossbow"); break;

        case WPN_GLAIVE:
            valued += 55;
            break;              //strcat(glog , "glaive"); break;

        case WPN_QUARTERSTAFF:
            valued += 25;
            break;              //strcat(glog , "quarterstaff"); break;

        case WPN_SCYTHE:
            valued += 30;
            break;              //strcat(glog , "scythe"); break;

        case WPN_GIANT_CLUB:
            valued += 17;
            break;              //strcat(glog , "giant club"); break;

        case WPN_GIANT_SPIKED_CLUB:
            valued += 19;
            break;              //strcat(glog , "giant spiked club"); break;

        case WPN_EVENINGSTAR:
            valued += 65;
            break;              // eveningstar

        case WPN_QUICK_BLADE:
            valued += 250;
            break;              // quick blade

        case WPN_KATANA:
            valued += 300;
            break;              // katana

        case WPN_EXECUTIONERS_AXE:
            valued += 100;
            break;              // exec axe

        case WPN_DOUBLE_SWORD:
            valued += 200;
            break;              // 2x sw

        case WPN_TRIPLE_SWORD:
            valued += 300;
            break;              // 3x sw

        case WPN_HAMMER:
            valued += 30;
            break;              // hammer

        case WPN_ANCUS:
            valued += 40;
            break;              // ancus

        case WPN_WHIP:
            valued += 25;
            break;              // whip

        case WPN_SABRE:
            valued += 40;
            break;              // sabre

        case WPN_DEMON_BLADE:
            valued += 300;
            break;              // demon blade

        case WPN_DEMON_WHIP:
            valued += 230;
            break;              // demon whip

        }


        if (ident_lev > 1)
        {
            switch (item_da % 30)
            {
            case SPWPN_NORMAL:
                break;
            case SPWPN_FLAMING:
                valued *= 25;
                break;          //strcat(glog, " of flaming"); break;

            case SPWPN_FREEZING:
                valued *= 25;
                break;          //strcat(glog, " of freezing"); break;

            case SPWPN_HOLY_WRATH:
                valued *= 50;
                break;          //strcat(glog, " of holy wrath"); break;

            case SPWPN_ELECTROCUTION:
                valued *= 30;
                break;          //strcat(glog, " of electrocution"); break;

            case SPWPN_ORC_SLAYING:
                valued *= 21;
                break;          //strcat(glog, " of orc slaying"); break;

            case SPWPN_VENOM:
                valued *= 23;
                break;          //strcat(glog, " of venom"); break;

            case SPWPN_PROTECTION:
                valued *= 20;
                break;          //strcat(glog, " of protection"); break;

            case SPWPN_DRAINING:
                valued *= 64;
                break;          //strcat(glog, " of draining"); break;

            case SPWPN_SPEED:
                valued *= 40;
                break;          //strcat(glog, " of speed"); break;

            case SPWPN_VORPAL:
                valued *= 30;
                break;          // slicing etc

            case SPWPN_FLAME:
                valued *= 50;
                break;          // flaming

            case SPWPN_FROST:
                valued *= 50;
                break;          // frost

            case SPWPN_VAMPIRICISM:
                valued *= 60;
                break;          // vampiric

            case SPWPN_DISRUPTION:
                valued *= 50;
                break;          // mace of disruption

            case SPWPN_PAIN:
                valued *= 30;
                break;          // pain

            case SPWPN_DISTORTION:
                valued *= 30;
                break;          // distortion

            default:
                valued *= 70;
                break;          /* randart */
            }
            if (item_da % 30 != 0)
                valued /= 10;
        }

        if (item_da / 30 == DWPN_ELVEN || item_da / 30 == DWPN_DWARVEN)         // elf/dwarf

        {
            valued *= 12;
            valued /= 10;
        }

        if (item_da / 30 == 6)  // orc

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
                if (valued < 1)
                    valued = 1;
//          break;
            }

            if (it_plus2 % 50 > 30)
            {
                valued -= 5;
                valued += ((it_plus2 % 50) - 50) * ((it_plus2 % 50) - 50) * ((it_plus2 % 50) - 50);
                if (valued < 1)
                    valued = 1;
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


    case OBJ_MISSILES:          // ammunition

        if (ident_lev > 0)
        {
            // assume not cursed (can they be anyway?)
            if (it_plus < 50)
                valued -= 11150;
            if (it_plus >= 50)
                valued += (it_plus % 50) * 2;
        }

        switch (item_typ)
        {
        case MI_STONE:
            valued += 1;
            break;              //strcat(glog , "stone"); break;

        case MI_ARROW:
            valued += 2;
            break;              //strcat(glog , "arrow"); break;

        case MI_BOLT:
            valued += 2;
            break;              //strcat(glog , "bolt"); break;

        case MI_DART:
            valued += 1;
            break;              //strcat(glog , "dart"); break;

        case MI_EGGPLANT:
            valued += 1;
            break;              //strcat(glog , "eggplant"); break;

        case MI_LARGE_ROCK:
            valued += 1;
            break;              //strcat(glog , "large rock"); break;

        case 6:
            valued += 5;
            break;              //strcat(glog , ""); break;

        case 7:
            valued += 5;
            break;              //strcat(glog , ""); break;

        case 8:
            valued += 5;
            break;              //strcat(glog , ""); break;

        case 9:
            valued += 5;
            break;              //strcat(glog , ""); break;

        case 10:
            valued += 5;
            break;              //strcat(glog , ""); break;

        case 12:
            valued += 5;
            break;              //strcat(glog , ""); break;

        case 13:
            valued += 5;
            break;              //strcat(glog , ""); break;

        case 14:
            valued += 5;
            break;              //strcat(glog , ""); break;

        case 15:
            valued += 5;
            break;              //strcat(glog , ""); break;

        case 16:
            valued += 5;
            break;              //strcat(glog , ""); break;

        }

        break;



    case OBJ_ARMOUR:            // armour


        /*
           if (ident_lev > 2)
           {
           if (it_plus >= 50 && (it_plus <= 130 || it_plus >= 150)) valued += it_plus % 50 * 15;
           if (it_plus < 50) valued -= 20 + (50 - it_plus) * 15;

           //             valued += it_plus % 50;
           } */

        switch (item_typ)
        {
        case ARM_ROBE:
            valued += 7;
            break;              //strcat(glog , "robe"); break;

        case ARM_LEATHER_ARMOUR:
            valued += 20;
            break;              //strcat(glog , "leather armour");

        case ARM_RING_MAIL:
            valued += 40;
            break;              //strcat(glog , "ring mail"); break;

        case ARM_SCALE_MAIL:
            valued += 83;
            break;              //strcat(glog , "scale mail"); break;

        case ARM_CHAIN_MAIL:
            valued += 110;
            break;              //strcat(glog , "chain mail"); break;

        case ARM_SPLINT_MAIL:
            valued += 140;
            break;              //strcat(glog , "splint mail"); break;

        case ARM_BANDED_MAIL:
            valued += 150;
            break;              //strcat(glog , "banded mail"); break;

        case ARM_PLATE_MAIL:
            valued += 230;
            break;              //strcat(glog , "plate mail"); break;

        case ARM_SHIELD:
            valued += 45;
            break;              //strcat(glog , "shield"); break;

        case ARM_CLOAK:
            valued += 10;
            break;              //strcat(glog , "cloak");

        case ARM_HELMET:
            valued += 25;
            break;              //strcat(glog , "helmet"); break;

        case ARM_GLOVES:
            valued += 12;
            break;              //strcat(glog , "gloves");

        case ARM_BOOTS:
            valued += 15;
            break;              //strcat(glog , "boots");

        case ARM_BUCKLER:
            valued += 25;
            break;              //strcat(glog , "buckler"); break;

        case ARM_LARGE_SHIELD:
            valued += 75;
            break;              //strcat(glog , "large shield"); break;

        case ARM_DRAGON_HIDE:
            valued += 500;
            break;              //strcat(glog , "dragon hide"); break;

        case ARM_TROLL_HIDE:
            valued += 130;
            break;              // strcat(glog , "troll hide"); break;

        case ARM_CRYSTAL_PLATE_MAIL:
            valued += 500;
            break;              // strcat(glog , "crystal plate mail"); break;

        case ARM_DRAGON_ARMOUR:
            valued += 750;
            break;              // strcat(glog , "dragon scale mail"); break;

        case ARM_TROLL_LEATHER_ARMOUR:
            valued += 500;
            break;              // strcat(glog , "troll leather armour"); break;

        case ARM_ICE_DRAGON_HIDE:
            valued += 500;
            break;              // strcat(glog , "ice dragon hide"); break;

        case ARM_ICE_DRAGON_ARMOUR:
            valued += 750;
            break;              // strcat(glog , "ice dragon scale mail"); break;

        case ARM_STEAM_DRAGON_HIDE:
            valued += 200;
            break;              //strcat(glog , "steam dragon hide"); break;

        case ARM_STEAM_DRAGON_ARMOUR:
            valued += 300;
            break;              //  , "steam dragon scale mail"); break;

        case ARM_MOTTLED_DRAGON_HIDE:
            valued += 300;
            break;              //  , "mottled dragon hide"); break;

        case ARM_MOTTLED_DRAGON_ARMOUR:
            valued += 400;
            break;              //  , "mottled dragon scale mail"); break;

        case ARM_STORM_DRAGON_HIDE:
            valued += 900;
            break;              //  , "storm dragon hide"); break;

        case ARM_STORM_DRAGON_ARMOUR:
            valued += 1050;
            break;              //  , "storm dragon scale mail"); break;

        case ARM_GOLD_DRAGON_HIDE:
            valued += 1400;
            break;              //  , "gold dragon hide"); break;

        case ARM_GOLD_DRAGON_ARMOUR:
            valued += 1600;
            break;              //  , "gold dragon scale mail"); break;

        case ARM_ANIMAL_SKIN:
            valued += 3;
            break;              // animal skins

        case ARM_SWAMP_DRAGON_HIDE:
            valued += 400;
            break;              // strcat(glog , "swamp dragon hide"); break;

        case ARM_SWAMP_DRAGON_ARMOUR:
            valued += 650;
            break;              // strcat(glog , "swamp dragon scale mail"); break;


        }


        if (ident_lev > 1)
        {
            switch (item_da % 30)
            {
            case SPARM_NORMAL:
                break;
            case SPARM_RUNNING:
                valued *= 40;
                break;          //strcat(glog, " of running"); break;

            case SPARM_FIRE_RESISTANCE:
                valued *= 30;
                break;          //strcat(glog, " of fire resistance"); break;

            case SPARM_COLD_RESISTANCE:
                valued *= 30;
                break;          //strcat(glog, " of cold resistance"); break;

            case SPARM_POISON_RESISTANCE:
                valued *= 20;
                break;          //strcat(glog, " of poison resistance"); break;

            case SPARM_SEE_INVISIBLE:
                valued *= 30;
                break;          //strcat(glog, " of see invisible"); break;

            case SPARM_DARKNESS:
                valued *= 60;
                break;          //strcat(glog, " of invisibility"); break;

            case SPARM_STRENGTH:
                valued *= 30;
                break;          //strcat(glog, " of strength"); break;

            case SPARM_DEXTERITY:
                valued *= 30;
                break;          //strcat(glog, " of dexterity"); break;

            case SPARM_INTELLIGENCE:
                valued *= 30;
                break;          //strcat(glog, " of intelligence"); break;

            case SPARM_PONDEROUSNESS:
                valued *= 5;
                break;          //strcat(glog, " of ponderousness"); break;

            case SPARM_LEVITATION:
                valued *= 30;
                break;          //strcat(glog, " of levitation"); break;

            case SPARM_MAGIC_RESISTANCE:
                valued *= 40;
                break;          //strcat(glog, " of magic resistance"); break;

            case SPARM_PROTECTION:
                valued *= 40;
                break;          //strcat(glog, " of protection"); break;

            case SPARM_STEALTH:
                valued *= 30;
                break;          // of stealth

            case SPARM_RESISTANCE:
                valued *= 60;
                break;          // of resistance

            case SPARM_POSITIVE_ENERGY:
                valued *= 50;
                break;          // of life prot

            case SPARM_ARCHMAGI:
                valued *= 100;
                break;          // of the Archmagi

            case SPARM_PRESERVATION:
                valued *= 30;
                break;          // of preservation

            default:            /* assume a randart */
                valued *= 70;
                break;
            }
            if (item_da % 30 != 0)
                valued /= 10;
        }

        if (item_da / 30 == DARM_ELVEN || item_da / 30 == DARM_DWARVEN)         // elf/dwarf

        {
            valued *= 12;
            valued /= 10;
        }

        if (item_da / 30 == DARM_ORCISH)        // orc

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
                if (valued < 1)
                    valued = 1;
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


    case OBJ_WANDS:             // devices

        charge_value = 0;
        if (id[0][item_typ] >= 1)
        {
            switch (item_typ)
            {
            case WAND_FLAME:
                valued += 5;
                charge_value += 2;
                break;          //strcat(glog, "wand of flame"); break;

            case WAND_FROST:
                valued += 5;
                charge_value += 2;
                break;          //strcat(glog, "wand of frost"); break;

            case WAND_SLOWING:
                valued += 10;
                charge_value += 3;
                break;          //strcat(glog, "wand of slowing"); break;

            case WAND_HASTING:
                valued += 15;
                charge_value += 3;
                break;          //strcat(glog, "wand of hasting"); break;

            case WAND_MAGIC_DARTS:
                valued += 3;
                charge_value += 1;
                break;          //strcat(glog, "wand of magic darts"); break;

            case WAND_HEALING:
                valued += 7;
                charge_value += 3;
                break;          //strcat(glog, "wand of healing"); break;

            case WAND_PARALYSIS:
                valued += 12;
                charge_value += 3;
                break;          //strcat(glog, "wand of paralysis"); break;

            case WAND_FIRE:
                valued += 15;
                charge_value += 3;
                break;          //strcat(glog, "wand of fire"); break;

            case WAND_COLD:
                valued += 15;
                charge_value += 3;
                break;          //strcat(glog, "wand of cold"); break;

            case WAND_CONFUSION:
                valued += 10;
                charge_value += 2;
                break;          //strcat(glog, "wand of confusion"); break;

            case WAND_INVISIBILITY:
                valued += 15;
                charge_value += 2;
                break;          //strcat(glog, "wand of invisibility"); break;

            case WAND_DIGGING:
                valued += 10;
                charge_value += 2;
                break;          //strcat(glog, "wand of digging"); break;

            case WAND_FIREBALL:
                valued += 20;
                charge_value += 5;
                break;          //strcat(glog, "wand of fireball"); break;

            case 13:
                valued += WAND_INVISIBILITY;
                charge_value += 2;
                break;          //strcat(glog, "wand of teleportation"); break;

            case 14:
                valued += 20;
                charge_value += 5;
                break;          //strcat(glog, "wand of lightning"); break;

            case 15:
                valued += WAND_POLYMORPH_OTHER;
                charge_value += 4;
                break;          //strcat(glog, "wand of polymorph"); break;

            case 16:
                valued += WAND_POLYMORPH_OTHER;
                charge_value += 3;
                break;          //strcat(glog, "wand of enslavement"); break;

            case 17:
                valued += 20;
                charge_value += 4;
                break;          //strcat(glog, "wand of draining"); break;

            case 18:
                valued += WAND_TELEPORTATION;
                charge_value += 3;
                break;          //strcat(glog, "wand of random effects"); break;

            case 19:
                valued += WAND_DRAINING;
                charge_value += 4;
                break;          //strcat(glog, "wand of disintegration"); break;

            default:
                valued += 10;
                break;
            }                   //else valued = 10;

            if (ident_lev > 1)
            {
                valued += it_plus * charge_value;
            }
            valued *= 3;
            if (it_plus == 0)
                valued = 3;     // change if wands are rechargeable!

        }
        else
            valued = 35;
        break;




    case OBJ_POTIONS:           // potions

        if (id[3][item_typ] == 0)
        {
            valued += 9;
            break;
        }
        switch (item_typ)
        {
        case POT_HEALING:
            valued += 20;
            break;              //strcat(glog , "healing"); break;

        case POT_HEAL_WOUNDS:
            valued += 30;
            break;              //strcat(glog , "heal wounds"); break;

        case POT_SPEED:
            valued += 25;
            break;              //strcat(glog , "speed"); break;

        case POT_MIGHT:
            valued += 25;
            break;              //strcat(glog , "might"); break;

        case POT_GAIN_STRENGTH:
            valued += 350;
            break;              //strcat(glog , "gain strength"); break;

        case POT_GAIN_DEXTERITY:
            valued += 350;
            break;              //strcat(glog , "gain dexterity"); break;

        case POT_GAIN_INTELLIGENCE:
            valued += 350;
            break;              //strcat(glog , "gain intelligence"); break;

        case POT_LEVITATION:
            valued += 20;
            break;              //strcat(glog , "levitation"); break;

        case POT_POISON:
            valued += 1;
            break;              //strcat(glog , "poison"); break;

        case POT_SLOWING:
            valued += 1;
            break;              //strcat(glog , "slowing"); break;

        case POT_PARALYSIS:
            valued += 1;
            break;              //strcat(glog , "paralysis"); break;

        case POT_CONFUSION:
            valued += 1;
            break;              //strcat(glog , "confusion"); break;

        case POT_INVISIBILITY:
            valued += 55;
            break;              //strcat(glog , "invisibility"); break;

        case POT_PORRIDGE:
            valued += 10;
            break;              //strcat(glog , "porridge"); break;

        case POT_DEGENERATION:
            valued += 1;
            break;              //strcat(glog , "degeneration"); break;

        case POT_DECAY:
            valued += 1;
            break;              //strcat(glog , "decay"); break; // Rare!

        case POT_WATER:
            valued += 1;
            break;              //strcat(glog , "water"); break;

        case POT_EXPERIENCE:
            valued += 500;
            break;              //strcat(glog , "experience"); break;

        case POT_MAGIC:
            valued += 120;
            break;              //strcat(glog , "magic"); break;

        case POT_RESTORE_ABILITIES:
            valued += 50;
            break;              //strcat(glog , "restore abilities"); break;

        case POT_STRONG_POISON:
            valued += 1;
            break;              //strcat(glog , "strong poison"); break;

        case POT_BERSERK_RAGE:
            valued += 30;
            break;              // berserk rage

        case POT_CURE_MUTATION:
            valued += 150;
            break;              // cure mutation

        case POT_MUTATION:
            valued += 50;
            break;              // mutation

        }
        break;

    case OBJ_FOOD:              // food
        //      icolour  = 6;

        switch (item_typ)
        {
        case FOOD_MEAT_RATION:
            valued = 10;
            break;              //strcat(glog , "meat ration"); break;

        case FOOD_BREAD_RATION:
            valued = 10;
            break;              //strcat(glog , "bread ration"); break;

        case FOOD_PEAR:
            valued = 3;
            break;              //strcat(glog , "pear"); break;

        case FOOD_APPLE:
            valued = 3;
            break;              //strcat(glog , "apple"); break; break; // make this less common.

        case FOOD_CHOKO:
            valued = 5;
            break;              //strcat(glog , "choko"); break;

        case FOOD_HONEYCOMB:
            valued = 14;
            break;              //strcat(glog , "honeycomb"); break;

        case FOOD_ROYAL_JELLY:
            valued = 35;
            break;              //strcat(glog , "royal jell"); break; break; // maybe a joke monster of the same name? - mix something with jelly crystals?

        case FOOD_SNOZZCUMBER:
            valued = 1;
            break;              //strcat(glog , "snozzcumber"); break;

        case FOOD_PIZZA:
            valued = 5;
            break;              //strcat(glog , "slice of pizza"); break;

        case FOOD_APRICOT:
            valued = 3;
            break;              //strcat(glog , "apricot"); break;

        case FOOD_ORANGE:
            valued = 4;
            break;              //strcat(glog , "orange"); break;

        case FOOD_BANANA:
            valued = 3;
            break;              //strcat(glog , "banana"); break;

        case FOOD_STRAWBERRY:
            valued = 1;
            break;              //strcat(glog , "strawberr"); break;

        case FOOD_RAMBUTAN:
            valued = 1;
            break;              //strcat(glog , "rambutan"); break;

        case FOOD_LEMON:
            valued = 4;
            break;              //strcat(glog , "lemon"); break;

        case FOOD_GRAPE:
            valued = 1;
            break;              //strcat(glog , "grape"); break;

        case FOOD_SULTANA:
            valued = 1;
            break;              //strcat(glog , "sultana"); break;

        case FOOD_LYCHEE:
            valued = 2;
            break;              //strcat(glog , "lychee"); break;

        case FOOD_BEEF_JERKY:
            valued = 7;
            break;              //strcat(glog , "beef jerk"); break;

        case FOOD_CHEESE:
            valued = 6;
            break;              //strcat(glog , "cheese"); break;

        case FOOD_SAUSAGE:
            valued = 4;
            break;              //strcat(glog , "sausage"); break;

        case FOOD_CHUNK:
            valued = 1;
            break;
            // break;
        }
        valued *= 8;
        break;

    case OBJ_SCROLLS:           // scrolls

        if (id[1][item_typ] == 0)
        {
            valued += 10;
        }

        switch (item_typ)
        {
        case SCR_IDENTIFY:
            valued += 20;
            break;              //strcat(glog , "of identify"); break;

        case SCR_TELEPORTATION:
            valued += 30;
            break;              //strcat(glog , "of teleportation"); break;

        case SCR_FEAR:
            valued += 45;
            break;              //strcat(glog , "of fear"); break;

        case SCR_NOISE:
            valued += 2;
            break;              //strcat(glog , "of noise"); break;

        case SCR_REMOVE_CURSE:
            valued += 30;
            break;              //strcat(glog , "of remove curse"); break;

        case SCR_DETECT_CURSE:
            valued += 20;
            break;              //strcat(glog , "of detect curse"); break;

        case SCR_SUMMONING:
            valued += 95;
            break;              //strcat(glog , "of summoning"); break;

        case SCR_ENCHANT_WEAPON_I:
            valued += 48;
            break;              //strcat(glog , "of enchant weapon"); break;

        case SCR_ENCHANT_ARMOUR:
            valued += 48;
            break;              //strcat(glog , "of enchant armour"); break;

        case SCR_TORMENT:
            valued += 75;
            break;              //strcat(glog , "of torment"); break;

        case SCR_RANDOM_USELESSNESS:
            valued += 2;
            break;              //strcat(glog , "of random uselessness"); break;

        case SCR_CURSE_WEAPON:
            valued += 1;
            break;              //strcat(glog , "of curse weapon"); break;

        case SCR_CURSE_ARMOUR:
            valued += 1;
            break;              //strcat(glog , "of curse armour"); break;

        case SCR_IMMOLATION:
            valued += 1;
            break;              //strcat(glog , "of immolation"); break;

        case SCR_BLINKING:
            valued += 30;
            break;              //strcat(glog , "of blinking"); break;

        case SCR_PAPER:
            valued += 1;
            break;              //strcat(glog , "of paper"); break;

        case SCR_MAGIC_MAPPING:
            valued += 35;
            break;              //strcat(glog , "of magic mapping"); break;

        case SCR_FORGETFULNESS:
            valued += 1;
            break;              //strcat(glog , "of forgetfulness"); break;

        case SCR_ACQUIREMENT:
            valued += 520;
            break;              //strcat(glog , "of acquirement"); break;

        case SCR_ENCHANT_WEAPON_II:
            valued += 55;
            break;              //strcat(glog , "of enchant weapon II"); break;

        case SCR_VORPALISE_WEAPON:
            valued += 200;
            break;              //strcat(glog , "of vorpalise weapon"); break;

        case SCR_RECHARGING:
            valued += 50;
            break;              // recharging

        case SCR_ENCHANT_WEAPON_III:
            valued += 200;
            break;              // ench wpn iii

        }
        break;

    case OBJ_JEWELLERY: // jewellery

        if (id[2][item_typ] == 0)
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


        if (id[2][item_typ] > 0)
        {

            if (ident_lev > 1 && item_typ == RING_PROTECTION || item_typ == RING_STRENGTH || item_typ == RING_EVASION || item_typ == RING_DEXTERITY || item_typ == RING_INTELLIGENCE)
            {
                if (it_plus >= 50 && (it_plus <= 130 || it_plus >= 150))
                    valued += 10 * (it_plus % 50);
            }

            switch (item_typ)
            {
            case RING_REGENERATION:
                valued += 75;
                break;          //strcat(glog , "ring of regeneration"); break;

            case RING_PROTECTION:
                valued += 10;
                break;          //strcat(glog , "ring of protection"); break;

            case RING_PROTECTION_FROM_FIRE:
                valued += 30;
                break;          //strcat(glog , "ring of protection from fire"); break;

            case RING_POISON_RESISTANCE:
                valued += 30;
                break;          //strcat(glog , "ring of poison resistance"); break;

            case RING_PROTECTION_FROM_COLD:
                valued += 30;
                break;          //strcat(glog , "ring of protection from cold"); break;

            case RING_STRENGTH:
                valued += 10;
                break;          //strcat(glog , "ring of strength"); break;

            case RING_SLAYING:
                valued += 30;
                break;          //strcat(glog , "ring of slaying"); break;

            case RING_SEE_INVISIBLE:
                valued += 20;
                break;          //strcat(glog , "ring of see invisible"); break;

            case RING_INVISIBILITY:
                valued += 100;
                break;          //strcat(glog , "ring of invisibility"); break;

            case RING_HUNGER:
                valued -= 50;
                break;          //strcat(glog , "ring of hunger"); break;

            case RING_TELEPORTATION:
                valued -= 10;
                break;          //strcat(glog , "ring of teleportation"); break;

            case RING_EVASION:
                valued += 10;
                break;          //strcat(glog , "ring of evasion"); break;

            case RING_SUSTAIN_ABILITIES:
                valued += 25;
                break;          //strcat(glog , "ring of sustain abilities"); break;

            case RING_SUSTENANCE:
                valued += 25;
                break;          //strcat(glog , "ring of sustenance"); break;

            case RING_DEXTERITY:
                valued += 10;
                break;          //strcat(glog , "ring of dexterity"); break;

            case RING_INTELLIGENCE:
                valued += 10;
                break;          //strcat(glog , "ring of intelligence"); break;

            case RING_WIZARDRY:
                valued += 35;
                break;          //strcat(glog , "ring of wizardry"); break;

            case RING_MAGICAL_POWER:
                valued += 40;
                break;          //strcat(glog , "ring of magical power"); break;

            case RING_LEVITATION:
                valued += 30;
                break;          //strcat(glog , "ring of levitation"); break;

            case RING_LIFE_PROTECTION:
                valued += 60;
                break;          //strcat(glog , "ring of life protection"); break;

            case RING_PROTECTION_FROM_MAGIC:
                valued += 40;
                break;          //strcat(glog , "ring of protection from magic"); break;

            case RING_FIRE:
                valued += 62;
                break;          //strcat(glog , "ring of fire"); break;

            case RING_ICE:
                valued += 62;
                break;          //strcat(glog , "ring of ice"); break;

            case RING_TELEPORT_CONTROL:
                valued += 42;
                break;          // ring of teleport control

            case AMU_RAGE:
                valued += 20;
                break;          //amulet of rage

            case AMU_MAINTAIN_SPEED:
                valued += 30;
                break;          //amulet of maintain speed - not foolproof

            case AMU_CLARITY:
                valued += 30;
                break;          //amulet of clarity - not foolproof

            case AMU_WARDING:
                valued += 30;
                break;          //amulet of warding

            case AMU_RESIST_CORROSION:
                valued += 30;
                break;          //amulet of resist corrosion

            case AMU_THE_GOURMAND:
                valued += 15;
                break;          //amulet of the gourmand

            case AMU_CONSERVATION:
                valued += 25;
                break;          //amulet of conservation

            case AMU_CONTROLLED_FLIGHT:
                valued += 25;
                break;          //amulet of controlled flight

            case AMU_INACCURACY:
                valued -= 50;
                break;          //amulet of inaccuracy

            case AMU_RESIST_MUTATION:
                valued += 30;
                break;          //amulet of resist mutation

                // got to do delusion!
            }

            if (item_da == 200)
                valued += 50;

            valued *= 7;

            if (valued <= 0)
                valued = 1;

        }                       // end of if ident_lev

        break;

    case OBJ_MISCELLANY:        // Miscellaneous

        switch (ident_lev)
        {
        case 0:
            switch (item_typ)
            {
            case MISC_BOTTLED_EFREET:
                valued += 350;
                break;          //strcat(glog, "sealed bronze flask"); break;

            case MISC_CRYSTAL_BALL_OF_SEEING:
                valued += 450;
                break;          //strcat(glog, "crystal ball"); break;
                // case 2: valued += 40; break; //strcat(glog, ""); break;

            case MISC_HORN_OF_GERYON:
                valued += 1000;
                break;          // silver horn

            case MISC_RUNE_OF_ZOT:
                valued += 1000;
                break;          // rune

            case MISC_DECK_OF_TRICKS:
                valued += 100;
                break;          // deck of tricks

            default:
                valued += 400;
            }
            break;

        default:
            switch (item_typ)
            {
            case MISC_BOTTLED_EFREET:
                valued += 400;
                break;          //strcat(glog, "bottled efreet"); break;

            case MISC_CRYSTAL_BALL_OF_SEEING:
                valued += 500;
                break;          //strcat(glog, "crystal ball of seeing"); break;
                // case 2: valued += 40; break; //strcat(glog, ""); break;

            case MISC_HORN_OF_GERYON:
                valued += 5000;
                break;          // horn of Geryon

            case MISC_EMPTY_EBONY_CASKET:
            case 12:
                valued += 20;
                break;
            case MISC_DISC_OF_STORMS:   // disc of storms

                valued += 2000;
                break;
            case MISC_RUNE_OF_ZOT:
                valued += 1200;
                break;          // rune of Zot

            default:
                valued += 500;
            }
            break;

        }                       // end switch ident_lev

        break;

//case 10: break;

    case OBJ_BOOKS:
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


    case OBJ_STAVES:            // magical staves

        if (ident_lev == 0)
        {
            valued = 120;
            break;
        }                       // end if

        valued = 250;
        if (item_typ == STAFF_SMITING || item_typ == STAFF_WARDING || item_typ == STAFF_DISCOVERY)
        {
            valued = 150;
        }
        break;


    case OBJ_ORBS:
        valued = 10000;
        //strcpy(glog , "Orb of Zot");
        break;


    }                           // end switch

    if (valued <= 1)
        valued = 1;
    valued *= item_quant;
    return valued;

//}
}




char book_rarity(char which_book)
{

    switch (which_book)
    {
    case BOOK_MINOR_MAGIC_I:
    case BOOK_MINOR_MAGIC_II:
    case BOOK_MINOR_MAGIC_III:
//  case 3:
        //  case 4:
        return 1;

    case BOOK_CONJURATIONS_I:
    case BOOK_CONJURATIONS_II:
        return 3;

    case BOOK_FLAMES:
    case BOOK_FROST:
        return 4;

    case BOOK_INVOCATIONS:
        return 18;

    case BOOK_FIRE:
    case BOOK_ICE:
        return 10;

    case BOOK_SURVEYANCES:
        return 1;

    case BOOK_SPATIAL_TRANSLOCATIONS:
        return 5;

    case BOOK_ENCHANTMENTS:
        return 7;

    case BOOK_POISONINGS:
        return 1;

    case BOOK_STORMS_AND_FIRE:
    case BOOK_DEATH:
        return 11;

    case BOOK_HINDERANCE:
        return 1;

    case BOOK_CHANGES:
        return 2;

    case BOOK_TRANSFIGURATIONS:
        return 8;

    case BOOK_USEFUL_MAGIC:
        return 3;

    case BOOK_CLOUDS:
        return 6;

    case BOOK_HEALING:
        return 100;

    case BOOK_NECROMANCY:
        return 3;

    case BOOK_NECRONOMICON:
        return 20;

    case BOOK_SUMMONINGS:
        return 3;

    case BOOK_CHARMS:
        return 2;

    case BOOK_DEMONOLOGY:
        return 15;

    case BOOK_AIR:
        return 4;

    case BOOK_SKY:
        return 10;

    case BOOK_DIVINATIONS:
        return 8;

    case BOOK_WARP:
        return 6;

    case BOOK_ENVENOMATIONS:
        return 12;

    case BOOK_ANNIHILATIONS:
        return 17;

    case BOOK_UNLIFE:
        return 10;

    case BOOK_DESTRUCTION:      // tome of destruction

        return 30;

    case BOOK_CONTROL:
        return 10;

    case BOOK_MUTATIONS:
        return 12;

    case BOOK_TUKIMA:
        return 16;

    case BOOK_GEOMANCY:
        return 4;

    case BOOK_EARTH:
        return 10;

    case BOOK_MANUAL:           // manuals

        return 20;

    case BOOK_WIZARDRY: // wizardry

        return 3;

    case BOOK_POWER:            // power

        return 6;

    }

    return 1;

}

void shop(void)
{

    int i;

    for (i = 0; i < 6; i++)
    {
        if (i == 5)
        {
            strcpy(info, "Help! Non-existent shop.");
            mpr(info);
            return;
        }
        if (env.sh_x[i] == you.x_pos && env.sh_y[i] == you.y_pos)
            break;
    }

    char identy[4][50];

    save_id(identy);

    in_a_shop(i, identy);
    you.redraw_gold = 1;
    burden_change();
#ifdef PLAIN_TERM
    redraw_screen();
#endif
}

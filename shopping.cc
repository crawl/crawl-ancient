/*
 *  File:       shopping.cc
 *  Summary:    Shop keeper functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <2>     Oct 31 99      CDL             right justify prices
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "shopping.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "describe.h"
#include "invent.h"
#include "itemname.h"
#include "player.h"
#include "spl-book.h"
#include "stuff.h"

#ifdef MACROS
#include "macro.h"
#endif


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


char in_a_shop(char shoppy, char id[4][50]);
char more3(void);
void purchase(int item_got);
void shop_init_id(int i, int shop_id[4][50]);
void shop_print(char *shoppy, char sh_line);
void shop_set_id(int i, int shop_id[4][50], unsigned char base_type, unsigned char sub_type);
void shop_uninit_id(int i, int shop_id[4][50]);




char in_a_shop( char shoppy, char id[4][50] )
{

    unsigned int greedy = env.sh_greed[shoppy];
    unsigned char shop_type = env.sh_type[shoppy];    // easier to work with {dlb}

    int shop_id[4][50];
    int shop_items[20];

    char st_pass[60];

    char gold_p[10];

    int gp_value = 0;
    char i;
    unsigned char ft;


#ifdef DOS_TERM
    char buffer[4800];

    gettext(1, 1, 80, 25, buffer);
#endif

    strcpy(st_pass, "");

#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif

    clrscr();
    int itty = 0;
    char sh_name[40];

    strcpy(sh_name, "Welcome to ");

    char str_pass[50];
    make_name(env.keeper_name[shoppy][0], env.keeper_name[shoppy][1], env.keeper_name[shoppy][2], 3, str_pass);

    strcat(sh_name, str_pass);
    strcat(sh_name, "'s ");

    if ( shop_type == SHOP_WEAPON_ANTIQUE
          || shop_type == SHOP_ARMOUR_ANTIQUE )
      strcat(sh_name, "Antique ");

    strcat(sh_name, (shop_type == SHOP_WEAPON
                      || shop_type == SHOP_WEAPON_ANTIQUE) ? "Weapon" :
                    (shop_type == SHOP_ARMOUR
                      || shop_type == SHOP_ARMOUR_ANTIQUE) ? "Armour" :
                    (shop_type == SHOP_JEWELLERY )         ? "Jewellery" :
                    (shop_type == SHOP_WAND )              ? "Magical Wand" :
                    (shop_type == SHOP_BOOK )              ? "Book" :
                    (shop_type == SHOP_FOOD )              ? "Food" :
                    (shop_type == SHOP_SCROLL )            ? "Magic Scroll" :
                    (shop_type == SHOP_GENERAL_ANTIQUE )   ? "Assorted Antiques" :
                    (shop_type == SHOP_DISTILLERY )        ? "Distillery" :
                    (shop_type == SHOP_GENERAL )           ? "General Store"
                                                           : "Bug" );

    if ( shop_type != SHOP_GENERAL
        && shop_type != SHOP_GENERAL_ANTIQUE
        && shop_type != SHOP_DISTILLERY )
      strcat(sh_name," Shop");

    strcat(sh_name,"!");

    shop_print(sh_name, 20);

    more3();

    shop_init_id(shoppy, shop_id);

/* *************************************
THINGS TO DO:
  Allow inventory
  Remove id change for antique shops
  selling?
************************************* */

    save_id(id);

print_stock:
    clrscr();
    itty = igrd[0][5 + shoppy];

    if (igrd[0][5 + shoppy] == ING)
    {
empty_shop:
        shop_print("I'm sorry, my shop is empty now.", 20);
        more3();
        goto goodbye;
    }

    if ( you.num_inv_items >= ENDOFPACK )
    {
        shop_print("You seem to be carrying too much.", 20);
        more3();
        goto goodbye;
    }

    if (itty == ING)
        goto empty_shop;

    for (i = 1; i < 20; i++)
    {
        shop_items[i - 1] = itty;

        if ( itty == ING )        //mitm.link [itty] == ING)
        {
            shop_items[i - 1] = ING;
            continue;
        }

        itty = mitm.link[itty];
    }

    itty = igrd[0][5 + shoppy];

    for (i = 1; i < 18; i++)
    {
        gotoxy(1, i);

        textcolor( (i % 2) ? WHITE : LIGHTGREY );

        it_name(itty, 3, st_pass);
        putch(i + 96);
        cprintf(" - ");
        cprintf(st_pass);

        gp_value = greedy * item_value(mitm.base_type[itty], mitm.sub_type[itty], mitm.special[itty], mitm.pluses[itty], mitm.pluses2[itty], mitm.quantity[itty], mitm.id[itty], id);
        gp_value /= 10;
        if (gp_value <= 1)
            gp_value = 1;
        gotoxy(60, i);
        // cdl - itoa(gp_value, st_pass, 10);
        sprintf(st_pass, "%5d", gp_value);
        cprintf(st_pass);
        cprintf(" gold");
        if (mitm.link[itty] == ING)
            break;

        itty = mitm.link[itty];

    }

    textcolor(LIGHTGREY);

    shop_print("Type letter to buy item, x/Esc to leave, ?/* for inventory, v to examine.", 23);

purchase:
    strcpy(sh_name, "You have ");
    itoa(you.gold, gold_p, 10);
    strcat(sh_name, gold_p);
    strcat(sh_name, " gold piece");
    strcat(sh_name, (you.gold == 1) ? "." : "s.");

    shop_print(sh_name, 19);

    shop_print("What would you like to purchase?", 20);

    ft = get_ch();

    if ( ft == 'x' || ft == 27 )
      goto goodbye;

    if (ft == 'v')
    {
        shop_print("Examine which item?", 20);
        ft = get_ch();

        if ( ft < 'a' || ft > 'z' )     // wonder whether this should be recoded to permit uppercase, too? {dlb}
          goto huh;

        ft -= 'a';  // see above comment {dlb}

        if ( ft > 18 )
          goto huh;

        if (shop_items[ft] == ING)
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
        invent(-1, false);
        shop_init_id(shoppy, shop_id);
#ifdef DOS_TERM
        window(1, 1, 80, 25);
#endif
        goto print_stock;
    }

    if ( ft < 'a' || ft > 'z' )    // see earlier comments re: uppercase {dlb}
    {
huh:
        shop_print("Huh?", 20);
        more3();
        goto purchase;
    }

    ft -= 'a';    // see earlier comments re: uppercase {dlb}

    if ( ft > 18 )
      goto huh;

    if (shop_items[ft] == ING)
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

    if ( ft == 0 )
      igrd[0][5 + shoppy] = mitm.link[shop_items[ft]];
    else
      mitm.link[shop_items[ft - 1]] = mitm.link[shop_items[ft]];

    goto print_stock;

goodbye:
    //clear_line();
    shop_print("Goodbye!", 20);
    more3();

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
    gotoxy(1, 1);
    cprintf(" ");
#endif

    shop_uninit_id(shoppy, shop_id);
    return 0;

}


void shop_init_id(int i, int shop_id[4][50])
{

    unsigned char j = 0;

    if ( env.sh_type[i] != SHOP_WEAPON_ANTIQUE
        && env.sh_type[i] != SHOP_ARMOUR_ANTIQUE
        && env.sh_type[i] != SHOP_GENERAL_ANTIQUE )
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

    unsigned char j = 0;

    if ( env.sh_type[i] != SHOP_WEAPON_ANTIQUE
        && env.sh_type[i] != SHOP_ARMOUR_ANTIQUE
        && env.sh_type[i] != SHOP_GENERAL_ANTIQUE )
      for (j = 0; j < 50; j++)
      {
          set_id(OBJ_WANDS, j, shop_id[0][j]);
          set_id(OBJ_SCROLLS, j, shop_id[1][j]);
          set_id(OBJ_JEWELLERY, j, shop_id[2][j]);
          set_id(OBJ_POTIONS, j, shop_id[3][j]);
      }

}




void shop_set_id(int i, int shop_id[4][50], unsigned char base_type, unsigned char sub_type)
{

    if ( env.sh_type[i] != SHOP_WEAPON_ANTIQUE
        && env.sh_type[i] != SHOP_ARMOUR_ANTIQUE
        && env.sh_type[i] != SHOP_GENERAL_ANTIQUE )
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




void shop_print( char *shoppy, char sh_lines )
{

    gotoxy(1, sh_lines);

    cprintf(shoppy);

    for (int i = strlen(shoppy); i < 80; i++)
      cprintf(" ");

}




char more3( void )
{
    char keyin = 0;

    gotoxy(70, 20);
    cprintf("-more-");
    keyin = getch();
    if (keyin == 0)
        getch();
    //clear_line();
    return keyin;
}




void purchase( int item_got )
{

    unsigned char i = 0;

    for (i = 0; i < (ENDOFPACK + 1); i++)
    {
        if ( i == ENDOFPACK )
        {
            shop_print("The demon of the infinite void grins at you.", 20);
            more3();
            return;
        }

        if ( you.inv_quantity[i] < 1 )
          break;
    }

    you.inv_ident[i] = mitm.id[item_got];
    you.inv_class[i] = mitm.base_type[item_got];
    you.inv_type[i] = mitm.sub_type[item_got];
    you.inv_plus[i] = mitm.pluses[item_got];
    you.inv_plus2[i] = mitm.pluses2[item_got];
    you.inv_dam[i] = mitm.special[item_got];
    you.inv_colour[i] = mitm.colour[item_got];
    you.inv_quantity[i] = mitm.quantity[item_got];

    you.num_inv_items++;

}          // end purchase()




unsigned int item_value( unsigned char item_clas, unsigned char item_typ, unsigned char item_da, unsigned char it_plus, unsigned char it_plus2, unsigned int item_quant, char ident_lev, char id[4][50] )
{

    int valued = 0;
    int charge_value = 0;

    switch ( item_clas )
    {
      case OBJ_WEAPONS:

        if ( item_da >= NWPN_SINGING_SWORD )
        {
            if (ident_lev > 0)
            {
                switch (item_da)
                {
                case NWPN_SWORD_OF_CEREBOV:
                    valued += 2000;
                    break;

                case NWPN_SCEPTRE_OF_ASMODEUS:
                    valued += 1500;
                    break;

                case NWPN_SWORD_OF_ZONGULDROK:
                    valued += 1250;
                    break;

                case NWPN_SCEPTRE_OF_TORMENT:
                case NWPN_SINGING_SWORD:
                case NWPN_STAFF_OF_DISPATER:
                    valued += 1200;
                    break;

                case NWPN_GLAIVE_OF_PRUNE:
                case NWPN_WRATH_OF_TROG:
                    valued += 1000;
                    break;

                case NWPN_SCYTHE_OF_CURSES:
                    valued += 800;
                    break;

                case NWPN_MACE_OF_VARIABILITY:
                    valued += 700;
                    break;

                default:
                    valued += 1000;
                    break;
                }
                break;
            }                   // end if ident_lev

        }                       // end uniques

        switch ( item_typ )
        {
        case WPN_CLUB:
        case WPN_KNIFE:
            valued += 10;
            break;

        case WPN_SLING:
            valued += 15;
            break;

        case WPN_GIANT_CLUB:
            valued += 17;
            break;

        case WPN_GIANT_SPIKED_CLUB:
            valued += 19;
            break;

        case WPN_DAGGER:
            valued += 20;
            break;

        case WPN_QUARTERSTAFF:
        case WPN_WHIP:
            valued += 25;
            break;

        case WPN_HAND_AXE:
            valued += 28;
            break;

        case WPN_HAMMER:
        case WPN_MACE:
        case WPN_SCYTHE:
            valued += 30;
            break;

        case WPN_BOW:
            valued += 31;
            break;

        case WPN_SHORT_SWORD:
        case WPN_SPEAR:
            valued += 32;
            break;

        case WPN_FLAIL:
            valued += 35;
            break;

        case WPN_ANCUS:
        case WPN_AXE:
        case WPN_MORNINGSTAR:
        case WPN_SABRE:
            valued += 40;
            break;

        case WPN_CROSSBOW:
            valued += 41;
            break;

        case WPN_TRIDENT:
            valued += 42;
            break;

        case WPN_LONG_SWORD:
        case WPN_SCIMITAR:
            valued += 45;
            break;

        case WPN_SPIKED_FLAIL:
            valued += 50;

        case WPN_HAND_CROSSBOW:
            valued += 51;
            break;

        case WPN_HALBERD:
            valued += 52;
            break;

        case WPN_GLAIVE:
            valued += 55;
            break;

        case WPN_BROAD_AXE:
        case WPN_GREAT_SWORD:
            valued += 60;
            break;

        case WPN_BATTLEAXE:
        case WPN_GREAT_MACE:
            valued += 65;
            break;

        case WPN_GREAT_FLAIL:
            valued += 75;
            break;

        case WPN_EVENINGSTAR:
            valued += 65;
            break;

        case WPN_EXECUTIONERS_AXE:
            valued += 100;
            break;

        case WPN_DOUBLE_SWORD:
            valued += 200;
            break;

        case WPN_DEMON_WHIP:
            valued += 230;
            break;

        case WPN_QUICK_BLADE:
            valued += 250;
            break;

        case WPN_KATANA:
        case WPN_TRIPLE_SWORD:
        case WPN_DEMON_BLADE:
            valued += 300;
            break;

        case WPN_DEMON_TRIDENT:
            valued += 333;
            break;

        }


        if (ident_lev > 1)
        {
            switch ( item_da % 30 )
            {
              case SPWPN_NORMAL:
                  break;

              case SPWPN_DRAINING:
                  valued *= 64;
                  break;

              case SPWPN_VAMPIRICISM:
                  valued *= 60;
                  break;

              case SPWPN_DISRUPTION:
              case SPWPN_FLAME:
              case SPWPN_FROST:
              case SPWPN_HOLY_WRATH:
              case SPWPN_REACHING:
                  valued *= 50;
                  break;

              case SPWPN_SPEED:
                  valued *= 40;
                  break;

              case SPWPN_DISTORTION:
              case SPWPN_ELECTROCUTION:
              case SPWPN_PAIN:
              case SPWPN_VORPAL:
                  valued *= 30;
                  break;

              case SPWPN_FLAMING:
              case SPWPN_FREEZING:
                  valued *= 25;
                  break;

              case SPWPN_VENOM:
                  valued *= 23;
                  break;

              case SPWPN_ORC_SLAYING:
                  valued *= 21;
                  break;

              case SPWPN_PROTECTION:
                  valued *= 20;
                  break;

              default:    // randart
                  valued *= 70;
                  break;
            }

            if ( item_da % 30 != SPWPN_NORMAL )
              valued /= 10;
        }

        if (item_da / 30 == DWPN_ELVEN || item_da / 30 == DWPN_DWARVEN)         // elf/dwarf
        {
            valued *= 12;
            valued /= 10;
        }

        if (item_da / 30 == DWPN_ORCISH)  // value was "6" but comment read "orc", so I went with comment {dlb}
        {
            valued *= 8;
            valued /= 10;
        }


        if (ident_lev > 2)
        {

            if (it_plus >= 50 && (it_plus <= 130 || it_plus >= 150))
            {
                valued += (it_plus % 50) << 1;
                valued *= 10 + (it_plus % 50) + 2 * (it_plus % 50);
                valued /= 10;
            }

            if (it_plus2 >= 50)
            {
                valued += (it_plus2 % 50) << 1;
                valued *= 10 + (it_plus2 % 50) + 2 * (it_plus2 % 50);
                valued /= 10;
            }

            if (it_plus % 50 > 30)
            {
                valued -= 5;
                valued += ((it_plus % 50) - 50) * ((it_plus % 50) - 50) * ((it_plus % 50) - 50);
                if (valued < 1)
                    valued = 1;
                //break;
            }

            if (it_plus2 % 50 > 30)
            {
                valued -= 5;
                valued += ((it_plus2 % 50) - 50) * ((it_plus2 % 50) - 50) * ((it_plus2 % 50) - 50);
                if (valued < 1)
                    valued = 1;
            }

        }

        if (ident_lev > 0 && it_plus >= 130)
        {
            valued *= 6;
            valued /= 10;
        }


        break;


    case OBJ_MISSILES:          // ammunition

        if (ident_lev > 0)
        {
            // assume not cursed (can they be anyway?)
            if ( it_plus < 50 )
              valued -= 11150;
            if ( it_plus >= 50 )
              valued += (it_plus % 50) << 1;
        }

        switch ( item_typ )
        {
          case MI_DART:
          case MI_LARGE_ROCK:
          case MI_STONE:
          case MI_EGGPLANT:
            valued++;
            break;
          case MI_ARROW:
          case MI_BOLT:
            valued += 2;
            break;
          default:              // was: cases 6 through 16 with empty strcat()'s 15jan2000 {dlb}
            valued += 5;
            break;              //strcat(glog , ""); break;

        }

        break;


    case OBJ_ARMOUR:
/*
           if (ident_lev > 2)
           {
           if (it_plus >= 50 && (it_plus <= 130 || it_plus >= 150)) valued += it_plus % 50 * 15;
           if (it_plus < 50) valued -= 20 + (50 - it_plus) * 15;

           //             valued += it_plus % 50;
           }
*/

        switch (item_typ)
        {
        case ARM_GOLD_DRAGON_ARMOUR:
            valued += 1600;
            break;

        case ARM_GOLD_DRAGON_HIDE:
            valued += 1400;
            break;

        case ARM_STORM_DRAGON_ARMOUR:
            valued += 1050;
            break;

        case ARM_STORM_DRAGON_HIDE:
            valued += 900;
            break;

        case ARM_DRAGON_ARMOUR:
        case ARM_ICE_DRAGON_ARMOUR:
            valued += 750;
            break;

        case ARM_SWAMP_DRAGON_ARMOUR:
            valued += 650;
            break;

        case ARM_DRAGON_HIDE:
        case ARM_CRYSTAL_PLATE_MAIL:
        case ARM_TROLL_LEATHER_ARMOUR:
        case ARM_ICE_DRAGON_HIDE:
            valued += 500;
            break;

        case ARM_MOTTLED_DRAGON_ARMOUR:
        case ARM_SWAMP_DRAGON_HIDE:
            valued += 400;
            break;

        case ARM_STEAM_DRAGON_ARMOUR:
        case ARM_MOTTLED_DRAGON_HIDE:
            valued += 300;
            break;

        case ARM_PLATE_MAIL:
            valued += 230;
            break;

        case ARM_STEAM_DRAGON_HIDE:
            valued += 200;
            break;

        case ARM_BANDED_MAIL:
            valued += 150;
            break;

        case ARM_SPLINT_MAIL:
            valued += 140;
            break;

        case ARM_TROLL_HIDE:
            valued += 130;
            break;

        case ARM_CHAIN_MAIL:
            valued += 110;
            break;

        case ARM_SCALE_MAIL:
            valued += 83;
            break;

        case ARM_LARGE_SHIELD:
            valued += 75;
            break;

        case ARM_SHIELD:
            valued += 45;
            break;

        case ARM_RING_MAIL:
            valued += 40;
            break;

        case ARM_HELMET:
        case ARM_BUCKLER:
            valued += 25;
            break;

        case ARM_LEATHER_ARMOUR:
            valued += 20;
            break;

        case ARM_BOOTS:
            valued += 15;
            break;

        case ARM_GLOVES:
            valued += 12;
            break;

        case ARM_CLOAK:
            valued += 10;
            break;

        case ARM_ROBE:
            valued += 7;
            break;

        case ARM_ANIMAL_SKIN:
            valued += 3;
            break;


        }


        if (ident_lev > 1)
        {
            switch (item_da % 30)
            {
            case SPARM_NORMAL:
                break;

            case SPARM_ARCHMAGI:
                valued *= 100;
                break;

            case SPARM_DARKNESS:
            case SPARM_RESISTANCE:
                valued *= 60;
                break;

            case SPARM_POSITIVE_ENERGY:
                valued *= 50;
                break;

            case SPARM_MAGIC_RESISTANCE:
            case SPARM_PROTECTION:
            case SPARM_RUNNING:
                valued *= 40;
                break;

            case SPARM_COLD_RESISTANCE:
            case SPARM_DEXTERITY:
            case SPARM_FIRE_RESISTANCE:
            case SPARM_SEE_INVISIBLE:
            case SPARM_INTELLIGENCE:
            case SPARM_LEVITATION:
            case SPARM_PRESERVATION:
            case SPARM_STEALTH:
            case SPARM_STRENGTH:
                valued *= 30;
                break;

            case SPARM_POISON_RESISTANCE:
                valued *= 20;
                break;

            case SPARM_PONDEROUSNESS:
                valued *= 5;
                break;

            default:            /* assume a randart */
                valued *= 70;
                break;
            }
            if (item_da % 30 != 0)
                valued /= 10;
        }

        if (item_da / 30 == DARM_ELVEN || item_da / 30 == DARM_DWARVEN)
        {
            valued *= 12;
            valued /= 10;
        }

        if (item_da / 30 == DARM_ORCISH)
        {
            valued *= 8;
            valued /= 10;
        }

        if (ident_lev > 1)
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

        if (ident_lev != 0 && it_plus >= 130)
        {
            valued *= 6;
            valued /= 10;
        }

        break;


    case OBJ_WANDS:
        charge_value = 0;

        if ( id[0][item_typ] )
        {
            switch (item_typ)
            {
            case WAND_FIREBALL:
            case WAND_LIGHTNING:
                valued += 20;
                charge_value += 5;
                break;

            case WAND_DRAINING:
                valued += 20;
                charge_value += 4;
                break;

            case WAND_DISINTEGRATION:
                valued += 17;
                charge_value += 4;
                break;

            case WAND_POLYMORPH_OTHER:
                valued += 15;
                charge_value += 4;
                break;

            case WAND_COLD:
            case WAND_ENSLAVEMENT:
            case WAND_FIRE:
            case WAND_HASTING:
                valued += 15;
                charge_value += 3;
                break;

            case WAND_INVISIBILITY:
                valued += 15;
                charge_value += 2;
                break;

            case WAND_RANDOM_EFFECTS:
                valued += 13;
                charge_value += 3;
                break;

            case WAND_PARALYSIS:
                valued += 12;
                charge_value += 3;
                break;

            case WAND_SLOWING:
                valued += 10;
                charge_value += 3;
                break;

            case WAND_CONFUSION:
            case WAND_DIGGING:
            case WAND_TELEPORTATION:
                valued += 10;
                charge_value += 2;
                break;

            case WAND_HEALING:
                valued += 7;
                charge_value += 3;
                break;

            case WAND_FLAME:
            case WAND_FROST:
                valued += 5;
                charge_value += 2;
                break;

            case WAND_MAGIC_DARTS:
                valued += 3;
                charge_value++;
                break;

            default:    // no default charge_value ??? 15jan2000 {dlb}
                valued += 10;
                break;
            }

            if (ident_lev > 1)
                valued += it_plus * charge_value;

            valued *= 3;

            if (it_plus == 0)
                valued = 3;     // change if wands are rechargeable!

        }

        else
            valued = 35;        // = 10;

        break;


    case OBJ_POTIONS:
        if ( !id[3][item_typ] )
          valued += 9;
        else
          switch ( item_typ )
          {
            case POT_EXPERIENCE:
              valued += 500;
              break;
            case POT_GAIN_DEXTERITY:
            case POT_GAIN_INTELLIGENCE:
            case POT_GAIN_STRENGTH:
              valued += 350;
              break;
            case POT_CURE_MUTATION:
              valued += 150;
              break;
            case POT_MAGIC:
              valued += 120;
              break;
            case POT_INVISIBILITY:
              valued += 55;
              break;
            case POT_MUTATION:
            case POT_RESTORE_ABILITIES:
              valued += 50;
              break;
            case POT_BERSERK_RAGE:
            case POT_HEAL_WOUNDS:
              valued += 30;
              break;
            case POT_MIGHT:
            case POT_SPEED:
              valued += 25;
              break;
            case POT_HEALING:
            case POT_LEVITATION:
              valued += 20;
              break;
            case POT_PORRIDGE:
              valued += 10;
              break;
            case POT_CONFUSION:
            case POT_DECAY:
            case POT_DEGENERATION:
            case POT_PARALYSIS:
            case POT_POISON:
            case POT_SLOWING:
            case POT_STRONG_POISON:
            case POT_WATER:
              valued++;
              break;
          }
        break;

      case OBJ_FOOD:
        switch ( item_typ )
        {
          case FOOD_ROYAL_JELLY:
            valued = 280;
            break;
          case FOOD_HONEYCOMB:
            valued = 112;
            break;
          case FOOD_MEAT_RATION:
          case FOOD_BREAD_RATION:
            valued = 80;
            break;
          case FOOD_BEEF_JERKY:
            valued = 56;
            break;
          case FOOD_CHEESE:
            valued = 48;
            break;
          case FOOD_CHOKO:
          case FOOD_PIZZA:
            valued = 40;
            break;
          case FOOD_LEMON:
          case FOOD_ORANGE:
          case FOOD_SAUSAGE:
            valued = 32;
            break;
          case FOOD_APPLE:
          case FOOD_APRICOT:
          case FOOD_PEAR:
          case FOOD_BANANA:
            valued = 24;
            break;
          case FOOD_LYCHEE:
            valued = 16;
            break;
          case FOOD_SNOZZCUMBER:
          case FOOD_STRAWBERRY:
          case FOOD_RAMBUTAN:
          case FOOD_GRAPE:
          case FOOD_SULTANA:
          case FOOD_CHUNK:
            valued = 8;
            break;
        }
        break;

      case OBJ_SCROLLS:
        if ( !id[1][item_typ] )
          valued += 10;
        else
          switch ( item_typ )
          {
            case SCR_ACQUIREMENT:
              valued += 520;
              break;
            case SCR_ENCHANT_WEAPON_III:
            case SCR_VORPALISE_WEAPON:
              valued += 200;
              break;
            case SCR_SUMMONING:
              valued += 95;
              break;
            case SCR_TORMENT:
              valued += 75;
              break;
            case SCR_ENCHANT_WEAPON_II:
              valued += 55;
              break;
            case SCR_RECHARGING:
              valued += 50;
              break;
            case SCR_ENCHANT_ARMOUR:
            case SCR_ENCHANT_WEAPON_I:
              valued += 48;
              break;
            case SCR_FEAR:
              valued += 45;
              break;
            case SCR_MAGIC_MAPPING:
              valued += 35;
              break;
            case SCR_BLINKING:
            case SCR_REMOVE_CURSE:
            case SCR_TELEPORTATION:
              valued += 30;
              break;
            case SCR_DETECT_CURSE:
            case SCR_IDENTIFY:
              valued += 20;
              break;
            case SCR_NOISE:
            case SCR_RANDOM_USELESSNESS:
              valued += 2;
              break;
            case SCR_CURSE_ARMOUR:
            case SCR_CURSE_WEAPON:
            case SCR_FORGETFULNESS:
            case SCR_PAPER:
            case SCR_IMMOLATION:
              valued++;
              break;
          }
        break;

      case OBJ_JEWELLERY:
        if ( !id[2][item_typ] )
          valued += 50;

        if ( ident_lev > 0 && it_plus >= 130 )
          valued -= 10;

        if (id[2][item_typ] > 0)
        {
            if (ident_lev > 1 && item_typ == RING_PROTECTION || item_typ == RING_STRENGTH || item_typ == RING_EVASION || item_typ == RING_DEXTERITY || item_typ == RING_INTELLIGENCE)
            {
                if (it_plus >= 50 && (it_plus <= 130 || it_plus >= 150))
                    valued += 10 * (it_plus % 50);
            }

            switch (item_typ)
            {
              case RING_INVISIBILITY:
                valued += 100;
                break;
              case RING_REGENERATION:
                valued += 75;
                break;
              case RING_FIRE:
              case RING_ICE:
                valued += 62;
                break;
              case RING_LIFE_PROTECTION:
                valued += 60;
                break;
              case RING_TELEPORT_CONTROL:
                valued += 42;
                break;
              case RING_MAGICAL_POWER:
              case RING_PROTECTION_FROM_MAGIC:
                valued += 40;
                break;
              case RING_WIZARDRY:
                valued += 35;
                break;
              case RING_LEVITATION:
              case RING_POISON_RESISTANCE:
              case RING_PROTECTION_FROM_COLD:
              case RING_PROTECTION_FROM_FIRE:
              case RING_SLAYING:
                valued += 30;
                break;
              case RING_SUSTAIN_ABILITIES:
              case RING_SUSTENANCE:
                valued += 25;
                break;
              case RING_SEE_INVISIBLE:
                valued += 20;
                break;
              case RING_DEXTERITY:
              case RING_EVASION:
              case RING_INTELLIGENCE:
              case RING_PROTECTION:
              case RING_STRENGTH:
                valued += 10;
                break;
              case RING_TELEPORTATION:
                valued -= 10;
                break;
              case RING_HUNGER:
                valued -= 50;
                break;
              case AMU_CLARITY:
              case AMU_RESIST_CORROSION:
              case AMU_RESIST_MUTATION:
              case AMU_RESIST_SLOW:
              case AMU_WARDING:
                valued += 30;
                break;
              case AMU_CONSERVATION:
              case AMU_CONTROLLED_FLIGHT:
                valued += 25;
                break;
              case AMU_RAGE:
                valued += 20;
                break;
              case AMU_THE_GOURMAND:
                valued += 15;
                break;
              case AMU_INACCURACY:
                valued -= 50;
                break;
              // got to do delusion!
            }

            if ( item_da == 200 )
              valued += 50;

            valued *= 7;
        }
        break;

      case OBJ_MISCELLANY:
        switch (ident_lev)
        {
          case 0:
            switch (item_typ)
            {
              case MISC_HORN_OF_GERYON:
              case MISC_RUNE_OF_ZOT:
                valued += 1000;
                break;
              case MISC_CRYSTAL_BALL_OF_SEEING:
                valued += 450;
                break;
              case MISC_BOTTLED_EFREET:
                valued += 350;
                break;
              case MISC_DECK_OF_TRICKS:
                valued += 100;
                break;
              default:
                valued += 400;
            }
            break;
          default:
            switch (item_typ)
            {
              case MISC_HORN_OF_GERYON:
                valued += 5000;
                break;
              case MISC_DISC_OF_STORMS:
                valued += 2000;
                break;
              case MISC_RUNE_OF_ZOT:
                valued += 1200;
                break;
              case MISC_CRYSTAL_BALL_OF_SEEING:
                valued += 500;
                break;
              case MISC_BOTTLED_EFREET:
                valued += 400;
                break;
              case MISC_CRYSTAL_BALL_OF_FIXATION:
              case MISC_EMPTY_EBONY_CASKET:
                valued += 20;
                break;
              default:
                valued += 500;
            }
            break;
        }
        break;

//case 10: break;

      case OBJ_BOOKS:
        valued = 150 + ( (ident_lev == 0) ? 0 : book_rarity(item_typ) * 50 );
        break;


      case OBJ_STAVES:
        if ( ident_lev == 0 )
          valued = 120;
        else if (item_typ == STAFF_SMITING || item_typ == STAFF_WARDING || item_typ == STAFF_DISCOVERY)
          valued = 150;
        else
          valued = 250;
        break;


      case OBJ_ORBS:
        valued = 10000;
        break;
    }                           // end switch

    if (valued < 1)
      valued = 1;

    valued *= item_quant;

    return valued;

}          // end item_value()





void shop( void )
{

    unsigned char i = 0;

    for (i = 0; i < 6; i++)
    {
        if ( i == 5 )
        {
            mpr("Help! Non-existent shop.");
            return;
        }

        if ( env.sh_x[i] == you.x_pos && env.sh_y[i] == you.y_pos )
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

}          // end shop()

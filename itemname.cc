#include "config.h"

#ifdef DOS
#include <conio.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "externs.h"

#include "itemname.h"
#include "macro.h"
#include "mstruct.h"
#include "randart.h"
#include "skills2.h"

char item_name_2(unsigned char item_plus2, char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char glog [60]);

char reduce(unsigned char reducee);
char is_a_vowel(unsigned char let);
char retbit(char sed);
char retvow(char sed);
void make_name(unsigned char var1, unsigned char var2, unsigned char var3, char ncase, char str_pass [50]);


char id [4] [50];

int prop [4] [50] [3];
int mss [20] [50];



void it_name(int itn, char des, char str_pass [80])
{
                item_name(mitm.iplus2 [itn], mitm.iclass [itn], mitm.itype [itn], mitm.idam [itn], mitm.iplus [itn], mitm.iquant [itn], mitm.iid [itn], des, str_pass);
}

void in_name(int inn, char des, char str_pass [80])
{
                item_name(you[0].inv_plus2 [inn], you[0].inv_class [inn], you[0].inv_type [inn], you[0].inv_dam [inn], you[0].inv_plus [inn], you[0].inv_quant [inn], you[0].inv_ident [inn], des, str_pass);
}

char item_name(unsigned char item_plus2, char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char descrip, char glag [60])
{

char itm_name [60] = "";
char tmp_quant [5];

strcpy(glag, "");

item_name_2(item_plus2, item_clas, item_typ, item_da, it_plus, it_quant, ident_lev, itm_name);


if (item_clas == 12 || (item_clas == 0 && item_da > 180 && ident_lev > 0) || (item_clas == 13 && item_typ == 6 && ident_lev > 0) || (item_clas == 7 && item_da == 200 && ident_lev >= 2) || ((item_clas == 0 || item_clas == 2) && item_da % 30 >= 25 && ident_lev > 0))
{
 switch(descrip)
 {
  case 2:
  case 4:
        case 0: strcat(glag, "The "); break;
  case 3:
  case 5:
  case 7: // !
        case 1: strcat(glag, "the "); break;
 }
} else
if (it_quant > 1)
{
switch(descrip)
{
        case 0: strcat(glag, "The "); break;
        case 1: strcat(glag, "the "); break;
        case 2: break; // A/An
        case 3: break; // a/an
        case 4: strcat(glag, "Your "); break;
        case 5: strcat(glag, "your "); break;
/*      case 6: nothing */
        case 7: strcat(glag, "its "); break;
}

        itoa(it_quant, tmp_quant, 10);
        strcat(glag, tmp_quant);
        strcat(glag, " ");
        } else
{
        switch(descrip)
        {
                case 0: strcat(glag, "The "); break;
                case 1: strcat(glag, "the "); break;
                case 2: strcat(glag, "A");
                        if (itm_name [0] == 97 ^ itm_name [0] == 101 ^ itm_name [0] == 105 ^ itm_name [0] == 111 ^ itm_name [0] == 117)
                        {
                                strcat(glag, "n");
                        }
                strcat(glag, " ");
                break; // A/An
                case 3:strcat(glag, "a");
                        if (itm_name [0] == 97 ^ itm_name [0] == 101 ^ itm_name [0] == 105 ^ itm_name [0] == 111 ^ itm_name [0] == 117)
                        {
                                strcat(glag, "n");
                        }
                strcat(glag, " ");
                break; // a/an
                case 4: strcat(glag, "Your "); break;
                case 5: strcat(glag, "your "); break;
/*              case 6: nothing */
                case 7: strcat(glag, "its "); break;
        }

} // end of else

strcat(glag, itm_name);

return 1;

} // end of char name_item



char item_name_2(unsigned char item_plus2, char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char glog [60])
{

char tmp_quant [5];
char str_pass2 [80];

strcpy(glog, "");

switch(item_clas)
        {

        case 0: // weapons
        if (ident_lev > 0)
        {
         if (it_plus >= 130)
         {
                strcat(glog, "cursed ");
         }
        }


        if (ident_lev > 2)
        {

        if (it_plus % 100 == 50 && item_plus2 == 50)
        {
         strcat(glog, "+0 ");
        }
        else
        {
         if (it_plus >= 50 && (it_plus <= 130 || it_plus >= 150)) strcat(glog, "+");
         itoa(it_plus - 50, tmp_quant, 10);
         if (it_plus > 130) itoa((it_plus - 150), tmp_quant, 10);
         strcat(glog, tmp_quant);
         strcat(glog, ",");
         if (item_plus2 >= 50) strcat(glog, "+");
         itoa(item_plus2 - 50, tmp_quant, 10);
         strcat(glog, tmp_quant);
         strcat(glog, " ");
        }

        }

 if (item_da % 30 >= 25 && item_da % 30 <= 29)
 {
  strcat(glog, randart_name(item_clas, item_typ, item_da, it_plus, item_plus2, ident_lev));
  break;
 }

 if (item_da > 180)
 {

  if (ident_lev > 0)
  {
   switch(item_da - 180)
   {
        case 1: strcat(glog , "Singing Sword"); break;
        case 2: strcat(glog , "Wrath of Trog"); break;
        case 3: strcat(glog , "Scythe of Curses"); break;
        case 4: strcat(glog , "Mace of Variability"); break;
        case 5: strcat(glog , "Glaive of Prune"); break;
        case 6: strcat(glog , "Sceptre of Torment"); break;
        case 7: strcat(glog , "Sword of Zonguldrok"); break;
        case 8: strcat(glog , "Sword of Cerebov"); break;
        case 9: strcat(glog , "Staff of Dispater"); break;
        case 10: strcat(glog , "Sceptre of Asmodeus"); break;
        case 11: strcat(glog , "Sword of Power"); break;
        case 12: strcat(glog , "Knife of Accuracy"); break;
        case 13: strcat(glog , "Staff of Olgreb"); break;
        case 14: strcat(glog , "Vampire's Tooth"); break;
        case 15: strcat(glog , "Staff of Wucad Mu"); break; // this random name generator makes some weird things

   }
  break;
  } // end if ident_lev
   switch(item_da - 180)
   {
        case 1: strcat(glog , "golden long sword"); break;
        case 2: strcat(glog , "bloodstained battleaxe"); break;
        case 3: strcat(glog , "warped scythe"); break;
        case 4: strcat(glog , "shimmering mace"); break;
        case 5: strcat(glog , "purple glaive"); break;
        case 6: strcat(glog , "jewelled golden mace"); break;
        case 7: strcat(glog , "bone long sword"); break;
        case 8: strcat(glog , "great serpentine sword"); break;
        case 9: strcat(glog , "golden staff"); break;
        case 10: strcat(glog , "ruby sceptre"); break;
        case 11: strcat(glog , "chunky great sword"); break;
        case 12: strcat(glog , "thin dagger"); break;
        case 13: strcat(glog , "green glowing staff"); break;
        case 14: strcat(glog , "ivory dagger"); break;
        case 15: strcat(glog , "quarterstaff"); break;

   }
 break;
 } // end uniques


if (ident_lev <= 1 || item_da % 30 == 0 || item_da / 30 >= 3)
{
switch(item_da / 30)
{
   case 1: strcat(glog, "runed "); break;
   case 2: strcat(glog, "glowing "); break;
   case 3: strcat(glog, "orcish "); break;
   case 4: strcat(glog, "elven "); break;
   case 5: strcat(glog, "dwarven "); break;
} // end switch
} // end if

if (ident_lev > 1)
{
switch(item_da % 30)
{
 case 13: strcat(glog, "vampiric "); break;
} // end switch
} // end if

standard_name_weap(item_typ, str_pass2); /* in randart.cc */
strcat(glog, str_pass2);

if (ident_lev > 1)
{
 switch(item_da % 30)
 {
  case 0: break;
  case 1: strcat(glog, " of flaming"); break;
  case 2: strcat(glog, " of freezing"); break;
  case 3: strcat(glog, " of holy wrath"); break;
  case 4: strcat(glog, " of electrocution"); break;
  case 5: strcat(glog, " of orc slaying"); break;
  case 6: strcat(glog, " of venom"); break;
  case 7: strcat(glog, " of protection"); break;
  case 8: strcat(glog, " of draining"); break;
  case 9: strcat(glog, " of speed"); break;
  case 10:
  switch(damage_type(item_clas, item_typ))
  {
   case 0: strcat(glog, " of crushing"); break;
   case 1: strcat(glog, " of slicing"); break;
   case 2: strcat(glog, " of piercing"); break;
   case 3: strcat(glog, " of chopping"); break;
  }
  break;
  case 11: strcat(glog, " of flame"); break; // bows/xbows
  case 12: strcat(glog, " of frost"); break; // bows/xbows
  /* 13 - vamp */
  case 14: strcat(glog, " of disruption"); break;
  case 15: strcat(glog, " of pain"); break;
  case 16: strcat(glog, " of distortion"); break;

  /* 25 - 29 are randarts */
 }
}

// Hellbrand; zap to reduce plus by 1 and cast hellfire?

 break;


        case 1: // ammunition
if (item_da % 30 == 3 || item_da % 30 == 4)
{
 strcat(glog, "poisoned ");
}
        if (ident_lev > 2)
        {
                unsigned char gokh = it_plus;
                //it_plus -= 50;
                if (gokh >= 50 && (gokh <= 130 || gokh >= 150)) strcat(glog, "+");
                itoa(gokh - 50, tmp_quant, 10);
                if (gokh > 130) itoa((gokh - 150), tmp_quant, 10);
                strcat(glog, tmp_quant);
                strcat(glog, " ");
        }

        if (item_da != 0)
        {
         switch(item_da / 30)
         {
           case 3: strcat(glog, "orcish "); break;
           case 4: strcat(glog, "elven "); break;
           case 5: strcat(glog, "dwarven "); break;
         }
        }

        switch(item_typ)
        {
        case 0: strcat(glog , "stone"); break;
        case 1: strcat(glog , "arrow"); break;
        case 2: strcat(glog , "bolt"); break;
        case 3: strcat(glog , "dart"); break;
        case 4: strcat(glog , "eggplant"); break;
        case 5: strcat(glog , "large rock"); break;
        case 6: strcat(glog , ""); break;
        case 7: strcat(glog , ""); break;
        case 8: strcat(glog , ""); break;
        case 9: strcat(glog , ""); break;
        case 10: strcat(glog , ""); break;
        case 12: strcat(glog , ""); break;
        case 13: strcat(glog , ""); break;
        case 14: strcat(glog , ""); break;
        case 15: strcat(glog , ""); break;
        case 16: strcat(glog , ""); break;
        }
   if (it_quant > 1) strcat(glog, "s");

if (ident_lev > 1)
{
 switch(item_da % 30)
 {
  case 0: break;
  case 1: strcat(glog, " of flame"); break;
  case 2: strcat(glog, " of ice"); break;
//  case 3: strcat(glog, " of venom"); break;
//  case 4: strcat(glog, " of venom"); break; /* temporary, produced by spell */
 }
}
 break;

        case 2: // armour


        if (ident_lev > 0)
        {
                if (it_plus >= 130)
                {
                        strcat(glog, "cursed ");
                }
        }




        if (ident_lev > 2)
        {
                if (it_plus >= 50 && (it_plus <= 130 || it_plus >= 150)) strcat(glog, "+");
                itoa(it_plus - 50, tmp_quant, 10);
                if (it_plus > 130) itoa((it_plus - 150), tmp_quant, 10);
                strcat(glog, tmp_quant);
                strcat(glog, " ");
        }



if (item_typ == 11 || (item_typ == 12 && item_plus2 == 0)) strcat(glog, "pair of ");

if (item_da % 30 >= 25 && item_da % 30 <= 29)
{
 strcat(glog, randart_armour_name(item_clas, item_typ, item_da, it_plus, item_plus2, ident_lev));
 break;
}

if (ident_lev < 2 || item_da % 30 == 0 || item_da / 30 >= 4)
{
if (item_typ != 10)
{
 switch((int) item_da / 30)
 {
   case 1:
   if (item_typ == 0 || item_typ == 9 || item_typ == 11 || item_typ == 12 || (item_typ == 10 && item_plus2 >= 2))
   strcat(glog, "embroidered "); else
                if (item_typ == 1) strcat(glog, ""); else
                   strcat(glog, "shiny "); break;
   case 2: strcat(glog, "runed "); break;
   case 3: strcat(glog, "glowing "); break;
   case 4: strcat(glog, "elven "); break;
   case 5: strcat(glog, "dwarven "); break;
   case 6: strcat(glog, "orcish "); break;
 } // end switch
}
if (item_typ == 10 && item_plus2 <= 1)
{
switch(item_da / 30)
{
   case 1: strcat(glog, "winged "); break;
   case 2: strcat(glog, "horned "); break;
   case 3: strcat(glog, "crested "); break;
   case 4: strcat(glog, "plumed "); break;
   case 5: strcat(glog, "spiked "); break;
   case 6: strcat(glog, "visored "); break;
   case 7: strcat(glog, "jewelled "); break;
   case 13: strcat(glog, " "); break;
   case 14: strcat(glog, " "); break;
   case 15: strcat(glog, " "); break;
}
}

} // end if

standard_name_armour(item_typ, item_plus2, str_pass2); /* in randart.cc */
strcat(glog, str_pass2);



 if (ident_lev > 1)
 {
  switch(item_da % 30)
  {
   case 1: strcat(glog, " of running"); break;
   case 2: strcat(glog, " of fire resistance"); break;
   case 3: strcat(glog, " of cold resistance"); break;
   case 4: strcat(glog, " of poison resistance"); break;
   case 5: strcat(glog, " of see invisible"); break;
   case 6: strcat(glog, " of darkness"); break;
   case 7: strcat(glog, " of strength"); break;
   case 8: strcat(glog, " of dexterity"); break;
   case 9: strcat(glog, " of intelligence"); break;
   case 10: strcat(glog, " of ponderousness"); break;
   case 11: strcat(glog, " of levitation"); break;
   case 12: strcat(glog, " of magic resistance"); break;
   case 13: strcat(glog, " of protection"); break;
   case 14: strcat(glog, " of stealth"); break;
   case 15: strcat(glog, " of resistance"); break;
   case 16: strcat(glog, " of positive energy"); break;
   case 17: strcat(glog, " of the Archmagi"); break;
   case 18: strcat(glog, " of preservation"); break;
  }
 }

 break;


        case 3: // devices
        if (id [0] [item_typ] == 1)
        {
                switch(item_typ)
                {
                case 0: strcat(glog, "wand of flame"); break;
                case 1: strcat(glog, "wand of frost"); break;
                case 2: strcat(glog, "wand of slowing"); break;
                case 3: strcat(glog, "wand of hasting"); break;
                case 4: strcat(glog, "wand of magic darts"); break;
                case 5: strcat(glog, "wand of healing"); break;
                case 6: strcat(glog, "wand of paralysis"); break;
                case 7: strcat(glog, "wand of fire"); break;
                case 8: strcat(glog, "wand of cold"); break;
                case 9: strcat(glog, "wand of confusion"); break;
                case 10: strcat(glog, "wand of invisibility"); break;
                case 11: strcat(glog, "wand of digging"); break;
        case 12: strcat(glog, "wand of fireball"); break;
        case 13: strcat(glog, "wand of teleportation"); break;
                case 14: strcat(glog, "wand of lightning"); break;
        case 15: strcat(glog, "wand of polymorph other"); break;
        case 16: strcat(glog, "wand of enslavement"); break;
        case 17: strcat(glog, "wand of draining"); break;
        case 18: strcat(glog, "wand of random effects"); break;
        case 19: strcat(glog, "wand of disintegration"); break;
      }
        } else
        {
        if (item_da <= 11)
        {
        switch(item_da )
        {
        case 0: strcat(glog , "iron wand"); break;
        case 1: strcat(glog , "brass wand"); break;
        case 2: strcat(glog , "bone wand"); break;
        case 3: strcat(glog , "wooden wand"); break;
        case 4: strcat(glog , "copper wand"); break;
        case 5: strcat(glog , "gold wand"); break;
        case 6: strcat(glog , "silver wand"); break;
        case 7: strcat(glog , "bronze wand"); break;
        case 8: strcat(glog , "ivory wand"); break;
        case 9: strcat(glog , "glass wand"); break;
        case 10: strcat(glog , "lead wand"); break;
        case 11: strcat(glog , "plastic wand"); break;
        } // end of switch(randnum)

        } else //ie if it's uncommon:
        {
        switch(item_da / 12) // secondary characteristic of wand
        {
         case 1: strcat(glog , "jewelled "); break;
         case 2: strcat(glog , "curved "); break;
         case 3: strcat(glog , "long "); break;
         case 4: strcat(glog , "short "); break;
         case 5: strcat(glog , "twisted "); break;
         case 6: strcat(glog , "crooked "); break;
         case 7: strcat(glog , "forked "); break;
         case 8: strcat(glog , "shiny "); break;
         case 9: strcat(glog , "blackened "); break;
         case 10: strcat(glog , "tapered "); break;
         case 11: strcat(glog , "glowing "); break;
         case 12: strcat(glog , "worn "); break;
         case 13: strcat(glog , "encrusted "); break;
         case 14: strcat(glog , "runed "); break;
         case 15: strcat(glog , "sharpened "); break;
        } // end of switch(randnum)

        switch((int) item_da % 12)
        {
         case 0: strcat(glog , "iron wand"); break;
         case 1: strcat(glog , "brass wand"); break;
         case 2: strcat(glog , "bone wand"); break;
         case 3: strcat(glog , "wooden wand"); break;
         case 4: strcat(glog , "copper wand"); break;
         case 5: strcat(glog , "gold wand"); break;
         case 6: strcat(glog , "silver wand"); break;
         case 7: strcat(glog , "bronze wand"); break;
         case 8: strcat(glog , "ivory wand"); break;
         case 9: strcat(glog , "glass wand"); break;
         case 10: strcat(glog , "lead wand"); break;
         case 11: strcat(glog , "plastic wand"); break;
        } // end of switch(randnum)

        } // end of if uncommon wand

        } // end of else for ident_lev

        if (ident_lev > 1)
        {
                strcat(glog, " (");
                itoa(it_plus, tmp_quant, 10);
                if (it_plus > 80) itoa((it_plus - 100), tmp_quant, 10);
                strcat(glog, tmp_quant);
                strcat(glog, ")");
        }
        break;

        case 8: // potions

        if (id [3] [item_typ] == 1)
        {
                if (it_quant == 1)
                {
                        strcat(glog, "potion of ");
                } else
                {
                        strcat(glog, "potions of ");
                }
        }

/*
Remember, potions food and scrolls stack on the basis of class and type only.
*/
        if (id [3] [item_typ] == 1)
        {
                switch(item_typ)
                {
                case 0: strcat(glog , "healing"); break;
                case 1: strcat(glog , "heal wounds"); break;
                case 2: strcat(glog , "speed"); break;
                case 3: strcat(glog , "might"); break;
                case 4: strcat(glog , "gain strength"); break;
                case 5: strcat(glog , "gain dexterity"); break;
                case 6: strcat(glog , "gain intelligence"); break;
                case 7: strcat(glog , "levitation"); break;
                case 8: strcat(glog , "poison"); break;
                case 9: strcat(glog , "slowing"); break;
                case 10: strcat(glog , "paralysis"); break;
                case 11: strcat(glog , "confusion"); break;
                case 12: strcat(glog , "invisibility"); break;
                case 13: strcat(glog , "porridge"); break;
                case 14: strcat(glog , "degeneration"); break;
                case 15: strcat(glog , "decay"); break; // Rare!
            case 16: strcat(glog , "water"); break;
            case 17: strcat(glog , "experience"); break;
            case 18: strcat(glog , "magic"); break;
            case 19: strcat(glog , "restore abilities"); break;
                case 20: strcat(glog , "strong poison"); break;
        case 21: strcat(glog , "berserk rage"); break;
        case 22: strcat(glog , "cure mutation"); break;
        case 23: strcat(glog , "mutation"); break;
   }
        break;
        } // end of if ident_lev

 if (item_da <= 13)
        {
        switch(item_da )
        {
                case 0: strcat(glog , "clear potion"); break;
                case 1: strcat(glog , "blue potion"); break;
                case 2: strcat(glog , "black potion"); break;
                case 3: strcat(glog , "silvery potion"); break;
                case 4: strcat(glog , "cyan potion"); break;
                case 5: strcat(glog , "purple potion"); break;
                case 6: strcat(glog , "orange potion"); break;
                case 7: strcat(glog , "inky potion"); break;
                case 8: strcat(glog , "red potion"); break;
                case 9: strcat(glog , "yellow potion"); break;
                case 10: strcat(glog , "green potion"); break;
                case 11: strcat(glog , "brown potion"); break;
                case 12: strcat(glog , "pink potion"); break;
                case 13: strcat(glog , "white potion"); break;
        } // end of switch(randnum)

        } else
        {
        switch(item_da / 14) // secondary characteristic of potion
        {
                case 0: strcat(glog , "bubbling "); break;
                case 1: strcat(glog , "lumpy "); break;
                case 2: strcat(glog , "fuming "); break;
                case 3: strcat(glog , "smoky "); break;
                case 4: strcat(glog , "fizzy "); break;
                case 5: strcat(glog , "glowing "); break;
                case 6: strcat(glog , "sedimented "); break;
                case 7: strcat(glog , "metallic "); break;
                case 8: strcat(glog , "murky "); break;
                case 9: strcat(glog , "gluggy "); break;
                case 10: strcat(glog , "viscous "); break;
                case 11: strcat(glog , "oily "); break;
                case 12: strcat(glog , "slimy "); break;
        } // end of switch(randnum)

        switch(item_da % 14)
        {
                case 0: strcat(glog , "clear potion"); break;
                case 1: strcat(glog , "blue potion"); break;
                case 2: strcat(glog , "black potion"); break;
                case 3: strcat(glog , "silvery potion"); break;
                case 4: strcat(glog , "cyan potion"); break;
                case 5: strcat(glog , "purple potion"); break;
                case 6: strcat(glog , "orange potion"); break;
                case 7: strcat(glog , "inky potion"); break;
                case 8: strcat(glog , "red potion"); break;
                case 9: strcat(glog , "yellow potion"); break;
                case 10: strcat(glog , "green potion"); break;
                case 11: strcat(glog , "brown potion"); break;
                case 12: strcat(glog , "pink potion"); break;
                case 13: strcat(glog , "white potion"); break;
        } // end of switch(randnum)
        } // end of else.
   if (it_quant > 1) strcat(glog, "s");
        break;

        case 4: // food
        switch(item_typ )
        {
        case 0: strcat(glog , "meat ration"); break;
        case 1: strcat(glog , "bread ration"); break;
        case 2: strcat(glog , "pear"); break;
        case 3: strcat(glog , "apple"); break; // make this less common.
        case 4: strcat(glog , "choko"); break;
        case 5: strcat(glog , "honeycomb"); break;
        case 6: strcat(glog , "royal jell"); break; // maybe a joke monster of the same name? - mix something with jelly crystals?
        case 7: strcat(glog , "snozzcumber"); break;
    case 8: strcat(glog , "slice of pizza"); break;
    case 9: strcat(glog , "apricot"); break;
    case 10: strcat(glog , "orange"); break;
    case 11: strcat(glog , "banana"); break;
    case 12: strcat(glog , "strawberr"); break;
    case 13: strcat(glog , "rambutan"); break;
    case 14: strcat(glog , "lemon"); break;
    case 15: strcat(glog , "grape"); break;
    case 16: strcat(glog , "sultana"); break;
    case 17: strcat(glog , "lychee"); break;
    case 18: strcat(glog , "beef jerk"); break;
    case 19: strcat(glog , "cheese"); break;
    case 20: strcat(glog , "sausage"); break;
 case 21:
 char gmo_n2 [40];
 moname(it_plus, 0, 1, 100, gmo_n2);
 if (item_da < 100) strcat(glog, "rotting ");
 strcat(glog, "chunk");
 if (it_quant > 1) strcat(glog, "s");
 strcat(glog, " of ");
 strcat(glog, gmo_n2);
 strcat(glog, " flesh");
 break;

// Note: If I add another food type, must set for carnivorous chars (Kobolds and mutants)
        }

 if (item_typ == 6 || item_typ == 12 || item_typ == 18)
        {
  if (it_quant > 1)
  {
   strcat(glog, "ie");
  } else strcat(glog, "y");
        }
 break;

        case 6: // scrolls
        if (it_quant == 1)
        {
                strcat(glog, "scroll ");
        } else
        {
                strcat(glog, "scrolls ");
        }

        if (id [1] [item_typ] == 1)
        {

        switch(item_typ)
        {
        case 0: strcat(glog , "of identify"); break;
        case 1: strcat(glog , "of teleportation"); break;
        case 2: strcat(glog , "of fear"); break;
        case 3: strcat(glog , "of noise"); break;
        case 4: strcat(glog , "of remove curse"); break;
        case 5: strcat(glog , "of detect curse"); break;
        case 6: strcat(glog , "of summoning"); break;
        case 7: strcat(glog , "of enchant weapon I"); break;
        case 8: strcat(glog , "of enchant armour"); break;
        case 9: strcat(glog , "of torment"); break;
        case 10: strcat(glog , "of random uselessness"); break;
        case 11: strcat(glog , "of curse weapon"); break;
        case 12: strcat(glog , "of curse armour"); break;
        case 13: strcat(glog , "of immolation"); break;
        case 14: strcat(glog , "of blinking"); break;
        case 15: strcat(glog , "of paper"); break;
        case 16: strcat(glog , "of magic mapping"); break;
        case 17: strcat(glog , "of forgetfulness"); break;
        case 18: strcat(glog , "of acquirement"); break;
    case 19: strcat(glog , "of enchant weapon II"); break;
        case 20: strcat(glog , "of vorpalise weapon"); break;
    case 21: strcat(glog , "of recharging"); break;
    case 22: strcat(glog , "of enchant weapon III"); break;
//        case 23: strcat(glog , "of portal travel"); break;
        } break;

        }

        strcat(glog, "labelled ");

        char str_pass [50];
        make_name(item_da, it_plus, item_clas, 2, str_pass);
        strcat(glog, str_pass);

        break; // end of scrolls


        case 7: // jewellery

   if (ident_lev > 0)
   {
      if (it_plus >= 80)
      {
         strcat(glog, "cursed ");
      }
   }

   if (ident_lev > 2)
   {
      if (item_da == 200)
      {
       strcat(glog, randart_ring_name(item_clas, item_typ, item_da, it_plus, item_plus2, ident_lev));
       break;
      }
   }

   if (id [2] [item_typ] > 0)
        {

        if (ident_lev > 1 && item_typ == 1 || item_typ == 5 || item_typ == 6 || item_typ == 11 || item_typ == 14 || item_typ == 15)
        {
                char gokh = it_plus - 50;
                if (gokh >= 0 && (gokh <= 80 || gokh >= 100)) strcat(glog, "+");
                itoa(gokh, tmp_quant, 10);
                if (gokh > 80) itoa((gokh - 100), tmp_quant, 10);
                strcat(glog, tmp_quant);
        if (item_typ == 6)
        {
             strcat(glog, ",");
             if (item_plus2 >= 50) strcat(glog, "+");
             itoa(item_plus2 - 50, tmp_quant, 10);
         strcat(glog, tmp_quant);
        }
        strcat(glog, " ");
        }

        switch(item_typ)
        {
        case 0: strcat(glog , "ring of regeneration"); break;
        case 1: strcat(glog , "ring of protection"); break;
        case 2: strcat(glog , "ring of protection from fire"); break;
        case 3: strcat(glog , "ring of poison resistance"); break;
        case 4: strcat(glog , "ring of protection from cold"); break;
        case 5: strcat(glog , "ring of strength"); break;
        case 6: strcat(glog , "ring of slaying"); break;
        case 7: strcat(glog , "ring of see invisible"); break;
        case 8: strcat(glog , "ring of invisibility"); break;
        case 9: strcat(glog , "ring of hunger"); break;
        case 10: strcat(glog , "ring of teleportation"); break;
        case 11: strcat(glog , "ring of evasion"); break;
    case 12: strcat(glog , "ring of sustain abilities"); break;
    case 13: strcat(glog , "ring of sustenance"); break;
    case 14: strcat(glog , "ring of dexterity"); break;
    case 15: strcat(glog , "ring of intelligence"); break;
    case 16: strcat(glog , "ring of wizardry"); break;
    case 17: strcat(glog , "ring of magical power"); break;
    case 18: strcat(glog , "ring of levitation"); break;
    case 19: strcat(glog , "ring of life protection"); break;
    case 20: strcat(glog , "ring of protection from magic"); break;
    case 21: strcat(glog , "ring of fire"); break;
    case 22: strcat(glog , "ring of ice"); break;
    case 23: strcat(glog , "ring of teleport control"); break;

    case 35: strcat(glog , "amulet of rage"); break;
    case 36: strcat(glog , "amulet of maintain speed"); break;
    case 37: strcat(glog , "amulet of clarity"); break;
    case 38: strcat(glog , "amulet of warding"); break;
    case 39: strcat(glog , "amulet of resist corrosion"); break;
    case 40: strcat(glog , "amulet of the gourmand"); break;
    case 41: strcat(glog , "amulet of conservation"); break;
    case 42: strcat(glog , "amulet of controlled flight"); break;
    case 43: strcat(glog , "amulet of inaccuracy"); break;
    case 44: strcat(glog , "amulet of resist mutation"); break;
 }
/* ? of imputed learning - 100% exp from tames/summoned kills */
        break;
        } // end of if ident_lev

       if (item_typ < 35) // rings
       {
       if (item_da == 200)
       {
        strcat(glog, randart_ring_name(item_clas, item_typ, item_da, it_plus, item_plus2, ident_lev));
        break;
       }
        switch(item_da / 13) // secondary characteristic of ring
        {
                case 1: strcat(glog, "encrusted "); break;
                case 2: strcat(glog, "glowing "); break;
                case 3: strcat(glog, "tubular "); break;
                case 4: strcat(glog, "runed "); break;
                case 5: strcat(glog, "blackened "); break;
                case 6: strcat(glog, "scratched "); break;
                case 7: strcat(glog, "small "); break;
                case 8: strcat(glog, "large "); break;
                case 9: strcat(glog, "twisted "); break;
                case 10: strcat(glog, "shiny "); break;
                case 11: strcat(glog, "notched "); break;
                case 12: strcat(glog, "knobbly "); break;
        } // end of switch(randnum)


        switch(item_da % 13)
        {
        case 0: strcat(glog, "wooden ring"); break;
        case 1: strcat(glog, "silver ring"); break;
        case 2: strcat(glog, "golden ring"); break;
        case 3: strcat(glog, "iron ring"); break;
        case 4: strcat(glog, "steel ring"); break;
        case 5: strcat(glog, "bronze ring"); break;
        case 6: strcat(glog, "brass ring"); break;
        case 7: strcat(glog, "copper ring"); break;
        case 8: strcat(glog, "granite ring"); break;
        case 9: strcat(glog, "ivory ring"); break;
        case 10: strcat(glog, "bone ring"); break;
        case 11: strcat(glog, "marble ring"); break;
        case 12: strcat(glog, "jade ring"); break;
        case 13: strcat(glog, "glass ring"); break;
        } // end of switch(randnum)
       } // end of rings
        else // ie is an amulet
       {
       if (item_da == 200)
       {
        strcat(glog, randart_ring_name(item_clas, item_typ, item_da, it_plus, item_plus2, ident_lev));
        break;
       }
        if (item_da > 13)
        switch(item_da / 13) // secondary characteristic of amulet
        {
                case 0: strcat(glog, "dented "); break;
                case 1: strcat(glog, "square "); break;
                case 2: strcat(glog, "thick "); break;
                case 3: strcat(glog, "thin "); break;
                case 4: strcat(glog, "runed "); break;
                case 5: strcat(glog, "blackened "); break;
                case 6: strcat(glog, "glowing "); break;
                case 7: strcat(glog, "small "); break;
                case 8: strcat(glog, "large "); break;
                case 9: strcat(glog, "twisted "); break;
                case 10: strcat(glog, "tiny "); break;
                case 11: strcat(glog, "triangular "); break;
                case 12: strcat(glog, "lumpy "); break;
        } // end of switch(randnum)


        switch(item_da % 13)
        {
        case 0: strcat(glog, "zirconium amulet"); break;
        case 1: strcat(glog, "sapphire amulet"); break;
        case 2: strcat(glog, "golden amulet"); break;
        case 3: strcat(glog, "emerald amulet"); break;
        case 4: strcat(glog, "garnet amulet"); break;
        case 5: strcat(glog, "bronze amulet"); break;
        case 6: strcat(glog, "brass amulet"); break;
        case 7: strcat(glog, "copper amulet"); break;
        case 8: strcat(glog, "ruby amulet"); break;
        case 9: strcat(glog, "ivory amulet"); break;
        case 10: strcat(glog, "bone amulet"); break;
        case 11: strcat(glog, "platinum amulet"); break;
        case 12: strcat(glog, "jade amulet"); break;
        case 13: strcat(glog, "plastic amulet"); break;
        } // end of switch(randnum)
       } // end of amulets
        break;

case 13: // Miscellaneous
switch(ident_lev)
{
case 0:
 switch(item_typ)
 {
 case 0: strcat(glog, "sealed bronze flask"); break;
 case 1: strcat(glog, "crystal ball"); break;
 case 2: strcat(glog, "fan"); break;
 case 3: strcat(glog, "lamp"); break;
 case 4: strcat(glog, "lump of rock"); break;
 case 5: strcat(glog, "bone lantern"); break;
 case 6: strcat(glog, "silver horn"); break;
 case 7: strcat(glog, "small ebony casket"); break;
 case 8: strcat(glog, "deck of cards"); break;
 case 9: strcat(glog, "deck of cards"); break;
 case 10: strcat(glog, "crystal ball"); break;
 case 11: strcat(glog, "small ebony casket"); break;
 case 12: strcat(glog, "crystal ball"); break;
 case 13: strcat(glog, "grey disc"); break;
 case 14:
 switch(it_plus)
 {
  case 1: strcat(glog, "iron "); break;
  case 2: strcat(glog, "basalt "); break;
  case 4: strcat(glog, "icy "); break;
  case 5: strcat(glog, "bone "); break;
  case 13: strcat(glog, "slimy "); break;
  case 14: strcat(glog, "silver "); break;
  case 19: strcat(glog, "serpentine "); break;
  case 20: strcat(glog, "elven "); break;
  case 21: strcat(glog, "golden "); break;
  case 22: strcat(glog, "decaying "); break;
 } /* If more are added here, must also add below. */
 strcat(glog, "rune");
 break;

 }
break;

default:
 switch(item_typ)
 {
 case 0: strcat(glog, "bottled efreet"); break;
 case 1: strcat(glog, "crystal ball of seeing"); break;
 case 2: strcat(glog, "air elemental fan"); break;
 case 3: strcat(glog, "lamp of fire"); break;
 case 4: strcat(glog, "stone of earth elementals"); break;
 case 5: strcat(glog, "lantern of shadows"); break;
 case 6: strcat(glog, "Horn of Geryon"); break;

 case 7: strcat(glog, "box of beasts"); break;
 case 8: strcat(glog, "deck of wonders"); break;
 case 9: strcat(glog, "deck of summonings"); break;
 case 10: strcat(glog, "crystal ball of energy"); break;
 case 11: strcat(glog, "empty ebony casket"); break;
 case 12: strcat(glog, "crystal ball of fixation"); break;
 case 13: strcat(glog, "disc of storms"); break;
 case 14:
 switch(it_plus)
 {
  case 1: strcat(glog, "iron "); break;
  case 2: strcat(glog, "basalt "); break;
  case 4: strcat(glog, "icy "); break;
  case 5: strcat(glog, "bone "); break;
  case 13: strcat(glog, "slimy "); break;
  case 14: strcat(glog, "silver "); break;
  case 19: strcat(glog, "serpentine "); break;
  case 20: strcat(glog, "elven "); break;
  case 21: strcat(glog, "golden "); break;
  case 22: strcat(glog, "decaying "); break;
 } /* If more are added here, must also add above. */
 strcat(glog, "rune of Zot");
 break;
 }
break;

} // end switch ident_lev
break;


case 10:
strcpy(glog, "");
if (ident_lev == 0)
{
switch(item_da / 10)
        {
        case 0: strcat(glog , ""); break;
        case 1: strcat(glog , "chunky "); break;
        case 2: strcat(glog , "thick "); break;
        case 3: strcat(glog , "thin "); break;
        case 4: strcat(glog , "wide "); break;
        case 5: strcat(glog , "glowing "); break;
        case 6: strcat(glog , "dog-eared "); break;
        case 7: strcat(glog , "oblong "); break;
        case 8: strcat(glog , "runed "); break;
        case 9: strcat(glog , " "); break;
        case 10: strcat(glog , " "); break;
        case 11: strcat(glog , " "); break;

 }
        switch(item_da % 10)
        {
        case 0: strcat(glog , "paperback book"); break;
        case 1: strcat(glog , "hardcover book"); break;
        case 2: strcat(glog , "leatherbound book"); break;
        case 3: strcat(glog , "metal-bound book"); break;
        case 4: strcat(glog , "papyrus book"); break;
        case 5: strcat(glog , "book"); break;
        case 6: strcat(glog , "book"); break;
 }
} else
        switch(item_typ)
        {
        case 0: strcat(glog , "book of Minor Magic"); break;
         case 1: strcat(glog , "book of Minor Magic"); break;
        case 2: strcat(glog , "book of Minor Magic"); break;
        case 3: strcat(glog , "book of Conjurations"); break;
        case 4: strcat(glog , "book of Conjurations"); break;
        case 5: strcat(glog , "book of Flames"); break;
        case 6: strcat(glog , "book of Frost"); break;
  case 7: strcat(glog , "book of Invocations"); break;
 case 8: strcat(glog , "book of Fire"); break;
 case 9: strcat(glog , "book of Ice"); break;
 case 10: strcat(glog , "book of Surveyances"); break;
 case 11: strcat(glog , "book of Spatial Translocations"); break;
 case 12: strcat(glog , "book of Enchantments"); break;
 case 13: strcat(glog , "book of Poisonings"); break;
 case 14: strcat(glog , "book of Storms and Fire"); break;
 case 15: strcat(glog , "book of Death"); break;
 case 16: strcat(glog , "book of Hinderance"); break;
 case 17: strcat(glog , "book of Changes"); break;
 case 18: strcat(glog , "book of Transfigurations"); break;
 case 19: strcat(glog , "book of Useful Magic"); break;
 case 20: strcat(glog , "book of War Chants"); break;
 case 21: strcat(glog , "book of Clouds"); break;
 case 22: strcat(glog , "book of Healing"); break;
 case 23: strcat(glog , "book of Necromancy"); break;
 case 24: strcat(glog , "Necronomicon"); break;
 case 25: strcat(glog , "book of Summonings"); break;
 case 26: strcat(glog , "book of Charms"); break;
 case 27: strcat(glog , "book of Demonology"); break;

 case 28: strcat(glog , "book of Air"); break;
 case 29: strcat(glog , "book of the Sky"); break;
 case 30: strcat(glog , "book of Divinations"); break;
 case 31: strcat(glog , "book of the Warp"); break;
 case 32: strcat(glog , "book of Envenomations"); break;
 case 33: strcat(glog , "book of Annihilations"); break;
 case 34: strcat(glog , "book of Unlife"); break;

 case 35: strcat(glog , "tome of destruction"); break;

 case 36: strcat(glog , "book of Control"); break;
 case 37: strcat(glog , "book of Mutations"); break;
 case 38: strcat(glog , "book of Tukima"); break;
 case 39: strcat(glog , "book of Geomancy"); break;
 case 40: strcat(glog , "book of Earth"); break;

 case 41: strcat(glog , "manual of ");
 strcat(glog, skill_name(it_plus));
 break;

 case 42: strcat(glog , "book of Wizardry"); break;
 case 43: strcat(glog , "book of Power"); break;

        } // end switch
/*
Spellbook binary thing:
6 spells: 127
5 spells: 126
4 spells: 124
3 spells: 120
2 spells: 112
1 spells: 96
0 spells: 64
(assuming all from start, no empty spaces)
*/

break;


case 11: // more magical devices
if (ident_lev == 0)
{
 switch(item_da) // gnarled
 {
  case 0: strcat(glog, "curved "); break;
  case 1: strcat(glog, "glowing "); break;
  case 2: strcat(glog, "thick "); break;
  case 3: strcat(glog, "thin "); break;
  case 4: strcat(glog, "long "); break;
  case 5: strcat(glog, "twisted "); break;
  case 6: strcat(glog, "jewelled "); break;
  case 7: strcat(glog, "runed "); break;
  case 8: strcat(glog, "smoking "); break;
  case 9: strcat(glog, " "); break;
  case 10: strcat(glog, " "); break;
  case 11: strcat(glog, " "); break;
  case 12: strcat(glog, " "); break;
  case 13: strcat(glog, " "); break;
  case 14: strcat(glog, " "); break;
  case 15: strcat(glog, " "); break;
  case 16: strcat(glog, " "); break;
  case 17: strcat(glog, " "); break;
  case 18: strcat(glog, " "); break;
  case 19: strcat(glog, " ");break;
  case 20: strcat(glog, " ");break;
  case 21: strcat(glog, " "); break;
  case 22: strcat(glog, " "); break;
  case 23: strcat(glog, " "); break;
  case 24: strcat(glog, " "); break;
  case 25: strcat(glog, " "); break;
  case 26: strcat(glog, " "); break;
  case 27: strcat(glog, " "); break;
  case 28: strcat(glog, " "); break;
  case 29: strcat(glog, " "); break;
 } // end switch
} // end if

if (ident_lev > 0 && item_typ >= 10 && item_typ < 25) strcat(glog, "spell ");

strcat(glog, "staff");

if (ident_lev > 0)
{
        switch(item_typ)
        {
        case 0: strcat(glog , " of wizardry"); break;
         case 1: strcat(glog , " of power"); break;
        case 2: strcat(glog , " of fire"); break;
        case 3: strcat(glog , " of cold"); break;
        case 4: strcat(glog , " of poison"); break;
  case 5: strcat(glog , " of energy"); break; // crappy name. oh well
  case 6: strcat(glog , " of death"); break;
  case 7: strcat(glog , " of conjuration"); break;
  case 8: strcat(glog , " of enchantment"); break;
  case 9: strcat(glog , " of summoning"); break;

 case 10: strcat(glog , " of smiting"); break;
 case 11: strcat(glog , " of summoning"); break;
 case 12: strcat(glog , " of destruction"); break;
 case 13: strcat(glog , " of destruction"); break;
 case 14: strcat(glog , " of destruction"); break;
 case 15: strcat(glog , " of destruction"); break;
 case 16: strcat(glog , " of warding"); break;
 case 17: strcat(glog , " of discovery"); break;
 case 18: strcat(glog , " of demonology"); break;

 case 25: strcat(glog , " of air"); break;
 case 26: strcat(glog , " of earth"); break;
 case 27: strcat(glog , " of channeling"); break;

        } // end switch
} // end if
break;


case 12:
switch(item_typ)
{
case 0:
default : strcpy(glog , "Orb of Zot"); break;
/*
case 1: strcpy(glog, "Orb of Zug"); break;
case 2: strcpy(glog, "Orb of Xob"); break;
case 3: strcpy(glog, "Orb of Ix"); break;

case 4: strcpy(glog, "Orb of Xug"); break;
case 5: strcpy(glog, "Orb of Zob"); break;
case 6: strcpy(glog, "Orb of Ik"); break;
case 7: strcpy(glog, "Orb of Grolp"); break;
case 8: strcpy(glog, "Orb of fo brO ehT"); break;
case 9: strcpy(glog, "Orb of Plob"); break;
case 10: strcpy(glog, "Orb of Zuggle-Glob"); break;
case 11: strcpy(glog, "Orb of Zin"); break;
case 12: strcpy(glog, "Orb of Qexigok"); break;
case 13: strcpy(glog, "Orb of Bujuk"); break;
case 14: strcpy(glog, "Orb of Uhen Tiquritu"); break;
case 15: strcpy(glog, "Orb of Idohoxom Sovuf"); break;
case 16: strcpy(glog, "Orb of Voc Vocilicoso"); break;
case 17: strcpy(glog, "Orb of Chanuaxydiviha"); break;
case 18: strcpy(glog, "Orb of Ihexodox"); break;
case 19: strcpy(glog, "Orb of Rynok Pol"); break;

case 20: strcpy(glog, "Orb of Nemelex"); break;
case 21: strcpy(glog, "Orb of Sif Muna"); break;
case 22: strcpy(glog, "Orb of Okawaru"); break;
case 23: strcpy(glog, "Orb of Kikubaaqudgha"); break;
*/
}
break;





case 16: // Gems - obviously not yet implemented
break;


case 14: // corpses
char gmo_n [40];
switch(item_typ)
{
case 0: // corpse
moname(it_plus, 0, 1, 100, gmo_n);
if (item_da < 100) strcat(glog, "rotting ");
strcat(glog, gmo_n);
strcat(glog, " corpse");
break;

case 1: // skeleton
moname(it_plus, 0, 1, 100, gmo_n);
strcat(glog, gmo_n);
strcat(glog, " skeleton");
break;

} // end corpse switch



break;




        default: strcat(glog, "!");
        break;


        } // end of switch?
//} // end of for lopp

if (strlen(glog) < 3)
{
        strcat(glog, "questionable item (c");
        char ugug [4];
        itoa(item_clas, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ",t");
        itoa(item_typ, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ",p");
        itoa(it_plus, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ",p(2)");
        itoa(it_plus, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ",d");
        itoa(item_da, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ":q");
        itoa(it_quant, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ")");

}




if (it_quant > 1 && item_clas != 1 && item_clas != 6 && item_clas != 8 && (item_clas != 4 || item_typ != 21))
{
        strcat(glog, "s");
}

return 1;

} // end of char item_name_2









void save_id(char identy [4] [50])
{

char ix = 0;
char jx = 0;

for (ix = 0; ix < 4; ix ++)
{
 for (jx = 0; jx < 50; jx ++)
 {
  identy [ix] [jx] = id [ix] [jx];
 }
}

}


void initial(void)
{
char i = 0;
char j = 0;

for (i = 0; i < 4; i ++)
{
        for (j = 0; j < 50; j ++)
        {
                id [i] [j] = 0;
        }
}

} // end of void initial


void set_id(char cla, char ty, char setting)
{
        if (cla > 99)
        {
                cla -= 100;
                id [cla] [ty] = setting;
                return;
        }

        switch(cla)
        {
        case 3: id [0] [ty] = setting; break;
        case 6: id [1] [ty] = setting; break;
        case 7: id [2] [ty] = setting; break;
        case 8: id [3] [ty] = setting; break;
        /* if it's none of these, will just return*/
        }
} /* end of void set_id */


char get_id(char cla, char ty)
{
        if (cla > 99)
        {
                cla -= 100;
                return id [cla] [ty];
        }

        switch(cla)
        {
        case 3: return id [0] [ty];
        case 6: return id [1] [ty];
        case 7: return id [2] [ty];
        case 8: return id [3] [ty];
        /* if it's none of these, will just return 0 */
        }
return 0;

} /* end of char get_id */




int property(int pr1, int pr2, int pr3)
{
 return prop [pr1] [pr2] [pr3];
}

int mass(int pr1, int pr2)
{
 return mss [pr1] [pr2];
}

void init_properties(void)
{

        //strcpy(all_items [2] [0], "robe");
        prop [2] [0] [0] = 1; // AC
        prop [2] [0] [1] = 0; // evasion
        mss [2] [0] = 60;

        //strcpy(all_items [2] [0], "leather armour");
        prop [2] [1] [0] = 2; // AC
        prop [2] [1] [1] = -1; // evasion
        mss [2] [1] = 150;

        //strcpy(all_items [2] [1], "ring mail");
        prop [2] [2] [0] = 3; // AC
        prop [2] [2] [1] = -2; // evasion
        mss [2] [2] = 300;

        //strcpy(all_items [2] [2], "scale mail");
        prop [2] [3] [0] = 4; // AC
        prop [2] [3] [1] = -2; // evasion
        mss [2] [3] = 400;

        //strcpy(all_items [2] [3], "chain mail");
        prop [2] [4] [0] = 5; // AC
        prop [2] [4] [1] = -3; // evasion
        mss [2] [4] = 450;

        //strcpy(all_items [2] [4], "splint mail");
        prop [2] [5] [0] = 6; // AC
        prop [2] [5] [1] = -5; // evasion
        mss [2] [5] = 550;

        //strcpy(all_items [2] [5], "banded mail");
        prop [2] [6] [0] = 6; // AC
        prop [2] [6] [1] = -4; // evasion
        mss [2] [6] = 500;

        //strcpy(all_items [2] [6], "plate mail");
        prop [2] [7] [0] = 7; // AC
        prop [2] [7] [1] = -5; // evasion
        mss [2] [7] = 650;

        // dragon hide
        prop [2] [15] [0] = 2; // AC
        prop [2] [15] [1] = -2; // evasion
        mss [2] [15] = 220;

        // troll hide
        prop [2] [16] [0] = 1; // AC
        prop [2] [16] [1] = -1; // evasion
        mss [2] [16] = 180;

        // crystal plate
        prop [2] [17] [0] = 14; // AC
        prop [2] [17] [1] = -8; // evasion
        mss [2] [17] = 1200;

        // dragon scale
        prop [2] [18] [0] = 8; // AC
        prop [2] [18] [1] = -2; // evasion
        mss [2] [18] = 220;

        // troll leather
        prop [2] [19] [0] = 2; // AC
        prop [2] [19] [1] = -1; // evasion
        mss [2] [19] = 180;

        // ice dragon hide
        prop [2] [20] [0] = 2; // AC
        prop [2] [20] [1] = -2; // evasion
        mss [2] [20] = 220;

        // ice dragon scale
        prop [2] [21] [0] = 8; // AC
        prop [2] [21] [1] = -2; // evasion
        mss [2] [21] = 220;

        // steam dragon hide
        prop [2] [22] [0] = 0; // AC
        prop [2] [22] [1] = 0; // evasion
        mss [2] [22] = 120;

        // steam dragon armour
        prop [2] [23] [0] = 3; // AC
        prop [2] [23] [1] = 0; // evasion
        mss [2] [23] = 120;

        // mottled dragon hide
        prop [2] [24] [0] = 1; // AC
        prop [2] [24] [1] = -1; // evasion
        mss [2] [24] = 150;

        // mottled dragon hide
        prop [2] [25] [0] = 5; // AC
        prop [2] [25] [1] = -1; // evasion
        mss [2] [25] = 150;

        // storm dragon hide
        prop [2] [26] [0] = 2; // AC
        prop [2] [26] [1] = -5; // evasion
        mss [2] [26] = 400;

        // storm dragon armour
        prop [2] [27] [0] = 9; // AC
        prop [2] [27] [1] = -5; // evasion
        mss [2] [27] = 400;

        // gold dragon hide
        prop [2] [28] [0] = 2; // AC
        prop [2] [28] [1] = -10; // evasion
        mss [2] [28] = 1100;

        // gold dragon armour
        prop [2] [29] [0] = 10; // AC
        prop [2] [29] [1] = -10; // evasion
        mss [2] [29] = 1100;

        // animal skin
        prop [2] [30] [0] = 1; // AC
        prop [2] [30] [1] = 0; // evasion
        mss [2] [30] = 100;

    // swamp dragon hide
        prop [2] [31] [0] = 1; // AC
        prop [2] [31] [1] = -2; // evasion
        mss [2] [31] = 200;

    // swamp dragon hide
        prop [2] [32] [0] = 6; // AC
        prop [2] [32] [1] = -2; // evasion
        mss [2] [32] = 200;



        // other armour:

        // shield:
        prop [2] [8] [0] = 0; // AC
        prop [2] [8] [1] = 0; // evasion
        mss [2] [8] = 100;

        // cloak
        prop [2] [9] [0] = 1; // AC
        prop [2] [9] [1] = 0; // evasion
        mss [2] [9] = 20;

        // helmet
        prop [2] [10] [0] = 1; // AC
        prop [2] [10] [1] = 0; // evasion
        mss [2] [10] = 80;


        // gloves
        prop [2] [11] [0] = 1; // AC
        prop [2] [11] [1] = 0; // evasion
        mss [2] [11] = 20;

        // boots
        prop [2] [12] [0] = 1; // AC
        prop [2] [12] [1] = 0; // evasion
        mss [2] [12] = 40;

        // buckler:
        prop [2] [13] [0] = 0; // AC
        prop [2] [13] [1] = 0; // evasion
        mss [2] [13] = 50;

        // large shield:
        prop [2] [14] [0] = 0; // AC
        prop [2] [14] [1] = 0; // evasion
        mss [2] [14] = 250;


int i = 0;

for (i = 0; i < 50; i ++)
{
        mss [3] [i] = 100;
        mss [4] [i] = 100;
        mss [5] [i] = 200;
        mss [6] [i] = 50;
        mss [7] [i] = 20;
        mss [8] [i] = 60;
        mss [9] [i] = 5;
   // don't know what these are, yet:
   mss [10] [i] = 100; // books
   mss [11] [i] = 130; // staves
   mss [12] [i] = 300; // the Orb!
   mss [13] [i] = 100;
   mss [14] [i] = 100;




}

// this is food, right?
mss [4] [0] = 80;
mss [4] [1] = 80;
mss [4] [2] = 20;
mss [4] [3] = 20;
mss [4] [4] = 30;
mss [4] [5] = 40;
mss [4] [6] = 55;
mss [4] [7] = 50;
mss [4] [8] = 40;
mss [4] [9] = 15;
mss [4] [10] = 20;
mss [4] [11] = 20;
mss [4] [12] = 5;
mss [4] [13] = 10;
mss [4] [14] = 20;
mss [4] [15] = 5;
mss [4] [16] = 3;
mss [4] [17] = 10;
mss [4] [18] = 20;
mss [4] [19] = 40;
mss [4] [20] = 40;
mss [4] [21] = 100; // chunk of flesh
/*mss [4] [21] = 40;
mss [4] [22] = 50;
mss [4] [23] = 60;
mss [4] [24] = 60;
mss [4] [25] = 100;*/

mss [13] [0] = 250; // flask
mss [13] [1] = 200; // crystal ball

/*
        case 0: strcat(glog , "meat ration"); break;
        case 1: strcat(glog , "bread ration"); break;
        case 2: strcat(glog , "pear"); break;
        case 3: strcat(glog , "apple"); break; // make this less common.
        case 4: strcat(glog , "choko"); break;
        case 5: strcat(glog , "honeycomb"); break;
        case 6: strcat(glog , "royal jell"); break; // maybe a joke monster of the same name? - mix something with jelly crystals?
        case 7: strcat(glog , "snozzcumber"); break;
 case 8: strcat(glog , "slice of pizza"); break;
 case 9: strcat(glog , "apricot"); break;
 case 10: strcat(glog , "orange"); break;
 case 11: strcat(glog , "banana"); break;
 case 12: strcat(glog , "strawberr"); break;
 case 13: strcat(glog , "rambutan"); break;
 case 14: strcat(glog , "lemon"); break;
 case 15: strcat(glog , "grape"); break;
 case 16: strcat(glog , "sultana"); break;
 case 17: strcat(glog , "lychee"); break;
 case 18: strcat(glog , "beef jerk"); break;
 case 19: strcat(glog , "cheese"); break;
 case 20: strcat(glog , "sausage"); break;
*/

        // weapons: blunt weapons are first to help grouping them together
        //  note: AC prop can't be 0 or -ve because of division.
        //        If it's 1, makes no difference


        // NOTE: I have *removed* AC bit for weapons - just doesn't work

        // prop [x] [2] is speed

        // club
        prop [0] [0] [0] = 5; // damage
        prop [0] [0] [1] = 4; // helps to get past evasion
        prop [0] [0] [2] = 12; // speed
        mss [0] [0] = 50;

        // mace:
        prop [0] [1] [0] = 7; // damage
        prop [0] [1] [1] = 3; // helps to get past evasion
        prop [0] [1] [2] = 13; // speed
        mss [0] [1] = 140;

        // flail
        prop [0] [2] [0] = 8; // damage
        prop [0] [2] [1] = 1; // helps to get past evasion
        prop [0] [2] [2] = 15; // speed
        mss [0] [2] = 150;

        // dagger
        prop [0] [3] [0] = 3; // damage
        prop [0] [3] [1] = 6; // helps to get past evasion
        prop [0] [3] [2] = 11; // speed
        mss [0] [3] = 20;

        // morningstar
        prop [0] [4] [0] = 8; // damage
        prop [0] [4] [1] = 3; // helps to get past evasion
        prop [0] [4] [2] = 14; // speed
        mss [0] [4] = 120;

        // short sword
        prop [0] [5] [0] = 6; // damage
        prop [0] [5] [1] = 5; // helps to get past evasion
        prop [0] [5] [2] = 12; // speed
        mss [0] [5] = 100;

        // long sword
        prop [0] [6] [0] = 10; // damage
        prop [0] [6] [1] = 3; // helps to get past evasion
        prop [0] [6] [2] = 14; // speed
        mss [0] [6] = 160;

        // great sword
        prop [0] [7] [0] = 16; // damage
        prop [0] [7] [1] = -1; // helps to get past evasion
        prop [0] [7] [2] = 17; // speed
        mss [0] [7] = 250;

        // scimitar
        prop [0] [8] [0] = 11; // damage
        prop [0] [8] [1] = 1; // helps to get past evasion
        prop [0] [8] [2] = 14; // speed
        mss [0] [8] = 170;

        // hand axe
        prop [0] [9] [0] = 7; // damage
        prop [0] [9] [1] = 2; // helps to get past evasion
        prop [0] [9] [2] = 13; // speed
        mss [0] [9] = 110;

        // battleaxe
        prop [0] [10] [0] = 17; // damage
        prop [0] [10] [1] = -2; // helps to get past evasion
        prop [0] [10] [2] = 18; // speed
        mss [0] [10] = 200;

        // spear
        prop [0] [11] [0] = 5; // damage
        prop [0] [11] [1] = 3; // helps to get past evasion
        prop [0] [11] [2] = 13; // speed
        mss [0] [11] = 50;

        // halberd
        prop [0] [12] [0] = 13; // damage
        prop [0] [12] [1] = -3; // helps to get past evasion
        prop [0] [12] [2] = 19; // speed
        mss [0] [12] = 200;

        // sling - the three properties are _not_ irrelevant here - when something hits something else (either may be you) in melee, these are used.
        prop [0] [13] [0] = 1; // damage
        prop [0] [13] [1] = -1; // helps to get past evasion
        prop [0] [13] [2] = 11; // speed
        mss [0] [13] = 10;

        // bow
        prop [0] [14] [0] = 2; // damage
        prop [0] [14] [1] = -3; // helps to get past evasion
        prop [0] [14] [2] = 11; // speed
        mss [0] [14] = 100;

        // crossbow
        prop [0] [15] [0] = 2; // damage
        prop [0] [15] [1] = -1; // helps to get past evasion
        prop [0] [15] [2] = 15; // speed
        mss [0] [15] = 250;

        // hand crossbow
        prop [0] [16] [0] = 1; // damage
        prop [0] [16] [1] = -1; // helps to get past evasion
        prop [0] [16] [2] = 15; // speed
        mss [0] [16] = 70;

        // glaive
        prop [0] [17] [0] = 15; // damage
        prop [0] [17] [1] = -3; // helps to get past evasion
        prop [0] [17] [2] = 18; // speed
        mss [0] [17] = 200;

        // staff - hmmm
        prop [0] [18] [0] = 5; // damage
        prop [0] [18] [1] = 4; // helps to get past evasion
        prop [0] [18] [2] = 12; // speed
        mss [0] [18] = 130;

        // scythe
        prop [0] [19] [0] = 14; // damage
        prop [0] [19] [1] = -4; // helps to get past evasion
        prop [0] [19] [2] = 22; // speed
        mss [0] [19] = 230;


        // giant club
        prop [0] [20] [0] = 15; // damage
        prop [0] [20] [1] = -5; // helps to get past evasion
        prop [0] [20] [2] = 16; // speed
        mss [0] [20] = 750;

        // giant spiked club
        prop [0] [21] [0] = 18; // damage
        prop [0] [21] [1] = -6; // helps to get past evasion
        prop [0] [21] [2] = 17; // speed
        mss [0] [21] = 850;

        // these two ^^^ should have the same speed because of 2-h ogres.


        // eveningstar
        prop [0] [22] [0] = 12; // damage
        prop [0] [22] [1] = 2; // helps to get past evasion
        prop [0] [22] [2] = 15; // speed
        mss [0] [22] = 150;

        // quick blade
        prop [0] [23] [0] = 5; // damage
        prop [0] [23] [1] = 6; // helps to get past evasion
        prop [0] [23] [2] = 7; // speed
        mss [0] [23] = 100;

        // katana
        prop [0] [24] [0] = 13; // damage
        prop [0] [24] [1] = 4; // helps to get past evasion
        prop [0] [24] [2] = 13; // speed
        mss [0] [24] = 160;

        // exec axe
        prop [0] [25] [0] = 20; // damage
        prop [0] [25] [1] = -4; // helps to get past evasion
        prop [0] [25] [2] = 20; // speed
        mss [0] [25] = 320;

        // double sword
        prop [0] [26] [0] = 15; // damage
        prop [0] [26] [1] = 3; // helps to get past evasion
        prop [0] [26] [2] = 16; // speed
        mss [0] [26] = 220;

        // triple sword
        prop [0] [27] [0] = 19; // damage
        prop [0] [27] [1] = -1; // helps to get past evasion
        prop [0] [27] [2] = 19; // speed
        mss [0] [27] = 300;

        // hammer
        prop [0] [28] [0] = 7; // damage
        prop [0] [28] [1] = 2; // helps to get past evasion
        prop [0] [28] [2] = 13; // speed
        mss [0] [28] = 130;

        // ancus
        prop [0] [29] [0] = 8; // damage
        prop [0] [29] [1] = 2; // helps to get past evasion
        prop [0] [29] [2] = 15; // speed
        mss [0] [29] = 160;

        // whip
        prop [0] [30] [0] = 3; // damage
        prop [0] [30] [1] = 1; // helps to get past evasion
        prop [0] [30] [2] = 14; // speed
        mss [0] [30] = 30;

        // sabre
        prop [0] [31] [0] = 7; // damage
        prop [0] [31] [1] = 4; // helps to get past evasion
        prop [0] [31] [2] = 12; // speed
        mss [0] [31] = 110;

        // demon blade
        prop [0] [32] [0] = 13; // damage
        prop [0] [32] [1] = 2; // helps to get past evasion
        prop [0] [32] [2] = 15; // speed
        mss [0] [32] = 200;

        // demon whip
        prop [0] [33] [0] = 10; // damage
        prop [0] [33] [1] = 1; // helps to get past evasion
        prop [0] [33] [2] = 14; // speed
        mss [0] [33] = 30;


        // Missiles:

        // stone
        prop [1] [0] [0] = 2;
        prop [1] [0] [1] = 4;
        mss [1] [0] = 5;

        // arrow
        prop [1] [1] [0] = 2;
        prop [1] [1] [1] = 7;
        mss [1] [1] = 10;

        // crossbow bolt
        prop [1] [2] [0] = 2;
        prop [1] [2] [1] = 9; // but increases less with skill
        mss [1] [2] = 12;

        // dart
        prop [1] [3] [0] = 3;
        prop [1] [3] [1] = 5; //whatever - for hand crossbow
        mss [1] [3] = 5;

        // large rock
        prop [1] [5] [0] = 20;
        mss [1] [5] = 1000;

}



unsigned char check_item_knowledge(void)
{


   char st_pass [60];

   int i;
   int j;
   char lines = 0;
   unsigned char anything = 0;
   char ft;

   char yps = 0;
#ifdef DOS_TERM
   char buffer[2400];
   gettext(35, 1, 80, 25, buffer);
#endif

   strcpy(st_pass, "");

#ifdef DOS_TERM
   window(35, 1, 80, 25);
#endif

   clrscr();



int inv_count = 0;
unsigned char ki;



for (i = 0; i < 4; i++)
{
 for (j = 0; j < 30; j ++)
 {
        if (id [i] [j] != 0)
        {
                inv_count++;
        }
 }
}

if (inv_count == 0)
{
   cprintf("You don't recognise anything yet!");
   if (getch() == 0) getch();
   goto putty;
}
textcolor(BLUE);
cprintf("  You recognise:");
textcolor(LIGHTGREY);
lines++;


for (i = 0; i < 4; i ++)
        {
switch(i)
{
 case 0: ft = 3; break; // magic devices
 case 1: ft = 6; break; // scrolls
 case 2: ft = 7; break; // rings
 case 3: ft = 8; break; // potions
}

        for (j = 0; j < 30; j++)
        {

                if (lines > 23 && inv_count > 0)
                {
                        gotoxy(1,25);
                        cprintf("-more-");
                        ki = getch();
                        if (ki == 27)
                        {
#ifdef DOS_TERM
                                puttext(35, 1, 80, 25, buffer);
#endif
                                return 27;
                        }
                        if (ki >= 65 && ki < 123)
                        {
#ifdef DOS_TERM
                                puttext(35, 1, 80, 25, buffer);
#endif
                                return ki;
                        }
                        if (ki == 0) ki = getch();
                        lines = 0;
                        clrscr();
                        gotoxy(1,1);
                        anything = 0;
                }

                if (id [i] [j] == 1)
                {
                        anything ++;

                        if (lines > 0) cprintf(EOL);
                        lines++;
                                cprintf(" ");

   yps = wherey();

//   item_name_2(i, j, 0, 0, 1, 3, 3, st_pass);
//char item_name_2(char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char glog [60])
item_name_2(0, ft, j, 0, 0, 1, 0, st_pass);

                        cprintf(st_pass);

         inv_count --;

   if (wherey() != yps) lines++;

                }
        } // end of j loop
}



   if (anything > 0)
   {
           ki = getch();
           //ki = getch();
           //ki = anything;
           if (ki >= 65 && ki < 123)
           {
#ifdef DOS_TERM
                   puttext(35, 1, 80, 25, buffer);
#endif
                   return ki;
           }
           if (ki == 0) ki = getch();
#ifdef DOS_TERM
           puttext(35, 1, 80, 25, buffer);
#endif
           return anything;
   }

   putty :
#ifdef DOS_TERM
   puttext(35, 1, 80, 25, buffer);
#endif

   return ki;

} // end of check_item_knowledge



char weapon_skill(char wclass, char wtype)
{
if (wclass == 11) return 7; // staff

if (wclass != 0) return 0; // no skill

switch(wtype)
{

        case 0: return 5; //strcat(glog , "club"); break;
        case 1: return 5; //strcat(glog , "mace"); break;
        case 2: return 5; //strcat(glog , "flail"); break;
        case 3: return 1; //strcat(glog , "dagger"); break;
        case 4: return 5;//??? //strcat(glog , "spiked mace"); break;
        case 5: return 1; //strcat(glog , "short sword"); break;
        case 6: return 2; //strcat(glog , "long sword"); break;
        case 7: return 3; //strcat(glog , "great sword"); break;
        case 8: return 2; //strcat(glog , "scimitar"); break;
        case 9: return 4; //strcat(glog , "hand axe"); break;
        case 10: return 4; //strcat(glog , "battleaxe"); break;
        case 11: return 6; //strcat(glog , "spear"); break;
        case 12: return 6; //strcat(glog , "halberd"); break;
        case 13: return 8; //strcat(glog , "sling"); break;
        case 14: return 9; //strcat(glog , "bow"); break;
        case 15: return 10; //strcat(glog , "crossbow"); break;
 case 16: return 10; //strcat(glog , "hand crossbow"); break;
 case 17: return 6; //strcat(glog , "glaive"); break;
        case 18: return 7; //strcat(glog , "quarterstaff"); break;
 case 19: return 6; //strcat(glog , "scythe"); break;
        case 20: return 5; //strcat(glog , "giant club"); break;
        case 21: return 5; //strcat(glog , "giant spiked club"); break;
        case 22: return 5; // eveningstar
        case 23: return 1; // quick blade
        case 24: return 2; // katana

        case 25: return 4; // exec axe
        case 26: return 2; // 2x sword
        case 27: return 3; // 3x sword
        case 28: return 5; // hammer
        case 29: return 5; // ancus
        case 30: return 5; // whip
        case 31: return 1; // sabre
        case 32: return 2; // demon blade
        case 33: return 5; // demon whip

}

return 0;

}


char damage_type(char wclass, char wtype)
{

if (wclass != 0) return 0; // bludgeon
// 1 = slicing
// 2 = piercing
// 3 = chopping

switch(wtype)
{

        case 0: return 0; //strcat(glog , "club"); break;
        case 1: return 0; //strcat(glog , "mace"); break;
        case 2: return 0; //strcat(glog , "flail"); break;
        case 3: return 1; //strcat(glog , "dagger"); break;
        case 4: return 2;//??? //strcat(glog , "spiked mace"); break;
        case 5: return 1; //strcat(glog , "short sword"); break;
        case 6: return 1; //strcat(glog , "long sword"); break;
        case 7: return 1; //strcat(glog , "great sword"); break;
        case 8: return 1; //strcat(glog , "scimitar"); break;
        case 9: return 3; //strcat(glog , "hand axe"); break;
        case 10: return 3; //strcat(glog , "battleaxe"); break;
        case 11: return 2; //strcat(glog , "spear"); break;
        case 12: return 3; //strcat(glog , "halberd"); break;
        case 13: return 0; //strcat(glog , "sling"); break;
        case 14: return 0; //strcat(glog , "bow"); break;
        case 15: return 0; //strcat(glog , "crossbow"); break;
 case 16: return 0; //strcat(glog , "hand crossbow"); break;
 case 17: return 3; //strcat(glog , "glaive"); break;
        case 18: return 0; //strcat(glog , "quarterstaff"); break;
 case 19: return 1; //strcat(glog , "scythe"); break;
        case 20: return 0; //strcat(glog , "giant club"); break;
        case 21: return 2; //strcat(glog , "giant spiked club"); break;
        case 22: return 2; // eveningstar
        case 23: return 1; // quick blade
        case 24: return 1; // katana
        case 25: return 3; // exec axe
        case 26: return 1; // 2x sword
        case 27: return 1; // 3x sword
        case 28: return 0; // hammer
        case 29: return 0; // ancus
        case 30: return 0; // whip
        case 31: return 1; // sabre
        case 32: return 1; // demon blade
        case 33: return 0; // demon whip


}

return 0;

} // end damage_type


void make_name(unsigned char var1, unsigned char var2, unsigned char var3, char ncase, char str_pass [50])
{

char name [30] = "";
unsigned char numb [15];
char len;
char i = 0;
char nexty = 0;
char j = 0;
char igo = 0;

int ix = 0;


char glag [30];

strcpy(glag, "");


numb [0] = var1 * var2;
numb [1] = var1 * var3;
numb [2] = var2 * var3;
numb [3] = var1 * var2 * var3;
numb [4] = var1 + var2;
numb [5] = var2 + var3;
numb [6] = var1 * var2 + var3;
numb [7] = var1 * var3 + var2;
numb [8] = var2 * var3 + var1 ;
numb [9] = var1 * var2 * var3 - var1;
numb [10] = var1 + var2 + var2;
numb [11] = var2 + var3 * var1;
numb [12] = var1 * var2 * var3;
numb [13] = var1 * var3 * var1;
numb [14] = var2 * var3 * var3;


for (i = 0; i < 15; i ++)
{
        while(numb [i] >= 25)
        {
                numb [i] -= 25;
        }
} // end of for i

j = numb [6];

len = reduce(numb [reduce(numb [11]) / 2]);
while (len < 5 && j < 10)
{
        len += reduce(numb [j] + 1) + 1;
        j++;
}

while (len > 14)
{
        len -= 8;
}

nexty = retbit(numb [4]);
char k = 0;
j = 0;

for (i = 0; i < len; i ++)
{
j ++;
if (j >= 15)
{
        j = 0;
        k++;
        if (k > 9) break;
}

if (nexty == 1 || (i > 0 && is_a_vowel(name [i]) == 0))
{
        name [i] = retvow(numb [j]);
        if ((i == 0 || i == len - 1) && name [i] == 32)
        {
                i--;
                continue;
        }
} else
{
        if (numb [i / 2] <= 1 && i > 3 && is_a_vowel(name [i]) == 1) goto two_letter;
             else
             name [i] = numb [j];
        hello : igo ++;
}

if ((nexty == 0 && is_a_vowel(name [i]) == 1) || (nexty == 1 && is_a_vowel(name [i]) == 0))
{
        if (nexty == 1 && i > 0 && is_a_vowel(name [i - 1]) == 0) i --;
        i--;
        continue;
}


if (is_a_vowel(name [i]) == 0)
{
        nexty = 1;
} else nexty = 0;

ix++;

}

switch(ncase)
{
case 2:
for (i = 0; i < len + 1; i ++)
{
        if (i > 3 && name [i] == 0 && name [i + 1] == 0)
        {
                name [i] = 0;
                if (name [i - 1] == 32) name [i - 1] = 0;
                break;
        }
        if (name [i] != 32 && name [i] < 30)
                name [i] += 65;
        if (name [i] > 96) name [i] -= 32;
}
break;

case 3:
for (i = 0; i < len + 0; i ++)
{
        if (i != 0 && name [i] >= 65 && name [i] < 97)
        {
         if (name [i - 1] == 32) name [i] += 32;
        }

        if (name [i] > 97)
        {
         if (i == 0 || name [i - 1] == 32) name [i] -= 32;
        }


        if (name [i] < 30)
        {
         if (i == 0 || (name [i] != 32 && name [i - 1] == 32)) name [i] += 65;
           else name [i] += 97;
        }


}
break;

case 0:
for (i = 0; i < len; i ++)
{
        if (name [i] != 32 && name [i] < 30)
                name [i] += 97;
}
break;

case 1:
name [i] += 65;
for (i = 1; i < len; i ++)
{
        if (name [i] != 32 && name [i] < 30)
                name [i] += 97; //97;
}
break;
}

strcpy(glag, name);

if (strlen(glag) == 0) strcpy(glag, "Plog");

strcpy(str_pass, glag);

return;




two_letter :
if (nexty == 1) goto hello;

if (!is_a_vowel(name [i - 1])) goto hello;

i ++;
switch(i * (retbit(j) + 1))
{
case 0: strcat(name, "sh"); break;
case 1: strcat(name, "ch"); break;
case 2: strcat(name, "tz"); break;
case 3: strcat(name, "ts"); break;
case 4: strcat(name, "cs"); break;
case 5: strcat(name, "cz"); break;
case 6: strcat(name, "xt"); break;
case 7: strcat(name, "xk"); break;
case 8: strcat(name, "kl"); break;
case 9: strcat(name, "cl"); break;
case 10: strcat(name, "fr"); break;
case 11: strcat(name, "sh"); break;
case 12: strcat(name, "ch"); break;
case 13: strcat(name, "gh"); break;
case 14: strcat(name, "pr"); break;
case 15: strcat(name, "tr"); break;
case 16: strcat(name, "mn"); break;
case 17: strcat(name, "ph"); break;
case 18: strcat(name, "pn"); break;
case 19: strcat(name, "cv"); break;
case 20: strcat(name, "zx"); break;
case 21: strcat(name, "xz"); break;
case 23: strcat(name, "dd"); break;
case 24: strcat(name, "tt"); break;
case 25: strcat(name, "ll"); break;
//case 26: strcat(name, "sh"); break;
//case 12: strcat(name, "sh"); break;
//case 13: strcat(name, "sh"); break;
default:
i--;
goto hello;
}

ix += 2;

goto hello;

} // end of make_name



char reduce(unsigned char reducee)
{

while(reducee >= 26)
{
        reducee -= 26;
}

return reducee;

} // end of char reduce



char is_a_vowel(unsigned char let)
{
        //if (let == 'a' || let == 'e' || let == 'i' || let == 'o' || let == 'u')
        if (let == 0 || let == 4 || let == 8 || let == 14 || let == 20 || let == 24 || let == 32)
        {
                return 1;
        } else return 0;
} // end of char reduce



char retvow(char sed)
{

while(sed > 6)
{
        sed -= 6;
}

switch(sed)
{
        case 0: return 0;
        case 1: return 4;
        case 2: return 8;
        case 3: return 14;
        case 4: return 20;
        case 5: return 24;
        case 6: return 32;
}

return 0;
}


char retbit(char sed)
{

        if (sed % 2 == 0) return 0;

        return 1;

}

/*
 *  File:       itemname.cc
 *  Summary:    Misc functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <3>      6/13/99        BWR             Upped the base AC for heavy armour
 *      <2>      5/20/99        BWR             Extended screen lines support
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "itemname.h"

#ifdef DOS
#include <conio.h>
#endif

#ifdef USE_CURSES
#include <curses.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "externs.h"
#include "enum.h"
#ifdef MACROS
  #include "macro.h"
#endif
#include "mstruct.h"
#include "randart.h"
#include "skills2.h"

char item_name_2(unsigned char item_plus2, char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char glog[60]);

char reduce(unsigned char reducee);
char is_a_vowel(unsigned char let);
char retbit(char sed);
char retvow(char sed);
void make_name(unsigned char var1, unsigned char var2, unsigned char var3, char ncase, char str_pass[50]);


char id[4][50];

int prop[4][50][3];
int mss[20][50];



void it_name(int itn, char des, char str_pass[80])
{
    item_name(mitm.pluses2[itn], mitm.base_type[itn], mitm.sub_type[itn], mitm.special[itn], mitm.pluses[itn], mitm.quantity[itn], mitm.id[itn], des, str_pass);
}

void in_name(int inn, char des, char str_pass[80])
{
    item_name(you.inv_plus2[inn], you.inv_class[inn], you.inv_type[inn], you.inv_dam[inn], you.inv_plus[inn], you.inv_quantity[inn], you.inv_ident[inn], des, str_pass);
}


char item_name(unsigned char item_plus2, char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char descrip, char glag[60])
{

    char itm_name[60] = "";
    char tmp_quant[5];

    strcpy(glag, "");

    item_name_2(item_plus2, item_clas, item_typ, item_da, it_plus, it_quant, ident_lev, itm_name);

    if (item_clas == OBJ_ORBS || (item_clas == OBJ_WEAPONS && item_da > 180 && ident_lev > 0) || (item_clas == OBJ_MISCELLANY && item_typ == MISC_HORN_OF_GERYON && ident_lev > 0) || (item_clas == OBJ_JEWELLERY && (item_da == 200 || item_da == 201) && ident_lev >= 2) || ((item_clas == OBJ_WEAPONS || item_clas == OBJ_ARMOUR) && item_da % 30 >= 25 && ident_lev > 0))
    {
        switch (descrip)
        {
        case 2:
        case 4:
        case 0:
            strcat(glag, "The ");
            break;
        case 3:
        case 5:
        case 7:         // !

        case 1:
            strcat(glag, "the ");
            break;
        }
    }
    else if (it_quant > 1)
    {
        switch (descrip)
        {
        case 0:
            strcat(glag, "The ");
            break;
        case 1:
            strcat(glag, "the ");
            break;
        case 2:
            break;              // A/An

        case 3:
            break;              // a/an

        case 4:
            strcat(glag, "Your ");
            break;
        case 5:
            strcat(glag, "your ");
            break;
/*      case 6: nothing */
        case 7:
            strcat(glag, "its ");
            break;
        }

        itoa(it_quant, tmp_quant, 10);
        strcat(glag, tmp_quant);
        strcat(glag, " ");
    }
    else
    {
        switch (descrip)
        {
        case 0:
            strcat(glag, "The ");
            break;
        case 1:
            strcat(glag, "the ");
            break;
        case 2:
            strcat(glag, "A");
            if (itm_name[0] == 97 ^ itm_name[0] == 101 ^ itm_name[0] == 105 ^ itm_name[0] == 111 ^ itm_name[0] == 117)
            {
                strcat(glag, "n");
            }
            strcat(glag, " ");
            break;              // A/An

        case 3:
            strcat(glag, "a");
            if (itm_name[0] == 97 ^ itm_name[0] == 101 ^ itm_name[0] == 105 ^ itm_name[0] == 111 ^ itm_name[0] == 117)
            {
                strcat(glag, "n");
            }
            strcat(glag, " ");
            break;              // a/an

        case 4:
            strcat(glag, "Your ");
            break;
        case 5:
            strcat(glag, "your ");
            break;
/*              case 6: nothing */
        case 7:
            strcat(glag, "its ");
            break;
        }

    }                           // end of else

    strcat(glag, itm_name);

    return 1;

}                               // end of char name_item



char item_name_2(unsigned char item_plus2, char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char glog[60])
{

    char tmp_quant[5];
    char str_pass2[80];

    strcpy(glog, "");

    switch (item_clas)
    {

    case OBJ_WEAPONS:           // weapons

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
                if (it_plus >= 50 && (it_plus <= 130 || it_plus >= 150))
                    strcat(glog, "+");
                itoa(it_plus - 50, tmp_quant, 10);
                if (it_plus > 130)
                    itoa((it_plus - 150), tmp_quant, 10);
                strcat(glog, tmp_quant);
                strcat(glog, ",");
                if (item_plus2 >= 50)
                    strcat(glog, "+");
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
                switch (item_da - 180)
                {
                case 1:
                    strcat(glog, "Singing Sword");
                    break;
                case 2:
                    strcat(glog, "Wrath of Trog");
                    break;
                case 3:
                    strcat(glog, "Scythe of Curses");
                    break;
                case 4:
                    strcat(glog, "Mace of Variability");
                    break;
                case 5:
                    strcat(glog, "Glaive of Prune");
                    break;
                case 6:
                    strcat(glog, "Sceptre of Torment");
                    break;
                case 7:
                    strcat(glog, "Sword of Zonguldrok");
                    break;
                case 8:
                    strcat(glog, "Sword of Cerebov");
                    break;
                case 9:
                    strcat(glog, "Staff of Dispater");
                    break;
                case 10:
                    strcat(glog, "Sceptre of Asmodeus");
                    break;
                case 11:
                    strcat(glog, "Sword of Power");
                    break;
                case 12:
                    strcat(glog, "Knife of Accuracy");
                    break;
                case 13:
                    strcat(glog, "Staff of Olgreb");
                    break;
                case 14:
                    strcat(glog, "Vampire's Tooth");
                    break;
                case 15:
                    strcat(glog, "Staff of Wucad Mu");
                    break;      // this random name generator makes some weird things

                }
                break;
            }                   // end if ident_lev

            switch (item_da - 180)
            {
            case 1:
                strcat(glog, "golden long sword");
                break;
            case 2:
                strcat(glog, "bloodstained battleaxe");
                break;
            case 3:
                strcat(glog, "warped scythe");
                break;
            case 4:
                strcat(glog, "shimmering mace");
                break;
            case 5:
                strcat(glog, "purple glaive");
                break;
            case 6:
                strcat(glog, "jewelled golden mace");
                break;
            case 7:
                strcat(glog, "bone long sword");
                break;
            case 8:
                strcat(glog, "great serpentine sword");
                break;
            case 9:
                strcat(glog, "golden staff");
                break;
            case 10:
                strcat(glog, "ruby sceptre");
                break;
            case 11:
                strcat(glog, "chunky great sword");
                break;
            case 12:
                strcat(glog, "thin dagger");
                break;
            case 13:
                strcat(glog, "green glowing staff");
                break;
            case 14:
                strcat(glog, "ivory dagger");
                break;
            case 15:
                strcat(glog, "quarterstaff");
                break;

            }
            break;
        }                       // end uniques


        if (ident_lev <= 1 || item_da % 30 == SPWPN_NORMAL || item_da / 30 >= DWPN_ORCISH)
        {
            switch (item_da / 30)
            {
            case DWPN_RUNED:
                strcat(glog, "runed ");
                break;
            case DWPN_GLOWING:
                strcat(glog, "glowing ");
                break;
            case DWPN_ORCISH:
                strcat(glog, "orcish ");
                break;
            case DWPN_ELVEN:
                strcat(glog, "elven ");
                break;
            case DWPN_DWARVEN:
                strcat(glog, "dwarven ");
                break;
            }                   // end switch

        }                       // end if

        if (ident_lev > 1)
        {
            switch (item_da % 30)
            {
            case SPWPN_VAMPIRICISM:
                strcat(glog, "vampiric ");
                break;
            }                   // end switch

        }                       // end if

        standard_name_weap(item_typ, str_pass2);        /* in randart.cc */
        strcat(glog, str_pass2);

        if (ident_lev > 1)
        {
            switch (item_da % 30)
            {
            case SPWPN_NORMAL:
                break;
            case SPWPN_FLAMING:
                strcat(glog, " of flaming");
                break;
            case SPWPN_FREEZING:
                strcat(glog, " of freezing");
                break;
            case SPWPN_HOLY_WRATH:
                strcat(glog, " of holy wrath");
                break;
            case SPWPN_ELECTROCUTION:
                strcat(glog, " of electrocution");
                break;
            case SPWPN_ORC_SLAYING:
                strcat(glog, " of orc slaying");
                break;
            case SPWPN_VENOM:
                strcat(glog, " of venom");
                break;
            case SPWPN_PROTECTION:
                strcat(glog, " of protection");
                break;
            case SPWPN_DRAINING:
                strcat(glog, " of draining");
                break;
            case SPWPN_SPEED:
                strcat(glog, " of speed");
                break;
            case SPWPN_VORPAL:
                switch (damage_type(item_clas, item_typ))
                {
                case DVORP_CRUSHING:
                    strcat(glog, " of crushing");
                    break;
                case DVORP_SLICING:
                    strcat(glog, " of slicing");
                    break;
                case DVORP_PIERCING:
                    strcat(glog, " of piercing");
                    break;
                case DVORP_CHOPPING:
                    strcat(glog, " of chopping");
                    break;
                }
                break;
            case SPWPN_FLAME:
                strcat(glog, " of flame");
                break;          // bows/xbows

            case SPWPN_FROST:
                strcat(glog, " of frost");
                break;          // bows/xbows
                /* 13 - vamp */
            case SPWPN_DISRUPTION:
                strcat(glog, " of disruption");
                break;
            case SPWPN_PAIN:
                strcat(glog, " of pain");
                break;
            case SPWPN_DISTORTION:
                strcat(glog, " of distortion");
                break;

                /* 25 - 29 are randarts */
            }
        }

// Hellbrand; zap to reduce plus by 1 and cast hellfire?

        break;


    case OBJ_MISSILES:          // ammunition

        if (item_da % 30 == 3 || item_da % 30 == 4)
        {
            strcat(glog, "poisoned ");
        }
        if (ident_lev > 2)
        {
            unsigned char gokh = it_plus;

            //it_plus -= 50;
            if (gokh >= 50 && (gokh <= 130 || gokh >= 150))
                strcat(glog, "+");
            itoa(gokh - 50, tmp_quant, 10);
            if (gokh > 130)
                itoa((gokh - 150), tmp_quant, 10);
            strcat(glog, tmp_quant);
            strcat(glog, " ");
        }

        if (item_da != 0)
        {
            switch (item_da / 30)
            {
            case DWPN_ORCISH:
                strcat(glog, "orcish ");
                break;
            case DWPN_ELVEN:
                strcat(glog, "elven ");
                break;
            case DWPN_DWARVEN:
                strcat(glog, "dwarven ");
                break;
            }
        }

        switch (item_typ)
        {
        case MI_STONE:
            strcat(glog, "stone");
            break;
        case MI_ARROW:
            strcat(glog, "arrow");
            break;
        case MI_BOLT:
            strcat(glog, "bolt");
            break;
        case MI_DART:
            strcat(glog, "dart");
            break;
        case MI_EGGPLANT:
            strcat(glog, "eggplant");
            break;
        case MI_LARGE_ROCK:
            strcat(glog, "large rock");
            break;
        case 6:
            strcat(glog, "");
            break;
        case 7:
            strcat(glog, "");
            break;
        case 8:
            strcat(glog, "");
            break;
        case 9:
            strcat(glog, "");
            break;
        case 10:
            strcat(glog, "");
            break;
        case 12:
            strcat(glog, "");
            break;
        case 13:
            strcat(glog, "");
            break;
        case 14:
            strcat(glog, "");
            break;
        case 15:
            strcat(glog, "");
            break;
        case 16:
            strcat(glog, "");
            break;
        }
        if (it_quant > 1)
            strcat(glog, "s");

        if (ident_lev > 1)
        {
            switch (item_da % 30)
            {
            case 0:
                break;
            case 1:
                strcat(glog, " of flame");
                break;
            case 2:
                strcat(glog, " of ice");
                break;
//  case 3: strcat(glog, " of venom"); break;
                //  case 4: strcat(glog, " of venom"); break; /* temporary, produced by spell */
            }
        }
        break;

    case OBJ_ARMOUR:            // armour


        if (ident_lev > 0)
        {
            if (it_plus >= 130)
            {
                strcat(glog, "cursed ");
            }
        }




        if (ident_lev > 2)
        {
            if (it_plus >= 50 && (it_plus <= 130 || it_plus >= 150))
                strcat(glog, "+");
            itoa(it_plus - 50, tmp_quant, 10);
            if (it_plus > 130)
                itoa((it_plus - 150), tmp_quant, 10);
            strcat(glog, tmp_quant);
            strcat(glog, " ");
        }



        if (item_typ == ARM_GLOVES || (item_typ == ARM_BOOTS && item_plus2 == 0))
            strcat(glog, "pair of ");

        if (item_da % 30 >= 25 && item_da % 30 <= 29)
        {
            strcat(glog, randart_armour_name(item_clas, item_typ, item_da, it_plus, item_plus2, ident_lev));
            break;
        }

        if (ident_lev < 2 || item_da % 30 == 0 || item_da / 30 >= 4)
        {
            if (item_typ != 10)
            {
                switch ((int) item_da / 30)
                {
                case DARM_EMBROIDERED_SHINY:
                    if (item_typ == ARM_ROBE || item_typ == ARM_CLOAK || item_typ == ARM_GLOVES || item_typ == ARM_BOOTS || (item_typ == ARM_HELMET && item_plus2 >= 2))
                        strcat(glog, "embroidered ");
                    else if (item_typ == 1)
                        strcat(glog, "");
                    else
                        strcat(glog, "shiny ");
                    break;
                case DARM_RUNED:
                    strcat(glog, "runed ");
                    break;
                case DARM_GLOWING:
                    strcat(glog, "glowing ");
                    break;
                case DARM_ELVEN:
                    strcat(glog, "elven ");
                    break;
                case DARM_DWARVEN:
                    strcat(glog, "dwarven ");
                    break;
                case DARM_ORCISH:
                    strcat(glog, "orcish ");
                    break;
                }               // end switch

            }
            if (item_typ == ARM_HELMET && item_plus2 <= 1)
            {
                switch (item_da / 30)
                {
                case DHELM_WINGED:
                    strcat(glog, "winged ");
                    break;
                case DHELM_HORNED:
                    strcat(glog, "horned ");
                    break;
                case DHELM_CRESTED:
                    strcat(glog, "crested ");
                    break;
                case DHELM_PLUMED:
                    strcat(glog, "plumed ");
                    break;
                case DHELM_SPIKED:
                    strcat(glog, "spiked ");
                    break;
                case DHELM_VISORED:
                    strcat(glog, "visored ");
                    break;
                case DHELM_JEWELLED:
                    strcat(glog, "jewelled ");
                    break;
                case 13:
                    strcat(glog, " ");
                    break;
                case 14:
                    strcat(glog, " ");
                    break;
                case 15:
                    strcat(glog, " ");
                    break;
                }
            }

        }                       // end if

        standard_name_armour(item_typ, item_plus2, str_pass2);  /* in randart.cc */
        strcat(glog, str_pass2);



        if (ident_lev > 1)
        {
            switch (item_da % 30)
            {
            case SPARM_RUNNING:
                strcat(glog, " of running");
                break;
            case SPARM_FIRE_RESISTANCE:
                strcat(glog, " of fire resistance");
                break;
            case SPARM_COLD_RESISTANCE:
                strcat(glog, " of cold resistance");
                break;
            case SPARM_POISON_RESISTANCE:
                strcat(glog, " of poison resistance");
                break;
            case SPARM_SEE_INVISIBLE:
                strcat(glog, " of see invisible");
                break;
            case SPARM_DARKNESS:
                strcat(glog, " of darkness");
                break;
            case SPARM_STRENGTH:
                strcat(glog, " of strength");
                break;
            case SPARM_DEXTERITY:
                strcat(glog, " of dexterity");
                break;
            case SPARM_INTELLIGENCE:
                strcat(glog, " of intelligence");
                break;
            case SPARM_PONDEROUSNESS:
                strcat(glog, " of ponderousness");
                break;
            case SPARM_LEVITATION:
                strcat(glog, " of levitation");
                break;
            case SPARM_MAGIC_RESISTANCE:
                strcat(glog, " of magic resistance");
                break;
            case SPARM_PROTECTION:
                strcat(glog, " of protection");
                break;
            case SPARM_STEALTH:
                strcat(glog, " of stealth");
                break;
            case SPARM_RESISTANCE:
                strcat(glog, " of resistance");
                break;
            case SPARM_POSITIVE_ENERGY:
                strcat(glog, " of positive energy");
                break;
            case SPARM_ARCHMAGI:
                strcat(glog, " of the Archmagi");
                break;
            case SPARM_PRESERVATION:
                strcat(glog, " of preservation");
                break;
            }
        }

        break;


    case OBJ_WANDS:             // devices

        if (id[0][item_typ] == 1)
        {
            switch (item_typ)
            {
            case WAND_FLAME:
                strcat(glog, "wand of flame");
                break;
            case WAND_FROST:
                strcat(glog, "wand of frost");
                break;
            case WAND_SLOWING:
                strcat(glog, "wand of slowing");
                break;
            case WAND_HASTING:
                strcat(glog, "wand of hasting");
                break;
            case WAND_MAGIC_DARTS:
                strcat(glog, "wand of magic darts");
                break;
            case WAND_HEALING:
                strcat(glog, "wand of healing");
                break;
            case WAND_PARALYSIS:
                strcat(glog, "wand of paralysis");
                break;
            case WAND_FIRE:
                strcat(glog, "wand of fire");
                break;
            case WAND_COLD:
                strcat(glog, "wand of cold");
                break;
            case WAND_CONFUSION:
                strcat(glog, "wand of confusion");
                break;
            case WAND_INVISIBILITY:
                strcat(glog, "wand of invisibility");
                break;
            case WAND_DIGGING:
                strcat(glog, "wand of digging");
                break;
            case WAND_FIREBALL:
                strcat(glog, "wand of fireball");
                break;
            case WAND_TELEPORTATION:
                strcat(glog, "wand of teleportation");
                break;
            case WAND_LIGHTNING:
                strcat(glog, "wand of lightning");
                break;
            case WAND_POLYMORPH_OTHER:
                strcat(glog, "wand of polymorph other");
                break;
            case WAND_ENSLAVEMENT:
                strcat(glog, "wand of enslavement");
                break;
            case WAND_DRAINING:
                strcat(glog, "wand of draining");
                break;
            case WAND_RANDOM_EFFECTS:
                strcat(glog, "wand of random effects");
                break;
            case WAND_DISINTEGRATION:
                strcat(glog, "wand of disintegration");
                break;
            }
        }
        else
        {
            if (item_da <= 11)
            {
                switch (item_da)
                {
                case 0:
                    strcat(glog, "iron wand");
                    break;
                case 1:
                    strcat(glog, "brass wand");
                    break;
                case 2:
                    strcat(glog, "bone wand");
                    break;
                case 3:
                    strcat(glog, "wooden wand");
                    break;
                case 4:
                    strcat(glog, "copper wand");
                    break;
                case 5:
                    strcat(glog, "gold wand");
                    break;
                case 6:
                    strcat(glog, "silver wand");
                    break;
                case 7:
                    strcat(glog, "bronze wand");
                    break;
                case 8:
                    strcat(glog, "ivory wand");
                    break;
                case 9:
                    strcat(glog, "glass wand");
                    break;
                case 10:
                    strcat(glog, "lead wand");
                    break;
                case 11:
                    strcat(glog, "plastic wand");
                    break;
                }               // end of switch(randnum)

            }
            else                //ie if it's uncommon:

            {
                switch (item_da / 12)   // secondary characteristic of wand

                {
                case 1:
                    strcat(glog, "jewelled ");
                    break;
                case 2:
                    strcat(glog, "curved ");
                    break;
                case 3:
                    strcat(glog, "long ");
                    break;
                case 4:
                    strcat(glog, "short ");
                    break;
                case 5:
                    strcat(glog, "twisted ");
                    break;
                case 6:
                    strcat(glog, "crooked ");
                    break;
                case 7:
                    strcat(glog, "forked ");
                    break;
                case 8:
                    strcat(glog, "shiny ");
                    break;
                case 9:
                    strcat(glog, "blackened ");
                    break;
                case 10:
                    strcat(glog, "tapered ");
                    break;
                case 11:
                    strcat(glog, "glowing ");
                    break;
                case 12:
                    strcat(glog, "worn ");
                    break;
                case 13:
                    strcat(glog, "encrusted ");
                    break;
                case 14:
                    strcat(glog, "runed ");
                    break;
                case 15:
                    strcat(glog, "sharpened ");
                    break;
                }               // end of switch(randnum)

                switch ((int) item_da % 12)
                {
                case 0:
                    strcat(glog, "iron wand");
                    break;
                case 1:
                    strcat(glog, "brass wand");
                    break;
                case 2:
                    strcat(glog, "bone wand");
                    break;
                case 3:
                    strcat(glog, "wooden wand");
                    break;
                case 4:
                    strcat(glog, "copper wand");
                    break;
                case 5:
                    strcat(glog, "gold wand");
                    break;
                case 6:
                    strcat(glog, "silver wand");
                    break;
                case 7:
                    strcat(glog, "bronze wand");
                    break;
                case 8:
                    strcat(glog, "ivory wand");
                    break;
                case 9:
                    strcat(glog, "glass wand");
                    break;
                case 10:
                    strcat(glog, "lead wand");
                    break;
                case 11:
                    strcat(glog, "plastic wand");
                    break;
                }               // end of switch(randnum)

            }                   // end of if uncommon wand

        }                       // end of else for ident_lev

        if (ident_lev > 1)
        {
            strcat(glog, " (");
            itoa(it_plus, tmp_quant, 10);
            if (it_plus > 80)
                itoa((it_plus - 100), tmp_quant, 10);
            strcat(glog, tmp_quant);
            strcat(glog, ")");
        }
        break;

    case OBJ_POTIONS:           // potions

        if (id[3][item_typ] == 1)
        {
            if (it_quant == 1)
            {
                strcat(glog, "potion of ");
            }
            else
            {
                strcat(glog, "potions of ");
            }
        }

/*
   Remember, potions food and scrolls stack on the basis of class and type only.
 */
        if (id[3][item_typ] == 1)
        {
            switch (item_typ)
            {
            case POT_HEALING:
                strcat(glog, "healing");
                break;
            case POT_HEAL_WOUNDS:
                strcat(glog, "heal wounds");
                break;
            case POT_SPEED:
                strcat(glog, "speed");
                break;
            case POT_MIGHT:
                strcat(glog, "might");
                break;
            case POT_GAIN_STRENGTH:
                strcat(glog, "gain strength");
                break;
            case POT_GAIN_DEXTERITY:
                strcat(glog, "gain dexterity");
                break;
            case POT_GAIN_INTELLIGENCE:
                strcat(glog, "gain intelligence");
                break;
            case POT_LEVITATION:
                strcat(glog, "levitation");
                break;
            case POT_POISON:
                strcat(glog, "poison");
                break;
            case POT_SLOWING:
                strcat(glog, "slowing");
                break;
            case POT_PARALYSIS:
                strcat(glog, "paralysis");
                break;
            case POT_CONFUSION:
                strcat(glog, "confusion");
                break;
            case POT_INVISIBILITY:
                strcat(glog, "invisibility");
                break;
            case POT_PORRIDGE:
                strcat(glog, "porridge");
                break;
            case POT_DEGENERATION:
                strcat(glog, "degeneration");
                break;
            case POT_DECAY:
                strcat(glog, "decay");
                break;          // Rare!

            case POT_WATER:
                strcat(glog, "water");
                break;
            case POT_EXPERIENCE:
                strcat(glog, "experience");
                break;
            case POT_MAGIC:
                strcat(glog, "magic");
                break;
            case POT_RESTORE_ABILITIES:
                strcat(glog, "restore abilities");
                break;
            case POT_STRONG_POISON:
                strcat(glog, "strong poison");
                break;
            case POT_BERSERK_RAGE:
                strcat(glog, "berserk rage");
                break;
            case POT_CURE_MUTATION:
                strcat(glog, "cure mutation");
                break;
            case POT_MUTATION:
                strcat(glog, "mutation");
                break;
            }
            break;
        }                       // end of if ident_lev

        if (item_da <= 13)
        {
            switch (item_da)
            {
            case 0:
                strcat(glog, "clear potion");
                break;
            case 1:
                strcat(glog, "blue potion");
                break;
            case 2:
                strcat(glog, "black potion");
                break;
            case 3:
                strcat(glog, "silvery potion");
                break;
            case 4:
                strcat(glog, "cyan potion");
                break;
            case 5:
                strcat(glog, "purple potion");
                break;
            case 6:
                strcat(glog, "orange potion");
                break;
            case 7:
                strcat(glog, "inky potion");
                break;
            case 8:
                strcat(glog, "red potion");
                break;
            case 9:
                strcat(glog, "yellow potion");
                break;
            case 10:
                strcat(glog, "green potion");
                break;
            case 11:
                strcat(glog, "brown potion");
                break;
            case 12:
                strcat(glog, "pink potion");
                break;
            case 13:
                strcat(glog, "white potion");
                break;
            }                   // end of switch(randnum)

        }
        else
        {
            switch (item_da / 14)       // secondary characteristic of potion

            {
            case 0:
                strcat(glog, "bubbling ");
                break;
            case 1:
                strcat(glog, "lumpy ");
                break;
            case 2:
                strcat(glog, "fuming ");
                break;
            case 3:
                strcat(glog, "smoky ");
                break;
            case 4:
                strcat(glog, "fizzy ");
                break;
            case 5:
                strcat(glog, "glowing ");
                break;
            case 6:
                strcat(glog, "sedimented ");
                break;
            case 7:
                strcat(glog, "metallic ");
                break;
            case 8:
                strcat(glog, "murky ");
                break;
            case 9:
                strcat(glog, "gluggy ");
                break;
            case 10:
                strcat(glog, "viscous ");
                break;
            case 11:
                strcat(glog, "oily ");
                break;
            case 12:
                strcat(glog, "slimy ");
                break;
            }                   // end of switch(randnum)

            switch (item_da % 14)
            {
            case 0:
                strcat(glog, "clear potion");
                break;
            case 1:
                strcat(glog, "blue potion");
                break;
            case 2:
                strcat(glog, "black potion");
                break;
            case 3:
                strcat(glog, "silvery potion");
                break;
            case 4:
                strcat(glog, "cyan potion");
                break;
            case 5:
                strcat(glog, "purple potion");
                break;
            case 6:
                strcat(glog, "orange potion");
                break;
            case 7:
                strcat(glog, "inky potion");
                break;
            case 8:
                strcat(glog, "red potion");
                break;
            case 9:
                strcat(glog, "yellow potion");
                break;
            case 10:
                strcat(glog, "green potion");
                break;
            case 11:
                strcat(glog, "brown potion");
                break;
            case 12:
                strcat(glog, "pink potion");
                break;
            case 13:
                strcat(glog, "white potion");
                break;
            }                   // end of switch(randnum)

        }                       // end of else.

        if (it_quant > 1)
            strcat(glog, "s");
        break;

    case OBJ_FOOD:              // food

        switch (item_typ)
        {
        case FOOD_MEAT_RATION:
            strcat(glog, "meat ration");
            break;
        case FOOD_BREAD_RATION:
            strcat(glog, "bread ration");
            break;
        case FOOD_PEAR:
            strcat(glog, "pear");
            break;
        case FOOD_APPLE:
            strcat(glog, "apple");
            break;              // make this less common.

        case FOOD_CHOKO:
            strcat(glog, "choko");
            break;
        case FOOD_HONEYCOMB:
            strcat(glog, "honeycomb");
            break;
        case FOOD_ROYAL_JELLY:
            strcat(glog, "royal jell");
            break;              // maybe a joke monster of the same name? - mix something with jelly crystals?

        case FOOD_SNOZZCUMBER:
            strcat(glog, "snozzcumber");
            break;
        case FOOD_PIZZA:
            strcat(glog, "slice of pizza");
            break;
        case FOOD_APRICOT:
            strcat(glog, "apricot");
            break;
        case FOOD_ORANGE:
            strcat(glog, "orange");
            break;
        case FOOD_BANANA:
            strcat(glog, "banana");
            break;
        case FOOD_STRAWBERRY:
            strcat(glog, "strawberr");
            break;
        case FOOD_RAMBUTAN:
            strcat(glog, "rambutan");
            break;
        case FOOD_LEMON:
            strcat(glog, "lemon");
            break;
        case FOOD_GRAPE:
            strcat(glog, "grape");
            break;
        case FOOD_SULTANA:
            strcat(glog, "sultana");
            break;
        case FOOD_LYCHEE:
            strcat(glog, "lychee");
            break;
        case FOOD_BEEF_JERKY:
            strcat(glog, "beef jerk");
            break;
        case FOOD_CHEESE:
            strcat(glog, "cheese");
            break;
        case FOOD_SAUSAGE:
            strcat(glog, "sausage");
            break;
        case FOOD_CHUNK:
            char gmo_n2[40];

            moname(it_plus, 0, 1, 100, gmo_n2);
            if (item_da < 100)
                strcat(glog, "rotting ");
            strcat(glog, "chunk");
            if (it_quant > 1)
                strcat(glog, "s");
            strcat(glog, " of ");
            strcat(glog, gmo_n2);
            strcat(glog, " flesh");
            break;

// Note: If I add another food type, must set for carnivorous chars (Kobolds and mutants)
        }

        if (item_typ == FOOD_ROYAL_JELLY || item_typ == FOOD_STRAWBERRY || item_typ == FOOD_BEEF_JERKY)
        {
            if (it_quant > 1)
            {
                strcat(glog, "ie");
            }
            else
                strcat(glog, "y");
        }
        break;

    case OBJ_SCROLLS:           // scrolls

        if (it_quant == 1)
        {
            strcat(glog, "scroll ");
        }
        else
        {
            strcat(glog, "scrolls ");
        }

        if (id[1][item_typ] == 1)
        {

            switch (item_typ)
            {
            case SCR_IDENTIFY:
                strcat(glog, "of identify");
                break;
            case SCR_TELEPORTATION:
                strcat(glog, "of teleportation");
                break;
            case SCR_FEAR:
                strcat(glog, "of fear");
                break;
            case SCR_NOISE:
                strcat(glog, "of noise");
                break;
            case SCR_REMOVE_CURSE:
                strcat(glog, "of remove curse");
                break;
            case SCR_DETECT_CURSE:
                strcat(glog, "of detect curse");
                break;
            case SCR_SUMMONING:
                strcat(glog, "of summoning");
                break;
            case SCR_ENCHANT_WEAPON_I:
                strcat(glog, "of enchant weapon I");
                break;
            case SCR_ENCHANT_ARMOUR:
                strcat(glog, "of enchant armour");
                break;
            case SCR_TORMENT:
                strcat(glog, "of torment");
                break;
            case SCR_RANDOM_USELESSNESS:
                strcat(glog, "of random uselessness");
                break;
            case SCR_CURSE_WEAPON:
                strcat(glog, "of curse weapon");
                break;
            case SCR_CURSE_ARMOUR:
                strcat(glog, "of curse armour");
                break;
            case SCR_IMMOLATION:
                strcat(glog, "of immolation");
                break;
            case SCR_BLINKING:
                strcat(glog, "of blinking");
                break;
            case SCR_PAPER:
                strcat(glog, "of paper");
                break;
            case SCR_MAGIC_MAPPING:
                strcat(glog, "of magic mapping");
                break;
            case SCR_FORGETFULNESS:
                strcat(glog, "of forgetfulness");
                break;
            case SCR_ACQUIREMENT:
                strcat(glog, "of acquirement");
                break;
            case SCR_ENCHANT_WEAPON_II:
                strcat(glog, "of enchant weapon II");
                break;
            case SCR_VORPALISE_WEAPON:
                strcat(glog, "of vorpalise weapon");
                break;
            case SCR_RECHARGING:
                strcat(glog, "of recharging");
                break;
            case SCR_ENCHANT_WEAPON_III:
                strcat(glog, "of enchant weapon III");
                break;
//        case 23: strcat(glog , "of portal travel"); break;
            }
            break;

        }

        strcat(glog, "labelled ");

        char str_pass[50];

        make_name(item_da, it_plus, item_clas, 2, str_pass);
        strcat(glog, str_pass);

        break;                  // end of scrolls


    case OBJ_JEWELLERY: // jewellery

        if (ident_lev > 0)
        {
            if (it_plus >= 80)
            {
                strcat(glog, "cursed ");
            }
        }

        if (ident_lev > 1 || id[2][item_typ] > 0)
        {
            if (item_da == 200 || item_da == 201)
            {
                strcat(glog, randart_ring_name(item_clas, item_typ, item_da, it_plus, item_plus2, ident_lev));
                break;
            }
        }

        if (id[2][item_typ] > 0)
        {

            if (ident_lev > 1 && (item_typ == RING_PROTECTION || item_typ == RING_STRENGTH || item_typ == RING_SLAYING || item_typ == RING_EVASION || item_typ == RING_DEXTERITY || item_typ == RING_INTELLIGENCE))
            {
                char gokh = it_plus - 50;

                if (gokh >= 0 && (gokh <= 80 || gokh >= 100))
                    strcat(glog, "+");
                itoa(gokh, tmp_quant, 10);
                if (gokh > 80)
                    itoa((gokh - 100), tmp_quant, 10);
                strcat(glog, tmp_quant);
                if (item_typ == RING_SLAYING)
                {
                    strcat(glog, ",");
                    if (item_plus2 >= 50)
                        strcat(glog, "+");
                    itoa(item_plus2 - 50, tmp_quant, 10);
                    strcat(glog, tmp_quant);
                }
                strcat(glog, " ");
            }

            switch (item_typ)
            {
            case RING_REGENERATION:
                strcat(glog, "ring of regeneration");
                break;
            case RING_PROTECTION:
                strcat(glog, "ring of protection");
                break;
            case RING_PROTECTION_FROM_FIRE:
                strcat(glog, "ring of protection from fire");
                break;
            case RING_POISON_RESISTANCE:
                strcat(glog, "ring of poison resistance");
                break;
            case RING_PROTECTION_FROM_COLD:
                strcat(glog, "ring of protection from cold");
                break;
            case RING_STRENGTH:
                strcat(glog, "ring of strength");
                break;
            case RING_SLAYING:
                strcat(glog, "ring of slaying");
                break;
            case RING_SEE_INVISIBLE:
                strcat(glog, "ring of see invisible");
                break;
            case RING_INVISIBILITY:
                strcat(glog, "ring of invisibility");
                break;
            case RING_HUNGER:
                strcat(glog, "ring of hunger");
                break;
            case RING_TELEPORTATION:
                strcat(glog, "ring of teleportation");
                break;
            case RING_EVASION:
                strcat(glog, "ring of evasion");
                break;
            case RING_SUSTAIN_ABILITIES:
                strcat(glog, "ring of sustain abilities");
                break;
            case RING_SUSTENANCE:
                strcat(glog, "ring of sustenance");
                break;
            case RING_DEXTERITY:
                strcat(glog, "ring of dexterity");
                break;
            case RING_INTELLIGENCE:
                strcat(glog, "ring of intelligence");
                break;
            case RING_WIZARDRY:
                strcat(glog, "ring of wizardry");
                break;
            case RING_MAGICAL_POWER:
                strcat(glog, "ring of magical power");
                break;
            case RING_LEVITATION:
                strcat(glog, "ring of levitation");
                break;
            case RING_LIFE_PROTECTION:
                strcat(glog, "ring of life protection");
                break;
            case RING_PROTECTION_FROM_MAGIC:
                strcat(glog, "ring of protection from magic");
                break;
            case RING_FIRE:
                strcat(glog, "ring of fire");
                break;
            case RING_ICE:
                strcat(glog, "ring of ice");
                break;
            case RING_TELEPORT_CONTROL:
                strcat(glog, "ring of teleport control");
                break;

            case AMU_RAGE:
                strcat(glog, "amulet of rage");
                break;
            case AMU_MAINTAIN_SPEED:
                strcat(glog, "amulet of maintain speed");
                break;
            case AMU_CLARITY:
                strcat(glog, "amulet of clarity");
                break;
            case AMU_WARDING:
                strcat(glog, "amulet of warding");
                break;
            case AMU_RESIST_CORROSION:
                strcat(glog, "amulet of resist corrosion");
                break;
            case AMU_THE_GOURMAND:
                strcat(glog, "amulet of the gourmand");
                break;
            case AMU_CONSERVATION:
                strcat(glog, "amulet of conservation");
                break;
            case AMU_CONTROLLED_FLIGHT:
                strcat(glog, "amulet of controlled flight");
                break;
            case AMU_INACCURACY:
                strcat(glog, "amulet of inaccuracy");
                break;
            case AMU_RESIST_MUTATION:
                strcat(glog, "amulet of resist mutation");
                break;
            }
/* ? of imputed learning - 100% exp from tames/summoned kills */
            break;
        }                       // end of if ident_lev

        if (item_typ < AMU_RAGE)        // rings

        {
            if (item_da == 200 || item_da == 201)
            {
                strcat(glog, randart_ring_name(item_clas, item_typ, item_da, it_plus, item_plus2, ident_lev));
                break;
            }
            switch (item_da / 13)       // secondary characteristic of ring

            {
            case 1:
                strcat(glog, "encrusted ");
                break;
            case 2:
                strcat(glog, "glowing ");
                break;
            case 3:
                strcat(glog, "tubular ");
                break;
            case 4:
                strcat(glog, "runed ");
                break;
            case 5:
                strcat(glog, "blackened ");
                break;
            case 6:
                strcat(glog, "scratched ");
                break;
            case 7:
                strcat(glog, "small ");
                break;
            case 8:
                strcat(glog, "large ");
                break;
            case 9:
                strcat(glog, "twisted ");
                break;
            case 10:
                strcat(glog, "shiny ");
                break;
            case 11:
                strcat(glog, "notched ");
                break;
            case 12:
                strcat(glog, "knobbly ");
                break;
            }                   // end of switch(randnum)


            switch (item_da % 13)
            {
            case 0:
                strcat(glog, "wooden ring");
                break;
            case 1:
                strcat(glog, "silver ring");
                break;
            case 2:
                strcat(glog, "golden ring");
                break;
            case 3:
                strcat(glog, "iron ring");
                break;
            case 4:
                strcat(glog, "steel ring");
                break;
            case 5:
                strcat(glog, "bronze ring");
                break;
            case 6:
                strcat(glog, "brass ring");
                break;
            case 7:
                strcat(glog, "copper ring");
                break;
            case 8:
                strcat(glog, "granite ring");
                break;
            case 9:
                strcat(glog, "ivory ring");
                break;
            case 10:
                strcat(glog, "bone ring");
                break;
            case 11:
                strcat(glog, "marble ring");
                break;
            case 12:
                strcat(glog, "jade ring");
                break;
            case 13:
                strcat(glog, "glass ring");
                break;
            }                   // end of switch(randnum)

        }                       // end of rings

        else                    // ie is an amulet

        {
            if (item_da == 200 || item_da == 201)
            {
                strcat(glog, randart_ring_name(item_clas, item_typ, item_da, it_plus, item_plus2, ident_lev));
                break;
            }
            if (item_da > 13)
                switch (item_da / 13)   // secondary characteristic of amulet

                {
                case 0:
                    strcat(glog, "dented ");
                    break;
                case 1:
                    strcat(glog, "square ");
                    break;
                case 2:
                    strcat(glog, "thick ");
                    break;
                case 3:
                    strcat(glog, "thin ");
                    break;
                case 4:
                    strcat(glog, "runed ");
                    break;
                case 5:
                    strcat(glog, "blackened ");
                    break;
                case 6:
                    strcat(glog, "glowing ");
                    break;
                case 7:
                    strcat(glog, "small ");
                    break;
                case 8:
                    strcat(glog, "large ");
                    break;
                case 9:
                    strcat(glog, "twisted ");
                    break;
                case 10:
                    strcat(glog, "tiny ");
                    break;
                case 11:
                    strcat(glog, "triangular ");
                    break;
                case 12:
                    strcat(glog, "lumpy ");
                    break;
                }               // end of switch(randnum)


            switch (item_da % 13)
            {
            case 0:
                strcat(glog, "zirconium amulet");
                break;
            case 1:
                strcat(glog, "sapphire amulet");
                break;
            case 2:
                strcat(glog, "golden amulet");
                break;
            case 3:
                strcat(glog, "emerald amulet");
                break;
            case 4:
                strcat(glog, "garnet amulet");
                break;
            case 5:
                strcat(glog, "bronze amulet");
                break;
            case 6:
                strcat(glog, "brass amulet");
                break;
            case 7:
                strcat(glog, "copper amulet");
                break;
            case 8:
                strcat(glog, "ruby amulet");
                break;
            case 9:
                strcat(glog, "ivory amulet");
                break;
            case 10:
                strcat(glog, "bone amulet");
                break;
            case 11:
                strcat(glog, "platinum amulet");
                break;
            case 12:
                strcat(glog, "jade amulet");
                break;
            case 13:
                strcat(glog, "plastic amulet");
                break;
            }                   // end of switch(randnum)

        }                       // end of amulets

        break;

    case OBJ_MISCELLANY:        // Miscellaneous

        switch (ident_lev)
        {
        case 0:
            switch (item_typ)
            {
            case MISC_BOTTLED_EFREET:
                strcat(glog, "sealed bronze flask");
                break;
            case MISC_CRYSTAL_BALL_OF_SEEING:
                strcat(glog, "crystal ball");
                break;
            case MISC_AIR_ELEMENTAL_FAN:
                strcat(glog, "fan");
                break;
            case MISC_LAMP_OF_FIRE:
                strcat(glog, "lamp");
                break;
            case MISC_STONE_OF_EARTH_ELEMENTALS:
                strcat(glog, "lump of rock");
                break;
            case MISC_LANTERN_OF_SHADOWS:
                strcat(glog, "bone lantern");
                break;
            case MISC_HORN_OF_GERYON:
                strcat(glog, "silver horn");
                break;
            case MISC_BOX_OF_BEASTS:
                strcat(glog, "small ebony casket");
                break;
            case MISC_DECK_OF_WONDERS:
                strcat(glog, "deck of cards");
                break;
            case MISC_DECK_OF_SUMMONINGS:
                strcat(glog, "deck of cards");
                break;
            case MISC_CRYSTAL_BALL_OF_ENERGY:
                strcat(glog, "crystal ball");
                break;
            case MISC_EMPTY_EBONY_CASKET:
                strcat(glog, "small ebony casket");
                break;
            case MISC_CRYSTAL_BALL_OF_FIXATION:
                strcat(glog, "crystal ball");
                break;
            case MISC_DISC_OF_STORMS:
                strcat(glog, "grey disc");
                break;
            case MISC_RUNE_OF_ZOT:
                switch (it_plus)
                {
                case 1:
                    strcat(glog, "iron ");
                    break;
                case 2:
                    strcat(glog, "obsidian ");
                    break;
                case 4:
                    strcat(glog, "icy ");
                    break;
                case 5:
                    strcat(glog, "bone ");
                    break;
                case 13:
                    strcat(glog, "slimy ");
                    break;
                case 14:
                    strcat(glog, "silver ");
                    break;
                case 19:
                    strcat(glog, "serpentine ");
                    break;
                case 20:
                    strcat(glog, "elven ");
                    break;
                case 21:
                    strcat(glog, "golden ");
                    break;
                case 22:
                    strcat(glog, "decaying ");
                    break;
                case 50:
                    strcat(glog, "demonic ");
                    break;      /* found in pandemonium */
                case 51:
                    strcat(glog, "abyssal ");
                    break;      /* found in abyss */
                }               /* If more are added here, must also add below. */
                strcat(glog, "rune");
                break;
            case MISC_DECK_OF_TRICKS:
                strcat(glog, "deck of cards");
                break;
            case MISC_DECK_OF_POWER:
                strcat(glog, "deck of cards");
                break;
            case MISC_PORTABLE_ALTAR_OF_NEMELEX:
                strcat(glog, "portable altar of Nemelex");
                break;

            }
            break;

        default:
            switch (item_typ)
            {
            case MISC_BOTTLED_EFREET:
                strcat(glog, "bottled efreet");
                break;
            case MISC_CRYSTAL_BALL_OF_SEEING:
                strcat(glog, "crystal ball of seeing");
                break;
            case MISC_AIR_ELEMENTAL_FAN:
                strcat(glog, "air elemental fan");
                break;
            case MISC_LAMP_OF_FIRE:
                strcat(glog, "lamp of fire");
                break;
            case MISC_STONE_OF_EARTH_ELEMENTALS:
                strcat(glog, "stone of earth elementals");
                break;
            case MISC_LANTERN_OF_SHADOWS:
                strcat(glog, "lantern of shadows");
                break;
            case MISC_HORN_OF_GERYON:
                strcat(glog, "Horn of Geryon");
                break;

            case MISC_BOX_OF_BEASTS:
                strcat(glog, "box of beasts");
                break;
            case MISC_DECK_OF_WONDERS:
                strcat(glog, "deck of wonders");
                break;
            case MISC_DECK_OF_SUMMONINGS:
                strcat(glog, "deck of summonings");
                break;
            case MISC_CRYSTAL_BALL_OF_ENERGY:
                strcat(glog, "crystal ball of energy");
                break;
            case MISC_EMPTY_EBONY_CASKET:
                strcat(glog, "empty ebony casket");
                break;
            case MISC_CRYSTAL_BALL_OF_FIXATION:
                strcat(glog, "crystal ball of fixation");
                break;
            case MISC_DISC_OF_STORMS:
                strcat(glog, "disc of storms");
                break;
            case MISC_RUNE_OF_ZOT:
                switch (it_plus)
                {
                case 1:
                    strcat(glog, "iron ");
                    break;
                case 2:
                    strcat(glog, "obsidian ");
                    break;
                case 4:
                    strcat(glog, "icy ");
                    break;
                case 5:
                    strcat(glog, "bone ");
                    break;
                case 13:
                    strcat(glog, "slimy ");
                    break;
                case 14:
                    strcat(glog, "silver ");
                    break;
                case 19:
                    strcat(glog, "serpentine ");
                    break;
                case 20:
                    strcat(glog, "elven ");
                    break;
                case 21:
                    strcat(glog, "golden ");
                    break;
                case 22:
                    strcat(glog, "decaying ");
                    break;
                case 50:
                    strcat(glog, "demonic ");
                    break;      /* found in pandemonium */
                case 51:
                    strcat(glog, "abyssal ");
                    break;      /* found in abyss */
                }               /* If more are added here, must also add above. */
                strcat(glog, "rune of Zot");
                break;
            case MISC_DECK_OF_TRICKS:
                strcat(glog, "deck of tricks");
                break;
            case MISC_DECK_OF_POWER:
                strcat(glog, "deck of power");
                break;
            case MISC_PORTABLE_ALTAR_OF_NEMELEX:
                strcat(glog, "portable altar of Nemelex");
                break;
            }
            break;

        }                       // end switch ident_lev

        break;


    case OBJ_BOOKS:
        strcpy(glog, "");
        if (ident_lev == 0)
        {
            switch (item_da / 10)
            {
            case 0:
                strcat(glog, "");
                break;
            case 1:
                strcat(glog, "chunky ");
                break;
            case 2:
                strcat(glog, "thick ");
                break;
            case 3:
                strcat(glog, "thin ");
                break;
            case 4:
                strcat(glog, "wide ");
                break;
            case 5:
                strcat(glog, "glowing ");
                break;
            case 6:
                strcat(glog, "dog-eared ");
                break;
            case 7:
                strcat(glog, "oblong ");
                break;
            case 8:
                strcat(glog, "runed ");
                break;
            case 9:
                strcat(glog, " ");
                break;
            case 10:
                strcat(glog, " ");
                break;
            case 11:
                strcat(glog, " ");
                break;

            }
            switch (item_da % 10)
            {
            case 0:
                strcat(glog, "paperback book");
                break;
            case 1:
                strcat(glog, "hardcover book");
                break;
            case 2:
                strcat(glog, "leatherbound book");
                break;
            case 3:
                strcat(glog, "metal-bound book");
                break;
            case 4:
                strcat(glog, "papyrus book");
                break;
            case 5:
                strcat(glog, "book");
                break;
            case 6:
                strcat(glog, "book");
                break;
            }
        }
        else
            switch (item_typ)
            {
            case BOOK_MINOR_MAGIC_I:
                strcat(glog, "book of Minor Magic");
                break;
            case BOOK_MINOR_MAGIC_II:
                strcat(glog, "book of Minor Magic");
                break;
            case BOOK_MINOR_MAGIC_III:
                strcat(glog, "book of Minor Magic");
                break;
            case BOOK_CONJURATIONS_I:
                strcat(glog, "book of Conjurations");
                break;
            case BOOK_CONJURATIONS_II:
                strcat(glog, "book of Conjurations");
                break;
            case BOOK_FLAMES:
                strcat(glog, "book of Flames");
                break;
            case BOOK_FROST:
                strcat(glog, "book of Frost");
                break;
            case BOOK_INVOCATIONS:
                strcat(glog, "book of Invocations");
                break;
            case BOOK_FIRE:
                strcat(glog, "book of Fire");
                break;
            case BOOK_ICE:
                strcat(glog, "book of Ice");
                break;
            case BOOK_SURVEYANCES:
                strcat(glog, "book of Surveyances");
                break;
            case BOOK_SPATIAL_TRANSLOCATIONS:
                strcat(glog, "book of Spatial Translocations");
                break;
            case BOOK_ENCHANTMENTS:
                strcat(glog, "book of Enchantments");
                break;
            case BOOK_POISONINGS:
                strcat(glog, "book of Poisonings");
                break;
            case BOOK_STORMS_AND_FIRE:
                strcat(glog, "book of Storms and Fire");
                break;
            case BOOK_DEATH:
                strcat(glog, "book of Death");
                break;
            case BOOK_HINDERANCE:
                strcat(glog, "book of Hinderance");
                break;
            case BOOK_CHANGES:
                strcat(glog, "book of Changes");
                break;
            case BOOK_TRANSFIGURATIONS:
                strcat(glog, "book of Transfigurations");
                break;
            case BOOK_USEFUL_MAGIC:
                strcat(glog, "book of Useful Magic");
                break;
            case BOOK_WAR_CHANTS:
                strcat(glog, "book of War Chants");
                break;
            case BOOK_CLOUDS:
                strcat(glog, "book of Clouds");
                break;
            case BOOK_HEALING:
                strcat(glog, "book of Healing");
                break;
            case BOOK_NECROMANCY:
                strcat(glog, "book of Necromancy");
                break;
            case BOOK_NECRONOMICON:
                strcat(glog, "Necronomicon");
                break;
            case BOOK_SUMMONINGS:
                strcat(glog, "book of Summonings");
                break;
            case BOOK_CHARMS:
                strcat(glog, "book of Charms");
                break;
            case BOOK_DEMONOLOGY:
                strcat(glog, "book of Demonology");
                break;

            case BOOK_AIR:
                strcat(glog, "book of Air");
                break;
            case BOOK_SKY:
                strcat(glog, "book of the Sky");
                break;
            case BOOK_DIVINATIONS:
                strcat(glog, "book of Divinations");
                break;
            case BOOK_WARP:
                strcat(glog, "book of the Warp");
                break;
            case BOOK_ENVENOMATIONS:
                strcat(glog, "book of Envenomations");
                break;
            case BOOK_ANNIHILATIONS:
                strcat(glog, "book of Annihilations");
                break;
            case BOOK_UNLIFE:
                strcat(glog, "book of Unlife");
                break;

            case BOOK_DESTRUCTION:
                strcat(glog, "tome of destruction");
                break;

            case BOOK_CONTROL:
                strcat(glog, "book of Control");
                break;
            case BOOK_MUTATIONS:
                strcat(glog, "book of Mutations");
                break;
            case BOOK_TUKIMA:
                strcat(glog, "book of Tukima");
                break;
            case BOOK_GEOMANCY:
                strcat(glog, "book of Geomancy");
                break;
            case BOOK_EARTH:
                strcat(glog, "book of Earth");
                break;

            case BOOK_MANUAL:
                strcat(glog, "manual of ");
                strcat(glog, skill_name(it_plus));
                break;

            case BOOK_WIZARDRY:
                strcat(glog, "book of Wizardry");
                break;
            case BOOK_POWER:
                strcat(glog, "book of Power");
                break;

            }                   // end switch
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


    case OBJ_STAVES:            // more magical devices

        if (ident_lev == 0)
        {
            switch (item_da)    // gnarled

            {
            case 0:
                strcat(glog, "curved ");
                break;
            case 1:
                strcat(glog, "glowing ");
                break;
            case 2:
                strcat(glog, "thick ");
                break;
            case 3:
                strcat(glog, "thin ");
                break;
            case 4:
                strcat(glog, "long ");
                break;
            case 5:
                strcat(glog, "twisted ");
                break;
            case 6:
                strcat(glog, "jewelled ");
                break;
            case 7:
                strcat(glog, "runed ");
                break;
            case 8:
                strcat(glog, "smoking ");
                break;
            case 9:
                strcat(glog, " ");
                break;
            case 10:
                strcat(glog, " ");
                break;
            case 11:
                strcat(glog, " ");
                break;
            case 12:
                strcat(glog, " ");
                break;
            case 13:
                strcat(glog, " ");
                break;
            case 14:
                strcat(glog, " ");
                break;
            case 15:
                strcat(glog, " ");
                break;
            case 16:
                strcat(glog, " ");
                break;
            case 17:
                strcat(glog, " ");
                break;
            case 18:
                strcat(glog, " ");
                break;
            case 19:
                strcat(glog, " ");
                break;
            case 20:
                strcat(glog, " ");
                break;
            case 21:
                strcat(glog, " ");
                break;
            case 22:
                strcat(glog, " ");
                break;
            case 23:
                strcat(glog, " ");
                break;
            case 24:
                strcat(glog, " ");
                break;
            case 25:
                strcat(glog, " ");
                break;
            case 26:
                strcat(glog, " ");
                break;
            case 27:
                strcat(glog, " ");
                break;
            case 28:
                strcat(glog, " ");
                break;
            case 29:
                strcat(glog, " ");
                break;
            }                   // end switch

        }                       // end if

        if (ident_lev > 0 && item_typ >= STAFF_SMITING && item_typ < STAFF_AIR)
            strcat(glog, "spell ");

        strcat(glog, "staff");

        if (ident_lev > 0)
        {
            switch (item_typ)
            {
            case STAFF_WIZARDRY:
                strcat(glog, " of wizardry");
                break;
            case STAFF_POWER:
                strcat(glog, " of power");
                break;
            case STAFF_FIRE:
                strcat(glog, " of fire");
                break;
            case STAFF_COLD:
                strcat(glog, " of cold");
                break;
            case STAFF_POISON:
                strcat(glog, " of poison");
                break;
            case STAFF_ENERGY:
                strcat(glog, " of energy");
                break;          // crappy name. oh well

            case STAFF_DEATH:
                strcat(glog, " of death");
                break;
            case STAFF_CONJURATION:
                strcat(glog, " of conjuration");
                break;
            case STAFF_ENCHANTMENT:
                strcat(glog, " of enchantment");
                break;
            case STAFF_SUMMONING_I:
                strcat(glog, " of summoning");
                break;

            case STAFF_SMITING:
                strcat(glog, " of smiting");
                break;
            case STAFF_SUMMONING_II:
                strcat(glog, " of summoning");
                break;
            case STAFF_DESTRUCTION_I:
                strcat(glog, " of destruction");
                break;
            case STAFF_DESTRUCTION_II:
                strcat(glog, " of destruction");
                break;
            case STAFF_DESTRUCTION_III:
                strcat(glog, " of destruction");
                break;
            case STAFF_DESTRUCTION_IV:
                strcat(glog, " of destruction");
                break;
            case STAFF_WARDING:
                strcat(glog, " of warding");
                break;
            case STAFF_DISCOVERY:
                strcat(glog, " of discovery");
                break;
            case STAFF_DEMONOLOGY:
                strcat(glog, " of demonology");
                break;

            case STAFF_AIR:
                strcat(glog, " of air");
                break;
            case STAFF_EARTH:
                strcat(glog, " of earth");
                break;
            case STAFF_CHANNELING:
                strcat(glog, " of channeling");
                break;

            }                   // end switch

        }                       // end if

        break;


    case OBJ_ORBS:
        switch (item_typ)
        {
        case ORB_ZOT:
        default:
            strcpy(glog, "Orb of Zot");
            break;
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


    case 15:                    // money

        strcat(glog, "gold piece");
//if (it_quant > 1) strcat(glog, "s");
        break;


    case 16:                    // Gems - obviously not yet implemented

        break;


    case OBJ_CORPSES:
        char gmo_n[40];

        switch (item_typ)
        {
        case CORPSE_BODY:
            moname(it_plus, 0, 1, 100, gmo_n);
            if (item_da < 100)
                strcat(glog, "rotting ");
            strcat(glog, gmo_n);
            strcat(glog, " corpse");
            break;

        case CORPSE_SKELETON:
            moname(it_plus, 0, 1, 100, gmo_n);
            strcat(glog, gmo_n);
            strcat(glog, " skeleton");
            break;
        }
        break;

    default:
        strcat(glog, "!");
    }                           // end of switch?
    //} // end of for lopp

    if (strlen(glog) < 3)
    {
        strcat(glog, "questionable item (c");
        char ugug[4];

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




    if (it_quant > 1 && item_clas != OBJ_MISSILES && item_clas != OBJ_SCROLLS && item_clas != OBJ_POTIONS && (item_clas != OBJ_FOOD || item_typ != FOOD_CHUNK))
    {
        strcat(glog, "s");
    }

    return 1;

}                               // end of char item_name_2









void save_id(char identy[4][50])
{

    char x = 0;
    char jx = 0;

    for (x = 0; x < 4; x++)
    {
        for (jx = 0; jx < 50; jx++)
        {
            identy[x][jx] = id[x][jx];
        }
    }

}


void initial()
{
    char i = 0;
    char j = 0;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 50; j++)
        {
            id[i][j] = 0;
        }
    }
}                               // end of void initial


void set_id(char cla, char ty, char setting)
{
    if (cla > 99)
    {
        cla -= 100;
        id[cla][ty] = setting;
        return;
    }

    switch (cla)
    {
    case 3:
        id[0][ty] = setting;
        break;
    case 6:
        id[1][ty] = setting;
        break;
    case 7:
        id[2][ty] = setting;
        break;
    case 8:
        id[3][ty] = setting;
        break;
        /* if it's none of these, will just return */
    }
}                               /* end of void set_id */


char get_id(char cla, char ty)
{
    if (cla > 99)
    {
        cla -= 100;
        return id[cla][ty];
    }

    switch (cla)
    {
    case 3:
        return id[0][ty];
    case 6:
        return id[1][ty];
    case 7:
        return id[2][ty];
    case 8:
        return id[3][ty];
        /* if it's none of these, will just return 0 */
    }
    return 0;

}                               /* end of char get_id */




int property(int pr1, int pr2, int pr3)
{
    return prop[pr1][pr2][pr3];
}


int mass(int pr1, int pr2)
{
    return mss[pr1][pr2];
}


void init_properties()
{
    //strcpy(all_items [2] [0], "robe");
    prop[OBJ_ARMOUR][ARM_ROBE][PARM_AC] = 1;    // AC

    prop[OBJ_ARMOUR][ARM_ROBE][PARM_EVASION] = 0;       // evasion

    mss[OBJ_ARMOUR][ARM_ROBE] = 60;

    //strcpy(all_items [2] [0], "leather armour");
    prop[OBJ_ARMOUR][ARM_LEATHER_ARMOUR][PARM_AC] = 2;  // AC

    prop[OBJ_ARMOUR][ARM_LEATHER_ARMOUR][PARM_EVASION] = -1;    // evasion

    mss[OBJ_ARMOUR][ARM_LEATHER_ARMOUR] = 150;

    //strcpy(all_items [2] [1], "ring mail");
    prop[OBJ_ARMOUR][ARM_RING_MAIL][PARM_AC] = 4;       // AC

    prop[OBJ_ARMOUR][ARM_RING_MAIL][PARM_EVASION] = -2;         // evasion

    mss[OBJ_ARMOUR][ARM_RING_MAIL] = 300;

    //strcpy(all_items [2] [2], "scale mail");
    prop[OBJ_ARMOUR][ARM_SCALE_MAIL][PARM_AC] = 5;      // AC

    prop[OBJ_ARMOUR][ARM_SCALE_MAIL][PARM_EVASION] = -2;        // evasion

    mss[OBJ_ARMOUR][ARM_SCALE_MAIL] = 400;

    //strcpy(all_items [2] [3], "chain mail");
    prop[OBJ_ARMOUR][ARM_CHAIN_MAIL][PARM_AC] = 6;      // AC

    prop[OBJ_ARMOUR][ARM_CHAIN_MAIL][PARM_EVASION] = -3;        // evasion

    mss[OBJ_ARMOUR][ARM_CHAIN_MAIL] = 450;

    //strcpy(all_items [2] [4], "splint mail");
    prop[OBJ_ARMOUR][ARM_SPLINT_MAIL][PARM_AC] = 8;     // AC

    prop[OBJ_ARMOUR][ARM_SPLINT_MAIL][PARM_EVASION] = -5;       // evasion

    mss[OBJ_ARMOUR][ARM_SPLINT_MAIL] = 550;

    //strcpy(all_items [2] [5], "banded mail");
    prop[OBJ_ARMOUR][ARM_BANDED_MAIL][PARM_AC] = 7;     // AC

    prop[OBJ_ARMOUR][ARM_BANDED_MAIL][PARM_EVASION] = -4;       // evasion

    mss[OBJ_ARMOUR][ARM_BANDED_MAIL] = 500;

    //strcpy(all_items [2] [6], "plate mail");
    prop[OBJ_ARMOUR][ARM_PLATE_MAIL][PARM_AC] = 9;      // AC

    prop[OBJ_ARMOUR][ARM_PLATE_MAIL][PARM_EVASION] = -5;        // evasion

    mss[OBJ_ARMOUR][ARM_PLATE_MAIL] = 650;

    // dragon hide
    prop[OBJ_ARMOUR][ARM_DRAGON_HIDE][PARM_AC] = 2;     // AC

    prop[OBJ_ARMOUR][ARM_DRAGON_HIDE][PARM_EVASION] = -2;       // evasion

    mss[OBJ_ARMOUR][ARM_DRAGON_HIDE] = 220;

    // troll hide
    prop[OBJ_ARMOUR][ARM_TROLL_HIDE][PARM_AC] = 1;      // AC

    prop[OBJ_ARMOUR][ARM_TROLL_HIDE][PARM_EVASION] = -1;        // evasion

    mss[OBJ_ARMOUR][ARM_TROLL_HIDE] = 180;

    // crystal plate
    prop[OBJ_ARMOUR][ARM_CRYSTAL_PLATE_MAIL][PARM_AC] = 16;     // AC

    prop[OBJ_ARMOUR][ARM_CRYSTAL_PLATE_MAIL][PARM_EVASION] = -8;        // evasion

    mss[OBJ_ARMOUR][ARM_CRYSTAL_PLATE_MAIL] = 1200;

    // dragon scale
    prop[OBJ_ARMOUR][ARM_DRAGON_ARMOUR][PARM_AC] = 10;   // AC

    prop[OBJ_ARMOUR][ARM_DRAGON_ARMOUR][PARM_EVASION] = -2;     // evasion

    mss[OBJ_ARMOUR][ARM_DRAGON_ARMOUR] = 220;

    // troll leather
    prop[OBJ_ARMOUR][ARM_TROLL_LEATHER_ARMOUR][PARM_AC] = 2;    // AC

    prop[OBJ_ARMOUR][ARM_TROLL_LEATHER_ARMOUR][PARM_EVASION] = -1;      // evasion

    mss[OBJ_ARMOUR][ARM_TROLL_LEATHER_ARMOUR] = 180;

    // ice dragon hide
    prop[OBJ_ARMOUR][ARM_ICE_DRAGON_HIDE][PARM_AC] = 2;         // AC

    prop[OBJ_ARMOUR][ARM_ICE_DRAGON_HIDE][PARM_EVASION] = -2;   // evasion

    mss[OBJ_ARMOUR][ARM_ICE_DRAGON_HIDE] = 220;

    // ice dragon scale
    prop[OBJ_ARMOUR][ARM_ICE_DRAGON_ARMOUR][PARM_AC] = 10;       // AC

    prop[OBJ_ARMOUR][ARM_ICE_DRAGON_ARMOUR][PARM_EVASION] = -2;         // evasion

    mss[OBJ_ARMOUR][ARM_ICE_DRAGON_ARMOUR] = 220;

    // steam dragon hide
    prop[OBJ_ARMOUR][ARM_STEAM_DRAGON_HIDE][PARM_AC] = 0;       // AC

    prop[OBJ_ARMOUR][ARM_STEAM_DRAGON_HIDE][PARM_EVASION] = 0;  // evasion

    mss[OBJ_ARMOUR][ARM_STEAM_DRAGON_HIDE] = 120;

    // steam dragon armour
    prop[OBJ_ARMOUR][ARM_STEAM_DRAGON_ARMOUR][PARM_AC] = 3;     // AC

    prop[OBJ_ARMOUR][ARM_STEAM_DRAGON_ARMOUR][PARM_EVASION] = 0;        // evasion

    mss[OBJ_ARMOUR][ARM_STEAM_DRAGON_ARMOUR] = 120;

    // mottled dragon hide
    prop[OBJ_ARMOUR][ARM_MOTTLED_DRAGON_HIDE][PARM_AC] = 1;     // AC

    prop[OBJ_ARMOUR][ARM_MOTTLED_DRAGON_HIDE][PARM_EVASION] = -1;       // evasion

    mss[OBJ_ARMOUR][ARM_MOTTLED_DRAGON_HIDE] = 150;

    // mottled dragon hide
    prop[OBJ_ARMOUR][ARM_MOTTLED_DRAGON_ARMOUR][PARM_AC] = 5;   // AC

    prop[OBJ_ARMOUR][ARM_MOTTLED_DRAGON_ARMOUR][PARM_EVASION] = -1;     // evasion

    mss[OBJ_ARMOUR][ARM_MOTTLED_DRAGON_ARMOUR] = 150;

    // storm dragon hide
    prop[OBJ_ARMOUR][ARM_STORM_DRAGON_HIDE][PARM_AC] = 2;       // AC

    prop[OBJ_ARMOUR][ARM_STORM_DRAGON_HIDE][PARM_EVASION] = -5;         // evasion

    mss[OBJ_ARMOUR][ARM_STORM_DRAGON_HIDE] = 400;

    // storm dragon armour
    prop[OBJ_ARMOUR][ARM_STORM_DRAGON_ARMOUR][PARM_AC] = 11;     // AC

    prop[OBJ_ARMOUR][ARM_STORM_DRAGON_ARMOUR][PARM_EVASION] = -5;       // evasion

    mss[OBJ_ARMOUR][ARM_STORM_DRAGON_ARMOUR] = 400;

    // gold dragon hide
    prop[OBJ_ARMOUR][ARM_GOLD_DRAGON_HIDE][PARM_AC] = 2;        // AC

    prop[OBJ_ARMOUR][ARM_GOLD_DRAGON_HIDE][PARM_EVASION] = -10;         // evasion

    mss[OBJ_ARMOUR][ARM_GOLD_DRAGON_HIDE] = 1100;

    // gold dragon armour
    prop[OBJ_ARMOUR][ARM_GOLD_DRAGON_ARMOUR][PARM_AC] = 12;     // AC

    prop[OBJ_ARMOUR][ARM_GOLD_DRAGON_ARMOUR][PARM_EVASION] = -10;       // evasion

    mss[OBJ_ARMOUR][ARM_GOLD_DRAGON_ARMOUR] = 1100;

    // animal skin
    prop[OBJ_ARMOUR][ARM_ANIMAL_SKIN][PARM_AC] = 1;     // AC

    prop[OBJ_ARMOUR][ARM_ANIMAL_SKIN][PARM_EVASION] = 0;        // evasion

    mss[OBJ_ARMOUR][ARM_ANIMAL_SKIN] = 100;

    // swamp dragon hide
    prop[OBJ_ARMOUR][ARM_SWAMP_DRAGON_HIDE][PARM_AC] = 1;       // AC

    prop[OBJ_ARMOUR][ARM_SWAMP_DRAGON_HIDE][PARM_EVASION] = -2;         // evasion

    mss[OBJ_ARMOUR][ARM_SWAMP_DRAGON_HIDE] = 200;

    // swamp dragon hide
    prop[OBJ_ARMOUR][ARM_SWAMP_DRAGON_ARMOUR][PARM_AC] = 8;     // AC

    prop[OBJ_ARMOUR][ARM_SWAMP_DRAGON_ARMOUR][PARM_EVASION] = -2;       // evasion

    mss[OBJ_ARMOUR][ARM_SWAMP_DRAGON_ARMOUR] = 200;



    // other armour:

    // shield:
    prop[OBJ_ARMOUR][ARM_SHIELD][PARM_AC] = 0;  // AC

    prop[OBJ_ARMOUR][ARM_SHIELD][PARM_EVASION] = 0;     // evasion

    mss[OBJ_ARMOUR][ARM_SHIELD] = 100;

    // cloak
    prop[OBJ_ARMOUR][ARM_CLOAK][PARM_AC] = 1;   // AC

    prop[OBJ_ARMOUR][ARM_CLOAK][PARM_EVASION] = 0;      // evasion

    mss[OBJ_ARMOUR][ARM_CLOAK] = 20;

    // helmet
    prop[OBJ_ARMOUR][ARM_HELMET][PARM_AC] = 1;  // AC

    prop[OBJ_ARMOUR][ARM_HELMET][PARM_EVASION] = 0;     // evasion

    mss[OBJ_ARMOUR][ARM_HELMET] = 80;


    // gloves
    prop[OBJ_ARMOUR][ARM_GLOVES][PARM_AC] = 1;  // AC

    prop[OBJ_ARMOUR][ARM_GLOVES][PARM_EVASION] = 0;     // evasion

    mss[OBJ_ARMOUR][ARM_GLOVES] = 20;

    // boots
    prop[OBJ_ARMOUR][ARM_BOOTS][PARM_AC] = 1;   // AC

    prop[OBJ_ARMOUR][ARM_BOOTS][PARM_EVASION] = 0;      // evasion

    mss[OBJ_ARMOUR][ARM_BOOTS] = 40;

    // buckler:
    prop[OBJ_ARMOUR][ARM_BUCKLER][PARM_AC] = 0;         // AC

    prop[OBJ_ARMOUR][ARM_BUCKLER][PARM_EVASION] = 0;    // evasion

    mss[OBJ_ARMOUR][ARM_BUCKLER] = 50;

    // large shield:
    prop[OBJ_ARMOUR][ARM_LARGE_SHIELD][PARM_AC] = 0;    // AC

    prop[OBJ_ARMOUR][ARM_LARGE_SHIELD][PARM_EVASION] = 0;       // evasion

    mss[OBJ_ARMOUR][ARM_LARGE_SHIELD] = 250;


    int i = 0;

    for (i = 0; i < 50; i++)
    {
        mss[OBJ_WANDS][i] = 100;
        mss[OBJ_FOOD][i] = 100;
        mss[5][i] = 200;
        mss[OBJ_SCROLLS][i] = 50;
        mss[OBJ_JEWELLERY][i] = 20;
        mss[OBJ_POTIONS][i] = 60;
        mss[9][i] = 5;
        // don't know what these are, yet:
        mss[OBJ_BOOKS][i] = 100;        // books

        mss[OBJ_STAVES][i] = 130;       // staves

        mss[OBJ_ORBS][i] = 300; // the Orb!

        mss[OBJ_MISCELLANY][i] = 100;
        mss[OBJ_CORPSES][i] = 100;




    }

// this is food, right?
    mss[OBJ_FOOD][FOOD_MEAT_RATION] = 80;
    mss[OBJ_FOOD][FOOD_BREAD_RATION] = 80;
    mss[OBJ_FOOD][FOOD_PEAR] = 20;
    mss[OBJ_FOOD][FOOD_APPLE] = 20;
    mss[OBJ_FOOD][FOOD_CHOKO] = 30;
    mss[OBJ_FOOD][FOOD_HONEYCOMB] = 40;
    mss[OBJ_FOOD][FOOD_ROYAL_JELLY] = 55;
    mss[OBJ_FOOD][FOOD_SNOZZCUMBER] = 50;
    mss[OBJ_FOOD][FOOD_PIZZA] = 40;
    mss[OBJ_FOOD][FOOD_APRICOT] = 15;
    mss[OBJ_FOOD][FOOD_ORANGE] = 20;
    mss[OBJ_FOOD][FOOD_BANANA] = 20;
    mss[OBJ_FOOD][FOOD_STRAWBERRY] = 5;
    mss[OBJ_FOOD][FOOD_RAMBUTAN] = 10;
    mss[OBJ_FOOD][FOOD_LEMON] = 20;
    mss[OBJ_FOOD][FOOD_GRAPE] = 5;
    mss[OBJ_FOOD][FOOD_SULTANA] = 3;
    mss[OBJ_FOOD][FOOD_LYCHEE] = 10;
    mss[OBJ_FOOD][FOOD_BEEF_JERKY] = 20;
    mss[OBJ_FOOD][FOOD_CHEESE] = 40;
    mss[OBJ_FOOD][FOOD_SAUSAGE] = 40;
    mss[OBJ_FOOD][FOOD_CHUNK] = 100;    // chunk of flesh
    /*mss [4] [21] = 40;
       mss [OBJ_FOOD] [22] = 50;
       mss [OBJ_FOOD] [23] = 60;
       mss [OBJ_FOOD] [24] = 60;
       mss [OBJ_FOOD] [25] = 100; */

    mss[OBJ_MISCELLANY][MISC_BOTTLED_EFREET] = 250;     // flask

    mss[OBJ_MISCELLANY][MISC_CRYSTAL_BALL_OF_SEEING] = 200;     // crystal ball

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
    prop[OBJ_WEAPONS][WPN_CLUB][PWPN_DAMAGE] = 5;       // damage

    prop[OBJ_WEAPONS][WPN_CLUB][PWPN_HIT] = 4;  // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_CLUB][PWPN_SPEED] = 12;       // speed

    mss[OBJ_WEAPONS][WPN_CLUB] = 50;

    // mace:
    prop[OBJ_WEAPONS][WPN_MACE][PWPN_DAMAGE] = 7;       // damage

    prop[OBJ_WEAPONS][WPN_MACE][PWPN_HIT] = 3;  // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_MACE][PWPN_SPEED] = 13;       // speed

    mss[OBJ_WEAPONS][WPN_MACE] = 140;

    // flail
    prop[OBJ_WEAPONS][WPN_FLAIL][PWPN_DAMAGE] = 8;      // damage

    prop[OBJ_WEAPONS][WPN_FLAIL][PWPN_HIT] = 1;         // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_FLAIL][PWPN_SPEED] = 15;      // speed

    mss[OBJ_WEAPONS][WPN_FLAIL] = 150;

    // dagger
    prop[OBJ_WEAPONS][WPN_DAGGER][PWPN_DAMAGE] = 3;     // damage

    prop[OBJ_WEAPONS][WPN_DAGGER][PWPN_HIT] = 6;        // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_DAGGER][PWPN_SPEED] = 11;     // speed

    mss[OBJ_WEAPONS][WPN_DAGGER] = 20;

    // morningstar
    prop[OBJ_WEAPONS][WPN_MORNINGSTAR][PWPN_DAMAGE] = 8;        // damage

    prop[OBJ_WEAPONS][WPN_MORNINGSTAR][PWPN_HIT] = 3;   // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_MORNINGSTAR][PWPN_SPEED] = 14;        // speed

    mss[OBJ_WEAPONS][WPN_MORNINGSTAR] = 120;

    // short sword
    prop[OBJ_WEAPONS][WPN_SHORT_SWORD][PWPN_DAMAGE] = 6;        // damage

    prop[OBJ_WEAPONS][WPN_SHORT_SWORD][PWPN_HIT] = 5;   // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_SHORT_SWORD][PWPN_SPEED] = 12;        // speed

    mss[OBJ_WEAPONS][WPN_SHORT_SWORD] = 100;

    // long sword
    prop[OBJ_WEAPONS][WPN_LONG_SWORD][PWPN_DAMAGE] = 10;        // damage

    prop[OBJ_WEAPONS][WPN_LONG_SWORD][PWPN_HIT] = 3;    // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_LONG_SWORD][PWPN_SPEED] = 14;         // speed

    mss[OBJ_WEAPONS][WPN_LONG_SWORD] = 160;

    // great sword
    prop[OBJ_WEAPONS][WPN_GREAT_SWORD][PWPN_DAMAGE] = 16;       // damage

    prop[OBJ_WEAPONS][WPN_GREAT_SWORD][PWPN_HIT] = -1;  // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_GREAT_SWORD][PWPN_SPEED] = 17;        // speed

    mss[OBJ_WEAPONS][WPN_GREAT_SWORD] = 250;

    // scimitar
    prop[OBJ_WEAPONS][WPN_SCIMITAR][PWPN_DAMAGE] = 11;  // damage

    prop[OBJ_WEAPONS][WPN_SCIMITAR][PWPN_HIT] = 1;      // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_SCIMITAR][PWPN_SPEED] = 14;   // speed

    mss[OBJ_WEAPONS][WPN_SCIMITAR] = 170;

    // hand axe
    prop[OBJ_WEAPONS][WPN_HAND_AXE][PWPN_DAMAGE] = 7;   // damage

    prop[OBJ_WEAPONS][WPN_HAND_AXE][PWPN_HIT] = 2;      // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_HAND_AXE][PWPN_SPEED] = 13;   // speed

    mss[OBJ_WEAPONS][WPN_HAND_AXE] = 110;

    // battleaxe
    prop[OBJ_WEAPONS][WPN_BATTLEAXE][PWPN_DAMAGE] = 17;         // damage

    prop[OBJ_WEAPONS][WPN_BATTLEAXE][PWPN_HIT] = -2;    // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_BATTLEAXE][PWPN_SPEED] = 18;  // speed

    mss[OBJ_WEAPONS][WPN_BATTLEAXE] = 200;

    // spear
    prop[OBJ_WEAPONS][WPN_SPEAR][PWPN_DAMAGE] = 5;      // damage

    prop[OBJ_WEAPONS][WPN_SPEAR][PWPN_HIT] = 3;         // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_SPEAR][PWPN_SPEED] = 13;      // speed

    mss[OBJ_WEAPONS][WPN_SPEAR] = 50;

    // halberd
    prop[OBJ_WEAPONS][WPN_HALBERD][PWPN_DAMAGE] = 13;   // damage

    prop[OBJ_WEAPONS][WPN_HALBERD][PWPN_HIT] = -3;      // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_HALBERD][PWPN_SPEED] = 19;    // speed

    mss[OBJ_WEAPONS][WPN_HALBERD] = 200;

    // sling - the three properties are _not_ irrelevant here - when something hits something else (either may be you) in melee, these are used.
    prop[OBJ_WEAPONS][WPN_SLING][PWPN_DAMAGE] = 1;      // damage

    prop[OBJ_WEAPONS][WPN_SLING][PWPN_HIT] = -1;        // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_SLING][PWPN_SPEED] = 11;      // speed

    mss[OBJ_WEAPONS][WPN_SLING] = 10;

    // bow
    prop[OBJ_WEAPONS][WPN_BOW][PWPN_DAMAGE] = 2;        // damage

    prop[OBJ_WEAPONS][WPN_BOW][PWPN_HIT] = -3;  // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_BOW][PWPN_SPEED] = 11;        // speed

    mss[OBJ_WEAPONS][WPN_BOW] = 100;

    // crossbow
    prop[OBJ_WEAPONS][WPN_CROSSBOW][PWPN_DAMAGE] = 2;   // damage

    prop[OBJ_WEAPONS][WPN_CROSSBOW][PWPN_HIT] = -1;     // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_CROSSBOW][PWPN_SPEED] = 15;   // speed

    mss[OBJ_WEAPONS][WPN_CROSSBOW] = 250;

    // hand crossbow
    prop[OBJ_WEAPONS][WPN_HAND_CROSSBOW][PWPN_DAMAGE] = 1;      // damage

    prop[OBJ_WEAPONS][WPN_HAND_CROSSBOW][PWPN_HIT] = -1;        // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_HAND_CROSSBOW][PWPN_SPEED] = 15;      // speed

    mss[OBJ_WEAPONS][WPN_HAND_CROSSBOW] = 70;

    // glaive
    prop[OBJ_WEAPONS][WPN_GLAIVE][PWPN_DAMAGE] = 15;    // damage

    prop[OBJ_WEAPONS][WPN_GLAIVE][PWPN_HIT] = -3;       // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_GLAIVE][PWPN_SPEED] = 18;     // speed

    mss[OBJ_WEAPONS][WPN_GLAIVE] = 200;

    // staff - hmmm
    prop[OBJ_WEAPONS][WPN_QUARTERSTAFF][PWPN_DAMAGE] = 7;       // damage

    prop[OBJ_WEAPONS][WPN_QUARTERSTAFF][PWPN_HIT] = 6;  // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_QUARTERSTAFF][PWPN_SPEED] = 12;       // speed

    mss[OBJ_WEAPONS][WPN_QUARTERSTAFF] = 130;

    // scythe
    prop[OBJ_WEAPONS][WPN_SCYTHE][PWPN_DAMAGE] = 14;    // damage

    prop[OBJ_WEAPONS][WPN_SCYTHE][PWPN_HIT] = -4;       // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_SCYTHE][PWPN_SPEED] = 22;     // speed

    mss[OBJ_WEAPONS][WPN_SCYTHE] = 230;


    // giant club
    prop[OBJ_WEAPONS][WPN_GIANT_CLUB][PWPN_DAMAGE] = 15;        // damage

    prop[OBJ_WEAPONS][WPN_GIANT_CLUB][PWPN_HIT] = -5;   // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_GIANT_CLUB][PWPN_SPEED] = 16;         // speed

    mss[OBJ_WEAPONS][WPN_GIANT_CLUB] = 750;

    // giant spiked club
    prop[OBJ_WEAPONS][WPN_GIANT_SPIKED_CLUB][PWPN_DAMAGE] = 18;         // damage

    prop[OBJ_WEAPONS][WPN_GIANT_SPIKED_CLUB][PWPN_HIT] = -6;    // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_GIANT_SPIKED_CLUB][PWPN_SPEED] = 17;  // speed

    mss[OBJ_WEAPONS][WPN_GIANT_SPIKED_CLUB] = 850;

    // these two ^^^ should have the same speed because of 2-h ogres.


    // eveningstar
    prop[OBJ_WEAPONS][WPN_EVENINGSTAR][PWPN_DAMAGE] = 12;       // damage

    prop[OBJ_WEAPONS][WPN_EVENINGSTAR][PWPN_HIT] = 2;   // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_EVENINGSTAR][PWPN_SPEED] = 15;        // speed

    mss[OBJ_WEAPONS][WPN_EVENINGSTAR] = 150;

    // quick blade
    prop[OBJ_WEAPONS][WPN_QUICK_BLADE][PWPN_DAMAGE] = 5;        // damage

    prop[OBJ_WEAPONS][WPN_QUICK_BLADE][PWPN_HIT] = 6;   // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_QUICK_BLADE][PWPN_SPEED] = 7;         // speed

    mss[OBJ_WEAPONS][WPN_QUICK_BLADE] = 100;

    // katana
    prop[OBJ_WEAPONS][WPN_KATANA][PWPN_DAMAGE] = 13;    // damage

    prop[OBJ_WEAPONS][WPN_KATANA][PWPN_HIT] = 4;        // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_KATANA][PWPN_SPEED] = 13;     // speed

    mss[OBJ_WEAPONS][WPN_KATANA] = 160;

    // exec axe
    prop[OBJ_WEAPONS][WPN_EXECUTIONERS_AXE][PWPN_DAMAGE] = 20;  // damage

    prop[OBJ_WEAPONS][WPN_EXECUTIONERS_AXE][PWPN_HIT] = -4;     // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_EXECUTIONERS_AXE][PWPN_SPEED] = 20;   // speed

    mss[OBJ_WEAPONS][WPN_EXECUTIONERS_AXE] = 320;

    // double sword
    prop[OBJ_WEAPONS][WPN_DOUBLE_SWORD][PWPN_DAMAGE] = 15;      // damage

    prop[OBJ_WEAPONS][WPN_DOUBLE_SWORD][PWPN_HIT] = 3;  // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_DOUBLE_SWORD][PWPN_SPEED] = 16;       // speed

    mss[OBJ_WEAPONS][WPN_DOUBLE_SWORD] = 220;

    // triple sword
    prop[OBJ_WEAPONS][WPN_TRIPLE_SWORD][PWPN_DAMAGE] = 19;      // damage

    prop[OBJ_WEAPONS][WPN_TRIPLE_SWORD][PWPN_HIT] = -1;         // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_TRIPLE_SWORD][PWPN_SPEED] = 19;       // speed

    mss[OBJ_WEAPONS][WPN_TRIPLE_SWORD] = 300;

    // hammer
    prop[OBJ_WEAPONS][WPN_HAMMER][PWPN_DAMAGE] = 7;     // damage

    prop[OBJ_WEAPONS][WPN_HAMMER][PWPN_HIT] = 2;        // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_HAMMER][PWPN_SPEED] = 13;     // speed

    mss[OBJ_WEAPONS][WPN_HAMMER] = 130;

    // ancus
    prop[OBJ_WEAPONS][WPN_ANCUS][PWPN_DAMAGE] = 8;      // damage

    prop[OBJ_WEAPONS][WPN_ANCUS][PWPN_HIT] = 2;         // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_ANCUS][PWPN_SPEED] = 15;      // speed

    mss[OBJ_WEAPONS][WPN_ANCUS] = 160;

    // whip
    prop[OBJ_WEAPONS][WPN_WHIP][PWPN_DAMAGE] = 3;       // damage

    prop[OBJ_WEAPONS][WPN_WHIP][PWPN_HIT] = 1;  // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_WHIP][PWPN_SPEED] = 14;       // speed

    mss[OBJ_WEAPONS][WPN_WHIP] = 30;

    // sabre
    prop[OBJ_WEAPONS][WPN_SABRE][PWPN_DAMAGE] = 7;      // damage

    prop[OBJ_WEAPONS][WPN_SABRE][PWPN_HIT] = 4;         // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_SABRE][PWPN_SPEED] = 12;      // speed

    mss[OBJ_WEAPONS][WPN_SABRE] = 110;

    // demon blade
    prop[OBJ_WEAPONS][WPN_DEMON_BLADE][PWPN_DAMAGE] = 13;       // damage

    prop[OBJ_WEAPONS][WPN_DEMON_BLADE][PWPN_HIT] = 2;   // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_DEMON_BLADE][PWPN_SPEED] = 15;        // speed

    mss[OBJ_WEAPONS][WPN_DEMON_BLADE] = 200;

    // demon whip
    prop[OBJ_WEAPONS][WPN_DEMON_WHIP][PWPN_DAMAGE] = 10;        // damage

    prop[OBJ_WEAPONS][WPN_DEMON_WHIP][PWPN_HIT] = 1;    // helps to get past evasion

    prop[OBJ_WEAPONS][WPN_DEMON_WHIP][PWPN_SPEED] = 14;         // speed

    mss[OBJ_WEAPONS][WPN_DEMON_WHIP] = 30;


    // Missiles:

    // stone
    prop[OBJ_MISSILES][MI_STONE][PWPN_DAMAGE] = 2;
    prop[OBJ_MISSILES][MI_STONE][PWPN_HIT] = 4;
    mss[OBJ_MISSILES][MI_STONE] = 5;

    // arrow
    prop[OBJ_MISSILES][MI_ARROW][PWPN_DAMAGE] = 2;
    prop[OBJ_MISSILES][MI_ARROW][PWPN_HIT] = 7;
    mss[OBJ_MISSILES][MI_ARROW] = 10;

    // crossbow bolt
    prop[OBJ_MISSILES][MI_BOLT][PWPN_DAMAGE] = 2;
    prop[OBJ_MISSILES][MI_BOLT][PWPN_HIT] = 9;  // but increases less with skill

    mss[OBJ_MISSILES][MI_BOLT] = 12;

    // dart
    prop[OBJ_MISSILES][MI_DART][PWPN_DAMAGE] = 3;
    prop[OBJ_MISSILES][MI_DART][PWPN_HIT] = 5;  //whatever - for hand crossbow

    mss[OBJ_MISSILES][MI_DART] = 5;

    // large rock
    prop[OBJ_MISSILES][MI_LARGE_ROCK][PWPN_DAMAGE] = 20;
    mss[OBJ_MISSILES][MI_LARGE_ROCK] = 1000;

}



unsigned char check_item_knowledge(void)
{


    char st_pass[60];

    int i;
    int j;
    char lines = 0;
    unsigned char anything = 0;
    char ft = 0;

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
    unsigned char ki = 0;



    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 30; j++)
        {
            if (id[i][j] != 0)
            {
                inv_count++;
            }
        }
    }

    if (inv_count == 0)
    {
        cprintf("You don't recognise anything yet!");
        if (getch() == 0)
            getch();
        goto putty;
    }
    textcolor(BLUE);
    cprintf("  You recognise:");
    textcolor(LIGHTGREY);
    lines++;


    for (i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0:
            ft = OBJ_WANDS;
            break;              // magic devices

        case 1:
            ft = OBJ_SCROLLS;
            break;              // scrolls

        case 2:
            ft = OBJ_JEWELLERY;
            break;              // rings

        case 3:
            ft = OBJ_POTIONS;
            break;              // potions

        }

        for (j = 0; j < 30; j++)
        {

            if (lines > NUMBER_OF_LINES - 2 && inv_count > 0)
            {
                gotoxy(1, NUMBER_OF_LINES);
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
                if (ki == 0)
                    ki = getch();
                lines = 0;
                clrscr();
                gotoxy(1, 1);
                anything = 0;
            }

            if (id[i][j] == 1)
            {
                anything++;

                if (lines > 0)
                    cprintf(EOL);
                lines++;
                cprintf(" ");

                yps = wherey();

//   item_name_2(i, j, 0, 0, 1, 3, 3, st_pass);
                //char item_name_2(char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char glog [60])
                item_name_2(0, ft, j, 0, 0, 1, 0, st_pass);

                cprintf(st_pass);

                inv_count--;

                if (wherey() != yps)
                    lines++;

            }
        }                       // end of j loop

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
        if (ki == 0)
            ki = getch();
#ifdef DOS_TERM
        puttext(35, 1, 80, 25, buffer);
#endif
        return anything;
    }

putty:
#ifdef DOS_TERM
    puttext(35, 1, 80, 25, buffer);
#endif

    return ki;

}                               // end of check_item_knowledge



char weapon_skill(char wclass, char wtype)
{
    if (wclass == 11)
        return 7;               // staff

    if (wclass != 0)
        return 0;               // no skill

    switch (wtype)
    {

    case WPN_CLUB:
        return SK_MACES_FLAILS; //strcat(glog , "club"); break;

    case WPN_MACE:
        return SK_MACES_FLAILS; //strcat(glog , "mace"); break;

    case WPN_FLAIL:
        return SK_MACES_FLAILS; //strcat(glog , "flail"); break;

    case WPN_DAGGER:
        return SK_SHORT_BLADES; //strcat(glog , "dagger"); break;

    case WPN_MORNINGSTAR:
        return SK_MACES_FLAILS; //??? //strcat(glog , "spiked mace"); break;

    case WPN_SHORT_SWORD:
        return SK_SHORT_BLADES; //strcat(glog , "short sword"); break;

    case WPN_LONG_SWORD:
        return SK_LONG_SWORDS;  //strcat(glog , "long sword"); break;

    case WPN_GREAT_SWORD:
        return SK_GREAT_SWORDS; //strcat(glog , "great sword"); break;

    case WPN_SCIMITAR:
        return SK_LONG_SWORDS;  //strcat(glog , "scimitar"); break;

    case WPN_HAND_AXE:
        return SK_AXES;         //strcat(glog , "hand axe"); break;

    case WPN_BATTLEAXE:
        return SK_AXES;         //strcat(glog , "battleaxe"); break;

    case WPN_SPEAR:
        return SK_POLEARMS;     //strcat(glog , "spear"); break;

    case WPN_HALBERD:
        return SK_POLEARMS;     //strcat(glog , "halberd"); break;

    case WPN_SLING:
        return SK_SLINGS;       //strcat(glog , "sling"); break;

    case WPN_BOW:
        return SK_BOWS;         //strcat(glog , "bow"); break;

    case WPN_CROSSBOW:
        return SK_CROSSBOWS;    //strcat(glog , "crossbow"); break;

    case WPN_HAND_CROSSBOW:
        return SK_CROSSBOWS;    //strcat(glog , "hand crossbow"); break;

    case WPN_GLAIVE:
        return SK_POLEARMS;     //strcat(glog , "glaive"); break;

    case WPN_QUARTERSTAFF:
        return SK_STAVES;       //strcat(glog , "quarterstaff"); break;

    case WPN_SCYTHE:
        return SK_POLEARMS;     //strcat(glog , "scythe"); break;

    case WPN_GIANT_CLUB:
        return SK_MACES_FLAILS; //strcat(glog , "giant club"); break;

    case WPN_GIANT_SPIKED_CLUB:
        return SK_MACES_FLAILS; //strcat(glog , "giant spiked club"); break;

    case WPN_EVENINGSTAR:
        return SK_MACES_FLAILS; // eveningstar

    case WPN_QUICK_BLADE:
        return SK_SHORT_BLADES; // quick blade

    case WPN_KATANA:
        return SK_LONG_SWORDS;  // katana

    case WPN_EXECUTIONERS_AXE:
        return SK_AXES;         // exec axe

    case WPN_DOUBLE_SWORD:
        return SK_LONG_SWORDS;  // 2x sword

    case WPN_TRIPLE_SWORD:
        return SK_GREAT_SWORDS; // 3x sword

    case WPN_HAMMER:
        return SK_MACES_FLAILS; // hammer

    case WPN_ANCUS:
        return SK_MACES_FLAILS; // ancus

    case WPN_WHIP:
        return SK_MACES_FLAILS; // whip

    case WPN_SABRE:
        return SK_SHORT_BLADES; // sabre

    case WPN_DEMON_BLADE:
        return SK_LONG_SWORDS;  // demon blade

    case WPN_DEMON_WHIP:
        return SK_MACES_FLAILS; // demon whip

    }

    return 0;

}


char damage_type(char wclass, char wtype)
{

    if (wclass != 0)
        return 0;               // bludgeon
    // 1 = slicing
    // 2 = piercing
    // 3 = chopping

    switch (wtype)
    {

    case WPN_CLUB:
        return DVORP_CRUSHING;  //strcat(glog , "club"); break;

    case WPN_MACE:
        return DVORP_CRUSHING;  //strcat(glog , "mace"); break;

    case WPN_FLAIL:
        return DVORP_CRUSHING;  //strcat(glog , "flail"); break;

    case WPN_DAGGER:
        return DVORP_SLICING;   //strcat(glog , "dagger"); break;

    case WPN_MORNINGSTAR:
        return DVORP_PIERCING;  //??? //strcat(glog , "spiked mace"); break;

    case WPN_SHORT_SWORD:
        return DVORP_SLICING;   //strcat(glog , "short sword"); break;

    case WPN_LONG_SWORD:
        return DVORP_SLICING;   //strcat(glog , "long sword"); break;

    case WPN_GREAT_SWORD:
        return DVORP_SLICING;   //strcat(glog , "great sword"); break;

    case WPN_SCIMITAR:
        return DVORP_SLICING;   //strcat(glog , "scimitar"); break;

    case WPN_HAND_AXE:
        return DVORP_CHOPPING;  //strcat(glog , "hand axe"); break;

    case WPN_BATTLEAXE:
        return DVORP_CHOPPING;  //strcat(glog , "battleaxe"); break;

    case WPN_SPEAR:
        return DVORP_PIERCING;  //strcat(glog , "spear"); break;

    case WPN_HALBERD:
        return DVORP_CHOPPING;  //strcat(glog , "halberd"); break;

    case WPN_SLING:
        return DVORP_CRUSHING;  //strcat(glog , "sling"); break;

    case WPN_BOW:
        return DVORP_CRUSHING;  //strcat(glog , "bow"); break;

    case WPN_CROSSBOW:
        return DVORP_CRUSHING;  //strcat(glog , "crossbow"); break;

    case WPN_HAND_CROSSBOW:
        return DVORP_CRUSHING;  //strcat(glog , "hand crossbow"); break;

    case WPN_GLAIVE:
        return DVORP_CHOPPING;  //strcat(glog , "glaive"); break;

    case WPN_QUARTERSTAFF:
        return DVORP_CRUSHING;  //strcat(glog , "quarterstaff"); break;

    case WPN_SCYTHE:
        return DVORP_SLICING;   //strcat(glog , "scythe"); break;

    case WPN_GIANT_CLUB:
        return DVORP_CRUSHING;  //strcat(glog , "giant club"); break;

    case WPN_GIANT_SPIKED_CLUB:
        return DVORP_PIERCING;  //strcat(glog , "giant spiked club"); break;

    case WPN_EVENINGSTAR:
        return DVORP_PIERCING;  // eveningstar

    case WPN_QUICK_BLADE:
        return DVORP_SLICING;   // quick blade

    case WPN_KATANA:
        return DVORP_SLICING;   // katana

    case WPN_EXECUTIONERS_AXE:
        return DVORP_CHOPPING;  // exec axe

    case WPN_DOUBLE_SWORD:
        return DVORP_SLICING;   // 2x sword

    case WPN_TRIPLE_SWORD:
        return DVORP_SLICING;   // 3x sword

    case WPN_HAMMER:
        return DVORP_CRUSHING;  // hammer

    case WPN_ANCUS:
        return DVORP_CRUSHING;  // ancus

    case WPN_WHIP:
        return DVORP_CRUSHING;  // whip

    case WPN_SABRE:
        return DVORP_SLICING;   // sabre

    case WPN_DEMON_BLADE:
        return DVORP_SLICING;   // demon blade

    case WPN_DEMON_WHIP:
        return DVORP_CRUSHING;  // demon whip

    }

    return 0;
}                               // end damage_type


void make_name(unsigned char var1, unsigned char var2, unsigned char var3, char ncase, char str_pass[50])
{

    char name[30] = "";
    unsigned char numb[15];
    char len;
    char i = 0;
    char nexty = 0;
    char j = 0;
    char igo = 0;

    int x = 0;


    char glag[30];

    strcpy(glag, "");


    numb[0] = var1 * var2;
    numb[1] = var1 * var3;
    numb[2] = var2 * var3;
    numb[3] = var1 * var2 * var3;
    numb[4] = var1 + var2;
    numb[5] = var2 + var3;
    numb[6] = var1 * var2 + var3;
    numb[7] = var1 * var3 + var2;
    numb[8] = var2 * var3 + var1;
    numb[9] = var1 * var2 * var3 - var1;
    numb[10] = var1 + var2 + var2;
    numb[11] = var2 + var3 * var1;
    numb[12] = var1 * var2 * var3;
    numb[13] = var1 * var3 * var1;
    numb[14] = var2 * var3 * var3;


    for (i = 0; i < 15; i++)
    {
        while (numb[i] >= 25)
        {
            numb[i] -= 25;
        }
    }                           // end of for i

    j = numb[6];

    len = reduce(numb[reduce(numb[11]) / 2]);
    while (len < 5 && j < 10)
    {
        len += reduce(numb[j] + 1) + 1;
        j++;
    }

    while (len > 14)
    {
        len -= 8;
    }

    nexty = retbit(numb[4]);
    char k = 0;

    j = 0;

    for (i = 0; i < len; i++)
    {
        j++;
        if (j >= 15)
        {
            j = 0;
            k++;
            if (k > 9)
                break;
        }

        if (nexty == 1 || (i > 0 && is_a_vowel(name[i]) == 0))
        {
            name[i] = retvow(numb[j]);
            if ((i == 0 || i == len - 1) && name[i] == 32)
            {
                i--;
                continue;
            }
        }
        else
        {
            if (numb[i / 2] <= 1 && i > 3 && is_a_vowel(name[i]) == 1)
                goto two_letter;
            else
                name[i] = numb[j];
          hello:igo++;
        }

        if ((nexty == 0 && is_a_vowel(name[i]) == 1) || (nexty == 1 && is_a_vowel(name[i]) == 0))
        {
            if (nexty == 1 && i > 0 && is_a_vowel(name[i - 1]) == 0)
                i--;
            i--;
            continue;
        }


        if (is_a_vowel(name[i]) == 0)
        {
            nexty = 1;
        }
        else
            nexty = 0;

        x++;

    }

    switch (ncase)
    {
    case 2:
        for (i = 0; i < len + 1; i++)
        {
            if (i > 3 && name[i] == 0 && name[i + 1] == 0)
            {
                name[i] = 0;
                if (name[i - 1] == 32)
                    name[i - 1] = 0;
                break;
            }
            if (name[i] != 32 && name[i] < 30)
                name[i] += 65;
            if (name[i] > 96)
                name[i] -= 32;
        }
        break;

    case 3:
        for (i = 0; i < len + 0; i++)
        {
            if (i != 0 && name[i] >= 65 && name[i] < 97)
            {
                if (name[i - 1] == 32)
                    name[i] += 32;
            }

            if (name[i] > 97)
            {
                if (i == 0 || name[i - 1] == 32)
                    name[i] -= 32;
            }


            if (name[i] < 30)
            {
                if (i == 0 || (name[i] != 32 && name[i - 1] == 32))
                    name[i] += 65;
                else
                    name[i] += 97;
            }


        }
        break;

    case 0:
        for (i = 0; i < len; i++)
        {
            if (name[i] != 32 && name[i] < 30)
                name[i] += 97;
        }
        break;

    case 1:
        name[i] += 65;
        for (i = 1; i < len; i++)
        {
            if (name[i] != 32 && name[i] < 30)
                name[i] += 97;  //97;

        }
        break;
    }

    strcpy(glag, name);

    if (strlen(glag) == 0)
        strcpy(glag, "Plog");

    strcpy(str_pass, glag);

    return;




two_letter:
    if (nexty == 1)
        goto hello;

    if (!is_a_vowel(name[i - 1]))
        goto hello;

    i++;
    switch (i * (retbit(j) + 1))
    {
    case 0:
        strcat(name, "sh");
        break;
    case 1:
        strcat(name, "ch");
        break;
    case 2:
        strcat(name, "tz");
        break;
    case 3:
        strcat(name, "ts");
        break;
    case 4:
        strcat(name, "cs");
        break;
    case 5:
        strcat(name, "cz");
        break;
    case 6:
        strcat(name, "xt");
        break;
    case 7:
        strcat(name, "xk");
        break;
    case 8:
        strcat(name, "kl");
        break;
    case 9:
        strcat(name, "cl");
        break;
    case 10:
        strcat(name, "fr");
        break;
    case 11:
        strcat(name, "sh");
        break;
    case 12:
        strcat(name, "ch");
        break;
    case 13:
        strcat(name, "gh");
        break;
    case 14:
        strcat(name, "pr");
        break;
    case 15:
        strcat(name, "tr");
        break;
    case 16:
        strcat(name, "mn");
        break;
    case 17:
        strcat(name, "ph");
        break;
    case 18:
        strcat(name, "pn");
        break;
    case 19:
        strcat(name, "cv");
        break;
    case 20:
        strcat(name, "zx");
        break;
    case 21:
        strcat(name, "xz");
        break;
    case 23:
        strcat(name, "dd");
        break;
    case 24:
        strcat(name, "tt");
        break;
    case 25:
        strcat(name, "ll");
        break;
//case 26: strcat(name, "sh"); break;
        //case 12: strcat(name, "sh"); break;
        //case 13: strcat(name, "sh"); break;
    default:
        i--;
        goto hello;
    }

    x += 2;

    goto hello;
}                               // end of make_name


char reduce(unsigned char reducee)
{
    while (reducee >= 26)
    {
        reducee -= 26;
    }

    return reducee;

}                               // end of char reduce



char is_a_vowel(unsigned char let)
{
    //if (let == 'a' || let == 'e' || let == 'i' || let == 'o' || let == 'u')
    if (let == 0 || let == 4 || let == 8 || let == 14 || let == 20 || let == 24 || let == 32)
    {
        return 1;
    }
    else
        return 0;
}                               // end of char reduce



char retvow(char sed)
{

    while (sed > 6)
    {
        sed -= 6;
    }

    switch (sed)
    {
    case 0:
        return 0;
    case 1:
        return 4;
    case 2:
        return 8;
    case 3:
        return 14;
    case 4:
        return 20;
    case 5:
        return 24;
    case 6:
        return 32;
    }

    return 0;
}


char retbit(char sed)
{
    if (sed % 2 == 0)
        return 0;

    return 1;
}

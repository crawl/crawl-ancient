/*
 *  File:       itemname.cc
 *  Summary:    Misc functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <4>      9/09/99        BWR             Added hands_required function
 *      <3>      6/13/99        BWR             Upped the base AC for heavy armour
 *      <2>      5/20/99        BWR             Extended screen lines support
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "itemname.h"

#include <string.h>
#include <stdlib.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "mon-util.h"
#include "randart.h"
#include "skills2.h"
#include "wpn-misc.h"

#ifdef MACROS
#include "macro.h"
#endif

#ifdef USE_CURSES
#include <curses.h>
#endif


char id[4][50];
int prop[4][50][3];
FixedArray < int, 20, 50 > mss;

bool is_a_vowel(unsigned char let);
char item_name_2(unsigned char item_plus2, char item_clas, char item_typ,
                 unsigned char item_da, unsigned char it_plus,
                 unsigned int it_quant, char ident_lev, char glog[60]);

char reduce(unsigned char reducee);
char retbit(char sed);
char retvow(char sed);


void it_name(int itn, char des, char str_pass[80])
{

    item_name(mitm.pluses2[itn], mitm.base_type[itn], mitm.sub_type[itn],
              mitm.special[itn], mitm.pluses[itn], mitm.quantity[itn],
              mitm.id[itn], des, str_pass);

}                               // end it_name()


void in_name(int inn, char des, char str_pass[80])
{

    item_name(you.inv_plus2[inn], you.inv_class[inn], you.inv_type[inn],
              you.inv_dam[inn], you.inv_plus[inn], you.inv_quantity[inn],
              you.inv_ident[inn], des, str_pass);

}                               // end in_name()

void in_quant_name(int inn, int quant, char des, char str_pass[80])
{

    item_name(you.inv_plus2[inn], you.inv_class[inn], you.inv_type[inn],
              you.inv_dam[inn], you.inv_plus[inn], quant,
              you.inv_ident[inn], des, str_pass);
}                               // end in_quant_name()

char item_name(unsigned char item_plus2, char item_clas, char item_typ,
               unsigned char item_da, unsigned char it_plus,
               unsigned int it_quant, char ident_lev, char descrip,
               char glag[60])
{

    char itm_name[60] = "";
    char tmp_quant[5];

    strcpy(glag, "");

    item_name_2(item_plus2, item_clas, item_typ, item_da, it_plus, it_quant,
                ident_lev, itm_name);

    if (item_clas == OBJ_ORBS
        || (item_clas == OBJ_WEAPONS && item_da >= NWPN_SINGING_SWORD
            && ident_lev > 0) || (item_clas == OBJ_MISCELLANY
                                  && item_typ == MISC_HORN_OF_GERYON
                                  && ident_lev > 0)
        || (item_clas == OBJ_JEWELLERY && (item_da == 200 || item_da == 201)
            && ident_lev > 1) || ((item_clas == OBJ_WEAPONS
                                   || item_clas == OBJ_ARMOUR)
                                  && item_da % 30 >= 25 && ident_lev > 0))
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
        case 2:         // A/An
        case 3:         // a/an
            break;
        case 4:
            strcat(glag, "Your ");
            break;
        case 5:
            strcat(glag, "your ");
            break;
            //case 6: nothing
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

            if (itm_name[0] == 'a' || itm_name[0] == 'e' || itm_name[0] == 'i'
                || itm_name[0] == 'o' || itm_name[0] == 'u')
            {
                strcat(glag, "n");
            }

            strcat(glag, " ");
            break;              // A/An

        case 3:
            strcat(glag, "a");

            if (itm_name[0] == 'a' || itm_name[0] == 'e' || itm_name[0] == 'i'
                || itm_name[0] == 'o' || itm_name[0] == 'u')
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
            // case 6: nothing
        case 7:
            strcat(glag, "its ");
            break;
        }
    }                           // end of else

    strcat(glag, itm_name);

    return 1;
}                               // end item_name()


char item_name_2(unsigned char item_plus2, char item_clas, char item_typ,
                 unsigned char item_da, unsigned char it_plus,
                 unsigned int it_quant, char ident_lev, char glog[60])
{

    char tmp_quant[5];
    char str_pass2[80];

    strcpy(glog, "");

    switch (item_clas)
    {
    case OBJ_WEAPONS:
        if (ident_lev > 0)
        {
            // We don't bother printing "uncursed" it the item it identified
            // for pluses (it's state should be obvious), this is so that
            // the weapon name is kept short (there isn't a lot of room
            // for the name on the main screen).  If you're going to change
            // this behaviour, *please* make it so that there is an option
            // that maintains this behaviour. -- bwr
            if (it_plus >= 130)
                strcat(glog, "cursed ");
            else if (Options.show_uncursed && ident_lev < 3)
                strcat(glog, "uncursed ");
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
                    itoa(it_plus - 150, tmp_quant, 10);

                strcat(glog, tmp_quant);
                strcat(glog, ",");

                if (item_plus2 >= 50)
                    strcat(glog, "+");

                itoa(item_plus2 - 50, tmp_quant, 10);
                strcat(glog, tmp_quant);
                strcat(glog, " ");
            }

        }

        if (item_da % 30 >= SPWPN_RANDART_I
            && item_da % 30 <= SPWPN_RANDART_V)
        {
            strcat(glog, randart_name(item_clas, item_typ, item_da, it_plus,
                                      item_plus2, ident_lev));
            break;
        }

        if (item_da >= NWPN_SINGING_SWORD)
        {
            if (ident_lev > 0)
            {
                strcat(glog,
                       (item_da == NWPN_SINGING_SWORD) ? "Singing Sword" :
                       (item_da == NWPN_WRATH_OF_TROG) ? "Wrath of Trog" :
                       (item_da == NWPN_SCYTHE_OF_CURSES) ? "Scythe of Curses" :
                       (item_da == NWPN_MACE_OF_VARIABILITY) ? "Mace of Variability" :
                       (item_da == NWPN_GLAIVE_OF_PRUNE) ? "Glaive of Prune" :
                       (item_da == NWPN_SCEPTRE_OF_TORMENT) ? "Sceptre of Torment" :
                       (item_da == NWPN_SWORD_OF_ZONGULDROK) ? "Sword of Zonguldrok" :
                       (item_da == NWPN_SWORD_OF_CEREBOV) ? "Sword of Cerebov" :
                       (item_da == NWPN_STAFF_OF_DISPATER) ? "Staff of Dispater" :
                       (item_da == NWPN_SCEPTRE_OF_ASMODEUS) ? "Sceptre of Asmodeus" :
                       (item_da == NWPN_SWORD_OF_POWER) ? "Sword of Power" :
                       (item_da == NWPN_KNIFE_OF_ACCURACY) ? "Knife of Accuracy" :
                       (item_da == NWPN_STAFF_OF_OLGREB) ? "Staff of Olgreb" :
                       (item_da == NWPN_VAMPIRES_TOOTH) ? "Vampire's Tooth" :
                       (item_da == NWPN_STAFF_OF_WUCAD_MU) ? "Staff of Wucad Mu"
                                                   : "Brodale's Buggy Bola");
            }
            else
            {
                strcat(glog,
                       (item_da == NWPN_SINGING_SWORD) ? "golden long sword" :
                       (item_da == NWPN_WRATH_OF_TROG) ? "bloodstained battleaxe" :
                       (item_da == NWPN_SCYTHE_OF_CURSES) ? "warped scythe" :
                       (item_da == NWPN_MACE_OF_VARIABILITY) ? "shimmering mace" :
                       (item_da == NWPN_GLAIVE_OF_PRUNE) ? "purple glaive" :
                       (item_da == NWPN_SCEPTRE_OF_TORMENT) ? "jeweled golden mace" :
                       (item_da == NWPN_SWORD_OF_ZONGULDROK) ? "bone long sword" :
                       (item_da == NWPN_SWORD_OF_CEREBOV) ? "great serpentine sword" :
                       (item_da == NWPN_STAFF_OF_DISPATER) ? "golden staff" :
                       (item_da == NWPN_SCEPTRE_OF_ASMODEUS) ? "ruby sceptre" :
                       (item_da == NWPN_SWORD_OF_POWER) ? "chunky great sword" :
                       (item_da == NWPN_KNIFE_OF_ACCURACY) ? "thin dagger" :
                       (item_da == NWPN_STAFF_OF_OLGREB) ? "green glowing staff" :
                       (item_da == NWPN_VAMPIRES_TOOTH) ? "ivory dagger" :
                       (item_da == NWPN_STAFF_OF_WUCAD_MU) ? "quarterstaff"
                                                           : "bola");
            }

            break;
        }

        if (ident_lev < 2 || item_da % 30 == SPWPN_NORMAL
            || item_da / 30 >= DWPN_ORCISH)
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
            if (item_da % 30 == SPWPN_VAMPIRICISM)
                strcat(glog, "vampiric ");

        }                       // end if

        standard_name_weap(item_typ, str_pass2);
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

            case SPWPN_REACHING:
                strcat(glog, " of reaching");
                break;

                /* 25 - 29 are randarts */
            }
        }

        // Hellbrand; zap to reduce plus by 1 and cast hellfire?
        break;


    // compacted 15 Apr 2000 {dlb}: -- in progress
    case OBJ_MISSILES:
        if (item_da % 30 == SPMSL_POISONED
            || item_da % 30 == SPMSL_POISONED_II)
            strcat(glog, "poisoned ");

        if (ident_lev > 2)
        {
            if (it_plus >= 50 && (it_plus <= 130 || it_plus >= 150))
                strcat(glog, "+");

            itoa(it_plus - 50, tmp_quant, 10);

            if (it_plus > 130)
                itoa(it_plus - 150, tmp_quant, 10);

            strcat(glog, tmp_quant);
            strcat(glog, " ");
        }

        if (item_da != 0)
        {
            unsigned char dwpn = (item_da / 30);

            strcat(glog, (dwpn == DWPN_ORCISH) ? "orcish " :
                   (dwpn == DWPN_ELVEN) ? "elven " :
                   (dwpn == DWPN_DWARVEN) ? "dwarven " :
                   (dwpn == DWPN_PLAIN) ? "" : "buggy ");
                   // this should probably be "" {dlb}
        }

        strcat(glog, (item_typ == MI_STONE) ? "stone" :
               (item_typ == MI_ARROW) ? "arrow" :
               (item_typ == MI_BOLT) ? "bolt" :
               (item_typ == MI_DART) ? "dart" :
               (item_typ == MI_NEEDLE) ? "needle" :
               (item_typ == MI_EGGPLANT) ? "eggplant" :
               (item_typ == MI_LARGE_ROCK) ? "large rock" : "program bug");
               // this should probably be "" {dlb}

        if (it_quant > 1)
            strcat(glog, "s");

        if (ident_lev > 1)
        {
            unsigned char leftover = (item_da % 30);

            strcat(glog, (leftover == SPMSL_NORMAL) ? "" :
                   (leftover == SPMSL_POISONED
                   || leftover == SPMSL_POISONED_II) ? "" :
                   // see above, temporary enchantment {dlb}
                   (leftover == SPMSL_FLAME) ? " of flame" :
                   (leftover == SPMSL_ICE) ? " of ice" : " of bugginess");
        }
        break;

    case OBJ_ARMOUR:
        if (ident_lev > 0)
        {
            if (it_plus >= 130)
                strcat(glog, "cursed ");
            else if (Options.show_uncursed && ident_lev < 3)
                strcat(glog, "uncursed ");
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

        if (item_typ == ARM_GLOVES
            || (item_typ == ARM_BOOTS && item_plus2 == 0))
        {
            strcat(glog, "pair of ");
        }

        if (item_da % 30 >= SPARM_RANDART_I
            && item_da % 30 <= SPARM_RANDART_V)
        {
            strcat(glog,
                   randart_armour_name(item_clas, item_typ, item_da, it_plus,
                                       item_plus2, ident_lev));
            break;
        }

        if (ident_lev < 2 || item_da % 30 == SPARM_NORMAL
            || item_da / 30 >= DARM_ELVEN)
        {
            if (item_typ != ARM_HELMET)
            {
                switch ((int) item_da / 30)
                {
                case DARM_EMBROIDERED_SHINY:
                    if (item_typ == ARM_ROBE || item_typ == ARM_CLOAK
                        || item_typ == ARM_GLOVES || item_typ == ARM_BOOTS
                        || (item_typ == ARM_HELMET && item_plus2 >= 2))
                    {
                        strcat(glog, "embroidered ");
                    }
                    else if (item_typ == ARM_LEATHER_ARMOUR)
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
                unsigned char dhelm = (item_da / 30);

                if (item_da != 0)
                {
                    strcat(glog,
                           (dhelm == DHELM_WINGED) ? "winged " :
                           (dhelm == DHELM_WINGED) ? "winged " :
                           (dhelm == DHELM_HORNED) ? "horned " :
                           (dhelm == DHELM_CRESTED) ? "crested " :
                           (dhelm == DHELM_PLUMED) ? "plumed " :
                           (dhelm == DHELM_SPIKED) ? "spiked " :
                           (dhelm == DHELM_VISORED) ? "visored " :
                           (dhelm == DHELM_JEWELLED) ? "jeweled " :
                           // these three were single spaces {dlb}
                           (dhelm == 13) ? " " :
                           (dhelm == 14) ? " " : (dhelm == 15) ? " " : "buggy ");
                }
            }
        }                       // end if

        standard_name_armour(item_typ, item_plus2, str_pass2);  // in randart.cc
        strcat(glog, str_pass2);

        if (ident_lev > 1 && (item_da % 30 != SPARM_NORMAL))
        {
            unsigned char sparm = (item_da % 30);

            strcat(glog, " of ");

            strcat(glog, (sparm == SPARM_RUNNING) ? "running" :
                   (sparm == SPARM_FIRE_RESISTANCE) ? "fire resistance" :
                   (sparm == SPARM_COLD_RESISTANCE) ? "cold resistance" :
                   (sparm == SPARM_POISON_RESISTANCE) ? "poison resistance" :
                   (sparm == SPARM_SEE_INVISIBLE) ? "see invisible" :
                   (sparm == SPARM_DARKNESS) ? "darkness" :
                   (sparm == SPARM_STRENGTH) ? "strength" :
                   (sparm == SPARM_DEXTERITY) ? "dexterity" :
                   (sparm == SPARM_INTELLIGENCE) ? "intelligence" :
                   (sparm == SPARM_PONDEROUSNESS) ? "ponderousness" :
                   (sparm == SPARM_LEVITATION) ? "levitation" :
                   (sparm == SPARM_MAGIC_RESISTANCE) ? "magic resistance" :
                   (sparm == SPARM_PROTECTION) ? "protection" :
                   (sparm == SPARM_STEALTH) ? "stealth" :
                   (sparm == SPARM_RESISTANCE) ? "resistance" :
                   (sparm == SPARM_POSITIVE_ENERGY) ? "positive energy" :
                   (sparm == SPARM_ARCHMAGI) ? "the Archmagi" :
                   (sparm == SPARM_PRESERVATION) ? "preservation"
                                                   : "bugginess");
        }
        break;

    // compacted 15 Apr 2000 {dlb}:
    case OBJ_WANDS:
        if (id[0][item_typ] == 1)
        {
            strcat(glog, "wand of ");
            strcat(glog, (item_typ == WAND_FLAME) ? "flame" :
                   (item_typ == WAND_FROST) ? "frost" :
                   (item_typ == WAND_SLOWING) ? "slowing" :
                   (item_typ == WAND_HASTING) ? "hasting" :
                   (item_typ == WAND_MAGIC_DARTS) ? "magic darts" :
                   (item_typ == WAND_HEALING) ? "healing" :
                   (item_typ == WAND_PARALYSIS) ? "paralysis" :
                   (item_typ == WAND_FIRE) ? "fire" :
                   (item_typ == WAND_COLD) ? "cold" :
                   (item_typ == WAND_CONFUSION) ? "confusion" :
                   (item_typ == WAND_INVISIBILITY) ? "invisibility" :
                   (item_typ == WAND_DIGGING) ? "digging" :
                   (item_typ == WAND_FIREBALL) ? "fireball" :
                   (item_typ == WAND_TELEPORTATION) ? "teleportation" :
                   (item_typ == WAND_LIGHTNING) ? "lightning" :
                   (item_typ == WAND_POLYMORPH_OTHER) ? "polymorph other" :
                   (item_typ == WAND_ENSLAVEMENT) ? "enslavement" :
                   (item_typ == WAND_DRAINING) ? "draining" :
                   (item_typ == WAND_RANDOM_EFFECTS) ? "random effects" :
                   (item_typ == WAND_DISINTEGRATION) ? "disintegration"
                                                   : "bugginess");
        }
        else
        {
            char primary = (item_da % 12);
            char secondary = (item_da / 12);

            strcat(glog, (secondary == 0) ? "" :        // hope this works {dlb}
                   (secondary == 1) ? "jeweled" :
                   (secondary == 2) ? "curved" :
                   (secondary == 3) ? "long" :
                   (secondary == 4) ? "short" :
                   (secondary == 5) ? "twisted" :
                   (secondary == 6) ? "crooked" :
                   (secondary == 7) ? "forked" :
                   (secondary == 8) ? "shiny" :
                   (secondary == 9) ? "blackened" :
                   (secondary == 10) ? "tapered" :
                   (secondary == 11) ? "glowing" :
                   (secondary == 12) ? "worn" :
                   (secondary == 13) ? "encrusted" :
                   (secondary == 14) ? "runed" :
                   (secondary == 15) ? "sharpened" : "buggily");

            if (secondary != 0)
                strcat(glog, " ");

            strcat(glog, (primary == 0) ? "iron" :
                   (primary == 1) ? "brass" :
                   (primary == 2) ? "bone" :
                   (primary == 3) ? "wooden" :
                   (primary == 4) ? "copper" :
                   (primary == 5) ? "gold" :
                   (primary == 6) ? "silver" :
                   (primary == 7) ? "bronze" :
                   (primary == 8) ? "ivory" :
                   (primary == 9) ? "glass" :
                   (primary == 10) ? "lead" :
                   (primary == 11) ? "plastic" : "buggy");

            strcat(glog, " wand");
        }

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

    // NB: potions, food, and scrolls stack on the basis of class and
    // type ONLY !!!

    // compacted 15 Apr 2000 {dlb}:
    case OBJ_POTIONS:
        if (id[3][item_typ] == 1)
        {
            strcat(glog, "potion");
            strcat(glog, (it_quant == 1) ? " " : "s ");
            strcat(glog, "of ");
            strcat(glog, (item_typ == POT_HEALING) ? "healing" :
                   (item_typ == POT_HEAL_WOUNDS) ? "heal wounds" :
                   (item_typ == POT_SPEED) ? "speed" :
                   (item_typ == POT_MIGHT) ? "might" :
                   (item_typ == POT_GAIN_STRENGTH) ? "gain strength" :
                   (item_typ == POT_GAIN_DEXTERITY) ? "gain dexterity" :
                   (item_typ == POT_GAIN_INTELLIGENCE) ? "gain intelligence" :
                   (item_typ == POT_LEVITATION) ? "levitation" :
                   (item_typ == POT_POISON) ? "poison" :
                   (item_typ == POT_SLOWING) ? "slowing" :
                   (item_typ == POT_PARALYSIS) ? "paralysis" :
                   (item_typ == POT_CONFUSION) ? "confusion" :
                   (item_typ == POT_INVISIBILITY) ? "invisibility" :
                   (item_typ == POT_PORRIDGE) ? "porridge" :
                   (item_typ == POT_DEGENERATION) ? "degeneration" :
                   (item_typ == POT_DECAY) ? "decay" :
                   (item_typ == POT_WATER) ? "water" :
                   (item_typ == POT_EXPERIENCE) ? "experience" :
                   (item_typ == POT_MAGIC) ? "magic" :
                   (item_typ == POT_RESTORE_ABILITIES) ? "restore abilities" :
                   (item_typ == POT_STRONG_POISON) ? "strong poison" :
                   (item_typ == POT_BERSERK_RAGE) ? "berserk rage" :
                   (item_typ == POT_CURE_MUTATION) ? "cure mutation" :
                   (item_typ == POT_MUTATION) ? "mutation" : "bugginess");
        }
        else
        {
            char primary = (item_da < 14) ? -1 : (item_da / 14);
            char secondary = (item_da < 14) ? item_da : (item_da % 14);

            if (primary != -1)
            {
                strcat(glog,
                       (primary ==  0) ? "bubbling " :
                       (primary ==  1) ? "lumpy " :
                       (primary ==  2) ? "fuming " :
                       (primary ==  3) ? "smoky " :
                       (primary ==  4) ? "fizzy " :
                       (primary ==  5) ? "glowing " :
                       (primary ==  6) ? "sedimented " :
                       (primary ==  7) ? "metallic " :
                       (primary ==  8) ? "murky " :
                       (primary ==  9) ? "gluggy " :
                       (primary == 10) ? "viscous " :
                       (primary == 11) ? "oily " :
                       (primary == 12) ? "slimy " :
                       (primary == 13) ? "emulsified " : "buggy ");
            }

            strcat(glog,
                   (secondary ==  0) ? "clear" :
                   (secondary ==  1) ? "blue" :
                   (secondary ==  2) ? "black" :
                   (secondary ==  3) ? "silvery" :
                   (secondary ==  4) ? "cyan" :
                   (secondary ==  5) ? "purple" :
                   (secondary ==  6) ? "orange" :
                   (secondary ==  7) ? "inky" :
                   (secondary ==  8) ? "red" :
                   (secondary ==  9) ? "yellow" :
                   (secondary == 10) ? "green" :
                   (secondary == 11) ? "brown" :
                   (secondary == 12) ? "pink" :
                   (secondary == 13) ? "white" : "buggy");

            strcat(glog, " potion");

            if (it_quant > 1)
                strcat(glog, "s");
        }
        break;

    // NB: adding another food type == must set for carnivorous chars
    // (Kobolds and mutants)
    case OBJ_FOOD:
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
        case FOOD_APPLE:        // make this less common
            strcat(glog, "apple");
            break;
        case FOOD_CHOKO:
            strcat(glog, "choko");
            break;
        case FOOD_HONEYCOMB:
            strcat(glog, "honeycomb");
            break;
        case FOOD_ROYAL_JELLY:
            strcat(glog, "royal jell");
            break;
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

        }

        if (item_typ == FOOD_ROYAL_JELLY || item_typ == FOOD_STRAWBERRY
            || item_typ == FOOD_BEEF_JERKY)
            strcat(glog, (it_quant > 1) ? "ie" : "y");
        break;

    // compacted 15 Apr 2000 {dlb}:
    case OBJ_SCROLLS:
        strcat(glog, "scroll");
        strcat(glog, (it_quant == 1) ? " " : "s ");

        if (id[1][item_typ] == 1)
        {
            strcat(glog, "of ");
            strcat(glog, (item_typ == SCR_IDENTIFY) ? "identify" :
                   (item_typ == SCR_TELEPORTATION) ? "teleportation" :
                   (item_typ == SCR_FEAR) ? "fear" :
                   (item_typ == SCR_NOISE) ? "noise" :
                   (item_typ == SCR_REMOVE_CURSE) ? "remove curse" :
                   (item_typ == SCR_DETECT_CURSE) ? "detect curse" :
                   (item_typ == SCR_SUMMONING) ? "summoning" :
                   (item_typ == SCR_ENCHANT_WEAPON_I) ? "enchant weapon I" :
                   (item_typ == SCR_ENCHANT_ARMOUR) ? "enchant armour" :
                   (item_typ == SCR_TORMENT) ? "torment" :
                   (item_typ == SCR_RANDOM_USELESSNESS) ? "random uselessness" :
                   (item_typ == SCR_CURSE_WEAPON) ? "curse weapon" :
                   (item_typ == SCR_CURSE_ARMOUR) ? "curse armour" :
                   (item_typ == SCR_IMMOLATION) ? "immolation" :
                   (item_typ == SCR_BLINKING) ? "blinking" :
                   (item_typ == SCR_PAPER) ? "paper" :
                   (item_typ == SCR_MAGIC_MAPPING) ? "magic mapping" :
                   (item_typ == SCR_FORGETFULNESS) ? "forgetfulness" :
                   (item_typ == SCR_ACQUIREMENT) ? "acquirement" :
                   (item_typ == SCR_ENCHANT_WEAPON_II) ? "enchant weapon II" :
                   (item_typ == SCR_VORPALISE_WEAPON) ? "vorpalise weapon" :
                   (item_typ == SCR_RECHARGING) ? "recharging" :
                   //(item_typ == 23)                   ? "portal travel" :
                   (item_typ == SCR_ENCHANT_WEAPON_III) ? "enchant weapon III"
                                                       : "bugginess");
        }
        else
        {
            strcat(glog, "labeled ");
            char str_pass[50];

            make_name(item_da, it_plus, item_clas, 2, str_pass);
            strcat(glog, str_pass);
        }
        break;

    // compacted 15 Apr 2000 {dlb}: -- on hold ... what a mess!
    case OBJ_JEWELLERY:
        if (ident_lev > 0)
        {
            if (it_plus >= 130)
                strcat(glog, "cursed ");
            else if (Options.show_uncursed && ident_lev == 1)
                strcat(glog, "uncursed ");
        }

        if ((ident_lev > 1 || id[2][item_typ] > 0)
            && (item_da == 200 || item_da == 201))
        {
            strcat(glog,
                   randart_ring_name(item_clas, item_typ, item_da, it_plus,
                                     item_plus2, ident_lev));
            break;
        }

        if (id[2][item_typ] > 0)
        {

            if (ident_lev > 1
                && (item_typ == RING_PROTECTION || item_typ == RING_STRENGTH
                    || item_typ == RING_SLAYING || item_typ == RING_EVASION
                    || item_typ == RING_DEXTERITY
                    || item_typ == RING_INTELLIGENCE))
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
            case AMU_RESIST_SLOW:
                strcat(glog, "amulet of resist slowing");
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
                strcat(glog,
                       randart_ring_name(item_clas, item_typ, item_da,
                                         it_plus, item_plus2, ident_lev));
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
            }

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
            }
        }                       // end of rings
        else                    // ie is an amulet
        {
            if (item_da == 200 || item_da == 201)
            {
                strcat(glog,
                       randart_ring_name(item_clas, item_typ, item_da,
                                         it_plus, item_plus2, ident_lev));
                break;
            }

            if (item_da > 13)
            {
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
                }
            }

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
            }
        }                       // end of amulets
        break;

    // compacted 15 Apr 2000 {dlb}:
    case OBJ_MISCELLANY:
        switch (item_typ)
        {
        case MISC_RUNE_OF_ZOT:
            strcat(glog, (it_plus == 1) ? "iron" :
                   (it_plus == 2) ? "obsidian" :
                   (it_plus == 4) ? "icy" :
                   (it_plus == 5) ? "bone" :
                   (it_plus == 13) ? "slimy" :
                   (it_plus == 14) ? "silver" :
                   (it_plus == 19) ? "serpentine" :
                   (it_plus == 20) ? "elven" :
                   (it_plus == 21) ? "golden" :
                   (it_plus == 22) ? "decaying" :
                   (it_plus == 50) ? "demonic" :
                   (it_plus == 51) ? "abyssal" : "buggy");
            strcat(glog, " ");
            strcat(glog, "rune");
            if (it_quant > 1)
                strcat(glog, "s");
            if (ident_lev != 0)
                strcat(glog, " of Zot");
            break;

        case MISC_DECK_OF_POWER:
        case MISC_DECK_OF_SUMMONINGS:
        case MISC_DECK_OF_TRICKS:
        case MISC_DECK_OF_WONDERS:
            strcat(glog, "deck of ");
            strcat(glog, (ident_lev == 0) ? "cards" :
                   (item_typ == MISC_DECK_OF_WONDERS) ? "wonders" :
                   (item_typ == MISC_DECK_OF_SUMMONINGS) ? "summonings" :
                   (item_typ == MISC_DECK_OF_TRICKS) ? "tricks" :
                   (item_typ == MISC_DECK_OF_POWER) ? "power" : "bugginess");
            break;

        case MISC_CRYSTAL_BALL_OF_ENERGY:
        case MISC_CRYSTAL_BALL_OF_FIXATION:
        case MISC_CRYSTAL_BALL_OF_SEEING:
            strcat(glog, "crystal ball");
            if (ident_lev != 0)
            {
                strcat(glog, " of ");
                strcat(glog,
                       (item_typ == MISC_CRYSTAL_BALL_OF_SEEING) ? "seeing" :
                       (item_typ == MISC_CRYSTAL_BALL_OF_ENERGY) ? "energy" :
                       (item_typ == MISC_CRYSTAL_BALL_OF_FIXATION) ? "fixation"
                                                               : "bugginess");
            }
            break;

        case MISC_BOX_OF_BEASTS:
        case MISC_EMPTY_EBONY_CASKET:
            if (ident_lev == 0)
                strcat(glog, "small");
            else
            {
                strcat(glog,
                       (item_typ == MISC_BOX_OF_BEASTS) ? "box of beasts" :
                       (item_typ == MISC_EMPTY_EBONY_CASKET) ? "empty" : "buggy");
            }

            if (ident_lev == 0 || item_typ != MISC_BOX_OF_BEASTS)
                strcat(glog, " ebony casket");
            break;

        case MISC_AIR_ELEMENTAL_FAN:
            if (ident_lev != 0)
                strcat(glog, "air elemental ");
            strcat(glog, "fan");
            break;

        case MISC_LAMP_OF_FIRE:
            strcat(glog, "lamp");
            if (ident_lev != 0)
                strcat(glog, " of fire");
            break;

        case MISC_LANTERN_OF_SHADOWS:
            if (ident_lev == 0)
                strcat(glog, "bone ");
            strcat(glog, "lantern");

            if (ident_lev != 0)
                strcat(glog, " of shadows");
            break;

        case MISC_HORN_OF_GERYON:
            if (ident_lev == 0)
                strcat(glog, "silver ");
            strcat(glog, "horn");

            if (ident_lev != 0)
                strcat(glog, " of Geryon");
            break;

        case MISC_DISC_OF_STORMS:
            if (ident_lev == 0)
                strcat(glog, "grey ");
            strcat(glog, "disc");

            if (ident_lev != 0)
                strcat(glog, " of storms");
            break;

        case MISC_STONE_OF_EARTH_ELEMENTALS:
            if (ident_lev == 0)
                strcat(glog, "nondescript ");
            strcat(glog, "stone");

            if (ident_lev != 0)
                strcat(glog, " of earth elementals");
            break;

        case MISC_BOTTLED_EFREET:
            strcat(glog,
                   (ident_lev == 0) ? "sealed bronze flask" : "bottled efreet");
            break;

        case MISC_PORTABLE_ALTAR_OF_NEMELEX:
            strcat(glog, "portable altar of Nemelex");
            break;

        default:
            strcat(glog, "buggy miscellaneous item");
            break;
        }
        break;

    // compacted 15 Apr 2000 {dlb}:
    case OBJ_BOOKS:
        //strcpy(glog, "");    // is this required? I think not. {dlb}
        if (ident_lev == 0)
        {
            char primary = (item_da / 10);
            char secondary = (item_da % 10);

            strcat(glog, (primary == 0) ? "" :
                   (primary == 1) ? "chunky " :
                   (primary == 2) ? "thick " :
                   (primary == 3) ? "thin " :
                   (primary == 4) ? "wide " :
                   (primary == 5) ? "glowing " :
                   (primary == 6) ? "dog-eared " :
                   (primary == 7) ? "oblong " :
                   (primary == 8) ? "runed " :

                   // these last three were single spaces {dlb}
                   (primary == 9) ? "" :
                   (primary == 10) ? "" : (primary == 11) ? "" : "buggily ");

            strcat(glog, (secondary == 0) ? "paperback " :
                   (secondary == 1) ? "hardcover " :
                   (secondary == 2) ? "leatherbound " :
                   (secondary == 3) ? "metal-bound " :
                   (secondary == 4) ? "papyrus " :
                   // these two were single spaces, too {dlb}
                   (secondary == 5) ? "" :
                   (secondary == 6) ? "" : "buggy ");
            strcat(glog, "book");
        }
        else if (item_typ == BOOK_MANUAL)
        {
            strcat(glog, "manual of ");
            strcat(glog, skill_name(it_plus));
        }
        else if (item_typ == BOOK_NECRONOMICON)
            strcat(glog, "Necronomicon");
        else if (item_typ == BOOK_DESTRUCTION)
            strcat(glog, "tome of Destruction");
        else if (item_typ == BOOK_YOUNG_POISONERS)
            strcat(glog, "Young Poisoner's Handbook");
        else if (item_typ == BOOK_BEASTS)
            strcat(glog, "Monster Manual");
        //jmf: or "book of Beasts"      // NO! {dlb}
        //jmf: dude, what's your problem?
        else
        {
            strcat(glog, "book of ");
            strcat(glog, (item_typ == BOOK_MINOR_MAGIC_I
                          || item_typ == BOOK_MINOR_MAGIC_II
                          || item_typ == BOOK_MINOR_MAGIC_III) ? "Minor Magic" :
                          (item_typ == BOOK_CONJURATIONS_I
                          || item_typ == BOOK_CONJURATIONS_II) ? "Conjurations" :
                          (item_typ == BOOK_FLAMES) ? "Flames" :
                          (item_typ == BOOK_FROST) ? "Frost" :
                          (item_typ == BOOK_SUMMONINGS) ? "Summonings" :
                          (item_typ == BOOK_FIRE) ? "Fire" :
                          (item_typ == BOOK_ICE) ? "Ice" :
                          (item_typ == BOOK_SURVEYANCES) ? "Surveyances" :
                          (item_typ == BOOK_SPATIAL_TRANSLOCATIONS) ? "Spatial Translocations" :
                          (item_typ == BOOK_ENCHANTMENTS) ? "Enchantments" :
                          (item_typ == BOOK_TEMPESTS) ? "the Tempests" :
                          (item_typ == BOOK_DEATH) ? "Death" :
                          (item_typ == BOOK_HINDERANCE) ? "Hinderance" :
                          (item_typ == BOOK_CHANGES) ? "Changes" :
                          (item_typ == BOOK_TRANSFIGURATIONS) ? "Transfigurations" :
                          (item_typ == BOOK_PRACTICAL_MAGIC) ? "Practical Magic" :
                          (item_typ == BOOK_WAR_CHANTS) ? "War Chants" :
                          (item_typ == BOOK_CLOUDS) ? "Clouds" :
                          (item_typ == BOOK_HEALING) ? "Healing" :
                          (item_typ == BOOK_NECROMANCY) ? "Necromancy" :
                          (item_typ == BOOK_CALLINGS) ? "Callings" :
                          (item_typ == BOOK_CHARMS) ? "Charms" :
                          (item_typ == BOOK_DEMONOLOGY) ? "Demonology" :
                          (item_typ == BOOK_AIR) ? "Air" :
                          (item_typ == BOOK_SKY) ? "the Sky" :
                          (item_typ == BOOK_DIVINATIONS) ? "Divinations" :
                          (item_typ == BOOK_WARP) ? "the Warp" :
                          (item_typ == BOOK_ENVENOMATIONS) ? "Envenomations" :
                          (item_typ == BOOK_ANNIHILATIONS) ? "Annihilations" :
                          (item_typ == BOOK_UNLIFE) ? "Unlife" :
                          (item_typ == BOOK_CONTROL) ? "Control" :
                          (item_typ == BOOK_MUTATIONS) ? "Morphology" :
                          (item_typ == BOOK_TUKIMA) ? "Tukima" :
                          (item_typ == BOOK_GEOMANCY) ? "Geomancy" :
                          (item_typ == BOOK_EARTH) ? "the Earth" :
                          (item_typ == BOOK_WIZARDRY) ? "Wizardry" :
                          (item_typ == BOOK_POWER) ? "Power" :
                          (item_typ == BOOK_CANTRIPS) ? "Cantrips" :
                          (item_typ == BOOK_PARTY_TRICKS) ? "Party Tricks" :
                          (item_typ == BOOK_STALKING) ? "Stalking"
                                                           : "Bugginess");
        }
        break;

    // compacted 15 Apr 2000 {dlb}:
    case OBJ_STAVES:
        if (ident_lev == 0)
        {
            strcat(glog, (item_da == 0) ? "curved" :
                   (item_da == 1) ? "glowing" :
                   (item_da == 2) ? "thick" :
                   (item_da == 3) ? "thin" :
                   (item_da == 4) ? "long" :
                   (item_da == 5) ? "twisted" :
                   (item_da == 6) ? "jeweled" :
                   (item_da == 7) ? "runed" :
                   (item_da == 8) ? "smoking" :
                   (item_da == 9) ? "gnarled" :    // was "" {dlb}
                   (item_da == 10) ? "" :
                   (item_da == 11) ? "" :
                   (item_da == 12) ? "" :
                   (item_da == 13) ? "" :
                   (item_da == 14) ? "" :
                   (item_da == 15) ? "" :
                   (item_da == 16) ? "" :
                   (item_da == 17) ? "" :
                   (item_da == 18) ? "" :
                   (item_da == 19) ? "" :
                   (item_da == 20) ? "" :
                   (item_da == 21) ? "" :
                   (item_da == 22) ? "" :
                   (item_da == 23) ? "" :
                   (item_da == 24) ? "" :
                   (item_da == 25) ? "" :
                   (item_da == 26) ? "" :
                   (item_da == 27) ? "" :
                   (item_da == 28) ? "" :
                   (item_da == 29) ? "" : "buggy");
            strcat(glog, " ");
        }

        if (ident_lev > 0)
        {
                   strcat(glog, (// confusing that II is spell and I is not {dlb}
                             item_typ == STAFF_SUMMONING_II
                          || item_typ == STAFF_DESTRUCTION_I
                          || item_typ == STAFF_DESTRUCTION_II
                          || item_typ == STAFF_DESTRUCTION_III
                          || item_typ == STAFF_DESTRUCTION_IV
                          || item_typ == STAFF_WARDING
                          || item_typ == STAFF_DISCOVERY
                          || item_typ == STAFF_DEMONOLOGY) ? "spell " : "");
                          // hope this does not cause problems {dlb}
        }

        strcat(glog, "staff");

        if (ident_lev > 0)
        {
            strcat(glog, " of ");

            strcat(glog, (item_typ == STAFF_WIZARDRY) ? "wizardry" :
                   (item_typ == STAFF_POWER) ? "power" :
                   (item_typ == STAFF_FIRE) ? "fire" :
                   (item_typ == STAFF_COLD) ? "cold" :
                   (item_typ == STAFF_POISON) ? "poison" :
                   (item_typ == STAFF_ENERGY) ? "energy" :
                   (item_typ == STAFF_DEATH) ? "death" :
                   (item_typ == STAFF_CONJURATION) ? "conjuration" :
                   (item_typ == STAFF_ENCHANTMENT) ? "enchantment" :
                   (item_typ == STAFF_SMITING) ? "smiting" :
                   (item_typ == STAFF_WARDING) ? "warding" :
                   (item_typ == STAFF_DISCOVERY) ? "discovery" :
                   (item_typ == STAFF_DEMONOLOGY) ? "demonology" :
                   (item_typ == STAFF_AIR) ? "air" :
                   (item_typ == STAFF_EARTH) ? "earth" :
                   (item_typ == STAFF_SUMMONING_I
                    || item_typ == STAFF_SUMMONING_II) ? "summoning" :
                   (item_typ == STAFF_DESTRUCTION_I
                    || item_typ == STAFF_DESTRUCTION_II
                    || item_typ == STAFF_DESTRUCTION_III
                    || item_typ == STAFF_DESTRUCTION_IV) ? "destruction" :
                   (item_typ == STAFF_CHANNELING) ? "channeling"
                   : "bugginess");
        }
        break;


    // rearranged 15 Apr 2000 {dlb}:
    case OBJ_ORBS:
        strcpy(glog, "Orb of ");
        strcat(glog, (item_typ == ORB_ZOT) ? "Zot" :
/* ******************************************************************
                     (item_typ ==  1)      ? "Zug" :
                     (item_typ ==  2)      ? "Xob" :
                     (item_typ ==  3)      ? "Ix" :
                     (item_typ ==  4)      ? "Xug" :
                     (item_typ ==  5)      ? "Zob" :
                     (item_typ ==  6)      ? "Ik" :
                     (item_typ ==  7)      ? "Grolp" :
                     (item_typ ==  8)      ? "fo brO ehT" :
                     (item_typ ==  9)      ? "Plob" :
                     (item_typ == 10)      ? "Zuggle-Glob" :
                     (item_typ == 11)      ? "Zin" :
                     (item_typ == 12)      ? "Qexigok" :
                     (item_typ == 13)      ? "Bujuk" :
                     (item_typ == 14)      ? "Uhen Tiquritu" :
                     (item_typ == 15)      ? "Idohoxom Sovuf" :
                     (item_typ == 16)      ? "Voc Vocilicoso" :
                     (item_typ == 17)      ? "Chanuaxydiviha" :
                     (item_typ == 18)      ? "Ihexodox" :
                     (item_typ == 19)      ? "Rynok Pol" :
                     (item_typ == 20)      ? "Nemelex" :
                     (item_typ == 21)      ? "Sif Muna" :
                     (item_typ == 22)      ? "Okawaru" :
                     (item_typ == 23)      ? "Kikubaaqudgha" :
****************************************************************** */
               "Bugginess");    // change back to "Zot" if source of problems cannot be found {dlb}
        break;

    case OBJ_GOLD:
        strcat(glog, "gold piece");
        //if (it_quant > 1) strcat(glog, "s");
        break;

    // still not implemented, yet:
    case OBJ_GEMSTONES:
        break;

    // rearranged 15 Apr 2000 {dlb}:
    case OBJ_CORPSES:
        if (item_typ == CORPSE_BODY && item_da < 100)
            strcat(glog, "rotting ");

        char gmo_n[40];

        moname(it_plus, 0, 1, 100, gmo_n);

        strcat(glog, gmo_n);
        strcat(glog, " ");
        strcat(glog, (item_typ == CORPSE_BODY) ? "corpse" :
               (item_typ == CORPSE_SKELETON) ? "skeleton" : "corpse bug");
        break;

    default:
        strcat(glog, "!");
    }                           // end of switch?
    //} // end of for loop

    // debugging output -- oops, I probably block it above ... dang! {dlb}
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

    // hackish {dlb}
    if (it_quant > 1
        && item_clas != OBJ_MISSILES
        && item_clas != OBJ_SCROLLS
        && item_clas != OBJ_POTIONS
        && item_clas != OBJ_MISCELLANY
        && (item_clas != OBJ_FOOD || item_typ != FOOD_CHUNK))
    {
        strcat(glog, "s");
    }

    return 1;
}                               // end item_name_2()

void save_id(char identy[4][50])
{
    char x = 0, jx = 0;

    for (x = 0; x < 4; x++)
    {
        for (jx = 0; jx < 50; jx++)
        {
            identy[x][jx] = id[x][jx];
        }
    }
}                               // end save_id()

void clear_ids(void)
{

    char i = 0, j = 0;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 50; j++)
        {
            id[i][j] = 0;
        }
    }

}                               // end clear_ids()


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
    case OBJ_WANDS:
        id[0][ty] = setting;
        break;
    case OBJ_SCROLLS:
        id[1][ty] = setting;
        break;
    case OBJ_JEWELLERY:
        id[2][ty] = setting;
        break;
    case OBJ_POTIONS:
        id[3][ty] = setting;
        break;
    default:
        break;
    }
}                               // end set_id()

char get_id(char cla, char ty)
{
    if (cla > 99)
    {
        cla -= 100;
        return id[cla][ty];
    }

    switch (cla)
    {
    case OBJ_WANDS:
        return id[0][ty];
    case OBJ_SCROLLS:
        return id[1][ty];
    case OBJ_JEWELLERY:
        return id[2][ty];
    case OBJ_POTIONS:
        return id[3][ty];
    default:
        return 0;
    }
}                               // end get_id()


int property(int pr1, int pr2, int pr3)
{
    return prop[pr1][pr2][pr3];
    // pr1 = OBJ class; pr2 = SUBCLASS; pr3 = properties {dlb}
}                               // end property()


int mass(int pr1, int pr2)
{
    return mss[pr1][pr2];       // pr1 = OBJ class; pr2 = SUBCLASS {dlb}
}                               // end mass()


void init_properties(void)
{
    prop[OBJ_ARMOUR][ARM_ROBE][PARM_AC] = 1;
    prop[OBJ_ARMOUR][ARM_ROBE][PARM_EVASION] = 0;
    mss[OBJ_ARMOUR][ARM_ROBE] = 60;

    prop[OBJ_ARMOUR][ARM_LEATHER_ARMOUR][PARM_AC] = 2;
    prop[OBJ_ARMOUR][ARM_LEATHER_ARMOUR][PARM_EVASION] = -1;
    mss[OBJ_ARMOUR][ARM_LEATHER_ARMOUR] = 150;

    prop[OBJ_ARMOUR][ARM_RING_MAIL][PARM_AC] = 4;
    prop[OBJ_ARMOUR][ARM_RING_MAIL][PARM_EVASION] = -2;
    mss[OBJ_ARMOUR][ARM_RING_MAIL] = 300;

    prop[OBJ_ARMOUR][ARM_SCALE_MAIL][PARM_AC] = 5;
    prop[OBJ_ARMOUR][ARM_SCALE_MAIL][PARM_EVASION] = -2;
    mss[OBJ_ARMOUR][ARM_SCALE_MAIL] = 400;

    prop[OBJ_ARMOUR][ARM_CHAIN_MAIL][PARM_AC] = 6;
    prop[OBJ_ARMOUR][ARM_CHAIN_MAIL][PARM_EVASION] = -3;
    mss[OBJ_ARMOUR][ARM_CHAIN_MAIL] = 450;

    prop[OBJ_ARMOUR][ARM_SPLINT_MAIL][PARM_AC] = 8;
    prop[OBJ_ARMOUR][ARM_SPLINT_MAIL][PARM_EVASION] = -5;
    mss[OBJ_ARMOUR][ARM_SPLINT_MAIL] = 550;

    prop[OBJ_ARMOUR][ARM_BANDED_MAIL][PARM_AC] = 7;
    prop[OBJ_ARMOUR][ARM_BANDED_MAIL][PARM_EVASION] = -4;
    mss[OBJ_ARMOUR][ARM_BANDED_MAIL] = 500;

    prop[OBJ_ARMOUR][ARM_PLATE_MAIL][PARM_AC] = 9;
    prop[OBJ_ARMOUR][ARM_PLATE_MAIL][PARM_EVASION] = -5;
    mss[OBJ_ARMOUR][ARM_PLATE_MAIL] = 650;

    prop[OBJ_ARMOUR][ARM_DRAGON_HIDE][PARM_AC] = 2;
    prop[OBJ_ARMOUR][ARM_DRAGON_HIDE][PARM_EVASION] = -2;
    mss[OBJ_ARMOUR][ARM_DRAGON_HIDE] = 220;

    prop[OBJ_ARMOUR][ARM_TROLL_HIDE][PARM_AC] = 1;
    prop[OBJ_ARMOUR][ARM_TROLL_HIDE][PARM_EVASION] = -1;
    mss[OBJ_ARMOUR][ARM_TROLL_HIDE] = 180;

    prop[OBJ_ARMOUR][ARM_CRYSTAL_PLATE_MAIL][PARM_AC] = 16;
    prop[OBJ_ARMOUR][ARM_CRYSTAL_PLATE_MAIL][PARM_EVASION] = -8;
    mss[OBJ_ARMOUR][ARM_CRYSTAL_PLATE_MAIL] = 1200;

    prop[OBJ_ARMOUR][ARM_DRAGON_ARMOUR][PARM_AC] = 8;
    prop[OBJ_ARMOUR][ARM_DRAGON_ARMOUR][PARM_EVASION] = -2;
    mss[OBJ_ARMOUR][ARM_DRAGON_ARMOUR] = 220;

    prop[OBJ_ARMOUR][ARM_TROLL_LEATHER_ARMOUR][PARM_AC] = 3;
    prop[OBJ_ARMOUR][ARM_TROLL_LEATHER_ARMOUR][PARM_EVASION] = -1;
    mss[OBJ_ARMOUR][ARM_TROLL_LEATHER_ARMOUR] = 180;

    prop[OBJ_ARMOUR][ARM_ICE_DRAGON_HIDE][PARM_AC] = 2;
    prop[OBJ_ARMOUR][ARM_ICE_DRAGON_HIDE][PARM_EVASION] = -2;
    mss[OBJ_ARMOUR][ARM_ICE_DRAGON_HIDE] = 220;

    prop[OBJ_ARMOUR][ARM_ICE_DRAGON_ARMOUR][PARM_AC] = 9;
    prop[OBJ_ARMOUR][ARM_ICE_DRAGON_ARMOUR][PARM_EVASION] = -2;
    mss[OBJ_ARMOUR][ARM_ICE_DRAGON_ARMOUR] = 220;

    prop[OBJ_ARMOUR][ARM_STEAM_DRAGON_HIDE][PARM_AC] = 0;
    prop[OBJ_ARMOUR][ARM_STEAM_DRAGON_HIDE][PARM_EVASION] = 0;
    mss[OBJ_ARMOUR][ARM_STEAM_DRAGON_HIDE] = 120;

    prop[OBJ_ARMOUR][ARM_STEAM_DRAGON_ARMOUR][PARM_AC] = 3;
    prop[OBJ_ARMOUR][ARM_STEAM_DRAGON_ARMOUR][PARM_EVASION] = 0;
    mss[OBJ_ARMOUR][ARM_STEAM_DRAGON_ARMOUR] = 120;

    prop[OBJ_ARMOUR][ARM_MOTTLED_DRAGON_HIDE][PARM_AC] = 1;
    prop[OBJ_ARMOUR][ARM_MOTTLED_DRAGON_HIDE][PARM_EVASION] = -1;
    mss[OBJ_ARMOUR][ARM_MOTTLED_DRAGON_HIDE] = 150;

    prop[OBJ_ARMOUR][ARM_MOTTLED_DRAGON_ARMOUR][PARM_AC] = 5;
    prop[OBJ_ARMOUR][ARM_MOTTLED_DRAGON_ARMOUR][PARM_EVASION] = -1;
    mss[OBJ_ARMOUR][ARM_MOTTLED_DRAGON_ARMOUR] = 150;

    prop[OBJ_ARMOUR][ARM_STORM_DRAGON_HIDE][PARM_AC] = 2;
    prop[OBJ_ARMOUR][ARM_STORM_DRAGON_HIDE][PARM_EVASION] = -5;
    mss[OBJ_ARMOUR][ARM_STORM_DRAGON_HIDE] = 400;

    prop[OBJ_ARMOUR][ARM_STORM_DRAGON_ARMOUR][PARM_AC] = 10;
    prop[OBJ_ARMOUR][ARM_STORM_DRAGON_ARMOUR][PARM_EVASION] = -5;
    mss[OBJ_ARMOUR][ARM_STORM_DRAGON_ARMOUR] = 400;

    prop[OBJ_ARMOUR][ARM_GOLD_DRAGON_HIDE][PARM_AC] = 2;
    prop[OBJ_ARMOUR][ARM_GOLD_DRAGON_HIDE][PARM_EVASION] = -10;
    mss[OBJ_ARMOUR][ARM_GOLD_DRAGON_HIDE] = 1100;

    prop[OBJ_ARMOUR][ARM_GOLD_DRAGON_ARMOUR][PARM_AC] = 13;
    prop[OBJ_ARMOUR][ARM_GOLD_DRAGON_ARMOUR][PARM_EVASION] = -10;
    mss[OBJ_ARMOUR][ARM_GOLD_DRAGON_ARMOUR] = 1100;

    prop[OBJ_ARMOUR][ARM_ANIMAL_SKIN][PARM_AC] = 1;
    prop[OBJ_ARMOUR][ARM_ANIMAL_SKIN][PARM_EVASION] = 0;
    mss[OBJ_ARMOUR][ARM_ANIMAL_SKIN] = 100;

    prop[OBJ_ARMOUR][ARM_SWAMP_DRAGON_HIDE][PARM_AC] = 1;
    prop[OBJ_ARMOUR][ARM_SWAMP_DRAGON_HIDE][PARM_EVASION] = -2;
    mss[OBJ_ARMOUR][ARM_SWAMP_DRAGON_HIDE] = 200;

    prop[OBJ_ARMOUR][ARM_SWAMP_DRAGON_ARMOUR][PARM_AC] = 7;
    prop[OBJ_ARMOUR][ARM_SWAMP_DRAGON_ARMOUR][PARM_EVASION] = -2;
    mss[OBJ_ARMOUR][ARM_SWAMP_DRAGON_ARMOUR] = 200;

    prop[OBJ_ARMOUR][ARM_SHIELD][PARM_AC] = 0;
    prop[OBJ_ARMOUR][ARM_SHIELD][PARM_EVASION] = 0;
    mss[OBJ_ARMOUR][ARM_SHIELD] = 100;

    prop[OBJ_ARMOUR][ARM_CLOAK][PARM_AC] = 1;
    prop[OBJ_ARMOUR][ARM_CLOAK][PARM_EVASION] = 0;
    mss[OBJ_ARMOUR][ARM_CLOAK] = 20;

    prop[OBJ_ARMOUR][ARM_HELMET][PARM_AC] = 1;
    prop[OBJ_ARMOUR][ARM_HELMET][PARM_EVASION] = 0;
    mss[OBJ_ARMOUR][ARM_HELMET] = 80;

    prop[OBJ_ARMOUR][ARM_GLOVES][PARM_AC] = 1;
    prop[OBJ_ARMOUR][ARM_GLOVES][PARM_EVASION] = 0;
    mss[OBJ_ARMOUR][ARM_GLOVES] = 20;

    prop[OBJ_ARMOUR][ARM_BOOTS][PARM_AC] = 1;
    prop[OBJ_ARMOUR][ARM_BOOTS][PARM_EVASION] = 0;
    mss[OBJ_ARMOUR][ARM_BOOTS] = 40;

    prop[OBJ_ARMOUR][ARM_BUCKLER][PARM_AC] = 0;
    prop[OBJ_ARMOUR][ARM_BUCKLER][PARM_EVASION] = 0;
    mss[OBJ_ARMOUR][ARM_BUCKLER] = 50;

    prop[OBJ_ARMOUR][ARM_LARGE_SHIELD][PARM_AC] = 0;
    prop[OBJ_ARMOUR][ARM_LARGE_SHIELD][PARM_EVASION] = 0;
    mss[OBJ_ARMOUR][ARM_LARGE_SHIELD] = 250;

    int i = 0;

    for (i = 0; i < 50; i++)
    {
        mss[OBJ_WANDS][i] = 100;
        mss[OBJ_FOOD][i] = 100;
        mss[OBJ_UNKNOWN_I][i] = 200;    // what are these? {dlb}

    //jmf: made scrolls, jewellery and potions weigh less.
#ifdef USE_LIGHTER_MAGIC_ITEMS
        mss[OBJ_SCROLLS][i] = 20;
        mss[OBJ_JEWELLERY][i] = 10;
        mss[OBJ_POTIONS][i] = 40;
        mss[OBJ_UNKNOWN_II][i] = 5;     // don't know what these are, yet:
        mss[OBJ_BOOKS][i] = 70;
#else
        mss[OBJ_SCROLLS][i] = 50;
        mss[OBJ_JEWELLERY][i] = 20;
        mss[OBJ_POTIONS][i] = 60;
        mss[OBJ_UNKNOWN_II][i] = 5;     // don't know what these are, yet:
        mss[OBJ_BOOKS][i] = 100;
#endif

        mss[OBJ_STAVES][i] = 130;
        mss[OBJ_ORBS][i] = 300;
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
    mss[OBJ_FOOD][FOOD_CHUNK] = 100;
    /* mss [OBJ_FOOD] [21] = 40;
       mss [OBJ_FOOD] [22] = 50;
       mss [OBJ_FOOD] [23] = 60;
       mss [OBJ_FOOD] [24] = 60;
       mss [OBJ_FOOD] [25] = 100; */

    mss[OBJ_MISCELLANY][MISC_BOTTLED_EFREET] = 250;
    mss[OBJ_MISCELLANY][MISC_CRYSTAL_BALL_OF_SEEING] = 200;

    // weapons: blunt weapons are first to help grouping them together
    //  note: AC prop can't be 0 or less because of division.
    //        If it's 1, makes no difference

    // NOTE: I have *removed* AC bit for weapons - just doesn't work
    // prop [x] [2] is speed

    prop[OBJ_WEAPONS][WPN_CLUB][PWPN_DAMAGE] = 5;
    prop[OBJ_WEAPONS][WPN_CLUB][PWPN_HIT] = 4;  // helps to get past evasion
    prop[OBJ_WEAPONS][WPN_CLUB][PWPN_SPEED] = 12;
    mss[OBJ_WEAPONS][WPN_CLUB] = 50;

    prop[OBJ_WEAPONS][WPN_MACE][PWPN_DAMAGE] = 8;
    prop[OBJ_WEAPONS][WPN_MACE][PWPN_HIT] = 3;  // helps to get past evasion
    prop[OBJ_WEAPONS][WPN_MACE][PWPN_SPEED] = 14;
    mss[OBJ_WEAPONS][WPN_MACE] = 140;

    prop[OBJ_WEAPONS][WPN_GREAT_MACE][PWPN_DAMAGE] = 16;
    prop[OBJ_WEAPONS][WPN_GREAT_MACE][PWPN_HIT] = -3;
    prop[OBJ_WEAPONS][WPN_GREAT_MACE][PWPN_SPEED] = 18;
    mss[OBJ_WEAPONS][WPN_GREAT_MACE] = 260;

    prop[OBJ_WEAPONS][WPN_FLAIL][PWPN_DAMAGE] = 9;
    prop[OBJ_WEAPONS][WPN_FLAIL][PWPN_HIT] = 2; // helps to get past evasion
    prop[OBJ_WEAPONS][WPN_FLAIL][PWPN_SPEED] = 15;
    mss[OBJ_WEAPONS][WPN_FLAIL] = 150;

    prop[OBJ_WEAPONS][WPN_SPIKED_FLAIL][PWPN_DAMAGE] = 12;
    prop[OBJ_WEAPONS][WPN_SPIKED_FLAIL][PWPN_HIT] = 1;
    prop[OBJ_WEAPONS][WPN_SPIKED_FLAIL][PWPN_SPEED] = 16;
    mss[OBJ_WEAPONS][WPN_SPIKED_FLAIL] = 170;

    prop[OBJ_WEAPONS][WPN_GREAT_FLAIL][PWPN_DAMAGE] = 17;
    prop[OBJ_WEAPONS][WPN_GREAT_FLAIL][PWPN_HIT] = -4;
    prop[OBJ_WEAPONS][WPN_GREAT_FLAIL][PWPN_SPEED] = 19;
    mss[OBJ_WEAPONS][WPN_GREAT_FLAIL] = 300;

    prop[OBJ_WEAPONS][WPN_DAGGER][PWPN_DAMAGE] = 3;
    prop[OBJ_WEAPONS][WPN_DAGGER][PWPN_HIT] = 6;  // helps to get past evasion
    prop[OBJ_WEAPONS][WPN_DAGGER][PWPN_SPEED] = 11;
    mss[OBJ_WEAPONS][WPN_DAGGER] = 20;

    prop[OBJ_WEAPONS][WPN_KNIFE][PWPN_DAMAGE] = 2;
    prop[OBJ_WEAPONS][WPN_KNIFE][PWPN_HIT] = 0; // helps to get past evasion
    prop[OBJ_WEAPONS][WPN_KNIFE][PWPN_SPEED] = 11;
    mss[OBJ_WEAPONS][WPN_KNIFE] = 10;

    prop[OBJ_WEAPONS][WPN_MORNINGSTAR][PWPN_DAMAGE] = 10;
    prop[OBJ_WEAPONS][WPN_MORNINGSTAR][PWPN_HIT] = 2;
    prop[OBJ_WEAPONS][WPN_MORNINGSTAR][PWPN_SPEED] = 15;
    mss[OBJ_WEAPONS][WPN_MORNINGSTAR] = 150;

    prop[OBJ_WEAPONS][WPN_SHORT_SWORD][PWPN_DAMAGE] = 6;
    prop[OBJ_WEAPONS][WPN_SHORT_SWORD][PWPN_HIT] = 5;
    prop[OBJ_WEAPONS][WPN_SHORT_SWORD][PWPN_SPEED] = 12;
    mss[OBJ_WEAPONS][WPN_SHORT_SWORD] = 100;

    prop[OBJ_WEAPONS][WPN_LONG_SWORD][PWPN_DAMAGE] = 10;
    prop[OBJ_WEAPONS][WPN_LONG_SWORD][PWPN_HIT] = 3;
    prop[OBJ_WEAPONS][WPN_LONG_SWORD][PWPN_SPEED] = 14;
    mss[OBJ_WEAPONS][WPN_LONG_SWORD] = 160;

    prop[OBJ_WEAPONS][WPN_GREAT_SWORD][PWPN_DAMAGE] = 16;
    prop[OBJ_WEAPONS][WPN_GREAT_SWORD][PWPN_HIT] = -1;
    prop[OBJ_WEAPONS][WPN_GREAT_SWORD][PWPN_SPEED] = 17;
    mss[OBJ_WEAPONS][WPN_GREAT_SWORD] = 250;

    prop[OBJ_WEAPONS][WPN_SCIMITAR][PWPN_DAMAGE] = 11;
    prop[OBJ_WEAPONS][WPN_SCIMITAR][PWPN_HIT] = 1;
    prop[OBJ_WEAPONS][WPN_SCIMITAR][PWPN_SPEED] = 14;
    mss[OBJ_WEAPONS][WPN_SCIMITAR] = 170;

    prop[OBJ_WEAPONS][WPN_HAND_AXE][PWPN_DAMAGE] = 7;
    prop[OBJ_WEAPONS][WPN_HAND_AXE][PWPN_HIT] = 2;
    prop[OBJ_WEAPONS][WPN_HAND_AXE][PWPN_SPEED] = 13;
    mss[OBJ_WEAPONS][WPN_HAND_AXE] = 110;

    prop[OBJ_WEAPONS][WPN_AXE][PWPN_DAMAGE] = 11;
    prop[OBJ_WEAPONS][WPN_AXE][PWPN_HIT] = 0;
    prop[OBJ_WEAPONS][WPN_AXE][PWPN_SPEED] = 16;
    mss[OBJ_WEAPONS][WPN_AXE] = 150;

    prop[OBJ_WEAPONS][WPN_BROAD_AXE][PWPN_DAMAGE] = 14;
    prop[OBJ_WEAPONS][WPN_BROAD_AXE][PWPN_HIT] = 1;
    prop[OBJ_WEAPONS][WPN_BROAD_AXE][PWPN_SPEED] = 17;
    mss[OBJ_WEAPONS][WPN_BROAD_AXE] = 180;

    prop[OBJ_WEAPONS][WPN_BATTLEAXE][PWPN_DAMAGE] = 17;
    prop[OBJ_WEAPONS][WPN_BATTLEAXE][PWPN_HIT] = -2;
    prop[OBJ_WEAPONS][WPN_BATTLEAXE][PWPN_SPEED] = 18;
    mss[OBJ_WEAPONS][WPN_BATTLEAXE] = 200;

    prop[OBJ_WEAPONS][WPN_SPEAR][PWPN_DAMAGE] = 5;
    prop[OBJ_WEAPONS][WPN_SPEAR][PWPN_HIT] = 3;
    prop[OBJ_WEAPONS][WPN_SPEAR][PWPN_SPEED] = 13;
    mss[OBJ_WEAPONS][WPN_SPEAR] = 50;

    prop[OBJ_WEAPONS][WPN_TRIDENT][PWPN_DAMAGE] = 9;
    prop[OBJ_WEAPONS][WPN_TRIDENT][PWPN_HIT] = -2;
    prop[OBJ_WEAPONS][WPN_TRIDENT][PWPN_SPEED] = 17;
    mss[OBJ_WEAPONS][WPN_TRIDENT] = 160;

    prop[OBJ_WEAPONS][WPN_DEMON_TRIDENT][PWPN_DAMAGE] = 15;
    prop[OBJ_WEAPONS][WPN_DEMON_TRIDENT][PWPN_HIT] = -2;
    prop[OBJ_WEAPONS][WPN_DEMON_TRIDENT][PWPN_SPEED] = 17;
    mss[OBJ_WEAPONS][WPN_DEMON_TRIDENT] = 160;

    prop[OBJ_WEAPONS][WPN_HALBERD][PWPN_DAMAGE] = 13;
    prop[OBJ_WEAPONS][WPN_HALBERD][PWPN_HIT] = -3;
    prop[OBJ_WEAPONS][WPN_HALBERD][PWPN_SPEED] = 19;
    mss[OBJ_WEAPONS][WPN_HALBERD] = 200;

    // sling
    // - the three properties are _not_ irrelevant here
    // - when something hits something else (either may be you)
    //   in melee, these are used.
    prop[OBJ_WEAPONS][WPN_SLING][PWPN_DAMAGE] = 1;
    prop[OBJ_WEAPONS][WPN_SLING][PWPN_HIT] = -1;
    prop[OBJ_WEAPONS][WPN_SLING][PWPN_SPEED] = 11;
    mss[OBJ_WEAPONS][WPN_SLING] = 10;

    prop[OBJ_WEAPONS][WPN_BOW][PWPN_DAMAGE] = 2;
    prop[OBJ_WEAPONS][WPN_BOW][PWPN_HIT] = -3;  // helps to get past evasion
    prop[OBJ_WEAPONS][WPN_BOW][PWPN_SPEED] = 11;
    mss[OBJ_WEAPONS][WPN_BOW] = 100;

    prop[OBJ_WEAPONS][WPN_BLOWGUN][PWPN_DAMAGE] = 1;
    prop[OBJ_WEAPONS][WPN_BLOWGUN][PWPN_HIT] = 0;  // helps to get past evasion
    prop[OBJ_WEAPONS][WPN_BLOWGUN][PWPN_SPEED] = 10;
    mss[OBJ_WEAPONS][WPN_BLOWGUN] = 50;

    prop[OBJ_WEAPONS][WPN_CROSSBOW][PWPN_DAMAGE] = 2;
    prop[OBJ_WEAPONS][WPN_CROSSBOW][PWPN_HIT] = -1;
    prop[OBJ_WEAPONS][WPN_CROSSBOW][PWPN_SPEED] = 15;
    mss[OBJ_WEAPONS][WPN_CROSSBOW] = 250;

    prop[OBJ_WEAPONS][WPN_HAND_CROSSBOW][PWPN_DAMAGE] = 1;
    prop[OBJ_WEAPONS][WPN_HAND_CROSSBOW][PWPN_HIT] = -1;
    prop[OBJ_WEAPONS][WPN_HAND_CROSSBOW][PWPN_SPEED] = 15;
    mss[OBJ_WEAPONS][WPN_HAND_CROSSBOW] = 70;

    prop[OBJ_WEAPONS][WPN_GLAIVE][PWPN_DAMAGE] = 15;
    prop[OBJ_WEAPONS][WPN_GLAIVE][PWPN_HIT] = -3;
    prop[OBJ_WEAPONS][WPN_GLAIVE][PWPN_SPEED] = 18;
    mss[OBJ_WEAPONS][WPN_GLAIVE] = 200;

    // staff - hmmm
    prop[OBJ_WEAPONS][WPN_QUARTERSTAFF][PWPN_DAMAGE] = 7;
    prop[OBJ_WEAPONS][WPN_QUARTERSTAFF][PWPN_HIT] = 6;
    prop[OBJ_WEAPONS][WPN_QUARTERSTAFF][PWPN_SPEED] = 12;
    mss[OBJ_WEAPONS][WPN_QUARTERSTAFF] = 130;

    prop[OBJ_WEAPONS][WPN_SCYTHE][PWPN_DAMAGE] = 14;
    prop[OBJ_WEAPONS][WPN_SCYTHE][PWPN_HIT] = -4;
    prop[OBJ_WEAPONS][WPN_SCYTHE][PWPN_SPEED] = 22;
    mss[OBJ_WEAPONS][WPN_SCYTHE] = 230;

    // these two should have the same speed because of 2-h ogres.
    prop[OBJ_WEAPONS][WPN_GIANT_CLUB][PWPN_DAMAGE] = 15;
    prop[OBJ_WEAPONS][WPN_GIANT_CLUB][PWPN_HIT] = -5;
    prop[OBJ_WEAPONS][WPN_GIANT_CLUB][PWPN_SPEED] = 16;
    mss[OBJ_WEAPONS][WPN_GIANT_CLUB] = 750;

    prop[OBJ_WEAPONS][WPN_GIANT_SPIKED_CLUB][PWPN_DAMAGE] = 18;
    prop[OBJ_WEAPONS][WPN_GIANT_SPIKED_CLUB][PWPN_HIT] = -6;
    prop[OBJ_WEAPONS][WPN_GIANT_SPIKED_CLUB][PWPN_SPEED] = 17;
    mss[OBJ_WEAPONS][WPN_GIANT_SPIKED_CLUB] = 850;
    // these two should have the same speed because of 2-h ogres.

    prop[OBJ_WEAPONS][WPN_EVENINGSTAR][PWPN_DAMAGE] = 12;
    prop[OBJ_WEAPONS][WPN_EVENINGSTAR][PWPN_HIT] = 2;
    prop[OBJ_WEAPONS][WPN_EVENINGSTAR][PWPN_SPEED] = 15;
    mss[OBJ_WEAPONS][WPN_EVENINGSTAR] = 150;

    prop[OBJ_WEAPONS][WPN_QUICK_BLADE][PWPN_DAMAGE] = 5;
    prop[OBJ_WEAPONS][WPN_QUICK_BLADE][PWPN_HIT] = 6;
    prop[OBJ_WEAPONS][WPN_QUICK_BLADE][PWPN_SPEED] = 7;
    mss[OBJ_WEAPONS][WPN_QUICK_BLADE] = 100;

    prop[OBJ_WEAPONS][WPN_KATANA][PWPN_DAMAGE] = 13;
    prop[OBJ_WEAPONS][WPN_KATANA][PWPN_HIT] = 4;
    prop[OBJ_WEAPONS][WPN_KATANA][PWPN_SPEED] = 13;
    mss[OBJ_WEAPONS][WPN_KATANA] = 160;

    prop[OBJ_WEAPONS][WPN_EXECUTIONERS_AXE][PWPN_DAMAGE] = 20;
    prop[OBJ_WEAPONS][WPN_EXECUTIONERS_AXE][PWPN_HIT] = -4;
    prop[OBJ_WEAPONS][WPN_EXECUTIONERS_AXE][PWPN_SPEED] = 20;
    mss[OBJ_WEAPONS][WPN_EXECUTIONERS_AXE] = 320;

    prop[OBJ_WEAPONS][WPN_DOUBLE_SWORD][PWPN_DAMAGE] = 15;
    prop[OBJ_WEAPONS][WPN_DOUBLE_SWORD][PWPN_HIT] = 3;
    prop[OBJ_WEAPONS][WPN_DOUBLE_SWORD][PWPN_SPEED] = 16;
    mss[OBJ_WEAPONS][WPN_DOUBLE_SWORD] = 220;

    prop[OBJ_WEAPONS][WPN_TRIPLE_SWORD][PWPN_DAMAGE] = 19;
    prop[OBJ_WEAPONS][WPN_TRIPLE_SWORD][PWPN_HIT] = -1;
    prop[OBJ_WEAPONS][WPN_TRIPLE_SWORD][PWPN_SPEED] = 19;
    mss[OBJ_WEAPONS][WPN_TRIPLE_SWORD] = 300;

    prop[OBJ_WEAPONS][WPN_HAMMER][PWPN_DAMAGE] = 7;
    prop[OBJ_WEAPONS][WPN_HAMMER][PWPN_HIT] = 2;
    prop[OBJ_WEAPONS][WPN_HAMMER][PWPN_SPEED] = 13;
    mss[OBJ_WEAPONS][WPN_HAMMER] = 130;

    prop[OBJ_WEAPONS][WPN_ANCUS][PWPN_DAMAGE] = 9;
    prop[OBJ_WEAPONS][WPN_ANCUS][PWPN_HIT] = 1;
    prop[OBJ_WEAPONS][WPN_ANCUS][PWPN_SPEED] = 14;
    mss[OBJ_WEAPONS][WPN_ANCUS] = 160;

    prop[OBJ_WEAPONS][WPN_WHIP][PWPN_DAMAGE] = 3;
    prop[OBJ_WEAPONS][WPN_WHIP][PWPN_HIT] = 1;  // helps to get past evasion
    prop[OBJ_WEAPONS][WPN_WHIP][PWPN_SPEED] = 14;
    mss[OBJ_WEAPONS][WPN_WHIP] = 30;

    prop[OBJ_WEAPONS][WPN_SABRE][PWPN_DAMAGE] = 7;
    prop[OBJ_WEAPONS][WPN_SABRE][PWPN_HIT] = 4;
    prop[OBJ_WEAPONS][WPN_SABRE][PWPN_SPEED] = 12;
    mss[OBJ_WEAPONS][WPN_SABRE] = 110;

    prop[OBJ_WEAPONS][WPN_DEMON_BLADE][PWPN_DAMAGE] = 13;
    prop[OBJ_WEAPONS][WPN_DEMON_BLADE][PWPN_HIT] = 2;
    prop[OBJ_WEAPONS][WPN_DEMON_BLADE][PWPN_SPEED] = 15;
    mss[OBJ_WEAPONS][WPN_DEMON_BLADE] = 200;

    prop[OBJ_WEAPONS][WPN_DEMON_WHIP][PWPN_DAMAGE] = 10;
    prop[OBJ_WEAPONS][WPN_DEMON_WHIP][PWPN_HIT] = 1;
    prop[OBJ_WEAPONS][WPN_DEMON_WHIP][PWPN_SPEED] = 14;
    mss[OBJ_WEAPONS][WPN_DEMON_WHIP] = 30;


    // MISSILES:

    prop[OBJ_MISSILES][MI_STONE][PWPN_DAMAGE] = 2;
    prop[OBJ_MISSILES][MI_STONE][PWPN_HIT] = 4;
    mss[OBJ_MISSILES][MI_STONE] = 5;

    prop[OBJ_MISSILES][MI_ARROW][PWPN_DAMAGE] = 2;
    prop[OBJ_MISSILES][MI_ARROW][PWPN_HIT] = 6;
    mss[OBJ_MISSILES][MI_ARROW] = 10;

    prop[OBJ_MISSILES][MI_NEEDLE][PWPN_DAMAGE] = 0;
    prop[OBJ_MISSILES][MI_NEEDLE][PWPN_HIT] = 1;
    mss[OBJ_MISSILES][MI_NEEDLE] = 1;

    prop[OBJ_MISSILES][MI_BOLT][PWPN_DAMAGE] = 2;
    prop[OBJ_MISSILES][MI_BOLT][PWPN_HIT] = 8;
    mss[OBJ_MISSILES][MI_BOLT] = 12;

    prop[OBJ_MISSILES][MI_DART][PWPN_DAMAGE] = 2;
    prop[OBJ_MISSILES][MI_DART][PWPN_HIT] = 4;  //whatever - for hand crossbow
    mss[OBJ_MISSILES][MI_DART] = 5;

    // large rock
    prop[OBJ_MISSILES][MI_LARGE_ROCK][PWPN_DAMAGE] = 20;
    prop[OBJ_MISSILES][MI_LARGE_ROCK][PWPN_HIT] = 10;
    mss[OBJ_MISSILES][MI_LARGE_ROCK] = 1000;
}


unsigned char check_item_knowledge(void)
{
    char st_pass[60];
    int i, j;
    char lines = 0;
    unsigned char anything = 0;
    char ft = 0;
    char yps = 0;
    int inv_count = 0;
    unsigned char ki = 0;

#ifdef DOS_TERM
    char buffer[2400];

    gettext(35, 1, 80, 25, buffer);
#endif

    strcpy(st_pass, "");

#ifdef DOS_TERM
    window(35, 1, 80, 25);
#endif

    clrscr();

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 30; j++)
        {
            if (id[i][j])
                inv_count++;
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
            break;
        case 1:
            ft = OBJ_SCROLLS;
            break;
        case 2:
            ft = OBJ_JEWELLERY;
            break;
        case 3:
            ft = OBJ_POTIONS;
            break;
        }

        for (j = 0; j < 50; j++)        // no more 50 types of any object -- cdl
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
                if (ki >= 'A' && ki <= 'z')
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

                //item_name_2(i, j, 0, 0, 1, 3, 3, st_pass);
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

        if (ki >= 'A' && ki <= 'z')
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
}                               // end check_item_knowledge()


// must be certain that you are passing the subtype
// to an OBJ_ARMOUR and nothing else, or as they say,
// "Bad Things Will Happen" {dlb}:
bool hide2armour(unsigned char *which_subtype)
{
    switch (*which_subtype)
    {
    case ARM_DRAGON_HIDE:
        *which_subtype = ARM_DRAGON_ARMOUR;
        return true;
    case ARM_TROLL_HIDE:
        *which_subtype = ARM_TROLL_LEATHER_ARMOUR;
        return true;
    case ARM_ICE_DRAGON_HIDE:
        *which_subtype = ARM_ICE_DRAGON_ARMOUR;
        return true;
    case ARM_MOTTLED_DRAGON_HIDE:
        *which_subtype = ARM_MOTTLED_DRAGON_ARMOUR;
        return true;
    case ARM_STORM_DRAGON_HIDE:
        *which_subtype = ARM_STORM_DRAGON_ARMOUR;
        return true;
    case ARM_GOLD_DRAGON_HIDE:
        *which_subtype = ARM_GOLD_DRAGON_ARMOUR;
        return true;
    case ARM_SWAMP_DRAGON_HIDE:
        *which_subtype = ARM_SWAMP_DRAGON_ARMOUR;
        return true;
    case ARM_STEAM_DRAGON_HIDE:
        *which_subtype = ARM_STEAM_DRAGON_ARMOUR;
        return true;
    default:
        return false;
    }
}                               // end hide2armour()


void make_name(unsigned char var1, unsigned char var2, unsigned char var3,
               char ncase, char str_pass[50])
{
    char name[30] = "";
    char glag[30] = "";
    FixedVector < unsigned char, 15 > numb;
    char len;
    char i = 0;
    char nexty = 0;
    char j = 0;
    char igo = 0;

    int x = 0;

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
    }

    j = numb[6];

    len = reduce(numb[reduce(numb[11]) / 2]);

    while (len < 5 && j < 10)
    {
        len += 1 + reduce(1 + numb[j]);
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

        if (nexty == 1 || (i > 0 && !is_a_vowel(name[i])))
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
            if (numb[i / 2] <= 1 && i > 3 && is_a_vowel(name[i]))
                goto two_letter;
            else
                name[i] = numb[j];

          hello:
            igo++;
        }

        if ((nexty == 0 && is_a_vowel(name[i]))
            || (nexty == 1 && !is_a_vowel(name[i])))
        {
            if (nexty == 1 && i > 0 && !is_a_vowel(name[i - 1]))
                i--;

            i--;
            continue;
        }

        if (!is_a_vowel(name[i]))
            nexty = 1;
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
}                               // end make_name()


char reduce(unsigned char reducee)
{
    while (reducee >= 26)
    {
        reducee -= 26;
    }

    return reducee;
}                               // end reduce()

bool is_a_vowel(unsigned char let)
{
    return (let == 0 || let == 4 || let == 8 || let == 14 || let == 20
            || let == 24 || let == 32);
}                               // end is_a_vowel()

char retvow(char sed)
{

    while (sed > 6)
        sed -= 6;

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
}                               // end retvow()

char retbit(char sed)
{
    return (sed % 2);
}                               // end retbit()

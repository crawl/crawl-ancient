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
#include "stuff.h"
#include "wpn-misc.h"
#include "view.h"

#ifdef MACROS
#include "macro.h"
#endif

char id[4][50];
int  prop[4][50][3];
FixedArray < int, 20, 50 > mss;

bool is_a_vowel(unsigned char let);
static char item_name_2( const item_def &item, char glog[80] );

char reduce(unsigned char reducee);
char retbit(char sed);
char retvow(char sed);

// Some convenient functions to hide the bit operations and create
// an interface layer between the code and the data in case this
// gets changed again. -- bwr
bool item_cursed( const item_def &item )
{
    return (item.flags & ISFLAG_CURSED);
}

bool item_uncursed( const item_def &item )
{
    return !(item.flags & ISFLAG_CURSED);
}

bool item_known_cursed( const item_def &item )
{
    return ((item.flags & ISFLAG_KNOW_CURSE) && (item.flags & ISFLAG_CURSED));
}

bool item_known_uncursed( const item_def &item )
{
    return ((item.flags & ISFLAG_KNOW_CURSE) && !(item.flags & ISFLAG_CURSED));
}

bool fully_identified( const item_def &item )
{
    return ((item.flags & ISFLAG_IDENT_MASK) == ISFLAG_IDENT_MASK);
}

bool item_ident( const item_def &item, unsigned long flags )
{
    return (item.flags & flags);
}

bool item_not_ident( const item_def &item, unsigned long flags )
{
    return ( !(item.flags & flags) );
}

void do_curse_item( item_def &item )
{
    item.flags |= ISFLAG_CURSED;
}

void do_uncurse_item( item_def &item )
{
    item.flags &= (~ISFLAG_CURSED);
}

void set_ident_flags( item_def &item, unsigned long flags )
{
    item.flags |= flags;
}

void unset_ident_flags( item_def &item, unsigned long flags )
{
    item.flags &= (~flags);
}

// These six functions might seem silly, but they provide a nice layer
// for later changes to these systems. -- bwr
unsigned long get_equip_race( const item_def &item )
{
    return (item.flags & ISFLAG_RACIAL_MASK);
}

unsigned long get_equip_desc( const item_def &item )
{
    return (item.flags & ISFLAG_COSMETIC_MASK);
}

bool cmp_equip_race( const item_def &item, unsigned long val )
{
    return (get_equip_race( item ) == val);
}

bool cmp_equip_desc( const item_def &item, unsigned long val )
{
    return (get_equip_desc( item ) == val);
}

void set_equip_race( item_def &item, unsigned long flags )
{
    ASSERT( (flags & ~ISFLAG_RACIAL_MASK) == 0 );

    item.flags &= ~ISFLAG_RACIAL_MASK; // delete previous
    item.flags |= flags;
}

void set_equip_desc( item_def &item, unsigned long flags )
{
    ASSERT( (flags & ~ISFLAG_COSMETIC_MASK) == 0 );

    item.flags &= ~ISFLAG_COSMETIC_MASK; // delete previous
    item.flags |= flags;
}

short get_helmet_type( const item_def &item )
{
    ASSERT( item.base_type == OBJ_ARMOUR && item.sub_type == ARM_HELMET );

    return (item.plus2 & THELM_TYPE_MASK);
}

short get_helmet_desc( const item_def &item )
{
    ASSERT( item.base_type == OBJ_ARMOUR && item.sub_type == ARM_HELMET );

    return (item.plus2 & THELM_DESC_MASK);
}

void set_helmet_type( item_def &item, short type )
{
    ASSERT( (type & ~THELM_TYPE_MASK) == 0 );
    ASSERT( item.base_type == OBJ_ARMOUR && item.sub_type == ARM_HELMET );

    item.plus2 &= ~THELM_TYPE_MASK;
    item.plus2 |= type;
}

void set_helmet_desc( item_def &item, short type )
{
    ASSERT( (type & ~THELM_DESC_MASK) == 0 );
    ASSERT( item.base_type == OBJ_ARMOUR && item.sub_type == ARM_HELMET );

    item.plus2 &= ~THELM_DESC_MASK;
    item.plus2 |= type;
}

void set_helmet_random_desc( item_def &item )
{
    ASSERT( item.base_type == OBJ_ARMOUR && item.sub_type == ARM_HELMET );

    item.plus2 &= ~THELM_DESC_MASK;
    item.plus2 |= (random2(8) << 8);
}

bool cmp_helmet_type( const item_def &item, short val )
{
    ASSERT( item.base_type == OBJ_ARMOUR && item.sub_type == ARM_HELMET );

    return (get_helmet_type( item ) == val);
}

bool cmp_helmet_desc( const item_def &item, short val )
{
    ASSERT( item.base_type == OBJ_ARMOUR && item.sub_type == ARM_HELMET );

    return (get_helmet_desc( item ) == val);
}


// it_name() and in_name() are now somewhat obsolete now that itemname
// takes item_def, so consider them depricated.
void it_name(int itn, char des, char str_pass[80])
{
    item_name( mitm[itn], des, str_pass );
}                               // end it_name()


void in_name(int inn, char des, char str_pass[80])
{
    item_name( you.inv[inn], des, str_pass );
}                               // end in_name()

// quant_name is usful since it prints out a different number of items
// than the item actually contains.
void quant_name(const item_def &item, int quant, char des, char str_pass[80])
{
    // item_name now requires a "real" item, so we'll mangle a tmp
    item_def tmp = item;
    tmp.quantity = quant;

    item_name( tmp, des, str_pass );
}                               // end quant_name()

char item_name( const item_def &item, char descrip, char glag[80] )
{
    const int item_clas = item.base_type;
    const int item_typ = item.sub_type;
    const int it_quant = item.quantity;

    char itm_name[60] = "";
    char tmp_quant[5];

    item_name_2( item, itm_name );

    strcpy(glag, "");

    if (descrip == DESC_INVENTORY_EQUIP || descrip == DESC_INVENTORY)
    {
        if (item.x == -1 && item.y == -1)
        {
            // actually in inventory
            char letter = index_to_letter( item.link );
            glag[0] = letter;
            glag[1] = '\0';

            strcat( glag, " - " );
        }
        else
        {
            // not in inventory
            descrip = DESC_CAP_A;
        }
    }

    if (item_clas == OBJ_ORBS
        || (item_ident( item, ISFLAG_KNOW_TYPE )
            && ((item_clas == OBJ_MISCELLANY
                    && item_typ == MISC_HORN_OF_GERYON)
                || (is_fixed_artefact( item )
                || (is_random_artefact( item ))))))
    {
        // artefacts always get "the" unless we just want the plain name
        switch (descrip)
        {
        case DESC_CAP_A:
        case DESC_CAP_YOUR:
        case DESC_CAP_THE:
            strcat(glag, "The ");
            break;
        case DESC_NOCAP_A:
        case DESC_NOCAP_YOUR:
        case DESC_NOCAP_THE:
        case DESC_NOCAP_ITS:
        case DESC_INVENTORY_EQUIP:
        case DESC_INVENTORY:
            strcat(glag, "the ");
            break;
        default:
        case DESC_PLAIN:
            break;
        }
    }
    else if (it_quant > 1)
    {
        switch (descrip)
        {
        case DESC_CAP_THE:
            strcat(glag, "The ");
            break;
        case DESC_NOCAP_THE:
            strcat(glag, "the ");
            break;
        case DESC_CAP_A:
        case DESC_NOCAP_A:
        case DESC_INVENTORY_EQUIP:
        case DESC_INVENTORY:
            break;
        case DESC_CAP_YOUR:
            strcat(glag, "Your ");
            break;
        case DESC_NOCAP_YOUR:
            strcat(glag, "your ");
            break;
        case DESC_NOCAP_ITS:
            strcat(glag, "its ");
            break;
        case DESC_PLAIN:
        default:
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
        case DESC_CAP_THE:
            strcat(glag, "The ");
            break;
        case DESC_NOCAP_THE:
            strcat(glag, "the ");
            break;
        case DESC_CAP_A:
            strcat(glag, "A");

            if (itm_name[0] == 'a' || itm_name[0] == 'e' || itm_name[0] == 'i'
                || itm_name[0] == 'o' || itm_name[0] == 'u')
            {
                strcat(glag, "n");
            }

            strcat(glag, " ");
            break;              // A/An

        case DESC_NOCAP_A:
        case DESC_INVENTORY_EQUIP:
        case DESC_INVENTORY:
            strcat(glag, "a");

            if (itm_name[0] == 'a' || itm_name[0] == 'e' || itm_name[0] == 'i'
                || itm_name[0] == 'o' || itm_name[0] == 'u')
            {
                strcat(glag, "n");
            }

            strcat(glag, " ");
            break;              // a/an

        case DESC_CAP_YOUR:
            strcat(glag, "Your ");
            break;
        case DESC_NOCAP_YOUR:
            strcat(glag, "your ");
            break;
        case DESC_NOCAP_ITS:
            strcat(glag, "its ");
            break;
        case DESC_PLAIN:
        default:
            break;
        }
    }                           // end of else

    strcat(glag, itm_name);

    if (descrip == DESC_INVENTORY_EQUIP && item.x == -1 && item.y == -1)
    {
        ASSERT( item.link != -1 );

        if (item.link == you.equip[EQ_WEAPON])
        {
            if (you.inv[ you.equip[EQ_WEAPON] ].base_type == OBJ_WEAPONS
                || you.inv[ you.equip[EQ_WEAPON] ].base_type == OBJ_STAVES)
            {
                strcat( glag, " (weapon)" );
            }
            else
            {
                strcat( glag, " (in hand)" );
            }
        }
        else if (item.link == you.equip[EQ_CLOAK]
                || item.link == you.equip[EQ_HELMET]
                || item.link == you.equip[EQ_GLOVES]
                || item.link == you.equip[EQ_BOOTS]
                || item.link == you.equip[EQ_SHIELD]
                || item.link == you.equip[EQ_BODY_ARMOUR])
        {
            strcat( glag, " (worn)" );
        }
        else if (item.link == you.equip[EQ_LEFT_RING])
        {
            strcat( glag, " (left hand)" );
        }
        else if (item.link == you.equip[EQ_RIGHT_RING])
        {
            strcat( glag, " (right hand)" );
        }
        else if (item.link == you.equip[EQ_AMULET])
        {
            strcat( glag, " (around neck)" );
        }
    }

    return (1);
}                               // end item_name()


static char item_name_2( const item_def &item, char glog[80] )
{
    const int item_clas = item.base_type;
    const int item_typ = item.sub_type;
    const int it_plus = item.plus;
    const int item_plus2 = item.plus2;
    const int item_special = item.special;
    const int it_quant = item.quantity;

    char tmp_quant[5];
    char str_pass2[80];

    strcpy(glog, "");

    switch (item_clas)
    {
    case OBJ_WEAPONS:
        if (item_ident( item, ISFLAG_KNOW_CURSE ))
        {
            // We don't bother printing "uncursed" if the item is identified
            // for pluses (it's state should be obvious), this is so that
            // the weapon name is kept short (there isn't a lot of room
            // for the name on the main screen).  If you're going to change
            // this behaviour, *please* make it so that there is an option
            // that maintains this behaviour. -- bwr
            if (item_cursed( item ))
                strcat(glog, "cursed ");
            else if (Options.show_uncursed
                    && item_not_ident( item, ISFLAG_KNOW_PLUSES ))
            {
                strcat(glog, "uncursed ");
            }
        }

        if (item_ident( item, ISFLAG_KNOW_PLUSES ))
        {
            if (it_plus == 0 && item_plus2 == 0)
            {
                strcat(glog, "+0 ");
            }
            else
            {
                if (it_plus >= 0)
                    strcat( glog, "+" );

                itoa( it_plus, tmp_quant, 10 );

                strcat( glog, tmp_quant );
                strcat( glog, "," );

                if (item_plus2 >= 0)
                    strcat(glog, "+");

                itoa( item_plus2, tmp_quant, 10 );

                strcat( glog, tmp_quant );
                strcat( glog, " " );
            }
        }

        if (is_random_artefact( item ))
        {
            strcat( glog, randart_name(item) );
            break;
        }

        if (is_fixed_artefact( item ))
        {
            if (item_ident( item, ISFLAG_KNOW_TYPE ))
            {
                strcat(glog,
                       (item_special == SPWPN_SINGING_SWORD) ? "Singing Sword" :
                       (item_special == SPWPN_WRATH_OF_TROG) ? "Wrath of Trog" :
                       (item_special == SPWPN_SCYTHE_OF_CURSES) ? "Scythe of Curses" :
                       (item_special == SPWPN_MACE_OF_VARIABILITY) ? "Mace of Variability" :
                       (item_special == SPWPN_GLAIVE_OF_PRUNE) ? "Glaive of Prune" :
                       (item_special == SPWPN_SCEPTRE_OF_TORMENT) ? "Sceptre of Torment" :
                       (item_special == SPWPN_SWORD_OF_ZONGULDROK) ? "Sword of Zonguldrok" :
                       (item_special == SPWPN_SWORD_OF_CEREBOV) ? "Sword of Cerebov" :
                       (item_special == SPWPN_STAFF_OF_DISPATER) ? "Staff of Dispater" :
                       (item_special == SPWPN_SCEPTRE_OF_ASMODEUS) ? "Sceptre of Asmodeus" :
                       (item_special == SPWPN_SWORD_OF_POWER) ? "Sword of Power" :
                       (item_special == SPWPN_KNIFE_OF_ACCURACY) ? "Knife of Accuracy" :
                       (item_special == SPWPN_STAFF_OF_OLGREB) ? "Staff of Olgreb" :
                       (item_special == SPWPN_VAMPIRES_TOOTH) ? "Vampire's Tooth" :
                       (item_special == SPWPN_STAFF_OF_WUCAD_MU) ? "Staff of Wucad Mu"
                                                   : "Brodale's Buggy Bola");
            }
            else
            {
                strcat(glog,
                       (item_special == SPWPN_SINGING_SWORD) ? "golden long sword" :
                       (item_special == SPWPN_WRATH_OF_TROG) ? "bloodstained battleaxe" :
                       (item_special == SPWPN_SCYTHE_OF_CURSES) ? "warped scythe" :
                       (item_special == SPWPN_MACE_OF_VARIABILITY) ? "shimmering mace" :
                       (item_special == SPWPN_GLAIVE_OF_PRUNE) ? "purple glaive" :
                       (item_special == SPWPN_SCEPTRE_OF_TORMENT) ? "jeweled golden mace" :
                       (item_special == SPWPN_SWORD_OF_ZONGULDROK) ? "bone long sword" :
                       (item_special == SPWPN_SWORD_OF_CEREBOV) ? "great serpentine sword" :
                       (item_special == SPWPN_STAFF_OF_DISPATER) ? "golden staff" :
                       (item_special == SPWPN_SCEPTRE_OF_ASMODEUS) ? "ruby sceptre" :
                       (item_special == SPWPN_SWORD_OF_POWER) ? "chunky great sword" :
                       (item_special == SPWPN_KNIFE_OF_ACCURACY) ? "thin dagger" :
                       (item_special == SPWPN_STAFF_OF_OLGREB) ? "green glowing staff" :
                       (item_special == SPWPN_VAMPIRES_TOOTH) ? "ivory dagger" :
                       (item_special == SPWPN_STAFF_OF_WUCAD_MU) ? "quarterstaff"
                                                           : "bola");
            }
            break;
        }

        // Now that we can have "glowing elven" weapons, it's
        // probably a good idea to cut out the descriptive
        // term once it's become obsolete. -- bwr
        if (item_not_ident( item, ISFLAG_KNOW_PLUSES ))
        {
            switch (get_equip_desc( item ))
            {
            case ISFLAG_RUNED:
                strcat(glog, "runed ");
                break;
            case ISFLAG_GLOWING:
                strcat(glog, "glowing ");
                break;
            }
        }

        // always give racial type (it does have game effects)
        switch (get_equip_race( item ))
        {
        case ISFLAG_ORCISH:
            strcat(glog, "orcish ");
            break;
        case ISFLAG_ELVEN:
            strcat(glog, "elven ");
            break;
        case ISFLAG_DWARVEN:
            strcat(glog, "dwarven ");
            break;
        }

        if (item_ident( item, ISFLAG_KNOW_TYPE ))
        {
            if (item_special == SPWPN_VAMPIRICISM)
                strcat(glog, "vampiric ");
        }                       // end if

        standard_name_weap(item_typ, str_pass2);
        strcat(glog, str_pass2);

        if (item_ident( item, ISFLAG_KNOW_TYPE ))
        {
            switch (item_special)
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
        break;

    case OBJ_MISSILES:
        // maybe type of poison should be hidden? -- bwr
        if (item_special == SPMSL_POISONED || item_special == SPMSL_POISONED_II)
            strcat(glog, "poisoned ");

        if (item_ident( item, ISFLAG_KNOW_PLUSES ))
        {
            if (it_plus >= 0)
                strcat(glog, "+");

            itoa( it_plus, tmp_quant, 10 );

            strcat(glog, tmp_quant);
            strcat(glog, " ");
        }

        if (get_equip_race( item ))
        {
            int dwpn = get_equip_race( item );

            strcat(glog, (dwpn == ISFLAG_ORCISH) ? "orcish " :
                   (dwpn == ISFLAG_ELVEN) ? "elven " :
                   (dwpn == ISFLAG_DWARVEN) ? "dwarven " : "buggy");
        }

        strcat(glog, (item_typ == MI_STONE) ? "stone" :
               (item_typ == MI_ARROW) ? "arrow" :
               (item_typ == MI_BOLT) ? "bolt" :
               (item_typ == MI_DART) ? "dart" :
               (item_typ == MI_NEEDLE) ? "needle" :
               (item_typ == MI_EGGPLANT) ? "eggplant" :
               (item_typ == MI_LARGE_ROCK) ? "large rock" : "");
               // this should probably be "" {dlb}

        if (it_quant > 1)
            strcat(glog, "s");

        if (item_ident( item, ISFLAG_KNOW_TYPE ))
        {
            strcat(glog, (item_special == SPMSL_NORMAL)           ? "" :
                         (item_special == SPMSL_POISONED
                           || item_special == SPMSL_POISONED_II)  ? "" :
                         (item_special == SPMSL_FLAME)  ? " of flame"  :
                         (item_special == SPMSL_ICE)    ? " of ice"
                                                        : " of bugginess");
        }
        break;

    case OBJ_ARMOUR:
        if (item_ident( item, ISFLAG_KNOW_CURSE ))
        {
            if (item_cursed( item ))
                strcat(glog, "cursed ");
            else if (Options.show_uncursed
                    && item_not_ident( item, ISFLAG_KNOW_PLUSES ))
            {
                strcat(glog, "uncursed ");
            }
        }

        if (item_ident( item, ISFLAG_KNOW_PLUSES ))
        {
            if (it_plus >= 0)
                strcat(glog, "+");

            itoa( it_plus, tmp_quant, 10 );

            strcat(glog, tmp_quant);
            strcat(glog, " ");
        }

        if (item_typ == ARM_GLOVES
            || (item_typ == ARM_BOOTS && item_plus2 == TBOOT_BOOTS))
        {
            strcat(glog, "pair of ");
        }

        if (is_random_artefact( item ))
        {
            strcat(glog, randart_armour_name(item));
            break;
        }

        if (item_typ != ARM_HELMET)
        {
            // Now that we can have "glowing elven" armour, it's
            // probably a good idea to cut out the descriptive
            // term once it's become obsolete. -- bwr
            if (item_not_ident( item, ISFLAG_KNOW_PLUSES ))
            {
                switch (get_equip_desc( item ))
                {
                case ISFLAG_EMBROIDERED_SHINY:
                    if (item_typ == ARM_ROBE || item_typ == ARM_CLOAK
                        || item_typ == ARM_GLOVES || item_typ == ARM_BOOTS)
                    {
                        strcat(glog, "embroidered ");
                    }
                    else if (item_typ != ARM_LEATHER_ARMOUR)
                        strcat(glog, "shiny ");
                    break;

                case ISFLAG_RUNED:
                    strcat(glog, "runed ");
                    break;
                case ISFLAG_GLOWING:
                    strcat(glog, "glowing ");
                    break;
                }
            }

            // always give racial description (has game effects)
            switch (get_equip_race( item ))
            {
            case ISFLAG_ELVEN:
                strcat(glog, "elven ");
                break;
            case ISFLAG_DWARVEN:
                strcat(glog, "dwarven ");
                break;
            case ISFLAG_ORCISH:
                strcat(glog, "orcish ");
                break;
            }               // end switch
        }

        standard_name_armour( item, str_pass2 );  // in randart.cc
        strcat(glog, str_pass2);

        if (item_ident( item, ISFLAG_KNOW_TYPE )
            && (item_special != SPARM_NORMAL))
        {
            const unsigned char sparm = item_special;

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
        if (id[ IDTYPE_WANDS ][item_typ] == ID_KNOWN_TYPE
            || item_ident( item, ISFLAG_KNOW_TYPE ))
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
            char primary = (item_special % 12);
            char secondary = (item_special / 12);

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

            if (id[ IDTYPE_WANDS ][item_typ] == ID_TRIED_TYPE)
            {
                strcat( glog, " {tried}" );
            }
        }

        if (item_ident( item, ISFLAG_KNOW_PLUSES ))
        {
            strcat(glog, " (");
            itoa( it_plus, tmp_quant, 10 );
            strcat(glog, tmp_quant);
            strcat(glog, ")");
        }
        break;

    // NB: potions, food, and scrolls stack on the basis of class and
    // type ONLY !!!

    // compacted 15 Apr 2000 {dlb}:
    case OBJ_POTIONS:
        if (id[ IDTYPE_POTIONS ][item_typ] == ID_KNOWN_TYPE
            || item_ident( item, ISFLAG_KNOW_TYPE ))
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
            char primary = item_special / 14;
            char secondary = item_special % 14;

            strcat(glog,
                   (primary ==  0) ? "" :
                   (primary ==  1) ? "bubbling " :
                   (primary ==  2) ? "lumpy " :
                   (primary ==  3) ? "fuming " :
                   (primary ==  4) ? "smoky " :
                   (primary ==  5) ? "fizzy " :
                   (primary ==  6) ? "glowing " :
                   (primary ==  7) ? "sedimented " :
                   (primary ==  8) ? "metallic " :
                   (primary ==  9) ? "murky " :
                   (primary == 10) ? "gluggy " :
                   (primary == 11) ? "viscous " :
                   (primary == 12) ? "oily " :
                   (primary == 13) ? "slimy " :
                   (primary == 14) ? "emulsified " : "buggy ");

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

            if (id[ IDTYPE_POTIONS ][item_typ] == ID_TRIED_TYPE)
            {
                strcat( glog, " {tried}" );
            }
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

            moname( it_plus, true, DESC_PLAIN, gmo_n2 );

            if (item_special < 100)
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

        if (id[ IDTYPE_SCROLLS ][item_typ] == ID_KNOWN_TYPE
            || item_ident( item, ISFLAG_KNOW_TYPE ))
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

            make_name( item_special, it_plus, item_clas, 2, str_pass );
            strcat(glog, str_pass);

            if (id[ IDTYPE_SCROLLS ][item_typ] == ID_TRIED_TYPE)
            {
                strcat( glog, " {tried}" );
            }
        }
        break;

    // compacted 15 Apr 2000 {dlb}: -- on hold ... what a mess!
    case OBJ_JEWELLERY:
        // not using {tried} here because there are some confusing
        // issues to work out with how we want to handle jewellery
        // artefacts and base type id. -- bwr
        if (item_ident( item, ISFLAG_KNOW_CURSE ))
        {
            if (item_cursed( item ))
                strcat(glog, "cursed ");
            else if (Options.show_uncursed
                    && item_not_ident( item, ISFLAG_KNOW_PLUSES ))
            {
                strcat(glog, "uncursed ");
            }
        }

        if (item_ident( item, ISFLAG_KNOW_TYPE ) && is_random_artefact( item ))
        {
            strcat(glog, randart_ring_name(item));
            break;
        }

        if (id[ IDTYPE_JEWELLERY ][item_typ] == ID_KNOWN_TYPE
            || item_ident( item, ISFLAG_KNOW_TYPE ))
        {

            if (item_ident( item, ISFLAG_KNOW_PLUSES )
                && (item_typ == RING_PROTECTION || item_typ == RING_STRENGTH
                    || item_typ == RING_SLAYING || item_typ == RING_EVASION
                    || item_typ == RING_DEXTERITY
                    || item_typ == RING_INTELLIGENCE))
            {
                char gokh = it_plus;

                if (gokh >= 0)
                    strcat( glog, "+" );

                itoa( gokh, tmp_quant, 10 );
                strcat( glog, tmp_quant );

                if (item_typ == RING_SLAYING)
                {
                    strcat( glog, "," );

                    if (item_plus2 >= 0)
                        strcat(glog, "+");

                    itoa( item_plus2, tmp_quant, 10 );
                    strcat( glog, tmp_quant );
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
        }

        if (item_typ < AMU_RAGE)        // rings
        {
            if (is_random_artefact( item ))
            {
                strcat(glog, randart_ring_name(item));
                break;
            }

            switch (item_special / 13)       // secondary characteristic of ring
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

            switch (item_special % 13)
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
            if (is_random_artefact( item ))
            {
                strcat(glog, randart_ring_name(item));
                break;
            }

            if (item_special > 13)
            {
                switch (item_special / 13)   // secondary characteristic of amulet
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

            switch (item_special % 13)
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
            strcat( glog, (it_plus == RUNE_DIS)          ? "iron" :
                          (it_plus == RUNE_GEHENNA)      ? "obsidian" :
                          (it_plus == RUNE_COCYTUS)      ? "icy" :
                          (it_plus == RUNE_TARTARUS)     ? "bone" :
                          (it_plus == RUNE_SLIME_PITS)   ? "slimy" :
                          (it_plus == RUNE_VAULTS)       ? "silver" :
                          (it_plus == RUNE_SNAKE_PIT)    ? "serpentine" :
                          (it_plus == RUNE_ELVEN_HALLS)  ? "elven" :
                          (it_plus == RUNE_TOMB)         ? "golden" :
                          (it_plus == RUNE_SWAMP)        ? "decaying" :

                          // pandemonium and abyss runes:
                          (it_plus == RUNE_DEMONIC)      ? "demonic" :
                          (it_plus == RUNE_ABYSSAL)      ? "abyssal" :

                          // special pandemonium runes:
                          (it_plus == RUNE_MNOLEG)       ? "glowing" :
                          (it_plus == RUNE_LOM_LOBON)    ? "magical" :
                          (it_plus == RUNE_CEREBOV)      ? "firey" :
                          (it_plus == RUNE_GLOORX_VLOQ)  ? "dark"
                                                         : "buggy" );

            strcat(glog, " ");
            strcat(glog, "rune");

            if (it_quant > 1)
                strcat(glog, "s");

            if (item_ident( item, ISFLAG_KNOW_TYPE ))
                strcat(glog, " of Zot");
            break;

        case MISC_DECK_OF_POWER:
        case MISC_DECK_OF_SUMMONINGS:
        case MISC_DECK_OF_TRICKS:
        case MISC_DECK_OF_WONDERS:
            strcat(glog, "deck of ");
            strcat(glog, item_not_ident( item, ISFLAG_KNOW_TYPE )  ? "cards"  :
                   (item_typ == MISC_DECK_OF_WONDERS)      ? "wonders" :
                   (item_typ == MISC_DECK_OF_SUMMONINGS)   ? "summonings" :
                   (item_typ == MISC_DECK_OF_TRICKS)       ? "tricks" :
                   (item_typ == MISC_DECK_OF_POWER)        ? "power"
                                                           : "bugginess");
            break;

        case MISC_CRYSTAL_BALL_OF_ENERGY:
        case MISC_CRYSTAL_BALL_OF_FIXATION:
        case MISC_CRYSTAL_BALL_OF_SEEING:
            strcat(glog, "crystal ball");
            if (item_ident( item, ISFLAG_KNOW_TYPE ))
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
            if (item_ident( item, ISFLAG_KNOW_TYPE ))
                strcat(glog, "box of beasts");
            else
                strcat(glog, "small ebony casket");
            break;

        case MISC_EMPTY_EBONY_CASKET:
            if (item_ident( item, ISFLAG_KNOW_TYPE ))
                strcat(glog, "empty ebony casket");
            else
                strcat(glog, "small ebony casket");
            break;

        case MISC_AIR_ELEMENTAL_FAN:
            if (item_ident( item, ISFLAG_KNOW_TYPE ))
                strcat(glog, "air elemental ");
            strcat(glog, "fan");
            break;

        case MISC_LAMP_OF_FIRE:
            strcat(glog, "lamp");
            if (item_ident( item, ISFLAG_KNOW_TYPE ))
                strcat(glog, " of fire");
            break;

        case MISC_LANTERN_OF_SHADOWS:
            if (item_not_ident( item, ISFLAG_KNOW_TYPE ))
                strcat(glog, "bone ");
            strcat(glog, "lantern");

            if (item_ident( item, ISFLAG_KNOW_TYPE ))
                strcat(glog, " of shadows");
            break;

        case MISC_HORN_OF_GERYON:
            if (item_not_ident( item, ISFLAG_KNOW_TYPE ))
                strcat(glog, "silver ");
            strcat(glog, "horn");

            if (item_ident( item, ISFLAG_KNOW_TYPE ))
                strcat(glog, " of Geryon");
            break;

        case MISC_DISC_OF_STORMS:
            if (item_not_ident( item, ISFLAG_KNOW_TYPE ))
                strcat(glog, "grey ");
            strcat(glog, "disc");

            if (item_ident( item, ISFLAG_KNOW_TYPE ))
                strcat(glog, " of storms");
            break;

        case MISC_STONE_OF_EARTH_ELEMENTALS:
            if (item_not_ident( item, ISFLAG_KNOW_TYPE ))
                strcat(glog, "nondescript ");
            strcat(glog, "stone");

            if (item_ident( item, ISFLAG_KNOW_TYPE ))
                strcat(glog, " of earth elementals");
            break;

        case MISC_BOTTLED_EFREET:
            strcat(glog, (item_not_ident( item, ISFLAG_KNOW_TYPE ))
                                ? "sealed bronze flask" : "bottled efreet");
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
        if (item_not_ident( item, ISFLAG_KNOW_TYPE ))
        {
            char primary = (item_special / 10);
            char secondary = (item_special % 10);

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
        if (item_not_ident( item, ISFLAG_KNOW_TYPE ))
        {
            strcat(glog, (item_special == 0) ? "curved" :
                   (item_special == 1) ? "glowing" :
                   (item_special == 2) ? "thick" :
                   (item_special == 3) ? "thin" :
                   (item_special == 4) ? "long" :
                   (item_special == 5) ? "twisted" :
                   (item_special == 6) ? "jeweled" :
                   (item_special == 7) ? "runed" :
                   (item_special == 8) ? "smoking" :
                   (item_special == 9) ? "gnarled" :    // was "" {dlb}
                   (item_special == 10) ? "" :
                   (item_special == 11) ? "" :
                   (item_special == 12) ? "" :
                   (item_special == 13) ? "" :
                   (item_special == 14) ? "" :
                   (item_special == 15) ? "" :
                   (item_special == 16) ? "" :
                   (item_special == 17) ? "" :
                   (item_special == 18) ? "" :
                   (item_special == 19) ? "" :
                   (item_special == 20) ? "" :
                   (item_special == 21) ? "" :
                   (item_special == 22) ? "" :
                   (item_special == 23) ? "" :
                   (item_special == 24) ? "" :
                   (item_special == 25) ? "" :
                   (item_special == 26) ? "" :
                   (item_special == 27) ? "" :
                   (item_special == 28) ? "" :
                   (item_special == 29) ? "" : "buggy");
            strcat(glog, " ");
        }

        if (item_ident( item, ISFLAG_KNOW_TYPE ))
        {
                   strcat(glog, (
                          // confusing that II is spell and I is not {dlb}
                             item_typ == STAFF_SPELL_SUMMONING
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

        if (item_ident( item, ISFLAG_KNOW_TYPE ))
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
                   (item_typ == STAFF_SUMMONING
                    || item_typ == STAFF_SPELL_SUMMONING) ? "summoning" :
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
        break;

    // still not implemented, yet:
    case OBJ_GEMSTONES:
        break;

    // rearranged 15 Apr 2000 {dlb}:
    case OBJ_CORPSES:
        if (item_typ == CORPSE_BODY && item_special < 100)
            strcat(glog, "rotting ");

        char gmo_n[40];

        moname(it_plus, true, DESC_PLAIN, gmo_n);

        strcat(glog, gmo_n);
        strcat(glog, " ");
        strcat(glog, (item_typ == CORPSE_BODY) ? "corpse" :
               (item_typ == CORPSE_SKELETON) ? "skeleton" : "corpse bug");
        break;

    default:
        strcat(glog, "!");
    }                           // end of switch?

    // debugging output -- oops, I probably block it above ... dang! {dlb}
    if (strlen(glog) < 3)
    {
        char ugug[4];

        strcat(glog, "questionable item (cl:");
        itoa(item_clas, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ",ty:");
        itoa(item_typ, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ",pl:");
        itoa(it_plus, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ",pl2:");
        itoa(item_plus2, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ",sp:");
        itoa(item_special, ugug, 10);
        strcat(glog, ugug);
        strcat(glog, ",qu:");
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
            id[i][j] = ID_UNKNOWN_TYPE;
        }
    }

}                               // end clear_ids()


void set_ident_type( char cla, char ty, char setting )
{
    // don't allow tried to overwrite known
    if (setting == ID_TRIED_TYPE && get_ident_type( cla, ty ) == ID_KNOWN_TYPE)
        return;

    switch (cla)
    {
    case OBJ_WANDS:
        id[ IDTYPE_WANDS ][ty] = setting;
        break;

    case OBJ_SCROLLS:
        id[ IDTYPE_SCROLLS ][ty] = setting;
        break;

    case OBJ_JEWELLERY:
        id[ IDTYPE_JEWELLERY ][ty] = setting;
        break;

    case OBJ_POTIONS:
        id[ IDTYPE_POTIONS ][ty] = setting;
        break;

    default:
        break;
    }
}                               // end set_ident_type()

char get_ident_type(char cla, char ty)
{
    switch (cla)
    {
    case OBJ_WANDS:
        return id[ IDTYPE_WANDS ][ty];

    case OBJ_SCROLLS:
        return id[ IDTYPE_SCROLLS ][ty];

    case OBJ_JEWELLERY:
        return id[ IDTYPE_JEWELLERY ][ty];

    case OBJ_POTIONS:
        return id[ IDTYPE_POTIONS ][ty];

    default:
        return (ID_UNKNOWN_TYPE);
    }
}                               // end get_ident_type()

int property( const item_def &item, int prop_type )
{
    switch (item.base_type)
    {
    case OBJ_ARMOUR:
    case OBJ_WEAPONS:
    case OBJ_MISSILES:
        return (prop[ item.base_type ][ item.sub_type ][ prop_type ]);

    case OBJ_STAVES:
        return (prop[ OBJ_WEAPONS ][ WPN_QUARTERSTAFF ][ prop_type ]);

    default:
        return (0);
    }
}

int mass_item( const item_def &item )
{
    int unit_mass = 0;

    if (item.base_type == OBJ_GOLD)
    {
        unit_mass = 0;
    }
    else if (item.base_type == OBJ_CORPSES)
    {
        unit_mass = mons_weight( item.plus );

        if (item.sub_type == CORPSE_SKELETON)
            unit_mass /= 2;
    }
    else
    {
        unit_mass = mss[ item.base_type ][ item.sub_type ];
    }

    return (unit_mass > 0 ? unit_mass : 0);
}

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
        mss[OBJ_UNKNOWN_I][i] = 200;    // labeled as "books" elsewhere

        //jmf: made scrolls, jewellery and potions weigh less.
        mss[OBJ_SCROLLS][i] = 20;
        mss[OBJ_JEWELLERY][i] = 10;
        mss[OBJ_POTIONS][i] = 40;
        mss[OBJ_UNKNOWN_II][i] = 5;     // "gems"
        mss[OBJ_BOOKS][i] = 70;

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
    int ft = 0;
    int max = 0;
    int yps = 0;
    int inv_count = 0;
    unsigned char ki = 0;

    const int num_lines = get_number_of_lines();

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
            if (id[i][j] == ID_KNOWN_TYPE)
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
        case IDTYPE_WANDS:
            ft = OBJ_WANDS;
            max = NUM_WANDS;
            break;
        case IDTYPE_SCROLLS:
            ft = OBJ_SCROLLS;
            max = NUM_SCROLLS;
            break;
        case IDTYPE_JEWELLERY:
            ft = OBJ_JEWELLERY;
            max = NUM_JEWELLERY;
            break;
        case IDTYPE_POTIONS:
            ft = OBJ_POTIONS;
            max = NUM_POTIONS;
            break;
        }

        for (j = 0; j < max; j++)
        {
            if (lines > num_lines - 2 && inv_count > 0)
            {
                gotoxy(1, num_lines);
                cprintf("-more-");

                ki = getch();

                if (ki == ESCAPE)
                {
#ifdef DOS_TERM
                    puttext(35, 1, 80, 25, buffer);
#endif
                    return ESCAPE;
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

            int ident_level = get_ident_type( ft, j );

            if (ident_level == ID_KNOWN_TYPE)
            {
                anything++;

                if (lines > 0)
                    cprintf(EOL);
                lines++;
                cprintf(" ");

                yps = wherey();

                // item_name now requires a "real" item, so we'll create a tmp
                item_def tmp = { ft, j, 0, 0, 0, 1, 0, 0, 0, 0 };
                item_name( tmp, DESC_PLAIN, st_pass );

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
bool hide2armour( unsigned char *which_subtype )
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

/*
 *  File:       effects.cc
 *  Summary:    Misc stuff.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "effects.h"

#include <string.h>

#include "externs.h"
#include "enum.h"
#include "beam.h"
#include "direct.h"
#include "files.h"
#include "fight.h"
#include "player.h"
#include "ouch.h"
#include "mstruct.h"
#include "misc.h"
#include "mutation.h"
#include "player.h"
#include "stuff.h"
#include "dungeon.h"
#include "itemname.h"
#include "skills2.h"
#include "view.h"
#include "spells0.h"
#include "spells2.h"
#include "spells3.h"


void torment()
{
    int dmi = 0;

    if (you.is_undead != 0 || you.mutation[MUT_TORMENT_RESISTANCE] != 0)
    {
        strcpy(info, "You feel a surge of unholy energy.");
        mpr(info);
    }
    else
    {
        strcpy(info, "Your body is wracked with pain!");
        mpr(info);
        //you.hp -= (you.hp / 2) - 1;
        int tormdam = (you.hp / 2) - 1;

        if (tormdam <= 0)
            tormdam = 0;
        ouch(tormdam, 0, 0);
        you.redraw_hit_points = 1;
    }

    for (dmi = 0; dmi < MNST; dmi++)
    {
        if (menv[dmi].type == -1)
            continue;

        if (!mons_near(dmi))
            continue;

        //              int mres = random2(100);
        //              int mres2 = mon_resist_mag(mons_class [dmi], mons_HD [dmi]);

        //              if (mres < mres2) continue;

        if (mons_holiness(menv[dmi].type) > 0)
            continue;

        // maybe an exemption for undead?
        // maybe do something fun for magic circles?

        menv[dmi].hit_points -= (menv[dmi].hit_points / 2) - 1;

        if (menv[dmi].enchantment[2] == 6)
            continue;           // can't see it

        strcpy(info, monam(menv[dmi].number, menv[dmi].type, menv[dmi].enchantment[2], 0));
        strcat(info, " convulses!");
        mpr(info);
    }

}


/*
   char go_berserk(void)
   {
   if (you.berserker != 0 || you.slow != 0) return 0;
   if (you.is_undead == 2) return 0;
   strcpy(info, "A red film seems to cover your vision as you go berserk!");
   mpr(info);
   strcpy(info, "You feel yourself moving faster!");
   mpr(info);
   strcpy(info, "You feel mighty!");
   mpr(info);
   you.berserker += 10 + random2(10) + random2(10);
   if (you.might == 0)
   {
   you.redraw_strength = 1;
   you.strength += 5;
   you.max_strength += 5;
   }
   you.might += you.berserker;
   you.haste += you.berserker;
   return 1;
   }
 */
void banished(unsigned char gate_type)
{
    you_teleport2(0);           // this is to ensure that you're standing on a suitable space (67)

    grd[you.x_pos][you.y_pos] = gate_type;
    down_stairs(1, you.your_level);     // heh heh

}


char forget_spell(void)
{
    char spc2;

    if (you.spell_no <= 0)
        return 0;

    do
    {
        spc2 = random2(20);
        if (random2(1000) == 0)
            return 0;           // safety check

    }
    while (you.spells[spc2] == 210);


    you.spell_levels += spell_value(you.spells[spc2]);
    you.spell_no--;
    you.spells[spc2] = 210;
    return 1;
}


char lose_stat(char force_stat, char stat_loss)
{

    if (force_stat == 100)
        force_stat = random2(3);

    if (player_sust_abil() != 0)
    {
        strcpy(info, "You feel slightly ill for a moment.");
        mpr(info);
        return 0;
    }
    switch (force_stat)
    {
    case STAT_STRENGTH:
        strcpy(info, "You feel weakened.");
        mpr(info);
        you.strength -= stat_loss;
        if (you.strength < 3)
            you.strength = 3;
        you.redraw_strength = 1;
        burden_change();
        break;

    case STAT_DEXTERITY:
        strcpy(info, "You feel clumsy.");
        mpr(info);
        you.dex -= stat_loss;
        if (you.dex < 3)
            you.dex = 3;
        you.redraw_dexterity = 1;
        break;

    case STAT_INTELLIGENCE:
        strcpy(info, "You feel dopey.");
        mpr(info);
        you.intel -= stat_loss;
        if (you.intel < 3)
            you.intel = 3;
        you.redraw_intelligence = 1;
        break;

    }

    return 1;

}





void direct_effect(struct bolt beam[1])
{
    int hurted = 0;

    switch (beam[0].type)
    {
    case DMNBM_HELLFIRE:
        strcpy(info, "You are engulfed in a burst of hellfire!");
        mpr(info);
        hurted = random2(10) + random2(10) + 5;
        beam[0].flavour = 20;   // lava, but it's hellfire anyway

        strcpy(beam[0].beam_name, "hellfire");  // for ouch

        hurted = check_your_resists(hurted, beam[0].flavour);
        scrolls_burn(4, 6);
        ouch(hurted, beam[0].beam_source, 3);
        you.redraw_hit_points = 1;
        break;

    case DMNBM_SMITING:
        strcpy(info, "Something smites you!");
        mpr(info);
        hurted = random2(6) + random2(6) + 7;
        strcpy(beam[0].beam_name, "smiting");   // for ouch

        ouch(hurted, beam[0].beam_source, 3);
        you.redraw_hit_points = 1;
        break;

    case DMNBM_BRAIN_FEED:
        if (random2(3) == 0 && you.intel > 3 && player_sust_abil() == 0)
        {
            strcpy(info, "Something feeds on your intelligence!");
            mpr(info);
            strcpy(info, "You feel stupid.");
            mpr(info);
            you.intel--;
            you.redraw_intelligence = 1;
        }
        else
        {
            strcpy(info, "Something tries to feed on your intelligence!");
            mpr(info);
        }
        break;

    case DMNBM_MUTATION:
        if (mutate(100) == 0)
            mpr("You feel very strange for a moment.");
        break;
    }

    return;
}


void mons_direct_effect(struct bolt beam[1], int i)
{

    int o = menv[i].monster_foe;
    int hurted = 0;

    switch (beam[0].type)
    {


    case DMNBM_HELLFIRE:
        if (mons_near(menv[i].monster_foe))
        {
            strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
            strcat(info, " is engulfed in hellfire.");
            mpr(info);
        }
        strcpy(beam[0].beam_name, "hellfire");
        beam[0].flavour = 20;
        hurted = 0;
        hurted += random2(10) + random2(5) + 5;
        check_mons_resists(beam, o, hurted);
        menv[o].hit_points -= hurted;
        if (menv[o].behavior == 0)
            menv[o].behavior = 1;
        if (menv[o].hit_points <= 0)
        {
            monster_die(o, 4, i);
        }
        break;

    case DMNBM_SMITING:
        if (mons_near(o))
        {
            strcpy(info, monam(menv[o].number, menv[o].type, menv[o].enchantment[2], 0));
            strcat(info, " is smitten.");
            mpr(info);
        }
        strcpy(beam[0].beam_name, "smiting");
        beam[0].flavour = 0;
        hurted = 0;
        hurted += random2(6) + random2(6) + 7;
        menv[o].hit_points -= hurted;
        if (menv[o].hit_points <= 0)
        {
            monster_die(o, 4, i);
        }
        break;


    case DMNBM_BRAIN_FEED:      // brain feed - not implemented here (nor, probably, can be)

        break;

    }                           // end switch

}                               // end monster_direct_effect()



void random_uselessness(unsigned char ru, unsigned char sc_read_2)
{

    char wc[30];

    switch (ru)
    {
    case 0:
        strcpy(info, "The dust glows a ");
        weird_colours(random2(256), wc);
        strcat(info, wc);
        strcat(info, " colour!");
        mpr(info);
        break;

    case 1:
        strcpy(info, "The scroll reassembles itself in your hand!");
        you.inv_quantity[sc_read_2]++;
        mpr(info);
        burden_change();
        break;

    case 2:
        if (you.equip[EQ_WEAPON] != -1)
        {
            in_name(you.equip[EQ_WEAPON], 4, str_pass);
            strcpy(info, str_pass);
            strcat(info, " glows ");
            weird_colours(random2(256), wc);
            strcat(info, wc);
            strcat(info, " for a moment.");
            mpr(info);
            return;
            //strcat(info
        }
        else
        {
            strcpy(info, "Nothing appears to happen.");
            mpr(info);
            return;
        }
        //      break;

    case 3:
        strcpy(info, "You hear the distant roaring of an enraged ");
        switch (random2(8))
        {
        case 0:
            strcat(info, "frog!");
            break;
        case 1:
            strcat(info, "pill bug!");
            break;
        case 2:
            strcat(info, "millipede!");
            break;
        case 3:
            strcat(info, "eggplant!");
            break;
        case 4:
            strcat(info, "albino dragon!");
            break;
        case 5:
            strcat(info, "dragon!");
            break;
        case 6:
            strcat(info, "human!");
            break;
        case 7:
            strcat(info, "slug!");
            break;
        }
        mpr(info);
        return;

    case 4:
        strcpy(info, "You smell ");
        switch (random2(8))
        {
        case 0:
            strcat(info, "coffee.");
            break;
        case 1:
            strcat(info, "salt.");
            break;
        case 2:
            strcat(info, "burning hair!");
            break;
        case 3:
            strcat(info, "baking bread.");
            break;
        case 4:
            strcat(info, "something weird.");
            break;
        case 5:
            strcat(info, "wet wool.");
            break;
        case 6:
            strcat(info, "sulphur.");
            break;
        case 7:
            strcat(info, "fire and brimstone!");
            break;
        }
        mpr(info);
        return;



    case 5:
        strcpy(info, "You experience a momentary feeling of inescapable doom!");
        mpr(info);
        return;

    case 6:
        strcpy(info, "Your ");
        switch (random2(3))
        {
        case 0:
            strcat(info, "ears itch.");
            break;
        case 1:
            strcat(info, "brain hurts!");
            break;
        case 2:
            strcat(info, "nose twitches suddenly!");
            break;
        }
        mpr(info);
        return;

    case 7:
        strcpy(info, "You hear the tinkle of a tiny bell.");
        mpr(info);
        summon_butter();
        return;

    case 8:
        strcpy(info, "You hear ");
        switch (random2(9))
        {
        case 0:
            strcat(info, "snatches of song.");
            break;
        case 1:
            strcat(info, "a voice call someone else's name.");
            break;
        case 2:
            strcat(info, "a very strange noise.");
            break;
        case 3:
            strcat(info, "roaring flame.");
            break;
        case 4:
            strcat(info, "a very strange noise indeed.");
            break;
        case 5:
            strcat(info, "the chiming of a distant gong.");
            break;
        case 6:
            strcat(info, "the bellowing of a yak.");
            break;
        case 7:
            strcat(info, "a crunching sound.");
            break;
        case 8:
            strcat(info, "the tinkle of an enormous bell.");
            break;
        }
        mpr(info);
        return;


    }

    //abort();

}


void acquirement(unsigned char force_class)
{
    int thing_created = 0;

    // Remember lava!
    unsigned char class_wanted = 250;
    unsigned char type_wanted = 250;

    unsigned char force_plus = 0;

    unsigned char acqc = 0;
    char already_has[50];

    char glof = 99;
    unsigned char keyin;
    int func_pass[10];

    for (acqc = 0; acqc < 50; acqc++)
    {
        already_has[acqc] = 0;
    }

    if (force_class == 250)
    {

        mpr("This is a scroll of acquirement!");
query:
        mpr("a - Weapon, b - Armour, c - Jewellery,");
        mpr("d - Book, e - Staff, f - Miscellaneous");
        mpr("What kind of item would you like to acquire? ");
        keyin = get_ch();

        switch (keyin)
        {
        case 'a':
        case 'A':
            class_wanted = OBJ_WEAPONS;
            break;

        case 'b':
        case 'B':
            class_wanted = OBJ_ARMOUR;
            break;

        case 'c':
        case 'C':
            class_wanted = OBJ_JEWELLERY;
            break;

        case 'd':
        case 'D':
            class_wanted = OBJ_BOOKS;
            break;

        case 'e':
        case 'E':
            class_wanted = OBJ_STAVES;
            break;

        case 'f':
        case 'F':
            class_wanted = OBJ_MISCELLANY;
            break;
        }
    }
    else
        class_wanted = force_class;

    if (class_wanted > OBJ_ARMOUR)
    {
        for (acqc = 0; acqc < 52; acqc++)
        {
            if (you.inv_quantity[acqc] > 0 && you.inv_class[acqc] == class_wanted)
            {
                already_has[you.inv_type[acqc]] = 1;

                /*  if (class_wanted == 10)
                   {
                   if (you.inv_type [acqc] == 0)
                   {
                   already_has [1] = 1;
                   already_has [2] = 1;
                   }
                   if (you.inv_type [acqc] == 1)
                   {
                   already_has [0] = 1;
                   already_has [2] = 1;
                   }
                   if (you.inv_type [acqc] == 3) already_has [4] = 1;
                   if (you.inv_type [acqc] == 4) already_has [3] = 1;
                   } */
            }
        }

        /*if (class_wanted == 10) // these are books which that class wouldn't want (or shouldn't get, anyway)
           {
           / * if (you.char_class == JOB_NECROMANCER)
           {
           already_has [20] = 1; // holy books
           already_has [17] = 1;
           already_has [18] = 1;
           }
           if (you.char_class == JOB_FIRE_ELEMENTALIST)
           {
           already_has [6] = 1;
           already_has [9] = 1;
           }
           if (you.char_class == JOB_ICE_ELEMENTALIST)
           {
           already_has [5] = 1;
           already_has [8] = 1;
           }* /
           if (you.char_class == JOB_PRIEST || you.char_class == JOB_PALADIN)
           {
           already_has [15] = 1; // necromancy
           already_has [23] = 1;
           already_has [24] = 1;
           already_has [34] = 1;
           }
           } */

        do
        {
            switch (class_wanted)
            {

            case OBJ_JEWELLERY:
                type_wanted = 250;
                break;

            case OBJ_BOOKS:

                // remember, put rarer books higher in the list

                type_wanted = 99;

                glof = best_skill(26, 50, glof);

              which_book:switch (glof)
                {
                default:
                case SK_SPELLCASTING:   // spellcasting

                    if (you.had_item[43] == 0)
                        type_wanted = 43;
                    if (you.had_item[42] == 0)
                        type_wanted = 42;
                    break;

                case SK_POISON_MAGIC:   // Poison Magic

                    if (you.had_item[32] == 0)
                        type_wanted = 32;
                    if (you.had_item[13] == 0)
                        type_wanted = 13;
                    break;

                case SK_EARTH_MAGIC:    // Earth

                    if (you.had_item[40] == 0)
                        type_wanted = 40;
                    if (you.had_item[39] == 0)
                        type_wanted = 39;
                    break;

                case SK_AIR_MAGIC:      // Air

                    if (you.had_item[29] == 0)
                        type_wanted = 29;
                    if (you.had_item[28] == 0)
                        type_wanted = 28;
                    break;

                case SK_ICE_MAGIC:      // Ice

                    if (you.had_item[9] == 0)
                        type_wanted = 9;
                    if (you.had_item[6] == 0)
                        type_wanted = 6;
                    break;

                case SK_FIRE_MAGIC:     // fire

                    if (you.had_item[8] == 0)
                        type_wanted = 8;
                    if (you.had_item[5] == 0)
                        type_wanted = 5;
                    break;

                case SK_SUMMONINGS:     // summ

                    if (you.had_item[27] == 0)
                        type_wanted = 27;
                    if (you.had_item[7] == 0)
                        type_wanted = 7;
                    if (you.had_item[25] == 0)
                        type_wanted = 25;
                    break;

                case SK_ENCHANTMENTS:   // ench

                    if (you.had_item[12] == 0)
                        type_wanted = 12;
                    if (you.had_item[20] == 0)
                        type_wanted = 20;
                    if (you.had_item[36] == 0)
                        type_wanted = 36;
                    if (you.had_item[16] == 0)
                        type_wanted = 16;
                    if (you.had_item[26] == 0)
                        type_wanted = 26;
                    break;

                case SK_CONJURATIONS:   // conj

                    if (you.had_item[33] == 0)
                        type_wanted = 33;
                    if (you.had_item[3] == 0)
                        type_wanted = 3;
                    break;

                case SK_NECROMANCY:     // necro

                    if (you.had_item[24] == 0)
                        type_wanted = 24;
                    if (you.had_item[34] == 0)
                        type_wanted = 34;
                    if (you.had_item[15] == 0)
                        type_wanted = 15;
                    if (you.had_item[23] == 0)
                        type_wanted = 23;
                    break;

                case SK_TRANSLOCATIONS: // translocations

                    if (you.had_item[11] == 0)
                        type_wanted = 11;
                    if (you.had_item[31] == 0)
                        type_wanted = 31;
                    break;

                case SK_TRANSMIGRATION: // transmutation

                    if (you.had_item[17] == 0)
                        type_wanted = 17;
                    if (you.had_item[18] == 0)
                        type_wanted = 18;
                    break;


                }

                if (type_wanted == 99 && glof == best_skill(26, 50, 99))        // && best_skill(25, 50, 99) != best_skill(26, 50, 99))

                {
                    glof = best_skill(26, 50, best_skill(26, 50, 99));
                    goto which_book;
                }


                if (type_wanted == 99)
                    do
                    {
                        type_wanted = random2(27);
                        if (random2(500) == 0)
                            break;
                    }
                    while (you.had_item[type_wanted] == 1);


                force_plus = 127;

                // should guarantee a full spellbook, so:
                spellbook_template(type_wanted, func_pass);
                if (func_pass[6] == 210)
                    force_plus = 126;
                if (func_pass[5] == 210)
                    force_plus = 124;
                if (func_pass[4] == 210)
                    force_plus = 120;
                if (func_pass[3] == 210)
                    force_plus = 112;
                if (func_pass[2] == 210)
                    force_plus = 96;

                break;

            case OBJ_STAVES:
                type_wanted = random2(18);
                if (class_wanted == OBJ_STAVES && type_wanted > 9 && random2(5) != 0)
                    type_wanted = random2(10);
                break;

            case OBJ_MISCELLANY:
                type_wanted = random2(6);
                break;

            default:
                mesclr();
                goto query;
            }
        }
        while (already_has[type_wanted] == 1 && random2(200) != 0);

    }
    else
    {
        if (class_wanted == OBJ_WEAPONS)
            type_wanted = 250;  // 1 + random2(18); // weapons - no clubs

        else
            type_wanted = 250;  // always get random armour

    }

    if (grd[you.x_pos][you.y_pos] == 61 || grd[you.x_pos][you.y_pos] == 62)
    {
        strcpy(info, "You hear a splash.");     // how sad (and stupid)

        mpr(info);
        return;
    }

    thing_created =             /*items(you.unique_items, 1, you.item_description,
                                   grd, class_wanted, type_wanted, 1, 351, 250); */
        items(1, class_wanted, type_wanted, 1, 351, 250);


    if (you.species != SP_NAGA)
        strcpy(info, "Something appears at your feet!");
    else
        strcpy(info, "Something appears before you!");
    mpr(info);

    if (force_plus != 0)
        mitm.pluses[thing_created] = force_plus;

    int what_was_there = igrd[you.x_pos][you.y_pos];

    mitm.link[thing_created] = what_was_there;
    igrd[you.x_pos][you.y_pos] = thing_created;

    return;

}



char recharge_wand(void)
{

    // note that the scroll of recharging also recharges weapons of electrocution; see the scroll function

    if (you.equip[EQ_WEAPON] == -1)
    {
        return 0;
    }

    if (you.inv_class[you.equip[EQ_WEAPON]] != OBJ_WANDS)
    {
        return 0;               // not a wand

    }

    char charge_gain = 8;

    if (you.inv_type[you.equip[EQ_WEAPON]] == WAND_FIRE || you.inv_type[you.equip[EQ_WEAPON]] == WAND_COLD)
        charge_gain = 5;

    if (you.inv_type
        [you.equip
         [EQ_WEAPON]] == WAND_FIREBALL || you.inv_type
        [you.equip
         [EQ_WEAPON]] == WAND_LIGHTNING ||
        you.
        inv_type[you.equip[EQ_WEAPON]] ==
        WAND_DRAINING)
        charge_gain = 4;

    item_name(you.inv_plus2[you.equip[EQ_WEAPON]], you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON
                                                                         ]],
              you.inv_dam[you.equip[EQ_WEAPON]], you.inv_plus[you.equip[EQ_WEAPON]], you.inv_quantity[you.equip[EQ_WEAPON]]
              ,you.inv_ident[you.equip[EQ_WEAPON]], 4, str_pass);
    strcpy(info, str_pass);
    strcat(info, " glows for a moment.");
    mpr(info);

    you.inv_plus[you.equip[EQ_WEAPON]] += random2(charge_gain) + random2(charge_gain) + random2(charge_gain) + 1;

    if (you.inv_plus[you.equip[EQ_WEAPON]] > charge_gain * 3)
        you.inv_plus[you.equip[EQ_WEAPON]] = charge_gain * 3;

    return 1;

}


void yell(void)
{
    char targ_prev = 0;
    int mons_targd = 0;
    struct dist beam[1];

    mpr("What do you say?");
    mpr(" ! - Yell");
    mpr(" a - Order allies to attack a monster");
    if (you.prev_targ != MHITNOT && you.prev_targ < MNST)
        if (mons_near(you.prev_targ) && (menv[you.prev_targ].enchantment[2] != 6 || player_see_invis() != 0))
        {
            mpr(" p - Order allies to attack your previous target");
            targ_prev = 1;
        }
    strcpy(info, " Anything else - Stay silent ");
    if (random2(10) == 0)
        strcat(info, "(and be thought of as a fool)");
    mpr(info);

    char keyn = get_ch();

    switch (keyn)
    {

    case '!':
        strcpy(info, "You yell for attention!");
        mpr(info);
        you.turn_is_over = 1;
        noisy(12, you.x_pos, you.y_pos);
        return;

    case 'a':
        mpr("Gang up on whom?");
        direction(100, beam);

        if (beam[0].nothing == -1 || mgrd[beam[0].target_x][beam[0].target_y] == MNG)
        {
            mpr("Yeah, whatever.");
            return;
        }
        mons_targd = mgrd[beam[0].target_x][beam[0].target_y];
        break;


    case 'p':
        if (targ_prev == 1)
        {
            mons_targd = you.prev_targ;
            break;
        }
        /* fall through... */
    default:
        mpr("Okely-dokely.");
        return;
    }

    int i = 0;

    for (i = 0; i < MNST; i++)
    {
        if (menv[i].type == -1)
            continue;
        if (menv[i].behavior != BEH_ENSLAVED)
            continue;
        if (!mons_near(i))
            continue;
        menv[i].monster_foe = mons_targd;
        menv[i].target_x = menv[mons_targd].x;
        menv[i].target_y = menv[mons_targd].y;
    }

    noisy(10, you.x_pos, you.y_pos);
    mpr("Attack!");

}

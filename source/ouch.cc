/*
 *  File:       ouch.cc
 *  Summary:    Functions used when Bad Things happen to the player.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <8>      7/30/00        JDJ     Fixed end_game so that it works with filenames longer than 6 characters.
 *      <7>      19 June 2000   GDL     Changed handle to FILE *
 *      <6>      11/23/99       LRH     Fixed file purging for DOS?
 *      <5>      9/29/99        BCR     Fixed highscore so that it
 *                                      doesn't take so long.  Also
 *                                      added some whitespace to the scores.
 *                                      Fixed problem with uniques and 'a'.
 *      <4>      6/13/99        BWR     applied a mix of DML and my tmp
 *                                      file purging improvements.
 *      <3>      5/26/99        JDJ     highscore() will print more scores on
 *                                      larger windows.
 *      <2>      5/21/99        BWR     Added SCORE_FILE_ENTRIES, so
 *                                      that more top scores can be
 *                                      saved.
 *      <1>      -/--/--        LRH     Created
 */

#include "AppHdr.h"
#include "ouch.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
//#include <fstream.h>

#ifdef DOS
#include <conio.h>
#include <files.h>
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
#include <time.h>
#endif

#ifdef MAC
#include <stat.h>
#else
#include <sys/stat.h>
#endif

#include "externs.h"

#include "chardump.h"
#include "files.h"
#include "hiscores.h"
#include "invent.h"
#include "itemname.h"
#include "mon-util.h"
#include "player.h"
#include "religion.h"
#include "shopping.h"
#include "skills2.h"
#include "stuff.h"

#ifdef MACROS
#include "macro.h"
#endif

//jmf: brent sez:
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason ...
#ifdef USE_CURSES
#include <curses.h>
#endif

extern bool wield_change;       // defined in output.cc

void end_game(struct scorefile_entry &se);
void item_corrode(char itco);
static char *pad(char *str);


/* NOTE: DOES NOT check for hellfire!!! -- this is good */
int check_your_resists(int hurted, int flavour)
{
    switch (flavour)
    {
    case BEAM_FIRE:
        if (player_res_fire() > 100)
        {
            canned_msg(MSG_YOU_RESIST);
            const int res_fire = player_res_fire() - 100;
            hurted /= (1 + (res_fire * res_fire));
        }
        else if (player_res_fire() < 100)
        {
            mpr("It burns terribly!");
            hurted *= 15;
            hurted /= 10;
        }
        break;

    case BEAM_COLD:
        if (player_res_cold() > 100)
        {
            canned_msg(MSG_YOU_RESIST);
            const int res_cold = player_res_cold() - 100;
            hurted /= (1 + (res_cold * res_cold));
        }
        else if (player_res_cold() < 100)
        {
            mpr("You feel a terrible chill!");
            hurted *= 15;
            hurted /= 10;
        }
        break;

    case BEAM_ELECTRICITY:
        if (player_res_electricity() > 0)
        {
            canned_msg(MSG_YOU_RESIST);
            hurted /= 3;
        }
        break;


    case BEAM_POISON:
        if (player_res_poison())
        {
            canned_msg(MSG_YOU_RESIST);
            hurted /= 3;
        }
        else
            you.poison += (coinflip() ? 2 : 1);
        break;

    case BEAM_NEG:
        if (player_prot_life() > 0)
        {
            hurted -= (player_prot_life() * hurted) / 3;
        }
        drain_exp();
        break;

    case BEAM_ICE:
        if (player_res_cold() > 100)
        {
            mpr("You partially resist.");
            hurted /= 2;
        }
        else if (player_res_cold() < 100)
        {
            mpr("You feel a painful chill!");
            hurted *= 13;
            hurted /= 10;
        }
        break;

    case BEAM_LAVA:
        if (player_res_fire() > 101)
        {
            mpr("You partially resist.");
            hurted /= (1 + (player_res_fire() - 100));
        }
        else if (player_res_fire() < 100)
        {
            mpr("It burns terribly!");
            hurted *= 15;
            hurted /= 10;
        }
        break;
    }                           /* end switch */

    return hurted;
}                               // end check_your_resists()

void splash_with_acid(char acid_strength)
{
    /* affects equip only?
       assume that a message has already been sent, also that damage has
       already been done
     */
    char splc = 0;

    for (splc = EQ_CLOAK; splc <= EQ_BODY_ARMOUR; splc++)
    {
        if (you.equip[splc] == -1)
        {
            ouch(random2(acid_strength) / 5, 0, KILLED_BY_BEAM);
            continue;
            /* should take extra damage if little armour worn */
        }

        if (random2(20) <= acid_strength)
            item_corrode(you.equip[splc]);
    }
}                               // end splash_with_acid()

void weapon_acid(char acid_strength)
{
    char hand_thing = you.equip[EQ_WEAPON];

    if (you.equip[EQ_WEAPON] == -1)
    {
        if (you.equip[EQ_GLOVES] != -1)
            hand_thing = you.equip[EQ_GLOVES];
        else
            return;             // take extra damage
    }

    if (random2(20) <= acid_strength)
        item_corrode(hand_thing);
}                               // end weapon_acid()

void item_corrode(char itco)
{
    int chance_corr = 0;        // no idea what its full range is {dlb}
    bool it_resists = false;    // code simplifier {dlb}
    bool suppress_msg = false;  // code simplifier {dlb}
    unsigned char how_rusty =
                ((you.inv_class[itco] == OBJ_WEAPONS) ? you.inv_plus2[itco]
                                                      : you.inv_plus[itco]);

    // early return for "oRC and cloak/preservation {dlb}:
    if (wearing_amulet(AMU_RESIST_CORROSION) && !one_chance_in(10))
    {
#ifdef WIZARD
        mpr("Amulet protects.");
#endif
        return;
    }

    // early return for items already pretty d*** rusty {dlb}:
    if (how_rusty < 45)
        return;

    // determine possibility of resistance by object type {dlb}:
    switch (you.inv_class[itco])
    {
    case OBJ_ARMOUR:
        if (you.inv_dam[itco] % 30 >= SPARM_RANDART_I)
        {
            it_resists = true;
            suppress_msg = true;
        }
        else if ((you.inv_type[itco] == ARM_CRYSTAL_PLATE_MAIL
                    || you.inv_dam[itco] / 30 == DARM_DWARVEN)
                && !one_chance_in(5))
        {
            it_resists = true;
            suppress_msg = false;
        }
        break;

    case OBJ_WEAPONS:
        if (you.inv_dam[itco] >= NWPN_SINGING_SWORD
                    || you.inv_dam[itco] % 30 >= SPWPN_RANDART_I)
        {
            it_resists = true;
            suppress_msg = true;
        }
        else if (you.inv_dam[itco] / 30 == DWPN_DWARVEN && !one_chance_in(5))
        {
            it_resists = true;
            suppress_msg = false;
        }
        break;

    case OBJ_MISSILES:
        if (you.inv_dam[itco] / 30 == DAMMO_DWARVEN && !one_chance_in(5))
        {
            it_resists = true;
            suppress_msg = false;
        }
        break;
    }

    // determine chance of corrosion {dlb}:
    if (!it_resists)
    {
        chance_corr = how_rusty;

        // the following may seem odd -------------------------------
        if (chance_corr > 130)
            chance_corr -= 100;

        // lowest possible value is 45(?) - see above {dlb}
        if (chance_corr < 50)
            chance_corr = 100 - chance_corr;

        chance_corr -= 50;
        // ---------------------------- but it needs to stay this way
        //                              (as it *was* this way)

        // the embedded equation may look funny, but it actually works well
        // to generate a pretty probability ramp {10%, 19%, 35%, 67%, 99%}
        // for values [0,4] which closely matches the original, ugly switch
        // {dlb}
        if (chance_corr >= 0 && chance_corr <= 4)
        {
            it_resists = (random2(100) <
                            2 + (2 << (1 + chance_corr)) + (chance_corr * 8));
        }
        else
            it_resists = true;  // no idea how often this occurs {dlb}

        // if the checks get this far, you should hear about it {dlb}
        suppress_msg = false;
    }

    // handle message output and item damage {dlb}:
    if (!suppress_msg)
    {
        in_name(itco, 4, str_pass);
        strcpy(info, str_pass);
        strcat(info, (it_resists) ? " resists." : " is eaten away!");
        mpr(info);
    }

    if (!it_resists)
    {
        how_rusty--;

        if (you.inv_class[itco] == OBJ_WEAPONS)
            you.inv_plus2[itco] = how_rusty;
        else
            you.inv_plus[itco] = how_rusty;

        you.redraw_armor_class = 1;     // for armour, rings, etc. {dlb}

        if (you.equip[EQ_WEAPON] == itco)
            wield_change = true;
    }

    return;
}                               // end item_corrode()

void scrolls_burn(char burn_strength, char target_class)
{

    unsigned char burnc;
    unsigned char burn2;
    unsigned char burn_no = 0;

    if (wearing_amulet(AMU_CONSERVATION) && !one_chance_in(10))
    {
#ifdef WIZARD
        mpr("Amulet conserves.");
#endif
        return;
    }

    for (burnc = 0; burnc < ENDOFPACK; burnc++)
    {
        if (!you.inv_quantity[burnc])
            continue;
        if (you.inv_class[burnc] != target_class)
            continue;

        for (burn2 = 0; burn2 < you.inv_quantity[burnc]; burn2++)
        {
            if (random2(70) < burn_strength)
            {
                you.inv_quantity[burnc]--;
                burn_no++;

                if (you.inv_quantity[burnc] < 1)
                {
                    // I can't assume any level of intelligence on
                    // the player's behalf
                    if (burnc == you.equip[EQ_WEAPON])
                    {
                        you.equip[EQ_WEAPON] = -1;
                        mpr("You are now empty-handed.");
                    }
                    break;
                }
            }
        }
    }

    if (burn_no == 1)
    {
        if (target_class == OBJ_SCROLLS)
            mpr("A scroll you are carrying catches fire!");
        else if (target_class == OBJ_POTIONS)
            mpr("A potion you are carrying freezes and shatters!");
        else if (target_class == OBJ_FOOD)
            mpr("Some of your food is covered with spores!");
    }
    else if (burn_no > 1)
    {
        if (target_class == OBJ_SCROLLS)
            mpr("Some of the scrolls you are carrying catch fire!");
        else if (target_class == OBJ_POTIONS)
            mpr("Some of the potions you are carrying freeze and shatter!");
        else if (target_class == OBJ_FOOD)
            mpr("Some of your food is covered with spores!");
    }
    /* burn_no could be 0 */
}

                            // end scrolls_burn()
void lose_level(void)
{
    char temp_quant[5];

    // because you.experience is unsigned long, if it's going to be -ve
    // must die straightaway.
    if (you.experience_level == 1)
        ouch(-9999, 0, KILLED_BY_DRAINING);

    you.experience = exp_needed(you.experience_level + 1) - 1;
    you.experience_level--;

    strcpy(info, "You are now a level ");
    itoa(you.experience_level, temp_quant, 10);
    strcat(info, temp_quant);
    strcat(info, " ");
    strcat(info, you.class_name);
    strcat(info, "!");
    mpr(info, MSGCH_WARN);

    int brek = 0;

    if (you.experience_level > 20)
        brek = (coinflip()? 3 : 2);
    else if (you.experience_level > 11)
        brek = random2(3) + 2;
    else
        brek = random2(3) + 4;

    ouch(brek, 0, KILLED_BY_DRAINING);
    you.base_hp2 -= brek;

    dec_mp(1);
    you.base_magic_points2--;

    calc_hp();
    calc_mp();

    you.redraw_experience = 1;
}                               // end lose_level()

void drain_exp(void)
{
    int protection = player_prot_life();

    if (you.duration[DUR_PRAYER]
        && (you.religion == GOD_ZIN || you.religion == GOD_SHINING_ONE)
        && random2(150) < you.piety)
    {
        simple_god_message(" protects your life force!");
        return;
    }

    if (protection >= 3 || you.is_undead)
    {
        mpr("You fully resist.");
        return;
    }

    if (you.experience == 0)
        ouch(-9999, 0, KILLED_BY_DRAINING);

    if (you.experience_level == 1)
    {
        you.experience = 0;
        return;
    }

    unsigned long total_exp = exp_needed( you.experience_level + 2 )
                                    - exp_needed( you.experience_level + 1 );
    unsigned long exp_drained = total_exp * (10 + random2(11));

    exp_drained /= 100;

    if (protection > 0)
    {
        mpr("You partially resist.");
        exp_drained -= (protection * exp_drained) / 3;
    }

    if (exp_drained > 0)
    {
        mpr("You feel drained.");
        you.experience -= exp_drained;
        you.exp_available -= exp_drained;

        if (you.exp_available < 0)
            you.exp_available = 0;

#ifdef WIZARD
        strcpy(info, "You lose ");

        char temp_quant[20];

        itoa(exp_drained, temp_quant, 10);

        strcat(info, temp_quant);
        strcat(info, " experience points.");
        mpr(info);
#endif
        you.redraw_experience = 1;

        if (you.experience < exp_needed(you.experience_level + 1))
            lose_level();
    }
}                               // end drain_exp()

// death_source should be set to zero for non-monsters {dlb}
void ouch(int dam, int death_source, char death_type)
{
    int d = 0;
    int e = 0;

    if (you.deaths_door && death_type != KILLED_BY_LAVA
                                    && death_type != KILLED_BY_WATER)
    {
        return;
    }

    if (dam > 300)
        return;                 // assumed bug for high damage amounts

    if (dam > -9000)            // that is, a "death" caused by hp loss {dlb}
    {
        switch (you.religion)
        {
        case GOD_XOM:
            if (random2(you.hp_max) > you.hp && dam > random2(you.hp)
                                                    && one_chance_in(5))
            {
                simple_god_message( " protects you from harm!" );
                return;
            }
            break;

        case GOD_ZIN:
        case GOD_SHINING_ONE:
        case GOD_ELYVILON:
        case GOD_OKAWARU:
        case GOD_KIKUBAAQUDGHA:
            if (dam >= you.hp && you.duration[DUR_PRAYER]
                                                && random2(you.piety) >= 30)
            {
                simple_god_message( " protects you from harm!" );
                return;
            }
            break;
        }
    }

    // XXX: Assuming that this is imporatnt (negative damage won't be done
    // by dec_hp, so I take it that the (you.hp > 0) check might be
    // important for some reason. -- bwr
    if (dam > -9000)
    {
        dec_hp(dam, true);

        // Even if we have low HP messages off, we'll still give a
        // big hit warning (in this case, a hit for half our HPs) -- bwr
        if (dam > 0 && you.hp_max <= dam * 2)
            mpr( "Ouch!  That really hurt!", MSGCH_DANGER );

        if (you.hp > 0 && Options.hp_warning
            && you.hp <= (you.hp_max * Options.hp_warning) / 100)
        {
            mpr( "* * * LOW HITPOINT WARNING * * *", MSGCH_DANGER );
        }

        if (you.hp > 0)
            return;
    }

#ifdef WIZARD
    if (death_type != KILLED_BY_QUITTING && death_type != KILLED_BY_WINNING
                                            && death_type != KILLED_BY_LEAVING)
    {
#ifdef USE_OPTIONAL_WIZARD_DEATH
        if (!yesno("Die?", false))
        {
            set_hp(you.hp_max, false);
            return;
        }
#else
        mpr("Since you're a debugger, I'll let you live.");
        mpr("Be more careful next time, okay?");

        set_hp(you.hp_max, false);
        return;
#endif
    }
#endif

    //okay, so you're dead:

    // do points first.
    long points = you.gold;
    points += (you.experience * 7) / 10;

    //if (death_type == KILLED_BY_WINNING) points += points / 2;
    //if (death_type == KILLED_BY_LEAVING) points += points / 10;
    // these now handled by giving player the value of their inventory
    char temp_id[4][50];

    for (d = 0; d < 4; d++)
        for (e = 0; e < 50; e++)
            temp_id[d][e] = 1;

    if (death_type == KILLED_BY_LEAVING || death_type == KILLED_BY_WINNING)
    {
        for (d = 0; d < ENDOFPACK; d++)
        {
            points +=
                item_value( you.inv_class[d], you.inv_type[d], you.inv_dam[d],
                                you.inv_plus[d], you.inv_plus2[d],
                                you.inv_quantity[d], 3, temp_id );
        }
    }

    if (points > 99999999)
        points = 99999999;

    // for death by monster
    struct monsters *monster = NULL;

    // oh, oh, oh, this is really Bad.  XXX
    if (death_source >= 0)
    {
        monster =  &menv[death_source];
        if (monster->type < 0 || monster->type >= NUM_MONSTERS)
            monster = NULL;
    }

    // CONSTRUCT SCOREFILE ENTRY
    struct scorefile_entry se;
    se.version = 4;
    se.release = 0;
    strncpy(se.name, you.your_name, kNameLen);
    se.name[kNameLen - 1] = '\0';
#ifdef MULTIUSER
    se.uid = getuid();
#else
    se.uid = 0;
#endif

    se.points = points;
    se.race = you.species;
    se.cls = you.char_class;
    strcpy(se.race_class_name, "");
    se.lvl = you.experience_level;
    se.best_skill = best_skill(SK_FIGHTING, NUM_SKILLS-1, 99);
    se.best_skill_lvl = you.skills[ se.best_skill ];
    se.death_type = death_type;
    if (monster != NULL)
    {
        se.death_source = monster->type;
        se.mon_num = monster->number;
        if (monster->type < MONS_PROGRAM_BUG
            || (monster->type < MONS_TERENCE && monster->type >= MONS_NAGA_MAGE)
            || monster->type > MONS_BORIS && monster->type != MONS_PLAYER_GHOST)
        {
            strcpy(info, "a");
        }
        else
            info[0] = '\0';

        strcat(info, monam(monster->number, monster->type, 0, 99));
        strncpy(se.death_source_name, info, 40);
        se.death_source_name[39] = '\0';
    }
    else
    {
        se.death_source = death_source;
        se.mon_num = 0;
        se.death_source_name[0] = '\0';
    }

    se.final_hp = you.hp;
    // main dungeon: level is simply level
    se.dlvl = you.your_level;
    switch (you.where_are_you)
    {
        case BRANCH_ORCISH_MINES:
        case BRANCH_HIVE:
        case BRANCH_LAIR:
        case BRANCH_SLIME_PITS:
        case BRANCH_VAULTS:
        case BRANCH_CRYPT:
        case BRANCH_HALL_OF_BLADES:
        case BRANCH_HALL_OF_ZOT:
        case BRANCH_ECUMENICAL_TEMPLE:
        case BRANCH_SNAKE_PIT:
        case BRANCH_ELVEN_HALLS:
        case BRANCH_TOMB:
        case BRANCH_SWAMP:
            se.dlvl = you.your_level - you.branch_stairs[you.where_are_you - 10];
            break;

        case BRANCH_DIS:
        case BRANCH_GEHENNA:
        case BRANCH_VESTIBULE_OF_HELL:
        case BRANCH_COCYTUS:
        case BRANCH_TARTARUS:
        case BRANCH_INFERNO:
        case BRANCH_THE_PIT:
            se.dlvl = you.your_level + 1 - 26;
            break;
    }
    se.branch = you.where_are_you;      // no adjustments necessary.
    se.level_type = you.level_type;     // pandemonium, labyrinth, dungeon..

#ifdef WIZARD
    se.wiz_mode = 1;
#else
    se.wiz_mode = 0;
#endif

    // add this highscore to the highscore file.
    hiscores_new_entry(se);

    if (death_type != KILLED_BY_LEAVING && death_type != KILLED_BY_WINNING)
        save_ghost();

    end_game(se);
}


void end_game(struct scorefile_entry &se)
{
    int i;
    char del_file[300];         // massive overkill!
    bool dead = true;
    if (se.death_type == KILLED_BY_LEAVING ||
        se.death_type == KILLED_BY_WINNING)
        dead = false;

    // clean all levels that we think we have ever visited
    for (int level = 0; level < MAX_LEVELS; level++)
    {
        for (int dungeon = 0; dungeon < MAX_BRANCHES; dungeon++)
        {
            if (tmp_file_pairs[level][dungeon])
            {
                make_filename(info, you.your_name, level, dungeon,
                    false, false);
                unlink(info);
            }
        }
    }

    // temp level, if any
    make_filename(info, you.your_name, 0, 0, true, false);
    unlink(info);

    // create base file name
#ifdef SAVE_DIR_PATH
    sprintf(info, SAVE_DIR_PATH "%s%d", you.your_name, getuid());
#else
    strncpy(info, you.your_name, kFileNameLen);
    info[kFileNameLen] = '\0';
#endif

    // this is to catch the game package if it still exists.
#ifdef PACKAGE_SUFFIX
    strcpy(del_file, info);
    strcat(del_file, "." PACKAGE_SUFFIX);
    unlink(del_file);
#endif

    // last, but not least, delete player .sav file
    strcpy(del_file, info);
    strcat(del_file, ".sav");
    unlink(del_file);

    // death message
    if (dead)
        mpr("You die...");      // insert player name here? {dlb}

    viewwindow(1, false);
    more();

    for (i = 0; i < ENDOFPACK; i++)
        you.inv_ident[i] = 3;

    for (i = 0; i < ENDOFPACK; i++)
    {
        if (you.inv_class[i] != 0)
            set_id(you.inv_class[i], you.inv_type[i], 1);
    }

    invent(-1, !dead);

#ifdef USE_CURSES
    clear();
#endif

    if (!dump_char(!dead, "morgue.txt"))
        mpr("Char dump unsuccessful! Sorry about that.");
#ifdef DEBUG
    //jmf: switched logic and moved "success" message to debug-only
    else
        mpr("Char dump successful! (morgue.txt).");
#endif // DEBUG

    more();

    for (int p = 0; p < ENDOFPACK; p++)
    {
        for (i = 0; i < MAX_ITEMS; i++)
        {
            if (!mitm.quantity[i])
            {
                mitm.id[i] = 0;
                mitm.base_type[i] = you.inv_class[p];
                mitm.sub_type[i] = you.inv_type[p];
                mitm.pluses[i] = you.inv_plus[p];
                mitm.pluses2[i] = you.inv_plus2[p];
                mitm.special[i] = you.inv_dam[p];
                mitm.colour[i] = you.inv_colour[p];
                mitm.x[i] = you.x_pos;
                mitm.y[i] = you.y_pos;
                mitm.quantity[i] = you.inv_quantity[p];
                break;
            }
        }                       // end "for p,i"
    }

    for (i = 0; i < MAX_ITEMS; i++)
    {
        mitm.id[i] = 0;
    }

    clrscr();
#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif
    clrscr();
    cprintf("Goodbye, ");
    cprintf(you.your_name);
    cprintf(".");
    cprintf(EOL EOL);
    hiscores_print_single(se);
    cprintf(EOL EOL " Best Crawlers - " EOL);

    hiscores_print_list();

    // just to pause, actual value returned does not matter {dlb}
    get_ch();
    end(0);
}

/*
 *  File:       mstuff2.cc
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "mstuff2.h"

#include <string>
#include <string.h>

#include "externs.h"
#include "enum.h"

#include "debug.h"
#include "beam.h"
#include "bang.h"
#include "effects.h"
#include "fight.h"
#include "itemname.h"
#include "monplace.h"
#include "mstruct.h"
#include "player.h"
#include "spells.h"
#include "spells2.h"
#include "stuff.h"
#include "view.h"

void itrap(struct bolt beam[1], int trapped);
unsigned char monster_abjuration(int pow, char test);

/*
   NOTE: must fix species abils to not use duration 15


 */


void mons_trap(int i)
{
    struct bolt beem[1];
    int damage_taken=-1;
    int func_pass[10];

    int tr;

    for (tr = 0; tr < NTRAPS; tr++)
    {
        if (env.trap_x[tr] == menv[i].x && env.trap_y[tr] == menv[i].y)
            break;
    }

    /* Flying monsters can avoid mechanical traps, but not magical ones */
    if (env.trap_type[tr] < TRAP_TELEPORT || env.trap_type[tr] == TRAP_BLADE
                                            || env.trap_type[tr] == TRAP_BOLT)
    {
        if (mons_flies(menv[i].type))
        {
            if (mons_near(i) && grd[env.trap_x[tr]][env.trap_y[tr]]
                                                    != DNGN_UNDISCOVERED_TRAP)
            {
                strcpy(info, monam(menv[i].number, menv[i].type,
                                                menv[i].enchantment[2], 0));
                strcat(info, " flies over the trap.");

            }
            return;
        }
    }

    if (mons_near(i) && grd[env.trap_x[tr]][env.trap_y[tr]]
                                                    == DNGN_UNDISCOVERED_TRAP)
    {
        if (env.trap_type[tr] == TRAP_TELEPORT
                        || env.trap_type[tr] == TRAP_AMNESIA
                        || env.trap_type[tr] == TRAP_ZOT)
            grd[env.trap_x[tr]][env.trap_y[tr]] = DNGN_TRAP_II;
        else
            grd[env.trap_x[tr]][env.trap_y[tr]] = DNGN_TRAP_I;
    }

    //
    // Monsters don't have the trap damage as a function of level, this
    // is because the monsters are fairly stupid and tend to have fewer
    // hps than the player, so this prevent traps from easily killing
    // large monsters fairly deep in the dungeon.
    //
    switch (env.trap_type[tr])
    {
    case TRAP_DART:
        strcpy(beem[0].beam_name, " dart");
        beem[0].damage = 4;
        beem[0].colour = BLUE;
        beem[0].type = MI_DART;
        break;

    case TRAP_ARROW:
        strcpy(beem[0].beam_name, "n arrow");
        beem[0].damage = 7;
        beem[0].colour = BLUE;
        beem[0].type = MI_ARROW;
        break;

    case TRAP_SPEAR:
        strcpy(beem[0].beam_name, " spear");
        beem[0].damage = 10;
        beem[0].colour = BLACK;
        beem[0].type = MI_SPEAR;
        break;

    case TRAP_AXE:
        strcpy(beem[0].beam_name, "n axe");
        beem[0].damage = 15;
        beem[0].colour = BLACK;
        beem[0].type = MI_AXE;
        break;

    case TRAP_BOLT:
        strcpy(beem[0].beam_name, " bolt");
        beem[0].damage = 13;
        beem[0].colour = BLUE;
        beem[0].type = MI_BOLT;
        break;



    case TRAP_TELEPORT:
        if (menv[i].type == MONS_TUNNELING_WORM
                                        || menv[i].type == MONS_WORM_TAIL)
            return;
        monster_teleport(i, 1);
        return;

    case TRAP_AMNESIA:
        if (mons_near(i))
        {
            strcpy(info, monam(menv[i].number, menv[i].type,
                                                menv[i].enchantment[2], 0));
            strcat(info, " looks momentarily disoriented.");
            mpr(info);
        }
        return;

    case TRAP_BLADE:
        if (random2(5) == 0)
        {
            if (mons_near(i))
            {
                strcpy(info, monam(menv[i].number, menv[i].type,
                                                menv[i].enchantment[2], 0));
                strcat(info, " avoids triggering a blade trap.");
                mpr(info);
            }
            return;
        }
        if (random2(menv[i].evasion) > 8)
        {
            if (mons_near(i))
            {
                strcpy(info, monam(menv[i].number, menv[i].type,
                                                menv[i].enchantment[2], 0));
                strcat(info, " avoids a huge swinging blade.");
                mpr(info);
            }
            return;
        }

        if (mons_near(i))
        {
            strcpy(info, "A huge blade swings out and slices into ");
            strcat(info, monam(menv[i].number, menv[i].type,
                                                menv[i].enchantment[2], 1));
            strcat(info, "!");
            mpr(info);
        }

        damage_taken = 10 + random2(15) + random2(15)
                                            - random2(menv[i].armor_class + 1);

        if (damage_taken <= 0)
            damage_taken = 0;
        menv[i].hit_points -= damage_taken;
        if (menv[i].hit_points <= 0)
        {
            monster_die(i, KILL_MISC, 0);
            menv[i].speed_increment = 1;
        }
        return;

    case TRAP_ZOT:
        if (mons_near(i))
        {
            strcpy(info, monam(menv[i].number, menv[i].type,
                                                menv[i].enchantment[2], 0));
            strcat(info, " enters a Zot trap!");
            mpr(info);
        }
        else
            mpr("You hear a distant \"Zot\".");

        // Zot traps are out to get *YOU*! They benefit hostile monsters
        // and hurt friendly ones
        if (menv[i].behavior == BEH_ENSLAVED)
        {
            beem[0].thing_thrown = KILL_MON;   /* prob unnecessary */
            beem[0].colour = BLACK; /* default: slow it */
            if (random2(4) == 0)
                beem[0].colour = CYAN;     /* Or paralyse it. */
            if (random2(4) == 0)
                beem[0].colour = RED;     /* Or confuse it. */
            mons_ench_f2(i, mons_near(i), func_pass, beem);
            return;
        }

        /* Okay! Now, what nice thing can we do to the monster? */
        beem[0].thing_thrown = KILL_MON;       /* prob unnecessary */
        beem[0].colour = GREEN;                /* default: heal it */

        if (random2(4) == 0)
            beem[0].colour = BLUE; /* Maybe we can haste it instead. */

        if (random2(4) == 0)
            beem[0].colour = MAGENTA; /* Or even turn it invisible. */

        if (random2(5) == 0 && mons_near(i))
        {
            /* Or let it attack you with wild magic. */
            mpr("The power of Zot is invoked against you.");
            miscast_effect(10 + random2(15), random2(30) + 10,
                                                        75 + random2(100), 0);
            return;
        }

        mons_ench_f2(i, mons_near(i), func_pass, beem);
        return;
    }                           // end of switch

    if ((20 + you.your_level * 2) * random2(200) / 100 >= menv[i].evasion)
    {
        damage_taken = random2(beem[0].damage);
        damage_taken -= random2(menv[i].armor_class + 1);
        if (damage_taken > 0)
            menv[i].hit_points -= damage_taken;
    }

    if (damage_taken > 0)
    {
        if (mons_near(i))
        {
            strcpy(info, "A");
            strcat(info, beem[0].beam_name);
            strcat(info, " hits ");
            strcat(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 1));       //gmon_name [mons_class [i]]);

            strcat(info, "!");
            mpr(info);
        }
        if (menv[i].hit_points <= 0)
        {
            monster_die(i, KILL_MISC, 0);
            menv[i].speed_increment = 1;
        }
    }
    else if (mons_near(i))
    {
        strcpy(info, "A");
        strcat(info, beem[0].beam_name);
        strcat(info, " misses ");
        strcat(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 1));   //gmon_name [mons_class [i]]);

        strcat(info, "!");
        mpr(info);
    }

/*      if (random2(10) > 6 && it_no > 300) return; */

    if (random2(10) > 6)
        return;

    beem[0].bx = menv[i].x;
    beem[0].by = menv[i].y;

/*      trapped = tr; */

    itrap(beem, tr);

}                               // end of mons_trap()




void mons_cast(int i, struct bolt beem[1], int spell_cast)
{
    int sumcount = 0;
    int sumcount2;
    int summonik = 0;

    beem[0].beam_source = i;

    if (spell_cast == 20 || spell_cast == 23 || spell_cast == 30 || spell_cast == 43)
    {                           // etc...

        switch (spell_cast)
        {
        case MS_HELLFIRE_BURST:
            beem[0].type = DMNBM_HELLFIRE;
            break;
        case MS_BRAIN_FEED:
            beem[0].type = DMNBM_BRAIN_FEED;
            break;
        case MS_SMITE:
            beem[0].type = DMNBM_SMITING;
            break;
        case MS_MUTATION:
            beem[0].type = DMNBM_MUTATION;
            break;
        }

        if (beem[0].trac_targ == MHITYOU || beem[0].trac_targ == MHITNOT)
        {
            if (mons_near(i))
                direct_effect(beem);

            return;
        }

        mons_direct_effect(beem, i);
        return;
    }

    switch (spell_cast)
    {
    case MS_VAMPIRE_SUMMON:                    // vampire summoning
        sumcount2 = random2(4) + 1;
        for (sumcount = 0; sumcount < sumcount2; sumcount++)
            if (random2(3) != 0)
                create_monster(MONS_GIANT_BAT, 24, menv[i].behavior,
                            menv[i].x, menv[i].y, menv[i].monster_foe, 250);
            else
                create_monster(MONS_RAT, 24, menv[i].behavior,
                            menv[i].x, menv[i].y, menv[i].monster_foe, 250);
        return;

    case MS_LEVEL_SUMMON:         // summon anything appropriate for level
        if (menv[0].behavior == 1 && mons_near(i) && random2(2) == 0
                                            && monster_abjuration(1, 1) >= 1)
        {
            monster_abjuration(menv[i].hit_dice * 10, 0);
            return;
        }

        sumcount2 = random2(4) + 1;
        for (sumcount = 0; sumcount < sumcount2; sumcount++)
            create_monster(250, 24, menv[i].behavior, menv[i].x, menv[i].y,
                                                    menv[i].monster_foe, 250);
        return;

    case MS_FAKE_RAKSHASA_SUMMON:                    // summon fake R
        sumcount2 = random2(2) + 1;
        for (sumcount = 0; sumcount < sumcount2; sumcount++)
            create_monster(MONS_FAKE_RAKSHASA, 21, menv[i].behavior,
                            menv[i].x, menv[i].y, menv[i].monster_foe, 250);
        return;

    case MS_SUMMON_DEMON:                    // summon demon
        if (menv[0].behavior == 1 && mons_near(i) && random2(2) == 0
                                            && monster_abjuration(1, 1) >= 1)
        {
            monster_abjuration(menv[i].hit_dice * 10, 0);
            return;
        }

        create_monster(summon_any_demon(1), 22, menv[i].behavior,
                            menv[i].x, menv[i].y, menv[i].monster_foe, 250);
        return;

    case MS_ANIMATE_DEAD:                    // animate dead
        if (!mons_near(i) || animate_dead(100, menv[i].behavior,
                                                menv[i].monster_foe, 0) <= 0)
        {
            spell_cast = 100;
            return;
        }

        animate_dead(5 + random2(5), menv[i].behavior, menv[i].monster_foe, 1);
        return;

    case MS_SUMMON_LESSER_DEMON:                    // summon lesser demon
        create_monster(summon_any_demon(0), 21, menv[i].behavior,
                            menv[i].x, menv[i].y, menv[i].monster_foe, 250);
        return;

    case MS_SUMMON_UFETUBUS:                    // summon LIGHTCYAN lesser demon
        create_monster(MONS_UFETUBUS, 21, menv[i].behavior,
                            menv[i].x, menv[i].y, menv[i].monster_foe, 250);
        return;

        /*case 37: // summon any demon
           create_monster(summonik, 21, menv [i].behavior, menv [i].x, menv [i].y, menv [i].monster_foe, 250);
           return; */

    case MS_GERYON:                    // Geryon
        create_monster(MONS_BEAST, 23, menv[i].behavior,
                                menv[i].x, menv[i].y, menv[i].monster_foe, 250);
        return;

    case MS_SUMMON_UNDEAD:                    // summon undead around player
        if (menv[0].behavior == 1 && mons_near(i) && random2(2) == 0
                                            && monster_abjuration(1, 1) >= 1)
        {
            monster_abjuration(menv[i].hit_dice * 10, 0);
            return;
        }

        sumcount2 = random2(3) + 2;
        for (sumcount = 0; sumcount < sumcount2; sumcount++)
        {
            do
            {
                summonik = random2(241);
            }
            while (mons_holiness(summonik) != 1);

            create_monster(summonik, 21, menv[i].behavior,
                            you.x_pos, you.y_pos, menv[i].monster_foe, 250);
        }
        return;

    case MS_TORMENT:                    // torment
        if (!mons_near(i) || menv[i].behavior == 7)
            return;

        if (menv[i].enchantment[2] != ENCH_INVIS)
        {
            strcpy(info, monam(menv[i].number, menv[i].type,
                                                menv[i].enchantment[2], 0));
            strcat(info, " calls on the powers of Hell!");
            mpr(info);
        }

        /*  mmov_x = 0; mmov_y = 0; */
        torment();
        return;

    case MS_SUMMON_DEMON_1:                    // summon class 1 demon
        if (menv[0].behavior == 1 && !mons_near(i) && random2(2) == 0
                                            && monster_abjuration(1, 1) >= 1)
        {
            monster_abjuration(menv[i].hit_dice * 10, 0);
            return;
        }

        create_monster(summon_any_demon(2), 21, menv[i].behavior,
                            menv[i].x, menv[i].y, menv[i].monster_foe, 250);
        return;
    }

    //out_of_spec :
    viewwindow(1, false);

    beem[0].move_x = beem[0].trac_targ_x - menv[i].x;
    beem[0].move_y = beem[0].trac_targ_y - menv[i].y;

    beem[0].target_x = beem[0].trac_targ_x;
    beem[0].target_y = beem[0].trac_targ_y;

    beem[0].source_x = menv[i].x;
    beem[0].source_y = menv[i].y;

    // Need to correct this for power of spellcaster
    int power = menv[i].hit_dice * 12;
    SBeam theBeam = mons_spells(spell_cast, power);

    beem[0].colour = theBeam.colour;
    beem[0].range = theBeam.range;
    beem[0].damage = theBeam.damage;
    beem[0].hit = theBeam.hit;
    beem[0].type = theBeam.type;
    beem[0].flavour = theBeam.flavour;
    beem[0].thing_thrown = theBeam.thrown;

    strcpy(beem[0].beam_name, theBeam.name.c_str());

    beem[0].ench_power = beem[0].damage;        // !!!

    beem[0].ench_power = 5 * menv[i].hit_dice;

    if (spell_cast == 5 || spell_cast == 11 || spell_cast == 13
                                                        || spell_cast == 14)
    {                           // you.haste/you.invis

        beem[0].move_x = 0;
        beem[0].move_y = 0;
    }

    if (spell_cast == 14)
        beem[0].ench_power = 2000;

    if (spell_cast == 20)       // burst of hellfire
        return;

    if (theBeam.isBeam)
        beam(beem);
    else
        missile(beem, 0);
}                               // end of void mons_cast()




void monster_teleport(char monstel, char instan)
{
    int p = 0;

    if (instan == 0)
    {
        for (p = 0; p < 3; p++)
        {
            if (menv[monstel].enchantment[p] >= ENCH_TP_I && menv[monstel].enchantment[p] <= ENCH_TP_IV)
            {
                menv[monstel].enchantment[p] = 0;
                if (menv[monstel].enchantment[0] == 0 && menv[monstel].enchantment[1] == 0 && menv[monstel].enchantment[2] == 0)
                    menv[monstel].enchantment1 = 0;
                if (mons_near(monstel) && (player_see_invis() != 0 || menv[monstel].enchantment[2] != ENCH_INVIS))
                {
                    strcpy(info, monam(menv[monstel].number, menv[monstel].type, menv[monstel].enchantment[2], 0));
                    strcat(info, " seems more stable.");
                    mpr(info);
                }
                return;
            }
        }                       // end of for p

        for (p = 0; p < 3; p++)
        {
            if (menv[monstel].enchantment[p] == 0)
            {

                menv[monstel].enchantment[p] = ENCH_TP_III + random2(2);
                menv[monstel].enchantment1 = 1;
                break;
            }
        }
        return;
    }


    if (mons_near(monstel) && (player_see_invis() != 0 || menv[monstel].enchantment[2] != ENCH_INVIS))
    {
        strcpy(info, monam(menv[monstel].number, menv[monstel].type, menv[monstel].enchantment[2], 0));
        strcat(info, " disappears!");
        mpr(info);
    }
    mgrd[menv[monstel].x][menv[monstel].y] = MNG;

    char ogrid = 67;

    if (menv[monstel].type >= MLAVA0)
        ogrid = 61;
    if (menv[monstel].type >= MWATER0)
        ogrid = 62;

    do
    {
        menv[monstel].x = random2(60) + 10;
        menv[monstel].y = random2(50) + 10;
    }
    while ((grd[menv[monstel].x][menv[monstel].y] != ogrid && (ogrid != 62 || grd[menv[monstel].x][menv[monstel].y] != 65)) || mgrd[menv[monstel].x][menv[monstel].y] != MNG);

    mgrd[menv[monstel].x][menv[monstel].y] = monstel;

/* Mimics change form/colour when tported */
    if (menv[monstel].type >= MONS_GOLD_MIMIC && menv[monstel].type <= MONS_POTION_MIMIC)
    {
        menv[monstel].type = MONS_GOLD_MIMIC + random2(5);
        menv[monstel].number = 1 + random2(15);
        if (menv[monstel].type == MONS_WEAPON_MIMIC || menv[monstel].type == MONS_ARMOUR_MIMIC)         /* weapon/armour mimic */
        {
            if (random2(4) == 0)
                menv[monstel].number = LIGHTCYAN;
            else
                menv[monstel].number = BROWN;
            if (random2(10) == 0)
                menv[monstel].number = CYAN;
            if (random2(20) == 0)
                menv[monstel].number = random2(15) + 1;
        }
    }

}                               // end of void monster_teleport



void dragon(int i, struct bolt beem[1])
{
    viewwindow(1, false);
    beem[0].move_x = 0;
    beem[0].move_y = 0;

    beem[0].move_x = beem[0].trac_targ_x - menv[i].x;
    beem[0].move_y = beem[0].trac_targ_y - menv[i].y;

    if (beem[0].move_x != 0 || beem[0].move_y != 0)
    {
        beem[0].range = random2(10) + 5;

        beem[0].source_x = menv[i].x;
        beem[0].source_y = menv[i].y;

        strcpy(beem[0].beam_name, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 4));

        switch (menv[i].type)
        {
        case MONS_FIREDRAKE:
        case MONS_HELL_HOUND:
        case MONS_DRAGON:
        case MONS_LINDWORM:
        case MONS_XTAHUA:       // Xtahua

            strcat(beem[0].beam_name, "'s blast of flame");
            beem[0].flavour = BEAM_FIRE;
            beem[0].colour = RED;
            break;

        case MONS_ICE_DRAGON:
            strcat(beem[0].beam_name, "'s blast of cold");
            beem[0].flavour = BEAM_COLD;
            beem[0].colour = WHITE;
            break;

        default:
            DEBUGSTR("Bad monster class in dragon()");
        }

        beem[0].damage = 100 + menv[i].hit_dice * 2;
        beem[0].type = SYM_ZAP;
        beem[0].hit = 30;
        beem[0].beam_source = i;

        if (mons_near(i))
        {
            strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));
            strcat(info, " breathes.");
            mpr(info);
        }

        beem[0].thing_thrown = KILL_MON;
        beam(beem);
    }
}                               // end of dragon();




void mons_throw(int i, struct bolt beem[1], int hand_used)
{

    char shoot = 0;

    beem[0].range = 9;
    beem[0].beam_source = i;
/*if (menv [i].x > you.x_pos - 6 && menv [i].x < you.x_pos + 6 && menv [i].y > you.y_pos - 6 && menv [i].y < you.y_pos + 6)
   {

   if (show [menv [i].x - you.x_pos + 6] [menv [i].y - you.y_pos + 6] != 0)
   { */
    if (mons_near(i))           /* This is obviously a problem; no throwing unless mons on screen. */
    {
        beem[0].move_x = 0;
        beem[0].move_y = 0;

        beem[0].move_x = beem[0].trac_targ_x - menv[i].x;
        beem[0].move_y = beem[0].trac_targ_y - menv[i].y;

        if (beem[0].move_x != 0 || beem[0].move_y != 0)
        {
             beem[0].type = SYM_MISSILE;
            beem[0].source_x = menv[i].x;
            beem[0].source_y = menv[i].y;
            beem[0].colour = mitm.colour[hand_used];
            beem[0].flavour = BEAM_MISSILE;
            beem[0].damage = property(mitm.base_type[hand_used], mitm.sub_type[hand_used], 0) + mitm.pluses[hand_used] - 50;
            beem[0].hit = 7;
            beem[0].hit += mitm.pluses[hand_used] - 50;

            beem[0].thing_thrown = KILL_MON;

            if (menv[i].type == MONS_MANTICORE)
            {
                strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));   //gmon_name [mons_class [i]]);

                strcat(info, " flicks its tail!");
                beem[0].damage = 10;
                beem[0].hit *= 2;
            }





            if (menv[i].type != MONS_MANTICORE)
            {

                if (menv[i].inv[0] != 501 && mitm.base_type[menv[i].inv[0]] == OBJ_WEAPONS && mitm.sub_type[menv[i].inv[0]] == mitm.sub_type[menv[i].inv[1]] + 13 && mitm.sub_type[menv[i].inv[1]] <= WPN_HAND_CROSSBOW)
                {

                    beem[0].damage = property(mitm.base_type[menv[i].inv[1]], mitm.sub_type[menv[i].inv[1]], 1);

                    int hoggl = mitm.pluses2[menv[i].inv[0]] - 50;

                    if (mitm.pluses2[menv[i].inv[0]] > 80)
                        hoggl -= 100;

                    beem[0].damage += hoggl;
                    beem[0].damage += menv[i].hit_dice;


                    /* elven bow w/ elven arrow, also orcis */
                    if (mitm.special[menv[i].inv[0]] / 30 == mitm.special[menv[i].inv[1]] / 30)
                    {
                        beem[0].hit++;
                        beem[0].damage++;
                        if (mitm.special[menv[i].inv[0]] / 30 == 120)
                            beem[0].hit++;
                    }


                    int ghoggl = mitm.pluses[menv[i].inv[0]] - 50;

                    if (mitm.pluses[menv[i].inv[0]] > 80)
                        ghoggl -= 100;

                    beem[0].hit += ghoggl;

                    beem[0].range += 5;

                    if (mitm.pluses[menv[i].inv[0]] > 80)
                    {
                        beem[0].damage -= 100;
                        beem[0].hit -= 100;
                    }
                    shoot = 1;


                    if (mitm.special[menv[i].inv[0]] % 30 == SPWPN_FLAME)
                    {
                        beem[0].hit += 2;
                        beem[0].damage += 1 + random2(5);
                        beem[0].flavour = BEAM_FIRE;
                        strcpy(beem[0].beam_name, "bolt of ");
                        if (mitm.special[hand_used] % 30 == 3 || mitm.special[hand_used] % 30 == 4)
                            strcat(beem[0].beam_name, "poison ");
                        strcat(beem[0].beam_name, "flame");
                        beem[0].colour = RED;
                        beem[0].type = SYM_ZAP;
                        beem[0].thing_thrown = KILL_MON_MISSILE;
                    }
                    if (mitm.special[menv[i].inv[0]] % 30 == SPWPN_FROST)
                    {
                        beem[0].hit += 2;
                        beem[0].damage += 1 + random2(5);
                        beem[0].flavour = BEAM_COLD;
                        strcpy(beem[0].beam_name, "bolt of ");
                        if (mitm.special[hand_used] % 30 == 3 || mitm.special[hand_used] % 30 == 4)
                            strcat(beem[0].beam_name, "poison ");
                        strcat(beem[0].beam_name, "frost");
                        beem[0].colour = WHITE;
                        beem[0].type = SYM_ZAP;
                        beem[0].thing_thrown = KILL_MON_MISSILE;
                    }

                }


                strcpy(info, monam(menv[i].number, menv[i].type, menv[i].enchantment[2], 0));   //gmon_name [mons_class [i]]);

                if (shoot == 1)
                {
                    strcat(info, " shoots ");
                }
                else
                    strcat(info, " throws ");

                item_name(mitm.pluses2[hand_used], mitm.base_type[hand_used], mitm.sub_type[hand_used], mitm.special[hand_used], mitm.pluses[hand_used], 1, mitm.id[hand_used], 3, str_pass);
                strcat(info, str_pass);
                strcat(info, ".");

            }

            mpr(info);
            missile(beem, hand_used);
            mitm.quantity[hand_used]--;
            if (mitm.quantity[hand_used] == 0)
            {
                menv[i].inv[1] = 501;
/*       it_no --; */
            }
/*      brek = 1; */
        }
//}
    }

    if (menv[i].type == MONS_CENTAUR)
        menv[i].speed_increment -= 10;


}


void spore_goes_pop(int i)      /* should really do something about mons_hit, but can't be bothered */
{

    struct bolt beam[1];

    if (mons_near(i))
    {
        viewwindow(1, false);
        strcpy(info, "The giant spore explodes!");
        mpr(info);
    }
    beam[0].beam_source = i;
    beam[0].type = SYM_SPORE;
    beam[0].damage = 115;
    beam[0].flavour = BEAM_SPORE;       // <=- not sure about this

    beam[0].bx = menv[i].x;
    beam[0].by = menv[i].y;
    strcpy(beam[0].beam_name, "explosion of spores");
    beam[0].colour = LIGHTGREY;
    beam[0].thing_thrown = KILL_MON;   // someone else's explosion

    explosion(1, beam);
}



SBeam mons_spells(char spell_cast, int power)
{
    ASSERT(power > 0);

    SBeam beam;

#if DEBUG
    beam.name = "****";         // initialize to some bogus values so we can catch problems

    beam.colour = 1000;
    beam.range = -1;
    beam.damage = -1;
    beam.hit = -1;
    beam.type = -1;
    beam.flavour = -1;
    beam.thrown = -1;
#endif

    switch (spell_cast)
    {
    case MS_MMISSILE:                     // magic missile
        //strcpy(info [info_lines], "The ");
        //strcpy(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
        //strcat(info [info_lines], " hurls a ball of sizzling energy!");
        //incrl();

        beam.colour = LIGHTMAGENTA;       //inv_colour [throw_2];//icolour [inv_class [throw_2]] [inv_type [throw_2]];

        beam.name = "magic dart";       // inv_name [throw_2]);

        beam.range = random2(5) + 7;
        beam.damage = 104 + (power / 100);
        beam.hit = 1500;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        //return 0;
        beam.flavour = BEAM_MMISSILE;   // a magical missile

        beam.isBeam = false;
        break;

    case MS_FLAME:                     // flame
        //strcpy(info [info_lines], "The ";
        //strcpy(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
        //strcat(info [info_lines], " hurls a puff of flame!";
        //incrl();

        beam.colour = RED;
        beam.name = "puff of flame";
        beam.range = random2(5) + 7;
        beam.damage = 105 + power / 40;         // should this be the same as magic missile?

        beam.hit = 60;
        beam.type = SYM_ZAP;        // maybe make     put a # on wherever it hits?

        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_FIRE;   // fire

        beam.isBeam = false;
        break;

    case MS_FROST:                     // frost
        //strcpy(info [info_lines], "The ";
        //strcpy(info [info_lines], monam (mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
        //strcat(info [info_lines], " hurls a puff of frost!";
        //incrl();

        beam.colour = WHITE;
        beam.name = "puff of frost";
        beam.range = random2(5) + 7;
        beam.damage = 105 + power / 40;         // should this be the same as magic missile?

        beam.hit = 60;
        beam.type = SYM_ZAP;        // maybe make     put a # on wherever it hits?

        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_COLD;   // cold

        beam.isBeam = false;
        break;

    case MS_PARALYSIS:                     // paralysis

        beam.name = "0";
        beam.range = random2(5) + 6;
        beam.type = 0;
        beam.colour = CYAN;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MAGIC;   // magic

        beam.isBeam = true;
        break;

    case MS_SLOW:                     // slow monster

        beam.name = "0";
        beam.range = random2(5) + 6;
        beam.type = 0;
        beam.colour = BLACK;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MAGIC;   // magic

        beam.isBeam = true;
        break;

    case MS_HASTE:                     // haste self

        beam.name = "0";
        beam.range = random2(5) + 6;
        beam.type = 0;
        beam.colour = BLUE;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MAGIC;   // magic

        beam.isBeam = true;
        break;

    case MS_SLOW_DUP:                     // slow monster

        beam.name = "0";
        beam.range = random2(5) + 6;
        beam.type = 0;
        beam.colour = RED;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MAGIC;   // magic

        beam.isBeam = true;
        break;

    case MS_VENOM_BOLT:                     // venom bolt

        beam.name = "bolt of poison";
        beam.range = random2(10) + 8;
        beam.damage = 106 + power / 13;
        beam.colour = LIGHTGREEN;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_POISON;   // poison

        beam.hit = 7 + random2(power) / 80;
        beam.isBeam = true;
        break;

    case MS_FIRE_BOLT:
        beam.name = "bolt of fire";
        beam.range = random2(10) + 5;
        beam.damage = 108 + power / 11;
        beam.colour = RED;
        beam.type = SYM_ZAP;

        beam.thrown = KILL_MON;
        beam.flavour = BEAM_FIRE;   // fire

        beam.hit = 8 + random2(power) / 80;     // hit

        beam.isBeam = true;
        break;

    case MS_COLD_BOLT:
        beam.name = "bolt of cold";
        beam.range = random2(10) + 5;
        beam.damage = 108 + power / 11;
        beam.colour = WHITE;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_COLD;   // cold

        beam.hit = 8 + random2(power) / 80;     // hit

        beam.isBeam = true;
        break;

    case MS_LIGHTNING_BOLT:                    // lightning

        beam.name = "bolt of lightning";
        beam.range = random2(10) + 8;
        beam.damage = 110 + power / 9;
        beam.colour = LIGHTCYAN;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_ELECTRICITY;   // electricity

        beam.hit = 10 + random2(power) / 40;
        beam.isBeam = true;
        break;

    case MS_INVIS:                    // make invisible

        beam.name = "0";
        beam.range = random2(5) + 6;
        beam.type = 0;
        beam.colour = MAGENTA;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_MAGIC;   // magic

        beam.isBeam = true;
        break;

    case MS_FIREBALL:                    // fireball

        beam.colour = RED;
        beam.name = "fireball";
        beam.range = random2(5) + 7;
        beam.damage = 7 + beam.damage / 10;
        beam.hit = 40;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_YOU_MISSILE;  //DML: ???
        beam.flavour = BEAM_EXPLOSION;  // fire

        beam.isBeam = false;
        break;

    case MS_HEAL:                    // healing

        beam.name = "0";
        beam.range = random2(5) + 6;
        beam.type = 0;
        beam.colour = GREEN;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_MAGIC;   // magic

        beam.isBeam = true;
        break;

    case MS_TELEPORT:                    // teleportation

        beam.name = "0";
        beam.range = random2(5) + 6;
        beam.type = 0;
        beam.colour = LIGHTGREY;        // 6 is used by digging

        beam.thrown = KILL_MON;
        beam.flavour = BEAM_MAGIC;   // magic

        beam.isBeam = true;
        break;

    case MS_TELEPORT_OTHER:                    // teleport target away

        beam.name = "0";
        beam.range = random2(5) + 6;
        beam.type = 0;
        beam.colour = LIGHTGREY;        // 6 is used by digging

        beam.thrown = KILL_MON;
        beam.flavour = BEAM_MAGIC;   // magic

        beam.isBeam = true;
        break;

    case MS_BLINK:
        beam.isBeam = false;    // blink

        break;

    case MS_CRYSTAL_SPEAR:                    // was splinters

        beam.name = "crystal spear";
        beam.range = random2(10) + 8;
        beam.damage = 112 + power / 10;
        beam.colour = WHITE;
        beam.type = SYM_MISSILE;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_MMISSILE;
        beam.hit = 6;           // + random2(power) / 10;

        beam.isBeam = false;
        break;

    case MS_DIG:                    // digging

        beam.name = "0";
        beam.range = random2(5) + 4 + random2(power) / 10;
        beam.type = 0;
        beam.colour = BROWN;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_MAGIC;
        beam.isBeam = true;
        break;

    case MS_NEGATIVE_BOLT:                    // negative energy

        beam.name = "bolt of negative energy";
        beam.range = random2(10) + 8;
        beam.damage = 106 + power / 13;
        beam.colour = DARKGREY;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_NEG;   // -ve energy

        beam.hit = 7 + random2(power) / 80;
        beam.isBeam = true;
        break;

        // 20, 21 are used

    case MS_ORB_ENERGY:                    // mystic blast

        beam.colour = LIGHTMAGENTA;
        beam.name = "orb of energy";
        beam.range = random2(5) + 7;
        beam.damage = 107 + (power / 14);
        beam.hit = 10 + (power / 20);
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;   // a magical missile

        beam.isBeam = false;
        break;

        // 23 is brain feed

    case MS_STEAM_BALL:                    // ball of steam

        beam.colour = LIGHTGREY;
        beam.name = "ball of steam";
        beam.range = random2(5) + 7;
        beam.damage = 106;
        beam.hit = 11;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_FIRE;   // fire - I think this is appropriate

        beam.isBeam = false;
        break;

        // 27 is summon devils
        // 28 is animate dead

    case MS_PAIN:                    // pain

        beam.name = "0";
        beam.range = random2(8) + 8;
        beam.type = 0;
        beam.colour = LIGHTMAGENTA;       // pain

        beam.thrown = KILL_MON;
        beam.damage = 50;
        beam.hit = 7 + (power / 20);
        beam.flavour = BEAM_MAGIC;   // magic

        beam.isBeam = true;
        break;

        // 30 is smiting

    case MS_STICKY_FLAME:                    // sticky flame

        beam.colour = RED;
        beam.name = "sticky flame";
        beam.range = random2(5) + 7;
        beam.damage = 103 + power / 50;
        beam.hit = 8 + power / 15;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_FIRE;   // fire

        beam.isBeam = false;
        break;

    case MS_POISON_BLAST:                    // demon

        beam.name = "blast of poison";
        beam.range = random2(10) + 8;
        beam.damage = 103 + power / 25;
        beam.colour = LIGHTGREEN;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_POISON;   // poison

        beam.hit = 7 + random2(power) / 80;
        beam.isBeam = true;
        break;

    case MS_PURPLE_BLAST:                    // purple bang thing

        beam.colour = LIGHTMAGENTA;
        beam.name = "blast";
        beam.range = random2(5) + 7;
        beam.damage = 106;
        beam.hit = 9;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_EXPLOSION;  // an exploding magical missile

        beam.isBeam = false;
        break;

    case MS_ENERGY_BOLT:                    // eye of devastation

        beam.colour = YELLOW;
        beam.name = "bolt of energy";
        beam.range = random2(15) + 10;
        beam.damage = 120;
        beam.hit = 9;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_NUKE;  // a magical missile which destroys walls

        beam.isBeam = true;
        break;

    case MS_STING:                    // sting

        beam.colour = GREEN;
        beam.name = "sting";
        beam.range = random2(5) + 9;
        beam.damage = 6 + power / 25;
        beam.hit = 60;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_FIRE;   // fire

        beam.isBeam = false;
        break;

    case MS_IRON_BOLT:                    // Iron Bolt

        beam.colour = LIGHTCYAN;
        beam.name = "iron bolt";
        beam.range = random2(5) + 5;
        beam.damage = 108 + (power / 9);
        beam.hit = 6 + (power / 25);
        beam.type = SYM_MISSILE;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;   // a magical missile - or similarly unresisted thing

        beam.isBeam = false;
        break;

    case MS_STONE_ARROW:                    // stone arrow

        beam.colour = LIGHTGREY;
        beam.name = "stone arrow";
        beam.range = random2(5) + 9;
        beam.damage = 105 + (power / 10);
        beam.hit = 5 + power / 47;
        beam.type = SYM_MISSILE;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;   // a magical missile - or similarly unresisted thing

        beam.isBeam = false;
        break;

    case MS_POISON_SPLASH:                    // splash of poison

        beam.colour = GREEN;
        beam.name = "splash of poison";
        beam.range = random2(6) + 6;
        beam.damage = 4 + power / 10;
        beam.hit = 9;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_POISON;   // poison

        break;
        beam.isBeam = false;

    case MS_ZULZER:                    // zulzer

        beam.colour = YELLOW;
        beam.name = "zulzer blast";
        beam.range = random2(4) + 15;
        beam.damage = 125;
        beam.hit = 7;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;
        beam.isBeam = false;
        break;

    case MS_DISINTEGRATE:                    // disintegrate

        beam.name = "0";
        beam.range = random2(8) + 8;
        beam.type = 0;
        beam.colour = WHITE;       // disint

        beam.thrown = KILL_MON;
        beam.damage = 50;
        beam.hit = 30 + (power / 10);
        beam.flavour = BEAM_MAGIC;   // magic

        beam.isBeam = true;
        break;

    case MS_MARSH_GAS:                    // swamp drake

        beam.name = "foul vapour";
        beam.range = random2(10) + 8;
        beam.damage = 102 + power / 25;
        beam.colour = GREEN;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_POISON;   // poison

        beam.hit = 7 + random2(power) / 80;
        beam.isBeam = false;
        break;

    case MS_QUICKSILVER_BOLT:                    // Quicksilver dragon

        beam.colour = 1 + random2(15);
        beam.name = "bolt of energy";
        beam.range = random2(15) + 10;
        beam.damage = 125;
        beam.hit = 9;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MMISSILE;   // a magical missile

        beam.isBeam = false;
        break;

    case MS_HELLFIRE:                    // fiend's hellfire

        beam.name = "hellfire";
        beam.colour = RED;
        beam.range = random2(10) + 5;
        beam.damage = 25;
        beam.hit = 20;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_EXPLOSION;  // hellfire

        beam.isBeam = true;
        break;

    case MS_METAL_SPLINTERS:                    // metal splinters

        beam.name = "spray of metal splinters";
        beam.range = random2(10) + 8;
        beam.damage = 120 + power / 20;
        beam.colour = CYAN;
        beam.type = SYM_ZAP;
        beam.thrown = KILL_MON;
        beam.flavour = BEAM_FRAG;
        beam.hit = 15 + random2(power) / 50;
        beam.isBeam = true;
        break;

    case MS_BANISHMENT:                    // banishment

        beam.name = "0";
        beam.range = random2(5) + 6;
        beam.type = 0;
        beam.colour = LIGHTGREEN;
        beam.thrown = KILL_MON_MISSILE;
        beam.flavour = BEAM_MAGIC;   // magic
        beam.isBeam = true;
        break;

    default:
        DEBUGSTR("Unknown spell");
    }

    return beam;
}


unsigned char monster_abjuration(int pow, char test)
{

    int ab = 0;
    unsigned char result = 0;

    if (test == 0)
    {
        mpr("Send 'em back where they came from!");
    }

    for (ab = 0; ab < MNST; ab++)
    {
        if (menv[ab].type == -1)
            continue;
        if (!mons_near(ab))
            continue;
        if (menv[ab].behavior != BEH_ENSLAVED)
            continue;
        if (menv[ab].enchantment1 == 0 || menv[ab].enchantment[1] < ENCH_ABJ_I || menv[ab].enchantment[1] > ENCH_ABJ_VI)
            continue;
        result++;
        if (test == 1)
            continue;

        menv[ab].enchantment[1] -= 1 + random2(pow) / 3;
        if (menv[ab].enchantment[1] < ENCH_ABJ_I)
        {
            monster_die(ab, KILL_RESET, 0);
            continue;
        }
        strcpy(info, monam(menv[ab].number, menv[ab].type, menv[ab].enchantment[2], 0));
        strcat(info, " shudders.");
        mpr(info);

    }                           // end of for ab

    return result;

}                               // end of void abjuration

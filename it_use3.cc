/*
 *  File:       it_use3.cc
 *  Summary:    Functions for using some of the wackier inventory items.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <4>     6/13/99         BWR             Auto ID Channel staff
 *      <3>     5/22/99         BWR             SPWLD_POWER is now HP/13 - 3
 *      <2>     5/20/99         BWR             Capped SPWLD_POWER to +20
 *      <1>     -/--/--         LRH             Created
 */

#include "AppHdr.h"
#include "it_use3.h"

#include <string.h>

#include "externs.h"
#include "enum.h"
#include "it_use2.h"
#include "item_use.h"
#include "spells.h"
#include "stuff.h"
#include "itemname.h"
#include "decks.h"
#include "spell.h"
#include "spells2.h"
#include "effects.h"
#include "player.h"
#include "monplace.h"
#include "skills.h"
#include "skills2.h"
#include "bang.h"
#include "view.h"
#include "spells0.h"
#include "spells1.h"
#include "monstuff.h"
#include "items.h"
#include "misc.h"

void ball_of_seeing(void);
void efreet_flask(void);
void disc_of_storms(void);
void ball_of_energy(void);
void ball_of_fixation(void);
void box_of_beasts(void);
void staff_spell(char zap_device_2);

extern unsigned char show_green;        /* defined in view.cc, I think */
extern int book_thing;          /* defined in spells.cc */
extern unsigned char wield_change;      /* defined in output.cc */


void special_wielded()
{
    int old_plus;
    int old_plus2;

    switch (you.special_wield)
    {
    case SPWLD_SING:
        if (random2(20) == 0)
        {
            strcpy(info, "The Singing Sword ");
            switch (random2(32))
            {
            case 0:
                strcat(info, "hums a little tune.");
                mpr(info);
                break;
            case 1:
                strcat(info, "breaks into glorious song!");
                mpr(info);
                break;
            case 2:
                strcat(info, "sings.");
                mpr(info);
                break;
            case 3:
                strcat(info, "sings loudly.");
                mpr(info);
                break;
            case 4:
                strcat(info, "chimes melodiously.");
                mpr(info);
                break;
            case 5:
                strcat(info, "makes a horrible noise.");
                mpr(info);
                break;
            case 6:
                strcat(info, "sings off-key.");
                mpr(info);
                break;
            case 7:
                strcat(info, "sings 'tra-la-la'.");
                mpr(info);
                break;
            case 8:
                strcat(info, "burbles away merrily.");
                mpr(info);
                break;
            case 9:
                strcat(info, "gurgles.");
                mpr(info);
                break;
            case 10:
                strcat(info, "suddenly shrieks!");
                mpr(info);
                break;
            case 11:
                strcat(info, "cackles.");
                mpr(info);
                break;
            case 12:
                strcat(info, "warbles.");
                mpr(info);
                break;
            case 13:
                strcat(info, "chimes harmoniously.");
                mpr(info);
                break;
            case 14:
                strcat(info, "makes beautiful music.");
                mpr(info);
                break;
            case 15:
                strcat(info, "produces a loud orchestral chord.");
                mpr(info);
                break;
            case 16:
                strcat(info, "whines plaintively.");
                mpr(info);
                break;
            case 17:
                strcat(info, "tinkles.");
                mpr(info);
                break;
            case 18:
                strcat(info, "rings like a bell.");
                mpr(info);
                break;
            case 19:
                strcat(info, "wails mournfully.");
                mpr(info);
                break;
            case 20:
                strcat(info, "practices its scales.");
                mpr(info);
                break;
            case 21:
                strcat(info, "lilts tunefully.");
                mpr(info);
                break;
            case 22:
                strcat(info, "hums tunelessly.");
                mpr(info);
                break;
            case 23:
                strcat(info, "sighs.");
                mpr(info);
                break;
            case 24:
                strcat(info, "makes a deep moaning sound.");
                mpr(info);
                break;
            case 25:
                strcat(info, "makes a popping sound.");
                mpr(info);
                break;
            case 26:
                strcat(info, "sings a sudden stoccato note.");
                mpr(info);
                break;
            case 27:
                strcat(info, "says 'Hi! I'm the Singing Sword!'.");
                mpr(info);
                break;
            case 28:
                strcat(info, "whispers something.");
                mpr(info);
                break;
            case 29:
                strcat(info, "speaks gibberish.");
                mpr(info);
                break;
            case 30:
                strcat(info, "raves incoherently.");
                mpr(info);
                break;
            case 31:
                strcat(info, "yells in some weird language.");
                mpr(info);
                break;
            }
            noisy(25, you.x_pos, you.y_pos);
        }
        break;


    case SPWLD_CURSE:           // scythe of Curses

        if (random2(30) == 0)
            curse_an_item(0, 0);
        break;

    case SPWLD_VARIABLE:        // mace of variability

        if (you.inv_plus[you.equip[EQ_WEAPON]] > 100)
            you.inv_plus[you.equip[EQ_WEAPON]] -= 100;
        if (random2(5) != 0)
            break;
        you.inv_plus[you.equip[EQ_WEAPON]] += random2(2);
        you.inv_plus[you.equip[EQ_WEAPON]] -= random2(2);
        you.inv_plus2[you.equip[EQ_WEAPON]] += random2(2);
        you.inv_plus2[you.equip[EQ_WEAPON]] -= random2(2);
        if (you.inv_plus[you.equip[EQ_WEAPON]] < 46)
            you.inv_plus[you.equip[EQ_WEAPON]] = 46;
        if (you.inv_plus[you.equip[EQ_WEAPON]] > 57)
            you.inv_plus[you.equip[EQ_WEAPON]] = 57;
        if (you.inv_plus2[you.equip[EQ_WEAPON]] < 46)
            you.inv_plus2[you.equip[EQ_WEAPON]] = 46;
        if (you.inv_plus2[you.equip[EQ_WEAPON]] > 57)
            you.inv_plus2[you.equip[EQ_WEAPON]] = 57;
        you.inv_colour[you.equip[EQ_WEAPON]] = random2(15) + 1;
        wield_change = 1;
        wield_change = 1;
        break;

        // 5 - glaive

    case SPWLD_TORMENT: // mace of torment

        if (random2(200) == 0)
            torment();
        break;

    case SPWLD_ZONGULDROK:
        if (random2(5) == 0)
            animate_dead(1 + random2(3), 1, MHITYOU, 1);
        break;

    case SPWLD_POWER:
        old_plus = you.inv_plus[you.equip[EQ_WEAPON]];
        old_plus2 = you.inv_plus2[you.equip[EQ_WEAPON]];

        you.inv_plus[you.equip[EQ_WEAPON]] = 47 + you.hp / 13;
        you.inv_plus2[you.equip[EQ_WEAPON]] = 47 + you.hp / 13;

        // added cap to weaken the effects a bit
        if (you.inv_plus[you.equip[EQ_WEAPON]] > 70)
        {
            you.inv_plus[you.equip[EQ_WEAPON]] = 70;
            you.inv_plus2[you.equip[EQ_WEAPON]] = 70;
        }

        if (old_plus != you.inv_plus[you.equip[EQ_WEAPON]] ||
            old_plus2 != you.inv_plus2[you.equip[EQ_WEAPON]])
        {
            wield_change = 1;
        }
        break;

    case SPWLD_WUCAD_MU:
        old_plus = you.inv_plus[you.equip[EQ_WEAPON]];
        old_plus2 = you.inv_plus2[you.equip[EQ_WEAPON]];
        if (you.intel > 25)
        {
            you.inv_plus[you.equip[EQ_WEAPON]] = 72;
            you.inv_plus2[you.equip[EQ_WEAPON]] = 63;
        }
        else
        {
            you.inv_plus[you.equip[EQ_WEAPON]] = 47 + you.intel;
            you.inv_plus2[you.equip[EQ_WEAPON]] = 50 + you.intel / 2;
        }
        if (old_plus != you.inv_plus[you.equip[EQ_WEAPON]] || old_plus2 != you.inv_plus2[you.equip[EQ_WEAPON
                                                                         ]])
            wield_change = 1;
        break;

    case SPWLD_SHADOW:          // shadow lamp

        if (random2(8) <= 0 + player_spec_death())
            create_monster(MONS_SHADOW, 21, 7, you.x_pos, you.y_pos, MHITNOT, 250);
        //naughty(5, 1);
        show_green = DARKGREY;
        break;

    case 51:                    /* randart makes noises */
        if (random2(20) == 0)
        {
            in_name(you.equip[EQ_WEAPON], 4, str_pass);
            strcpy(info, str_pass);
            strcat(info, " lets out a weird humming sound.");
            mpr(info);
            noisy(25, you.x_pos, you.y_pos);
        }
        break;

    case 52:                    /* randart makes noises */
        if (random2(20) == 0)
        {
            in_name(you.equip[EQ_WEAPON], 4, str_pass);
            strcpy(info, str_pass);
            strcat(info, " chimes like a gong.");
            mpr(info);
            noisy(25, you.x_pos, you.y_pos);
        }
        break;

    case 53:                    /* randart makes noises */
        if (random2(20) == 0)
        {
            mpr("You hear a voice call your name.");
            noisy(25, you.x_pos, you.y_pos);
        }
        break;

    case 54:                    /* randart makes noises */
        if (random2(20) == 0)
        {
            mpr("You hear a shout.");
            noisy(25, you.x_pos, you.y_pos);
        }
        break;

    }                           // end switch

}                               // end void special_wielded





void invoke_wielded(void)
{
    char opened_gates = 0;
    unsigned char spell_casted = random2(21);
    int count_x, count_y;

    if (you.equip[EQ_WEAPON] == -1)
    {
        strcpy(info, "You aren't wielding anything!");
        mpr(info);
        return;
    }

    if (you.berserker != 0)
    {
        strcpy(info, "You are too berserk!");
        mpr(info);
        return;
    }

    you.turn_is_over = 1;

    switch (you.inv_class[you.equip[EQ_WEAPON]])
    {
    case OBJ_WEAPONS:
        switch (you.inv_dam[you.equip[EQ_WEAPON]])
        {
        case NWPN_STAFF_OF_DISPATER:    // staff of Dispater

            if (you.deaths_door != 0 || you.hp <= 10 || you.magic_points <= 4)
                goto nothing_hap;
            strcpy(info, "You feel the staff feeding on your energy!");
            mpr(info);
            you.hp -= random2(10) + random2(10) + 5;
            if (you.hp <= 0)
                you.hp = 1;
            you.magic_points -= random2(3) + random2(3) + 2;
            if (you.magic_points <= 0)
                you.magic_points = 1;
            you.redraw_hit_points = 1;
            you.redraw_magic_points = 1;
            your_spells(SPELL_HELLFIRE, 100, 0);        // power (2nd number) is meaningless

            break;

        case NWPN_SCEPTRE_OF_ASMODEUS:          // sceptre of Asmodeus

            spell_casted = random2(21);
            if (spell_casted == 0)
                goto nothing_hap;       // nothing happens

            if (spell_casted < 2)       // summon devils, maybe a Fiend

            {
                spell_casted = MONS_HELLION + random2(10);
                if (random2(4) == 0)
                {
                    strcpy(info, "\"Your arrogance condemns you, mortal!\"");
                    spell_casted = MONS_FIEND;        /* Fiend! */
                }
                else
                    strcpy(info, "The Sceptre summons one of its servants.");
                mpr(info);
                create_monster(spell_casted, 25, 1, you.x_pos, you.y_pos, MHITNOT, 250);
                break;
            }
            spell_casted = SPELL_BOLT_OF_FIRE;  // firebolt

            if (random2(3) == 0)
                spell_casted = SPELL_LIGHTNING_BOLT;    // lightning

            if (random2(4) == 0)
                spell_casted = SPELL_BOLT_OF_DRAINING;  // draining

            if (random2(20) == 0)
                spell_casted = SPELL_HELLFIRE;  // hellfire

            your_spells(spell_casted, 10, 0);
            break;

        case NWPN_STAFF_OF_OLGREB:      // staff of Olgreb

            if (you.magic_points <= 5 || you.skills[SK_SPELLCASTING] <= random2(11))
                goto nothing_hap;
            you.magic_points -= 4;
            if (you.magic_points <= 0)
                you.magic_points = 0;
            you.redraw_magic_points = 1;
            your_spells(SPELL_OLGREBS_TOXIC_RADIANCE, 100, 0);  // toxic rad

            your_spells(SPELL_VENOM_BOLT, 100, 0);      // venom bolt

            break;

        case NWPN_STAFF_OF_WUCAD_MU:    // staff of Wucad Mu

            if (you.magic_points == you.max_magic_points || random2(4) == 0)
            {
                strcpy(info, "Nothing appears to happen.");
                mpr(info);
                break;
            }
            you.magic_points += 3 + random2(5);
            if (you.magic_points > you.max_magic_points)
                you.magic_points = you.max_magic_points;
            you.redraw_magic_points = 1;
            strcpy(info, "Magical energy flows into your mind!");
            mpr(info);
            if (random2(3) == 0)
                miscast_effect(19, random2(9), random2(70), 100);
            break;

        default:
          nothing_hap:strcpy(info, "Nothing appears to happen.");
            mpr(info);
            break;
        }
        break;


    case OBJ_STAVES:
        if (you.inv_type[you.equip[EQ_WEAPON]] == STAFF_CHANNELING)
        {
            if (you.magic_points == you.max_magic_points || random2(4) == 0)
            {
                strcpy(info, "Nothing appears to happen.");
                mpr(info);
                break;
            }
            you.magic_points += 1 + random2(3);
            if (you.magic_points > you.max_magic_points)
                you.magic_points = you.max_magic_points;
            you.redraw_magic_points = 1;
            strcpy(info, "You channel some magical energy.");
            mpr(info);

            you.inv_ident[you.equip[EQ_WEAPON]] = 3;
            strcpy(info, "You are wielding ");
            in_name(you.equip[EQ_WEAPON], 3, str_pass);
            strcat(info, str_pass);
            strcat(info, ".");

            mpr(info);
            more();

            wield_change = 1;
            break;
        }
        staff_spell(you.equip[EQ_WEAPON]);
        break;

    case OBJ_MISCELLANY:        // misc

        switch (you.inv_type[you.equip[EQ_WEAPON]])
        {
        case MISC_BOTTLED_EFREET:
            efreet_flask();
            break;
        case MISC_CRYSTAL_BALL_OF_SEEING:
            ball_of_seeing();
            break;
        case MISC_AIR_ELEMENTAL_FAN:
            if (random2(2) == 0)
            {
                strcpy(info, "Nothing appears to happen.");
                mpr(info);
                break;
            }
            summon_elemental(100, MONS_AIR_ELEMENTAL, 4);
            break;
        case MISC_LAMP_OF_FIRE:
            if (random2(2) == 0)
            {
                strcpy(info, "Nothing appears to happen.");
                mpr(info);
                break;
            }
            summon_elemental(100, MONS_FIRE_ELEMENTAL, 4);
            break;
        case MISC_STONE_OF_EARTH_ELEMENTALS:
            if (random2(2) == 0)
            {
                strcpy(info, "Nothing appears to happen.");
                mpr(info);
                break;
            }
            summon_elemental(100, MONS_EARTH_ELEMENTAL, 4);
            break;
        case MISC_HORN_OF_GERYON:       // Horn of Geryon

            if (you.where_are_you == 3)
            {
                strcpy(info, "You produce a weird and mournful sound.");
                mpr(info);
                for (count_x = 0; count_x < 80; count_x++)
                {
                    for (count_y = 0; count_y < 80; count_y++)
                    {
                        if (grd[count_x][count_y] == 98)
                        {
                            opened_gates++;
                            switch (grd[count_x + 2][count_y])
                            {
                            case 67:
                                grd[count_x][count_y] = 92;
                                break;
                            case 61:
                                grd[count_x][count_y] = 93;
                                break;
                            case 1:
                                grd[count_x][count_y] = 95;
                                break;
                            case 62:
                                grd[count_x][count_y] = 94;
                                break;
                            }   // Note: This assumes that the Vestibule has not been substantially changed.

                        }
                    }
                }
                if (opened_gates != 0)
                {
                    strcpy(info, "Your way has been unbarred.");
                    mpr(info);
                }
                break;
            }
            else
            {
                strcpy(info, "You produce a hideous howling noise!");
                mpr(info);
                create_monster(MONS_BEAST, 23, 1, you.x_pos, you.y_pos, MHITYOU, 250);
            }
            break;

        case MISC_BOX_OF_BEASTS:        // box of beasts

            box_of_beasts();
            break;

        case MISC_DECK_OF_WONDERS:      // deck of wonders

            deck_of_cards(0);
            break;

        case MISC_DECK_OF_SUMMONINGS:   // deck of summonings

            deck_of_cards(1);
            break;

        case MISC_CRYSTAL_BALL_OF_ENERGY:       // crystal ball of energy

            ball_of_energy();
            break;

        case 12:                // crystal ball of fixation

            ball_of_fixation();
            break;

        case MISC_DISC_OF_STORMS:       // disc of storms

            disc_of_storms();
            break;

        case MISC_DECK_OF_TRICKS:       // deck of tricks

            deck_of_cards(2);
            break;

        case MISC_DECK_OF_POWER:        // deck of power

            deck_of_cards(3);
            break;

        case MISC_PORTABLE_ALTAR_OF_NEMELEX:    // card table

            if (you.where_are_you == 18)
            {
                mpr("Don't you think this level already has more than enough altars?");
                return;
            }
            if (grd[you.x_pos][you.y_pos] != 67)
            {
                mpr("You need a piece of empty floor to place this item.");
                break;
            }
            mpr("You unfold the altar and place it on the floor.");
            grd[you.x_pos][you.y_pos] = 190;
            unwield_item(you.equip[EQ_WEAPON]);
            you.inv_quantity[you.equip[EQ_WEAPON]] = 0;
            you.equip[EQ_WEAPON] = -1;
            you.num_inv_items--;
            break;

        default:
            strcpy(info, "Nothing appears to happen.");
            mpr(info);
            break;
        }
        break;


    default:
        strcpy(info, "Nothing appears to happen.");
        mpr(info);
        break;

    }

    you.turn_is_over = 1;

}                               // end invoke_wielded



void efreet_flask(void)
{

    strcpy(info, "You open the flask, and a huge efreet comes out.");
    mpr(info);

    unwield_item(you.equip[EQ_WEAPON]);
    you.inv_quantity[you.equip[EQ_WEAPON]] = 0;
    you.equip[EQ_WEAPON] = -1;
    you.num_inv_items--;

    if (random2(5) != 0)
    {
        strcpy(info, "\"Thank you for releasing me!\"");
        mpr(info);
        create_monster(MONS_EFREET, 24, 7, you.x_pos, you.y_pos, MHITNOT, 250);
        return;
    }

    strcpy(info, "It howls insanely!");
    mpr(info);
    create_monster(MONS_EFREET, 24, 1, you.x_pos, you.y_pos, MHITYOU, 250);

}                               // end efreet_flask


void ball_of_seeing(void)
{
    int use = 0;

    strcpy(info, "You gaze into the crystal ball.");
    mpr(info);
    use = random2(you.intel * 6);       //magic_ability(you.intel, you.intel));

    if (you.conf != 0)
        use = 0;
    if (use < 2)
    {
        strcpy(info, "Your head hurts!");
        mpr(info);
        if (you.intel > 3)
            you.intel--;
        you.redraw_intelligence = 1;
        return;
    }
    if (use < 5)
    {
        strcpy(info, "You feel power drain from you!");
        mpr(info);
        you.magic_points = 0;
        you.redraw_magic_points = 1;
        return;
    }
    if (use < 10)
    {
        strcpy(info, "You feel confused.");
        mpr(info);
        you.conf += 10 + random2(10);
        if (you.conf > 40)
            you.conf = 40;
        return;
    }
    if (use < 15 || random2(2) == 0 || you.level_type == 1 || you.level_type == 2)
    {
        strcpy(info, "You see nothing.");
        mpr(info);
        return;
    }

    strcpy(info, "You see a map of your surroundings!");
    mpr(info);
    magic_mapping(80, 95 + random2(10));

}                               // end ball_of_seeing()


void disc_of_storms()
{
    you.turn_is_over = 1;

    if ((random2(60) > 30 + you.skills[SK_AIR_MAGIC] && random2(3) != 0) || you.attribute[ATTR_SPEC_AIR] != 0)
    {
        strcpy(info, "Nothing appears to happen.");
        mpr(info);
        return;
    }

    if (random2(60) > 30 + you.skills[SK_AIR_MAGIC] && random2(3) != 0)
    {
        strcpy(info, "The disc glows for a moment, then fades.");
        mpr(info);
        return;
    }

    if (random2(60) > 30 + you.skills[SK_AIR_MAGIC] && random2(3) != 0)
    {
        strcpy(info, "Little bolts of electricity crackle over the disc.");
        mpr(info);
        return;
    }

    strcpy(info, "The disc erupts in an explosion of electricity!");
    mpr(info);

    char disc_count = 0;
    char disc_count2 = 0;
    unsigned char which_zap_thing = 0;

    disc_count2 = 2 + random2(4);

    do
    {

        switch (random2(3))
        {
        case 0:
            which_zap_thing = 14;
            break;              // lightning bolt

        case 1:
            which_zap_thing = 34;
            break;              // shock

        case 2:
            which_zap_thing = 35;
            break;              // orb of elec

        }
        struct bolt beam[1];

        beam[0].target_x = 0;
        beam[0].target_y = 0;
        beam[0].move_x = random2(13) - 6;
        beam[0].move_y = random2(13) - 6;
        beam[0].source_x = you.x_pos;
        beam[0].source_y = you.y_pos;
        zapping(which_zap_thing, 30 + random2(20), beam);
        disc_count++;

    }
    while (disc_count <= disc_count2);

}


void staff_spell(char zap_device_2)
{

    if (you.inv_plus[zap_device_2] == 64
                        || you.inv_type[zap_device_2] < STAFF_SMITING
                        || you.inv_type[zap_device_2] >= STAFF_AIR)
    {
        strcpy(info, "That staff has no spells in it.");
        mpr(info);
        return;
    }

    you.inv_ident[zap_device_2] = 3;
    wield_change = 1;
    read_book(zap_device_2);
    unsigned char specspell;
    int powc = magic_ability(player_mag_abil(), you.intel);

    int sc_read_1 = book_thing; // book_thing is got in read_book

    if (sc_read_1 < 65 || (sc_read_1 > 90 && sc_read_1 < 97) || sc_read_1 > 122)
    {
        strcpy(info, "What?");
        mpr(info);
        return;
    }

    int sc_read_2 = conv_lett(sc_read_1);

    if (sc_read_2 > 7)
    {
        goto whattt;
    }


    if (learn_a_spell(zap_device_2, sc_read_2) != 1)
    {
        goto whattt;
    }

    specspell = which_spell_in_book(you.inv_type[you.equip[EQ_WEAPON]] + 40, sc_read_2 + 1);

    if (you.magic_points < spell_value(specspell) || you.experience_level < spell_value(specspell))
    {
        strcpy(info, "Your brain hurts!");
        mpr(info);
        you.conf += 2 + random2(4);
        if (you.conf > 40)
            you.conf = 40;
        you.turn_is_over = 1;
        return;
    }

    // note that spell staves cannot have empty spaces in strungy before the last real spell.

    exercise_spell(specspell, 1, 1);
    your_spells(specspell, powc, 0);

    you.magic_points -= spell_value(specspell);
    you.redraw_magic_points = 1;

    //if (you.inv_type [you.equip [0]] != 10) // staff of smiting uses no food
    /*
       if (you.energy == 0)
       {
       spellh = spell_hunger(spell_value(specspell), specspell);
       spellh -= magic_ability(you.mag_abil, you.intel);
       if (spellh < 10) spellh = 10;
       you.hunger -= spellh;
       redraw_hunger = 1;
       how_hungered(spellh);
       }
     */
    you.turn_is_over = 1;

    return;

whattt:
    strcpy(info, "What?");
    mpr(info);

}                               // end staff_spell



void tome_of_power(char sc_read_2)
{

    int powc = 0;
    int spell_casted = 0;
    char keyin = 0;
    struct bolt beam[1];

    strcpy(info, "The book opens to a page covered in ");

    char str_pass[40];

    weird_writing(str_pass);
    strcat(info, str_pass);
    strcat(info, ".");
    mpr(info);

    strcpy(info, "Read it?");
    mpr(info);

question:
    keyin = get_ch();

    switch (keyin)
    {
    case 'y':
    case 'Y':
        goto read_it;

    case 'n':
    case 'N':
        you.turn_is_over = 1;
        return;

    default:
        goto question;
    }


read_it:
    you.inv_ident[sc_read_2] = 3;
    you.turn_is_over = 1;

    if (you.mutation[MUT_BLURRY_VISION] > 0 && random2(4) < you.mutation[MUT_BLURRY_VISION])
    {
        mpr("The page is too blurry for you to read.");
        you.turn_is_over = 1;
        return;
    }

    powc = magic_ability(player_mag_abil(), you.intel);

    strcpy(info, "You find yourself reciting the magical words!");
    mpr(info);

    switch (random2(50))
    {
    case 0:
    case 11:
    case 12:
        if (random2(5) == 0)
        {
            strcpy(info, "The book disappears in a mighty explosion!");
            mpr(info);
            you.inv_quantity[sc_read_2] = 0;
            you.num_inv_items--;
            if (sc_read_2 == you.equip[EQ_WEAPON])
            {
                unwield_item(sc_read_2);
                you.equip[EQ_WEAPON] = -1;
            }
            burden_change();
        }
        beam[0].type = 43;
        beam[0].damage = 115;
        beam[0].flavour = 2;    // <=- not sure about this

        beam[0].bx = you.x_pos;
        beam[0].by = you.y_pos;
        strcpy(beam[0].beam_name, "fiery explosion");
        beam[0].colour = RED;
        beam[0].thing_thrown = 1;       // your explosion, (not someone else's explosion)

        explosion(1, beam);
        return;

    case 1:
    case 14:
        strcpy(info, "A cloud of choking fumes pours from the book's pages!");
        mpr(info);
        big_cloud(4, you.x_pos, you.y_pos, 20);
        //if (random() % 3 == 0) return; else break;
        return;

    case 2:
    case 13:
        strcpy(info, "A cloud of freezing gas pours from the book's pages!");
        mpr(info);
        big_cloud(3, you.x_pos, you.y_pos, 20);
        //if (random() % 3 == 0) return; else break;
        return;

    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        strcpy(info, "A cloud of weird smoke pours from the book's pages!");
        mpr(info);
        big_cloud(5 + random2(3), you.x_pos, you.y_pos, 20);
        return;                 // else break;

    case 10:
        strcpy(info, "A horrible Thing appears!");
        mpr(info);
        strcpy(info, "It doesn't look too friendly.");
        mpr(info);
        create_monster(MONS_LARGE_ABOMINATION, 25, 1, you.x_pos, you.y_pos, MHITNOT, 250);
        return;

        // next is 14

    }

    viewwindow(1);

    switch (random2(23))
    {
    case 0:
        spell_casted = SPELL_MAGIC_DART;
        break;
    case 16:
    case 17:
    case 1:
        spell_casted = SPELL_FIREBALL;
        break;                  // fireball

    case 21:
    case 18:
    case 2:
        spell_casted = SPELL_BOLT_OF_FIRE;
        break;
    case 22:
    case 19:
    case 3:
        spell_casted = SPELL_BOLT_OF_COLD;
        break;
    case 20:
    case 4:
        spell_casted = SPELL_LIGHTNING_BOLT;
        break;
    case 5:
        spell_casted = SPELL_POLYMORPH_OTHER;
        break;
    case 6:
        spell_casted = SPELL_THROW_FLAME;
        break;
    case 7:
        spell_casted = SPELL_THROW_FROST;
        break;
    case 8:
        spell_casted = SPELL_MEPHITIC_CLOUD;
        break;
    case 9:
        spell_casted = SPELL_VENOM_BOLT;
        break;
    case 10:
        spell_casted = SPELL_BOLT_OF_DRAINING;
        break;
    case 11:
        spell_casted = SPELL_LEHUDIBS_CRYSTAL_SPEAR;
        break;
    case 12:
        spell_casted = SPELL_BOLT_OF_INACCURACY;
        break;
    case 13:
        spell_casted = SPELL_STICKY_FLAME;
        break;
    case 14:
        spell_casted = SPELL_CIGOTUVIS_DEGENERATION;
        break;
    case 15:
        spell_casted = SPELL_TELEPORT_SELF;
        break;
    }

    // also summon demons (when they're finished)

    your_spells(spell_casted, powc, 0);
    // note no exercise

}


void skill_manual(char sc_read_2)
{
    char skname[30];
    char keyin;

    strcpy(info, "This is a manual of ");
    strcat(info, skill_name(you.inv_plus[sc_read_2]));
    strcat(info, "!");
    mpr(info);

    strcpy(info, "Read it?");
    mpr(info);

question:
    keyin = get_ch();

    switch (keyin)
    {
    case 'y':
    case 'Y':
        goto read_it;

    case 'n':
    case 'N':
        you.turn_is_over = 1;
        return;

    default:
        goto question;
    }


read_it:
    you.inv_ident[sc_read_2] = 3;
    you.turn_is_over = 1;

    strcpy(skname, skill_name(you.inv_plus[sc_read_2]));

    strcpy(info, "You read about ");
    strcat(info, strlwr(skname));
    strcat(info, ".");
    mpr(info);

    exercise(you.inv_plus[sc_read_2], 100);

    if (random2(10) == 0)
    {
        strcpy(info, "The book crumbles into dust.");
        mpr(info);
        you.inv_quantity[sc_read_2] = 0;
        you.num_inv_items--;
        if (sc_read_2 == you.equip[EQ_WEAPON])
        {
            unwield_item(sc_read_2);
            you.equip[EQ_WEAPON] = -1;
        }
        burden_change();
    }
    else
    {
        strcpy(info, "The book looks increasingly old and worn.");
        mpr(info);
    }


}                               // end skill_manual



void box_of_beasts(void)
{

    int beasty = 0;

    you.turn_is_over = 1;

    strcpy(info, "You open the lid, ");

    if (random2(3) == 0)
    {
        strcat(info, "but nothing happens.");
        mpr(info);
        return;
    }

    if (random2(10) == 0)
    {
        strcat(info, "but nothing happens.");
        mpr(info);
        you.inv_type[you.equip[EQ_WEAPON]] = MISC_EMPTY_EBONY_CASKET;
        return;
    }

    switch (random2(11))
    {
    case 0:
        beasty = MONS_GIANT_BAT;
        break;                  // bat

    case 1:
        beasty = MONS_HOUND;
        break;                  // hound

    case 2:
        beasty = MONS_JACKAL;
        break;                  // jackal

    case 3:
        beasty = MONS_RAT;
        break;                  // rat

    case 4:
        beasty = MONS_ICE_BEAST;
        break;                  // ice beast

    case 5:
        beasty = MONS_SNAKE;
        break;                  // snake

    case 6:
        beasty = MONS_YAK;
        break;                  // yak

    case 7:
        beasty = MONS_BUTTERFLY;
        break;                  // butterfly

    case 8:
        beasty = MONS_HELL_HOUND;
        break;                  // hell hound

    case 9:
        beasty = MONS_BROWN_SNAKE;
        break;                  // brown snake

    case 10:
        beasty = MONS_GIANT_LIZARD;
        break;                  // giant lizard

    }

    create_monster(beasty, 21 + random2(4), 7, you.x_pos, you.y_pos, you.pet_target, 250);

    strcat(info, "and something leaps out!");
    mpr(info);


}


void ball_of_energy(void)
{

    int use = 0;
    int proportional = 0;

    strcpy(info, "You gaze into the crystal ball.");
    mpr(info);
    use = random2(you.intel * 6);
    if (you.conf != 0)
        use = 0;
    if (use < 2 || you.max_magic_points == 0)
    {
        strcpy(info, "Your head hurts!");
        mpr(info);
        if (you.intel > 3)
            you.intel--;
        you.redraw_intelligence = 1;
        return;
    }
    if (use < 4 || you.magic_points == you.max_magic_points)
    {
drain_away:
        strcpy(info, "You feel your power drain away!");
        mpr(info);
        you.magic_points = 0;
        you.redraw_magic_points = 1;
        return;
    }
    if (use < 6)
    {
        strcpy(info, "You feel confused.");
        mpr(info);
        you.conf += 10 + random2(10);
        if (you.conf > 40)
            you.conf = 40;
        return;
    }

    proportional = you.magic_points * 100;
    proportional /= you.max_magic_points;

    if (random2(20) + random2(20) + random2(20) + random2(10) > proportional || random2(25) == 0)
        goto drain_away;

    strcpy(info, "You are suffused with power!");
    mpr(info);
    you.magic_points += 12 + random2(12) + random2(12);
    if (you.magic_points > you.max_magic_points)
        you.magic_points = you.max_magic_points;
    you.redraw_magic_points = 1;
}


void ball_of_fixation(void)
{

    strcpy(info, "You gaze into the crystal ball.");
    mpr(info);
    strcpy(info, "You are mesmerised by a rainbow of scintillating colours!");
    mpr(info);

    you.paralysis = 100;
    you.slow = 100;

}

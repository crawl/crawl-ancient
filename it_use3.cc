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

#include "bang.h"
#include "decks.h"
#include "direct.h"
#include "effects.h"
#include "fight.h"
#include "it_use2.h"
#include "itemname.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "player.h"
#include "skills.h"
#include "skills2.h"
#include "spell.h"
#include "spells.h"
#include "spells1.h"
#include "spells2.h"
#include "spl-book.h"
#include "spl-util.h"
#include "stuff.h"
#include "view.h"
#include "wpn-misc.h"


extern int book_thing;       // defined in spells.cc
extern bool wield_change;    // defined in output.cc


static void ball_of_energy(void);
static void ball_of_fixation(void);
static void ball_of_seeing(void);
static void box_of_beasts(void);
static void disc_of_storms(void);
static void efreet_flask(void);
static void staff_spell(int zap_device_2);




void special_wielded( void )
{

    int temp_rand = 0;    // for probability determination {dlb}
    int old_plus = you.inv_plus[you.equip[EQ_WEAPON]];
    int old_plus2 = you.inv_plus2[you.equip[EQ_WEAPON]];
    char old_colour = you.inv_colour[you.equip[EQ_WEAPON]];
    bool makes_noise = (one_chance_in(20) && !silenced(you.x_pos, you.y_pos));

    switch ( you.special_wield )
    {
      case SPWLD_SING:
        if ( makes_noise )
        {
            strcpy(info, "The Singing Sword ");
            temp_rand = random2(32);
            strcat(info, (temp_rand ==  0) ? "hums a little tune." :
                         (temp_rand ==  1) ? "breaks into glorious song!" :
                         (temp_rand ==  2) ? "sings." :
                         (temp_rand ==  3) ? "sings loudly." :
                         (temp_rand ==  4) ? "chimes melodiously." :
                         (temp_rand ==  5) ? "makes a horrible noise." :
                         (temp_rand ==  6) ? "sings off-key." :
                         (temp_rand ==  7) ? "sings 'tra-la-la'." :
                         (temp_rand ==  8) ? "burbles away merrily." :
                         (temp_rand ==  9) ? "gurgles." :
                         (temp_rand == 10) ? "suddenly shrieks!" :
                         (temp_rand == 11) ? "cackles." :
                         (temp_rand == 12) ? "warbles." :
                         (temp_rand == 13) ? "chimes harmoniously." :
                         (temp_rand == 14) ? "makes beautiful music." :
                         (temp_rand == 15) ? "produces a loud orchestral chord." :
                         (temp_rand == 16) ? "whines plaintively." :
                         (temp_rand == 17) ? "tinkles." :
                         (temp_rand == 18) ? "rings like a bell." :
                         (temp_rand == 19) ? "wails mournfully." :
                         (temp_rand == 20) ? "practices its scales." :
                         (temp_rand == 21) ? "lilts tunefully." :
                         (temp_rand == 22) ? "hums tunelessly." :
                         (temp_rand == 23) ? "sighs." :
                         (temp_rand == 24) ? "makes a deep moaning sound." :
                         (temp_rand == 25) ? "makes a popping sound." :
                         (temp_rand == 26) ? "sings a sudden stoccato note." :
                         (temp_rand == 27) ? "says 'Hi! I'm the Singing Sword!'." :
                         (temp_rand == 28) ? "whispers something." :
                         (temp_rand == 29) ? "speaks gibberish." :
                         (temp_rand == 30) ? "raves incoherently."
                                           : "yells in some weird language." );
            mpr(info);
        }
        break;

      case SPWLD_CURSE:
        makes_noise = false;

        if ( one_chance_in(30) )
          curse_an_item(0, 0);
        break;

      case SPWLD_VARIABLE:
        makes_noise = false;

        if ( you.inv_plus[you.equip[EQ_WEAPON]] > 100 )
          you.inv_plus[you.equip[EQ_WEAPON]] -= 100;

        if ( random2(5) < 2 )    // 40% chance {dlb}
          coinflip() ? you.inv_plus[you.equip[EQ_WEAPON]]++ : you.inv_plus[you.equip[EQ_WEAPON]]--;

        if ( random2(5) < 2 )    // 40% chance {dlb}
          coinflip() ? you.inv_plus2[you.equip[EQ_WEAPON]]++ : you.inv_plus2[you.equip[EQ_WEAPON]]--;

        if ( you.inv_plus[you.equip[EQ_WEAPON]] < 46 )
          you.inv_plus[you.equip[EQ_WEAPON]] = 46;
        else if ( you.inv_plus[you.equip[EQ_WEAPON]] > 57 )
          you.inv_plus[you.equip[EQ_WEAPON]] = 57;

        if ( you.inv_plus2[you.equip[EQ_WEAPON]] < 46 )
          you.inv_plus2[you.equip[EQ_WEAPON]] = 46;
        else if ( you.inv_plus2[you.equip[EQ_WEAPON]] > 57 )
          you.inv_plus2[you.equip[EQ_WEAPON]] = 57;

        you.inv_colour[you.equip[EQ_WEAPON]] = random_colour();
        break;

      //case SPWLD_PRUNE

      case SPWLD_TORMENT:
        makes_noise = false;

        if ( one_chance_in(200) )
          torment(you.x_pos, you.y_pos);
        break;

      case SPWLD_ZONGULDROK:
        makes_noise = false;

        if ( one_chance_in(5) )
          animate_dead(1 + random2(3), BEH_CHASING_I, MHITYOU, 1);
        break;

      case SPWLD_POWER:
        makes_noise = false;

    // someone placed cap on effect to weaken it {dlb}:
        you.inv_plus[you.equip[EQ_WEAPON]] = 47 + (you.hp / 13);

        if ( you.inv_plus[you.equip[EQ_WEAPON]] > 70 )
          you.inv_plus[you.equip[EQ_WEAPON]] = 70;

        you.inv_plus2[you.equip[EQ_WEAPON]] = you.inv_plus[you.equip[EQ_WEAPON]];
        break;

      case SPWLD_WUCAD_MU:
        makes_noise = false;

        you.inv_plus[you.equip[EQ_WEAPON]] = ( (you.intel > 25) ? 72 : 47 + you.intel ) ;
        you.inv_plus2[you.equip[EQ_WEAPON]] = ( (you.intel > 25) ? 63 : 50 + (you.intel / 2) );
        break;

      case SPWLD_SHADOW:
        makes_noise = false;

        if ( random2(8) <= player_spec_death() )
          create_monster(MONS_SHADOW, 21, BEH_ENSLAVED, you.x_pos, you.y_pos, MHITNOT, 250);
        //naughty(NAUGHTY_ATTACK_FRIEND,1);
        show_green = DARKGREY;
        break;

      case SPWLD_HUM:
        if ( makes_noise )
        {
            in_name(you.equip[EQ_WEAPON], 4, str_pass);
            strcpy(info, str_pass);
            strcat(info, " lets out a weird humming sound.");
            mpr(info);
        }
        break;     // to noisy() call at foot 2apr2000 {dlb}

      case SPWLD_CHIME:
        if ( makes_noise )
        {
            in_name(you.equip[EQ_WEAPON], 4, str_pass);
            strcpy(info, str_pass);
            strcat(info, " chimes like a gong.");
            mpr(info);
        }
        break;

      case SPWLD_BECKON:
        if ( makes_noise )
          mpr("You hear a voice call your name.");
        break;

      case SPWLD_SHOUT:
        if ( makes_noise )
          mpr("You hear a shout.");
        break;

      default:
        return;
    }

    if ( makes_noise )
      noisy(25, you.x_pos, you.y_pos);

    if ( old_plus != you.inv_plus[you.equip[EQ_WEAPON]]
           || old_plus2 != you.inv_plus2[you.equip[EQ_WEAPON]]
           || you.inv_colour[you.equip[EQ_WEAPON]] != old_colour )
      wield_change = true;

    return;

}          // end special_wielded()




static void reaching_weapon_attack( void )
{

    struct dist beam;
    int x_distance, y_distance;
    int x_middle, y_middle;
    int skill;

    mpr("Attack whom?");

    direction(100, &beam);

    x_distance = abs(beam.target_x - you.x_pos);
    y_distance = abs(beam.target_y - you.y_pos);

    if ( x_distance == 0 && y_distance == 0 )
      canned_msg(MSG_UNTHINKING_ACT);
    else if ( x_distance > 2 || y_distance > 2 )
      mpr("Your weapon cannot reach that far!");
    else if ( beam.nothing == -1 || mgrd[beam.target_x][beam.target_y] == MNG
               || (beam.target_x == you.x_pos && beam.target_y == you.y_pos) )
      mpr("You attack empty space.");
    else
    {
        /* BCR - Added a check for monsters in the way.  Only checks cardinal
         *       directions.  Knight moves are ignored.  Assume the weapon
         *       slips between the squares.
         */

      // if we're attacking more than a space away
        if ( (x_distance > 1) || (y_distance > 1) )
        {

#define MAX(x,y) (((x) > (y)) ? (x) : (y))
            x_middle = MAX(beam.target_x, you.x_pos) - (x_distance / 2);
            y_middle = MAX(beam.target_y, you.y_pos) - (y_distance / 2);
#undef MAX
        // if either the x or the y is the same, we should check for a monster:
            if ( ( (beam.target_x == you.x_pos) || (beam.target_y == you.y_pos) )
                 && (mgrd[x_middle][y_middle] != MNG) )
            {
                skill = weapon_skill(you.inv_class[you.equip[EQ_WEAPON]], you.inv_type[you.equip[EQ_WEAPON]]);

                if ( (5 + (3 * skill)) > random2(100) )
                {
                    mpr("You reach to attack!");
                    you_attack(mgrd[beam.target_x][beam.target_y], false);
                }
                else
                {
                    mpr("You could not reach far enough!");
                    you_attack(mgrd[x_middle][y_middle], false);
                }
            }
            else
            {
                mpr("You reach to attack!");
                you_attack(mgrd[beam.target_x][beam.target_y], false);
            }
        }
        else
          you_attack(mgrd[beam.target_x][beam.target_y], false);
    }

    return;

}          // end reaching_weapon_attack()




void invoke_wielded( void )
{

    char opened_gates = 0;
    unsigned char spell_casted = random2(21);
    int count_x, count_y;
    int temp_rand = 0;     // for probability determination {dlb}

    if ( you.berserker )
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }
    else if ( you.equip[EQ_WEAPON] == -1 )
    {
        mpr("You aren't wielding anything!");
        return;
    }

    switch ( you.inv_class[you.equip[EQ_WEAPON]] )
    {
      case OBJ_WEAPONS:
        if ( you.inv_dam[you.equip[EQ_WEAPON]] < NWPN_SINGING_SWORD && you.inv_dam[you.equip[EQ_WEAPON]] % 30 == SPWPN_REACHING )
          reaching_weapon_attack();
        else
        {
            switch (you.inv_dam[you.equip[EQ_WEAPON]])
            {
              case NWPN_STAFF_OF_DISPATER:
                if ( you.deaths_door || !enough_hp(11, true) || !enough_mp(5, true) )
                  goto nothing_hap;

                mpr("You feel the staff feeding on your energy!");

                dec_hp(5 + random2avg(19,2), false);
                dec_mp( 2 + random2avg(5,2) );

                your_spells(SPELL_HELLFIRE, 100, false);    // power (2nd number) is meaningless
                break;

          // let me count the number of ways spell_casted is
          // used here ... one .. two .. three ... >CRUNCH<
          // three licks to get to the center of a ... {dlb}
              case NWPN_SCEPTRE_OF_ASMODEUS:
                spell_casted = random2(21);

                if ( spell_casted == 0 )
                  goto nothing_hap;

                if ( spell_casted < 2 )   // summon devils, maybe a Fiend
                {
                    spell_casted = ( one_chance_in(4) ? MONS_FIEND : MONS_HELLION + random2(10) );

                    bool good_summon = ( create_monster(spell_casted, 25, BEH_CHASING_I, you.x_pos, you.y_pos, MHITNOT, 250) != -1 );

                    if ( good_summon )
                    {
                        if ( spell_casted == MONS_FIEND )
                        {
                            set_colour(RED);
                            mpr("\"Your arrogance condemns you, mortal!\"");
                        }
                        else
                          mpr("The Sceptre summons one of its servants.");
                    }

                    break;
                }

                temp_rand = random2(240);

                if ( temp_rand > 125 )
                  spell_casted = SPELL_BOLT_OF_FIRE; // 114 in 240 chance {dlb}
                else if ( temp_rand > 68 )
                  spell_casted = SPELL_LIGHTNING_BOLT; //57 in 240 chance {dlb}
                else if ( temp_rand > 11 )
                  spell_casted = SPELL_BOLT_OF_DRAINING;//57 in 240 chance{dlb}
                else
                  spell_casted = SPELL_HELLFIRE;     //  12 in 240 chance {dlb}

                your_spells(spell_casted, 10, false);
                break;

              case NWPN_STAFF_OF_OLGREB:
                if ( !enough_mp(4, true) || you.skills[SK_SPELLCASTING] <= random2(11) )
                  goto nothing_hap;

                dec_mp(4);

                your_spells(SPELL_OLGREBS_TOXIC_RADIANCE, 100, false);
                your_spells(SPELL_VENOM_BOLT, 100, false);
                break;

              case NWPN_STAFF_OF_WUCAD_MU:
                if ( you.magic_points == you.max_magic_points || one_chance_in(4) )
                  goto nothing_hap;

                mpr("Magical energy flows into your mind!");

                inc_mp(3 + random2(5), false);

                if (one_chance_in(3))
                  miscast_effect(SPTYP_DIVINATION, random2(9), random2(70), 100);
                break;

            default:
            nothing_hap:
                canned_msg(MSG_NOTHING_HAPPENS);
                break;
            }
          }
        break;

      case OBJ_STAVES:
        switch( you.inv_type[you.equip[EQ_WEAPON]] )
          {
          case STAFF_CHANNELING:
            if ( you.magic_points == you.max_magic_points || one_chance_in(4) )
              canned_msg(MSG_NOTHING_HAPPENS);
            else
              {
                mpr("You channel some magical energy.");
                inc_mp(1 + random2(3), false);

                if ( you.inv_ident[you.equip[EQ_WEAPON]] < 3 )
                {
                    you.inv_ident[you.equip[EQ_WEAPON]] = 3;
                    strcpy(info, "You are wielding ");
                    in_name(you.equip[EQ_WEAPON], 3, str_pass);
                    strcat(info, str_pass);
                    strcat(info, ".");

                    mpr(info);
                    more();

                    wield_change = true;
                }
              }
            break;

          case STAFF_SMITING:
            if (enough_mp(4,true)) {
              dec_mp(4);
              your_spells( SPELL_SMITING, 5 + you.experience_level, false );

              if ( you.inv_ident[you.equip[EQ_WEAPON]] < 3 )
                {
                  you.inv_ident[you.equip[EQ_WEAPON]] = 3;
                  strcpy(info, "You are wielding ");
                  in_name(you.equip[EQ_WEAPON], 3, str_pass);
                  strcat(info, str_pass);
                  strcat(info, ".");

                  mpr(info);
                  more();

                  wield_change = true;
                }
            }
            else
              miscast_effect(SPTYP_CONJURATION, random2(5), random2(50), 100);
            break;

          default:
            staff_spell(you.equip[EQ_WEAPON]);
            break;
          }
        break;

      case OBJ_MISCELLANY:
        switch (you.inv_type[you.equip[EQ_WEAPON]])
        {
          case MISC_BOTTLED_EFREET:
            efreet_flask();
            break;
          case MISC_CRYSTAL_BALL_OF_SEEING:
            ball_of_seeing();
            break;
          case MISC_AIR_ELEMENTAL_FAN:
            if ( coinflip() )
              canned_msg(MSG_NOTHING_HAPPENS);
            else
              summon_elemental(100, MONS_AIR_ELEMENTAL, 4);
            break;
          case MISC_LAMP_OF_FIRE:
            if ( coinflip() )
              canned_msg(MSG_NOTHING_HAPPENS);
            else
              summon_elemental(100, MONS_FIRE_ELEMENTAL, 4);
            break;
          case MISC_STONE_OF_EARTH_ELEMENTALS:
            if ( coinflip() )
              canned_msg(MSG_NOTHING_HAPPENS);
            else
              summon_elemental(100, MONS_EARTH_ELEMENTAL, 4);
            break;
          case MISC_HORN_OF_GERYON:
            if ( you.where_are_you == BRANCH_VESTIBULE_OF_HELL )
            {
                mpr("You produce a weird and mournful sound.");

                for (count_x = 0; count_x < GXM; count_x++)
                  for (count_y = 0; count_y < GYM; count_y++)    // was "count_y < 80" but that is WRONG {dlb}
                  {
                      if ( grd[count_x][count_y] == DNGN_STONE_ARCH )
                      {
                          opened_gates++;

                          switch ( grd[count_x + 2][count_y] )    // this may generate faulty [][] values {dlb}
                          {
                              case DNGN_FLOOR:
                                grd[count_x][count_y] = DNGN_ENTER_DIS;
                                break;
                              case DNGN_LAVA:
                                grd[count_x][count_y] = DNGN_ENTER_GEHENNA;
                                break;
                              case DNGN_ROCK_WALL:
                                grd[count_x][count_y] = DNGN_ENTER_TARTARUS;
                                break;
                              case DNGN_DEEP_WATER:
                                grd[count_x][count_y] = DNGN_ENTER_COCYTUS;
                                break;
                          }   // Note: This assumes that the Vestibule has not been substantially changed.
                      }
                  }

                if ( opened_gates )
                  mpr("Your way has been unbarred.");
            }
            else
            {
                mpr("You produce a hideous howling noise!");
                create_monster(MONS_BEAST, 23, BEH_CHASING_I, you.x_pos, you.y_pos, MHITYOU, 250);
            }
            break;

          case MISC_DECK_OF_WONDERS:
            deck_of_cards(DECK_OF_WONDERS);
            break;

          case MISC_DECK_OF_SUMMONINGS:
            deck_of_cards(DECK_OF_SUMMONING);
            break;

          case MISC_DECK_OF_TRICKS:
            deck_of_cards(DECK_OF_TRICKS);
            break;

          case MISC_DECK_OF_POWER:
            deck_of_cards(DECK_OF_POWER);
            break;

          case MISC_BOX_OF_BEASTS:
            box_of_beasts();
            break;

          case MISC_CRYSTAL_BALL_OF_ENERGY:
            ball_of_energy();
            break;

          case MISC_CRYSTAL_BALL_OF_FIXATION:
            ball_of_fixation();
            break;

          case MISC_DISC_OF_STORMS:
            disc_of_storms();
            break;

          case MISC_PORTABLE_ALTAR_OF_NEMELEX:
            if ( you.where_are_you == BRANCH_ECUMENICAL_TEMPLE )
              mpr("Don't you think this level already has more than enough altars?");
            else if ( grd[you.x_pos][you.y_pos] != DNGN_FLOOR )
              mpr("You need a clear area to place this item.");
            else
            {
                mpr("You unfold the altar and place it on the floor.");
                grd[you.x_pos][you.y_pos] = DNGN_ALTAR_NEMELEX_XOBEH;
                unwield_item(you.equip[EQ_WEAPON]);
                you.inv_quantity[you.equip[EQ_WEAPON]] = 0;
                you.equip[EQ_WEAPON] = -1;
                you.num_inv_items--;
            }
            break;

          default:
            canned_msg(MSG_NOTHING_HAPPENS);
            break;
        }
        break;

      default:
        canned_msg(MSG_NOTHING_HAPPENS);
        break;
    }

    you.turn_is_over = 1;

}          // end invoke_wielded()




static void efreet_flask( void )
{

    int behavior = ( !one_chance_in(5) ? BEH_ENSLAVED : BEH_CHASING_I );

    mpr("You open the flask...");

    unwield_item(you.equip[EQ_WEAPON]);
    you.inv_quantity[you.equip[EQ_WEAPON]] = 0;
    you.equip[EQ_WEAPON] = -1;
    you.num_inv_items--;

    if ( create_monster(MONS_EFREET, 24, behavior, you.x_pos, you.y_pos, MHITYOU, 250) != -1 )
    {
        mpr("...and a huge efreet comes out.");

        if ( behavior == BEH_ENSLAVED )
          set_colour(RED);

        mpr( (behavior == BEH_ENSLAVED) ? "\"Thank you for releasing me!\"" : "It howls insanely!" );
    }
    else
      canned_msg(MSG_NOTHING_HAPPENS);

    return;

}          // end efreet_flask()




static void ball_of_seeing( void )
{

    int use = 0;

    mpr("You gaze into the crystal ball.");

    use = ( (!you.conf) ? random2(you.intel * 6) : 0 );

    if ( use < 2 )
    {
        lose_stat(STAT_INTELLIGENCE, 1);
    }
    else if ( use < 5 && enough_mp(1,true) )
    {
        mpr("You feel power drain from you!");
        set_mp(0, false);
    }
    else if ( use < 10 )
    {
        mpr("You feel confused.");
        if ( (you.conf += 10 + random2(10)) > 40 )
          you.conf = 40;
    }
    else if ( use < 15
               || you.level_type == LEVEL_LABYRINTH
               || you.level_type == LEVEL_ABYSS
               || coinflip() )
    {
        mpr("You see nothing.");
    }
    else
    {
        mpr("You see a map of your surroundings!");
        magic_mapping(80, 95 + random2(10));
    }

    return;

}          // end ball_of_seeing()




static void disc_of_storms( void )
{

    int temp_rand = 0;    // probability determination {dlb}
    struct bolt beam[1];
    int disc_count = 0;
    int fail_rate = (28 - you.skills[SK_AIR_MAGIC]);
    unsigned char which_zap = 0;

    if ( you.attribute[ATTR_SPEC_AIR] || ( random2(90) < fail_rate ) )
      canned_msg(MSG_NOTHING_HAPPENS);
    else if ( random2(90) < fail_rate )
      mpr("The disc glows for a moment, then fades.");
    else if ( random2(90) < fail_rate )
      mpr("Little bolts of electricity crackle over the disc.");
    else
    {
        mpr("The disc erupts in an explosion of electricity!");

        disc_count = 2 + random2(4);

        while ( disc_count )
        {
            temp_rand = random2(3);

            which_zap = ( (temp_rand > 1) ? ZAP_LIGHTNING :
                          (temp_rand > 0) ? ZAP_ELECTRICITY
                                          : ZAP_ORB_OF_ELECTRICITY );

            beam[0].target_x = 0;
            beam[0].target_y = 0;
            beam[0].move_x = random2(13) - 6;
            beam[0].move_y = random2(13) - 6;
            beam[0].source_x = you.x_pos;
            beam[0].source_y = you.y_pos;
            zapping(which_zap, 30 + random2(20), &beam[0]);

            disc_count--;
        }
    }

    return;

}          // end disc_of_storms()




static void staff_spell( int zap_device_2 )
{

    int sc_read_1, sc_read_2;

    if ( you.inv_plus[zap_device_2] == 64
          || you.inv_type[zap_device_2] < STAFF_SMITING
          || you.inv_type[zap_device_2] >= STAFF_AIR )
    {
      //mpr("That staff has no spells in it.");
      canned_msg(MSG_NOTHING_HAPPENS);
      return;
    }

    you.inv_ident[zap_device_2] = 3;
    wield_change = true;
    read_book(zap_device_2);
    unsigned char specspell;
    int powc = 1 + player_mag_abil(true) + you.experience_level;

    sc_read_1 = book_thing; // book_thing is got in read_book

    if ( sc_read_1 < 'A' || ( sc_read_1 > 'Z' && sc_read_1 < 'a' )
         || sc_read_1 > 'z' )
      goto whattt;

    sc_read_2 = letter_to_index(sc_read_1);

    if (sc_read_2 > SPELLBOOK_SIZE)
      goto whattt;

    if ( !learn_a_spell(zap_device_2, sc_read_2) )
      goto whattt;

    specspell = which_spell_in_book(you.inv_type[you.equip[EQ_WEAPON]] + 40,
                                    1 + sc_read_2);

    if ( specspell == SPELL_NO_SPELL )
      goto whattt;

    if ( you.magic_points < spell_mana(specspell) || you.experience_level < spell_difficulty(specspell) )
    {
        mpr("Your brain hurts!");

        you.conf += 2 + random2(4);

        if (you.conf > 40)
          you.conf = 40;

        you.turn_is_over = 1;
        return;
    }

    // note that spell staves cannot have empty spaces in strungy before the last real spell.

    exercise_spell(specspell, true, true);
    your_spells(specspell, powc, false);

    dec_mp(spell_mana(specspell));

    you.turn_is_over = 1;
    return;

whattt:
    mpr("What?");

    return;

}                               // end staff_spell()




void tome_of_power( char sc_read_2 )
{

    int temp_rand = 0;                  // probability determination {dlb}

    int powc = player_mag_abil(true);
    int spell_casted = 0;
    struct bolt beam[1];
    char str_pass[40];


    strcpy(info, "The book opens to a page covered in ");

    weird_writing(str_pass);
    strcat(info, str_pass);
    strcat(info, ".");
    mpr(info);

    you.turn_is_over = 1;

    if ( !yesno("Read it?") )
      return;

    you.inv_ident[sc_read_2] = 3;

    if (you.mutation[MUT_BLURRY_VISION] > 0 && random2(4) < you.mutation[MUT_BLURRY_VISION])
    {
        mpr("The page is too blurry for you to read.");
        return;
    }

    mpr("You find yourself reciting the magical words!");

    switch (random2(50))
    {
    case 0:
    case 11:
    case 12:
        if (one_chance_in(5))
        {
            mpr("The book disappears in a mighty explosion!");

            you.inv_quantity[sc_read_2] = 0;
            you.num_inv_items--;

            if (sc_read_2 == you.equip[EQ_WEAPON])
            {
                unwield_item(sc_read_2);
                you.equip[EQ_WEAPON] = -1;
            }

            burden_change();
        }

        beam[0].type = SYM_BURST;
        beam[0].damage = 115;
        beam[0].flavour = BEAM_FIRE;    // unsure about this    // BEAM_EXPLOSION instead? [dlb]
        beam[0].bx = you.x_pos;
        beam[0].by = you.y_pos;
        strcpy(beam[0].beam_name, "fiery explosion");
        beam[0].colour = RED;
        beam[0].thing_thrown = KILL_YOU;       // your explosion, (not someone else's explosion)

        explosion(true, &beam[0]);
        return;

    case 1:
    case 14:
        mpr("A cloud of choking fumes pours from the book's pages!");
        big_cloud(CLOUD_POISON, you.x_pos, you.y_pos, 20, 7 + random2(5));
        return;

    case 2:
    case 13:
        mpr("A cloud of freezing gas pours from the book's pages!");
        big_cloud(CLOUD_COLD, you.x_pos, you.y_pos, 20, 8 + random2(5));
        return;

    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        mpr("A cloud of weird smoke pours from the book's pages!");
        big_cloud(CLOUD_GREY_SMOKE + random2(3), you.x_pos, you.y_pos, 20, 10 + random2(8));
        return;

    case 10:
        if ( create_monster(MONS_ABOMINATION_SMALL, 25, BEH_CHASING_I, you.x_pos, you.y_pos, MHITNOT, 250) != -1 )
        {
            mpr("A horrible Thing appears!");
            mpr("It doesn't look too friendly.");
        }
        return;
    }

    viewwindow(1, false);

    temp_rand = random2(23);

    spell_casted = ( (temp_rand > 19) ? SPELL_FIREBALL :                  // 3 in 23 {dlb}
                     (temp_rand > 16) ? SPELL_BOLT_OF_FIRE :              // 3 in 23 {dlb}
                     (temp_rand > 13) ? SPELL_BOLT_OF_COLD :              // 3 in 23 {dlb}
                     (temp_rand > 11) ? SPELL_LIGHTNING_BOLT :            // 2 in 23 {dlb}
                     (temp_rand > 10) ? SPELL_MAGIC_DART :                // 1 in 23 {dlb}
                     (temp_rand >  9) ? SPELL_POLYMORPH_OTHER :           // 1 in 23 {dlb}
                     (temp_rand >  8) ? SPELL_THROW_FLAME :               // 1 in 23 {dlb}
                     (temp_rand >  7) ? SPELL_THROW_FROST :               // 1 in 23 {dlb}
                     (temp_rand >  6) ? SPELL_MEPHITIC_CLOUD :            // 1 in 23 {dlb}
                     (temp_rand >  5) ? SPELL_VENOM_BOLT :                // 1 in 23 {dlb}
                     (temp_rand >  4) ? SPELL_BOLT_OF_DRAINING :          // 1 in 23 {dlb}
                     (temp_rand >  3) ? SPELL_LEHUDIBS_CRYSTAL_SPEAR :    // 1 in 23 {dlb}
                     (temp_rand >  2) ? SPELL_BOLT_OF_INACCURACY :        // 1 in 23 {dlb}
                     (temp_rand >  1) ? SPELL_STICKY_FLAME :              // 1 in 23 {dlb}
                     (temp_rand >  0) ? SPELL_CIGOTUVIS_DEGENERATION      // 1 in 23 {dlb}
                                      : SPELL_TELEPORT_SELF );            // 1 in 23 {dlb}

// note: no exercise!!!
    your_spells(spell_casted, powc, false);

}          // end tome_of_power()




void skill_manual( char sc_read_2 )
{

    char skname[30];

    you.inv_ident[sc_read_2] = 3;
    strcpy(info, "This is a manual of ");
    strcat(info, skill_name(you.inv_plus[sc_read_2]));
    strcat(info, "!");
    mpr(info);

    you.turn_is_over = 1;

    if ( !yesno("Read it?") )
      return;

    you.inv_ident[sc_read_2] = 3;

    strcpy(skname, skill_name(you.inv_plus[sc_read_2]));
    strcpy(info, "You read about ");
    strcat(info, strlwr(skname));
    strcat(info, ".");
    mpr(info);

    exercise(you.inv_plus[sc_read_2], 500);

    if ( one_chance_in(10) )
    {
        mpr("The book crumbles into dust.");

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
      mpr("The book looks somewhat more worn.");

}          // end skill_manual()




static void box_of_beasts( void )
{

    int beasty = MONS_PROGRAM_BUG;    // error trapping {dlb}
    int temp_rand = 0;                // probability determination {dlb}

    mpr("You open the lid...");

    if ( random2(5) < 2 )    // 40% chance {dlb}
    {
        mpr("...but nothing happens.");

        if ( one_chance_in(6) )
          you.inv_type[you.equip[EQ_WEAPON]] = MISC_EMPTY_EBONY_CASKET;
    }
    else                    // 60% chance {dlb}
    {
        temp_rand = random2(11);

        beasty = ( (temp_rand ==  0) ? MONS_GIANT_BAT :
                   (temp_rand ==  1) ? MONS_HOUND :
                   (temp_rand ==  2) ? MONS_JACKAL :
                   (temp_rand ==  3) ? MONS_RAT :
                   (temp_rand ==  4) ? MONS_ICE_BEAST :
                   (temp_rand ==  5) ? MONS_SNAKE :
                   (temp_rand ==  6) ? MONS_YAK :
                   (temp_rand ==  7) ? MONS_BUTTERFLY :
                   (temp_rand ==  8) ? MONS_HELL_HOUND :
                   (temp_rand ==  9) ? MONS_BROWN_SNAKE
                                     : MONS_GIANT_LIZARD );

        if ( create_monster(beasty, 21 + random2(4), BEH_ENSLAVED, you.x_pos, you.y_pos, you.pet_target, 250) != -1 )
          mpr("...and something leaps out!");
    }

    return;

}          // end box_of_beasts()




static void ball_of_energy( void )
{

    int use = 0;
    int proportional = 0;

    mpr("You gaze into the crystal ball.");

    use = ( (!you.conf) ? random2(you.intel * 6) : 0 );

    if ( use < 2 || you.max_magic_points == 0 )
    {
        lose_stat(STAT_INTELLIGENCE, 1);
    }
    else if ( ( use < 4 && enough_mp(1, true) ) || you.magic_points == you.max_magic_points )
    {
        mpr("You feel your power drain away!");
        set_mp(0,false);
    }
    else if ( use < 6 )
    {
        mpr("You feel confused.");

        if ( (you.conf += 10 + random2(10)) > 40 )
          you.conf = 40;
    }
    else
    {
        proportional = you.magic_points * 100;
        proportional /= you.max_magic_points;

        if ( random2avg(67,4) > proportional || one_chance_in(25) )
        {
            mpr("You feel your power drain away!");
            set_mp(0,false);
        }
        else
        {
            mpr("You are suffused with power!");
            inc_mp(12 + random2avg(23,2), false);
        }
    }

    return;

}          // end ball_of_energy()




static void ball_of_fixation( void )
{

    mpr("You gaze into the crystal ball.");
    mpr("You are mesmerised by a rainbow of scintillating colours!");

    you.paralysis = 100;
    you.slow = 100;

}          // end ball_of_fixation()

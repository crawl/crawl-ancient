/*
 *  File:       spell.cc
 *  Summary:    Spell casting functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <4>      1/02/00        jmf             changed values, marked //jmf:
 *      <3>      6/13/99        BWR             Added Staff auto identify code
 *      <2>      5/20/99        BWR             Added some screen redraws
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "spell.h"

#include <stdlib.h>
#include <string.h>

#include "externs.h"

#include "beam.h"
#include "effects.h"
#include "fight.h"
#include "food.h"
#include "it_use2.h"
#include "itemname.h"
#include "monplace.h"
#include "mutation.h"
#include "player.h"
#include "religion.h"
#include "skills.h"
#include "spells.h"
#include "spells0.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "spells4.h"
#include "spl-util.h"
#include "stuff.h"
#include "transfor.h"
#include "view.h"


extern bool wield_change;    // defined in output.cc


static void surge_power( int spell )
{
    int enhanced = 0;

    //jmf: simplified
    enhanced += spell_enhancement( spell_type( spell ) );

    if ( enhanced )    // one way or the other {dlb}
    {
        strcpy(info, "You feel a");

        strcat(info, (enhanced  < -2) ? "n extraordinarily" :
                     (enhanced == -2) ? "n extremely" :
                     (enhanced ==  2) ? " strong" :
                     (enhanced  >  2) ? " huge"
                                      : "" );

        strcat(info, (enhanced < 0) ? " numb sensation."
                                    : " surge of power!" );

        mpr(info);
    }
}          // end surge_power()


void cast_a_spell( void )
{
    char spc = 0;
    char spc2 = 0;
    int spellh = 0;
    int powc = 0;
    unsigned char keyin = 0;
    char unthing = 0;


    if ( !you.spell_no )
    {
        mpr("You don't know any spells.");
        return;
    }

    if ( you.berserker )
    {
        canned_msg(MSG_TOO_BERSERK);
        return;
    }

// first query {dlb}:
    for (;;)
    {
      //jmf: FIXME: change to reflect range of known spells
        mpr("Cast which spell? [a-y] spell [?|*] list");

        keyin = get_ch();

        if ( keyin == '?' || keyin == '*' )
        {
            unthing = spell_list();

#ifdef PLAIN_TERM
            redraw_screen();
#endif

            if (unthing == 2)
              return;

            if ( unthing >= 'a' && unthing <= 'y' )
            {
                keyin = unthing;
                break;
            }
            else
              mesclr();
        }
        else
          break;
    }


    spc = (int) keyin;

    if ( spc < 'a' || spc > 'y' )
    {
        mpr("You don't know that spell.");
        return;
    }

    spc2 = letter_to_index(spc);

    if ( you.spells[spc2] == SPELL_NO_SPELL )
    {
        mpr("You don't know that spell.");
        return;
    }

    if ( spell_mana(you.spells[spc2]) > you.magic_points )
    {
        mpr("You don't have enough magic to cast that spell.");
        return;
    }

    if ( you.hunger_state < HS_HUNGRY  || you.hunger <= spell_hunger(you.spells[spc2]) )
    {
        mpr("You don't have the energy to cast that spell.");
        return;
    }

    dec_mp(spell_mana(you.spells[spc2]));

    if ( !player_energy() && you.is_undead != US_UNDEAD )
    {
        spellh = spell_hunger(you.spells[spc2]);
        spellh -= you.intel * you.skills[SK_SPELLCASTING];    // I wonder if a better algorithm is called for? {dlb}

        if ( spellh < 1 )
          spellh = 1;
        else
          make_hungry(spellh, true);
    }

    you.turn_is_over = 1;
    alert();

    if ( you.conf )
    {
        random_uselessness(2 + random2(7), 0);
        return;
    }

    exercise_spell( you.spells[spc2], true, your_spells(you.spells[spc2], powc, true) );

    you.turn_is_over = 1;
    naughty(NAUGHTY_SPELLCASTING, 1 + random2(5));

}          // end cast_a_spell()


bool your_spells( int spc2, int powc, bool allow_fail )
{
    int dem_hor = 0;
    int dem_hor2 = 0;
    int total_skill = 0;
    struct dist spd[1];
    struct bolt beam[1];

    alert();

// determine base power: modified by skills, magic ability, enhancements {dlb}:
    powc = spell_spec(spc2);

// further modify base power by weighting against intelligence {dlb}:
    powc *= you.intel;
    powc /= 10;

    if ( you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] == OBJ_STAVES
        && you.inv_ident[you.equip[EQ_WEAPON]] == 0 )
    {
        switch ( you.inv_type[you.equip[EQ_WEAPON]] )
        {
          case STAFF_ENERGY:
          case STAFF_WIZARDRY:
            total_skill = you.skills[SK_SPELLCASTING];
            break;
          case STAFF_FIRE:
            if ( spell_typematch(spc2, SPTYP_FIRE) )
              total_skill = you.skills[SK_FIRE_MAGIC];
            else if ( spell_typematch(spc2, SPTYP_ICE) )
              total_skill = you.skills[SK_ICE_MAGIC];
            break;
          case STAFF_COLD:
            if ( spell_typematch(spc2, SPTYP_ICE) )
              total_skill = you.skills[SK_ICE_MAGIC];
            else if ( spell_typematch(spc2, SPTYP_FIRE) )
              total_skill = you.skills[SK_FIRE_MAGIC];
            break;
          case STAFF_AIR:
            if ( spell_typematch(spc2, SPTYP_AIR) )
              total_skill = you.skills[SK_AIR_MAGIC];
            else if (spell_typematch(spc2, SPTYP_EARTH))
              total_skill = you.skills[SK_EARTH_MAGIC];
            break;
          case STAFF_EARTH:
            if ( spell_typematch(spc2, SPTYP_EARTH) )
              total_skill = you.skills[SK_EARTH_MAGIC];
            else if (spell_typematch(spc2, SPTYP_AIR))
              total_skill = you.skills[SK_AIR_MAGIC];
            break;
          case STAFF_POISON:
            if ( spell_typematch(spc2, SPTYP_POISON) )
              total_skill = you.skills[SK_POISON_MAGIC];
            break;
          case STAFF_DEATH:
            if ( spell_typematch(spc2, SPTYP_NECROMANCY) )
              total_skill = you.skills[SK_NECROMANCY];
            break;
          case STAFF_CONJURATION:
            if ( spell_typematch(spc2, SPTYP_CONJURATION) )
              total_skill = you.skills[SK_CONJURATIONS];
            break;
          case STAFF_ENCHANTMENT:
            if ( spell_typematch(spc2, SPTYP_ENCHANTMENT) )
              total_skill = you.skills[SK_ENCHANTMENTS];
            break;
          case STAFF_SUMMONING_I:
            if ( spell_typematch(spc2, SPTYP_SUMMONING) )
              total_skill = you.skills[SK_SUMMONINGS];
            break;
        }

        if ( you.skills[SK_SPELLCASTING] > total_skill )
          total_skill = you.skills[SK_SPELLCASTING];

        if ( random2(100) < total_skill )
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

    surge_power(spc2);

    if ( allow_fail )
    {
        int spfl = random2avg(100,3);

        if ( you.religion != GOD_SIF_MUNA
            && you.penance[GOD_SIF_MUNA]
            && one_chance_in(40) )
        {
            god_speaks(GOD_SIF_MUNA);
            mpr("You feel a surge of divine spite.");
        // This will cause failure and increase the miscast effect.
            spfl = - (you.penance[GOD_SIF_MUNA]);
            dec_penance(1 + random2(3));
        }

        if (spfl < spell_fail(spc2))
        {
            mpr("You miscast the spell.");

            if ( you.religion == GOD_SIF_MUNA
                && ( !player_under_penance()
                    && you.piety >= 100
                    && random2(150) <= you.piety ) )
              {
                canned_msg(MSG_NOTHING_HAPPENS);
                return false;
              }

            char sptype = 0;

            do
            {
                sptype = 1 + random2(SPTYP_RANDOM - 1);    // ewww! I need to write a function for this {dlb}
            }
            while (!spell_typematch(spc2, sptype));

            miscast_effect(sptype, spell_mana(spc2), spell_fail(spc2) - spfl, 100);

            if (you.religion == GOD_XOM && random2(75) < spell_mana(spc2))
              Xom_acts(coinflip(), spell_mana(spc2), false);

            return false;
        }
    }

    if (you.species == SP_MUMMY && spell_typematch(spc2, SPTYP_HOLY))
    {
        mpr("You can't use this type of magic!");
        return false;
    }

    if ( spc2 == SPELL_SUMMON_HORRIBLE_THINGS
          || spc2 == SPELL_CALL_IMP
          || spc2 == SPELL_SUMMON_DEMON
          || spc2 == SPELL_DEMONIC_HORDE
          || spc2 == SPELL_SUMMON_GREATER_DEMON
          || spc2 == SPELL_HELLFIRE )
      naughty(NAUGHTY_UNHOLY, 10 + spell_difficulty(spc2));

    if ( spell_typematch(spc2, SPTYP_NECROMANCY) )
      naughty(NAUGHTY_NECROMANCY, 10 + spell_difficulty(spc2));

    if ( you.religion == GOD_SIF_MUNA
        && you.piety < 200
        && random2(12) <= spell_difficulty(spc2) )
      gain_piety(1);

    switch ( spc2 )
    {
      case SPELL_IDENTIFY:
        identify(powc);
        return true;

      case SPELL_TELEPORT_SELF:
        you_teleport();
        return true;

      case SPELL_CAUSE_FEAR:
        mass_enchantment(4, powc);
        return true;

    case SPELL_CREATE_NOISE:
      if (silenced(you.x_pos, you.y_pos))
        return true;
      else
      {
          mpr("You hear a voice call your name!");
          noisy(25, you.x_pos, you.y_pos);
      }
      break;

    case SPELL_REMOVE_CURSE:
        remove_curse(false);
        return true;

    case SPELL_MAGIC_DART:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_MAGIC_DARTS, powc, &beam[0]);
        return true;

    case SPELL_FIREBALL:
        fireball(powc);
        return true;

    case SPELL_CONJURE_FLAME:
        conjure_flame(powc);
        return true;

    case SPELL_DIG:
        if (spell_direction(spd, &beam[0]) == -1)
          return true;
        if (beam[0].move_x == 0 && beam[0].move_y == 0)
          {
            canned_msg(MSG_UNTHINKING_ACT);
            return true;
          }
        zapping(ZAP_DIGGING, powc, &beam[0]);
        return true;

    case SPELL_BOLT_OF_FIRE:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_FIRE, powc, &beam[0]);
        return true;

    case SPELL_BOLT_OF_COLD:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_COLD, powc, &beam[0]);
        return true;

    case SPELL_LIGHTNING_BOLT:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_LIGHTNING, powc, &beam[0]);
        return true;

    case SPELL_BOLT_OF_MAGMA:
      if (spell_direction(spd, &beam[0]) == -1)
        return true;
      zapping(ZAP_MAGMA, powc, &beam[0]);
      return true;

    case SPELL_POLYMORPH_OTHER:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        if (beam[0].move_x == 0 && beam[0].move_y == 0)
        {
            mpr("Sorry, it doesn't work like that.");
            return true;
        }
        zapping(ZAP_POLYMORPH_OTHER, powc, &beam[0]);
        return true;

    case SPELL_SLOW:
        if (spell_direction(spd, &beam[0]) == -1)
          return true;
        zapping(ZAP_SLOWING, powc, &beam[0]);
        return true;

    case SPELL_HASTE:
        if (spell_direction(spd, &beam[0]) == -1)
          return true;
        zapping(ZAP_HASTING, powc, &beam[0]);
        return true;

    case SPELL_PARALYZE:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_PARALYSIS, powc, &beam[0]);
        return true;

    case SPELL_CONFUSE:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_CONFUSION, powc, &beam[0]);
        return true;

    case SPELL_CONFUSING_TOUCH:
      cast_confusing_touch( powc );
      return true;

    case SPELL_SURE_BLADE:
      cast_sure_blade( powc );
      return true;

    case SPELL_INVISIBILITY:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_INVISIBILITY, powc, &beam[0]);
        return true;               // you.invis

    case SPELL_THROW_FLAME:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_FLAME, powc, &beam[0]);
        return true;

    case SPELL_THROW_FROST:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_FROST, powc, &beam[0]);
        return true;

    case SPELL_CONTROLLED_BLINK:
        blink();
        return true;

    case SPELL_FREEZING_CLOUD:
        cast_big_c(powc, CLOUD_COLD);
        return true;

    case SPELL_MEPHITIC_CLOUD:
        stinking_cloud();
        return true;

    case SPELL_RING_OF_FLAMES:
        cast_ring_of_flames(powc);
        return true;

    case SPELL_RESTORE_STRENGTH:
        restore_stat(STAT_STRENGTH, false);
        return true;

    case SPELL_RESTORE_INTELLIGENCE:
        restore_stat(STAT_INTELLIGENCE, false);
        return true;

    case SPELL_RESTORE_DEXTERITY:
        restore_stat(STAT_DEXTERITY, false);
        return true;

    case SPELL_VENOM_BOLT:
        if ( spell_direction(spd, &beam[0]) == -1 )
          return true;
        zapping(ZAP_VENOM_BOLT, powc, &beam[0]);
        return true;

    case SPELL_OLGREBS_TOXIC_RADIANCE:
        cast_toxic_radiance();
        return true;

    case SPELL_TELEPORT_OTHER:
        if ( spell_direction(spd, &beam[0]) == -1 )
          return true;
        if (beam[0].move_x == 0 && beam[0].move_y == 0)
        {
            mpr("Sorry, it doesn't work like that.");
            return true;
        }
        zapping(ZAP_TELEPORTATION, powc, &beam[0]);    // teleport creature (I think)
        break;

    case SPELL_LESSER_HEALING:
        cast_lesser_healing();
        break;

    case SPELL_GREATER_HEALING:
        cast_greater_healing();
        break;

    case SPELL_CURE_POISON_I:   //jmf: `healing' version? group w/ S_C_P_II?
        cast_cure_poison(powc);
        break;

    case SPELL_PURIFICATION:
        purification();
        break;

    case SPELL_DEATHS_DOOR:
        cast_deaths_door(powc);
        break;

    case SPELL_SELECTIVE_AMNESIA:
      cast_selective_amnesia(false);
      break;                         //     Sif Muna power calls with true

    case SPELL_MASS_CONFUSION:
        mass_enchantment(5, powc);
        break;

    case SPELL_SMITING:
        cast_smiting(powc);
        break;

    case SPELL_REPEL_UNDEAD:
        turn_undead(50);
        break;

    case SPELL_HOLY_WORD:
        holy_word(50);
        //you.conf += random2(3) + 2;
        break;

    case SPELL_DETECT_CURSE:
        detect_curse(false);
        break;

    case SPELL_SUMMON_SMALL_MAMMAL:
        summon_small_mammals(powc); //jmf: hmm, that's definately *plural* ;-)
        break;

    case SPELL_ABJURATION_I:    //jmf: why not group with SPELL_ABJURATION_II?
        abjuration(powc);
        break;

    case SPELL_SUMMON_SCORPIONS:
        summon_scorpions(powc);
        break;

    case SPELL_LEVITATION:
        potion_effect(POT_LEVITATION, 40);
        break;

    case SPELL_BOLT_OF_DRAINING:
        if (spell_direction(spd, &beam[0]) == -1)
          return true;
        zapping(ZAP_NEGATIVE_ENERGY, powc, &beam[0]);
        return true;

    case SPELL_LEHUDIBS_CRYSTAL_SPEAR:
        if (spell_direction(spd, &beam[0]) == -1)
          return true;
        zapping(ZAP_CRYSTAL_SPEAR, powc, &beam[0]);
        return true;

    case SPELL_BOLT_OF_INACCURACY:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_BEAM_OF_ENERGY, powc, &beam[0]);
        return true;

    case SPELL_POISONOUS_CLOUD:
        cast_big_c(powc, CLOUD_POISON);
        return true;

    case SPELL_FIRE_STORM:
        cast_fire_storm(powc);
        return true;

    case SPELL_DETECT_TRAPS:
        strcpy(info, "You detect ");
        strcat(info, (detect_traps() > 0) ? "some traps!" : "nothing.");
        mpr(info);
        return true;

    case SPELL_BLINK:
        random_blink(true);
        return true;

    case SPELL_ISKENDERUNS_MYSTIC_BLAST:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_ORB_OF_ENERGY, powc, &beam[0]);
        return true;

    case SPELL_SWARM:
        summon_swarm(powc);
        return true;

    case SPELL_SUMMON_HORRIBLE_THINGS:
        summon_things(powc);
        return true;

    case SPELL_ENSLAVEMENT:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_ENSLAVEMENT, powc, &beam[0]);
        return true;

    case SPELL_MAGIC_MAPPING:
        if ( you.level_type == LEVEL_LABYRINTH || you.level_type == LEVEL_ABYSS )
          mpr("You feel momentarily disoriented.");
        else if ( you.level_type == LEVEL_PANDEMONIUM )
          mpr("Your Earth magic lacks the power to map Pandemonium.");
        else
        {
            mpr("You feel aware of your surroundings.");
            magic_mapping(5 + (powc * 2), 40 + (powc * 2));
        }
        return true;

    case SPELL_HEAL_OTHER:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        if (beam[0].move_x == 0 && beam[0].move_y == 0)
        {
            mpr("Sorry, it doesn't work like that.");
            return true;
        }
        zapping(ZAP_HEALING, powc, &beam[0]);
        return true;

    case SPELL_ANIMATE_DEAD:
        mpr("You call on the dead to walk for you.");
        animate_dead(powc + 1, BEH_ENSLAVED, you.pet_target, 1);
        return true;

    case SPELL_PAIN:
        if ( spell_direction(spd, &beam[0]) == -1 )
          return true;
        dec_hp(1, false);
        zapping(ZAP_PAIN, powc, &beam[0]);
        return true;

    case SPELL_EXTENSION:
        extension(powc);
        return true;

    case SPELL_CONTROL_UNDEAD:
        mass_enchantment(30, powc);
        return true;

    case SPELL_ANIMATE_SKELETON:
        mpr("You attempt to give life to the dead...");
        animate_a_corpse(you.x_pos, you.y_pos, BEH_ENSLAVED, you.pet_target, CORPSE_SKELETON);
        return true;

    case SPELL_VAMPIRIC_DRAINING:
        vampiric_drain(powc);
        return true;

    case SPELL_SUMMON_WRAITHS:
        summon_undead(powc);
        return true;

    case SPELL_DETECT_ITEMS:
        detect_items(50);
        return true;

    case SPELL_BORGNJORS_REVIVIFICATION:
        cast_revivification(powc);
        return true;

    case SPELL_BURN:
        burn_freeze(powc, BEAM_FIRE);
        return true;

    case SPELL_FREEZE:
        burn_freeze(powc, BEAM_COLD);
        return true;

    case SPELL_SUMMON_ELEMENTAL:
        summon_elemental(powc, 0, 2);
        break;

    case SPELL_OZOCUBUS_REFRIGERATION:
        cast_refrigeration(powc);
        return true;

    case SPELL_STICKY_FLAME:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_STICKY_FLAME, powc, &beam[0]);
        return true;

    case SPELL_SUMMON_ICE_BEAST:
        summon_ice_beast_etc(powc, MONS_ICE_BEAST);
        return true;

    case SPELL_OZOCUBUS_ARMOUR:
        ice_armour(powc, false);
        return true;

    case SPELL_CALL_IMP:
        if (one_chance_in(3))
            summon_ice_beast_etc(powc, MONS_WHITE_IMP);
        else if (one_chance_in(7))
            summon_ice_beast_etc(powc, MONS_SHADOW_IMP);
        else
            summon_ice_beast_etc(powc, MONS_IMP);
        return true;

    case SPELL_REPEL_MISSILES:
        missile_prot(powc);
        return true;

    case SPELL_BERSERKER_RAGE:
        cast_berserk();
        return true;

    case SPELL_DISPEL_UNDEAD:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_DISPEL_UNDEAD, powc, &beam[0]);
        return true;

    case SPELL_GUARDIAN:
        summon_ice_beast_etc(powc, MONS_ANGEL);
        return true;

        //jmf: FIXME: SPELL_PESTILENCE?

    case SPELL_THUNDERBOLT:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_LIGHTNING, powc, &beam[0]);
        return true;

    case SPELL_FLAME_OF_CLEANSING:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_CLEANSING_FLAME, powc, &beam[0]);
        return true;

        //jmf: FIXME: SPELL_SHINING_LIGHT?

    case SPELL_SUMMON_DAEVA:
        summon_ice_beast_etc(powc, MONS_ANGEL);
        return true;

    case SPELL_ABJURATION_II:
        abjuration(powc);
        break;

        // Remember that most holy spells above don't yet use powc!

    case SPELL_TWISTED_RESURRECTION:
        cast_twisted(powc, BEH_ENSLAVED, you.pet_target);
        break;

    case SPELL_REGENERATION:
        cast_regen(powc);
        return true;

    case SPELL_BONE_SHARDS:
        cast_bone_shards(powc);
        return true;

    case SPELL_BANISHMENT:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        /*   if (beam[0].move_x == 0 && beam[0].move_y == 0)
           {
           canned_msg(MSG_UNTHINKING_ACT);
           return true;
           } */
        zapping(ZAP_BANISHMENT, powc, &beam[0]);
        return true;

    case SPELL_CIGOTUVIS_DEGENERATION:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        if (beam[0].move_x == 0 && beam[0].move_y == 0)
        {
            canned_msg(MSG_UNTHINKING_ACT);
            return true;
        }
        zapping(ZAP_DEGENERATION, powc, &beam[0]);
        return true;

    case SPELL_STING:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_STING, powc, &beam[0]);
        return true;

    case SPELL_SUBLIMATION_OF_BLOOD:
        sublimation(powc);
        return true;

    case SPELL_TUKIMAS_DANCE:
        dancing_weapon(powc, false);
        return true;

    case SPELL_HELLFIRE:        // should only be available from:
                                // staff of Dispater & Sceptre of Asmodeus
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_HELLFIRE, powc, &beam[0]);
        return true;

    case SPELL_SUMMON_DEMON:
        mpr("You open a gate to Pandemonium!");
        summon_ice_beast_etc(powc, summon_any_demon(DEMON_COMMON));
        return true;

    case SPELL_DEMONIC_HORDE:
        mpr("You open a gate to Pandemonium!");
        dem_hor2 = 3 + random2(5);
        for (dem_hor = 0; dem_hor < 4 + dem_hor2; dem_hor++)
        {
            summon_ice_beast_etc(powc, summon_any_demon(DEMON_LESSER));
            //if ( one_chance_in(6) ) summon_ice_beast_etc(powc, summon_any_demon(DEMON_LESSER));
            //else summon_ice_beast_etc(powc, MONS_WHITE_IMP + random2(5));
        }
        return true;

    case SPELL_SUMMON_GREATER_DEMON:
        mpr("You open a gate to Pandemonium!");

        dem_hor = ( (random2(powc) <= 5) ? BEH_CHASING_I : BEH_CHASING_II );

        if ( dem_hor == BEH_CHASING_I )
          mpr("You don't feel so good about this...");

        create_monster(summon_any_demon(DEMON_GREATER), 24, dem_hor, you.x_pos, you.y_pos, MHITNOT, 250);

        return true;

    case SPELL_CORPSE_ROT:
        corpse_rot(0);
        return true;

    case SPELL_TUKIMAS_VORPAL_BLADE:
        if ( !brand_weapon(SPWPN_VORPAL, powc) )
          canned_msg(MSG_SPELL_FIZZLES);
        return true;

    case SPELL_FIRE_BRAND:
        if ( !brand_weapon(SPWPN_FLAMING, powc) )
          canned_msg(MSG_SPELL_FIZZLES);
        return true;

    case SPELL_FREEZING_AURA:
        if ( !brand_weapon(SPWPN_FREEZING, powc) )
          canned_msg(MSG_SPELL_FIZZLES);
        return true;

    case SPELL_LETHAL_INFUSION:
        if ( !brand_weapon(SPWPN_DRAINING, powc) )
          canned_msg(MSG_SPELL_FIZZLES);
        return true;

    case SPELL_MAXWELLS_SILVER_HAMMER:
        if (! brand_weapon(SPWPN_DUMMY_CRUSHING, powc))
          canned_msg(MSG_SPELL_FIZZLES);
        return true;

    case SPELL_POISON_WEAPON:
        if ( !brand_weapon(SPWPN_VENOM, powc) )
          canned_msg(MSG_SPELL_FIZZLES);
        return true;

    case SPELL_CRUSH:
        burn_freeze(powc, BEAM_MISSILE);
        return true;

    case SPELL_BOLT_OF_IRON:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_IRON_BOLT, powc, &beam[0]);
        return true;

    case SPELL_STONE_ARROW:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_STONE_ARROW, powc, &beam[0]);
        return true;

    case SPELL_TOMB_OF_DOROKLOHE:
        entomb();
        return true;

    case SPELL_STONEMAIL:
        stone_scales(powc);
        return true;

    case SPELL_SHOCK:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_ELECTRICITY, powc, &beam[0]);
        return true;

    case SPELL_SWIFTNESS:
        cast_swiftness(powc);
        return true;

    case SPELL_FLY:
        cast_fly(powc);
        return true;

    case SPELL_INSULATION:
        cast_insulation(powc);
        return true;

    case SPELL_ORB_OF_ELECTROCUTION:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_ORB_OF_ELECTRICITY, powc, &beam[0]);
        return true;

    case SPELL_DETECT_CREATURES:
        detect_creatures(powc);
        return true;

    case SPELL_CURE_POISON_II:  // poison magic version of cure poison
        cast_cure_poison(powc);
        break;

    case SPELL_CONTROL_TELEPORT:
        cast_teleport_control(powc);
        return true;

    case SPELL_POISON_AMMUNITION:
        cast_poison_ammo();
        return true;

    case SPELL_RESIST_POISON:
        cast_resist_poison(powc);
        return true;

    case SPELL_PROJECTED_NOISE:
        create_noise();
        return true;

    case SPELL_ALTER_SELF:
        if ( !enough_hp(you.hp_max / 2, true) )
        {
            mpr("Your body is in too poor a condition "
                "for this spell to function.");
            return true;
        }
        mpr("Your body is suffused with transfigurative energy!");

        set_hp(1 + random2(you.hp), false);

        if ( !mutate(100) )
          mpr("The spell fails.");
        return true;

    case SPELL_DEBUGGING_RAY:
        if (spell_direction(spd, &beam[0]) == -1)
          return true;
        zapping(ZAP_DEBUGGING_RAY, powc, &beam[0]);
        return true;

    case SPELL_RECALL:
        recall(0);
        return true;

    case SPELL_PORTAL:
        portal();
        return true;

    case SPELL_AGONY:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        if (beam[0].move_x == 0 && beam[0].move_y == 0)
        {
            canned_msg(MSG_UNTHINKING_ACT);
            return true;
        }
        zapping(ZAP_AGONY, powc, &beam[0]);
        return true;

    case SPELL_SPIDER_FORM:
        transform(powc, TRAN_SPIDER);
        return true;

    case SPELL_DISRUPT:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        if (beam[0].move_x == 0 && beam[0].move_y == 0)
        {
            canned_msg(MSG_UNTHINKING_ACT);
            return true;
        }
        zapping(ZAP_DISRUPTION, powc, &beam[0]);
        return true;

    case SPELL_DISINTEGRATE:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        if (beam[0].move_x == 0 && beam[0].move_y == 0)
        {
            canned_msg(MSG_UNTHINKING_ACT);
            return true;
        }
        zapping(ZAP_DISINTEGRATION, powc, &beam[0]);
        return true;

    case SPELL_BLADE_HANDS:
        transform(powc, TRAN_BLADE_HANDS);
        return true;

    case SPELL_STATUE_FORM:
        transform(powc, TRAN_STATUE);
        return true;

    case SPELL_ICE_FORM:
        transform(powc, TRAN_ICE_BEAST);
        return true;

    case SPELL_DRAGON_FORM:
        transform(powc, TRAN_DRAGON);
        return true;

    case SPELL_NECROMUTATION:
        transform(powc, TRAN_LICH);
        return true;

    case SPELL_DEATH_CHANNEL:
        cast_death_channel(powc);
        return true;

    case SPELL_SYMBOL_OF_TORMENT:
        if (you.is_undead || you.mutation[MUT_TORMENT_RESISTANCE])
        {
            mpr("To torment others, one must first know what torment means. ");
            return true;
        }
        torment(you.x_pos, you.y_pos);
        return true;

    case SPELL_DEFLECT_MISSILES:
        deflection(powc);
        return true;

    case SPELL_ORB_OF_FRAGMENTATION:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_ORB_OF_FRAGMENTATION, powc, &beam[0]);
        return true;

    case SPELL_ICE_BOLT:
        if (spell_direction(spd, &beam[0]) == -1)
            return true;
        zapping(ZAP_ICE_BOLT, powc, &beam[0]);
        return true;

    case SPELL_ARC:
        burn_freeze(powc, BEAM_ELECTRICITY);
        return true;

    case SPELL_AIRSTRIKE:
        airstrike(powc);
        return true;

    case SPELL_ICE_STORM:
        if (spell_direction(spd, &beam[0]) == -1)
          return true;
        zapping(ZAP_ICE_STORM, powc, &beam[0]);
        return true;

    case SPELL_SHADOW_CREATURES:
        mpr("Wisps of shadow whirl around you...");
        create_monster(RANDOM_MONSTER, 24, BEH_ENSLAVED, you.x_pos, you.y_pos, MHITNOT, 250);
        return true;


//jmf: new spells 19mar2000
    case SPELL_FLAME_TONGUE:
      if (spell_direction(spd, &beam[0]) == -1)
        return true;
      if (beam[0].move_x == 0 && beam[0].move_y == 0)
      {
          canned_msg(MSG_UNTHINKING_ACT);
          return true;
      }
      zapping(ZAP_FLAME_TONGUE, 2 + random2(4) + random2(4) + random2(powc)/25, &beam[0]);
      return true;

    case SPELL_PASSWALL:
      cast_passwall(powc);
      return true;

    case SPELL_IGNITE_POISON:
      cast_ignite_poison(powc);
      return true;

    case SPELL_STICKS_TO_SNAKES:
      cast_sticks_to_snakes(powc);
      return true;

    case SPELL_SUMMON_LARGE_MAMMAL:
      cast_summon_large_mammal(powc);
      return true;

    case SPELL_SUMMON_DRAGON:
      cast_summon_dragon(powc);
      return true;

    case SPELL_TAME_BEASTS:
      cast_tame_beasts(powc);
      return true;

    case SPELL_SLEEP:
      if ( spell_direction(spd, &beam[0]) == -1 )
        return true;
      zapping(ZAP_SLEEP, powc, &beam[0]);
      return true;

    case SPELL_MASS_SLEEP:
      cast_mass_sleep(powc);
      return true;

    case SPELL_DETECT_MAGIC:
      mpr("FIXME: implement!");
      return true;

    case SPELL_DETECT_SECRET_DOORS:
      cast_detect_secret_doors(powc);
      return true;

    case SPELL_SEE_INVISIBLE:
      cast_see_invisible(powc);
      return true;

    case SPELL_FORESCRY:
      cast_forescry(powc);
      return true;

    case SPELL_SUMMON_BUTTERFLIES:
      cast_summon_butterflies(powc);
      return true;

    case SPELL_WARP_BRAND:
      if ( !brand_weapon(SPWPN_DISTORTION, powc) )
        canned_msg(MSG_SPELL_FIZZLES);
      return true;

    case SPELL_SILENCE:
      cast_silence(powc);
      return true;

    case SPELL_SHATTER:
      cast_shatter(powc);
      return true;

    case SPELL_WARP_FIELD:
      cast_warp_field(powc);
      return true;

    case SPELL_DISCHARGE:
      cast_discharge(powc);
      return true;

    case SPELL_BEND:
      cast_bend(powc);
      return true;

    case SPELL_BACKLIGHT:
      if (spell_direction(spd, &beam[0]) == -1)
        return true;
      zapping(ZAP_BACKLIGHT, powc + 10, &beam[0]);
      return true;

    case SPELL_INTOXICATE:
      cast_intoxicate(powc);
      return true;

#ifdef USE_ELVISH_GLAMOUR_ABILITY
    case SPELL_GLAMOUR:
      cast_glamour(powc);
      return true;
#endif

    case SPELL_EVAPORATE:
      cast_evaporate(powc);
      return true;

    case SPELL_FRAGMENTATION:
      cast_fragmentation(powc);
      return true;

    case SPELL_AIR_WALK:
      transform(powc, TRAN_AIR);
      return true;

    case SPELL_SANDBLAST:
      cast_sandblast(powc);
      return true;

    case SPELL_ROTTING:
      cast_rotting(powc);
      return true;

    case SPELL_SHUGGOTH_SEED:
      cast_shuggoth_seed(powc);
      return true;

    case SPELL_CONDENSATION_SHIELD:
      cast_condensation_shield(powc);
      return true;

    case SPELL_SEMI_CONTROLLED_BLINK:
      cast_semi_controlled_blink(powc); //jmf: powc is ignored
      return true;

    case SPELL_STONESKIN:
      cast_stoneskin(powc);
      return true;

    default:
      mpr("Invalid spell!");
      break;
    }                           // end switch

    return false;                   // this should never(!) happen
}          // end you_spells()


void exercise_spell( int spell, bool spc, bool success )
{
  // (!success) reduces skill increase for miscast spells
  int ndx = 0;
  int skill;
  int workout = 0;

  unsigned int disciplines = spell_type( spell );
  disciplines &= (~SPTYP_HOLY); //jmf: evil evil evil -- exclude HOLY bit

  int skillcount = count_bits( disciplines );

  if ( !success )
    skillcount += 4 + random2(10);

  for (ndx = 0; ndx <= SPTYP_LAST_EXPONENT; ndx++)
    {
      if (! spell_typematch( spell, 1 << ndx )) continue;
      skill = spell_type2skill( 1 << ndx );
      workout = ( random2(1 + spell_difficulty(spell)) / skillcount );
      if ( !one_chance_in(5) )
        workout++; // most recently, this was an automatic add {dlb}
      exercise(skill, workout);
    }


  /* ******************************************************************
     Other recent formulae for the above:

     * workout = random2(spell_difficulty(spell_ex)
               * (10 + (spell_difficulty(spell_ex) * 2 )) / 10 / spellsy + 1);

     * workout = spell_difficulty(spell_ex)
               * (15 + spell_difficulty(spell_ex)) / 15 / spellsy;

     spellcasting had also been generally exercised at the same time
     ****************************************************************** */

  if ( spc )
    exercise(SK_SPELLCASTING, one_chance_in(3) ? 1 : 0 +
             random2(1 + random2(spell_difficulty(spell))));
             //+ (coinflip() ? 1 : 0) + (skillcount ? 1 : 0));

/* ******************************************************************
   3.02 was:

    if ( spc && spellsy )
      exercise(SK_SPELLCASTING, random2(random2(spell_difficulty(spell_ex) + 1) / spellsy)); // + 1);
    else if ( spc )
      exercise(SK_SPELLCASTING, random2(random2(spell_difficulty(spell_ex)))); // + 1);
****************************************************************** */

}          // end exercise_spell()

/*
 *  File:       spells0.cc
 *  Summary:    Some spell related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <4>    05jan2000        jmf   Changed many numbers to enums
 *      <3>      5/20/99        BWR   Extend screen lines support.
 *      <2>      5/09/99        JDJ   spellbook_contents prints unknown spells
 *                                    in light blue.
 *      <1>      -/--/--        LRH   Created
 */

#include "AppHdr.h"
#include "spells0.h"

#include <stdlib.h>

#ifdef DOS
#include <conio.h>
#endif

#ifdef USE_CURSES
#include <curses.h>
#endif

#include "externs.h"

#include "itemname.h"
#include "player.h"
#include "dungeon.h"

#ifdef MACROS
  #include "macro.h"
#endif

int spell_hunger(int spell_value, unsigned char spell);
int which_spell_in_book(int sbook_type, int spl);
void spell_name(unsigned char spell, char spln[60]);
char spell_value(unsigned char spell);
bool spell_type(unsigned char spell, unsigned char typy);
int spell_spec(int spell, int power);
char spell_fail(unsigned char spell);
void print_slash(char already);
//char spell_list(void);     // already declared in header {dlb}


char spell_list(void)
{
    char st_pass[60];
    int j;
    char lines = 0;
    unsigned char anything = 0;
    char strng[5] = "";
    char ft;
    char ki;
    char already = 0;

#ifdef DOS_TERM
    char buffer[4800];
    gettext(1, 1, 80, 25, buffer);
#endif

#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif

    strcpy(st_pass, "");
    clrscr();
    cprintf("  Your Spells                       Type                  Success   Level");
    lines++;

    if (lines > NUMBER_OF_LINES - 2)
    {
        gotoxy(1, NUMBER_OF_LINES);
        cprintf("-more-");
        ki = getch();
        if (ki == 27)
        {
#ifdef DOS_TERM
            puttext(1, 1, 80, 25, buffer);
#endif
            return 27;
        }
        if (ki >= 65 && ki < 123)
        {
#ifdef DOS_TERM
            puttext(1, 1, 80, 25, buffer);
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
    if (lines > 1)
        cprintf(EOL " ");

    for (j = 0; j < 25; j++)
    {
        if (lines > NUMBER_OF_LINES - 2)
        {
            gotoxy(1, NUMBER_OF_LINES);
            cprintf("-more-");
            ki = getch();
            if (ki == 27)
            {
#ifdef DOS_TERM
                puttext(1, 1, 80, 25, buffer);
#endif
                return 27;
            }
            if (ki >= 65 && ki < 123)
            {
#ifdef DOS_TERM
                puttext(1, 1, 80, 25, buffer);
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

        if ( you.spells[j] != SPELL_NO_SPELL )
        {
            anything++;
            if (lines > 0)
                cprintf(EOL);
            lines++;
            cprintf(" ");
            if (j < 26)
                ft = (char) j + 97;
            else
                ft = (char) j + 65;
            strng[0] = ft;
            cprintf(strng);
            cprintf(" - ");
            spell_name(you.spells[j], st_pass);

            cprintf(st_pass);

            gotoxy(35, wherey());

            already = 0;

            if ( spell_type(you.spells[j], SPTYP_HOLY) )
              {
                cprintf("Holy");
                already = 1;
              }
            if ( spell_type(you.spells[j], SPTYP_POISON) )
              {
                print_slash(already);
                cprintf("Poison");
                already = 1;
              }
            if ( spell_type(you.spells[j], SPTYP_FIRE) )
              {
                print_slash(already);
                cprintf("Fire");
                already = 1;
              }
            if ( spell_type(you.spells[j], SPTYP_ICE) )
              {
                print_slash(already);
                cprintf("Ice");
                already = 1;
              }
            if ( spell_type(you.spells[j], SPTYP_EARTH) )
              {
                print_slash(already);
                cprintf("Earth");
                already = 1;
             }
            if ( spell_type(you.spells[j], SPTYP_AIR) )
              {
                print_slash(already);
                cprintf("Air");
                already = 1;
              }
            if ( spell_type(you.spells[j], SPTYP_CONJURATION) )
              {
                print_slash(already);
                cprintf("Conjuration");
                already = 1;
              }
            if ( spell_type(you.spells[j], SPTYP_ENCHANTMENT) )
              {
                print_slash(already);
                cprintf("Enchantment");
                already = 1;
              }
            if ( spell_type(you.spells[j], SPTYP_DIVINATION) )
              {
                print_slash(already);
                cprintf("Divination");
                already = 1;
              }
            if ( spell_type(you.spells[j], SPTYP_TRANSLOCATION) )
              {
                print_slash(already);
                cprintf("Translocation");
                already = 1;
             }
            if ( spell_type(you.spells[j], SPTYP_SUMMONING) )
              {
                print_slash(already);
                cprintf("Summoning");
                already = 1;
              }
            if ( spell_type(you.spells[j], SPTYP_TRANSMIGRATION) )
              {
                print_slash(already);
                cprintf("Transmigration");
                already = 1;
              }
            if ( spell_type(you.spells[j], SPTYP_NECROMANCY) )
              {
                print_slash(already);
                cprintf("Necromancy");
                already = 1;
              }

            char sval[4];
            gotoxy(58, wherey());
            int spell_f = spell_fail(you.spells[j]);

            if (spell_f == 100)
              cprintf("Useless");
            else if (spell_f > 90)
              cprintf("Terrible");
            else if (spell_f > 80)
              cprintf("Cruddy");
            else if (spell_f > 70)
              cprintf("Bad");
            else if (spell_f > 60)
              cprintf("Very Poor");
            else if (spell_f > 50)
              cprintf("Poor");
            else if (spell_f > 40)
              cprintf("Fair");
            else if (spell_f > 30)
              cprintf("Good");
            else if (spell_f > 20)
              cprintf("Very Good");
            else if (spell_f > 10)
              cprintf("Great");
            else if (spell_f > 0)
              cprintf("Excellent");
            else
              cprintf("Perfect");

            gotoxy(70, wherey());
            itoa((int) spell_value(you.spells[j]), sval, 10);
            cprintf(sval);
        }
    }                           // end of j loop

    if (anything > 0)
    {
        ki = getch();
        if (ki >= 65 && ki < 123)
        {
#ifdef DOS_TERM
            puttext(1, 1, 80, 25, buffer);
#endif
            return ki;
        }
        if (ki == 0)
            ki = getch();
#ifdef DOS_TERM
        puttext(1, 1, 80, 25, buffer);
#endif
        return anything;
    }

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif
    // was 35
    ki = getch();
    return ki;
}




void print_slash(char already)
{
  if (already == 1)
    cprintf("/");
}




char spell_fail(unsigned char spell)
{
    int chance = 60;
    int chance2 = 0, armour = 0;

    chance -= spell_spec(spell, 0) * 6;
    chance -= you.intel * 2;

    //chance -= (you.intel - 10) * abs(you.intel - 10);
    //chance += spell_value(spell) * spell_value(spell) * 3; //spell_value(spell);

    if (you.equip[EQ_BODY_ARMOUR] != -1) {
      //jmf: armour skill now reduces failure due to armour
      armour += ( abs( property( OBJ_ARMOUR,
                                 you.inv_type[you.equip[EQ_BODY_ARMOUR]],
                                 PARM_EVASION ) ) * 5 ) - 2;
      if (armour > 0)
        armour -= you.skills[SK_ARMOUR] * (you.strength / 5 + 1);
      if (armour > 0)
        chance += armour;
    }

    if ( you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] != OBJ_STAVES )
      {
        int wpn_penalty = 0;

        if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS)
          {
            wpn_penalty = ( 5 *
                             ( property( OBJ_WEAPONS,
                                         you.inv_type[you.equip[EQ_WEAPON]],
                                         PWPN_SPEED ) - 12 ) ) / 2;
          }

        if (wpn_penalty > 0)
          chance += wpn_penalty;
      }

    if (you.equip[EQ_SHIELD] != -1)
    {
        switch (you.inv_type[you.equip[EQ_SHIELD]])
        {
            case ARM_BUCKLER:
                chance += 5;
                break;

            case ARM_SHIELD:
                chance += 15;
                break;

            case ARM_LARGE_SHIELD:
                // *BCR* Large chars now get a lower penalty for large shields
                if (you.species >= SP_OGRE && you.species <= SP_UNK2_DRACONIAN)
                    chance += 20;
                else
                    chance += 30;
                break;
        }
    }

    switch (spell_value(spell))
    {
    case 1:
        chance += 3;
        break;
    case 2:
        chance += 15;
        break;
    case 3:
        chance += 35;
        break;
    case 4:
        chance += 70;
        break;
    case 5:
        chance += 100;
        break;
    case 6:
        chance += 150;
        break;
    case 7:
        chance += 200;
        break;
    case 8:
        chance += 260;
        break;
    case 9:
        chance += 330;
        break;
    case 10:
        chance += 420;
        break;
    case 11:
        chance += 500;
        break;
    case 12:
        chance += 600;
        break;
    }

    //if (chance <= 0) chance = 0;
    if (chance > 100)
      chance = 100;

    chance2 = chance;

    if (chance < 45)
        chance2 = 45;
    if (chance < 42)
        chance2 = 43;
    if (chance < 38)
        chance2 = 41;
    if (chance < 35)
        chance2 = 40;
    if (chance < 32)
        chance2 = 38;
    if (chance < 28)
        chance2 = 36;
    if (chance < 22)
        chance2 = 34;
    if (chance < 16)
        chance2 = 32;
    if (chance < 10)
        chance2 = 30;
    if (chance < 2)
        chance2 = 28;
    if (chance < -7)
        chance2 = 26;
    if (chance < -12)
        chance2 = 24;
    if (chance < -18)
        chance2 = 22;
    if (chance < -24)
        chance2 = 20;
    if (chance < -30)
        chance2 = 18;
    if (chance < -38)
        chance2 = 16;
    if (chance < -46)
        chance2 = 14;
    if (chance < -60)
        chance2 = 12;
    if (chance < -80)
        chance2 = 10;
    if (chance < -100)
        chance2 = 8;
    if (chance < -120)
        chance2 = 6;
    if (chance < -140)
        chance2 = 4;
    if (chance < -160)
        chance2 = 2;
    if (chance < -180)
        chance2 = 0;

    if ( you.religion == GOD_VEHUMET
        && you.duration[DUR_PRAYER] != 0
        && you.piety >= 50
        && ( spell_type(spell, SPTYP_CONJURATION) || spell_type(spell, SPTYP_SUMMONING) ) )
    {
      chance2 /= 2;
    }

    if ( you.duration[DUR_TRANSFORMATION] > 0
        && you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS )
      {
        chance2 += 20;
      }

    return chance2;
}




int spell_spec(int spell, int power)
{

    int s = 0;            // loop variable
    int spellsy = 0;
    int enhanced = 0;

    power = 0;            // then why is this even passed? {dlb}

    for (s = SPTYP_CONJURATION; s <= SPTYP_AIR; s++)
      {
        //if (s == SPTYP_FIRE || s == SPTYP_ICE || s == SPTYP_HOLY) continue;
        if ( s == SPTYP_HOLY )
            continue;
        if ( spell_type(spell, s) )
            spellsy++;
      }

    if (spellsy != 0)
      {
        if ( spell_type(spell, SPTYP_CONJURATION) )
          power += (you.skills[SK_CONJURATIONS] * 2) / spellsy;

        if ( spell_type(spell, SPTYP_ENCHANTMENT) )
          power += (you.skills[SK_ENCHANTMENTS] * 2) / spellsy;

        if ( spell_type(spell, SPTYP_TRANSMIGRATION) )
          power += (you.skills[SK_TRANSMIGRATION] * 2) / spellsy;

        if ( spell_type(spell, SPTYP_NECROMANCY) )
          power += (you.skills[SK_NECROMANCY] * 2) / spellsy;

        if ( spell_type(spell, SPTYP_SUMMONING) )
          power += (you.skills[SK_SUMMONINGS] * 2) / spellsy;

        if ( spell_type(spell, SPTYP_DIVINATION) )
          power += (you.skills[SK_DIVINATIONS] * 2) / spellsy;

        if ( spell_type(spell, SPTYP_TRANSLOCATION) )
          power += (you.skills[SK_TRANSLOCATIONS] * 2) / spellsy;

        if ( spell_type(spell, SPTYP_POISON) )
          power += (you.skills[SK_POISON_MAGIC] * 2) / spellsy;

        if ( spell_type(spell, SPTYP_EARTH) )
          power += (you.skills[SK_EARTH_MAGIC] * 2) / spellsy;

        if ( spell_type(spell, SPTYP_AIR) )
          power += (you.skills[SK_AIR_MAGIC] * 2) / spellsy;

        if ( spell_type(spell, SPTYP_FIRE) )
          power += (you.skills[SK_FIRE_MAGIC] * 2) / spellsy;

        if ( spell_type(spell, SPTYP_ICE) )
          power += (you.skills[SK_ICE_MAGIC] * 2) / spellsy;

      }     // end if spellsy != 0

    power += ( you.skills[SK_SPELLCASTING] * 5 ) / 10;

    power += player_mag_abil();     // currently only affected by staves/rings of wizardry

    if ( spell_type(spell, SPTYP_NECROMANCY) )
      {
        enhanced += player_spec_death();
        enhanced -= player_spec_holy();
        //naughty(spell_value(spell) * 2, 1);
      }

    if ( spell_type(spell, SPTYP_FIRE) )
      {
        enhanced += player_spec_fire();
        enhanced -= player_spec_cold();
      }

    if ( spell_type(spell, SPTYP_ICE) )
      {
        enhanced += player_spec_cold();
        enhanced -= player_spec_fire();
      }

    if ( spell_type(spell, SPTYP_HOLY) )
      {
        enhanced -= player_spec_death();
        enhanced += player_spec_holy();
        if (you.special_wield == 50)
            enhanced -= 2;
      }          // holy - signs switched!

    if ( spell_type(spell, SPTYP_CONJURATION) )
      {
        enhanced += player_spec_conj();
      }

    if ( spell_type(spell, SPTYP_ENCHANTMENT) )
      {
        enhanced += player_spec_ench();
      }

    if ( spell_type(spell, SPTYP_SUMMONING) )
      {
        enhanced += player_spec_summ();
      }

    if ( spell_type(spell, SPTYP_POISON) )
      {
        enhanced += player_spec_poison();
      }

    if ( spell_type(spell, SPTYP_EARTH) )
      {
        enhanced += player_spec_earth();        // spec_earth
        enhanced -= player_spec_air();          // spec_air
      }

    if ( spell_type(spell, SPTYP_AIR) )
      {
        enhanced -= player_spec_earth();        // spec_earth
        enhanced += player_spec_air();          // spec_air
      }


/* If the bit just above changes, remember to also change it in crawl99.cc */

    if (enhanced > 0)
    {
        for (s = 0; s < enhanced; s++)
        {
            power *= 15;
            power /= 10;
        }
    }

    if (enhanced < 0)
    {
        for (s = enhanced; s < 0; s++)
        {
            power /= 2;     // /= 10;
        }
    }

    return power;

}                               // end int spell_spec




bool spell_type(unsigned char spell, unsigned char typy)
{
    unsigned long stype = 0;

    switch (spell)
    {
    case SPELL_DEBUGGING_RAY:
    case SPELL_BOLT_OF_INACCURACY:
    case SPELL_ISKENDERUNS_MYSTIC_BLAST:
    case SPELL_MAGIC_DART:
        stype = 11;
        break;

    case SPELL_BERSERKER_RAGE:
    case SPELL_CAUSE_FEAR:
    case SPELL_CONFUSE:
    case SPELL_CREATE_NOISE:
    case SPELL_ENSLAVEMENT:
    case SPELL_EXTENSION:
    case SPELL_HASTE:
    case SPELL_INVISIBILITY:
    case SPELL_MASS_CONFUSION:
    case SPELL_PARALYZE:
    case SPELL_PROJECTED_NOISE:
    case SPELL_REMOVE_CURSE:
    case SPELL_SELECTIVE_AMNESIA:
    case SPELL_SLOW:
    case SPELL_TUKIMAS_DANCE:
    case SPELL_TUKIMAS_VORPAL_BLADE:
        stype = 12;
        break;

    case SPELL_BURN:
        stype = 13;
        break;

    case SPELL_FREEZE:
    case SPELL_OZOCUBUS_REFRIGERATION:
        stype = 14;
        break;

    case SPELL_ALTER_SELF:
    case SPELL_BLADE_HANDS:
    case SPELL_DISRUPT:
    case SPELL_DISINTEGRATE:
        stype = 15;
        break;

    case SPELL_AGONY:
    case SPELL_ANIMATE_DEAD:
    case SPELL_ANIMATE_SKELETON:
    case SPELL_BONE_SHARDS:
    case SPELL_BORGNJORS_REVIVIFICATION:
    case SPELL_CORPSE_ROT:
    case SPELL_DEATH_CHANNEL:
    case SPELL_DISPEL_UNDEAD:
    case SPELL_PAIN:
    case SPELL_SUBLIMATION_OF_BLOOD:
    case SPELL_SYMBOL_OF_TORMENT:
    case SPELL_TWISTED_RESURRECTION:
    case SPELL_VAMPIRIC_DRAINING:
        stype = 16;
        break;

    case SPELL_ABJURATION_II:
    case SPELL_CURE_POISON_I:
    case SPELL_FLAME_OF_CLEANSING:
    case SPELL_GUARDIAN:
    case SPELL_GREATER_HEALING:
    case SPELL_HEAL_OTHER:
    case SPELL_HOLY_WORD:
    case SPELL_LESSER_HEALING:
    case SPELL_PESTILENCE:
    case SPELL_PURIFICATION:
    case SPELL_REPEL_UNDEAD:
    case SPELL_RESTORE_DEXTERITY:
    case SPELL_RESTORE_INTELLIGENCE:
    case SPELL_RESTORE_STRENGTH:
    case SPELL_SHINING_LIGHT:
    case SPELL_SMITING:
    case SPELL_SUMMON_DAEVA:
        stype = 17;
        break;

    case SPELL_ABJURATION_I:
    case SPELL_CALL_IMP:
    case SPELL_DEMONIC_HORDE:
    case SPELL_SWARM:
    case SPELL_SUMMON_DEMON:
    case SPELL_SUMMON_ELEMENTAL:
    case SPELL_SUMMON_GREATER_DEMON:
    case SPELL_SUMMON_HORRIBLE_THINGS:
    case SPELL_SUMMON_SMALL_MAMMAL:
        stype = 18;
        break;

    case SPELL_DETECT_CREATURES:
    case SPELL_DETECT_CURSE:
    case SPELL_DETECT_ITEMS:
    case SPELL_DETECT_TRAPS:
    case SPELL_IDENTIFY:
        stype = 19;
        break;

    case SPELL_BANISHMENT:
    case SPELL_BLINK:
    case SPELL_CONTROLLED_BLINK:
    case SPELL_PORTAL:
    case SPELL_TELEPORT_OTHER:
    case SPELL_TELEPORT_SELF:
        stype = 20;
        break;

    case SPELL_CURE_POISON_II:
    case SPELL_OLGREBS_TOXIC_RADIANCE:
        stype = 21;
        break;

    case SPELL_CRUSH:
        stype = 22;
        break;

    case SPELL_AIRSTRIKE:
    case SPELL_ARC:
        stype = 23;
        break;

    case SPELL_BOLT_OF_FIRE:
    case SPELL_FIREBALL:
    case SPELL_FIRE_STORM:
    case SPELL_STICKY_FLAME:
    case SPELL_THROW_FLAME:
        stype = 1311;
        break;

    case SPELL_BOLT_OF_COLD:
    case SPELL_THROW_FROST:
        stype = 1411;
        break;

    case SPELL_BOLT_OF_DRAINING:
        stype = 1611;
        break;

    case SPELL_SHADOW_CREATURES:          //jmf: was just 18
        stype = 1811;
        break;

    case SPELL_VENOM_BOLT:
        stype = 2111;
        break;

    case SPELL_LIGHTNING_BOLT:
        stype = 2311;
        break;

    case SPELL_RING_OF_FLAMES:
        stype = 1312;
        break;

    case SPELL_OZOCUBUS_ARMOUR:
        stype = 1412;
        break;

    case SPELL_POLYMORPH_OTHER:
        stype = 1512;
        break;

    case SPELL_CONTROL_UNDEAD:
    case SPELL_DEATHS_DOOR:
    case SPELL_REGENERATION:
        stype = 1612;
        break;

    case SPELL_POISON_AMMUNITION:
    case SPELL_POISON_WEAPON:
    case SPELL_RESIST_POISON:
        stype = 2112;
        break;

    case SPELL_HELLFIRE:
        stype = 1113;
        break;

    case SPELL_FIRE_BRAND:
        stype = 1213;
        break;

    case SPELL_ICE_BOLT:
    case SPELL_ICE_STORM:
        stype = 1114;
        break;

    case SPELL_FREEZING_AURA:
        stype = 1214;
        break;

    case SPELL_DRAGON_FORM:
        stype = 1315;
        break;

    case SPELL_ICE_FORM:
        stype = 1415;
        break;

    case SPELL_NECROMUTATION:
        stype = 1615;
        break;

    case SPELL_STATUE_FORM:
        stype = 2215;
        break;

    case SPELL_LETHAL_INFUSION:
        stype = 1216;
        break;

    case SPELL_CIGOTUVIS_DEGENERATION:
        stype = 1516;
        break;

    case SPELL_SUMMON_ICE_BEAST:
        stype = 1418;
        break;

    case SPELL_SUMMON_WRAITHS:
        stype = 1618;
        break;

    case SPELL_SUMMON_SCORPIONS:
        stype = 2118;
        break;

    case SPELL_RECALL:
        stype = 1820;
        break;

    case SPELL_CONTROL_TELEPORT:
        stype = 1220;
        break;

    case SPELL_STING:
        stype = 1121;
        break;

    case SPELL_SPIDER_FORM:
        stype = 1521;
        break;

    case SPELL_BOLT_OF_IRON:
    case SPELL_LEHUDIBS_CRYSTAL_SPEAR:
    case SPELL_ORB_OF_FRAGMENTATION:
    case SPELL_STONE_ARROW:
        stype = 1122;
        break;

    case SPELL_STONEMAIL:
        stype = 1222;
        break;

    case SPELL_DIG:
    case SPELL_TOMB_OF_DOROKLOHE:          //jmf: changed from 1122
        stype = 1522;
        break;

    case SPELL_MAGIC_MAPPING:
        stype = 1922;
        break;

    case SPELL_ORB_OF_ELECTROCUTION:
    case SPELL_SHOCK:
        stype = 1123;
        break;

    case SPELL_DEFLECT_MISSILES:
    case SPELL_FLY:
    case SPELL_INSULATION:
    case SPELL_LEVITATION:
    case SPELL_REPEL_MISSILES:
    case SPELL_SWIFTNESS:
        stype = 1223;
        break;

    case SPELL_THUNDERBOLT:
        stype = 1723;
        break;

    case SPELL_CONJURE_FLAME:
        stype = 231311;
        break;

    case SPELL_FREEZING_CLOUD:
        stype = 231411;
        break;

    case SPELL_MEPHITIC_CLOUD:
    case SPELL_POISONOUS_CLOUD:
        stype = 232111;
        break;

        // Make restore abilities necromancy?

    default:
      stype = 0;
      break;

    }


/*
   types of spells:
   11 = conjuration
   12 = enchantment
   13 = fire
   14 = ice
   15 = transmigration
   16 = necromancy
   17 = holy
   18 = summoning
   19 = divination
   20 = translocation
   21 = poison
   22 = Earth
   23 = Air
   -see guidelines below
 */


//if (stype == typy || (stype / 10) % 10 == typy || (stype / 100) % 10 == typy) return true;

//if (stype == typy || (stype / 10) % 10 == typy || (stype / 100) % 10 == typy || (stype / 1000) % 10 == typy || (stype / 10000) % 10 == typy) return true;

//if (stype % 100 == typy || stype / 100 == typy) return 1;

    if (stype % 100 == typy || (stype / 100) % 100 == typy || (stype / 10000) % 100 == typy)
      return true;
    else
      return false;

}          // end spell_type




/*
   In case anyone ever wants to add new spells, or just understand my reasons
   for putting a particular spell into a particular type, read on:

   Guidelines for typing spells

   Conjuration
   This type has a near monopoly on effective and relatively risk-free combat
   spells. All other types of combat spells are either indirect (enchantments),
   risky/detrimental/not versatile (necromancy) or just plain crappy (burn and
   freeze), although smiting (holy) is not too bad.
   Conjuration spells all involve the magical creation of matter and/or energy
   (which are the same thing anyway, right?). They are distinguished from
   summoning spells in that they do not involve the summoning of an entire
   creature from another place.

   Enchantment
   These spells mostly cause some kind of durational effect, which lasts only
   until the magic wears off. Enchantments are distinguished from trans-
   migrations in that the latter cause a permanent alteration in something
   which persists even after the magic has faded, while the effects of the
   former last only so long as the magic does. Sometimes enchantments may take
   advantage of the more powerful aspects of transmigration to induce some
   kind of radical change (eg polymorph).
   Some enchantments would also fall under the description of 'meta-magic'
   spells, like Selective Amnesia and Remove Curse (and if I ever implement
   Dispel Magic, it will be an enchantment).
   It is possible that some divinations could be retyped as
   divination/enchantment, as they appear to be primarily concerned with
   detecting enchantments. Detect Curse and Identify are what I'm thinking
   of here.

   Fire and Ice
   These are quite obvious. I'm trying to keep these two balanced with each
   other, but it can be difficult. I have to weigh up some useful fire spells,
   like Sticky Flame, Fireball, Ring of Flames and Firestorm, and the fact that
   Fire wizards have an advantage when summoning fire elementals by either
   spell or device, with the also quite useful Refrigeration, Ice Armour and
   Freezing Cloud. Ice wizards don't have a corresponding advantage with
   water elementals, because water and ice are two different things (ice is not
   necessarily water ice, for example).
   Generally, Fire spells tend towards chaos, disorder and entropy, while
   Ice spells tend towards order and stasis. But these trends are rather
   underdeveloped at the moment.
   Note that just about the only reason one would ever choose an ice or fire
   wizard over a conjurer would be the resistance gained at level 12.
   Especially because having a fire specialisation basically removes any chance
   of ever using ice spells effectively, and vice versa.

   Transmigration
   See enchantments.

   Necromancy
   This is the fun stuff. Necromancy is a mixed bag of many and various
   different kinds of spells, with a few common themes:
   -Differentiation of living, dead and undead. Some necromancy affects only
   the living (pain, vampiric draining etc), some affects only the dead
   (animate dead, twisted resurrection etc), and some affects only undead
   (dispel and control undead).
   -Actual or potential harm: eg risk in Death's Door, hp loss with Pain,
   disease with summon greater undead, etc. Also loss of potential experience
   gain with bolt of draining and degeneration.
   -Material components are central to many of the spells.
   -Some spells duplicate effects of other types, but do so in a different
   (neither superior or inferior) way. Eg bone shards is a very powerful spell
   for only 3 magic points, but requires preparation. Also, necromantic
   healing spells are different and more idiosyncratic than holy healing.
   Although regeneration is usually less useful than lesser healing and is
   level 3 instead of 2, it can be cast before combat (when 1 turn spent
   casting is less important), and is affected by extension.
   -Generally unholy theme of spells (I mean, Twisted Resurrection?).

   Holy
   The Holy type is also fairly various, but is rather less interesting than
   necromancy (after all, priests are better at fighting than necromancers).
   Holy spells do things like driving off undead and healing. Note that I
   consider item stickycursing to be more of an issue for enchantments rather
   than holy magic, which is why remove curse is enchantment.

   Summoning
   These spells involve bringing a creature from somewhere else (possibly on
   another plane of existence) to this world to do battle for the caster. Some
   future summonings could potentially be combination conjuration/summoning
   spells, eg the ball lightning spell I keep planning to implement.
   Also, potential exists for some risky high-level spells, maybe demon
   summoning?

   Divination
   These spells provide information to the caster. A diviner class would be
   possible (and having detect curse and identify would be very handy), but
   would be extremely difficult to play - there is no potential in this type
   for combat spells.

   Translocation
   Translocation spells deal with teleportation etc, also interplanar travel
   (eg Banishment, and the planned Gate spell).
   It is possible that I may give summoners some special access to trans-
   locations due to the obvious similarities.

   Poison
   These spells all involve poison. Most are also conjurations.
   I don't plan to implement a 'Poisoner' class, as it would become unplayable
   deep in the dungeon where most monsters are poison resistant.



   Many spells use magic from two types. These spells are equally
   available to either type; a conjurer is no worse at a fire/conjuration than
   at a pure conjuration. I guess a spell could be of three types, but they
   would have to be types with short names (limited space in the spell
   windows).
   - Note : this is no longer true, with the implementation of magic skills.
   Your skill for a spell is effectively the average of all types used in it.
   Poison has no skills, but still has a staff


 */




/*

   char spell_value(char spell)
   {

   switch(spell)
   {
   case 0: return 10; // identify
   case 1: return 8; // teleportation
   case 2: return 6; // cause fear
   case 3: return 1; // noise
   case 4: return 7; // remove curse
   case 5: return 1; // magic missile
   case 6: return 7; // fireball
   case 13: return 5; // conjure flame
   case 14: return 6; // dig
   case 15: return 6; // firebolt
   case 16: return 6; // bolt of freezing cold
   case 17: return 8; // bolt of lightning
   case 20: return 7; // polymorph other
   case 21: return 3;
   case 22: return 4;
   case 23: return 4;
   case 24: return 3;
   case 25: return 6; // invisibility
   case 26: return 2; // throw flame
   case 27: return 2; // throw frost
   case 28: return 4; // controlled blink
   case 29: return 11; // freezing cloud
   case 30: return 3; // stinking cloud
   case 31: return 12; // ring of flames
   case 32: return 2; //  restore strength:
   case 33: return 2; //          int
   case 34: return 2; //          dex
   case 35: return 6; // venom bolt
   case 36: return 4; // toxic radiance - uses lots of food?
   case 37: return 9; // teleport other
   case 38: return 3; // lesser healing
   case 39: return 8; // greater healing
   case 40: return 3; // cure poison
   case 41: return 6; // purification
   case 42: return 13; // death's door
   case 43: return 4; // selective amnesia
   case 44: return 10; // mass confusion
   case 45: return 6; // smiting
   case 46: return 3; // repel undead
   case 47: return 10; // holy word
   case 48: return 3; // detect curse
   case 49: return 1; // summon small mammal
   case 50: return 6; // Abjuration
   case 51: return 6; // summon scorpions
   case 52: return 3; // levitation
   case 53: return 7; // bolt of draining
   case 54: return 12; // splinters
   case 55: return 8; // innacuracy
   case 56: return 6; // poisonous cloud
   case 57: return 15; // fire storm
   case 58: return 2; // detect traps
   case 59: return 2; // random blink
   case 60: return 6; // Isk's blast
   case 61: return 6; // swarm
   case 62: return 12; // Summon Thing
   case 63: return 6; // Enslavement
   case 64: return 5; // Magic Mapping
   case 65: return 3; // heal other
   case 66: return 11; // Animate dead
   case 67: return 1; // Pain
   case 68: return 5; // Extension
   case 69: return 10; // Control Undead
   case 70: return 3; // animate skeleton
   case 71: return 5; // vampiric draining
   case 72: return 16; // summon greater undead
   case 73: return 3; // detect items
   case 74: return 9; // reviv
   default: return 2;

   }

   return 1;

   }
 */




char spell_value(unsigned char spell)
{

    switch (spell)
    {
    case SPELL_ANIMATE_SKELETON:
    case SPELL_ARC:
    case SPELL_BURN:
    case SPELL_CREATE_NOISE:
    case SPELL_CRUSH:
    case SPELL_DISRUPT:
    case SPELL_FREEZE:
    case SPELL_MAGIC_DART:
    case SPELL_PAIN:
    case SPELL_STING:
    case SPELL_SUMMON_SMALL_MAMMAL:
        return 1;

    case SPELL_BLINK:
    case SPELL_CORPSE_ROT:
    case SPELL_CURE_POISON_II:
    case SPELL_DETECT_CREATURES:
    case SPELL_DETECT_ITEMS:
    case SPELL_DETECT_TRAPS:
    case SPELL_FIRE_BRAND:
    case SPELL_FREEZING_AURA:
    case SPELL_LESSER_HEALING:
    case SPELL_LETHAL_INFUSION:
    case SPELL_LEVITATION:
    case SPELL_POISON_WEAPON:
    case SPELL_PROJECTED_NOISE:
    case SPELL_REPEL_MISSILES:
    case SPELL_RESTORE_DEXTERITY:
    case SPELL_RESTORE_INTELLIGENCE:
    case SPELL_RESTORE_STRENGTH:
    case SPELL_SUBLIMATION_OF_BLOOD:
    case SPELL_SWIFTNESS:
    case SPELL_THROW_FLAME:
    case SPELL_THROW_FROST:
    case SPELL_TUKIMAS_VORPAL_BLADE:
        return 2;

    case SPELL_ABJURATION_I:
    case SPELL_BERSERKER_RAGE:
    case SPELL_BONE_SHARDS:
    case SPELL_CALL_IMP:
    case SPELL_CONFUSE:
    case SPELL_CONJURE_FLAME:
    case SPELL_CURE_POISON_I:
    case SPELL_DETECT_CURSE:
    case SPELL_HEAL_OTHER:
    case SPELL_MEPHITIC_CLOUD:
    case SPELL_OZOCUBUS_ARMOUR:
    case SPELL_RECALL:
    case SPELL_REGENERATION:
    case SPELL_REPEL_UNDEAD:
    case SPELL_SELECTIVE_AMNESIA:
    case SPELL_SHOCK:
    case SPELL_SLOW:
    case SPELL_SPIDER_FORM:
    case SPELL_STONE_ARROW:
    case SPELL_TUKIMAS_DANCE:
    case SPELL_VAMPIRIC_DRAINING:
        return 3;

    case SPELL_ABJURATION_II:
    case SPELL_AIRSTRIKE:
    case SPELL_ANIMATE_DEAD:
    case SPELL_BLADE_HANDS:
    case SPELL_BOLT_OF_INACCURACY:
    case SPELL_CONTROL_TELEPORT:
    case SPELL_CONTROLLED_BLINK:
    case SPELL_DIG:
    case SPELL_DISPEL_UNDEAD:
    case SPELL_ENSLAVEMENT:
    case SPELL_FLY:
    case SPELL_ICE_BOLT:
    case SPELL_INSULATION:
    case SPELL_ISKENDERUNS_MYSTIC_BLAST:
    case SPELL_MAGIC_MAPPING:
    case SPELL_OLGREBS_TOXIC_RADIANCE:
    case SPELL_PARALYZE:
    case SPELL_PESTILENCE:
    case SPELL_POISON_AMMUNITION:
    case SPELL_RESIST_POISON:
    case SPELL_SMITING:
    case SPELL_STICKY_FLAME:
    case SPELL_SUMMON_ELEMENTAL:
    case SPELL_SUMMON_SCORPIONS:
        return 4;

    case SPELL_AGONY:
    case SPELL_BANISHMENT:
    case SPELL_BOLT_OF_COLD:
    case SPELL_BOLT_OF_FIRE:
    case SPELL_CAUSE_FEAR:
    case SPELL_CIGOTUVIS_DEGENERATION:
    case SPELL_EXTENSION:
    case SPELL_ICE_FORM:
    case SPELL_OZOCUBUS_REFRIGERATION:
    case SPELL_POLYMORPH_OTHER:
    case SPELL_PURIFICATION:
    case SPELL_REMOVE_CURSE:
    case SPELL_SHADOW_CREATURES:
    case SPELL_STATUE_FORM:
    case SPELL_SUMMON_DEMON:
    case SPELL_SUMMON_ICE_BEAST:
    case SPELL_TELEPORT_OTHER:
    case SPELL_TELEPORT_SELF:
    case SPELL_TWISTED_RESURRECTION:
    case SPELL_VENOM_BOLT:
        return 5;

    case SPELL_BOLT_OF_DRAINING:
    case SPELL_BOLT_OF_IRON:
    case SPELL_BORGNJORS_REVIVIFICATION:
    case SPELL_CONTROL_UNDEAD:
    case SPELL_DEFLECT_MISSILES:
    case SPELL_DEMONIC_HORDE:
    case SPELL_DISINTEGRATE:
    case SPELL_FIREBALL:
    case SPELL_GREATER_HEALING:
    case SPELL_IDENTIFY:
    case SPELL_INVISIBILITY:
    case SPELL_LIGHTNING_BOLT:
    case SPELL_MASS_CONFUSION:
    case SPELL_POISONOUS_CLOUD:
    case SPELL_STONEMAIL:
    case SPELL_SWARM:
    case SPELL_SYMBOL_OF_TORMENT:
    case SPELL_THUNDERBOLT:
        return 6;

    case SPELL_DEBUGGING_RAY:
    case SPELL_ALTER_SELF:
    case SPELL_FREEZING_CLOUD:
    case SPELL_GUARDIAN:
    case SPELL_HOLY_WORD:
    case SPELL_ORB_OF_ELECTROCUTION:
    case SPELL_ORB_OF_FRAGMENTATION:
    case SPELL_SHINING_LIGHT:
    case SPELL_SUMMON_GREATER_DEMON:
    case SPELL_SUMMON_WRAITHS:
    case SPELL_TOMB_OF_DOROKLOHE:
        return 7;

    case SPELL_DEATHS_DOOR:
    case SPELL_DRAGON_FORM:
    case SPELL_FLAME_OF_CLEANSING:
    case SPELL_HASTE:
    case SPELL_LEHUDIBS_CRYSTAL_SPEAR:
    case SPELL_NECROMUTATION:
    case SPELL_PORTAL:
    case SPELL_RING_OF_FLAMES:
    case SPELL_SUMMON_DAEVA:
    case SPELL_SUMMON_HORRIBLE_THINGS:
        return 8;

    case SPELL_DEATH_CHANNEL:
    case SPELL_FIRE_STORM:
    case SPELL_HELLFIRE:
    case SPELL_ICE_STORM:
        return 9;

    default:
        return 2;

    }

    return 1;

}




void spell_name(unsigned char spell, char spln[60])
{
    //char spln [30] = "";

    strcpy(spln, "Hello!");

    switch (spell)
    {
    case SPELL_IDENTIFY:
        strcpy(spln, "Identify");
        break;
    case SPELL_TELEPORT_SELF:
        strcpy(spln, "Teleport Self");
        break;
    case SPELL_CAUSE_FEAR:
        strcpy(spln, "Cause Fear");
        break;
    case SPELL_CREATE_NOISE:
        strcpy(spln, "Create Noise");
        break;
    case SPELL_REMOVE_CURSE:
        strcpy(spln, "Remove Curse");
        break;
    case SPELL_MAGIC_DART:
        strcpy(spln, "Magic Dart");
        break;
    case SPELL_FIREBALL:
        strcpy(spln, "Fireball");
        break;
    case SPELL_CONJURE_FLAME:
        strcpy(spln, "Conjure Flame");
        break;
    case SPELL_DIG:
        strcpy(spln, "Dig");
        break;
    case SPELL_BOLT_OF_FIRE:
        strcpy(spln, "Bolt of Fire");
        break;
    case SPELL_BOLT_OF_COLD:
        strcpy(spln, "Bolt of Cold");
        break;
    case SPELL_LIGHTNING_BOLT:
        strcpy(spln, "Lightning Bolt");
        break;
    case SPELL_POLYMORPH_OTHER:
        strcpy(spln, "Polymorph Other");
        break;
    case SPELL_SLOW:
        strcpy(spln, "Slow");
        break;
    case SPELL_HASTE:
        strcpy(spln, "Haste");
        break;
    case SPELL_PARALYZE:
        strcpy(spln, "Paralyze");
        break;
    case SPELL_CONFUSE:
        strcpy(spln, "Confuse");
        break;
    case SPELL_INVISIBILITY:
        strcpy(spln, "Invisibility");
        break;
    case SPELL_THROW_FLAME:
        strcpy(spln, "Throw Flame");
        break;
    case SPELL_THROW_FROST:
        strcpy(spln, "Throw Frost");
        break;
    case SPELL_CONTROLLED_BLINK:
        strcpy(spln, "Controlled Blink");
        break;
    case SPELL_FREEZING_CLOUD:
        strcpy(spln, "Freezing Cloud");
        break;
    case SPELL_MEPHITIC_CLOUD:
        strcpy(spln, "Mephitic Cloud");
        break;
    case SPELL_RING_OF_FLAMES:
        strcpy(spln, "Ring of Flames");
        break;
    case SPELL_RESTORE_STRENGTH:
        strcpy(spln, "Restore Strength");
        break;
    case SPELL_RESTORE_INTELLIGENCE:
        strcpy(spln, "Restore Intelligence");
        break;
    case SPELL_RESTORE_DEXTERITY:
        strcpy(spln, "Restore Dexterity");
        break;
    case SPELL_VENOM_BOLT:
        strcpy(spln, "Venom Bolt");
        break;
    case SPELL_OLGREBS_TOXIC_RADIANCE:
        strcpy(spln, "Olgreb's Toxic Radiance");
        break;
    case SPELL_TELEPORT_OTHER:
        strcpy(spln, "Teleport Other");
        break;
    case SPELL_LESSER_HEALING:
        strcpy(spln, "Lesser Healing");
        break;
    case SPELL_GREATER_HEALING:
        strcpy(spln, "Greater Healing");
        break;
    case SPELL_CURE_POISON_I:
        strcpy(spln, "Cure Poison");
        break;
    case SPELL_PURIFICATION:
        strcpy(spln, "Purification");
        break;
    case SPELL_DEATHS_DOOR:
        strcpy(spln, "Death's Door");
        break;
    case SPELL_SELECTIVE_AMNESIA:
        strcpy(spln, "Selective Amnesia");
        break;
    case SPELL_MASS_CONFUSION:
        strcpy(spln, "Mass Confusion");
        break;
    case SPELL_SMITING:
        strcpy(spln, "Smiting");
        break;
    case SPELL_REPEL_UNDEAD:
        strcpy(spln, "Repel Undead");
        break;
    case SPELL_HOLY_WORD:
        strcpy(spln, "Holy Word");
        break;
    case SPELL_DETECT_CURSE:
        strcpy(spln, "Detect Curse");
        break;
    case SPELL_SUMMON_SMALL_MAMMAL:
        strcpy(spln, "Summon Small Mammals");
        break;
    case SPELL_ABJURATION_I:
        strcpy(spln, "Abjuration");
        break;
    case SPELL_SUMMON_SCORPIONS:
        strcpy(spln, "Summon Scorpions");
        break;
    case SPELL_LEVITATION:
        strcpy(spln, "Levitation");
        break;
    case SPELL_BOLT_OF_DRAINING:
        strcpy(spln, "Bolt of Draining");
        break;
    case SPELL_LEHUDIBS_CRYSTAL_SPEAR:
        strcpy(spln, "Lehudib's Crystal Spear");
        break;
    case SPELL_BOLT_OF_INACCURACY:
      strcpy(spln, "Jagged Bolt"); //"Bolt of Inaccuracy");
        break;
    case SPELL_POISONOUS_CLOUD:
        strcpy(spln, "Poisonous Cloud");
        break;
    case SPELL_FIRE_STORM:
        strcpy(spln, "Fire Storm");
        break;
    case SPELL_DETECT_TRAPS:
        strcpy(spln, "Detect Traps");
        break;
    case SPELL_BLINK:
        strcpy(spln, "Blink");
        break;
    case SPELL_ISKENDERUNS_MYSTIC_BLAST:
        strcpy(spln, "Iskenderun's Mystic Blast");
        break; // this name was found in the hack.exe file of an early version of PCHACK. Credit goes to its creator (whoever that may be).
    case SPELL_SWARM:
        strcpy(spln, "Swarm");
        break;
    case SPELL_SUMMON_HORRIBLE_THINGS:
        strcpy(spln, "Summon Horrible Things");
        break;
    case SPELL_ENSLAVEMENT:
        strcpy(spln, "Enslavement");
        break;
    case SPELL_MAGIC_MAPPING:
        strcpy(spln, "Magic Mapping");
        break;
    case SPELL_HEAL_OTHER:
        strcpy(spln, "Heal Other");
        break;
    case SPELL_ANIMATE_DEAD:
        strcpy(spln, "Animate Dead");
        break;
    case SPELL_PAIN:
        strcpy(spln, "Pain");
        break;
    case SPELL_EXTENSION:
        strcpy(spln, "Extension");
        break;
    case SPELL_CONTROL_UNDEAD:
        strcpy(spln, "Control Undead");
        break;
    case SPELL_ANIMATE_SKELETON:
        strcpy(spln, "Animate Skeleton");
        break;
    case SPELL_VAMPIRIC_DRAINING:
        strcpy(spln, "Vampiric Draining");
        break;
    case SPELL_SUMMON_WRAITHS:
        strcpy(spln, "Summon Wraiths");
        break;
    case SPELL_DETECT_ITEMS:
        strcpy(spln, "Detect Items");
        break;
    case SPELL_BORGNJORS_REVIVIFICATION:
        strcpy(spln, "Borgnjor's Revivification");
        break;
    case SPELL_BURN:
        strcpy(spln, "Burn");
        break;
    case SPELL_FREEZE:
        strcpy(spln, "Freeze");
        break;
    case SPELL_SUMMON_ELEMENTAL:
        strcpy(spln, "Summon Elemental");
        break;
    case SPELL_OZOCUBUS_REFRIGERATION:
        strcpy(spln, "Ozocubu's Refrigeration");
        break;
    case SPELL_STICKY_FLAME:
        strcpy(spln, "Sticky Flame");
        break;
    case SPELL_SUMMON_ICE_BEAST:
        strcpy(spln, "Summon Ice Beast");
        break;
    case SPELL_OZOCUBUS_ARMOUR:
        strcpy(spln, "Ozocubu's Armour");
        break;
    case SPELL_CALL_IMP:
        strcpy(spln, "Call Imp");
        break;
    case SPELL_REPEL_MISSILES:
        strcpy(spln, "Repel Missiles");
        break;
    case SPELL_BERSERKER_RAGE:
        strcpy(spln, "Berserker Rage");
        break;
    case SPELL_DISPEL_UNDEAD:
        strcpy(spln, "Dispel Undead");
        break;
    case SPELL_GUARDIAN:
        strcpy(spln, "Guardian");
        break;
    case SPELL_PESTILENCE:
        strcpy(spln, "Pestilence");
        break;
    case SPELL_THUNDERBOLT:
        strcpy(spln, "Thunderbolt");
        break;
    case SPELL_FLAME_OF_CLEANSING:
        strcpy(spln, "Flame of Cleansing");
        break;
    case SPELL_SHINING_LIGHT:
        strcpy(spln, "Shining Light");
        break;
    case SPELL_SUMMON_DAEVA:
        strcpy(spln, "Summon Daeva");
        break;
    case SPELL_ABJURATION_II:
        strcpy(spln, "Abjuration");
        break;
    case SPELL_TWISTED_RESURRECTION:
        strcpy(spln, "Twisted Resurrection");
        break;
    case SPELL_REGENERATION:
        strcpy(spln, "Regeneration");
        break;
    case SPELL_BONE_SHARDS:
        strcpy(spln, "Bone Shards");
        break;
    case SPELL_BANISHMENT:
        strcpy(spln, "Banishment");
        break;
    case SPELL_CIGOTUVIS_DEGENERATION:
        strcpy(spln, "Cigotuvi's Degeneration");
        break;
    case SPELL_STING:
        strcpy(spln, "Sting");
        break;
    case SPELL_SUBLIMATION_OF_BLOOD:
        strcpy(spln, "Sublimation of Blood");
        break;
    case SPELL_TUKIMAS_DANCE:
        strcpy(spln, "Tukima's Dance");
        break;
    case SPELL_HELLFIRE:          // Staff of Dispater
        strcpy(spln, "Hellfire");
        break;
    case SPELL_SUMMON_DEMON:
        strcpy(spln, "Summon Demon");
        break;
    case SPELL_DEMONIC_HORDE:
        strcpy(spln, "Demonic Horde");
        break;
    case SPELL_SUMMON_GREATER_DEMON:
        strcpy(spln, "Summon Greater Demon");
        break;
    case SPELL_CORPSE_ROT:
        strcpy(spln, "Corpse Rot");
        break;
    case SPELL_TUKIMAS_VORPAL_BLADE:
        strcpy(spln, "Tukima's Vorpal Blade");
        break;
    case SPELL_FIRE_BRAND:
        strcpy(spln, "Fire Brand");
        break;
    case SPELL_FREEZING_AURA:
        strcpy(spln, "Freezing Aura");
        break;
    case SPELL_LETHAL_INFUSION:
        strcpy(spln, "Lethal Infusion");
        break;
    case SPELL_CRUSH:
        strcpy(spln, "Crush");
        break;
    case SPELL_BOLT_OF_IRON:
        strcpy(spln, "Bolt of Iron");
        break;
    case SPELL_STONE_ARROW:
        strcpy(spln, "Stone Arrow");
        break;
    case SPELL_TOMB_OF_DOROKLOHE:
        strcpy(spln, "Tomb of Doroklohe");
        break;
    case SPELL_STONEMAIL:
        strcpy(spln, "Stonemail");
        break;
    case SPELL_SHOCK:
        strcpy(spln, "Shock");
        break;
    case SPELL_SWIFTNESS:
        strcpy(spln, "Swiftness");
        break;
    case SPELL_FLY:
        strcpy(spln, "Fly");
        break;
    case SPELL_INSULATION:
        strcpy(spln, "Insulation");
        break;
    case SPELL_ORB_OF_ELECTROCUTION:
        strcpy(spln, "Orb of Electrocution");
        break;
    case SPELL_DETECT_CREATURES:
        strcpy(spln, "Detect creatures");
        break;
    case SPELL_CURE_POISON_II:
        strcpy(spln, "Cure Poison");
        break;
    case SPELL_CONTROL_TELEPORT:
        strcpy(spln, "Control Teleport");
        break;
    case SPELL_POISON_AMMUNITION:
        strcpy(spln, "Poison Ammunition");
        break;
    case SPELL_POISON_WEAPON:
        strcpy(spln, "Poison Weapon");
        break;
    case SPELL_RESIST_POISON:
        strcpy(spln, "Resist Poison");
        break;
    case SPELL_PROJECTED_NOISE:
        strcpy(spln, "Projected Noise");
        break;
    case SPELL_ALTER_SELF:
        strcpy(spln, "Alter Self");
        break;
    case SPELL_DEBUGGING_RAY:
        strcpy(spln, "Debugging ray");
        break;
    case SPELL_RECALL:
        strcpy(spln, "Recall");
        break;
    case SPELL_PORTAL:
        strcpy(spln, "Portal");
        break;
    case SPELL_AGONY:
        strcpy(spln, "Agony");
        break;
    case SPELL_SPIDER_FORM:
        strcpy(spln, "Spider Form");
        break;
    case SPELL_DISRUPT:
        strcpy(spln, "Disrupt");
        break;
    case SPELL_DISINTEGRATE:
        strcpy(spln, "Disintegrate");
        break;
    case SPELL_BLADE_HANDS:
        strcpy(spln, "Blade Hands");
        break;
    case SPELL_STATUE_FORM:
        strcpy(spln, "Statue Form");
        break;
    case SPELL_ICE_FORM:
        strcpy(spln, "Ice Form");
        break;
    case SPELL_DRAGON_FORM:
        strcpy(spln, "Dragon Form");
        break;
    case SPELL_NECROMUTATION:
        strcpy(spln, "Necromutation");
        break;
    case SPELL_DEATH_CHANNEL:
        strcpy(spln, "Death Channel");
        break;
    case SPELL_SYMBOL_OF_TORMENT:
        strcpy(spln, "Symbol of Torment");
        break;
    case SPELL_DEFLECT_MISSILES:
        strcpy(spln, "Deflect Missiles");
        break;
    case SPELL_ORB_OF_FRAGMENTATION:
        strcpy(spln, "Orb of Fragmentation");
        break;
    case SPELL_ICE_BOLT:
        strcpy(spln, "Ice Bolt");
        break;
    case SPELL_ICE_STORM:
        strcpy(spln, "Ice Storm");
        break;
    case SPELL_ARC:
        strcpy(spln, "Arc");
        break;
    case SPELL_AIRSTRIKE:
        strcpy(spln, "Airstrike");
        break;
    case SPELL_SHADOW_CREATURES:
        strcpy(spln, "Shadow Creatures");
        break;

        /* When adding enchantments, must add them to extension as well */
        /*
         *  spells to do:
         *  Contingency?
         *  Trigger contingency
         *  Preserve Corpses
         *  Permanency
         *  Ball Lightning
         *  Explosive rune?
         *  Fennel wands
         *  More summonings!
         */

    default:
        strcpy(spln, "another spell");
        break;
    }
}




char spellbook_contents(unsigned char plus, unsigned char type)
{
#ifdef DOS_TERM
    char buffer[4800];

    gettext(1, 1, 80, 25, buffer);

    window(1, 1, 80, 25);
#endif

    clrscr();

    int spell_types[10];

    spellbook_template(type, spell_types);

    char stringy[9];

    itoa(plus, stringy, 2);

    textcolor(LIGHTGREY);
    cprintf(" Spells                             Type                      Level" EOL);

    int spelcount = 0;

    for (int j = 1; j < 7; j++)
    {
        if (stringy[j] != 49)
          continue;

        cprintf(" ");
        bool knowsSpell = false;

        for (int i = 0; i < 25 && !knowsSpell; i++)
        {
          knowsSpell = you.spells[i] == spell_types[j];
        }

        //textcolor(knowsSpell ? LIGHTGREY : LIGHTBLUE);
        textcolor(knowsSpell ? DARKGREY : LIGHTGREY);

        char strng[2];

        if (spelcount < 26)
            strng[0] = (char) spelcount + 97;
        else
            strng[0] = (char) spelcount + 65;

        strng[1] = 0;
        cprintf(strng);
        cprintf(" - ");
        char st_pass[60];
        spell_name(spell_types[j], st_pass);
        cprintf(st_pass);
        gotoxy(35, wherey());
        char already = 0;

        if ( spell_type(spell_types[j], SPTYP_HOLY) )
          {
            cprintf("Holy");
            already = 1;
          }

        if ( spell_type(spell_types[j], SPTYP_POISON) )
          {
            print_slash(already);
            cprintf("Poison");
            already = 1;
          }

        if ( spell_type(spell_types[j], SPTYP_FIRE) )
          {
            print_slash(already);
            cprintf("Fire");
            already = 1;
          }

        if ( spell_type(spell_types[j], SPTYP_ICE) )
          {
            print_slash(already);
            cprintf("Ice");
            already = 1;
          }

        if ( spell_type(spell_types[j], SPTYP_EARTH) )
          {
            print_slash(already);
            cprintf("Earth");
            already = 1;
          }

        if ( spell_type(spell_types[j], SPTYP_AIR) )
          {
            print_slash(already);
            cprintf("Air");
            already = 1;
          }

        if ( spell_type(spell_types[j], SPTYP_CONJURATION) )
          {
            print_slash(already);
            cprintf("Conjuration");
            already = 1;
          }

        if (spell_type(spell_types[j], SPTYP_ENCHANTMENT) )
          {
            print_slash(already);
            cprintf("Enchantment");
            already = 1;
          }

        if (spell_type(spell_types[j], SPTYP_DIVINATION) )
          {
            print_slash(already);
            cprintf("Divination");
            already = 1;
          }

        if (spell_type(spell_types[j], SPTYP_TRANSLOCATION) )
          {
            print_slash(already);
            cprintf("Translocation");
            already = 1;
          }

        if (spell_type(spell_types[j], SPTYP_SUMMONING) )
          {
            print_slash(already);
            cprintf("Summoning");
            already = 1;
          }

        if (spell_type(spell_types[j], SPTYP_TRANSMIGRATION) )
          {
            print_slash(already);
            cprintf("Transmigration");
            already = 1;
          }

        if (spell_type(spell_types[j], SPTYP_NECROMANCY) )
          {
            print_slash(already);
            cprintf("Necromancy");
            already = 1;
          }

        gotoxy(65, wherey());
        char sval[2];
        itoa((int) spell_value(spell_types[j]), sval, 10);
        cprintf(sval);
        cprintf(EOL);
        spelcount++;
    }

    char keyn = getch();

    if (keyn == 0)
        getch();

#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
    window(1, 18, 80, 25);
#endif
    return keyn;
}


int which_spell_in_book(int sbook_type, int spl)
{
    int func_pass[10];
    spellbook_template(sbook_type, func_pass);
    return func_pass[spl];
}


int spell_hunger(int spell_value, unsigned char spell)
{
    switch (spell_value)
    {
    case 1:
        return 50;
    case 2:
        return 95;
    case 3:
        return 160;
    case 4:
        return 250;
    case 5:
        return 350;
    case 6:
        return 550;
    case 7:
        return 700;
    case 8:
        return 850;
    case 9:
        return 1000;
    case 10:
        return 1000;
    case 11:
        return 1100;
    case 12:
        return 1250;
    case 13:
        return 1380;
    case 14:
        return 1500;
    case 15:
        return 1600;
    default:
        return 1600 + spell_value * 20;
    }

    return spell;
}          // end of spell_hunger()




char undead_can_memorise(unsigned char spell)
{

    switch (spell)
    {
    //case SPELL_REGENERATION:
    case SPELL_BORGNJORS_REVIVIFICATION:
    case SPELL_CURE_POISON_II:
    case SPELL_DEATHS_DOOR:
    case SPELL_NECROMUTATION:
    case SPELL_RESIST_POISON:
    case SPELL_SYMBOL_OF_TORMENT:
        return 2;

    case SPELL_ALTER_SELF:
    case SPELL_BLADE_HANDS:
    case SPELL_DRAGON_FORM:
    case SPELL_ICE_FORM:
    case SPELL_SPIDER_FORM:
    case SPELL_STATUE_FORM:
    case SPELL_SUMMON_HORRIBLE_THINGS:
        return 1;

    default:
        return 0;
    }

}

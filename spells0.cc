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
#include "enum.h"

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
int spell_type(unsigned char spell, unsigned char typy);
int spell_spec(int spell, int power);
char spell_fail(unsigned char spell);
void print_slash(char already);
char spell_list(void);


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

        if (you.spells[j] != 210)
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
            if (spell_type(you.spells[j], SPTYP_HOLY) == 1) {
                cprintf("Holy");
                already = 1;
            }
            if (spell_type(you.spells[j], SPTYP_POISON) == 1) {
                print_slash(already);
                cprintf("Poison");
                already = 1;
            }
            if (spell_type(you.spells[j], SPTYP_FIRE) == 1) {
                print_slash(already);
                cprintf("Fire");
                already = 1;
            }
            if (spell_type(you.spells[j], SPTYP_ICE) == 1) {
                print_slash(already);
                cprintf("Ice");
                already = 1;
            }
            if (spell_type(you.spells[j], SPTYP_EARTH) == 1) {
                print_slash(already);
                cprintf("Earth");
                already = 1;
            }
            if (spell_type(you.spells[j], SPTYP_AIR) == 1) {
                print_slash(already);
                cprintf("Air");
                already = 1;
            }
            if (spell_type(you.spells[j], SPTYP_CONJURATION) == 1) {
                print_slash(already);
                cprintf("Conjuration");
                already = 1;
            }
            if (spell_type(you.spells[j], SPTYP_ENCHANTMENT) == 1) {
                print_slash(already);
                cprintf("Enchantment");
                already = 1;
            }
            if (spell_type(you.spells[j], SPTYP_DIVINATION) == 1) {
                print_slash(already);
                cprintf("Divination");
                already = 1;
            }
            if (spell_type(you.spells[j], SPTYP_TRANSLOCATION) == 1) {
                print_slash(already);
                cprintf("Translocation");
                already = 1;
            }
            if (spell_type(you.spells[j], SPTYP_SUMMONING) == 1) {
                print_slash(already);
                cprintf("Summoning");
                already = 1;
            }
            if (spell_type(you.spells[j], SPTYP_TRANSMIGRATION) == 1) {
                print_slash(already);
                cprintf("Transmigration");
                already = 1;
            }
            if (spell_type(you.spells[j], SPTYP_NECROMANCY) == 1) {
                print_slash(already);
                cprintf("Necromancy");
                already = 1;
            }

/*
 *  types of spells:
 *  11 = conjuration
 *  12 = enchantment
 *  13 = fire
 *  14 = ice
 *  15 = transmigration
 *  16 = necromancy
 *  17 = holy
 *  18 = summoning
 *  19 = divination
 *  20 = translocation
 *  21 = poison
 */
            char sval[4];
            gotoxy(58, wherey());
            int spell_f = spell_fail(you.spells[j]);

            if (spell_f == 100) {
              cprintf("Useless");
            }
            else if (spell_f > 90) {
              cprintf("Terrible");
            }
            else if (spell_f > 80) {
              cprintf("Cruddy");
            }
            else if (spell_f > 70) {
              cprintf("Bad");
            }
            else if (spell_f > 60) {
              cprintf("Very Poor");
            }
            else if (spell_f > 50) {
              cprintf("Poor");
            }
            else if (spell_f > 40) {
              cprintf("Fair");
            }
            else if (spell_f > 30) {
              cprintf("Good");
            }
            else if (spell_f > 20) {
              cprintf("Very Good");
            }
            else if (spell_f > 10) {
              cprintf("Great");
            }
            else if (spell_f > 0) {
              cprintf("Excellent");
            }
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
      armour += (abs(property(OBJ_ARMOUR,
                              you.inv_type[you.equip[EQ_BODY_ARMOUR]],
                              PARM_EVASION)) * 5) - 2;
      if (armour > 0)
        armour -= you.skills[SK_ARMOUR] * (you.strength / 5 + 1);
      if (armour > 0)
        chance += armour;
    }

    if (you.equip[EQ_WEAPON] != -1
                    && you.inv_class[you.equip[EQ_WEAPON]] != OBJ_STAVES)
    {
        int wpn_penalty = 0;

        if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS)
        {
            wpn_penalty = (5 * (property(OBJ_WEAPONS,
                                        you.inv_type[you.equip[EQ_WEAPON]],
                                        PWPN_SPEED) - 12)) / 2;
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

    if (   you.religion == GOD_VEHUMET
        && you.duration[DUR_PRAYER] != 0
        && you.piety >= 50
        && (spell_type(spell, 11) != 0
        || spell_type(spell, 18) != 0))
    {
      chance2 /= 2;
    }

    if (you.duration[DUR_TRANSFORMATION] > 0
        && you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS) {
      chance2 += 20;
    }
    return chance2;
}


int spell_spec(int spell, int power)
{

    int enhanced = 0;


    power = 0;

    int s = 0;
    int spellsy = 0;

    for (s = 11; s < 24; s++)
    {
// if (s == 13 || s == 14 || s == 17) continue;
        if (s == 17)
            continue;
        if (spell_type(spell, s) == 1)
            spellsy++;
    }

    if (spellsy != 0)
    {
        if (spell_type(spell, SPTYP_CONJURATION) == 1)
        {
            power += (you.skills[SK_CONJURATIONS] * 2) / spellsy;
        }
        if (spell_type(spell, SPTYP_ENCHANTMENT) == 1)
        {
            power += (you.skills[SK_ENCHANTMENTS] * 2) / spellsy;
        }
        if (spell_type(spell, SPTYP_TRANSMIGRATION) == 1)
        {
            power += (you.skills[SK_TRANSMIGRATION] * 2) / spellsy;
        }
        if (spell_type(spell, SPTYP_NECROMANCY) == 1)
        {
            power += (you.skills[SK_NECROMANCY] * 2) / spellsy;
        }
        if (spell_type(spell, SPTYP_SUMMONING) == 1)
        {
            power += (you.skills[SK_SUMMONINGS] * 2) / spellsy;
        }
        if (spell_type(spell, SPTYP_DIVINATION) == 1)
        {
            power += (you.skills[SK_DIVINATIONS] * 2) / spellsy;
        }
        if (spell_type(spell, SPTYP_TRANSLOCATION) == 1)
        {
            power += (you.skills[SK_TRANSLOCATIONS] * 2) / spellsy;
        }
        if (spell_type(spell, SPTYP_POISON) == 1)
        {
            power += (you.skills[SK_POISON_MAGIC] * 2) / spellsy;
        }
        if (spell_type(spell, SPTYP_EARTH) == 1)
        {
            power += (you.skills[SK_EARTH_MAGIC] * 2) / spellsy;
        }
        if (spell_type(spell, SPTYP_AIR) == 1)
        {
            power += (you.skills[SK_AIR_MAGIC] * 2) / spellsy;
        }
        if (spell_type(spell, SPTYP_FIRE) == 1)
        {
            power += (you.skills[SK_FIRE_MAGIC] * 2) / spellsy;
        }
        if (spell_type(spell, SPTYP_ICE) == 1)
        {
            power += (you.skills[SK_ICE_MAGIC] * 2) / spellsy;
        }
    }                           // end if spellsy != 0

    power += (you.skills[SK_SPELLCASTING] * 5) / 10;

    power += player_mag_abil(); // currently only affected by staves/rings of wizardry


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
 */

    if (spell_type(spell, SPTYP_NECROMANCY) == 1)
    {
        enhanced += player_spec_death();
        enhanced -= player_spec_holy();
//  naughty(spell_value(spell) * 2, 1);
    }                           // necromancy

    if (spell_type(spell, SPTYP_FIRE) == 1)
    {
        enhanced += player_spec_fire();
        enhanced -= player_spec_cold();
    }                           // fire

    if (spell_type(spell, SPTYP_ICE) == 1)
    {
        enhanced += player_spec_cold();
        enhanced -= player_spec_fire();
    }                           // ice

    if (spell_type(spell, SPTYP_HOLY) == 1)
    {
        enhanced -= player_spec_death();
        enhanced += player_spec_holy();
        if (you.special_wield == 50)
            enhanced -= 2;
    }                           // holy - signs switched!

    if (spell_type(spell, SPTYP_CONJURATION) == 1)
    {
        enhanced += player_spec_conj();
    }
    if (spell_type(spell, SPTYP_ENCHANTMENT) == 1)
    {
        enhanced += player_spec_ench();
    }
    if (spell_type(spell, SPTYP_SUMMONING) == 1)
    {
        enhanced += player_spec_summ();
    }
    if (spell_type(spell, SPTYP_POISON) == 1)
    {
        enhanced += player_spec_poison();
    }

    if (spell_type(spell, SPTYP_EARTH) == 1)
    {
        enhanced += player_spec_earth();        // spec_earth
        enhanced -= player_spec_air();          // spec_air

    }                           // earth

    if (spell_type(spell, SPTYP_AIR) == 1)
    {
        enhanced -= player_spec_earth();        // spec_earth
        enhanced += player_spec_air();          // spec_air
    }                           // air

/* If the bit just above changes, remember to also change it in crawl99.cc */

/*
   types of you.spells:
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
 */

/*
   if (enhanced == 1)
   {
   strcpy(info, "You feel a surge of power.");
   incrl();
   }
   if (enhanced == 2)
   {
   strcpy(info, "You feel a strong surge of power.");
   incrl();
   }
   if (enhanced >= 3) // how would you get this???
   {
   strcpy(info, "You feel a huge surge of power!");
   incrl();
   }
   if (enhanced == -1)
   {
   strcpy(info, "You feel a numb sensation.");
   incrl();
   }
   if (enhanced == -2)
   {
   strcpy(info, "You feel an extremely numb sensation.");
   incrl();
   }
   if (enhanced <= -3)
   {
   strcpy(info, "You feel an extraordinarily numb sensation.");
   incrl();
   }
 */

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
            power /= 2;
//  power /= 10;
        }
    }

    return power;

}                               // end int spell_spec






int spell_type(unsigned char spell, unsigned char typy)
{
    unsigned long stype = 0;

    switch (spell)
    {
    case SPELL_IDENTIFY:
        stype = 19;
        break;                  //6; // identify

    case SPELL_TELEPORT_SELF:
        stype = 20;
        break;                  //5; // teleportation

    case SPELL_CAUSE_FEAR:
        stype = 12;
        break;                  //4; // cause fear

    case SPELL_CREATE_NOISE:
        stype = 12;
        break;                  //1; // noise

    case SPELL_REMOVE_CURSE:
        stype = 12;
        break;                  //5; // remove curse

    case SPELL_MAGIC_DART:
        stype = 11;
        break;                  //1; // magic missile

    case SPELL_FIREBALL:
        stype = 1311;
        break;                  //5; // fireball

    case SPELL_CONJURE_FLAME:
        stype = 231311;
        break;                  //3; // conjure flame

    case SPELL_DIG:
        stype = 1522;
        break;                  //4; // dig

    case SPELL_BOLT_OF_FIRE:
        stype = 1311;
        break;                  //5; // firebolt

    case SPELL_BOLT_OF_COLD:
        stype = 1411;
        break;                  //5; // bolt of freezing cold

    case SPELL_LIGHTNING_BOLT:
        stype = 2311;
        break;                  //6; // bolt of lightning

    case SPELL_POLYMORPH_OTHER:
        stype = 1512;
        break;                  //6; // polymorph other

    case SPELL_SLOW:
        stype = 12;
        break;                  //3;

    case SPELL_HASTE:
        stype = 12;
        break;                  //4;

    case SPELL_PARALYZE:
        stype = 12;
        break;                  //4;

    case SPELL_CONFUSE:
        stype = 12;
        break;                  //3;

    case SPELL_INVISIBILITY:
        stype = 12;
        break;                  //4; // invisibility

    case SPELL_THROW_FLAME:
        stype = 1311;
        break;                  //2; // throw flame

    case SPELL_THROW_FROST:
        stype = 1411;
        break;                  //2; // throw frost

    case SPELL_CONTROLLED_BLINK:
        stype = 20;
        break;                  //4; // blink

    case SPELL_FREEZING_CLOUD:
        stype = 231411;
        break;                  //7; // freezing cloud

    case SPELL_MEPHITIC_CLOUD:
        stype = 232111;
        break;                  //3; // stinking cloud

    case SPELL_RING_OF_FLAMES:
        stype = 1312;
        break;                  //8; // ring of flames

    case SPELL_RESTORE_STRENGTH:
        stype = 17;
        break;                  //2; //  restore strength:

    case SPELL_RESTORE_INTELLIGENCE:
        stype = 17;
        break;                  //2; //          int

    case SPELL_RESTORE_DEXTERITY:
        stype = 17;
        break;                  //2; //          dex

    case SPELL_VENOM_BOLT:
        stype = 2111;
        break;                  //5; // venom bolt

    case SPELL_OLGREBS_TOXIC_RADIANCE:
        stype = 21;
        break;                  //4; // toxic radiance - uses lots of food?

    case SPELL_TELEPORT_OTHER:
        stype = 20;
        break;                  //5; // teleport other

    case SPELL_LESSER_HEALING:
        stype = 17;
        break;                  //2; // lesser healing

    case SPELL_GREATER_HEALING:
        stype = 17;
        break;                  //6; // greater healing

    case SPELL_CURE_POISON_I:
        stype = 17;
        break;                  //3; // cure poison

    case SPELL_PURIFICATION:
        stype = 17;
        break;                  //5; // purification

    case SPELL_DEATHS_DOOR:
        stype = 1612;
        break;                  //8; // death's door

    case SPELL_SELECTIVE_AMNESIA:
        stype = 12;
        break;                  //3; // selective amnesia

    case SPELL_MASS_CONFUSION:
        stype = 12;
        break;                  //7; // mass confusion

    case SPELL_SMITING:
        stype = 17;
        break;                  //4; // smiting

    case SPELL_REPEL_UNDEAD:
        stype = 17;
        break;                  //3; // repel undead

    case SPELL_HOLY_WORD:
        stype = 17;
        break;                  //7; // holy word

    case SPELL_DETECT_CURSE:
        stype = 19;
        break;                  //3; // detect curse

    case SPELL_SUMMON_SMALL_MAMMAL:
        stype = 18;
        break;                  //1; // summon small mammal

    case SPELL_ABJURATION_I:
        stype = 18;
        break;                  //4; // Abjuration

    case SPELL_SUMMON_SCORPIONS:
        stype = 2118;
        break;                  //4; // summon scorpions

    case SPELL_LEVITATION:
        stype = 1223;
        break;                  //3; // levitation

    case SPELL_BOLT_OF_DRAINING:
        stype = 1611;
        break;                  //6; // bolt of draining

    case SPELL_LEHUDIBS_CRYSTAL_SPEAR:
        stype = 1122;
        break;                  //8; // splinters

    case SPELL_BOLT_OF_INACCURACY:
        stype = 11;
        break;                  //4; // innacuracy

    case SPELL_POISONOUS_CLOUD:
        stype = 232111;
        break;                  //6; // poisonous cloud

    case SPELL_FIRE_STORM:
        stype = 1311;
        break;                  //8; // fire storm

    case SPELL_DETECT_TRAPS:
        stype = 19;
        break;                  //2; // detect traps

    case SPELL_BLINK:
        stype = 20;
        break;                  //2; // random blink

    case SPELL_ISKENDERUNS_MYSTIC_BLAST:
        stype = 11;
        break;                  //5; // Isk's blast

    case SPELL_SWARM:
        stype = 18;
        break;                  //5; // swarm

    case SPELL_SUMMON_HORRIBLE_THINGS:
        stype = 18;
        break;                  //8; // Summon Thing

    case SPELL_ENSLAVEMENT:
        stype = 12;
        break;                  //4; // Enslavement

    case SPELL_MAGIC_MAPPING:
        stype = 1922;
        break;                  //4; // Magic Mapping

    case SPELL_HEAL_OTHER:
        stype = 17;
        break;                  //3; // heal other

    case SPELL_ANIMATE_DEAD:
        stype = 16;
        break;                  //7; // Animate dead

    case SPELL_PAIN:
        stype = 16;
        break;                  //1; // Pain

    case SPELL_EXTENSION:
        stype = 12;
        break;                  //4; // Extension

    case SPELL_CONTROL_UNDEAD:
        stype = 1612;
        break;                  //6; // Control Undead

    case SPELL_ANIMATE_SKELETON:
        stype = 16;
        break;                  //3; // animate skeleton

    case SPELL_VAMPIRIC_DRAINING:
        stype = 16;
        break;                  //3; // vampiric draining

    case SPELL_SUMMON_WRAITHS:
        stype = 1618;
        break;                  // summon greater undead

    case SPELL_DETECT_ITEMS:
        stype = 19;
        break;                  // detect items

    case SPELL_BORGNJORS_REVIVIFICATION:
        stype = 16;
        break;

    case SPELL_BURN:
        stype = 13;
        break;                  // burn

    case SPELL_FREEZE:
        stype = 14;
        break;                  // freeze

    case SPELL_SUMMON_ELEMENTAL:
        stype = 18;
        break;                  // Summon elemental

    case SPELL_OZOCUBUS_REFRIGERATION:
        stype = 14;
        break;                  // refrigeration

    case SPELL_STICKY_FLAME:
        stype = 1311;
        break;                  // Sticky flame

    case SPELL_SUMMON_ICE_BEAST:
        stype = 1418;
        break;                  // ice beast

    case SPELL_OZOCUBUS_ARMOUR:
        stype = 1412;
        break;                  // ice armour

    case SPELL_CALL_IMP:
        stype = 18;
        break;                  // imp

    case SPELL_REPEL_MISSILES:
        stype = 1223;
        break;                  // repel missiles

    case SPELL_BERSERKER_RAGE:
        stype = 12;
        break;                  // berserk

    case SPELL_DISPEL_UNDEAD:
        stype = 16;
        break;                  // dispel undead

    case SPELL_GUARDIAN:
        stype = 17;
        break;                  // Guardian

    case SPELL_PESTILENCE:
        stype = 17;
        break;                  // Pestilence

    case SPELL_THUNDERBOLT:
        stype = 1723;
        break;                  // Thunderbolt

    case SPELL_FLAME_OF_CLEANSING:
        stype = 17;
        break;                  // Flame of Cleansing

    case SPELL_SHINING_LIGHT:
        stype = 17;
        break;                  // Shining Light

    case SPELL_SUMMON_DAEVA:
        stype = 17;
        break;                  // Summon Daeva

    case SPELL_ABJURATION_II:
        stype = 17;
        break;                  // Abjuration

    case SPELL_TWISTED_RESURRECTION:
        stype = 16;
        break;                  // twisted res

    case SPELL_REGENERATION:
        stype = 1612;
        break;                  // regen

    case SPELL_BONE_SHARDS:
        stype = 16;
        break;                  // bone shards

    case SPELL_BANISHMENT:
        stype = 20;
        break;                  // Banishment

    case SPELL_CIGOTUVIS_DEGENERATION:
        stype = 1516;
        break;                  // degeneration

    case SPELL_STING:
        stype = 1121;
        break;                  // sting

    case SPELL_SUBLIMATION_OF_BLOOD:
        stype = 16;
        break;                  // blood

    case SPELL_TUKIMAS_DANCE:
        stype = 12;
        break;                  // dance

    case SPELL_HELLFIRE:
        stype = 1113;
        break;                  // hellfire

    case SPELL_SUMMON_DEMON:
        stype = 18;
        break;                  // summon demon

    case SPELL_DEMONIC_HORDE:
        stype = 18;
        break;                  // demonic horde

    case SPELL_SUMMON_GREATER_DEMON:
        stype = 18;
        break;                  // summon greater demon

    case SPELL_CORPSE_ROT:
        stype = 16;
        break;                  // corpse rot

    case SPELL_TUKIMAS_VORPAL_BLADE:
        stype = 12;
        break;                  // Tukima's blade

    case SPELL_FIRE_BRAND:
        stype = 1213;
        break;                  // Fire brand

    case SPELL_FREEZING_AURA:
        stype = 1214;
        break;                  // Ice brand

    case SPELL_LETHAL_INFUSION:
        stype = 1216;
        break;                  // weapon of draining

    case SPELL_CRUSH:
        stype = 22;
        break;                  // crush

    case SPELL_BOLT_OF_IRON:
        stype = 1122;
        break;                  // bolt of iron

    case SPELL_STONE_ARROW:
        stype = 1122;
        break;                  // stone arrow

    case SPELL_TOMB_OF_DOROKLOHE:
      stype = 1522;          //jmf: changed from 1122
        break;                  // tomb of doro

    case SPELL_STONEMAIL:
        stype = 1222;
        break;                  // stonemail

    case SPELL_SHOCK:
        stype = 1123;
        break;                  // shock

    case SPELL_SWIFTNESS:
        stype = 1223;
        break;                  // swiftness

    case SPELL_FLY:
        stype = 1223;
        break;                  // flight

    case SPELL_INSULATION:
        stype = 1223;
        break;                  // insulation

    case SPELL_ORB_OF_ELECTROCUTION:
        stype = 1123;
        break;                  // orb of elec

    case SPELL_DETECT_CREATURES:
        stype = 19;
        break;                  // detect creat

    case SPELL_CURE_POISON_II:
        stype = 21;
        break;                  // cure poison (poison)

    case SPELL_CONTROL_TELEPORT:
        stype = 1220;
        break;                  // control teleport

    case SPELL_POISON_AMMUNITION:
        stype = 2112;
        break;                  // poison ammunition

    case SPELL_POISON_WEAPON:
        stype = 2112;
        break;                  // poison weapon

    case SPELL_RESIST_POISON:
        stype = 2112;
        break;                  // resist poison

    case SPELL_PROJECTED_NOISE:
        stype = 12;
        break;                  // noise 2

    case SPELL_ALTER_SELF:
        stype = 15;
        break;                  // alter self

    case SPELL_DEBUGGING_RAY:
        stype = 11;
        break;                  // debug ray

    case SPELL_RECALL:
        stype = 1820;
        break;                  // recall

    case SPELL_PORTAL:
        stype = 20;
        break;                  // Portal

    case SPELL_AGONY:
        stype = 16;
        break;                  // Agony

    case SPELL_SPIDER_FORM:
        stype = 1521;
        break;                  // Spider form

    case SPELL_DISRUPT:
        stype = 15;
        break;                  // Disrupt

    case SPELL_DISINTEGRATE:
        stype = 15;
        break;                  // Disintegrate

    case SPELL_BLADE_HANDS:
        stype = 15;
        break;                  // Blade Hands

    case SPELL_STATUE_FORM:
        stype = 2215;
        break;                  // Statue

    case SPELL_ICE_FORM:
        stype = 1415;
        break;                  // Ice Form

    case SPELL_DRAGON_FORM:
        stype = 1315;
        break;                  // Dragon Form

    case SPELL_NECROMUTATION:
        stype = 1615;
        break;                  // Lich Form

    case SPELL_DEATH_CHANNEL:
        stype = 16;
        break;                  // Death channel

    case SPELL_SYMBOL_OF_TORMENT:
        stype = 16;
        break;                  // Symbol of Torment

    case SPELL_DEFLECT_MISSILES:
        stype = 1223;
        break;                  // deflect missiles

    case SPELL_ORB_OF_FRAGMENTATION:
        stype = 1122;
        break;
    case SPELL_ICE_BOLT:
        stype = 1114;
        break;
    case SPELL_ICE_STORM:
        stype = 1114;
        break;
    case SPELL_ARC:
        stype = 23;
        break;
    case SPELL_AIRSTRIKE:
        stype = 23;
        break;
    case SPELL_SHADOW_CREATURES:
      stype = 1811;  //jmf: was just 18
      break;


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



        // Make restore abilities necromancy?

    default:
        stype = 0;
        break;
//default: return 2;

    }

//if (stype == typy || (stype / 10) % 10 == typy || (stype / 100) % 10 == typy) return 1;

//if (stype == typy || (stype / 10) % 10 == typy || (stype / 100) % 10 == typy || (stype / 1000) % 10 == typy || (stype / 10000) % 10 == typy) return 1;

    if (stype % 100 == typy || (stype / 100) % 100 == typy || (stype / 10000) % 100 == typy)
        return 1;

//232111

//if (stype % 100 == typy || stype / 100 == typy) return 1;

    return 0;

}                               // end spell_type

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
    case SPELL_IDENTIFY:
        return 6;               // identify

    case SPELL_TELEPORT_SELF:
        return 5;               // teleportation

    case SPELL_CAUSE_FEAR:
        return 5;               // cause fear

    case SPELL_CREATE_NOISE:
        return 1;               // noise

    case SPELL_REMOVE_CURSE:
        return 5;               // remove curse

    case SPELL_MAGIC_DART:
        return 1;               // magic missile

    case SPELL_FIREBALL:
        return 6;               // fireball

    case SPELL_CONJURE_FLAME:
        return 3;               // conjure flame

    case SPELL_DIG:
        return 4;               // dig

    case SPELL_BOLT_OF_FIRE:
        return 5;               // firebolt

    case SPELL_BOLT_OF_COLD:
        return 5;               // bolt of freezing cold

    case SPELL_LIGHTNING_BOLT:
        return 6;               // bolt of lightning

    case SPELL_POLYMORPH_OTHER:
        return 5;               // polymorph other

    case SPELL_SLOW:
        return 3;               // slow

    case SPELL_HASTE:
        return 8;               // haste

    case SPELL_PARALYZE:
        return 4;               // paralyse

    case SPELL_CONFUSE:
        return 3;               // confuse

    case SPELL_INVISIBILITY:
        return 6;               // invisibility

    case SPELL_THROW_FLAME:
        return 2;               // throw flame

    case SPELL_THROW_FROST:
        return 2;               // throw frost

    case SPELL_CONTROLLED_BLINK:
        return 4;               // controlledblink

    case SPELL_FREEZING_CLOUD:
        return 7;               // freezing cloud

    case SPELL_MEPHITIC_CLOUD:
        return 3;               // stinking cloud

    case SPELL_RING_OF_FLAMES:
        return 8;               // ring of flames

    case SPELL_RESTORE_STRENGTH:
        return 2;               //  restore strength:

    case SPELL_RESTORE_INTELLIGENCE:
        return 2;               //          int

    case SPELL_RESTORE_DEXTERITY:
        return 2;               //          dex

    case SPELL_VENOM_BOLT:
        return 5;               // venom bolt

    case SPELL_OLGREBS_TOXIC_RADIANCE:
        return 4;               // toxic radiance

    case SPELL_TELEPORT_OTHER:
        return 5;               // teleport other

    case SPELL_LESSER_HEALING:
        return 2;               // lesser healing

    case SPELL_GREATER_HEALING:
        return 6;               // greater healing

    case SPELL_CURE_POISON_I:
        return 3;               // cure poison

    case SPELL_PURIFICATION:
        return 5;               // purification

    case SPELL_DEATHS_DOOR:
        return 8;               // death's door

    case SPELL_SELECTIVE_AMNESIA:
        return 3;               // selective amnesia

    case SPELL_MASS_CONFUSION:
        return 6;               // mass confusion

    case SPELL_SMITING:
        return 4;               // smiting??

    case SPELL_REPEL_UNDEAD:
        return 3;               // repel undead

    case SPELL_HOLY_WORD:
        return 7;               // holy word

    case SPELL_DETECT_CURSE:
        return 3;               // detect curse

    case SPELL_SUMMON_SMALL_MAMMAL:
        return 1;               // summon small mammal

    case SPELL_ABJURATION_I:
        return 3;               // Abjuration

    case SPELL_SUMMON_SCORPIONS:
        return 4;               // summon scorpions

    case SPELL_LEVITATION:
        return 2;               // levitation

    case SPELL_BOLT_OF_DRAINING:
        return 6;               // bolt of draining

    case SPELL_LEHUDIBS_CRYSTAL_SPEAR:
        return 8;               // crystal spear

    case SPELL_BOLT_OF_INACCURACY:
        return 4;               // innacuracy

    case SPELL_POISONOUS_CLOUD:
        return 6;               // poisonous cloud

    case SPELL_FIRE_STORM:
        return 9;               // fire storm

    case SPELL_DETECT_TRAPS:
        return 2;               // detect traps

    case SPELL_BLINK:
        return 2;               // random blink

    case SPELL_ISKENDERUNS_MYSTIC_BLAST:
        return 4;               // Isk's blast

    case SPELL_SWARM:
        return 6;               // swarm

    case SPELL_SUMMON_HORRIBLE_THINGS:
        return 8;               // Summon Thing

    case SPELL_ENSLAVEMENT:
        return 4;               // Enslavement

    case SPELL_MAGIC_MAPPING:
        return 4;               // Magic Mapping

    case SPELL_HEAL_OTHER:
        return 3;               // heal other

    case SPELL_ANIMATE_DEAD:
        return 4;               // Animate dead

    case SPELL_PAIN:
        return 1;               // Pain

    case SPELL_EXTENSION:
        return 5;               // Extension

    case SPELL_CONTROL_UNDEAD:
        return 6;               // Control Undead

    case SPELL_ANIMATE_SKELETON:
        return 1;               // animate skeleton

    case SPELL_VAMPIRIC_DRAINING:
        return 3;               // vampiric draining

    case SPELL_SUMMON_WRAITHS:
        return 7;               // summon wraiths

    case SPELL_DETECT_ITEMS:
        return 2;               // detect items

    case SPELL_BORGNJORS_REVIVIFICATION:
        return 6;               // reviv

    case SPELL_BURN:
        return 1;               // burn

    case SPELL_FREEZE:
        return 1;               // freeze

    case SPELL_SUMMON_ELEMENTAL:
        return 4;               // Summon elemental

    case SPELL_OZOCUBUS_REFRIGERATION:
        return 5;               // refrigeration

    case SPELL_STICKY_FLAME:
        return 4;               // Sticky flame

    case SPELL_SUMMON_ICE_BEAST:
        return 5;               // Ice beast

    case SPELL_OZOCUBUS_ARMOUR:
        return 3;               // Ozocubu's Armour

    case SPELL_CALL_IMP:
        return 3;               // imp

    case SPELL_REPEL_MISSILES:
        return 2;               // repel missiles

    case SPELL_BERSERKER_RAGE:
        return 3;               // berserker

    case SPELL_DISPEL_UNDEAD:
        return 4;               // dispel undead

    case SPELL_GUARDIAN:
        return 7;               // Guardian

    case SPELL_PESTILENCE:
        return 4;               // Pestilence

    case SPELL_THUNDERBOLT:
        return 6;               // Thunderbolt

    case SPELL_FLAME_OF_CLEANSING:
        return 8;               // Flame of Cleansing

    case SPELL_SHINING_LIGHT:
        return 7;               // Shining Light

    case SPELL_SUMMON_DAEVA:
        return 8;               // Summon Daeva

    case SPELL_ABJURATION_II:
        return 4;               // Abjuration

    case SPELL_TWISTED_RESURRECTION:
        return 5;               // twisted res

    case SPELL_REGENERATION:
        return 3;               // regen

    case SPELL_BONE_SHARDS:
        return 3;               // bone shards

    case SPELL_BANISHMENT:
        return 5;               // Banishment

    case SPELL_CIGOTUVIS_DEGENERATION:
        return 5;               // Degeneration

    case SPELL_STING:
        return 1;               // sting

    case SPELL_SUBLIMATION_OF_BLOOD:
        return 2;               // blood

    case SPELL_TUKIMAS_DANCE:
        return 3;               // dance

    case SPELL_HELLFIRE:
        return 9;               // hellfire

    case SPELL_SUMMON_DEMON:
        return 5;               // summon demon

    case SPELL_DEMONIC_HORDE:
        return 6;               // demonic horde

    case SPELL_SUMMON_GREATER_DEMON:
        return 7;               // summon greater demon

    case SPELL_CORPSE_ROT:
        return 2;               // corpse rot

    case SPELL_TUKIMAS_VORPAL_BLADE:
        return 2;               // Tukima's v bl

    case SPELL_FIRE_BRAND:
        return 2;               // flaming weapon

    case SPELL_FREEZING_AURA:
        return 2;               // freezing weapon

    case SPELL_LETHAL_INFUSION:
        return 2;               // draining weapon

    case SPELL_CRUSH:
        return 1;               // throw pebble

    case SPELL_BOLT_OF_IRON:
        return 6;               // bolt of iron

    case SPELL_STONE_ARROW:
        return 3;               // stone arrow

    case SPELL_TOMB_OF_DOROKLOHE:
        return 7;               // tomb of doro

    case SPELL_STONEMAIL:
        return 6;               // stonemail

    case SPELL_SHOCK:
        return 3;               // shock

    case SPELL_SWIFTNESS:
        return 2;               // swiftness

    case SPELL_FLY:
        return 4;               // fly

    case SPELL_INSULATION:
        return 4;               // insulation

    case SPELL_ORB_OF_ELECTROCUTION:
        return 7;               // orb of elec

    case SPELL_DETECT_CREATURES:
        return 2;               // detect creat

    case SPELL_CURE_POISON_II:
        return 2;               // cure poison (poison)

    case SPELL_CONTROL_TELEPORT:
        return 4;               // control teleport

    case SPELL_POISON_AMMUNITION:
        return 4;               // poison ammunition

    case SPELL_POISON_WEAPON:
        return 2;               // poison weapon

    case SPELL_RESIST_POISON:
        return 4;               // resist poison

    case SPELL_PROJECTED_NOISE:
        return 2;               // create noise 2

    case SPELL_ALTER_SELF:
        return 7;               // alter self

    case SPELL_DEBUGGING_RAY:
        return 7;               // debug ray

    case SPELL_RECALL:
        return 3;               // recall

    case SPELL_PORTAL:
        return 8;               // portal

    case SPELL_AGONY:
        return 5;               // agony

    case SPELL_SPIDER_FORM:
        return 3;               // Spider form

    case SPELL_DISRUPT:
        return 1;               // Disrupt

    case SPELL_DISINTEGRATE:
        return 6;               // Disintegrate

    case SPELL_BLADE_HANDS:
        return 4;               // Blade Hands

    case SPELL_STATUE_FORM:
        return 5;               // Statue form

    case SPELL_ICE_FORM:
        return 5;               // Ice beast form

    case SPELL_DRAGON_FORM:
        return 8;               // Dragon Form

    case SPELL_NECROMUTATION:
        return 8;               // Lich Form

    case SPELL_DEATH_CHANNEL:
        return 9;               // Death Channel

    case SPELL_SYMBOL_OF_TORMENT:
        return 6;               // Symbol of Torment

    case SPELL_DEFLECT_MISSILES:
        return 6;               // deflect missiles

    case SPELL_ORB_OF_FRAGMENTATION:
        return 7;
    case SPELL_ICE_BOLT:
        return 4;
    case SPELL_ICE_STORM:
        return 9;
    case SPELL_ARC:
        return 1;
    case SPELL_AIRSTRIKE:
        return 4;
    case SPELL_SHADOW_CREATURES:
        return 5;

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
        // spelling?
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
    case SPELL_HELLFIRE:
        strcpy(spln, "Hellfire");
        break;                  // Staff of Dispater

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

        for (int i = 0; i < 25 && !knowsSpell; i++) {
          knowsSpell = you.spells[i] == spell_types[j];
        }
//        textcolor(knowsSpell ? LIGHTGREY : LIGHTBLUE);
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

        if (spell_type(spell_types[j], 17) == 1) {
            cprintf("Holy");
            already = 1;
        }

        if (spell_type(spell_types[j], 21) == 1) {
            print_slash(already);
            cprintf("Poison");
            already = 1;
        }

        if (spell_type(spell_types[j], 13) == 1) {
            print_slash(already);
            cprintf("Fire");
            already = 1;
        }

        if (spell_type(spell_types[j], 14) == 1) {
            print_slash(already);
            cprintf("Ice");
            already = 1;
        }

        if (spell_type(spell_types[j], 22) == 1) {
            print_slash(already);
            cprintf("Earth");
            already = 1;
        }

        if (spell_type(spell_types[j], 23) == 1) {
            print_slash(already);
            cprintf("Air");
            already = 1;
        }

        if (spell_type(spell_types[j], 11) == 1) {
            print_slash(already);
            cprintf("Conjuration");
            already = 1;
        }

        if (spell_type(spell_types[j], 12) == 1) {
            print_slash(already);
            cprintf("Enchantment");
            already = 1;
        }

        if (spell_type(spell_types[j], 19) == 1) {
            print_slash(already);
            cprintf("Divination");
            already = 1;
        }

        if (spell_type(spell_types[j], 20) == 1) {
            print_slash(already);
            cprintf("Translocation");
            already = 1;
        }

        if (spell_type(spell_types[j], 18) == 1) {
            print_slash(already);
            cprintf("Summoning");
            already = 1;
        }

        if (spell_type(spell_types[j], 15) == 1) {
            print_slash(already);
            cprintf("Transmigration");
            already = 1;
        }

        if (spell_type(spell_types[j], 16) == 1) {
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
}                               // end of spell_hunger


char undead_can_memorise(unsigned char spell)
{
    switch (spell)
    {
    case SPELL_DEATHS_DOOR:
        return 2;               // death's door

    case SPELL_SUMMON_HORRIBLE_THINGS:
        return 1;               // Summon Horrible things

    case SPELL_BORGNJORS_REVIVIFICATION:
      return 2;               // berserker
        //   case 111: return 2; // regen

    case SPELL_CURE_POISON_II:
        return 2;               // cure poison (poison)

    case SPELL_RESIST_POISON:
        return 2;               // resist poison

    case SPELL_ALTER_SELF:
        return 1;               // alter self

    case SPELL_SPIDER_FORM:
        return 1;               // Spider form

    case SPELL_BLADE_HANDS:
        return 1;               // Blade Hands

    case SPELL_STATUE_FORM:
        return 1;               // Statue form

    case SPELL_ICE_FORM:
        return 1;               // Ice beast form

    case SPELL_DRAGON_FORM:
        return 1;               // Dragon Form

    case SPELL_NECROMUTATION:
        return 2;               // Lich Form

    case SPELL_SYMBOL_OF_TORMENT:
        return 2;               // Symbol of Torment

    default:
        return 0;
    }
}

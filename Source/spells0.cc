/*
 *  File:       spells0.cc
 *  Summary:    Some spell related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 * <5>    06Mar2000        bwr   adjustments to spellcasting armour penalty
 * <4>    05jan2000        jmf   Changed many numbers to enums
 * <3>      5/20/99        BWR   Extend screen lines support.
 * <2>      5/09/99        JDJ   spellbook_contents prints unknown spells
 *                               in light blue.
 * <1>      -/--/--        LRH   Created
 */

#include "AppHdr.h"
#include "spells0.h"

#include <stdlib.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "itemname.h"
#include "player.h"
#include "stuff.h"
#include "spl-book.h"
#include "spl-util.h"

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



char spell_list( void )
{

    char st_pass[60];
    int j;
    int lines = 0;
    unsigned int anything = 0;
    unsigned int i;
    char strng[5] = "";
    int ft, ki;
    bool already = false;

#ifdef DOS_TERM
    char buffer[4800];

    gettext(1, 1, 80, 25, buffer);
#endif

#ifdef DOS_TERM
    window(1, 1, 80, 25);
#endif

    strcpy(st_pass, "");
    clrscr();
    cprintf(" Your Spells                      Type                          Success   Level");
    lines++;

    if ( lines > NUMBER_OF_LINES - 2 )
    {
        gotoxy(1, NUMBER_OF_LINES);
        cprintf("-more-");

        ki = getch();

        if ( ki == 27 )
        {
#ifdef DOS_TERM
            puttext(1, 1, 80, 25, buffer);
#endif
            return 27;
        }

        if ( ki >= 'A' && ki <= 'z' )
        {
#ifdef DOS_TERM
            puttext(1, 1, 80, 25, buffer);
#endif
            return ki;
        }

        if ( ki == 0 )
          ki = getch();

        lines = 0;
        clrscr();
        gotoxy(1, 1);
        anything = 0;
    }

    if ( lines > 1 )
      cprintf(EOL " ");

    for (j = 0; j < 25; j++)
    {
        if (lines > NUMBER_OF_LINES - 2)
        {
            gotoxy(1, NUMBER_OF_LINES);
            cprintf("-more-");

            ki = getch();

            if ( ki == 27 )
            {
#ifdef DOS_TERM
                puttext(1, 1, 80, 25, buffer);
#endif
                return 27;
            }

            if ( ki >= 'A' && ki <= 'z' )
            {
#ifdef DOS_TERM
                puttext(1, 1, 80, 25, buffer);
#endif
                return ki;
            }

            if ( ki == 0 )
              ki = getch();

            lines = 0;
            clrscr();
            gotoxy(1, 1);
            anything = 0;
        }

        if ( you.spells[j] != SPELL_NO_SPELL )
        {
            anything++;

            if ( lines > 0 )
              cprintf(EOL);

            lines++;
            cprintf(" ");

            ft = index_to_letter(j);

            strng[0] = ft;
            cprintf(strng);
            cprintf(" - ");

            cprintf(spell_title(you.spells[j]));

            gotoxy(35, wherey());

            already = false;

            for (i = 0; i <= SPTYP_LAST_EXPONENT; i++)
              if ( spell_typematch(you.spells[j], 1<<i) )
              {
                  print_slash(already);
                  cprintf(spelltype_name(1<<i));
                  already = true;
              }

            char sval[4];

            //gotoxy(58, wherey());
            gotoxy(65, wherey());

            int spell_f = spell_fail(you.spells[j]);

            cprintf( (spell_f == 100) ? "Useless" :
                     (spell_f  >  90) ? "Terrible" :
                     (spell_f  >  80) ? "Cruddy" :
                     (spell_f  >  70) ? "Bad" :
                     (spell_f  >  60) ? "Very Poor" :
                     (spell_f  >  50) ? "Poor" :
                     (spell_f  >  40) ? "Fair" :
                     (spell_f  >  30) ? "Good" :
                     (spell_f  >  20) ? "Very Good" :
                     (spell_f  >  10) ? "Great" :
                     (spell_f  >   0) ? "Excellent"
                                      : "Perfect" );

            //gotoxy(70, wherey());

            gotoxy(77, wherey());

            itoa((int) spell_difficulty(you.spells[j]), sval, 10);
            cprintf(sval);
        }
    }                           // end of j loop

    if ( anything > 0 )
    {

        ki = getch();

        if ( ki >= 'A' && ki <= 'z' )
        {
#ifdef DOS_TERM
            puttext(1, 1, 80, 25, buffer);
#endif
            return ki;
        }

        if ( ki == 0 )
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

}          // end spell_list()




void print_slash( bool already )
{

    if ( already )
      cprintf("/");

    return;

}          // end print_slash()




int spell_fail( int spell )
{
    int chance = 60;
    int chance2 = 0, armour = 0;

    chance -= 6 * spell_spec(spell);
    chance -= (you.intel << 1);

    //chance -= (you.intel - 10) * abs(you.intel - 10);
    //chance += spell_difficulty(spell) * spell_difficulty(spell) * 3; //spell_difficulty(spell);

    if ( you.equip[EQ_BODY_ARMOUR] != -1 )
    {

        int ev_penalty = abs(property(OBJ_ARMOUR,
                    you.inv_type[you.equip[EQ_BODY_ARMOUR]], PARM_EVASION));

        // The minus 15 is to make the -1 light armours not so bad
        armour += (20 * ev_penalty) - 15;

        //jmf: armour skill now reduces failure due to armour
        //bwr: this was far too good, an armour skill of 5 was
        //     completely negating plate mail.  Plate mail should
        //     hardly be completely negated, it should still be
        //     an important consideration for even high level characters.
        //     Truth is, even a much worse penalty than the above can
        //     easily be overcome by gaining spell skills... and a lot
        //     faster than any reasonable rate of bonus here.
        int lim_str = (you.strength > 30) ? 30 :
                      (you.strength < 10) ? 10
                                          : you.strength;

        armour -= ( (you.skills[SK_ARMOUR] * lim_str) / 15 );

        int racial_type = DARM_PLAIN;

        if ( player_genus(GENPC_DWARVEN) )
          racial_type = DARM_DWARVEN;
        else if ( player_genus(GENPC_ELVEN) )
          racial_type = DARM_ELVEN;
        else if ( you.species == SP_HILL_ORC )
          racial_type = DARM_ORCISH;

// Elven armour gives everyone some benefit to spellcasting,
// Dwarven armour hinders everyone.
        switch ( (int) (you.inv_dam[you.equip[EQ_BODY_ARMOUR]] / 30) )
        {
          case DARM_ELVEN:
            armour -= 10;
            break;
          case DARM_DWARVEN:
            armour += 10;
            break;
          default:
            break;
        }

// Armour of the same racial type reduces penalty.
        if ( racial_type != DARM_PLAIN
            && (int) (you.inv_dam[you.equip[EQ_BODY_ARMOUR]] / 30) == racial_type )
          armour -= 10;

        if (armour > 0)
          chance += armour;
    }

    if (you.equip[EQ_WEAPON] != -1
        && you.inv_class[you.equip[EQ_WEAPON]] != OBJ_STAVES)
    {
        int wpn_penalty = 0;

        if (you.inv_class[you.equip[EQ_WEAPON]] == OBJ_WEAPONS)
        {
            wpn_penalty = (5 *
                           (property(OBJ_WEAPONS,
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
            if ( ( you.species >= SP_OGRE && you.species <= SP_OGRE_MAGE ) || player_genus(GENPC_DRACONIAN) )
              chance += 20;
            else
              chance += 30;
            break;
        }
    }

    switch (spell_difficulty(spell))
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

    //if (chance < 1 ) chance = 0;
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
        && you.duration[DUR_PRAYER]
        && (!player_under_penance() && you.piety >= 50)
        && (spell_typematch(spell, SPTYP_CONJURATION)
            || spell_typematch(spell, SPTYP_SUMMONING)))
    {
        chance2 /= 2;
    }

    if (you.duration[DUR_TRANSFORMATION] > 0
        && you.attribute[ATTR_TRANSFORMATION] == TRAN_BLADE_HANDS)
    {
        chance2 += 20;
    }

    return chance2;
}          // end spell_fail()



int spell_spec( int spell )
{
  unsigned int bit;
  int ndx;
  int power = (you.skills[SK_SPELLCASTING] / 2) + player_mag_abil(false);
  int enhanced = 0;

  unsigned int disciplines = spell_type( spell );
  disciplines &= (~SPTYP_HOLY); //jmf: evil evil evil -- exclude HOLY bit
  int skillcount = count_bits( disciplines );

  if (skillcount)
    for (ndx = 0; ndx <= SPTYP_LAST_EXPONENT; ndx++)
      {
        bit = 1<<ndx;
        if ((bit != SPTYP_HOLY) && (disciplines & bit))
          {
            int skill = spell_type2skill(bit);
            power += (you.skills[skill] *2) / skillcount;
          }
      }

  enhanced += spell_enhancement( disciplines );

  if ( enhanced > 0 )
    {
      for (ndx = 0; ndx < enhanced; ndx++)
        {
          power *= 15;
          power /= 10;
        }
    }
  else if ( enhanced < 0 )
    {
      for (ndx = enhanced; ndx < 0; ndx++)
        power /= 2;
    }

  return power;
}          // end spell_spec()




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
 * When adding enchantments, must add them to extension as well!
 *
 * spells to do:
 *   Contingency?
 *   Trigger contingency
 *   Preserve Corpses
 *   Permanency
 *   Ball Lightning
 *   Explosive rune?
 *   Fennel wands
 *   More summonings!
 */




// recoded to answer whether an UNDEAD_STATE is
// barred from a particular spell passed to the
// function - note that the function can be expanded
// to prevent memorisation of certain spells by
// the living by setting up an US_ALIVE case returning
// a value of false for a set of spells ... might be
// an idea worth further consideration - 12mar2000 {dlb}
bool undead_cannot_memorise( unsigned char spell, unsigned char being )
{

    switch ( being )
    {
       case US_HUNGRY_DEAD:
         switch ( spell )
         {
           //case SPELL_REGENERATION:
           case SPELL_BORGNJORS_REVIVIFICATION:
           case SPELL_CURE_POISON_II:
           case SPELL_DEATHS_DOOR:
           case SPELL_NECROMUTATION:
           case SPELL_RESIST_POISON:
           case SPELL_SYMBOL_OF_TORMENT:
           case SPELL_TAME_BEASTS:
             return true;
         }
         break;

       case US_UNDEAD:
         switch ( spell )
         {
           case SPELL_AIR_WALK:
           case SPELL_ALTER_SELF:
           case SPELL_BLADE_HANDS:
           case SPELL_BORGNJORS_REVIVIFICATION:
           case SPELL_CURE_POISON_II:
           case SPELL_DEATHS_DOOR:
           case SPELL_DRAGON_FORM:
#ifdef USE_ELVISH_GLAMOUR_ABILITY
           case SPELL_GLAMOUR:
#endif
           case SPELL_ICE_FORM:
           case SPELL_INTOXICATE:
           case SPELL_NECROMUTATION:
           case SPELL_PASSWALL:
           case SPELL_REGENERATION:
           case SPELL_RESIST_POISON:
           case SPELL_SPIDER_FORM:
           case SPELL_STATUE_FORM:
           case SPELL_SUMMON_HORRIBLE_THINGS:
           case SPELL_SYMBOL_OF_TORMENT:
           case SPELL_TAME_BEASTS:
             return true;
         }
         break;

    }

    return false;

}          // end undead_cannot_memorise()




int spell_type2skill ( unsigned int spelltype )
{
  char buffer[80];

  switch ( spelltype )
    {
    case SPTYP_CONJURATION:
      return SK_CONJURATIONS;
    case SPTYP_ENCHANTMENT:
      return SK_ENCHANTMENTS;
    case SPTYP_FIRE:
      return SK_FIRE_MAGIC;
    case SPTYP_ICE:
      return SK_ICE_MAGIC;
    case SPTYP_TRANSMIGRATION:
      return SK_TRANSMIGRATION;
    case SPTYP_NECROMANCY:
      return SK_NECROMANCY;
    case SPTYP_SUMMONING:
      return SK_SUMMONINGS;
    case SPTYP_DIVINATION:
      return SK_DIVINATIONS;
    case SPTYP_TRANSLOCATION:
      return SK_TRANSLOCATIONS;
    case SPTYP_POISON:
      return SK_POISON_MAGIC;
    case SPTYP_EARTH:
      return SK_EARTH_MAGIC;
    case SPTYP_AIR:
      return SK_AIR_MAGIC;
    default:
    case SPTYP_HOLY:
      sprintf(buffer, "spell_type2skill: called with spelltype %d", spelltype);
      mpr(buffer);
      return -1;
    }

}          // end spell_type2skill()




int spell_enhancement ( unsigned int typeflags )
{

  unsigned int enhanced = 0;

  if ( typeflags & SPTYP_CONJURATION )
    enhanced += player_spec_conj();

  if ( typeflags & SPTYP_ENCHANTMENT )
    enhanced += player_spec_ench();

  if ( typeflags & SPTYP_SUMMONING )
    enhanced += player_spec_summ();

  if ( typeflags & SPTYP_POISON )
    enhanced += player_spec_poison();

  /* if ( ar_typeflags[SPTYP_HOLY] ) {
     enhanced += player_spec_holy() - player_spec_death();
     if ( you.special_wield == SPWLD_SHADOW )
     enhanced -= 2; } */

  if ( typeflags & SPTYP_NECROMANCY )
    enhanced += player_spec_death() - player_spec_holy();

  if ( typeflags & SPTYP_FIRE )
    enhanced += player_spec_fire() - player_spec_cold();

  if ( typeflags & SPTYP_ICE )
    enhanced += player_spec_cold() - player_spec_fire();

  if ( typeflags & SPTYP_EARTH )
    enhanced += player_spec_earth() - player_spec_air();

  if ( typeflags & SPTYP_AIR )
    enhanced += player_spec_air() - player_spec_earth();

  return ( enhanced );

}          // end spell_enhancement()



//jmf: deprecated
/*
  unsigned int set_spellflags ( int which_spell )
  {
  unsigned int s = 1;
  int spellcount = 0;

  for (s = SPTYP_CONJURATION; s < SPTYP_LAST_EXPONENT; s<<=1)
  if ( spell_typematch(which_spell, s) )
  {
  spellcount++;
  }


  return ( spellcount );

  }
*/

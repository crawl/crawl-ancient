#include "config.h"

#include <stdlib.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"

#include "player.h"
#include "dungeon.h"

int spell_hunger(int spell_value, unsigned char spell);
int which_spell_in_book(int sbook_type, int spl);
char spellbook_contents(unsigned char plus, unsigned char type);
void spell_name(unsigned char spell, char spln [60]);
char spell_value(unsigned char spell);
int spell_type(unsigned char spell, unsigned char typy);
int spell_spec(int spell, int power);
char spell_fail(unsigned char spell);
void print_slash(char already);
char spell_list(void);


char spell_list(void)
{
char st_pass [60];

   int j;
   char lines = 0;
   unsigned char anything = 0;
   char strng [5] = "";
   char ft;
   char ki;
   char already = 0;

#ifdef DOS
   char buffer[4800];
   gettext(1, 1, 80, 25, buffer);
#endif

#ifdef DOS
   window(1, 1, 80, 25);
#endif

   strcpy(st_pass, "");

   clrscr();

cprintf("  Your Spells                       Type                  Success   Level");
lines++;

        if (lines > 22)
        {
                gotoxy(1,25);
                cprintf("-more-");
                ki = getch();
                if (ki == 27)
                {
#ifdef DOS
                        puttext(1, 1, 80, 25, buffer);
#endif
                        return 27;
                }
                if (ki >= 65 && ki < 123)
                {
#ifdef DOS
                        puttext(1, 1, 80, 25, buffer);
#endif
                        return ki;
                }
                if (ki == 0) ki = getch();
                lines = 0;
                clrscr();
                gotoxy(1,1);
                anything = 0;

        }
        if (lines > 1) cprintf("\n\r ");

        for (j = 0; j < 25; j++)
        {

                if (lines > 23)
                {
                        gotoxy(1,25);
                        cprintf("-more-");
                        ki = getch();
                        if (ki == 27)
                        {
#ifdef DOS
                                puttext(1, 1, 80, 25, buffer);
#endif
                                return 27;
                        }
                        if (ki >= 65 && ki < 123)
                        {
#ifdef DOS
                                puttext(1, 1, 80, 25, buffer);
#endif
                                return ki;
                        }
                        if (ki == 0) ki = getch();
                        lines = 0;
                        clrscr();
                        gotoxy(1,1);
                        anything = 0;
                }

                if (you[0].spells [j] != 210)
                {

                        anything ++;
                        if (lines > 0) cprintf("\n\r");
                        lines++;
                                cprintf(" ");
                                if (j < 26) ft = (char) j + 97;
                                        else ft = (char) j  + 65;
                                strng [0] = ft;
                                cprintf(strng);
                        cprintf(" - ");
                        spell_name(you[0].spells [j], st_pass);

                        cprintf(st_pass);

   gotoxy(35, wherey());
   already = 0;
   if (spell_type(you[0].spells [j], 17) == 1)
   {
    cprintf("Holy");
    already = 1;
   }
   if (spell_type(you[0].spells [j], 21) == 1)
   {
    print_slash(already);
    cprintf("Poison");
    already = 1;
   }
   if (spell_type(you[0].spells [j], 13) == 1)
   {
    print_slash(already);
    cprintf("Fire");
    already = 1;
   }
   if (spell_type(you[0].spells [j], 14) == 1)
   {
    print_slash(already);
    cprintf("Ice");
    already = 1;
   }
   if (spell_type(you[0].spells [j], 22) == 1)
   {
    print_slash(already);
    cprintf("Earth");
    already = 1;
   }
   if (spell_type(you[0].spells [j], 23) == 1)
   {
    print_slash(already);
    cprintf("Air");
    already = 1;
   }
   if (spell_type(you[0].spells [j], 11) == 1)
   {
    print_slash(already);
    cprintf("Conjuration");
    already = 1;
   }
   if (spell_type(you[0].spells [j], 12) == 1)
   {
    print_slash(already);
    cprintf("Enchantment");
    already = 1;
   }
   if (spell_type(you[0].spells [j], 19) == 1)
   {
    print_slash(already);
    cprintf("Divination");
    already = 1;
   }
   if (spell_type(you[0].spells [j], 20) == 1)
   {
    print_slash(already);
    cprintf("Translocation");
    already = 1;
   }
   if (spell_type(you[0].spells [j], 18) == 1)
   {
    print_slash(already);
    cprintf("Summoning");
    already = 1;
   }
   if (spell_type(you[0].spells [j], 15) == 1)
   {
    print_slash(already);
    cprintf("Transmigration");
    already = 1;
   }
   if (spell_type(you[0].spells [j], 16) == 1)
   {
    print_slash(already);
    cprintf("Necromancy");
    already = 1;
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
*/
         char sval [4];

gotoxy(58, wherey());

int spell_f = spell_fail(you[0].spells [j]);

if (spell_f == 100)
{
 cprintf("Useless");
} else
if (spell_f > 90)
{
 cprintf("Terrible");
} else
if (spell_f > 80)
{
 cprintf("Cruddy");
} else
if (spell_f > 70)
{
 cprintf("Bad");
} else
if (spell_f > 60)
{
 cprintf("Very Poor");
} else
if (spell_f > 50)
{
 cprintf("Poor");
} else
if (spell_f > 40)
{
 cprintf("Fair");
} else
if (spell_f > 30)
{
 cprintf("Good");
} else
if (spell_f > 20)
{
 cprintf("Very Good");
} else
if (spell_f > 10)
{
 cprintf("Great");
} else
if (spell_f > 0)
{
 cprintf("Excellent");
} else
 cprintf("Perfect");

                        gotoxy(70, wherey());


         itoa((int) spell_value(you[0].spells [j]), sval, 10);
         cprintf(sval);

                }
        } // end of j loop
   if (anything > 0)
   {
           ki = getch();
           if (ki >= 65 && ki < 123)
           {
#ifdef DOS
                   puttext(1, 1, 80, 25, buffer);
#endif
                   return ki;
           }
           if (ki == 0) ki = getch();
#ifdef DOS
           puttext(1, 1, 80, 25, buffer);
#endif
           return anything;
   }

#ifdef DOS
   puttext(1, 1, 80, 25, buffer);
#endif
       // was 35
   ki = getch();

   return ki;


}


void print_slash(char already)
{

 if (already == 1) cprintf("/");

}


char spell_fail(unsigned char spell)
{

int chance = 60;
int chance2 = 0;


chance -= spell_spec(spell, 0) * 7;

chance -= you[0].intel;

chance -= (you[0].intel - 10) * abs(you[0].intel - 10);

//chance += spell_value(spell) * spell_value(spell) * 3; //spell_value(spell);

switch(spell_value(spell))
{
 case 1: chance += 3; break;
 case 2: chance += 15; break;
 case 3: chance += 35; break;
 case 4: chance += 70; break;
 case 5: chance += 100; break;
 case 6: chance += 150; break;
 case 7: chance += 200; break;
 case 8: chance += 260; break;
 case 9: chance += 330; break;
 case 10: chance += 420; break;
 case 11: chance += 500; break;
 case 12: chance += 600; break;
}

//if (chance <= 0) chance = 0;
if (chance > 100) chance = 100;

chance2 = chance;

if (chance < 45) chance2 = 45;
if (chance < 42) chance2 = 43;
if (chance < 38) chance2 = 41;
if (chance < 35) chance2 = 40;
if (chance < 32) chance2 = 38;
if (chance < 28) chance2 = 36;
if (chance < 22) chance2 = 34;
if (chance < 16) chance2 = 32;
if (chance < 10) chance2 = 30;
if (chance < 2) chance2 = 28;
if (chance < -7) chance2 = 26;
if (chance < -12) chance2 = 24;
if (chance < -18) chance2 = 22;
if (chance < -24) chance2 = 20;
if (chance < -30) chance2 = 18;
if (chance < -38) chance2 = 16;
if (chance < -46) chance2 = 14;
if (chance < -60) chance2 = 12;
if (chance < -80) chance2 = 10;
if (chance < -100) chance2 = 8;
if (chance < -120) chance2 = 6;
if (chance < -140) chance2 = 4;
if (chance < -160) chance2 = 2;
if (chance < -180) chance2 = 0;

return chance2;

}


int spell_spec(int spell, int power)
{

int enhanced = 0;


power = 0;

int s = 0;
int spellsy = 0;

for (s = 11; s < 24; s ++)
{
// if (s == 13 | s == 14 | s == 17) continue;
 if (s == 17 | s == 21) continue;
 if (spell_type(spell, s) == 1) spellsy ++;
}

if (spellsy != 0)
{
if (spell_type(spell, 11) == 1)
{
 power += (you[0].skills [26] * 2) / spellsy;
}
if (spell_type(spell, 12) == 1)
{
 power += (you[0].skills [27] * 2) / spellsy;
}
if (spell_type(spell, 15) == 1)
{
 power += (you[0].skills [31] * 2) / spellsy;
}
if (spell_type(spell, 16) == 1)
{
 power += (you[0].skills [29] * 2) / spellsy;
}
if (spell_type(spell, 18) == 1)
{
 power += (you[0].skills [28] * 2) / spellsy;
}
if (spell_type(spell, 19) == 1)
{
 power += (you[0].skills [32] * 2) / spellsy;
}
if (spell_type(spell, 20) == 1)
{
 power += (you[0].skills [30] * 2) / spellsy;
}
if (spell_type(spell, 21) == 1)
{
 power += (you[0].skills [37] * 2) / spellsy;
}
if (spell_type(spell, 22) == 1)
{
 power += (you[0].skills [36] * 2) / spellsy;
}
if (spell_type(spell, 23) == 1)
{
 power += (you[0].skills [35] * 2) / spellsy;
}
if (spell_type(spell, 13) == 1)
{
 power += (you[0].skills [33] * 2) / spellsy;
}
if (spell_type(spell, 14) == 1)
{
 power += (you[0].skills [34] * 2) / spellsy;
}
} // end if spellsy != 0

power += (you[0].skills [25] * 5) / 10;

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

if (spell_type(spell, 16) == 1)
{
  enhanced += player_spec_death();
  enhanced -= player_spec_holy();
//  naughty(spell_value(spell) * 2, 1);
} // necromancy
if (spell_type(spell, 13) == 1)
{
  enhanced += player_spec_fire();
  enhanced -= player_spec_cold();
} // fire
if (spell_type(spell, 14) == 1)
{
  enhanced += player_spec_cold();
  enhanced -= player_spec_fire();
} // ice
if (spell_type(spell, 17) == 1)
{
  enhanced -= player_spec_death();
  enhanced += player_spec_holy();
  if (you[0].special_wield == 50) enhanced -= 2;
} // holy - signs switched!
if (spell_type(spell, 11) == 1)
{
 enhanced += player_spec_conj();
}
if (spell_type(spell, 12) == 1)
{
 enhanced += player_spec_ench();
}
if (spell_type(spell, 18) == 1)
{
 enhanced += player_spec_summ();
}
if (spell_type(spell, 21) == 1)
{
 enhanced += player_spec_poison();
}
if (spell_type(spell, 22) == 1)
{
  enhanced += you[0].attribute [2]; // spec_earth
  enhanced -= you[0].attribute [1]; // spec_air
} // earth
if (spell_type(spell, 23) == 1)
{
  enhanced -= you[0].attribute [2]; // spec_earth
  enhanced += you[0].attribute [1]; // spec_air
} // air

/* If the bit just above changes, remember to also change it in crawl99.cc */

/*
types of you[0].spells:
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

} // end int spell_spec






int spell_type(unsigned char spell, unsigned char typy)
{
unsigned long stype = 0;

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

switch(spell)
{
        case 0: stype = 19; break; //6; // identify
        case 1: stype = 20; break; //5; // teleportation
        case 2: stype = 12; break; //4; // cause fear
        case 3: stype = 12; break; //1; // noise
        case 4: stype = 12; break; //5; // remove curse
        case 5: stype = 11; break; //1; // magic missile
        case 6: stype = 1311; break; //5; // fireball
        case 13: stype = 1311; break; //3; // conjure flame
   case 14: stype = 1522; break; //4; // dig
   case 15: stype = 1311; break; //5; // firebolt
   case 16: stype = 1411; break; //5; // bolt of freezing cold
   case 17: stype = 2311; break; //6; // bolt of lightning
   case 20: stype = 1512; break; //6; // polymorph other
   case 21: stype = 12; break; //3;
   case 22: stype = 12; break; //4;
   case 23: stype = 12; break; //4;
   case 24: stype = 12; break; //3;
   case 25: stype = 12; break; //4; // invisibility
   case 26: stype = 1311; break; //2; // throw flame
   case 27: stype = 1411; break; //2; // throw frost
   case 28: stype = 20; break; //4; // blink
   case 29: stype = 1411; break; //7; // freezing cloud
   case 30: stype = 232111; break; //3; // stinking cloud
   case 31: stype = 1312; break; //8; // ring of flames
   case 32: stype = 17; break; //2; //  restore strength:
   case 33: stype = 17; break; //2; //          int
   case 34: stype = 17; break; //2; //          dex
   case 35: stype = 2111; break; //5; // venom bolt
   case 36: stype = 21; break; //4; // toxic radiance - uses lots of food?
   case 37: stype = 20; break; //5; // teleport other
   case 38: stype = 17; break; //2; // lesser healing
   case 39: stype = 17; break; //6; // greater healing
   case 40: stype = 17; break; //3; // cure poison
   case 41: stype = 17; break; //5; // purification
   case 42: stype = 1612; break; //8; // death's door
   case 43: stype = 12; break; //3; // selective amnesia
   case 44: stype = 12; break; //7; // mass confusion
   case 45: stype = 17; break; //4; // smiting
   case 46: stype = 17; break; //3; // repel undead
   case 47: stype = 17; break; //7; // holy word
   case 48: stype = 19; break; //3; // detect curse
   case 49: stype = 18; break; //1; // summon small mammal
   case 50: stype = 18; break; //4; // Abjuration
   case 51: stype = 18; break; //4; // summon scorpions
   case 52: stype = 1223; break; //3; // levitation
   case 53: stype = 1611; break; //6; // bolt of draining
   case 54: stype = 1122; break; //8; // splinters
   case 55: stype = 11; break; //4; // innacuracy
   case 56: stype = 2111; break; //6; // poisonous cloud
   case 57: stype = 1311; break; //8; // fire storm
   case 58: stype = 19; break; //2; // detect traps
   case 59: stype = 20; break; //2; // random blink
   case 60: stype = 11; break; //5; // Isk's blast
   case 61: stype = 18; break; //5; // swarm
   case 62: stype = 18; break; //8; // Summon Thing
   case 63: stype = 12; break; //4; // Enslavement
   case 64: stype = 1922; break; //4; // Magic Mapping
   case 65: stype = 17; break; //3; // heal other
   case 66: stype = 16; break; //7; // Animate dead
   case 67: stype = 16; break; //1; // Pain
   case 68: stype = 12; break; //4; // Extension
   case 69: stype = 1612; break; //6; // Control Undead
   case 70: stype = 16; break; //3; // animate skeleton
   case 71: stype = 16; break; //3; // vampiric draining
   case 72: stype = 1618; break; // summon greater undead
   case 73: stype = 19; break; // detect items
   case 74: stype = 16; break;

   case 75: stype = 13; break; // burn
   case 76: stype = 14; break; // freeze
   case 77: stype = 18; break; // Summon elemental
   case 78: stype = 14; break; // refrigeration
   case 79: stype = 1311; break; // Sticky flame
   case 80: stype = 1418; break; // ice beast
   case 81: stype = 1412; break; // ice armour
   case 82: stype = 18; break; // imp
   case 83: stype = 1223; break; // deflect missiles
   case 84: stype = 12; break; // berserk
   case 85: stype = 16; break; // dispel undead
   case 86: stype = 17; break; // Guardian
   case 87: stype = 17; break; // ???
   case 88: stype = 1723; break; // Thunderbolt
   case 89: stype = 17; break; // Flame of Cleansing
   case 90: stype = 17; break; // Shining Light
   case 91: stype = 17; break; // Summon Deva
   case 92: stype = 17; break; // Abjuration

   case 110: stype = 16; break; // twisted res
   case 111: stype = 1612; break; // regen
   case 112: stype = 16; break; // bone shards
   case 113: stype = 20; break; // Banishment
   case 114: stype = 1516; break; // degeneration
   case 115: stype = 1121; break; // sting
   case 116: stype = 16; break; // blood
   case 117: stype = 12; break; // dance
   case 118: stype = 1113; break; // hellfire
   case 119: stype = 18; break; // summon demon
   case 120: stype = 18; break; // summon demon
   case 121: stype = 18; break; // summon demon
   case 122: stype = 16; break; // rot corpse
   case 123: stype = 12; break; // Tukima's blade
   case 124: stype = 1213; break; // Fire brand
   case 125: stype = 1214; break; // Ice brand
   case 126: stype = 1216; break; // weapon of draining

   case 127: stype = 1122; break; // throw pebble
   case 128: stype = 1122; break; // bolt of iron
   case 129: stype = 1122; break; // stone arrow
   case 130: stype = 1122; break; // tomb of doro
   case 131: stype = 1222; break; // stonemail

   case 132: stype = 1123; break; // shock
   case 133: stype = 1223; break; // swiftness
   case 134: stype = 1223; break; // flight
   case 135: stype = 1223; break; // insulation
   case 136: stype = 1123; break; // orb of elec
   case 137: stype = 19; break; // detect creat
   case 138: stype = 21; break; // cure poison (poison)
   case 139: stype = 1220; break; // control teleport
   case 140: stype = 21; break; // poison ammunition
   case 141: stype = 21; break; // poison weapon
   case 142: stype = 2112; break; // resist poison
   case 143: stype = 12; break; // noise 2
   case 144: stype = 15; break; // mutation
   case 145: stype = 11; break; // debug ray


   // Make restore abilities necromancy?

   default: stype = 0; break;
//default: return 2;

}

//if (stype == typy | (stype / 10) % 10 == typy | (stype / 100) % 10 == typy) return 1;

//if (stype == typy | (stype / 10) % 10 == typy | (stype / 100) % 10 == typy | (stype / 1000) % 10 == typy | (stype / 10000) % 10 == typy) return 1;

if (stype % 100 == typy | (stype / 100) % 100 == typy | (stype / 10000) % 100 == typy) return 1;

//232111

//if (stype % 100 == typy | stype / 100 == typy) return 1;

return 0;

} // end spell_type

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
until the magic wears off. Enchantments are distinguished from transmigrations
in that the latter cause a permanent alteration in something which persists
even after the magic has faded, while the effects of the former last only
so long as the magic does. Sometimes enchantments may take advantage of the
more powerful aspects of transmigration to induce some kind of radical change
(eg polymorph).
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
-Differentiation of living, dead and undead. Some necromancy affects only the
living (pain, vampiric draining etc), some affects only the dead (animate
dead, twisted resurrection etc), and some affects only undead (dispel and
control undead).
-Actual or potential harm: eg risk in Death's Door, hp loss with Pain,
disease with summon greater undead, etc. Also loss of potential xp gain
with bolt of draining and degeneration.
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
It is possible that I may give summoners some special access to translocations
due to the obvious similarities.

Poison
These spells all involve poison. Most are also conjurations.
I don't plan to implement a 'Poisoner' class, as it would become unplayable
deep in the dungeon where most monsters are poison resistant.



Many spells use magic from two types. These spells are equally
available to either type; a conjurer is no worse at a fire/conjuration than
at a pure conjuration. I guess a spell could be of three types, but they
would have to be types with short names (limited space in the spell windows).
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

switch(spell)
{
        case 0: return 6; // identify
        case 1: return 5; // teleportation
        case 2: return 6; // cause fear
        case 3: return 1; // noise
        case 4: return 5; // remove curse
        case 5: return 1; // magic missile
        case 6: return 6; // fireball
        case 13: return 3; // conjure flame
   case 14: return 4; // dig
   case 15: return 5; // firebolt
   case 16: return 5; // bolt of freezing cold
   case 17: return 6; // bolt of lightning
   case 20: return 6; // polymorph other
   case 21: return 3; // slow
   case 22: return 6; // haste
   case 23: return 4; // paralyse
   case 24: return 3; // confuse
   case 25: return 6; // invisibility
   case 26: return 2; // throw flame
   case 27: return 2; // throw frost
   case 28: return 4; // blink
   case 29: return 7; // freezing cloud
   case 30: return 3; // stinking cloud
   case 31: return 8; // ring of flames
   case 32: return 2; //  restore strength:
   case 33: return 2; //          int
   case 34: return 2; //          dex
   case 35: return 5; // venom bolt
   case 36: return 4; // toxic radiance - uses lots of food?
   case 37: return 5; // teleport other
   case 38: return 2; // lesser healing
   case 39: return 6; // greater healing
   case 40: return 3; // cure poison
   case 41: return 5; // purification
   case 42: return 8; // death's door
   case 43: return 3; // selective amnesia
   case 44: return 7; // mass confusion
   case 45: return 4; // smiting
   case 46: return 3; // repel undead
   case 47: return 7; // holy word
   case 48: return 3; // detect curse
   case 49: return 1; // summon small mammal
   case 50: return 4; // Abjuration
   case 51: return 4; // summon scorpions
   case 52: return 3; // levitation
   case 53: return 6; // bolt of draining
   case 54: return 8; // splinters
   case 55: return 4; // innacuracy
   case 56: return 6; // poisonous cloud
   case 57: return 9; // fire storm
   case 58: return 2; // detect traps
   case 59: return 2; // random blink
   case 60: return 4; // Isk's blast
   case 61: return 6; // swarm
   case 62: return 8; // Summon Thing
   case 63: return 4; // Enslavement
   case 64: return 4; // Magic Mapping
   case 65: return 3; // heal other
   case 66: return 4; // Animate dead
   case 67: return 1; // Pain
   case 68: return 5; // Extension
   case 69: return 6; // Control Undead
   case 70: return 1; // animate skeleton
   case 71: return 3; // vampiric draining
   case 72: return 7; // summon wraiths
   case 73: return 2; // detect items
   case 74: return 6; // reviv
   case 75: return 1; // burn
   case 76: return 1; // freeze
   case 77: return 4; // Summon elemental
   case 78: return 5; // refrigeration
   case 79: return 4; // Sticky flame
   case 80: return 5; // Ice beast
   case 81: return 4; // Ozocubu's Armour
   case 82: return 2; // imp
   case 83: return 2; // deflect missiles
   case 84: return 3; // berserker
   case 85: return 4; // dispel undead
   case 86: return 7; // Guardian
   case 87: return 4; // ???
   case 88: return 6; // Thunderbolt
   case 89: return 8; // Flame of Cleansing
   case 90: return 7; // Shining Light
   case 91: return 8; // Summon Deva
   case 92: return 4; // Abjuration

   case 110: return 5; // twisted res
   case 111: return 3; // regen
   case 112: return 3; // bone shards
   case 113: return 7; // Banishment
   case 114: return 6; // Degeneration
   case 115: return 1; // sting
   case 116: return 2; // blood
   case 117: return 3; // dance
   case 118: return 9; // hellfire
   case 119: return 4; // summon demon
   case 120: return 7; // demonic horde
   case 121: return 7; // summon greater demon

   case 122: return 2; // corpse rot
   case 123: return 2; // Tukima's v bl
   case 124: return 3; // flaming weapon
   case 125: return 3; // freezing weapon
   case 126: return 4; // draining weapon

   case 127: return 1; // throw pebble
   case 128: return 6; // bolt of iron
   case 129: return 3; // stone arrow
   case 130: return 7; // tomb of doro
   case 131: return 6; // stonemail
   case 132: return 1; // shock
   case 133: return 2; // swiftness
   case 134: return 6; // fly
   case 135: return 4; // insulation
   case 136: return 7; // orb of elec
   case 137: return 2; // detect creat
   case 138: return 2; // cure poison (poison)
   case 139: return 4; // control teleport
   case 140: return 4; // poison ammunition
   case 141: return 2; // poison weapon
   case 142: return 4; // resist poison
   case 143: return 2; // create noise 2
   case 144: return 7; // mutation
   case 145: return 7; // debug ray

   /*      case 86: strcpy(spln, "Guardian"); break;
      case 87: strcpy(spln, "Pestilence"); break;
      case 88: strcpy(spln, "Thunderbolt"); break;
      case 89: strcpy(spln, "Flame of Cleansing"); break;
      case 90: strcpy(spln, "Shining Light"); break;
      case 91: strcpy(spln, "Summon Deva"); break;
      case 92: strcpy(spln, "Abjuration"); break;
*/
default: return 2;

}

return 1;

}


void spell_name(unsigned char spell, char spln [60])
{
        //char spln [30] = "";

        strcpy(spln, "Hello!");

        switch(spell)
        {
                case 0: strcpy(spln, "Identify"); break;
                case 1: strcpy(spln, "Teleport Self"); break;
                case 2: strcpy(spln, "Cause Fear"); break;
                case 3: strcpy(spln, "Create Noise"); break;
                case 4: strcpy(spln, "Remove Curse"); break;
                case 5: strcpy(spln, "Magic Dart"); break;
                case 6: strcpy(spln, "Fireball"); break;
                case 13: strcpy(spln, "Conjure Flame"); break;
      case 14: strcpy(spln, "Dig"); break;
      case 15: strcpy(spln, "Bolt of Fire"); break;
      case 16: strcpy(spln, "Bolt of Cold"); break;
      case 17: strcpy(spln, "Lightning Bolt"); break;
      case 20: strcpy(spln, "Polymorph Other"); break;
      case 21: strcpy(spln, "Slow"); break;
      case 22: strcpy(spln, "Haste"); break;
      case 23: strcpy(spln, "Paralyze"); break;
      case 24: strcpy(spln, "Confuse"); break;
      case 25: strcpy(spln, "Invisibility"); break;
      case 26: strcpy(spln, "Throw Flame"); break;
      case 27: strcpy(spln, "Throw Frost"); break;
      case 28: strcpy(spln, "Controlled Blink"); break;
      case 29: strcpy(spln, "Freezing Cloud"); break;
      case 30: strcpy(spln, "Mephitic Cloud"); break;
      case 31: strcpy(spln, "Ring of Flames"); break;
      case 32: strcpy(spln, "Restore Strength"); break;
      case 33: strcpy(spln, "Restore Intelligence"); break;
      case 34: strcpy(spln, "Restore Dexterity"); break;
      case 35: strcpy(spln, "Venom Bolt"); break;
      case 36: strcpy(spln, "Olgreb's Toxic Radiance"); break;
      case 37: strcpy(spln, "Teleport Other"); break;
      case 38: strcpy(spln, "Lesser Healing"); break;
      case 39: strcpy(spln, "Greater Healing"); break;
      case 40: strcpy(spln, "Cure Poison"); break;
      case 41: strcpy(spln, "Purification"); break;
      case 42: strcpy(spln, "Death's Door"); break;
      case 43: strcpy(spln, "Selective Amnesia"); break;
      case 44: strcpy(spln, "Mass Confusion"); break;
      case 45: strcpy(spln, "Smiting"); break;
        case 46: strcpy(spln, "Repel Undead"); break;
        case 47: strcpy(spln, "Holy Word"); break;
      case 48: strcpy(spln, "Detect Curse"); break;
      case 49: strcpy(spln, "Summon Small Mammal"); break;
      case 50: strcpy(spln, "Abjuration"); break;
      case 51: strcpy(spln, "Summon Scorpions"); break;
      case 52: strcpy(spln, "Levitation"); break;
      case 53: strcpy(spln, "Bolt of Draining"); break;
      case 54: strcpy(spln, "Lehudib's Crystal Spear"); break;
      case 55: strcpy(spln, "Bolt of Inaccuracy"); break;
                                          // spelling?
      case 56: strcpy(spln, "Poisonous Cloud"); break;
      case 57: strcpy(spln, "Fire Storm"); break;
      case 58: strcpy(spln, "Detect Traps"); break;
      case 59: strcpy(spln, "Blink"); break;
      case 60: strcpy(spln, "Iskenderun's Mystic Blast"); break; // this name was found in the hack.exe file of an early version of PCHACK. Credit goes to its creator (whoever that may be).
      case 61: strcpy(spln, "Swarm"); break;
      case 62: strcpy(spln, "Summon Horrible Things"); break;
      case 63: strcpy(spln, "Enslavement"); break;
      case 64: strcpy(spln, "Magic Mapping"); break;
      case 65: strcpy(spln, "Heal Other"); break;
      case 66: strcpy(spln, "Animate Dead"); break;
      case 67: strcpy(spln, "Pain"); break;
      case 68: strcpy(spln, "Extension"); break;
      case 69: strcpy(spln, "Control Undead"); break;
      case 70: strcpy(spln, "Animate Skeleton"); break;
      case 71: strcpy(spln, "Vampiric Draining"); break;
      case 72: strcpy(spln, "Summon Wraiths"); break;
      case 73: strcpy(spln, "Detect Items"); break;
      case 74: strcpy(spln, "Borgnjor's Revivification"); break;

      case 75: strcpy(spln, "Burn"); break;
      case 76: strcpy(spln, "Freeze"); break;
      case 77: strcpy(spln, "Summon Elemental"); break;
      case 78: strcpy(spln, "Ozocubu's Refrigeration"); break;
      case 79: strcpy(spln, "Sticky Flame"); break;
      case 80: strcpy(spln, "Summon Ice Beast"); break;
      case 81: strcpy(spln, "Ozocubu's Armour"); break;
      case 82: strcpy(spln, "Call Imp"); break;
      case 83: strcpy(spln, "Deflect Missiles"); break;
      case 84: strcpy(spln, "Berserker Rage"); break;
      case 85: strcpy(spln, "Dispel Undead"); break;

      case 86: strcpy(spln, "Guardian"); break;
      case 87: strcpy(spln, "Pestilence"); break;
      case 88: strcpy(spln, "Thunderbolt"); break;
      case 89: strcpy(spln, "Flame of Cleansing"); break;
      case 90: strcpy(spln, "Shining Light"); break;
      case 91: strcpy(spln, "Summon Daeva"); break;
      case 92: strcpy(spln, "Abjuration"); break;

      case 110: strcpy(spln, "Twisted Resurrection"); break;
      case 111: strcpy(spln, "Regeneration"); break;
      case 112: strcpy(spln, "Bone Shards"); break;
      case 113: strcpy(spln, "Banishment"); break;
      case 114: strcpy(spln, "Cigotuvi's Degeneration"); break;
      case 115: strcpy(spln, "Sting"); break;
      case 116: strcpy(spln, "Sublimation of Blood"); break;
      case 117: strcpy(spln, "Tukima's Dance"); break;
      case 118: strcpy(spln, "Hellfire"); break; // Staff of Dispater
      case 119: strcpy(spln, "Summon Demon"); break;
      case 120: strcpy(spln, "Demonic Horde"); break;
      case 121: strcpy(spln, "Summon Greater Demon"); break;
      case 122: strcpy(spln, "Corpse Rot"); break;
      case 123: strcpy(spln, "Tukima's Vorpal Blade"); break;
      case 124: strcpy(spln, "Fire Brand"); break;
      case 125: strcpy(spln, "Freezing Aura"); break;
      case 126: strcpy(spln, "Lethal Infusion"); break;

      case 127: strcpy(spln, "Throw Pebble"); break;
      case 128: strcpy(spln, "Bolt of Iron"); break;
      case 129: strcpy(spln, "Stone Arrow"); break;
      case 130: strcpy(spln, "Tomb of Doroklohe"); break;
      case 131: strcpy(spln, "Stonemail"); break;

      case 132: strcpy(spln, "Shock"); break;
      case 133: strcpy(spln, "Swiftness"); break;
      case 134: strcpy(spln, "Fly"); break;
      case 135: strcpy(spln, "Insulation"); break;
      case 136: strcpy(spln, "Orb of Electrocution"); break;
      case 137: strcpy(spln, "Detect creatures"); break;
      case 138: strcpy(spln, "Cure Poison"); break;
      case 139: strcpy(spln, "Control Teleport"); break;
      case 140: strcpy(spln, "Poison Ammunition"); break;
      case 141: strcpy(spln, "Poison Weapon"); break;
      case 142: strcpy(spln, "Resist Poison"); break;
      case 143: strcpy(spln, "Projected Noise"); break;
      case 144: strcpy(spln, "Alter Self"); break;
      case 145: strcpy(spln, "Debugging ray"); break;


/* When adding enchantments, must add them to extension as well */

/*
spells to do:
Contingency?
Trigger contingency
Preserve Corpses
Permanency
Gate
Ball Lightning
Explosive rune?
Fennel wands (+ credit)
More summonings!
*/

default: strcpy(spln, "another spell"); break;
        }

// purify food?

        //itoa(spell, spln, 10);

//return spln;

}


char spellbook_contents(unsigned char plus, unsigned char type)
{

char st_pass [60];

char stringy [9];
int numby = plus;
int j;
int spelcount = 0;
char strng [2] = "";
char ft = 0;

//char st_pass [60];
char already = 0;


   strcpy(st_pass, "");

#ifdef DOS
  char buffer[4800];
  gettext(1, 1, 80, 25, buffer);
#endif

  strcpy(st_pass, "");

#ifdef DOS
  window(1, 1, 80, 25);
#endif

  clrscr();


   int spell_types [10];

   spellbook_template(type, spell_types);

//   numby -= 64;

   itoa(numby, stringy, 2);

   textcolor(LIGHTGREY);
cprintf(" Spells                             Type                      Level\n\r");


   for (j = 1; j < 7; j ++)
   {

      if (stringy [j] != 49) continue;

                                cprintf(" ");
                                if (spelcount < 26) ft = (char) spelcount + 97;
                                        else ft = (char) spelcount + 65;

               strcpy(strng, "");

                                strng [0] = ft;
            strng [1] = 0;

                                cprintf(strng);

                        cprintf(" - ");

//                      spell_name(???, st_pass);

//                      cprintf(st_pass);

                        spell_name(spell_types [j], st_pass);

                        cprintf(st_pass);

         //cprintf("A spell");

   gotoxy(35, wherey());
   already = 0;
   if (spell_type(spell_types [j], 17) == 1)
   {
    cprintf("Holy");
    already = 1;
   }
   if (spell_type(spell_types [j], 21) == 1)
   {
    print_slash(already);
    cprintf("Poison");
    already = 1;
   }
   if (spell_type(spell_types [j], 13) == 1)
   {
    print_slash(already);
    cprintf("Fire");
    already = 1;
   }
   if (spell_type(spell_types [j], 14) == 1)
   {
    print_slash(already);
    cprintf("Ice");
    already = 1;
   }
   if (spell_type(spell_types [j], 22) == 1)
   {
    print_slash(already);
    cprintf("Earth");
    already = 1;
   }
   if (spell_type(spell_types [j], 23) == 1)
   {
    print_slash(already);
    cprintf("Air");
    already = 1;
   }
   if (spell_type(spell_types [j], 11) == 1)
   {
    print_slash(already);
    cprintf("Conjuration");
    already = 1;
   }
   if (spell_type(spell_types [j], 12) == 1)
   {
    print_slash(already);
    cprintf("Enchantment");
    already = 1;
   }
   if (spell_type(spell_types [j], 19) == 1)
   {
    print_slash(already);
    cprintf("Divination");
    already = 1;
   }
   if (spell_type(spell_types [j], 20) == 1)
   {
    print_slash(already);
    cprintf("Translocation");
    already = 1;
   }
   if (spell_type(spell_types [j], 18) == 1)
   {
    print_slash(already);
    cprintf("Summoning");
    already = 1;
   }
   if (spell_type(spell_types [j], 15) == 1)
   {
    print_slash(already);
    cprintf("Transmigration");
    already = 1;
   }
   if (spell_type(spell_types [j], 16) == 1)
   {
    print_slash(already);
    cprintf("Necromancy");
    already = 1;
   }



                        gotoxy(65, wherey());

         char sval [2];

         itoa((int) spell_value(spell_types [j]), sval, 10);
         cprintf(sval);


         cprintf("\n\r");

   spelcount ++;


   } // end of for j




   /*
   for (i = 0; i < 9; i ++)
   {
      stringy [i] += 48;

   }*/


   //cprintf(stringy);

   //getch();


//   goto inpy;


//if (getch() == 0) getch();

char keyn = getch();
if (keyn == 0) getch();

#ifdef DOS
puttext(1, 1, 80, 25, buffer);
window(1, 18, 80, 25);
#endif

return keyn;

}




int which_spell_in_book(int sbook_type, int spl)
{
int func_pass [10];

spellbook_template(sbook_type, func_pass);

return func_pass [spl];



}

int spell_hunger(int spell_value, unsigned char spell)
{

switch(spell_value)
{
 case 1: return 50;
 case 2: return 95;
 case 3: return 160;
 case 4: return 250;
 case 5: return 350;
 case 6: return 550;
 case 7: return 700;
 case 8: return 850;
 case 9: return 1000;
 case 10: return 1000;
 case 11: return 1100;
 case 12: return 1250;
 case 13: return 1380;
 case 14: return 1500;
 case 15: return 1600;
 default: return 1600 + spell_value * 20;
}

return spell;

} // end of spell_hunger

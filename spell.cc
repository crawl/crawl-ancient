#include "config.h"

#include "beam.h"
#include "effects.h"
#include "externs.h"
#include "fight.h"
#include "items.h"
#include "monplace.h"
#include "mutation.h"
#include "player.h"
#include "religion.h"
#include "skills.h"
#include "spell.h"
#include "spells.h"
#include "spells0.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "stuff.h"
#include "transfor.h"
#include "it_use2.h"
#include "view.h"

/*void zapping(char ztype, int power, struct bolt beams [1])
char spell_direction(struct dist spelld [1], struct bolt beam [1])*/

void surge_power(int spell);

void cast_a_spell(void)
{

char spc = 0;
char spc2 = 0;
int spellh = 0;
int powc = 0;

if (you[0].spell_no == 0)
{
        strcpy(info, "You don't know any spells.");
        mpr(info);
        return;
}

if (you[0].berserker != 0)
{
 strcpy(info, "You are too berserk!");
 mpr(info);
 return;
}

query : strcpy(info, "Cast which spell? (? or * to list)");
mpr(info);

int keyin = get_ch();

if (keyin == '?' || keyin == '*')
{
        char unthing = spell_list(); //you[0].spells); //invent(0, you[0].inv_quant, you[0].inv_dam, you[0].inv_class, you[0].inv_type, you[0].inv_plus, you[0].inv_ident, you[0].equip [0], you[0].equip [6], you[0].equip [5], you[0].equip [2], you[0].equip [1], you[0].equip [3], you[0].equip [4], you[0].ring);
#ifdef PLAIN_TERM
redraw_screen();
#endif

        if (unthing == 2) return;

        if ((unthing >= 65 && unthing <= 90) || (unthing >= 97 && unthing <= 122))
        {
                keyin = unthing;
        } else
        {
       mesclr();
                goto query;
        }
}

spc = (int) keyin;

if (spc < 97 || spc > 121)
{
        unknown : strcpy(info, "You don't know that spell.");
        mpr(info);
        return;
}

spc2 = conv_lett(spc);

if (you[0].spells [spc2] == 210)
{
        goto unknown;
}

if (spell_value(you[0].spells [spc2]) > you[0].ep)
{
        strcpy(info, "You don't have enough magic to cast that spell.");
        mpr(info);
        return;
}

if (you[0].hunger <= spell_hunger(spell_value(you[0].spells [spc2]), you[0].spells [spc2]) || you[0].hung_state <= 1)
{
 strcpy(info, "You don't have the energy to cast that spell.");
 mpr(info);
 you[0].turnover = 1; // ??
 return;
}

you[0].ep -= spell_value(you[0].spells [spc2]);
you[0].ep_ch = 1;

if (player_energy() == 0 && you[0].is_undead < 2)
{
 spellh = spell_hunger(spell_value(you[0].spells [spc2]), you[0].spells [spc2]);
 spellh -= you[0].intel * you[0].skills [25];
 if (spellh < 1) spellh = 1;
 you[0].hunger -= spellh;
 you[0].hung_ch = 1;
 how_hungered(spellh);
}

//beam[0].thing_thrown = 3;

you[0].turnover = 1;
alert();

if (you[0].conf != 0)
{
 random_uselessness(2 + random2(7), 0);
 return;
}


switch(your_spells(you[0].spells [spc2], powc, 1))
{
 case 1: exercise_spell(you[0].spells [spc2], 1, 1); break;
 case 0: exercise_spell(you[0].spells [spc2], 1, 0); break;
/* -1 just falls through */
}

you[0].turnover = 1;


} // end of void cast_a_spell




char your_spells(int spc2, int powc, char allow_fail)
{
int dem_hor = 0;
int dem_hor2 = 0;

struct dist spd [1];
struct bolt beam [1];
//struct bolt beam2 [1];

alert();

powc = spell_spec(spc2, powc);
surge_power(spc2);
powc = magic_ability(powc, you[0].intel);

if (allow_fail == 1)
{

int spfl = random2(33) + random2(34) + random2(35);

 if (spfl < spell_fail(spc2))
 {
  mpr("You miscast the spell.");
  if (you[0].religion == 9 && you[0].piety >= 100 && random2(150) <= you[0].piety)
  {
   mpr("Nothing appears to happen.");
   return 0;
  }
  char sptype = 0;
  do
  {
   sptype = 11 + random2(13);
  } while (spell_type(spc2, sptype) == 0);
  miscast_effect(sptype, spell_value(spc2), spell_fail(spc2) - spfl, 100);
  if (you[0].religion == 5 && random2(75) < spell_value(spc2)) Xom_acts(random2(2), spell_value(spc2), 0);
  return 0;
 }
}

if (you[0].species == 12 && spell_type(spc2, 17) == 1)
{
  strcpy(info, "You can't use this type of magic!");
  mpr(info);
  return -1;
}

if (spc2 == 62 || spc2 == 82 || spc2 == 119 || spc2 == 120 || spc2 == 121 || spc2 == 118)
{
 naughty(2, 10 + spell_value(spc2)); // not necromancy, but bad for other reasons
}

if (spell_type(spc2, 16) == 1) naughty(1, 10 + spell_value(spc2));

if (you[0].religion == 9 && you[0].piety < 200) /* Sif Muna */
{
 if (random2(12) <= spell_value(spc2)) gain_piety(1);
}

switch(spc2)
{
        case 0: identify(0); return 1; // will only ever id one item
        case 1: you_teleport(); return 1;
        case 2: mass_enchantment(4, powc); return 1; // fear
        case 3:
                strcpy(info, "You hear a voice call your name!");
                mpr(info);
                noisy(25, you[0].x_pos, you[0].y_pos);
                break;
        case 4: remove_curse(); return 1;

        case 5:
 if (spell_direction(spd, beam) == -1) return 1;
 zapping(4, powc, beam);
 return 1; // magic missile

        case 6:
 fireball(powc); return 1;

   case 13: conjure_flame(powc); return 1;

   case 14:
   if (spell_direction(spd, beam) == -1) return 1;
   if (beam[0].move_x == 0 && beam[0].move_y == 0)
   {
    strcpy(info, "Why would you want to do that?");
    mpr(info);
    return 1;
   }
   zapping(11, powc, beam);
   return 1; // dig

   case 15: if (spell_direction(spd, beam) == -1) return 1;
   zapping(7, powc, beam);
   return 1; // fire bolt

   case 16: if (spell_direction(spd, beam) == -1) return 1;
   zapping(8, powc, beam);
   return 1; // frost bolt


   case 17: if (spell_direction(spd, beam) == -1) return 1;
   zapping(14, powc, beam);
   return 1; // lightning bolt


   case 20: if (spell_direction(spd, beam) == -1) return 1;
   if (beam[0].move_x == 0 && beam[0].move_y == 0)
   {
    strcpy(info, "Sorry, it doesn't work like that.");
    mpr(info);
    return 1;
   }
   zapping(15, powc, beam);
   return 1; // polymorph

   case 21: if (spell_direction(spd, beam) == -1) return 1;
   zapping(2, powc, beam);
   return 1; // you[0].slow

   case 22: if (spell_direction(spd, beam) == -1) return 1;
   zapping(3, powc, beam);
   return 1; // you[0].haste

   case 23: if (spell_direction(spd, beam) == -1) return 1;
   zapping(6, powc, beam);
   return 1; // paralyze

   case 24: if (spell_direction(spd, beam) == -1) return 1;
   zapping(9, powc, beam);
   return 1; // confuse

   case 25: if (spell_direction(spd, beam) == -1) return 1;
   zapping(10, powc, beam);
   return 1; // you[0].invis


   case 26: if (spell_direction(spd, beam) == -1) return 1;
   zapping(0, powc, beam);
   return 1; // throw flame

   case 27: if (spell_direction(spd, beam) == -1) return 1;
   zapping(1, powc, beam);
   return 1; // throw frost

   case 28:
   blink();
   return 1;

   case 29:
   cast_big_c(powc, 3);
   return 1;

   case 30:
   stinking_cloud();
   return 1;

   case 31:
   cast_ring_of_flames(powc);
   return 1;

   case 32:
   restore_str();
   return 1;

   case 33:
   restore_int();
   return 1;

   case 34:
   restore_dex();
   return 1;

   case 35: if (spell_direction(spd, beam) == -1) return 1;
   zapping(16, powc, beam);
   return 1; // venom bolt

   case 36: cast_toxic_radiance();
   return 1;

   case 37: if (spell_direction(spd, beam) == -1) return 1;
   if (beam[0].move_x == 0 && beam[0].move_y == 0)
   {
    strcpy(info, "Sorry, it doesn't work like that.");
    mpr(info);
    return 1;
   }
   zapping(13, powc, beam);
   break; // teleport creature (I think)

   case 38:
   cast_lesser_healing();
   break;

   case 39:
   cast_greater_healing();
   break;

   case 40:
   cast_cure_poison(powc);
   break;

   case 41:
   purification();
   break;

   case 42:
   cast_deaths_door(powc);
   break;

   case 43:
   cast_selective_amnesia();
   break;

   case 44: // mass you[0].conf
   mass_enchantment(5, powc);
   break;

   case 45: // smite
   cast_smiting(powc);
   break;

   case 46: // repel undead
   turn_undead(50);
   break;

   case 47: // holy word
   holy_word(50);
//   you[0].conf += random2(3) + 2;
   break;

   case 48:
   detect_curse();
   break;

   case 49:
   summon_small_mammals();
   break;

   case 50:
   abjuration(powc);
   break;

   case 51:
   summon_scorpions(powc);
   break;

   case 52: // levitation
   potion_effect(7, 40);
   break;

   case 53:  if (spell_direction(spd, beam) == -1) return 1;
   zapping(17, powc, beam);
   return 1; // bolt of draining

   case 54: if (spell_direction(spd, beam) == -1) return 1;
   zapping(18, powc, beam);
   return 1; // spray of metal

   case 55: if (spell_direction(spd, beam) == -1) return 1;
   zapping(19, powc, beam);
   return 1; // bolt of inaccuracy

   case 56: // you[0].poison cloud
   cast_big_c(powc, 4);
   return 1;

   case 57: // fire storm
   cast_fire_storm(powc);
   return 1;

   case 58: // detect_traps
   if (detect_traps() > 0)
   {
        strcpy(info, "You detect some traps!");
   } else strcpy(info, "You detect nothing.");
   mpr(info);
   return 1;

   case 59: // random blink
   random_blink();
   return 1;

   case 60: // Mystic blast
   if (spell_direction(spd, beam) == -1) return 1;
   zapping(20, powc, beam);
   return 1;

   case 61: // Swarm
   summon_swarm(powc);
   return 1;

   case 62: // Summon Things
   if (player_sust_abil() != 0 || you[0].intel == 3)
   {
        strcpy(info, "Your call goes unanswered.");
        mpr(info);
        return 1;
   }
   summon_things(powc);
   return 1;

   case 63: if (spell_direction(spd, beam) == -1) return 1;
   zapping(21, powc, beam);
   return 1; // enslavement

   case 64: // magic mapping
   if (you[0].level_type == 1 || you[0].level_type == 2)
   {
    strcpy(info, "You feel momentarily disoriented.");
    mpr(info);
    return 1;
   }
   strcpy(info, "You feel aware of your surroundings.");
   mpr(info);
   magic_mapping((powc * 2 + 5), 40 + powc * 2);
   return 1;

   case 65: // heal other
   if (spell_direction(spd, beam) == -1) return 1;
   if (beam[0].move_x == 0 && beam[0].move_y == 0)
   {
    strcpy(info, "Sorry, it doesn't work like that.");
    mpr(info);
    return 1;
   }
   zapping(5, powc, beam);
   return 1;

   case 66: // Animate Dead
   strcpy(info, "You call on the dead to walk for you.");
   mpr(info);
   animate_dead(powc + 1, 7, you[0].pet_target, 1);
   return 1;

   case 67: if (spell_direction(spd, beam) == -1) return 1;
   if (you[0].hp > 1) you[0].hp --;
   you[0].hp_ch = 1;
   zapping(22, powc, beam);
   return 1; // Pain

   case 68:
   extension(powc);
   return 1;

   case 69: // control undead
   mass_enchantment(30, powc);
   return 1;

   case 70: // animate skeleton
   strcpy(info, "You attempt to give life to the dead...");
   mpr(info);
   animate_a_corpse(you[0].x_pos, you[0].y_pos, 7, you[0].pet_target, 1);
   return 1;

   case 71: // vampiric draining
   vampiric_drain(powc);
   return 1;

   case 72: // Summon greater undead
   summon_undead(powc);
   return 1;

   case 73: // detect_items
   detect_items(50);
   return 1;

   case 74: // reviv
   cast_revivification(powc);
   return 1;

   case 75: // burn
   burn_freeze(powc, 2);
   return 1;

   case 76: // freeze
   burn_freeze(powc, 3);
   return 1;

   case 77: // summon elemental
   summon_elemental(powc, 0, 2);
   break;

   case 78: // refr
   cast_refrigeration(powc);
   return 1;

   case 79: // sticky flame
   if (spell_direction(spd, beam) == -1) return 1;
   zapping(23, powc, beam);
   return 1;

   case 80: // ice beast
   summon_ice_beast_etc(powc, 34);
   return 1;

   case 81: // ice armour
   ice_armour(powc, 0);
   return 1;

   case 82: // imp
   if (random2(3) == 0) summon_ice_beast_etc(powc, 220);
    else if (random2(7) == 0) summon_ice_beast_etc(powc, 237);
           else summon_ice_beast_etc(powc, 8);
   return 1;

   case 83: // deflect missiles
   missile_prot(powc);
   return 1;

   case 84: // berserk
   cast_berserk();
   return 1;

   case 85: // dispel undead
   if (spell_direction(spd, beam) == -1) return 1;
   zapping(24, powc, beam);
   return 1;

   case 86: // guardian
   summon_ice_beast_etc(powc, 26);
   return 1;

   case 88: // Thunderbolt
   if (spell_direction(spd, beam) == -1) return 1;
   zapping(14, powc, beam);
   return 1;

   case 89: // flame of cleansing
   if (spell_direction(spd, beam) == -1) return 1;
   zapping(25, powc, beam);
   return 1;


   case 91: // Summon Deva
   summon_ice_beast_etc(powc, 26);
   return 1;


   case 92:
   abjuration(powc);
   break;

   // Remember that most holy spells above don't yet use powc!

   case 110: // twisted resurrection
   cast_twisted(powc, 7, you[0].pet_target);
   break;

   case 111: // regeneration
   cast_regen(powc);
   return 1;

   case 112: // bone shards
   cast_bone_shards(powc);
   return 1;

   case 113: // banishment
   if (spell_direction(spd, beam) == -1) return 1;
/*   if (beam[0].move_x == 0 && beam[0].move_y == 0)
   {
    strcpy(info, "Why would you want to do that?");
    mpr(info);
    return 1;
   }*/
   zapping(27, powc, beam);
   return 1;

   case 114: // degeneration
   if (spell_direction(spd, beam) == -1) return 1;
   if (beam[0].move_x == 0 && beam[0].move_y == 0)
   {
    strcpy(info, "Why would you want to do that?");
    mpr(info);
    return 1;
   }
   zapping(28, powc, beam);
   return 1;

   case 115: // sting
   if (spell_direction(spd, beam) == -1) return 1;
   zapping(29, powc, beam);
   return 1;

   case 116: // sublimation
   sublimation(powc);
   return 1;

   case 117: // dancing weapon
   dancing_weapon(powc, 0);
   return 1;

   case 118: // hellfire - should only be available from staff of Dispater & Sceptre of Asmodeus
   if (spell_direction(spd, beam) == -1) return 1;
   zapping(30, powc, beam);
   return 1;

   case 119: // summon demon
   strcpy(info, "You open a gate to Pandemonium!");
   mpr(info);
   summon_ice_beast_etc(powc, summon_any_demon(1));
   return 1;

   case 120: // demonic horde
   strcpy(info, "You open a gate to Pandemonium!");
   mpr(info);
   dem_hor2 = 3 + random2(5);
   for (dem_hor = 0; dem_hor < 4 + dem_hor2; dem_hor ++)
   {
    summon_ice_beast_etc(powc, summon_any_demon(0));
/*    if (random2(6) == 0) summon_ice_beast_etc(powc, summon_any_demon(0));
    else summon_ice_beast_etc(powc, 220 + random2(5));*/
   }
   return 1;

   case 121: // greater demon
   strcpy(info, "You open a gate to Pandemonium!");
   mpr(info);
   dem_hor = 100;
   if (random2(powc) <= 5)
   {
                mpr("You don't feel so good about this...");
        dem_hor = 1;
   }
   create_monster(summon_any_demon(2), 24, dem_hor, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
   return 1;

   case 122: // rot corpses
   corpse_rot(0);
   return 1;

   case 123: // takumi's vorpal blade
   if (brand_weapon(10, powc) == 0)
   {
    strcpy(info, "The spell fizzles.");
    mpr(info);
   }
   return 1;

   case 124: // Fire brand
   if (brand_weapon(1, powc) == 0)
   {
    strcpy(info, "The spell fizzles.");
    mpr(info);
   }
   return 1;

   case 125: // Ice Aura
   if (brand_weapon(2, powc) == 0)
   {
    strcpy(info, "The spell fizzles.");
    mpr(info);
   }
   return 1;

   case 126: // lethal infusion
   if (brand_weapon(7, powc) == 0)
   {
    strcpy(info, "The spell fizzles.");
    mpr(info);
   }
   return 1;

   case 127: // pebble
   if (spell_direction(spd, beam) == -1) return 1;
   zapping(32, powc, beam);
   return 1;

   case 128: // bolt of iron
   if (spell_direction(spd, beam) == -1) return 1;
   zapping(31, powc, beam);
   return 1;

   case 129: // stone arrow
   if (spell_direction(spd, beam) == -1) return 1;
   zapping(33, powc, beam);
   return 1;

   case 130: // entomb
   entomb();
   return 1;

   case 131: // stone scales
   stone_scales(powc);
   return 1;


   case 132: // shock
   if (spell_direction(spd, beam) == -1) return 1;
   zapping(34, powc, beam);
   return 1;

   case 133: // swiftness
   cast_swiftness(powc);
   return 1;

   case 134: // Fly
   cast_fly(powc);
   return 1;

   case 135: // insulation
   cast_insulation(powc);
   return 1;

   case 136: // orb of electrocution
   if (spell_direction(spd, beam) == -1) return 1;
   zapping(35, powc, beam);
   return 1;

   case 137: // detect creatures
   detect_creatures(powc);
   return 1;

   case 138: // poison magic version of cure poison
   cast_cure_poison(powc);
   break;

   case 139: // teleport control
   cast_teleport_control(powc);
   return 1;

   case 140: // poison ammunition
   cast_poison_ammo();
   return 1;

   case 141: // poison blade
   if (brand_weapon(6, powc) == 0)
   {
    strcpy(info, "The spell fizzles.");
    mpr(info);
   }
   return 1;

   case 142: // resist poison
   cast_resist_poison(powc);
   return 1;

   case 143: // create noise 2
   create_noise2();
   return 1;

   case 144: // mutation
   if (you[0].hp < you[0].hp_max / 2)
   {
    mpr("Your body is in too bad a condition for this spell to function.");
    return 1;
   }
   mpr("Your body is suffused with transfigurative energy!");
   you[0].hp = random2(you[0].hp) + 1;
   you[0].hp_ch = 1;
   if (mutate(100) == 0) mpr("The spell fails.");
   return 1;

   case 145: // debug
   if (spell_direction(spd, beam) == -1) return 1;
   zapping(37, powc, beam);
   return 1;

   case 146: // recall
   recall(0);
   return 1;

   case 147: // portal
   portal();
   return 1;

   case 148: if (spell_direction(spd, beam) == -1) return 1;
   if (beam[0].move_x == 0 && beam[0].move_y == 0)
   {
    mpr("Why would you want to do that?");
    return 1;
   }
   zapping(44, powc, beam);
   return 1; // Agony

   case 149: // spider form
   transform(powc, 1);
   return 1;

   case 150: if (spell_direction(spd, beam) == -1) return 1;
   if (beam[0].move_x == 0 && beam[0].move_y == 0)
   {
    mpr("Why would you want to do that?");
    return 1;
   }
   zapping(45, powc, beam);
   return 1; // Disrupt

   case 151: if (spell_direction(spd, beam) == -1) return 1;
   if (beam[0].move_x == 0 && beam[0].move_y == 0)
   {
    strcpy(info, "Why would you want to do that?");
    mpr(info);
    return 1;
   }
   zapping(46, powc, beam);
   return 1; // Disintegrate

   case 152: // blade hands
   transform(powc, 2);
   return 1;

   case 153: // statue
   transform(powc, 3);
   return 1;

   case 154: // ice beast
   transform(powc, 4);
   return 1;

   case 155: // dragon form
   transform(powc, 5);
   return 1;


   case 156: // Lich
   transform(powc, 6);
   return 1;

   case 157: // death chan
   cast_death_channel(powc);
   return 1;
/*
   case 158: // Cross
   cross_direct : if (spell_direction(spd, beam) == -1) return 1;
    if (beam[0].move_x == 0 && beam[0].move_y == 0)
    {
     mpr("Try aiming it away from yourself, okay?");
     goto cross_direct;
    }
   dem_hor = beam[0].move_x;
   dem_hor2 = beam[0].move_y;
   zapping(47, powc, beam);
   if (beam[0].move_x == 0)
   {
    beam[0].move_y *= -1;
    zapping(47, powc, beam);
    beam[0].move_x = beam[0].move_y;
    beam[0].move_y = 0;
    zapping(47, powc, beam);
    beam[0].move_x *= -1;
    zapping(47, powc, beam);
    return 1;
   }
   if (beam[0].move_y == 0)
   {
    beam[0].move_x *= -1;
    zapping(47, powc, beam);
    beam[0].move_y = beam[0].move_x;
    beam[0].move_x = 0;
    zapping(47, powc, beam);
    beam[0].move_y *= -1;
    zapping(47, powc, beam);
    return 1;
   }
   dem_hor *= -1;
   beam[0].move_x = dem_hor;
   beam[0].move_y = dem_hor2;
   beam[0].source_x = you[0].x_pos;
   beam[0].source_y = you[0].y_pos;
   beam[0].bx = you[0].x_pos;
   beam[0].by = you[0].y_pos;
   beam[0].target_x = 0;
   beam[0].target_y = 0;
   zapping(47, powc, beam);
   dem_hor2 *= -1;
   beam[0].move_y = dem_hor2;
   beam[0].move_x = dem_hor;
   beam[0].source_x = you[0].x_pos;
   beam[0].source_y = you[0].y_pos;
   beam[0].bx = you[0].x_pos;
   beam[0].by = you[0].y_pos;
   beam[0].target_x = 0;
   beam[0].target_y = 0;
   zapping(47, powc, beam);
   dem_hor *= -1;
   beam[0].move_x = dem_hor;
   beam[0].move_y = dem_hor2;
   beam[0].source_x = you[0].x_pos;
   beam[0].source_y = you[0].y_pos;
   beam[0].bx = you[0].x_pos;
   beam[0].by = you[0].y_pos;
   beam[0].target_x = 0;
   beam[0].target_y = 0;
   zapping(47, powc, beam);
   return 1;
*/
default:
strcpy(info, "Invalid spell!");
mpr(info);
break;

} // end switch

return 0; /* hopefully this will never happen */

} // end spell_effects



void surge_power(int spell)
{

int enhanced = 0;

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
  if (you[0].special_wield == 50) enhanced -= 2; // shadow lantern
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
  enhanced += player_spec_earth(); // spec_earth
  enhanced -= player_spec_air(); // spec_air
} // earth
if (spell_type(spell, 23) == 1)
{
  enhanced -= player_spec_earth(); // spec_earth
  enhanced += player_spec_air(); // spec_air
} // air

/* If the bit just above changes, remember to also change it in fn2.cc */

if (enhanced == 1)
{
 strcpy(info, "You feel a surge of power.");
 mpr(info);
}
if (enhanced == 2)
{
 strcpy(info, "You feel a strong surge of power.");
 mpr(info);
}
if (enhanced >= 3) // how would you get this???
{
 strcpy(info, "You feel a huge surge of power!");
 mpr(info);
}
if (enhanced == -1)
{
 strcpy(info, "You feel a numb sensation.");
 mpr(info);
}
if (enhanced == -2)
{
 strcpy(info, "You feel an extremely numb sensation.");
 mpr(info);
}
if (enhanced <= -3)
{
 strcpy(info, "You feel an extraordinarily numb sensation.");
 mpr(info);
}


}




void exercise_spell(int spell_ex, int spc, int divide)
{

/* divide reduces skill increase for miscast spells */

int s = 0;
int spellsy = 0;

for (s = 11; s < 24; s ++)
{
 if (s == 17) continue; // Holy magic has no skill
 if (spell_type(spell_ex, s) == 1) spellsy ++;
}

if (divide == 1) spellsy += 2 + random2(10);

// exercise(26, random2((spell_value(spell_ex)) * (10 + spell_value(spell_ex) * 2) / 10 / spellsy + 1);
if (spellsy != 0)
{
if (spell_type(spell_ex, 11) == 1)
{
 exercise(26, (spell_value(spell_ex)) * (10 + spell_value(spell_ex)) / 10 / spellsy); // + 1);
}
if (spell_type(spell_ex, 12) == 1)
{
 exercise(27, (spell_value(spell_ex)) * (10 + spell_value(spell_ex)) / 10 / spellsy); // + 1);
}
if (spell_type(spell_ex, 15) == 1)
{
 exercise(31, (spell_value(spell_ex)) * (10 + spell_value(spell_ex)) / 10 / spellsy); // + 1);
}
if (spell_type(spell_ex, 16) == 1)
{
 exercise(29, (spell_value(spell_ex)) * (10 + spell_value(spell_ex)) / 10 / spellsy); // + 1);
}
if (spell_type(spell_ex, 18) == 1)
{
 exercise(28, (spell_value(spell_ex)) * (10 + spell_value(spell_ex)) / 10 / spellsy); // + 1);
}
if (spell_type(spell_ex, 19) == 1)
{
 exercise(32, (spell_value(spell_ex)) * (10 + spell_value(spell_ex)) / 10 / spellsy); // + 1);
}
if (spell_type(spell_ex, 20) == 1)
{
 exercise(30, (spell_value(spell_ex)) * (10 + spell_value(spell_ex)) / 10 / spellsy); // + 1);
}
if (spell_type(spell_ex, 21) == 1)
{
 exercise(37, (spell_value(spell_ex)) * (10 + spell_value(spell_ex)) / 10 / spellsy); // + 1);
}
if (spell_type(spell_ex, 22) == 1)
{
 exercise(36, (spell_value(spell_ex)) * (10 + spell_value(spell_ex)) / 10 / spellsy); // + 1);
}
if (spell_type(spell_ex, 23) == 1)
{
 exercise(35, (spell_value(spell_ex)) * (10 + spell_value(spell_ex)) / 10 / spellsy); // + 1);
}
if (spell_type(spell_ex, 13) == 1)
{
 exercise(33, (spell_value(spell_ex)) * (10 + spell_value(spell_ex)) / 10 / spellsy); // + 1);
}
if (spell_type(spell_ex, 14) == 1)
{
 exercise(34, (spell_value(spell_ex)) * (10 + spell_value(spell_ex)) / 10 / spellsy); // + 1);
}

} // end if spellsy != 0

/*if (you[0].skills [25] > 0)
{
 if (spellsy != 0)
 {
  if (spc == 1) exercise(25, (random2(spell_value(spell_ex)) * (10 + spell_value(spell_ex))) / 10 / spellsy); // + 1);
 } else if (spc == 1) exercise(25, (random2(spell_value(spell_ex)) * (10 + spell_value(spell_ex))) / 10 / 1); // + 1);
} else
 if (spellsy != 0)
 {
  if (spc == 1) exercise(25, random2(random2(spell_value(spell_ex)) * (10 + spell_value(spell_ex))) / 10 / spellsy); // + 1);
 } else if (spc == 1) exercise(25, random2(random2(spell_value(spell_ex)) * (10 + spell_value(spell_ex))) / 10 / 1); // + 1);
*/

//if (you[0].skills [25] > 0)
//{
 if (spellsy != 0)
 {
  if (spc == 1) exercise(25, random2(random2(spell_value(spell_ex)) / spellsy)); // + 1);
 } else if (spc == 1) exercise(25, random2(random2(spell_value(spell_ex)))); // + 1);
//}
/* else
 if (spellsy != 0)
 {
  if (spc == 1) exercise(25, random2(random2(spell_value(spell_ex)) / spellsy); // + 1);
 } else if (spc == 1) exercise(25, random2(random2(spell_value(spell_ex))); // + 1);
*/
//random2(spell_value(spell_ex) * 10 + spell_value(spell_ex) * 2) / 10);

//power += (player_skills [25] * 5) / 10;

}

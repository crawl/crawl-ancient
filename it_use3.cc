#include "config.h"

#include <string.h>

#include "externs.h"
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

extern unsigned char show_green; /* defined in view.cc, I think */
extern int book_thing; /* defined in spells.cc */

void special_wielded(void)
{

switch(you[0].special_wield)
{
case 1:
if (random2(20) == 0)
{
 strcpy(info, "The Singing Sword ");
 switch(random2(32))
 {
  case 0: strcat(info, "hums a little tune."); mpr(info); break;
  case 1: strcat(info, "breaks into glorious song!"); mpr(info); break;
  case 2: strcat(info, "sings."); mpr(info); break;
  case 3: strcat(info, "sings loudly."); mpr(info); break;
  case 4: strcat(info, "chimes melodiously."); mpr(info); break;
  case 5: strcat(info, "makes a horrible noise."); mpr(info); break;
  case 6: strcat(info, "sings off-key."); mpr(info); break;
  case 7: strcat(info, "sings 'tra-la-la'."); mpr(info); break;
  case 8: strcat(info, "burbles away merrily."); mpr(info); break;
  case 9: strcat(info, "gurgles."); mpr(info); break;
  case 10: strcat(info, "suddenly shrieks!"); mpr(info); break;
  case 11: strcat(info, "cackles."); mpr(info); break;
  case 12: strcat(info, "warbles."); mpr(info); break;
  case 13: strcat(info, "chimes harmoniously."); mpr(info); break;
  case 14: strcat(info, "makes beautiful music."); mpr(info); break;
  case 15: strcat(info, "produces a loud orchestral chord."); mpr(info); break;
  case 16: strcat(info, "whines plaintively."); mpr(info); break;
  case 17: strcat(info, "tinkles."); mpr(info); break;
  case 18: strcat(info, "rings like a bell."); mpr(info); break;
  case 19: strcat(info, "wails mournfully."); mpr(info); break;
  case 20: strcat(info, "practices its scales."); mpr(info); break;
  case 21: strcat(info, "lilts tunefully."); mpr(info); break;
  case 22: strcat(info, "hums tunelessly."); mpr(info); break;
  case 23: strcat(info, "sighs."); mpr(info); break;
  case 24: strcat(info, "makes a deep moaning sound."); mpr(info); break;
  case 25: strcat(info, "makes a popping sound."); mpr(info); break;
  case 26: strcat(info, "sings a sudden stoccato note."); mpr(info); break;
  case 27: strcat(info, "says 'Hi! I'm the Singing Sword!'."); mpr(info); break;
  case 28: strcat(info, "whispers something."); mpr(info); break;
  case 29: strcat(info, "speaks gibberish."); mpr(info); break;
  case 30: strcat(info, "raves incoherently."); mpr(info); break;
  case 31: strcat(info, "yells in some weird language."); mpr(info); break;
 }
   noisy(25, you[0].x_pos, you[0].y_pos);
}
break;


case 3: // scythe of Curses
if (random2(30) == 0) curse_an_item(0, 0);
break;

case 4: // mace of variability
if (you[0].inv_plus [you[0].equip [0]] > 100) you[0].inv_plus [you[0].equip [0]] -= 100;
if (random2(5) != 0) break;
you[0].inv_plus [you[0].equip [0]] += random2(2);
you[0].inv_plus [you[0].equip [0]] -= random2(2);
you[0].inv_plus2 [you[0].equip [0]] += random2(2);
you[0].inv_plus2 [you[0].equip [0]] -= random2(2);
if (you[0].inv_plus [you[0].equip [0]] < 46) you[0].inv_plus [you[0].equip [0]] = 46;
if (you[0].inv_plus [you[0].equip [0]] > 57) you[0].inv_plus [you[0].equip [0]] = 57;
if (you[0].inv_plus2 [you[0].equip [0]] < 46) you[0].inv_plus2 [you[0].equip [0]] = 46;
if (you[0].inv_plus2 [you[0].equip [0]] > 57) you[0].inv_plus2 [you[0].equip [0]] = 57;
you[0].inv_col [you[0].equip [0]] = random2(15) + 1;
break;

// 5 - glaive

case 6: // mace of torment
if (random2(200) == 0) torment();
break;

case 7:
if (random2(5) == 0) animate_dead(1 + random2(3), 1, MHITYOU, 1);
break;

case 8:
you[0].inv_plus [you[0].equip [0]] = 47 + you[0].hp / 11;
you[0].inv_plus2 [you[0].equip [0]] = 47 + you[0].hp / 11;
break;

case 9:
if (you[0].intel > 25)
{
 you[0].inv_plus [you[0].equip [0]] = 72;
 you[0].inv_plus2 [you[0].equip [0]] = 63;
} else
{
 you[0].inv_plus [you[0].equip [0]] = 47 + you[0].intel;
 you[0].inv_plus2 [you[0].equip [0]] = 50 + you[0].intel / 2;
}
break;

case 50: // shadow lamp
if (random2(8) <= 0 + player_spec_death()) create_monster(63, 21, 7, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
//naughty(5, 1);
show_green = DARKGREY;
break;

} // end switch

} // end void special_wielded





void invoke_wielded(void)
{
 char opened_gates = 0;
 unsigned char spell_casted = random2(21);
 int count_x, count_y;

if (you[0].equip [0] == -1)
{
 strcpy(info, "You aren't wielding anything!");
 mpr(info);
 return;
}

if (you[0].berserker != 0)
{
 strcpy(info, "You are too berserk!");
 mpr(info);
 return;
}

 you[0].turnover = 1;

switch(you[0].inv_class [you[0].equip [0]])
{
 case 0:
 switch(you[0].inv_dam [you[0].equip [0]])
 {
  case 189: // staff of Dispater
  if (you[0].deaths_door != 0 | you[0].hp <= 10 | you[0].ep <= 4) goto nothing_hap;
  strcpy(info, "You feel the staff feeding on your energy!");
  mpr(info);
  you[0].hp -= random2(10) + random2(10) + 5;
  if (you[0].hp <= 0) you[0].hp = 1;
  you[0].ep -= random2(3) + random2(3) + 2;
  if (you[0].ep <= 0) you[0].ep = 1;
  you[0].hp_ch = 1;
  you[0].ep_ch = 1;
  your_spells(118, 100, 0); // power (2nd number) is meaningless
  break;

  case 190: // sceptre of Asmodeus
  spell_casted = random2(21);
  if (spell_casted == 0) goto nothing_hap; // nothing happens
  if (spell_casted < 2) // summon devils, maybe a Fiend
  {
   spell_casted = 80 + random2(10);
   if (random2(4) == 0)
   {
    strcpy(info, "\"Your arrogance condemns you, mortal!\"");
    spell_casted = 31; /* Fiend! */
   } else strcpy(info, "The Sceptre summons one of its servants.");
    mpr(info);
    create_monster(spell_casted, 25, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
    break;
  }
  spell_casted = 15; // firebolt
  if (random2(3) == 0) spell_casted = 17; // lightning
  if (random2(4) == 0) spell_casted = 53; // draining
  if (random2(20) == 0) spell_casted = 118; // hellfire
  your_spells(spell_casted, 10, 0);
  break;

  case 193: // staff of Olgreb
  if (you[0].ep <= 5 | you[0].skills [25] <= random2(11)) goto nothing_hap;
  you[0].ep -= 4;
  if (you[0].ep <= 0) you[0].ep = 0;
  you[0].ep_ch = 1;
  your_spells(36, 100, 0); // toxic rad
  your_spells(35, 100, 0); // venom bolt
  break;

  case 195: // staff of Wucad Mu
  if (you[0].ep == you[0].ep_max | random2(4) == 0)
  {
   strcpy(info, "Nothing appears to happen.");
   mpr(info);
   break;
  }
  you[0].ep += 3 + random2(5);
  if (you[0].ep > you[0].ep_max) you[0].ep = you[0].ep_max;
  you[0].ep_ch = 1;
  strcpy(info, "Magical energy flows into your mind!");
  mpr(info);
  if (random2(3) == 0) miscast_effect(19, random2(9), random2(70), 100);
  break;

  default: nothing_hap : strcpy(info, "Nothing appears to happen.");
  mpr(info);
  break;
 }
 break;


 case 11:
 if (you[0].inv_type [you[0].equip [0]] == 27)
 {
  if (you[0].ep == you[0].ep_max | random2(4) == 0)
  {
   strcpy(info, "Nothing appears to happen.");
   mpr(info);
   break;
  }
  you[0].ep += 1 + random2(3);
  if (you[0].ep > you[0].ep_max) you[0].ep = you[0].ep_max;
  you[0].ep_ch = 1;
  strcpy(info, "You channel some magical energy.");
  mpr(info);
  break;
 }
 staff_spell(you[0].equip [0]);
 break;

 case 13: // misc
 switch(you[0].inv_type [you[0].equip [0]])
 {
  case 0: efreet_flask();
  break;
  case 1: ball_of_seeing();
  break;
  case 2:
  if (random2(2) == 0)
  {
   strcpy(info, "Nothing appears to happen.");
   mpr(info);
   break;
  }
  summon_elemental(100, 125, 4);
  break;
  case 3:
  if (random2(2) == 0)
  {
   strcpy(info, "Nothing appears to happen.");
   mpr(info);
   break;
  }
  summon_elemental(100, 124, 4);
  break;
  case 4:
  if (random2(2) == 0)
  {
   strcpy(info, "Nothing appears to happen.");
   mpr(info);
   break;
  }
  summon_elemental(100, 123, 4);
  break;
 case 6: // Horn of Geryon
 if (you[0].where_are_you == 3)
 {
  strcpy(info, "You produce a weird and mournful sound.");
  mpr(info);
  for (count_x = 0; count_x < 80; count_x ++)
  {
   for (count_y = 0; count_y < 80; count_y ++)
   {
    if (grd [count_x] [count_y] == 98)
    {
     opened_gates ++;
     switch(grd [count_x + 2] [count_y])
     {
      case 67: grd [count_x] [count_y] = 92; break;
      case 61: grd [count_x] [count_y] = 93; break;
      case 1: grd [count_x] [count_y] = 95; break;
      case 62: grd [count_x] [count_y] = 94; break;
     } // Note: This assumes that the Vestibule has not been substantially changed.
    }
   }
  }
  if (opened_gates != 0)
  {
   strcpy(info, "Your way has been unbarred.");
   mpr(info);
  }
 break;
 } else
 {
   strcpy(info, "You produce a hideous howling noise!");
   mpr(info);
   create_monster(88, 23, 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
 }
 break;

 case 7: // box of beasts
 box_of_beasts();
 break;

 case 8: // deck of wonders
 deck_of_wonders();
 break;

 case 9: // deck of summonings
 deck_of_summonings();
 break;

 case 10: // crystal ball of energy
 ball_of_energy();
 break;

 case 12: // crystal ball of fixation
 ball_of_fixation();
 break;

 case 13: // disc of storms
 disc_of_storms();
 break;


 default: strcpy(info, "Nothing appears to happen.");
 mpr(info);
 break;
 }
 break;


 default: strcpy(info, "Nothing appears to happen.");
 mpr(info);
 break;

}

you[0].turnover = 1;

} // end invoke_wielded



void efreet_flask(void)
{

strcpy(info, "You open the flask, and a huge efreet comes out.");
mpr(info);

unwield_item(you[0].equip [0]);
you[0].inv_quant [you[0].equip [0]] = 0;
you[0].equip [0] = -1;
you[0].inv_no --;

if (random2(5) != 0)
{
  strcpy(info, "\"Thank you for releasing me!\"");
  mpr(info);
  create_monster(68, 24, 7, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
  return;
}

  strcpy(info, "It howls insanely!");
  mpr(info);
  create_monster(68, 24, 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);

} // end efreet_flask

void ball_of_seeing(void)
{
 int use = 0;
 strcpy(info, "You gaze into the crystal ball.");
 mpr(info);
 use = random2(you[0].intel * 6); //magic_ability(you[0].intel, you[0].intel));
 if (you[0].conf != 0) use = 0;
 if (use < 2)
 {
  strcpy(info, "Your head hurts!");
  mpr(info);
  if (you[0].intel > 3) you[0].intel --;
  you[0].intel_ch = 1;
  return;
 }
 if (use < 5)
 {
  strcpy(info, "You feel power drain from you!");
  mpr(info);
  you[0].ep = 0;
  you[0].ep_ch = 1;
  return;
 }
 if (use < 10)
 {
  strcpy(info, "You feel confused.");
  mpr(info);
  you[0].conf += 10 + random2(10);
  if (you[0].conf > 40) you[0].conf = 40;
  return;
 }
 if (use < 15 | random2(2) == 0 | you[0].level_type == 1 | you[0].level_type == 2)
 {
  strcpy(info, "You see nothing.");
  mpr(info);
  return;
 }

 strcpy(info, "You see a map of your surroundings!");
 mpr(info);
 magic_mapping(80, 95 + random2(10));

} // end ball_of_seeing()

void disc_of_storms(void)
{

you[0].turnover = 1;

if ((random2(60) > 30 + you[0].skills [35] && random2(3) != 0) | you[0].attribute [1] != 0)
{
 strcpy(info, "Nothing appears to happen.");
 mpr(info);
 return;
}

if (random2(60) > 30 + you[0].skills [35] && random2(3) != 0)
{
 strcpy(info, "The disc glows for a moment, then fades.");
 mpr(info);
 return;
}

if (random2(60) > 30 + you[0].skills [35] && random2(3) != 0)
{
 strcpy(info, "Little bolts of electricity crackle over the disc.");
 mpr(info);
 return;
}

strcpy(info, "The disc erupts in an explosion of electricity!");
mpr(info);

char disc_count = 0;
char disc_count2 = 0;
unsigned char which_zap_thing;

disc_count2 = 2 + random2(4);

do
{

 switch(random2(3))
 {
  case 0: which_zap_thing = 14; break; // lightning bolt
  case 1: which_zap_thing = 34; break; // shock
  case 2: which_zap_thing = 35; break; // orb of elec
 }
 struct bolt beam [1];
 beam[0].target_x = 0;
 beam[0].target_y = 0;
 beam[0].move_x = random2(13) - 6;
 beam[0].move_y = random2(13) - 6;
 beam[0].source_x = you[0].x_pos;
 beam[0].source_y = you[0].y_pos;
 zapping(which_zap_thing, 30 + random2(20), beam);
 disc_count ++;

} while (disc_count <= disc_count2);

}





void staff_spell(char zap_device_2)
{

if (you[0].inv_plus [zap_device_2] == 64 | you[0].inv_type [zap_device_2] < 10 | you[0].inv_type [zap_device_2] >= 25)
{
   strcpy(info, "That staff has no spells in it.");
   mpr(info);
   return;
}


read_book(zap_device_2);
unsigned char specspell;
int powc = magic_ability(player_mag_abil(), you[0].intel);

int sc_read_1 = book_thing; // book_thing is got in read_book

if (sc_read_1 < 65 | (sc_read_1 > 90 && sc_read_1 < 97) | sc_read_1 > 122)
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



specspell = which_spell_in_book(you[0].inv_type [you[0].equip [0]] + 40, sc_read_2 + 1);

if (you[0].ep < spell_value(specspell) / 2 + 1 | you[0].xl < spell_value(specspell))
{
   strcpy(info, "Your brain hurts!");
   mpr(info);
   you[0].conf += 2 + random2(4);
   if (you[0].conf > 40) you[0].conf = 40;
   you[0].turnover = 1;
   return;
}

// note that spell staves cannot have empty spaces in strungy before the last real spell.

exercise_spell(specspell, 1, 1);
your_spells(specspell, powc, 0);

you[0].ep -= spell_value(specspell);
you[0].ep_ch = 1;

//if (you[0].inv_type [you[0].equip [0]] != 10) // staff of smiting uses no food
/*
if (you[0].energy == 0)
{
 spellh = spell_hunger(spell_value(specspell), specspell);
 spellh -= magic_ability(you[0].mag_abil, you[0].intel);
 if (spellh < 10) spellh = 10;
 you[0].hunger -= spellh;
 hung_ch = 1;
 how_hungered(spellh);
}
*/
you[0].turnover = 1;

return;

whattt:
strcpy(info, "What?");
mpr(info);

} // end staff_spell



void tome_of_power(char sc_read_2)
{

int powc = 0;
int spell_casted = 0;
char keyin = 0;
struct bolt beam [1];

strcpy(info, "The book opens to a page covered in ");

char str_pass [40];

weird_writing(str_pass);
strcat(info, str_pass);
strcat(info, ".");
mpr(info);

strcpy(info, "Read it?");
mpr(info);

question : keyin = get_ch();

switch (keyin)
{
 case 'y':
 case 'Y':
 goto read_it;

 case 'n':
 case 'N':
 you[0].turnover = 1;
 return;

 default: goto question;
}


read_it:

you[0].inv_ident [sc_read_2] = 3;
you[0].turnover = 1;

powc = magic_ability(player_mag_abil(), you[0].intel);


strcpy(info, "You find yourself reciting the magical words!");
mpr(info);

switch(random2(50))
{
case 0:
case 11:
case 12:
        if (random2(5) == 0)
        {
        strcpy(info, "The book disappears in a mighty explosion!");
        mpr(info);
        you[0].inv_quant [sc_read_2] = 0;
        you[0].inv_no --;
        if (sc_read_2 == you[0].equip [0])
        {
           unwield_item(sc_read_2);
           you[0].equip [0] = -1;
        }
           burden_change();
        }
        beam[0].type = 43;
        beam[0].damage = 115;
        beam[0].flavour = 2; // <=- not sure about this
        beam[0].bx = you[0].x_pos;
        beam[0].by = you[0].y_pos;
        strcpy(beam[0].beam_name, "fiery explosion");
        beam[0].colour = RED;
        beam[0].thing_thrown = 1; // your explosion, (not someone else's explosion)
        explosion(1, beam);
        return;

case 1:
case 14:
strcpy(info, "A cloud of choking fumes pours from the book's pages!");
mpr(info);
big_cloud(4, you[0].x_pos, you[0].y_pos, 20);
//if (random() % 3 == 0) return; else break;
return;

case 2:
case 13:
strcpy(info, "A cloud of freezing gas pours from the book's pages!");
mpr(info);
big_cloud(3, you[0].x_pos, you[0].y_pos, 20);
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
big_cloud(5 + random2(3), you[0].x_pos, you[0].y_pos, 20);
return;// else break;

case 10:
strcpy(info, "A horrible Thing appears!");
mpr(info);
strcpy(info, "It doesn't look too friendly.");
mpr(info);
create_monster(23, 25, 1, you[0].x_pos, you[0].y_pos, MHITNOT, 250);
return;

// next is 14

}

viewwindow(1);

switch(random2(23))
{
 case 0: spell_casted = 5; break;
 case 16:
 case 17:
 case 1: spell_casted = 6; break; // fireball
 case 21:
 case 18:
 case 2: spell_casted = 15; break;
 case 22:
 case 19:
 case 3: spell_casted = 16; break;
 case 20:
 case 4: spell_casted = 17; break;
 case 5: spell_casted = 20; break;
 case 6: spell_casted = 26; break;
 case 7: spell_casted = 27; break;
 case 8: spell_casted = 30; break;
 case 9: spell_casted = 35; break;
 case 10: spell_casted = 53; break;
 case 11: spell_casted = 54; break;
 case 12: spell_casted = 55; break;
 case 13: spell_casted = 79; break;
 case 14: spell_casted = 114; break;
 case 15: spell_casted = 1; break;
}

// also summon demons (when they're finished)

your_spells(spell_casted, powc, 0);
// note no exercise

}


void skill_manual(char sc_read_2)
{

char skname [30];
char keyin;

strcpy(info, "This is a manual of ");
strcat(info, skill_name(you[0].inv_plus [sc_read_2]));
strcat(info, "!");
mpr(info);

strcpy(info, "Read it?");
mpr(info);

question : keyin = get_ch();

switch (keyin)
{
 case 'y':
 case 'Y':
 goto read_it;

 case 'n':
 case 'N':
 you[0].turnover = 1;
 return;

 default: goto question;
}


read_it:

you[0].inv_ident [sc_read_2] = 3;
you[0].turnover = 1;

strcpy(skname, skill_name(you[0].inv_plus [sc_read_2]));

strcpy(info, "You read about ");
strcat(info, strlwr(skname));
strcat(info, ".");
mpr(info);

exercise(you[0].inv_plus [sc_read_2], 100);

if (random2(10) == 0)
{
        strcpy(info, "The book crumbles into dust.");
        mpr(info);
        you[0].inv_quant [sc_read_2] = 0;
        you[0].inv_no --;
        if (sc_read_2 == you[0].equip [0])
        {
         unwield_item(sc_read_2);
         you[0].equip [0] = -1;
        }
        burden_change();
} else
{
 strcpy(info, "The book looks increasingly old and worn.");
 mpr(info);
}


} // end skill_manual



void box_of_beasts(void)
{

int beasty = 0;

you[0].turnover = 1;

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
 you[0].inv_type [you[0].equip [0]] = 11;
 return;
}

switch(random2(11))
{
 case 0: beasty = 1; break; // bat
 case 1: beasty = 7; break; // hound
 case 2: beasty = 9; break; // jackal
 case 3: beasty = 17; break; // rat
 case 4: beasty = 34; break; // ice beast
 case 5: beasty = 44; break; // snake
 case 6: beasty = 50; break; // yak
 case 7: beasty = 66; break; // butterfly
 case 8: beasty = 73; break; // hell hound
 case 9: beasty = 128; break; // brown snake
 case 10: beasty = 129; break; // giant lizard
}

 create_monster(beasty, 21 + random2(4), 7, you[0].x_pos, you[0].y_pos, you[0].pet_target, 250);

 strcat(info, "and something leaps out!");
 mpr(info);


}

void ball_of_energy(void)
{

 int use = 0;
 int proportional = 0;

 strcpy(info, "You gaze into the crystal ball.");
 mpr(info);
 use = random2(you[0].intel * 6);
 if (you[0].conf != 0) use = 0;
 if (use < 2 | you[0].ep_max == 0)
 {
  strcpy(info, "Your head hurts!");
  mpr(info);
  if (you[0].intel > 3) you[0].intel --;
  you[0].intel_ch = 1;
  return;
 }
 if (use < 4 | you[0].ep == you[0].ep_max)
 {
  drain_away: strcpy(info, "You feel your power drain away!");
  mpr(info);
  you[0].ep = 0;
  you[0].ep_ch = 1;
  return;
 }
 if (use < 6)
 {
  strcpy(info, "You feel confused.");
  mpr(info);
  you[0].conf += 10 + random2(10);
  if (you[0].conf > 40) you[0].conf = 40;
  return;
 }

 proportional = you[0].ep * 100;
 proportional /= you[0].ep_max;

 if (random2(20) + random2(20) + random2(20) + random2(10) > proportional | random2(25) == 0)
    goto drain_away;

 strcpy(info, "You are suffused with power!");
 mpr(info);
 you[0].ep += 12 + random2(12) + random2(12);
 if (you[0].ep > you[0].ep_max) you[0].ep = you[0].ep_max;
 you[0].ep_ch = 1;
}

void ball_of_fixation(void)
{

 strcpy(info, "You gaze into the crystal ball.");
 mpr(info);
 strcpy(info, "You are mesmerised by a rainbow of scintillating colours!");
 mpr(info);

 you[0].paralysis = 100;
 you[0].slow = 100;

}

#include "config.h"

#include <string.h>

#include "externs.h"

/*#include "player.h"
#include "stuff.h"
#include "monplace.h"
#include "it_use2.h"
#include "effects.h"
#include "spells3.h"
#include "player.h"*/
#include "dot_h.h"
#include "mutation.h"

void deck_of_wonders(void)
{
int dvar [3];
int dvar1 = 0;

 strcpy(info, "You draw a card from the deck...");
 mpr(info);
// strcpy(info, "On it is a picture of ");

switch(random2(28))
{
default:
case 0:
strcpy(info, "It is blank.");
mpr(info);
break;
case 1:
strcpy(info, "It turns into a brightly coloured butterfly!");
mpr(info);
create_monster(66, 21 + random2(4), 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;
case 2:
strcpy(info, "On the card is a picture of you, surrounded by negative energies.");
mpr(info);
lose_level();
drain_exp();
break;
case 3:
strcpy(info, "On the card is a picture of you, looking more experienced.");
mpr(info);
if (you[0].xp < 27)
 you[0].xp = exp_needed(you[0].xl + 2, you[0].species) + 1;
level_change();
break;
case 4:
strcpy(info, "On the card is a picture of a chest brimming over with gold!");
mpr(info);
you[0].gp += 800 + random2(500) + random2(500);
you[0].gp_ch = 1;
break;
case 5:
strcpy(info, "On the card is a picture of a brain.");
mpr(info);
you[0].max_intel += 2 + random2(2) + random2(2);
you[0].intel_ch = 1; // note that only the maximum is increased
break;
case 6:
strcpy(info, "On the card is a picture of a muscle.");
mpr(info);
you[0].max_strength += 2 + random2(2) + random2(2);
you[0].strength_ch = 1;
break;
case 7:
strcpy(info, "On the card is a picture of a neural pathway.");
mpr(info);
you[0].max_dex += 2 + random2(2) + random2(2);
you[0].dex_ch = 1;
break;
case 8:
strcpy(info, "On the card is a picture of a swollen, deformed brain.");
mpr(info);
you[0].intel -= 2 + random2(2) + random2(2);
if (you[0].intel <= 3) you[0].intel = 0;
you[0].intel_ch = 1;
break;
case 9:
strcpy(info, "On the card is a picture of what looks like a heap of decaying muscle tissue.");
mpr(info);
you[0].strength -= 2 + random2(2) + random2(2);
if (you[0].strength <= 3) you[0].strength = 0;
you[0].strength_ch = 1;
break;
case 10:
strcpy(info, "On the card is a picture of a deteriorating neural pathway.");
mpr(info);
you[0].dex -= 2 + random2(2) + random2(2);
if (you[0].dex <= 3) you[0].dex = 0;
you[0].dex_ch = 1;
break;
case 11: // shuffle stats
strcpy(info, "On the card is a picture of a deck of cards being shuffled.");
mpr(info);
//you[0].strength = you[0].max_strength;
//you[0].intel = you[0].max_intel;
//you[0].dex = you[0].max_dex;
dvar [0] = you[0].strength;
dvar [1] = you[0].intel;
dvar [2] = you[0].dex;
you[0].strength = 101;
you[0].intel = 101;
you[0].dex = 101;
do
{
 dvar1 = random2(3);
 if (dvar [dvar1] == 101)
 {
  continue;
 }

 if (you[0].strength == 101)
 {
  you[0].strength = dvar [dvar1];
  you[0].max_strength = dvar [dvar1];
 } else
 if (you[0].intel == 101)
 {
  you[0].intel = dvar [dvar1];
  you[0].max_intel = dvar [dvar1];
 } else
 if (you[0].dex == 101)
 {
  you[0].dex = dvar [dvar1];
  you[0].max_dex = dvar [dvar1];
 }
 dvar [dvar1] = 101;
} while (dvar [0] != 101 | dvar [1] != 101 | dvar [2] != 101);
you[0].strength_ch = 1;
you[0].intel_ch = 1;
you[0].dex_ch = 1;
burden_change();
break;

case 12: // mutation
strcpy(info, "You see yourself as a deformed freak!");
mpr(info);
mutate(100);
mutate(100);
mutate(100);
mutate(100);
mutate(100);
mutate(100);
break;
case 13: // lose mutations
mpr("The card is blank.");
delete_mutation(100);
delete_mutation(100);
delete_mutation(100);
delete_mutation(100);
delete_mutation(100);
delete_mutation(100);
break;
case 23: // reaper
strcpy(info, "On the card is a picture of a hooded figure carrying a scythe.");
mpr(info);
strcpy(info, "Oh no! You drew the death card!");
mpr(info);
if (you[0].duration [13] != 0) you_teleport();
create_monster(83, 0, 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
create_monster(83, 0, 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
create_monster(83, 0, 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
create_monster(83, 0, 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
create_monster(83, 0, 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;
case 24: // soul eater
strcpy(info, "On the card is a picture of a depthless shadow.");
mpr(info);
create_monster(84, 0, 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;
case 25: // gate to abyss
strcpy(info, "On the card is a picture of a glowing gate.");
mpr(info);
more();
if (you[0].level_type != 0) break;
banished();
break;
case 26: // pet
strcpy(info, "On the card is a picture of a gleaming crystal statue.");
mpr(info);
create_monster(120, 0, 7, you[0].x_pos, you[0].y_pos, you[0].pet_target, 250);
break;
case 27: // acquirement
strcpy(info, "The card unfolds to form a scroll of paper.");
mpr(info);
acquirement(250);
break;

}

you[0].inv_plus [you[0].equip [0]] --;
if (you[0].inv_plus [you[0].equip [0]] == 0)
{
 strcpy(info, "The deck of cards disappears in a puff of smoke.");
 mpr(info);
 unwield_item(you[0].equip [0]);
 you[0].inv_quant [you[0].equip [0]] --;
 if (you[0].inv_quant [you[0].equip [0]] == 0) // can this be false?
 {
        you[0].inv_no --;
                you[0].equip [0] = -1;
                strcpy(info, "You are now empty handed.");
                mpr(info);
 }
}
burden_change();
}






void deck_of_summonings(void)
{

 strcpy(info, "You draw a card from the deck...");
 mpr(info);

switch(random2(13))
{
case 0:
strcpy(info, "It is blank.");
mpr(info);
break;
case 1:
strcpy(info, "It turns into a brightly coloured butterfly!");
mpr(info);
create_monster(66, 21 + random2(4), 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;
case 2:
strcpy(info, "On the card is a picture of a little demon.");
mpr(info);
strcpy(info, "The picture comes to life!");
mpr(info);
create_monster(220 + random2(5), 21 + random2(4), 7, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;
case 3:
strcpy(info, "On the card is a picture of a demon.");
mpr(info);
strcpy(info, "The picture comes to life!");
mpr(info);
create_monster(225 + random2(5), 21 + random2(3), 7, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;
case 4:
strcpy(info, "On the card is a picture of a huge demon.");
mpr(info);
strcpy(info, "The picture comes to life!");
mpr(info);
create_monster(225 + random2(5), 20 + random2(3), 7, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;
case 5:
strcpy(info, "On the card is a picture of a swarm of little demons.");
mpr(info);
strcpy(info, "The picture comes to life!");
mpr(info);
create_monster(220 + random2(5), 21 + random2(4), 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
create_monster(220 + random2(5), 21 + random2(4), 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
create_monster(220 + random2(5), 21 + random2(4), 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
create_monster(220 + random2(5), 21 + random2(4), 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
create_monster(220 + random2(5), 21 + random2(4), 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
create_monster(220 + random2(5), 21 + random2(4), 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
create_monster(220 + random2(5), 21 + random2(4), 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;
case 6:
strcpy(info, "On the card is a picture of a huge shaggy yak.");
mpr(info);
strcpy(info, "The picture comes to life!");
mpr(info);
create_monster(134, 25, 7, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;
case 7:
strcpy(info, "On the card is a picture of a huge scaly devil.");
mpr(info);
strcpy(info, "The picture comes to life!");
mpr(info);
create_monster(31, 21, 7, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;
case 8:
strcpy(info, "On the card is a picture of a huge scaly dragon.");
mpr(info);
strcpy(info, "The picture comes to life!");
mpr(info);
if (random2(2) == 0)
 create_monster(29, 22, 7, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
  else create_monster(75, 22, 7, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;
case 9:
strcpy(info, "On the card is a picture of a statue.");
mpr(info);
strcpy(info, "The picture comes to life!");
mpr(info);
create_monster(116 + random2(6), 21, 7, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;
case 10:
strcpy(info, "On the card is a picture of a very ugly thing.");
mpr(info);
strcpy(info, "The picture comes to life!");
mpr(info);
create_monster(102, 21, 7, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;
case 11:
strcpy(info, "On the card is a picture of a very irritated-looking skeletal thing.");
mpr(info);
strcpy(info, "The picture comes to life!");
mpr(info);
create_monster(37, 0, 1, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;
case 12: // unseen horror
if (player_see_invis() == 0)
{
 strcat(info, "It is blank");
 mpr(info);
} else
{
 strcpy(info, "On the card is a picture of a hideous abomination.");
 mpr(info);
 strcpy(info, "The picture comes to life!");
 mpr(info);
}
create_monster(46, 21, 7, you[0].x_pos, you[0].y_pos, MHITYOU, 250);
break;

}

you[0].inv_plus [you[0].equip [0]] --;
if (you[0].inv_plus [you[0].equip [0]] == 0)
{
 strcpy(info, "The deck of cards disappears in a puff of smoke.");
 mpr(info);
 unwield_item(you[0].equip [0]);
 you[0].inv_quant [you[0].equip [0]] --;
 if (you[0].inv_quant [you[0].equip [0]] == 0) // can this be false?
 {
        you[0].inv_no --;
                you[0].equip [0] = -1;
                strcpy(info, "You are now empty handed.");
                mpr(info);
 }
}
burden_change();
}
